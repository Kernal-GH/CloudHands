/*
 *
 *      Filename: ch_core_pool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-16 10:18:53
 * Last Modified: 2018-02-05 17:05:16
 */

#include "ch_core_pool.h"
#include "ch_errno.h"

#ifdef USE_DPDK
#include <rte_eal.h>
#include <rte_lcore.h>
#endif

static ch_core_t *
_core_create(ch_core_pool_t *cpool,uint32_t core_id,uint32_t core_idx){
	
	ch_core_t *core = (ch_core_t*)ch_pcalloc(cpool->mp,sizeof(*core));
	
	core->cpool = cpool;
	core->core_id = core_id;
	core->tsk = NULL;
	
#ifdef USE_DPDK
	/* unused */
	core_idx = core_idx;

	core->socket = rte_lcore_to_socket_id(core_id); 
#else

	core->core_idx = core_idx;
	core->state = WAITING;
	core->enabled = 0;

	/* By default, core 1:1 map to cpu id */
	CPU_SET(core_id,&core->cpuset);

	core->pipe_slave2master[0] = -1;
	core->pipe_slave2master[1] = -1;
	core->pipe_master2slave[0] = -1;
	core->pipe_master2slave[1] = -1;
#endif
	return core;
}


#ifndef USE_DPDK

#define SYS_CPU_DIR "/sys/devices/system/cpu/cpu%lu"
#define NUMA_NODE_PATH "/sys/devices/system/node"
#define CORE_ID_FILE "topology/core_id"
#define CPU_AFFINITY_STR_LEN 256

/* Check if a cpu is present by the presence of the cpu information for it */
static int
 _cpu_detected(uint32_t core_id)
{
	char path[PATH_MAX];
	int len = snprintf(path, sizeof(path), SYS_CPU_DIR
		"/"CORE_ID_FILE, (unsigned long)core_id);

	if (len <= 0 || (unsigned)len >= sizeof(path))
		return 0;

	if (access(path, F_OK) != 0)
		return 0;

	return 1;
}

/*
 * Get CPU socket id (NUMA node) for a logical core.
 *
 * This searches each nodeX directories in /sys for the symlink for the given
 * lcore_id and returns the numa node where the lcore is found. If lcore is not
 * found on any numa node, returns zero.
 */
static uint32_t
get_cpu_socket_id(uint32_t lcore_id)
{
	uint32_t socket;

	for (socket = 0; socket < CH_MAX_NUMA_NODES; socket++) {
		char path[PATH_MAX];

		snprintf(path, sizeof(path), "%s/node%u/cpu%u", NUMA_NODE_PATH,
				socket, lcore_id);
		if (access(path, F_OK) == 0)
			return socket;
	}

	return 0;
}

/*
 * Parse /sys/devices/system/cpu to get the number of physical and logical
 * processors on the machine.
 */
static int
_core_pool_init(ch_core_pool_t *cpool)
{
	uint32_t core_id;
	uint32_t count = 0;

	ch_core_t *core;
	
	for (core_id = 0; core_id < CH_CORE_MAX; core_id++) {
		
		cpool->cores[core_id] = NULL;

		if (_cpu_detected(core_id) == 0) {
			continue;
		}
		
		core = _core_create(cpool,core_id,count);
	
		core->socket = get_cpu_socket_id(core_id);
	
		/*add to core pool*/
		cpool->cores[count] = core;

		count++;
	}
	
	if(count == 0)
		return -1;

	cpool->sys_core_count = count;
	cpool->config_core_count = 0;

	return 0;
}

/*
 * Parse the coremask given as argument (hexadecimal string).
 */
static int xdigit2val(unsigned char c)
{
	int val;

	if (isdigit(c))
		val = c - '0';
	else if (isupper(c))
		val = c - 'A' + 10;
	else
		val = c - 'a' + 10;
	return val;
}

static int
_parse_coremask(ch_core_pool_t *cpool,const char *coremask)
{
	int i, j, idx = 0;
	uint32_t count = 0;
	char c;
	int val;

	if (coremask == NULL)
		return -1;
	/* Remove all blank characters ahead and after .
	 * Remove 0x/0X if exists.
	 */
	while (isblank(*coremask))
		coremask++;

	if (coremask[0] == '0' && ((coremask[1] == 'x')
		|| (coremask[1] == 'X')))
		coremask += 2;

	i = strlen(coremask);
	while ((i > 0) && isblank(coremask[i - 1]))
		i--;
	if (i == 0)
		return -1;

	for (i = i - 1; i >= 0 && idx < (int)cpool->sys_core_count; i--) {
		c = coremask[i];
		if (isxdigit(c) == 0) {
			/* invalid characters */
			return -1;
		}

		val = xdigit2val(c);
		for (j = 0; j < 4 && idx < (int)cpool->sys_core_count; j++, idx++)
		{
			if ((1 << j) & val) {
				cpool->cores[idx]->enabled = 1;
				count++;
			} else {
			
				cpool->cores[idx]->enabled = 0;
			}
		}
	}
	for (; i >= 0; i--)
		if (coremask[i] != '0')
			return -1;

	for (; idx < (int)cpool->sys_core_count; idx++) {
	
		cpool->cores[idx]->enabled = 0;
	}

	if (count == 0)
		return -1;

	cpool->config_core_count = count;

	return 0;
}

static int
_thread_set_affinity(ch_core_t *core)
{
	int s;
	pthread_t tid;

	tid = pthread_self();

	s = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &core->cpuset);
	if (s != 0) {
		fprintf(stderr, "pthread_setaffinity_np failed\n");
		return -1;
	}

	return 0;
}

static int
_thread_dump_affinity(ch_core_t *core,char *str, unsigned size)
{
	unsigned cpu;
	int ret;
	unsigned int out = 0;

	for (cpu = 0; cpu < core->cpool->sys_core_count; cpu++) {
		if (!CPU_ISSET(cpu, &core->cpuset))
			continue;

		ret = snprintf(str + out,
			       size - out, "%u,", cpu);
		if (ret < 0 || (unsigned)ret >= size - out) {
			/* string will be truncated */
			ret = -1;
			goto exit;
		}

		out += ret;
	}

	ret = 0;
exit:
	/* remove the last separator */
	if (out > 0)
		str[out - 1] = '\0';

	return ret;
}

static int _master_thread_init(ch_core_pool_t *cpool){
	
	int ret;
	char cpuset[CPU_AFFINITY_STR_LEN];

	ch_core_t *core;
	/*get the first enabled core to master*/
	
	core = ch_next_core_get(cpool,0,-1);
	if(core == NULL)
		return -1;
	
	cpool->master_core = core;
	core->tsk = NULL;
	_thread_set_affinity(core);
	
	ret =_thread_dump_affinity(core,cpuset, CPU_AFFINITY_STR_LEN);

	fprintf(stderr, "Master core %lu is ready (tid=%x;cpuset=[%s%s])\n",
		(unsigned long)core->core_id, (int)core->thread_id, cpuset,
		ret == 0 ? "" : "...");

	return 0;	
}


/* main loop of threads */
void *
_thread_loop(void *arg)
{
	char c;
	int n, ret;
	int m2s, s2m;
	ch_core_t *core = (ch_core_t*)arg;

	char cpuset[CPU_AFFINITY_STR_LEN];

	m2s = core->pipe_master2slave[0];
	s2m = core->pipe_slave2master[1];

	/* set CPU affinity */
	if (_thread_set_affinity(core)){
		fprintf(stderr,"cannot set affinity\n");
		return NULL;
 	}


	ret = _thread_dump_affinity(core,cpuset, CPU_AFFINITY_STR_LEN);
	fprintf(stdout,"core %lu is ready (tid=%x;cpuset=[%s%s])\n",
		(unsigned long)core->core_id, (int)core->thread_id, cpuset, ret == 0 ? "" : "...");

	/* read on our pipe to get commands */
	while (1) {

		/* wait command */
		do {
			n = read(m2s, &c, 1);
		} while (n < 0 && errno == EINTR);

		if (n <= 0){
			fprintf(stderr,"cannot read on configuration pipe\n");
			break;
		}

		core->state = RUNNING;

		/* send ack */
		n = 0;
		while (n == 0 || (n < 0 && errno == EINTR))
			n = write(s2m, &c, 1);
		if (n < 0){
			fprintf(stderr,"cannot write on configuration pipe\n");
			break;
		}

		if (core->tsk == NULL){
			fprintf(stderr,"This core:%lu has no been bond to a task!",(unsigned long)core->core_id);
			break;
		}

		/* run task */
		ch_task_run(core->tsk);
		core->state = FINISHED;
	}

	return NULL;
}

static int
_slave_threads_create(ch_core_pool_t *cpool){

	ch_core_t *core;
	int ret;
	char thread_name[16]={0};

	CPOOL_CORE_FOREACH_SLAVE(cpool,core) {

		/*
		 * create communication pipes between master thread
		 * and children
		 */
		if (pipe(core->pipe_master2slave) < 0){
			fprintf(stderr,"Cannot create pipe\n");
			return -1;
		}

		if (pipe(core->pipe_slave2master) < 0){
			fprintf(stderr,"Cannot create pipe\n");
			return -1;
		}

		core->state = WAITING;

		/* create a thread for each lcore */
		ret = pthread_create(&core->thread_id, NULL,
				     _thread_loop, (void*)core);

		if (ret != 0){
			fprintf(stderr,"Cannot create thread\n");
			return -1;
		}

		/* Set thread_name for aid in debugging. */
		snprintf(thread_name,16,
			"core-slave-%d", core->core_id);

		ret = pthread_setname_np(core->thread_id, thread_name);

		if (ret != 0){
			fprintf(stderr,"Cannot set name for lcore thread\n");
			return -1;
		};
	}

	return 0;
}


/*
 * Send a message to a slave  to run task
 * 
 */
static int
_core_remote_launch(ch_core_t *core)
{
	int n;
	char c = 0;
	int m2s = core->pipe_master2slave[1];
	int s2m = core->pipe_slave2master[0];

	if (core->state != WAITING)
		return -1;

	/* send message */
	n = 0;
	while (n == 0 || (n < 0 && errno == EINTR))
		n = write(m2s, &c, 1);
	if (n < 0){
		fprintf(stderr,"cannot write on configuration pipe\n");
		return -1;
	}

	/* wait ack */
	do {
		n = read(s2m, &c, 1);
	} while (n < 0 && errno == EINTR);

	if (n <= 0){
		fprintf(stderr,"cannot read on configuration pipe\n");
		return -1;
	}

	return 0;
}

/*
 * Wait until a lcore finished its job.
 */
static int
_wait_core(ch_core_t *core)
{
	if (core->state == WAITING)
		return 0;

	while (core->state != WAITING &&
	       core->state != FINISHED);

	/* we are in finished state, go to wait state */
	core->state = WAITING;
	return 0;
}
#endif /* no USE DPDK */

ch_core_pool_t * 
ch_core_pool_create(ch_pool_t *mp,const char *core_mask){

	ch_core_pool_t *cpool = (ch_core_pool_t*)ch_pcalloc(mp,sizeof(*cpool));
	cpool->mp = mp;
	
#ifdef USE_DPDK
    ch_core_t *core;	
	uint32_t lcore_id;
	uint32_t core_n = rte_lcore_count();
	uint32_t core_idx = 0;

	if(core_n == 0){
	
		fprintf(stderr,"no cpu core used to create core pool!\n");
		return NULL;
	}
	RTE_LCORE_FOREACH_SLAVE(lcore_id){
	
		core = _core_create(cpool,lcore_id,0);
		cpool->cores[core_idx++] = core;
		fprintf(stdout,"Create core[id=%lu,socket=%lu]!\n",
			(unsigned long)lcore_id,
			(unsigned long)core->socket);
	}
	cpool->config_core_count = core_idx;
	fprintf(stdout,"Create cpu cores[%lu] in core pool OK!\n",(unsigned long)core_idx);

#else /*not use dpdk*/

	cpool->master_core = NULL;
	cpool->sys_core_count = 0;
	cpool->config_core_count = 0;
	
	/*create cores available cpu in system*/
	if(_core_pool_init(cpool)){
		fprintf(stderr,"init core pool failed!\n");
		return NULL;
	}
	
	/*parse core mask*/
	if(_parse_coremask(cpool,core_mask)){
		fprintf(stderr,"parse core mask :%s,failed!\n",core_mask);
		return NULL;
	}

	/*init master thread */
	if(_master_thread_init(cpool)){
		fprintf(stderr,"init master thread failed!\n");
		return NULL;
	}

	if(_slave_threads_create(cpool)){
		fprintf(stderr,"create all slave threads failed!\n");
		return NULL;
	}
#endif
	/*ok!*/
	return cpool;
}

static ch_core_t *
_alloc_core(ch_core_pool_t *cpool){
	
	ch_core_t *core = NULL;
#ifdef USE_DPDK
	uint32_t i;
	for(i = 0; i<cpool->config_core_count;i++){
	
		core = cpool->cores[i];
		if(core->tsk == NULL)
			return core;
	}
#else
	CPOOL_CORE_FOREACH_SLAVE(cpool,core){

		if(core->tsk == NULL)
			return core;
	}
#endif
	return NULL;
}

int ch_core_pool_bind_task(ch_core_pool_t *cpool,ch_task_t *tsk){
	
	ch_core_t *core = _alloc_core(cpool);
	if(core == NULL){

		fprintf(stderr,"no core to bind this task!\n");
		return -1;
	}	

	core->tsk = tsk;
	tsk->core = core;
	fprintf(stdout,"Cpu core[%lu] was bound to task[%lu] ok!\n",(unsigned long)core->core_id,
	(unsigned long)tsk->tsk_id);
	return 0;
}

#ifdef USE_DPDK
static ch_core_t * _core_get_by_id(ch_core_pool_t *cpool,uint32_t core_id){

    ch_core_t *core;
    uint32_t i;

    for(i = 0; i<cpool->config_core_count; i++){
        core = cpool->cores[i];
        if(core->core_id == core_id)
            return core;
    }

    return NULL;
}

static int core_setup_main(void *priv_data){

    ch_core_t *core;

    uint32_t core_id;

    ch_core_pool_t *cpool = (ch_core_pool_t*)priv_data;
    core_id = rte_lcore_id();

    core = _core_get_by_id(cpool,core_id);
    
    if(core == NULL||core->tsk == NULL){
        fprintf(stderr,"Core was not be bound to any task,ignore it!\n");
        return 0;
    }

    /* run task! */
    ch_task_run(core->tsk);

    return 0;
}
#endif /* USE_DPDK */

int ch_core_pool_slaves_setup(ch_core_pool_t *cpool){

#ifdef USE_DPDK
	
   /* launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(core_setup_main, (void*)cpool, CALL_MASTER);
#else
	int ret;
	ch_core_t *core;

	CPOOL_CORE_FOREACH_SLAVE(cpool,core){
		
		if(core->state !=WAITING)
		{
			fprintf(stderr,"The slave[%lu] state is invalid!\n",(unsigned long)core->core_id);
			return -1;
		}
	}

	CPOOL_CORE_FOREACH_SLAVE(cpool,core){
		
		ret = _core_remote_launch(core);
		if(ret)
		{
			fprintf(stderr,"The slave[%lu] cannot been setup!\n",(unsigned long)core->core_id);
			return -1;
		}
	}

#endif

	return 0;
}



void ch_core_pool_wait_for_slaves(ch_core_pool_t *cpool){

#ifdef USE_DPDK
	/* unused */
	cpool = cpool;

	uint32_t core_id;

    RTE_LCORE_FOREACH_SLAVE(core_id) {
        if (rte_eal_wait_lcore(core_id) < 0){
            fprintf(stderr,"wait all tasks failed!");
        }
    } 
#else
	ch_core_t *core;
	CPOOL_CORE_FOREACH_SLAVE(cpool,core){
		_wait_core(core);
	}
#endif
}



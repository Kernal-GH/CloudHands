/*
 *
 *      Filename: ch_core_pool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-13 17:05:23
 * Last Modified: 2017-01-13 17:05:23
 */

#ifndef CH_CORE_POOL_H
#define CH_CORE_POOL_H

typedef struct ch_core_pool_t ch_core_pool_t;
typedef struct ch_core_t ch_core_t;

#include <sched.h>
#include "ch_task.h"
#include "ch_constants.h"
#include "ch_mpool.h"

#ifndef USE_DPDK
enum core_state {

	WAITING,
	RUNNING,
	FINISHED,
};
#endif

#define CH_CORE_MAX 256

struct ch_core_t {

	ch_core_pool_t *cpool;
	ch_task_t *tsk;
	uint32_t core_id;

	uint32_t socket;
	uint32_t core_idx;

	int state;

	int enabled;

	cpu_set_t cpuset;

	pthread_t thread_id;

	int pipe_master2slave[2];
	int pipe_slave2master[2];
};

struct ch_core_pool_t {

	ch_pool_t *mp;

	ch_core_t * cores[CH_CORE_MAX];
	
	uint32_t config_core_count;

#ifndef USE_DPDK
	ch_core_t *master_core;

	uint32_t sys_core_count;
	
#endif

};

extern ch_core_pool_t * ch_core_pool_create(ch_pool_t *mp,const char *core_mask);

extern int ch_core_pool_bind_task(ch_core_pool_t *cpool,ch_task_t *tsk);

extern int ch_core_pool_slaves_setup(ch_core_pool_t *cpool);

extern void ch_core_pool_wait_for_slaves(ch_core_pool_t *cpool);

#ifndef USE_DPDK
static inline ch_core_t * 
ch_next_core_get(ch_core_pool_t *cpool,int skip_master,int idx){

	ch_core_t *core = NULL;

	idx++;

	for(;idx<(int)cpool->sys_core_count;idx++){
	
		core = cpool->cores[idx];
		if((core == cpool->master_core&&skip_master)||(core->enabled == 0))
			continue;

		return core;
	}

	return NULL;
}

/**
 * Macro to browse all running lcores.
 */
#define CPOOL_CORE_FOREACH(cpool,core)						\
	for (core = ch_next_core_get(cpool, 0, -1);				\
	     core!=NULL;										\
	     core = ch_next_core_get(cpool, 0, core->core_idx))

/**
 * Macro to browse all running lcores except the master lcore.
 */

#define CPOOL_CORE_FOREACH_SLAVE(cpool,core)				\
	for (core = ch_next_core_get(cpool, 1, -1);				\
	     core!=NULL;										\
	     core = ch_next_core_get(cpool, 1, core->core_idx))

#endif /* !USE_DPDK */

#endif /* CH_CORE_POOL_H */

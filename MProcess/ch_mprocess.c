/*
 *
 *      Filename: ch_mprocess.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-13 17:09:11
 * Last Modified: 2018-02-01 15:23:03
 */

#include "ch_constants.h"
#include "ch_log.h"
#include "ch_config.h"
#include "ch_mprocess.h"


ch_mprocess_t * ch_mprocess_create(ch_pool_t *mp,ch_lua_filter_engine_t *lua_filter_engine,uint64_t size){

	ch_mprocess_t *mprocess = NULL;
	ch_mprocess_info_t *mp_info = NULL;

	mp_info = ch_mprocess_info_create(size);
	if(mp_info == NULL){
	
		ch_log(CH_LOG_ERR,"Create mprocess info failed!");
		return NULL;
	}


	mprocess = (ch_mprocess_t*)ch_pcalloc(mp,sizeof(*mprocess));
	
	mprocess->mp = mp;
	mprocess->mp_info = mp_info;
	mprocess->lua_filter_engine = lua_filter_engine;

	INIT_LIST_HEAD(&mprocess->qpools);

	return mprocess;
}

static ch_mprocess_queue_pool_t *_queue_pool_create(ch_mprocess_t *mprocess,ch_mprocess_queue_pool_info_t *qpool_info){


	ch_mprocess_queue_pool_t *qpool = NULL;

	qpool = (ch_mprocess_queue_pool_t *)ch_pcalloc(mprocess->mp,sizeof(*qpool));

	qpool->queues = ch_array_make(mprocess->mp,16,sizeof(ch_mprocess_queue_t*));
	qpool->pinfo = qpool_info;

	/*find lua filter by filter name*/
	qpool->lua_filter = ch_lua_filter_engine_script_find(mprocess->lua_filter_engine,qpool_info->filter_name);
	if(qpool->lua_filter == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find the filter name:%s",qpool_info->filter_name);
		return NULL;
	}

	/*find hash by hash name*/
	qpool->hash = NULL;

	/*add to mprocess*/
	list_add_tail(&qpool->node,&mprocess->qpools);

	return qpool;
}

static ch_mprocess_queue_t * _queue_create(ch_mprocess_t *mprocess,ch_mprocess_queue_pool_t *qpool,ch_mprocess_queue_info_t *qinfo){


	ch_mprocess_queue_t *queue = NULL;

	queue = (ch_mprocess_queue_t*)ch_pcalloc(mprocess->mp,sizeof(*queue));
	queue->qinfo = qinfo;

	queue->ring = rte_ring_create(qinfo->name,qinfo->queue_size,rte_socket_id(),0);

	/*add to queue pool*/
	*(ch_mprocess_queue_t**)ch_array_push(qpool->queues) = queue;

	return queue;
}

int ch_mprocess_register(ch_mprocess_t *mprocess,const char* qpool_name,const char *qname,uint32_t qsize){

	char filter_name[MAX_NAME_SIZE];
	char hash_name[MAX_NAME_SIZE];

	ch_mprocess_queue_pool_info_t *qpool_info = NULL;
	
	ch_mprocess_queue_pool_t *qpool = NULL;
	ch_mprocess_queue_info_t *qinfo = NULL;

	ch_mprocess_queue_t *queue = NULL;

	if(set_names(filter_name,hash_name,qpool_name)){
	
		ch_log(CH_LOG_ERR,"Invalid filter name and hash name,name size to large!");
		return -1;
	}

	qpool_info = ch_mprocess_queue_pool_info_lookup(mprocess->mp_info,qpool_name);
	if(qpool_info == NULL){
	
		qpool_info = ch_mprocess_queue_pool_info_create(mprocess->mp_info,qpool_name,filter_name,hash_name);
		if(qpool_info == NULL){
		
			ch_log(CH_LOG_ERR,"Create queue pool info failed!");
			return -1;
		}

		qpool = _queue_pool_create(mprocess,qpool_info);
		if(qpool == NULL){
		
			ch_log(CH_LOG_ERR,"Create Queue Pool failed!");
			return -1;
		}
	}

	qinfo = ch_mprocess_queue_info_lookup(mprocess->mp_info,qpool_info,qname);
	if(qinfo == NULL){
	
		qinfo = ch_mprocess_queue_info_create(mprocess->mp_info,qpool_info,qname,qsize);
		if(qinfo == NULL){
		
			ch_log(CH_LOG_ERR,"Create queue info failed!");
			return -1;
		}
		
		queue = _queue_create(mprocess,qpool,qinfo);
		if(queue == NULL){
		
			ch_log(CH_LOG_ERR,"Create queue failed!");
			return -1;
		}
	}

	/*ok*/
	return 0;
}


int ch_mprocess_packet_put(ch_mprocess_t *mprocess,ch_packet_t *pkt){

	ch_mprocess_queue_pool_t *qpool;
	ch_mprocess_queue_t *queue;
	int n = 0;
	int queue_index = 0;
	int queue_n = 0;


	list_for_each_entry(qpool,&mprocess->qpools,node){
	
		if(qpool->queues == NULL||qpool->queues->nelts==0)
			continue;

#if 0
		if(qpool->lua_filter){
		
			rc = ch_lua_filter_engine_script_run(mprocess->lua_filter_engine,qpool->lua_filter,pkt);
			if(rc == 1){
			
				/*The packet should been filterred(passed)*/
				continue;
			}
		}
#endif
		queue_index = 0;
		queue_n = qpool->queues->nelts;

		/*The packet should been accepted by this queue pool*/
		if(queue_n>1){
		
			/*has more than 1 queue,get one queue by hash functions*/
			queue_index = qpool->hash(pkt)%queue_n;

		}

		queue = ((ch_mprocess_queue_t**)qpool->queues->elts)[queue_index];

		/*append pkt pointer into this queue,if ring has more space*/
		if(!rte_ring_full(queue->ring)){
		
			rte_ring_enqueue(queue->ring,(void*)pkt->mbuf);
			n+=1;
		}
	}

	return n;
}

ch_mprocess_t * ch_mprocess_load(ch_pool_t *mp){

	ch_mprocess_t *mprocess = NULL;
	ch_mprocess_info_t *mp_info = NULL;

	mp_info = ch_mprocess_info_get();
	if(mp_info == NULL){
	
		ch_log(CH_LOG_ERR,"Find mprocess info failed!");
		return NULL;
	}

	mprocess = (ch_mprocess_t*)ch_pcalloc(mp,sizeof(*mprocess));
	
	mprocess->mp = mp;
	mprocess->mp_info = mp_info;
	mprocess->lua_filter_engine = NULL;

	INIT_LIST_HEAD(&mprocess->qpools);

	return mprocess;
}

ch_mprocess_queue_t * ch_mprocess_queue_get(ch_mprocess_t *mprocess,const char *qpool,const char *qname){


	ch_mprocess_queue_pool_info_t *qpool_info = NULL;
	ch_mprocess_queue_info_t *qinfo = NULL;
	ch_mprocess_queue_t *queue = NULL;
	struct rte_ring *ring = NULL;
	struct rte_mempool *pktmbuf_pool = NULL;

	qpool_info = ch_mprocess_queue_pool_info_lookup(mprocess->mp_info,qpool);
	if(qpool_info == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find the queue pool info:%s",qpool);
		return NULL;
	}

	qinfo = ch_mprocess_queue_info_lookup(mprocess->mp_info,qpool_info,qname);
	if(qinfo == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find the queue info:%s",qname);
		return NULL;
	}

	/*find pktmbuf pool*/
	pktmbuf_pool = rte_mempool_lookup(PKT_POOL_NAME);
	if(pktmbuf_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find packet mbuf pool:%s",qname);
		return NULL;
	}

	/*find ring*/
	ring = rte_ring_lookup(qinfo->name);
	
	if(ring == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find the queue info ring:%s",qname);
		return NULL;
	}

	queue = (ch_mprocess_queue_t*)ch_pcalloc(mprocess->mp,sizeof(*queue));
	queue->qinfo = qinfo;
	queue->ring = ring;
	queue->pktmbuf_pool = pktmbuf_pool;

	return queue;
}

struct rte_mbuf* ch_mprocess_packet_pop(ch_mprocess_t *mprocess ch_unused,ch_mprocess_queue_t *queue){


	struct rte_mbuf *mbuf = NULL;

	if(rte_ring_dequeue(queue->ring,(void**)(&mbuf))){
	
		/*no packets*/
		return NULL;
	}


	return mbuf;
}

void ch_mprocess_dump(ch_mprocess_t *mprocess,FILE *fp){


	ch_mprocess_info_dump(mprocess->mp_info,fp);

}

static const char *cmd_mprocess_register(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2,const char *p3){

	/*p1------>queue pool name
	 *p2------>queue name
	 *p3------>queue size
	 * */

    char *endptr;
	uint32_t qsize = 0;

	if(p1 == NULL || p2 == NULL ||p3 == NULL)
		return "Invalid mprocess register config";

    ch_mprocess_t  *mprocess = (ch_mprocess_t*)_dcfg;
   
    qsize = (uint32_t)strtoul(p3,&endptr,10);

	if(qsize == 0)
		return "Queue size 0";

	if(ch_mprocess_register(mprocess,p1,p2,qsize)){
	
		return "register mprocess failed!";
	}

	return NULL;
}


static const command_rec mprocess_directives[] ={

	CH_INIT_TAKE3(
            "CHMProcessRegister",
            cmd_mprocess_register,
            NULL,
            0,
            "register mprocess queue pool&queues"
            ),

};

int ch_mprocess_config(ch_mprocess_t *mprocess,const char *cfname){

	  const char *msg;
	  
	  if(cfname == NULL||strlen(cfname) == 0){
	
		  ch_log(CH_LOG_ERR,"Must specify the config path mprocess config!");
		
		  return -1;
	  
	  }

	  
	  msg = ch_process_command_config(mprocess_directives,(void*)mprocess,mprocess->mp,mprocess->mp,cfname);

	  /*config failed*/
	  if(msg!=NULL){

		
		  ch_log(CH_LOG_ERR,"Config mprocess failed:%s",msg);
		
		  return -1;
	  }

	  /*ok*/
	  return 0;
}

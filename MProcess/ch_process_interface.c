/*
 *
 *      Filename: ch_process_interface.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-31 19:04:21
 * Last Modified: 2018-08-15 10:06:24
 */
#include "ch_constants.h"
#include "ch_process_interface.h"
#include "ch_string.h"
#include "ch_log.h"

static inline const char * _qname_make(ch_pool_t *mp,const char *prefix,int i){

	return (const char*)ch_psprintf(mp,"%s_%d",prefix,i);
}

static int _process_queue_create(ch_process_interface_t *pint,int is_write){

	ch_process_queue_t *pqueue = NULL;
	struct rte_ring *r = NULL;
	const char *qname;

	int i;

	for(i = 0;i<(int)pint->qnumber;i++){
	
		qname = _qname_make(pint->mp,pint->qprefix,i);

		if(is_write){
		
			r = rte_ring_create(qname,pint->qsize,rte_socket_id(),0);
		}else{
		
			r = rte_ring_lookup(qname);
		}

		if(r == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot create or load ring:%s,%d",qname,pint->qsize);
			return -1;
		}
		pqueue = (ch_process_queue_t*)ch_palloc(pint->mp,sizeof(*pqueue));
		pqueue->qname = qname;
		pqueue->packets = 0;
		pqueue->ok_packets = 0;
		pqueue->error_packets = 0;
		pqueue->ring = r;

		/*add to array*/
		*(ch_process_queue_t**)ch_array_push(pint->queues) = pqueue;
	}

	/*ok*/
	return 0;
}

ch_process_interface_t * ch_process_interface_writer_create(ch_pool_t *mp,
	const char *qprefix,
	uint32_t qnumber,
	uint32_t qsize,
	int (*filter)(ch_packet_t *pkt,void *priv_data),
	uint32_t (*hash)(ch_packet_t *pkt,void *priv_data),
	void *priv_data){


	ch_process_interface_t *pint;

	pint = (ch_process_interface_t*)ch_palloc(mp,sizeof(*pint));
	pint->mp = mp;
	pint->qprefix = qprefix;
	pint->qnumber = qnumber;
	pint->qsize = qsize;
	pint->filter = filter;
	pint->hash = hash;
	pint->cur_index = 0;
	pint->priv_data = priv_data;

	pint->queues = ch_array_make(mp,qnumber,sizeof(ch_process_queue_t*));

	/*start to create queues*/
	if(_process_queue_create(pint,1)){
	
		ch_log(CH_LOG_ERR,"Cannot create process queue!");

		return NULL;
	}

	return pint;
}

static inline int _load_pkt_mempool(void){

	 if(NULL == rte_mempool_lookup(PKT_POOL_NAME))
		 return -1;

#if 0
	 if(NULL == rte_mempool_lookup(SA_POOL_NAME))
		 return -1;

#endif

	 return 0;
}

ch_process_interface_t * ch_process_interface_reader_create(ch_pool_t *mp,
	const char *qprefix,
	uint32_t qnumber){

	ch_process_interface_t *pint;

	pint = (ch_process_interface_t*)ch_palloc(mp,sizeof(*pint));
	pint->mp = mp;
	pint->qprefix = qprefix;
	pint->qnumber = qnumber;
	pint->qsize = 0;
	pint->filter = NULL;
	pint->hash = NULL;
	pint->cur_index = 0;
	pint->priv_data = NULL;

	pint->queues = ch_array_make(mp,qnumber,sizeof(ch_process_queue_t*));

	if(_load_pkt_mempool()){
	
		ch_log(CH_LOG_ERR,"Cannot load memory pool!");
		return NULL;
	}

	/*start to create queues*/
	if(_process_queue_create(pint,0)){
	
		ch_log(CH_LOG_ERR,"Cannot create process queue!");

		return NULL;
	}

	return pint;

}

int ch_process_interface_put(ch_process_interface_t *pint,ch_packet_t *pkt){


	ch_process_queue_t *queue;
	int queue_index = 0;

	if(pint->filter&&pint->filter(pkt,pint->priv_data)){
	
		/*ignore this packet!*/
		return -2;
	}

	if(pint->qnumber>1){
	
		queue_index = pint->hash(pkt,pint->priv_data)%pint->qnumber;
	}

	queue =  ((ch_process_queue_t**)pint->queues->elts)[queue_index];

	queue->packets+=1;
	
	if(!rte_ring_full(queue->ring)){
	
		rte_ring_enqueue(queue->ring,(void*)pkt);
		queue->ok_packets += 1;
		return 0;
	}

	queue->error_packets += 1;

	/*error*/
	return -1;
}

ch_packet_t * ch_process_queue_pop(ch_process_queue_t *queue){


	ch_packet_t *pkt;
	if(rte_ring_dequeue(queue->ring,(void**)(&pkt))){
	
		/*no packet*/
		return NULL;
	}

	return pkt;
}


ch_process_queue_t * ch_process_interface_queue_assign(ch_process_interface_t *pint){


	ch_process_queue_t *queue = NULL;
	if(pint->queues->nelts == 0||pint->qnumber == 0 ||pint->cur_index>=pint->qnumber){
	
		ch_log(CH_LOG_ERR,"Have no queue can been assigned!");
		return NULL;
	}

	queue =  ((ch_process_queue_t**)pint->queues->elts)[pint->cur_index];
	pint->cur_index+=1;

	return queue;
}

static void _dump_queue(ch_process_queue_t *queue,FILE *fp){

	fprintf(fp,"The queue name:%s\n",queue->qname);
	fprintf(fp,"The packets:%lu\n",(unsigned long)queue->packets);
	fprintf(fp,"The ok packets:%lu\n",(unsigned long)queue->ok_packets);
	fprintf(fp,"The error packets:%lu\n",(unsigned long)queue->error_packets);

	fprintf(fp,"Dump The Ring Informations:-------------------------------------\n");
	rte_ring_dump(fp,queue->ring);

}

void ch_process_interface_dump(ch_process_interface_t *pint,FILE *fp){

	fprintf(fp,"Dump the process information----------------------------\n");
	fprintf(fp,"The process queue number:%d\n",(int)pint->qnumber);
	fprintf(fp,"The process queue size:%d\n",(int)pint->qsize);

	ch_process_queue_t *queue;
	int i;
	for(i=0;i<pint->queues->nelts;i++){
	
		queue =  ((ch_process_queue_t**)pint->queues->elts)[i];
		_dump_queue(queue,fp);
	}
}


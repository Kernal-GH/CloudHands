/*
 *
 *      Filename: ch_mprocess.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-09 19:41:34
 * Last Modified: 2017-11-09 19:41:34
 */

#ifndef CH_MPROCESS_H
#define CH_MPROCESS_H

typedef struct ch_mprocess_t ch_mprocess_t;
typedef struct ch_mprocess_queue_pool_t ch_mprocess_queue_pool_t;
typedef struct ch_mprocess_queue_t ch_mprocess_queue_t;

#include <rte_mbuf.h>
#include "ch_mprocess_info.h"
#include "ch_tables.h"
#include "ch_list.h"
#include "ch_lua_filter_engine.h"
#include "ch_constants.h"

struct ch_mprocess_t {

	ch_pool_t *mp;
	ch_mprocess_info_t *mp_info;
	ch_lua_filter_engine_t *lua_filter_engine;

	struct list_head qpools;

};

struct ch_mprocess_queue_pool_t {

	struct list_head node;

	ch_array_header_t *queues;

	ch_mprocess_queue_pool_info_t *pinfo;
	ch_lua_script_t *lua_filter;

	uint32_t (*hash)(ch_packet_t *pkt);

};

struct ch_mprocess_queue_t {

	ch_mprocess_queue_info_t *qinfo;
   
	struct rte_mempool *pktmbuf_pool;

	struct rte_ring *ring;

};

static inline int set_names(char *filter_name,char *hash_name,const char *prefix){


	size_t plen = strlen(prefix);

	if(plen+7>=MAX_NAME_SIZE)
		return -1;

	snprintf(filter_name,MAX_NAME_SIZE,"%s_filter",prefix);
	snprintf(hash_name,MAX_NAME_SIZE,"%s_hash",prefix);

	return 0;
}

/*For writing process functions*/

extern ch_mprocess_t * ch_mprocess_create(ch_pool_t *mp,ch_lua_filter_engine_t *lua_filter_engine,uint64_t size);


extern int ch_mprocess_config(ch_mprocess_t *mprocess,const char *cfname);

extern int ch_mprocess_register(ch_mprocess_t *mprocess,const char* qpool_name,const char *qname,uint32_t qsize);

extern int ch_mprocess_packet_put(ch_mprocess_t *mprocess,ch_packet_t *pkt);

/*For writing process functions END*/

/*For reading process functions*/

extern ch_mprocess_t * ch_mprocess_load(ch_pool_t *mp);

extern ch_mprocess_queue_t * ch_mprocess_queue_get(ch_mprocess_t *mprocess,const char *qpool,const char *qname);

extern struct rte_mbuf* ch_mprocess_packet_pop(ch_mprocess_t *mprocess,ch_mprocess_queue_t *queue);

/*For reading process functions END*/

extern void ch_mprocess_dump(ch_mprocess_t *mprocess,FILE *fp);

#endif /*CH_MPROCESS_H*/

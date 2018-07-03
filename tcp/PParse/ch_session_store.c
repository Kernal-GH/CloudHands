/*
 *
 *      Filename: ch_session_store.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 11:17:01
 * Last Modified: 2018-07-03 15:23:14
 */

#include "ch_session_store.h"
#include "ch_log.h"
#include "ch_packet_record.h"

ch_session_store_t * ch_session_store_create(ch_pool_t *mp,ch_pp_context_t *pcontext) {


	ch_session_store_t *sstore = (ch_session_store_t*)ch_palloc(mp,sizeof(*sstore));

	sstore->pcontext = pcontext;
	sstore->shm_fmt = NULL;
	sstore->fstore = NULL;

	if(pcontext->fstore_dir){
		sstore->fstore = ch_file_store_create(mp,pcontext->fstore_dir,pcontext->fstore_buf_size,pcontext->create_dir_type,0);

		if(sstore->fstore == NULL){
		
			ch_log(CH_LOG_ERR,"Create file store failed for session store!");
			return NULL;
		}
	}else{

		sstore->shm_fmt = ch_shm_format_pkt_with_mmap_create(mp,pcontext->shm_fname,pcontext->shm_fsize,pcontext->shm_fentry_size,
			0,1);

		if(sstore->shm_fmt == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot Create shm store failed for session store!");
			return NULL;
		}
	}

    msgpack_sbuffer_init(&sstore->pk_buf);
    msgpack_packer_init(&sstore->pk,&sstore->pk_buf,msgpack_sbuffer_write);

	return sstore;
}


void ch_session_store_destroy(ch_session_store_t *sstore){

    msgpack_sbuffer_destroy(&sstore->pk_buf);
}

int ch_session_store_put(ch_session_entry_t *sentry,void *session){

	void *data = NULL;
	size_t dlen = 0;

	ch_pp_parser_t *pp_parser = sentry->pp_parser;

	ch_session_store_t *sstore = pp_parser->pp_pool->sstore;

	
	msgpack_packer *pk = &sstore->pk;

	/*packer the common data into msgpack*/
	ch_msgpack_map_start(pk,NULL,2);
	ch_msgpack_map_start(pk,"common",14);

	ch_msgpack_write_uint32(pk,"protoID",sentry->protocol_id);
	ch_msgpack_write_uint64(pk,"sessionID",sentry->session_id);
	ch_msgpack_write_uint64(pk,"reqTime",sentry->req_time);
	ch_msgpack_write_uint64(pk,"resTime",sentry->res_time);
	ch_msgpack_write_uint32(pk,"srcIP",sentry->src_ip);
	ch_msgpack_write_uint32(pk,"dstIP",sentry->dst_ip);
	ch_msgpack_write_uint16(pk,"srcPort",sentry->src_port);
	ch_msgpack_write_uint16(pk,"dstPort",sentry->dst_port);
	ch_msgpack_write_uint64(pk,"reqPackets",sentry->req_packets);
	ch_msgpack_write_uint64(pk,"reqBytes",sentry->req_bytes);
	ch_msgpack_write_uint64(pk,"reqPBytes",sentry->req_pbytes);
	
	ch_msgpack_write_uint64(pk,"resPackets",sentry->res_packets);
	ch_msgpack_write_uint64(pk,"resBytes",sentry->res_bytes);
	ch_msgpack_write_uint64(pk,"resPBytes",sentry->res_pbytes);

	/*packer the session data into msgpack*/

	pp_parser->session_format(pk,session);

    data = sstore->pk_buf.data;
    dlen = sstore->pk_buf.size;

	if(sstore->fstore){
		if(ch_file_store_write(sstore->fstore,data,dlen)){

		
			msgpack_sbuffer_clear(&sstore->pk_buf);
			
			ch_log(CH_LOG_ERR,"Write file store failed,size:%lu",(unsigned long)dlen);
			return -1;
		}
	} else{

		ch_packet_record_t pkt_rcd;
		pkt_rcd.type = pp_parser->pkt_rcd_type;
		pkt_rcd.meta_data_size = 0;
		pkt_rcd.time = 0;

		ch_packet_record_put(sstore->shm_fmt,&pkt_rcd,data,dlen);
	}

    msgpack_sbuffer_clear(&sstore->pk_buf);

	/*ok*/
	return 0;
}


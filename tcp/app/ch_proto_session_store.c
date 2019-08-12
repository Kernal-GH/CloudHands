/*
 *
 *      Filename: ch_proto_session_store.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-28 14:47:12
 * Last Modified: 2018-09-27 11:34:08
 */

#include "ch_proto_session_store.h"
#include "ch_log.h"
#include "ch_packet_record.h"
#include "ch_util.h"

#define HTTP_SHM_NAME(mp,prefix) ch_psprintf(mp,"%s_http",prefix)

ch_proto_session_store_t *ch_proto_session_store_create(ch_pool_t *mp,uint32_t task_id,const char *shm_fname,
	uint64_t shm_fsize,uint64_t fentry_size,uint64_t shm_flush_timeout){

	int i = 0;
	ch_proto_session_store_t *pstore = (ch_proto_session_store_t*)ch_palloc(mp,sizeof(*pstore));

	pstore->task_id = task_id;

	pstore->mp  = mp;
	pstore->shm_flush_timeout =  shm_flush_timeout;
	pstore->shm_last_time = ch_get_current_timems()/1000;

	pstore->other_shm_fmt = ch_shm_format_pkt_with_mmap_create(mp,shm_fname,shm_fsize,fentry_size,0,1);

	if(pstore->other_shm_fmt == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create other protocol shm store failed for proto session store!");
		return NULL;
	}
	
	pstore->http_shm_fmt = ch_shm_format_pkt_with_mmap_create(mp,HTTP_SHM_NAME(mp,shm_fname),shm_fsize,fentry_size,0,1);

	if(pstore->http_shm_fmt == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create http protocol shm store failed for proto session store!");
		return NULL;
	}
	
    if(ch_buffer_init(&pstore->g_buffer)){
	
		ch_log(CH_LOG_ERR,"Cannot create buffer for protocol session store!");
		return NULL;
	}

    msgpack_sbuffer_init(&pstore->pk_buf);
    msgpack_packer_init(&pstore->pk,&pstore->pk_buf,msgpack_sbuffer_write);

	ch_proto_body_store_t *bstore;

	for(i= 0;i<PROTOCOL_MAX;i++){
	
		bstore = &pstore->body_stores[i];
		bstore->req_fpath = NULL;
		bstore->res_fpath = NULL;
	}

	return pstore;

}

void ch_proto_session_store_destroy(ch_proto_session_store_t *pstore){

    msgpack_sbuffer_destroy(&pstore->pk_buf);

}

static void _pstore_shm_flush(ch_proto_session_store_t *pstore){

	uint64_t cur_time = ch_get_current_timems()/1000;

	if(cur_time-pstore->shm_last_time>pstore->shm_flush_timeout){

		ch_shm_format_flush(pstore->other_shm_fmt);

		pstore->shm_last_time = cur_time;
	}
}

#define PSTORE_SHM_GET(pstore,proto) ((proto)==PROTOCOL_HTTP?pstore->http_shm_fmt:pstore->other_shm_fmt)

int ch_proto_session_store_write(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,ch_proto_session_entry_t *session){

	void *data;
	size_t dlen;

	ch_tcp_app_t *app = tsession->app;

	msgpack_packer *pk = &pstore->pk;

	ch_shm_format_t *shm_fmt = PSTORE_SHM_GET(pstore,app->protocol_id);

	/*packer the common data into msgpack*/
	ch_msgpack_map_start(pk,NULL,2);
	ch_msgpack_map_start(pk,"common",17);

	ch_msgpack_write_uint32(pk,"protoID",app->protocol_id);
	ch_msgpack_write_uint64(pk,"sessionID",tsession->session_id);
	ch_msgpack_write_uint64(pk,"reqTime",ch_tcp_session_reqtime_get(tsession));
	ch_msgpack_write_uint64(pk,"resTime",ch_tcp_session_restime_get(tsession));
    ch_msgpack_write_uint8(pk,"isIPV6",ch_tcp_session_is_ipv6(tsession));
	ch_msgpack_write_uint32(pk,"srcIP",ch_tcp_session_srcip_get(tsession));
	ch_msgpack_write_uint32(pk,"dstIP",ch_tcp_session_dstip_get(tsession));
    ch_msgpack_write_bin_kv(pk,"srcAddr",ch_tcp_session_srcaddr_get(tsession),16);
    ch_msgpack_write_bin_kv(pk,"dstAddr",ch_tcp_session_dstaddr_get(tsession),16);

	ch_msgpack_write_uint16(pk,"srcPort",ch_tcp_session_srcport_get(tsession));
	ch_msgpack_write_uint16(pk,"dstPort",ch_tcp_session_dstport_get(tsession));
	ch_msgpack_write_uint64(pk,"reqPackets",session->reqPackets);
	ch_msgpack_write_uint64(pk,"reqBytes",session->reqBytes);
	ch_msgpack_write_uint64(pk,"reqPBytes",session->reqBytes);
	
	ch_msgpack_write_uint64(pk,"resPackets",session->resPackets);
	ch_msgpack_write_uint64(pk,"resBytes",session->resBytes);
	ch_msgpack_write_uint64(pk,"resPBytes",session->resBytes);

	/*packer the session data into msgpack*/
	app->proto_session_format(pk,(void*)session);

    data = pstore->pk_buf.data;
    dlen = pstore->pk_buf.size;


	ch_packet_record_t pkt_rcd;
	pkt_rcd.type = app->pkt_rcd_type;
	pkt_rcd.meta_data_size = 0;
	pkt_rcd.time = ch_tcp_session_reqtime_get(tsession);

	ch_packet_record_put(shm_fmt,&pkt_rcd,data,dlen);

    msgpack_sbuffer_clear(&pstore->pk_buf);

	_pstore_shm_flush(pstore);

	/*ok*/
	return 0;

}


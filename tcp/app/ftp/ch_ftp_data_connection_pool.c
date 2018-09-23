/*
 *
 *      Filename: ch_ftp_data_connection_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 11:32:35
 * Last Modified: 2018-09-21 11:00:55
 */

#include "ch_ftp_data_connection_pool.h"

static ch_ftp_data_connection_pool_t fd_conn_pool_tmp,*ftp_conn_pool = &fd_conn_pool_tmp;

#include "do_ftp_data_session_entry_create.c"
#include "do_ftp_data_session_parse.c"
#include "do_ftp_data_session_format.c"

void ch_ftp_data_connection_pool_init(ch_pool_t *mp,const char *fstore_dir,int fstore_dir_create_type){

	int i;
	ftp_conn_pool->fstore_dir = fstore_dir;
	ftp_conn_pool->fstore_dir_create_type = fstore_dir_create_type;

	ch_ftp_data_connection_list_head_t *header;

	for(i = 0;i<MAX_THREAD_NUM;i++){
	
		header = &ftp_conn_pool->dcon_header_arr[i];

		header->fstore_path = ch_fpath_create(mp,fstore_dir,fstore_dir_create_type,0,i+1);

		INIT_LIST_HEAD(&header->dcon_list);
	}
	
}

#define LIST_HEAD_GET(task_id) &(ftp_conn_pool->dcon_header_arr[(task_id)].dcon_list)

#define ENTRY_IS_EQUAL(dcon,pkt_tcp) \
	((((pkt_tcp)->src_ip==(dcon)->dst_ip)&&((pkt_tcp)->src_port == (dcon)->dst_port)&&((pkt_tcp)->dst_ip == (dcon)->src_ip))||\
	(((pkt_tcp)->dst_ip==(dcon)->dst_ip)&&((pkt_tcp)->dst_port == (dcon)->dst_port)&&((pkt_tcp)->src_ip == (dcon)->src_ip)))

#define TASK_ID_CHECK(task_id,is_ret,retv) do { \
	if(task_id>=MAX_THREAD_NUM){ \
		if(is_ret) \
			return retv; \
		return; \
	} \
}while(0)

static ch_tcp_app_t * _ftp_data_app_create(ch_pool_t *mp){

	ch_tcp_app_t *app = (ch_tcp_app_t *)ch_pcalloc(mp,sizeof(*app));

    app->protocol_id = PROTOCOL_FTP;
    app->pkt_rcd_type = PKT_RECORD_TYPE_TCP_FTP_DATA;
    app->context = NULL;
	app->is_accept_by_port = NULL;
	app->is_accept_by_content = NULL;
    app->proto_session_entry_create = do_ftp_data_session_entry_create;
    app->proto_session_entry_clean = do_ftp_data_session_entry_clean;
    app->proto_session_format = do_ftp_data_session_format;
	app->request_content_parse = do_ftp_data_session_request_parse;
	app->response_content_parse = do_ftp_data_session_response_parse;

}

ch_ftp_data_connection_t * ch_ftp_data_connection_create(uint32_t task_id,ch_ftp_session_t *ftp_session,
	uint32_t src_ip,uint32_t dst_ip,uint32_t dst_port){

	TASK_ID_CHECK(task_id,1,NULL);
	struct list_head *h = LIST_HEAD_GET(task_id);
    ch_pool_t *mp;

    mp = ch_pool_create(1024);

    if(mp == NULL){
    
        ch_log(CH_LOG_ERR,"Cannot Create a memmory pool for ftp data connection!");
        return NULL;
    }

	ch_ftp_data_connection_t *fd_conn = (ch_ftp_data_connection_t*)ch_pcalloc(mp,sizeof(*fd_conn));

    fd_conn->mp = mp;
	fd_conn->ftp_data_app = _ftp_data_app_create(mp);
	fd_conn->ftp_data_app->context = (void*)fd_conn;
	fd_conn->src_ip = src_ip;
	fd_conn->dst_ip = dst_ip;
	fd_conn->dst_port = dst_port;

	fd_conn->ftp_session = ftp_session;
	fd_conn->header = &(ftp_conn_pool->dcon_header_arr[task_id]);

    fd_conn->data_session_entry = NULL;

	list_add(&fd_conn->node,h);

	return fd_conn;
}

ch_ftp_data_connection_t * ch_ftp_data_connection_find(uint32_t task_id,ch_packet_tcp_t *pkt_tcp){

	TASK_ID_CHECK(task_id,1,NULL);
	struct list_head *h = LIST_HEAD_GET(task_id);

	ch_ftp_data_connection_t *fd_conn;

	list_for_each_entry(fd_conn,h,node){

		if(ENTRY_IS_EQUAL(fd_conn,pkt_tcp))
			return fd_conn;
	}

	return fd_conn;
}

void ch_ftp_data_connection_fin(uint32_t task_id,ch_ftp_data_connection_t *fd_conn){

	TASK_ID_CHECK(task_id,0,NULL);

	list_del(&fd_conn->node);
    ch_pool_destroy(fd_conn->mp);

}

void ch_ftp_data_connection_fin_output(uint32_t task_id,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_ftp_data_connection_t *fd_conn){

	TASK_ID_CHECK(task_id,0,NULL);

    if(fd_conn->data_session_entry){
    
    
        ch_ftp_data_session_entry_fin_output(pstore,tsession,fd_conn->data_session_entry);

    }

    ch_ftp_data_connection_fin(task_id,fd_conn);

}


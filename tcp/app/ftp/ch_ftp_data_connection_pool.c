/*
 *
 *      Filename: ch_ftp_data_connection_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 11:32:35
 * Last Modified: 2018-09-20 17:09:01
 */

#include "ch_ftp_data_connection_pool.h"

static ch_ftp_data_connection_pool_t fd_conn_pool_tmp,*ftp_conn_pool = &fd_conn_pool_tmp;

static ch_tcp_app_t ftp_data_app = {

    .protocol_id = PROTOCOL_FTP,
    .pkt_rcd_type = PKT_RECORD_TYPE_TCP_FTP_DATA,
    .context = NULL,
	.is_accept_by_port = NULL,
	.is_accept_by_content = NULL,
    .proto_session_entry_create = do_ftp_data_session_entry_create,
    .proto_session_entry_clean = do_ftp_data_session_entry_clean,
    .proto_session_format = do_ftp_data_session_format,
	.request_content_parse = do_ftp_data_request_parse,
	.response_content_parse = do_ftp_data_response_parse
};

void ch_ftp_data_connection_pool_init(const char *fstore_dir,int fstore_dir_create_type){

	int i;
	ftp_conn_pool->fstore_dir = fstore_dir;
	ftp_conn_pool->fstore_dir_create_type = fstore_dir_create_type;

	for(i = 0;i<MAX_THREAD_NUM;i++){
	
		INIT_LIST_HEAD(&ftp_conn_pool->dcon_list[i]);
	}
	
}

#define LIST_HEAD_GET(task_id) &(ftp_conn_pool->dcon_list[(task_id)])

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

ch_ftp_data_connection_t * ch_ftp_data_connection_create(ch_pool_t *mp,uint32_t task_id,ch_ftp_session_t *ftp_session,
	uint32_t src_ip,uint32_t dst_ip,uint32_t dst_port){

	TASK_ID_CHECK(task_id,1,NULL);
	struct list_head *h = LIST_HEAD_GET(task_id);

	ch_ftp_data_connection_t *fd_conn = (ch_ftp_data_connection_t*)ch_pcalloc(mp,sizeof(*fd_conn));
	if(fd_conn == NULL)
	{
	
		ch_log(CH_LOG_ERR,"Have no memory to create ftp data connection!");
		return NULL;
	}

	fd_conn->ftp_data_app = &ftp_data_app;
	fd_conn->ftp_data_app->context = (void*)fd_conn;
	fd_conn->src_ip = src_ip;
	fd_conn->dst_ip = dst_ip;
	fd_conn->dst_port = dst_port;

	fd_conn->ftp_session = ftp_session;
	fd_conn->ftp_dcon_pool = ftp_conn_pool;

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

void ch_ftp_data_connection_remove(uint32_t task_id,ch_ftp_data_connection_t *fd_conn){

	TASK_ID_CHECK(task_id,0,NULL);

	list_del(&fd_conn->node);
}


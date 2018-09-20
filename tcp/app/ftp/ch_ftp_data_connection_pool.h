/*
 *
 *      Filename: ch_ftp_data_connection_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 11:08:03
 * Last Modified: 2018-09-20 11:08:03
 */

#ifndef CH_FTP_DATA_CONNECTION_POOL_H
#define CH_FTP_DATA_CONNECTION_POOL_H

typedef struct ch_ftp_data_connection_pool_t ch_ftp_data_connection_pool_t;
typedef struct ch_ftp_data_connection_t ch_ftp_data_connection_t;

#include "ch_packet_tcp.h"
#include "ch_list.h"
#include "ch_ftp_session.h"

#define MAX_THREAD_NUM 256

struct ch_ftp_data_connection_pool_t {

	struct list_head dcon_list[MAX_THREAD_NUM];
	
	const char *fstore_dir;
	int fstore_dir_create_type;
};


struct ch_ftp_data_connection_t {

	struct list_head node;

	ch_ftp_data_connection_pool_t *ftp_dcon_pool;
	ch_tcp_app_t *ftp_data_app;

	ch_ftp_session_t *ftp_session;

	uint32_t src_ip;
	uint32_t dst_ip;
	uint32_t dst_port;

};

static inline ch_ftp_session_t * ch_ftp_session_get_from_app(ch_tcp_app_t *app){

	ch_ftp_data_connection_t *ftp_dcon = (ch_ftp_data_connection_t*)app->context;
	return ftp_dcon->ftp_session;
}

extern void ch_ftp_data_connection_pool_init(const char *fstore_dir,int fstore_dir_create_type);

extern ch_ftp_data_connection_t * ch_ftp_data_connection_create(uint32_t task_id,ch_ftp_session_t *ftp_session,
	uint32_t src_ip,uint32_t dst_ip,uint32_t dst_port);

extern ch_ftp_data_connection_t * ch_ftp_data_connection_find(uint32_t task_id,ch_packet_tcp_t *pkt_tcp);

extern void ch_ftp_data_connection_remove(uint32_t task_id,ch_ftp_data_connection_t *fd_conn);


#endif /*CH_FTP_DATA_CONNECTION_POOL_H*/

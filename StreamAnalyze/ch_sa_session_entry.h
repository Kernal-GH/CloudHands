/*
 *
 *      Filename: ch_sa_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-22 17:25:29
 * Last Modified: 2018-03-22 17:25:29
 */

#ifndef CH_SA_SESSION_ENTRY_H
#define CH_SA_SESSION_ENTRY_H


typedef struct ch_sa_session_entry_t ch_sa_session_entry_t;
typedef struct ch_sa_session_tcp_request_entry_t ch_sa_session_tcp_request_entry_t;

#include "ch_sa_data_store_pool.h"

struct ch_sa_session_tcp_request_entry_t {

	uint64_t req_packets;
	uint64_t res_packets;

	uint64_t req_start_time;
	uint64_t req_last_time;

	uint64_t res_start_time;
	uint64_t res_last_time;

};

struct ch_sa_session_entry_t {

	ch_sa_data_store_t *req_dstore;
	ch_sa_data_store_t *res_dstore;

	int req_error;
	int res_error;

	uint64_t req_packets;
	uint64_t res_packets;
	uint64_t req_bytes;
	uint64_t res_bytes;
	uint64_t req_start_time;
	uint64_t req_last_time;

	uint64_t res_start_time;
	uint64_t res_last_time;
};

#define ch_sa_sentry_dstore_free(sentry) do {						\
	ch_sa_data_store_t *rq_dstore = sentry->req_dstore;				\
	ch_sa_data_store_t *rs_dstore = sentry->res_dstore;				\
	if(rq_dstore)													\
		ch_sa_data_store_free(rq_dstore);							\
	if(rs_dstore)													\
		ch_sa_data_store_free(rs_dstore);							\
}while(0)

#define ch_sa_sentry_dstore_size(dstore) ((dstore)?ch_sa_data_store_size(dstore):0)

#endif /*CH_SA_SESSION_ENTRY_H*/

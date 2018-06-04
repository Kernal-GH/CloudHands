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

#include "ch_rdb_store.h"

struct ch_sa_session_tcp_request_entry_t {

	uint64_t req_packets;
	uint64_t res_packets;

	uint64_t req_start_time;
	uint64_t req_last_time;

	uint64_t res_start_time;
	uint64_t res_last_time;

};

struct ch_sa_session_entry_t {

	ch_rdb_store_t *rdb_store;

	uint64_t req_packets;
	uint64_t res_packets;
	uint64_t req_bytes;
	uint64_t res_bytes;
	uint64_t req_start_time;
	uint64_t req_last_time;

	uint64_t res_start_time;
	uint64_t res_last_time;

	uint32_t req_data_size;
	uint32_t res_data_size;
	uint32_t req_data_seq;
	uint32_t res_data_seq;
};

#endif /*CH_SA_SESSION_ENTRY_H*/

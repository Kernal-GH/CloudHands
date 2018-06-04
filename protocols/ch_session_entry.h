/*
 *
 *      Filename: ch_session_entry.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-24 03:00:07
 * Last Modified: 2016-10-24 03:00:07
 */

#ifndef CH_SESSION_ENTRY_H
#define CH_SESSION_ENTRY_H

typedef struct ch_session_entry_t ch_session_entry_t;

#include <apr_pools.h>
#include "ch_util.h"
#include "ch_list.h"
#include "ch_mmap_file_format.h"
#include "ch_table.h"

struct ch_session_entry_t {

	ch_table_entry_t entry;

	void *priv_data;

    uint64_t session_id;
    uint64_t req_time;
    uint64_t res_time;

    uint32_t protocol_id;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
};

static inline void ch_session_entry_init(ch_session_entry_t *sentry, ch_mmap_file_entry_t *fentry)
{
    sentry->entry.last_time = 0;

    sentry->session_id = fentry->session_id;

	if(fentry->session_direct == SESSION_DIRECT_REQ){
	
		sentry->req_time = fentry->time;
		sentry->res_time = 0;
	}else{
	
		sentry->res_time = fentry->time;
		sentry->req_time =0;
	}

    sentry->protocol_id = fentry->protocol_id;
    sentry->src_ip = fentry->src_ip;
    sentry->dst_ip = fentry->dst_ip;
    sentry->src_port = fentry->src_port;
    sentry->dst_port = fentry->dst_port;
}

#endif /* CH_SESSION_ENTRY_H */

/*
 *
 *      Filename: ch_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:40:00
 * Last Modified: 2018-05-14 17:40:00
 */

#ifndef CH_SESSION_ENTRY_H
#define CH_SESSION_ENTRY_H

typedef struct ch_session_entry_t ch_session_entry_t;

#include "ch_ptable.h"
#include "ch_pp_pool.h"

struct ch_session_entry_t {
	
	ch_ptable_entry_t entry;
	ch_pp_parser_t *pp_parser;

	void *session;
    
	uint64_t session_id;
    uint64_t req_time;
    uint64_t res_time;

    uint32_t protocol_id;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;

	uint64_t req_packets;
	uint64_t req_bytes;
	uint64_t req_pbytes;

	uint64_t res_packets;
	uint64_t res_bytes;
	uint64_t res_pbytes;
	
};

#define ch_session_entry_update(sen,tcp_rcd) do { \
	(sen)->req_packets+=(tcp_rcd)->req_packets;   \
	(sen)->req_bytes += (tcp_rcd)->req_bytes;     \
	(sen)->res_packets+=(tcp_rcd)->res_packets;   \
	(sen)->res_bytes += (tcp_rcd)->res_bytes;     \
}while(0)

#define ch_session_entry_req_pbytes_update(sen,dlen) ((sen)->req_pbytes+=(dlen))
#define ch_session_entry_res_pbytes_update(sen,dlen) ((sen)->res_pbytes+=(dlen))

extern int ch_session_entry_init(ch_session_entry_t *sentry,ch_pp_parser_t *pparser,ch_tcp_record_t *tcp_record);

#define ch_my_session_entry_get(sentry,entry_type) (entry_type*)((sentry)->session)

#define ch_my_parse_context_get(sentry,p_type) (p_type*)((sentry)->pp_parser->parse_context)

#endif /*CH_SESSION_ENTRY_H*/

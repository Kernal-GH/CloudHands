/*
 *
 *      Filename: ch_dns_session.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 21:19:54
 * Last Modified: 2016-10-30 21:19:54
 */

#ifndef CH_DNS_SESSION_H
#define CH_DNS_SESSION_H

typedef struct ch_dns_hdr_t ch_dns_hdr_t;
typedef struct ch_dns_question_t ch_dns_question_t;
typedef struct ch_dns_answer_t ch_dns_answer_t;
typedef struct ch_dns_session_t ch_dns_session_t;

#include <stdint.h>
#include <apr_pools.h>
#include "ch_list.h"
#include "ch_dns_rdata_parser.h"

#define DNS_PORT 53
#define DNS_HDR_LEN 12

struct ch_dns_hdr_t {
	uint16_t id;
	uint16_t flags;
	int8_t qr;
	int8_t AA;
	int8_t TC;
	uint8_t rcode;
	uint8_t opcode;

	uint16_t qcount;
	uint16_t ancount;
	uint16_t aucount;
	uint16_t adcount;	
};

struct ch_dns_question_t {

	struct ch_list_head node;
	
	const char* qname;
	uint16_t qtype;
	uint16_t qclass;
};

struct ch_dns_answer_t {
	struct ch_list_head node;

	const char* rname;
	uint16_t rtype;
	uint16_t rclass;
	uint32_t rttl;
	uint16_t rdatalen;
	ch_dns_rdata_t *rdata;
};

struct ch_dns_session_t {

	apr_pool_t *mp;
	ch_dns_hdr_t hdr;

	uint16_t qcount;
	struct ch_list_head qlist;
	
	uint16_t ancount;
	struct ch_list_head alist;
	
	uint16_t aucount;
	struct ch_list_head aulist;

	uint16_t adcount;
	struct ch_list_head adlist;

};


extern ch_dns_session_t * ch_dns_session_create(apr_pool_t *mp);

extern ch_dns_question_t * ch_dns_question_create(ch_dns_session_t *session);

extern ch_dns_answer_t * ch_dns_answer_create(ch_dns_session_t *session);

static inline void ch_dns_qlist_add(ch_dns_session_t *session,ch_dns_question_t *q){


	list_add_tail(&q->node,&session->qlist);
	session->qcount+=1;
}

static inline void ch_dns_alist_add(ch_dns_session_t *session,ch_dns_answer_t *ans){


	list_add_tail(&ans->node,&session->alist);
	session->ancount+=1;
}

static inline void ch_dns_aulist_add(ch_dns_session_t *session,ch_dns_answer_t *au){


	list_add_tail(&au->node,&session->aulist);
	session->aucount+=1;
}

static inline void ch_dns_adlist_add(ch_dns_session_t *session,ch_dns_answer_t *ad){


	list_add_tail(&ad->node,&session->adlist);
	session->adcount+=1;
}

#endif /* CH_DNS_SESSION_H */

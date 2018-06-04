/*
 *
 *      Filename: ch_dns_session.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-11-01 23:16:04
 * Last Modified: 2016-11-01 23:52:16
 */

#include "ch_dns_session.h"

ch_dns_session_t * ch_dns_session_create(apr_pool_t *mp){

	
	ch_dns_session_t *session = (ch_dns_session_t*)apr_palloc(mp,sizeof(*session));

	session->mp = mp;

	session->hdr.id = 0;
	session->hdr.flags = 0;
	session->hdr.qr = 0;
	session->hdr.AA = 0;
	session->hdr.TC = 0;
	session->hdr.rcode = 0;
	session->hdr.opcode = 0;
	session->hdr.qcount = 0;
	session->hdr.ancount = 0;
	session->hdr.aucount = 0;
	session->hdr.adcount = 0;

	session->qcount = 0;
	CH_INIT_LIST_HEAD(&session->qlist);

	session->ancount = 0;
	CH_INIT_LIST_HEAD(&session->alist);

	session->aucount = 0;
	CH_INIT_LIST_HEAD(&session->aulist);
	
	session->adcount = 0;
	CH_INIT_LIST_HEAD(&session->adlist);

	return session;
}

ch_dns_question_t * ch_dns_question_create(ch_dns_session_t *session){

	ch_dns_question_t *q = (ch_dns_question_t*)apr_palloc(session->mp,sizeof(*q));

	q->qname = NULL;
	q->qtype = 0;
	q->qclass = 0;

	return q;
}

ch_dns_answer_t * ch_dns_answer_create(ch_dns_session_t *session){

	ch_dns_answer_t *ans = (ch_dns_answer_t*)apr_palloc(session->mp,sizeof(*ans));
	ans->rname = NULL;
	ans->rtype = 0;
	ans->rclass = 0;
	ans->rttl = 0;
	ans->rdatalen = 0;
	ans->rdata = NULL;

	return ans;
}

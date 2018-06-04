/*
 *
 *      Filename: ch_dns_session_format_msgpack.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-11-02 01:03:16
 * Last Modified: 2016-11-02 03:01:12
 */

static void _dns_rr_list_format(msgpack_packer *pk,const char *name,struct ch_list_head *h,uint16_t count){

	ch_dns_answer_t *rr;

	ch_msgpack_array_start(pk,name,count);

	list_for_each_entry(rr,h,node){
	
		ch_msgpack_map_start(pk,NULL,6);
		ch_msgpack_write_kv(pk,"rname",rr->rname);
		ch_msgpack_write_uint16(pk,"rtype",rr->rtype);
		ch_msgpack_write_uint16(pk,"rclass",rr->rclass);
		ch_msgpack_write_uint32(pk,"rttl",rr->rttl);
		ch_msgpack_write_uint16(pk,"rdatalen",rr->rdatalen);

		rr->rdata->msgpack_format(pk,rr->rdata);

	}
}

static void _dns_question_list_format(msgpack_packer *pk,ch_dns_session_t *dns_session){

	ch_dns_question_t *q;

	ch_msgpack_array_start(pk,"question",dns_session->qcount);

	list_for_each_entry(q,&dns_session->qlist,node){
	
		ch_msgpack_map_start(pk,NULL,3);
		ch_msgpack_write_kv(pk,"qname",q->qname);
		ch_msgpack_write_uint16(pk,"qtype",q->qtype);
		ch_msgpack_write_uint16(pk,"qclass",q->qclass);

	}
}

static int _dns_session_format(ch_session_format_t *fmt,void *session,void *priv_data){

	/* unused */
	priv_data = priv_data;

	ch_dns_session_t *dns_session = (ch_dns_session_t*)session;
	ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;
	msgpack_packer *pk = &msgpack_fmt->pk;

	ch_dns_hdr_t *hdr = &dns_session->hdr;

	ch_msgpack_map_start(pk,"dns",10);
	
	/* format header */
	ch_msgpack_write_uint16(pk,"id",hdr->id);
	ch_msgpack_write_uint16(pk,"flags",hdr->flags);

	ch_msgpack_write_uint16(pk,"qcount",hdr->qcount);
	ch_msgpack_write_uint16(pk,"ancount",hdr->ancount);
	ch_msgpack_write_uint16(pk,"aucount",hdr->aucount);
	ch_msgpack_write_uint16(pk,"adcount",hdr->adcount);

	/* format questions */
	_dns_question_list_format(pk,dns_session);

	/* format answers */
	_dns_rr_list_format(pk,"ans",&dns_session->alist,dns_session->ancount);

	/* format auth */
	_dns_rr_list_format(pk,"auth",&dns_session->aulist,dns_session->aucount);
	
	/* format addtionals */
	_dns_rr_list_format(pk,"add",&dns_session->adlist,dns_session->adcount);
	
	return 0;
}

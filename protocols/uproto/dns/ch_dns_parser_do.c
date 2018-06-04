/*
 *
 *      Filename: ch_dns_parser_do.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-31 00:21:25
 * Last Modified: 2016-11-02 03:20:56
 */

/* rr types */
#define DNS_RR_AN 0
#define DNS_RR_AU 1
#define DNS_RR_AD 2

static int _dns_parse_request(ch_app_proto_t *aproto,ch_session_entry_t *sentry,
	void *data,uint32_t dlen,void *priv_data){

	/* do nothiong */
	aproto = aproto;
	sentry = sentry;
	data = data;
	dlen = dlen;
	priv_data = priv_data;

	return PARSE_BREAK;	
}


static int _dns_rr_parse(apr_pool_t *mp,ch_dns_session_t *dns_session,const unsigned char *packet,
	uint32_t *packet_p,uint32_t id_pos,uint32_t len,uint16_t count,int rr_type){

	uint16_t i;
	int rc;

	ch_dns_answer_t *ans;

	for(i = 0; i<count; i++){
		ans = ch_dns_answer_create(dns_session);

		rc = ch_dns_answer_parse(mp,ans,
			packet,
			packet_p,
			id_pos,
			len);
		
		if(rc == -1){
		
			return -1;
		}

		switch(rr_type){
		
		case DNS_RR_AN:
			ch_dns_alist_add(dns_session,ans);
			break;

		case DNS_RR_AU:
			ch_dns_aulist_add(dns_session,ans);
			break;
		
		case DNS_RR_AD:
			ch_dns_adlist_add(dns_session,ans);
			break;
		default:
			break;
		}
	}

	return 0;
}

static int _dns_parse_response(ch_app_proto_t *aproto,ch_session_entry_t *sentry,
	void *data,uint32_t dlen,void *priv_data){

	/* unused */
	priv_data = priv_data;

	uint16_t i;
	uint32_t pos = 0;
	uint32_t id_pos = 0;
	int rc;

	ch_dns_question_t *q;
	ch_dns_session_t *dns_session;
	ch_dns_proto_t *dns_proto = (ch_dns_proto_t*)aproto;

	apr_pool_t *mp = ch_dns_proto_mp_get(dns_proto);

	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot get memory pool to parse dns protocol!");
		return PARSE_BREAK;
	}

	/* parse the header */
	if(dlen-pos<12){

		ch_log(CH_LOG_WARN,"The dns header has been truncated!");
		return PARSE_BREAK;
	}

	dns_session = ch_dns_session_create(mp);

	rc = ch_dns_hdr_parse(&dns_session->hdr,(const unsigned char*)data,
		pos,dlen);
	if(rc == -1){
	
		return PARSE_BREAK;
	}

	/* parse dns questions */
	pos += DNS_HDR_LEN;

	for(i = 0; i<dns_session->hdr.qcount;i++){
	
		q = ch_dns_question_create(dns_session);

		rc = ch_dns_question_parse(mp,q,
			(const unsigned char *)data,
			&pos,
			id_pos,
			dlen);

		if(rc == -1){
		
			return PARSE_BREAK;
		}

		ch_dns_qlist_add(dns_session,q);
	}

	/* parse dns answer */
	if(pos<dlen){

		rc = _dns_rr_parse(mp,dns_session,
			(const unsigned char*)data,
			&pos,id_pos,dlen,dns_session->hdr.ancount,DNS_RR_AN);
		if(rc == -1){
		
			return PARSE_BREAK;
		}
	}

	/* parse auth answer */
	if(pos<dlen){

		 _dns_rr_parse(mp,dns_session,
			(const unsigned char*)data,
			&pos,id_pos,dlen,dns_session->hdr.aucount,DNS_RR_AU);
		
	}

	/* parse additional  */
	if(pos<dlen){
		_dns_rr_parse(mp,dns_session,
			(const unsigned char*)data,
			&pos,id_pos,dlen,dns_session->hdr.adcount,DNS_RR_AD);
	}

	/* ok format the parse result */
	ch_session_format(aproto->fmt,sentry,(void*)dns_session,NULL);
	dns_proto->sessions_parsed+=1;

	return	PARSE_DONE;
}

static int _dns_is_request(ch_app_proto_t *aproto,ch_mmap_file_entry_t *fe,void *priv_data){

	/* unused */
	aproto = aproto;
	priv_data = priv_data;

	return fe->dst_port == DNS_PORT;
}




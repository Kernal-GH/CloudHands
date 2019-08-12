/*
 *
 *      Filename: ch_dns_request.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-07 16:13:58
 * Last Modified: 2018-06-26 12:04:00
 */

#include "ch_dns_request.h"
#include "ch_log.h"

ch_dns_requst_t *ch_dns_request_parse(ch_pool_t *mp,ch_dns_data_input_t *din){

	int i;
	ch_dns_question_t *dnsq;

	ch_dns_requst_t *dns_req = ch_pcalloc(mp,sizeof(*dns_req));

	INIT_LIST_HEAD(&dns_req->questions);

	if(ch_dns_header_parse(din,&dns_req->hdr)){
	
		ch_log(CH_LOG_ERR,"Parse dns request header failed!");
		return NULL;
	}

	for(i = 0;i<dns_req->hdr.qcount;i++){
		
		if(ch_dns_data_input_empty(din))
			break;
	
		dnsq = ch_dns_question_parse(mp,din);
		if(dnsq == NULL){
		
			ch_log(CH_LOG_ERR,"Parse request question failed!");
			return NULL;
		}

		list_add_tail(&dnsq->node,&dns_req->questions);

	}

	return dns_req;
}

ch_dns_question_t *ch_dns_question_parse(ch_pool_t *mp,ch_dns_data_input_t *din){


	ch_dns_question_t *dnsq = ch_pcalloc(mp,sizeof(*dnsq));

	if(ch_dns_name_parse(mp,din,&dnsq->qname)){
	
		ch_log(CH_LOG_ERR,"Parse question domain name failed!");
		return NULL;

	}

	if(ch_dns_data_input_rdlen(din)<4){
	
		ch_log(CH_LOG_ERR,"Parse question failed,invalid dns packet!");
		return NULL;
	}

	dnsq->qtype = ch_dns_data_input_uint16_read(din);
	dnsq->qclass = ch_dns_data_input_uint16_read(din);


	return dnsq;

}

void ch_dns_question_dump(ch_dns_question_t *dnsq,FILE *fp){

	fprintf(fp,"dns.request.question:\n");
	fprintf(fp,"dns.request.question.name:\n");
	ch_dns_name_dump(&dnsq->qname,fp);
	fprintf(fp,"dns.request.question.qtype:%u\n",dnsq->qtype);
	fprintf(fp,"dns.request.question.qclass:%u\n",dnsq->qclass);

}

void ch_dns_request_dump(ch_dns_requst_t *dnsr,FILE *fp){

	ch_dns_question_t *dnsq;

	fprintf(fp,"dns.request.header:\n");
	ch_dns_header_dump(&dnsr->hdr,fp);

	list_for_each_entry(dnsq,&dnsr->questions,node){
	
		ch_dns_question_dump(dnsq,fp);
	}

}

const char *ch_dns_request_name_get(ch_dns_requst_t *dns_req){

	ch_dns_question_t *dnsq;

	list_for_each_entry(dnsq,&dns_req->questions,node){
	
        if(dnsq->qtype == 1&&(dnsq->qname.ndata&&strlen((const char*)dnsq->qname.ndata))>0){

            return (const char*)dnsq->qname.ndata;
        }
	}

    return NULL;

}

void ch_dns_question_store(ch_dns_question_t *dnsq,ch_msgpack_store_t *dstore){

    ch_msgpack_store_map_start(dstore,"question",3);
    ch_dns_name_store(&dnsq->qname,dstore);
    ch_msgpack_store_write_uint16(dstore,"qtype",dnsq->qtype);
    ch_msgpack_store_write_uint16(dstore,"qclass",dnsq->qclass);
}

ssize_t ch_dns_question_write(ch_dns_question_t *dnsq,ch_data_output_t *dout){

	ssize_t rc,len = 0;

	CH_DNS_NAME_WRITE(dout,&dnsq->qname,len,rc);
	CH_DOUT_UINT16_WRITE(dout,dnsq->qtype,len,rc);
	CH_DOUT_UINT16_WRITE(dout,dnsq->qclass,len,rc);

	return len;
}

static inline uint16_t _dns_questions_count(ch_dns_requst_t *dnsr){

	ch_dns_question_t *dnsq;
    uint16_t c = 0;
	
    list_for_each_entry(dnsq,&dnsr->questions,node){

        c++;
	}

    return c;
}

ssize_t ch_dns_request_write(ch_dns_requst_t *dnsr,ch_data_output_t *dout){

	ch_dns_question_t *dnsq;

	ssize_t rc,len = 0;
	CH_DNS_HEADER_WRITE(&dnsr->hdr,dout,len,rc);
	
	list_for_each_entry(dnsq,&dnsr->questions,node){
	
		if(-1 == (rc=ch_dns_question_write(dnsq,dout)))
			return -1;

		len+=rc;
	}

	return len;
}

void  ch_dns_request_store(ch_msgpack_store_t *dstore,ch_dns_requst_t *dnsr) {

    ch_msgpack_store_map_start(dstore,"req",2);

    ch_dns_header_store(&dnsr->hdr,dstore);
    ch_msgpack_store_array_start(dstore,"questions",_dns_questions_count(dnsr));

	ch_dns_question_t *dnsq;
	
    list_for_each_entry(dnsq,&dnsr->questions,node){
        
        ch_dns_question_store(dnsq,dstore);
	}
}

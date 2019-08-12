/*
 *
 *      Filename: ch_dns_response.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-07 18:41:54
 * Last Modified: 2018-06-26 12:02:55
 */

#include "ch_dns_response.h"
#include "ch_log.h"

static int _dns_rr_parse(ch_pool_t *mp,
	ch_dns_data_input_t *din,ch_dns_rdata_pool_t *rdata_pool,struct list_head *list,uint16_t n){

	uint16_t i;

	ch_dns_rdata_t *rdata;

	for(i = 0;i<n;i++){
		
		if(ch_dns_data_input_empty(din))
			break;

		rdata = ch_dns_rdata_parse(mp,rdata_pool,din);
		if(rdata == NULL){
		
			ch_log(CH_LOG_ERR,"Parse dns response rdata failed!");
			return -1;
		}

		list_add_tail(&rdata->node,list);
	}

	return 0;
}

ch_dns_response_t* ch_dns_response_parse(ch_pool_t *mp,ch_dns_rdata_pool_t *rdata_pool,ch_dns_data_input_t *din){

	uint16_t i;

	ch_dns_question_t *dnsq = NULL;
	ch_dns_response_t *dnsr = (ch_dns_response_t*)ch_pcalloc(mp,sizeof(*dnsr));

	if(ch_dns_header_parse(din,&dnsr->hdr)){
	
		ch_log(CH_LOG_ERR,"Cannot parse dns response header!");

		return NULL;
	}

	INIT_LIST_HEAD(&dnsr->qlist);
	INIT_LIST_HEAD(&dnsr->alist);
	INIT_LIST_HEAD(&dnsr->aulist);
	INIT_LIST_HEAD(&dnsr->adlist);

	for(i = 0;i<dnsr->hdr.qcount;i++){

		if(ch_dns_data_input_empty(din))
			break;

		dnsq = ch_dns_question_parse(mp,din);
		if(dnsq == NULL){
		
			ch_log(CH_LOG_ERR,"Parse request question failed!");
			return NULL;
		}

		list_add_tail(&dnsq->node,&dnsr->qlist);
	}

	if(dnsr->hdr.ancount){
	
		if(_dns_rr_parse(mp,din,rdata_pool,&dnsr->alist,dnsr->hdr.ancount)){
		
			ch_log(CH_LOG_ERR,"Parse dns response alist failed!");

			return NULL;
		}
	}
	
	if(dnsr->hdr.aucount){
	
		
		if(_dns_rr_parse(mp,din,rdata_pool,&dnsr->aulist,dnsr->hdr.aucount)){
		
			ch_log(CH_LOG_ERR,"Parse dns response aulist failed!");

			return NULL;
		}
	}

	if(dnsr->hdr.adcount){
	
		if(_dns_rr_parse(mp,din,rdata_pool,&dnsr->adlist,dnsr->hdr.adcount)){
		
			ch_log(CH_LOG_ERR,"Parse dns response adlist failed!");

			return NULL;
		}
	}

	return dnsr;
}

static inline void _rdata_list_dump(struct list_head *list,const char *key,FILE *fp){

	ch_dns_rdata_t *rdata;

	fprintf(fp,"dns.response.rdata.%s:\n",key);

	list_for_each_entry(rdata,list,node){
	
		ch_dns_rdata_dump(rdata,fp);

	}
}

void ch_dns_response_dump(ch_dns_response_t *dnsr,FILE *fp){

	fprintf(fp,"dns.response.hdr:\n");
	ch_dns_header_dump(&dnsr->hdr,fp);


	fprintf(fp,"dns.response.qlist:\n");
	ch_dns_question_t *qu;
	list_for_each_entry(qu,&dnsr->qlist,node){
	
		ch_dns_question_dump(qu,fp);
	}

	_rdata_list_dump(&dnsr->alist,"alist",fp);
	_rdata_list_dump(&dnsr->aulist,"aulist",fp);
	_rdata_list_dump(&dnsr->adlist,"adlist",fp);
}

static inline uint16_t _rdata_list_count(struct list_head *list){

    uint16_t c = 0;
    ch_dns_rdata_t *rdata;
    list_for_each_entry(rdata,list,node){

        c++;
    }

    return c;
}

static inline void _rdata_list_store(ch_msgpack_store_t *dstore,const char *name,struct list_head *list){

    ch_dns_rdata_t *rdata;

    ch_msgpack_store_map_start(dstore,name,1);
    ch_msgpack_store_array_start(dstore,"rdatas",_rdata_list_count(list));
	
    list_for_each_entry(rdata,list,node){

        ch_dns_rdata_store(rdata,dstore);
	}

}

static inline ssize_t _rdata_list_write(struct list_head *list,ch_data_output_t *dout){

	ssize_t rc,len = 0;
	ch_dns_rdata_t *rdata;

	list_for_each_entry(rdata,list,node){
	
		CH_DNS_RDATA_WRITE(dout,rdata,len,rc);
	}

	return len;
}
#define _RDATA_LIST_WRITE(list,dout,len,rc) do{   \
	if(-1 == (rc = _rdata_list_write(list,dout))) \
		return -1;                                \
	len+=rc;                                      \
}while(0)

static inline uint16_t _qlist_count(struct list_head *qlist){

    uint16_t c = 0;
    ch_dns_question_t *qu;
    list_for_each_entry(qu,qlist,node){

        c++;
    }

    return c;
}

ssize_t ch_dns_response_write(ch_dns_response_t *dnsr,ch_data_output_t *dout){

	
	ssize_t rc,len = 0;
	CH_DNS_HEADER_WRITE(&dnsr->hdr,dout,len,rc);
	
	ch_dns_question_t *qu;
	list_for_each_entry(qu,&dnsr->qlist,node){
	
		CH_DNS_QUESTION_WRITE(dout,qu,len,rc);
	}

	_RDATA_LIST_WRITE(&dnsr->alist,dout,len,rc);
	_RDATA_LIST_WRITE(&dnsr->aulist,dout,len,rc);
	_RDATA_LIST_WRITE(&dnsr->adlist,dout,len,rc);

	return len;
}

void ch_dns_response_store(ch_msgpack_store_t *dstore,ch_dns_response_t *dnsr){

    ch_msgpack_store_map_start(dstore,"res",5);
    ch_dns_header_store(&dnsr->hdr,dstore);
    ch_msgpack_store_array_start(dstore,"questions",_qlist_count(&dnsr->qlist));

	ch_dns_question_t *dnsq;
	
    list_for_each_entry(dnsq,&dnsr->qlist,node){
        
        ch_dns_question_store(dnsq,dstore);
	}

    _rdata_list_store(dstore,"alist",&dnsr->alist);
    _rdata_list_store(dstore,"aulist",&dnsr->aulist);
    _rdata_list_store(dstore,"adlist",&dnsr->adlist);

}

/*
 *
 *      Filename: ch_dns_rdata_rp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-06-14 18:02:03
 */

#include "ch_dns_rdata_rp.h"
#include "ch_log.h"

static void _rdata_rp_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_rp_t *rp = (ch_dns_rdata_rp_t*)rdata;

	fprintf(fp,"Dump The rdata.rp:\n");
	fprintf(fp,"rdata.rp.mailbox:\n");
	ch_dns_name_dump(&rp->mailbox,fp);

	fprintf(fp,"rdata.rp.textdomain:\n");
	ch_dns_name_dump(&rp->textdomain,fp);

}

static ssize_t  _rdata_rp_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_rp_t *rp = (ch_dns_rdata_rp_t*)rdata;
	ch_dns_name_t *mailbox = &rp->mailbox;
	ch_dns_name_t *textdomain = &rp->textdomain;


	CH_DNS_NAME_WRITE(dout,mailbox,len,rc);
	CH_DNS_NAME_WRITE(dout,textdomain,len,rc);

	return len;
}

static void _rdata_rp_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_rp_t *rp = (ch_dns_rdata_rp_t*)rdata;
    ch_dns_name_t *mailbox = &rp->mailbox; 
    ch_dns_name_t *textdomain = &rp->textdomain;
    ch_msgpack_store_map_start(dstore,"rp",2);
    ch_dns_name_store_wkey(mailbox,dstore,"mailbox");
    ch_dns_name_store_wkey(textdomain,dstore,"textdomain");

}

static ch_dns_rdata_t * _rdata_rp_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_rp_t *rp = (ch_dns_rdata_rp_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_rp_t));

	ch_dns_name_t *mailbox = &rp->mailbox;
	ch_dns_name_t *textdomain = &rp->textdomain;



	rp->rdata.rdata_dump = _rdata_rp_dump;
	rp->rdata.rdata_write = _rdata_rp_write;
	rp->rdata.rdata_store = _rdata_rp_store;
	
	CH_DNS_NAME_INIT(mailbox);
	CH_DNS_NAME_INIT(textdomain);

	return (ch_dns_rdata_t*)rp;

}

static int _rdata_rp_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;

	ch_dns_rdata_rp_t *rp = (ch_dns_rdata_rp_t*)rdata;
	
	if(rdata->dlen  == 0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);


	if(ch_dns_name_parse(mp,din,&rp->mailbox)){
	
		ch_log(CH_LOG_ERR,"Parse rp.name.mailbox failed!");
		return -1;
	}
	
	if(ch_dns_name_parse(mp,din,&rp->textdomain)){
	
		ch_log(CH_LOG_ERR,"Parse rp.name.textdomain failed!");
		return -1;
	}

	return 0;
}


static ch_dns_rdata_parser_t rp_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_rp,
	_rdata_rp_create,
	_rdata_rp_parse
};


void ch_dns_rdata_rp_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&rp_parser);

}


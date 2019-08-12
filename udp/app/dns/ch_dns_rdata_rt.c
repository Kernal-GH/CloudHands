/*
 *
 *      Filename: ch_dns_rdata_rt.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-06-14 19:28:24
 */

#include "ch_dns_rdata_rt.h"
#include "ch_log.h"

static void _rdata_rt_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_rt_t *rt = (ch_dns_rdata_rt_t*)rdata;

	fprintf(fp,"Dump The rdata.rt:\n");
	fprintf(fp,"rdata.rt.preference:%u\n",rt->preference);
	fprintf(fp,"rdata.rt.midhost:\n");

	ch_dns_name_dump(&rt->midhost,fp);

}

static ssize_t  _rdata_rt_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_rt_t *rt = (ch_dns_rdata_rt_t*)rdata;
	ch_dns_name_t *name = &rt->midhost;

	CH_DOUT_UINT16_WRITE(dout,rt->preference,len,rc);
	CH_DNS_NAME_WRITE(dout,name,len,rc);

	return len;
}

static void _rdata_rt_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_rt_t *rt = (ch_dns_rdata_rt_t*)rdata; 
     ch_dns_name_t *name = &rt->midhost;

     ch_msgpack_store_map_start(dstore,"rt",2);
     ch_msgpack_store_write_uint16(dstore,"preference",rt->preference);
     ch_dns_name_store(name,dstore);
}

static ch_dns_rdata_t * _rdata_rt_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_rt_t *rt = (ch_dns_rdata_rt_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_rt_t));

	ch_dns_name_t *name = &rt->midhost;



	rt->rdata.rdata_dump = _rdata_rt_dump;
	rt->rdata.rdata_write = _rdata_rt_write;
	rt->rdata.rdata_store = _rdata_rt_store;
    
	rt->preference = 0;
	CH_DNS_NAME_INIT(name);

	return (ch_dns_rdata_t*)rt;

}

static int _rdata_rt_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;

	ch_dns_rdata_rt_t *rt = (ch_dns_rdata_rt_t*)rdata;
	
	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	rt->preference = ch_dns_data_input_uint16_read(din);

	if(ch_dns_name_parse(mp,din,&rt->midhost)){
	
		ch_log(CH_LOG_ERR,"Parse dns.rdata.rt.midhost failed!");
		return -1;
	}

	return 0;
}


static ch_dns_rdata_parser_t rt_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_rt,
	_rdata_rt_create,
	_rdata_rt_parse
};


void ch_dns_rdata_rt_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&rt_parser);

}


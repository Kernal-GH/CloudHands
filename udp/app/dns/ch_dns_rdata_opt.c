/*
 *
 *      Filename: ch_dns_rdata_opt.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:57:41
 */


#include "ch_dns_rdata_opt.h"
#include "ch_log.h"
#include "ch_util.h"

static void _opt_entry_dump(ch_dns_rdata_opt_entry_t *entry,FILE *fp){

	fprintf(fp,"--------------------------------------------\n");
	fprintf(fp,"rdata.opt.entry.code:%u\n",entry->code);
	fprintf(fp,"rdata.opt.entry.dlen:%u\n",entry->dlen);

	ch_hex_string_dump("rdata.opt.entry.data",entry->data,entry->dlen,fp);

}

static void _rdata_opt_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_opt_t *opt = (ch_dns_rdata_opt_t*)rdata;

	fprintf(fp,"Dump The rdata.opt:\n");
	fprintf(fp,"rdata.opt.entres.num:%u\n",opt->entries_n);

	ch_dns_rdata_opt_entry_t *entry;

	list_for_each_entry(entry,&opt->entries,node){
	
		_opt_entry_dump(entry,fp);
	}

}

static ssize_t _opt_entry_write(ch_dns_rdata_opt_entry_t *entry,ch_data_output_t *dout){

	ssize_t rc,len = 0;

	unsigned char *data = entry->data;
	uint16_t dlen = entry->dlen;

	CH_DOUT_UINT16_WRITE(dout,entry->code,len,rc);

	CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);

	return len;
}

static void _opt_entry_store(ch_dns_rdata_opt_entry_t *entry,ch_msgpack_store_t *dstore){

    ch_msgpack_store_map_start(dstore,NULL,2);
    ch_msgpack_store_write_uint16(dstore,"code",entry->code);
    ch_msgpack_store_write_bin_kv(dstore,"data",(void*)entry->data,entry->dlen);

}

#define ENTRY_WRITE(entry,dout,len,rc) do {     \
	if(-1 == (rc=_opt_entry_write(entry,dout))) \
		return -1;                              \
	len+=rc;                                    \
}while(0)

static ssize_t  _rdata_opt_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_opt_t *opt = (ch_dns_rdata_opt_t*)rdata;
	ssize_t rc,len = 0;
	ch_dns_rdata_opt_entry_t *entry;

	CH_DOUT_UINT16_WRITE(dout,opt->entries_n,len,rc);

	list_for_each_entry(entry,&opt->entries,node){
	
		ENTRY_WRITE(entry,dout,len,rc);

	}

	return len;
}

static void _rdata_opt_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_opt_t *opt = (ch_dns_rdata_opt_t*)rdata; 
     ch_msgpack_store_map_start(dstore,"opt",1);
     ch_msgpack_store_array_start(dstore,"entries",opt->entries_n);

     ch_dns_rdata_opt_entry_t *entry; 

     list_for_each_entry(entry,&opt->entries,node){

         _opt_entry_store(entry,dstore);
     }

}

static ch_dns_rdata_t * _rdata_opt_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_opt_t *opt = (ch_dns_rdata_opt_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_opt_t));

	opt->rdata.rdata_dump = _rdata_opt_dump;
	opt->rdata.rdata_write = _rdata_opt_write;
	opt->rdata.rdata_store = _rdata_opt_store;

	opt->entries_n = 0;
	INIT_LIST_HEAD(&opt->entries);

	return (ch_dns_rdata_t*)opt;

}

static ch_dns_rdata_opt_entry_t * _opt_entry_create(ch_pool_t *mp,ch_dns_data_input_t *din){

	ch_dns_rdata_opt_entry_t *entry = ch_pcalloc(mp,sizeof(*entry));
	entry->code = ch_dns_data_input_uint16_read(din);
	entry->dlen = ch_dns_data_input_uint16_read(din);
	CH_DNS_DLEN_CHECK(entry->dlen,din,NULL);
	
	entry->data = ch_dns_data_input_bytes_read(din,mp,entry->dlen);

	return entry;
}

static int _rdata_opt_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	size_t rdlen;

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_opt_t *opt = (ch_dns_rdata_opt_t*)rdata;
	ch_dns_rdata_opt_entry_t *entry;

	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	while((rdlen = ch_dns_data_input_rdlen(din))){
	
		if(rdlen<4)
			break;

		entry = _opt_entry_create(mp,din);
		if(entry == NULL)
			break;

		opt->entries_n+=1;
		list_add_tail(&entry->node,&opt->entries);

	}


	return 0;
}


static ch_dns_rdata_parser_t opt_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_opt,
	_rdata_opt_create,
	_rdata_opt_parse
};


void ch_dns_rdata_opt_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&opt_parser);
}



/*
 *
 *      Filename: ch_dns_rdata_apl.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:43:34
 */


#include "ch_dns_rdata_apl.h"
#include "ch_log.h"
#include "ch_util.h"

static void _apl_entry_dump(ch_dns_rdata_apl_entry_t *entry,FILE *fp){

	fprintf(fp,"--------------------------------------------\n");
	fprintf(fp,"rdata.apl.entry.family:%u\n",entry->family);
	fprintf(fp,"rdata.apl.entry.prefix:%u\n",entry->prefix);
	fprintf(fp,"rdata.apl.entry.neg:%u\n",entry->neg);
	fprintf(fp,"rdata.apl.entry.len:%u\n",entry->dlen);

}

static void _rdata_apl_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_apl_t *apl = (ch_dns_rdata_apl_t*)rdata;

	fprintf(fp,"Dump The rdata.apl:\n");
	fprintf(fp,"rdata.apl.entres.num:%u\n",apl->entries_n);

	ch_dns_rdata_apl_entry_t *entry;

	list_for_each_entry(entry,&apl->entries,node){
	
		_apl_entry_dump(entry,fp);
	}

}

static ssize_t _apl_entry_write(ch_dns_rdata_apl_entry_t *entry,ch_data_output_t *dout){

	ssize_t rc,len = 0;

	unsigned char *data = entry->data;
	uint16_t dlen = entry->dlen;

	CH_DOUT_UINT16_WRITE(dout,entry->family,len,rc);
	CH_DOUT_UINT8_WRITE(dout,entry->prefix,len,rc);
	CH_DOUT_UINT8_WRITE(dout,entry->neg,len,rc);

	CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);

	return len;
}

static void _apl_entry_store(ch_dns_rdata_apl_entry_t *entry,ch_msgpack_store_t *dstore){

    ch_msgpack_store_map_start(dstore,NULL,4);

    ch_msgpack_store_write_uint16(dstore,"family",entry->family);
    ch_msgpack_store_write_uint8(dstore,"prefix",entry->prefix);
    ch_msgpack_store_write_uint8(dstore,"neg",entry->neg);
    ch_msgpack_store_write_bin_kv(dstore,"data",(void*)entry->data,entry->dlen);

}

#define ENTRY_WRITE(entry,dout,len,rc) do {     \
	if(-1 == (rc=_apl_entry_write(entry,dout))) \
		return -1;                              \
	len+=rc;                                    \
}while(0)

static ssize_t  _rdata_apl_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_apl_t *apl = (ch_dns_rdata_apl_t*)rdata;
	ssize_t rc,len = 0;
	ch_dns_rdata_apl_entry_t *entry;

	CH_DOUT_UINT16_WRITE(dout,apl->entries_n,len,rc);

	list_for_each_entry(entry,&apl->entries,node){
	
		ENTRY_WRITE(entry,dout,len,rc);

	}

	return len;
}

static void _rdata_apl_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

	ch_dns_rdata_apl_t *apl = (ch_dns_rdata_apl_t*)rdata;
    ch_dns_rdata_apl_entry_t *entry;  

    ch_msgpack_store_map_start(dstore,"apl",1);
    ch_msgpack_store_array_start(dstore,"entries",apl->entries_n);

    list_for_each_entry(entry,&apl->entries,node){

        _apl_entry_store(entry,dstore);
    }

}

static ch_dns_rdata_t * _rdata_apl_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_apl_t *apl = (ch_dns_rdata_apl_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_apl_t));

	apl->rdata.rdata_dump = _rdata_apl_dump;
	apl->rdata.rdata_write = _rdata_apl_write;
    apl->rdata.rdata_store = _rdata_apl_store;
	apl->entries_n = 0;
	INIT_LIST_HEAD(&apl->entries);

	return (ch_dns_rdata_t*)apl;

}

static ch_dns_rdata_apl_entry_t * _apl_entry_create(ch_pool_t *mp,ch_dns_data_input_t *din){

	ch_dns_rdata_apl_entry_t *entry = ch_pcalloc(mp,sizeof(*entry));
	entry->family = ch_dns_data_input_uint16_read(din);
	entry->prefix = ch_dns_data_input_uint8_read(din);
	
	uint8_t v = ch_dns_data_input_uint8_read(din);

	entry->neg = (v&0x80)!=0;
	entry->dlen = v&(~0x80);
	
	CH_DNS_DLEN_CHECK(entry->dlen,din,NULL);

	entry->data = ch_dns_data_input_bytes_read(din,mp,entry->dlen);

	return entry;
}

static int _rdata_apl_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	size_t rdlen;

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_apl_t *apl = (ch_dns_rdata_apl_t*)rdata;
	ch_dns_rdata_apl_entry_t *entry;

	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	while((rdlen = ch_dns_data_input_rdlen(din))){
	
		if(rdlen<4)
			break;

		entry = _apl_entry_create(mp,din);
		if(entry == NULL)
			break;

		apl->entries_n+=1;
		list_add_tail(&entry->node,&apl->entries);

	}


	return 0;
}


static ch_dns_rdata_parser_t apl_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_apl,
	_rdata_apl_create,
	_rdata_apl_parse
};


void ch_dns_rdata_apl_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&apl_parser);
}



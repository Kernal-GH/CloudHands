/*
 *
 *      Filename: ch_dns_rdata_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-04 18:29:37
 * Last Modified: 2018-06-26 13:27:00
 */

#include "ch_log.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_rdata_cname.h"
#include "ch_dns_rdata_afsdb.h"
#include "ch_dns_rdata_hinfo.h"
#include "ch_dns_rdata_ipv4.h"
#include "ch_dns_rdata_ipv6.h"
#include "ch_dns_rdata_mb.h"
#include "ch_dns_rdata_md.h"
#include "ch_dns_rdata_mf.h"
#include "ch_dns_rdata_mg.h"
#include "ch_dns_rdata_minfo.h"
#include "ch_dns_rdata_mr.h"
#include "ch_dns_rdata_mx.h"
#include "ch_dns_rdata_ns.h"
#include "ch_dns_rdata_null.h"
#include "ch_dns_rdata_ptr.h"
#include "ch_dns_rdata_soa.h"
#include "ch_dns_rdata_txt.h"
#include "ch_dns_rdata_wks.h"
#include "ch_dns_rdata_caa.h"
#include "ch_dns_rdata_cert.h"
#include "ch_dns_rdata_dhcid.h"
#include "ch_dns_rdata_dlv.h"
#include "ch_dns_rdata_apl.h"
#include "ch_dns_rdata_dname.h"
#include "ch_dns_rdata_dnskey.h"
#include "ch_dns_rdata_ds.h"
#include "ch_dns_rdata_gpos.h"
#include "ch_dns_rdata_ipseckey.h"
#include "ch_dns_rdata_isdn.h"
#include "ch_dns_rdata_key.h"
#include "ch_dns_rdata_kx.h"
#include "ch_dns_rdata_loc.h"
#include "ch_dns_rdata_naptr.h"
#include "ch_dns_rdata_nsap_ptr.h"
#include "ch_dns_rdata_nsap.h"
#include "ch_dns_rdata_nsec3param.h"
#include "ch_dns_rdata_nsec3.h"
#include "ch_dns_rdata_nsec.h"
#include "ch_dns_rdata_nxt.h"
#include "ch_dns_rdata_openpgpkey.h"
#include "ch_dns_rdata_opt.h"
#include "ch_dns_rdata_px.h"
#include "ch_dns_rdata_rp.h"
#include "ch_dns_rdata_rrsig.h"
#include "ch_dns_rdata_rt.h"
#include "ch_dns_rdata_sig.h"
#include "ch_dns_rdata_smimea.h"
#include "ch_dns_rdata_spf.h"
#include "ch_dns_rdata_srv.h"
#include "ch_dns_rdata_sshfp.h"
#include "ch_dns_rdata_tkey.h"
#include "ch_dns_rdata_tlsa.h"
#include "ch_dns_rdata_tsig.h"
#include "ch_dns_rdata_x25.h"
#include "ch_dns_rdata_uri.h"

ch_dns_rdata_pool_t *ch_dns_rdata_pool_create(ch_pool_t *mp){


	ch_dns_rdata_pool_t *rdata_pool = ch_palloc(mp,sizeof(*rdata_pool));

	rdata_pool->mp = mp;

	INIT_LIST_HEAD(&rdata_pool->rdata_parsers);
	
	ch_dns_rdata_ipv4_init(rdata_pool);
	ch_dns_rdata_ipv6_init(rdata_pool);
	ch_dns_rdata_cname_init(rdata_pool);
	ch_dns_rdata_afsdb_init(rdata_pool);
	ch_dns_rdata_txt_init(rdata_pool);
	ch_dns_rdata_hinfo_init(rdata_pool);
	ch_dns_rdata_minfo_init(rdata_pool);
	ch_dns_rdata_mb_init(rdata_pool);
	ch_dns_rdata_mf_init(rdata_pool);
	ch_dns_rdata_md_init(rdata_pool);
	ch_dns_rdata_mg_init(rdata_pool);
	ch_dns_rdata_mr_init(rdata_pool);
	ch_dns_rdata_mx_init(rdata_pool);
	ch_dns_rdata_ns_init(rdata_pool);
	ch_dns_rdata_null_init(rdata_pool);
	ch_dns_rdata_ptr_init(rdata_pool);
	ch_dns_rdata_soa_init(rdata_pool);
	ch_dns_rdata_wks_init(rdata_pool);
	ch_dns_rdata_caa_init(rdata_pool);
	ch_dns_rdata_cert_init(rdata_pool);
	ch_dns_rdata_dhcid_init(rdata_pool);
	ch_dns_rdata_dlv_init(rdata_pool);
	ch_dns_rdata_apl_init(rdata_pool);
	ch_dns_rdata_dname_init(rdata_pool);
	ch_dns_rdata_dnskey_init(rdata_pool);
	ch_dns_rdata_ds_init(rdata_pool);
	ch_dns_rdata_gpos_init(rdata_pool);
	ch_dns_rdata_ipseckey_init(rdata_pool);
	ch_dns_rdata_isdn_init(rdata_pool);
	ch_dns_rdata_key_init(rdata_pool);
	ch_dns_rdata_kx_init(rdata_pool);
	ch_dns_rdata_loc_init(rdata_pool);
	ch_dns_rdata_naptr_init(rdata_pool);
	ch_dns_rdata_nsap_ptr_init(rdata_pool);
	ch_dns_rdata_nsap_init(rdata_pool);
	ch_dns_rdata_nsec3param_init(rdata_pool);
	ch_dns_rdata_nsec3_init(rdata_pool);
	ch_dns_rdata_nsec_init(rdata_pool);
	ch_dns_rdata_nxt_init(rdata_pool);
	ch_dns_rdata_openpgpkey_init(rdata_pool);
	ch_dns_rdata_opt_init(rdata_pool);
	ch_dns_rdata_px_init(rdata_pool);
	ch_dns_rdata_rp_init(rdata_pool);
	ch_dns_rdata_rrsig_init(rdata_pool);
	ch_dns_rdata_rt_init(rdata_pool);
	ch_dns_rdata_sig_init(rdata_pool);
	ch_dns_rdata_smimea_init(rdata_pool);
	ch_dns_rdata_spf_init(rdata_pool);
	ch_dns_rdata_srv_init(rdata_pool);
	ch_dns_rdata_sshfp_init(rdata_pool);
	ch_dns_rdata_tkey_init(rdata_pool);
	ch_dns_rdata_tlsa_init(rdata_pool);
	ch_dns_rdata_tsig_init(rdata_pool);
	ch_dns_rdata_x25_init(rdata_pool);
	ch_dns_rdata_uri_init(rdata_pool);

	return rdata_pool;
}

void ch_dns_rdata_parser_register(ch_dns_rdata_pool_t *rdata_pool,ch_dns_rdata_parser_t *parser){


	list_add_tail(&parser->node,&rdata_pool->rdata_parsers);

}

ch_dns_rdata_parser_t * ch_dns_rdata_parser_find(ch_dns_rdata_pool_t *rdata_pool,uint16_t cls ch_unused,uint16_t type){


	ch_dns_rdata_parser_t *rdata_parser = NULL;

	list_for_each_entry(rdata_parser,&rdata_pool->rdata_parsers,node){
	

		if(rdata_parser->type == type)
			return rdata_parser;
	}


	return NULL;
}

ch_dns_rdata_t * ch_dns_rdata_parse(ch_pool_t *mp,ch_dns_rdata_pool_t *rdata_pool,ch_dns_data_input_t *din){

	uint16_t dclass,type;

	ch_dns_name_t tmp_name;

	ch_dns_rdata_parser_t *dns_parser = NULL;

	ch_dns_rdata_t *rdata = NULL;

	if(ch_dns_data_input_rdlen(din)<10){
	
		ch_log(CH_LOG_ERR,"Invalid DNS Rdata!");
		return NULL;
	}


	if(ch_dns_name_parse(mp,din,&tmp_name))
	{
		ch_log(CH_LOG_ERR,"Parse rdata.name failed!");
		return NULL;
	}

	type = ch_dns_data_input_uint16_read(din);
	dclass = ch_dns_data_input_uint16_read(din);

	dns_parser = ch_dns_rdata_parser_find(rdata_pool,dclass,type);
	if(dns_parser){
	
		rdata = dns_parser->rdata_create(mp,NULL);
		if(rdata == NULL){
		
			ch_log(CH_LOG_ERR,"Create rdata instance failed!");
			return NULL;
		}

	}else{
	
		rdata = (ch_dns_rdata_t*)ch_pcalloc(mp,sizeof(*rdata));

	}
	
	ch_dns_name_clone(mp,&rdata->name,&tmp_name);

	rdata->type = type;
	rdata->dclass = dclass;

	rdata->ttl = ch_dns_data_input_uint32_read(din);
	rdata->dlen = ch_dns_data_input_uint16_read(din);

	if(rdata->dlen>ch_dns_data_input_rdlen(din)){
	
		ch_log(CH_LOG_ERR,"Invalid rdata len:%d!",(int)rdata->dlen);
		return NULL;

	}

	rdata->data = rdata->dlen>0?ch_dns_data_input_pos(din):NULL; 
	rdata->base = ch_dns_data_input_base(din);

	if(rdata->dlen>0&&rdata->data&&dns_parser){
	
		dns_parser->rdata_parse(mp,rdata,NULL);

	}else if(rdata->dlen>0&&rdata->data){

		/*copy rdata data*/
		void *addr = ch_pcalloc(mp,rdata->dlen+32);
		memcpy(addr,rdata->data,rdata->dlen);
		rdata->data = addr;
	}

	ch_dns_data_input_pos_update(din,rdata->dlen);

	return rdata;
}


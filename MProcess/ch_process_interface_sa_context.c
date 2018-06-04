/*
 *
 *      Filename: ch_process_interface_sa_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-31 14:50:16
 * Last Modified: 2018-05-11 16:53:04
 */

#include "ch_process_interface_sa_context.h"

#include "ch_log.h"
#include "ch_config.h"
#include "ch_util.h"

static void do_pint_sa_context_init(ch_process_interface_sa_context_t *pint_context){

	pint_context->use_rss = 1;
	pint_context->pool_name = "sa_process_interface_pool";
	pint_context->qprefix = "sa_process_interface_queue";
	pint_context->qnumber = 1;
	pint_context->qsize = 65536;

}


static const char *cmd_use_rss(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_process_interface_sa_context_t *context = (ch_process_interface_sa_context_t*)_dcfg;

    if(strcasecmp(p1,"on") == 0){
	
		context->use_rss = 1;
	}else {
	
		context->use_rss = 0;
	}

    return NULL;
}


static const char *cmd_process_interface_name(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){


    ch_process_interface_sa_context_t *context = (ch_process_interface_sa_context_t*)_dcfg;

    context->pool_name = p1;
    context->qprefix = p2;

    return NULL;
}

static const char *cmd_process_interface_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_process_interface_sa_context_t *context = (ch_process_interface_sa_context_t*)_dcfg;

    context->qnumber = (uint32_t)strtoul(p1,&endptr,10);
    context->qsize = (uint32_t)strtoul(p2,&endptr,10);
    
    if(context->qnumber <=0||context->qsize<=0){
        return "invalid process pool queue number and queue size config value !";
    }

    return NULL;
}


static const command_rec pint_context_sa_directives[] = {

	CH_INIT_TAKE1(
            "CHSAUSERSS",
            cmd_use_rss,
            NULL,
            0,
            "set use rss(on/off)"
            ),
    
	CH_INIT_TAKE2(
            "CHSAProcessInterfaceName",
            cmd_process_interface_name,
            NULL,
            0,
            "set sa process interface pool name and queue prefix config item"
            ),

    CH_INIT_TAKE2(
            "CHSAProcessInterfaceSize",
            cmd_process_interface_size,
            NULL,
            0,
            "set sa process interface queue number and queue size"
            ),
    
};

static inline void dump_pint_sa_context(ch_process_interface_sa_context_t *pint_context){

    fprintf(stdout,"Dump process interface sa  context-------------------------------------------\n");

	fprintf(stdout,"use rss hash?%s\n",pint_context->use_rss?"yes":"no");
    fprintf(stdout,"sa process interface pool name:%s\n",pint_context->pool_name);
    fprintf(stdout,"sa process interface queue prefix:%s\n",pint_context->qprefix);
    fprintf(stdout,"sa process interface queue number:%lu\n",(unsigned long)pint_context->qnumber);
    fprintf(stdout,"sa process interface queue size:%lu\n",(unsigned long)pint_context->qsize);


}

static int _sa_filter(ch_packet_t *pkt ch_unused,void *priv_data ch_unused){

	
	//ch_process_interface_sa_context_t *pint_context = (ch_process_interface_sa_context_t*)priv_data;


	return 0;
}

static uint32_t _sa_hash(ch_packet_t *pkt,void *priv_data){

	ch_process_interface_sa_context_t *pint_context = (ch_process_interface_sa_context_t*)priv_data;

	if(pint_context->use_rss){
	
		return pkt->mbuf->hash.rss;
	}

	return pkt->hash;
}


ch_process_interface_sa_context_t * ch_process_interface_sa_context_create(ch_pool_t *mp,const char *cfname,int is_write){

    const char * msg = NULL;

	ch_process_interface_sa_context_t *pint_context = ch_palloc(mp,sizeof(*pint_context));

	pint_context->mp = mp;

    do_pint_sa_context_init(pint_context);


    msg = ch_process_command_config(pint_context_sa_directives,(void*)pint_context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config process interface sa  context error:%s",msg);
        return NULL;
    }

	if(is_write){
	
		pint_context->pint = ch_process_interface_writer_create(mp,
			pint_context->qprefix,
			pint_context->qnumber,
			pint_context->qsize,
			_sa_filter,
			_sa_hash,
			(void*)pint_context);

	}else {
	
	
		pint_context->pint = ch_process_interface_reader_create(mp,
			pint_context->qprefix,
			pint_context->qnumber);
	}

	if(pint_context->pint == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create process interface!");
		return NULL;
	}

    dump_pint_sa_context(pint_context);

    return pint_context;

}


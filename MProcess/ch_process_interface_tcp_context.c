/*
 *
 *      Filename: ch_process_interface_tcp_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-06 11:16:43
 * Last Modified: 2018-09-25 17:16:17
 */

#include "ch_process_interface_tcp_context.h"
#include "ch_log.h"
#include "ch_config.h"
#include "ch_packet_tcp.h"
#include "ch_jhash.h" 
#include "ch_util.h"

static void do_pint_tcp_context_init(ch_process_interface_tcp_context_t *pint_context){

	pint_context->use_rss = 1;
	pint_context->pool_name = "tcp_process_interface_pool";
	pint_context->qprefix = "tcp_process_interface_queue";
	pint_context->qnumber = 1;
	pint_context->qsize = 65536;

	pint_context->smon_mmap_fname = NULL;
	pint_context->smon_mmap_fsize = 0;

	memset(pint_context->accept_ports,0,MAX_PORT_ARRAY_SIZE);
}


static const char *cmd_use_rss(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_process_interface_tcp_context_t *context = (ch_process_interface_tcp_context_t*)_dcfg;

    if(strcasecmp(p1,"on") == 0){
	
		context->use_rss = 1;
	}else {
	
		context->use_rss = 0;
	}

    return NULL;
}


static const char *cmd_process_interface_name(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){


    ch_process_interface_tcp_context_t *context = (ch_process_interface_tcp_context_t*)_dcfg;

    context->pool_name = p1;
    context->qprefix = p2;

    return NULL;
}

static const char *cmd_process_interface_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_process_interface_tcp_context_t *context = (ch_process_interface_tcp_context_t*)_dcfg;

    context->qnumber = (uint32_t)strtoul(p1,&endptr,10);
    context->qsize = (uint32_t)strtoul(p2,&endptr,10);
    
    if(context->qnumber <=0||context->qsize<=0){
        return "invalid process pool queue number and queue size config value !";
    }

    return NULL;
}

static const char *cmd_smon(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_process_interface_tcp_context_t *context = (ch_process_interface_tcp_context_t*)_dcfg;

	if(p1 == NULL||strlen(p1)==0)
		return "must specify session monitor mmapFileName!";

    context->smon_mmap_fname = p1;

    context->smon_mmap_fsize = (size_t)strtoul(p2,&endptr,10);
    

    return NULL;
}

static const char * cmd_accept_ports(cmd_parms *cmd ch_unused,void *_dcfg,int argc,char *const argv[]){

    char *endptr;
	int i;
	uint16_t port;

	ch_process_interface_tcp_context_t *context = (ch_process_interface_tcp_context_t*)_dcfg;

    if(argc>MAX_PORT_ARRAY_SIZE){
    
        return "The ports is too much!!";
    }
    
	for(i = 0; i<argc;i++){
	
		port = (uint16_t)strtoul(argv[i],&endptr,10);
		if(port == 0){
		
			return "The port is zero!";
		}
		
		context->accept_ports[i] = port;
	}

    return NULL;

}


static const command_rec pint_context_tcp_directives[] = {

	CH_INIT_TAKE1(
            "CHTCPUSERSS",
            cmd_use_rss,
            NULL,
            0,
            "set use rss(on/off)"
            ),
    
	CH_INIT_TAKE2(
            "CHTCPProcessInterfaceName",
            cmd_process_interface_name,
            NULL,
            0,
            "set tcp process interface pool name and queue prefix config item"
            ),

    CH_INIT_TAKE2(
            "CHTCPProcessInterfaceSize",
            cmd_process_interface_size,
            NULL,
            0,
            "set tcp process interface queue number and queue size"
            ),
    
	CH_INIT_TAKE_ARGV(
            "CHTCPAcceptPorts",
            cmd_accept_ports,
            NULL,
            0,
            "set  tcp accept ports"
            ),

    CH_INIT_TAKE2(
            "CHTCPSmon",
            cmd_smon,
            NULL,
            0,
            "set tcp process interface session monitor mmapFileName and mmapFileSize"
            ),
};

static inline void dump_pint_tcp_context(ch_process_interface_tcp_context_t *pint_context){

	int i;
    fprintf(stdout,"Dump process interface tcp  context-------------------------------------------\n");

	fprintf(stdout,"use rss hash?%s\n",pint_context->use_rss?"yes":"no");
    fprintf(stdout,"tcp process interface pool name:%s\n",pint_context->pool_name);
    fprintf(stdout,"tcp process interface queue prefix:%s\n",pint_context->qprefix);
    fprintf(stdout,"tcp process interface queue number:%lu\n",(unsigned long)pint_context->qnumber);
    fprintf(stdout,"tcp process interface queue size:%lu\n",(unsigned long)pint_context->qsize);
    fprintf(stdout,"tcp process interface smon.mmapFIleName:%s\n",pint_context->smon_mmap_fname);
    fprintf(stdout,"tcp process interface smon.mmapFIleSize:%lu\n",pint_context->smon_mmap_fsize);

	fprintf(stdout,"accept tcp ports:\n");
	for(i = 0;i<MAX_PORT_ARRAY_SIZE;i++){

		if(pint_context->accept_ports[i] == 0)
			break;

		fprintf(stdout,"%d,",(int)pint_context->accept_ports[i]);
	}

}

static int _tcp_filter(ch_packet_t *pkt,void *priv_data){

	ch_wb_list_ip_match_value_t t1,*s_match=&t1,t2,*d_match=&t2;

	ch_packet_tcp_t tcp_pkt;
	
	ch_process_interface_tcp_context_t *pint_context = (ch_process_interface_tcp_context_t*)priv_data;

	if(pkt->pkt_type!=PKT_TYPE_TCP){
	
		return 1;
	}

	ch_packet_tcp_init_from_pkt(&tcp_pkt,pkt);

	s_match->ip = tcp_pkt.src_ip;
	s_match->port = tcp_pkt.src_port;
	d_match->ip = tcp_pkt.dst_ip;
	d_match->port = tcp_pkt.dst_port;

	if(!ch_wb_list_is_accept(pint_context->ip_white_list,pint_context->ip_black_list,(void*)s_match,(void*)d_match))
		return 1;

#if 0
	if(ch_ports_equal(pint_context->accept_ports,MAX_PORT_ARRAY_SIZE,
			tcp_pkt.src_port,tcp_pkt.dst_port))
		return 0;

	if(ch_session_monitor_item_find(&pint_context->monitor,0,0,tcp_pkt.src_port,tcp_pkt.dst_port)!=NULL)
		return 0;

#endif 

	return 0;
}

static uint32_t _tcp_hash(ch_packet_t *pkt,void *priv_data){

	ch_process_interface_tcp_context_t *pint_context = (ch_process_interface_tcp_context_t*)priv_data;

	if(pint_context->use_rss){
	
		return pkt->mbuf->hash.rss;
	}


    return pkt->hash;

}


ch_process_interface_tcp_context_t * ch_process_interface_tcp_context_create(ch_pool_t *mp,const char *cfname,int is_write){

    const char * msg = NULL;

	ch_process_interface_tcp_context_t *pint_context = ch_palloc(mp,sizeof(*pint_context));

	pint_context->mp = mp;

    do_pint_tcp_context_init(pint_context);


    msg = ch_process_command_config(pint_context_tcp_directives,(void*)pint_context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config process interface tcp  context error:%s",msg);
        return NULL;
    }

	if(is_write){
	
		pint_context->pint = ch_process_interface_writer_create(mp,
			pint_context->qprefix,
			pint_context->qnumber,
			pint_context->qsize,
			_tcp_filter,
			_tcp_hash,
			(void*)pint_context);
		
		if(ch_session_monitor_load(&pint_context->monitor,pint_context->smon_mmap_fname,pint_context->smon_mmap_fsize))
		{
		
			ch_log(CH_LOG_ERR,"Cannot load session monitor from MMapFile:%s",pint_context->smon_mmap_fname);
			return NULL;

		}

	}else {
	
	
		pint_context->pint = ch_process_interface_reader_create(mp,
			pint_context->qprefix,
			pint_context->qnumber);
	}

	if(pint_context->pint == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create process interface!");
		return NULL;
	}



    dump_pint_tcp_context(pint_context);

    return pint_context;

}


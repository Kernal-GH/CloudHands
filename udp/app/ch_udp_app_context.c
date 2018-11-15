/*
 *
 *      Filename: ch_udp_app_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 14:25:48
 * Last Modified: 2018-09-14 15:09:33
 */

#include "ch_udp_app_context.h"
#include "ch_config.h"
#include "ch_log.h"

static void _udp_app_context_init(ch_udp_app_context_t *ucontext){


	ucontext->dns_is_on = 0;
	ucontext->tftp_is_on = 0;
	ucontext->smon_is_on = 0;

	ucontext->dns_cfname = "/usr/local/dpdk/CloudHands/conf/udp/app/dns.conf";
	ucontext->tftp_cfname = "/usr/local/dpdk/CloudHands/conf/udp/app/tftp.conf";
	ucontext->smon_cfname = "/usr/local/dpdk/CloudHands/conf/udp/app/smon.conf";
}

#define _cfg_set(is_on,cfname,p1,p2) do {     \
	(is_on) = 0;							  \
	if(strcasecmp(p1,"on") == 0){			  \
		(is_on) = 1;						  \
	}										  \
	(cfname) = p2;							  \
}while(0)


static const char *cmd_dns(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_udp_app_context_t *ucontext = (ch_udp_app_context_t*)_dcfg;

	_cfg_set(ucontext->dns_is_on,ucontext->dns_cfname,p1,p2);

	return NULL;
}

static const char *cmd_tftp(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_udp_app_context_t *ucontext = (ch_udp_app_context_t*)_dcfg;

	_cfg_set(ucontext->tftp_is_on,ucontext->tftp_cfname,p1,p2);

	return NULL;
}

static const char *cmd_smon(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_udp_app_context_t *ucontext = (ch_udp_app_context_t*)_dcfg;

	_cfg_set(ucontext->smon_is_on,ucontext->smon_cfname,p1,p2);

	return NULL;
}

static const command_rec ucontext_directives[] = {

    CH_INIT_TAKE2(
            "CHUDPAPPDns",
            cmd_dns,
            NULL,
            0,
            "set udp app dns <on/off> <dns conf file path>"
            ),
    
    CH_INIT_TAKE2(
            "CHUDPAPPTFtp",
            cmd_tftp,
            NULL,
            0,
            "set udp app tftp <on/off> <tftp conf file path>"
            ),

    CH_INIT_TAKE2(
            "CHUDPAPPSmon",
            cmd_smon,
            NULL,
            0,
            "set udp app smon <on/off> <smon conf file path>"
            ),
    
};

#define _cfg_dump(prefix,is_on,cfname) do { \
	fprintf(stdout,"%s is on:%s,cfname:%s\n",prefix,is_on?"on":"off",cfname); \
}while(0)


static void _udp_app_context_dump(ch_udp_app_context_t *ucontext) {

	fprintf(stdout,"Dump UDP APP Context ----------------------------\n");
	
	_cfg_dump("dns",ucontext->dns_is_on,ucontext->dns_cfname);
	_cfg_dump("tftp",ucontext->tftp_is_on,ucontext->tftp_cfname);
	_cfg_dump("smon",ucontext->smon_is_on,ucontext->smon_cfname);

}

ch_udp_app_context_t * ch_udp_app_context_create(ch_pool_t *mp,const char *cfname){

    const char * msg = NULL;
    ch_udp_app_context_t *ucontext = (ch_udp_app_context_t*)ch_palloc(mp,sizeof(ch_udp_app_context_t));

    _udp_app_context_init(ucontext);

    msg = ch_process_command_config(ucontext_directives,(void*)ucontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config udp  app context error:%s",msg);
        return NULL;
    }


    _udp_app_context_dump(ucontext);

    return ucontext;
}


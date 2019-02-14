/*
 *
 *      Filename: ch_tcp_app_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 14:25:48
 * Last Modified: 2018-09-25 17:07:03
 */

#include "ch_tcp_app_context.h"
#include "ch_config.h"
#include "ch_log.h"

static void _tcp_app_context_init(ch_tcp_app_context_t *tcontext){


	tcontext->http_is_on = 0;
	tcontext->mail_is_on = 0;
	tcontext->ftp_is_on = 0;
	tcontext->telnet_is_on = 0;
	tcontext->smon_is_on = 0;
	tcontext->ssh_is_on = 0;


	tcontext->http_cfname = "/usr/local/dpdk/CloudHands/conf/tcp/app/http.conf";
	tcontext->mail_cfname = "/usr/local/dpdk/CloudHands/conf/tcp/app/mail.conf";
	tcontext->ftp_cfname = "/usr/local/dpdk/CloudHands/conf/tcp/app/ftp.conf";
	tcontext->telnet_cfname = "/usr/local/dpdk/CloudHands/conf/tcp/app/telnet.conf";
	tcontext->smon_cfname = "/usr/local/dpdk/CloudHands/conf/tcp/app/smon.conf";
	tcontext->smon_cfname = "/usr/local/dpdk/CloudHands/conf/tcp/app/ssh.conf";

}

#define _cfg_set(is_on,cfname,p1,p2) do {     \
	(is_on) = 0;							  \
	if(strcasecmp(p1,"on") == 0){			  \
		(is_on) = 1;						  \
	}										  \
	(cfname) = p2;							  \
}while(0)

static const char *cmd_http(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)_dcfg;

	_cfg_set(tcontext->http_is_on,tcontext->http_cfname,p1,p2);

	return NULL;
}

static const char *cmd_mail(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)_dcfg;

	_cfg_set(tcontext->mail_is_on,tcontext->mail_cfname,p1,p2);

	return NULL;
}

static const char *cmd_ftp(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)_dcfg;

	_cfg_set(tcontext->ftp_is_on,tcontext->ftp_cfname,p1,p2);

	return NULL;
}

static const char *cmd_telnet(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)_dcfg;

	_cfg_set(tcontext->telnet_is_on,tcontext->telnet_cfname,p1,p2);

	return NULL;
}

static const char *cmd_smon(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)_dcfg;

	_cfg_set(tcontext->smon_is_on,tcontext->smon_cfname,p1,p2);

	return NULL;
}

static const char *cmd_ssh(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)_dcfg;

	_cfg_set(tcontext->ssh_is_on,tcontext->ssh_cfname,p1,p2);

	return NULL;
}


static const command_rec tcontext_directives[] = {

    CH_INIT_TAKE2(
            "CHTCPAPPHttp",
            cmd_http,
            NULL,
            0,
            "set tcp app http <on/off> <http conf file path>"
            ),
    
	CH_INIT_TAKE2(
            "CHTCPAPPMail",
            cmd_mail,
            NULL,
            0,
            "set tcp app mail <on/off> <smtp conf file path>"
            ),
    
	CH_INIT_TAKE2(
            "CHTCPAPPFtp",
            cmd_ftp,
            NULL,
            0,
            "set tcp app ftp <on/off> <ftp conf file path>"
            ),
    
	CH_INIT_TAKE2(
            "CHTCPAPPTelnet",
            cmd_telnet,
            NULL,
            0,
            "set tcp app telnet <on/off> <ftp conf file path>"
            ),

    CH_INIT_TAKE2(
            "CHTCPAPPSmon",
            cmd_smon,
            NULL,
            0,
            "set tcp app smon <on/off> <smon conf file path>"
            ),
    
    CH_INIT_TAKE2(
            "CHTCPAPPSSH",
            cmd_ssh,
            NULL,
            0,
            "set tcp app ssh <on/off> <ssh conf file path>"
            ),
};

#define _cfg_dump(prefix,is_on,cfname) do { \
	fprintf(stdout,"%s is on:%s,cfname:%s\n",prefix,is_on?"on":"off",cfname); \
}while(0)


static void _tcp_app_context_dump(ch_tcp_app_context_t *tcontext) {

	fprintf(stdout,"Dump TCP APP Context ----------------------------\n");
	
	_cfg_dump("http",tcontext->http_is_on,tcontext->http_cfname);
	_cfg_dump("mail",tcontext->mail_is_on,tcontext->mail_cfname);
	_cfg_dump("ftp",tcontext->ftp_is_on,tcontext->ftp_cfname);
	_cfg_dump("telnet",tcontext->telnet_is_on,tcontext->telnet_cfname);
	_cfg_dump("smon",tcontext->smon_is_on,tcontext->smon_cfname);
	_cfg_dump("ssh",tcontext->ssh_is_on,tcontext->ssh_cfname);

}

ch_tcp_app_context_t * ch_tcp_app_context_create(ch_pool_t *mp,const char *cfname){

    const char * msg = NULL;
    ch_tcp_app_context_t *tcontext = (ch_tcp_app_context_t*)ch_palloc(mp,sizeof(ch_tcp_app_context_t));

    _tcp_app_context_init(tcontext);

    msg = ch_process_command_config(tcontext_directives,(void*)tcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp  app context error:%s",msg);
        return NULL;
    }


    _tcp_app_context_dump(tcontext);

    return tcontext;
}


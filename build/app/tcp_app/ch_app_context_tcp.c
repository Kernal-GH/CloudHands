/*
 * =====================================================================================
 *
 *       Filename:  ch_app_context_tcp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月1日 11时00分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_log.h"
#include "ch_config.h"
#include "ch_app_context_tcp.h"
#include "ch_http.h"
#include "ch_smtp.h"
#include "ch_imap.h"
#include "ch_pop3.h"
#include "ch_telnet.h"
#include "ch_ftp.h"
#include "ch_debug.h"

static inline void app_context_tcp_init(ch_app_context_tcp_t *app_tcp_context){

    app_tcp_context->http_is_on = 1;
    app_tcp_context->smtp_is_on = 1;
    app_tcp_context->pop3_is_on = 1;
    app_tcp_context->imap_is_on = 1;
    app_tcp_context->telnet_is_on = 1;
    app_tcp_context->ftp_is_on = 1;
	app_tcp_context->debug_is_on = 0;

	memset(app_tcp_context->http_ports,0,PORT_MAX_SIZE);
	memset(app_tcp_context->smtp_ports,0,PORT_MAX_SIZE);
	memset(app_tcp_context->pop3_ports,0,PORT_MAX_SIZE);
	memset(app_tcp_context->imap_ports,0,PORT_MAX_SIZE);
	memset(app_tcp_context->telnet_ports,0,PORT_MAX_SIZE);
	memset(app_tcp_context->ftp_ports,0,PORT_MAX_SIZE);
	memset(app_tcp_context->debug_ports,0,PORT_MAX_SIZE);
}

static const char *cmd_http_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->http_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->http_is_on = 0;

    return NULL;
}

static const char *cmd_smtp_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->smtp_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->smtp_is_on = 0;

    return NULL;
}

static const char *cmd_pop3_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->pop3_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->pop3_is_on = 0;

    return NULL;
}

static const char *cmd_imap_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->imap_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->imap_is_on = 0;

    return NULL;
}

static const char *cmd_telnet_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->telnet_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->telnet_is_on = 0;

    return NULL;
}

static const char *cmd_ftp_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->ftp_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->ftp_is_on = 0;

    return NULL;
}

static const char *cmd_debug_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

    app_tcp_context->debug_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_tcp_context->debug_is_on = 0;

    return NULL;
}

static const char * _config_ports(uint16_t *ports,int argc,char *const argv[]){

    char *endptr;
	int i;
	uint16_t port;

    if(argc>PORT_MAX_SIZE){
    
        return "The ports is too much!!";
    }
    
	for(i = 0; i<argc;i++){
	
		port = (uint16_t)strtoul(argv[i],&endptr,10);
		if(port == 0){
		
			return "The port is zero!";
		}
		
		ports[i] = port;
	}

    return NULL;

}

static const char *cmd_http_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->http_ports,argc,argv);
}

static const char *cmd_smtp_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->smtp_ports,argc,argv);
}

static const char *cmd_pop3_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->pop3_ports,argc,argv);
}

static const char *cmd_imap_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->imap_ports,argc,argv);
}

static const char *cmd_telnet_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->telnet_ports,argc,argv);
}

static const char *cmd_ftp_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->ftp_ports,argc,argv);
}

static const char *cmd_debug_ports(cmd_parms * cmd, void *_dcfg, int argc,char *const argv[]){

    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)_dcfg;

	return _config_ports(app_tcp_context->debug_ports,argc,argv);
}

static const command_rec app_context_tcp_directives[] = {

    CH_INIT_TAKE1(
            "CHAPPTCPHttpIsON",
            cmd_http_is_on,
            NULL,
            0,
            "set http on/off"
            ),
    
    CH_INIT_TAKE1(
            "CHAPPTCPSMTPIsON",
            cmd_smtp_is_on,
            NULL,
            0,
            "set smtp on/off"
            ),

    CH_INIT_TAKE1(
            "CHAPPTCPPOP3IsON",
            cmd_pop3_is_on,
            NULL,
            0,
            "set pop3 on/off"
            ),

    CH_INIT_TAKE1(
            "CHAPPTCPIMAPIsON",
            cmd_imap_is_on,
            NULL,
            0,
            "set imap on/off"
            ),

    CH_INIT_TAKE1(
            "CHAPPTCPTELNETIsON",
            cmd_telnet_is_on,
            NULL,
            0,
            "set telnet on/off"
            ),

    CH_INIT_TAKE1(
            "CHAPPTCPFTPIsON",
            cmd_ftp_is_on,
            NULL,
            0,
            "set ftp on/off"
            ),
    
	CH_INIT_TAKE1(
            "CHAPPTCPDebugIsON",
            cmd_debug_is_on,
            NULL,
            0,
            "set debug on/off"
            ),
    
	CH_INIT_TAKE_ARGV(
            "CHAPPTCPHttpPorts",
            cmd_http_ports,
            NULL,
            0,
            "set  http ports"
            ),

	CH_INIT_TAKE_ARGV(
            "CHAPPTCPSmtpPorts",
            cmd_smtp_ports,
            NULL,
            0,
            "set  smtp ports"
            ),

	CH_INIT_TAKE_ARGV(
            "CHAPPTCPPop3Ports",
            cmd_pop3_ports,
            NULL,
            0,
            "set  pop3 ports"
            ),

	CH_INIT_TAKE_ARGV(
            "CHAPPTCPImapPorts",
            cmd_imap_ports,
            NULL,
            0,
            "set imap ports"
            ),

	CH_INIT_TAKE_ARGV(
            "CHAPPTCPTelnetPorts",
            cmd_telnet_ports,
            NULL,
            0,
            "set telnet ports"
            ),

	CH_INIT_TAKE_ARGV(
            "CHAPPTCPFtpPorts",
            cmd_ftp_ports,
            NULL,
            0,
            "set ftp ports"
            ),

	CH_INIT_TAKE_ARGV(
            "CHAPPTCPDebugPorts",
            cmd_debug_ports,
            NULL,
            0,
            "set  debug ports"
            )
};

static inline void tcp_apps_register(ch_app_context_tcp_t *app_tcp_context){


    if(app_tcp_context->http_is_on)
        ch_http_init((ch_app_context_t*)app_tcp_context);

    if(app_tcp_context->smtp_is_on)
        ch_smtp_init((ch_app_context_t*)app_tcp_context);

    if(app_tcp_context->imap_is_on)
        ch_imap_init((ch_app_context_t*)app_tcp_context);

    if(app_tcp_context->pop3_is_on)
        ch_pop3_init((ch_app_context_t*)app_tcp_context);

	if (app_tcp_context->telnet_is_on)
		ch_telnet_init((ch_app_context_t *) app_tcp_context);

	if (app_tcp_context->ftp_is_on)
		ch_ftp_init((ch_app_context_t *) app_tcp_context);
    
	if(app_tcp_context->debug_is_on)
        ch_debug_init((ch_app_context_t*)app_tcp_context);
}

static inline void _dump_ports(uint16_t* ports,const char *pre){

	int i;
	uint16_t port;

	fprintf(stdout,"%s ports:",pre);

	for(i = 0; i<PORT_MAX_SIZE;i++){
	
		port = ports[i];

		if(port == 0)
			break;
		fprintf(stdout,"%ld,",(unsigned long)port);
	}

	fprintf(stdout,"\n\n");
}

static inline void dump_tcp_app_context(ch_app_context_tcp_t *app_tcp_context){

    fprintf(stdout,"Dump Tcp app context-----------------------------------------\n");
    fprintf(stdout,"Http is on:%d\n",app_tcp_context->http_is_on);
	_dump_ports(app_tcp_context->http_ports,"http");

	fprintf(stdout,"smtp is on:%d\n",app_tcp_context->smtp_is_on);
	_dump_ports(app_tcp_context->smtp_ports,"smtp");

    fprintf(stdout,"pop3 is on:%d\n",app_tcp_context->pop3_is_on);
	_dump_ports(app_tcp_context->pop3_ports,"pop3");
    
	fprintf(stdout,"imap is on:%d\n",app_tcp_context->imap_is_on);
	_dump_ports(app_tcp_context->imap_ports,"imap");

	fprintf(stdout,"telnet is on:%d\n",app_tcp_context->telnet_is_on);
	_dump_ports(app_tcp_context->telnet_ports,"telnet");

	fprintf(stdout,"ftp is on:%d\n",app_tcp_context->ftp_is_on);
	_dump_ports(app_tcp_context->ftp_ports,"ftp");

	fprintf(stdout,"debug is on:%d\n",app_tcp_context->debug_is_on);
	_dump_ports(app_tcp_context->debug_ports,"debug");

}

ch_app_context_t * ch_app_context_tcp_create(ch_context_t *context){

    const char * msg = NULL;
    ch_app_context_tcp_t *app_tcp_context = (ch_app_context_tcp_t*)apr_palloc(context->mp,sizeof(ch_app_context_tcp_t));

    app_context_tcp_init(app_tcp_context);

    msg = ch_process_command_config(app_context_tcp_directives,(void*)app_tcp_context,context->mp,context->mp,
            context->app_tcp_cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config app tcp context error:%s",msg);
        return NULL;
    }

    ch_app_context_init((ch_app_context_t*)app_tcp_context,context);

    tcp_apps_register(app_tcp_context);

    dump_tcp_app_context(app_tcp_context);

    return (ch_app_context_t*)app_tcp_context;

}

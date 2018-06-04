/*
 *
 *      Filename: ch_proto_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 00:16:46
 * Last Modified: 2017-01-19 14:51:03
 */

#include <apr_strings.h>
#include "ch_log.h"
#include "ch_config.h"
#include "ch_proto_context.h"
#include "ch_proto_pool.h"
#include "ch_proto_work.h"

static inline void _proto_context_init(ch_proto_context_t *pcontext,const char *mmap_file){

    pcontext->mmap_file = apr_psprintf(pcontext->mp,"%s",mmap_file);
    pcontext->log_level = CH_LOG_INFO;
    pcontext->log_file = "/tmp/proto_main.log";
    pcontext->mmfmt = NULL;
	pcontext->trans_proto_type = TPT_MAX;
	
	pcontext->http_proto_cfile = NULL;
	pcontext->mail_proto_cfile = NULL;
	pcontext->debug_proto_cfile = NULL;
	pcontext->telnet_proto_cfile = NULL;
	pcontext->ftp_proto_cfile = NULL;

	pcontext->dns_proto_cfile = NULL;

	pcontext->fmt_type = FMT_TYPE_MSGPACK;
	pcontext->work_sleep_time = 10000;/* 10ms */
}

static const char *cmd_log(cmd_parms *cmd, void *_dcfg, const char *p1,const char *p2){

	/* unused */
	cmd = cmd;

    ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

    pcontext->log_file = p1;

    if(strcasecmp(p2,"info") == 0){
        pcontext->log_level = CH_LOG_INFO;
    }else if(strcasecmp(p2,"debug") == 0){
        pcontext->log_level = CH_LOG_DEBUG;
    }else if(strcasecmp(p2,"notice") == 0){
        pcontext->log_level = CH_LOG_NOTICE;
    }else if(strcasecmp(p2,"warn") == 0){
        pcontext->log_level = CH_LOG_WARN;
    }else if(strcasecmp(p2,"error") == 0){
        pcontext->log_level = CH_LOG_ERR;
    }else if(strcasecmp(p2,"crit") == 0){
        pcontext->log_level = CH_LOG_CRIT;
    }else if(strcasecmp(p2,"alert") == 0){
        pcontext->log_level = CH_LOG_ALERT;
    }else if(strcasecmp(p2,"emerg") == 0){
        pcontext->log_level = CH_LOG_EMERG;
    }else {

        return "unknown log level name!";
    }

    return NULL;
}

static const char *cmd_http_proto_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->http_proto_cfile = p1;

	return NULL;
}

static const char *cmd_mail_proto_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->mail_proto_cfile = p1;
	return NULL;
}

static const char *cmd_dns_proto_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->dns_proto_cfile = p1;
	return NULL;
}

static const char *cmd_debug_proto_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->debug_proto_cfile = p1;
	return NULL;
}

static const char *cmd_telnet_proto_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->telnet_proto_cfile = p1;
	return NULL;
}

static const char *cmd_ftp_proto_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->ftp_proto_cfile = p1;
	return NULL;
}

static const char *cmd_trans_proto_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	if(strcasecmp(p1,TPT_TCP_N) == 0){
	
		pcontext->trans_proto_type = TPT_TCP;
	}else if(strcasecmp(p1,TPT_UDP_N) == 0){
	
		pcontext->trans_proto_type = TPT_UDP;
	}else{
		
		return "Implement transport proto!";
	}

	return NULL;
}

static const char *cmd_output_format_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	if(strcasecmp(p1,FMT_MSGPACK) == 0){
	
		pcontext->fmt_type = FMT_TYPE_MSGPACK;
	}else if(strcasecmp(p1,FMT_JSON) == 0){

		pcontext->fmt_type = FMT_TYPE_JSON;
	}else if(strcasecmp(p1,FMT_XML) == 0){
	
		pcontext->fmt_type = FMT_TYPE_XML;
	}else if(strcasecmp(p1,FMT_BIN) == 0){
	
		pcontext->fmt_type = FMT_TYPE_BIN;
	}else if(strcasecmp(p1,FMT_TXT) == 0){
	
		pcontext->fmt_type = FMT_TYPE_TXT;
	}else{
	
		return "Unkown format type!";
	}

	return NULL;
}


static const char *cmd_work_sleep_time(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	char *end;

	ch_proto_context_t *pcontext = (ch_proto_context_t*)_dcfg;

	pcontext->work_sleep_time = (uint64_t)strtoul(p1,&end,10);

	return NULL;
}

static const command_rec proto_context_directives[] = {

    CH_INIT_TAKE2(
            "PLog",
            cmd_log,
            NULL,
            0,
            "set log file and level"
            ),

    CH_INIT_TAKE1(
            "PHttpCFile",
            cmd_http_proto_config_file,
            NULL,
            0,
            "set the http proto config file full path name"
            ),

    CH_INIT_TAKE1(
            "PMailCFile",
            cmd_mail_proto_config_file,
            NULL,
            0,
            "set the mail proto config file full path name"
            ),
    
	CH_INIT_TAKE1(
            "PDNSCFile",
            cmd_dns_proto_config_file,
            NULL,
            0,
            "set the dns proto config file full path name"
            ),
	
	CH_INIT_TAKE1(
            "PDebugCFile",
            cmd_debug_proto_config_file,
            NULL,
            0,
            "set the debug proto config file full path name"
            ),

	CH_INIT_TAKE1(
            "PTelnetCFile",
            cmd_telnet_proto_config_file,
            NULL,
            0,
            "set the telnet proto config file full path name"
            ),

	CH_INIT_TAKE1(
            "PFtpCFile",
            cmd_ftp_proto_config_file,
            NULL,
            0,
            "set the ftp proto config file full path name"
            ),

    CH_INIT_TAKE1(
            "POutputFormatType",
            cmd_output_format_type,
            NULL,
            0,
            "set the parse result output format type(msgpack,json,xml,bin,txt)"
            ),
    
	CH_INIT_TAKE1(
            "PTransProtoType",
            cmd_trans_proto_type,
            NULL,
            0,
            "set the transport protocol type(tcp,udp...)"
            ),
    
	CH_INIT_TAKE1(
            "PWorkSleepTime",
            cmd_work_sleep_time,
            NULL,
            0,
            "set work sleep time"
            ),
};


ch_proto_context_t * ch_proto_context_create(apr_pool_t *mp,
	const char *conf_path,const char *mmap_file){

    const char *msg = NULL;
    ch_proto_context_t *pcontext = apr_palloc(mp,sizeof(ch_proto_context_t));

	pcontext->pid = getpid();

    pcontext->mp = mp;

    _proto_context_init(pcontext,mmap_file);

    msg = ch_process_command_config(proto_context_directives,(void*)pcontext,mp,mp,conf_path);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config proto context error:%s",msg);
        return NULL;
    }

    return pcontext;
}

int ch_proto_context_start(ch_proto_context_t *pcontext){

	/*create  mmap file format for reading*/
	pcontext->mmfmt = ch_mmap_file_format_create(pcontext->mp,pcontext->mmap_file,0,0,0);

	if(pcontext->mmfmt == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot open mmap file:%s for reading!",pcontext->mmap_file);
		return -1;
	}

	/* create proto pool */
	pcontext->ppool = ch_proto_pool_create(pcontext);
	if(pcontext->ppool == NULL){
	
		ch_log(CH_LOG_ERR,"Create protocol pool failed!");
		return -1;
	}
	/*ok! */

	/*start proto work*/

	ch_proto_work_run(pcontext);

	return 0;
}

void ch_proto_context_exit(ch_proto_context_t *pcontext){


	ch_proto_work_exit(pcontext);

}

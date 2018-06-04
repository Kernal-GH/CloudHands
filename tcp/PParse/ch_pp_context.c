/*
 *
 *      Filename: ch_pp_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 00:16:46
 * Last Modified: 2018-05-17 19:43:33
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_pp_context.h"
#include "ch_entry_pool.h"
#include "ch_fpath.h"

static inline void _pp_context_init(ch_pp_context_t *pcontext){

    pcontext->log_level = CH_LOG_INFO;
    pcontext->log_file = "/tmp/PPMain.log";
	
	pcontext->fstore_dir = NULL;
	pcontext->fstore_buf_size = 64*1024;
	pcontext->create_dir_type = BY_HOUR;

	pcontext->shm_fname = NULL;
	pcontext->shm_fsize =  4*1024*1024*1024UL;
	pcontext->shm_fentry_size = 16*1024*1024UL;

	pcontext->http_pp_cfile = NULL;
	pcontext->mail_pp_cfile = NULL;
	pcontext->debug_pp_cfile = NULL;
	pcontext->telnet_pp_cfile = NULL;
	pcontext->ftp_pp_cfile = NULL;

	pcontext->fmt_type = FMT_TYPE_MSGPACK;
	pcontext->work_sleep_time = 10000;/* 10ms */
	
	pcontext->tbl_pool_type = FROM_OBJ_POOL;
    pcontext->tbl_session_limits = 10000000; 
    pcontext->tbl_session_tbl_size = 65536;
    pcontext->tbl_session_cache_limits = 1024;
    pcontext->tbl_session_timeout = 3*60;
}

static const char *cmd_log(cmd_parms *cmd, void *_dcfg, const char *p1,const char *p2){

	/* unused */
	cmd = cmd;

    ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

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

static const char *cmd_shm_file_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

    pcontext->shm_fsize = (uint64_t)strtoul(p1,&endptr,10);
    
    if(pcontext->shm_fsize <=0){
        return "invalid share memory size config value";
    }

    return NULL;
}

static const char *cmd_shm_file_entry_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

    pcontext->shm_fentry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(pcontext->shm_fentry_size <=0){
        return "invalid entry size config value";
    }

    return NULL;
}

static const char *cmd_shm_file_name(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

    pcontext->shm_fname = p1;
    return NULL;
}

static const char *cmd_http_pp_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->http_pp_cfile = p1;

	return NULL;
}

static const char *cmd_mail_pp_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->mail_pp_cfile = p1;
	return NULL;
}

static const char *cmd_debug_pp_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->debug_pp_cfile = p1;
	return NULL;
}

static const char *cmd_telnet_pp_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->telnet_pp_cfile = p1;
	return NULL;
}

static const char *cmd_ftp_pp_config_file(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->ftp_pp_cfile = p1;
	return NULL;
}

static const char *cmd_output_format_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

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

	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->work_sleep_time = (uint64_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_tbl_pool_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_pp_context_t *context = (ch_pp_context_t*)_dcfg;

	if(strcasecmp(p1,"dpdk_pool")==0){

		context->tbl_pool_type = FROM_DPDK_POOL;
	}else{
	
		context->tbl_pool_type = FROM_OBJ_POOL;
	}

    return NULL;
}

static const char *cmd_tbl_session_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pp_context_t *context = (ch_pp_context_t*)_dcfg;

    context->tbl_session_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tbl_session_limits <=0){
        return "invalid tbl session number limits config value";
    }

    return NULL;
}

static const char *cmd_tbl_session_tbl_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pp_context_t *context = (ch_pp_context_t*)_dcfg;

    context->tbl_session_tbl_size = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tbl_session_tbl_size <=0){
        return "invalid tbl session table size config value";
    }

    return NULL;
}

static const char *cmd_tbl_session_cache_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pp_context_t *context = (ch_pp_context_t*)_dcfg;

    context->tbl_session_cache_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tbl_session_cache_limits <=0){
        return "invalid tbl session table cache limits config value";
    }

    return NULL;
}

static const char *cmd_tbl_session_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pp_context_t *context = (ch_pp_context_t*)_dcfg;

    context->tbl_session_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->tbl_session_timeout <=0){
        return "invalid tbl  session timeout config value";
    }

    return NULL;
}

static const char *cmd_header_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc ;
	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid proto header dir create type config item!";

	}

	pcontext->create_dir_type = rc;

	return NULL;
}

static const char *cmd_file_store_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	char *end;

	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->fstore_buf_size = (size_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_header_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


	ch_pp_context_t *pcontext = (ch_pp_context_t*)_dcfg;

	pcontext->fstore_dir = p1;

	return NULL;
}

static const command_rec pp_context_directives[] = {

    CH_INIT_TAKE2(
            "PLog",
            cmd_log,
            NULL,
            0,
            "set log file and level"
            ),
    
	CH_INIT_TAKE1(
            "PFStoreDir",
            cmd_header_dir,
            NULL,
            0,
            "set the store path of  header"
            ),
    
	CH_INIT_TAKE1(
            "PFStoreDirCreateType",
            cmd_header_dir_create_type,
            NULL,
            0,
            "set the type created of header dir by time"
            ),
    
	CH_INIT_TAKE1(
            "PFStoreSize",
            cmd_file_store_size,
            NULL,
            0,
            "set the size of http file store"
            ),
	
	CH_INIT_TAKE1(
            "PSHMFName",
            cmd_shm_file_name,
            NULL,
            0,
            "set shm file name"
            ),
    
	CH_INIT_TAKE1(
            "PSHMFSize",
            cmd_shm_file_size,
            NULL,
            0,
            "set the shm file size"
            ),
    
	CH_INIT_TAKE1(
            "PSHMFEntrySize",
            cmd_shm_file_entry_size,
            NULL,
            0,
            "set the shm file entry size"
            ),

    CH_INIT_TAKE1(
            "PHttpCFile",
            cmd_http_pp_config_file,
            NULL,
            0,
            "set the http pp config file full path name"
            ),

    CH_INIT_TAKE1(
            "PMailCFile",
            cmd_mail_pp_config_file,
            NULL,
            0,
            "set the mail pp config file full path name"
            ),
    
	CH_INIT_TAKE1(
            "PDebugCFile",
            cmd_debug_pp_config_file,
            NULL,
            0,
            "set the debug pp config file full path name"
            ),

	CH_INIT_TAKE1(
            "PTelnetCFile",
            cmd_telnet_pp_config_file,
            NULL,
            0,
            "set the telnet pp config file full path name"
            ),

	CH_INIT_TAKE1(
            "PFtpCFile",
            cmd_ftp_pp_config_file,
            NULL,
            0,
            "set the ftp pp config file full path name"
            ),

    CH_INIT_TAKE1(
            "POutputFormatType",
            cmd_output_format_type,
            NULL,
            0,
            "set the parse result output format type(msgpack,json,xml,bin,txt)"
            ),
    
	CH_INIT_TAKE1(
            "PWorkSleepTime",
            cmd_work_sleep_time,
            NULL,
            0,
            "set work sleep time"
            ),
	
	CH_INIT_TAKE1(
            "PTBLPoolType",
            cmd_tbl_pool_type,
            NULL,
            0,
            "set tbl session  pool type(dpdk_pool or obj_pool) config item"
            ),

    CH_INIT_TAKE1(
            "PTBLSessionLimits",
            cmd_tbl_session_limits,
            NULL,
            0,
            "set tbl session limits config item"
            ),

    CH_INIT_TAKE1(
            "PTBLSessionTblSize",
            cmd_tbl_session_tbl_size,
            NULL,
            0,
            "set tbl session table size config item"
            ),

    CH_INIT_TAKE1(
            "PTBLSessionTblCacheLimits",
            cmd_tbl_session_cache_limits,
            NULL,
            0,
            "set tbl  session table cache limits config item"
            ),

    CH_INIT_TAKE1(
            "PTBLSessionTimeout",
            cmd_tbl_session_timeout,
            NULL,
            0,
            "set tbl session timeout config item"
            ),
};


ch_pp_context_t * ch_pp_context_create(ch_pool_t *mp,const char *cfpath){

    const char *msg = NULL;
    ch_pp_context_t *pcontext = ch_palloc(mp,sizeof(ch_pp_context_t));

    pcontext->mp = mp;

    _pp_context_init(pcontext);

    msg = ch_process_command_config(pp_context_directives,(void*)pcontext,mp,mp,cfpath);

    /*config failed*/
    if(msg!=NULL){

        return NULL;
    }

    return pcontext;
}

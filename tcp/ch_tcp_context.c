/*
 *
 *      Filename: ch_tcp_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-30 12:19:59
 * Last Modified: 2018-09-27 11:12:04
 */

#include "ch_config.h"
#include "ch_tcp_context.h"
#include "ch_entry_pool.h"
#include "ch_log.h"

static void do_tcp_context_init(ch_tcp_context_t *tcp_context){

	tcp_context->log_name = "/tmp/TCPMain.log";
	tcp_context->log_level = CH_LOG_NOTICE;

	tcp_context->entry_size = 16*1024*1024UL;
	tcp_context->shm_size = 4*1024*1024*1024UL;
    tcp_context->mmap_file_dir = NULL;
	tcp_context->shm_flush_timeout = 3*60;

	tcp_context->key = NULL;
	tcp_context->proj_id = 0;
	tcp_context->tasks_n = 1;
    tcp_context->tcp_session_request_pool_type = FROM_OBJ_POOL;
	tcp_context->tcp_session_pool_type = FROM_OBJ_POOL;
	tcp_context->tcp_session_request_limits = 10000000;
    tcp_context->tcp_session_limits = 10000000; 
    
    tcp_context->tcp_session_tbl_size = 65536;
    tcp_context->tcp_session_request_tbl_size = 65536;
    tcp_context->tcp_session_cache_limits = 1024;
    tcp_context->tcp_session_request_cache_limits = 1024;

    tcp_context->tcp_session_request_timeout = 60;
    tcp_context->tcp_session_timeout = 5*60;

	tcp_context->mm_max_cache_size = 128*1024*1024;
	tcp_context->mm_timeout = 60;
	tcp_context->app_pool_cfname = NULL;

	tcp_context->pint_cfname = NULL;

}

static const char *cmd_tcp_log(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_tcp_context_t  *tcp_context = (ch_tcp_context_t *)_dcfg;

    tcp_context->log_name = p1;

    if(strcasecmp(p2,"info") == 0){
        tcp_context->log_level = CH_LOG_INFO;
    }else if(strcasecmp(p2,"debug") == 0){
        tcp_context->log_level = CH_LOG_DEBUG;
    }else if(strcasecmp(p2,"notice") == 0){
        tcp_context->log_level = CH_LOG_NOTICE;
    }else if(strcasecmp(p2,"warn") == 0){
        tcp_context->log_level = CH_LOG_WARN;
    }else if(strcasecmp(p2,"error") == 0){
        tcp_context->log_level = CH_LOG_ERR;
    }else if(strcasecmp(p2,"crit") == 0){
        tcp_context->log_level = CH_LOG_CRIT;
    }else if(strcasecmp(p2,"alert") == 0){
        tcp_context->log_level = CH_LOG_ALERT;
    }else if(strcasecmp(p2,"emerg") == 0){
        tcp_context->log_level = CH_LOG_EMERG;
    }else {

        return "unknown log level name!";
    }

    return NULL;
}

static const char *cmd_mmap_file_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->mmap_file_dir = p1;
    return NULL;
}

static const char *cmd_shm_key(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->key = p1;
    return NULL;
}

static const char *cmd_tasks_n(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tasks_n = (uint16_t)strtoul(p1,&endptr,10);
    
    if(context->tasks_n <=0){
        return "invalid tcp session tasks number config value";
    }

    return NULL;
}

static const char *cmd_session_pool_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

	if(strcasecmp(p1,"dpdk_pool")==0){

		context->tcp_session_pool_type = FROM_DPDK_POOL;
	}else{
	
		context->tcp_session_pool_type = FROM_OBJ_POOL;
	}

    return NULL;
}

static const char *cmd_session_request_pool_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

	if(strcasecmp(p1,"dpdk_pool")==0){

		context->tcp_session_request_pool_type = FROM_DPDK_POOL;
	}else{
	
		context->tcp_session_request_pool_type = FROM_OBJ_POOL;
	}

    return NULL;
}


static const char *cmd_tcp_session_request_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_request_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_request_limits <=0){
        return "invalid tcp session requests number limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_limits <=0){
        return "invalid tcp session number limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_tbl_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_tbl_size = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_tbl_size <=0){
        return "invalid tcp session table size config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_request_tbl_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_request_tbl_size = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_request_tbl_size <=0){
        return "invalid tcp session requests table size config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_cache_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_cache_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_cache_limits <=0){
        return "invalid tcp session table cache limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_request_cache_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_request_cache_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_request_cache_limits <=0){
        return "invalid session request table cache limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_request_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_request_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_request_timeout <=0){
        return "invalid tcp session request timeout config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->tcp_session_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_timeout <=0){
        return "invalid tcp  session timeout config value";
    }

    return NULL;
}

static const char *cmd_entry_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->entry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->entry_size <=0){
        return "invalid entry size config value";
    }

    return NULL;
}

static const char *cmd_shm_proj_id(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->proj_id = (int)strtol(p1,&endptr,10);
    

    return NULL;
}

static const char *cmd_shm_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->shm_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->shm_size <=0){
        return "invalid share memory size config value";
    }

    return NULL;
}

static const char *cmd_shm_flush_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->shm_flush_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->shm_flush_timeout <=0){
        return "invalid share memory flush timeout config value";
    }

    return NULL;
}

static const char *cmd_process_interface_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->pint_cfname = p1;

    return NULL;
}


static const char *cmd_mm(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;

    context->mm_max_cache_size = (size_t)strtoul(p1,&endptr,10);
    context->mm_timeout = (uint64_t)strtoul(p2,&endptr,10);
    
    if(context->mm_timeout <=0||context->mm_max_cache_size<=0){
        return "invalid memory max cache size and timeout config value !";
    }

    return NULL;
}

static const char *cmd_app_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_tcp_context_t *context = (ch_tcp_context_t*)_dcfg;
	context->app_pool_cfname = p1;

    return NULL;
}


static const command_rec tcp_context_directives[] = {
    
	CH_INIT_TAKE2(
            "CHTCPLog",
            cmd_tcp_log,
            NULL,
            0,
            "set tcp log name and level"
            ),

    CH_INIT_TAKE1(
            "CHTCPMMapFileDir",
            cmd_mmap_file_dir,
            NULL,
            0,
            "set tcp mmap file dir config item"
            ),
    
	CH_INIT_TAKE1(
            "CHTCPSHMKey",
            cmd_shm_key,
            NULL,
            0,
            "set tcp share memory key config item"
            ),
	
	CH_INIT_TAKE1(
            "CHTCPSHMProjID",
            cmd_shm_proj_id,
            NULL,
            0,
            "set tcp share memory proj_Id config item"
            ),
	CH_INIT_TAKE1(
            "CHTCPSessionPoolType",
            cmd_session_pool_type,
            NULL,
            0,
            "set tcp session pool type(dpdk_pool or obj_pool) config item"
            ),
	
	CH_INIT_TAKE1(
            "CHTCPSessionRequestPoolType",
            cmd_session_request_pool_type,
            NULL,
            0,
            "set tcp session request pool type(dpdk_pool or obj_pool) config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionTasks",
            cmd_tasks_n,
            NULL,
            0,
            "set tcp session tasks number config item"
            ),
    
    CH_INIT_TAKE1(
            "CHTCPSessionRequestLimits",
            cmd_tcp_session_request_limits,
            NULL,
            0,
            "set tcp session requests limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionLimits",
            cmd_tcp_session_limits,
            NULL,
            0,
            "set tcp  session limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionTblSize",
            cmd_tcp_session_tbl_size,
            NULL,
            0,
            "set tcp session table size config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionRequestTblSize",
            cmd_tcp_session_request_tbl_size,
            NULL,
            0,
            "set tcp session request table size config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionTblCacheLimits",
            cmd_tcp_session_cache_limits,
            NULL,
            0,
            "set tcp  session table cache limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionRequestTblCacheLimits",
            cmd_tcp_session_request_cache_limits,
            NULL,
            0,
            "set tcp session request table cache limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionRequestTimeout",
            cmd_tcp_session_request_timeout,
            NULL,
            0,
            "set tcp session request timeout config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionTimeout",
            cmd_tcp_session_timeout,
            NULL,
            0,
            "set tcp session timeout config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSHMEntrySize",
            cmd_entry_size,
            NULL,
            0,
            "set tcp share memory entry size config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSHMSize",
            cmd_shm_size,
            NULL,
            0,
            "set tcp share memory size config item"
            ),
    
	CH_INIT_TAKE1(
            "CHTCPSHMFlushTimeout",
            cmd_shm_flush_timeout,
            NULL,
            0,
            "set tcp share memory flush timeout config item"
            ),


    CH_INIT_TAKE1(
            "CHTCPPintCFName",
            cmd_process_interface_cfname,
            NULL,
            0,
            "set tcp process interface config name"
            ),

	CH_INIT_TAKE2(
            "CHTCPMM",
            cmd_mm,
            NULL,
            0,
            "set memory max cache size and timeout"
            ),
	
	CH_INIT_TAKE1(
            "CHTCPAppCFName",
            cmd_app_cfname,
            NULL,
            0,
            "set tcp app config name"
            ),
};

static inline void dump_tcp_context(ch_tcp_context_t *tcp_context){

    fprintf(stdout,"Dump tcp  context-------------------------------------------\n");
    fprintf(stdout,"share memory size  :%lu\n",(unsigned long)tcp_context->shm_size);
    fprintf(stdout,"share memory entry size:%lu\n",(unsigned long)tcp_context->entry_size);
    fprintf(stdout,"share memory flush timeout:%lu\n",(unsigned long)tcp_context->shm_flush_timeout);
    fprintf(stdout,"share memory key:%s\n",tcp_context->key==NULL?"":tcp_context->key);
    fprintf(stdout,"share memory projID:%d\n",tcp_context->proj_id);
    fprintf(stdout,"mmap file dir:%s\n",tcp_context->mmap_file_dir == NULL?"":tcp_context->mmap_file_dir);
    fprintf(stdout,"tcp session tasks number:%d\n",(int)tcp_context->tasks_n);
    
	fprintf(stdout,"tcp session pool type:%s\n",tcp_context->tcp_session_pool_type==FROM_DPDK_POOL?"dpdk_pool":"obj_pool");
    fprintf(stdout,"tcp session request pool type:%s\n",tcp_context->tcp_session_request_pool_type==FROM_DPDK_POOL?"dpdk_pool":"obj_pool");
	
	fprintf(stdout,"tcp session requests limits:%lu\n",(unsigned long)tcp_context->tcp_session_request_limits);
    fprintf(stdout,"tcp session limits:%lu\n",(unsigned long)tcp_context->tcp_session_limits);
    fprintf(stdout,"tcp session table size:%lu\n",(unsigned long)tcp_context->tcp_session_tbl_size);
    fprintf(stdout,"tcp session request table size:%lu\n",(unsigned long)tcp_context->tcp_session_request_tbl_size);
    fprintf(stdout,"tcp session table cache limits:%lu\n",(unsigned long)tcp_context->tcp_session_cache_limits);
    fprintf(stdout,"tcp session request table cache limits:%lu\n",(unsigned long)tcp_context->tcp_session_request_cache_limits);
    fprintf(stdout,"tcp session request timeout:%lu\n",(unsigned long)tcp_context->tcp_session_request_timeout);
    fprintf(stdout,"tcp session timeout:%lu\n",(unsigned long)tcp_context->tcp_session_timeout);
    fprintf(stdout,"tcp process interface config file name:%s\n",tcp_context->pint_cfname);
    fprintf(stdout,"tcp memory max cache size:%lu\n",(unsigned long)tcp_context->mm_max_cache_size);
    fprintf(stdout,"tcp memory timeout:%lu\n",(unsigned long)tcp_context->mm_timeout);
    fprintf(stdout,"tcp application config name:%s\n",tcp_context->app_pool_cfname);

}

ch_tcp_context_t * ch_tcp_context_create(ch_pool_t *mp,const char *cfname){

    const char * msg = NULL;

	ch_tcp_context_t *tcp_context = ch_palloc(mp,sizeof(*tcp_context));

	tcp_context->mp = mp;

    do_tcp_context_init(tcp_context);


    msg = ch_process_command_config(tcp_context_directives,(void*)tcp_context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp context error:%s",msg);
        return NULL;
    }

    dump_tcp_context(tcp_context);

    return tcp_context;

}


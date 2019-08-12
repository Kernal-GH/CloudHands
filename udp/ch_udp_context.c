/*
 *
 *      Filename: ch_udp_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 10:42:39
 * Last Modified: 2018-09-13 16:53:58
 */

#include "ch_udp_context.h"
#include "ch_config.h"
#include "ch_entry_pool.h"
#include "ch_log.h"

static void do_udp_context_init(ch_udp_context_t *udp_context){

	udp_context->log_name = "/tmp/UDPMain.log";
	udp_context->log_level = CH_LOG_NOTICE;

	udp_context->app_udp_cfname = NULL;
	udp_context->pint_udp_cfname = NULL;
	udp_context->entry_size = 16*1024*1024UL;
	udp_context->shm_size = 4*1024*1024*1024UL;
    udp_context->mmap_file_dir = NULL;
	udp_context->key = NULL;
	udp_context->proj_id = 0;
	udp_context->tasks_n = 1;


	udp_context->udp_session_pool_type = FROM_OBJ_POOL;
    udp_context->udp_session_limits = 10000000; 
    udp_context->udp_session_tbl_size = 65536;
    udp_context->udp_session_cache_limits = 1024;
    udp_context->udp_session_timeout = 3*60;

	udp_context->udp_session_request_pool_type = FROM_OBJ_POOL;
    udp_context->udp_session_request_limits = 10000000; 
    udp_context->udp_session_request_timeout = 3*60;
    udp_context->use_msgpack = 1;
}

static const char *cmd_udp_log(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_udp_context_t  *udp_context = (ch_udp_context_t *)_dcfg;

    udp_context->log_name = p1;

    if(strcasecmp(p2,"info") == 0){
        udp_context->log_level = CH_LOG_INFO;
    }else if(strcasecmp(p2,"debug") == 0){
        udp_context->log_level = CH_LOG_DEBUG;
    }else if(strcasecmp(p2,"notice") == 0){
        udp_context->log_level = CH_LOG_NOTICE;
    }else if(strcasecmp(p2,"warn") == 0){
        udp_context->log_level = CH_LOG_WARN;
    }else if(strcasecmp(p2,"error") == 0){
        udp_context->log_level = CH_LOG_ERR;
    }else if(strcasecmp(p2,"crit") == 0){
        udp_context->log_level = CH_LOG_CRIT;
    }else if(strcasecmp(p2,"alert") == 0){
        udp_context->log_level = CH_LOG_ALERT;
    }else if(strcasecmp(p2,"emerg") == 0){
        udp_context->log_level = CH_LOG_EMERG;
    }else {

        return "unknown log level name!";
    }

    return NULL;
}

static const char *cmd_mmap_file_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->mmap_file_dir = p1;
    return NULL;
}

static const char *cmd_shm_key(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->key = p1;
    return NULL;
}

static const char *cmd_tasks_n(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->tasks_n = (uint16_t)strtoul(p1,&endptr,10);
    
    if(context->tasks_n <=0){
        return "invalid tcp session tasks number config value";
    }

    return NULL;
}


static const char *cmd_entry_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->entry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->entry_size <=0){
        return "invalid entry size config value";
    }

    return NULL;
}

static const char *cmd_shm_proj_id(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->proj_id = (int)strtol(p1,&endptr,10);
    

    return NULL;
}

static const char *cmd_shm_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->shm_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->shm_size <=0){
        return "invalid share memory size config value";
    }

    return NULL;
}

static const char *cmd_udp_app_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->app_udp_cfname = p1;

    return NULL;
}

static const char *cmd_udp_pint_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->pint_udp_cfname = p1;

    return NULL;
}


static const char *cmd_session_pool_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

	if(strcasecmp(p1,"dpdk_pool")==0){

		context->udp_session_pool_type = FROM_DPDK_POOL;
	}else{
	
		context->udp_session_pool_type = FROM_OBJ_POOL;
	}

    return NULL;
}

static const char *cmd_udp_session_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->udp_session_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_limits <=0){
        return "invalid udp session number limits config value";
    }

    return NULL;
}

static const char *cmd_udp_session_tbl_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->udp_session_tbl_size = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_tbl_size <=0){
        return "invalid udp session table size config value";
    }

    return NULL;
}

static const char *cmd_udp_use_msgpack(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->use_msgpack = (int)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_udp_session_cache_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->udp_session_cache_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_cache_limits <=0){
        return "invalid udp session table cache limits config value";
    }

    return NULL;
}

static const char *cmd_udp_session_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->udp_session_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_timeout <=0){
        return "invalid udp  session timeout config value";
    }

    return NULL;
}

static const char *cmd_session_request_pool_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

	if(strcasecmp(p1,"dpdk_pool")==0){

		context->udp_session_request_pool_type = FROM_DPDK_POOL;
	}else{
	
		context->udp_session_request_pool_type = FROM_OBJ_POOL;
	}

    return NULL;
}

static const char *cmd_udp_session_request_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->udp_session_request_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_request_timeout <=0){
        return "invalid udp  session request timeout config value";
    }

    return NULL;
}

static const char *cmd_udp_session_request_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_udp_context_t *context = (ch_udp_context_t*)_dcfg;

    context->udp_session_request_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_request_limits <=0){
        return "invalid udp session request number limits config value";
    }

    return NULL;
}

static const command_rec udp_context_directives[] = {
    
	CH_INIT_TAKE2(
            "CHUDPLog",
            cmd_udp_log,
            NULL,
            0,
            "set udp log name and level"
            ),
	
	CH_INIT_TAKE1(
            "CHUDPAppCFName",
            cmd_udp_app_cfname,
            NULL,
            0,
            "set udp application config file name"
            ),
	
	CH_INIT_TAKE1(
            "CHUDPPintCFName",
            cmd_udp_pint_cfname,
            NULL,
            0,
            "set udp process interface config file name"
            ),

    CH_INIT_TAKE1(
            "CHUDPMMapFileDir",
            cmd_mmap_file_dir,
            NULL,
            0,
            "set udp mmap file dir config item"
            ),
    
	CH_INIT_TAKE1(
            "CHUDPSHMKey",
            cmd_shm_key,
            NULL,
            0,
            "set udp share memory key config item"
            ),
	
	CH_INIT_TAKE1(
            "CHUDPSHMProjID",
            cmd_shm_proj_id,
            NULL,
            0,
            "set udp share memory proj_Id config item"
            ),
	

    CH_INIT_TAKE1(
            "CHUDPSessionTasks",
            cmd_tasks_n,
            NULL,
            0,
            "set udp session tasks number config item"
            ),
    

    CH_INIT_TAKE1(
            "CHUDPSHMEntrySize",
            cmd_entry_size,
            NULL,
            0,
            "set udp share memory entry size config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPSHMSize",
            cmd_shm_size,
            NULL,
            0,
            "set udp share memory size config item"
            ),
    
	
	CH_INIT_TAKE1(
            "CHUDPUDPSessionPoolType",
            cmd_session_pool_type,
            NULL,
            0,
            "set udp session  pool type(dpdk_pool or obj_pool) config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPUDPSessionLimits",
            cmd_udp_session_limits,
            NULL,
            0,
            "set udp session limits config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPUDPSessionTblSize",
            cmd_udp_session_tbl_size,
            NULL,
            0,
            "set udp session table size config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPUDPSessionTblCacheLimits",
            cmd_udp_session_cache_limits,
            NULL,
            0,
            "set udp  session table cache limits config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPUDPSessionTimeout",
            cmd_udp_session_timeout,
            NULL,
            0,
            "set udp session timeout config item"
            ),
    
	CH_INIT_TAKE1(
            "CHUDPUDPSessionRequestPoolType",
            cmd_session_request_pool_type,
            NULL,
            0,
            "set udp session request pool type(dpdk_pool or obj_pool) config item"
            ),
    
	CH_INIT_TAKE1(
            "CHUDPUDPSessionRequestTimeout",
            cmd_udp_session_request_timeout,
            NULL,
            0,
            "set udp session request timeout config item"
            ),
    
	CH_INIT_TAKE1(
            "CHUDPUDPSessionRequestLimits",
            cmd_udp_session_request_limits,
            NULL,
            0,
            "set udp session request limits config item"
            ),
    
    CH_INIT_TAKE1(
            "CHUDPUseMsgpack",
            cmd_udp_use_msgpack,
            NULL,
            0,
            "set udp session write by msgpack?"
            ),

};

static inline void dump_udp_context(ch_udp_context_t *udp_context){

    fprintf(stdout,"Dump UDP context-------------------------------------------\n");
    fprintf(stdout,"share memory size  :%lu\n",(unsigned long)udp_context->shm_size);
    fprintf(stdout,"share memory entry size:%lu\n",(unsigned long)udp_context->entry_size);
    fprintf(stdout,"share memory key:%s\n",udp_context->key==NULL?"":udp_context->key);
    fprintf(stdout,"share memory projID:%d\n",udp_context->proj_id);
    fprintf(stdout,"udp application config file name:%s\n",udp_context->app_udp_cfname);
    fprintf(stdout,"udp process interface config file name:%s\n",udp_context->pint_udp_cfname);
    fprintf(stdout,"mmap file dir:%s\n",udp_context->mmap_file_dir == NULL?"":udp_context->mmap_file_dir);
	
	fprintf(stdout,"udp session pool type:%s\n",udp_context->udp_session_pool_type==FROM_DPDK_POOL?"dpdk_pool":"obj_pool");
    fprintf(stdout,"udp session limits:%lu\n",(unsigned long)udp_context->udp_session_limits);
    fprintf(stdout,"udp session table size:%lu\n",(unsigned long)udp_context->udp_session_tbl_size);
    fprintf(stdout,"udp session table cache limits:%lu\n",(unsigned long)udp_context->udp_session_cache_limits);
    fprintf(stdout,"udp session timeout:%lu\n",(unsigned long)udp_context->udp_session_timeout);
	fprintf(stdout,"udp session request pool type:%s\n",udp_context->udp_session_request_pool_type==FROM_DPDK_POOL?"dpdk_pool":"obj_pool");

    fprintf(stdout,"udp session request timeout:%lu\n",(unsigned long)udp_context->udp_session_request_timeout);
    
	fprintf(stdout,"udp session request limits:%lu\n",(unsigned long)udp_context->udp_session_request_limits);
	
    fprintf(stdout,"udp session use msgpack:%s\n",udp_context->use_msgpack?"yes":"no");
}

ch_udp_context_t * ch_udp_context_create(ch_pool_t *mp,const char *cfname){

    const char * msg = NULL;

	ch_udp_context_t *udp_context = ch_palloc(mp,sizeof(*udp_context));

	udp_context->mp = mp;

    do_udp_context_init(udp_context);


    msg = ch_process_command_config(udp_context_directives,(void*)udp_context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp context error:%s",msg);
        return NULL;
    }

    dump_udp_context(udp_context);

    return udp_context;

}


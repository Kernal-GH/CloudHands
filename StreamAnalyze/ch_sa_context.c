/*
 *
 *      Filename: ch_sa_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-30 10:41:45
 * Last Modified: 2018-06-06 16:13:34
 */

#include "ch_sa_context.h"

#include "ch_config.h"
#include "ch_tcp_context.h"
#include "ch_entry_pool.h"
#include "ch_log.h"

static void do_sa_context_init(ch_sa_context_t *sa_context){

	sa_context->log_name = "/tmp/SAMain.log";
	sa_context->log_level = CH_LOG_NOTICE;

	sa_context->entry_size = 16*1024*1024UL;
	sa_context->shm_size = 4*1024*1024*1024UL;
    sa_context->mmap_file_dir = NULL;
	sa_context->key = NULL;
	sa_context->proj_id = 0;
	sa_context->tasks_n = 1;

	sa_context->tcp_cfg_path = NULL;
	sa_context->rdb_store_dir = NULL;
	sa_context->rdb_name_create_type = "D";
	sa_context->payload_data_size = 128;

	sa_context->tcp_cfname = NULL;

	sa_context->pint_sa_cfname = NULL;


	sa_context->udp_session_pool_type = FROM_OBJ_POOL;
    sa_context->udp_session_limits = 10000000; 
    sa_context->udp_session_tbl_size = 65536;
    sa_context->udp_session_cache_limits = 1024;
    sa_context->udp_session_timeout = 3*60;
	sa_context->rdb_using_timeout = 3*60;



	sa_context->dstore_limits = 100000;

    sa_context->lua_path = NULL;
    sa_context->lua_fname = NULL;

    sa_context->tcp_sa_on = 0;
    sa_context->udp_sa_on = 0;
    sa_context->arp_sa_on = 0;
    sa_context->icmp_sa_on = 0;

}

static const char *cmd_sa_log(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_sa_context_t  *sa_context = (ch_sa_context_t *)_dcfg;

    sa_context->log_name = p1;

    if(strcasecmp(p2,"info") == 0){
        sa_context->log_level = CH_LOG_INFO;
    }else if(strcasecmp(p2,"debug") == 0){
        sa_context->log_level = CH_LOG_DEBUG;
    }else if(strcasecmp(p2,"notice") == 0){
        sa_context->log_level = CH_LOG_NOTICE;
    }else if(strcasecmp(p2,"warn") == 0){
        sa_context->log_level = CH_LOG_WARN;
    }else if(strcasecmp(p2,"error") == 0){
        sa_context->log_level = CH_LOG_ERR;
    }else if(strcasecmp(p2,"crit") == 0){
        sa_context->log_level = CH_LOG_CRIT;
    }else if(strcasecmp(p2,"alert") == 0){
        sa_context->log_level = CH_LOG_ALERT;
    }else if(strcasecmp(p2,"emerg") == 0){
        sa_context->log_level = CH_LOG_EMERG;
    }else {

        return "unknown log level name!";
    }

    return NULL;
}

static const char *cmd_mmap_file_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->mmap_file_dir = p1;
    return NULL;
}

static const char *cmd_shm_key(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->key = p1;
    return NULL;
}

static const char *cmd_rdb_store(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->rdb_store_dir = p1;

    return NULL;
}

static const char *cmd_rdb_name_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->rdb_name_create_type = p1;

    return NULL;
}

static const char *cmd_tasks_n(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->tasks_n = (uint16_t)strtoul(p1,&endptr,10);
    
    if(context->tasks_n <=0){
        return "invalid tcp session tasks number config value";
    }

    return NULL;
}


static const char *cmd_entry_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->entry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->entry_size <=0){
        return "invalid entry size config value";
    }

    return NULL;
}

static const char *cmd_shm_proj_id(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->proj_id = (int)strtol(p1,&endptr,10);
    

    return NULL;
}

static const char *cmd_shm_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->shm_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->shm_size <=0){
        return "invalid share memory size config value";
    }

    return NULL;
}

static const char *cmd_tcp_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->tcp_cfname = p1;

    return NULL;
}

static const char *cmd_sa_pint_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->pint_sa_cfname = p1;

    return NULL;
}

static const char *cmd_session_pool_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

	if(strcasecmp(p1,"dpdk_pool")==0){

		context->udp_session_pool_type = FROM_DPDK_POOL;
	}else{
	
		context->udp_session_pool_type = FROM_OBJ_POOL;
	}

    return NULL;
}

static const char *cmd_udp_session_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->udp_session_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_limits <=0){
        return "invalid udp session number limits config value";
    }

    return NULL;
}

static const char *cmd_udp_session_tbl_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->udp_session_tbl_size = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_tbl_size <=0){
        return "invalid udp session table size config value";
    }

    return NULL;
}

static const char *cmd_udp_session_cache_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->udp_session_cache_limits = (size_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_cache_limits <=0){
        return "invalid udp session table cache limits config value";
    }

    return NULL;
}

static const char *cmd_udp_session_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->udp_session_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->udp_session_timeout <=0){
        return "invalid udp  session timeout config value";
    }

    return NULL;
}

static const char *cmd_payload_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->payload_data_size = (uint32_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_rdb_using_timeout(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->rdb_using_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    return NULL;
}



static const char *cmd_sa_dstore_limits(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->dstore_limits = (uint32_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_sa_lua_path(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->lua_path = p1;
    
    return NULL;
}

static const char *cmd_sa_lua_fname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->lua_fname = p1;
    
    return NULL;
}

static const char *cmd_sa_tcp_on(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->tcp_sa_on  = 0;
    if(strcasecmp(p1,"on")==0){

        context->tcp_sa_on = 1;
    }
    
    return NULL;
}

static const char *cmd_sa_udp_on(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->udp_sa_on  = 0;
    if(strcasecmp(p1,"on")==0){

        context->udp_sa_on = 1;
    }
    
    return NULL;
}

static const char *cmd_sa_icmp_on(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->icmp_sa_on  = 0;
    if(strcasecmp(p1,"on")==0){

        context->icmp_sa_on = 1;
    }
    
    return NULL;
}

static const char *cmd_sa_arp_on(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_sa_context_t *context = (ch_sa_context_t*)_dcfg;

    context->arp_sa_on  = 0;
    if(strcasecmp(p1,"on")==0){

        context->arp_sa_on = 1;
    }
    
    return NULL;
}

static const command_rec sa_context_directives[] = {
    
	CH_INIT_TAKE2(
            "CHSALog",
            cmd_sa_log,
            NULL,
            0,
            "set sa log name and level"
            ),

    CH_INIT_TAKE1(
            "CHSAMMapFileDir",
            cmd_mmap_file_dir,
            NULL,
            0,
            "set sa mmap file dir config item"
            ),
    
	CH_INIT_TAKE1(
            "CHSASHMKey",
            cmd_shm_key,
            NULL,
            0,
            "set sa share memory key config item"
            ),
	
	CH_INIT_TAKE1(
            "CHSASHMProjID",
            cmd_shm_proj_id,
            NULL,
            0,
            "set sa share memory proj_Id config item"
            ),
	

    CH_INIT_TAKE1(
            "CHSASessionTasks",
            cmd_tasks_n,
            NULL,
            0,
            "set sa session tasks number config item"
            ),
    
    CH_INIT_TAKE1(
            "CHSARDBStoreDir",
            cmd_rdb_store,
            NULL,
            0,
            "set sa rdb store dir"
            ),
    
	CH_INIT_TAKE1(
            "CHSARDBNameCreateType",
            cmd_rdb_name_create_type,
            NULL,
            0,
            "set sa rdb store name create type"
            ),
    

    CH_INIT_TAKE1(
            "CHSASHMEntrySize",
            cmd_entry_size,
            NULL,
            0,
            "set sa share memory entry size config item"
            ),

    CH_INIT_TAKE1(
            "CHSASHMSize",
            cmd_shm_size,
            NULL,
            0,
            "set sa share memory size config item"
            ),
    
	CH_INIT_TAKE1(
            "CHSATCPCFName",
            cmd_tcp_cfname,
            NULL,
            0,
            "set tcp config file name"
            ),

	CH_INIT_TAKE1(
            "CHSAPintCFName",
            cmd_sa_pint_cfname,
            NULL,
            0,
            "set sa process interface config file name"
            ),
	
	CH_INIT_TAKE1(
            "CHSAUDPSessionPoolType",
            cmd_session_pool_type,
            NULL,
            0,
            "set udp session  pool type(dpdk_pool or obj_pool) config item"
            ),

    CH_INIT_TAKE1(
            "CHSAUDPSessionLimits",
            cmd_udp_session_limits,
            NULL,
            0,
            "set udp session limits config item"
            ),

    CH_INIT_TAKE1(
            "CHSAUDPSessionTblSize",
            cmd_udp_session_tbl_size,
            NULL,
            0,
            "set udp session table size config item"
            ),

    CH_INIT_TAKE1(
            "CHSAUDPSessionTblCacheLimits",
            cmd_udp_session_cache_limits,
            NULL,
            0,
            "set udp  session table cache limits config item"
            ),

    CH_INIT_TAKE1(
            "CHSAUDPSessionTimeout",
            cmd_udp_session_timeout,
            NULL,
            0,
            "set udp session timeout config item"
            ),
    
	CH_INIT_TAKE1(
            "CHSAPayloadSize",
            cmd_payload_size,
            NULL,
            0,
            "set the store session payload size"
            ),
	
	CH_INIT_TAKE1(
            "CHSARDBUsingTimeout",
            cmd_rdb_using_timeout,
            NULL,
            0,
            "set the store using timeout"
            ),
	

	
	CH_INIT_TAKE1(
            "CHSADataStoreLimits",
            cmd_sa_dstore_limits,
            NULL,
            0,
            "set the data store limits can been allocated!"
            ),
	
    CH_INIT_TAKE1(
            "CHSALuaPath",
            cmd_sa_lua_path,
            NULL,
            0,
            "set the sa lua path"
            ),

    CH_INIT_TAKE1(
            "CHSALuaFName",
            cmd_sa_lua_fname,
            NULL,
            0,
            "set the sa lua fname"
            ),

    CH_INIT_TAKE1(
            "CHSATCPIsON",
            cmd_sa_tcp_on,
            NULL,
            0,
            "set the sa tcp is on/off"
            ),
    
    CH_INIT_TAKE1(
            "CHSAUDPIsON",
            cmd_sa_udp_on,
            NULL,
            0,
            "set the sa udp is on/off"
            ),

    CH_INIT_TAKE1(
            "CHSAICMPIsON",
            cmd_sa_icmp_on,
            NULL,
            0,
            "set the sa icmp is on/off"
            ),

    CH_INIT_TAKE1(
            "CHSAARPIsON",
            cmd_sa_arp_on,
            NULL,
            0,
            "set the sa arp is on/off"
            ),
};

static inline void dump_sa_context(ch_sa_context_t *sa_context){

    fprintf(stdout,"Dump SA context-------------------------------------------\n");
    fprintf(stdout,"share memory size  :%lu\n",(unsigned long)sa_context->shm_size);
    fprintf(stdout,"share memory entry size:%lu\n",(unsigned long)sa_context->entry_size);
    fprintf(stdout,"share memory key:%s\n",sa_context->key==NULL?"":sa_context->key);
    fprintf(stdout,"share memory projID:%d\n",sa_context->proj_id);
    fprintf(stdout,"mmap file dir:%s\n",sa_context->mmap_file_dir == NULL?"":sa_context->mmap_file_dir);
    fprintf(stdout,"sa session tasks number:%d\n",(int)sa_context->tasks_n);
    fprintf(stdout,"tcp config file name:%s\n",sa_context->tcp_cfname);
    fprintf(stdout,"rdb store dir:%s\n",sa_context->rdb_store_dir);
    fprintf(stdout,"rdb store name create type:%s\n",sa_context->rdb_name_create_type);

    fprintf(stdout,"sa process interface config file name:%s\n",sa_context->pint_sa_cfname);
	
	fprintf(stdout,"udp session pool type:%s\n",sa_context->udp_session_pool_type==FROM_DPDK_POOL?"dpdk_pool":"obj_pool");
    fprintf(stdout,"udp session limits:%lu\n",(unsigned long)sa_context->udp_session_limits);
    fprintf(stdout,"udp session table size:%lu\n",(unsigned long)sa_context->udp_session_tbl_size);
    fprintf(stdout,"udp session table cache limits:%lu\n",(unsigned long)sa_context->udp_session_cache_limits);
    fprintf(stdout,"udp session timeout:%lu\n",(unsigned long)sa_context->udp_session_timeout);
    fprintf(stdout,"store session payload size:%lu\n",(unsigned long)sa_context->payload_data_size);
    fprintf(stdout,"store session using timeout:%lu\n",(unsigned long)sa_context->rdb_using_timeout);
    

    fprintf(stdout,"data store limits:%lu\n",(unsigned long)sa_context->dstore_limits);
    fprintf(stdout,"sa lua path:%s\n",sa_context->lua_path);
    fprintf(stdout,"sa lua fname:%s\n",sa_context->lua_fname);
    
    fprintf(stdout,"sa tcp is on:%s\n",sa_context->tcp_sa_on?"on":"off");
    fprintf(stdout,"sa udp is on:%s\n",sa_context->udp_sa_on?"on":"off");
    fprintf(stdout,"sa icmp is on:%s\n",sa_context->icmp_sa_on?"on":"off");
    fprintf(stdout,"sa arp is on:%s\n",sa_context->arp_sa_on?"on":"off");

}

ch_sa_context_t * ch_sa_context_create(ch_pool_t *mp,const char *cfname){

    const char * msg = NULL;

	ch_sa_context_t *sa_context = ch_palloc(mp,sizeof(*sa_context));

	sa_context->mp = mp;

    do_sa_context_init(sa_context);


    msg = ch_process_command_config(sa_context_directives,(void*)sa_context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp context error:%s",msg);
        return NULL;
    }

    dump_sa_context(sa_context);

    return sa_context;

}


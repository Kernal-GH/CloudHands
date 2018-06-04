/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_context_tcp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月29日 15时02分16秒
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
#include "ch_assemble_context_tcp.h"

static void do_tcp_context_init(ch_assemble_context_tcp_t *tcp_context){

    tcp_context->mmap_file_dir = "/tmp/";
    tcp_context->assemble_tasks_n = 1;
    tcp_context->tcp_session_requests_limits = 10000000;
    tcp_context->tcp_assemble_sessions_limits = 10000000; 
    
    tcp_context->tcp_assemble_sessions_tbl_size = 65536;
    tcp_context->tcp_session_requests_tbl_size = 65536;
    tcp_context->tcp_assemble_sessions_tbl_caches_limits = 1024;
    tcp_context->tcp_session_requests_tbl_caches_limits = 1024;

    tcp_context->tcp_session_request_timeout = 60;
    tcp_context->tcp_assemble_session_timeout = 5*60;
    tcp_context->mmap_file_size = 4*1024*1024*1024UL;
    tcp_context->mmap_file_entry_size = 16*1024*1024UL;

}

static const char *cmd_mmap_file_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->mmap_file_dir = p1;
    return NULL;
}

static const char *cmd_assemble_tasks_n(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->assemble_tasks_n = (uint16_t)strtoul(p1,&endptr,10);
    
    if(context->assemble_tasks_n <=0){
        return "invalid assemble tasks number config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_requests_limits(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_session_requests_limits = (uint32_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_requests_limits <=0){
        return "invalid tcp session requests number limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_assemble_sessions_limits(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_assemble_sessions_limits = (uint32_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_assemble_sessions_limits <=0){
        return "invalid assemble sessions number limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_assemble_sessions_tbl_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_assemble_sessions_tbl_size = (uint32_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_assemble_sessions_tbl_size <=0){
        return "invalid assemble sessions table size config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_requests_tbl_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_session_requests_tbl_size = (uint32_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_requests_tbl_size <=0){
        return "invalid tcp session requests table size config value";
    }

    return NULL;
}

static const char *cmd_tcp_assemble_sessions_tbl_caches_limits(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_assemble_sessions_tbl_caches_limits = (uint32_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_assemble_sessions_tbl_caches_limits <=0){
        return "invalid assemble sessions table caches limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_requests_tbl_caches_limits(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_session_requests_tbl_caches_limits = (uint32_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_requests_tbl_caches_limits <=0){
        return "invalid session requests table caches limits config value";
    }

    return NULL;
}

static const char *cmd_tcp_session_request_timeout(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_session_request_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_session_request_timeout <=0){
        return "invalid tcp session request timeout config value";
    }

    return NULL;
}

static const char *cmd_tcp_assemble_session_timeout(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->tcp_assemble_session_timeout = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->tcp_assemble_session_timeout <=0){
        return "invalid tcp assemble session timeout config value";
    }

    return NULL;
}

static const char *cmd_mmap_file_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->mmap_file_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->mmap_file_size <=0){
        return "invalid mmap file size config value";
    }

    return NULL;
}

static const char *cmd_mmap_file_entry_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_tcp_t *context = (ch_assemble_context_tcp_t*)_dcfg;

    context->mmap_file_entry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->mmap_file_entry_size <=0){
        return "invalid mmap file entry size config value";
    }

    return NULL;
}

static const command_rec tcp_context_directives[] = {

    CH_INIT_TAKE1(
            "CHTCPMMapFileDir",
            cmd_mmap_file_dir,
            NULL,
            0,
            "set tcp mmap file dir config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPAssembleTasks",
            cmd_assemble_tasks_n,
            NULL,
            0,
            "set tcp assemble tasks number config item"
            ),
    
    CH_INIT_TAKE1(
            "CHTCPSessionRequestsLimits",
            cmd_tcp_session_requests_limits,
            NULL,
            0,
            "set tcp session requests limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPAssembleSessionsLimits",
            cmd_tcp_assemble_sessions_limits,
            NULL,
            0,
            "set tcp assemble sessions limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPAssembleSessionsTblSize",
            cmd_tcp_assemble_sessions_tbl_size,
            NULL,
            0,
            "set tcp assemble sessions table size config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionRequestsTblSize",
            cmd_tcp_session_requests_tbl_size,
            NULL,
            0,
            "set tcp session requests table size config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPAssembleSessionsTblCachesLimits",
            cmd_tcp_assemble_sessions_tbl_caches_limits,
            NULL,
            0,
            "set tcp assemble sessions table caches limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionRequestsTblCachesLimits",
            cmd_tcp_session_requests_tbl_caches_limits,
            NULL,
            0,
            "set tcp session requests table caches limits config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPSessionRequestTimeout",
            cmd_tcp_session_request_timeout,
            NULL,
            0,
            "set tcp session request timeout config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPAssembleSessionTimeout",
            cmd_tcp_assemble_session_timeout,
            NULL,
            0,
            "set tcp assemble session timeout config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPMMapFileSize",
            cmd_mmap_file_size,
            NULL,
            0,
            "set mmap file size config item"
            ),

    CH_INIT_TAKE1(
            "CHTCPMMapFileEntrySize",
            cmd_mmap_file_entry_size,
            NULL,
            0,
            "set tcp mmap file entry size config item"
            ),

};

static inline void dump_assemble_context_tcp(ch_assemble_context_tcp_t *tcp_context){

    fprintf(stdout,"Dump tcp assemble context-------------------------------------------\n");
    fprintf(stdout,"mmap dir:%s\n",tcp_context->mmap_file_dir);
    fprintf(stdout,"assemble tasks:%lu\n",(unsigned long)tcp_context->assemble_tasks_n);
    fprintf(stdout,"session requests limits:%lu\n",(unsigned long)tcp_context->tcp_session_requests_limits);
    fprintf(stdout,"assembl sessions limits:%lu\n",(unsigned long)tcp_context->tcp_assemble_sessions_limits);
    fprintf(stdout,"assemble sessions table size:%lu\n",(unsigned long)tcp_context->tcp_assemble_sessions_tbl_size);
    fprintf(stdout,"session requests table size:%lu\n",(unsigned long)tcp_context->tcp_session_requests_tbl_size);
    fprintf(stdout,"assemble sessions table caches limits:%lu\n",(unsigned long)tcp_context->tcp_assemble_sessions_tbl_caches_limits);
    fprintf(stdout,"session requests table caches limits:%lu\n",(unsigned long)tcp_context->tcp_session_requests_tbl_caches_limits);
    fprintf(stdout,"session request timeout:%lu\n",(unsigned long)tcp_context->tcp_session_request_timeout);
    fprintf(stdout,"assemble session timeout:%lu\n",(unsigned long)tcp_context->tcp_assemble_session_timeout);
    fprintf(stdout,"mmap file size  :%lu\n",(unsigned long)tcp_context->mmap_file_size);
    fprintf(stdout,"mmap fentry size:%lu\n",(unsigned long)tcp_context->mmap_file_entry_size);

}

int ch_assemble_context_tcp_init(ch_context_t *context,ch_assemble_context_tcp_t *tcp_context){

    const char * msg = NULL;

    do_tcp_context_init(tcp_context);

    tcp_context->context = context;

    msg = ch_process_command_config(tcp_context_directives,(void*)tcp_context,context->mp,context->mp,
            context->tcp_cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp context error:%s",msg);
        return -1;
    }

    dump_assemble_context_tcp(tcp_context);

    return 0;
}

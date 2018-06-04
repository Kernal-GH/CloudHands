/*
 *
 *      Filename: ch_assemble_context_udp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 15:35:11
 * Last Modified: 2016-10-17 23:27:58
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_assemble_context_udp.h"

static void do_udp_context_init(ch_assemble_context_udp_t *udp_context){

    udp_context->mmap_file_dir = "/tmp/";

	udp_context->ip_frag_tbl_bucket_num = 4096;
	udp_context->ip_frag_tbl_bucket_entries = 16;
	udp_context->ip_frag_tbl_max_entries = 4096;
	udp_context->ip_frag_tbl_max_cycles = (rte_get_tsc_hz() + 1000 - 1) / 1000 *1000;

    udp_context->mmap_file_size = 4*1024*1024*1024UL;
    udp_context->mmap_file_entry_size = 16*1024*1024UL;

}

static const char *cmd_mmap_file_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    context->mmap_file_dir = p1;
    return NULL;
}

static const char *cmd_ip_frag_tbl_bucket_num(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    context->ip_frag_tbl_bucket_num = (uint32_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_ip_frag_tbl_bucket_entries(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    context->ip_frag_tbl_bucket_entries = (uint32_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_ip_frag_tbl_max_entries(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    context->ip_frag_tbl_max_entries = (uint32_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_ip_frag_tbl_max_cycles(cmd_parms *cmd, void *_dcfg, const char *p1){

	uint64_t ttl;

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    ttl = (uint64_t)strtoul(p1,&endptr,10);
    
	context->ip_frag_tbl_max_cycles = (rte_get_tsc_hz() + 1000 - 1) / 1000 *ttl;
    
	return NULL;
}

static const char *cmd_mmap_file_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    context->mmap_file_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->mmap_file_size <=0){
        return "invalid mmap file size config value";
    }

    return NULL;
}

static const char *cmd_mmap_file_entry_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_assemble_context_udp_t *context = (ch_assemble_context_udp_t*)_dcfg;

    context->mmap_file_entry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(context->mmap_file_entry_size <=0){
        return "invalid mmap file entry size config value";
    }

    return NULL;
}

static const command_rec udp_context_directives[] = {

    CH_INIT_TAKE1(
            "CHUDPMMapFileDir",
            cmd_mmap_file_dir,
            NULL,
            0,
            "set udp mmap file dir config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPIPFragBucketNum",
            cmd_ip_frag_tbl_bucket_num,
            NULL,
            0,
            "set ip frag table bucket number"
            ),
    
    CH_INIT_TAKE1(
            "CHUDPIPFragBucketEntries",
            cmd_ip_frag_tbl_bucket_entries,
            NULL,
            0,
            "set ip frag table bucket entries number"
            ),

    CH_INIT_TAKE1(
            "CHUDPIPFragTableMaxEntries",
            cmd_ip_frag_tbl_max_entries,
            NULL,
            0,
            "set ip frag table max entries number"
            ),

    CH_INIT_TAKE1(
            "CHUDPIPFragTableMaxCycles",
            cmd_ip_frag_tbl_max_cycles,
            NULL,
            0,
            "set ip frag table max cycles"
            ),

    CH_INIT_TAKE1(
            "CHUDPMMapFileSize",
            cmd_mmap_file_size,
            NULL,
            0,
            "set mmap file size config item"
            ),

    CH_INIT_TAKE1(
            "CHUDPMMapFileEntrySize",
            cmd_mmap_file_entry_size,
            NULL,
            0,
            "set udp mmap file entry size config item"
            ),

};

static inline void dump_assemble_context_udp(ch_assemble_context_udp_t *udp_context){

    fprintf(stdout,"Dump udp assemble context-------------------------------------------\n");
    fprintf(stdout,"mmap dir:%s\n",udp_context->mmap_file_dir);
    fprintf(stdout,"ip frag table bucket num:%lu\n",(unsigned long)udp_context->ip_frag_tbl_bucket_num);
    fprintf(stdout,"ip frag table bucket entries:%lu\n",(unsigned long)udp_context->ip_frag_tbl_bucket_entries);
    fprintf(stdout,"ip frag table max entries:%lu\n",(unsigned long)udp_context->ip_frag_tbl_max_entries);
    fprintf(stdout,"ip frag table max cycles:%lu\n",(unsigned long)udp_context->ip_frag_tbl_max_cycles);
    fprintf(stdout,"mmap file size  :%lu\n",(unsigned long)udp_context->mmap_file_size);
    fprintf(stdout,"mmap fentry size:%lu\n",(unsigned long)udp_context->mmap_file_entry_size);

}

int ch_assemble_context_udp_init(ch_context_t *context,ch_assemble_context_udp_t *udp_context){

    const char * msg = NULL;

    do_udp_context_init(udp_context);

    udp_context->context = context;

    msg = ch_process_command_config(udp_context_directives,(void*)udp_context,context->mp,context->mp,
            context->udp_cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config udp context error:%s",msg);
        return -1;
    }

    dump_assemble_context_udp(udp_context);

    return 0;
}

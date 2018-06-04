/*
 *
 *      Filename: ch_packets_context.c
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-05-31 12:00:03
 * Last Modified: 2017-05-31 17:30:20
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_packets_context.h"

static void do_packets_context_init(ch_packets_context_t *pcontext){

    pcontext->mmap_file_dir = "/tmp/packets.data";
    
    pcontext->mmap_file_size = 4*1024*1024*1024UL;
    pcontext->mmap_file_entry_size = 16*1024*1024UL;

	pcontext->max_packet_data_size = 128;
}

static const char *cmd_mmap_file_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_packets_context_t *pcontext = (ch_packets_context_t*)_dcfg;

    pcontext->mmap_file_dir = p1;
    return NULL;
}

static const char *cmd_mmap_file_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_packets_context_t *pcontext = (ch_packets_context_t*)_dcfg;

    pcontext->mmap_file_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(pcontext->mmap_file_size <=0){
        return "invalid mmap file size config value";
    }

    return NULL;
}

static const char *cmd_mmap_file_entry_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_packets_context_t *pcontext = (ch_packets_context_t*)_dcfg;

    pcontext->mmap_file_entry_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(pcontext->mmap_file_entry_size <=0){
        return "invalid mmap file entry size config value";
    }

    return NULL;
}

static const char *cmd_packet_data_max_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_packets_context_t *pcontext = (ch_packets_context_t*)_dcfg;

    pcontext->max_packet_data_size = (uint64_t)strtoul(p1,&endptr,10);
    
    if(pcontext->max_packet_data_size <=0){
        return "invalid max packet data  size config value";
    }

    return NULL;
}

static const command_rec pcontext_directives[] = {

    CH_INIT_TAKE1(
            "CHPacketsMMapFileDir",
            cmd_mmap_file_dir,
            NULL,
            0,
            "set packets mmap file dir config item"
            ),

    CH_INIT_TAKE1(
            "CHPacketsMMapFileSize",
            cmd_mmap_file_size,
            NULL,
            0,
            "set packets mmap file size config item"
            ),

    CH_INIT_TAKE1(
            "CHPacketsMMapFileEntrySize",
            cmd_mmap_file_entry_size,
            NULL,
            0,
            "set packets mmap file entry size config item"
            ),
    
	CH_INIT_TAKE1(
            "CHPacketsMaxPacketDataSize",
            cmd_packet_data_max_size,
            NULL,
            0,
            "set packets max packet data  size config item"
            ),
};

static  void dump_packets_context_tcp(ch_packets_context_t *pcontext){

    fprintf(stdout,"Dump packets  context-------------------------------------------\n");
    fprintf(stdout,"mmap dir:%s\n",pcontext->mmap_file_dir);
    fprintf(stdout,"mmap file size  :%lu\n",(unsigned long)pcontext->mmap_file_size);
    fprintf(stdout,"mmap fentry size:%lu\n",(unsigned long)pcontext->mmap_file_entry_size);
    fprintf(stdout,"max packet data size:%lu\n",(unsigned long)pcontext->max_packet_data_size);

}


int ch_packets_context_init(ch_packets_context_t *pcontext,const char *cfname){

    const char * msg = NULL;

    do_packets_context_init(pcontext);

    msg = ch_process_command_config(pcontext_directives,(void*)pcontext,pcontext->mp,pcontext->mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config packets context error:%s",msg);
        return -1;
    }


	dump_packets_context_tcp(pcontext);

    return 0;

}



/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_data_writer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月1日 11时50分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_log.h"
#include "ch_tcp_data_writer.h"
#include "ch_assemble_session_tcp.h"
#include "ch_mmap_file_format.h"

static inline uint8_t _session_direct(ch_assemble_session_tcp_t *ass){

    if(ass->cur_ep == &ass->endpoint_req){

        return SESSION_DIRECT_REQ; 
    }

    return SESSION_DIRECT_RES;
}

static void _app_mmap_file_entry_init(ch_mmap_file_entry_t *fentry,ch_assemble_session_tcp_t *ass,void *data,size_t dlen,
        uint8_t packet_type,uint32_t protocol_id){


    fentry->magic = EIMAGIC;
    fentry->entry_size = dlen+CH_MMAP_FILE_ENTRY_HEADER_LEN;
    fentry->packet_type = packet_type;
    fentry->session_direct = _session_direct(ass);
    fentry->protocol_id = protocol_id;
    fentry->session_id = ass->session_id;
    fentry->time = ass->cur_ep->time;
    fentry->src_ip = ch_assemble_session_tcp_srcip_get(ass); 
    fentry->dst_ip = ch_assemble_session_tcp_dstip_get(ass);
    fentry->src_port = ch_assemble_session_tcp_srcport_get(ass);
    fentry->dst_port = ch_assemble_session_tcp_dstport_get(ass);

    fentry->data_offset = ass->cur_ep->last_offset - dlen;
    fentry->data_len = dlen;
    fentry->data = data;
}


int ch_tcp_data_write(void *tcp_session,void *data,size_t dlen,uint8_t packet_type,uint32_t protocol_id){

    int rc;

    ch_mmap_file_entry_t fentry;

    ch_assemble_session_tcp_t *ass = (ch_assemble_session_tcp_t*)tcp_session;

    _app_mmap_file_entry_init(&fentry,ass,data,dlen,packet_type,protocol_id);


    for(;;){

        rc = ch_mmap_file_format_put(ass->as->mmfmt,&fentry);
        if(rc == 0){
            return PARSE_RETURN_DONE;
        }
        if(rc == -2){

            ch_log(CH_LOG_ERR,"The data is too large:%lu,discard it!",dlen);
            /*write a close packet*/
            _app_mmap_file_entry_init(&fentry,ass,NULL,0,PACKET_TYPE_CLOSE,protocol_id);
            
            while(ch_mmap_file_format_put(ass->as->mmfmt,&fentry)){
                usleep(5);
            }

            return PARSE_RETURN_DISCARD;
        }

        ch_log(CH_LOG_DEBUG,"The mmap file:[%s] is full,sleep and try again!",
			ass->as->mmfmt->mm->fname);

        usleep(5);
    }

}

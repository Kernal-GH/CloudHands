/*
 *
 *      Filename: ch_udp_data_writer.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-16 19:14:48
 * Last Modified: 2017-11-14 19:11:16
 */

#include "ch_log.h"
#include "ch_assemble_packet_udp.h"
#include "ch_mmap_file_format.h"
#include "ch_udp_data_writer.h"
#include "ch_assemble.h"
#include "ch_app_context.h"

static void _app_udp_mmap_file_entry_init(ch_mmap_file_entry_t *fentry,ch_assemble_packet_udp_t *as_udp_pkt,void *data,size_t dlen,
        uint8_t packet_type,uint32_t protocol_id){

	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)as_udp_pkt;

    fentry->magic = EIMAGIC;
    fentry->entry_size = dlen+CH_MMAP_FILE_ENTRY_HEADER_LEN;
    fentry->packet_type = packet_type;
    fentry->session_direct = 0;
    fentry->protocol_id = protocol_id;
    fentry->session_id = 0;
    fentry->time = as_udp_pkt->time;
    fentry->src_ip = as_pkt->src_ip; 
    fentry->dst_ip = as_pkt->dst_ip;
    fentry->src_port = as_pkt->src_port;
    fentry->dst_port = as_pkt->dst_port;

    fentry->data_offset = 0;
    fentry->data_len = dlen;
    fentry->data = data;
}

int ch_udp_data_write(void *udp_data,void *data,size_t dlen,uint8_t packet_type,uint32_t protocol_id){

    int rc;

	ch_assemble_t *as;

    ch_mmap_file_entry_t fentry;

     ch_assemble_packet_udp_t *as_udp_pkt = (ch_assemble_packet_udp_t*)udp_data;

	 as = (ch_assemble_t*)as_udp_pkt->priv_data;

	 if(data == NULL || dlen == 0 || as == NULL)
		 return PARSE_RETURN_DISCARD;

    _app_udp_mmap_file_entry_init(&fentry,as_udp_pkt,data,dlen,packet_type,protocol_id);


    for(;;){

        rc = ch_mmap_file_format_put(as->mmfmt,&fentry);
        if(rc == 0){
            return PARSE_RETURN_DONE;
        }
        if(rc == -2){

            ch_log(CH_LOG_ERR,"The data is too large:%lu,discard it!",dlen);
            /*write a close packet*/
#if 0
			_app_mmap_file_entry_init(&fentry,ass,NULL,0,PACKET_TYPE_CLOSE,protocol_id);
            
            while(ch_mmap_file_format_put(ass->as->mmfmt,&fentry)){
                usleep(5);
            }
#endif

            return PARSE_RETURN_DISCARD;
        }

        ch_log(CH_LOG_DEBUG,"The mmap file:[%s] is full,sleep and try again!",
			as->mmfmt->mm->fname);
        usleep(5);
    }

}

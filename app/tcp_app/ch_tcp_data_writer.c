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
#include "ch_shm_format.h"
#include "ch_tcp_record.h"
#include "ch_app_context.h"

int ch_tcp_data_write(ch_shm_format_t *fmt,ch_shm_record_t *rcd,int proto_id){

    int rc;
	ch_tcp_record_t *tcp_rcd = (ch_tcp_record_t*)rcd;
	tcp_rcd->protocol_id = proto_id;

    for(;;){

        rc = ch_shm_format_put(fmt,rcd);
        if(rc == 0){
            return PARSE_RETURN_DONE;
        }
        if(rc == -2){

            ch_log(CH_LOG_ERR,"The data is too large:%lu,discard it!",rcd->data_len);
            /*write a close packet*/
			tcp_rcd->packet_type = PACKET_TYPE_CLOSE;
			rcd->data = NULL;
			rcd->data_len = 0;
			rcd->data_offset = 0;
			rcd->record_size = CH_TCP_RECORD_HEADER_SIZE;

            
            while(ch_shm_format_put(fmt,rcd)){
                usleep(5);
            }

            return PARSE_RETURN_DISCARD;
        }

        ch_log(CH_LOG_WARN,"The shm is full,sleep and try again!");

        usleep(5);
    }

}

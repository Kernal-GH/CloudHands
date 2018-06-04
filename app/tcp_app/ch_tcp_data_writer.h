/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_data_writer.h
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

#ifndef CH_TCP_DATA_WRITER_H
#define CH_TCP_DATA_WRITER_H

#include <stdint.h>

extern int ch_tcp_data_write(void *tcp_session,void *data,size_t dlen,uint8_t packet_type,uint32_t protocol_id);

#endif /*CH_TCP_DATA_WRITER_H*/

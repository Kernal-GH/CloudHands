/*
 *
 *      Filename: ch_udp_data_writer.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-16 19:10:35
 * Last Modified: 2016-10-16 19:10:35
 */

#ifndef CH_UDP_DATA_WRITER_H
#define CH_UDP_DATA_WRITER_H

#include <stdint.h>

extern int ch_udp_data_write(void *udp_data,void *data,size_t dlen,uint8_t packet_type,uint32_t protocol_id);

#endif /* CH_UDP_DATA_WRITER_H */

/*
 *
 *      Filename: ch_tftp_packet.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-14 10:11:48
 * Last Modified: 2018-09-14 10:11:48
 */

#ifndef CH_TFTP_PACKET_H
#define CH_TFTP_PACKET_H

#include <stdint.h>

#define OP_RRQ   (uint16_t)1
#define OP_WRQ   (uint16_t)2
#define OP_DATA  (uint16_t)3
#define OP_ACK   (uint16_t)4
#define OP_ERROR (uint16_t)5

#define DATA_SIZE 512

#define READ_UINT16(p) ((uint16_t)                       \
                      ((uint16_t)*((const uint8_t *)(p)+0)<<8|  \
                      (uint16_t)*((const uint8_t *)(p)+1)<<0))

#define READ_SKIP(p,sz) (void*)((p)+(sz))
#define READ_SKIP_STR(p,v) (void*)((v)==NULL?p:p+strlen(v)+1)

#endif /*CH_TFTP_PACKET_H*/

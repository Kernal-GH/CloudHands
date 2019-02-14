/*
 *
 *      Filename: ch_mysql_packet.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-10-30 16:42:21
 * Last Modified: 2018-10-30 16:42:21
 */

#ifndef CH_MYSQL_PACKET_H
#define CH_MYSQL_PACKET_H

typedef struct ch_mysql_packet_t ch_mysql_packet_t;

#include <stdint.h>

struct ch_mysql_packet_t {


};

#define BVMAKE(dt,i) (*((uint8_t*)(dt+i)))

static inline int ch_mysql_packet_parse(ch_mysql_packet_t *mpkt,void *data,size_t dlen){

	if(data == NULL || dlen<4){
	
		return -1;
	}

	mpkt->plen = (uint32_t)(BVMAKE(data,0)|(BVMAKE(data,1)<<8)|(BVMAKE(data,2)<<16));

	if(mpkt->plen!=dlen-4){
	
		return -1;
	}

	mpkt->seq = BVMAKE(data,3);
	mpkt->data = data+4;


	return 0;
}


#endif /*CH_MYSQL_PACKET_H*/

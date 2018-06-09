/*
 *
 *      Filename: ch_stat_obj.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-04 15:29:54
 * Last Modified: 2018-06-04 15:29:54
 */

#ifndef CH_STAT_OBJ_H
#define CH_STAT_OBJ_H

typedef struct ch_stat_obj_t ch_stat_obj_t;
typedef struct ch_stat_obj_hdr_t ch_stat_obj_hdr_t;

#include <stdio.h>
#include "ch_stat_entry.h"
#include "ch_data_output.h"
#pragma pack(push,1)
struct ch_stat_obj_hdr_t {

	uint32_t obj_type;
	uint32_t entry_num;
	uint32_t entry_size;
};
#pragma pack(pop)

struct ch_stat_obj_t {

	ch_stat_obj_hdr_t *obj_hdr;

	ch_stat_entry_t *stat_entris;

};

#define ch_stat_obj_entry_find(stat_obj,index) ((stat_obj)->obj_hdr->entry_num<=(index)?NULL:(stat_obj)->stat_entris+(index))

extern void ch_stat_obj_init(ch_stat_obj_t *stat_obj,void *addr,uint32_t obj_type,uint32_t entry_num);

extern void ch_stat_obj_load(ch_stat_obj_t *stat_obj,void *addr);

extern void ch_stat_obj_reset(ch_stat_obj_t *stat_obj);

extern void ch_stat_obj_handle(ch_stat_obj_t *stat_obj,uint32_t index,uint64_t pkt_size);

extern void ch_stat_obj_dump(ch_stat_obj_t *stat_obj,uint32_t n,FILE *fp);

#include "ch_stat_pool.h"
static inline const char * ch_stat_obj_type_name(ch_stat_obj_t *stat_obj){

	const char *name = "";

	switch(stat_obj->obj_hdr->obj_type){
	
	case STAT_ALL:
		name = "total";
		break;

	case STAT_TCP:
		name = "tcp";
		break;
	
	case STAT_UDP:
		name = "udp";
		break;

	case STAT_ICMP:
		name = "icmp";
		break;

	case STAT_ARP:
		name = "arp";
		break;

	case STAT_OTHER:
		name = "other";
		break;
	default:
		name = "unknown";
		break;
	}

	return name;
}

extern ssize_t ch_stat_obj_out(ch_stat_obj_t *stat_obj,ch_data_output_t *dout,int type,uint32_t n);

#endif /*CH_STAT_OBJ_H*/

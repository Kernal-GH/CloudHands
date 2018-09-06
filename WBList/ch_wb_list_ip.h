/*
 *
 *      Filename: ch_wb_list_ip.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-04 11:02:20
 * Last Modified: 2018-09-04 11:02:20
 */

#ifndef CH_WB_LIST_IP_H
#define CH_WB_LIST_IP_H

typedef struct ch_wb_list_ip_entry_t ch_wb_list_ip_entry_t;
typedef struct ch_wb_list_ip_add_value_t ch_wb_list_ip_add_value_t;
typedef struct ch_wb_list_ip_match_value_t ch_wb_list_ip_match_value_t;

#include "ch_wb_list.h"

#pragma pack(push,1)
struct ch_wb_list_ip_entry_t {
	ch_wb_list_entry_t entry;
	uint32_t netmask;
	uint32_t ip_start;
	uint32_t ip_end;

	uint16_t port_start;
	uint16_t port_end;
};
#pragma pack(pop)

struct ch_wb_list_ip_add_value_t {

	uint32_t netmask;
	uint32_t ip_start;
	uint32_t ip_end;
	uint16_t port_start;
	uint16_t port_end;
};

struct ch_wb_list_ip_match_value_t {

	uint32_t ip;
	uint16_t port;
};

extern int ch_wb_list_ip_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize);



#endif /*CH_WB_LIST_IP_H*/

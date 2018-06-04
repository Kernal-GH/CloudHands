/*
 *
 *      Filename: ch_black_white_list.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-17 19:06:33
 * Last Modified: 2017-11-17 19:06:33
 */

#ifndef CH_BLACK_WHITE_LIST_H
#define CH_BLACK_WHITE_LIST_H

#include "ch_list.h"
#include <apr_pools.h>

typedef struct ch_bw_entry_t ch_bw_entry_t;
typedef struct ch_black_white_list_t ch_black_white_list_t;

struct ch_bw_entry_t {

	struct ch_list_head node; 
	uint32_t ip_start;
	uint32_t ip_end;
	uint16_t port_start;
	uint16_t port_end;
};

struct ch_black_white_list_t {

	struct ch_list_head entries;

	apr_pool_t *mp;

};


extern ch_black_white_list_t* ch_black_white_list_load(apr_pool_t *mp,const char *fname);

extern int ch_black_white_list_match(ch_black_white_list_t *bwl,uint32_t ip,uint16_t port);

#endif /*CH_BLACK_WHITE_LIST_H*/

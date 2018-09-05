/*
 *
 *      Filename: ch_wb_list_str.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-05 15:07:28
 * Last Modified: 2018-09-05 15:07:28
 */

#ifndef CH_WB_LIST_STR_H
#define CH_WB_LIST_STR_H

typedef struct ch_wb_list_str_entry_t ch_wb_list_str_entry_t;

#include "ch_wb_list.h"

#pragma pack(push,1)
struct ch_wb_list_str_entry_t {
	ch_wb_list_entry_t entry;
	char value[0];
};
#pragma pack(pop)

extern int ch_wb_list_str_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize,size_t max_str_len);

#endif /*CH_WB_LIST_STR_H*/

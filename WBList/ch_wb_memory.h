/*
 *
 *      Filename: ch_wb_memory.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-03 15:53:20
 * Last Modified: 2018-09-03 15:53:20
 */

#ifndef CH_WB_MEMORY_H
#define CH_WB_MEMORY_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct ch_wb_memory_t ch_wb_memory_t;

struct ch_wb_memory_t {

	int fd;
	void *start;
	void *pos;
	void *end;
};

#define CH_WB_MEMORY_FULL(wbm,size) ((wbm)->pos+(size)>=(wbm)->end)
#define CH_WB_MEMORY_UPDATE(wbm,size) ((wbm)->pos=(wbm)->pos+(size))
#define CH_WB_MEMORY_SIZE(wbm) (size_t)((wbm)->end-(wbm)->start)

extern int ch_wb_memory_init(ch_wb_memory_t *wbm,const char *fname,size_t msize);

extern void ch_wb_memory_fin(ch_wb_memory_t *wbm);

extern void *ch_wb_memory_alloc(ch_wb_memory_t *wbm,size_t size);

extern void *ch_wb_memory_calloc(ch_wb_memory_t *wbm,size_t size);

#endif /*CH_WB_MEMORY_H*/

/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_fragment.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月15日 15时19分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_FRAGMENT_H
#define CH_ASSEMBLE_FRAGMENT_H


typedef struct ch_assemble_fragment_t ch_assemble_fragment_t;
typedef struct ch_data_fragment_t ch_data_fragment_t;

#include <stdlib.h>
#include "ch_ring.h"
#include "ch_memory.h"

struct ch_data_fragment_t {
    CH_RING_ENTRY(ch_data_fragment_t) link;
    void *data;
    uint32_t offset;
    uint32_t len;
};

struct ch_assemble_fragment_t {
    
    CH_RING_ENTRY(ch_data_fragment_t) frags_head;
	ch_memory_t *mm;
};

static inline void ch_assemble_fragment_info_get(ch_assemble_fragment_t *as_frag,size_t *df_count,size_t *dfs_len){
    
    *df_count = 0;
    *dfs_len = 0;
    ch_data_fragment_t *df;

    CH_RING_FOREACH(df,&as_frag->frags_head,ch_data_fragment_t,link){

        *df_count =*df_count+1;
        *dfs_len = *dfs_len +df->len;
    }

}

static inline void ch_assemble_data_fragment_free(ch_assemble_fragment_t *as_frag,ch_data_fragment_t *df){
    
    if(df){
        
		ch_memory_free(as_frag->mm,(void*)df);
    }
}

static inline int _is_assemble_fragment_empty(ch_assemble_fragment_t *as_frag){

    return CH_RING_EMPTY(&as_frag->frags_head,ch_data_fragment_t,link);
}

extern void ch_assemble_fragment_init(ch_assemble_fragment_t *as_frag,ch_memory_t *mm);


extern void ch_assemble_fragment_fin(ch_assemble_fragment_t *as_frag);


extern int ch_assemble_fragment_push(ch_assemble_fragment_t *as_frag,void *data,uint32_t dlen,uint32_t offset);



extern ch_data_fragment_t* ch_assemble_fragment_pop(ch_assemble_fragment_t *as_frag,uint32_t last_offset);

#define DATA_FRAGMENT_DUMP(df,fp) do{\
	fprintf(fp,"Dump The Data Fragment informations:\n");\
	fprintf(fp,"Offset:%lu,Len:%lu\n",(unsigned long)df->offset,(unsigned long)df->len);\
}while(0)

static inline void ch_assemble_fragment_dump(ch_assemble_fragment_t *as_frag,FILE *fp){

    ch_data_fragment_t *df;

	fprintf(fp,"Dump The Assemble Fragment Informations:\n");

    CH_RING_FOREACH(df,&as_frag->frags_head,ch_data_fragment_t,link){

		DATA_FRAGMENT_DUMP(df,fp);
    }
}

#endif /*CH_ASSEMBLE_FRAGMENT_H*/

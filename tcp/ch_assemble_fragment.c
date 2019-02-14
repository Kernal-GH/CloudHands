/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_fragment.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月15日 16时03分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_log.h"
#include "ch_assemble_fragment.h"

void ch_assemble_fragment_init(ch_assemble_fragment_t *as_frag,ch_memory_t *mm){

    CH_RING_INIT(&as_frag->frags_head,ch_data_fragment_t,link);

	as_frag->mm = mm;
}

void ch_assemble_fragment_fin(ch_assemble_fragment_t *as_frag){

    ch_data_fragment_t *df,*df_tmp;

    CH_RING_FOREACH_SAFE(df,df_tmp,&as_frag->frags_head,ch_data_fragment_t,link){

        CH_RING_REMOVE(df,link);

		ch_memory_free(as_frag->mm,(void*)df);
	}

}

static inline int _after(ch_data_fragment_t *a,ch_data_fragment_t *b){

    return a->offset>=b->offset;
}

static inline int _before(ch_data_fragment_t *a,ch_data_fragment_t *b){

    return a->offset+a->len<=b->offset;
}

static inline int _before_empty(ch_assemble_fragment_t *as_frag,ch_data_fragment_t *df){

    return CH_RING_PREV(df,link) == CH_RING_SENTINEL(&as_frag->frags_head,ch_data_fragment_t,link);
}

static void _data_fragment_neighbors_get(ch_assemble_fragment_t *as_frag,ch_data_fragment_t *cur_df,
        ch_data_fragment_t **before,ch_data_fragment_t **after)
{
    ch_data_fragment_t *df;

    *before = NULL;
    *after = NULL;

    CH_RING_FOREACH(df,&as_frag->frags_head,ch_data_fragment_t,link){

        if(_after(df,cur_df)){

            *after = df;
            *before = _before_empty(as_frag,df)?NULL:CH_RING_PREV(df,link);
            return;
        }
    }

    if(!CH_RING_EMPTY(&as_frag->frags_head,ch_data_fragment_t,link)){
        *before = CH_RING_LAST(&as_frag->frags_head);
    }

}

static void _data_fragment_trim(ch_assemble_fragment_t *as_frag ch_unused,ch_data_fragment_t *df,
        ch_data_fragment_t *before,ch_data_fragment_t *after){
   
    uint32_t diff;

    if(df->data == NULL)
    {
        /*covered!||end!*/
        return;
    }

    if(before){
        if(before->offset+before->len>df->offset){
            diff = before->offset+before->len-df->offset;
            df->data = df->data+diff;
            df->len = df->len-diff;
            df->offset = df->offset+diff;
        }
    }

    if(after){
        if(df->offset+df->len>after->offset){
            diff = df->offset+df->len-after->offset;
            df->len = df->len-diff;
        }
    }
    /*ok!*/
}

static inline int _is_data_fragments_mergable(ch_data_fragment_t *a,ch_data_fragment_t *b){

    return a->offset+a->len == b->offset;
}

static ch_data_fragment_t *  _data_fragments_merge(ch_assemble_fragment_t *as_frag,ch_data_fragment_t *df,
        ch_data_fragment_t *bfd,ch_data_fragment_t *afd){

    uint32_t tsize = df->len;
    uint32_t offset = df->offset;

    int ma = 0,mb = 0;
    ch_data_fragment_t *mdf = NULL;
    void *p;

    if(bfd&&_is_data_fragments_mergable(bfd,df)){
        ma = 1;
        offset = bfd->offset;
        tsize+=bfd->len;
    }
    
    if(afd&&_is_data_fragments_mergable(df,afd)){
        tsize+=afd->len;
        mb = 1;
    }

    /*create */
    mdf = (ch_data_fragment_t*)ch_memory_alloc(as_frag->mm,tsize+sizeof(ch_data_fragment_t));
		
    if(!mdf){
        ch_log(CH_LOG_ERR,"No memory used to create data fragment!");
        return NULL;
    }
    
    mdf->data =(void*)(mdf+1);
    mdf->len = tsize;
    mdf->offset = offset;

    /*copy data*/
    p = mdf->data;
    if(ma){
        memcpy(p,bfd->data,bfd->len);
        p+=bfd->len;
    }
    
    memcpy(p,df->data,df->len);
    p+=df->len;

    if(mb){
        memcpy(p,afd->data,afd->len);
    }

    /*insert*/
    if(bfd){
        CH_RING_INSERT_AFTER(bfd,mdf,link); 
    }else if(afd){
        
        CH_RING_INSERT_BEFORE(afd,mdf,link);
    }else{
        CH_RING_INSERT_HEAD(&as_frag->frags_head,mdf,ch_data_fragment_t,link);
    }

    /*delete*/
    if(bfd&&ma){
       CH_RING_REMOVE(bfd,link);

	   ch_memory_free(as_frag->mm,(void*)bfd);
    }

    if(afd&&mb){
       CH_RING_REMOVE(afd,link);
	   ch_memory_free(as_frag->mm,(void*)afd); 
    }

    /*ok*/
    return mdf;
}

static inline int _data_fragment_covered(ch_data_fragment_t *a,ch_data_fragment_t *b){

    return (a->offset>=b->offset)&&(b->offset+b->len>=a->offset+a->len);
}

static void _data_fragment_covered_free(ch_assemble_fragment_t *as_frag,ch_data_fragment_t *df,int * is_covered){

    ch_data_fragment_t *cur_df,*cur_tmp_df;

    CH_RING_FOREACH_SAFE(cur_df,cur_tmp_df,&as_frag->frags_head,ch_data_fragment_t,link){

        if(_data_fragment_covered(cur_df,df)){

            CH_RING_REMOVE(cur_df,link);
            
			ch_memory_free(as_frag->mm,(void*)cur_df);

        }else if(_data_fragment_covered(df,cur_df)){
            *is_covered = 1;
            break;
        }
    }
}

int ch_assemble_fragment_push(ch_assemble_fragment_t *as_frag,void *data,uint32_t dlen,uint32_t offset){
   
    int is_covered = 0;

    ch_data_fragment_t tmp_df;
    tmp_df.data = data;
    tmp_df.len = dlen;
    tmp_df.offset = offset;

    ch_data_fragment_t *before,*after;

    _data_fragment_covered_free(as_frag,&tmp_df,&is_covered);

    if(is_covered){
        return 0;
    }

    /*find neighbors*/
    _data_fragment_neighbors_get(as_frag,&tmp_df,&before,&after);

    /*Trim the data covered by before/after data fragments*/
    _data_fragment_trim(as_frag,&tmp_df,before,after);

    /*covered completely*/
    if(tmp_df.data == NULL||tmp_df.len == 0){
        /*do nothing*/
        return 0;
    }

    /*merge data fragments*/
    if(_data_fragments_merge(as_frag,&tmp_df,before,after) == NULL){
        return -1;
    }

    /*ok*/
    return 0;

}

static inline int _is_covered_all(ch_data_fragment_t *df,uint32_t last_offset){

    return df->offset+df->len<=last_offset;
}

static inline int _is_covered_partial(ch_data_fragment_t *df,uint32_t last_offset){
    return df->offset+df->len>last_offset&&df->offset<last_offset;
}

static void _data_fragments_covered_free(ch_assemble_fragment_t *as_frag,uint32_t last_offset){

    ch_data_fragment_t *df,*df_tmp;
    uint32_t diff;

    CH_RING_FOREACH_SAFE(df,df_tmp,&as_frag->frags_head,ch_data_fragment_t,link){
        
        if(_is_covered_all(df,last_offset)){
            CH_RING_REMOVE(df,link);
            ch_memory_free(as_frag->mm,(void*)df);
            continue;
        }

        if(_is_covered_partial(df,last_offset)){

            diff = last_offset-df->offset;
            df->len = df->len-diff;
            df->offset+=diff;
            df->data+=diff;
        }

        break;
    }
}

ch_data_fragment_t* ch_assemble_fragment_pop(ch_assemble_fragment_t *as_frag,uint32_t last_offset){

    ch_data_fragment_t *df = NULL;

    if(CH_RING_EMPTY(&as_frag->frags_head,ch_data_fragment_t,link))
        return NULL;

    _data_fragments_covered_free(as_frag,last_offset);

    if(CH_RING_EMPTY(&as_frag->frags_head,ch_data_fragment_t,link))
        return NULL;

    df = CH_RING_FIRST(&as_frag->frags_head);
    if(last_offset == df->offset){
        CH_RING_REMOVE(df,link);
        return df;
    }

    return NULL;
}


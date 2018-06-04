/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月18日 14时52分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <apr_strings.h>
#include "ch_log.h"
#include "ch_util.h"
#include "ch_assemble.h"

static inline const char * _get_mmap_file_name(apr_pool_t *mp,const char *dir,const char *app_name,int as_id){

    return apr_psprintf(mp,"%s/assemble_session_data_%s_%d",dir,app_name,as_id);
}

int ch_assemble_init(ch_context_t *context,ch_assemble_t *as,int as_id,const char *fdir,const char *app_name,
	uint64_t fsize,uint64_t fentry_size,
        void (*do_assemble)(ch_assemble_t *as,struct rte_mbuf *m)
        ){

    const char *fname;

    fname = _get_mmap_file_name(context->mp,fdir,app_name,as_id);
    as->assemble_id = as_id;
    as->mmfmt = ch_mmap_file_format_create(context->mp,fname,fsize,fentry_size,1);
    as->cur_id = 1;
    as->as_task = NULL;
    if(as->mmfmt == NULL){
        ch_log(CH_LOG_ERR,"Create mmap file format failed for assemble :%d",as_id);
        return -1;
    }
    as->do_assemble = do_assemble;
    return 0;
}

void ch_assemble_fin(ch_assemble_t *as){

    ch_mmap_file_format_destroy(as->mmfmt);
}

void ch_assemble_do(ch_assemble_t *as,struct rte_mbuf *m){

    as->do_assemble(as,m);
}


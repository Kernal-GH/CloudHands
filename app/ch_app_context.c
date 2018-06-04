/*
 * =====================================================================================
 *
 *       Filename:  ch_app_context.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月11日 14时11分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_app_context.h"
#include "ch_log.h"

void ch_app_context_init(ch_app_context_t *app_context,ch_pool_t *mp){


	app_context->mp = mp;
    app_context->apps = ch_array_make(app_context->mp,16,sizeof(ch_app_t*));

}


void ch_app_context_register(ch_app_context_t *app_context,ch_app_t *app){

	app->app_context = app_context;

    *(ch_app_t**)ch_array_push(app_context->apps) = app;
}


ch_app_t * ch_app_context_recognize_by_port(ch_app_context_t *app_context,uint16_t src_port,uint16_t dst_port){
    
    ch_app_t **apps,*app;
    int i;
	int rc;

    apps = (ch_app_t **)app_context->apps->elts;

    for(i=0;i<app_context->apps->nelts;i++){
        
        app = apps[i];

        if(app->app_recognize_by_port){
            
            rc = app->app_recognize_by_port(app,src_port,dst_port,NULL);

            if(rc){
                return app;
            }
        }
    }

    /*no found*/
    return NULL;
}

ch_app_t * ch_app_context_recognize_by_content(ch_app_context_t *app_context,void *data,size_t dlen){
    
    ch_app_t **apps,*app;
    int i;
	int rc;

    apps = (ch_app_t **)app_context->apps->elts;

    for(i=0;i<app_context->apps->nelts;i++){
        
        app = apps[i];

        if(app->app_recognize_by_content){
            
            rc = app->app_recognize_by_content(app,data,dlen,NULL);

            if(rc){
                return app;
            }
        }
    }

    /*no found*/
    return NULL;
}


int ch_app_context_content_process(ch_app_context_t *app_context,ch_app_t *app,ch_shm_format_t *fmt,ch_shm_record_t *rcd){

    if(app == NULL){
    
        ch_log(CH_LOG_ERR,"No app process this data!");
        return PARSE_RETURN_DISCARD;
    }

    return app->app_content_process(app,fmt,rcd); 
}

void ch_app_context_content_flush(ch_app_context_t *app_context,ch_app_t *app,ch_shm_format_t *fmt,ch_shm_record_t *rcd){

    app->app_content_flush(app,fmt,rcd);
}


void ch_app_context_content_close(ch_app_context_t *app_context,ch_app_t *app,ch_shm_format_t *fmt,ch_shm_record_t *rcd){

    app->app_content_close(app,fmt,rcd);
}

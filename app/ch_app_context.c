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

ch_app_context_t * ch_app_context_create(ch_context_t *context){

    ch_app_context_t *app_context = (ch_app_context_t*)apr_palloc(context->mp,sizeof(ch_app_context_t));

    app_context->context = context;

    app_context->apps = apr_array_make(context->mp,16,sizeof(ch_app_t*));

    return app_context;
}


void ch_app_context_register(ch_app_context_t *app_context,ch_app_t *app){

    *(ch_app_t**)apr_array_push(app_context->apps) = app;
}


ch_app_t * _app_context_find_by_common_port(ch_app_context_t *app_context,uint16_t src_port,uint16_t dst_port){
    
    ch_app_t **apps,*app;
    int i;
    uint32_t common_port;

    apps = (ch_app_t **)app_context->apps->elts;

    for(i=0;i<app_context->apps->nelts;i++){
        
        app = apps[i];
        common_port = app->common_port;

        if(common_port == src_port || common_port == dst_port){
            
            /*Found*/
            return app;
        }
    }

    /*no found*/
    return NULL;
}


int  ch_app_context_recognize_by_port(ch_app_context_t *app_context,uint16_t src_port,uint16_t dst_port){
    
    ch_app_t **apps,*app;
    int i;
    int rc;

    uint32_t common_port;

    apps = (ch_app_t **)app_context->apps->elts;

    for(i=0;i<app_context->apps->nelts;i++){

        app = apps[i];
        common_port = app->common_port;

        if(common_port == src_port || common_port == dst_port){
            
            return 1;
        }

        if(app->app_recognize_by_port){
            
            rc = app->app_recognize_by_port(app,src_port,dst_port,app->priv_data);

            if(rc){
                return 1;
            }
        }
    }

    return 0;

}


int ch_app_context_content_parse(ch_app_context_t *app_context,ch_assemble_session_t *ass,void *data,size_t dlen){

    ch_app_t *app = ass->app;
    ch_app_t **apps;

    int rc,i;

    if(app == NULL){
        /*try to find app by common port*/
        app = _app_context_find_by_common_port(app_context,
                ch_assemble_session_srcport_get(ass),
                ch_assemble_session_dstport_get(ass));
    }

    if(app){
        return app->app_content_parse(app,ass,data,dlen,app->priv_data);
    }

    /*call all apps to parse content*/
    apps = (ch_app_t **)app_context->apps->elts;

    for(i = 0; i<app_context->apps->nelts; i++){

        app = apps[i];

        rc = app->app_content_parse(app,ass,data,dlen,app->priv_data);
        
        if(rc!=PARSE_RETURN_DISCARD){

            ass->app = app;
            return rc;
        }

    }
    
    return PARSE_RETURN_DISCARD;
}


void ch_app_context_content_flush(ch_app_context_t *app_context,ch_assemble_session_t *ass){

}


void ch_app_context_content_close(ch_app_context_t *app_context,ch_assemble_session_t *ass){

    
}

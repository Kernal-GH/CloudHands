/*
 *
 *      Filename: ch_tcp_app_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 14:13:07
 * Last Modified: 2018-07-30 17:05:07
 */

#include "ch_tcp_app_pool.h"
#include "ch_log.h"
#include "ch_http.h"
#include "ch_mail.h"
#include "ch_smon.h"

#define process_register_retv(rc,proto) do { \
	if(rc){\
		ch_log(CH_LOG_ERR,"register proto:%s failed!",proto);\
		return -1;\
	}\
}while(0)


static int _register_all_apps(ch_tcp_app_pool_t *ta_pool,ch_tcp_app_context_t *tcontext){

	int rc;

	if(tcontext->http_is_on){
		rc = ch_http_init(ta_pool,tcontext->http_cfname);
		process_register_retv(rc,"http");
	}

	if(tcontext->mail_is_on){
		rc = ch_mail_init(ta_pool,tcontext->mail_cfname);
		process_register_retv(rc,"mail");
	}

	if(tcontext->smon_is_on){
		rc = ch_smon_init(ta_pool,tcontext->smon_cfname);
		process_register_retv(rc,"smon");
	}

    return 0;
}


ch_tcp_app_pool_t * ch_tcp_app_pool_create(ch_pool_t *mp,const char *cfname){

	ch_tcp_app_pool_t * ta_pool = (ch_tcp_app_pool_t*)ch_palloc(mp,sizeof(*ta_pool));

	ta_pool->mp = mp;
	ta_pool->tcontext = ch_tcp_app_context_create(mp,cfname);

	if(ta_pool->tcontext == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot load tcp app config:%s",cfname);
		return NULL;
	}

	ta_pool->apps = ch_array_make(mp,16,sizeof(ch_tcp_app_t*));

	if(_register_all_apps(ta_pool,ta_pool->tcontext)){
	
		return NULL;
	}

	return ta_pool;
}


ch_tcp_app_t * ch_tcp_app_find_by_port(ch_tcp_app_pool_t *ta_pool,ch_packet_tcp_t *tcp_pkt){

    ch_tcp_app_t **apps,*app;
    int i;
	int rc;

    apps = (ch_tcp_app_t **)ta_pool->apps->elts;

    for(i=0;i<ta_pool->apps->nelts;i++){
        
        app = apps[i];

        if(app->is_accept_by_port){
            
            rc = app->is_accept_by_port(app,tcp_pkt);

            if(rc){
                return app;
            }
        }
    }

    /*no found*/
    return NULL;

}

ch_tcp_app_t * ch_tcp_app_find_by_content(ch_tcp_app_pool_t *ta_pool,ch_packet_tcp_t *tcp_pkt,void *data,size_t dlen){

    ch_tcp_app_t **apps,*app;
    int i;
	int rc;

    apps = (ch_tcp_app_t **)ta_pool->apps->elts;

    for(i=0;i<ta_pool->apps->nelts;i++){
        
        app = apps[i];

        if(app->is_accept_by_content){
            
            rc = app->is_accept_by_content(app,tcp_pkt,data,dlen);

            if(rc){
                return app;
            }
        }
    }

    /*no found*/
    return NULL;

}

int ch_tcp_app_request_content_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
	ch_tcp_session_t *tsession,void *data,size_t dlen){

    if(app == NULL || app->request_content_parse == NULL||tsession->sentry == NULL){
    
        ch_log(CH_LOG_ERR,"No app  and parse method to handle this data!");
        return PARSE_BREAK;
    }

    return app->request_content_parse(app,pstore,tsession,data,dlen); 

}

int ch_tcp_app_response_content_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
	 ch_tcp_session_t *tsession,void *data,size_t dlen){

    if(app == NULL || app->response_content_parse == NULL||tsession->sentry == NULL){
    
        ch_log(CH_LOG_ERR,"No app and response parse method to handle this data!");
        return PARSE_BREAK;
    }

    return app->response_content_parse(app,pstore,tsession,data,dlen); 

}

void* ch_tcp_app_session_entry_create(ch_tcp_app_t *app,ch_proto_session_store_t *pstore){

    if(app == NULL || app->proto_session_entry_create == NULL){
    
        ch_log(CH_LOG_ERR,"No app and session entry create method to handle this data!");
        return NULL;

    }else {

		return app->proto_session_entry_create(app,pstore); 
	}

}


void ch_tcp_app_session_entry_clean(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
	 ch_tcp_session_t *tsession){

    if(tsession == NULL||tsession->sentry == NULL)
        return;

    if(app == NULL || app->proto_session_entry_clean == NULL){
    
        ch_log(CH_LOG_ERR,"No app and session entry clean method to handle this data!");
    }else {

		app->proto_session_entry_clean(app,pstore,tsession); 
	}

	tsession->sentry = NULL;

}


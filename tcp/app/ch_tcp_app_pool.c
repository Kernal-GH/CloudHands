/*
 *
 *      Filename: ch_tcp_app_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 14:13:07
 * Last Modified: 2018-07-13 10:05:36
 */

#include "ch_tcp_app_pool.h"
#include "ch_log.h"
#include "ch_http.h"
#include "ch_smtp.h"
#include "ch_pop3.h"
#include "ch_imap.h"
#include "ch_telnet.h"
#include "ch_ftp.h"

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

	if(tcontext->smtp_is_on){
		rc = ch_smtp_init(ta_pool,tcontext->smtp_cfname);
		process_register_retv(rc,"smtp");
	}

	if(tcontext->pop3_is_on){
		rc = ch_pop3_init(ta_pool,tcontext->pop3_cfname);
		process_register_retv(rc,"pop3");
	}
	
	if(tcontext->imap_is_on){
		rc = ch_imap_init(ta_pool,tcontext->imap_cfname);
		process_register_retv(rc,"imap");
	}

	if(tcontext->telnet_is_on){
		rc = ch_telnet_init(ta_pool,tcontext->telnet_cfname);
		process_register_retv(rc,"telnet");
	}

	if(tcontext->ftp_is_on){
		rc = ch_ftp_init(ta_pool,tcontext->ftp_cfname);
		process_register_retv(rc,"ftp");
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

int ch_tcp_app_request_content_process(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){

    if(app == NULL || app->request_content_process == NULL){
    
        ch_log(CH_LOG_ERR,"No app process and request method to handle this data!");
        return PARSE_RETURN_DISCARD;
    }

    return app->request_content_process(app,fmt,tsession,data,dlen); 

}

int ch_tcp_app_response_content_process(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	 ch_tcp_session_t *tsession,void *data,size_t dlen){

    if(app == NULL || app->response_content_process == NULL){
    
        ch_log(CH_LOG_ERR,"No app process and response method to handle this data!");
        return PARSE_RETURN_DISCARD;
    }

    return app->response_content_process(app,fmt,tsession,data,dlen); 

}

void ch_tcp_app_content_flush(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	 ch_tcp_session_t *tsession,void *data,size_t dlen){

    if(app == NULL || app->content_flush == NULL){
    
        ch_log(CH_LOG_ERR,"No app process and flush method to handle this data!");
    }else{
    
		app->content_flush(app,fmt,tsession,data,dlen);
	}
}

void ch_tcp_app_content_close(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	 ch_tcp_session_t *tsession,void *data,size_t dlen){

    if(app == NULL || app->content_close == NULL){
    
        ch_log(CH_LOG_ERR,"No app process and close method to handle this data!");
    }else {
		app->content_close(app,fmt,tsession,data,dlen); 
	}

}


/*
 *
 *      Filename: ch_http_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-27 19:05:36
 * Last Modified: 2018-05-17 11:12:52
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_http_context.h"
#include "ch_fpath.h"

static const char *cmd_http_reqbody_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->req_body_dir = p1;

	return NULL;
}

static const char *cmd_http_resbody_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->res_body_dir = p1;

	return NULL;
}


static const char *cmd_http_body_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid http header dir create type config item!";

	}

	hcontext->create_body_dir_type = rc;
	return NULL;
}

static const command_rec http_context_directives[] = {

    CH_INIT_TAKE1(
            "PHTTPReqBodyDir",
            cmd_http_reqbody_dir,
            NULL,
            0,
            "set the store path of http request body"
            ),

    CH_INIT_TAKE1(
            "PHTTPResBodyDir",
            cmd_http_resbody_dir,
            NULL,
            0,
            "set the store path of http response body"
            ),

    CH_INIT_TAKE1(
            "PHTTPBodyDirCreateType",
            cmd_http_body_dir_create_type,
            NULL,
            0,
            "set the type created of http body dir by time"
            ),

};

static inline void _http_context_init(ch_http_context_t *hcontext){

	hcontext->req_body_dir = "/tmp/http/reqBody";
	hcontext->res_body_dir = "/tmp/http/resBody";
	hcontext->create_body_dir_type = BY_MIN;
}

ch_http_context_t* ch_http_context_create(ch_pp_context_t *pcontext,const char *cfname){

	const char *msg;
    
	ch_http_context_t *hcontext = NULL;
	
	hcontext = (ch_http_context_t*)ch_palloc(pcontext->mp,sizeof(*hcontext));

	
	hcontext->pcontext = pcontext;
	
	_http_context_init(hcontext);

    msg = ch_process_command_config(http_context_directives,
		(void*)hcontext,pcontext->mp,pcontext->mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config http parser context error:%s",msg);
        return NULL;
    }

	return hcontext;
}


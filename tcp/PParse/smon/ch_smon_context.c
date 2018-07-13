/*
 *
 *      Filename: ch_smon_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-27 19:05:36
 * Last Modified: 2018-07-13 13:52:19
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_smon_context.h"
#include "ch_fpath.h"

static const char *cmd_smon_reqbody_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	ch_smon_context_t *scontext = (ch_smon_context_t*)_dcfg;

	scontext->req_body_dir = p1;

	return NULL;
}

static const char *cmd_smon_resbody_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	ch_smon_context_t *scontext = (ch_smon_context_t*)_dcfg;

	scontext->res_body_dir = p1;

	return NULL;
}


static const char *cmd_smon_body_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	ch_smon_context_t *scontext = (ch_smon_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid smon header dir create type config item!";

	}

	scontext->create_body_dir_type = rc;
	return NULL;
}

static const command_rec smon_context_directives[] = {

    CH_INIT_TAKE1(
            "PSMONReqBodyDir",
            cmd_smon_reqbody_dir,
            NULL,
            0,
            "set the store path of smon request body"
            ),

    CH_INIT_TAKE1(
            "PSMONResBodyDir",
            cmd_smon_resbody_dir,
            NULL,
            0,
            "set the store path of smon response body"
            ),

    CH_INIT_TAKE1(
            "PSMONBodyDirCreateType",
            cmd_smon_body_dir_create_type,
            NULL,
            0,
            "set the type created of smon body dir by time"
            ),

};

static inline void _smon_context_init(ch_smon_context_t *scontext){

	scontext->req_body_dir = "/tmp/smon/reqBody";
	scontext->res_body_dir = "/tmp/smon/resBody";
	scontext->create_body_dir_type = BY_MIN;
}

ch_smon_context_t* ch_smon_context_create(ch_pp_context_t *pcontext,const char *cfname){

	const char *msg;
    
	ch_smon_context_t *scontext = NULL;
	
	scontext = (ch_smon_context_t*)ch_palloc(pcontext->mp,sizeof(*scontext));

	
	scontext->pcontext = pcontext;
	
	_smon_context_init(scontext);

    msg = ch_process_command_config(smon_context_directives,
		(void*)scontext,pcontext->mp,pcontext->mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config smon parser context error:%s",msg);
        return NULL;
    }

	return scontext;
}


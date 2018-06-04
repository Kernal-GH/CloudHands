/*
 *
 *      Filename: ./protocols/mail/ch_mail_context.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 12:15:34
 * Last Modified: 2018-05-18 14:12:36
 */


#include "ch_mail_context.h"
#include "ch_log.h"
#include "ch_fpath.h"
#include "ch_config.h"

static const char *
cmd_mail_content_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1)
{
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->content_dir = p1;

	return NULL;
}

static const char *
cmd_mail_attach_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1)
{
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->attach_dir = p1;

	return NULL;
}

static const char *
cmd_mail_body_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc ;
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid mail header dir create type config item!";

	}

	mcontext->create_body_dir_type = rc;

	return NULL;
}

static const command_rec mail_context_directives[] = {

    CH_INIT_TAKE1(
            "PMAILContentDir",
            cmd_mail_content_dir,
            NULL,
            0,
            "set the store path for mail content"
            ),

    CH_INIT_TAKE1(
            "PMAILAttachDir",
            cmd_mail_attach_dir,
            NULL,
            0,
            "set the store path for mail attach"
            ),


    CH_INIT_TAKE1(
            "PMAILSaveDirCreateType",
            cmd_mail_body_dir_create_type,
            NULL,
            0,
            "set the type created for content and attachment by time"
            ),

};

static inline void _mail_context_init(ch_mail_context_t *mcontext)
{
	mcontext->content_dir = "/tmp/mail/content";
	mcontext->attach_dir = "/tmp/mail/attach";
	mcontext->create_body_dir_type = BY_MIN;
}


ch_mail_context_t *ch_mail_context_create(ch_pp_context_t *pcontext,const char *cfname)
{
	const char *err_msg;

    ch_mail_context_t *mcontext = NULL;
	
	mcontext = (ch_mail_context_t*)ch_palloc(pcontext->mp, sizeof(*mcontext));
	mcontext->pcontext = pcontext;

	_mail_context_init(mcontext);
    
	err_msg = ch_process_command_config(mail_context_directives,
		(void*)mcontext, pcontext->mp, pcontext->mp,cfname);

    /*config failed*/
    if (err_msg != NULL) {
        ch_log(CH_LOG_ERR, "Config mail parser context error:%s", err_msg);
        return NULL;
    }

    return mcontext;
}


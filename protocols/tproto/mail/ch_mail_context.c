/*
 *
 *      Filename: ./protocols/mail/ch_mail_context.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 12:15:34
 * Last Modified: 2016-10-28 02:06:22
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_fpath.h"
#include "ch_config.h"
#include "ch_mail_context.h"

static const char *
cmd_mail_profile_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;

	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->fstore_dir = p1;

	return NULL;
}

static const char *
cmd_mail_content_dir(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	/* unused */
	cmd = cmd;
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->content_dir = p1;

	return NULL;
}

static const char *
cmd_mail_attach_dir(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	/* unused */
	cmd = cmd;
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->attach_dir = p1;

	return NULL;
}

static const char *
cmd_mail_profile_store_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->fstore_buf_size = (size_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *
cmd_mail_profile_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc ;
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid mail header dir create type config item!";

	}

	mcontext->create_dir_type = rc;

	return NULL;
}

static const char *
cmd_mail_body_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc ;
	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid mail header dir create type config item!";

	}

	mcontext->create_body_dir_type = rc;

	return NULL;
}

static const char *
cmd_mail_hash_entries_limited(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	/* unused */
	cmd = cmd;
	char *end;

	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->n_entries_limit = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_mail_hash_table_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->tbl_size = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_mail_hash_table_entry_timeout(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->entry_timeout = (uint64_t)strtoul(p1,&end,10);


	return NULL;
}

static const char *cmd_mail_hash_table_cache_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_mail_context_t *mcontext = (ch_mail_context_t*)_dcfg;

	mcontext->n_caches_limits = (uint32_t)strtoul(p1,&end,10);


	return NULL;
}

static const command_rec mail_context_directives[] = {

    CH_INIT_TAKE1(
            "PMAILProfileDir",
            cmd_mail_profile_dir,
            NULL,
            0,
            "set the store path for mail profile"
            ),
    
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
            "PMAILProfileStoreSize",
            cmd_mail_profile_store_size,
            NULL,
            0,
            "set the size for profile store"
            ),

    CH_INIT_TAKE1(
            "PMAILProfileDirCreateType",
            cmd_mail_profile_dir_create_type,
            NULL,
            0,
            "set the type created for profile by time"
            ),

    CH_INIT_TAKE1(
            "PMAILSaveDirCreateType",
            cmd_mail_body_dir_create_type,
            NULL,
            0,
            "set the type created for content and attachment by time"
            ),

    CH_INIT_TAKE1(
            "PMAILHashEntriesLimited",
            cmd_mail_hash_entries_limited,
            NULL,
            0,
            "set the mail hash entries limited"
            ),

    CH_INIT_TAKE1(
            "PMAILHashTableSize",
            cmd_mail_hash_table_size,
            NULL,
            0,
            "set the mail hash table size"
            ),

    CH_INIT_TAKE1(
            "PMAILHashTableEntryTimeout",
            cmd_mail_hash_table_entry_timeout,
            NULL,
            0,
            "set the hash table entry timeout"
            ),

    CH_INIT_TAKE1(
            "PMAILHashTableCacheSize",
            cmd_mail_hash_table_cache_size,
            NULL,
            0,
            "set the mail hash table cache size"
            ),
};

static inline void _mail_context_init(ch_mail_context_t *mcontext)
{
	mcontext->fstore_dir = "/tmp/mail/profile";
	mcontext->content_dir = "/tmp/mail/content";
	mcontext->attach_dir = "/tmp/mail/attach";
	mcontext->fstore_buf_size = 64*1024;
	mcontext->create_dir_type = BY_HOUR;
	mcontext->create_body_dir_type = BY_MIN;
	mcontext->n_entries_limit = 1000000;
	mcontext->tbl_size = 1024;
	mcontext->entry_timeout = 5*60;
	mcontext->n_caches_limits = 1024;
}

static int 
ch_mail_context_init(ch_proto_context_t *pcontext,ch_mail_context_t *mcontext)
{

	const char *err_msg;

	mcontext->pcontext = pcontext;
	
	_mail_context_init(mcontext);

    err_msg = ch_process_command_config(mail_context_directives,
		(void*)mcontext, pcontext->mp, pcontext->mp,pcontext->mail_proto_cfile);

    /*config failed*/
    if (err_msg != NULL) {
        ch_log(CH_LOG_ERR, "Config mail parser context error:%s", err_msg);
        return -1;
    }

	return 0;
}

ch_mail_context_t *ch_mail_context_create(ch_proto_context_t *pcontext)
{

    ch_mail_context_t *mcontext = NULL;
	
	mcontext = (ch_mail_context_t*)apr_palloc(pcontext->mp, sizeof(*mcontext));
	if (mcontext == NULL) {
		ch_log(CH_LOG_ERR, "%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	if (ch_mail_context_init(pcontext, mcontext)) {
		return NULL;
	}

    return mcontext;
}


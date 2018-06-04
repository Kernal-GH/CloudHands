/*
 *
 *      Filename: ch_debug_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 22:07:10
 * Last Modified: 2016-12-06 00:43:14
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_debug_context.h"
#include "ch_fpath.h"

static const char *cmd_debug_header_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->fstore_dir = p1;

	return NULL;
}

static const char *cmd_debug_reqbody_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->req_body_dir = p1;

	return NULL;
}
static const char *cmd_debug_resbody_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->res_body_dir = p1;

	return NULL;
}

static const char *cmd_debug_file_store_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->fstore_buf_size = (size_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_debug_header_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc ;
	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid debug header dir create type config item!";

	}

	dcontext->create_dir_type = rc;

	return NULL;
}

static const char *cmd_debug_body_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid debug header dir create type config item!";

	}

	dcontext->create_body_dir_type = rc;
	return NULL;
}

static const char *cmd_debug_hash_entries_limited(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->n_entries_limit = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_debug_hash_table_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->tbl_size = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_debug_hash_table_entry_timeout(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->entry_timeout = (uint64_t)strtoul(p1,&end,10);


	return NULL;
}

static const char *cmd_debug_hash_table_cache_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_debug_context_t *dcontext = (ch_debug_context_t*)_dcfg;

	dcontext->n_caches_limits = (uint32_t)strtoul(p1,&end,10);


	return NULL;
}

static const command_rec debug_context_directives[] = {

    CH_INIT_TAKE1(
            "PDEBUGHeaderDir",
            cmd_debug_header_dir,
            NULL,
            0,
            "set the store path of debug header"
            ),
    
    CH_INIT_TAKE1(
            "PDEBUGReqBodyDir",
            cmd_debug_reqbody_dir,
            NULL,
            0,
            "set the store path of debug request body"
            ),

    CH_INIT_TAKE1(
            "PDEBUGResBodyDir",
            cmd_debug_resbody_dir,
            NULL,
            0,
            "set the store path of debug response body"
            ),

    CH_INIT_TAKE1(
            "PDEBUGFileStoreSize",
            cmd_debug_file_store_size,
            NULL,
            0,
            "set the size of debug file store"
            ),

    CH_INIT_TAKE1(
            "PDEBUGHeaderDirCreateType",
            cmd_debug_header_dir_create_type,
            NULL,
            0,
            "set the type created of header dir by time"
            ),

    CH_INIT_TAKE1(
            "PDEBUGBodyDirCreateType",
            cmd_debug_body_dir_create_type,
            NULL,
            0,
            "set the type created of debug body dir by time"
            ),

    CH_INIT_TAKE1(
            "PDEBUGHashEntriesLimited",
            cmd_debug_hash_entries_limited,
            NULL,
            0,
            "set the debug hash entries limited"
            ),

    CH_INIT_TAKE1(
            "PDEBUGHashTableSize",
            cmd_debug_hash_table_size,
            NULL,
            0,
            "set the debug hash table size"
            ),

    CH_INIT_TAKE1(
            "PDEBUGHashTableCacheSize",
            cmd_debug_hash_table_cache_size,
            NULL,
            0,
            "set the debug hash table cache size"
            ),

    CH_INIT_TAKE1(
            "PDEBUGHashTableEntryTimeout",
            cmd_debug_hash_table_entry_timeout,
            NULL,
            0,
            "set the hash table entry timeout"
            ),
};

static inline void _do_debug_context_init(ch_debug_context_t *dcontext){

	dcontext->fstore_dir = "/tmp/debug/header";
	dcontext->req_body_dir = "/tmp/debug/reqBody";
	dcontext->res_body_dir = "/tmp/debug/resBody";
	dcontext->fstore_buf_size = 64*1024;
	dcontext->create_dir_type = BY_HOUR;
	dcontext->create_body_dir_type = BY_MIN;
	dcontext->n_entries_limit = 1000000;
	dcontext->tbl_size = 1024;
	dcontext->entry_timeout = 5*60;
	dcontext->n_caches_limits = 1024;
}

static int _debug_context_init(ch_proto_context_t *pcontext,ch_debug_context_t *dcontext){

	const char *msg;

	dcontext->pcontext = pcontext;
	
	_do_debug_context_init(dcontext);

    msg = ch_process_command_config(debug_context_directives,
		(void*)dcontext,pcontext->mp,pcontext->mp,pcontext->debug_proto_cfile);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config debug parser context error:%s",msg);
        return -1;
    }

	return 0;
}

ch_debug_context_t * ch_debug_context_create(ch_proto_context_t *pcontext){

    ch_debug_context_t *dcontext = NULL;
	
	dcontext = (ch_debug_context_t*)apr_palloc(pcontext->mp,sizeof(*dcontext));

	if(_debug_context_init(pcontext,dcontext))
		return NULL;

    return dcontext;
}


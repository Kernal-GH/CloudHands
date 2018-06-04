/*
 *
 *      Filename: ch_http_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-27 19:05:36
 * Last Modified: 2016-12-09 15:24:01
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_http_context.h"
#include "ch_fpath.h"

static const char *cmd_http_header_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->fstore_dir = p1;

	return NULL;
}

static const char *cmd_http_reqbody_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->req_body_dir = p1;

	return NULL;
}
static const char *cmd_http_resbody_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->res_body_dir = p1;

	return NULL;
}

static const char *cmd_http_file_store_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->fstore_buf_size = (size_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_http_header_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc ;
	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid http header dir create type config item!";

	}

	hcontext->create_dir_type = rc;

	return NULL;
}

static const char *cmd_http_body_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid http header dir create type config item!";

	}

	hcontext->create_body_dir_type = rc;
	return NULL;
}

static const char *cmd_http_hash_entries_limited(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->n_entries_limit = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_http_hash_table_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->tbl_size = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_http_hash_table_entry_timeout(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->entry_timeout = (uint64_t)strtoul(p1,&end,10);


	return NULL;
}

static const char *cmd_http_hash_table_cache_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->n_caches_limits = (uint32_t)strtoul(p1,&end,10);


	return NULL;
}

static const char *cmd_http_parser_buf_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_http_context_t *hcontext = (ch_http_context_t*)_dcfg;

	hcontext->parser_buf_size = (size_t)strtoul(p1,&end,10);

	return NULL;
}

static const command_rec http_context_directives[] = {

    CH_INIT_TAKE1(
            "PHTTPHeaderDir",
            cmd_http_header_dir,
            NULL,
            0,
            "set the store path of http header"
            ),
    
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
            "PHTTPFileStoreSize",
            cmd_http_file_store_size,
            NULL,
            0,
            "set the size of http file store"
            ),

    CH_INIT_TAKE1(
            "PHTTPHeaderDirCreateType",
            cmd_http_header_dir_create_type,
            NULL,
            0,
            "set the type created of header dir by time"
            ),

    CH_INIT_TAKE1(
            "PHTTPBodyDirCreateType",
            cmd_http_body_dir_create_type,
            NULL,
            0,
            "set the type created of http body dir by time"
            ),

    CH_INIT_TAKE1(
            "PHTTPHashEntriesLimited",
            cmd_http_hash_entries_limited,
            NULL,
            0,
            "set the http hash entries limited"
            ),

    CH_INIT_TAKE1(
            "PHTTPHashTableSize",
            cmd_http_hash_table_size,
            NULL,
            0,
            "set the http hash table size"
            ),

    CH_INIT_TAKE1(
            "PHTTPHashTableCacheSize",
            cmd_http_hash_table_cache_size,
            NULL,
            0,
            "set the http hash table cache size"
            ),

    CH_INIT_TAKE1(
            "PHTTPHashTableEntryTimeout",
            cmd_http_hash_table_entry_timeout,
            NULL,
            0,
            "set the hash table entry timeout"
            ),

    CH_INIT_TAKE1(
            "PHTTPParserBufSize",
            cmd_http_parser_buf_size,
            NULL,
            0,
            "set the http parser buffer size"
            ),
};

static inline void _http_context_init(ch_http_context_t *hcontext){

	hcontext->fstore_dir = "/tmp/http/header";
	hcontext->req_body_dir = "/tmp/http/reqBody";
	hcontext->res_body_dir = "/tmp/http/resBody";
	hcontext->fstore_buf_size = 64*1024;
	hcontext->create_dir_type = BY_HOUR;
	hcontext->create_body_dir_type = BY_MIN;
	hcontext->n_entries_limit = 1000000;
	hcontext->tbl_size = 1024;
	hcontext->entry_timeout = 5*60;
	hcontext->n_caches_limits = 1024;
	hcontext->parser_buf_size = 64*1024;
}

int ch_http_context_init(ch_proto_context_t *pcontext,ch_http_context_t *hcontext){

	const char *msg;

	hcontext->pcontext = pcontext;
	
	_http_context_init(hcontext);

    msg = ch_process_command_config(http_context_directives,
		(void*)hcontext,pcontext->mp,pcontext->mp,pcontext->http_proto_cfile);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config http parser context error:%s",msg);
        return -1;
    }

	return 0;
}

ch_http_context_t * ch_http_context_create(ch_proto_context_t *pcontext){

    ch_http_context_t *hcontext = NULL;
	
	hcontext = (ch_http_context_t*)apr_palloc(pcontext->mp,sizeof(*hcontext));

	if(ch_http_context_init(pcontext,hcontext))
		return NULL;

    return hcontext;
}

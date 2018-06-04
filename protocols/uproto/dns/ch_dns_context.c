/*
 *
 *      Filename: ch_dns_context.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 19:26:26
 * Last Modified: 2016-11-02 03:39:57
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_dns_context.h"
#include "ch_fpath.h"

static const char *cmd_dns_header_dir(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;

	ch_dns_context_t *dcontext = (ch_dns_context_t*)_dcfg;

	dcontext->fstore_dir = p1;

	return NULL;
}

static const char *cmd_dns_file_store_size(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_dns_context_t *dcontext = (ch_dns_context_t*)_dcfg;

	dcontext->fstore_buf_size = (size_t)strtoul(p1,&end,10);

	return NULL;
}

static const char *cmd_dns_header_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	int rc ;
	ch_dns_context_t *dcontext = (ch_dns_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid dns header dir create type config item!";

	}

	dcontext->create_dir_type = rc;

	return NULL;
}


static const char *cmd_sessions_limit_mp(cmd_parms *cmd, void *_dcfg, const char *p1){

	/* unused */
	cmd = cmd;
	char *end;

	ch_dns_context_t *dcontext = (ch_dns_context_t*)_dcfg;

	dcontext->sessions_limit_mp = (uint32_t)strtoul(p1,&end,10);

	return NULL;
}


static const command_rec dns_context_directives[] = {

    CH_INIT_TAKE1(
            "PDNSHeaderDir",
            cmd_dns_header_dir,
            NULL,
            0,
            "set the store path of dns header"
            ),
    

    CH_INIT_TAKE1(
            "PDNSFileStoreSize",
            cmd_dns_file_store_size,
            NULL,
            0,
            "set the size of dns file store"
            ),

    CH_INIT_TAKE1(
            "PDNSHeaderDirCreateType",
            cmd_dns_header_dir_create_type,
            NULL,
            0,
            "set the type created of header dir by time"
            ),


    CH_INIT_TAKE1(
            "PDNSSessionsMPLimited",
            cmd_sessions_limit_mp,
            NULL,
            0,
            "set session entries number limited in a memory pool"
            ),


};

static inline void _dns_context_init(ch_dns_context_t *dcontext){

	dcontext->fstore_dir = "/tmp/dns/profile";
	dcontext->fstore_buf_size = 64*1024;
	dcontext->create_dir_type = BY_HOUR;

	dcontext->sessions_limit_mp = 1000;
}


ch_dns_context_t * ch_dns_context_create(ch_proto_context_t *pcontext){

	const char *msg;
    
	ch_dns_context_t *dcontext = NULL;
	
	dcontext = (ch_dns_context_t*)apr_palloc(pcontext->mp,sizeof(*dcontext));
	dcontext->pcontext = pcontext;

	_dns_context_init(dcontext);

    msg = ch_process_command_config(dns_context_directives,
		(void*)dcontext,pcontext->mp,pcontext->mp,pcontext->dns_proto_cfile);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config dns protocol context error:%s",msg);
        return NULL;
    }

    return dcontext;
}


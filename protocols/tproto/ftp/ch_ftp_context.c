/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_context.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 17:13:25
 * Last Modified: 2017-01-10 11:42:01
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_config.h"
#include "ch_ftp_context.h"

#define CFG_FTP_DFT_FSTORE_DIR "/tmp/ftp/profile";
#define CFG_FTP_DFT_DATAS_DIR "/tmp/ftp/interactive_content";
#define CFG_FTP_DFT_FSTORE_BUF_SIZE 1
#define CFG_FTP_DFT_CREATE_DIR_TYPE BY_HOUR
#define CFG_FTP_DFT_CREATE_DATA_DIR_TYPE BY_MIN
#define CFG_FTP_DFT_ENTRY_LIMIT 1000000
#define CFG_FTP_DFT_TABLE_SIZE 1024
#define CFG_FTP_DFT_ENTRY_TIMEOUT 5 * 60
#define CFG_FTP_DFT_CACHE_LIMIT 1024

static const char *
cmd_ftp_profile_dir(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->fstore_dir = p1;

	return NULL;
}

static const char *
cmd_ftp_interactive_content_dir(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->cmd_save_dir = p1;

	return NULL;
}

static const char *
cmd_ftp_profile_store_size(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->fstore_buf_size = (size_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_ftp_profile_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	int rc;
	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	cmd = cmd;

	rc = ch_fpath_create_type_get(p1);
	if (rc == -1) {
		return "Invalid ftp profike dir create type config item!";
	}

	fcontext->create_dir_type = rc;

	return NULL;
}

static const char *
cmd_ftp_datas_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	int rc ;
	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	cmd = cmd;

	rc = ch_fpath_create_type_get(p1);
	if (rc == -1) {
		return "Invalid ftp transfer datas dir create type config item!";
	}

	fcontext->create_cmd_dir_type = rc;

	return NULL;
}

static const char *
cmd_ftp_hash_entries_limited(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->n_entries_limit = (uint32_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_ftp_hash_table_size(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->tbl_size = (uint32_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_ftp_hash_table_entry_timeout(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->entry_timeout = (uint64_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_ftp_hash_table_cache_size(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_ftp_context_t *fcontext = (ch_ftp_context_t *) _dcfg;

	fcontext->n_caches_limits = (uint32_t) strtoul(p1, &end, 10);

	return NULL;
}

static const command_rec ftp_context_directives[] = {
    CH_INIT_TAKE1(
            "PFTPProfileDir",
            cmd_ftp_profile_dir,
            NULL,
            0,
            "set the store path for ftp profile"
            ),

    CH_INIT_TAKE1(
            "PFTPInteractiveContentDir",
            cmd_ftp_interactive_content_dir,
            NULL,
            0,
            "set the store path for ftp transfer datas"
            ),

    CH_INIT_TAKE1(
            "PFTPProfileStoreSize",
            cmd_ftp_profile_store_size,
            NULL,
            0,
            "set the size for profile store"
            ),

    CH_INIT_TAKE1(
            "PFTPProfileDirCreateType",
            cmd_ftp_profile_dir_create_type,
            NULL,
            0,
            "set the type created for profile by time"
            ),

    CH_INIT_TAKE1(
            "PFTPDatasDirCreateType",
            cmd_ftp_datas_dir_create_type,
            NULL,
            0,
            "set the type created for transfer datas by time"
            ),

    CH_INIT_TAKE1(
            "PFTPHashEntriesLimited",
            cmd_ftp_hash_entries_limited,
            NULL,
            0,
            "set the ftp hash entries limited"
            ),

    CH_INIT_TAKE1(
            "PFTPHashTableSize",
            cmd_ftp_hash_table_size,
            NULL,
            0,
            "set the ftp hash table size"
            ),

    CH_INIT_TAKE1(
            "PFTPHashTableCacheSize",
            cmd_ftp_hash_table_entry_timeout,
            NULL,
            0,
            "set the hash table entry timeout"
            ),

    CH_INIT_TAKE1(
            "PFTPHashTableEntryTimeout",
            cmd_ftp_hash_table_cache_size,
            NULL,
            0,
            "set the ftp hash table cache size"
            ),
};

static inline void
_ftp_context_init(ch_ftp_context_t *fcontext)
{
	fcontext->fstore_dir = CFG_FTP_DFT_FSTORE_DIR;
	fcontext->cmd_save_dir = CFG_FTP_DFT_DATAS_DIR;
	fcontext->fstore_buf_size = CFG_FTP_DFT_FSTORE_BUF_SIZE;
	fcontext->create_dir_type = CFG_FTP_DFT_CREATE_DIR_TYPE;
	fcontext->create_cmd_dir_type = CFG_FTP_DFT_CREATE_DATA_DIR_TYPE;
	fcontext->n_entries_limit = CFG_FTP_DFT_ENTRY_LIMIT;
	fcontext->tbl_size = CFG_FTP_DFT_TABLE_SIZE;
	fcontext->entry_timeout = CFG_FTP_DFT_ENTRY_TIMEOUT;
	fcontext->n_caches_limits = CFG_FTP_DFT_CACHE_LIMIT;
}

static int
ch_ftp_context_init(ch_proto_context_t *pcontext, ch_ftp_context_t *fcontext)
{
	const char *err_msg = NULL;

	fcontext->pcontext = pcontext;

	_ftp_context_init(fcontext);

	err_msg = ch_process_command_config(ftp_context_directives,
		(void *)fcontext, pcontext->mp, pcontext->mp, pcontext->ftp_proto_cfile);

	/* config failed. */
	if (err_msg != NULL) {
		ch_log(CH_LOG_ERR, "Config ftp parser context error: %s.\n", err_msg);
		return -1;
	}

	return 0;
}

ch_ftp_context_t *ch_ftp_context_create(ch_proto_context_t *pcontext)
{
	ch_ftp_context_t *fcontext = NULL;

	fcontext = (ch_ftp_context_t *) apr_palloc(pcontext->mp, sizeof(*pcontext));
	if (fcontext == NULL) {
		ch_log(CH_LOG_ERR, "%s: apr_palloc call faield.\n", __func__);
		return NULL;
	}

	if (ch_ftp_context_init(pcontext, fcontext)) {
		ch_log(CH_LOG_ERR, "%s: init ch_ftp_context_t failed.\n", __func__);
		return NULL;
	}

	return fcontext;
}


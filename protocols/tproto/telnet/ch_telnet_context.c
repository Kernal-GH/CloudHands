/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_context.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 14:37:22
 * Last Modified: 2016-12-13 18:33:40
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_fpath.h"
#include "ch_config.h"
#include "ch_telnet_context.h"

#define CFG_TELNET_DFT_FSTORE_DIR "/tmp/telnet/profile"
#define CFG_TELNET_DFT_DATAS_DIR "/tmp/telnet/transferdatas"
#define CFG_TELNET_DFT_FSTORE_BUF_SIZE 1
#define CFG_TELNET_DFT_CREATE_DIR_TYPE BY_HOUR
#define CFG_TELNET_DFT_CREATE_DATA_DIR_TYPE BY_MIN
#define CFG_TELNET_DFT_ENTRY_LIMIT 1000000
#define CFG_TELNET_DFT_TABLE_SIZE 1024
#define CFG_TELNET_DFT_ENTRY_TIMEOUT 5 * 60
#define CFG_TELNET_DFT_CACHE_LIMIT 1024

static const char *
cmd_telnet_profile_dir(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->fstore_dir = p1;

	return NULL;
}

static const char *
cmd_telnet_datas_dir(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->trans_data_dir = p1;

	return NULL;
}

static const char *
cmd_telnet_profile_store_size(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->fstore_buf_size = (size_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_telnet_profile_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	int rc;
	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	cmd = cmd;

	rc = ch_fpath_create_type_get(p1);
	if (rc == -1) {
		return "Invalid telnet profike dir create type config item!";
	}

	tcontext->create_dir_type = rc;

	return NULL;
}

static const char *
cmd_telnet_datas_dir_create_type(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	int rc ;
	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	cmd = cmd;

	rc = ch_fpath_create_type_get(p1);
	if (rc == -1) {
		return "Invalid telnet transfer datas dir create type config item!";
	}

	tcontext->create_data_dir_type = rc;

	return NULL;
}

static const char *
cmd_telnet_hash_entries_limited(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->n_entries_limit = (uint32_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_telnet_hash_table_size(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->tbl_size = (uint32_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_telnet_hash_table_entry_timeout(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->entry_timeout = (uint64_t) strtoul(p1, &end, 10);

	return NULL;
}

static const char *
cmd_telnet_hash_table_cache_size(cmd_parms *cmd, void *_dcfg, const char *p1)
{
	char *end;

	cmd = cmd;

	ch_telnet_context_t *tcontext = (ch_telnet_context_t *) _dcfg;

	tcontext->n_caches_limits = (uint32_t) strtoul(p1, &end, 10);

	return NULL;
}

static const command_rec telnet_context_directives[] = {
    CH_INIT_TAKE1(
            "PTelnetProfileDir",
            cmd_telnet_profile_dir,
            NULL,
            0,
            "set the store path for telnet profile"
            ),

    CH_INIT_TAKE1(
            "PTelnetDatasDir",
            cmd_telnet_datas_dir,
            NULL,
            0,
            "set the store path for telnet transfer datas"
            ),

    CH_INIT_TAKE1(
            "PTelnetProfileStoreSize",
            cmd_telnet_profile_store_size,
            NULL,
            0,
            "set the size for profile store"
            ),

    CH_INIT_TAKE1(
            "PTelnetProfileDirCreateType",
            cmd_telnet_profile_dir_create_type,
            NULL,
            0,
            "set the type created for profile by time"
            ),

    CH_INIT_TAKE1(
            "PTelnetDatasDirCreateType",
            cmd_telnet_datas_dir_create_type,
            NULL,
            0,
            "set the type created for transfer datas by time"
            ),

    CH_INIT_TAKE1(
            "PTelnetHashEntriesLimited",
            cmd_telnet_hash_entries_limited,
            NULL,
            0,
            "set the telnet hash entries limited"
            ),

    CH_INIT_TAKE1(
            "PTelnetHashTableSize",
            cmd_telnet_hash_table_size,
            NULL,
            0,
            "set the telnet hash table size"
            ),

    CH_INIT_TAKE1(
            "PTelnetHashTableCacheSize",
            cmd_telnet_hash_table_entry_timeout,
            NULL,
            0,
            "set the hash table entry timeout"
            ),

    CH_INIT_TAKE1(
            "PTelnetHashTableEntryTimeout",
            cmd_telnet_hash_table_cache_size,
            NULL,
            0,
            "set the telnet hash table cache size"
            ),
};

static inline void 
_telnet_context_init(ch_telnet_context_t *tcontext)
{
	tcontext->fstore_dir = CFG_TELNET_DFT_FSTORE_DIR;
	tcontext->trans_data_dir = CFG_TELNET_DFT_DATAS_DIR;
	tcontext->fstore_buf_size = CFG_TELNET_DFT_FSTORE_BUF_SIZE;
	tcontext->create_dir_type = CFG_TELNET_DFT_CREATE_DIR_TYPE;
	tcontext->create_data_dir_type = CFG_TELNET_DFT_CREATE_DATA_DIR_TYPE;

	tcontext->n_entries_limit = CFG_TELNET_DFT_ENTRY_LIMIT;
	tcontext->tbl_size = CFG_TELNET_DFT_TABLE_SIZE;
	tcontext->entry_timeout = CFG_TELNET_DFT_ENTRY_TIMEOUT;
	tcontext->n_caches_limits = CFG_TELNET_DFT_CACHE_LIMIT;
}

static int
ch_telnet_context_init(ch_proto_context_t *pcontext, ch_telnet_context_t *tcontext)
{
	const char *err_msg;

	tcontext->pcontext = pcontext;

	_telnet_context_init(tcontext);

	err_msg = ch_process_command_config(telnet_context_directives,
		(void *)tcontext, pcontext->mp, pcontext->mp, pcontext->telnet_proto_cfile);

	/* config failed */
	if (err_msg != NULL) {
		ch_log(CH_LOG_ERR, "Config telnet parser context error: %s.", err_msg);
		return -1;
	}
	
	return 0;
}

ch_telnet_context_t *ch_telnet_context_create(ch_proto_context_t *pcontext)
{
	ch_telnet_context_t *tcontext = NULL;

	tcontext = (ch_telnet_context_t *) apr_palloc(pcontext->mp, sizeof(*tcontext));
	if (tcontext == NULL) {
		ch_log(CH_LOG_ERR, "%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	if (ch_telnet_context_init(pcontext, tcontext)) {
		ch_log(CH_LOG_ERR, "%s: init ch_telnet_context_t failed.\n", __func__);
		return NULL;
	}

	return tcontext;
}

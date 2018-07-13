/*
 *
 *      Filename: ch_pp_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 15:28:32
 * Last Modified: 2018-05-14 15:28:32
 */

#ifndef CH_PP_CONTEXT_H
#define CH_PP_CONTEXT_H

typedef struct ch_pp_context_t ch_pp_context_t;

#include "ch_mpool.h"

#define FMT_MSGPACK "msgpack"
#define FMT_JSON "json"
#define FMT_XML "xml"
#define FMT_BIN "bin"
#define FMT_TXT "txt"

#define FMT_TYPE_MSGPACK 0
#define FMT_TYPE_JSON 1
#define FMT_TYPE_XML 2
#define FMT_TYPE_BIN 3
#define FMT_TYPE_TXT 4

struct ch_pp_context_t {

    ch_pool_t *mp;

    int log_level;
    const char *log_file;
	
	const char *fstore_dir;
	size_t fstore_buf_size;
	int create_dir_type;

	const char *shm_fname;
	uint64_t shm_fsize;
	uint64_t shm_fentry_size;

	/* config path based on tcp */
	const char *http_pp_cfile;
	const char *mail_pp_cfile;
	const char *debug_pp_cfile;
	const char *telnet_pp_cfile;
	const char *ftp_pp_cfile;
	const char *smon_pp_cfile;

	int fmt_type;

	uint64_t work_sleep_time;

	int  tbl_pool_type;
	
	size_t tbl_session_limits;
	
	size_t tbl_session_tbl_size;

	size_t tbl_session_cache_limits;

	uint64_t tbl_session_timeout;
};

extern ch_pp_context_t * ch_pp_context_create(ch_pool_t *mp,const char *cfpath);

#endif /*CH_PP_CONTEXT_H*/

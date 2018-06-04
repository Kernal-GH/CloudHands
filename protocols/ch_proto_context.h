/*
 *
 *      Filename: ch_proto_context.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 00:01:13
 * Last Modified: 2016-10-27 00:01:13
 */

#ifndef CH_PROTO_CONTEXT_H
#define CH_PROTO_CONTEXT_H

typedef struct ch_proto_context_t ch_proto_context_t;

#include <apr_pools.h>
#include "ch_mmap_file_format.h"
#include "ch_proto_pool.h"

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

/* trans proto types */
enum {

	TPT_TCP,
	TPT_UDP,
	TPT_MAX
};

#define TPT_TCP_N "tcp"
#define TPT_UDP_N "udp"

struct ch_proto_context_t {

    apr_pool_t *mp;

	pid_t pid;
    int log_level;
    const char *log_file;
    const char *mmap_file;

	int trans_proto_type;

	/* config path based on tcp */
	const char *http_proto_cfile;
	const char *mail_proto_cfile;
	const char *debug_proto_cfile;
	const char *telnet_proto_cfile;
	const char *ftp_proto_cfile;

	/* config path based on udp */
	const char *dns_proto_cfile;

	int fmt_type;

	uint64_t work_sleep_time;

    ch_mmap_file_format_t *mmfmt;

	ch_proto_pool_t *ppool;
};

extern ch_proto_context_t * ch_proto_context_create(apr_pool_t *mp,const char *conf_path,const char *mmap_file);

extern int ch_proto_context_start(ch_proto_context_t *pcontext);

extern void ch_proto_context_exit(ch_proto_context_t *pcontext);

#endif /* CH_PROTO_CONTEXT_H */

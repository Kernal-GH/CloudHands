/*
 *
 *      Filename: ch_proto_pool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 02:43:18
 * Last Modified: 2016-10-27 02:43:18
 */

#ifndef CH_PROTO_POOL_H
#define CH_PROTO_POOL_H

/* Parse data status returned! */
#define PARSE_DONE 0
#define PARSE_CONTINUE 1
#define PARSE_BREAK 2

typedef struct ch_proto_pool_t ch_proto_pool_t;
typedef struct ch_proto_trans_t ch_proto_trans_t;
typedef struct ch_app_proto_t ch_app_proto_t;

#include "ch_fpath.h"
#include "ch_file_store.h"
#include "ch_proto_context.h"
#include "ch_session_format.h"
#include "ch_session_entry.h"

struct ch_proto_pool_t {

	ch_proto_context_t *pcontext;
	ch_proto_trans_t * trans_proto;
};

struct ch_proto_trans_t {

	int trans_proto_type;
	
	ch_app_proto_t* app_protos[PROTOCOL_MAX];

	void (*parse)(ch_proto_trans_t *pt,ch_app_proto_t *aproto,ch_mmap_file_entry_t *fe);
};

struct ch_app_proto_t {

	int trans_proto_type;

	unsigned int proto_id;

	ch_proto_context_t *pcontext;

	ch_session_format_t *fmt;

	ch_table_t *htbl;

	ch_fpath_t *fpath;

	void *priv_data;

	int (*parse_request)(ch_app_proto_t *aproto,ch_session_entry_t *session,
		void *data,uint32_t dlen,void *priv_data);
	
	int (*parse_response)(ch_app_proto_t *aproto,ch_session_entry_t *session,
		void *data,uint32_t dlen,void *priv_data);

	int (*is_request)(ch_app_proto_t *aproto,ch_mmap_file_entry_t *fe,void *priv_data);
};

extern ch_proto_pool_t* ch_proto_pool_create(ch_proto_context_t *pcontext);

extern void ch_app_proto_register(ch_proto_pool_t *ppool,ch_app_proto_t *aproto);

extern void ch_proto_parse(ch_proto_pool_t *ppool,ch_mmap_file_entry_t *fentry);

#endif /* CH_PROTO_POOL_H */

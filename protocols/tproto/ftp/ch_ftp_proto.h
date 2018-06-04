/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_proto.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 15:32:59
 * Last Modified: 2017-01-04 15:32:59
 */

#ifndef __CH_FTP_PROTO_H__
#define __CH_FTP_PROTO_H__

#include "ch_ftp_context.h"

typedef struct ch_ftp_proto_s {
	ch_app_proto_t aproto;

	ch_ftp_context_t *fcontext;

	ch_fpath_t *cmd_save_fpath;
} ch_ftp_proto_t;

ch_app_proto_t *ch_ftp_proto_create(apr_pool_t *mp, ch_proto_context_t *pcontext);

#endif //__CH_FTP_PROTO_H__


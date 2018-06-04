/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_interactive_content_data.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-05 12:01:43
 * Last Modified: 2017-01-05 12:01:43
 */


#ifndef __CH_FTP_INTERACTIVE_CONTENT_DATA_H__
#define __CH_FTP_INTERACTIVE_CONTENT_DATA_H__

#include "ch_file.h"
#include "ch_fpath.h"
#include "apr_pools.h"

typedef struct ch_ftp_cmds_data_s {
	ch_file_t file;
	int saved;

	const char *fname; /* filename to store datas */
	size_t body_len;
} ch_ftp_cmds_data_t;

ch_ftp_cmds_data_t *
ch_ftp_cmds_data_create(apr_pool_t *mp, ch_fpath_t *fpath);

void ch_ftp_cmds_data_destroy(ch_ftp_cmds_data_t *cmds_data);

int 
ch_ftp_cmds_data_append_data(ch_ftp_cmds_data_t *cmds_data, const char *data, uint32_t data_len);

#endif //__CH_FTP_INTERACTIVE_CONTENT_DATA_H__


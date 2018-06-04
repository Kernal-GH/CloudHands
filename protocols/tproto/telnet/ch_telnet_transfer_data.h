/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_transfer_data.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 11:50:46
 * Last Modified: 2016-12-09 11:50:46
 */

#ifndef __CH_TELNET_TRANSFER_DATA_H__
#define __CH_TELNET_TRANSFER_DATA_H__

#include "ch_file.h"
#include "ch_fpath.h"
#include "apr_pools.h"

typedef struct ch_telnet_transfer_data_s {
	ch_file_t file;
	int saved;

	const char *fname; /* filename to store body */
	size_t body_len;
} ch_telnet_transfer_data_t;

ch_telnet_transfer_data_t *
ch_telnet_transfer_data_create(apr_pool_t *mp, ch_fpath_t *fpath);

void ch_telnet_transfer_data_destroy(ch_telnet_transfer_data_t *trans_data);

int 
ch_telnet_transfer_data_append_data(ch_telnet_transfer_data_t *trans_data, const char *data, uint32_t data_len);

#endif //__CH_TELNET_TRANSFER_DATA_H__


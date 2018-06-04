/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_transfer_data.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-13 11:21:23
 * Last Modified: 2017-01-05 17:08:08
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "apr_strings.h"
#include "ch_telnet_transfer_data.h"

ch_telnet_transfer_data_t *
ch_telnet_transfer_data_create(apr_pool_t *mp, ch_fpath_t *fpath)
{
	int fd = -1;
	char *fname, *fname_tmp;
	ch_telnet_transfer_data_t *trans_data = NULL;

	trans_data = (ch_telnet_transfer_data_t *) apr_palloc(mp, sizeof(*trans_data));
	if (trans_data == NULL) {
		CH_ERR_LOG("%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	ch_fpath_name_create(fpath, &fname, &fname_tmp);
	if (fname == NULL) {
		CH_ERR_LOG("Can not create a file used to store telnet content.\n");
		return NULL;
	}

	trans_data->fname = apr_pstrdup(mp, fname);
	if (trans_data->fname == NULL) {
		CH_ERR_LOG("%s: apr_pstrdup failed.\n", __func__);
		return NULL;
	}

	fd = ch_file_open_for_write_common(trans_data->fname);
	if (fd < 0) {
		CH_ERR_LOG("Open file [%s] failed.", fname);
		return NULL;
	}

	trans_data->file.fd = fd;
	trans_data->file.offset = 0;
	trans_data->file.sys_offset = 0;

	trans_data->saved = 0;
	trans_data->body_len = 0;

	return trans_data;
}

void ch_telnet_transfer_data_destroy(ch_telnet_transfer_data_t *trans_data)
{
	if (trans_data == NULL) {
		CH_DEBUG_LOG("%s: Invalid parameter.", __func__);
		return;
	}

	if (trans_data->file.fd > 0 && trans_data->saved != 1) {
		close(trans_data->file.fd);
		trans_data->saved = 1;
	}
}

int ch_telnet_transfer_data_append_data(ch_telnet_transfer_data_t *trans_data, const char *data, uint32_t data_len)
{
	ssize_t written = 0;

	if (trans_data == NULL) {
		CH_DEBUG_LOG("%s: Invalid parameter.", __func__);
		return -1;
	}

	if (data == NULL || data_len <= 0) {
		CH_DEBUG_LOG("%s: Nothing to be append.", __func__);
		return 0;
	}

	written = ch_file_write(&trans_data->file, (void *)data, data_len, trans_data->file.offset);

	if (written == -1 || data_len != written) {
        CH_ERR_LOG("%s: write data[%d] into body's file[%s] failed!", __func__,
			data_len, trans_data->fname);
        return -1;
	}

	trans_data->body_len += data_len;

	return 0;
}

/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_interactive_content_data.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-05 12:06:53
 * Last Modified: 2017-01-10 11:44:08
 */


#include <stdio.h>
#include <stdlib.h>

#include <apr_pools.h>
#include <apr_strings.h>

#include "ch_log.h"
#include "ch_ftp_interactive_content_data.h"

ch_ftp_cmds_data_t *
ch_ftp_cmds_data_create(apr_pool_t *mp, ch_fpath_t *fpath)
{
	int fd = -1;
	char *fname, *fname_tmp;
	ch_ftp_cmds_data_t *cmds_data = NULL;

	cmds_data = (ch_ftp_cmds_data_t *) apr_palloc(mp, sizeof(*cmds_data));
	if (cmds_data == NULL) {
		CH_ERR_LOG("%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	ch_fpath_name_create(fpath, &fname, &fname_tmp);
	if (fname == NULL) {
		CH_ERR_LOG("Can not create a file used to store ftp commands content.\n");
		return NULL;
	}

	cmds_data->fname = apr_pstrdup(mp, fname);
	if (cmds_data->fname == NULL) {
		CH_ERR_LOG("%s: apr_pstrdup failed.\n", __func__);
		return NULL;
	}

	fd = ch_file_open_for_write_common(cmds_data->fname);
	if (fd < 0) {
		CH_ERR_LOG("Open file [%s] failed.", fname);
		return NULL;
	}

	cmds_data->file.fd = fd;
	cmds_data->file.offset = 0;
	cmds_data->file.sys_offset = 0;

	cmds_data->saved = 0;
	cmds_data->body_len = 0;

	return cmds_data;
}

void ch_ftp_cmds_data_destroy(ch_ftp_cmds_data_t *cmds_data)
{
	if (cmds_data == NULL) {
		CH_DEBUG_LOG("%s: Invalid parameter.", __func__);
		return;
	}

	if (cmds_data->file.fd > 0 && cmds_data->saved != 1) {
		close(cmds_data->file.fd);
		cmds_data->saved = 1;
	}
}

int 
ch_ftp_cmds_data_append_data(ch_ftp_cmds_data_t *cmds_data, const char *data, uint32_t data_len)
{
	ssize_t written = 0;

	if (cmds_data == NULL) {
		CH_DEBUG_LOG("%s: Invalid parameter.", __func__);
		return -1;
	}

	if (data == NULL || data_len <= 0) {
		CH_DEBUG_LOG("%s: Nothing to be append.", __func__);
		return 0;
	}

	written = ch_file_write(&cmds_data->file, (void *)data, data_len, cmds_data->file.offset);

	if (written == -1 || data_len != written) {
        CH_ERR_LOG("%s: write data[%d] into body's file[%s] failed!", __func__,
			data_len, cmds_data->fname);
        return -1;
	}

	cmds_data->body_len += data_len;

	return 0;
}

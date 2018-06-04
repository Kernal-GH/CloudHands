#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "ch_log.h"
#include "apr_strings.h"
#include "ch_mail_file.h"

ch_mail_ctnt_t *
ch_mail_ctnt_create(apr_pool_t *pool, const char *path)
{
	int fd;
	ch_mail_ctnt_t *ctnt = NULL;

	ctnt = (ch_mail_ctnt_t *) apr_palloc(pool, sizeof(ch_mail_ctnt_t));
	if (ctnt == NULL) {
		return NULL;
	}
	
	ctnt->path = apr_pstrdup(pool, path);
	if (ctnt->path == NULL) {
		return NULL;
	}

    fd = ch_file_open_for_write_common(ctnt->path);
    if(fd<0){
        ch_log(CH_LOG_ERR,"Open file [%s] failed!", ctnt->path);
        return NULL;
    }
	ctnt->file.fd = fd;
	ctnt->file.offset = 0;
	ctnt->file.sys_offset = 0;

	ctnt->len = 0;
	ctnt->saved = 0;

	return ctnt;
}

int ch_mail_ctnt_close(ch_mail_ctnt_t *ctnt)
{
	if (ctnt == NULL) {
		CH_DEBUG_LOG("Invalid parameter.\n");
		return -1;
	}

	close(ctnt->file.fd);
	ctnt->saved = 1;

	return 0;
}

int 
ch_mail_ctnt_append_data(ch_mail_ctnt_t *ctnt, const uint8_t *data, uint32_t data_len)
{
	int ret = 0;
	ssize_t written = 0;

	if (ctnt == NULL) {
		CH_DEBUG_LOG("Invalid parameter.\n");
		return -1;
	}

	if (data == NULL || data_len == 0) {
		CH_DEBUG_LOG("Nothing to be append\n");
		return 0;
	}

    written = ch_file_write(&ctnt->file, (void *) data, data_len, ctnt->file.offset);

    if ((written == -1) || (data_len != written)) {
        /*error*/
        CH_ERR_LOG("write data[%d] into body's file[%s] failed!", 
			data_len, ctnt->path);
        return -1;
    }

	ctnt->len += data_len;

	return ret;
}

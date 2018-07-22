#ifndef __CH_PRASE_FILE_H__
#define __CH_PRASE_FILE_H__


#include "ch_list.h"
#include "ch_string.h"
#include "ch_sfile.h"
#include "ch_mpool.h"

typedef struct ch_mail_ctnt_s {
	struct list_head anchor;
	char *filename; /* original filename, only used to save mail attachment name */
	char *path;		/* file save path */
	ch_sfile_t file;
	ch_pool_t *mp;
	size_t len; /* file content length */
	short saved; /* 1 for file has been saved, 0 for not saved */
} ch_mail_ctnt_t;

int ch_mail_ctnt_close(ch_mail_ctnt_t *ctnt);

int 
ch_mail_ctnt_append_data(ch_mail_ctnt_t *ctnt, const uint8_t *data, uint32_t data_len);

ch_mail_ctnt_t *
ch_mail_ctnt_create(ch_pool_t *mp, const char *path);

#endif

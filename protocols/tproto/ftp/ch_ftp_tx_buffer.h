/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_tx_buffer.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-06 10:50:19
 * Last Modified: 2017-01-06 10:50:19
 */


#ifndef __CH_FTP_TX_BUFFER_H__
#define __CH_FTP_TX_BUFFER_H__

#include <stdint.h>

#include "ch_string.h"

typedef struct ch_ftp_tx_buf_s {
    /** used to indicate if the current_line buffer is a malloced buffer.  We
     * use a malloced buffer, if a line is fragmented */
	uint8_t *db;
	int32_t db_len;
	uint8_t current_line_db;
    /** we have see LF for the currently parsed line */
	uint8_t current_line_lf_seen;
} ch_ftp_tx_buf_t;

typedef struct ch_ftp_curr_line_s {
	uint8_t *data;
	int32_t len;
	uint8_t delimiter_len;
} ch_ftp_curr_line_t;

static inline void ch_ftp_tx_buffer_init(ch_ftp_tx_buf_t *buf)
{
	buf->db = NULL;
	buf->db_len = 0;
	buf->current_line_db = 0;
	buf->current_line_lf_seen = 0;
}

static inline void ch_ftp_tx_buffer_clean(ch_ftp_tx_buf_t *buf)
{
	free(buf->db);
	buf->db = NULL;
}

int 
ch_ftp_get_line(ch_str_t *input, ch_ftp_tx_buf_t *tx_buf, ch_ftp_curr_line_t *curr_line);

#endif //__CH_FTP_TX_BUFFER_H__


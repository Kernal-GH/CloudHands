/*
 *
 *      Filename: protocols/mail/ch_mail_session_buffer.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-05 09:49:38
 * Last Modified: 2016-08-05 09:49:38
 */

#ifndef __CH_MAIL_SESSION_BUFFER_H__
#define __CH_MAIL_SESSION_BUFFER_H__

typedef struct ch_mail_curr_line_t ch_mail_curr_line_t;
typedef struct ch_mail_tx_buf_t ch_mail_tx_buf_t;

struct ch_mail_tx_buf_t {
    /** used to indicate if the current_line buffer is a malloced buffer.  We
     * use a malloced buffer, if a line is fragmented */
	uint8_t *db;
	int32_t db_len;
	uint8_t current_line_db;
    /** we have see LF for the currently parsed line */
	uint8_t current_line_lf_seen;
};

struct ch_mail_curr_line_t {
	uint8_t *data;
	int32_t len;
	uint8_t delimiter_len;
};

#define ch_mail_tx_buffer_init(buf) do     \
{										   \
	(buf)->db = NULL;					   \
	(buf)->db_len = 0;					   \
	(buf)->current_line_db = 0;			   \
	(buf)->current_line_lf_seen = 0;	   \
}while(0)


#define ch_mail_tx_buffer_clean(buf) free((buf)->db);

#endif //__CH_MAIL_SESSION_BUFFER_H__


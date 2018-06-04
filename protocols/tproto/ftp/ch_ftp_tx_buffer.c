/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_tx_buffer.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-10 15:41:11
 * Last Modified: 2017-01-10 17:14:11
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_ftp_tx_buffer.h"

#define SCCalloc calloc
#define SCMalloc malloc
#define SCRealloc realloc
#define SCFree free

int 
ch_ftp_get_line(ch_str_t *input, ch_ftp_tx_buf_t *tx_buf, ch_ftp_curr_line_t *curr_line)
{
	void *ptmp;

	if (input == NULL || tx_buf == NULL || curr_line == NULL) {
		CH_ERR_LOG("%s: Invalid parameter.\n", __func__);
		return -1;
	}

	/* we have run out of input */
	if (input->len <= 0)
		return -1;

	if (tx_buf->current_line_lf_seen == 1) {
		/* we have seen the lf for the previous line.  Clear the parser
		 * details to parse new line */
		tx_buf->current_line_lf_seen = 0;
		if (tx_buf->current_line_db == 1) {
			tx_buf->current_line_db = 0;
			SCFree(tx_buf->db);
			tx_buf->db = NULL;
			tx_buf->db_len = 0;
			curr_line->data = NULL;
			curr_line->len = 0;
		}
	}

	uint8_t *lf_idx = memchr(input->data, 0x0a, input->len);

	if (lf_idx == NULL) {
		/* fragmented lines.  Decoder event for special cases.  Not all
		 * fragmented lines should be treated as a possible evasion
		 * attempt.  With multi payload smtp chunks we can have valid
		 * cases of fragmentation.  But within the same segment chunk
		 * if we see fragmentation then it's definitely something you
		 * should alert about */
		if (tx_buf->current_line_db == 0) {
			tx_buf->db = SCMalloc(input->len);
			if (tx_buf->db == NULL) {
				return -1;
			}
			tx_buf->current_line_db = 1;
			memcpy(tx_buf->db, input->data, input->len);
			tx_buf->db_len = input->len;
		} else {
			ptmp = SCRealloc(tx_buf->db, (tx_buf->db_len + input->len));
			if (ptmp == NULL) {
				SCFree(tx_buf->db);
				tx_buf->db = NULL;
				tx_buf->db_len = 0;
				return -1;
			}
			tx_buf->db = ptmp;

			memcpy(tx_buf->db + tx_buf->db_len, input->data, input->len);
			tx_buf->db_len += input->len;
		} /* else */
		input->data += input->len;
		input->len = 0;

		return -1;

	} else {
		tx_buf->current_line_lf_seen = 1;

		if (tx_buf->current_line_db == 1) {
			ptmp = SCRealloc(tx_buf->db,
				(tx_buf->db_len + (lf_idx + 1 - input->data)));
			if (ptmp == NULL) {
				SCFree(tx_buf->db);
				tx_buf->db = NULL;
				tx_buf->db_len = 0;
				return -1;
			}
			tx_buf->db = ptmp;

			memcpy(tx_buf->db + tx_buf->db_len,
				input->data, (lf_idx + 1 - input->data));
			tx_buf->db_len += (lf_idx + 1 - input->data);

			if (tx_buf->db_len > 1 &&
				tx_buf->db[tx_buf->db_len - 2] == 0x0D) {
				tx_buf->db_len -= 2;
				curr_line->delimiter_len = 2;
			} else {
				tx_buf->db_len -= 1;
				curr_line->delimiter_len = 1;
			}

			curr_line->data = tx_buf->db;
			curr_line->len = tx_buf->db_len;

		} else {
			curr_line->data = input->data;
			curr_line->len = lf_idx - input->data;

			if (input->data != lf_idx &&
				*(lf_idx - 1) == 0x0D) {
				curr_line->len--;
				curr_line->delimiter_len = 2;
			} else {
				curr_line->delimiter_len = 1;
			}
		}

		input->len -= (lf_idx - input->data) + 1;
		input->data = (lf_idx + 1);

		return 0;
	}
}

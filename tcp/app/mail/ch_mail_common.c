/*
 *
 *      Filename: ./ch_parser_mail_common.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-14 14:18:06
 * Last Modified: 2018-05-18 17:33:05
 */


#include "ch_log.h"
#include "ch_list.h"
#include "ch_mail_session.h"
#include "ch_mail_session_entry.h"
#include "ch_mail_decode_mime.h"
#include "ch_mail_common.h"

struct mail_header_operator_t {
	char *name;
	size_t name_len;
	int (*oper_act)(const char *value, uint32_t vlen, ch_pool_t *pool, ch_mail_session_t *s);
};

static int 
_process_subject(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s);

static int 
_process_mail_from(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s);

static int 
_process_mail_to(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s);

static int 
_process_mail_cc(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s);

struct mail_header_operator_t mh_opt[] = {
	{
		"subject",
		7,
		_process_subject,
	},
	{
		"from",
		4,
		_process_mail_from,
	},
	{
		"to",
		2,
		_process_mail_to,
	},
	{
		"cc",
		2,
		_process_mail_cc,
	},
};

static int 
_process_subject(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s)
{
	s->subject.data = (unsigned char *)ch_pstrndup(pool, value, vlen);
	if (s->subject.data == NULL) {
		CH_ERR_LOG("%s, ch_pstrndup failed.\n", __func__);
		return -1;
	}

	s->subject.len = vlen;

	return 0;
}

static int 
_process_mail_from(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s)
{
	s->mail_from.data = (unsigned char *)ch_pstrndup(pool, value, vlen);
	if (s->mail_from.data == NULL) {
		CH_ERR_LOG("%s, ch_pstrndup failed.\n", __func__);
		return -1;
	}

	s->mail_from.len = vlen;

	return 0;
}

static int 
__process_mail_to(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s)
{
	mail_addr_node_t *mail_addr;

	mail_addr = (mail_addr_node_t *) ch_palloc(pool, sizeof(mail_addr_node_t));
	if (mail_addr == NULL) {
		CH_ERR_LOG("%s, ch_palloc failed.\n", __func__);
		return -1;
	}
	mail_addr->name.data = (unsigned char *)ch_pstrndup(pool, value, vlen);
	if (mail_addr->name.data == NULL) {
		CH_ERR_LOG("%s, ch_pstrndup failed.\n", __func__);
		return -1;
	}
	mail_addr->name.len = vlen;

	list_add_tail(&mail_addr->anchor, &s->mail_to_list);
	s->mail_to_cnt++;

	return 0;
}

static int
_process_mail_to(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s)
{
	int ret = 0;
	const char *p = value;
	const char *addr_begin = value;
	const char *end = value + vlen;

	for (; p < end; p++) {
		if (*p != ',') {
			continue;
		}

		ret = __process_mail_to(addr_begin, p - addr_begin, pool, s);

		if (ret != 0) return ret;

		addr_begin = p + 1;
	}

	return __process_mail_to(addr_begin, p - addr_begin, pool, s);
}

static int 
__process_mail_cc(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s)
{
	mail_addr_node_t *mail_addr;

	mail_addr = (mail_addr_node_t *) ch_palloc(pool, sizeof(mail_addr_node_t));
	if (mail_addr == NULL) {
		CH_ERR_LOG("%s, ch_palloc failed.\n", __func__);
		return -1;
	}
	mail_addr->name.data = (unsigned char *)ch_pstrndup(pool, value, vlen);
	if (mail_addr->name.data == NULL) {
		CH_ERR_LOG("%s, ch_pstrndup failed.\n", __func__);
		return -1;
	}
	memcpy(mail_addr->name.data, value, vlen);
	mail_addr->name.len = vlen;

	list_add_tail(&mail_addr->anchor, &s->mail_cc_list);
	s->mail_cc_cnt++;

	return 0;
}

static int 
_process_mail_cc(const char *value, uint32_t vlen, ch_pool_t *pool,
	ch_mail_session_t *s)
{
	int ret = 0;
	const char *p = value;
	const char *addr_begin = value;
	const char *end = value + vlen;

	for (; p < end; p++) {
		if (*p != ',') {
			continue;
		}

		ret = __process_mail_cc(addr_begin, p - addr_begin, pool, s);

		if (ret != 0) return ret;

		addr_begin = p + 1;
	}

	return __process_mail_cc(addr_begin, p - addr_begin, pool, s);
}

static int 
mail_process_header(const uint8_t *name, uint32_t nlen, const uint8_t *value,
	uint32_t vlen, ch_pool_t *pool, ch_mail_session_t *s)
{
	struct mail_header_operator_t *opt;
	size_t i = 0;

	if (!name || nlen == 0 || !value || vlen == 0 || !pool || !s) {
		CH_ERR_LOG("%s: Invalid Parameter.\n", __func__);
		return -1;
	}

	for (i = 0; i < sizeof (mh_opt) / sizeof (struct mail_header_operator_t); i++) {
		opt = &mh_opt[i];

		if (opt->name_len == nlen && \
			strncasecmp((const char *)name, opt->name, nlen) == 0) 
		{
			return opt->oper_act((const char *)value, vlen, pool, s);
		}
	}	

	return 0;
}

static int _mail_process_data(const uint8_t *chunk, uint32_t len, 
						MimeDecParseState *state, ch_mail_ctnt_t *ctnt)
{
	int ret = MIME_DEC_OK;

	if (ch_mail_ctnt_append_data(ctnt, (uint8_t *) chunk, len) < 0) {
		ret = MIME_DEC_ERR_DATA;
		CH_ERR_LOG("%s: file_append_data failed: %d", __func__, ret);
	}

	/* append data */
	if (state->body_end) {
		CH_DEBUG_LOG("%s: Closing file...%u bytes\n", __func__, len);

		ret = ch_mail_ctnt_close(ctnt);
		if (ret != 0) {
			ret = MIME_DEC_ERR_DATA;
			CH_ERR_LOG("FileCloseFile() failed: %d", ret);
		}
	}
	
	return ret;
}

static ch_mail_ctnt_t *
_get_file_node_for_mail_ctnt_save(ch_pool_t *pool, 
	ch_proto_body_store_t *bstore, int is_attach)
{
	ch_mail_ctnt_t *ctnt;
	char *fname, *fname_tmp;
	ch_fpath_t *fpath = is_attach ? bstore->res_fpath : bstore->req_fpath;

	ch_fpath_name_create(fpath, &fname, &fname_tmp);
	if (fname == NULL) {
		CH_ERR_LOG("Can not create a file used to store mail content.\n");
		return NULL;
	}

	ctnt = ch_mail_ctnt_create(pool, (const char*)fname);
	if (ctnt == NULL) {
		return NULL;
	}

	return ctnt;
}

static int 
mail_process_content_plain_text(ch_proto_body_store_t *bstore,const uint8_t *chunk, uint32_t len,
	MimeDecParseState *state, ch_mail_session_t *s)
{

	ch_mail_ctnt_t *ctnt = NULL;

	ch_mail_session_entry_t *sentry = NULL;

	sentry = (ch_mail_session_entry_t *) state->data;

	if (state->body_begin) {
		/* get file path and append data */
		ctnt = _get_file_node_for_mail_ctnt_save(sentry->mp,bstore, 0);

		if (ctnt == NULL) {
			CH_ERR_LOG("%s: add file_node_s failed.\n", __func__);
			return MIME_DEC_ERR_DATA;
		}
		s->ctnt_plain_file = ctnt;
	} else {
		ctnt = s->ctnt_plain_file;
	}
	if (ctnt == NULL) {
		CH_ERR_LOG("%s: ctnt is null", __func__);
		return MIME_DEC_ERR_MEM;
	}

	return _mail_process_data(chunk, len, state, ctnt);
}

static int 
mail_process_content_html(ch_proto_body_store_t *bstore,const uint8_t *chunk, uint32_t len,
	MimeDecParseState *state, ch_mail_session_t *s)
{
	ch_mail_ctnt_t *ctnt = NULL;
	ch_mail_session_entry_t *sentry = NULL;

	sentry = (ch_mail_session_entry_t *) state->data;

	if (state->body_begin) {
		/* get file path and append data */
		ctnt = _get_file_node_for_mail_ctnt_save(sentry->mp,bstore, 0);

		if (ctnt == NULL) {
			CH_ERR_LOG("%s: add file_node_s failed.\n", __func__);
			return MIME_DEC_ERR_DATA;
		}
		s->ctnt_html_file= ctnt;
	} else {
		ctnt = s->ctnt_html_file;
	}
	if (ctnt == NULL) {
		CH_ERR_LOG("%s: ctnt is null", __func__);
		return MIME_DEC_ERR_MEM;
	}

	return _mail_process_data(chunk, len, state, ctnt);
}

static int 
mail_process_attachment(ch_proto_body_store_t *bstore,const uint8_t *chunk, uint32_t len, 
	MimeDecParseState *state, ch_mail_session_t *s)
{
	ch_mail_ctnt_t *ctnt = NULL;
	ch_mail_session_entry_t *sentry = NULL;
	MimeDecEntity *entity = (MimeDecEntity *) state->stack->top->data;

	sentry = (ch_mail_session_entry_t *) state->data;

	/* Open file if necessary */
	if (state->body_begin) {
		/* get file path and append data */
		ctnt = _get_file_node_for_mail_ctnt_save(sentry->mp, bstore, 1);
								
		if (ctnt == NULL) {
			CH_ERR_LOG("%s: add file_node_s failed.\n", __func__);
			return MIME_DEC_ERR_DATA;
		}
		ctnt->filename = ch_pstrndup(sentry->mp, (const char *)entity->filename, 
			entity->filename_len);

		list_add_tail(&ctnt->anchor, &s->attach_list);
		s->attach_cnt++;
	} else {
		ctnt = list_last_entry(&s->attach_list, ch_mail_ctnt_t, anchor);
	}
	if (ctnt == NULL) {
		CH_ERR_LOG("%s: file is null", __func__);
		return MIME_DEC_ERR_MEM;
	}

	return _mail_process_data(chunk, len, state, ctnt);
}

/**
 * \internal
 * \brief Get the next line from input.  It doesn't do any length validation.
 *
 * \param state The smtp state.
 *
 * \retval  0 On suceess.
 * \retval -1 Either when we don't have any new lines to supply anymore or
 *            on failure.
 */
int 
ch_mail_get_line(ch_str_t *input, ch_mail_tx_buf_t *tx_buf, 
	ch_mail_curr_line_t *curr_line)
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

int 
MailProcessMimeHeader(const uint8_t *name, uint32_t nlen, 
	const uint8_t *value, uint32_t vlen, MimeDecParseState *state)
{
	ch_mail_session_entry_t *sentry = NULL;
	ch_mail_session_t *s = NULL;

	sentry = (ch_mail_session_entry_t *) state->data;

	s = (ch_mail_session_t *) sentry->curr_session;

	mail_process_header(name, nlen, value, vlen, sentry->mp, s);

	return 0;
}

int 
MailProcessDataChunk(const uint8_t *chunk, uint32_t len, MimeDecParseState *state)
{
	int ret = 0;
	ch_mail_session_entry_t *sentry = NULL;
    MimeDecEntity *entity = (MimeDecEntity *) state->stack->top->data;
    ch_mail_session_t *s = NULL;

	sentry = (ch_mail_session_entry_t *) state->data;

	ch_proto_body_store_t *bstore = sentry->bstore;

	s = (ch_mail_session_t *) sentry->curr_session;

	if (entity->ctnt_flags & CTNT_IS_ATTACHMENT) {
		ret = mail_process_attachment(bstore,chunk, len, state, s);
	} else if (entity->ctnt_flags & CTNT_IS_TEXT) {
		ret = mail_process_content_plain_text(bstore,chunk, len, state, s);
	} else if (entity->ctnt_flags & CTNT_IS_HTML) {
		ret = mail_process_content_html(bstore,chunk, len, state, s);
	}

	if (ret != 0) {
		CH_ERR_LOG("%s: process data(%s) failed.\n", __func__, 
			(entity->ctnt_flags & CTNT_IS_ATTACHMENT) ? "attach" : "content");
	}

	return ret;
}

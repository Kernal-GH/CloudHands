/*
 *
 *      Filename: ./protocols/imap/imap_parser.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-05 19:00:12
 * Last Modified: 2017-01-10 16:36:59
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ch_log.h"
#include "apr_pools.h"
#include "apr_strings.h"
#include "imap_parser.h"
#include "ch_mail_session.h"
#include "ch_mmap_file_format.h"
#include "ch_mail_session_entry.h"
#include "ch_mail_session_format_msgpack.h"

#define SSL3_FIRST_BYTE 0x16
#define SSL3_SECOND_BYTE 0x03
#define SSL2_CHELLO_BYTE 0x01
#define SSL2_SHELLO_BYTE 0x04

#define IMAP_RET_OK		1
#define IMAP_RET_OK_STR	"OK"
#define IMAP_RET_OK_LEN 2

#define IMAP_RET_NO		2
#define IMAP_RET_NO_STR	"NO"
#define IMAP_RET_NO_LEN 2

#define IMAP_RET_BAD		3
#define IMAP_RET_BAD_STR	"BAD"
#define IMAP_RET_BAD_LEN 3

#define IMAP_RET_PREAUTH		4
#define IMAP_RET_PREAUTH_STR	"PREAUTH"
#define IMAP_RET_PREAUTH_LEN	7 

#define IMAP_RET_BYE		5
#define IMAP_RET_BYE_STR	"BYE"
#define IMAP_RET_BYE_LEN	3

#define IMAP_STATE_CMD					0x00
#define IMAP_STATE_DATA_APPEND			0x01
#define IMAP_STATE_DATA_UID_FETCH		0x02
#define IMAP_STATE_AUTH					0x04
#define IMAP_STATE_TRANS				0x08
#define IMAP_STATE_MULTILINE_REPLY		0x10
#define IMAP_STATE_PARSING_SVR_RESPONSE	0x20
#define IMAP_STATE_STARTTLS				0x40
#define IMAP_STATE_FIRST_REPLY_SEEN		0x80

#define IMAP_CMD_STARTTLS	1
#define IMAP_CMD_FETCH		2
#define IMAP_CMD_UID_FETCH	3
#define IMAP_CMD_LOGIN		4
#define IMAP_CMD_LOGOUT		5
#define IMAP_CMD_APPEND		6
#define IMAP_CMD_OTHER_CMD	7

static void *ch_mail_imap_state_create(apr_pool_t *pool)
{
	if (pool == NULL) {
		return NULL;
	}

	return (imap_state_t *) apr_pcalloc(pool, sizeof(imap_state_t));
}

static int imap_process_cmd_starttls(ch_mail_session_entry_t *sentry,
	ch_mail_curr_line_t *cur_line)
{
	/* unused */
	sentry = sentry;
	cur_line = cur_line;

	return 0;
}

static int 
imap_mime_decode_parse(ch_mail_session_entry_t *sentry,
	ch_mail_curr_line_t *cur_line)
{
	int ret = 0;
	ch_mail_session_t *s = sentry->curr_session;
	imap_state_t *state = (imap_state_t *) sentry->priv_data;

	if (s->mime_state) {
		ret = MimeDecParseLine((const uint8_t *) cur_line->data,
			cur_line->len, cur_line->delimiter_len, s->mime_state);

		if (ret != MIME_DEC_OK) {
			CH_ERR_LOG("%s: MimeDecParseLine() function returned an error code: %d",
				__func__, ret);
		}
	}
	state->body_read += cur_line->len;
	state->body_read += cur_line->delimiter_len;

	return ret;
}

static int 
imap_process_cmd_append(ch_mail_session_entry_t *sentry,
	ch_mail_curr_line_t *cur_line)
{
    if (!(sentry->parser_state & IMAP_STATE_DATA_APPEND)) {
        /* looks like are still waiting for a confirmination from the server */
        return 0;
    }

	return imap_mime_decode_parse(sentry, cur_line);
}

static int NoNewTx(ch_mail_session_entry_t *sentry,
	ch_mail_curr_line_t *cur_line)
{
	char *ptr;
	uint32_t len = cur_line->len;

	ptr = strstr((char *)cur_line->data, " ");
	if (ptr == NULL)
		return -1;

	if (ptr < (char *)cur_line->data + cur_line->len) {
		ptr++;
	}
	len = cur_line->len - (ptr - (char *)cur_line->data);

	if (!(sentry->parser_state & IMAP_STATE_DATA_APPEND)) {
		if (len >= 6 &&
			strncasecmp("logout", (char *)cur_line->data, 6) == 0) {
			return 1;
		}
	}
	return 0;
}

static int 
imap_parse_command_login(ch_mail_session_entry_t *sentry, 
	ch_mail_curr_line_t *cur_line)
{
	uint8_t *tmp;
	uint8_t *ptr, *end;
	apr_pool_t *pool = sentry->mp;

	ptr = cur_line->data;
	end = cur_line->data + cur_line->len;

	ptr = (uint8_t *)strstr((char *)cur_line->data, "LOGIN");
	if (ptr == NULL)
		return -1;

	while(ptr < end && *ptr != ' ') {
		ptr++;
	}
	if (ptr < end) ptr++;

	tmp = ptr;
	while(ptr < end && *ptr != ' ') {
		ptr++;
	}
	sentry->login.data = (unsigned char *) apr_pstrndup(pool, (const char *)tmp, ptr - tmp);
	if (sentry->login.data == NULL)
		return -1;

	sentry->login.len = ptr - tmp;

	if (ptr < end) ptr++;

	sentry->passwd.data = (unsigned char *) apr_pstrndup(pool, (const char *)ptr, end - ptr);
	if (sentry->passwd.data == NULL)
		return -1;

	sentry->passwd.len = end - ptr;

	return 0;
}

static int 
imap_process_request(ch_mail_session_entry_t *sentry, ch_mail_curr_line_t *cur_line)
{
	ch_mail_session_t *s = sentry->curr_session;
	apr_pool_t *pool = sentry->mp;
	int r = 0;
	char *ptr;
	uint32_t len;

	if (s == NULL || (s->done && !NoNewTx(sentry, cur_line))) {
		s = ch_mail_session_create(pool);
		if (s == NULL)
			return -1;
		sentry->curr_session = s;
		list_add_tail(&s->anchor, &sentry->session_list);
	}

	/* if not in data mode */
	if (!(sentry->parser_state & IMAP_STATE_DATA_APPEND)) {
		/* Note: find the first space, and the command followed, eg.:
		 * "C1 CAPABILITY" */
		ptr = strstr((char *)cur_line->data, " ");
		if (ptr == NULL) {
			return -1;
		}
		if (ptr < (char *)cur_line->data + cur_line->len)
			ptr++;

		len = cur_line->len - (ptr - (char *)cur_line->data);

		if (len >= 8 && \
			strncasecmp("starttls", (char *)ptr, 8) == 0) {
			sentry->current_command = IMAP_CMD_STARTTLS;
		} else if (len >= 9 && \
			strncasecmp("UID FETCH", (char *)ptr, 9) == 0) {
			sentry->current_command = IMAP_CMD_UID_FETCH;
		} else if (len >= 6 && \
			strncasecmp("APPEND", (char *)ptr, 6) == 0) {
			sentry->current_command = IMAP_CMD_APPEND;
		} else if (len >= 6 && \
			strncasecmp("LOGOUT", (char *)ptr, 6) == 0) {
			sentry->current_command = IMAP_CMD_LOGOUT;
		} else if (len >= 5 && \
			strncasecmp("LOGIN", (char *)ptr, 5) == 0) {
			r = imap_parse_command_login(sentry, cur_line);
			if (r == -1) {
				CH_ERR_LOG("imap_parse_command_login failed.\n");
				return -1;
			}
			sentry->current_command = IMAP_CMD_OTHER_CMD;
		} else {
			sentry->current_command = IMAP_CMD_OTHER_CMD;
		}

		return r;
	}

	switch (sentry->current_command) {
	case IMAP_CMD_STARTTLS:
		imap_process_cmd_starttls(sentry, cur_line);
		break;
	case IMAP_CMD_APPEND:
		imap_process_cmd_append(sentry, cur_line);
		break;
	default:
		break;
	}

	return 0;
}

static int imap_get_reply_code(uint8_t *line, int32_t len)
{
	char *ptr = NULL;

	if (line == NULL || len == 0) {
		CH_DEBUG_LOG("%s: no input.\n", __func__);
		return 0;
	}

	ptr = strstr((char *)line, " ");
	if (ptr == NULL)
		return 0;

	if (ptr < (char *)line + len)
		ptr++;

	if (strncmp(ptr, IMAP_RET_OK_STR, IMAP_RET_OK_LEN) == 0) {
		return IMAP_RET_OK;
	} else if (strncmp(ptr, IMAP_RET_NO_STR, IMAP_RET_NO_LEN) == 0) {
		return IMAP_RET_NO;
	} else if (strncmp(ptr, IMAP_RET_BAD_STR, IMAP_RET_BAD_LEN) == 0) {
		return IMAP_RET_BAD;
	} else if (strncmp(ptr, IMAP_RET_PREAUTH_STR, IMAP_RET_PREAUTH_LEN) == 0) {
		return IMAP_RET_PREAUTH;
	} else if (strncmp(ptr, IMAP_RET_BYE_STR, IMAP_RET_BYE_LEN) == 0) {
		return IMAP_RET_BYE;
	}

	return 0;
}

int imap_create_new_mime_parser_state(ch_mail_session_entry_t *sentry)
{
	ch_mail_session_t *s = sentry->curr_session;

	if (s->mime_state) {
		s->mime_state->state_flag = MIME_PARSE_ERROR;
		s = ch_mail_session_create(sentry->mp);
		if (s == NULL)
			return -1;
		sentry->curr_session = s;
		list_add_tail(&s->anchor, &sentry->session_list);
	}

	s->mime_state = MimeDecInitParser(sentry, 
		MailProcessDataChunk, MailProcessMimeHeader);
	if (s->mime_state == NULL) {
		CH_ERR_LOG("%s: MimeDecInitParser() failed to allocate data", __func__);
		return MIME_DEC_ERR_MEM;
	}

	/* Add new MIME message to end of list */
	if (s->msg_head == NULL) {
		s->msg_head = s->mime_state->msg;
		s->msg_tail = s->mime_state->msg;
	} else {
		s->msg_tail->next = s->mime_state->msg;
		s->msg_tail = s->mime_state->msg;
	}

	return 0;
}

static void 
imap_update_multi_reply_state(ch_mail_session_entry_t *sentry, 
	ch_mail_curr_line_t *cur_line)
{
	/* set IMAP_STATE_MULTILINE_REPLY state */
	if (cur_line->len >= 1) {
		if (sentry->parser_state & IMAP_STATE_MULTILINE_REPLY) {
			if (cur_line->data[0] != '*' && \
				!(sentry->parser_state & IMAP_STATE_DATA_UID_FETCH)) 
			{
				sentry->parser_state &= ~IMAP_STATE_MULTILINE_REPLY;
			}
		} else {
			if (cur_line->data[0] == '*' && \
				(sentry->parser_state & IMAP_STATE_FIRST_REPLY_SEEN)) {
				sentry->parser_state |= IMAP_STATE_MULTILINE_REPLY;
			} else if (cur_line->data[0] == '+') {
				if (sentry->current_command == IMAP_CMD_APPEND) {
					sentry->parser_state |= IMAP_STATE_DATA_APPEND;
				}
			}
		}
	} else {
		if (sentry->parser_state & IMAP_STATE_MULTILINE_REPLY && \
				!(sentry->parser_state & IMAP_STATE_DATA_UID_FETCH)) {
			sentry->parser_state &= ~IMAP_STATE_MULTILINE_REPLY;
		}
	}

	if (!(sentry->parser_state & IMAP_STATE_FIRST_REPLY_SEEN)) {
		if (!(sentry->parser_state & IMAP_STATE_MULTILINE_REPLY)) {
			sentry->parser_state |= IMAP_STATE_FIRST_REPLY_SEEN;
		}
	}

	return;
}

static int imap_set_mime_decode_complete(ch_mail_session_entry_t *sentry)
{
	ch_mail_session_t *s = sentry->curr_session;
	int ret = -1;

	if (s->mime_state != NULL) {
		ret = MimeDecParseComplete(s->mime_state);
		if (ret != MIME_DEC_OK) {
			CH_ERR_LOG("MimeDecParseComplete() function failed");
		}
	}
	s->done = 1;

	mail_format_cur_session(&sentry->mpproto->aproto, 
		sentry, s);

	return ret;
}

static int 
imap_process_uid_fetch_data(ch_mail_session_entry_t *sentry, 
	ch_mail_curr_line_t *cur_line)
{
	imap_state_t *state = (imap_state_t *) sentry->priv_data;

	if (cur_line->len == 1 && cur_line->data[0] == ')') {
		sentry->parser_state &= ~IMAP_STATE_DATA_UID_FETCH;
		imap_set_mime_decode_complete(sentry);
		sentry->current_command = IMAP_CMD_OTHER_CMD;
		state->body_len = state->body_read = 0;
		return 0;
	}

	if (state->body_len > state->body_read) {
		return imap_mime_decode_parse(sentry, cur_line);
	} 

	return -1;
}

static int 
imap_process_reply(ch_mail_session_entry_t *sentry,
	ch_mail_curr_line_t *cur_line)
{
	uint8_t reply_code = 0;
	uint8_t *ptr;
	uint8_t *end;
	uint8_t *body_start, *body_end;
	uint32_t len = 0;
	char *eptr;
	char *tmp;
	imap_state_t *state = (imap_state_t *) sentry->priv_data;

	ptr = cur_line->data;
	end = ptr + cur_line->len;

	imap_update_multi_reply_state(sentry, cur_line);

	if (!(sentry->parser_state & IMAP_STATE_MULTILINE_REPLY)) {
		reply_code = imap_get_reply_code(cur_line->data, cur_line->len);
		/* reply_code > 0 means a reply code is detected. */
		if (reply_code > 0) {
			if (sentry->current_command == IMAP_CMD_STARTTLS) {
				if (reply_code == IMAP_RET_OK) {
					sentry->parser_state |= IMAP_STATE_STARTTLS;
					return -1;
				}
			} else if (sentry->current_command == IMAP_CMD_APPEND) {
				if (reply_code > 0 && (sentry->parser_state & IMAP_STATE_DATA_APPEND)) {
					sentry->parser_state &= ~IMAP_STATE_DATA_APPEND;
					imap_set_mime_decode_complete(sentry);
					sentry->current_command = IMAP_CMD_OTHER_CMD;
				}
			}
		}
	} else {
		/* if state IMAP_STATE_MULTILINE_REPLY IMAP_STATE_DATA_UID_FETCH is set
		 * and we should handle UID FETCH data. */
		if (sentry->parser_state & IMAP_STATE_DATA_UID_FETCH) {
			return imap_process_uid_fetch_data(sentry, cur_line);
		}
	}

	/* get UID FETCH body length and set parse state */
	if (sentry->current_command == IMAP_CMD_UID_FETCH) {
		body_start = body_end = NULL;
		state->body_len = state->body_read = 0;
		sentry->parser_state &= ~IMAP_STATE_DATA_UID_FETCH;

		tmp = ch_strlcasestrn((char *)ptr, (char *)end, "BODY", 4 - 1);
		if (tmp != NULL) {
			sentry->parser_state |= IMAP_STATE_DATA_UID_FETCH;
		} else {
			tmp = ch_strlcasestrn((char *)ptr, (char *)end, "RFC822", 6 - 1);
			if (tmp != NULL) {
				sentry->parser_state |= IMAP_STATE_DATA_UID_FETCH;
			} 
		}
		if (sentry->parser_state & IMAP_STATE_DATA_UID_FETCH) {
			body_start = (uint8_t *)memchr((char *)ptr, '{', (end - ptr));
			if (body_start == NULL) {
				sentry->parser_state &= ~IMAP_STATE_DATA_UID_FETCH;
			} else {
				if (body_start + 1 < end) {
					len = strtoul((char *)body_start + 1, &eptr, 10);
					if (*eptr != '}') {
						sentry->parser_state &= ~IMAP_STATE_DATA_UID_FETCH;
					} else {
						state->body_len = len;
					}
					len = 0;
				} else {
					sentry->parser_state &= ~IMAP_STATE_DATA_UID_FETCH;
				}
			}
		}
		if ((sentry->parser_state & IMAP_STATE_DATA_UID_FETCH) || \
			(sentry->parser_state & IMAP_STATE_DATA_APPEND)) {

			imap_create_new_mime_parser_state(sentry);
		}
	} 

	return 0;
}

static int imap_parse(int dir, ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, uint8_t *input, uint32_t input_len)
{
	/* unused */
	mpproto = mpproto;

	ch_mail_curr_line_t cur_line;
	ch_str_t input_data;

    if (input == NULL || input_len == 0) {
		CH_ERR_LOG("%s: no input.\n", __func__);
        return PARSE_CONTINUE;
    }

	input_data.data = input;
	input_data.len = input_len;

    /* toserver */
    if (dir == 0) {
        while (ch_mail_get_line(&input_data, &sentry->cur_req_buf, &cur_line) >= 0)
		{
            if (imap_process_request(sentry, &cur_line) == -1)
				return PARSE_BREAK;
        }

        /* toclient */
    } else {
		while (ch_mail_get_line(&input_data, &sentry->cur_res_buf, &cur_line) >= 0) 
		{
            if (imap_process_reply(sentry, &cur_line) == -1)
				return PARSE_BREAK;
        }
    }

	return PARSE_CONTINUE;
}

int mail_imap_request_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	/* unused */
	priv_data = priv_data;

	if (sentry->priv_data == NULL) {
		sentry->priv_data = ch_mail_imap_state_create(sentry->mp);

		if (sentry->priv_data == NULL) {
			CH_ERR_LOG("create imap parser state failed.\n");
			return PARSE_DONE;
		}
	}

	return imap_parse(0, mpproto, sentry, data, dlen);
}

int mail_imap_response_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	/* unused */
	priv_data = priv_data;
	
	if (sentry->priv_data == NULL) {
		sentry->priv_data = ch_mail_imap_state_create(sentry->mp);

		if (sentry->priv_data == NULL) {
			CH_ERR_LOG("create imap parser state failed.\n");
			return PARSE_DONE;
		}
	}

	return imap_parse(1, mpproto, sentry, data, dlen);
}


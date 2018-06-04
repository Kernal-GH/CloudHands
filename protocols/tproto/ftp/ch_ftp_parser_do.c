/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_parser_do.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-05 17:35:17
 * Last Modified: 2017-01-11 11:51:17
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apr_strings.h>

#include "ch_log.h"
#include "ch_string.h"
#include "ch_proto_pool.h"
#include "ch_ftp_parser_do.h"
#include "ch_session_entry.h"
#include "ch_ftp_tx_buffer.h"
#include "ch_ftp_session_entry.h"

static int
_ch_ftp_get_request_command(ch_ftp_curr_line_t *cur_line)
{
	uint8_t *ptr = cur_line->data;

	if (strncasecmp("USER", (char *)ptr, 4) == 0) {
		return FTP_COMMAND_USER;
	} else if (strncasecmp("PASS", (char *)ptr, 4) == 0) {
		return FTP_COMMAND_PASS;
	}

	return FTP_COMMAND_UNKNOWN;
}

static void
_ch_ftp_get_user(ch_ftp_session_entry_t *fsentry, ch_ftp_session_t *session, ch_ftp_curr_line_t *cur_line)
{
	uint8_t *ptr = NULL;
	uint8_t *end = cur_line->data + cur_line->len;
	apr_pool_t *mp = fsentry->mp;

	ptr = cur_line->data + 4;

	if (ptr >= end) return;

	ptr++;

	session->user.data = (unsigned char *) apr_pstrndup(mp, (const char *) ptr, end - ptr);
	session->user.len = end -ptr;
}

static void
_ch_ftp_get_pass(ch_ftp_session_entry_t *fsentry, ch_ftp_session_t *session, ch_ftp_curr_line_t *cur_line)
{
	uint8_t *ptr = NULL;
	uint8_t *end = cur_line->data + cur_line->len;
	apr_pool_t *mp = fsentry->mp;

	ptr = cur_line->data + 4;

	if (ptr >= end) return;

	ptr++;

	session->passwd.data = (unsigned char *) apr_pstrndup(mp, (const char *) ptr, end - ptr);
	session->passwd.len = end -ptr;
}

static int
_ch_ftp_process_request(ch_ftp_session_entry_t *fsentry, ch_ftp_curr_line_t *cur_line)
{
	ch_ftp_session_t *session = fsentry->session;
	int cmd = _ch_ftp_get_request_command(cur_line);

	if (cmd  == FTP_COMMAND_USER) {
		_ch_ftp_get_user(fsentry, session, cur_line);
	} else if (cmd == FTP_COMMAND_PASS) {
		_ch_ftp_get_pass(fsentry, session, cur_line);
	}

	ch_ftp_cmds_data_append_data(session->cmds_data, (const char *) cur_line->data, cur_line->len);
	ch_ftp_cmds_data_append_data(session->cmds_data, "\n", 1);

	return 0;
}

static int
_ch_ftp_process_response(ch_ftp_session_entry_t *fsentry, ch_ftp_curr_line_t *cur_line)
{
	ch_ftp_session_t *session = fsentry->session;

	ch_ftp_cmds_data_append_data(session->cmds_data, (const char *) cur_line->data, cur_line->len);
	ch_ftp_cmds_data_append_data(session->cmds_data, "\n", 1);

	return 0;
}

static int
_ch_ftp_handle_data(ch_ftp_session_entry_t *fsentry, char *buf, int buf_len, int dir)
{
	ch_ftp_curr_line_t cur_line;
	ch_str_t input_data;

	if (buf == NULL || buf_len == 0) {
		CH_ERR_LOG("%s: no input data.", __func__);
		return PARSE_CONTINUE;
	}

	input_data.data = (unsigned char *) buf;
	input_data.len = buf_len;

	/* toserver */
	if (dir == 0) {
		while (ch_ftp_get_line(&input_data, &fsentry->cur_req_buf, &cur_line) >= 0) {
			if (_ch_ftp_process_request(fsentry, &cur_line) == -1)
				return PARSE_BREAK;
		}
	/* toclient */
	} else {
		while (ch_ftp_get_line(&input_data, &fsentry->cur_res_buf, &cur_line) >= 0) {
			if (_ch_ftp_process_response(fsentry, &cur_line) == -1)
				return PARSE_BREAK;
		}
	}

	return PARSE_CONTINUE;
}

static ch_ftp_session_t *
_ch_ftp_get_session(ch_ftp_session_entry_t *fsentry)
{
	ch_ftp_session_t *session = NULL;

	if (fsentry->session != NULL) {
		return fsentry->session;
	}

	session = ch_ftp_session_create(fsentry->mp);
	if (session == NULL) {
		ch_log(CH_LOG_ERR, "%s: create ftp session failed.\n", __func__);
		return NULL;
	}

	session->cmds_data = ch_ftp_cmds_data_create(fsentry->mp, fsentry->fpproto->cmd_save_fpath);
	if (session->cmds_data == NULL) {
		ch_log(CH_LOG_ERR, "create cmds data failed.", __func__);
		return NULL;
	}

	fsentry->session = session;

	return session;
}

int ch_ftp_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	ch_ftp_session_entry_t *fsentry = (ch_ftp_session_entry_t *) sentry->priv_data;
	ch_ftp_session_t *session = _ch_ftp_get_session(fsentry);

	aproto = aproto;
	priv_data = priv_data;

	if (session == NULL) {
		ch_log(CH_LOG_ERR, "%s: ftp session is null.", __func__);
		return PARSE_BREAK;
	}

	if (data == NULL || dlen == 0) {
		return PARSE_CONTINUE;
	}

	return _ch_ftp_handle_data(fsentry, data, dlen, 0);
}

int ch_ftp_response_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	ch_ftp_session_entry_t *fsentry = (ch_ftp_session_entry_t *) sentry->priv_data;
	ch_ftp_session_t *session = _ch_ftp_get_session(fsentry);

	aproto = aproto;
	priv_data = priv_data;

	if (session == NULL) {
		ch_log(CH_LOG_ERR, "%s: ftp session is null.", __func__);
		return PARSE_BREAK;
	}

	if (data == NULL || dlen == 0) {
		return PARSE_CONTINUE;
	}

	return _ch_ftp_handle_data(fsentry, data, dlen, 1);
}

/*
 *
 *      Filename: ch_telnet_parser.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-08 13:52:26
 * Last Modified: 2016-12-30 18:37:03
 */


#include <stdio.h>
#include <stdlib.h>
#include <arpa/telnet.h>

#include "ch_log.h"
#include "ch_telnet_session.h"
#include "ch_telnet_parser_do.h"
#include "ch_telnet_session_entry.h"

typedef unsigned char byte;

#define DUMP_FILE

#ifdef DUMP_FILE

#define DUMP_FILE_NAME "./debug_dump_file.txt"

#define LOG_DATA(fmt, args...) do { \
	int fd = open(DUMP_FILE_NAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR); \
	write(fd, fmt, ##args); \
	close(fd); \
} while(0)

#else

#define LOG_DATA(fmt, args...) do { \
	fprintf(stderr, fmt, ##args); \
} while(0) 

#endif

#define CHANGE_TELNET_PARSER_STATE(session, dir, state) \
	((dir) == 0 ? ((session)->cli_state = (state)) : ((session)->svr_state = (state)))

#define TELNET_PARSER_STATE(session, dir) \
	((dir) == 0 ? (session)->cli_state : (session)->svr_state)

#if 0
static void telopt(byte c)
{
	fprintf(stderr, " %s", TELOPT(c));
}
#endif

static void subneg(ch_telnet_session_t *session, byte c, int dir)
{
	char tmp[128] = {0};

	switch (TELNET_PARSER_STATE(session, dir)) {
	case TS_SUB1:
		if (c == IAC) {
			//state->telstate = TS_SUB2;
			CHANGE_TELNET_PARSER_STATE(session, dir, TS_SUB2);
		} else {
			//fprintf(stderr, " %s", TELCMD(c));
			//fprintf(stderr, " %d", c);
			snprintf(tmp, 128, " %d", c);
			ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
		}
		break;

	case TS_SUB2:
		if (c == SE) {
			//state->telstate = TS_COPY;
			CHANGE_TELNET_PARSER_STATE(session, dir, TS_COPY);

			//fprintf(stderr, " %s\n", TELCMD(c));
			snprintf(tmp, 128, " %s\n", TELCMD(c));
			ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));

			return;
		}

		//state->telstate = TS_SUB1;
		CHANGE_TELNET_PARSER_STATE(session, dir, TS_SUB1);
		break;
	}
}

static int _handle_data(ch_telnet_session_t *session, char *buf, int buf_len, int dir)
{
	int i = 0;
	char tmp[128] = {0};
	//int cstart = 0;

	for (i = 0; i < buf_len; i++) {
		byte c = buf[i];

		//if (state->telstate == TS_NORMAL) {
		if (TELNET_PARSER_STATE(session, dir) == TS_NORMAL) {
			if (c == IAC) {
				//state->telstate = TS_IAC;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_IAC);
			} else if (c == '\r') {
				//state->telstate = TS_CR;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_CR);
			} else {
				//fprintf(stderr, "%c", c);
				snprintf(tmp, 128, "%c", c);
				ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
			}

			continue;
		}

		switch (TELNET_PARSER_STATE(session, dir)) {
		case TS_CR:
			//state->telstate = TS_COPY;
			CHANGE_TELNET_PARSER_STATE(session, dir, TS_COPY);
			if (c == '\0') {
				fprintf(stderr, "break");
				break;
			}

		case TS_COPY:
			if (c == IAC) {
				//state->telstate = TS_IAC;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_IAC);
			} else {
				//fprintf(stderr, "%c", c);
				snprintf(tmp, 128, "%c", c);
				ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
			}

			if (c == '\r') {
				//state->telstate = TS_CR;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_CR);
			}

			break;

		case TS_IAC:
			/* IAC IAC -> one IAC */
			if (c == IAC) {
				//state->telstate = TS_COPY;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_COPY);
				//fprintf(stderr, "%c", c);
				snprintf(tmp, 128, "%c", c);
				ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
				break;
			}
			/* else */
			switch (c) {
			case SB:
				//fprintf(stderr, " %s", TELCMD(c));
				snprintf(tmp, 128, " %s", TELCMD(c));
				ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
				//state->telstate = TS_SUB1;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_SUB1);
				break;

			case DO:
			case DONT:
			case WILL:
			case WONT:
				//fprintf(stderr, " %s", TELCMD(c));
				snprintf(tmp, 128, " %s", TELCMD(c));
				ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
				//state->telstate = TS_OPT;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_OPT);
				break;
			default:
				//state->telstate = TS_COPY;
				CHANGE_TELNET_PARSER_STATE(session, dir, TS_COPY);
				//fprintf(stderr, "\n");
				snprintf(tmp, 128, "\n");
				ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
			}
			break;

		case TS_OPT:
			//telopt(c);
			//state->telstate = TS_COPY;
			CHANGE_TELNET_PARSER_STATE(session, dir, TS_COPY);
			//fprintf(stderr, "\n");
			snprintf(tmp, 128, "\n");
			ch_telnet_transfer_data_append_data(session->trans_data, tmp, strlen(tmp));
			break;
		case TS_SUB1:
		case TS_SUB2:
			subneg(session, c, dir);
			break;
		}

		if (TELNET_PARSER_STATE(session, dir) == TS_COPY) {
			//state->telstate = TS_NORMAL;
			CHANGE_TELNET_PARSER_STATE(session, dir, TS_NORMAL);
		}
	}

	return PARSE_CONTINUE;
}

static ch_telnet_session_t *
_ch_telnet_get_session(ch_telnet_session_entry_t *tsentry)
{
	ch_telnet_session_t *session = NULL;

	if (tsentry->cur_session != NULL) {
		return tsentry->cur_session;
	}

	session = ch_telnet_session_create(tsentry->mp);
	if (session == NULL) return NULL;

	session->trans_data = ch_telnet_transfer_data_create(tsentry->mp, tsentry->tpproto->trans_fpath);

	if (session->trans_data == NULL) {
		CH_ERR_LOG("%s: Create transfer datas failed.", __func__);
		return NULL;
	}

	tsentry->cur_session = session;

	return session;
}

int ch_telnet_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	ch_telnet_session_entry_t *tsentry = (ch_telnet_session_entry_t *) sentry->priv_data;
	ch_telnet_session_t *session = _ch_telnet_get_session(tsentry);

	if (session == NULL) {
		CH_ERR_LOG("Internal error: %s: create telnet session failed.", __func__);
		return PARSE_BREAK;
	}

	if (data == NULL || dlen == 0) {
		return PARSE_CONTINUE;
	}

	return _handle_data(session, data, dlen, 0);
}

int ch_telnet_response_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	ch_telnet_session_entry_t *tsentry = (ch_telnet_session_entry_t *) sentry->priv_data;
	ch_telnet_session_t *session = _ch_telnet_get_session(tsentry);

	if (session == NULL) {
		CH_ERR_LOG("Internal error: %s: create telnet session failed.", __func__);
		return PARSE_BREAK;
	}

	if (data == NULL || dlen == 0) {
		return PARSE_CONTINUE;
	}

	return _handle_data(session, data, dlen, 1);
}



/*
 *
 *      Filename: ./pop3_parser.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-01 16:23:15
 * Last Modified: 2018-07-30 10:55:46
 */

#include "pop3_parser.h"
#include "ch_mail_session.h"
#include "ch_mail_decode_mime.h"
#include "ch_mail_common.h"
#include "ch_log.h"

#define POP3_CMD_PASS 1
#define POP3_CMD_USER 2
#define POP3_CMD_STAT 3
#define POP3_CMD_LIST 4
#define POP3_CMD_RETR 5
#define POP3_CMD_DELE 6
#define POP3_CMD_NOOP 7
#define POP3_CMD_REST 8
#define POP3_CMD_TOP  9
#define POP3_CMD_UIDL 10
#define POP3_CMD_QUIT 11

#define POP3_RET_ERR 1
#define POP3_RET_OK	 2

static void *ch_mail_pop3_state_create(ch_pool_t *pool)
{
    if (pool == NULL) {
        return NULL;
    }

    return (pop3_state_t *) ch_pcalloc(pool, sizeof(pop3_state_t ));
}

static bool pop3_get_cmd(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{
	/* unused */
	msentry = msentry;

	char *ptr = (char *)cur_line->data;
	int32_t len = cur_line->len;
	char *end = (char *)ptr + len;
	int32_t cmd_len;

	while (ptr < end && *ptr == ' ') ptr++;

	if (ptr == end) return false;

	cmd_len = end - ptr;

	if (cmd_len >= 4 && strncasecmp(ptr, "USER", 4) == 0) {
		state->m_Cmd = POP3_CMD_USER;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "PASS", 4) == 0) {
		state->m_Cmd = POP3_CMD_PASS;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "STAT", 4) == 0) {
		state->m_Cmd = POP3_CMD_STAT;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "LIST", 4) == 0) {
		state->m_Cmd = POP3_CMD_LIST;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "RETR", 4) == 0) {
		state->m_Cmd = POP3_CMD_RETR;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "DELE", 4) == 0) {
		state->m_Cmd = POP3_CMD_DELE;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "NOOP", 4) == 0) {
		state->m_Cmd = POP3_CMD_NOOP;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "REST", 4) == 0) {
		state->m_Cmd = POP3_CMD_REST;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "TOP", 3) == 0) {
		state->m_Cmd = POP3_CMD_TOP;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "UIDL", 4) == 0) {
		state->m_Cmd = POP3_CMD_UIDL;
	} else if (cmd_len >= 4 && strncasecmp(ptr, "QUIT", 4) == 0) {
		state->m_Cmd = POP3_CMD_QUIT;
	} else {
		return false;
	}

	return true;
}

static bool
pop3_process_command_user(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{

	/* unused */
	state = state;

	char *ptr;
	size_t len;
	ch_pool_t *pool = msentry->mp;
	char *end = (char *)cur_line->data + cur_line->len;

	ptr = strstr((const char *) cur_line->data, "USER");
	if (ptr == NULL) {
		return false;
	}

	while (ptr < end && *ptr != ' ') ptr++;

	if (ptr == end) return false;

	while (ptr < end && *ptr == ' ') ptr++;

	if (ptr == end) return false;

	len = end - ptr;

	msentry->login.data = (unsigned char *) ch_pstrndup(pool, ptr, len);
	if (msentry->login.data == NULL) {
		return false;
	}
	msentry->login.len = len;

	return true;
}

static bool
pop3_process_command_pass(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{
	/* unused */
	state = state;
	char *ptr;
	size_t len;
	ch_pool_t *pool = msentry->mp;
	char *end = (char *)cur_line->data + cur_line->len;

	ptr = strstr((const char *) cur_line->data, "PASS");
	if (ptr == NULL) {
		return false;
	}

	while (ptr < end && *ptr != ' ') ptr++;

	if (ptr == end) return false;

	while (ptr < end && *ptr == ' ') ptr++;

	if (ptr == end) return false;

	len = end - ptr;

	msentry->passwd.data = (unsigned char *) ch_pstrndup(pool, ptr, len);
	if (msentry->passwd.data == NULL) {
		return false;
	}
	msentry->passwd.len = len;

	return true;
}

static bool 
ParseAuth(ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{
	if (!pop3_get_cmd(msentry, cur_line, state)) {
		return false;
	}

    state->m_State = psAuthorization;

	if (state->m_Cmd == POP3_CMD_USER) {
		return pop3_process_command_user(msentry, cur_line, state);
	} else if (state->m_Cmd == POP3_CMD_PASS) {
		return pop3_process_command_pass(msentry, cur_line, state);
	}

	return true;
}

/*
 *Change State
 */
static void ChangeState(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_mail_session_entry_t *msentry, pop3_state_t *state)
{
	ch_mail_session_t *s = NULL;

	if (state->m_Cmd == POP3_CMD_PASS) {
		state->m_Is_Logined = 1;
		state->m_State = psTransaction;
	}

	if (state->m_Is_Logined == 0 || state->m_State != psTransaction) {
		if (state->m_Cmd == POP3_CMD_STAT || \
			state->m_Cmd == POP3_CMD_LIST || \
			state->m_Cmd == POP3_CMD_RETR || \
			state->m_Cmd == POP3_CMD_DELE || \
			state->m_Cmd == POP3_CMD_NOOP || \
			state->m_Cmd == POP3_CMD_REST || \
			state->m_Cmd == POP3_CMD_TOP  || \
			state->m_Cmd == POP3_CMD_UIDL)
		{
			state->m_Is_Logined = 1;
			state->m_State = psTransaction;
		}
	}

	if ((state->m_Is_Logined == 1) && (state->m_State == psTransaction)) {
		/* in logined status. and on "TRANSACTION" status */
		if (state->m_Cmd == POP3_CMD_QUIT) {
			state->m_State = psUpdate;
			s = msentry->curr_session;
			if (s && s->mime_state) {
				ch_mail_session_store(pstore,tsession, msentry, s);
			}
		} else if (state->m_Cmd == POP3_CMD_RETR) {
			/*start to parse and save mail body!!*/
			state->m_State = psData;
		}
	}
}

/*
 *get response Retcode from server recoder with client CMD
 */
static bool 
GetRetcodeAndData(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{
    const uint8_t* pdata = (const uint8_t *)cur_line->data;
    uint16_t pdata_len = cur_line->len;

    /* get data lenth exclude \r\n */ 
    while( pdata_len > 0 ) {
        if( pdata[pdata_len -1] == '\n' || pdata[pdata_len -1] == '\r') {
            pdata_len--;
        } else {
            break;
		}
    }

    if( pdata_len >= 3 && strncasecmp((const char*)pdata, "+OK", 3) == 0){
		state->m_RetCode = POP3_RET_OK;
    } else if(pdata_len >= 4 && strncasecmp((const char*)pdata, "-ERR", 4 ) == 0){
		state->m_RetCode = POP3_RET_ERR;
    } else {
		state->m_RetCode = 0;
	}
    
	if (state->m_RetCode == POP3_RET_OK) {
        /* Change m_State */
        ChangeState(pstore,tsession,msentry, state);
    }
    return true;
}

static bool 
ParseAnswer(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{
	if (!GetRetcodeAndData(pstore,tsession,msentry, cur_line, state)) {
		return false;
	}

    return true;
}


static bool 
ParseCmd(ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{
	if (!pop3_get_cmd(msentry, cur_line, state)) {
		return false;
	}

	state->m_State = psTransaction;

    return true;
}

static bool 
ParseData(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line, pop3_state_t *state)
{   
	/* unused */
	state = state;
	int ret;
	ch_pool_t *pool = msentry->mp;
	ch_mail_session_t *s = msentry->curr_session;

    if (s == NULL) {
        s = ch_mail_session_create(pool);
        if (s == NULL)
            return false;

		msentry->curr_session = s;
		list_add_tail(&s->anchor, &msentry->session_list);
    }
    
	if (s->mime_state == NULL) {
		s->mime_state = MimeDecInitParser(msentry, MailProcessDataChunk, 
										MailProcessMimeHeader);
		if (s->mime_state == NULL) {
			CH_ERR_LOG("MimeDecInitParser fail.\n");
			return false;
		}
        /* Add new MIME message to end of list */
        if (s->msg_head == NULL) {
            s->msg_head = s->mime_state->msg;
            s->msg_tail = s->mime_state->msg;
        } else {
            s->msg_tail->next = s->mime_state->msg;
            s->msg_tail = s->mime_state->msg;
        }
    }

	/* data transfer done */
	if (cur_line->len == 1 && cur_line->data[0] == '.') {
		if (s->mime_state != NULL) {
			ret = MimeDecParseComplete(s->mime_state);
			if (ret != MIME_DEC_OK) {
                CH_ERR_LOG("%s:MimeDecParseComplete failed, ret: %d.\n", 
					__func__, ret);
			}
			s->done = 1;
			
			ch_mail_session_store(pstore,tsession,msentry, s);
            
            return true;
		}
	}

	ret = MimeDecParseLine((const uint8_t *) cur_line->data,
		cur_line->len, cur_line->delimiter_len, s->mime_state);

	if (ret != MIME_DEC_OK) {
		CH_ERR_LOG("%s, MimeDecParseLine return error: %d.\n", __func__,ret );
	}
    
    return true;
}

static bool 
handle_client_data(ch_mail_session_entry_t *msentry, ch_mail_curr_line_t *cur_line)
{
	pop3_state_t *state = (pop3_state_t *) msentry->priv_data;

	if (state->m_State == psData) {
		return ParseCmd(msentry, cur_line, state);
	} else if (state->m_State == psAuthorization) {
		return ParseAuth(msentry, cur_line, state);
	} else if (state->m_State == psTransaction || \
		state->m_State == psUpdate) {
		return ParseCmd(msentry, cur_line, state);
	}

	return false;
}

static bool 
handle_server_data(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_mail_session_entry_t *msentry, ch_mail_curr_line_t *cur_line)
{
	pop3_state_t *state = (pop3_state_t *) msentry->priv_data;

	if (state->m_State == psData) {
		return ParseData(pstore,tsession,msentry, cur_line, state); 
	} else if (state->m_State == psAuthorization) {
		return ParseAnswer(pstore,tsession,msentry, cur_line, state);
	} else if (state->m_State == psTransaction || \
		state->m_State == psUpdate) {
		return ParseAnswer(pstore,tsession,msentry, cur_line, state);
	}

	return false;
}

static int pop3_parse(int dir,ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_mail_session_entry_t *msentry, uint8_t *input, uint32_t input_len)
{

	ch_mail_curr_line_t cur_line;
	ch_str_t input_data;

    if (input == NULL || input_len == 0) {
		CH_ERR_LOG("%s: no input.\n", __func__);
        return PARSE_CONTINUE;
    }

	input_data.data = input;
	input_data.len = input_len;

	if (dir == 0) {
		while (ch_mail_get_line(&input_data, &msentry->cur_req_buf, &cur_line) >= 0) 
		{
			if (handle_client_data(msentry, &cur_line) == false) {
				return PARSE_BREAK;
			}
		}
	} else if (dir == 1) {
		while (ch_mail_get_line(&input_data, &msentry->cur_res_buf, &cur_line) >= 0) 
		{
			if (handle_server_data(pstore,tsession,msentry, &cur_line) == false) {
				return PARSE_BREAK;
			}
		}
	}

	return PARSE_CONTINUE;
}

int mail_pop3_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data,size_t dlen)
{
    ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t*)tsession->sentry;

	if (msentry->priv_data == NULL) {
		msentry->priv_data = ch_mail_pop3_state_create(msentry->mp);

		if (msentry->priv_data == NULL) {
			CH_ERR_LOG("create smtp parser state failed.\n");
			return PARSE_BREAK;
		}
	}

	return pop3_parse(0,app,pstore,tsession, msentry, data, dlen);
}

int mail_pop3_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data, size_t dlen)
{
    ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t*)tsession->sentry;

	if (msentry->priv_data == NULL) {
		msentry->priv_data = ch_mail_pop3_state_create(msentry->mp);

		if (msentry->priv_data == NULL) {
			CH_ERR_LOG("create smtp parser state failed.\n");
			return PARSE_BREAK;
		}
	}

	return pop3_parse(1,app,pstore,tsession, msentry, data, dlen);
}

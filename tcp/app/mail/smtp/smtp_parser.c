/************************************************************************
 *
 *      Filename: smtp_parser.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-06-13 02:33:06
 * Last Modified: 2018-08-16 18:04:18
 *************************************************************************/

#include "smtp_parser.h"
#include "ch_mail_session.h"
#include "ch_mail_auth.h"
#include "ch_log.h"
#include "ch_tcp_app_pool.h"

#define SMTP_COMMAND_BUFFER_STEPS 7

/* we are in process of parsing a fresh command.  Just a placeholder.  If we
 * are not in STATE_COMMAND_DATA_MODE, we have to be in this mode */
#define SMTP_PARSER_STATE_COMMAND_MODE            0x00
/* we are in mode of parsing a command's data.  Used when we are parsing tls
 * or accepting the rfc 2822 mail after DATA command */
#define SMTP_PARSER_STATE_COMMAND_DATA_MODE       0x01
/* Used when we are still in the process of parsing a server command.  Used
 * with multi-line replies and the stream is fragmented before all the lines
 * for a response is seen */
#define SMTP_PARSER_STATE_PARSING_SERVER_RESPONSE 0x02
/* Used to indicate that the parser has seen the first reply */
#define SMTP_PARSER_STATE_FIRST_REPLY_SEEN        0x04

#define SMTP_PARSER_STATE_AUTH_USERNAME           0x08
#define SMTP_PARSER_STATE_AUTH_PASSWORD           0x10
#define SMTP_PARSER_STATE_AUTH_PLAIN              0x20
#define SMTP_PARSER_STATE_AUTH_CRAM_MD5           0x40

/* Used to indicate that the parser is parsing a multiline reply */
#define SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY 0x80

/* Various SMTP commands
 * We currently have var-ified just STARTTLS and DATA, since we need to them
 * for state transitions.  The rest are just indicate as OTHER_CMD.  Other
 * commands would be introduced as and when needed */
#define SMTP_COMMAND_STARTTLS  1
#define SMTP_COMMAND_DATA      2
#define SMTP_COMMAND_BDAT      3
/* not an actual command per se, but the mode where we accept the mail after
 * DATA has it's own reply code for completion, from the server.  We give this
 * stage a pseudo command of it's own, so that we can add this to the command
 * buffer to match with the reply */
#define SMTP_COMMAND_DATA_MODE 4
#define SMTP_COMMAND_QUIT      5
#define SMTP_COMMAND_AUTH      6
/* All other commands are represented by this var */
#define SMTP_COMMAND_OTHER_CMD 7

typedef struct {
	char *name;
	int value;
} enum_map_t;

/* smtp reply codes.  If an entry is made here, please make a simultaneous
 * entry in smtp_reply_map */
enum {
	SMTP_REPLY_211,
	SMTP_REPLY_214,
	SMTP_REPLY_220,
	SMTP_REPLY_221,
	SMTP_REPLY_235,
	SMTP_REPLY_250,
	SMTP_REPLY_251,
	SMTP_REPLY_252,

	SMTP_REPLY_334,
	SMTP_REPLY_354,

	SMTP_REPLY_421,
	SMTP_REPLY_450,
	SMTP_REPLY_451,
	SMTP_REPLY_452,
	SMTP_REPLY_455,

	SMTP_REPLY_500,
	SMTP_REPLY_501,
	SMTP_REPLY_502,
	SMTP_REPLY_503,
	SMTP_REPLY_504,
	SMTP_REPLY_550,
	SMTP_REPLY_551,
	SMTP_REPLY_552,
	SMTP_REPLY_553,
	SMTP_REPLY_554,
	SMTP_REPLY_555,
};

enum_map_t smtp_reply_map[ ] = {
	{ "211", SMTP_REPLY_211 },
	{ "214", SMTP_REPLY_214 },
	{ "220", SMTP_REPLY_220 },
	{ "221", SMTP_REPLY_221 },
	{ "235", SMTP_REPLY_235 },
	{ "250", SMTP_REPLY_250 },
	{ "251", SMTP_REPLY_251 },
	{ "252", SMTP_REPLY_252 },

	{ "334", SMTP_REPLY_334 },
	{ "354", SMTP_REPLY_354 },

	{ "421", SMTP_REPLY_421 },
	{ "450", SMTP_REPLY_450 },
	{ "451", SMTP_REPLY_451 },
	{ "452", SMTP_REPLY_452 },
	{ "455", SMTP_REPLY_455 },

	{ "500", SMTP_REPLY_500 },
	{ "501", SMTP_REPLY_501 },
	{ "502", SMTP_REPLY_502 },
	{ "503", SMTP_REPLY_503 },
	{ "504", SMTP_REPLY_504 },
	{ "550", SMTP_REPLY_550 },
	{ "551", SMTP_REPLY_551 },
	{ "552", SMTP_REPLY_552 },
	{ "553", SMTP_REPLY_553 },
	{ "554", SMTP_REPLY_554 },
	{ "555", SMTP_REPLY_555 },
	{  NULL,  -1 },
};

SMTPConfig smtp_config = {1, 0, 0, 0};

static void *ch_mail_smtp_state_create(ch_pool_t *pool)
{
	smtp_state_t *smtp_state = NULL;

	if (pool == NULL) {
		return NULL;
	}

	smtp_state = (smtp_state_t *) ch_pcalloc(pool, sizeof(*smtp_state));
	if (smtp_state == NULL)
		return NULL;

	smtp_state->cmds = (uint8_t *) ch_pcalloc(pool, 
						sizeof(uint8_t) * SMTP_COMMAND_BUFFER_STEPS);
	if (smtp_state->cmds == NULL) {
		return NULL;
	}
	
	smtp_state->cmds_buffer_len = SMTP_COMMAND_BUFFER_STEPS;
	INIT_LIST_HEAD(&smtp_state->rcpt_list);

	return smtp_state;
}

static int
smtp_insert_cmd_buffer(uint8_t command, ch_mail_session_entry_t *msentry)
{
	void *ptmp;
	ch_pool_t *pool = msentry->mp;
	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

	if (state->cmds_cnt >= state->cmds_buffer_len) {
		int increment = SMTP_COMMAND_BUFFER_STEPS;
		if ((int)(state->cmds_buffer_len + SMTP_COMMAND_BUFFER_STEPS) > (int)USHRT_MAX) {
			increment = USHRT_MAX - state->cmds_buffer_len;
		}

		ptmp = (uint8_t *) ch_pcalloc(pool, 
						sizeof(uint8_t) *(state->cmds_buffer_len + increment));
		if (ptmp == NULL) {
			//SCFree(state->cmds);
			state->cmds = NULL;
			CH_ERR_LOG("%s: SCRealloc failure", __func__);
			return -1;
		}
		state->cmds = ptmp;

		state->cmds_buffer_len += increment;
	}
	if (state->cmds_cnt >= 1 &&
		((state->cmds[state->cmds_cnt - 1] == SMTP_COMMAND_STARTTLS) ||
		 (state->cmds[state->cmds_cnt - 1] == SMTP_COMMAND_DATA))) {
		/* decoder event */
		/* we have to have EHLO, DATA, VRFY, EXPN, TURN, QUIT, NOOP,
		 * STARTTLS as the last command in pipelined mode */
	}

	/** \todo decoder event */
	if ((int)(state->cmds_cnt + 1) > (int)USHRT_MAX) {
		CH_ERR_LOG("%s: command buffer overflow", __func__);
		return -1;
	}

	state->cmds[state->cmds_cnt] = command;
	state->cmds_cnt++;

	return 0;
}

static int 
smtp_process_cmd_starttls(ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line)
{
	/* unused */
	msentry = msentry;
	cur_line = cur_line;

	return 0;
}

static int 
smtp_process_cmd_data(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line)
{
	ch_mail_session_t *s = msentry->curr_session;

    if (!(msentry->parser_state & SMTP_PARSER_STATE_COMMAND_DATA_MODE)) {
        /* looks like are still waiting for a confirmination from the server */
        return 0;
    }

    if (cur_line->len == 1 && cur_line->data[0] == '.') {
        msentry->parser_state &= ~SMTP_PARSER_STATE_COMMAND_DATA_MODE;
        /* kinda like a hack.  The mail sent in DATA mode, would be
         * acknowledged with a reply.  We insert a dummy command to
         * the command buffer to be used by the reply handler to match
         * the reply received */
        smtp_insert_cmd_buffer(SMTP_COMMAND_DATA_MODE, msentry);

        if (smtp_config.decode_mime && s->mime_state != NULL) {
            /* Complete parsing task */
            int ret = MimeDecParseComplete(s->mime_state);
            if (ret != MIME_DEC_OK) {
                CH_ERR_LOG("%s: MimeDecParseComplete() function failed", __func__);
            }

        }
        s->done = 1;
		ch_mail_session_store(pstore,tsession, msentry, s);
    }

    /* If DATA, then parse out a MIME message */
    if (msentry->current_command == SMTP_COMMAND_DATA &&
            (msentry->parser_state & SMTP_PARSER_STATE_COMMAND_DATA_MODE)) {

        if (smtp_config.decode_mime && s->mime_state) {
            int ret = MimeDecParseLine((const uint8_t *) cur_line->data,
                    cur_line->len, cur_line->delimiter_len, s->mime_state);

            if (ret != MIME_DEC_OK) {
                CH_ERR_LOG("%s: MimeDecParseLine() function returned an error code: %d", 
					__func__, ret);
            }
        }
    }

    return 0;
}

static int 
smtp_process_cmd_bdat(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

    state->bdat_chunk_idx += (cur_line->len +
                              cur_line->delimiter_len);
    if (state->bdat_chunk_idx > state->bdat_chunk_len) {
        msentry->parser_state &= ~SMTP_PARSER_STATE_COMMAND_DATA_MODE;
        /* decoder event */
        return -1;
    } else if (state->bdat_chunk_idx == state->bdat_chunk_len) {
        msentry->parser_state &= ~SMTP_PARSER_STATE_COMMAND_DATA_MODE;
    }
	return 0;
}

/* consider 'rset' and 'quit' to be part of the existing state */
static int NoNewTx(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	if (!(msentry->parser_state & SMTP_PARSER_STATE_COMMAND_DATA_MODE)) {
		if (cur_line->len >= 4 &&
			strncasecmp("rset", (char *)cur_line->data, 4) == 0) {
			return 1;
		} else if (cur_line->len >= 4 &&
			strncasecmp("quit", (char *)cur_line->data, 4) == 0) {
			return 1;
		}
	}
	return 0;
}

static int
smtp_parse_cmd_with_param(ch_mail_session_entry_t *msentry,
					ch_mail_curr_line_t *cur_line,
					uint8_t prefix_len,
					uint8_t **target,
					uint16_t *target_len)
{
	int i = prefix_len + 1;
	int spc_i = 0;

	while (i < cur_line->len) {
		if (cur_line->data[i] != ' ') {
			break;
		}
		i++;
	}

	/* rfc1870: with the size extension the mail from can be followed by an option.
		We use the space separator to detect it. */
	spc_i = i;
	while (spc_i < cur_line->len) {
		if (cur_line->data[spc_i] == ' ') {
			break;
		}
		spc_i++;
	}

	*target = (uint8_t *) ch_pstrndup(msentry->mp, (const char *)cur_line->data + i, spc_i - i);
	if (*target == NULL)
		return -1;

	*target_len = spc_i - i;

	return 0;
}

static int 
smtp_parse_command_bdat(ch_mail_session_entry_t *msentry, 
	ch_mail_curr_line_t *cur_line)
{
    int i = 4;
	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

    while (i < cur_line->len) {
        if (cur_line->data[i] != ' ') {
            break;
        }
        i++;
    }
    if (i == 4) {
        /* decoder event */
        return -1;
    }
    if (i == cur_line->len) {
        /* decoder event */
        return -1;
    }
    char *endptr = NULL;
    state->bdat_chunk_len = strtoul((const char *)cur_line->data + i,
                                    (char **)&endptr, 10);
    if ((uint8_t *)endptr == cur_line->data + i) {
        /* decoder event */
        return -1;
    }
	return 0;
}

static int 
smtp_parse_command_helo(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

	return smtp_parse_cmd_with_param(msentry, cur_line, 4, 
		&state->helo, &state->helo_len);
}

static int 
smtp_parse_command_mailfrom(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

	return smtp_parse_cmd_with_param(msentry, cur_line, 9,
		(uint8_t **)&state->smtp_from.data,
		(uint16_t *)&state->smtp_from.len);
}

static int 
smtp_parse_command_auth_plain(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	char *ptr = (char *)cur_line->data + 10; /* skip 'auth plain' */
	char *end = (char *)cur_line->data + cur_line->len;
	ch_pool_t *pool = msentry->mp;

	if (*ptr != ' ') {
		return -1;
	}

	while (ptr < end) {
		if (*ptr != ' ')
			break;
		ptr++;
	}

	if (ptr == end) {
		return -1;
	}

	return ch_mail_auth_plain(pool, &msentry->login, 
								&msentry->passwd,
								(unsigned char*)ptr, end - ptr);
}

static int 
smtp_parse_command_auth_login_username(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	int ret;

	ret = ch_mail_auth_login_username(msentry->mp, &msentry->login,
		cur_line->data, cur_line->len);

	msentry->parser_state &= ~SMTP_PARSER_STATE_AUTH_USERNAME;

	msentry->parser_state |= SMTP_PARSER_STATE_AUTH_PASSWORD;

	return ret;
}

static int 
smtp_parse_command_auth_login_password(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	int ret;

	ret = ch_mail_auth_login_password(msentry->mp, &msentry->passwd,
		cur_line->data, cur_line->len);

	msentry->parser_state &= ~SMTP_PARSER_STATE_AUTH_PASSWORD;

	return ret;
}

static int 
smtp_parse_command_auth_login(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	/* unused */
	msentry = msentry;
	cur_line = cur_line;
	return 0;
}

static int 
smtp_parse_command_auth_cram_md5(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	/* unused */
	msentry = msentry;
	cur_line = cur_line;
	return 0;
}

static int 
smtp_parse_command_auth(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	if (cur_line->len >= 10 && \
		strncasecmp("auth plain", (char *)cur_line->data, 10) == 0) {
		return smtp_parse_command_auth_plain(msentry, cur_line);
	} else if (cur_line->len >= 10 && \
		strncasecmp("auth login", (char *)cur_line->data, 10) == 0) {
		return smtp_parse_command_auth_login(msentry, cur_line);
	} else if (cur_line->len >= 13 && \
		strncasecmp("auth cram-md5", (char *)cur_line->data, 13) == 0) {
		return smtp_parse_command_auth_cram_md5(msentry, cur_line);
	}
	return -1;
}

static int 
smtp_parse_command_rcptto(ch_mail_session_entry_t *msentry,
	ch_mail_curr_line_t *cur_line)
{
	uint8_t *rcptto;
	uint16_t rcptto_len;
	ch_pool_t *pool = msentry->mp;

	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

	if (smtp_parse_cmd_with_param(msentry, cur_line, 7, &rcptto, &rcptto_len) == 0) {
		mail_addr_node_t *rcptto_node = \
			(mail_addr_node_t *)ch_palloc(pool, sizeof(mail_addr_node_t));
		if (rcptto_node) {
			rcptto_node->name.data = (unsigned char *)rcptto;
			rcptto_node->name.len = rcptto_len;
			list_add_tail(&rcptto_node->anchor, &state->rcpt_list);
		} else {
			CH_ERR_LOG("%s, call ch_palloc failed.\n");
			return -1;
		}
	} else {
		return -1;
	}
	return 0;
}

static int 
smtp_process_request(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        ch_mail_session_entry_t *msentry, ch_mail_curr_line_t *cur_line)
{
	ch_mail_session_t *s = msentry->curr_session;
	ch_pool_t *pool = msentry->mp;

	if (s == NULL || (s->done && !NoNewTx(msentry, cur_line))) {
		s = ch_mail_session_create(pool);
		if (s == NULL)
			return -1;
		msentry->curr_session = s;
		list_add_tail(&s->anchor, &msentry->session_list);
	}

    /* there are 2 commands that can push it into this COMMAND_DATA mode -
     * STARTTLS and DATA */
    if (!(msentry->parser_state & SMTP_PARSER_STATE_COMMAND_DATA_MODE)) {
        int r = 0;

        if (cur_line->len >= 8 &&
            strncasecmp("starttls", (char *)cur_line->data, 8) == 0) {
            msentry->current_command = SMTP_COMMAND_STARTTLS;
		} else if (cur_line->len >= 4 &&
			strncasecmp("data", (char *)cur_line->data, 4) == 0) {
			msentry->current_command = SMTP_COMMAND_DATA;
			/* start to decode mime */
			if (s->mime_state) {
				/* We have 2 chained mails and did not detect the end
				 * of first one. So we start a new transaction. */
				s->mime_state->state_flag = MIME_PARSE_ERROR;
				s = ch_mail_session_create(pool);
				if (s== NULL)
					return -1;
				msentry->curr_session = s;
				list_add_tail(&s->anchor, &msentry->session_list);
			}
			s->mime_state = MimeDecInitParser(msentry, 
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

        } else if (cur_line->len >= 4 &&
                   strncasecmp("bdat", (char *)cur_line->data, 4) == 0) {
            r = smtp_parse_command_bdat(msentry, cur_line);
            if (r == -1) {
                return -1;
            }
            msentry->current_command = SMTP_COMMAND_BDAT;
            msentry->parser_state |= SMTP_PARSER_STATE_COMMAND_DATA_MODE;
        } else if (cur_line->len >= 4 &&
                   ((strncasecmp("helo", (char *)cur_line->data, 4) == 0) ||
                    strncasecmp("ehlo", (char *)cur_line->data, 4) == 0))  {
            r = smtp_parse_command_helo(msentry, cur_line);
            if (r == -1) {
                return -1;
            }
            msentry->current_command = SMTP_COMMAND_OTHER_CMD;
        } else if (cur_line->len >= 4 &&
                   strncasecmp("auth", (char *)cur_line->data, 4) == 0) {
			r = smtp_parse_command_auth(msentry, cur_line);
			if (r == -1) {
				return -1;
			}
			msentry->current_command = SMTP_COMMAND_AUTH;
		} else if (cur_line->len >= 9 &&
                   strncasecmp("mail from", (char *)cur_line->data, 9) == 0) {
            r = smtp_parse_command_mailfrom(msentry, cur_line);
            if (r == -1) {
                return -1;
            }
            msentry->current_command = SMTP_COMMAND_OTHER_CMD;
        } else if (cur_line->len >= 7 &&
                   strncasecmp("rcpt to", (char *)cur_line->data, 7) == 0) {
            r = smtp_parse_command_rcptto(msentry, cur_line);
            if (r == -1) {
                return -1;
            }
            msentry->current_command = SMTP_COMMAND_OTHER_CMD;
        } else if (cur_line->len >= 4 &&
                   strncasecmp("quit", (char *)cur_line->data, 4) == 0) {
            msentry->current_command = SMTP_COMMAND_QUIT;
		}else {
			if (msentry->parser_state & SMTP_PARSER_STATE_AUTH_USERNAME) {
				smtp_parse_command_auth_login_username(msentry, cur_line);
			} else if (msentry->parser_state & SMTP_PARSER_STATE_AUTH_PASSWORD) {
				smtp_parse_command_auth_login_password(msentry, cur_line);
			} else {
				msentry->current_command = SMTP_COMMAND_OTHER_CMD;
			}
        }

        /* Every command is inserted into a command buffer, to be matched
         * against reply(ies) sent by the server */
        if (smtp_insert_cmd_buffer(msentry->current_command, msentry) == -1) {
            return -1;
        }

		return r;
    }

	switch (msentry->current_command) {
	case SMTP_COMMAND_STARTTLS:
		return smtp_process_cmd_starttls(msentry, cur_line);

	case SMTP_COMMAND_DATA:
		return smtp_process_cmd_data(pstore,tsession,msentry, cur_line);

	case SMTP_COMMAND_BDAT:
		return smtp_process_cmd_bdat(msentry, cur_line);

	default:
		/* we have nothing to do with any other command at this instant.
		 * Just let it go through */
		return 0;
	}
}

static int smtp_get_reply_code(uint8_t *line, int32_t len)
{
	size_t i = 0;
	enum_map_t *t;

	if (line == NULL || len == 0) {
		CH_DEBUG_LOG("%s: no input string is specified.\n");
		return 0;
	}

	for (; i < sizeof (smtp_reply_map) / sizeof (enum_map_t); i++) {
		t = &smtp_reply_map[i];
		if (t->name != NULL && strncmp((char *)line, t->name, 3) == 0) {
			return t->value;
		}
	}

	return 0;
}

static int 
smtp_process_reply(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused,ch_tcp_session_t *tsession ch_unused,
        ch_mail_session_entry_t *msentry, ch_mail_curr_line_t *cur_line)
{
    uint64_t reply_code = 0;
	smtp_state_t *state = (smtp_state_t *) msentry->priv_data;

	
    /* the reply code has to contain at least 3 bytes, to hold the 3 digit
     * reply code */
    if (cur_line->len < 3) {
        return -1;
    }

    if (cur_line->len >= 4) {
        if (msentry->parser_state & SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY) {
            if (cur_line->data[3] != '-') {
                msentry->parser_state &= ~SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY;
            }
        } else {
            if (cur_line->data[3] == '-') {
                msentry->parser_state |= SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY;
            }
        }
    } else {
        if (msentry->parser_state & SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY) {
            msentry->parser_state &= ~SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY;
        }
    }

	reply_code = smtp_get_reply_code(cur_line->data, cur_line->len);

    if (state->cmds_idx == state->cmds_cnt) {
        if (!(msentry->parser_state & SMTP_PARSER_STATE_FIRST_REPLY_SEEN)) {
            /* the first server reply can be a multiline message. Let's
             * flag the fact that we have seen the first reply only at the end
             * of a multiline reply
             */
            if (!(msentry->parser_state & SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY)) {
                msentry->parser_state |= SMTP_PARSER_STATE_FIRST_REPLY_SEEN;
				//fprintf(stderr, "id: %d, set first reply seen flag.\n", msentry->tbl_entry.session_id);
			} 

			return 0;
        } else {
            /* decoder event - unable to match reply with request */
            CH_DEBUG_LOG("unable to match reply with request.\n");
            return 0;
        }
    }

    if (state->cmds_cnt == 0) {
        /* reply but not a command we have stored, fall through */
    } else if (state->cmds[state->cmds_idx] == SMTP_COMMAND_STARTTLS) {
        if (reply_code == SMTP_REPLY_220) {
            /* we are entering STARRTTLS data mode */
            msentry->parser_state |= SMTP_PARSER_STATE_COMMAND_DATA_MODE;

			return PARSE_BREAK;
        }
    } else if (state->cmds[state->cmds_idx] == SMTP_COMMAND_DATA) {
        if (reply_code == SMTP_REPLY_354) {
            /* Next comes the mail for the DATA command in toserver direction */
            msentry->parser_state |= SMTP_PARSER_STATE_COMMAND_DATA_MODE;
        } 
    } else if (state->cmds[state->cmds_idx] == SMTP_COMMAND_AUTH) {
	    if ((reply_code == SMTP_REPLY_334) && \
			    ((msentry->parser_state & SMTP_PARSER_STATE_AUTH_USERNAME) == 0) && \
			    ((msentry->parser_state & SMTP_PARSER_STATE_AUTH_PASSWORD) == 0))
	    {
		    msentry->parser_state |= SMTP_PARSER_STATE_AUTH_USERNAME;
	    }
	}

    /* if it is a multi-line reply, we need to move the index only once for all
     * the line of the reply.  We unset the multiline flag on the last
     * line of the multiline reply, following which we increment the index */
    if (!(msentry->parser_state & SMTP_PARSER_STATE_PARSING_MULTILINE_REPLY)) {
        state->cmds_idx++;
    }

    /* if we have matched all the buffered commands, reset the cnt and index */
    if (state->cmds_idx == state->cmds_cnt) {
        state->cmds_cnt = 0;
        state->cmds_idx = 0;
    }

    return 0;
}


static int smtp_parse(int direction,ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
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

    /* toserver */
    if (direction == 0) {
		while (ch_mail_get_line(&input_data, &msentry->cur_req_buf, &cur_line) >= 0) 
		{
            if (smtp_process_request(app,pstore,tsession,msentry, &cur_line) == -1)
				return PARSE_BREAK;
		}

		/* toclient */
	} else {
		while (ch_mail_get_line(&input_data, &msentry->cur_res_buf, &cur_line) >= 0) 
		{
			if (smtp_process_reply(app,pstore,tsession,msentry, &cur_line) == -1)
				return PARSE_BREAK;
		}
	}

	return PARSE_CONTINUE;
}

int mail_smtp_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        void *data,size_t  dlen){

    ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t*)tsession->sentry;

	if (msentry->priv_data == NULL) {
		msentry->priv_data = ch_mail_smtp_state_create(msentry->mp);

		if (msentry->priv_data == NULL) {
			CH_ERR_LOG("create smtp parser state failed.\n");
			return PARSE_BREAK;
		}
	}

	return smtp_parse(0,app,pstore,tsession, msentry, data, dlen);
}

int mail_smtp_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data, size_t dlen)
{
    ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t*)tsession->sentry;
	
    if (msentry->priv_data == NULL) {
		msentry->priv_data = ch_mail_smtp_state_create(msentry->mp);

		if (msentry->priv_data == NULL) {
			CH_ERR_LOG("create smtp parser state failed.\n");
			return PARSE_BREAK;
		}
	}

	return smtp_parse(1,app,pstore,tsession, msentry, data, dlen);
}


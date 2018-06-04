#ifndef __SMTP_PARSER_H__
#define __SMTP_PARSER_H__

#include "ch_list.h"
#include "ch_string.h"
#include "ch_mail_proto.h"
#include "ch_mail_common.h"
#include "ch_mail_session_entry.h"
#include "ch_mail_decode_mime.h"

typedef struct SMTPConfig {

    int decode_mime;
    uint32_t content_limit;
    uint32_t content_inspect_min_size;
    uint32_t content_inspect_window;
} SMTPConfig;

typedef struct smtp_state_s {
    /** bdat chunk len */
    uint32_t bdat_chunk_len;
    /** bdat chunk idx */
    uint32_t bdat_chunk_idx;

    /** the command buffer */
    uint8_t *cmds;
    /** the buffer length */
    uint16_t cmds_buffer_len;
    /** no of commands stored in the above buffer */
    uint16_t cmds_cnt;
    /** index of the command in the buffer, currently in inspection by reply
     *  handler */
    uint16_t cmds_idx;

    /* HELO of HELO message content */
    uint8_t *helo;
    uint16_t helo_len;

	ch_str_t smtp_from;
	struct ch_list_head rcpt_list;
} smtp_state_t;

/* Create SMTP config structure */
extern SMTPConfig smtp_config;

int mail_smtp_request_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

int mail_smtp_response_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

#endif //__SMTP_PARSER_H__

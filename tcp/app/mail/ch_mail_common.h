#ifndef __CH_PARSER_MAIL_COMMON_H__
#define __CH_PARSER_MAIL_COMMON_H__
/*
 *
 *      Filename: ch_parser_mail_common.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-14 10:38:28
 * Last Modified: 2016-07-14 10:38:28
 */

#include "ch_list.h"
#include "ch_mail_session.h"
#include "ch_mail_file.h"
#include "ch_mail_decode_mime.h"
#include "ch_mail_session_buffer.h"

#define IP_STR_LEN  128

#define SCCalloc calloc
#define SCMalloc malloc
#define SCRealloc realloc
#define SCFree free

#ifndef MIN
#define MIN(x, y) (((x)<(y))?(x):(y))
#endif

#ifndef MAX
#define MAX(x, y) (((x)<(y))?(y):(x))
#endif

typedef struct mail_addr_node_s {
	struct list_head anchor;
	ch_str_t name;
} mail_addr_node_t;

int 
ch_mail_get_line(ch_str_t *input, ch_mail_tx_buf_t *tx_buf, 
	ch_mail_curr_line_t *curr_line);

int 
MailProcessMimeHeader(const uint8_t *name, uint32_t nlen, 
	const uint8_t *value, uint32_t vlen, MimeDecParseState *state);

int 
MailProcessDataChunk(const uint8_t *chunk, uint32_t len, MimeDecParseState *state);

#endif //__CH_PARSER_MAIL_COMMON_H__

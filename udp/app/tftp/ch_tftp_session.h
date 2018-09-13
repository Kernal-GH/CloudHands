/*
 *
 *      Filename: ch_tftp_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-11 11:42:49
 * Last Modified: 2018-09-11 11:42:49
 */

#ifndef CH_TFTP_SESSION_H
#define CH_TFTP_SESSION_H

typedef struct ch_tftp_session_t ch_tftp_session_t;

struct ch_tftp_session_t {

	ch_udp_app_session_t app_session;

	uint32_t src_ip;
	uint32_t dst_ip;
	uint32_t src_port;
	uint32_t dst_port;

	ch_pool_t *mp;

	int is_read;
	int is_error;

	uint16_t lask_block;

	const char *fname;
	const char *mode;
	const char *errmsg;

	const char *fpath;
	FILE *fp;
};

#endif /*CH_TFTP_SESSION_H*/

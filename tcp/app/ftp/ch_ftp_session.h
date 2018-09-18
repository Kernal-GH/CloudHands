/*
 *
 *      Filename: ch_ftp_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-18 16:06:22
 * Last Modified: 2018-09-18 16:06:22
 */

#ifndef CH_FTP_SESSION_H
#define CH_FTP_SESSION_H

typedef struct ch_ftp_cmd_t ch_ftp_cmd_t;
typedef struct ch_ftp_ans_t ch_ftp_ans_t;

typedef struct ch_ftp_session_data_t ch_ftp_session_data_t;
typedef struct ch_ftp_session_t ch_ftp_session_t;

#define PASV_MODE 1
#define PORT_MODE 2

struct ch_ftp_cmd_t {

	struct list_head node;
	struct list_head ans_list;
	const char *cmd;
	const char *args;
};


struct ch_ftp_ans_t {

	struct list_head node;
	uint16_t code;
	const char *phase;
};

struct ch_ftp_session_t {

	struct list_head cmd_list;

	ch_pool_t *mp;

	const char *user;
	const char *passwd;

};

struct ch_ftp_session_data_t {

	int mode;
	const char *path;
	const char *name; 
	
	uint32_t src_ip;
	uint32_t dst_ip;
	uint16_t src_port;
	uint16_t dst_port;

};

#endif /*CH_FTP_SESSION_H*/

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

#define CMD_USER "USER"
#define CMD_PASS "PASS"
#define CMD_PWD "PWD"
#define CMD_CMD "CMD"
#define CMD_PASV "PASV"

struct ch_ftp_cmd_t {

	struct list_head node;
	struct list_head ans_list;
	const char *cmd;
	const char *args;
};


struct ch_ftp_ans_t {

	struct list_head node;
	uint16_t code;
	const char *phrase;
};

struct ch_ftp_session_t {

	struct list_head cmd_list;
	ch_ftp_cmd_t *cur_cmd;

	struct list_head dsession_list;
	ch_ftp_session_data_t *cur_dsession;

	ch_pool_t *mp;

	const char *user;
	const char *passwd;

};

struct ch_ftp_session_data_t {

	struct list_head node;

	int is_ok;

	int mode;
	const char *path;
	const char *name; 

	uint32_t src_ip;
	uint32_t dst_ip;
	uint16_t src_port;
	uint16_t dst_port;

	uint64_t size;
	uint64_t rx_size;

	const char *fstore_path;
	FILE *fstore_fp;

};

extern ch_ftp_session_t * ch_ftp_session_create(ch_pool_t *mp);

extern void ch_ftp_session_destroy(ch_ftp_session_t *ftp_session);

extern void ch_ftp_session_cmd_add(ch_ftp_session_t *ftp_session,ch_str_t *cmd,ch_str_t *args);

extern void ch_ftp_session_ans_add(ch_ftp_session_t *ftp_session,ch_str_t *code,ch_str_t *phrase);

extern ch_ftp_session_data_t * ch_ftp_session_data_find(ch_ftp_session_t *ftp_session,
	uint32_t src_ip,
	uint32_t dst_ip,
	uint16_t src_port,
	uint16_t dst_port);

extern ch_ftp_session_data_t * ch_ftp_session_data_create(ch_ftp_session_t *ftp_session,ch_ftp_cmd_t *cmd);

extern int ch_ftp_session_data_append(ch_ftp_session_t *ftp_session,ch_ftp_session_data_t *ftp_dsession,void *data,size_t dlen);

#endif /*CH_FTP_SESSION_H*/

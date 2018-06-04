/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_parser_do.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 14:20:28
 * Last Modified: 2017-01-04 14:20:28
 */


#ifndef __CH_FTP_PARSER_DO_H__
#define __CH_FTP_PARSER_DO_H__

typedef enum {
    FTP_COMMAND_UNKNOWN = 0,
    FTP_COMMAND_ABOR,
    FTP_COMMAND_ACCT,
    FTP_COMMAND_ALLO,
    FTP_COMMAND_APPE,
    FTP_COMMAND_CDUP,
    FTP_COMMAND_CHMOD,
    FTP_COMMAND_CWD,
    FTP_COMMAND_DELE,
    FTP_COMMAND_HELP,
    FTP_COMMAND_IDLE,
    FTP_COMMAND_LIST,
    FTP_COMMAND_MAIL,
    FTP_COMMAND_MDTM,
    FTP_COMMAND_MKD,
    FTP_COMMAND_MLFL,
    FTP_COMMAND_MODE,
    FTP_COMMAND_MRCP,
    FTP_COMMAND_MRSQ,
    FTP_COMMAND_MSAM,
    FTP_COMMAND_MSND,
    FTP_COMMAND_MSOM,
    FTP_COMMAND_NLST,
    FTP_COMMAND_NOOP,
    FTP_COMMAND_PASS,
    FTP_COMMAND_PASV,
    FTP_COMMAND_PORT,
    FTP_COMMAND_PWD,
    FTP_COMMAND_QUIT,
    FTP_COMMAND_REIN,
    FTP_COMMAND_REST,
    FTP_COMMAND_RETR,
    FTP_COMMAND_RMD,
    FTP_COMMAND_RNFR,
    FTP_COMMAND_RNTO,
    FTP_COMMAND_SITE,
    FTP_COMMAND_SIZE,
    FTP_COMMAND_SMNT,
    FTP_COMMAND_STAT,
    FTP_COMMAND_STOR,
    FTP_COMMAND_STOU,
    FTP_COMMAND_STRU,
    FTP_COMMAND_SYST,
    FTP_COMMAND_TYPE,
    FTP_COMMAND_UMASK,
    FTP_COMMAND_USER
    /** \todo more if missing.. */
} ch_ftp_req_cmd;

typedef struct ch_ftp_parser_state_s {
	ch_ftp_req_cmd cmd;
} ch_ftp_parser_state_t;

int ch_ftp_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

int ch_ftp_response_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

#endif //__CH_FTP_PARSER_DO_H__


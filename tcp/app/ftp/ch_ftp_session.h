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
typedef struct ch_ftp_session_t ch_ftp_session_t;

#include "ch_list.h"
#include "ch_mpool.h"
#include "ch_ftp_data_connection_pool.h"
#include "ch_string.h"

#define PASV_MODE 1
#define PORT_MODE 2

#define CMD_USER "USER"
#define CMD_PASS "PASS"
#define CMD_PWD "PWD"
#define CMD_CMD "CMD"
#define CMD_PASV "PASV"
#define CMD_RETR "RETR"
#define CMD_STOR "STOR"
#define CMD_LIST "LIST"
#define CMD_PORT "PORT"
#define CMD_ACCT "ACCT"
#define CMD_CDUP "CDUP"
#define CMD_SMNT "SMNT"
#define CMD_QUIT "QUIT"
#define CMD_REIN "REIN"
#define CMD_TYPE  "TYPE"
#define CMD_STRU "STRU"
#define CMD_MODE  "MODE"
#define CMD_APPE  "APPE"
#define CMD_ALLO  "ALLO"
#define CMD_REST  "RESET"
#define CMD_RNFR  "RNFR"
#define CMD_RNTO  "RNTO"
#define CMD_ABOR  "ABOR"
#define CMD_DELE  "DELE"
#define CMD_RMD   "RMD"
#define CMD_MKD   "MKD"
#define CMD_NLIST "NLIST"
#define CMD_SITE  "SITE"
#define CMD_SYST  "SYST"
#define CMD_STAT  "STAT"
#define CMD_HELP  "HELP"
#define CMD_NOOP  "NOOP"

#define FTP_DATACONN          150

#define FTP_NOOPOK            200
#define FTP_TYPEOK            200
#define FTP_PORTOK            200
#define FTP_EPRTOK            200
#define FTP_UMASKOK           200
#define FTP_CHMODOK           200
#define FTP_EPSVALLOK         200
#define FTP_STRUOK            200
#define FTP_MODEOK            200
#define FTP_PBSZOK            200
#define FTP_PROTOK            200
#define FTP_OPTSOK            200
#define FTP_ALLOOK            202
#define FTP_FEAT              211
#define FTP_STATOK            211
#define FTP_SIZEOK            213
#define FTP_MDTMOK            213
#define FTP_STATFILE_OK       213
#define FTP_SITEHELP          214
#define FTP_HELP              214
#define FTP_SYSTOK            215
#define FTP_GREET             220
#define FTP_GOODBYE           221
#define FTP_ABOR_NOCONN       225
#define FTP_TRANSFEROK        226
#define FTP_ABOROK            226
#define FTP_PASVOK            227
#define FTP_EPSVOK            229
#define FTP_LOGINOK           230
#define FTP_AUTHOK            234
#define FTP_CWDOK             250
#define FTP_RMDIROK           250
#define FTP_DELEOK            250
#define FTP_RENAMEOK          250
#define FTP_PWDOK             257
#define FTP_MKDIROK           257

#define FTP_GIVEPWORD         331
#define FTP_RESTOK            350
#define FTP_RNFROK            350

#define FTP_IDLE_TIMEOUT      421
#define FTP_DATA_TIMEOUT      421
#define FTP_TOO_MANY_USERS    421
#define FTP_IP_LIMIT          421
#define FTP_IP_DENY           421
#define FTP_TLS_FAIL          421
#define FTP_BADSENDCONN       425
#define FTP_BADSENDNET        426
#define FTP_BADSENDFILE       451

#define FTP_BADCMD            500
#define FTP_BADOPTS           501
#define FTP_COMMANDNOTIMPL    502
#define FTP_NEEDUSER          503
#define FTP_NEEDRNFR          503
#define FTP_BADPBSZ           503
#define FTP_BADPROT           503
#define FTP_BADSTRU           504
#define FTP_BADMODE           504
#define FTP_BADAUTH           504
#define FTP_NOSUCHPROT        504
#define FTP_NEEDENCRYPT       522
#define FTP_EPSVBAD           522
#define FTP_DATATLSBAD        522
#define FTP_LOGINERR          530
#define FTP_NOHANDLEPROT      536
#define FTP_FILEFAIL          550
#define FTP_NOPERM            550
#define FTP_UPLOADFAIL        553

struct ch_ftp_cmd_t {

	struct list_head node;
	struct list_head ans_list;

	ch_ftp_data_connection_t *ftp_dcon;

	size_t ans_n;

	const char *cmd;
	const char *args;
};


struct ch_ftp_ans_t {

	struct list_head node;
	ch_ftp_cmd_t *cmd;
	uint16_t code;
	const char *phrase;
};

struct ch_ftp_session_t {

	struct list_head cmd_list;

	ch_ftp_cmd_t *cur_cmd;

	ch_pool_t *mp;

	size_t cmd_n;

	const char *user;
	const char *passwd;

};

extern ch_ftp_session_t * ch_ftp_session_create(ch_pool_t *mp);

extern ch_ftp_cmd_t* ch_ftp_session_cmd_add(ch_ftp_session_t *ftp_session,ch_str_t *cmd,ch_str_t *args);

extern ch_ftp_ans_t* ch_ftp_session_ans_add(ch_ftp_session_t *ftp_session,ch_str_t *code,ch_str_t *phrase);

#define IS_CMD(ftp_cmd,cmd_name) (memcmp(cmd_name,ftp_cmd->cmd,strlen(cmd_name))==0)

#define IS_CMD_STR(cmd_str,cmd_name) (memcmp(cmd_name,cmd_str->data,cmd_str->len)==0)

static inline ch_ftp_cmd_t * ch_ftp_session_find_last_cmd(ch_ftp_session_t *ftp_session,const char *cmd){

	ch_ftp_cmd_t *ftp_cmd;
	
	list_for_each_entry(ftp_cmd,&ftp_session->cmd_list,node){
	
		if(IS_CMD(ftp_cmd,cmd))
			return ftp_cmd;
	}

	return NULL;
}

#ifndef STR_IS_EMPTY
#define STR_IS_EMPTY(v) ((v)==NULL||strlen(v)==0)
#endif

#define FIRST_ANS_ENTRY(ftp_cmd,entry_ans) do { \
	if(list_empty(&ftp_cmd->ans_list)) \
		entry_ans = NULL; \
	else \
		entry_ans = list_first_entry(&ftp_cmd->ans_list,ch_ftp_ans_t,node); \
}while(0)

static inline void ch_ftp_session_fpath_name_set(ch_ftp_session_t *ftp_session,const char **fpath,const char **fname){

	ch_ftp_cmd_t *ftp_cmd;
	ch_ftp_ans_t *ans;

	*fpath = "";
	*fname = "";

	list_for_each_entry(ftp_cmd,&ftp_session->cmd_list,node){

		if(IS_CMD(ftp_cmd,CMD_RETR)||IS_CMD(ftp_cmd,CMD_STOR)){

			*fname = ftp_cmd->args;
			break;
		}

	}

	if(STR_IS_EMPTY(*fname)){

		ftp_cmd =  ch_ftp_session_find_last_cmd(ftp_session,CMD_LIST);
		if(ftp_cmd){

			if(!STR_IS_EMPTY(ftp_cmd->args))
				*fpath = ftp_cmd->args;
		}
	}

	if(STR_IS_EMPTY(*fpath)){
		ftp_cmd = ch_ftp_session_find_last_cmd(ftp_session,CMD_PWD);
		
		if(ftp_cmd){
			
			FIRST_ANS_ENTRY(ftp_cmd,ans);
			if(ans){
			
				*fpath = ans->phrase;
			}
		}
	}
}

#endif /*CH_FTP_SESSION_H*/

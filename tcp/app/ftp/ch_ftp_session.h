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
#define CMD_RETR "RETR"
#define CMD_STORE "STORE"
#define CMD_LIST "LIST"

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

#define IS_CMD(ftp_cmd,cmd_name) (memcmp(cmd_name,ftp_cmd->cmd,strlen(cmd_name))==0)

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

#define FIRST_ANS_ENTRY(ftp_cmd,entry) do { \
	if(list_empty(&ftp_cmd->ans_list)) \
		entry = NULL; \
	else \
		entry = list_first_entry(entry,ch_ftp_ans_t,node); \
}while(0)

static inline void ch_ftp_session_fpath_name_set(ch_ftp_session_t *ftp_session,const char **fpath,const char **fname){

	ch_ftp_cmd_t *ftp_cmd;
	ch_ftp_ans_t *ans;

	*fpath = "";
	*fname = "";

	list_for_each_entry(ftp_cmd,&ftp_session->cmd_list,node){

		if(IS_CMD(ftp_cmd,CMD_RETR)||IS_CMD(ftp_cmd,CMD_STORE)){

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

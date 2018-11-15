/*
 *
 *      Filename: ./protocols/mail/ch_mail_context.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 12:15:34
 * Last Modified: 2018-05-18 14:12:36
 */

static const char *cmd_smtp_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_mail_context_t *mcontext = (private_mail_context_t*)_dcfg;

	return ch_config_ports(mcontext->smtp_ports,argc,argv,MAIL_PORTS_MAX);
}

static const char *cmd_pop3_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_mail_context_t *mcontext = (private_mail_context_t*)_dcfg;

	return ch_config_ports(mcontext->pop3_ports,argc,argv,MAIL_PORTS_MAX);
}


static const char *cmd_imap_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_mail_context_t *mcontext = (private_mail_context_t*)_dcfg;

	return ch_config_ports(mcontext->imap_ports,argc,argv,MAIL_PORTS_MAX);
}

static const char *
cmd_mail_content_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1)
{
	private_mail_context_t *mcontext = (private_mail_context_t*)_dcfg;

	mcontext->content_dir = p1;

	return NULL;
}

static const char *
cmd_mail_attach_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1)
{
	private_mail_context_t *mcontext = (private_mail_context_t*)_dcfg;

	mcontext->attach_dir = p1;

	return NULL;
}

static const char *
cmd_mail_body_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc ;
	private_mail_context_t *mcontext = (private_mail_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid mail header dir create type config item!";

	}

	mcontext->create_body_dir_type = rc;

	return NULL;
}

static const command_rec mail_context_directives[] = {
    
    CH_INIT_TAKE_ARGV(
            "CHTCPAPPMailSmtpPorts",
            cmd_smtp_ports,
            NULL,
            0,
            "set  smtp ports"
            ),

    CH_INIT_TAKE_ARGV(
            "CHTCPAPPMailPop3Ports",
            cmd_pop3_ports,
            NULL,
            0,
            "set  pop3 ports"
            ),
	
    CH_INIT_TAKE_ARGV(
            "CHTCPAPPMailImapPorts",
            cmd_imap_ports,
            NULL,
            0,
            "set imap ports"
            ),

    CH_INIT_TAKE1(
            "CHTCPAPPMailContentDir",
            cmd_mail_content_dir,
            NULL,
            0,
            "set the store path for mail content"
            ),

    CH_INIT_TAKE1(
            "CHTCPAPPMailAttachDir",
            cmd_mail_attach_dir,
            NULL,
            0,
            "set the store path for mail attach"
            ),


    CH_INIT_TAKE1(
            "CHTCPAPPMailDirCreateType",
            cmd_mail_body_dir_create_type,
            NULL,
            0,
            "set the type created for content and attachment by time"
            ),

};

static int  do_mail_context_init(ch_pool_t *mp,private_mail_context_t *mcontext,const char *cfname)
{

	const char *err_msg;

	err_msg = ch_process_command_config(mail_context_directives,
		(void*)mcontext, mp, mp,cfname);

    /*config failed*/
    if (err_msg != NULL) {
        ch_log(CH_LOG_ERR, "Config mail context error:%s", err_msg);
        return -1;
    }

    ch_dump_ports(mcontext->smtp_ports,"tcp.mail.smtp",MAIL_PORTS_MAX);
    ch_dump_ports(mcontext->pop3_ports,"tcp.mail.pop3",MAIL_PORTS_MAX);
    ch_dump_ports(mcontext->imap_ports,"tcp.mail.imap",MAIL_PORTS_MAX);
    
    return 0;
}


/*
 *
 *      Filename: do_smtp_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-12 19:42:03
 */

static const char *cmd_smtp_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_smtp_context_t *scontext = (private_smtp_context_t*)_dcfg;

	return ch_config_ports(scontext->smtp_ports,argc,argv,SMTP_PORTS_MAX);
}

static const command_rec scontext_directives[] = {

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPSmtpPorts",
            cmd_smtp_ports,
            NULL,
            0,
            "set  smtp ports"
            ),
};


static int do_smtp_context_init(ch_pool_t *mp,private_smtp_context_t *scontext,const char *cfname){

    const char * msg = NULL;
	
	memset(scontext->smtp_ports,0,SMTP_PORTS_MAX);

    msg = ch_process_command_config(scontext_directives,(void*)scontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app smtp  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(scontext->smtp_ports,"tcp.smtp",SMTP_PORTS_MAX);

    return 0;

}

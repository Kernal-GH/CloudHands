/*
 *
 *      Filename: do_imap_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-12 19:27:23
 */

static const char *cmd_imap_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_imap_context_t *icontext = (private_imap_context_t*)_dcfg;

	return ch_config_ports(icontext->imap_ports,argc,argv,IMAP_PORTS_MAX);
}

static const command_rec icontext_directives[] = {

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPImapPorts",
            cmd_imap_ports,
            NULL,
            0,
            "set  imap ports"
            ),
};


static int do_imap_context_init(ch_pool_t *mp,private_imap_context_t *icontext,const char *cfname){

    const char * msg = NULL;
	
	memset(icontext->imap_ports,0,IMAP_PORTS_MAX);

    msg = ch_process_command_config(icontext_directives,(void*)icontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app imap  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(icontext->imap_ports,"tcp.imap",IMAP_PORTS_MAX);

    return 0;

}

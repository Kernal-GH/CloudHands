/*
 *
 *      Filename: do_pop3_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-12 19:27:23
 */

static const char *cmd_pop3_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_pop3_context_t *pcontext = (private_pop3_context_t*)_dcfg;

	return ch_config_ports(pcontext->pop3_ports,argc,argv,POP3_PORTS_MAX);
}

static const command_rec pcontext_directives[] = {

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPPop3Ports",
            cmd_pop3_ports,
            NULL,
            0,
            "set  pop3 ports"
            ),
};


static int do_pop3_context_init(ch_pool_t *mp,private_pop3_context_t *pcontext,const char *cfname){

    const char * msg = NULL;
	
	memset(pcontext->pop3_ports,0,POP3_PORTS_MAX);

    msg = ch_process_command_config(pcontext_directives,(void*)pcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app pop3  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(pcontext->pop3_ports,"tcp.pop3",POP3_PORTS_MAX);

    return 0;

}

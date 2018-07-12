/*
 *
 *      Filename: do_telnet_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-12 19:27:23
 */

static const char *cmd_telnet_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_telnet_context_t *tcontext = (private_telnet_context_t*)_dcfg;

	return ch_config_ports(tcontext->telnet_ports,argc,argv,TELNET_PORTS_MAX);
}

static const command_rec tcontext_directives[] = {

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPTelnetPorts",
            cmd_telnet_ports,
            NULL,
            0,
            "set  telnet ports"
            ),
};


static int do_telnet_context_init(ch_pool_t *mp,private_telnet_context_t *tcontext,const char *cfname){

    const char * msg = NULL;
	
	memset(tcontext->telnet_ports,0,TELNET_PORTS_MAX);

    msg = ch_process_command_config(tcontext_directives,(void*)tcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app telnet  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(tcontext->telnet_ports,"tcp.telnet",TELNET_PORTS_MAX);

    return 0;

}

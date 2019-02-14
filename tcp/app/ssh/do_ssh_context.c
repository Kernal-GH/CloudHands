/*
 *
 *      Filename: do_ssh_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-09-21 11:49:46
 */

static const char *cmd_ssh_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_ssh_context_t *context = (private_ssh_context_t*)_dcfg;

	return ch_config_ports(context->ssh_ports,argc,argv,SSH_PORTS_MAX);
}

static const command_rec sshcontext_directives[] = {
    
	CH_INIT_TAKE_ARGV(
            "CHTCPAPPSSHPorts",
            cmd_ssh_ports,
            NULL,
            0,
            "set ssh ports"
            ),

};


static void _ssh_context_dump(private_ssh_context_t *context){

	fprintf(stdout,"Dump ssh Config Context Informations:\n");

    ch_dump_ports(context->ssh_ports,"tcp.ssh",SSH_PORTS_MAX);
}

static int do_ssh_context_init(ch_pool_t *mp,private_ssh_context_t *context,const char *cfname){

    const char * msg = NULL;
	

    msg = ch_process_command_config(sshcontext_directives,(void*)context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app ssh  context error:%s",msg);
        return -1;
    }

	_ssh_context_dump(context);

    return 0;

}


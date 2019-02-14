/*
 *
 *      Filename: do_telnet_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-30 10:44:01
 */

static const char *cmd_telnet_content_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_telnet_context_t *mcontextt = (private_telnet_context_t*)_dcfg;

	mcontextt->content_dir = p1;

	return NULL;
}

static const char *cmd_telnet_content_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	private_telnet_context_t *mcontextt = (private_telnet_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid telnet header dir create type config item!";

	}

	mcontextt->create_dir_type = rc;
	return NULL;
}

static const char *cmd_telnet_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_telnet_context_t *mcontext = (private_telnet_context_t*)_dcfg;

	return ch_config_ports(mcontext->telnet_ports,argc,argv,TELNET_PORTS_MAX);
}


static const command_rec mcontext_directives[] = {
    
    CH_INIT_TAKE1(
            "CHTCPAPPTELNETContentDir",
            cmd_telnet_content_dir,
            NULL,
            0,
            "set the store path of telnet content"
            ),

    CH_INIT_TAKE1(
            "CHTCPAPPTELNETContentDirCreateType",
            cmd_telnet_content_dir_create_type,
            NULL,
            0,
            "set the type created of telnet content dir by time"
            ),
	
    CH_INIT_TAKE_ARGV(
            "CHTCPAPPTELNETPorts",
            cmd_telnet_ports,
            NULL,
            0,
            "set telnet ports"
            ),

};


static void _telnet_context_dump(private_telnet_context_t *mcontext){

	fprintf(stdout,"Dump Session Monitor Context Informations:\n");
	fprintf(stdout,"session.monitor.contentDir:%s\n",mcontext->content_dir);
    ch_dump_ports(mcontext->telnet_ports,"tcp.telnet",TELNET_PORTS_MAX);

}

static int do_telnet_context_init(ch_pool_t *mp,private_telnet_context_t *mcontext,const char *cfname){

    const char * msg = NULL;
	

    msg = ch_process_command_config(mcontext_directives,(void*)mcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app telnet  context error:%s",msg);
        return -1;
    }

	_telnet_context_dump(mcontext);

    return 0;

}

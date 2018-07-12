/*
 *
 *      Filename: do_http_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-12 19:27:23
 */

static const char *cmd_http_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_http_context_t *hcontext = (private_http_context_t*)_dcfg;

	return ch_config_ports(hcontext->http_ports,argc,argv,HTTP_PORTS_MAX);
}

static const command_rec hcontext_directives[] = {

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPHttpPorts",
            cmd_http_ports,
            NULL,
            0,
            "set  http ports"
            ),
};


static int do_http_context_init(ch_pool_t *mp,private_http_context_t *hcontext,const char *cfname){

    const char * msg = NULL;
	
	memset(hcontext->http_ports,0,HTTP_PORTS_MAX);

    msg = ch_process_command_config(hcontext_directives,(void*)hcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app http  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(hcontext->http_ports,"tcp.http",HTTP_PORTS_MAX);

    return 0;

}

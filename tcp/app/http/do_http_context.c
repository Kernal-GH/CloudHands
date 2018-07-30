/*
 *
 *      Filename: do_http_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-30 09:58:51
 */

static const char *cmd_http_reqbody_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_http_context_t *hcontext = (private_http_context_t*)_dcfg;

	hcontext->req_body_dir = p1;

	return NULL;
}

static const char *cmd_http_resbody_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_http_context_t *hcontext = (private_http_context_t*)_dcfg;

	hcontext->res_body_dir = p1;

	return NULL;
}


static const char *cmd_http_body_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	private_http_context_t *hcontext = (private_http_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid http header dir create type config item!";

	}

	hcontext->create_dir_type = rc;
	return NULL;
}

static const char *cmd_http_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_http_context_t *hcontext = (private_http_context_t*)_dcfg;

	return ch_config_ports(hcontext->http_ports,argc,argv,HTTP_PORTS_MAX);
}

static const command_rec hcontext_directives[] = {
    
    CH_INIT_TAKE1(
            "CHTCPAPPHttpReqBodyDir",
            cmd_http_reqbody_dir,
            NULL,
            0,
            "set the store path of http request body"
            ),

    CH_INIT_TAKE1(
            "CHTCPAPPHttpResBodyDir",
            cmd_http_resbody_dir,
            NULL,
            0,
            "set the store path of http response body"
            ),

    CH_INIT_TAKE1(
            "CHTCPAPPHttpBodyDirCreateType",
            cmd_http_body_dir_create_type,
            NULL,
            0,
            "set the type created of http body dir by time"
            ),


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
	
    msg = ch_process_command_config(hcontext_directives,(void*)hcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app http  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(hcontext->http_ports,"tcp.http",HTTP_PORTS_MAX);

    return 0;

}

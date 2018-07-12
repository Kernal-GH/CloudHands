/*
 *
 *      Filename: do_ftp_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-12 19:27:23
 */

static const char *cmd_ftp_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_ftp_context_t *fcontext = (private_ftp_context_t*)_dcfg;

	return ch_config_ports(fcontext->ftp_ports,argc,argv,FTP_PORTS_MAX);
}

static const command_rec fcontext_directives[] = {

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPFtpPorts",
            cmd_ftp_ports,
            NULL,
            0,
            "set  ftp ports"
            ),
};


static int do_ftp_context_init(ch_pool_t *mp,private_ftp_context_t *fcontext,const char *cfname){

    const char * msg = NULL;
	
	memset(fcontext->ftp_ports,0,FTP_PORTS_MAX);

    msg = ch_process_command_config(fcontext_directives,(void*)fcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app ftp  context error:%s",msg);
        return -1;
    }

    ch_dump_ports(fcontext->ftp_ports,"tcp.ftp",FTP_PORTS_MAX);

    return 0;

}

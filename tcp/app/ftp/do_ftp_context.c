/*
 *
 *      Filename: do_ftp_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-09-21 11:49:46
 */

static const char *cmd_ftp_store_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_ftp_context_t *fcontext = (private_ftp_context_t*)_dcfg;

	fcontext->fstore_dir = p1;

	return NULL;
}

static const char *cmd_ftp_store_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	private_ftp_context_t *fcontext = (private_ftp_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid ftp header dir create type config item!";

	}

	fcontext->fstore_dir_create_type = rc;
	return NULL;
}

static const char *cmd_ftp_ports(cmd_parms * cmd ch_unused, void *_dcfg, int argc,char *const argv[]){

    private_ftp_context_t *fcontext = (private_ftp_context_t*)_dcfg;

	return ch_config_ports(fcontext->ftp_ports,argc,argv,FTP_PORTS_MAX);
}

static const command_rec fcontext_directives[] = {
    
    CH_INIT_TAKE1(
            "CHTCPAPPFTPFileStoreDir",
            cmd_ftp_store_dir,
            NULL,
            0,
            "set the ftp file store dir"
            ),

    CH_INIT_TAKE1(
            "CHTCPAPPFTPFileStoreDirCreateType",
            cmd_ftp_store_dir_create_type,
            NULL,
            0,
            "set the type created of ftp file store dir by time"
            ),

	CH_INIT_TAKE_ARGV(
            "CHTCPAPPFTPPorts",
            cmd_ftp_ports,
            NULL,
            0,
            "set ftp ports"
            ),

};


static void _ftp_context_dump(private_ftp_context_t *fcontext){

	fprintf(stdout,"Dump FTP Config Context Informations:\n");
	fprintf(stdout,"ftp.file.store.dir:%s\n",fcontext->fstore_dir);

    ch_dump_ports(fcontext->ftp_ports,"tcp.ftp",FTP_PORTS_MAX);
}

static int do_ftp_context_init(ch_pool_t *mp,private_ftp_context_t *fcontext,const char *cfname){

    const char * msg = NULL;
	

    msg = ch_process_command_config(fcontext_directives,(void*)fcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app ftp  context error:%s",msg);
        return -1;
    }

	_ftp_context_dump(fcontext);

    return 0;

}

/*
 *
 *      Filename: do_tftp_app_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-09-14 11:44:42
 */

static const char *cmd_tftp_fstore_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_tftp_app_context_t *context = (private_tftp_app_context_t*)_dcfg;

	context->fstore_dir = p1;

	return NULL;
}

static const char *cmd_tftp_fstore_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	private_tftp_app_context_t *context = (private_tftp_app_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid tftp fstore dir create type config item!";

	}

	context->fstore_dir_create_type = rc;
	return NULL;
}

static const command_rec tftp_app_context_directives[] = {


	CH_INIT_TAKE1(
            "CHUDPAPPTFTPFStoreDir",
            cmd_tftp_fstore_dir,
            NULL,
            0,
            "set the store path of tftp file content"
            ),


	CH_INIT_TAKE1(
            "CHUDPAPPTFTPFStoreDirCreateType",
            cmd_tftp_fstore_dir_create_type,
            NULL,
            0,
            "set the type created of tftp fstore  dir by time(s,m,h,d)"
            ),

};


static void _tftp_app_context_dump(private_tftp_app_context_t *context){

	fprintf(stdout,"Dump TFTP App  Context Informations for UDP:\n");
	fprintf(stdout,"udp.tftp.app.context.fstore.dir:%s\n",context->fstore_dir);
}

static int do_tftp_app_context_init(ch_pool_t *mp,private_tftp_app_context_t *context,const char *cfname){

    const char * msg = NULL;
	

    msg = ch_process_command_config(tftp_app_context_directives,(void*)context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config udp app tftp  context error:%s",msg);
        return -1;
    }

	_tftp_app_context_dump(context);

    return 0;
}

/*
 *
 *      Filename: do_smon_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-16 19:28:55
 */

static const char *cmd_smon_app_mmap_file(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    private_smon_app_context_t *mcontext = (private_smon_app_context_t*)_dcfg;

	if(p1 == NULL || strlen(p1) == 0)
		return "must specify the mmap file path for session monitor!";

    mcontext->mmap_fname = p1;

	return NULL;
}

static const char *cmd_smon_app_mmap_file_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;
    private_smon_app_context_t *mcontext = (private_smon_app_context_t*)_dcfg;


    mcontext->mmap_fsize = (size_t)strtoul(p1,&endptr,10);

	return NULL;
}

static const char *cmd_smon_app_req_body_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_smon_app_context_t *mcontext = (private_smon_app_context_t*)_dcfg;

	mcontext->req_body_dir = p1;

	return NULL;
}

static const char *cmd_smon_app_res_body_dir(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	private_smon_app_context_t *mcontext = (private_smon_app_context_t*)_dcfg;

	mcontext->res_body_dir = p1;

	return NULL;
}


static const char *cmd_smon_app_body_dir_create_type(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

	int rc;

	private_smon_app_context_t *mcontext = (private_smon_app_context_t*)_dcfg;

	rc = ch_fpath_create_type_get(p1);
	if(rc == -1){
	
		return "Invalid smon header dir create type config item!";

	}

	mcontext->create_body_dir_type = rc;
	return NULL;
}


static const command_rec smon_app_context_directives[] = {

	CH_INIT_TAKE1(
            "CHUDPAPPSmonMMapFile",
            cmd_smon_app_mmap_file,
            NULL,
            0,
            "set  session monitor mmap file path"
            ),

	CH_INIT_TAKE1(
            "CHUDPAPPSmonMMapFileSize",
            cmd_smon_app_mmap_file_size,
            NULL,
            0,
            "set  session monitor mmap file size"
            ),

	CH_INIT_TAKE1(
            "CHUDPAPPSmonReqBodyDir",
            cmd_smon_app_req_body_dir,
            NULL,
            0,
            "set the store path of smon request body"
            ),

	CH_INIT_TAKE1(
            "CHUDPAPPSmonResBodyDir",
            cmd_smon_app_res_body_dir,
            NULL,
            0,
            "set the store path of smon response body"
            ),


	CH_INIT_TAKE1(
            "CHUDPAPPSmonBodyDirCreateType",
            cmd_smon_app_body_dir_create_type,
            NULL,
            0,
            "set the type created of smon body dir by time(s,m,h,d)"
            ),

};


static void _smon_app_context_dump(private_smon_app_context_t *mcontext){

	fprintf(stdout,"Dump Session Monitor Context Informations for UDP:\n");
	fprintf(stdout,"udp.smon.app.context.mmapFileName:%s\n",mcontext->mmap_fname);
	fprintf(stdout,"udp.smon.app.context.mmapFileSize:%lu\n",(unsigned long)mcontext->mmap_fsize);
	fprintf(stdout,"udp.smon.app.context.reqBodyDir:%s\n",mcontext->req_body_dir);
	fprintf(stdout,"udp.smon.app.context.resBodyDir:%s\n",mcontext->res_body_dir);


}

static int do_smon_app_context_init(ch_pool_t *mp,private_smon_app_context_t *mcontext,const char *cfname){

    const char * msg = NULL;
	

    msg = ch_process_command_config(smon_app_context_directives,(void*)mcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config udp app smon  context error:%s",msg);
        return -1;
    }

	_smon_app_context_dump(mcontext);

    return 0;

}

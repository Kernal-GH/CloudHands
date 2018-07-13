/*
 *
 *      Filename: do_smon_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 17:30:26
 * Last Modified: 2018-07-13 11:35:37
 */

static const char *cmd_smon_mmap_file(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    private_smon_context_t *mcontext = (private_smon_context_t*)_dcfg;

	if(p1 == NULL || strlen(p1) == 0)
		return "must specify the mmap file path for session monitor!";

    mcontext->mmap_fname = p1;

	return NULL;
}

static const char *cmd_smon_mmap_file_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;
    private_smon_context_t *mcontext = (private_smon_context_t*)_dcfg;


    mcontext->mmap_fsize = (size_t)strtoul(p1,&endptr,10);

	return NULL;
}


static const command_rec mcontext_directives[] = {

	CH_INIT_TAKE1(
            "CHTCPAPPSmonMMapFile",
            cmd_smon_mmap_file,
            NULL,
            0,
            "set  session monitor mmap file path"
            ),

	CH_INIT_TAKE1(
            "CHTCPAPPSmonMMapFileSize",
            cmd_smon_mmap_file_size,
            NULL,
            0,
            "set  session monitor mmap file size"
            ),

};


static void _smon_context_dump(private_smon_context_t *mcontext){

	fprintf(stdout,"Dump Session Monitor Context Informations:\n");
	fprintf(stdout,"session.monitor.mmapFileName:%s\n",mcontext->mmap_fname);
	fprintf(stdout,"session.monitor.mmapFileSize:%lu\n",(unsigned long)mcontext->mmap_fsize);

}

static int do_smon_context_init(ch_pool_t *mp,private_smon_context_t *mcontext,const char *cfname){

    const char * msg = NULL;
	

    msg = ch_process_command_config(mcontext_directives,(void*)mcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config tcp app smon  context error:%s",msg);
        return -1;
    }

	_smon_context_dump(mcontext);

    return 0;

}

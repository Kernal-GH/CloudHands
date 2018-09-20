/*
 *
 *      Filename: ch_ftp_data_session_entry.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 16:34:56
 * Last Modified: 2018-09-20 16:49:22
 */

#include "ch_ftp_data_session_entry.h"
#include "ch_log.h"

void ch_ftp_data_session_entry_init(ch_ftp_data_session_entry_t *ftp_data_entry,ch_pool_t *mp,
	const char *ftp_path,const char *ftp_fname){

	ch_proto_session_entry_init(&smon_entry->psEntry);

	ftp_data_entry->mp = mp;
	ftp_data_entry->ftp_path = ftp_path;
	ftp_data_entry->ftp_fname = ftp_fname;
	ftp_data_entry->fstore_path = NULL;
	ftp_data_entry->fstore_fp = NULL;

}

int  ch_ftp_data_session_entry_fpath_init(ch_ftp_data_session_entry_t *ftp_data_entry,const char *fpath){

	FILE *fp = fopen(fpath,"a+");
	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"cannot open file:%s to writing!",fpath);
		return -1;
	}

	ftp_data_entry->fstore_path = ch_pstrdup(smon_entry->mp,fpath);
	ftp_data_entry->fstore_fp = fp;

	return 0;
}

void ch_ftp_data_session_entry_write(ch_ftp_data_session_entry_t *ftp_data_entry,void *data,size_t dlen){

	fwrite(data,dlen,1,ftp_data_entry->fstore_fp);
	fflush(ftp_data_entry->fstore_fp);

}

void ch_ftp_data_session_entry_fin(ch_ftp_data_session_entry_t *ftp_data_entry){

	if(ftp_data_entry->fstore_fp){

		fclose(ftp_data_entry->fstore_fp);
		ftp_data_entry->fstore_fp = NULL;
	}
}


/*
 * =====================================================================================
 *
 *       Filename:  ch_telnet_session_entry.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2018 04:27:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_telnet_session_entry.h"
#include "ch_log.h"

void ch_telnet_session_entry_init(ch_telnet_session_entry_t *telnet_entry,ch_pool_t *mp){

	ch_proto_session_entry_init(&telnet_entry->psEntry);

	telnet_entry->mp = mp;
	telnet_entry->user = NULL;
    telnet_entry->passwd = NULL;
	telnet_entry->content_fpath = NULL;
	telnet_entry->content_fp = NULL;
}

int  ch_telnet_session_entry_fpath_init(ch_telnet_session_entry_t *telnet_entry,const char *fpath){

	FILE *fp = fopen(fpath,"a+");
	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"cannot open file:%s to writing!",fpath);
		return -1;
	}
    
    telnet_entry->content_fp = fp;
    telnet_entry->content_fpath = ch_pstrdup(telnet_entry->mp,fpath);


	return 0;

}

void ch_telnet_session_entry_write(ch_telnet_session_entry_t *telnet_entry,void *data,size_t dlen){

    fwrite(data,dlen,1,telnet_entry->content_fp);
	fflush(telnet_entry->content_fp);

}

void ch_telnet_session_entry_fin(ch_telnet_session_entry_t *telnet_entry){

	if(telnet_entry->content_fp)
		fclose(telnet_entry->content_fp);


}




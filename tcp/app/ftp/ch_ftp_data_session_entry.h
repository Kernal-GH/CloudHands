/*
 *
 *      Filename: ch_ftp_data_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 16:21:41
 * Last Modified: 2018-09-20 16:21:41
 */

#ifndef CH_FTP_DATA_SESSION_ENTRY_H
#define CH_FTP_DATA_SESSION_ENTRY_H

typedef struct ch_ftp_data_session_entry_t ch_ftp_data_session_entry_t;

#include "ch_ftp_data_connection_pool.h"
#include "ch_proto_session_entry.h"
#include "ch_proto_session_store.h"

struct ch_ftp_data_session_entry_t {

	ch_proto_session_entry_t psEntry;
	ch_pool_t *mp;

    ch_ftp_data_connection_t *ftp_dcon;
	const char *ftp_path;
	const char *ftp_fname;

	const char *fstore_path;
	FILE *fstore_fp;
};

extern void ch_ftp_data_session_entry_init(ch_ftp_data_session_entry_t *ftp_data_entry,ch_pool_t *mp,
        ch_ftp_data_connection_t *ftp_dcon,
        const char *ftp_path,const char *ftp_fname);

extern int  ch_ftp_data_session_entry_fpath_init(ch_ftp_data_session_entry_t *ftp_data_entry,const char *fpath);

extern void ch_ftp_data_session_entry_write(ch_ftp_data_session_entry_t *ftp_data_entry,void *data,size_t dlen);

extern void ch_ftp_data_session_entry_fin(ch_ftp_data_session_entry_t *ftp_data_entry);

extern void ch_ftp_data_session_entry_fin_output(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,ch_ftp_data_session_entry_t *ftp_data_entry);

#endif /*CH_FTP_DATA_SESSION_ENTRY_H*/

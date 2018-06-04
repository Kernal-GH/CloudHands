/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_session_tbl_ops.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 17:59:20
 * Last Modified: 2017-01-04 17:59:20
 */


#ifndef __CH_FTP_SESSION_TBL_OPS_T__
#define __CH_FTP_SESSION_TBL_OPS_T__

uint32_t 
ftp_session_entry_hash(void *key, void *priv_data);

int 
ftp_session_entry_equal(ch_table_entry_t *entry, void *key, void *priv_data);

void 
ftp_session_entry_clean(ch_table_entry_t *tentry,void *priv_data);

ch_table_entry_t * 
ftp_session_entry_create(void *key,void *priv_data);

#endif //__CH_FTP_SESSION_TBL_OPS_T__


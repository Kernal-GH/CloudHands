/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session_tbl_ops.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 18:06:33
 * Last Modified: 2016-12-09 18:06:33
 */

#ifndef __CH_TELNET_SESSION_TBL_OPS_T__
#define __CH_TELNET_SESSION_TBL_OPS_T__

uint32_t 
telnet_session_entry_hash(void *key, void *priv_data);

int 
telnet_session_entry_equal(ch_table_entry_t *entry, void *key, void *priv_data);

void 
telnet_session_entry_clean(ch_table_entry_t *tentry,void *priv_data);

ch_table_entry_t * 
telnet_session_entry_create(void *key,void *priv_data);

#endif //__CH_TELNET_SESSION_TBL_OPS_T__


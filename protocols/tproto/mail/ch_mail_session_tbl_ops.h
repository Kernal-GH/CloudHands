/*
 *
 *      Filename: ./protocols/mail/ch_mail_session_tbl_ops.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 15:09:10
 * Last Modified: 2016-08-01 15:09:10
 */

#ifndef __CH_MAIL_SESSION_TBL_OPS_H__
#define __CH_MAIL_SESSION_TBL_OPS_H__

uint32_t 
mail_session_entry_hash(void *key, void *priv_data);

int 
mail_session_entry_equal(ch_table_entry_t *entry, void *key, void *priv_data);

void 
mail_session_entry_clean(ch_table_entry_t *tentry,void *priv_data);

ch_table_entry_t * 
mail_session_entry_create(void *key,void *priv_data);

#endif //__CH_MAIL_SESSION_TBL_OPS_H__


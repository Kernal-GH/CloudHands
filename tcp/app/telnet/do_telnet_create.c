/*
 * =====================================================================================
 *
 *       Filename:  do_telnet_create.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2018 02:14:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

static void * do_telnet_session_entry_create(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused){

	ch_pool_t *mp;

	ch_telnet_session_entry_t *telnet_entry = NULL;

	mp = ch_pool_create(512);

	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create memory pool for telnet session entry!");
		return NULL;
	}

	telnet_entry = (ch_telnet_session_entry_t*)ch_pcalloc(mp,sizeof(*telnet_entry));

	ch_telnet_session_entry_init(telnet_entry,mp);


	return (void*)telnet_entry;
}


#define TELNET_ENTRY_CAN_STORE(entry) ((entry)->content_fp!=NULL)


static void do_telnet_session_entry_clean(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession){

	ch_telnet_session_entry_t *telnet_entry = (ch_telnet_session_entry_t*)tsession->sentry;

	if (TELNET_ENTRY_CAN_STORE(telnet_entry)) {


		ch_proto_session_store_write(pstore,tsession,(void*)telnet_entry);
	}
	

	ch_telnet_session_entry_fin(telnet_entry);

	ch_pool_destroy(telnet_entry->mp);

}


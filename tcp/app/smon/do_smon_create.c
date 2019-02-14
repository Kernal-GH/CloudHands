/*
 *
 *      Filename: do_smon_other.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 17:41:03
 * Last Modified: 2018-07-17 17:01:06
 */


static void * do_smon_session_entry_create(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused){

	ch_pool_t *mp;

	ch_smon_session_entry_t *smon_entry = NULL;

	mp = ch_pool_create(512);

	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create memory pool for smon session entry!");
		return NULL;
	}

	smon_entry = (ch_smon_session_entry_t*)ch_pcalloc(mp,sizeof(*smon_entry));

	ch_smon_session_entry_init(smon_entry,mp);


	return (void*)smon_entry;
}


#define SMON_ENTRY_CAN_STORE(entry) ((entry)->req_content_fpath!=NULL || (entry)->res_content_fpath!=NULL)


static void do_smon_session_entry_clean(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession){

	ch_smon_session_entry_t *smon_entry = (ch_smon_session_entry_t*)tsession->sentry;
    private_smon_context_t *mcontext = (private_smon_context_t*)app->context;

    uint32_t src_ip = ch_tcp_session_srcip_get(tsession);
    uint32_t dst_ip = ch_tcp_session_dstip_get(tsession);
    uint16_t src_port = ch_tcp_session_srcport_get(tsession);
    uint16_t dst_port = ch_tcp_session_dstport_get(tsession);

    ch_session_monitor_t *monitor = &mcontext->monitor;
	ch_session_monitor_item_t *item;

	if (SMON_ENTRY_CAN_STORE(smon_entry)) {

		item = ch_session_monitor_item_find(monitor,src_ip,dst_ip,src_port,dst_port);
		
		if(item == NULL){
			item = ch_session_monitor_item_find_ignore_state(monitor,src_ip,dst_ip,src_port,dst_port);
		}

		if(item)
			smon_entry->id = item->id;

		ch_proto_session_store_write(pstore,tsession,(void*)smon_entry);
	}
	

	ch_smon_session_entry_fin(smon_entry);

	ch_pool_destroy(smon_entry->mp);

}


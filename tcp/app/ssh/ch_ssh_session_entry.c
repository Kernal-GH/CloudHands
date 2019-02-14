/*
 * =====================================================================================
 *
 *       Filename:  ch_ssh_session_entry.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2018 03:41:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_ssh_session_entry.h"


static inline void _init_data(ch_ssh_data_t *ssh_data){

    ssh_data->state = SSH_STATE_INIT;
    ssh_data->version = SSH_VERSION_UNKNOWN;
    ssh_data->proto_version = NULL;
    ssh_data->pkts = 0;
    ssh_data->bytes = 0;
    ssh_data->enc_pkts = 0;
    ssh_data->enc_bytes = 0;
    ssh_data->enc_minBytes = 0;
    ssh_data->enc_maxBytes = 0;

}

void ch_ssh_session_entry_init(ch_ssh_session_entry_t *ssh_entry,ch_pool_t *mp,ch_buffer_t *g_buffer){


	ch_proto_session_entry_init(&ssh_entry->psEntry);
    ssh_entry->mp = mp;

    _init_data(&ssh_entry->srv_data);
    _init_data(&ssh_entry->cli_data);

	ch_pp_din_init(&ssh_entry->data_input_cli,g_buffer,mp); 
	ch_pp_din_init(&ssh_entry->data_input_srv,g_buffer,mp); 

}

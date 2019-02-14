/*
 *
 *      Filename: ch_tftp_session.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-14 14:37:09
 * Last Modified: 2018-09-14 15:02:15
 */

#include "ch_tftp_packet.h"
#include "ch_tftp_session.h"
#include "ch_log.h"

ch_tftp_session_t * ch_tftp_session_create(ch_pool_t *mp,ch_packet_udp_t *pkt_udp){

	uint16_t opcode;

	void *p = pkt_udp->pdata;
	size_t dlen = pkt_udp->payload_len;

	if(p == NULL||dlen<4){
	
		ch_log(CH_LOG_ERR,"Invalid tftp request packet!");
		return NULL;
	}

	opcode = READ_UINT16(p);
	p = READ_SKIP(p,2);
	if(opcode!=OP_RRQ&&opcode!=OP_WRQ){
		ch_log(CH_LOG_ERR,"Only process tftp read/write file request!");
		return NULL;
	}

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)ch_pcalloc(mp,sizeof(*tftp_session));

	tftp_session->src_ip = pkt_udp->src_ip;
	tftp_session->dst_ip = pkt_udp->dst_ip;
	tftp_session->src_port = pkt_udp->src_port;
	tftp_session->dst_port = pkt_udp->dst_port;
	tftp_session->mp = mp;
	tftp_session->is_read = (opcode == OP_RRQ);
	tftp_session->is_error = 0;
	tftp_session->last_block = 0;
	tftp_session->fname = (const char*)ch_pstrdup(mp,(const char*)p);
	p+= strlen((const char*)p)+1;
	tftp_session->mode = (const char*)ch_pstrdup(mp,(const char*)p);
	tftp_session->errmsg = NULL;
	tftp_session->fpath = NULL;
	tftp_session->fp = NULL;

	return tftp_session;
}

void ch_tftp_session_destroy(ch_tftp_session_t *tftp_session){

	if(tftp_session->fp){
	
		fclose(tftp_session->fp);
		tftp_session->fp = NULL;
	}


}

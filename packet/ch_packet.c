/*
 *
 *      Filename: ch_packet.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-22 10:14:18
 * Last Modified: 2018-08-01 11:44:27
 */

#include "ch_packet.h"
#include "ch_log.h"
#include "ch_list.h"
#include "ch_packet_ipv4.h"
#include "ch_packet_ipv4.h"
#include "ch_packet_tcp.h"
#include "ch_packet_udp.h"
#include "ch_packet_ether.h"
#include "ch_packet_arp.h"
#include "ch_packet_icmp.h"
#include "ch_packet_ipv6.h"

static struct list_head packet_parsers[L_MAX];

void ch_packet_init(void){

	int i;

	for(i =0;i<L_MAX;i++){
	
		INIT_LIST_HEAD(&packet_parsers[i]);
	}

	/*init ipv4 packet parser*/
	ch_packet_ipv4_init();
	
    /*init ipv6 packet parser*/
	ch_packet_ipv6_init();

	/*init tcp packet parser*/
	ch_packet_tcp_init();

	/*init udp packet parser*/
	ch_packet_udp_init();

	/*init arp packet parser*/
	ch_packet_arp_init();
	
	/*init icmp packet parser*/
	ch_packet_icmp_init();
}

void ch_packet_parser_register(ch_packet_parser_t *parser,int level){

	struct list_head *list = &packet_parsers[level];

	list_add_tail(&parser->node,list);

}

static inline ch_packet_parser_t *_packet_parser_find(int level,uint16_t proto){

	struct list_head *list = &packet_parsers[level];

	ch_packet_parser_t *pkt_parser;

	list_for_each_entry(pkt_parser,list,node){


		if(pkt_parser->proto == proto)
			return pkt_parser;

	}

	return NULL;

}



static inline void _packet_init(ch_packet_t *pkt,struct rte_mbuf *mbuf){

	pkt->mbuf = mbuf;
	pkt->l2_len = 0;
	pkt->l3_len = 0;
	pkt->l4_len = 0;

	pkt->l3_proto = 0;
	pkt->l4_proto = 0;
	pkt->parse_off = 0;
	pkt->hash = 0;
	pkt->pkt_type = PKT_TYPE_OTHER;
    pkt->is_ipv6 = 0;
}

int ch_packet_parse(ch_packet_t *pkt,struct rte_mbuf *mbuf){


	int rc;
	ch_packet_parser_t *parser;

	_packet_init(pkt,mbuf);

	/*parse ethernet packet!*/
	rc = ch_packet_ether_parse(pkt);

	if(rc == PKT_PARSE_OK){
	
		/*Parse level 3 protocol*/
		parser = _packet_parser_find(L3_INDEX,pkt->l3_proto);
		if(parser){
		
			rc = parser->parse(pkt);

			if(rc == PKT_PARSE_OK){
			
				/*Parse level 4 protocol*/
				parser = _packet_parser_find(L4_INDEX,pkt->l4_proto);
				if(parser){
				
					rc = parser->parse(pkt);
				}
			}
		}
	}


	return rc;
}

static inline int  _packet_copy(struct rte_mbuf *mi,struct rte_mbuf *m){

	char *dst_data,*src_data;

	if(m->data_len>mi->buf_len)
	{
	
		return -1;
	}

	mi->data_len = m->data_len;
	mi->port = m->port;
	mi->vlan_tci = m->vlan_tci;
	mi->vlan_tci_outer = m->vlan_tci_outer;
	mi->tx_offload = m->tx_offload;
	mi->hash = m->hash;

	mi->next = NULL;
	mi->pkt_len = mi->data_len;
	mi->nb_segs = 1;
	mi->ol_flags = m->ol_flags;
	mi->packet_type = m->packet_type;
	mi->timestamp = m->timestamp;

	dst_data = rte_pktmbuf_mtod(mi, char *);
	src_data = rte_pktmbuf_mtod(m, char *);

	rte_memcpy(dst_data,src_data, m->data_len);


	return 0;

}

ch_packet_t *ch_packet_clone(ch_packet_t *pkt,struct rte_mempool *mp){

	ch_packet_t *mi_pkt;
	int err = 0;
	struct rte_mbuf *mc, *mi, **prev,*md;
	uint32_t pktlen;
	uint8_t nseg;

	if (unlikely ((mc = rte_pktmbuf_alloc(mp)) == NULL))
		return NULL;

	md = pkt->mbuf;

	mi = mc;
	prev = &mi->next;
	pktlen = md->pkt_len;
	nseg = 0;

	do {
		nseg++;

		if(_packet_copy(mi,md) == -1)
		{
		
			err = 1;
			break;
		}

		*prev = mi;
		prev = &mi->next;
	} while ((md = md->next) != NULL &&
	    (mi = rte_pktmbuf_alloc(mp)) != NULL);

	*prev = NULL;
	mc->nb_segs = nseg;
	mc->pkt_len = pktlen;

	/* Allocation of new indirect segment failed */
	if (unlikely (mi == NULL||err)) {
		rte_pktmbuf_free(mc);
		return NULL;
	}

	mi_pkt = ch_packet_get_from_mbuf(mc);
	if(unlikely(mi_pkt == NULL))
	{

		rte_pktmbuf_free(mc);
		return NULL;	
	}

	/*init packet!*/
	mi_pkt->mbuf = mc;
	mi_pkt->l2_len = pkt->l2_len;
	mi_pkt->l3_len = pkt->l3_len;
	mi_pkt->l4_len = pkt->l4_len;
	mi_pkt->l3_proto = pkt->l3_proto;
	mi_pkt->l4_proto = pkt->l4_proto;
	mi_pkt->parse_off = pkt->parse_off;
	mi_pkt->hash = pkt->hash;
	mi_pkt->pkt_type = pkt->pkt_type;

	return mi_pkt;
}

struct ch_packet_t *ch_packet_part_clone(ch_packet_t *pkt,struct rte_mempool *mp,uint32_t dlen){


	struct rte_mbuf *md,*seg = pkt->mbuf,*m = pkt->mbuf;
	uint32_t copy_len,c_dlen = 0;
	char *buf;
	uint32_t r_dlen = CH_MIN(dlen,m->pkt_len); 
	ch_packet_t *m_pkt;

	if(unlikely(r_dlen == 0)){
	
		ch_log(CH_LOG_DEBUG,"No data can been clone!");
		return NULL;
	}

	if (unlikely ((md = rte_pktmbuf_alloc(mp)) == NULL)){
	
		ch_log(CH_LOG_DEBUG,"Cannot alloc a new mbuf!");

		return NULL;
	
	}

	if(unlikely((m_pkt=ch_packet_get_from_mbuf(md)) == NULL)){
	
		ch_log(CH_LOG_DEBUG,"Cannot get pkt from mbuf!");
		rte_pktmbuf_free(md);

		return NULL;
	}

	buf = rte_pktmbuf_mtod_offset(md, char *, 0);

	/*copy data*/
	while (r_dlen > 0&&seg) {
		
		copy_len = rte_pktmbuf_data_len(seg);

		if (copy_len > r_dlen)
			copy_len = r_dlen;

		rte_memcpy(buf,
			rte_pktmbuf_mtod_offset(seg, char *, 0), copy_len);

		buf += copy_len;
		r_dlen -= copy_len;

		seg = seg->next;
		c_dlen += copy_len;
	}

	/*init new mbuf*/
	md->data_len = c_dlen;
	md->pkt_len = m->pkt_len;
	md->port = m->port;
	md->vlan_tci = m->vlan_tci;
	md->vlan_tci_outer = m->vlan_tci_outer;
	md->tx_offload = m->tx_offload;
	md->hash = m->hash;

	md->next = NULL;
	md->nb_segs = 1;
	md->ol_flags = m->ol_flags;
	md->packet_type = m->packet_type;
	md->timestamp = m->timestamp;
	
	/*init packet!*/
	m_pkt->mbuf = md;
	m_pkt->l2_len = pkt->l2_len;
	m_pkt->l3_len = pkt->l3_len;
	m_pkt->l4_len = pkt->l4_len;
	m_pkt->l3_proto = pkt->l3_proto;
	m_pkt->l4_proto = pkt->l4_proto;
	m_pkt->parse_off = pkt->parse_off;
	m_pkt->hash = pkt->hash;
	m_pkt->pkt_type = pkt->pkt_type;

	return m_pkt;
}

void ch_packet_dump(ch_packet_t *pkt,FILE *out){


	fprintf(out,"Dump The Packet Information:\n");
	fprintf(out,"Packet Type:%d\n",(int)pkt->pkt_type);
	fprintf(out,"Packet l2_len:%d\n",(int)pkt->l2_len);
	fprintf(out,"Packet l3_len:%d\n",(int)pkt->l3_len);
	fprintf(out,"Packet l4_len:%d\n",(int)pkt->l4_len);
	fprintf(out,"Packet l3_proto:%d\n",(int)pkt->l3_proto);
	fprintf(out,"Packet l4_proto:%d\n",(int)pkt->l4_proto);
	fprintf(out,"Packet parse_off:%d\n",(int)pkt->parse_off);
	fprintf(out,"Packet hash:%d\n",(int)pkt->hash);


}

/*
 *
 *      Filename: ch_assemble_udp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-16 20:17:47
 * Last Modified: 2017-06-13 16:15:18
 */

#include "ch_log.h"
#include "ch_dpdk_util.h"
#include "ch_assemble_udp.h"
#include "ch_assemble_packet_udp.h"
#include "ch_packets_interface.h"

static inline struct rte_mbuf * _ip_frag_reassemble(ch_assemble_udp_t *uas,struct rte_mbuf *mbuf,struct ipv4_hdr *iph){

	struct rte_mbuf *mo;
	/* process this fragment. */
	mo = rte_ipv4_frag_reassemble_packet(uas->frag_tbl, &uas->dr, mbuf, rte_rdtsc(), iph);

	return mo;
}

static void do_assemble_udp(ch_assemble_t *as,struct rte_mbuf *mbuf){

	ch_assemble_packet_udp_t as_udp_pkt,*as_udp_pkt_ptr = &as_udp_pkt;

	ch_assemble_packet_t *as_pkt = &as_udp_pkt.as_pkt;

	ch_app_t *app;
	ch_assemble_udp_t *uas = (ch_assemble_udp_t*)as;
	struct rte_mbuf *m = mbuf;
	struct ipv4_hdr *iph = ch_ipv4_hdr_get(m); 

	if(rte_ipv4_frag_pkt_is_fragmented(iph)){
	
		m = _ip_frag_reassemble(uas,mbuf,iph);

		if(m == NULL){
	
			/* continue assemble ip fragment */
			rte_ip_frag_free_death_row(&uas->dr,3);

			return;
		}
		/* reassemble ip fragment ok, */

	}

	m->l4_len = sizeof(struct udp_hdr);

	ch_assemble_packet_udp_init(&as_udp_pkt,m,(void*)as);
	ch_packets_interface_put(uas->udp_context->context->pin,(ch_assemble_packet_t*)as_udp_pkt_ptr,PROTO_UDP);
	
	app = ch_app_context_recognize_by_port(uas->aspu->app_udp_context,as_pkt->src_port,as_pkt->dst_port);
	if(app&&as_udp_pkt.payload_len>0&&as_udp_pkt.pdata){
	
		/* write data  */
		ch_app_context_content_process(uas->aspu->app_udp_context,app,as_udp_pkt.pdata,as_udp_pkt.payload_len,
			(void*)as_udp_pkt_ptr);

	}

	rte_ip_frag_free_death_row(&uas->dr,3);
	rte_pktmbuf_free(m);
}

ch_assemble_t * ch_assemble_udp_create(ch_assemble_pool_udp_t *aspu,int assemble_id){

    ch_assemble_context_udp_t *udp_context = &aspu->udp_context;

    ch_assemble_udp_t * uas = NULL;

    uas = (ch_assemble_udp_t*)apr_palloc(udp_context->context->mp,sizeof(*uas));

    uas->aspu = aspu;
	uas->udp_context = udp_context;

    if(-1 == ch_assemble_init(udp_context->context,(ch_assemble_t*)uas,assemble_id,udp_context->mmap_file_dir,"udp",
                udp_context->mmap_file_size,
                udp_context->mmap_file_entry_size,
                do_assemble_udp))
    {
    
        ch_log(CH_LOG_ERR,"Create udp assemble failed!");
        return NULL;
    }

	uas->frag_tbl = rte_ip_frag_table_create(udp_context->ip_frag_tbl_bucket_num,
		udp_context->ip_frag_tbl_bucket_entries,
		udp_context->ip_frag_tbl_max_entries,
		udp_context->ip_frag_tbl_max_cycles,
		ch_socket_id_get());

	if(uas->frag_tbl == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp assemble failed ,cannot create frag table!");
		return NULL;
	}

    return (ch_assemble_t*)uas;

}

#ifndef __CH_DPDK_UTIL_H__
#define __CH_DPDK_UTIL_H__
/*
 *
 *      Filename: util/ch_dpdk_util.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-21 17:17:22
 * Last Modified: 2016-07-21 17:17:22
 */

#include <rte_ether.h>
#include <apr_pools.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>

/*GET ipv4 header*/
static inline struct ipv4_hdr * ch_ipv4_hdr_get(struct rte_mbuf * mbuf){

    return (struct ipv4_hdr*)(rte_pktmbuf_mtod(mbuf,void*) + mbuf->l2_len);
}

/*GET tcp header*/
static inline struct tcp_hdr* ch_tcp_hdr_get(struct rte_mbuf *mbuf){

    return (struct tcp_hdr*)(rte_pktmbuf_mtod(mbuf,void*)+mbuf->l2_len+mbuf->l3_len);
}

/*GET udp header*/
static inline struct udp_hdr* ch_udp_hdr_get(struct rte_mbuf *mbuf){

    return (struct udp_hdr*)(rte_pktmbuf_mtod(mbuf,void*)+mbuf->l2_len+mbuf->l3_len);
}

static inline unsigned ch_socket_id_get(void){

	unsigned cid = rte_lcore_id();
	if(cid == LCORE_ID_ANY)
		return 0;

	return rte_lcore_to_socket_id(cid);
}

extern char * ch_macaddr_str_get(char *buf,size_t blen,struct ether_addr *maddr);

#endif //__CH_DPDK_UTIL_H__

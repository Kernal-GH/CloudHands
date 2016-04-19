/*
 * =====================================================================================
 *
 *       Filename:  ch_util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月26日 16时41分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_UTIL_H
#define CH_UTIL_H

typedef struct ch_four_tuple_t ch_four_tuple_t;

#include <rte_ether.h>
#include <apr_pools.h>
#include <rte_ip.h>
#include <rte_tcp.h>

#define CH_TH_OFF(th)	(((th)->data_off & 0xf0) >> 4)

/* TCP flags */
#define	CH_TH_FIN      0x01
#define	CH_TH_SYN	   0x02
#define	CH_TH_RST	   0x04
#define	CH_TH_PUSH	   0x08
#define	CH_TH_ACK	   0x10
#define	CH_TH_URG	   0x20
#define CH_TH_ECNECHO  0x40	/* ECN Echo */
#define CH_TH_CWR	   0x80	/* ECN Cwnd Reduced */

static inline int is_tcp_syn_packet(struct tcp_hdr *th){
   
    return (th->tcp_flags&CH_TH_SYN)&&((th->tcp_flags&CH_TH_ACK)==0);
}

static inline int is_tcp_syn_ack_packet(struct tcp_hdr *th){
    
    return (th->tcp_flags&CH_TH_SYN)&&(th->tcp_flags&CH_TH_ACK);
}

static inline int is_tcp_fin_packet(struct tcp_hdr *th){
    
    return th->tcp_flags&CH_TH_FIN;
}

static inline int is_tcp_rst_packet(struct tcp_hdr *th){

    return th->tcp_flags&CH_TH_RST;
}

static inline void ch_packet_data_payload_get(struct rte_mbuf *m,void **data,size_t *dlen){

    uint32_t pdlen;
    uint32_t tth_len = m->l2_len+m->l3_len+m->l4_len;

    pdlen = m->data_len - tth_len;

    pdlen == 6?0:pdlen;

    *dlen = (size_t)pdlen;

    *data = pdlen>0?rte_pktmbuf_mtod(m,void*) + tth_len:NULL; 
}

struct ch_four_tuple_t {

    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
};

/*GET ipv4 header*/
static inline struct ipv4_hdr * ch_ipv4_hdr_get(struct rte_mbuf * mbuf){

    return (struct ipv4_hdr*)(rte_pktmbuf_mtod(mbuf,void*) + mbuf->l2_len);
}

/*GET tcp header*/
static inline struct tcp_hdr* ch_tcp_hdr_get(struct rte_mbuf *mbuf){

    return (struct tcp_hdr*)(rte_pktmbuf_mtod(mbuf,void*)+mbuf->l2_len+mbuf->l3_len);
}

static inline void ch_four_tuple_init(ch_four_tuple_t *tuple,struct rte_mbuf *mbuf){

    struct ipv4_hdr * iph = ch_ipv4_hdr_get(mbuf);
    struct tcp_hdr *th = ch_tcp_hdr_get(mbuf);

    tuple->src_ip = iph->src_addr; 
    tuple->dst_ip = iph->dst_addr;
    tuple->src_port = th->src_port;
    tuple->dst_port = th->dst_port;
}

static inline void ch_four_tuple_init2(ch_four_tuple_t *tuple,struct ipv4_hdr *iph,struct tcp_hdr *th){

    tuple->src_ip = iph->src_addr; 
    tuple->dst_ip = iph->dst_addr;
    tuple->src_port = th->src_port;
    tuple->dst_port = th->dst_port;
}

extern char  *ch_current_logtime(apr_pool_t *mp);

extern char  *ch_current_logtime_with_buf(char *buf,size_t buf_size);

extern char  *ch_current_filetime(apr_pool_t *mp);

extern unsigned long ch_get_current_timems(void);

extern char * ch_macaddr_str_get(char *buf,size_t blen,struct ether_addr *maddr);

extern char * ch_ipaddr_str_get(char *buf,size_t blen,uint32_t addr);

extern uint16_t ch_ip_fast_csum(const void *iph, unsigned int ihl);

extern int ch_dir_make(const char *orig_path);

#endif /*CH_UTIL_H*/


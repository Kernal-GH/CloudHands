/*
 * =====================================================================================
 *
 *       Filename:  ch_packet_ipv6.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/02/2019 11:42:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_packet_ipv6.h"

#include "ch_log.h"

/* parse ipv6 extended headers, update offset and return next proto */
int _net_skip_ip6_ext(uint16_t proto, struct rte_mbuf *m, uint16_t *off,
	int *frag)
{
	struct ext_hdr {
		uint8_t next_hdr;
		uint8_t len;
	};
	const struct ext_hdr *xh;
	struct ext_hdr xh_copy;
	unsigned int i;

	*frag = 0;

#define MAX_EXT_HDRS 5
	for (i = 0; i < MAX_EXT_HDRS; i++) {
		switch (proto) {
		case IPPROTO_HOPOPTS:
		case IPPROTO_ROUTING:
		case IPPROTO_DSTOPTS:
			xh = rte_pktmbuf_read(m, *off, sizeof(*xh),
				&xh_copy);
			if (xh == NULL)
				return -1;
			*off += (xh->len + 1) * 8;
			proto = xh->next_hdr;
			break;
		case IPPROTO_FRAGMENT:
			xh = rte_pktmbuf_read(m, *off, sizeof(*xh),
				&xh_copy);
			if (xh == NULL)
				return -1;
			*off += 8;
			proto = xh->next_hdr;
			*frag = 1;
			return proto; /* this is always the last ext hdr */
		case IPPROTO_NONE:
			return 0;
		default:
			return proto;
		}
	}
	return -1;
}

/* get l3 packet type from ip6 next protocol */
static uint32_t
ptype_l3_ip6(uint8_t ip6_proto)
{
	static const uint32_t ip6_ext_proto_map[256] = {
		[IPPROTO_HOPOPTS] = RTE_PTYPE_L3_IPV6_EXT - RTE_PTYPE_L3_IPV6,
		[IPPROTO_ROUTING] = RTE_PTYPE_L3_IPV6_EXT - RTE_PTYPE_L3_IPV6,
		[IPPROTO_FRAGMENT] = RTE_PTYPE_L3_IPV6_EXT - RTE_PTYPE_L3_IPV6,
		[IPPROTO_ESP] = RTE_PTYPE_L3_IPV6_EXT - RTE_PTYPE_L3_IPV6,
		[IPPROTO_AH] = RTE_PTYPE_L3_IPV6_EXT - RTE_PTYPE_L3_IPV6,
		[IPPROTO_DSTOPTS] = RTE_PTYPE_L3_IPV6_EXT - RTE_PTYPE_L3_IPV6,
	};

	return RTE_PTYPE_L3_IPV6 + ip6_ext_proto_map[ip6_proto];
}

static int _packet_ipv6_parse(ch_packet_t *pkt){

    const struct ipv6_hdr *ip6h;
    struct ipv6_hdr ip6h_copy;
    int frag = 0;
	struct rte_mbuf *m = pkt->mbuf;
	uint16_t off = pkt->parse_off;
	uint16_t proto;
	uint32_t pkt_type;
	int ret;

    ip6h = rte_pktmbuf_read(m, off, sizeof(*ip6h), &ip6h_copy);
    if (unlikely(ip6h == NULL)){

		ch_log(CH_LOG_DEBUG,"Invalid ipv6 packet!");
		return PKT_PARSE_DROP;
    }

    proto = ip6h->proto;
    pkt->l3_len = sizeof(*ip6h);
    off += pkt->l3_len;
    pkt->parse_off+=pkt->l3_len;
    pkt->is_ipv6 = 1;
    pkt_type = ptype_l3_ip6(proto);
    if ((pkt_type & RTE_PTYPE_L3_MASK) == RTE_PTYPE_L3_IPV6_EXT) {
        
        ret = _net_skip_ip6_ext(proto, m, &off, &frag);
        if (ret < 0){
		
            ch_log(CH_LOG_DEBUG,"Invalid ipv6 ext packet!");
		
            return PKT_PARSE_DROP;
            
        }

        proto = ret;
        pkt->l3_len = off - pkt->l2_len;
        pkt->parse_off = off;
    }

    if (proto == 0){

		ch_log(CH_LOG_DEBUG,"Invalid ipv6 packet proto value:%d!",(int)proto);
		return PKT_PARSE_DROP;
    }

    if (frag) {
        pkt->l4_len = 0;
		pkt->l4_proto = 0;

		return PKT_PARSE_BREAK;
    }

    pkt->l4_proto = proto;

    return PKT_PARSE_OK;
}

static ch_packet_parser_t ipv6_parser = {

	{NULL,NULL},
	CH_ETH_P_IPV6,
	_packet_ipv6_parse,
};

void ch_packet_ipv6_init(void){

	ch_packet_parser_register(&ipv6_parser,L3_INDEX);

}


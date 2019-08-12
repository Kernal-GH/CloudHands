/*
 *
 *      Filename: ch_packet.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-09 17:40:14
 * Last Modified: 2017-11-09 17:40:14
 */

#ifndef CH_PACKET_H
#define CH_PACKET_H

typedef struct ch_packet_t ch_packet_t;
typedef struct ch_packet_parser_t ch_packet_parser_t;

#include "ch_list.h"
#include <rte_mbuf.h>
#include "ch_atomic.h"

#define L2_INDEX 0
#define L3_INDEX 1
#define L4_INDEX 2
#define L_MAX 3

#define PKT_PARSE_OK 0
#define PKT_PARSE_DROP -1
#define PKT_PARSE_BREAK 1

/*Define packet types*/
#define PKT_TYPE_OTHER 0
#define PKT_TYPE_TCP 1
#define PKT_TYPE_UDP 2
#define PKT_TYPE_ICMP 3
#define PKT_TYPE_ARP 4

struct ch_packet_t {
	
	ch_atomic16_t ref_count;

	struct rte_mbuf *mbuf;

	uint16_t pkt_type;

	uint16_t l2_len;
	uint16_t l3_len;
	uint16_t l4_len;

	uint16_t l3_proto;
	uint16_t l4_proto;

	uint16_t parse_off;

	uint32_t hash;
    uint8_t  is_ipv6;
};


struct ch_packet_parser_t {

	struct list_head node;
	uint16_t proto;


	int (*parse)(ch_packet_t *pkt);

};


/*
*These are the defined Ethernet Protocol ID's.
*/
 
#define CH_ETH_P_LOOP      0x0060          /* Ethernet Loopback packet     */
#define CH_ETH_P_PUP       0x0200          /* Xerox PUP packet             */
#define CH_ETH_P_PUPAT     0x0201          /* Xerox PUP Addr Trans packet  */
#define CH_ETH_P_IP        0x0800          /* Internet Protocol packet     */
#define CH_ETH_P_X25       0x0805          /* CCITT X.25                   */
#define CH_ETH_P_ARP       0x0806          /* Address Resolution packet    */
#define CH_ETH_P_BPQ       0x08FF          /* G8BPQ AX.25 Ethernet Packet  [ NOT AN OFFICIALLY REGISTERED ID ] */
#define CH_ETH_P_IEEEPUP   0x0a00          /* Xerox IEEE802.3 PUP packet */
#define CH_ETH_P_IEEEPUPAT 0x0a01          /* Xerox IEEE802.3 PUP Addr Trans packet */
#define CH_ETH_P_BATMAN    0x4305          /* B.A.T.M.A.N.-Advanced packet [ NOT AN OFFICIALLY REGISTERED ID ] */
#define CH_ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define CH_ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define CH_ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define CH_ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define CH_ETH_P_LAT       0x6004          /* DEC LAT                      */
#define CH_ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define CH_ETH_P_CUST      0x6006          /* DEC Customer use             */
#define CH_ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define CH_ETH_P_TEB       0x6558          /* Trans Ether Bridging         */
#define CH_ETH_P_RARP      0x8035          /* Reverse Addr Res packet      */
#define CH_ETH_P_ATALK     0x809B          /* Appletalk DDP                */
#define CH_ETH_P_AARP      0x80F3          /* Appletalk AARP               */
#define CH_ETH_P_8021Q     0x8100          /* 802.1Q VLAN Extended Header  */
#define CH_ETH_P_IPX       0x8137          /* IPX over DIX                 */
#define CH_ETH_P_IPV6      0x86DD          /* IPv6 over bluebook           */
#define CH_ETH_P_PAUSE     0x8808          /* IEEE Pause frames. See 802.3 31B */
#define CH_ETH_P_SLOW      0x8809          /* Slow Protocol. See 802.3ad 43B */
#define CH_ETH_P_WCCP      0x883E          /* Web-cache coordination protocol
                                           * defined in draft-wilson-wrec-wccp-v2-00.txt */
#define CH_ETH_P_MPLS_UC   0x8847          /* MPLS Unicast traffic         */
#define CH_ETH_P_MPLS_MC   0x8848          /* MPLS Multicast traffic       */
#define CH_ETH_P_ATMMPOA   0x884c          /* MultiProtocol Over ATM       */
#define CH_ETH_P_PPP_DISC  0x8863          /* PPPoE discovery messages     */
#define CH_ETH_P_PPP_SES   0x8864          /* PPPoE session messages       */
#define CH_ETH_P_LINK_CTL  0x886c          /* HPNA, wlan link local tunnel */
#define CH_ETH_P_ATMFATE   0x8884          /* Frame-based ATM Transport
                                            * over Ethernet
                                            */
#define CH_ETH_P_PAE       0x888E          /* Port Access Entity (IEEE 802.1X) */
#define CH_ETH_P_AOE       0x88A2          /* ATA over Ethernet            */
#define CH_ETH_P_8021AD    0x88A8          /* 802.1ad Service VLAN         */
#define CH_ETH_P_802_EX1   0x88B5          /* 802.1 Local Experimental 1.  */
#define CH_ETH_P_TIPC      0x88CA          /* TIPC                         */
#define CH_ETH_P_8021AH    0x88E7          /* 802.1ah Backbone Service Tag */
#define CH_ETH_P_MVRP      0x88F5          /* 802.1Q MVRP                  */
#define CH_ETH_P_1588      0x88F7          /* IEEE 1588 Timesync */
#define CH_ETH_P_PRP       0x88FB          /* IEC 62439-3 PRP/HSRv0        */
#define CH_ETH_P_FCOE      0x8906          /* Fibre Channel over Ethernet  */
#define CH_ETH_P_TDLS      0x890D          /* TDLS */
#define CH_ETH_P_FIP       0x8914          /* FCoE Initialization Protocol */
#define CH_ETH_P_80221     0x8917          /* IEEE 802.21 Media Independent Handover Protocol */
#define CH_ETH_P_LOOPBACK  0x9000          /* Ethernet loopback packet, per IEEE 802.3 */
#define CH_ETH_P_QINQ1     0x9100          /* deprecated QinQ VLAN [ NOT AN OFFICIALLY REGISTERED ID ] */
#define CH_ETH_P_QINQ2     0x9200          /* deprecated QinQ VLAN [ NOT AN OFFICIALLY REGISTERED ID ] */
#define CH_ETH_P_QINQ3     0x9300          /* deprecated QinQ VLAN [ NOT AN OFFICIALLY REGISTERED ID ] */
#define CH_ETH_P_EDSA      0xDADA          /* Ethertype DSA [ NOT AN OFFICIALLY REGISTERED ID ] */
#define CH_ETH_P_AF_IUCV   0xFBFB          /* IBM af_iucv [ NOT AN OFFICIALLY REGISTERED ID ] */

#define CH_ETH_P_802_3_MIN 0x0600          /* If the value in the ethernet type is less than this value
                                          * then the frame is Ethernet II. Else it is 802.3 */

#define CH_ETH_LENGTH_MAX 0x05DC

/* Standard well-defined IP protocols.  */
 enum {
   CH_IPPROTO_IP = 0,               /* Dummy protocol for TCP               */
 #define CH_IPPROTO_IP              CH_IPPROTO_IP
   CH_IPPROTO_ICMP = 1,             /* Internet Control Message Protocol    */
 #define CH_IPPROTO_ICMP            CH_IPPROTO_ICMP
   CH_IPPROTO_IGMP = 2,             /* Internet Group Management Protocol   */
 #define CH_IPPROTO_IGMP            CH_IPPROTO_IGMP
   CH_IPPROTO_IPIP = 4,             /* IPIP tunnels (older KA9Q tunnels use 94) */
 #define CH_IPPROTO_IPIP            CH_IPPROTO_IPIP
   CH_IPPROTO_TCP = 6,              /* Transmission Control Protocol        */
 #define CH_IPPROTO_TCP             CH_IPPROTO_TCP
   CH_IPPROTO_EGP = 8,              /* Exterior Gateway Protocol            */
 #define CH_IPPROTO_EGP             CH_IPPROTO_EGP
   CH_IPPROTO_PUP = 12,             /* PUP protocol                         */
 #define CH_IPPROTO_PUP             CH_IPPROTO_PUP
   CH_IPPROTO_UDP = 17,             /* User Datagram Protocol               */
 #define CH_IPPROTO_UDP             CH_IPPROTO_UDP
   CH_IPPROTO_IDP = 22,             /* XNS IDP protocol                     */
 #define CH_IPPROTO_IDP             CH_IPPROTO_IDP
   CH_IPPROTO_TP = 29,              /* SO Transport Protocol Class 4        */
 #define CH_IPPROTO_TP              CH_IPPROTO_TP
   CH_IPPROTO_DCCP = 33,            /* Datagram Congestion Control Protocol */
 #define CH_IPPROTO_DCCP            CH_IPPROTO_DCCP
   CH_IPPROTO_IPV6 = 41,            /* IPv6-in-IPv4 tunnelling              */
 #define CH_IPPROTO_IPV6            CH_IPPROTO_IPV6
   CH_IPPROTO_RSVP = 46,            /* RSVP Protocol                        */
 #define CH_IPPROTO_RSVP            CH_IPPROTO_RSVP
   CH_IPPROTO_GRE = 47,             /* Cisco GRE tunnels (rfc 1701,1702)    */
 #define CH_IPPROTO_GRE             CH_IPPROTO_GRE
   CH_IPPROTO_ESP = 50,             /* Encapsulation Security Payload protocol */
 #define CH_IPPROTO_ESP             CH_IPPROTO_ESP
   CH_IPPROTO_AH = 51,              /* Authentication Header protocol       */
 #define CH_IPPROTO_AH              CH_IPPROTO_AH
   CH_IPPROTO_MTP = 92,             /* Multicast Transport Protocol         */
 #define CH_IPPROTO_MTP             CH_IPPROTO_MTP
   CH_IPPROTO_BEETPH = 94,          /* IP option pseudo header for BEET     */
 #define CH_IPPROTO_BEETPH          CH_IPPROTO_BEETPH
   CH_IPPROTO_ENCAP = 98,           /* Encapsulation Header                 */
 #define CH_IPPROTO_ENCAP           CH_IPPROTO_ENCAP
   CH_IPPROTO_PIM = 103,            /* Protocol Independent Multicast       */
 #define CH_IPPROTO_PIM             CH_IPPROTO_PIM
   CH_IPPROTO_COMP = 108,           /* Compression Header Protocol          */
 #define CH_IPPROTO_COMP            CH_IPPROTO_COMP
   CH_IPPROTO_SCTP = 132,           /* Stream Control Transport Protocol    */
 #define CH_IPPROTO_SCTP            CH_IPPROTO_SCTP
   CH_IPPROTO_UDPLITE = 136,        /* UDP-Lite (RFC 3828)                  */
 #define CH_IPPROTO_UDPLITE         CH_IPPROTO_UDPLITE
   CH_IPPROTO_MPLS = 137,           /* MPLS in IP (RFC 4023)                */
 #define CH_IPPROTO_MPLS            CH_IPPROTO_MPLS
   CH_IPPROTO_RAW = 255,            /* Raw IP packets                       */
 #define CH_IPPROTO_RAW             CH_IPPROTO_RAW
   CH_IPPROTO_MAX
 };

#define ch_packet_ref_count_set(pkt,v) rte_mbuf_refcnt_set(pkt->mbuf,v)

#if 0
#define ch_packet_free(pkt) do {        \
	struct rte_mbuf *fmbuf = pkt->mbuf; \
	ch_atomic16_sub(&pkt->ref_count,1); \
	if(ch_atomic16_read(&pkt->ref_count) <=0&&fmbuf){ \
		pkt->mbuf = NULL; \
		rte_pktmbuf_free(fmbuf);\
	}\
}while(0)
#endif

#define ch_packet_free(pkt) rte_pktmbuf_free(pkt->mbuf)

#define ch_packet_size(pkt) ((pkt)->mbuf->data_len)

extern void ch_packet_init(void);

extern int ch_packet_parse(ch_packet_t *pkt,struct rte_mbuf *mbuf);

extern void ch_packet_parser_register(ch_packet_parser_t *parser,int level);


static inline ch_packet_t * ch_packet_get_from_mbuf(struct rte_mbuf *mbuf){

	uint16_t priv_size = mbuf->priv_size;
	
	if(priv_size == 0||priv_size<sizeof(ch_packet_t)){
	
		return NULL;
	}

	return (ch_packet_t*)(mbuf+1);
}

extern  ch_packet_t *ch_packet_clone(ch_packet_t *pkt,struct rte_mempool *mp);

extern  ch_packet_t *ch_packet_part_clone(ch_packet_t *pkt,struct rte_mempool *mp,uint32_t dlen);

extern void ch_packet_dump(ch_packet_t *pkt,FILE *out);


#endif /*CH_PACKET_H*/

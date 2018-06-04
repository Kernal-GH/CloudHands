/*
 * =====================================================================================
 *
 *       Filename:  ch_protocol_type.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月27日 11时50分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PROTOCOL_TYPE_H
#define CH_PROTOCOL_TYPE_H

typedef struct ch_protocol_type_t ch_protocol_type_t;

#include <rte_mbuf.h>
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

struct ch_protocol_type_t{
    uint8_t proto;
    void * proto_type_priv;
    int (*handler)(ch_protocol_type_t *protype,struct rte_mbuf *mbuf);
};

#endif /*CH_PROTOCOL_TYPE_H*/

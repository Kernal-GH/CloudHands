/*
 * =====================================================================================
 *
 *       Filename:  ch_ethertype.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月25日 16时03分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ETHERTYPE_H
#define CH_ETHERTYPE_H

typedef struct ch_ethertype_t ch_ethertype_t;

#include <apr_pools.h>
#include "ch_list.h"
#include "ch_packet_type.h"
#include "ch_context.h"
#include "ch_rxtask_pool.h"
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

#define CH_ETH_ETHERTYPE_HASH_SIZE 16
#define CH_ETH_ETHERTYPE_HASH_MASK (CH_ETH_ETHERTYPE_HASH_SIZE-1)

struct ch_ethertype_t{
    struct ch_list_head packet_types[CH_ETH_ETHERTYPE_HASH_SIZE];
    ch_context_t *context; 
};


extern ch_ethertype_t * ch_ethertype_create(ch_context_t *context,ch_rxtask_pool_t *rxtp);

extern void ch_ethertype_packet_type_register(ch_ethertype_t *ethertype,ch_packet_type_t *ptype);

extern ch_packet_type_t * ch_ethertype_packet_type_get(ch_ethertype_t *ethertype,uint16_t type);

static inline struct ch_list_head* ch_ethertype_packet_types_head_get(ch_ethertype_t *ethertype,uint16_t type){

    return &ethertype->packet_types[type&CH_ETH_ETHERTYPE_HASH_MASK];
}   

#endif /*CH_ETHERTYPE_H*/


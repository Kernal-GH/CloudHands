/*
 *
 *      Filename: ch_sa_arp_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-20 18:04:03
 * Last Modified: 2018-04-20 18:04:03
 */

#ifndef CH_SA_ARP_HANDLER_H
#define CH_SA_ARP_HANDLER_H

#include "ch_sa_session_task.h"
#include "ch_packet.h"

extern int ch_sa_arp_handle(ch_sa_session_task_t *sa_task,ch_packet_t *pkt);

#endif /*CH_SA_ARP_HANDLER_H*/

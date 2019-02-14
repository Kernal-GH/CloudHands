/*
 * =====================================================================================
 *
 *       Filename:  ch_ssh_session_entry.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2018 03:41:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SSH_SESSION_ENTRY_H
#define CH_SSH_SESSION_ENTRY_H

typedef struct ch_ssh_session_entry_t ch_ssh_session_entry_t;
typedef struct ch_ssh_data_t ch_ssh_data_t;
typedef struct ch_ssh_packet_t ch_ssh_packet_t;

#include "ch_mpool.h"
#include "ch_pp_data_input.h"
#include "ch_proto_session_entry.h"

#define SSH_STATE_INIT                     0
#define SSH_STATE_VERSION                  1
#define SSH_STATE_NKEY                     4
#define SSH_STATE_ENC                      5


/* MSG_CODE */
// SSH v1 message types (of interest)
#define SSH_MSG_V1_SMSG_PUBLIC_KEY  2
#define SSH_MSG_V1_CMSG_SESSION_KEY 3

// SSH v2 message types (of interest)
#define SSH_MSG_KEXINIT     20
#define SSH_MSG_NEWKEYS     21
#define SSH_MSG_KEXDH_INIT  30
#define SSH_MSG_KEXDH_REPLY 31

#define SSH_MSG_KEXDH_GEX_REQ   34
#define SSH_MSG_KEXDH_GEX_GRP   33
#define SSH_MSG_KEXDH_GEX_INIT  32
#define SSH_MSG_KEXDH_GEX_REPLY 31


#define SSH_VERSION_UNKNOWN     0
#define SSH_VERSION_1           1
#define SSH_VERSION_2           2

struct ch_ssh_data_t {

    uint8_t state;
    uint8_t version;

    const char *proto_version;

    uint64_t pkts;
    uint64_t bytes;
    uint64_t enc_pkts;
    uint64_t enc_bytes;
    uint64_t enc_minBytes;
    uint64_t enc_maxBytes;
};

struct ch_ssh_session_entry_t {

    ch_proto_session_entry_t psEntry;
    ch_pool_t *mp;

    ch_ssh_data_t srv_data;
    ch_ssh_data_t cli_data;
	
    ch_pp_data_input_t data_input_cli;
	ch_pp_data_input_t data_input_srv;

};

struct ch_ssh_packet_t {

    uint32_t pkt_len;
    uint8_t pad_len;
    uint8_t msg_code;

};

extern void ch_ssh_session_entry_init(ch_ssh_session_entry_t *ssh_entry,ch_pool_t *mp,ch_buffer_t *g_buffer);

#endif /*CH_SSH_SESSION_ENTRY_H*/

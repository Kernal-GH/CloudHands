/*
 *
 *      Filename: ch_assemble_context_udp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 15:25:59
 * Last Modified: 2016-10-14 15:25:59
 */

#ifndef CH_ASSEMBLE_CONTEXT_UDP_H
#define CH_ASSEMBLE_CONTEXT_UDP_H

typedef struct ch_assemble_context_udp_t ch_assemble_context_udp_t;

#include "ch_context.h"
struct ch_assemble_context_udp_t {

	ch_context_t *context;
	const char *mmap_file_dir;

    uint64_t mmap_file_size;
    uint64_t mmap_file_entry_size;

	uint32_t ip_frag_tbl_bucket_num;
	uint32_t ip_frag_tbl_bucket_entries;
	uint32_t ip_frag_tbl_max_entries;
	uint64_t ip_frag_tbl_max_cycles;
};

extern int ch_assemble_context_udp_init(ch_context_t *context,ch_assemble_context_udp_t *udp_context);

#endif /* CH_ASSEMBLE_CONTEXT_UDP_H */

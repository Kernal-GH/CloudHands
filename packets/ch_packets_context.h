/*
 *
 *      Filename: ch_packets_context.h
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-05-31 11:08:52
 * Last Modified: 2017-05-31 17:29:54
 */

#ifndef CH_PACKETS_CONTEXT_H
#define CH_PACKETS_CONTEXT_H

typedef struct ch_packets_context_t ch_packets_context_t;

#include <apr_pools.h>

struct ch_packets_context_t {

	apr_pool_t *mp;
	const char *mmap_file_dir;
	uint64_t mmap_file_size;
	uint64_t mmap_file_entry_size;

	uint16_t max_packet_data_size;

};

extern int ch_packets_context_init(ch_packets_context_t *pcontext,const char *cfname);

#endif /*CH_PACKETS_CONTEXT_H*/

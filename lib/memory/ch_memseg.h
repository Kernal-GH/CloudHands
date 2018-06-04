/*
 *
 *      Filename: ch_memseg.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 14:11:59
 * Last Modified: 2017-02-10 14:11:59
 */

#ifndef CH_MEMSEG_H
#define CH_MEMSEG_H

typedef struct ch_memseg_t ch_memseg_t;

#include "ch_constants.h"

/**
 * Physical memory segment descriptor.
 */
struct ch_memseg_t {
	phys_addr_t phys_addr;      /**< Start physical address. */
	union {
		void *addr;         /**< Start virtual address. */
		uint64_t addr_64;   /**< Makes sure addr is always 64 bits */
	};
	size_t len;               /**< Length of the segment. */
	uint64_t hugepage_sz;       /**< The pagesize of underlying memory */
	int32_t socket_id;          /**< NUMA socket ID. */
	uint32_t nchannel;          /**< Number of channels. */
	uint32_t nrank;             /**< Number of ranks. */
} __ch_packed;


int ch_memseg_init(ch_memory_pool_t *mem_pool);

#endif /* CH_MEMSEG_H */

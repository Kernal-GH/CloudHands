/*
 *
 *      Filename: ch_hugepage_info.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 16:00:53
 * Last Modified: 2017-02-10 16:00:53
 */

#ifndef GW_HUGEPAGE_INFO_H
#define GW_HUGEPAGE_INFO_H


typedef struct ch_hugepage_info_t ch_hugepage_info_t;

#include "ch_constants.h"

struct ch_hugepage_info_t {

	uint64_t hugepage_sz;   /**< size of a huge page */
	const char *hugedir;    /**< dir where hugetlbfs is mounted */
	uint32_t num_pages[CH_MAX_NUMA_NODES];
				/**< number of hugepages of that size on each socket */
	int lock_descriptor;    /**< file descriptor for hugepage dir */

};

extern int ch_hugepage_info_init(ch_memory_pool_t *mem_pool);

#endif /* GW_HUGEPAGE_INFO_H */

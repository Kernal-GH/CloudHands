/*
 *
 *      Filename: ch_hugepage_file.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-13 10:53:15
 * Last Modified: 2017-02-13 10:53:15
 */

#ifndef CH_HUGEPAGE_FILE_H
#define CH_HUGEPAGE_FILE_H

#define MAX_HUGEPAGE_PATH PATH_MAX

typedef struct ch_hugepage_file_t ch_hugepage_file_t;

#include "ch_constants.h"

/**
 * Structure used to store informations about hugepages that we mapped
 * through the files in hugetlbfs.
 */
struct ch_hugepage_file_t {
	void *orig_va;      /**< virtual addr of first mmap() */
	void *final_va;     /**< virtual addr of 2nd mmap() */
	uint64_t physaddr;  /**< physical addr */
	size_t size;        /**< the page size */
	int socket_id;      /**< NUMA socket ID */
	int file_id;        /**< the '%d' in HUGEFILE_FMT */
	int memseg_id;      /**< the memory segment to which page belongs */
	char filepath[MAX_HUGEPAGE_PATH]; /**< path to backing file on filesystem */
};


extern int ch_hugepage_file_init(ch_memory_pool_t *mem_pool);

#endif /* CH_HUGEPAGE_FILE_H */

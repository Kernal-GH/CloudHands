/*
 *
 *      Filename: ch_memory_pool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 13:57:49
 * Last Modified: 2017-02-10 13:57:49
 */

#ifndef CH_MEMORY_POOL_H
#define CH_MEMORY_POOL_H

#include <stdint.h>
#include <inttypes.h>
#include "ch_constants.h"
#include "ch_mpool.h"
#include "ch_memseg.h"
#include "ch_hugepage_info.h"
#include "ch_hugepage_file.h"
#include "ch_rwlock.h"

struct ch_memory_pool_t {

	ch_pool_t *mp;

	/*  memory topology */
	uint32_t nchannel;    /* *< Number of channels (0 if unknown). */
	
	uint32_t nrank;       /* *< Number of ranks (0 if unknown). */

	unsigned hugepage_unlink;         /**< true to unlink backing files */
	
	uintptr_t base_virtaddr;          /**< base address to try and reserve memory from */
	
	const char *hugefile_prefix;      /**< the base filename of hugetlbfs files */
	const char *hugepage_dir;         /**< specific hugetlbfs directory to use */
	unsigned num_hugepage_sizes;      /**< how many sizes on this system */


	/* * true to try allocating memory on specific sockets */
	unsigned force_sockets;

	size_t memory;           /* *< amount of asked memory */

	uint64_t socket_mem[CH_MAX_NUMA_NODES]; /* *< amount of memory per socket */

	int nr_hugefiles;

	ch_rwlock_t mlock;

	/* memory segments */
	ch_memseg_t memseg[CH_MAX_MEMSEG];

	ch_hugepage_info_t hugepage_info[MAX_HUGEPAGE_SIZES];

	ch_hugepage_file_t *hugepage_files;
};

#define ch_get_physmem_layout(mem_pool) ((mem_pool)->memseg)

/* return the number of memory channels */
#define ch_memory_get_nchannel(mem_pool) ((mem_pool)->nchannel)

#define ch_memory_get_nrank(mem_pool) ((mem_pool)->nrank)


ch_memory_pool_t * ch_memory_pool_create(ch_pool_t *mp,
	uint32_t nchannel,
	uint32_t nrank,
	unsigned hugepage_unlink,
	unsigned force_sockets,
	uintptr_t base_virtaddr,
	size_t memory,
	const char *hugepage_dir,
	const char *hugefile_prefix);

uint64_t ch_get_physmem_size(ch_memory_pool_t *mem_pool);



/* Dump the physical memory layout on console */
void ch_dump_physmem_layout(ch_memory_pool_t *mem_pool,FILE *f);


size_t ch_get_hugepage_mem_size(ch_memory_pool_t *mem_pool);

/**
 * Lock page in physical memory and prevent from swapping.
 *
 * @param virt
 *   The virtual address.
 * @return
 *   0 on success, negative on error.
 */
int ch_mem_lock_page(const void *virt);

/**
 * Get physical address of any mapped virtual address in the current process.
 * It is found by browsing the /proc/self/pagemap special file.
 * The page must be locked.
 *
 * @param virt
 *   The virtual address.
 * @return
 *   The physical address or CH_BAD_PHYS_ADDR on error.
 */
phys_addr_t ch_mem_virt2phy(const void *virt);



/**
 * Return the physical address of elt, which is an element of the pool mp.
 *
 * @param memseg_id
 *   Identifier of the memory segment owning the physical address. If
 *   set to -1, find it automatically.
 * @param phy_addr
 *   physical address of elt.
 *
 * @return
 *   The physical address or CH_BAD_PHYS_ADDR on error.
 */
static inline phys_addr_t
ch_mem_phy2mch(int32_t memseg_id , const phys_addr_t phy_addr)
{

	/*unused*/
	memseg_id = memseg_id;


	return phy_addr;
}


#endif /* CH_MEMORY_POOL_H */

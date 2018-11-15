/*
 *
 *      Filename: ch_memory_pool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-14 10:47:19
 * Last Modified: 2018-01-23 17:45:27
 */

#include <sys/mman.h>
#include "ch_memory_pool.h"
#include "ch_log.h"

/* get the total size of memory */
uint64_t
ch_get_physmem_size(ch_memory_pool_t *mem_pool)
{
	unsigned i = 0;
	uint64_t total_len = 0;

	for (i = 0; i < CH_MAX_MEMSEG; i++) {
		if (mem_pool->memseg[i].addr == NULL)
			break;

		total_len += mem_pool->memseg[i].len;
	}

	return total_len;
}

/* Dump the physical memory layout on console */
void
ch_dump_physmem_layout(ch_memory_pool_t *mem_pool,FILE *f)
{
	unsigned i = 0;

	for (i = 0; i < CH_MAX_MEMSEG; i++) {
		if (mem_pool->memseg[i].addr == NULL)
			break;

		fprintf(f, "Segment %u: phys:0x%"PRIx64", len:%zu, "
		       "virt:%p, socket_id:%"PRId32", "
		       "hugepage_sz:%"PRIu64", nchannel:%"PRIx32", "
		       "nrank:%"PRIx32"\n", i,
		       mem_pool->memseg[i].phys_addr,
		       mem_pool->memseg[i].len,
		       mem_pool->memseg[i].addr,
		       mem_pool->memseg[i].socket_id,
		       mem_pool->memseg[i].hugepage_sz,
		       mem_pool->memseg[i].nchannel,
		       mem_pool->memseg[i].nrank);
	}
}

size_t
ch_get_hugepage_mem_size(ch_memory_pool_t *mem_pool)
{
	uint64_t size = 0;
	unsigned i, j;

	for (i = 0; i < mem_pool->num_hugepage_sizes; i++) {
		ch_hugepage_info_t *hpi = &mem_pool->hugepage_info[i];
		if (hpi->hugedir != NULL) {
			for (j = 0; j < CH_MAX_NUMA_NODES; j++) {
				size += hpi->hugepage_sz * hpi->num_pages[j];
			}
		}
	}

	return (size < SIZE_MAX) ? (size_t)(size) : SIZE_MAX;
}

#define PFN_MASK_SIZE	8
#define RANDOMIZE_VA_SPACE_FILE "/proc/sys/kernel/randomize_va_space"

static int 
test_proc_pagemap_readable(void)
{
	int fd = open("/proc/self/pagemap", O_RDONLY);

	if (fd < 0) {
		ch_log(CH_LOG_ERR,
			"Cannot open /proc/self/pagemap virt2phys address translation will not work!");
		return 0;
	}

	/* Is readable */
	close(fd);
	return 1;
}

/* Lock page in physical memory and prevent from swapping. */
int
ch_mem_lock_page(const void *virt)
{
	unsigned long virtual = (unsigned long)virt;
	int page_size = getpagesize();
	unsigned long aligned = (virtual & ~ (page_size - 1));
	return mlock((void*)aligned, page_size);
}

/*
 * Get physical address of any mapped virtual address in the current process.
 */
phys_addr_t
ch_mem_virt2phy(const void *virtaddr)
{
	int fd, retval;
	uint64_t page, physaddr;
	unsigned long virt_pfn;
	int page_size;
	off_t offset;

	/* Cannot parse /proc/self/pagemap, no need to log errors everywhere */
	if (!test_proc_pagemap_readable())
		return CH_BAD_PHYS_ADDR;

	/* standard page size */
	page_size = getpagesize();

	fd = open("/proc/self/pagemap", O_RDONLY);
	if (fd < 0) {
		
		ch_log(CH_LOG_ERR, "%s(): cannot open /proc/self/pagemap",
			__func__);

		return CH_BAD_PHYS_ADDR;
	}

	virt_pfn = (unsigned long)virtaddr / page_size;
	offset = sizeof(uint64_t) * virt_pfn;
	if (lseek(fd, offset, SEEK_SET) == (off_t) -1) {
		ch_log(CH_LOG_ERR, "%s(): seek error in /proc/self/pagemap",
				__func__);
		close(fd);
		return CH_BAD_PHYS_ADDR;
	}

	retval = read(fd, &page, PFN_MASK_SIZE);
	close(fd);
	if (retval < 0) {
		ch_log(CH_LOG_ERR, "%s(): cannot read /proc/self/pagemap",
				__func__);

		return CH_BAD_PHYS_ADDR;
	} else if (retval != PFN_MASK_SIZE) {
		ch_log(CH_LOG_ERR, "%s(): read %d bytes from /proc/self/pagemap "
				"but expected %d",
				__func__, retval, PFN_MASK_SIZE);

		return CH_BAD_PHYS_ADDR;
	}

	/*
	 * the pfn (page frame number) are bits 0-54 (see
	 * pagemap.txt in linux Documentation)
	 */
	physaddr = ((page & 0x7fffffffffffffULL) * page_size)
		+ ((unsigned long)virtaddr % page_size);

	return physaddr;
}

ch_memory_pool_t * ch_memory_pool_create(ch_pool_t *mp,
	uint32_t nchannel,
	uint32_t nrank,
	unsigned hugepage_unlink,
	unsigned force_sockets,
	uintptr_t base_virtaddr,
	size_t memory,
	const char *hugepage_dir,
	const char *hugefile_prefix){

	ch_memory_pool_t *mpool = ch_pcalloc(mp,sizeof(*mpool));

	mpool->mp = mp;
	mpool->nchannel = nchannel;
	mpool->nrank = nrank;
	mpool->hugepage_unlink = hugepage_unlink;
	mpool->base_virtaddr = base_virtaddr;
	mpool->hugepage_dir = hugepage_dir;
	mpool->hugefile_prefix = hugefile_prefix;

	mpool->num_hugepage_sizes = 0;
	mpool->force_sockets = force_sockets;
	mpool->memory = memory;
	mpool->nr_hugefiles = 0;
	ch_rwlock_init(&mpool->mlock);
	
	mpool->hugepage_files = NULL;

	if(ch_hugepage_info_init(mpool)){
	
		ch_log(CH_LOG_ERR,"Hugepage info init failed!");
		return NULL;

	}

	if(ch_hugepage_file_init(mpool)){
	
		ch_log(CH_LOG_ERR,"Hugepage file init failed!");
		return NULL;
	}
	
	if(ch_memseg_init(mpool)){
	
		ch_log(CH_LOG_ERR,"Memory segment init failed!");
		return NULL;
	}


	return mpool;
}

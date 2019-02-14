/*
 *
 *      Filename: ch_hugepage_file.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-13 10:53:52
 * Last Modified: 2018-01-23 17:46:48
 */

#include <sys/mman.h>
#include <sys/file.h>
#include "ch_hugepage_file.h"
#include "ch_hugepage_info.h"
#include "ch_log.h"
#include "ch_memory_pool.h"
#include "ch_filesystem.h"

static struct sigaction huge_action_old;
static int huge_need_recover;
static uint64_t baseaddr_offset;
static sigjmp_buf huge_jmpenv;

static void huge_sigbus_handler(int signo)
{
	/*unused */
	signo = signo;

	siglongjmp(huge_jmpenv, 1);
}

/* Put setjmp into a wrap method to avoid compiling error. Any non-volatile,
 * non-static local variable in the stack frame calling sigsetjmp might be
 * clobbered by a call to longjmp.
 */
static int huge_wrap_sigsetjmp(void)
{
	return sigsetjmp(huge_jmpenv, 1);
}

static void
huge_register_sigbus(void)
{
	sigset_t mask;
	struct sigaction action;

	sigemptyset(&mask);
	sigaddset(&mask, SIGBUS);
	action.sa_flags = 0;
	action.sa_mask = mask;
	action.sa_handler = huge_sigbus_handler;

	huge_need_recover = !sigaction(SIGBUS, &action, &huge_action_old);
}

static void
huge_recover_sigbus(void)
{
	if (huge_need_recover) {
		sigaction(SIGBUS, &huge_action_old, NULL);
		huge_need_recover = 0;
	}
}
/*
 * Try to mmap *size bytes in /dev/zero. If it is successful, return the
 * pointer to the mmap'd area and keep *size unmodified. Else, retry
 * with a smaller zone: decrease *size by hugepage_sz until it reaches
 * 0. In this case, return NULL. Note: this function returns an address
 * which is a multiple of hugepage size.
 */
static void *
get_virtual_area(ch_memory_pool_t *mem_pool,size_t *size, size_t hugepage_sz)
{
	void *addr;
	int fd;
	long aligned_addr;

	if (mem_pool->base_virtaddr != 0) {
		addr = (void*) (uintptr_t) (mem_pool->base_virtaddr +
				baseaddr_offset);
	}
	else addr = NULL;

	ch_log(CH_LOG_DEBUG, "Ask a virtual area of %lu bytes", (unsigned long)*size);

	fd = open("/dev/zero", O_RDONLY);
	if (fd < 0){
		ch_log(CH_LOG_ERR, "Cannot open /dev/zero");
		return NULL;
	}
	do {
		addr = mmap(addr,
				(*size) + hugepage_sz, PROT_READ, MAP_PRIVATE, fd, 0);
		if (addr == MAP_FAILED)
			*size -= hugepage_sz;
	} while (addr == MAP_FAILED && *size > 0);

	if (addr == MAP_FAILED) {
		close(fd);
		ch_log(CH_LOG_ERR, "Cannot get a virtual area");
		return NULL;
	}

	munmap(addr, (*size) + hugepage_sz);
	close(fd);

	/* align addr to a huge page size boundary */
	aligned_addr = (long)addr;
	aligned_addr += (hugepage_sz - 1);
	aligned_addr &= (~(hugepage_sz - 1));
	addr = (void *)(aligned_addr);

	ch_log(CH_LOG_DEBUG, "Virtual area found at 0x%x (size =%lu bytes)",
		(unsigned long)addr, (unsigned long)*size);

	/* increment offset */
	baseaddr_offset += *size;

	return addr;
}

/*
 * Mmap all hugepages of hugepage table: it first open a file in
 * hugetlbfs, then mmap() hugepage_sz data in it. If orig is set, the
 * virtual address is stored in hugepg_tbl[i].orig_va, else it is stored
 * in hugepg_tbl[i].final_va. The second mapping (when orig is 0) tries to
 * map continguous physical blocks in contiguous virtual blocks.
 */
static unsigned
map_all_hugepages(ch_memory_pool_t *mem_pool,ch_hugepage_file_t *hugepg_tbl,
		ch_hugepage_info_t *hpi, int orig)
{
	int fd;
	unsigned i;
	void *virtaddr;
	void *vma_addr = NULL;
	size_t vma_len = 0;

	for (i = 0; i < hpi->num_pages[0]; i++) {
		uint64_t hugepage_sz = hpi->hugepage_sz;

		if (orig) {
			hugepg_tbl[i].file_id = i;
			hugepg_tbl[i].size = hugepage_sz;
			ch_get_hugefile_path(hugepg_tbl[i].filepath,
					sizeof(hugepg_tbl[i].filepath), hpi->hugedir,
					mem_pool->hugefile_prefix,
					hugepg_tbl[i].file_id);
			hugepg_tbl[i].filepath[sizeof(hugepg_tbl[i].filepath) - 1] = '\0';
		}

		else if (vma_len == 0) {
			unsigned j, num_pages;

			/* reserve a virtual area for next contiguous
			 * physical block: count the number of
			 * contiguous physical pages. */
			for (j = i+1; j < hpi->num_pages[0] ; j++) {
				if (hugepg_tbl[j].physaddr !=
				    hugepg_tbl[j-1].physaddr + hugepage_sz)
					break;
			}
			num_pages = j - i;
			vma_len = num_pages * hugepage_sz;

			/* get the biggest virtual memory area up to
			 * vma_len. If it fails, vma_addr is NULL, so
			 * let the kernel provide the address. */
			vma_addr = get_virtual_area(mem_pool,&vma_len, hpi->hugepage_sz);
			if (vma_addr == NULL)
				vma_len = hugepage_sz;
		}

		/* try to create hugepage file */
		fd = open(hugepg_tbl[i].filepath, O_CREAT | O_RDWR, 0600);
		if (fd < 0) {
			ch_log(CH_LOG_DEBUG, "%s(): open failed", __func__);
			return i;
		}

		/* map the segment, and populate page tables,
		 * the kernel fills this segment with zeros */
		virtaddr = mmap(vma_addr, hugepage_sz, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_POPULATE, fd, 0);

		if (virtaddr == MAP_FAILED) {
			ch_log(CH_LOG_DEBUG, "%s(): mmap failed", __func__);
			close(fd);
			return i;
		}

		if (orig) {
			hugepg_tbl[i].orig_va = virtaddr;
		}
		else {
			hugepg_tbl[i].final_va = virtaddr;
		}

		if (orig) {
			/* In linux, hugetlb limitations, like cgroup, are
			 * enforced at fault time instead of mmap(), even
			 * with the option of MAP_POPULATE. Kernel will send
			 * a SIGBUS signal. To avoid to be killed, save stack
			 * environment here, if SIGBUS happens, we can jump
			 * back here.
			 */
			if (huge_wrap_sigsetjmp()) {
				ch_log(CH_LOG_DEBUG, "SIGBUS: Cannot mmap more "
					"hugepages of size %u MB",
					(unsigned)(hugepage_sz / 0x100000));

				munmap(virtaddr, hugepage_sz);
				close(fd);
				unlink(hugepg_tbl[i].filepath);
				return i;
			}
			*(int *)virtaddr = 0;
		}


		close(fd);

		vma_addr = (char *)vma_addr + hugepage_sz;
		vma_len -= hugepage_sz;
	}

	return i;
}

/*
 * For each hugepage in hugepg_tbl, fill the physaddr value. We find
 * it by browsing the /proc/self/pagemap special file.
 */
static int
find_physaddrs(ch_hugepage_file_t *hugepg_tbl,ch_hugepage_info_t *hpi)
{
	unsigned i;
	phys_addr_t addr;

	for (i = 0; i < hpi->num_pages[0]; i++) {
		addr = ch_mem_virt2phy(hugepg_tbl[i].orig_va);
		if (addr == CH_BAD_PHYS_ADDR)
			return -1;

		hugepg_tbl[i].physaddr = addr;
	}

	return 0;
}

/*
 * Parse /proc/self/numa_maps to get the NUMA socket ID for each huge
 * page.
 */
static int
find_numasocket(ch_memory_pool_t *mem_pool,ch_hugepage_file_t *hugepg_tbl,ch_hugepage_info_t *hpi)
{
	int socket_id;
	char *end, *nodestr;
	unsigned i, hp_count = 0;
	uint64_t virt_addr;
	char buf[BUFSIZ];
	char hugedir_str[PATH_MAX];
	FILE *f;

	f = fopen("/proc/self/numa_maps", "r");
	if (f == NULL) {
		ch_log(CH_LOG_NOTICE, "cannot open /proc/self/numa_maps,"
				" consider that all memory is in socket_id 0");
		return 0;
	}

	snprintf(hugedir_str, sizeof(hugedir_str),
			"%s/%s", hpi->hugedir, mem_pool->hugefile_prefix);

	/* parse numa map */
	while (fgets(buf, sizeof(buf), f) != NULL) {

		/* ignore non huge page */
		if (strstr(buf, " huge ") == NULL &&
				strstr(buf, hugedir_str) == NULL)
			continue;

		/* get zone addr */
		virt_addr = strtoull(buf, &end, 16);
		if (virt_addr == 0 || end == buf) {
			ch_log(CH_LOG_ERR, "%s(): error in numa_maps parsing", __func__);
			goto error;
		}

		/* get node id (socket id) */
		nodestr = strstr(buf, " N");
		if (nodestr == NULL) {
			ch_log(CH_LOG_ERR, "%s(): error in numa_maps parsing", __func__);
			goto error;
		}

		nodestr += 2;
		end = strstr(nodestr, "=");
		if (end == NULL) {
			ch_log(CH_LOG_ERR, "%s(): error in numa_maps parsing", __func__);
			goto error;
		}
		end[0] = '\0';
		end = NULL;

		socket_id = strtoul(nodestr, &end, 0);
		if ((nodestr[0] == '\0') || (end == NULL) || (*end != '\0')) {
			ch_log(CH_LOG_ERR, "%s(): error in numa_maps parsing", __func__);
			goto error;
		}

		/* if we find this page in our mappings, set socket_id */
		for (i = 0; i < hpi->num_pages[0]; i++) {
			void *va = (void *)(unsigned long)virt_addr;
			if (hugepg_tbl[i].orig_va == va) {
				hugepg_tbl[i].socket_id = socket_id;
				hp_count++;
			}
		}
	}

	if (hp_count < hpi->num_pages[0])
		goto error;

	fclose(f);
	return 0;

error:
	fclose(f);
	return -1;
}

static int
cmp_physaddr(const void *a, const void *b)
{
	const ch_hugepage_file_t *p1 = (const ch_hugepage_file_t *)a;
	const ch_hugepage_file_t *p2 = (const ch_hugepage_file_t *)b;

	if (p1->physaddr < p2->physaddr)
		return -1;
	else if (p1->physaddr > p2->physaddr)
		return 1;
	else
		return 0;
}

/* Unmap all hugepages from original mapping */
static int
unmap_all_hugepages_orig(ch_hugepage_file_t *hugepg_tbl, ch_hugepage_info_t *hpi)
{
        unsigned i;
        for (i = 0; i < hpi->num_pages[0]; i++) {
                if (hugepg_tbl[i].orig_va) {
                        munmap(hugepg_tbl[i].orig_va, hpi->hugepage_sz);
                        hugepg_tbl[i].orig_va = NULL;
                }
        }
        return 0;
}

static inline uint64_t
get_socket_mem_size(ch_memory_pool_t *mem_pool,int socket)
{
	uint64_t size = 0;
	unsigned i;

	for (i = 0; i < mem_pool->num_hugepage_sizes; i++){
		ch_hugepage_info_t *hpi = &mem_pool->hugepage_info[i];
		if (hpi->hugedir != NULL)
			size += hpi->hugepage_sz * hpi->num_pages[socket];
	}

	return size;
}

/*
 * This function is a NUMA-aware equivalent of calc_num_pages.
 * It takes in the list of hugepage sizes and the
 * number of pages thereof, and calculates the best number of
 * pages of each size to fulfill the request for <memory> ram
 */
static int
calc_num_pages_per_socket(ch_memory_pool_t *mem_pool,uint64_t * memory,
		ch_hugepage_info_t *hp_info,
		ch_hugepage_info_t *hp_used,
		unsigned num_hp_info)
{
	unsigned socket, j, i = 0;
	unsigned requested, available;
	int total_num_pages = 0;
	uint64_t remaining_mem, cur_mem;
	uint64_t total_mem = mem_pool->memory;

	if (num_hp_info == 0)
		return -1;

#if 0
	/* if specific memory amounts per socket weren't requested */
	if (mem_pool->force_sockets == 0) {
		int cpu_per_socket[CH_MAX_NUMA_NODES];
		size_t default_size, total_size;
		ch_core_t *core;
		uint32_t core_count = mem_pool->cpool->config_core_count;

		/* Compute number of cores per socket */
		memset(cpu_per_socket, 0, sizeof(cpu_per_socket));
		
		CPOOL_CORE_FOREACH(mem_pool->cpool,core) {
			cpu_per_socket[core->socket]++;
		}

		/*
		 * Automatically spread requested memory amongst detected sockets according
		 * to number of cores from cpu mask present on each socket
		 */
		total_size = mem_pool->memory;
		for (socket = 0; socket < CH_MAX_NUMA_NODES && total_size != 0; socket++) {

			/* Set memory amount per socket */
			default_size = (mem_pool->memory * cpu_per_socket[socket])
			                /core_count;

			/* Limit to maximum available memory on socket */
			default_size = CH_MIN(default_size, get_socket_mem_size(mem_pool,socket));

			/* Update sizes */
			memory[socket] = default_size;
			total_size -= default_size;
		}

		/*
		 * If some memory is remaining, try to allocate it by getting all
		 * available memory from sockets, one after the other
		 */
		for (socket = 0; socket < CH_MAX_NUMA_NODES && total_size != 0; socket++) {
			/* take whatever is available */
			default_size = CH_MIN(get_socket_mem_size(mem_pool,socket) - memory[socket],
			                       total_size);

			/* Update sizes */
			memory[socket] += default_size;
			total_size -= default_size;
		}
	}
#endif

	for (socket = 0; socket < CH_MAX_NUMA_NODES && total_mem != 0; socket++) {
		/* skips if the memory on specific socket wasn't requested */
		for (i = 0; i < num_hp_info && memory[socket] != 0; i++){
			hp_used[i].hugedir = hp_info[i].hugedir;
			hp_used[i].num_pages[socket] = CH_MIN(
					memory[socket] / hp_info[i].hugepage_sz,
					hp_info[i].num_pages[socket]);

			cur_mem = hp_used[i].num_pages[socket] *
					hp_used[i].hugepage_sz;

			memory[socket] -= cur_mem;
			total_mem -= cur_mem;

			total_num_pages += hp_used[i].num_pages[socket];

			/* check if we have met all memory requests */
			if (memory[socket] == 0)
				break;

			/* check if we have any more pages left at this size, if so
			 * move on to next size */
			if (hp_used[i].num_pages[socket] == hp_info[i].num_pages[socket])
				continue;
			/* At this point we know that there are more pages available that are
			 * bigger than the memory we want, so lets see if we can get enough
			 * from other page sizes.
			 */
			remaining_mem = 0;
			for (j = i+1; j < num_hp_info; j++)
				remaining_mem += hp_info[j].hugepage_sz *
				hp_info[j].num_pages[socket];

			/* is there enough other memory, if not allocate another page and quit */
			if (remaining_mem < memory[socket]){
				cur_mem = CH_MIN(memory[socket],
						hp_info[i].hugepage_sz);
				memory[socket] -= cur_mem;
				total_mem -= cur_mem;
				hp_used[i].num_pages[socket]++;
				total_num_pages++;
				break; /* we are done with this socket*/
			}
		}
		/* if we didn't satisfy all memory requirements per socket */
		if (memory[socket] > 0) {
			/* to prevent icc errors */
			requested = (unsigned) (mem_pool->socket_mem[socket] /
					0x100000);
			available = requested -
					((unsigned) (memory[socket] / 0x100000));
			ch_log(CH_LOG_ERR, "Not enough memory available on socket %u! "
					"Requested: %uMB, available: %uMB\n", socket,
					requested, available);
			return -1;
		}
	}

	/* if we didn't satisfy total memory requirements */
	if (total_mem > 0) {
		requested = (unsigned) (mem_pool->memory / 0x100000);
		available = requested - (unsigned) (total_mem / 0x100000);
		ch_log(CH_LOG_ERR, "Not enough memory available! Requested: %uMB,"
				" available: %uMB\n", requested, available);
		return -1;
	}
	return total_num_pages;
}

/*
 * unmaps hugepages that are not going to be used. since we originally allocate
 * ALL hugepages (not just those we need), additional unmapping needs to be done.
 */
static int
unmap_unneeded_hugepages(ch_memory_pool_t *mem_pool,ch_hugepage_file_t *hugepg_tbl,
		ch_hugepage_info_t *hpi,
		unsigned num_hp_info)
{
	unsigned socket, size;
	int page, nrpages = 0;

	/* get total number of hugepages */
	for (size = 0; size < num_hp_info; size++)
		for (socket = 0; socket < CH_MAX_NUMA_NODES; socket++)
			nrpages += mem_pool->hugepage_info[size].num_pages[socket];

	for (size = 0; size < num_hp_info; size++) {
		for (socket = 0; socket < CH_MAX_NUMA_NODES; socket++) {
			unsigned pages_found = 0;

			/* traverse until we have unmapped all the unused pages */
			for (page = 0; page < nrpages; page++) {
				ch_hugepage_file_t *hp = &hugepg_tbl[page];

				/* find a page that matches the criteria */
				if ((hp->size == hpi[size].hugepage_sz) &&
						(hp->socket_id == (int) socket)) {

					/* if we skipped enough pages, unmap the rest */
					if (pages_found == hpi[size].num_pages[socket]) {
						uint64_t unmap_len;

						unmap_len = hp->size;

						/* get start addr and len of the remaining segment */
						munmap(hp->final_va, (size_t) unmap_len);

						hp->final_va = NULL;
						if (unlink(hp->filepath) == -1) {
							ch_log(CH_LOG_ERR, "%s(): Removing %s failed",
									__func__, hp->filepath);
							return -1;
						}
					} else {
						/* lock the page and skip */
						pages_found++;
					}

				} /* match page */
			} /* foreach page */
		} /* foreach socket */
	} /* foreach pagesize */

	return 0;
}

static int
unlink_hugepage_files(ch_memory_pool_t *mem_pool,ch_hugepage_file_t *hugepg_tbl,
		unsigned num_hp_info)
{
	unsigned socket, size;
	int page, nrpages = 0;

	/* get total number of hugepages */
	for (size = 0; size < num_hp_info; size++)
		for (socket = 0; socket < CH_MAX_NUMA_NODES; socket++)
			nrpages +=
			mem_pool->hugepage_info[size].num_pages[socket];

	for (page = 0; page < nrpages; page++) {
		ch_hugepage_file_t *hp = &hugepg_tbl[page];

		if (hp->final_va != NULL && unlink(hp->filepath)) {
			ch_log(CH_LOG_WARN, "%s(): Removing %s failed",
				__func__, hp->filepath);
		}
	}

	return 0;
}



int ch_hugepage_file_init(ch_memory_pool_t *mem_pool){
	
	 ch_hugepage_info_t used_hp[MAX_HUGEPAGE_SIZES];

	uint64_t memory[CH_MAX_NUMA_NODES];

	unsigned hp_offset;
	int i, j;
	int nr_hugefiles, nr_hugepages = 0;

	memset(used_hp, 0, sizeof(used_hp));

	/* calculate total number of hugepages available. at this point we haven't
	 * yet started sorting them so they all are on socket 0 */
	for (i = 0; i < (int) mem_pool->num_hugepage_sizes; i++) {
		/* meanwhile, also initialize used_hp hugepage sizes in used_hp */
		used_hp[i].hugepage_sz = mem_pool->hugepage_info[i].hugepage_sz;

		nr_hugepages += mem_pool->hugepage_info[i].num_pages[0];
	}

	mem_pool->hugepage_files = ch_pcalloc(mem_pool->mp,nr_hugepages * sizeof(ch_hugepage_file_t));

	if (mem_pool->hugepage_files == NULL)
		goto fail;

	hp_offset = 0; /* where we start the current page size entries */
	huge_register_sigbus();

	/* map all hugepages and sort them */
	for (i = 0; i < (int)mem_pool->num_hugepage_sizes; i ++){
		unsigned pages_old, pages_new;
		ch_hugepage_info_t *hpi;

		/*
		 * we don't yet mark hugepages as used at this stage, so
		 * we just map all hugepages available to the system
		 * all hugepages are still located on socket 0
		 */
		hpi = &mem_pool->hugepage_info[i];

		if (hpi->num_pages[0] == 0)
			continue;

		/* map all hugepages available */
		pages_old = hpi->num_pages[0];
		pages_new = map_all_hugepages(mem_pool,&mem_pool->hugepage_files[hp_offset], hpi, 1);

		if (pages_new < pages_old) {
			ch_log(CH_LOG_DEBUG,
				"%d not %d hugepages of size %u MB allocated",
				pages_new, pages_old,
				(unsigned)(hpi->hugepage_sz / 0x100000));

			int pages = pages_old - pages_new;

			nr_hugepages -= pages;
			hpi->num_pages[0] = pages_new;
			if (pages_new == 0)
				continue;
		}

		/* find physical addresses and sockets for each hugepage */
		if (find_physaddrs(&mem_pool->hugepage_files[hp_offset], hpi) < 0){
			ch_log(CH_LOG_DEBUG, "Failed to find phys addr for %u MB pages",
					(unsigned)(hpi->hugepage_sz / 0x100000));
			goto fail;
		}

		if (find_numasocket(mem_pool,&mem_pool->hugepage_files[hp_offset], hpi) < 0){
			ch_log(CH_LOG_DEBUG, "Failed to find NUMA socket for %u MB pages",
					(unsigned)(hpi->hugepage_sz / 0x100000));
			goto fail;
		}

		qsort(&mem_pool->hugepage_files[hp_offset], hpi->num_pages[0],
		      sizeof(ch_hugepage_file_t), cmp_physaddr);

		/* remap all hugepages */
		if (map_all_hugepages(mem_pool,&mem_pool->hugepage_files[hp_offset], hpi, 0) !=
		    hpi->num_pages[0]) {
			ch_log(CH_LOG_ERR, "Failed to remap %u MB pages",
					(unsigned)(hpi->hugepage_sz / 0x100000));
			goto fail;
		}

		/* unmap original mappings */
		if (unmap_all_hugepages_orig(&mem_pool->hugepage_files[hp_offset], hpi) < 0)
			goto fail;

		/* we have processed a num of hugepages of this size, so inc offset */
		hp_offset += hpi->num_pages[0];
	}

	huge_recover_sigbus();

	if (mem_pool->memory == 0 && mem_pool->force_sockets == 0)
		mem_pool->memory = ch_get_hugepage_mem_size(mem_pool);

	nr_hugefiles = nr_hugepages;

	mem_pool->nr_hugefiles = nr_hugefiles;

	/* clean out the numbers of pages */
	for (i = 0; i < (int) mem_pool->num_hugepage_sizes; i++)
		for (j = 0; j < CH_MAX_NUMA_NODES; j++)
			mem_pool->hugepage_info[i].num_pages[j] = 0;

	/* get hugepages for each socket */
	for (i = 0; i < nr_hugefiles; i++) {
		int socket = mem_pool->hugepage_files[i].socket_id;

		/* find a hugepage info with right size and increment num_pages */
		const int nb_hpsizes = CH_MIN(MAX_HUGEPAGE_SIZES,
				(int)mem_pool->num_hugepage_sizes);

		for (j = 0; j < nb_hpsizes; j++) {
			if (mem_pool->hugepage_files[i].size ==
					mem_pool->hugepage_info[j].hugepage_sz) {
				mem_pool->hugepage_info[j].num_pages[socket]++;
			}
		}
	}

	/* make a copy of socket_mem, needed for number of pages calculation */
	for (i = 0; i < CH_MAX_NUMA_NODES; i++)
		memory[i] = mem_pool->socket_mem[i];

	/* calculate final number of pages */
	nr_hugepages = calc_num_pages_per_socket(mem_pool,memory,
			mem_pool->hugepage_info, used_hp,
			mem_pool->num_hugepage_sizes);

	/* error if not enough memory available */
	if (nr_hugepages < 0)
		goto fail;

	/* reporting in! */
	for (i = 0; i < (int) mem_pool->num_hugepage_sizes; i++) {
		for (j = 0; j < CH_MAX_NUMA_NODES; j++) {
			if (used_hp[i].num_pages[j] > 0) {
				ch_log(CH_LOG_DEBUG,
					"Requesting %u pages of size %uMB"
					" from socket %i",
					used_hp[i].num_pages[j],
					(unsigned)
					(used_hp[i].hugepage_sz / 0x100000),
					j);
			}
		}
	}

	/*
	 * unmap pages that we won't need (looks at used_hp).
	 * also, sets final_va to NULL on pages that were unmapped.
	 */
	if (unmap_unneeded_hugepages(mem_pool,mem_pool->hugepage_files, used_hp,
			mem_pool->num_hugepage_sizes) < 0) {
		ch_log(CH_LOG_ERR, "Unmapping and locking hugepages failed!");
		goto fail;
	}

	/* free the hugepage backing files */
	if (mem_pool->hugepage_unlink &&
		unlink_hugepage_files(mem_pool,mem_pool->hugepage_files, mem_pool->num_hugepage_sizes) < 0) {
		ch_log(CH_LOG_ERR, "Unlinking hugepage files failed!");
		goto fail;
	}


	return 0;

fail:
	huge_recover_sigbus();

	return -1;
}


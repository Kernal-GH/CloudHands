/*
 *
 *      Filename: ch_hugepage_info.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 16:16:21
 * Last Modified: 2018-01-23 17:18:27
 */

#include <fnmatch.h>
#include <sys/file.h>
#include "ch_hugepage_info.h"
#include "ch_filesystem.h"
#include "ch_log.h"
#include "ch_memory_pool.h"

static const char sys_dir_path[] = "/sys/kernel/mm/hugepages";

static uint64_t
get_default_hp_size(void)
{
	const char proc_meminfo[] = "/proc/meminfo";
	const char str_hugepagesz[] = "Hugepagesize:";
	unsigned hugepagesz_len = sizeof(str_hugepagesz) - 1;
	char buffer[256];
	unsigned long long size = 0;

	FILE *fd = fopen(proc_meminfo, "r");
	if (fd == NULL){
		ch_log(CH_LOG_ERR,"Cannot open %s", proc_meminfo);
		return 0;
	}

	while(fgets(buffer, sizeof(buffer), fd)){
		if (strncmp(buffer, str_hugepagesz, hugepagesz_len) == 0){
			size = ch_str_to_size(&buffer[hugepagesz_len]);
			break;
		}
	}

	fclose(fd);
	return size;
}

static const char *
get_hugepage_dir(ch_memory_pool_t *mem_pool,uint64_t hugepage_sz)
{
	enum proc_mount_fieldnames {
		DEVICE = 0,
		MOUNTPT,
		FSTYPE,
		OPTIONS,
		_FIELDNAME_MAX
	};
	static uint64_t default_size = 0;
	const char proc_mounts[] = "/proc/mounts";
	const char hugetlbfs_str[] = "hugetlbfs";
	const size_t htlbfs_str_len = sizeof(hugetlbfs_str) - 1;
	const char pagesize_opt[] = "pagesize=";
	const size_t pagesize_opt_len = sizeof(pagesize_opt) - 1;
	const char split_tok = ' ';
	char *splitstr[_FIELDNAME_MAX];
	char buf[BUFSIZ];
	char *retval = NULL;

	FILE *fd = fopen(proc_mounts, "r");
	if (fd == NULL){
		ch_log(CH_LOG_ERR,"Cannot open %s", proc_mounts);
		return NULL;
	}

	if (default_size == 0){
		default_size = get_default_hp_size();
	
		if(default_size == 0){
		
			ch_log(CH_LOG_ERR,"Cannot get default huge page size!");
			return NULL;
		}
	}

	while (fgets(buf, sizeof(buf), fd)){
		if (ch_strsplit(buf, sizeof(buf), splitstr, _FIELDNAME_MAX,
				split_tok) != _FIELDNAME_MAX) {
			
			ch_log(CH_LOG_ERR, "Error parsing %s", proc_mounts);
			break; /* return NULL */
		}

		/* we have a specified --huge-dir option, only examine that dir */
		if (mem_pool->hugepage_dir != NULL &&
				strcmp(splitstr[MOUNTPT], mem_pool->hugepage_dir) != 0)
			continue;

		if (strncmp(splitstr[FSTYPE], hugetlbfs_str, htlbfs_str_len) == 0){
			const char *pagesz_str = strstr(splitstr[OPTIONS], pagesize_opt);

			/* if no explicit page size, the default page size is compared */
			if (pagesz_str == NULL){
				if (hugepage_sz == default_size){
					retval = ch_pstrdup(mem_pool->mp,splitstr[MOUNTPT]);
					break;
				}
			}
			/* there is an explicit page size, so check it */
			else {
				uint64_t pagesz = ch_str_to_size(&pagesz_str[pagesize_opt_len]);
				if (pagesz == hugepage_sz) {
					retval = ch_pstrdup(mem_pool->mp,splitstr[MOUNTPT]);
					break;
				}
			}
		} /* end if strncmp hugetlbfs */
	} /* end while fgets */

	fclose(fd);
	return retval;
}

static uint32_t
get_num_hugepages(const char *subdir)
{
	char path[PATH_MAX];
	long unsigned resv_pages, num_pages = 0;
	const char *nr_hp_file = "free_hugepages";
	const char *nr_rsvd_file = "resv_hugepages";

	/* first, check how many reserved pages kernel reports */
	snprintf(path, sizeof(path), "%s/%s/%s",
			sys_dir_path, subdir, nr_rsvd_file);
	
	if (ch_parse_sysfs_value(path, &resv_pages) < 0)
		return 0;

	snprintf(path, sizeof(path), "%s/%s/%s",
			sys_dir_path, subdir, nr_hp_file);

	if (ch_parse_sysfs_value(path, &num_pages) < 0)
		return 0;

	if (num_pages == 0)
	{
		ch_log(CH_LOG_WARN, "No free hugepages reported in %s",
				subdir);
	}

	/* adjust num_pages */
	if (num_pages >= resv_pages)
		num_pages -= resv_pages;
	else if (resv_pages)
		num_pages = 0;

	/* we want to return a uint32_t and more than this looks suspicious
	 * anyway ... */
	if (num_pages > UINT32_MAX)
		num_pages = UINT32_MAX;

	return num_pages;
}

/*
 * Clear the hugepage directory of whatever hugepage files
 * there are. Checks if the file is locked (i.e.
 * if it's in use by another DPDK process).
 */
static int
clear_hugedir(const char * hugedir)
{
	DIR *dir;
	struct dirent *dirent;
	int dir_fd, fd;
	const char filter[] = "*map_*"; /* matches hugepage files */

	/* open directory */
	dir = opendir(hugedir);
	if (!dir) {
		ch_log(CH_LOG_ERR, "Unable to open hugepage directory %s",
				hugedir);
		goto error;
	}

	dir_fd = dirfd(dir);

	dirent = readdir(dir);
	if (!dirent) {
		ch_log(CH_LOG_ERR, "Unable to read hugepage directory %s",
				hugedir);

		goto error;
	}

	while(dirent != NULL){
		/* skip files that don't match the hugepage pattern */
		if (fnmatch(filter, dirent->d_name, 0) > 0) {
			dirent = readdir(dir);
			continue;
		}

		/* try and lock the file */
		fd = openat(dir_fd, dirent->d_name, O_RDONLY);

		/* skip to next file */
		if (fd == -1) {
			dirent = readdir(dir);
			continue;
		}

		close (fd);
		dirent = readdir(dir);
	}

	closedir(dir);
	return 0;

error:
	if (dir)
		closedir(dir);

	ch_log(CH_LOG_ERR, "Error while clearing hugepage dir");
	return -1;
}

static int
compare_hpi(const void *a, const void *b)
{
	const ch_hugepage_info_t *hpi_a = a;
	const ch_hugepage_info_t *hpi_b = b;

	return hpi_b->hugepage_sz - hpi_a->hugepage_sz;
}

/*
 * when we initialize the hugepage info, everything goes
 * to socket 0 by default. it will later get sorted by memory
 * initialization procedure.
 */
int
ch_hugepage_info_init(ch_memory_pool_t *mem_pool)
{
	const char dirent_start_text[] = "hugepages-";
	const size_t dirent_start_len = sizeof(dirent_start_text) - 1;
	unsigned i, num_sizes = 0;
	DIR *dir;
	struct dirent *dirent;

	dir = opendir(sys_dir_path);
	if (dir == NULL){
		ch_log(CH_LOG_ERR,"Cannot open directory %s to read system hugepage "
			  "info", sys_dir_path);
		return -1;
	}

	for (dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
		ch_hugepage_info_t *hpi;

		if (strncmp(dirent->d_name, dirent_start_text,
			    dirent_start_len) != 0)
			continue;

		if (num_sizes >= MAX_HUGEPAGE_SIZES)
			break;

		hpi = &mem_pool->hugepage_info[num_sizes];
		hpi->hugepage_sz =ch_str_to_size(&dirent->d_name[dirent_start_len]);

		hpi->hugedir = get_hugepage_dir(mem_pool,hpi->hugepage_sz);

		/* first, check if we have a mountpoint */
		if (hpi->hugedir == NULL) {
			uint32_t num_pages;

			num_pages = get_num_hugepages(dirent->d_name);
			if (num_pages > 0)
				ch_log(CH_LOG_NOTICE,
					"%" PRIu32 " hugepages of size "
					"%" PRIu64 " reserved, but no mounted "
					"hugetlbfs found for that size",
					num_pages, hpi->hugepage_sz);
			continue;
		}


		/* clear out the hugepages dir from unused pages */
		if (clear_hugedir(hpi->hugedir) == -1)
			break;

		/* for now, put all pages into socket 0,
		 * later they will be sorted */
		hpi->num_pages[0] = get_num_hugepages(dirent->d_name);

		num_sizes++;
	}

	closedir(dir);

	/* something went wrong, and we broke from the for loop above */
	if (dirent != NULL)
		return -1;

	mem_pool->num_hugepage_sizes = num_sizes;

	/* sort the page directory entries by size, largest to smallest */
	qsort(&mem_pool->hugepage_info[0], num_sizes,
	      sizeof(mem_pool->hugepage_info[0]), compare_hpi);

	/* now we have all info, check we have at least one valid size */
	for (i = 0; i < num_sizes; i++)
		if (mem_pool->hugepage_info[i].hugedir != NULL &&
		    mem_pool->hugepage_info[i].num_pages[0] > 0)
			return 0;

	/* no valid hugepage mounts available, return error */
	return -1;
}

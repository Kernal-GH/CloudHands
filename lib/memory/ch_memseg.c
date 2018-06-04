/*
 *
 *      Filename: ch_memory.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 14:44:19
 * Last Modified: 2018-01-23 17:22:12
 */

#include "ch_memseg.h"
#include "ch_log.h"
#include "ch_memory_pool.h"

int ch_memseg_init(ch_memory_pool_t *mem_pool){

	int i,j,new_memseg;

	ch_hugepage_file_t *hugepage = mem_pool->hugepage_files;

	/* first memseg index shall be 0 after incrementing it below */
	j = -1;
	for (i = 0; i < mem_pool->nr_hugefiles; i++) {
		new_memseg = 0;

		/* if this is a new section, create a new memseg */
		if (i == 0)
			new_memseg = 1;
		
		else if (hugepage[i].socket_id != hugepage[i-1].socket_id)
			new_memseg = 1;
		
		else if (hugepage[i].size != hugepage[i-1].size)
			new_memseg = 1;

		else if ((hugepage[i].physaddr - hugepage[i-1].physaddr) !=
		    hugepage[i].size)
			new_memseg = 1;

		else if (((unsigned long)hugepage[i].final_va -
		    (unsigned long)hugepage[i-1].final_va) != hugepage[i].size)
			new_memseg = 1;

		if (new_memseg) {
			j += 1;
			if (j == CH_MAX_MEMSEG)
				break;

			mem_pool->memseg[j].phys_addr = hugepage[i].physaddr;
			mem_pool->memseg[j].addr = hugepage[i].final_va;
			mem_pool->memseg[j].len = hugepage[i].size;
			mem_pool->memseg[j].socket_id = hugepage[i].socket_id;
			mem_pool->memseg[j].hugepage_sz = hugepage[i].size;
		}
		/* continuation of previous memseg */
		else {

			mem_pool->memseg[j].len += mem_pool->memseg[j].hugepage_sz;
		}

		hugepage[i].memseg_id = j;
	}

	if (i < mem_pool->nr_hugefiles) {
		ch_log(CH_LOG_ERR, "Can only reserve %d pages "
			"from %d requested\n"
			"Current =%d is not enough\n"
			"Please either increase it or request less amount "
			"of memory.\n",
			i, mem_pool->nr_hugefiles,
			CH_MAX_MEMSEG);

		return -1;
	}


	return 0;
}

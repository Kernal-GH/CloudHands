/*
 *
 *      Filename: ch_proto_work.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 00:33:40
 * Last Modified: 2017-01-11 11:22:17
 */

#include <unistd.h>
#include "ch_log.h"
#include "ch_proto_work.h"
#include "ch_proto_pool.h"

static int is_exit = 0;

int ch_proto_work_run(ch_proto_context_t *pcontext)
{
	ch_mmap_file_format_t *mmfmt = pcontext->mmfmt;
	ch_mmap_file_iterator_t *iter = NULL;
	ch_mmap_file_entry_t *fe = NULL; 

	while(!is_exit) {
		
		iter = ch_mmap_file_format_iterator_prefare(mmfmt);
		
		if (iter == NULL) {
			ch_log(CH_LOG_INFO,"%s: iter is null, sleep %d useconds.", __func__, pcontext->work_sleep_time);
			
			usleep(pcontext->work_sleep_time);

			continue;
		}

		while((fe = iter->next(iter))){
			
			/* do parse */
			ch_proto_parse(pcontext->ppool,fe);
		}

        ch_mmap_file_format_iterator_commit(mmfmt, iter);
	}

	/* exit */
	is_exit = 0;

	ch_log(CH_LOG_WARN,"Parser work will exit---------------------------------");

	if (mmfmt && iter)
		ch_mmap_file_format_iterator_commit(mmfmt, iter);

	return 0;
}

void ch_proto_work_exit(ch_proto_context_t *pcontext){

	/* unused */
	pcontext = pcontext;

	is_exit = 1;

}


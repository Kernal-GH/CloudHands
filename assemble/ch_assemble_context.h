/*
 *
 *      Filename: ch_assemble_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-20 09:39:29
 * Last Modified: 2017-11-20 09:39:29
 */

#ifndef	CH_ASSEMBLE_CONTEXT_H
#define CH_ASSEMBLE_CONTEXT_H

typedef struct ch_assemble_context_t ch_assemble_context_t;

#include "ch_mpool.h"
#include "ch_mprocess.h"

#define ASSEMBLE_LOG_NAME_DEFAULT "/tmp/assemble.log"
#define ASSEMBLE_LOG_LEVEL_DEFAULT CH_LOG_NOTICE
#define ASSEMBLE_QPOOL_NAME_DEFAULT "assemble_qpool"
#define ASSEMBLE_QUEUE_NAME_DEFAULT "assemble_queue"

struct ch_assemble_context_t {


	ch_pool_t *mp;


	ch_mprocess_t *mprocess;

	ch_mprocess_queue_t *mprocess_queue;


	const char *qpool_name;
	
	const char *qname;

	const char *log_name;

	int log_level;

};


extern ch_assemble_context_t * ch_assemble_context_create(ch_pool_t *mp,const char *cfname);

extern int ch_assemble_context_start(ch_assemble_context_t *acontext);

extern void ch_assemble_context_stop(ch_assemble_context_t *acontext);

#endif /*CH_ASSEMBLE_CONTEXT_H*/

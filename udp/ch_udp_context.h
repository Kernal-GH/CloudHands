/*
 *
 *      Filename: ch_udp_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 10:29:21
 * Last Modified: 2018-05-08 10:29:21
 */

#ifndef CH_UDP_CONTEXT_H
#define CH_UDP_CONTEXT_H

typedef struct ch_udp_context_t ch_udp_context_t;

#include "ch_mpool.h"

struct ch_udp_context_t {

	ch_pool_t *mp;

	const char *log_name;
	int log_level;
	uint64_t entry_size;
	uint64_t shm_size;

	const char *key;
	int proj_id;

	const char *app_udp_cfname;
	
	const char *pint_udp_cfname;
	
	const char *mmap_file_dir;

	uint16_t tasks_n;

	int  udp_session_pool_type;
	
	size_t udp_session_limits;
	
	size_t udp_session_tbl_size;

	size_t udp_session_cache_limits;

	uint64_t udp_session_timeout;

	int  udp_session_request_pool_type;
	
	size_t udp_session_request_limits;
	
	uint64_t udp_session_request_timeout;

    int use_msgpack;
};

extern ch_udp_context_t * ch_udp_context_create(ch_pool_t *mp,const char *cfname);

#endif /*CH_UDP_CONTEXT_H*/

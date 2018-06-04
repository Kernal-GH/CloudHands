/*
 *
 *      Filename: ch_fpath.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-25 01:43:55
 * Last Modified: 2016-07-25 01:43:55
 */

#ifndef CH_FPATH_H
#define CH_FPATH_H

typedef struct ch_fpath_t ch_fpath_t;

#include "ch_constants.h"
#include "ch_mpool.h"

typedef enum {

    BY_SEC = 0,
    BY_MIN,
    BY_HOUR,
    BY_DAY
}dir_create_by_time_e;

static inline int ch_fpath_create_type_get(const char *v){

	if(*v=='s'||*v=='S')
		return BY_SEC;
	
	if(*v=='m'||*v=='M')
		return BY_MIN;

	if(*v=='h'||*v=='H')
		return BY_HOUR;

	if(*v=='d'||*v=='D')
		return BY_DAY;

	return -1;
}

struct ch_fpath_t {

	const char *root;
	int log_id; /* log identifier(to uniquely id events for multiple snorts) */
	uint64_t counter;
	int create_dir_type;
	int is_need_tmp_buf;

	char *fname_buf;
	char *fname_buf_tmp;
	
	size_t fname_buf_size;
};

static inline uint64_t ch_fpath_counter_update(ch_fpath_t *fpath,uint64_t incr){

	uint64_t ret = fpath->counter;

    fpath->counter = (ret+incr)%UINT64_MAX;
	return ret;
}

#define ch_fpath_fname_buf_size_get(root_dir) (strlen(root_dir)+3+40+2*CH_UINT64_MAX_VALUE_LEN+32)

extern ch_fpath_t* ch_fpath_create(ch_pool_t *mp,const char *root,int create_dir_type,int is_need_tmp_buf, int log_id);

extern int ch_fpath_init(ch_fpath_t *fpath,const char *root,int create_dir_type,int is_need_tmp_buf, int log_id);

extern void ch_fpath_fin(ch_fpath_t *fpath);

extern void ch_fpath_name_create(ch_fpath_t *fpath,char **fname,char **fname_tmp);

#endif /* CH_FPATH_H */

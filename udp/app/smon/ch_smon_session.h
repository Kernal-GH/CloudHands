/*
 *
 *      Filename: ch_smon_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-16 17:47:43
 * Last Modified: 2018-07-16 17:47:43
 */

#ifndef CH_SMON_SESSION_H
#define CH_SMON_SESSION_H

typedef struct ch_smon_session_t ch_smon_session_t;

#include "ch_mpool.h"
#include "ch_data_output.h"
#include "ch_udp_app_pool.h"

struct ch_smon_session_t {

	ch_udp_app_session_t app_session;

	ch_pool_t *mp;

	const char *req_content_fpath;

	const char *res_content_fpath;

	FILE *req_content_fp;
	FILE *res_content_fp;

};


extern ch_smon_session_t * ch_smon_session_create(ch_pool_t *mp);

extern void ch_smon_session_destroy(ch_smon_session_t *smon_session);

extern int ch_smon_session_content_fpath_init(ch_smon_session_t *smon_session,const char *fpath,int is_req);

extern void ch_smon_session_content_write(ch_smon_session_t *smon_session,void *data,size_t dlen,int is_req);

static inline void ch_smon_session_dump(ch_smon_session_t *smon_session,FILE *fp){

	fprintf(fp,"smon.session:\n");
	fprintf(fp,"smon.session.reqBodyPath:%s\n",smon_session->req_content_fpath);
	fprintf(fp,"smon.session.resBodyPath:%s\n",smon_session->res_content_fpath);

}

static inline ssize_t ch_smon_session_write(ch_smon_session_t *smon_session,ch_data_output_t *dout){

	ssize_t rc,len = 0;

	CH_DOUT_STRING16_WRITE2(dout,smon_session->req_content_fpath,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,smon_session->res_content_fpath,len,rc);

	return len;
}

static inline int ch_smon_session_store(ch_smon_session_t *smon_session,ch_msgpack_store_t *dstore,uint64_t id){


    ch_msgpack_store_map_start(dstore,"smon",3);

    ch_msgpack_store_write_uint64(dstore,"id",id);
    ch_msgpack_store_write_kv(dstore,"reqFpath",smon_session->req_content_fpath);
    ch_msgpack_store_write_kv(dstore,"resFpath",smon_session->res_content_fpath);


    return 0;
}


#endif /*CH_SMON_SESSION_H*/

/*
 *
 *      Filename: ch_tftp_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-11 11:42:49
 * Last Modified: 2018-09-11 11:42:49
 */

#ifndef CH_TFTP_SESSION_H
#define CH_TFTP_SESSION_H

typedef struct ch_tftp_session_t ch_tftp_session_t;

#include "ch_log.h"
#include "ch_udp_app_pool.h"
#include "ch_msgpack_store.h"

struct ch_tftp_session_t {

	ch_udp_app_session_t app_session;

	uint32_t src_ip;
	uint32_t dst_ip;
	uint32_t src_port;
	uint32_t dst_port;

	ch_pool_t *mp;

	int is_read;
	int is_error;

	uint16_t last_block;

	const char *fname;
	const char *mode;
	const char *errmsg;

	const char *fpath;
	FILE *fp;
};

extern ch_tftp_session_t * ch_tftp_session_create(ch_pool_t *mp,ch_packet_udp_t *pkt_udp);

extern void ch_tftp_session_destroy(ch_tftp_session_t *tftp_session);

static inline void ch_tftp_session_dump(ch_tftp_session_t *tftp_session,FILE *fp){

	fprintf(fp,"tftp.session:\n");
	fprintf(fp,"tftp.session.action:%s\n",tftp_session->is_read?"read":"write");
	fprintf(fp,"tftp.session.isError:%s\n",tftp_session->is_error?"true":"false");
	fprintf(fp,"tftp.session.lastBlock:%lu\n",(unsigned long)tftp_session->last_block);
	fprintf(fp,"tftp.session.fname:%s\n",tftp_session->fname==NULL?"":tftp_session->fname);
	fprintf(fp,"tftp.session.mode:%s\n",tftp_session->mode==NULL?"":tftp_session->mode);
	fprintf(fp,"tftp.session.errmsg:%s\n",tftp_session->errmsg==NULL?"":tftp_session->errmsg);
	fprintf(fp,"tftp.session.fpath:%s\n",tftp_session->fpath==NULL?"":tftp_session->fpath);


}

static inline ssize_t ch_tftp_session_write(ch_tftp_session_t *tftp_session,ch_data_output_t *dout){

	ssize_t rc,len = 0;
	if(tftp_session == NULL)
		return 0;

	CH_DOUT_UINT8_WRITE(dout,tftp_session->is_read?1:0,len,rc);
	CH_DOUT_UINT8_WRITE(dout,tftp_session->is_error?1:0,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,tftp_session->fname,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,tftp_session->mode,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,tftp_session->errmsg,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,tftp_session->fpath,len,rc);

	return len;
}

static inline int ch_tftp_session_store(ch_tftp_session_t *tftp_session,ch_msgpack_store_t *dstore){

    if(tftp_session == NULL)
        return -1;

    ch_msgpack_store_map_start(dstore,"tftp",6);
    ch_msgpack_store_write_uint8(dstore,"isRead",tftp_session->is_read?1:0);
    ch_msgpack_store_write_uint8(dstore,"isError",tftp_session->is_error?1:0);
    ch_msgpack_store_write_kv(dstore,"fname",tftp_session->fname);
    ch_msgpack_store_write_kv(dstore,"mode",tftp_session->mode);
    ch_msgpack_store_write_kv(dstore,"errmsg",tftp_session->errmsg);
    ch_msgpack_store_write_kv(dstore,"fpath",tftp_session->fpath);

    return 0;
}

static inline int ch_tftp_session_fpath_init(ch_tftp_session_t *tftp_session,const char *fpath){

	FILE *fp = fopen(fpath,"a+");

	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"cannot open file:%s to writing!",fpath);
		return -1;
	}
	
	tftp_session->fp  = fp;
	tftp_session->fpath = ch_pstrdup(tftp_session->mp,fpath);


	return 0;
}

#define ch_tftp_session_file_write(tftp_session,data,dlen) do { \
	fwrite(data,dlen,1,tftp_session->fp); \
	fflush(tftp_session->fp); \
}while(0)


#endif /*CH_TFTP_SESSION_H*/

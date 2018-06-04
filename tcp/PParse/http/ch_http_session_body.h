/*
 * =====================================================================================
 *
 *       Filename:  ch_http_session_body.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月24日 10时42分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_HTTP_SESSION_BODY_H
#define CH_HTTP_SESSION_BODY_H

#include "ch_sfile.h"
#include "ch_http_data_parse.h"

typedef struct {
    ch_sfile_t sfile; /*the file innernest used to store body*/
	int saved;

	ch_pool_t *mp;

    /*file name used to store body*/
    const char * fname; 
    
    /*if the body content has been chunk code,
     * then,this @chunk will poniter to a instance 
     * of ch_http_chunked_t ,used to store chunk state parsed*/
    ch_http_chunked_t *chunk; 
    
    /*if can known the body len from HTTP's header
     * ,then this @body_len value set to it's value*/
    size_t body_len;
    
    /*current body length has been accepted*/
    size_t body_len_accepted;

} ch_http_session_body_t;


static inline void ch_http_session_body_chunk_init(ch_http_session_body_t *body){

	ch_http_chunked_t *chunk = NULL;
	
	if(body->chunk == NULL){
	
		chunk = (ch_http_chunked_t*)ch_palloc(body->mp,sizeof(ch_http_chunked_t));
		body->chunk = chunk;

		chunk->size = 0;
		chunk->length = 0;
		chunk->din = NULL;
		chunk->state = 0;
	}
}

/*Create an instance of session body
 * @mp,the memory pool instance used to create session body instance
 * @fname,the file name used to store body
 * @body_len,the total size of session body
 * return: if ok,return the instance of session body,otherwise return NULL*/
extern ch_http_session_body_t * ch_http_session_body_create(ch_pool_t *mp,const char *fname,size_t body_len);


/*Destroy an instance of session body
 * @body,the instance to destroy*/
extern void ch_http_session_body_destroy(ch_http_session_body_t *body);


/*Write data into body's file
 * @body,the instance of session body
 * @data,the data start address writting
 * @data_len,the data length writting
 * return: if ok,return 0,otherwise return -1*/
extern int ch_http_body_append(ch_http_session_body_t *body,void *data,size_t data_len);

#endif /* CH_HTTP_SESSION_BODY_H */


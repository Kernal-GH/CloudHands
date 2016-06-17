/*
 * =====================================================================================
 *
 *       Filename:  ch_http_session_buf.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月18日 15时32分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_HTTP_SESSION_BUF_H
#define CH_HTTP_SESSION_BUF_H

typedef struct ch_http_session_buf_t ch_http_session_buf_t; 

/*include hear*/


struct ch_http_session_buf_t {

    size_t offset;          /*the offset that is in low level storage handled currently*/
    unsigned char* start;  /*the buf start address */
    unsigned char* pos;    /*the buf postion handled currently*/ 
    unsigned char* end;    /*the buf end address*/
};

#endif /*CH_HTTP_SESSION_BUF_H*/

/*
 * =====================================================================================
 *
 *       Filename:  ch_string.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时23分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_STRING_H
#define CH_STRING_H

#include <stdint.h>
#include <apr_pools.h>
#include "ch_list.h"


typedef struct {
    size_t len;
    unsigned char *data;
}ch_str_t;

#define ch_string(str) { sizeof(str) - 1, str}


extern unsigned char *ch_strcasestrn(unsigned char *s1, const char *s2, size_t n);

extern void ch_encode_base64(ch_str_t *dst, ch_str_t *src);

extern int ch_decode_base64(ch_str_t *dst, ch_str_t *src);

extern int ch_strncasecmp(char *s1, char *s2, size_t n);

extern char *ch_strlcasestrn(char *s1, char *last, char *s2, size_t n);

extern  char * ch_read_rr_name(apr_pool_t *mp,const unsigned char * packet, uint32_t * packet_p, 
                    uint32_t id_pos, uint32_t len);

extern char * ch_escape_data(apr_pool_t *mp,const unsigned char * packet, uint32_t start, uint32_t end);

extern char * ch_encode_base64_packet(apr_pool_t *mp,const unsigned char * data, uint32_t pos, uint16_t length);

#endif /*CH_STRING_H*/


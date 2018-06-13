/*
 *
 *      Filename: ch_util.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:36:34
 * Last Modified: 2017-01-18 10:36:34
 */

#ifndef CH_UTIL_H
#define CH_UTIL_H

#define UNICODE_ERROR_CHARACTERS_MISSING    -1
#define UNICODE_ERROR_INVALID_ENCODING      -2
#define UNICODE_ERROR_OVERLONG_CHARACTER    -3
#define UNICODE_ERROR_RESTRICTED_CHARACTER  -4
#define UNICODE_ERROR_DECODING_ERROR        -5

#include "ch_mpool.h"
#include "ch_tables.h"

#define CREATEMODE_UNISTD ( S_IRUSR | S_IWUSR | S_IRGRP )
#define CREATEMODE ( CH_UREAD | CH_UWRITE | CH_GREAD )



extern char  *ch_utf8_unicode_inplace_ex(ch_pool_t *mp, unsigned char *input, long int input_len, int *changed);

extern int  ch_normalize_path_inplace(unsigned char *input, int len, int win, int *changed);

extern int  ch_parse_boolean(const char *input);

extern char  *ch_remove_quotes(ch_pool_t *mptmp, const char *input, int input_len);


extern char  *ch_remove_escape(ch_pool_t *mptmp, const char *input, int input_len);

extern int  ch_parse_name_eq_value(ch_pool_t *mp, const char *input, char **name, char **value);

extern char  *ch_url_encode(ch_pool_t *mp, char *input, unsigned int input_len, int *changed);

extern char  *ch_file_dirname(ch_pool_t *p, const char *filename);

extern char  *ch_file_basename(ch_pool_t *p, const char *filename);

extern int  ch_sql_hex2bytes_inplace(unsigned char *data, int len);

extern int  ch_hex2bytes_inplace(unsigned char *data, int len);

extern char  *ch_bytes2hex(ch_pool_t *pool, unsigned char *data, int len);

extern char  *ch_bytes2hex_malloc(unsigned char *data, int len);

extern int  ch_is_token_char(unsigned char c);

extern int  ch_remove_lf_crlf_inplace(char *text);

extern char  *ch_guess_tmp_dir(ch_pool_t *p);

extern char  *ch_current_logtime(ch_pool_t *mp);

extern char  *ch_current_logtime_with_buf(char *buf,size_t buf_size);

extern char  *ch_current_filetime(ch_pool_t *mp);

extern unsigned long ch_get_current_timems(void);

extern int  ch_mkstemp_ex(char *templat, int mode);

extern int  ch_mkstemp(char *templat);

extern char  *ch_strtolower_inplace(unsigned char *str);

extern char  *ch_log_escape_re(ch_pool_t *p, const char *text);

extern char  *ch_log_escape(ch_pool_t *p, const char *text);

extern char  *ch_log_escape_nq(ch_pool_t *p, const char *text);

extern char  *ch_log_escape_ex(ch_pool_t *p, const char *text, unsigned long int text_length);

extern char  *ch_log_escape_nq_ex(ch_pool_t *p, const char *text, unsigned long int text_length);

extern char  *ch_log_escape_hex(ch_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern char  *ch_log_escape_raw(ch_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern char  *ch_log_escape_nul(ch_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern int  ch_decode_base64_ext(char *plain_text, const unsigned char *input, int input_len);

extern unsigned char* ch_encode_base64(ch_pool_t *mp,const unsigned char *input,int input_len);

extern unsigned char* ch_encode_base64_malloc(const unsigned char *input,int input_len);

extern int  ch_convert_to_int(const char c);

extern int  ch_js_decode_nonstrict_inplace(unsigned char *input, long int input_len);


extern int  ch_urldecode_nonstrict_inplace_ex(unsigned char *input, long int input_length, int *invalid_count, int *changed);

extern int  ch_html_entities_decode_inplace(ch_pool_t *mp, unsigned char *input, int len);

extern int  ch_ansi_c_sequences_decode_inplace(unsigned char *input, int len);


extern char  *ch_resolve_relative_path(ch_pool_t *pool, const char *parent_filename, const char *filename);

extern int  ch_css_decode_inplace(unsigned char *input, long int input_len);

extern unsigned char ch_is_netmask_v4(char *ip_strv4);

extern unsigned char ch_is_netmask_v6(char *ip_strv6);

extern int  ch_headers_to_buffer(const ch_array_header_t *arr, char *buffer, int max_length);

extern int ch_read_line(char *buff, int size, FILE *fp);

extern int ch_dir_make_r(const char *orig_path);

static inline int ch_port_equal(uint16_t src_port,uint16_t dst_port,uint16_t v){

    return src_port == v || dst_port == v;
}

static inline int ch_ports_equal(uint16_t *ports,int n,uint16_t src_port,uint16_t dst_port){

    int i ;

    uint16_t v;

    for(i= 0;i<n;i++){
    
        v = ports[i];
		
		if(v == 0)
			break;

        if(ch_port_equal(src_port,dst_port,v))
            return 1;
        
    }

    return 0;
}
#endif /* CH_UTIL_H */

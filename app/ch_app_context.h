/*
 * =====================================================================================
 *
 *       Filename:  ch_app_context.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月11日 13时21分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_APP_CONTEXT_H
#define CH_APP_CONTEXT_H

typedef struct ch_app_context_t ch_app_context_t;
typedef struct ch_app_t ch_app_t;

#include <apr_tables.h>
#include "ch_context.h"

#define PARSE_RETURN_CONTINUE 0X01
#define PARSE_RETURN_DONE     0X02
#define PARSE_RETURN_DISCARD  0X04
#define PARSE_RETURN_CLOSE    0X08

struct ch_app_context_t {

    ch_context_t *context;
    apr_array_header_t *apps;
};

struct ch_app_t {

	ch_app_context_t *app_context;

    int (*app_recognize_by_port)(ch_app_t *app,uint16_t src_port,uint16_t dst_port,void *priv_data);

    int (*app_recognize_by_content)(ch_app_t *app,void *data,size_t dlen,void *priv_data);

    int (*app_content_process)(ch_app_t *app,void *data,size_t dlen,void *priv_data);

    void (*app_content_flush)(ch_app_t *app, void *priv_data);

    void (*app_content_close)(ch_app_t *app,void *priv_data);

};

extern void ch_app_context_init(ch_app_context_t *app_context,ch_context_t *context);

extern void ch_app_context_register(ch_app_context_t *app_context,ch_app_t *app);

extern ch_app_t * ch_app_context_recognize_by_port(ch_app_context_t *app_context,uint16_t src_port,uint16_t dst_port);

extern ch_app_t*  ch_app_context_recognize_by_content(ch_app_context_t *app_context,void *data,size_t dlen);

extern int ch_app_context_content_process(ch_app_context_t *app_context,ch_app_t *app,void *data,size_t dlen,void *priv_data);

extern void ch_app_context_content_flush(ch_app_context_t *app_context,ch_app_t *app,void *priv_data);

extern void ch_app_context_content_close(ch_app_context_t *app_context,ch_app_t *app,void *priv_data);

#endif /*CH_APP_CONTEXT_H*/

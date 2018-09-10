/*
 *
 *      Filename: ch_http_accept.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-07 17:06:11
 * Last Modified: 2018-09-07 17:49:11
 */

#ifndef CH_HTTP_ACCEPT_H
#define CH_HTTP_ACCEPT_H

#include "ch_wb_list.h"
#include "ch_wb_list_str.h"
#include "ch_mpool.h"
#include "ch_uri.h"
#include "ch_http_ctypes.h"
#include "ch_http_session.h"

#define IS_EMPTY_STR(str) (str == NULL||strlen(str)==0)

static inline int ch_extName_is_accept_from_uri(ch_wb_list_t *wb_list,ch_pool_t *mp,const char *uri){

    const char *ext_name;
    ch_uri_t t,*uri_parser = &t;
    
    if(wb_list->header->is_on == 0||ch_wb_list_empty(wb_list)||IS_EMPTY_STR(uri))
        return 1;

    ch_uri_parse(mp,uri,uri_parser);

    ext_name = (const char*)uri_parser->ext_name;
    if(IS_EMPTY_STR(ext_name))
        return 1;

    return !ch_wb_list_is_match(wb_list,(void*)ext_name);

}

static inline int ch_extName_is_accept_from_ctype(ch_wb_list_t *wb_list,const char *ctype){

    char *extName;
    char *extNames[10];
    int n = 0;
    int i;

    if(wb_list->header->is_on == 0||ch_wb_list_empty(wb_list)||IS_EMPTY_STR(ctype))
        return 1;

    n = ch_http_extNames_get(ctype,extNames,10);
    if(n==0)
        return 1;

    for(i = 0;i<n;i++){
    
        extName = extNames[i];
        if(ch_wb_list_is_match(wb_list,(void*)extName))
            return 0;
    }

    return 1;
}

static inline int ch_host_is_accept(ch_wb_list_t *w_list,ch_wb_list_t *b_list,const char *host){

	int is_white;

    if(IS_EMPTY_STR(host))
        return 1;

	ch_wb_list_t *m_list = ch_wb_list_should_match_list(w_list,b_list,&is_white);

	if(m_list == NULL)
		return 1;

    int is_match = ch_wb_list_is_match(m_list,(void*)host);

	return is_white?is_match:!is_match;

}

static inline int ch_http_session_is_accept(ch_wb_list_t *host_wlist,ch_wb_list_t *host_blist,ch_wb_list_t *ext_blist,
	ch_http_session_t *session){

	if(!ch_extName_is_accept_from_uri(ext_blist,session->mp,(const char*)session->uri))
		return 0;
	
	if(!ch_host_is_accept(host_wlist,host_blist,(const char*)session->host))
		return 0;

	if(session->headers_out.content_type&&!ch_extName_is_accept_from_ctype(ext_blist,(const char*)session->headers_out.content_type->val))
		return 0;

	return 1;
}

#endif /*CH_HTTP_ACCEPT_H*/

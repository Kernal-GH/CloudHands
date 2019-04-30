/*
 * =====================================================================================
 *
 *       Filename:  ch_dns_app_context.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2019 04:40:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_dns_app_context.h"
#include "ch_log.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_string.h"

static const char *cmd_domain_wblist_fpath(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_dns_app_context_t *context = (ch_dns_app_context_t*)_dcfg;

    context->wblist_fpath = p1;

    return NULL;
}

static const char *cmd_redis_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_dns_app_context_t *context = (ch_dns_app_context_t*)_dcfg;

    context->redis_cfname = p1;

    return NULL;
}

static const command_rec dns_context_directives[] = {
    
	CH_INIT_TAKE1(
            "CHUDPDNSAppDomainsWBListFPath",
            cmd_domain_wblist_fpath,
            NULL,
            0,
            "set dns wblist domains file path"
            ),
	
	CH_INIT_TAKE1(
            "CHUDPDNSAppRedisCFname",
            cmd_redis_cfname,
            NULL,
            0,
            "set redis ip wblist config file path"
            ),

};

static inline void dump_dns_context(ch_dns_app_context_t *dns_context){

    fprintf(stdout,"Dump DNS Context-------------------------------------------\n");
    fprintf(stdout,"dns.domains.wblist.fpath:%s\n",dns_context->wblist_fpath);
    fprintf(stdout,"dns.redis.ipWBList.cfpath:%s\n",dns_context->redis_cfname);
}


static int _load_domains(ch_dns_app_context_t *dns_context){

    char buf[64] = {0};

    FILE *fp = fopen(dns_context->wblist_fpath,"r");
    if(fp == NULL){

        ch_log(CH_LOG_ERR,"Cannot open wblist domains file:%s to read!",dns_context->wblist_fpath);
        return -1;
    }

    while(1==ch_read_line(buf,63,fp)){

        *(const char**)ch_array_push(dns_context->wblist_domains) = (const char*)ch_pstrdup(dns_context->mp,buf);
    }

    printf("Load domains number:%d\n",(int)dns_context->wblist_domains->nelts);

    fclose(fp);

    return 0;
}

ch_dns_app_context_t *ch_dns_app_context_create(const char *cfname){

    const char *msg = NULL;

    ch_dns_app_context_t *dns_context = NULL;

    ch_pool_t *mp = ch_pool_create(4096);
    if(mp == NULL){

        ch_log(CH_LOG_ERR,"Cannot create memory pool for dns context!");
        return NULL;
    }

    dns_context = (ch_dns_app_context_t*)ch_pcalloc(mp,sizeof(*dns_context));
    dns_context->mp = mp;

    msg = ch_process_command_config(dns_context_directives,(void*)dns_context,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config dns context error:%s",msg);
        ch_pool_destroy(mp);
        return NULL;
    }

    dump_dns_context(dns_context);

    dns_context->ip_wblist = ch_redis_ip_wblist_create(mp,dns_context->redis_cfname);
    if(dns_context->ip_wblist == NULL){

        ch_log(CH_LOG_ERR,"Connet load redis config for dns context!");
        ch_pool_destroy(mp);
        return NULL; 
    }

    dns_context->wblist_domains = ch_array_make(mp,100,sizeof(const char*));
    if(dns_context->wblist_domains == NULL){

        ch_log(CH_LOG_ERR,"Cannot create array for wblist domains!");
        ch_pool_destroy(mp);
        return NULL;

    }

    if(_load_domains(dns_context)){

        ch_log(CH_LOG_ERR,"Load domains from file:%s failed!",dns_context->wblist_fpath);
        return NULL;
    }

    /*OK! */
    return dns_context;
}

static inline int domain_is_in(ch_dns_app_context_t *dns_context,const char *domain){

    const char **domains,*match;
    size_t i,n;

    domains = (const char**)dns_context->wblist_domains->elts;
    n = dns_context->wblist_domains->nelts;

    for(i = 0;i<n;i++){

        match = domains[i];
        if(ch_string_endsWith(domain,match))
            return 1;
    }

    return 0;
}

int ch_dns_app_context_wblist_domain_is_accept(ch_dns_app_context_t *dns_context,const char *domain){

    if(domain == NULL ||strlen(domain)==0||dns_context->ip_wblist->is_on==0)
        return 1;

    int rc = domain_is_in(dns_context,domain);

    return dns_context->ip_wblist->is_blacklist?!rc:rc;
}


/*
 * =====================================================================================
 *
 *       Filename:  ch_redis_ip_wblist.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2019 01:59:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_redis_ip_wblist.h"
#include "ch_config.h"
#include "ch_log.h"

static const char *cmd_redis_ison(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)_dcfg;

    if(strcasecmp(p1,"on")==0){
        ip_wblist->is_on = 1;
    }else{

        ip_wblist->is_on = 0;
    }

    return NULL;
}

static const char *cmd_redis_is_blacklist(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)_dcfg;

    if(strcasecmp(p1,"true")==0){
        ip_wblist->is_blacklist = 1;
    }else{

        ip_wblist->is_blacklist= 0;
    }

    return NULL;
}

static const char *cmd_redis_host(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)_dcfg;

    ip_wblist->host = p1;

    return NULL;
}

static const char *cmd_redis_passwd(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)_dcfg;

    ip_wblist->passwd = p1;

    return NULL;
}

static const char *cmd_redis_iptab_name(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)_dcfg;

    ip_wblist->iptab_name = p1;

    return NULL;
}


static const char *cmd_redis_port(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)_dcfg;

    ip_wblist->port = (uint16_t)atoi(p1);
    
    if(ip_wblist->port <=0){
    
        ip_wblist->port = 6379;
    }

    return NULL;
}

static const command_rec redis_directives[] = {
    
    CH_INIT_TAKE1(
            "CHRedisIPWBListHost",
            cmd_redis_host,
            NULL,
            0,
            "set redis host"
            ),
    
	CH_INIT_TAKE1(
            "CHRedisIPWBListPort",
            cmd_redis_port,
            NULL,
            0,
            "set redis port"
            ),
	
	CH_INIT_TAKE1(
            "CHRedisIPWBListPasswd",
            cmd_redis_passwd,
            NULL,
            0,
            "set redis passwd"
            ),

	CH_INIT_TAKE1(
            "CHRedisIPWBListIPTabName",
            cmd_redis_iptab_name,
            NULL,
            0,
            "set redis ip wblist table name"
            ),
	
	CH_INIT_TAKE1(
            "CHRedisIPWBListIsON",
            cmd_redis_ison,
            NULL,
            0,
            "set redis ip wblist is_on(on/off)"
            ),

    CH_INIT_TAKE1(
            "CHRedisIPWBListIsBlacklist",
            cmd_redis_is_blacklist,
            NULL,
            0,
            "set redis ip wblist is black list(true/false)"
            ),
};

static inline void dump_redis_ipwblist(ch_redis_ip_wblist_t *ip_wblist){

    fprintf(stdout,"Dump redis ip wblist info-------------------------------------------\n");
    fprintf(stdout,"ipwblist.redis.host:%s\n",ip_wblist->host);
    fprintf(stdout,"ipwblist.redis.port:%d\n",(int)ip_wblist->port);
	fprintf(stdout,"ipwblist.redis.passwd:%s\n",ip_wblist->passwd);

    fprintf(stdout,"ipwblist.redis.iptabName:%s\n",ip_wblist->iptab_name);
	
	fprintf(stdout,"ipwblist.redis.isON:%s\n",ip_wblist->is_on?"on":"off");
    fprintf(stdout,"ipwblist.redis.isBlackList:%s\n",ip_wblist->is_blacklist?"true":"false");

}

ch_redis_ip_wblist_t * ch_redis_ip_wblist_create(ch_pool_t *mp,const char *cfname){


    ch_redis_ip_wblist_t *ip_wblist = (ch_redis_ip_wblist_t*)ch_pcalloc(mp,sizeof(*ip_wblist));

    const char * msg = NULL;

	ip_wblist->mp = mp;

    msg = ch_process_command_config(redis_directives,(void*)ip_wblist,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config redis ip wblist error:%s",msg);
        return NULL;
    }

    dump_redis_ipwblist(ip_wblist);

    if(ip_wblist->is_on){

        ip_wblist->redis = (ch_redis_t*)ch_pcalloc(mp,sizeof(ch_redis_t));
        if(ip_wblist->redis == NULL)
            return NULL;

        if(ch_redis_init(ip_wblist->redis,ip_wblist->host,ip_wblist->port,ip_wblist->passwd)){
            ch_log(CH_LOG_ERR," Cannot connect redis:%s,%d",ip_wblist->host,(int)ip_wblist->port);
            return NULL;
        }

    }

    return ip_wblist;
}


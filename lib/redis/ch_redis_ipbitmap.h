/*
 * =====================================================================================
 *
 *       Filename:  ch_redis_ipbitmap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2019 03:23:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_REDIS_IPBITMAP_H
#define CH_REDIS_IPBITMAP_H

#include "ch_redis.h"

static inline int ch_redis_ipbitmap_init(ch_redis_t *redis,const char *iptab_name){

    char cmd[128] = {0};
    int rc;

    snprintf(cmd,128,"setbit %s 4294967295 0",iptab_name);

    redisReply *reply = ch_redis_exe_cmd(redis,cmd);

    rc= reply == NULL?1:0;

    ch_redis_reply_free(reply);

    return rc;
}

static inline void ch_redis_ipbitmap_del(ch_redis_t *redis,const char *iptab_name){

    char cmd[64] = {0};
    snprintf(cmd,64,"del %s",iptab_name);
    ch_redis_exe_cmd(redis,cmd);
}

static inline int ch_redis_ipbitmap_reset(ch_redis_t *redis,const char *iptab_name){

    ch_redis_ipbitmap_del(redis,iptab_name);

    return ch_redis_ipbitmap_init(redis,iptab_name);
}



static inline int ch_redis_ipbitmap_set(ch_redis_t *redis,const char *iptab_name,uint32_t ip){

    char cmd[128] = {0};
    int rc;

    snprintf(cmd,128,"setbit %s %lu 1",iptab_name,(unsigned long)ip);

    redisReply *reply = ch_redis_exe_cmd(redis,cmd);

    rc= reply == NULL?1:0;
    ch_redis_reply_free(reply);

    return rc;

}

static inline int ch_redis_ipbitmap_unset(ch_redis_t *redis,const char *iptab_name,uint32_t ip){

    char cmd[128] = {0};
    int rc;

    snprintf(cmd,128,"setbit %s %lu 0",iptab_name,(unsigned long)ip);

    redisReply *reply = ch_redis_exe_cmd(redis,cmd);

    rc= reply==NULL?1:0;
    ch_redis_reply_free(reply);

    return rc;

}

static inline int ch_redis_ipbitmap_is_in(ch_redis_t *redis,const char *iptab_name,uint32_t ip){
    int rc;
    char cmd[128] = {0};
    snprintf(cmd,128,"getbit %s %lu",iptab_name,(unsigned long)ip);

    redisReply *reply = ch_redis_exe_cmd(redis,cmd);

    rc = (reply==NULL||reply->integer==0)?0:1;

    ch_redis_reply_free(reply);

    return rc;
}

static inline int ch_redis_ipbitmap_srcORdstIP_is_in(ch_redis_t *redis,const char *iptab_name,uint32_t srcIP,uint32_t dstIP){


    return ch_redis_ipbitmap_is_in(redis,iptab_name,srcIP)||ch_redis_ipbitmap_is_in(redis,iptab_name,dstIP);
}

#endif /*CH_REDIS_IPBITMAP_H */


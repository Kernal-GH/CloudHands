/*
 * =====================================================================================
 *
 *       Filename:  ch_redis.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2019 02:40:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_redis.h"
#include <string.h>
#define STR_IS_EMPTY(str) ((str)==NULL||strlen(str)==0)

int ch_redis_init(ch_redis_t *redis,const char *host,uint16_t port,const char *passwd){

    redis->host = STR_IS_EMPTY(host)?"127.0.0.1":host;
    redis->port = port == 0?6379:port;
    redis->passwd = passwd;

    redis->context = redisConnect(redis->host,redis->port);
    if(redis->context == NULL){

        printf("Cannot connect redis:%s,%d\n",redis->host,redis->port);

        return -1;
    }

    redisEnableKeepAlive(redis->context);

    if(ch_redis_auth(redis))
    {
        printf("redis auth failed!\n");
        return -1;
    }

    return 0;
}

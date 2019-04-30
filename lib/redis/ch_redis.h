/*
 * =====================================================================================
 *
 *       Filename:  ch_redis.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2019 02:22:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_REDIS_H
#define CH_REDIS_H

typedef struct ch_redis_t ch_redis_t;

#include <hiredis/hiredis.h>
#include <string.h>

struct ch_redis_t {

    redisContext *context;

    const char *host;
    uint16_t port;

    const char *passwd;
};


extern int ch_redis_init(ch_redis_t *redis,const char *host,uint16_t port,const char *passwd);

static inline redisReply *ch_redis_exe_cmd(ch_redis_t *redis,const char *cmd) {

    return redisCommand(redis->context,cmd);

}

static inline void  ch_redis_free(ch_redis_t *redis) {
    redisFree(redis->context);
}

static inline void ch_redis_reply_free(redisReply *rep) {
    freeReplyObject(rep);

}

static inline int ch_redis_auth(ch_redis_t *redis){

    char cmd[64] = {0};
    snprintf(cmd,64,"AUTH %s",redis->passwd);

    redisReply *reply = ch_redis_exe_cmd(redis,cmd);

    if(reply == NULL||strncmp(reply->str,"OK",2)!=0)
        return -1;

    return 0;
}


#endif /* CH_REDIS_H */

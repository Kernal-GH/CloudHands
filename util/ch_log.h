/*
 * =====================================================================================
 *
 *       Filename:  ch_log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 15时55分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_LOG_H
#define CH_LOG_H

#include <unistd.h>
#include <apr_pools.h>
#include <apr_file_io.h>
#include "ch_errno.h"

#define CH_LOG_STDERR            0
#define CH_LOG_EMERG             1
#define CH_LOG_ALERT             2
#define CH_LOG_CRIT              3
#define CH_LOG_ERR               4
#define CH_LOG_WARN              5
#define CH_LOG_NOTICE            6
#define CH_LOG_INFO              7
#define CH_LOG_DEBUG             8

typedef struct {
    int       log_level;
    apr_file_t     *file;
    apr_pool_t     *mp;
}ch_log_t;


#define CH_MAX_ERROR_STR   2048

#define ch_log(level,fmt,...) ch_log_error_core(level,0,fmt,##__VA_ARGS__)

#define CH_DEBUG_LOG(fmt, ...) ch_log_error_core(CH_LOG_DEBUG,0,fmt,##__VA_ARGS__)

#define CH_INFO_LOG(fmt, ...) ch_log_error_core(CH_LOG_INFO,0,fmt,##__VA_ARGS__)

#define CH_NOTICH_LOG(fmt, ...) ch_log_error_core(CH_LOG_NOTICH,0,fmt,##__VA_ARGS__)

#define CH_WARN_LOG(fmt, ...) ch_log_error_core(CH_LOG_WARN,0,fmt,##__VA_ARGS__)

#define CH_ERR_LOG(fmt, ...) ch_log_error_core(CH_LOG_ERR,0,fmt,##__VA_ARGS__)

#define CH_CRIT_LOG(fmt, ...) ch_log_error_core(CH_LOG_CRIT,0,fmt,##__VA_ARGS__)

#define CH_ALERT_LOG(fmt, ...) ch_log_error_core(CH_LOG_ALERT,0,fmt,##__VA_ARGS__)

#define CH_EMERG_LOG(fmt, ...) ch_log_error_core(CH_LOG_EMERG,0,fmt,##__VA_ARGS__)

void ch_log_error_core(int level,ch_err_t err,
    const char *fmt, ...);


void ch_log_init(apr_pool_t *mp,const char *name,int level);

static inline void
ch_write_stderr(char *text)
{
    write(STDERR_FILENO, text, strlen(text));
}




#endif /*CH_LOG_H*/

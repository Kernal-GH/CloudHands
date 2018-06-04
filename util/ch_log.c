/*
 * =====================================================================================
 *
 *       Filename:  ch_log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 15时57分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <apr_strings.h>
#include "ch_constants.h"
#include "ch_util.h"
#include "ch_log.h"

static ch_log_t log_s,*log_ptr=&log_s;

static const char* err_levels[] = {
    "stderr",
    "emerg",
    "alert",
    "crit",
    "error",
    "warn",
    "notich",
    "info",
    "debug"
};


void
ch_log_error_core(int level,ch_err_t err,
    const char *fmt, ...)

{
    va_list  args;
    char   errstr1[CH_MAX_ERROR_STR];
    char   errstr2[CH_MAX_ERROR_STR];
    char   tstr[100]={0};
    size_t nbytes_written,nbytes;

    if(log_ptr->log_level<level||log_ptr->file == NULL) {
        return;
    }

    va_start(args, fmt);
    apr_vsnprintf(errstr1,sizeof(errstr1), fmt, args);
    va_end(args);

   
    apr_snprintf(errstr2,sizeof(errstr2),"[%s][%d][%s][%s] %s\n",
            ch_current_logtime_with_buf(tstr,100),
            getpid(),
            err_levels[level],
            err?ch_strerror(err):"--",
            errstr1
            );

    nbytes = strlen(errstr2);
    apr_file_write_full(log_ptr->file,errstr2,nbytes,&nbytes_written);
}

void
ch_log_init(apr_pool_t *mp,const char *name,int level)
{

    apr_status_t rc;

    if(level<CH_LOG_STDERR||level>CH_LOG_DEBUG)
        level = CH_LOG_NOTICE;

    log_ptr->log_level = level;
    log_ptr->mp = mp;

    rc = apr_file_open(&log_ptr->file,name,
                   APR_WRITE | APR_APPEND | APR_CREATE | APR_BINARY,
                   CREATEMODE, mp);

    if (rc != APR_SUCCESS) {
        printf("Failed to open  log file: %s\n",
            name);
        exit(-1);
    }
}


/*
 *
 *      Filename: ch_log.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:19:29
 * Last Modified: 2017-01-18 11:02:24
 */

#include "ch_log.h"
#include "ch_util.h"

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
ch_log_error_core(int level,int err,
    const char *fmt, ...)

{
    va_list  args;
    char   errstr1[CH_MAX_ERROR_STR]={0};
    char   errstr2[CH_MAX_ERROR_STR]={0};
	char   errstr3[CH_MAX_ERROR_STR]={0};

    char   tstr[100]={0};
    size_t nbytes_written,nbytes;

    if(log_ptr->log_level<level||log_ptr->file == NULL) {
        return;
    }

    va_start(args, fmt);
    ch_vsnprintf(errstr1,sizeof(errstr1), fmt, args);
    va_end(args);


    ch_snprintf(errstr2,sizeof(errstr2),"[%s][%d][%s][%s] %s\n",
            ch_current_logtime_with_buf(tstr,100),
            getpid(),
            err_levels[level],
            err?ch_strerror(err,errstr3,CH_MAX_ERROR_STR):"--",
            errstr1
            );

    nbytes = strlen(errstr2);
    ch_file_write_full(log_ptr->file,errstr2,nbytes,&nbytes_written);
}

void
ch_log_init(ch_pool_t *mp,const char *name,int level)
{

    int rc;

    if(level<CH_LOG_STDERR||level>CH_LOG_DEBUG)
        level = CH_LOG_NOTICE;

    log_ptr->log_level = level;
    log_ptr->mp = mp;

    rc = ch_file_open(&log_ptr->file,name,
                   CH_WRITE | CH_APPEND | CH_CREATE | CH_BINARY,
                   CREATEMODE, mp);

    if (rc != 0) {
        printf("Failed to open  log file: %s\n",
            name);
        exit(-1);
    }
}




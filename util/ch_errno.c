/*
 * =====================================================================================
 *
 *       Filename:  ch_errno.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时22分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include "ch_string.h"
#include "ch_errno.h"
#include "ch_constants.h"


static ch_str_t  *ch_sys_errlist;
static ch_str_t   ch_unknown_error = ch_string("Unknown error");


char *ch_strerror(ch_err_t err)
{
    ch_str_t  *chg;

    chg = ((int) err < CH_SYS_NERR) ?\
    &ch_sys_errlist[err]:&ch_unknown_error;


    return chg->data;
}


int ch_strerror_init(void)
{
    char       *chg;
    char     *p;
    size_t      len;
    ch_err_t   err;

    /*
     * ch_strerror() is not ready to work at this stage, therefore,
     * malloc() is used and possible errors are logged using strerror().
     */

    len = CH_SYS_NERR * sizeof(ch_str_t);

    ch_sys_errlist =(ch_str_t*)malloc(len);
    if (ch_sys_errlist == NULL) {
        goto failed;
    }

    for (err = 0; err < CH_SYS_NERR; err++) {
        chg = strerror(err);
        len = strlen(chg);

        p =(char*)malloc(len);
        if (p == NULL) {
            goto failed;
        }

        memcpy(p, chg, len);
        ch_sys_errlist[err].len = len;
        ch_sys_errlist[err].data = p;
    }

    return CH_OK;

failed:

    err = errno;

    return CH_ERROR;
}


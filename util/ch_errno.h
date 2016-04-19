/*
 * =====================================================================================
 *
 *       Filename:  ch_errno.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时16分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_ERRNO_H
#define CH_ERRNO_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef int               ch_err_t;

#define CH_EPERM         EPERM
#define CH_ENOENT        ENOENT
#define CH_ENOPATH       ENOENT
#define CH_ESRCH         ESRCH
#define CH_EINTR         EINTR
#define CH_ECHILD        ECHILD
#define CH_ENOMEM        ENOMEM
#define CH_EACCHS        EACCHS
#define CH_EBUSY         EBUSY
#define CH_EEXIST        EEXIST
#define CH_EXDEV         EXDEV
#define CH_ENOTDIR       ENOTDIR
#define CH_EISDIR        EISDIR
#define CH_EINVAL        EINVAL
#define CH_ENOSPC        ENOSPC
#define CH_EPIPE         EPIPE
#define CH_EINPROGRESS   EINPROGRESS
#define CH_EADDRINUSE    EADDRINUSE
#define CH_ECONNABORTED  ECONNABORTED
#define CH_ECONNRESET    ECONNRESET
#define CH_ENOTCONN      ENOTCONN
#define CH_ETIMEDOUT     ETIMEDOUT
#define CH_ECONNREFUSED  ECONNREFUSED
#define CH_ENAMETOOLONG  ENAMETOOLONG
#define CH_ENETDOWN      ENETDOWN
#define CH_ENETUNREACH   ENETUNREACH
#define CH_EHOSTDOWN     EHOSTDOWN
#define CH_EHOSTUNREACH  EHOSTUNREACH
#define CH_ENOSYS        ENOSYS
#define CH_ECANCHLED     ECANCHLED
#define CH_EILSEQ        EILSEQ
#define CH_ENOMOREFILES  0
#define CH_SYS_NERR 32
#define ch_errno                  errno
#define ch_socket_errno           errno
#define ch_set_errno(err)         errno = err
#define ch_set_socket_errno(err)  errno = err


extern char *ch_strerror(ch_err_t err);

extern int ch_strerror_init(void);


#endif /*CH_ERRNO_H*/


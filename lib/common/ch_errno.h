/*
 *
 *      Filename: ch_errno.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:08:19
 * Last Modified: 2017-01-11 10:08:19
 */

#ifndef CH_ERRNO_H
#define CH_ERRNO_H

#include <errno.h>
#include "ch_constants.h"

extern char * ch_strerror(int statcode, char *buf,
                                 size_t bufsize);

/**
 * CH_OS_START_ERROR is where the GW specific error values start.
 */
#define CH_OS_START_ERROR     20000
/**
 * CH_OS_ERRSPACE_SIZE is the maximum number of errors you can fit
 *    into one of the error/status ranges below -- except for
 *    CH_OS_START_USERERR, which see.
 */
#define CH_OS_ERRSPACE_SIZE 50000
/**
 * CH_UTIL_ERRSPACE_SIZE is the size of the space that is reserved for
 * use within gw-util. This space is reserved above that used by GW
 * internally.
 * @note This number MUST be smaller than CH_OS_ERRSPACE_SIZE by a
 *       large enough amount that GW has sufficient room for its
 *       codes.
 */
#define CH_UTIL_ERRSPACE_SIZE 20000
/**
 * CH_OS_START_STATUS is where the GW specific status codes start.
 */
#define CH_OS_START_STATUS    (CH_OS_START_ERROR + CH_OS_ERRSPACE_SIZE)
/**
 * CH_UTIL_START_STATUS is where GW-Util starts defining its
 * status codes.
 */
#define CH_UTIL_START_STATUS   (CH_OS_START_STATUS + \
                           (CH_OS_ERRSPACE_SIZE - CH_UTIL_ERRSPACE_SIZE))
/**
 * CH_OS_START_USERERR are reserved for applications that use GW that
 *     layer their own error codes along with GW's.  Note that the
 *     error immediately following this one is set ten times farther
 *     away than usual, so that users of gw have a lot of room in
 *     which to declare custom error codes.
 *
 * In general applications should try and create unique error codes. To try
 * and assist in finding suitable ranges of numbers to use, the following
 * ranges are known to be used by the listed applications. If your
 * application defines error codes please advise the range of numbers it
 * uses to dev@gw.apache.org for inclusion in this list.
 *
 * Ranges shown are in relation to CH_OS_START_USERERR
 *
 * Subversion - Defined ranges, of less than 100, at intervals of 5000
 *              starting at an offset of 5000, e.g.
 *               +5000 to 5100,  +10000 to 10100
 *
 * Apache HTTPD - +2000 to 2999
 */
#define CH_OS_START_USERERR    (CH_OS_START_STATUS + CH_OS_ERRSPACE_SIZE)
/**
 * CH_OS_START_USEERR is obsolete, defined for compatibility only.
 * Use CH_OS_START_USERERR instead.
 */
#define CH_OS_START_USEERR     CH_OS_START_USERERR
/**
 * CH_OS_START_CANONERR is where GW versions of errno values are defined
 *     on systems which don't have the corresponding errno.
 */
#define CH_OS_START_CANONERR  (CH_OS_START_USERERR \
                                 + (CH_OS_ERRSPACE_SIZE * 10))
/**
 * CH_OS_START_EAIERR folds EAI_ error codes from getaddrinfo() into
 *     ch_status_t values.
 */
#define CH_OS_START_EAIERR    (CH_OS_START_CANONERR + CH_OS_ERRSPACE_SIZE)
/**
 * CH_OS_START_SYSERR folds platform-specific system error values into
 *     ch_status_t values.
 */
#define CH_OS_START_SYSERR    (CH_OS_START_EAIERR + CH_OS_ERRSPACE_SIZE)

/**
 * @defgroup CH_ERROR_map GW Error Space
 * <PRE>
 * The following attempts to show the relation of the various constants
 * used for mapping GW Status codes.
 *
 *       0
 *
 *  20,000     CH_OS_START_ERROR
 *
 *         + CH_OS_ERRSPACE_SIZE (50,000)
 *
 *  70,000      CH_OS_START_STATUS
 *
 *         + CH_OS_ERRSPACE_SIZE - CH_UTIL_ERRSPACE_SIZE (30,000)
 *
 * 100,000      CH_UTIL_START_STATUS
 *
 *         + CH_UTIL_ERRSPACE_SIZE (20,000)
 *
 * 120,000      CH_OS_START_USERERR
 *
 *         + 10 x CH_OS_ERRSPACE_SIZE (50,000 * 10)
 *
 * 620,000      CH_OS_START_CANONERR
 *
 *         + CH_OS_ERRSPACE_SIZE (50,000)
 *
 * 670,000      CH_OS_START_EAIERR
 *
 *         + CH_OS_ERRSPACE_SIZE (50,000)
 *
 * 720,000      CH_OS_START_SYSERR
 *
 * </PRE>
 */

/** no error. */
#define CH_OK 0

/**
 * @defgroup CH_Error GW Error Values
 * <PRE>
 * <b>GW ERROR VALUES</b>
 * CH_ENOSTAT      GW was unable to perform a stat on the file
 * CH_ENOPOOL      GW was not provided a pool with which to allocate memory
 * CH_EBADDATE     GW was given an invalid date
 * CH_EINVALSOCK   GW was given an invalid socket
 * CH_ENOPROC      GW was not given a process structure
 * CH_ENOTIME      GW was not given a time structure
 * CH_ENODIR       GW was not given a directory structure
 * CH_ENOLOCK      GW was not given a lock structure
 * CH_ENOPOLL      GW was not given a poll structure
 * CH_ENOSOCKET    GW was not given a socket
 * CH_ENOTHREAD    GW was not given a thread structure
 * CH_ENOTHDKEY    GW was not given a thread key structure
 * CH_ENOSHMAVAIL  There is no more shared memory available
 * CH_EDSOOPEN     GW was unable to open the dso object.  For more
 *                  information call ch_dso_error().
 * CH_EGENERAL     General failure (specific information not available)
 * CH_EBADIP       The specified IP address is invalid
 * CH_EBADMASK     The specified netmask is invalid
 * CH_ESYMNOTFOUND Could not find the requested symbol
 * CH_ENOTENOUGHENTROPY Not enough entropy to continue
 * </PRE>
 *
 * <PRE>
 * <b>GW STATUS VALUES</b>
 * CH_INCHILD        Program is currently executing in the child
 * CH_INPARENT       Program is currently executing in the parent
 * CH_DETACH         The thread is detached
 * CH_NOTDETACH      The thread is not detached
 * CH_CHILD_DONE     The child has finished executing
 * CH_CHILD_NOTDONE  The child has not finished executing
 * CH_TIMEUP         The operation did not finish before the timeout
 * CH_INCOMPLETE     The operation was incomplete although some processing
 *                    was performed and the results are partially valid
 * CH_BADCH          Getopt found an option not in the option string
 * CH_BADARG         Getopt found an option that is missing an argument
 *                    and an argument was specified in the option string
 * CH_EOF            GW has encountered the end of the file
 * CH_NOTFOUND       GW was unable to find the socket in the poll structure
 * CH_ANONYMOUS      GW is using anonymous shared memory
 * CH_FILEBASED      GW is using a file name as the key to the shared memory
 * CH_KEYBASED       GW is using a shared key as the key to the shared memory
 * CH_EINIT          Ininitalizer value.  If no option has been found, but
 *                    the status variable requires a value, this should be used
 * CH_ENOTIMPL       The GW function has not been implemented on this
 *                    platform, either because nobody has gotten to it yet,
 *                    or the function is impossible on this platform.
 * CH_EMISMATCH      Two passwords do not match.
 * CH_EABSOLUTE      The given path was absolute.
 * CH_ERELATIVE      The given path was relative.
 * CH_EINCOMPLETE    The given path was neither relative nor absolute.
 * CH_EABOVEROOT     The given path was above the root path.
 * CH_EBUSY          The given lock was busy.
 * CH_EPROC_UNKNOWN  The given process wasn't recognized by GW
 * </PRE>
 * @{
 */
/** @see CH_STATUS_IS_ENOSTAT */
#define CH_ENOSTAT        (CH_OS_START_ERROR + 1)
/** @see CH_STATUS_IS_ENOPOOL */
#define CH_ENOPOOL        (CH_OS_START_ERROR + 2)
/* empty slot: +3 */
/** @see CH_STATUS_IS_EBADDATE */
#define CH_EBADDATE       (CH_OS_START_ERROR + 4)
/** @see CH_STATUS_IS_EINVALSOCK */
#define CH_EINVALSOCK     (CH_OS_START_ERROR + 5)
/** @see CH_STATUS_IS_ENOPROC */
#define CH_ENOPROC        (CH_OS_START_ERROR + 6)
/** @see CH_STATUS_IS_ENOTIME */
#define CH_ENOTIME        (CH_OS_START_ERROR + 7)
/** @see CH_STATUS_IS_ENODIR */
#define CH_ENODIR         (CH_OS_START_ERROR + 8)
/** @see CH_STATUS_IS_ENOLOCK */
#define CH_ENOLOCK        (CH_OS_START_ERROR + 9)
/** @see CH_STATUS_IS_ENOPOLL */
#define CH_ENOPOLL        (CH_OS_START_ERROR + 10)
/** @see CH_STATUS_IS_ENOSOCKET */
#define CH_ENOSOCKET      (CH_OS_START_ERROR + 11)
/** @see CH_STATUS_IS_ENOTHREAD */
#define CH_ENOTHREAD      (CH_OS_START_ERROR + 12)
/** @see CH_STATUS_IS_ENOTHDKEY */
#define CH_ENOTHDKEY      (CH_OS_START_ERROR + 13)
/** @see CH_STATUS_IS_EGENERAL */
#define CH_EGENERAL       (CH_OS_START_ERROR + 14)
/** @see CH_STATUS_IS_ENOSHMAVAIL */
#define CH_ENOSHMAVAIL    (CH_OS_START_ERROR + 15)
/** @see CH_STATUS_IS_EBADIP */
#define CH_EBADIP         (CH_OS_START_ERROR + 16)
/** @see CH_STATUS_IS_EBADMASK */
#define CH_EBADMASK       (CH_OS_START_ERROR + 17)
/* empty slot: +18 */
/** @see CH_STATUS_IS_EDSOPEN */
#define CH_EDSOOPEN       (CH_OS_START_ERROR + 19)
/** @see CH_STATUS_IS_EABSOLUTE */
#define CH_EABSOLUTE      (CH_OS_START_ERROR + 20)
/** @see CH_STATUS_IS_ERELATIVE */
#define CH_ERELATIVE      (CH_OS_START_ERROR + 21)
/** @see CH_STATUS_IS_EINCOMPLETE */
#define CH_EINCOMPLETE    (CH_OS_START_ERROR + 22)
/** @see CH_STATUS_IS_EABOVEROOT */
#define CH_EABOVEROOT     (CH_OS_START_ERROR + 23)
/** @see CH_STATUS_IS_EBADPATH */
#define CH_EBADPATH       (CH_OS_START_ERROR + 24)
/** @see CH_STATUS_IS_EPATHWILD */
#define CH_EPATHWILD      (CH_OS_START_ERROR + 25)
/** @see CH_STATUS_IS_ESYMNOTFOUND */
#define CH_ESYMNOTFOUND   (CH_OS_START_ERROR + 26)
/** @see CH_STATUS_IS_EPROC_UNKNOWN */
#define CH_EPROC_UNKNOWN  (CH_OS_START_ERROR + 27)
/** @see CH_STATUS_IS_ENOTENOUGHENTROPY */
#define CH_ENOTENOUGHENTROPY (CH_OS_START_ERROR + 28)
/** @} */

/**
 * @defgroup CH_STATUS_IS Status Value Tests
 * @warning For any particular error condition, more than one of these tests
 *      may match. This is because platform-specific error codes may not
 *      always match the semantics of the POSIX codes these tests (and the
 *      corresponding GW error codes) are named after. A notable example
 *      are the CH_STATUS_IS_ENOENT and CH_STATUS_IS_ENOTDIR tests on
 *      Win32 platforms. The programmer should always be aware of this and
 *      adjust the order of the tests accordingly.
 * @{
 */
/**
 * GW was unable to perform a stat on the file
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_ENOSTAT(s)        ((s) == CH_ENOSTAT)
/**
 * GW was not provided a pool with which to allocate memory
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_ENOPOOL(s)        ((s) == CH_ENOPOOL)
/** GW was given an invalid date  */
#define CH_STATUS_IS_EBADDATE(s)       ((s) == CH_EBADDATE)
/** GW was given an invalid socket */
#define CH_STATUS_IS_EINVALSOCK(s)     ((s) == CH_EINVALSOCK)
/** GW was not given a process structure */
#define CH_STATUS_IS_ENOPROC(s)        ((s) == CH_ENOPROC)
/** GW was not given a time structure */
#define CH_STATUS_IS_ENOTIME(s)        ((s) == CH_ENOTIME)
/** GW was not given a directory structure */
#define CH_STATUS_IS_ENODIR(s)         ((s) == CH_ENODIR)
/** GW was not given a lock structure */
#define CH_STATUS_IS_ENOLOCK(s)        ((s) == CH_ENOLOCK)
/** GW was not given a poll structure */
#define CH_STATUS_IS_ENOPOLL(s)        ((s) == CH_ENOPOLL)
/** GW was not given a socket */
#define CH_STATUS_IS_ENOSOCKET(s)      ((s) == CH_ENOSOCKET)
/** GW was not given a thread structure */
#define CH_STATUS_IS_ENOTHREAD(s)      ((s) == CH_ENOTHREAD)
/** GW was not given a thread key structure */
#define CH_STATUS_IS_ENOTHDKEY(s)      ((s) == CH_ENOTHDKEY)
/** Generic Error which can not be put into another spot */
#define CH_STATUS_IS_EGENERAL(s)       ((s) == CH_EGENERAL)
/** There is no more shared memory available */
#define CH_STATUS_IS_ENOSHMAVAIL(s)    ((s) == CH_ENOSHMAVAIL)
/** The specified IP address is invalid */
#define CH_STATUS_IS_EBADIP(s)         ((s) == CH_EBADIP)
/** The specified netmask is invalid */
#define CH_STATUS_IS_EBADMASK(s)       ((s) == CH_EBADMASK)
/* empty slot: +18 */
/**
 * GW was unable to open the dso object.
 * For more information call ch_dso_error().
 */
#define CH_STATUS_IS_EDSOOPEN(s)       ((s) == CH_EDSOOPEN)
/** The given path was absolute. */
#define CH_STATUS_IS_EABSOLUTE(s)      ((s) == CH_EABSOLUTE)
/** The given path was relative. */
#define CH_STATUS_IS_ERELATIVE(s)      ((s) == CH_ERELATIVE)
/** The given path was neither relative nor absolute. */
#define CH_STATUS_IS_EINCOMPLETE(s)    ((s) == CH_EINCOMPLETE)
/** The given path was above the root path. */
#define CH_STATUS_IS_EABOVEROOT(s)     ((s) == CH_EABOVEROOT)
/** The given path was bad. */
#define CH_STATUS_IS_EBADPATH(s)       ((s) == CH_EBADPATH)
/** The given path contained wildcards. */
#define CH_STATUS_IS_EPATHWILD(s)      ((s) == CH_EPATHWILD)
/** Could not find the requested symbol.
 * For more information call ch_dso_error().
 */
#define CH_STATUS_IS_ESYMNOTFOUND(s)   ((s) == CH_ESYMNOTFOUND)
/** The given process was not recognized by GW. */
#define CH_STATUS_IS_EPROC_UNKNOWN(s)  ((s) == CH_EPROC_UNKNOWN)
/** GW could not gather enough entropy to continue. */
#define CH_STATUS_IS_ENOTENOUGHENTROPY(s) ((s) == CH_ENOTENOUGHENTROPY)

/** @} */

/**
 * @addtogroup CH_Error
 * @{
 */
/** @see CH_STATUS_IS_INCHILD */
#define CH_INCHILD        (CH_OS_START_STATUS + 1)
/** @see CH_STATUS_IS_INPARENT */
#define CH_INPARENT       (CH_OS_START_STATUS + 2)
/** @see CH_STATUS_IS_DETACH */
#define CH_DETACH         (CH_OS_START_STATUS + 3)
/** @see CH_STATUS_IS_NOTDETACH */
#define CH_NOTDETACH      (CH_OS_START_STATUS + 4)
/** @see CH_STATUS_IS_CHILD_DONE */
#define CH_CHILD_DONE     (CH_OS_START_STATUS + 5)
/** @see CH_STATUS_IS_CHILD_NOTDONE */
#define CH_CHILD_NOTDONE  (CH_OS_START_STATUS + 6)
/** @see CH_STATUS_IS_TIMEUP */
#define CH_TIMEUP         (CH_OS_START_STATUS + 7)
/** @see CH_STATUS_IS_INCOMPLETE */
#define CH_INCOMPLETE     (CH_OS_START_STATUS + 8)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/** @see CH_STATUS_IS_BADCH */
#define CH_BADCH          (CH_OS_START_STATUS + 12)
/** @see CH_STATUS_IS_BADARG */
#define CH_BADARG         (CH_OS_START_STATUS + 13)
/** @see CH_STATUS_IS_EOF */
#define CH_EOF            (CH_OS_START_STATUS + 14)
/** @see CH_STATUS_IS_NOTFOUND */
#define CH_NOTFOUND       (CH_OS_START_STATUS + 15)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/** @see CH_STATUS_IS_ANONYMOUS */
#define CH_ANONYMOUS      (CH_OS_START_STATUS + 19)
/** @see CH_STATUS_IS_FILEBASED */
#define CH_FILEBASED      (CH_OS_START_STATUS + 20)
/** @see CH_STATUS_IS_KEYBASED */
#define CH_KEYBASED       (CH_OS_START_STATUS + 21)
/** @see CH_STATUS_IS_EINIT */
#define CH_EINIT          (CH_OS_START_STATUS + 22)
/** @see CH_STATUS_IS_ENOTIMPL */
#define CH_ENOTIMPL       (CH_OS_START_STATUS + 23)
/** @see CH_STATUS_IS_EMISMATCH */
#define CH_EMISMATCH      (CH_OS_START_STATUS + 24)
/** @see CH_STATUS_IS_EBUSY */
#define CH_EBUSY          (CH_OS_START_STATUS + 25)
/** @} */

/**
 * @addtogroup CH_STATUS_IS
 * @{
 */
/**
 * Program is currently executing in the child
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code */
#define CH_STATUS_IS_INCHILD(s)        ((s) == CH_INCHILD)
/**
 * Program is currently executing in the parent
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_INPARENT(s)       ((s) == CH_INPARENT)
/**
 * The thread is detached
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_DETACH(s)         ((s) == CH_DETACH)
/**
 * The thread is not detached
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_NOTDETACH(s)      ((s) == CH_NOTDETACH)
/**
 * The child has finished executing
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_CHILD_DONE(s)     ((s) == CH_CHILD_DONE)
/**
 * The child has not finished executing
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_CHILD_NOTDONE(s)  ((s) == CH_CHILD_NOTDONE)
/**
 * The operation did not finish before the timeout
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_TIMEUP(s)         ((s) == CH_TIMEUP)
/**
 * The operation was incomplete although some processing was performed
 * and the results are partially valid.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_INCOMPLETE(s)     ((s) == CH_INCOMPLETE)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/**
 * Getopt found an option not in the option string
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_BADCH(s)          ((s) == CH_BADCH)
/**
 * Getopt found an option not in the option string and an argument was
 * specified in the option string
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_BADARG(s)         ((s) == CH_BADARG)
/**
 * GW has encountered the end of the file
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_EOF(s)            ((s) == CH_EOF)
/**
 * GW was unable to find the socket in the poll structure
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_NOTFOUND(s)       ((s) == CH_NOTFOUND)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/**
 * GW is using anonymous shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_ANONYMOUS(s)      ((s) == CH_ANONYMOUS)
/**
 * GW is using a file name as the key to the shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_FILEBASED(s)      ((s) == CH_FILEBASED)
/**
 * GW is using a shared key as the key to the shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_KEYBASED(s)       ((s) == CH_KEYBASED)
/**
 * Ininitalizer value.  If no option has been found, but
 * the status variable requires a value, this should be used
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_EINIT(s)          ((s) == CH_EINIT)
/**
 * The GW function has not been implemented on this
 * platform, either because nobody has gotten to it yet,
 * or the function is impossible on this platform.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_ENOTIMPL(s)       ((s) == CH_ENOTIMPL)
/**
 * Two passwords do not match.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_EMISMATCH(s)      ((s) == CH_EMISMATCH)
/**
 * The given lock was busy
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define CH_STATUS_IS_EBUSY(s)          ((s) == CH_EBUSY)

/** @} */

/**
 * @addtogroup CH_Error GW Error Values
 * @{
 */
/* GW CANONICAL ERROR VALUES */
/** @see CH_STATUS_IS_EACCES */
#ifdef EACCES
#define CH_EACCES EACCES
#else
#define CH_EACCES         (CH_OS_START_CANONERR + 1)
#endif

/** @see CH_STATUS_IS_EEXIST */
#ifdef EEXIST
#define CH_EEXIST EEXIST
#else
#define CH_EEXIST         (CH_OS_START_CANONERR + 2)
#endif

/** @see CH_STATUS_IS_ENAMETOOLONG */
#ifdef ENAMETOOLONG
#define CH_ENAMETOOLONG ENAMETOOLONG
#else
#define CH_ENAMETOOLONG   (CH_OS_START_CANONERR + 3)
#endif

/** @see CH_STATUS_IS_ENOENT */
#ifdef ENOENT
#define CH_ENOENT ENOENT
#else
#define CH_ENOENT         (CH_OS_START_CANONERR + 4)
#endif

/** @see CH_STATUS_IS_ENOTDIR */
#ifdef ENOTDIR
#define CH_ENOTDIR ENOTDIR
#else
#define CH_ENOTDIR        (CH_OS_START_CANONERR + 5)
#endif

/** @see CH_STATUS_IS_ENOSPC */
#ifdef ENOSPC
#define CH_ENOSPC ENOSPC
#else
#define CH_ENOSPC         (CH_OS_START_CANONERR + 6)
#endif

/** @see CH_STATUS_IS_ENOMEM */
#ifdef ENOMEM
#define CH_ENOMEM ENOMEM
#else
#define CH_ENOMEM         (CH_OS_START_CANONERR + 7)
#endif

/** @see CH_STATUS_IS_EMFILE */
#ifdef EMFILE
#define CH_EMFILE EMFILE
#else
#define CH_EMFILE         (CH_OS_START_CANONERR + 8)
#endif

/** @see CH_STATUS_IS_ENFILE */
#ifdef ENFILE
#define CH_ENFILE ENFILE
#else
#define CH_ENFILE         (CH_OS_START_CANONERR + 9)
#endif

/** @see CH_STATUS_IS_EBADF */
#ifdef EBADF
#define CH_EBADF EBADF
#else
#define CH_EBADF          (CH_OS_START_CANONERR + 10)
#endif

/** @see CH_STATUS_IS_EINVAL */
#ifdef EINVAL
#define CH_EINVAL EINVAL
#else
#define CH_EINVAL         (CH_OS_START_CANONERR + 11)
#endif

/** @see CH_STATUS_IS_ESPIPE */
#ifdef ESPIPE
#define CH_ESPIPE ESPIPE
#else
#define CH_ESPIPE         (CH_OS_START_CANONERR + 12)
#endif

/**
 * @see CH_STATUS_IS_EAGAIN
 * @warning use CH_STATUS_IS_EAGAIN instead of just testing this value
 */
#ifdef EAGAIN
#define CH_EAGAIN EAGAIN
#elif defined(EWOULDBLOCK)
#define CH_EAGAIN EWOULDBLOCK
#else
#define CH_EAGAIN         (CH_OS_START_CANONERR + 13)
#endif

/** @see CH_STATUS_IS_EINTR */
#ifdef EINTR
#define CH_EINTR EINTR
#else
#define CH_EINTR          (CH_OS_START_CANONERR + 14)
#endif

/** @see CH_STATUS_IS_ENOTSOCK */
#ifdef ENOTSOCK
#define CH_ENOTSOCK ENOTSOCK
#else
#define CH_ENOTSOCK       (CH_OS_START_CANONERR + 15)
#endif

/** @see CH_STATUS_IS_ECONNREFUSED */
#ifdef ECONNREFUSED
#define CH_ECONNREFUSED ECONNREFUSED
#else
#define CH_ECONNREFUSED   (CH_OS_START_CANONERR + 16)
#endif

/** @see CH_STATUS_IS_EINPROGRESS */
#ifdef EINPROGRESS
#define CH_EINPROGRESS EINPROGRESS
#else
#define CH_EINPROGRESS    (CH_OS_START_CANONERR + 17)
#endif

/**
 * @see CH_STATUS_IS_ECONNABORTED
 * @warning use CH_STATUS_IS_ECONNABORTED instead of just testing this value
 */

#ifdef ECONNABORTED
#define CH_ECONNABORTED ECONNABORTED
#else
#define CH_ECONNABORTED   (CH_OS_START_CANONERR + 18)
#endif

/** @see CH_STATUS_IS_ECONNRESET */
#ifdef ECONNRESET
#define CH_ECONNRESET ECONNRESET
#else
#define CH_ECONNRESET     (CH_OS_START_CANONERR + 19)
#endif

/** @see CH_STATUS_IS_ETIMEDOUT
 *  @deprecated */
#ifdef ETIMEDOUT
#define CH_ETIMEDOUT ETIMEDOUT
#else
#define CH_ETIMEDOUT      (CH_OS_START_CANONERR + 20)
#endif

/** @see CH_STATUS_IS_EHOSTUNREACH */
#ifdef EHOSTUNREACH
#define CH_EHOSTUNREACH EHOSTUNREACH
#else
#define CH_EHOSTUNREACH   (CH_OS_START_CANONERR + 21)
#endif

/** @see CH_STATUS_IS_ENETUNREACH */
#ifdef ENETUNREACH
#define CH_ENETUNREACH ENETUNREACH
#else
#define CH_ENETUNREACH    (CH_OS_START_CANONERR + 22)
#endif

/** @see CH_STATUS_IS_EFTYPE */
#ifdef EFTYPE
#define CH_EFTYPE EFTYPE
#else
#define CH_EFTYPE        (CH_OS_START_CANONERR + 23)
#endif

/** @see CH_STATUS_IS_EPIPE */
#ifdef EPIPE
#define CH_EPIPE EPIPE
#else
#define CH_EPIPE         (CH_OS_START_CANONERR + 24)
#endif

/** @see CH_STATUS_IS_EXDEV */
#ifdef EXDEV
#define CH_EXDEV EXDEV
#else
#define CH_EXDEV         (CH_OS_START_CANONERR + 25)
#endif

/** @see CH_STATUS_IS_ENOTEMPTY */
#ifdef ENOTEMPTY
#define CH_ENOTEMPTY ENOTEMPTY
#else
#define CH_ENOTEMPTY     (CH_OS_START_CANONERR + 26)
#endif

/** @see CH_STATUS_IS_EAFNOSUPPORT */
#ifdef EAFNOSUPPORT
#define CH_EAFNOSUPPORT EAFNOSUPPORT
#else
#define CH_EAFNOSUPPORT  (CH_OS_START_CANONERR + 27)
#endif

/** @} */

#define CH_FROM_OS_ERROR(e)  (e)
#define CH_TO_OS_ERROR(e)    (e)

#define ch_get_os_error()    (errno)
#define ch_set_os_error(e)   (errno = (e))

/* A special case, only socket calls require this:
 */
#define ch_get_netos_error() (errno)
#define ch_set_netos_error(e) (errno = (e))

/**
 * @addtogroup CH_STATUS_IS
 * @{
 */

/** permission denied */
#define CH_STATUS_IS_EACCES(s)         ((s) == CH_EACCES)
/** file exists */
#define CH_STATUS_IS_EEXIST(s)         ((s) == CH_EEXIST)
/** path name is too long */
#define CH_STATUS_IS_ENAMETOOLONG(s)   ((s) == CH_ENAMETOOLONG)
/**
 * no such file or directory
 * @remark
 * EMVSCATLG can be returned by the automounter on z/OS for
 * paths which do not exist.
 */
#ifdef EMVSCATLG
#define CH_STATUS_IS_ENOENT(s)         ((s) == CH_ENOENT \
                                      || (s) == EMVSCATLG)
#else
#define CH_STATUS_IS_ENOENT(s)         ((s) == CH_ENOENT)
#endif
/** not a directory */
#define CH_STATUS_IS_ENOTDIR(s)        ((s) == CH_ENOTDIR)
/** no space left on device */
#ifdef EDQUOT
#define CH_STATUS_IS_ENOSPC(s)         ((s) == CH_ENOSPC \
                                      || (s) == EDQUOT)
#else
#define CH_STATUS_IS_ENOSPC(s)         ((s) == CH_ENOSPC)
#endif
/** not enough memory */
#define CH_STATUS_IS_ENOMEM(s)         ((s) == CH_ENOMEM)
/** too many open files */
#define CH_STATUS_IS_EMFILE(s)         ((s) == CH_EMFILE)
/** file table overflow */
#define CH_STATUS_IS_ENFILE(s)         ((s) == CH_ENFILE)
/** bad file # */
#define CH_STATUS_IS_EBADF(s)          ((s) == CH_EBADF)
/** invalid argument */
#define CH_STATUS_IS_EINVAL(s)         ((s) == CH_EINVAL)
/** illegal seek */
#define CH_STATUS_IS_ESPIPE(s)         ((s) == CH_ESPIPE)

/** operation would block */
#if !defined(EWOULDBLOCK) || !defined(EAGAIN)
#define CH_STATUS_IS_EAGAIN(s)         ((s) == CH_EAGAIN)
#elif (EWOULDBLOCK == EAGAIN)
#define CH_STATUS_IS_EAGAIN(s)         ((s) == CH_EAGAIN)
#else
#define CH_STATUS_IS_EAGAIN(s)         ((s) == CH_EAGAIN \
                                      || (s) == EWOULDBLOCK)
#endif

/** interrupted system call */
#define CH_STATUS_IS_EINTR(s)          ((s) == CH_EINTR)
/** socket operation on a non-socket */
#define CH_STATUS_IS_ENOTSOCK(s)       ((s) == CH_ENOTSOCK)
/** Connection Refused */
#define CH_STATUS_IS_ECONNREFUSED(s)   ((s) == CH_ECONNREFUSED)
/** operation now in progress */
#define CH_STATUS_IS_EINPROGRESS(s)    ((s) == CH_EINPROGRESS)

/**
 * Software caused connection abort
 * @remark
 * EPROTO on certain older kernels really means ECONNABORTED, so we need to
 * ignore it for them.  See discussion in new-httpd archives nh.9701 & nh.9603
 *
 * There is potentially a bug in Solaris 2.x x<6, and other boxes that
 * implement tcp sockets in userland (i.e. on top of STREAMS).  On these
 * systems, EPROTO can actually result in a fatal loop.  See PR#981 for
 * example.  It's hard to handle both uses of EPROTO.
 */
#ifdef EPROTO
#define CH_STATUS_IS_ECONNABORTED(s)    ((s) == CH_ECONNABORTED \
                                       || (s) == EPROTO)
#else
#define CH_STATUS_IS_ECONNABORTED(s)    ((s) == CH_ECONNABORTED)
#endif

/** Connection Reset by peer */
#define CH_STATUS_IS_ECONNRESET(s)      ((s) == CH_ECONNRESET)
/** Operation timed out
 *  @deprecated */
#define CH_STATUS_IS_ETIMEDOUT(s)      ((s) == CH_ETIMEDOUT)
/** no route to host */
#define CH_STATUS_IS_EHOSTUNREACH(s)    ((s) == CH_EHOSTUNREACH)
/** network is unreachable */
#define CH_STATUS_IS_ENETUNREACH(s)     ((s) == CH_ENETUNREACH)
/** inappropiate file type or format */
#define CH_STATUS_IS_EFTYPE(s)          ((s) == CH_EFTYPE)
/** broken pipe */
#define CH_STATUS_IS_EPIPE(s)           ((s) == CH_EPIPE)
/** cross device link */
#define CH_STATUS_IS_EXDEV(s)           ((s) == CH_EXDEV)
/** Directory Not Empty */
#define CH_STATUS_IS_ENOTEMPTY(s)       ((s) == CH_ENOTEMPTY || \
                                          (s) == CH_EEXIST)
/** Address Family not supported */
#define CH_STATUS_IS_EAFNOSUPPORT(s)    ((s) == CH_EAFNOSUPPORT)
/** @} */


#endif /* CH_ERRNO_H */

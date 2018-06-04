/*
 *
 *      Filename: ch_signal.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 16:52:52
 * Last Modified: 2017-01-11 17:07:05
 */

#include "ch_signal.h"
#include "ch_errno.h"

/*
 * Replace standard signal() with the more reliable sigaction equivalent
 * from W. Richard Stevens' "Advanced Programming in the UNIX Environment"
 * (the version that does not automatically restart system calls).
 */
ch_sigfunc_t * ch_signal(int signo, ch_sigfunc_t * func)
{
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT             /* SunOS */
    act.sa_flags |= SA_INTERRUPT;
#endif
#if defined(__osf__) && defined(__alpha)
    /* XXX jeff thinks this should be enabled whenever SA_NOCLDWAIT is defined */

    /* this is required on Tru64 to cause child processes to
     * disappear gracefully - XPG4 compatible 
     */
    if ((signo == SIGCHLD) && (func == SIG_IGN)) {
        act.sa_flags |= SA_NOCLDWAIT;
    }
#endif
    if (sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return oact.sa_handler;
}


/* AC_DECL_SYS_SIGLIST defines either of these symbols depending
 * on the version of autoconf used. */

void ch_signal_init(ch_pool_t *pglobal)
{

	pglobal = pglobal;

}

const char *ch_signal_description_get(int signum)
{
    return (signum >= 0) ? sys_siglist[signum] : "unknown signal (number)";
}


int ch_signal_block(int signum)
{
    sigset_t sig_mask;
    int rv;

    sigemptyset(&sig_mask);

    sigaddset(&sig_mask, signum);

    if ((rv = sigprocmask(SIG_BLOCK, &sig_mask, NULL)) != 0) {
        rv = errno;
    }
    return rv;
}

int ch_signal_unblock(int signum)
{
    sigset_t sig_mask;
    int rv;

    sigemptyset(&sig_mask);

    sigaddset(&sig_mask, signum);

    if ((rv = sigprocmask(SIG_UNBLOCK, &sig_mask, NULL)) != 0) {
        rv = errno;
    }
    return rv;
}

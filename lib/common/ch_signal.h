/*
 *
 *      Filename: ch_signal.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 16:48:01
 * Last Modified: 2017-01-11 16:48:01
 */

#ifndef CH_SIGNAL_H
#define CH_SIGNAL_H

#include "ch_mpool.h"

/** Function prototype for signal handlers */
typedef void ch_sigfunc_t(int);

/**
 * Set the signal handler function for a given signal
 * @param signo The signal (eg... SIGWINCH)
 * @param func the function to get called
 */
ch_sigfunc_t * ch_signal(int signo, ch_sigfunc_t * func);

#if defined(SIG_IGN) && !defined(SIG_ERR)
#define SIG_ERR ((ch_sigfunc_t *) -1)
#endif



/**
 * Get the description for a specific signal number
 * @param signum The signal number
 * @return The description of the signal
 */
const char * ch_signal_description_get(int signum);

/**
 * GW-private function for initializing the signal package
 * @internal
 * @param pglobal The internal, global pool
 */
void ch_signal_init(ch_pool_t *pglobal);

/**
 * Block the delivery of a particular signal
 * @param signum The signal number
 * @return status
 */
int ch_signal_block(int signum);

/**
 * Enable the delivery of a particular signal
 * @param signum The signal number
 * @return status
 */
int ch_signal_unblock(int signum);


#endif /* CH_SIGNAL_H */

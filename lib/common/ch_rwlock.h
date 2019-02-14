/*
 *
 *      Filename: ch_rwlock.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 13:25:44
 * Last Modified: 2017-02-10 13:25:44
 */

#ifndef CH_RWLOCK_H
#define CH_RWLOCK_H

#include "ch_constants.h"
#include "ch_atomic.h"



/**
 * The ch_rwlock_t type.
 *
 * cnt is -1 when write lock is held, and > 0 when read locks are held.
 */
typedef struct {
	volatile int32_t cnt; /**< -1 when W lock held, > 0 when R locks held. */
} ch_rwlock_t;

/**
 * A static rwlock initializer.
 */
#define CH_RWLOCK_INITIALIZER { 0 }

/**
 * Initialize the rwlock to an unlocked state.
 *
 * @param rwl
 *   A pointer to the rwlock structure.
 */
static inline void
ch_rwlock_init(ch_rwlock_t *rwl)
{
	rwl->cnt = 0;
}

/**
 * Take a read lock. Loop until the lock is held.
 *
 * @param rwl
 *   A pointer to a rwlock structure.
 */
static inline void
ch_rwlock_read_lock(ch_rwlock_t *rwl)
{
	int32_t x;
	int success = 0;

	while (success == 0) {
		x = rwl->cnt;
		/* write lock is held */
		if (x < 0) {
			ch_pause();
			continue;
		}
		success = ch_atomic32_cmpset((volatile uint32_t *)&rwl->cnt,
					      x, x + 1);
	}
}

/**
 * Release a read lock.
 *
 * @param rwl
 *   A pointer to the rwlock structure.
 */
static inline void
ch_rwlock_read_unlock(ch_rwlock_t *rwl)
{
	ch_atomic32_dec((ch_atomic32_t *)(intptr_t)&rwl->cnt);
}

/**
 * Take a write lock. Loop until the lock is held.
 *
 * @param rwl
 *   A pointer to a rwlock structure.
 */
static inline void
ch_rwlock_write_lock(ch_rwlock_t *rwl)
{
	int32_t x;
	int success = 0;

	while (success == 0) {
		x = rwl->cnt;
		/* a lock is held */
		if (x != 0) {
			ch_pause();
			continue;
		}
		success = ch_atomic32_cmpset((volatile uint32_t *)&rwl->cnt,
					      0, -1);
	}
}

/**
 * Release a write lock.
 *
 * @param rwl
 *   A pointer to a rwlock structure.
 */
static inline void
ch_rwlock_write_unlock(ch_rwlock_t *rwl)
{
	ch_atomic32_inc((ch_atomic32_t *)(intptr_t)&rwl->cnt);
}



#endif /*CH_RWLOCK_H*/

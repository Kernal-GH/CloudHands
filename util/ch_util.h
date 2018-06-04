/*
 * =====================================================================================
 *
 *       Filename:  ch_util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月26日 16时41分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_UTIL_H
#define CH_UTIL_H

#include <apr_pools.h>

/*********** Macros to work with powers of 2 ********/

/**
 * Returns true if n is a power of 2
 * @param n
 *     Number to check
 * @return 1 if true, 0 otherwise
 */
static inline int ch_is_power_of_2(uint32_t n)
{
	return n && !(n & (n - 1));
}

/**
 * Aligns input parameter to the next power of 2
 *
 * @param x
 *   The integer value to algin
 *
 * @return
 *   Input parameter aligned to the next power of 2
 */
static inline uint32_t ch_align32pow2(uint32_t x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return x + 1;
}

/**
 * Aligns 64b input parameter to the next power of 2
 *
 * @param v
 *   The 64b value to align
 *
 * @return
 *   Input parameter aligned to the next power of 2
 */
static inline uint64_t ch_align64pow2(uint64_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;

	return v + 1;
}

static inline size_t ch_uint64_zshu(uint64_t v){

	size_t i = 0;
	while((v>>(i++))!=1);

	return i-1;
}
/*********** Macros for calculating min and max **********/

/**
 * Macro to return the minimum of two numbers
 */
#define CH_MIN(a, b) ({ \
		typeof (a) _a = (a); \
		typeof (b) _b = (b); \
		_a < _b ? _a : _b; \
	})

/**
 * Macro to return the maximum of two numbers
 */
#define CH_MAX(a, b) ({ \
		typeof (a) _a = (a); \
		typeof (b) _b = (b); \
		_a > _b ? _a : _b; \
	})


extern char  *ch_current_logtime(apr_pool_t *mp);

extern char  *ch_current_logtime_with_buf(char *buf,size_t buf_size);

extern char  *ch_current_filetime(apr_pool_t *mp);

extern unsigned long ch_get_current_timems(void);


extern char * ch_ipaddr_str_get(char *buf,size_t blen,uint32_t addr);

extern uint16_t ch_ip_fast_csum(const void *iph, unsigned int ihl);

extern int ch_dir_make(const char *orig_path);

#endif /*CH_UTIL_H*/


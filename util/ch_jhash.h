/*
 * =====================================================================================
 *
 *       Filename:  ch_jhash.h
 *
 *    Description: from linux kernel's jhash.h  
 *
 *        Version:  1.0
 *        Created:  2016年02月26日 12时29分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_JHASH_H
#define CH_JHASH_H

#include "ch_bitops.h"

#define __do_swap(a,b) do {uint32_t t; if(a>b){t = a; a=b;b=t;}}while(0)

/* Best hash sizes are of power of two */
#define ch_jhash_size(n)   ((uint32_t)1<<(n))
/* Mask the hash value, i.e (value & jhash_mask(n)) instead of (value % n) */
#define ch_jhash_mask(n)   (jhash_size(n)-1)

/* __jhash_mix -- mix 3 32-bit values reversibly. */
#define __jhash_mix(a, b, c)			\
{						\
	a -= c;  a ^= rol32(c, 4);  c += b;	\
	b -= a;  b ^= rol32(a, 6);  a += c;	\
	c -= b;  c ^= rol32(b, 8);  b += a;	\
	a -= c;  a ^= rol32(c, 16); c += b;	\
	b -= a;  b ^= rol32(a, 19); a += c;	\
	c -= b;  c ^= rol32(b, 4);  b += a;	\
}

/* __jhash_final - final mixing of 3 32-bit values (a,b,c) into c */
#define __jhash_final(a, b, c)			\
{						\
	c ^= b; c -= rol32(b, 14);		\
	a ^= c; a -= rol32(c, 11);		\
	b ^= a; b -= rol32(a, 25);		\
	c ^= b; c -= rol32(b, 16);		\
	a ^= c; a -= rol32(c, 4);		\
	b ^= a; b -= rol32(a, 14);		\
	c ^= b; c -= rol32(b, 24);		\
}

/* An arbitrary initial parameter */
#define JHASH_INITVAL		0xdeadbeef

/* ch_jhash - hash an arbitrary key
 * @k: sequence of bytes as key
 * @length: the length of the key
 * @initval: the previous hash, or an arbitray value
 *
 * The generic version, hashes an arbitrary sequence of bytes.
 * No alignment or length assumptions are made about the input key.
 *
 * Returns the hash value of the key. The result depends on endianness.
 */
static inline uint32_t ch_jhash(const unsigned char *key, uint32_t length, uint32_t initval)
{
	uint32_t a, b, c;
	const unsigned char *k = key;

	/* Set up the internal state */
	a = b = c = JHASH_INITVAL + length + initval;

	/* All but the last block: affect some 32 bits of (a,b,c) */
	while (length > 12) {
		a += (uint32_t)k[0];
		b += (uint32_t)k[4];
		c += (uint32_t)k[8];
		__jhash_mix(a, b, c);
		length -= 12;
		k += 12;
	}
	/* Last block: affect all 32 bits of (c) */
	/* All the case statements fall through */
	switch (length) {
	case 12: c += (uint32_t)k[11]<<24;
	case 11: c += (uint32_t)k[10]<<16;
	case 10: c += (uint32_t)k[9]<<8;
	case 9:  c += k[8];
	case 8:  b += (uint32_t)k[7]<<24;
	case 7:  b += (uint32_t)k[6]<<16;
	case 6:  b += (uint32_t)k[5]<<8;
	case 5:  b += k[4];
	case 4:  a += (uint32_t)k[3]<<24;
	case 3:  a += (uint32_t)k[2]<<16;
	case 2:  a += (uint32_t)k[1]<<8;
	case 1:  a += k[0];
		 __jhash_final(a, b, c);
	case 0: /* Nothing left to add */
		break;
	}

	return c;
}

/* ch_jhash2 - hash an array of uint32_t's
 * @k: the key which must be an array of uint32_t's
 * @length: the number of uint32_t's in the key
 * @initval: the previous hash, or an arbitray value
 *
 * Returns the hash value of the key.
 */
static inline uint32_t ch_jhash2(const uint32_t *k, uint32_t length, uint32_t initval)
{
	uint32_t a, b, c;

	/* Set up the internal state */
	a = b = c = JHASH_INITVAL + (length<<2) + initval;

	/* Handle most of the key */
	while (length > 3) {
		a += k[0];
		b += k[1];
		c += k[2];
		__jhash_mix(a, b, c);
		length -= 3;
		k += 3;
	}

	/* Handle the last 3 uint32_t's: all the case statements fall through */
	switch (length) {
	case 3: c += k[2];
	case 2: b += k[1];
	case 1: a += k[0];
		__jhash_final(a, b, c);
	case 0:	/* Nothing left to add */
		break;
	}

	return c;
}


/* __jhash_nwords - hash exactly 3, 2 or 1 word(s) */
static inline uint32_t __jhash_nwords(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{
	a += initval;
	b += initval;
	c += initval;

	__jhash_final(a, b, c);

	return c;
}

static inline uint32_t ch_jhash_4words(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t initval){
    uint32_t words[4];
    words[0]=a;
    words[1]=b;
    words[2]=c;
    words[3]=d;

    return ch_jhash2(words,4,initval);
}

static inline uint32_t ch_jhash_4words_sort(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t initval){
    uint32_t words[4];
    __do_swap(a,b);
    __do_swap(a,c);
    __do_swap(a,d);
    __do_swap(b,c);
    __do_swap(b,d);
    __do_swap(c,d);

    words[0]=a;
    words[1]=b;
    words[2]=c;
    words[3]=d;

    return ch_jhash2(words,4,initval);
}

static inline uint32_t ch_jhash_3words(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{
	return __jhash_nwords(a, b, c, initval + JHASH_INITVAL + (3 << 2));
}

static inline uint32_t ch_jhash_3words_sort(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{

    __do_swap(a,b);
    __do_swap(a,c);
    __do_swap(b,c);

	return __jhash_nwords(a, b, c, initval + JHASH_INITVAL + (3 << 2));
}

static inline uint32_t ch_jhash_2words(uint32_t a, uint32_t b, uint32_t initval)
{
	return __jhash_nwords(a, b, 0, initval + JHASH_INITVAL + (2 << 2));
}

static inline uint32_t ch_jhash_2words_sort(uint32_t a, uint32_t b, uint32_t initval)
{
    __do_swap(a,b);

	return __jhash_nwords(a,b, 0, initval + JHASH_INITVAL + (2 << 2));
}

static inline uint32_t ch_jhash_1word(uint32_t a, uint32_t initval)
{
	return __jhash_nwords(a, 0, 0, initval + JHASH_INITVAL + (1 << 2));
}
#endif /*CH_JHASH_H*/


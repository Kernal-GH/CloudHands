/*
 *
 *      Filename: ch_stat_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-04 11:09:20
 * Last Modified: 2018-06-04 11:09:20
 */

#ifndef CH_STAT_POOL_H
#define CH_STAT_POOL_H

typedef struct ch_stat_pool_t ch_stat_pool_t;

/*Stat types*/
enum {

	STAT_ALL,
	STAT_TCP,
	STAT_UDP,
	STAT_ICMP,
	STAT_ARP,
	STAT_NUM,
};

struct ch_stat_pool_t {

	uint64_t stat_time_up;
	uint64_t stat_time_tv;

};



#endif /*CH_STAT_POOL_H*/

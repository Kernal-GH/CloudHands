/*
 *
 *      Filename: ch_packets_stats.h
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-05-26 16:15:12
 * Last Modified: 2017-05-31 10:35:12
 */

#ifndef CH_PACKETS_STATS_H
#define CH_PACKETS_STATS_H

typedef struct ch_packets_stats_t ch_packets_stats_t;


struct ch_packets_stats_t {

	uint64_t total_packets;
	uint64_t total_packets_size;

};

#endif /*CH_PACKETS_STATS_H*/

/*
 *
 *      Filename: ch_process_interface.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-31 17:58:58
 * Last Modified: 2018-01-31 17:58:58
 */

#ifndef CH_PROCESS_INTERFACE_H
#define CH_PROCESS_INTERFACE_H

typedef struct ch_process_interface_t ch_process_interface_t;
typedef struct ch_process_queue_t ch_process_queue_t;

#include "ch_tables.h"
#include "ch_mpool.h"
#include "ch_packet.h"

struct ch_process_interface_t {

	ch_pool_t *mp;
	const char *qprefix;

	uint32_t qnumber;
	uint32_t qsize;

	int (*filter)(ch_packet_t *pkt,void *priv_data);
	uint32_t (*hash)(ch_packet_t *pkt,void *priv_data);

	uint32_t cur_index;

	ch_array_header_t *queues;

	void *priv_data;
};

struct ch_process_queue_t {

	const char *qname;
	
	uint64_t packets;
	uint64_t ok_packets;
	uint64_t error_packets;

	struct rte_ring *ring;
};

extern ch_process_interface_t * ch_process_interface_writer_create(ch_pool_t *mp,
	const char *qprefix,
	uint32_t qnumber,
	uint32_t qsize,
	int (*filter)(ch_packet_t *pkt,void *priv_data),
	uint32_t (*hash)(ch_packet_t *pkt,void *priv_data),
	void *priv_data);

extern ch_process_interface_t * ch_process_interface_reader_create(ch_pool_t *mp,
	const char *qprefix,
	uint32_t qnumber);

extern int ch_process_interface_put(ch_process_interface_t *pint,ch_packet_t *pkt);

extern ch_packet_t * ch_process_queue_pop(ch_process_queue_t *queue);

extern ch_process_queue_t * ch_process_interface_queue_assign(ch_process_interface_t *pint);

extern void ch_process_interface_dump(ch_process_interface_t *pint,FILE *fp);

#endif /*CH_PROCESS_INTERFACE_H*/

/*
 *
 *      Filename: ch_session_monitor.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-09 17:40:14
 * Last Modified: 2017-11-09 17:40:14
 */

#ifndef CH_SESSION_MONITOR_H
#define CH_SESSION_MONITOR_H

typedef struct ch_session_monitor_t ch_session_monitor_t;
typedef struct ch_session_monitor_item_t ch_session_monitor_item_t;
typedef struct ch_session_monitor_hdr_t ch_session_monitor_hdr_t;

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*monitor states*/
#define MON_STATE_INIT 0
#define MON_STATE_START 1
#define MON_STATE_STOP 2
#define MON_STATE_LIVING 3
#define MON_STATE_DEL 4

/*item types*/
#define MON_ITEM_TYPE_IP 0
#define MON_ITEM_TYPE_PORT 1
#define MON_ITEM_TYPE_IP_PORT 2
#define MON_ITEM_TYPE_SESSION 3

struct ch_session_monitor_t {

	int fd;

	ch_session_monitor_hdr_t *monitor_hdr;
	ch_session_monitor_item_t *monitor_items;
};

#define CHECK_VALUE 1234

#define ID_INIT_VALUE 12345

#pragma pack(push,1)
struct ch_session_monitor_hdr_t {

	int check_start;
	uint32_t item_number;
	uint64_t next_id;
};
#pragma pack(pop)

#pragma pack(push,1)
struct ch_session_monitor_item_t {

	int check_start;
	uint64_t id;
	uint32_t src_ip;
	uint32_t dst_ip;

	uint16_t src_port;
	uint16_t dst_port;

	uint64_t monitor_time_tv;
	uint64_t monitor_start_time;
	uint64_t monitor_last_time;
	int monitor_type;
	int monitor_state;

};
#pragma pack(pop)

#define MON_VALUE_CHECK(v) do { \
	if((v)!=CHECK_VALUE)        \
		abort();                \
}while(0)

#define ch_session_monitor_item_index(monitor,item) ((item)-(monitor)->monitor_items)

static inline uint64_t ch_session_monitor_alloc_id(ch_session_monitor_t *monitor){

	uint64_t id = monitor->monitor_hdr->next_id;

	 monitor->monitor_hdr->next_id = (monitor->monitor_hdr->next_id+1)%UINT64_MAX;

	 return id;
}

extern int ch_session_monitor_load(ch_session_monitor_t *monitor,const char *mmap_fname,size_t msize);

extern void ch_session_monitor_exit(ch_session_monitor_t *monitor);

extern ch_session_monitor_item_t * ch_session_monitor_item_find(ch_session_monitor_t *monitor,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port);

extern ch_session_monitor_item_t * ch_session_monitor_item_find_ignore_state(ch_session_monitor_t *monitor,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port);

extern ch_session_monitor_item_t * ch_session_monitor_item_findById(ch_session_monitor_t *monitor,uint64_t id);


extern uint64_t ch_session_monitor_item_add_ip(ch_session_monitor_t *monitor,uint32_t ip,uint64_t monitor_time_tv);

extern uint64_t ch_session_monitor_item_add_port(ch_session_monitor_t *monitor,uint16_t port,uint64_t monitor_time_tv);

extern uint64_t ch_session_monitor_item_add_ip_port(ch_session_monitor_t *monitor,uint32_t ip,uint16_t port,
        uint64_t monitor_time_tv);

extern uint64_t ch_session_monitor_item_add_session(ch_session_monitor_t *monitor,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,
	uint64_t monitor_time_tv);


extern void ch_session_monitor_item_del(ch_session_monitor_t *monitor,uint64_t id);

extern void ch_session_monitor_item_stop(ch_session_monitor_t *monitor,uint64_t id);


extern void ch_session_monitor_item_restart(ch_session_monitor_t *monitor,uint64_t id);

extern ch_session_monitor_item_t * ch_session_monitor_item_get(ch_session_monitor_t *monitor,int index);

extern int ch_session_monitor_item_count(ch_session_monitor_t *monitor);

extern void ch_session_monitor_dump(ch_session_monitor_t *monitor,FILE *fp);

extern void ch_session_monitor_item_dump(ch_session_monitor_item_t *item,FILE *fp);

#endif /*CH_SESSION_MONITOR_H*/

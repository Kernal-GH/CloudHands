/*
 *
 *      Filename: ch_mprocess_shared_info.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-09 19:39:26
 * Last Modified: 2017-11-09 19:39:26
 */

#ifndef CH_MPROCESS_INFO_H
#define CH_MPROCESS_INFO_H

typedef struct ch_mprocess_info_t ch_mprocess_info_t;
typedef struct ch_mprocess_queue_pool_info_t ch_mprocess_queue_pool_info_t;
typedef struct ch_mprocess_queue_info_t ch_mprocess_queue_info_t;

#define MAX_NAME_SIZE 64
#define MPROCESS_NAME "mprocess"

#define MPROCESS_MEMSIZE_DEFAULT 512*1024

struct ch_mprocess_info_t {

	void *base_addr;
	uint64_t size;

	uint32_t qpool_number;
	uint64_t alloc_offset;
	uint64_t qpool_head;
};

struct ch_mprocess_queue_pool_info_t {

	char name[MAX_NAME_SIZE];
	char filter_name[MAX_NAME_SIZE];
	char hash_name[MAX_NAME_SIZE];

	uint32_t queue_number;
	uint64_t queue_head;
	uint64_t next;

	struct {
	
		uint64_t packets;
		uint64_t drop_packets;
	} stats;
};

struct ch_mprocess_queue_info_t {

	char name[MAX_NAME_SIZE];
	uint32_t queue_size;

	uint64_t next;

	struct {
	
		uint64_t packets;
		uint64_t drop_packets;
	} stats;
};

#define OFFSET_TO_ADDR(mp_info,off) (void*)((mp_info)->base_addr+(off))
#define ADDR_TO_OFFSET(mp_info,addr) (uint64_t)((addr)-(mp_info)->base_addr)

#define IS_FULL(mp_info,sz) ((mp_info)->alloc_offset+(sz)>(mp_info)->size) 

static inline int check_name(const char *name){

	if(name == NULL||strlen(name)>=MAX_NAME_SIZE)
		return -1;

	return 0;
}

static inline ch_mprocess_queue_pool_info_t* ch_mprocess_queue_pool_info_first(ch_mprocess_info_t *mp_info){

	uint64_t off = mp_info->qpool_head;

	if(mp_info->qpool_number == 0||mp_info->qpool_head == 0)
		return NULL;

	void *addr = OFFSET_TO_ADDR(mp_info,off);
	return (ch_mprocess_queue_pool_info_t*)addr;
}

static inline ch_mprocess_queue_pool_info_t* ch_mprocess_queue_pool_info_next(ch_mprocess_info_t *mp_info,
	ch_mprocess_queue_pool_info_t *prev){

	if(prev->next == 0){
	
		return NULL;
	}

	void *addr = OFFSET_TO_ADDR(mp_info,prev->next);

	return (ch_mprocess_queue_pool_info_t*)addr;
}

#define list_for_mprocess_queue_pool_info(mp_info,qpool_info) \
	for(qpool_info = ch_mprocess_queue_pool_info_first(mp_info);qpool_info!=NULL;qpool_info = ch_mprocess_queue_pool_info_next(mp_info,qpool_info))

static inline ch_mprocess_queue_info_t* ch_mprocess_queue_info_first(ch_mprocess_info_t *mp_info,ch_mprocess_queue_pool_info_t *qpool_info){

	uint64_t off = qpool_info->queue_head;

	if(off == 0 || qpool_info->queue_number == 0)
		return NULL;

	void *addr = OFFSET_TO_ADDR(mp_info,off);
	return (ch_mprocess_queue_info_t*)addr;
}

static inline ch_mprocess_queue_info_t* ch_mprocess_queue_info_next(ch_mprocess_info_t *mp_info,
	ch_mprocess_queue_info_t *prev){

	if(prev->next == 0){
	
		return NULL;
	}

	void *addr = OFFSET_TO_ADDR(mp_info,prev->next);

	return (ch_mprocess_queue_info_t*)addr;
}

#define list_for_mprocess_queue_info(mp_info,qpool_info,queue) \
	for(queue = ch_mprocess_queue_info_first(mp_info,qpool_info);queue!=NULL;queue = ch_mprocess_queue_info_next(mp_info,queue))


static inline void* ch_mprocess_malloc(ch_mprocess_info_t *mp_info,uint64_t sz){

	void *addr;
	uint64_t cur_offset = mp_info->alloc_offset;

	if(sz == 0||IS_FULL(mp_info,sz))
		return NULL;

	addr = OFFSET_TO_ADDR(mp_info,cur_offset);
	mp_info->alloc_offset+=sz;

	return addr;
}

static inline void* ch_mprocess_calloc(ch_mprocess_info_t *mp_info,uint64_t sz){

	void *addr;
	uint64_t cur_offset = mp_info->alloc_offset;

	if(sz == 0||IS_FULL(mp_info,sz))
		return NULL;

	addr = OFFSET_TO_ADDR(mp_info,cur_offset);
	mp_info->alloc_offset+=sz;

	memset(addr,0,sz);

	return addr;
}


extern ch_mprocess_info_t * ch_mprocess_info_create(uint64_t size);

extern ch_mprocess_info_t * ch_mprocess_info_get(void);

extern void ch_mprocess_info_dump(ch_mprocess_info_t *mp_info,FILE *fp);

extern ch_mprocess_queue_pool_info_t* ch_mprocess_queue_pool_info_lookup(ch_mprocess_info_t *mp_info,const char * name);

extern ch_mprocess_queue_pool_info_t* ch_mprocess_queue_pool_info_create(ch_mprocess_info_t *mp_info,const char *name,
	const char *filter_name,const char *hash_name);

extern ch_mprocess_queue_info_t* ch_mprocess_queue_info_lookup(ch_mprocess_info_t *mp_info,ch_mprocess_queue_pool_info_t *qpool_info,const char * name);

extern ch_mprocess_queue_info_t* ch_mprocess_queue_info_create(ch_mprocess_info_t *mp_info,ch_mprocess_queue_pool_info_t *qpool_info,const char *name,uint32_t queue_size);

#endif /*CH_MPROCESS_INFO_H*/

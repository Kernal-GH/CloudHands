/*
 *
 *      Filename: ch_stat_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-01 10:47:26
 * Last Modified: 2018-06-01 10:47:26
 */

#ifndef CH_STAT_ENTRY_H
#define CH_STAT_ENTRY_H

typedef struct ch_stat_entry_t ch_stat_entry_t;

#include <stdint.h>

#pragma pack(push,1)
struct ch_stat_entry_t {
	uint64_t st_n;
	uint64_t st_v;
};
#pragma pack(pop)


#define ch_stat_entry_init(entry) do {  \
    (entry)->st_n = 0;					\
    (entry)->st_v = 0;                  \
}while(0)


#define ch_stat_entry_value_update(entry,uv) do { \
	(entry)->st_n = (entry)->st_n+1;			  \
	(entry)->st_v = (entry)->st_v+(uv);			  \
}while(0)


#endif /*CH_STAT_ENTRY_H*/

/*
 *
 *      Filename: ch_proto_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-08-16 16:47:50
 * Last Modified: 2018-08-16 16:47:50
 */

#ifndef CH_PROTO_SESSION_ENTRY_H
#define CH_PROTO_SESSION_ENTRY_H

typedef struct ch_proto_session_entry_t ch_proto_session_entry_t;

#include <stdint.h>

struct ch_proto_session_entry_t {

	uint64_t reqPackets;
	uint64_t resPackets;
	uint64_t reqBytes;
	uint64_t resBytes;

};

#define ch_proto_session_entry_init(entry) do { \
	(entry)->reqPackets = 0;\
	(entry)->resPackets = 0;\
	(entry)->reqBytes = 0;\
	(entry)->resBytes = 0;\
}while(0)

#define ch_proto_session_entry_update(entry,dsize,is_req) do { \
	if(is_req){ \
		(entry)->reqPackets+=1; \
		(entry)->reqBytes += (dsize); \
	}else{ \
		(entry)->resPackets+=1; \
		(entry)->resBytes += (dsize);	\
	} \
}while(0)

#endif /*CH_PROTO_SESSION_ENTRY_H*/

/*
 *
 *      Filename: ch_mysql_ok.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-10-31 11:37:23
 * Last Modified: 2018-10-31 11:37:23
 */

#ifndef CH_MYSQL_OK_H
#define CH_MYSQL_OK_H

typedef struct ch_mysql_ok_t ch_mysql_ok_t;
typedef struct ch_mysql_session_track_t ch_mysql_session_track_t;
typedef struct ch_mysql_session_track_sysvar_t ch_mysql_session_track_sysvar_t;
typedef struct ch_mysql_session_track_schema_t ch_mysql_session_track_schema_t;
typedef struct ch_mysql_session_track_state_change_t ch_mysql_session_track_state_change_t;
typedef struct ch_mysql_session_track_unkown_t ch_mysql_session_track_unkown_t;

struct ch_mysql_ok_t {

	uint64_t affected_rows;
	uint64_t insert_id;
	uint16_t server_status;
	uint16_t warns;
	
	const char *message;
	
	uint32_t session_track_n;

	struct list_head session_track_list;
};

struct ch_mysql_session_track_t {

	struct list_head node;
	uint8_t data_type;

};

struct ch_mysql_session_track_sysvar_t {

	ch_mysql_session_track_t track;
	const char *name;
	const char *value;
};

struct ch_mysql_session_track_schema_t {

	ch_mysql_session_track_t track;
	const char *schema;
};

struct ch_mysql_session_track_state_change_t {

	ch_mysql_session_track_t track;
	uint8_t state;
};

struct ch_mysql_session_track_unkown_t {

	ch_mysql_session_track_t track;
	uint32_t dlen;
	unsigned char *data;
};

#endif /*CH_MYSQL_OK_H*/

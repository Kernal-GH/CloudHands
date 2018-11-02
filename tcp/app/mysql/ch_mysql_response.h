/*
 *
 *      Filename: ch_mysql_response.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-10-31 12:05:05
 * Last Modified: 2018-10-31 12:05:05
 */

#ifndef CH_MYSQL_RESPONSE_H
#define CH_MYSQL_RESPONSE_H

typedef struct ch_mysql_response_t ch_mysql_response_t;
typedef struct ch_mysql_field_pool_t ch_mysql_field_pool_t;
typedef struct ch_mysql_field_t ch_mysql_field_t;
typedef struct ch_mysql_row_t ch_mysql_row_t;


struct ch_mysql_field_pool_t {

	uint32_t field_num;
	uint32_t extra_num;

	struct list_head field_list;
	struct list_head row_list;
};

struct ch_mysql_field_t {

	struct list_head node;
	
	const char *catalog;
	const char *db;
	const char *table;
	const char *org_table;
	const char *name;
	const char *org_name;
	
	uint16_t charset;
	uint32_t length;
	uint8_t  type;
	uint16_t flags;
	uint8_t decimals;
	
	const char *dft;
};

struct ch_mysql_row_t {
	struct list_head node;
	const char *value;
};

struct ch_mysql_response_t {

	uint8_t response_code;

};

#endif /*CH_MYSQL_RESPONSE_H*/

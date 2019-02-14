/*
 *
 *      Filename: ch_mysql_error.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-10-31 11:24:16
 * Last Modified: 2018-10-31 11:24:16
 */

#ifndef CH_MYSQL_ERROR_H
#define CH_MYSQL_ERROR_H

typedef struct ch_mysql_error_t ch_mysql_error_t;

struct ch_mysql_error_t {

	uint16_t err_code;
	const char *sql_state;
	const char *err_string;
};

#endif /*CH_MYSQL_ERROR_H*/

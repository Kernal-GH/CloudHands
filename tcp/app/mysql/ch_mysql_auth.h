/*
 *
 *      Filename: ch_mysql_auth.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-10-30 18:07:21
 * Last Modified: 2018-10-30 18:07:21
 */

#ifndef CH_MYSQL_AUTH_H
#define CH_MYSQL_AUTH_H

typedef struct ch_mysql_auth_t ch_mysql_auth_t;

struct ch_mysql_auth_t {

	uint8_t proto;

	const char *version;
	
	uint32_t thread_id;
	
	uint16_t server_caps;

	uint8_t server_language;

	uint16_t server_status;
	
	uint16_t server_ext_caps;
	
	uint8_t auth_plugin_len;

	const char *salt;
	const char *auth_plugin;

};


#endif /*CH_MYSQL_AUTH_H*/

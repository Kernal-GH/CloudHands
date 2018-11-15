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
typedef struct ch_mysql_auth_conn_attr_t ch_mysql_auth_conn_attr_t;

/* client/server capabilities
 * Source: http://dev.mysql.com/doc/internals/en/capability-flags.html
 * Source: mysql_com.h
 */
#define MYSQL_CAPS_LP 0x0001 /* CLIENT_LONG_PASSWORD */
#define MYSQL_CAPS_FR 0x0002 /* CLIENT_FOUND_ROWS */
#define MYSQL_CAPS_LF 0x0004 /* CLIENT_LONG_FLAG */
#define MYSQL_CAPS_CD 0x0008 /* CLIENT_CONNECT_WITH_DB */
#define MYSQL_CAPS_NS 0x0010 /* CLIENT_NO_SCHEMA */
#define MYSQL_CAPS_CP 0x0020 /* CLIENT_COMPRESS */
#define MYSQL_CAPS_OB 0x0040 /* CLIENT_ODBC */
#define MYSQL_CAPS_LI 0x0080 /* CLIENT_LOCAL_FILES */
#define MYSQL_CAPS_IS 0x0100 /* CLIENT_IGNORE_SPACE */
#define MYSQL_CAPS_CU 0x0200 /* CLIENT_PROTOCOL_41 */
#define MYSQL_CAPS_IA 0x0400 /* CLIENT_INTERACTIVE */
#define MYSQL_CAPS_SL 0x0800 /* CLIENT_SSL */
#define MYSQL_CAPS_II 0x1000 /* CLIENT_IGNORE_SPACE */
#define MYSQL_CAPS_TA 0x2000 /* CLIENT_TRANSACTIONS */
#define MYSQL_CAPS_RS 0x4000 /* CLIENT_RESERVED */
#define MYSQL_CAPS_SC 0x8000 /* CLIENT_SECURE_CONNECTION */

/* extended capabilities: 4.1+ client only
 *
 * These are libmysqlclient flags and NOT present
 * in the protocol:
 * CLIENT_SSL_VERIFY_SERVER_CERT (1UL << 30)
 * CLIENT_REMEMBER_OPTIONS (1UL << 31)
 */
#define MYSQL_CAPS_MS 0x0001 /* CLIENT_MULTI_STATMENTS */
#define MYSQL_CAPS_MR 0x0002 /* CLIENT_MULTI_RESULTS */
#define MYSQL_CAPS_PM 0x0004 /* CLIENT_PS_MULTI_RESULTS */
#define MYSQL_CAPS_PA 0x0008 /* CLIENT_PLUGIN_AUTH */
#define MYSQL_CAPS_CA 0x0010 /* CLIENT_CONNECT_ATTRS */
#define MYSQL_CAPS_AL 0x0020 /* CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA */
#define MYSQL_CAPS_EP 0x0040 /* CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS */
#define MYSQL_CAPS_ST 0x0080 /* CLIENT_SESSION_TRACK */
#define MYSQL_CAPS_DE 0x0100 /* CLIENT_DEPRECATE_EOF */
#define MYSQL_CAPS_UNUSED 0xFE00

struct ch_mysql_auth_t {

	/*server-->client*/
	uint8_t proto;

	const char *version;
	
	uint32_t thread_id;
	
	uint16_t server_caps;

	uint8_t server_language;

	uint16_t server_status;
	
	uint16_t server_ext_caps;
	
	uint8_t auth_plugin_len;

	const char *salt;
	const char *server_auth_plugin;

	/*client-->server*/
	uint16_t client_caps;
	uint16_t client_ext_caps;
	uint32_t max_packets;
	uint8_t  client_language;
	
	const char *user;
	const char *passwd;
	const char *client_auth_plugin;

	uint32_t conn_attr_num;
	struct list_head conn_attr_list;

};

struct ch_mysql_auth_conn_attr_t {

	struct list_head node;
	const char *key;
	const char *value;
};


#endif /*CH_MYSQL_AUTH_H*/

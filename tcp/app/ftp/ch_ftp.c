/*
 *
 *      Filename: ch_ftp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-19 10:03:29
 * Last Modified: 2018-09-20 11:07:41
 */

#include "ch_ftp.h"
#include "ch_config.h"
#include "ch_log.h"
#include "ch_list.h"


typedef struct private_ftp_context_t private_ftp_context_t;
#define FTP_PORTS_MAX 64

struct private_ftp_context_t {

	const char *fstore_dir;
	int fstore_dir_create_type;

	uint16_t ftp_ports[FTP_PORTS_MAX];

};

static private_ftp_context_t tmp_context,*g_fcontext = &tmp_context;

#include "do_ftp_context.c"
#include "do_ftp_create.c"
#include "do_ftp_format.c"
#include "do_ftp_parse.c"



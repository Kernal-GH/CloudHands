/*
 *
 *      Filename: ch_mail.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 16:21:25
 * Last Modified: 2018-09-25 16:03:38
 */

#include "ch_mail.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_tcp_record.h"
#include "ch_tcp_app_util.h"
#include "ch_log.h"
#include "ch_list.h"
#include "ch_packet_record.h"
#include "smtp_parser.h"
#include "pop3_parser.h"
#include "imap_parser.h"

typedef struct private_mail_context_t private_mail_context_t;

#define MAIL_PORTS_MAX 16

struct private_mail_context_t {

	const char *content_dir; /* store path for mail content */
	const char *attach_dir; /* store path for mail attachment */

	int create_body_dir_type;

	uint16_t smtp_ports[MAIL_PORTS_MAX];
	uint16_t pop3_ports[MAIL_PORTS_MAX];
	uint16_t imap_ports[MAIL_PORTS_MAX];
};

static  private_mail_context_t tmp_context,*g_mcontext = &tmp_context;


#include "do_mail_context.c"
#include "do_mail_create.c"
#include "do_mail_format.c"
#include "do_mail_parse.c"


static ch_tcp_app_t * find_by_port_for_mail(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt){

	private_mail_context_t *mcontext = (private_mail_context_t*)app->context;

    uint16_t *ports;
    
    switch(app->protocol_id){
    
        case PROTOCOL_SMTP:
            ports = mcontext->smtp_ports;
            break;
        
        case PROTOCOL_POP3:
            ports = mcontext->pop3_ports;
            break;
        case PROTOCOL_IMAP:
            ports = mcontext->imap_ports;
            break;

        default:
            return NULL;

    }

    if(ch_ports_equal(ports,MAIL_PORTS_MAX,tcp_pkt->src_port,tcp_pkt->dst_port))
		return app;

	return NULL;
}

static ch_tcp_app_t * find_by_content_for_mail(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){


    return NULL;

}

static void _mail_proto_register(ch_tcp_app_pool_t *ta_pool,private_mail_context_t *mcontext,uint32_t proto_id){

	ch_tcp_app_t  *mail_app = (ch_tcp_app_t*)ch_pcalloc(ta_pool->mp,sizeof(*mail_app));

	mail_app->protocol_id = proto_id;
	mail_app->pkt_rcd_type = PKT_RECORD_TYPE_TCP_MAIL;
	mail_app->context = (void*)mcontext;
    mail_app->find_by_port = find_by_port_for_mail,
    mail_app->find_by_content = find_by_content_for_mail,
	mail_app->proto_session_entry_create = do_mail_session_entry_create;
	mail_app->request_content_parse = do_mail_request_parse;
	mail_app->response_content_parse = do_mail_response_parse;
	mail_app->proto_session_format = do_mail_session_format;
	mail_app->proto_session_entry_clean = do_mail_session_entry_clean;

	ch_tcp_app_register(ta_pool,mail_app);
}

int ch_mail_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

    memset(g_mcontext->smtp_ports,0,MAIL_PORTS_MAX);
    memset(g_mcontext->pop3_ports,0,MAIL_PORTS_MAX);
    memset(g_mcontext->imap_ports,0,MAIL_PORTS_MAX);

	if(do_mail_context_init(ta_pool->mp,g_mcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP mail config file:%s failed!",cfname);
		return -1;
	}

    _mail_proto_register(ta_pool,g_mcontext,PROTOCOL_SMTP);
    _mail_proto_register(ta_pool,g_mcontext,PROTOCOL_POP3);
    _mail_proto_register(ta_pool,g_mcontext,PROTOCOL_IMAP);

	return 0;
}


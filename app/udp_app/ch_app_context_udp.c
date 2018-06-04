/*
 *
 *      Filename: ch_app_context_udp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 18:46:07
 * Last Modified: 2016-10-18 00:34:16
 */

#include "ch_log.h"
#include "ch_config.h"
#include "ch_dns.h"
#include "ch_app_context_udp.h"

static inline void app_context_udp_init(ch_app_context_udp_t *app_udp_context){

    app_udp_context->dns_is_on = 1;
}

static const char *cmd_dns_is_on(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_app_context_udp_t *app_udp_context = (ch_app_context_udp_t*)_dcfg;

    app_udp_context->dns_is_on = 1;

    if(strcasecmp(p1,"off") == 0)
        app_udp_context->dns_is_on = 0;

    return NULL;
}

static const command_rec app_context_udp_directives[] = {

    CH_INIT_TAKE1(
            "CHAPPUDPDNSIsON",
            cmd_dns_is_on,
            NULL,
            0,
            "set dns on/off"
            ),
};

static inline void udp_apps_register(ch_app_context_udp_t *app_udp_context){


    if(app_udp_context->dns_is_on)
        ch_dns_init((ch_app_context_t*)app_udp_context);

}


static inline void dump_udp_app_context(ch_app_context_udp_t *app_udp_context){

    fprintf(stdout,"Dump UDP app context-----------------------------------------\n");
    fprintf(stdout,"DNS is on:%d\n",app_udp_context->dns_is_on);

}

ch_app_context_t * ch_app_context_udp_create(ch_context_t *context){

    const char * msg = NULL;
    ch_app_context_udp_t *app_udp_context = (ch_app_context_udp_t*)apr_palloc(context->mp,sizeof(ch_app_context_udp_t));

    app_context_udp_init(app_udp_context);

    msg = ch_process_command_config(app_context_udp_directives,(void*)app_udp_context,context->mp,context->mp,
            context->app_udp_cfname);

    /*config failed*/
    if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config app udp context error:%s",msg);
        return NULL;
    }

    ch_app_context_init((ch_app_context_t*)app_udp_context,context);

    udp_apps_register(app_udp_context);

    dump_udp_app_context(app_udp_context);

    return (ch_app_context_t*)app_udp_context;

}

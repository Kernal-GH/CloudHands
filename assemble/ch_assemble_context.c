/*
 *
 *      Filename: ch_assemble_context.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-15 11:52:46
 * Last Modified: 2017-12-19 11:20:05
 */

#include "ch_assemble_context.h"
#include "ch_packet.h"
#include "ch_log.h"
#include "ch_config.h"

static void do_acontext_init(ch_assemble_context_t *acontext){

    acontext->mprocess = NULL;
    acontext->mprocess_queue = NULL;

    acontext->log_name = ASSEMBLE_LOG_NAME_DEFAULT;
    acontext->log_level = ASSEMBLE_LOG_LEVEL_DEFAULT;

	acontext->qpool_name = ASSEMBLE_QPOOL_NAME_DEFAULT;
	acontext->qname = ASSEMBLE_QUEUE_NAME_DEFAULT;

}

static const char *cmd_log(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

	ch_assemble_context_t  *acontext = (ch_assemble_context_t *)_dcfg;

	acontext->log_name = p1;

	if(strcasecmp(p2,"info") == 0){
		acontext->log_level = CH_LOG_INFO;
	}else if(strcasecmp(p2,"debug") == 0){
		acontext->log_level = CH_LOG_DEBUG;
	}else if(strcasecmp(p2,"notice") == 0){
		acontext->log_level = CH_LOG_NOTICE;
	}else if(strcasecmp(p2,"warn") == 0){
		acontext->log_level = CH_LOG_WARN;
	}else if(strcasecmp(p2,"error") == 0){
		acontext->log_level = CH_LOG_ERR;
	}else if(strcasecmp(p2,"crit") == 0){
		acontext->log_level = CH_LOG_CRIT;
	}else if(strcasecmp(p2,"alert") == 0){
		acontext->log_level = CH_LOG_ALERT;
	}else if(strcasecmp(p2,"emerg") == 0){
		acontext->log_level = CH_LOG_EMERG;
	}else {

		return "unknown log level name!";
	}

	return NULL;
}

static const command_rec acontext_directives[] ={
    
    CH_INIT_TAKE2(
            "CHLog",
            cmd_log,
            NULL,
            0,
            "set log name and level"
            ),
};

ch_assemble_context_t * ch_assemble_context_create(ch_pool_t *mp,const char *cfname){

    ch_assemble_context_t * acontext = NULL;

	const char *msg;
	
    if(mp == NULL || cfname == NULL||strlen(cfname) == 0){
        return NULL;
    }

    acontext = (ch_assemble_context_t*)ch_pcalloc(mp,sizeof(*acontext));

    acontext->mp = mp;

    do_acontext_init(acontext);

    msg = ch_process_command_config(acontext_directives,(void*)acontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        printf("%s\n",msg);
        return NULL;
    }

    return acontext;

}

int ch_assemble_context_start(ch_assemble_context_t *acontext){


	/*Load th mprocess*/
	acontext->mprocess = ch_mprocess_load(acontext->mp);
	if(acontext->mprocess == NULL){
	
		ch_log(CH_LOG_ERR,"Load mporcess failed!");
		return -1;
	}

	/*Get the mprocess queue to read*/
	acontext->mprocess_queue = ch_mprocess_queue_get(acontext->mprocess,acontext->qpool_name,acontext->qname);
	if(acontext->mprocess_queue == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Get the mprocess queue:%s,%s",acontext->qpool_name,acontext->qname);

		return -1;
	}

	struct rte_mbuf *mbuf;
	ch_packet_t *pkt;

	while(1){
	

		mbuf = ch_mprocess_packet_pop(acontext->mprocess,acontext->mprocess_queue);
		if(mbuf){
	
			pkt = ch_packet_get_from_mbuf(mbuf);

			/*ok*/
			printf("Parse: l2_len:%u,l3_len:%u,l4_len:%u,l3_proto:%u,l4_proto:%u\n",
				pkt->l2_len,
				pkt->l3_len,
				pkt->l4_len,
				pkt->l3_proto,
				pkt->l4_proto);

			rte_pktmbuf_free(mbuf);   

		}
	}

	return 0;
}

void ch_assemble_context_stop(ch_assemble_context_t *acontext){


}

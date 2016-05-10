/*************************************************************************
 > File Name: test_tcp_session.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年05月10日 星期二 13时51分46秒
 ************************************************************************/
#include "ch_constants.h"
#include "ch_test.h"
#include "ch_tcp_session.h"

static ch_tcp_session_t *tsession;

static inline int _test_tcp_session_context_init(){

    /*create all cpu cores,*/
    context->cpool = ch_core_pool_create(context);
    if(context->cpool == NULL){
        fprintf(stderr,"Create cpu cores for context failed!");
        return -1;
    }

    /*create all ethernet ports*/
    context->ppool = NULL;

    /*create all tasks*/
    context->tpool = ch_task_pool_create(context);
    if(context->tpool == NULL){
        fprintf(stderr,"Create tasks for context failed!");
        return -1;
    }

    /*create application context*/
    context->app_context = ch_app_context_create(context);
    if(context->app_context == NULL){
        fprintf(stderr,"Create application context for context failed!");
        return -1;
    }

    ch_http_init(context->app_context);


	/*create tcp session*/
	tsession = ch_tcp_session_create(context);
	if(!tsession){
		fprintf(stderr,"create tcp session failed!\n");
		return -1;
	}

    /*setup all cores that were been bound to tasks*/
    //ch_core_pool_cores_setup(context->cpool);

    /*ok*/
    return 0;

}

/*drop the non syn packet when received first packet!*/
static int _assert_drop_no_syn_packet(void){
	struct tcp_hdr *th;
	struct ipv4_hdr *iph;
	int rc;

	ch_packet_info_t pinfo,*pinfo_ptr = &pinfo;
	uint32_t src_ip = IPv4(192,168,100,10);
	uint32_t dst_ip = IPv4(192,168,100,11);
	struct rte_mbuf *mbuf = alloc_a_mbuf(src_ip,dst_ip,1234,80,"test",4,1,0,0);
    iph = ch_ipv4_hdr_get(mbuf);
	th = ch_tcp_hdr_get(mbuf);
	pinfo_ptr->mbuf = mbuf;
	/*ack packet*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_ACK;
	ch_packet_ipinfo_init(pinfo_ptr,iph);
	ch_packet_tcpinfo_init(pinfo_ptr,th);
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");

	/*fin packet*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_FIN;
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	
	/*rest packet*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_RST;
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	
	/*other non packet!*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_PUSH|CH_TH_URG|CH_TH_ECNECHO|CH_TH_CWR;

	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");

	/*OK*/
	return 0;
}

static int test_tcp_session_packet_process(void){

	if(_assert_drop_no_syn_packet())
		return -1;

	/*test ok!*/
	return 0;
}

static int test_tcp_session(void){

	if(_test_tcp_session_context_init())
		return -1;

	return test_tcp_session_packet_process();

}

static ch_test_command_t tcp_session_cmd = {

	.command = "tcp_session_test",
	.callback = test_tcp_session,
};

CH_REGISTER_TEST_COMMAND(tcp_session_cmd);


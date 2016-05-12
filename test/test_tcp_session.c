/*************************************************************************
 > File Name: test_tcp_session.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年05月10日 星期二 13时51分46秒
 ************************************************************************/
#include "ch_constants.h"
#include "ch_test.h"
#include "ch_tcp_session.h"
#include "ch_session_request_pool.h"
#include "ch_assemble_task.h"
#include "ch_assemble_session.h"

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



static void _alloc_packet(ch_packet_info_t *pinfo_ptr,struct tcp_hdr **th,uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,uint32_t seq,uint32_t seq_ack,const char *data){

	struct tcp_hdr *lth;
	struct ipv4_hdr *iph;
	struct rte_mbuf *mbuf = alloc_a_mbuf(src_ip,dst_ip,src_port,dst_port,(void*)data,data == NULL?0:strlen(data),seq,seq_ack,0);
    iph = ch_ipv4_hdr_get(mbuf);
	lth = ch_tcp_hdr_get(mbuf);
	*th=lth;

	pinfo_ptr->mbuf = mbuf;
	
	/*ack packet*/
	lth->tcp_flags = 0;
	lth->tcp_flags = CH_TH_ACK;
	ch_packet_ipinfo_init(pinfo_ptr,iph);
	ch_packet_tcpinfo_init(pinfo_ptr,lth);
}

static int _assert_syn_state(ch_packet_info_t *pinfo){

	ch_session_request_t *sreq = ch_session_request_find(tsession->req_pool,pinfo);
	CH_TEST_ASSERT_NOT_NULL(sreq,"must  create session request,when recieve first syn packet!");
	CH_TEST_ASSERT_EQUAL(sreq->req_sn_init,pinfo->sent_seq+1,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_ip,pinfo->src_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_port,pinfo->src_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_ip,pinfo->dst_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_port,pinfo->dst_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->three_way_state,THREE_WAY_SYN,"");

	/*ok*/
	return 0;
}
#define ASSERT_SYN_STATE(pinfo) if(_assert_syn_state(pinfo)) return -1

static int _assert_syn_ack_state(ch_packet_info_t *pinfo){

	ch_session_request_t *sreq = ch_session_request_find(tsession->req_pool,pinfo);
	CH_TEST_ASSERT_NOT_NULL(sreq,"session request should been created!");
	CH_TEST_ASSERT_EQUAL(sreq->res_sn_init,pinfo->sent_seq+1,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_ip,pinfo->dst_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_port,pinfo->dst_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_ip,pinfo->src_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_port,pinfo->src_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->three_way_state,THREE_WAY_SYN_ACK,"");

	/*ok*/
	return 0;
}
#define ASSERT_SYN_ACK_STATE(pinfo) if(_assert_syn_ack_state(pinfo))  return -1

static int _assert_ack_ack_state(ch_packet_info_t *pinfo){

	ch_session_request_t *sreq = ch_session_request_find(tsession->req_pool,pinfo);
	CH_TEST_ASSERT_NOT_NULL(sreq,"session request should been created!");
	CH_TEST_ASSERT_EQUAL(sreq->req_ip,pinfo->src_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_port,pinfo->src_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_ip,pinfo->dst_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_port,pinfo->dst_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->three_way_state,THREE_WAY_ACK_ACK,"");

	/*ok*/
	return 0;
}
#define ASSERT_ACK_ACK_STATE(pinfo) if(_assert_ack_ack_state(pinfo))  return -1

/*drop the non syn packet when received first packet!*/
static int _assert_drop_no_syn_packet(void){
	struct tcp_hdr *th;
	int rc;
	ch_session_request_t *sreq;

	ch_packet_info_t pinfo,*pinfo_ptr = &pinfo;
	uint32_t src_ip = IPv4(192,168,100,10);
	uint32_t dst_ip = IPv4(192,168,100,11);
	_alloc_packet(pinfo_ptr,&th,src_ip,dst_ip,1234,80,1,0,NULL);

	/*ack packet*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_ACK;
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	sreq = ch_session_request_find(tsession->req_pool,pinfo_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	/*fin packet*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_FIN;
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	sreq = ch_session_request_find(tsession->req_pool,pinfo_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	
	/*rest packet*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_RST;
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	sreq = ch_session_request_find(tsession->req_pool,pinfo_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	
	/*other non packet!*/
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_PUSH|CH_TH_URG|CH_TH_ECNECHO|CH_TH_CWR;

	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	sreq = ch_session_request_find(tsession->req_pool,pinfo_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");

	/*shoud create a session request*/
	th->tcp_flags = CH_TH_SYN;	
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop  syn packet!");
	ASSERT_SYN_STATE(pinfo_ptr);

	/*OK*/
	return 0;
}
#define ASSERT_DROP_NO_SYN_PACKET() if(_assert_drop_no_syn_packet()) return -1

static int _assert_three_ways_handshake(uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port){

	struct tcp_hdr *th;
	int rc;

	ch_packet_info_t pinfo,*pinfo_ptr = &pinfo;
	_alloc_packet(pinfo_ptr,&th,src_ip,dst_ip,src_port,dst_port,1,0,NULL);
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_SYN;

	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop  syn packet!");
	ASSERT_SYN_STATE(pinfo_ptr);
	
	_alloc_packet(pinfo_ptr,&th,dst_ip,src_ip,dst_port,src_port,1,2,NULL);
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_ACK|CH_TH_SYN;

	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop  syn ack packet!");
	ASSERT_SYN_ACK_STATE(pinfo_ptr);

	_alloc_packet(pinfo_ptr,&th,src_ip,dst_ip,src_port,dst_port,2,2,NULL);
	th->tcp_flags = 0;
	th->tcp_flags = CH_TH_ACK;

	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop syn-ack-ack packet!");
	ASSERT_ACK_ACK_STATE(pinfo_ptr);

	return 0;
}
#define ASSERT_THREE_WAYS_HANDSHAKE(src_ip,dst_ip,src_port,dst_port) if(_assert_three_ways_handshake(src_ip,dst_ip,src_port,dst_port)) return -1

static int _test_assemble_session_find(ch_assemble_session_t **ass,ch_assemble_task_t **astask,
        ch_packet_info_t *pinfo){

    unsigned int i,n;
    ch_assemble_task_t **astsks,*astsk;
    ch_assemble_session_t *as;
    apr_array_header_t *astsk_arr = tsession->context->tpool->as_tsks; 
    *ass = NULL;
    *astask = NULL;

    n = astsk_arr->nelts;
    astsks = (ch_assemble_task_t**)(astsk_arr->elts);
    
    for(i = 0; i<n;i++){

        astsk = astsks[i];
        as = ch_assemble_task_session_find(astsk,pinfo);
        if(as){
            /*Found*/
            *ass = as;
            *astask = astsk;
            return 0;
        }
    }
    /*no found!*/
    return -1;
} 

#define ASSERT_APPEND_DATA(seq,ack) do{\
	_alloc_packet(pinfo_ptr,&th,src_ip,dst_ip,src_port,dst_port,seq,ack,"test");\
	th->tcp_flags = 0;\
	th->tcp_flags = CH_TH_ACK;\
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);\
	_test_assemble_session_find(&as,&astask,pinfo_ptr);\
	sreq = ch_session_request_find(tsession->req_pool,pinfo_ptr);\
	CH_TEST_ASSERT_NULL(sreq,"session request should been freed,if three ways handshake completed!");\
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_OK,"data packet should been added!");\
	CH_TEST_ASSERT_NOT_NULL(as,"Assemble session should been created!");\
}while(0)

#define ASSERT_APPEND_EMPTY_ACK(seq,ack) do{\
	_alloc_packet(pinfo_ptr,&th,src_ip,dst_ip,src_port,dst_port,seq,ack,NULL);\
	th->tcp_flags = 0;\
	th->tcp_flags = CH_TH_ACK;\
	rc = ch_tcp_session_packet_process(tsession,pinfo_ptr);\
	_test_assemble_session_find(&as,&astask,pinfo_ptr);\
	sreq = ch_session_request_find(tsession->req_pool,pinfo_ptr);\
	CH_TEST_ASSERT_NULL(sreq,"session request should been freed,if three ways handshake completed!");\
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"data packet should been droped for empty ack packet!");\
	CH_TEST_ASSERT_NOT_NULL(as,"Assemble session should been created!");\
}while(0)

static int _assert_tcp_session_data_push(void){

	uint32_t src_ip,dst_ip;
	uint16_t src_port,dst_port;

	int rc;
	struct tcp_hdr *th;
	ch_assemble_task_t *astask;
	ch_assemble_session_t *as;
	ch_session_request_t *sreq;

	ch_packet_info_t pinfo,*pinfo_ptr = &pinfo;

	src_ip = IPv4(192,168,100,13);
	dst_ip = IPv4(192,168,100,14);
	src_port = 1234;
	dst_port = 80;

	ASSERT_APPEND_DATA(2,2);
	ASSERT_APPEND_DATA(6,2);
	ASSERT_APPEND_DATA(10,2);

	src_ip = IPv4(192,168,100,14);
	dst_ip = IPv4(192,168,100,13);
	src_port = 80;
	dst_port = 1234;
	ASSERT_APPEND_EMPTY_ACK(2,6);
	ASSERT_APPEND_DATA(2,10);
	ASSERT_APPEND_DATA(2,14);
	
	src_ip = IPv4(192,168,100,13);
	dst_ip = IPv4(192,168,100,14);
	src_port = 1234;
	dst_port = 80;
	ASSERT_APPEND_EMPTY_ACK(10,10);

	src_ip = IPv4(192,168,100,15);
	dst_ip = IPv4(192,168,100,14);
	src_port = 1234;
	dst_port = 80;

	ASSERT_APPEND_DATA(2,2);
	ASSERT_APPEND_DATA(6,2);
	ASSERT_APPEND_DATA(10,2);

	src_ip = IPv4(192,168,100,14);
	dst_ip = IPv4(192,168,100,15);
	src_port = 80;
	dst_port = 1234;
	ASSERT_APPEND_EMPTY_ACK(2,6);
	ASSERT_APPEND_DATA(2,10);
	ASSERT_APPEND_DATA(2,14);
	
	src_ip = IPv4(192,168,100,15);
	dst_ip = IPv4(192,168,100,14);
	src_port = 1234;
	dst_port = 80;
	ASSERT_APPEND_EMPTY_ACK(10,10);
	/*ok*/
	return 0;
}
#define ASSERT_TCP_SESSION_PUSH_DATA() if(_assert_tcp_session_data_push()) return -1

static int test_tcp_session_packet_process(void){

	uint32_t src_ip,dst_ip;

	ASSERT_DROP_NO_SYN_PACKET();

	src_ip = IPv4(192,168,100,13);
	dst_ip = IPv4(192,168,100,14);
	ASSERT_THREE_WAYS_HANDSHAKE(src_ip,dst_ip,1234,80);

	src_ip = IPv4(192,168,100,15);
	dst_ip = IPv4(192,168,100,14);
	ASSERT_THREE_WAYS_HANDSHAKE(src_ip,dst_ip,1234,80);

	ASSERT_TCP_SESSION_PUSH_DATA();

	/*test ok!*/
	return 0;
}

static int test_tcp_session(void){

	if(_test_tcp_session_context_init())
		return -1;

	test_tcp_session_packet_process();

    //ch_core_pool_cores_setup(context->cpool);
}

static ch_test_command_t tcp_session_cmd = {

	.command = "tcp_session_test",
	.callback = test_tcp_session,
};

CH_REGISTER_TEST_COMMAND(tcp_session_cmd);


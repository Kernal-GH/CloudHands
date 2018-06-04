/*************************************************************************
 > File Name: test_assemble_pool_tcp.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年05月10日 星期二 13时51分46秒
 ************************************************************************/
#include "ch_constants.h"
#include "ch_test.h"
#include "ch_assemble_pool_tcp.h"
#include "ch_assemble_packet_tcp.h"
#include "ch_assemble_session_pool_tcp.h"
#include "ch_assemble_session_tcp.h"
#include "ch_assemble_tcp.h"
#include "ch_test_util.h"
#include "ch_dpdk_util.h"

static ch_assemble_pool_tcp_t *aspt = NULL;

static inline int _test_tcp_session_context_init(){

    /*create all cpu cores,*/
    context->cpool = ch_core_pool_create(context);
    if(context->cpool == NULL){
        fprintf(stderr,"Create cpu cores for context failed!");
        return -1;
    }

	/*create tcp session*/
	aspt = (ch_assemble_pool_tcp_t*)ch_assemble_pool_tcp_create(context);

	if(!aspt){
		fprintf(stderr,"create tcp assemble pool failed!\n");
		return -1;
	}

    /*ok*/
    return 0;

}

static void _alloc_packet(ch_assemble_packet_tcp_t *tcp_pkt_ptr,struct tcp_hdr **th,
        uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,uint32_t seq,uint32_t seq_ack,const char *data){

	struct tcp_hdr *lth;
	struct ipv4_hdr *iph;
	struct rte_mbuf *mbuf = alloc_a_mbuf(src_ip,dst_ip,src_port,dst_port,(void*)data,data == NULL?0:strlen(data),seq,seq_ack,0);
    iph = ch_ipv4_hdr_get(mbuf);
	lth = ch_tcp_hdr_get(mbuf);
	*th=lth;

	/*ack packet*/
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK;

    ch_assemble_packet_tcp_init(tcp_pkt_ptr,mbuf);
}

static int _assert_syn_state(ch_assemble_packet_tcp_t *tcp_pkt){

    ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)tcp_pkt;

	ch_tcp_session_request_t *sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt);
	CH_TEST_ASSERT_NOT_NULL(sreq,"must  create session request,when recieve first syn packet!");
	CH_TEST_ASSERT_EQUAL(sreq->req_sn_init,tcp_pkt->sent_seq+1,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_ip,as_pkt->src_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_port,as_pkt->src_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_ip,as_pkt->dst_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_port,as_pkt->dst_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->three_way_state,THREE_WAY_SYN,"");

	/*ok*/
	return CH_TEST_SUCCESS;
}
#define ASSERT_SYN_STATE(tcp_pkt) if(CH_TEST_SUCCESS!=_assert_syn_state(tcp_pkt)) return CH_TEST_FAILED

static int _assert_syn_ack_state(ch_assemble_packet_tcp_t *tcp_pkt){

    ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)tcp_pkt;
	ch_tcp_session_request_t *sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt);
	CH_TEST_ASSERT_NOT_NULL(sreq,"session request should been created!");
	CH_TEST_ASSERT_EQUAL(sreq->res_sn_init,tcp_pkt->sent_seq+1,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_ip,as_pkt->dst_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_port,as_pkt->dst_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_ip,as_pkt->src_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_port,as_pkt->src_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->three_way_state,THREE_WAY_SYN_ACK,"");

	/*ok*/
	return CH_TEST_SUCCESS;
}
#define ASSERT_SYN_ACK_STATE(tcp_pkt) if(CH_TEST_SUCCESS!=_assert_syn_ack_state(tcp_pkt))  return CH_TEST_FAILED

static int _assert_ack_ack_state(ch_assemble_packet_tcp_t *tcp_pkt){

    ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)tcp_pkt;
	ch_tcp_session_request_t *sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt);
	CH_TEST_ASSERT_NOT_NULL(sreq,"session request should been created!");
	CH_TEST_ASSERT_EQUAL(sreq->req_ip,as_pkt->src_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->req_port,as_pkt->src_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_ip,as_pkt->dst_ip,"");
	CH_TEST_ASSERT_EQUAL(sreq->res_port,as_pkt->dst_port,"");
	CH_TEST_ASSERT_EQUAL(sreq->three_way_state,THREE_WAY_ACK_ACK,"");

	/*ok*/
	return CH_TEST_SUCCESS;
}
#define ASSERT_ACK_ACK_STATE(tcp_pkt) if(CH_TEST_SUCCESS!=_assert_ack_ack_state(tcp_pkt))  return CH_TEST_FAILED

/*drop the non syn packet when received first packet!*/
static int _assert_drop_no_syn_packet(void){
	struct tcp_hdr *th;
	int rc;
	ch_tcp_session_request_t *sreq;

	ch_assemble_packet_tcp_t tcp_pkt,*tcp_pkt_ptr = &tcp_pkt;
	uint32_t src_ip = IPv4(192,168,100,10);
	uint32_t dst_ip = IPv4(192,168,100,11);
	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,1234,80,1,0,NULL);

	/*ack packet*/
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK;
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	/*fin packet*/
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_FIN;
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	
	/*rest packet*/
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_RST;
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");
	
	/*other non packet!*/
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_PUSH|CH_TH_URG|CH_TH_ECNECHO|CH_TH_CWR;

	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_NULL(sreq,"must not create session request,is not first syn packet!");
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop non syn packet for not exist session!");

	/*shoud create a session request*/
	tcp_pkt_ptr->tcp_flags = CH_TH_SYN;	
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop  syn packet!");
	ASSERT_SYN_STATE(tcp_pkt_ptr);

	/*OK*/
	return CH_TEST_SUCCESS;
}
#define ASSERT_DROP_NO_SYN_PACKET() if(CH_TEST_SUCCESS!=_assert_drop_no_syn_packet()) return CH_TEST_FAILED

static int _assert_three_ways_handshake(uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port){

	struct tcp_hdr *th;
	int rc;

	ch_assemble_packet_tcp_t tcp_pkt,*tcp_pkt_ptr = &tcp_pkt;
	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,src_port,dst_port,1,0,NULL);
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_SYN;

	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop  syn packet!");
	ASSERT_SYN_STATE(tcp_pkt_ptr);
	
	_alloc_packet(tcp_pkt_ptr,&th,dst_ip,src_ip,dst_port,src_port,1,2,NULL);
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK|CH_TH_SYN;

	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop  syn ack packet!");
	ASSERT_SYN_ACK_STATE(tcp_pkt_ptr);

	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,src_port,dst_port,2,2,NULL);
	tcp_pkt_ptr->tcp_flags = 0;
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK;

	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"must drop syn-ack-ack packet!");
	ASSERT_ACK_ACK_STATE(tcp_pkt_ptr);

	return CH_TEST_SUCCESS;
}
#define ASSERT_THREE_WAYS_HANDSHAKE(src_ip,dst_ip,src_port,dst_port) if(CH_TEST_SUCCESS!=_assert_three_ways_handshake(src_ip,dst_ip,src_port,dst_port)) return CH_TEST_FAILED

static int _test_assemble_session_find(ch_assemble_session_tcp_t **ass,ch_assemble_task_t **astask,
        ch_assemble_packet_tcp_t *tcp_pkt){

    unsigned int i,n;
    ch_assemble_task_t **astsks,*astsk;
    ch_assemble_session_tcp_t *as;
    ch_assemble_tcp_t *tas;
    apr_array_header_t *astsk_arr =aspt->asp.assemble_tasks; 
    *ass = NULL;
    *astask = NULL;
    
    n = astsk_arr->nelts;
    astsks = (ch_assemble_task_t**)(astsk_arr->elts);
    
    for(i = 0; i<n;i++){

        astsk = astsks[i];
        tas = (ch_assemble_tcp_t*)astsk->as;
        as = ch_assemble_session_pool_tcp_entry_find(tas->asspt,(ch_assemble_packet_t*)tcp_pkt);
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
	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,src_port,dst_port,seq,ack,"test");\
	tcp_pkt_ptr->tcp_flags = 0;\
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK;\
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	_test_assemble_session_find(&as,&astask,tcp_pkt_ptr);\
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	CH_TEST_ASSERT_NULL(sreq,"session request should been freed,if three ways handshake completed!");\
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_OK,"data packet should been added!");\
	CH_TEST_ASSERT_NOT_NULL(as,"Assemble session should been created!");\
}while(0)

#define ASSERT_APPEND_EMPTY_ACK(seq,ack) do{\
	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,src_port,dst_port,seq,ack,NULL);\
	tcp_pkt_ptr->tcp_flags = 0;\
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK;\
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	_test_assemble_session_find(&as,&astask,tcp_pkt_ptr);\
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	CH_TEST_ASSERT_NULL(sreq,"session request should been freed,if three ways handshake completed!");\
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_DROP,"data packet should been droped for empty ack packet!");\
	CH_TEST_ASSERT_NOT_NULL(as,"Assemble session should been created!");\
}while(0)

#define ASSERT_APPEND_FIN_ACK(seq,ack) do{\
	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,src_port,dst_port,seq,ack,NULL);\
	tcp_pkt_ptr->tcp_flags = 0;\
	tcp_pkt_ptr->tcp_flags = CH_TH_ACK;\
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	_test_assemble_session_find(&as,&astask,tcp_pkt_ptr);\
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	CH_TEST_ASSERT_NULL(sreq,"session request should been freed,if three ways handshake completed!");\
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_OK,"data FIN ACK packet should been NOT droped!");\
	CH_TEST_ASSERT_NOT_NULL(as,"Assemble session should been created!");\
}while(0)

#define ASSERT_APPEND_FIN(seq,ack) do{\
	_alloc_packet(tcp_pkt_ptr,&th,src_ip,dst_ip,src_port,dst_port,seq,ack,NULL);\
	tcp_pkt_ptr->tcp_flags = 0;\
	tcp_pkt_ptr->tcp_flags = CH_TH_FIN;\
	rc = ch_assemble_pool_packet_process((ch_assemble_pool_t*)aspt,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	_test_assemble_session_find(&as,&astask,tcp_pkt_ptr);\
	sreq = ch_tcp_session_request_find(aspt->tsr_pool,(ch_assemble_packet_t*)tcp_pkt_ptr);\
	CH_TEST_ASSERT_NULL(sreq,"session request should been freed,if three ways handshake completed!");\
	CH_TEST_ASSERT_EQUAL(rc,PROCESSOR_RET_OK,"data packet should been not been drop close packet!");\
	CH_TEST_ASSERT_NOT_NULL(as,"Assemble session should been created!");\
}while(0)

static int _assert_tcp_session_data_push(void){

	uint32_t src_ip,dst_ip;
	uint16_t src_port,dst_port;

	int rc;
	struct tcp_hdr *th;
	ch_assemble_task_t *astask;
	ch_assemble_session_tcp_t *as;
	ch_tcp_session_request_t *sreq;

	ch_assemble_packet_tcp_t tcp_pkt,*tcp_pkt_ptr = &tcp_pkt;

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
	return CH_TEST_SUCCESS;
}
#define ASSERT_TCP_SESSION_PUSH_DATA() if(CH_TEST_SUCCESS!=_assert_tcp_session_data_push()) return CH_TEST_FAILED


static int test_tcp_close_packet(void){

	uint32_t src_ip,dst_ip;
	uint16_t src_port,dst_port;

	int rc;
	struct tcp_hdr *th;
	ch_assemble_task_t *astask;
	ch_assemble_session_tcp_t *as;
	ch_tcp_session_request_t *sreq;

	ch_assemble_packet_tcp_t tcp_pkt,*tcp_pkt_ptr = &tcp_pkt;

	src_port = 1234;
	dst_port = 80;

	src_ip = IPv4(192,168,100,101);
	dst_ip = IPv4(192,168,100,102);
	ASSERT_THREE_WAYS_HANDSHAKE(src_ip,dst_ip,src_port,dst_port);

	ASSERT_APPEND_DATA(2,2);
	ASSERT_APPEND_DATA(6,2);
	ASSERT_APPEND_DATA(10,2);

    ASSERT_APPEND_FIN(14,2);
	src_port = 80;
	dst_port = 1234;

	src_ip = IPv4(192,168,100,102);
	dst_ip = IPv4(192,168,100,101);
    ASSERT_APPEND_FIN_ACK(2,15);
    ASSERT_APPEND_FIN(2,15);
    ASSERT_APPEND_FIN_ACK(16,2);

	/*ok*/
	return CH_TEST_SUCCESS;
}
#define ASSERT_TCP_CLOSE() if(test_tcp_close_packet()!=CH_TEST_SUCCESS) return CH_TEST_FAILED

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

    ASSERT_TCP_CLOSE();

	/*test ok!*/
	return CH_TEST_SUCCESS;
}

static int test_tcp_session(void){

	if(_test_tcp_session_context_init())
		return -1;

	test_tcp_session_packet_process();

   ch_core_pool_cores_setup(context->cpool);
}

static ch_test_command_t tcp_session_cmd = {

	.command = "tcp_session_test",
	.callback = test_tcp_session,
};

CH_REGISTER_TEST_COMMAND(tcp_session_cmd);


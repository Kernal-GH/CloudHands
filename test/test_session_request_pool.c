/*
 * =====================================================================================
 *
 *       Filename:  test_session_request_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月23日 00时14分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_test.h"
#include "ch_session_request_pool.h"

static ch_session_request_pool_t *req_pool = NULL;

static inline void init_packet_info(ch_packet_info_t *pinfo,
        uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port){

    pinfo->time = 0;
    pinfo->mbuf = NULL;
    pinfo->port = NULL;

    pinfo->src_ip = src_ip;
    pinfo->dst_ip = dst_ip;
    pinfo->src_port = src_port;
    pinfo->dst_port = dst_port;

    pinfo->sent_seq = 0;
    pinfo->recv_ack = 0;
    pinfo->tth_len = 0;
    pinfo->payload_len = 0;
}

static inline void init_session_request(ch_session_request_t *req,ch_packet_info_t *pinfo){
	
    req->req_ip = pinfo->src_ip;
    req->req_port = pinfo->src_port;
    req->res_ip = pinfo->dst_ip;
    req->res_port = pinfo->dst_port;
}

static int test_session_request_pool(void){

    ch_packet_info_t pinfo,*pinfo_ptr = &pinfo;
    ch_session_request_t *sreq,*sreq1;

    req_pool = ch_session_request_pool_create(context);
    CH_TEST_ASSERT_NOT_NULL(req_pool,"create request pool failed!");

    init_packet_info(pinfo_ptr,4026531834,4026531835,1234,80);
    sreq = ch_session_request_create(req_pool,pinfo_ptr);
    init_session_request(sreq,pinfo_ptr);
    CH_TEST_ASSERT_NOT_NULL(sreq,"Create session request failed!");

    sreq1 = ch_session_request_find(req_pool,pinfo_ptr);
    CH_TEST_ASSERT_NOT_NULL(sreq1,"session request find assert failed!");

    CH_TEST_ASSERT(sreq==sreq1,"session request create&&find assert failed!");

    /*response*/
    init_packet_info(pinfo_ptr,4026531835,4026531834,80,1234);
    sreq1 = ch_session_request_find(req_pool,pinfo_ptr);
    CH_TEST_ASSERT_NOT_NULL(sreq1,"session request find assert failed!");
    CH_TEST_ASSERT(sreq==sreq1,"session request create&&find assert failed!");

    ch_session_request_free(req_pool,sreq);
    sreq1 = ch_session_request_find(req_pool,pinfo_ptr);
    CH_TEST_ASSERT_NULL(sreq1,"free a session request,find should null!");

    sreq1 = sreq;
    sreq = ch_session_request_create(req_pool,pinfo_ptr);
    CH_TEST_ASSERT_NOT_NULL(sreq,"Create session request failed!");
    CH_TEST_ASSERT(sreq==sreq1,"Create session request from free list firstly!");

    return 0;
}

static ch_test_command_t session_request_cmd = {
    .command = "session_request_pool_test",
    .callback = test_session_request_pool,
};

CH_REGISTER_TEST_COMMAND(session_request_cmd);


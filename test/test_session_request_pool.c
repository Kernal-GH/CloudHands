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

static ch_context_t ctx,*ctx_ptr = &ctx;
static ch_session_request_pool_t *req_pool = NULL;

static int test_session_request_pool(void){

    ctx_ptr->n_session_requests_limit = 100;
    ctx_ptr->n_assemble_tasks = 0;

    req_pool = ch_session_request_pool_create(ctx_ptr);
    CH_TEST_ASSERT_NOT_NULL(req_pool,"create request pool failed!");

    return 0;
}

static ch_test_command_t session_request_cmd = {
    .command = "session_request_pool_test",
    .callback = test_session_request_pool,
};

CH_REGISTER_TEST_COMMAND(session_request_cmd);


/*
 * =====================================================================================
 *
 *       Filename:  test_assemble_fragment.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月21日 15时58分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdint.h>
#include "ch_test.h"
#include "ch_log.h"
#include "ch_assemble_fragment.h"

ch_assemble_fragment_t as_frag,*as_frag_ptr = &as_frag;

static void assemble_fragment_init(void){

    ch_assemble_fragment_init(as_frag_ptr);
}

static int test_assemble_fragment_push(){

}

static int test_assemble_fragment(void){

    assemble_fragment_init();

    return 0;
}

static ch_test_command_t as_frag_cmd = {
    
    .command = "as_frag_test",
    .callback = test_assemble_fragment,
};

CH_REGISTER_TEST_COMMAND(as_frag_cmd);





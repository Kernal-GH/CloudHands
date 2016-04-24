/*
 * =====================================================================================
 *
 *       Filename:  ch_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月21日 13时01分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_test.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <ctype.h>
#include <sys/queue.h>
#include <apr_pools.h>
#include "ch_errno.h"
#include "ch_log.h"
#include "ch_context.h"
#include "ch_constants.h"
#include "ch_task.h"

static int is_cmd_init = 0; 

ch_context_t *context = NULL;

void ch_add_test_command(ch_test_command_t *t){

    if(is_cmd_init == 0){

        is_cmd_init = 1;
        CH_RING_INIT(&test_commands_list,ch_test_command_t,link);
    }

    CH_RING_INSERT_TAIL(&test_commands_list,t,ch_test_command_t,link);
}

int ch_unit_test_suite_runner(ch_unit_test_suite_t *suite)
{
	int retval, i = 0;

	if (suite->suite_name)
		printf("Test Suite : %s\n", suite->suite_name);

	if (suite->setup)
		if (suite->setup() != 0)
			return -1;

	while (suite->unit_test_cases[i].testcase) {
		/* Run test case setup */
		if (suite->unit_test_cases[i].setup) {
			retval = suite->unit_test_cases[i].setup();
			if (retval != 0)
				return retval;
		}

		/* Run test case */
		if (suite->unit_test_cases[i].testcase() == 0) {
			printf("TestCase %2d: %s\n", i,
					suite->unit_test_cases[i].success_msg ?
					suite->unit_test_cases[i].success_msg :
					"passed");
		}
		else {
			printf("TestCase %2d: %s\n", i, suite->unit_test_cases[i].fail_msg ?
					suite->unit_test_cases[i].fail_msg :
					"failed");
			return -1;
		}

		/* Run test case teardown */
		if (suite->unit_test_cases[i].teardown) {
			retval = suite->unit_test_cases[i].teardown();
			if (retval != 0)
				return retval;
		}

		i++;
	}

	/* Run test suite teardown */
	if (suite->teardown)
		if (suite->teardown() != 0)
			return -1;

	return 0;
}

static void _run_cmd(ch_test_command_t *cmd){

    if(cmd->callback()==0){
        printf("Test [%s] OK!\n",cmd->command);
    }else{ 
        printf("Test [%s] Failed!\n",cmd->command);
    }
    fflush(stdout);
}

static void _run_cmd_by_name(const char *name){

    ch_test_command_t *cmd;
    
    CH_RING_FOREACH(cmd,&test_commands_list,ch_test_command_t,link){

        if(strcmp(cmd->command,name)==0){

            _run_cmd(cmd);
            return;
        }
    }

    printf("Command [%s] not existed!\n",name);
    fflush(stdout);
}

static void _run_all_cmds(void){
    
    ch_test_command_t *cmd;
    
    CH_RING_FOREACH(cmd,&test_commands_list,ch_test_command_t,link){
        
        _run_cmd(cmd);
    }
}

int main(int argc, const char * const argv[]){

	int ret;
    const char *cfname;
    const char *test_cmd_name = NULL;

    apr_pool_t *mp;

	ret = rte_eal_init(argc, (char**)argv);
	if (ret < 0)
		return -1;

    argc -= ret;
    argv += ret;

    if(argc<=1){
        fprintf(stderr,"Usage:app [test_cmd_name] <conf path>\n");
        exit(-1);
    }

    apr_app_initialize(&argc,&argv,NULL);
    
    apr_pool_create(&mp,NULL);

    if(mp == NULL){
        fprintf(stderr,"create a memory pool for testing failed!\n");
        exit(-1);
    }

    if(argc == 2){
        cfname = argv[1];
    }else{
        cfname = argv[2];
        test_cmd_name = argv[1];
    }

    context = ch_context_create(mp,cfname);
    if(context == NULL){
        
        fprintf(stderr,"create context failed from config file%s\n",cfname);
        exit(-1);
    }
    
    ch_strerror_init();
    ch_log_init(mp,context->log_name,context->log_level);

    if(test_cmd_name){
        
       _run_cmd_by_name(test_cmd_name);
    }else{
        _run_all_cmds();
    }

    return 0;
}

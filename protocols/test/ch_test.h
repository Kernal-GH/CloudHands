/*
 * =====================================================================================
 *
 *       Filename:  ch_test.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月21日 12时45分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_TEST_H
#define CH_TEST_H

typedef struct ch_unit_test_suite_t ch_unit_test_suite_t;
typedef struct ch_unit_test_case_t ch_unit_test_case_t;
typedef struct ch_test_command_t ch_test_command_t;

#include <sys/queue.h>
#include "ch_ring.h"
#include "ch_parser_context.h"

extern ch_parser_context_t *pcontext;

#define CH_TEST_SUCCESS  (0)
#define CH_TEST_FAILED  (-1)

/* Before including test.h file you can define
 * CH_TEST_TRACE_FAILURE(_file, _line, _func) macro to better trace/debug test
 * failures. Mostly useful in test development phase. */
#ifndef CH_TEST_TRACE_FAILURE
# define CH_TEST_TRACE_FAILURE(_file, _line, _func)
#endif

#define CH_TEST_ASSERT(cond, msg, ...) do {                         \
		if (!(cond)) {                                           \
			printf("TestCase %s() line %d failed: "              \
				msg "\n", __func__, __LINE__, ##__VA_ARGS__);    \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

#define CH_TEST_ASSERT_EQUAL(a, b, msg, ...) do {                   \
		if (!(a == b)) {                                         \
			printf("TestCase %s() line %d failed: "              \
				msg "\n", __func__, __LINE__, ##__VA_ARGS__);    \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

#define CH_TEST_ASSERT_NOT_EQUAL(a, b, msg, ...) do {               \
		if (!(a != b)) {                                         \
			printf("TestCase %s() line %d failed: "              \
				msg "\n", __func__, __LINE__, ##__VA_ARGS__);    \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

#define CH_TEST_ASSERT_SUCCESS(val, msg, ...) do {                  \
		typeof(val) _val = (val);                                \
		if (!(_val == 0)) {                                      \
			printf("TestCase %s() line %d failed (err %d): "     \
				msg "\n", __func__, __LINE__, _val,              \
				##__VA_ARGS__);                                  \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

#define CH_TEST_ASSERT_FAIL(val, msg, ...) do {                     \
		if (!(val != 0)) {                                       \
			printf("TestCase %s() line %d failed: "              \
				msg "\n", __func__, __LINE__, ##__VA_ARGS__);    \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

#define CH_TEST_ASSERT_NULL(val, msg, ...) do {                     \
		if (!(val == NULL)) {                                    \
			printf("TestCase %s() line %d failed: "              \
				msg "\n", __func__, __LINE__, ##__VA_ARGS__);    \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

#define CH_TEST_ASSERT_NOT_NULL(val, msg, ...) do {                 \
		if (!(val != NULL)) {                                    \
			printf("TestCase %s() line %d failed: "              \
				msg "\n", __func__, __LINE__, ##__VA_ARGS__);    \
			CH_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);    \
			return CH_TEST_FAILED;                                  \
		}                                                        \
} while (0)

struct ch_unit_test_case_t {
	int (*setup)(void);
	int (*teardown)(void);
	int (*testcase)(void);
	const char *success_msg;
	const char *fail_msg;
};

#define CH_TEST_CASE(fn) { NULL, NULL, fn, #fn " succeeded", #fn " failed"}

#define CH_TEST_CASE_NAMED(name, fn) { NULL, NULL, fn, name " succeeded", \
		name " failed"}

#define CH_TEST_CASE_ST(setup, teardown, testcase)         \
		{ setup, teardown, testcase, #testcase " succeeded",    \
		#testcase " failed "}

#define CH_TEST_CASES_END() { NULL, NULL, NULL, NULL, NULL }

struct ch_unit_test_suite_t {
	const char *suite_name;
	int (*setup)(void);
	int (*teardown)(void);
	struct ch_unit_test_case_t unit_test_cases[];
};

extern int ch_unit_test_suite_runner(ch_unit_test_suite_t *suite);

typedef int (test_callback)(void);

CH_RING_ENTRY(ch_test_command_t) test_commands_list;

struct ch_test_command_t {
	CH_RING_ENTRY(ch_test_command_t) link;
	const char *command;
	test_callback *callback;
};

extern void ch_add_test_command(ch_test_command_t *t);

#define CH_REGISTER_TEST_COMMAND(t) \
static void __attribute__((used)) testfn_##t(void);\
void __attribute__((constructor, used)) testfn_##t(void)\
{\
	ch_add_test_command(&t);\
}


#endif /*CH_TEST_H*/


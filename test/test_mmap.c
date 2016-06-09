/*************************************************************************
 > File Name: test_mmap.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月09日 星期四 08时47分25秒
 ************************************************************************/

#include "ch_test.h"
#include "ch_mmap.h"

#define FNAME "/tmp/test_mmap_data"
static uint64_t FSIZE = (uint64_t)4*1024*1024*1024;

static uint64_t ENTRY_SIZE = (uint64_t)32*1024*1024;

static int test_mmap_create(void){

	ch_mmap_t *mm_r = NULL,*mm_w = NULL;
	
	/*reader cannot create a mmap*/
	mm_r = ch_mmap_create(context->mp,FNAME,FSIZE,ENTRY_SIZE,0);
	CH_TEST_ASSERT_NULL(mm_r,"Reader cannot create a mmap!");

	mm_w = ch_mmap_create(context->mp,FNAME,FSIZE,ENTRY_SIZE,1);
	mm_r = ch_mmap_create(context->mp,FNAME,FSIZE,ENTRY_SIZE,0);

	ch_mmap_header_t *mmh_w = mm_w->mmap_header;
	ch_mmap_header_t *mmh_r = mm_r->mmap_header;

	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_start,mmh_r->mmap_entries_start,"mmap entries start should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_count,mmh_r->mmap_entries_count,"mmap entries count should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entry_size,mmh_r->mmap_entry_size,"mmap entry size should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_write_entry_pos,mmh_r->mmap_write_entry_pos,"mmap write entry pos should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_read_entry_pos,mmh_r->mmap_read_entry_pos,"mmap read entry pos should equal!");

    mmh_w->mmap_write_entry_pos += mmh_w->mmap_entry_size;
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_start,mmh_r->mmap_entries_start,"mmap entries start should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_count,mmh_r->mmap_entries_count,"mmap entries count should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entry_size,mmh_r->mmap_entry_size,"mmap entry size should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_write_entry_pos,mmh_r->mmap_write_entry_pos,"mmap write entry pos should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_read_entry_pos,mmh_r->mmap_read_entry_pos,"mmap read entry pos should equal!");

    mmh_w->mmap_read_entry_pos += mmh_w->mmap_entry_size;
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_start,mmh_r->mmap_entries_start,"mmap entries start should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_count,mmh_r->mmap_entries_count,"mmap entries count should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entry_size,mmh_r->mmap_entry_size,"mmap entry size should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_write_entry_pos,mmh_r->mmap_write_entry_pos,"mmap write entry pos should equal!");
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_read_entry_pos,mmh_r->mmap_read_entry_pos,"mmap read entry pos should equal!");
	return CH_TEST_SUCCESS;
}

static int test_mmap(void){

	int rc;
	if((rc=test_mmap_create())!=CH_TEST_SUCCESS)
		return rc;

	return CH_TEST_SUCCESS;
}

static ch_test_command_t test_mmap_cmd = {
    .command = "mmap_test",
    .callback = test_mmap,
};

CH_REGISTER_TEST_COMMAND(test_mmap_cmd);


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

#define ASSERT_MMAP_HEADERS(mmh_w,mmh_r) do{\
	CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_start,mmh_r->mmap_entries_start,"mmap entries start should equal!");\
    CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entries_count,mmh_r->mmap_entries_count,"mmap entries count should equal!");\
    CH_TEST_ASSERT_EQUAL(mmh_w->mmap_entry_size,mmh_r->mmap_entry_size,"mmap entry size should equal!");\
    CH_TEST_ASSERT_EQUAL(mmh_w->mmap_write_entry_pos,mmh_r->mmap_write_entry_pos,"mmap write entry pos should equal!");\
    CH_TEST_ASSERT_EQUAL(mmh_w->mmap_read_entry_pos,mmh_r->mmap_read_entry_pos,"mmap read entry pos should equal!");\
}while(0)

#define ASSERT_TEST_MMAP(fn,mm_r,mm_w) if(fn(mm_r,mm_w)!=CH_TEST_SUCCESS) return CH_TEST_FAILED

static int test_mmap_create(ch_mmap_t *mm_r,ch_mmap_t *mm_w){



	ch_mmap_header_t *mmh_w = mm_w->mmap_header;
	ch_mmap_header_t *mmh_r = mm_r->mmap_header;
    ASSERT_MMAP_HEADERS(mmh_w,mmh_r);

    mmh_w->mmap_write_entry_pos += 1;
    ASSERT_MMAP_HEADERS(mmh_w,mmh_r);


    mmh_w->mmap_read_entry_pos += 1;
    ASSERT_MMAP_HEADERS(mmh_w,mmh_r);

    mmh_w->mmap_write_entry_pos = 0;
    mmh_w->mmap_read_entry_pos = 0;
	return CH_TEST_SUCCESS;
}

#define ASSERT_A_MMAP_BUF(a,b) do{\
    CH_TEST_ASSERT_EQUAL(a,b->mm,"mmap instance not equal mmb->mm");\
    CH_TEST_ASSERT_EQUAL((uint64_t)(b->end-b->start),a->mmap_header->mmap_entry_size,"buf size not equal entry size!");\
}while(0)

static int test_mmap_buf_get(ch_mmap_t *mm_r,ch_mmap_t *mm_w){

    int rc;
    ch_mmap_buf_t mmb_w = {NULL,0,NULL,NULL,NULL},*mmb_w_ptr = &mmb_w;
    ch_mmap_buf_t mmb_r = {NULL,0,NULL,NULL,NULL},*mmb_r_ptr = &mmb_r;

    rc = ch_mmap_buf_get(mm_r,mmb_r_ptr);
    CH_TEST_ASSERT_EQUAL(rc,-1,"Empty mmap,has no buf can been read!");

    /*get a buf to write*/
    rc = ch_mmap_buf_get(mm_w,mmb_w_ptr);
    CH_TEST_ASSERT_EQUAL(rc,0,"Has free mmap buf should been get!");
    ASSERT_A_MMAP_BUF(mm_w,mmb_w_ptr);
    memcpy(mmb_w_ptr->start,"test",4);

    rc = ch_mmap_buf_get(mm_r,mmb_r_ptr);
    CH_TEST_ASSERT_EQUAL(rc,-1,"Empty mmap,has no buf can been read!");
    
    ch_mmap_buf_commit(mm_w,mmb_w_ptr);
    rc = ch_mmap_buf_get(mm_r,mmb_r_ptr);
    CH_TEST_ASSERT_EQUAL(rc,0,"Not empty mmap,should can been read!");
    ASSERT_A_MMAP_BUF(mm_r,mmb_r_ptr);
    CH_TEST_ASSERT_EQUAL(memcmp(mmb_r_ptr->start,"test",4),0,"Read content is invalid data!");
    ch_mmap_buf_commit(mm_r,mmb_r_ptr);
    rc = ch_mmap_buf_get(mm_r,mmb_r_ptr);
    CH_TEST_ASSERT_EQUAL(rc,-1,"Empty mmap,has no buf can been read!");

    /*write all free space*/
    for(;;){
        rc = ch_mmap_buf_get(mm_w,mmb_w_ptr);
        if(rc == -1){

            break;
        }else{
            ASSERT_A_MMAP_BUF(mm_w,mmb_w_ptr); 
            ch_mmap_buf_commit(mm_w,mmb_w_ptr);
        }

    }
    
    CH_TEST_ASSERT_EQUAL(mm_w->mmap_header->mmap_entries_count,mm_w->mmap_header->mmap_entries_count_cur,"Full state error");
    rc = ch_mmap_buf_get(mm_r,mmb_r_ptr);
    CH_TEST_ASSERT_EQUAL(rc,0,"Not empty mmap,should can been read!");
    ch_mmap_buf_commit(mm_r,mmb_r_ptr);
    /*now should can been written one buf*/
    rc = ch_mmap_buf_get(mm_w,mmb_w_ptr);
    CH_TEST_ASSERT_EQUAL(rc,0,"Has free mmap buf should been get!");
    ASSERT_A_MMAP_BUF(mm_w,mmb_w_ptr);
    ch_mmap_buf_commit(mm_w,mmb_w_ptr);
    rc = ch_mmap_buf_get(mm_w,mmb_w_ptr);
    CH_TEST_ASSERT_EQUAL(rc,-1,"Has no free mmap buf should been not get!");

    /*read all buf*/
    uint64_t c = 0;
    for(;;){
        rc = ch_mmap_buf_get(mm_r,mmb_r_ptr);
        if(rc == -1){

            break;
        }else{
            ASSERT_A_MMAP_BUF(mm_r,mmb_r_ptr); 
            ch_mmap_buf_commit(mm_r,mmb_r_ptr);
            c++;
        }
    
    }
    CH_TEST_ASSERT_EQUAL(c,mm_w->mmap_header->mmap_entries_count,"Read state error!");
    CH_TEST_ASSERT_EQUAL(mm_w->mmap_header->mmap_entries_count_cur,0,"Read state error!");

    return CH_TEST_SUCCESS;
}


static int test_mmap(void){

    ch_mmap_t *mm_r,*mm_w;

	/*reader cannot create a mmap*/
	mm_r = ch_mmap_create(context->mp,FNAME,FSIZE,ENTRY_SIZE,0,0);
	CH_TEST_ASSERT_NULL(mm_r,"Reader cannot create a mmap!");

	mm_w = ch_mmap_create(context->mp,FNAME,FSIZE,ENTRY_SIZE,0,1);
	mm_r = ch_mmap_create(context->mp,FNAME,FSIZE,ENTRY_SIZE,0,0);

    ASSERT_TEST_MMAP(test_mmap_create,mm_r,mm_w);
    ASSERT_TEST_MMAP(test_mmap_buf_get,mm_r,mm_w);

    ch_mmap_destroy(mm_w);

	return CH_TEST_SUCCESS;
}

static ch_test_command_t test_mmap_cmd = {
    .command = "mmap_test",
    .callback = test_mmap,
};

CH_REGISTER_TEST_COMMAND(test_mmap_cmd);


/*************************************************************************
 > File Name: test_mmap_file_format.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月16日 星期四 16时47分25秒
 ************************************************************************/
#include "ch_test.h"
#include "ch_mmap_file_format.h"

#define TFNAME "/tmp/mmap_file_format_data"
#define TFSIZE (4*1024*1024*1024L)
#define TENTRY_SIZE 64*1024

static ch_mmap_file_entry_t fentry = {
    .magic = EIMAGIC,
    .entry_size = CH_MMAP_FILE_ENTRY_HEADER_LEN+11,
    .packet_type = 12,
    .session_direct = 1,
    .session_id = 1234567,
    .time = 1466067011,
    .src_ip = 3232238262L, 
    .dst_ip = 3232238265L,
    .src_port = 1234,
    .dst_port = 80,
    .data_offset = 0,
    .data_len = 11,
    .data = (void*)"shajianfeng",
};
static ch_mmap_file_entry_t *fentry_ptr = &fentry;
static size_t one_buf_entries_number = 0;

static inline int assert_one_file_entry(ch_mmap_file_entry_t *fe){

    CH_TEST_ASSERT_EQUAL(fe->magic,fentry_ptr->magic,"fentry magic not equal!");
    CH_TEST_ASSERT_EQUAL(fe->entry_size,fentry_ptr->entry_size,"fentry entry_size not equal!");
    CH_TEST_ASSERT_EQUAL(fe->packet_type,fentry_ptr->packet_type,"fentry packet_type not equal!");
    CH_TEST_ASSERT_EQUAL(fe->session_direct,fentry_ptr->session_direct,"fentry session_direct not equal!");
    CH_TEST_ASSERT_EQUAL(fe->session_id,fentry_ptr->session_id,"fentry session_id not equal!");
    CH_TEST_ASSERT_EQUAL(fe->time,fentry_ptr->time,"fentry time not equal!");
    CH_TEST_ASSERT_EQUAL(fe->src_ip,fentry_ptr->src_ip,"fentry src_ip not equal!");
    CH_TEST_ASSERT_EQUAL(fe->dst_ip,fentry_ptr->dst_ip,"fentry dst_ip not equal!");
    CH_TEST_ASSERT_EQUAL(fe->src_port,fentry_ptr->src_port,"fentry src_port not equal!");
    CH_TEST_ASSERT_EQUAL(fe->dst_port,fentry_ptr->dst_port,"fentry dst_port not equal!");
    CH_TEST_ASSERT_EQUAL(fe->data_offset,fentry_ptr->data_offset,"fentry data_offset not equal!");
    CH_TEST_ASSERT_EQUAL(fe->data_len,fentry_ptr->data_len,"fentry data_len not equal!");
    CH_TEST_ASSERT_EQUAL(memcmp(fe->data,fentry_ptr->data,fe->data_len),0,"fentry data not equal!");

    return CH_TEST_SUCCESS;
}

static size_t  _write_n_mmap_bufs(ch_mmap_file_format_t *mmfmt,uint64_t n){

    int rc;
    size_t c = 0;

    for(;;){

        rc = ch_mmap_file_format_put(mmfmt,fentry_ptr);
        if(rc == -1|| mmfmt->cur_fheader.mmb.entry_index == n)
            break;

        c++;
    }

    return c;
}

static int _read_n_mmap_bufs(ch_mmap_file_format_t *mmfmt,uint64_t n,uint64_t tn){

    ch_mmap_file_iterator_t *iter;
    ch_mmap_file_entry_t *fe;
    size_t c = 0;
    uint64_t last_index = 0;


    for(;;){
        iter = ch_mmap_file_format_iterator_prefare(mmfmt);
        if(iter == NULL)
            break;
        while((fe = iter->next(iter))){

            if(CH_TEST_SUCCESS!=assert_one_file_entry(fe))
                return CH_TEST_FAILED;
            c++;
        }
        last_index = mmfmt->cur_fheader.mmb.entry_index;

        ch_mmap_file_format_iterator_commit(mmfmt,iter);
    }

    //CH_TEST_ASSERT_EQUAL(last_index,n-1,"The Buf's index is  invalid!");
    printf("Assert:index-->%lu,w_number:%lu,r_number:%lu\n",n,tn,c);

    return CH_TEST_SUCCESS;
}

#define ASSERT_READ_MMAP_BUFS(mmfmt,mmfmt_w,n,tn) if(_read_n_mmap_bufs(mmfmt,n,tn)!=CH_TEST_SUCCESS) do{ch_mmap_file_format_destroy(mmfmt_w); return CH_TEST_FAILED;}while(0)

static int test_mmap_file_format(void){

    size_t tn = 0;

    ch_mmap_file_format_t *mmfmt_w,*mmfmt_r;

    mmfmt_w = ch_mmap_file_format_create(context->mp,TFNAME,TFSIZE,TENTRY_SIZE,1);
    mmfmt_r = ch_mmap_file_format_create(context->mp,TFNAME,TFSIZE,TENTRY_SIZE,0);


    tn = _write_n_mmap_bufs(mmfmt_w,1);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,1,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");

    tn = _write_n_mmap_bufs(mmfmt_w,2);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,2,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");

    tn = _write_n_mmap_bufs(mmfmt_w,10);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,10,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");

    /*write all*/
    tn = _write_n_mmap_bufs(mmfmt_w,0);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,mmfmt_w->cur_fheader.mmb.entry_index,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");
    
    tn = _write_n_mmap_bufs(mmfmt_w,1);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,1,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");

    tn = _write_n_mmap_bufs(mmfmt_w,2);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,2,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");

    tn = _write_n_mmap_bufs(mmfmt_w,10);
    ASSERT_READ_MMAP_BUFS(mmfmt_r,mmfmt_w,10,tn);
    CH_TEST_ASSERT_NULL(ch_mmap_file_format_iterator_prefare(mmfmt_r),"Should no buf can be read!");
    ch_mmap_file_format_destroy(mmfmt_w);

    return CH_TEST_SUCCESS;
}

static ch_test_command_t test_mmap_file_format_cmd = {

    .command = "mmap_file_format_test",
    .callback = test_mmap_file_format,
};

CH_REGISTER_TEST_COMMAND(test_mmap_file_format_cmd);


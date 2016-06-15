
/*************************************************************************
 > File Name: test_bin_format.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月15日 星期三 15时47分25秒
 ************************************************************************/
#include "ch_test.h"
#include "ch_bin_format.h"

static int test_bin_format(void){

    ch_bin_format_t bfs,*bf_fmt = &bfs,*bf=&bfs;
    void *start = malloc(1024);
    ch_bf_init(bf_fmt,start,start+1024);
    ssize_t rc;
    size_t tlen = 0;

    /*test write*/
    rc = ch_bf_bool_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,1,"Write bool ,return size error!");

    rc = ch_bf_bool_write(bf_fmt,0);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,1,"Write bool ,return size error!");

    rc = ch_bf_short_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(short),"Write short ,return size error!");
    rc = ch_bf_short_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_short_write(bf_fmt,-1);
    tlen+=rc;
    
    rc = ch_bf_int_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int),"Write int ,return size error!");
    rc = ch_bf_int_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_int_write(bf_fmt,-1);
    tlen+=rc;
 
    rc = ch_bf_long_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(long),"Write Long ,return size error!");
    rc = ch_bf_long_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_long_write(bf_fmt,-1);
    tlen+=rc;

    rc = ch_bf_ushort_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(unsigned short),"Write ushort ,return size error!");
    rc = ch_bf_ushort_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_uint_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(unsigned int),"Write uint ,return size error!");
    rc = ch_bf_uint_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_ulong_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(unsigned long),"Write ulong ,return size error!");
    rc = ch_bf_ulong_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_size_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(size_t),"Write size_t ,return size error!");
    rc = ch_bf_size_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_off_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(off_t),"Write off_t ,return size error!");
    rc = ch_bf_off_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_ssize_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(ssize_t),"Write ssize_t ,return size error!");
    rc = ch_bf_ssize_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_ssize_write(bf_fmt,-1);
    tlen+=rc;

    rc = ch_bf_int8_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int8_t),"Write int8 ,return size error!");
    rc = ch_bf_int8_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_int8_write(bf_fmt,-1);
    tlen+=rc;
   

    rc = ch_bf_int16_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int16_t),"Write int16 ,return size error!");
    rc = ch_bf_int16_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_int16_write(bf_fmt,-1);
    tlen+=rc;

    rc = ch_bf_int32_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int32_t),"Write int32 ,return size error!");
    rc = ch_bf_int32_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_int32_write(bf_fmt,-1);
    tlen+=rc;

    rc = ch_bf_int64_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int64_t),"Write int64 ,return size error!");
    rc = ch_bf_int64_write(bf_fmt,0);
    tlen+=rc;
    rc = ch_bf_int64_write(bf_fmt,-1);
    tlen+=rc;
    
    rc = ch_bf_uint8_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(uint8_t),"Write uint8 ,return size error!");
    rc = ch_bf_uint8_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_uint16_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(uint16_t),"Write uint16 ,return size error!");
    rc = ch_bf_uint16_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_uint32_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(uint32_t),"Write uint32 ,return size error!");
    rc = ch_bf_uint32_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_uint64_write(bf_fmt,1);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(uint64_t),"Write uint64 ,return size error!");
    rc = ch_bf_uint64_write(bf_fmt,0);
    tlen+=rc;

    rc = ch_bf_float_write(bf_fmt,3.14);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int),"Write float ,return size error!");
    
    rc = ch_bf_float_write(bf_fmt,-3.14);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int),"Write float ,return size error!");
    
    rc = ch_bf_double_write(bf_fmt,0.314);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int64_t),"Write double ,return size error!");
    
    rc = ch_bf_double_write(bf_fmt,-0.314);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,sizeof(int64_t),"Write double ,return size error!");

    rc = ch_bf_bytes_write(bf_fmt,"shajf",5);
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,5+sizeof(size_t),"Write bytes ,return size error!");

    rc = ch_bf_string_write(bf_fmt,"shajf");
    tlen+=rc;
    CH_TEST_ASSERT_EQUAL(rc,5+sizeof(size_t),"Write String ,return size error!");


    CH_TEST_ASSERT_EQUAL(tlen,(size_t)(bf_fmt->pos-bf_fmt->start),"Write end size 's state error");

    /*test read*/
    ch_bf_reset(bf_fmt);
    CH_TEST_ASSERT_EQUAL(ch_bf_bool_read(bf),1,"Bool Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_bool_read(bf),0,"Bool Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_short_read(bf),1,"Short Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_short_read(bf),0,"Short Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_short_read(bf),-1,"Short Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int_read(bf),1,"Int Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int_read(bf),0,"Int Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int_read(bf),-1,"Int Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_long_read(bf),1,"Long Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_long_read(bf),0,"Long Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_long_read(bf),-1,"Long Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ushort_read(bf),1,"Ushort Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ushort_read(bf),0,"Ushort Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_uint_read(bf),1,"UInt Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_uint_read(bf),0,"UInt Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ulong_read(bf),1,"ULong Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ulong_read(bf),0,"ULong Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_size_read(bf),1,"Size Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_size_read(bf),0,"Size Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_off_read(bf),1,"off Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_off_read(bf),0,"off Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ssize_read(bf),1,"SSize Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ssize_read(bf),0,"SSize Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_ssize_read(bf),-1,"SSize Read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_int8_read(bf),1,"int8 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int8_read(bf),0,"int8 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int8_read(bf),-1,"int8 Read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_int16_read(bf),1,"int16 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int16_read(bf),0,"int16 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int16_read(bf),-1,"int16 Read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_int32_read(bf),1,"int32 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int32_read(bf),0,"int32 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int32_read(bf),-1,"int32 Read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_int64_read(bf),1,"int64 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int64_read(bf),0,"int64 Read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_int64_read(bf),-1,"int64 Read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_uint8_read(bf),1,"uint8 read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_uint8_read(bf),0,"uint8 read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_uint16_read(bf),1,"uint16 read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_uint16_read(bf),0,"uint16 read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_uint32_read(bf),1,"uint32 read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_uint32_read(bf),0,"uint32 read error!");

    CH_TEST_ASSERT_EQUAL(ch_bf_uint64_read(bf),1,"uint64 read error!");
    CH_TEST_ASSERT_EQUAL(ch_bf_uint64_read(bf),0,"uint64 read error!");

    float ff = ch_bf_float_read(bf);
    printf("%f\n",ff);

    ff = ch_bf_float_read(bf);
    printf("%f\n",ff);

    double dd = ch_bf_double_read(bf);
    printf("%f\n",dd);
    dd = ch_bf_double_read(bf);
    printf("%f\n",dd);

    unsigned char *data;
    size_t len;

    ch_bf_bytes_read(bf,&data,&len);
    CH_TEST_ASSERT_EQUAL(memcmp(data,"shajf",5),0,"Read bytes error!");
    CH_TEST_ASSERT_EQUAL(len,5,"Read bytes error!");

    ch_bf_bytes_read(bf,&data,&len);
    CH_TEST_ASSERT_EQUAL(memcmp(data,"shajf",5),0,"Read bytes error!");
    CH_TEST_ASSERT_EQUAL(len,5,"Read bytes error!");

    free(start);

    return CH_TEST_SUCCESS;
}

static ch_test_command_t test_bin_format_cmd = {

    .command = "bin_format_test",
    .callback = test_bin_format,
};

CH_REGISTER_TEST_COMMAND(test_bin_format_cmd);

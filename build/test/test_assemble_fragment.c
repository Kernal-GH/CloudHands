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

typedef struct {
    const char * data;
    size_t dlen;
    size_t offset;
}test_data_fragment_t;

static inline void test_data_fragment_init(test_data_fragment_t *tdf,const char *df,size_t offset){

    tdf->data = df;
    tdf->dlen = strlen(df);
    tdf->offset = offset;
}

static void assemble_fragment_init(void){

    ch_assemble_fragment_init(as_frag_ptr);
}

static int test_assemble_fragment_push(void){

    char data[128]={0};

    size_t df_count,dfs_len;
    ch_data_fragment_t *df;

    test_data_fragment_t tdf1,tdf2,tdf3,tdf4,tdf5,tdf6;

    test_data_fragment_init(&tdf1,"My name is  shajf,",0);
    test_data_fragment_init(&tdf2,"I have three boies,",tdf1.offset+tdf1.dlen);
    test_data_fragment_init(&tdf3,"I love my wife,",tdf2.offset+tdf2.dlen);
    test_data_fragment_init(&tdf4,"I love my family!",tdf3.offset+tdf3.dlen);
    test_data_fragment_init(&tdf5,"I love my father!",tdf4.offset+tdf4.dlen);
    test_data_fragment_init(&tdf6,"I love my mather!",tdf5.offset+tdf5.dlen);

    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);

    CH_TEST_ASSERT(df_count==0&&dfs_len==0,
            "Init state error,data fragments' count[%lu] is not zero or data fragments' size [%lu] is not zero!",
            df_count,dfs_len);

    CH_TEST_ASSERT(_is_assemble_fragment_empty(as_frag_ptr) == 1,"Init state error,data fragments' list is not empty!");

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);

    CH_TEST_ASSERT(df_count == 1&&dfs_len == tdf1.dlen,
            "Push a data fragment:Data fragment's count[%lu,%lu],size[%lu,%lu]",1,df_count,tdf1.dlen,dfs_len);

    df = ch_assemble_fragment_pop(as_frag_ptr,0);

    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);  
    
    CH_TEST_ASSERT(df!=NULL,"Push one order data fragment,Pop should not be NULL!");
    CH_TEST_ASSERT(df->len == tdf1.dlen&&df->offset==0,"Push one order data fragment,Pop data fragment's member error!");
    CH_TEST_ASSERT(df_count == 0&&dfs_len == 0,"After pop,data fragment and data fragments' size should be zero!");
    CH_TEST_ASSERT(strcmp(df->data,tdf1.data)==0,"After Push one order data fragment,the data has been changed!");
    ch_assemble_data_fragment_free(df);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count == 1&&dfs_len == tdf2.dlen,
            "After push a data fragment[count=[%lu,%lu,must eq],size=[%lu,%lu must eq]",1,df_count,tdf2.dlen,dfs_len);

    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    CH_TEST_ASSERT(df==NULL,"Push one unorder data fragment,Pop should be NULL!");

    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count == 1&&dfs_len == tdf2.dlen,
            "After pop an unorder, a data fragment[count=[%lu,%lu,must eq],size=[%lu,%lu must eq]",1,df_count,tdf2.dlen,dfs_len);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf3.data,tdf3.dlen,tdf3.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len); 
    CH_TEST_ASSERT(df_count==1&&(dfs_len==tdf2.dlen+tdf3.dlen),"Two continue data fragments,should be merged into one!");
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    CH_TEST_ASSERT(df==NULL,"Push two unorder data fragments,Pop should be NULL!");

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset); 
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==1&&(dfs_len==tdf1.dlen+tdf2.dlen+tdf3.dlen),"Three continue data fragments,should be merged into one!");
    
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    CH_TEST_ASSERT(df!=NULL,"Push Three order data fragment,Pop should not be NULL!");
    CH_TEST_ASSERT(df->offset==0&&(df->len==tdf1.dlen+tdf2.dlen+tdf3.dlen),"Three continue data fragments length error!");

    strcat(data,tdf1.data);
    strcat(data,tdf2.data);
    strcat(data,tdf3.data);

    CH_TEST_ASSERT(strcmp(df->data,data)==0,"After Push three order data fragment,the data has been changed!");
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==0&&dfs_len==0,"Three continue data fragments has been pop!");
    ch_assemble_data_fragment_free(df);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf4.data,tdf4.dlen,tdf4.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf6.data,tdf6.dlen,tdf6.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==3&&dfs_len==(tdf2.dlen+tdf4.dlen+tdf6.dlen),"Three unorder data fragments push error!");
    
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf5.data,tdf5.dlen,tdf5.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==2&&dfs_len==(tdf2.dlen+tdf4.dlen+tdf6.dlen+tdf5.dlen),"[df2,df4~df6]!");

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf3.data,tdf3.dlen,tdf3.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==1&&dfs_len==(tdf2.dlen+tdf4.dlen+tdf6.dlen+tdf5.dlen+tdf3.dlen),"[df2~df6]!");

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==1&&dfs_len==(tdf1.dlen+tdf2.dlen+tdf3.dlen+tdf4.dlen+tdf6.dlen+tdf5.dlen),"[df1~df6]!");

    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df&&df_count==0&&dfs_len==0&&df->len==(tdf1.dlen+tdf2.dlen+tdf3.dlen+tdf4.dlen+tdf6.dlen+tdf5.dlen),"Six continue data fragments has been pop!");
    
    memset(data,0,128);
    strcat(data,tdf1.data);
    strcat(data,tdf2.data);
    strcat(data,tdf3.data);
    strcat(data,tdf4.data);
    strcat(data,tdf5.data);
    strcat(data,tdf6.data);

    CH_TEST_ASSERT(strncmp(data,df->data,df->len)==0,"The Data has been changed!");
    ch_assemble_data_fragment_free(df);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==1&&dfs_len==tdf2.dlen,"push df2,df2 error!");

    memset(data,0,128);
    strcat(data,tdf1.data);
    strcat(data,"ffff");
    ch_assemble_fragment_push(as_frag_ptr,(void*)data,strlen(data),0);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==1&&dfs_len==(tdf1.dlen+tdf2.dlen),"Two order data has been pushed,and some data is the same!");
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df&&df_count==0&&dfs_len==0&&df->len==(tdf1.dlen+tdf2.dlen),"Two order data has been poped!");
    ch_assemble_data_fragment_free(df);

    memset(data,0,128);
    strcat(data,tdf1.data);
    strcat(data,tdf2.data);
    CH_TEST_ASSERT(strncmp(data,df->data,df->len)==0,"The Data has been changed!");
    
    memset(data,0,128);
    strcat(data,tdf1.data);
    strcat(data,tdf2.data);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)data,strlen(data),0);
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    CH_TEST_ASSERT(df&&df_count==0&&dfs_len==0&&df->len==(tdf1.dlen+tdf2.dlen),"Two order data has been poped!");
    CH_TEST_ASSERT(strncmp(data,df->data,df->len)==0,"The Data has been changed!");
    ch_assemble_data_fragment_free(df);

    const char *tmp_data = "my name is shageeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";

    memset(data,0,128);
    strcat(data,tdf1.data);
    strcat(data,tmp_data);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)data,strlen(data),0);
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    CH_TEST_ASSERT(df&&df_count==0&&dfs_len==0&&df->len==strlen(data),"Two order data has been poped!");
    CH_TEST_ASSERT(strncmp(data,df->data,df->len)==0,"The Data has been changed!");
    ch_assemble_data_fragment_free(df);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf4.data,tdf4.dlen,tdf4.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf6.data,tdf6.dlen,tdf6.offset);
    
    tmp_data = "aafffdddkkklllmmmLLLpppG";
    ch_assemble_fragment_push(as_frag_ptr,(void*)tmp_data,strlen(tmp_data),16);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==3&&dfs_len==(tdf4.dlen+tdf6.dlen+strlen(tmp_data)+tdf1.dlen-2),"df2 was covered!");
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==2&&dfs_len==(tdf4.dlen+tdf6.dlen),"after pop,should have df4 and df6!");
    
    CH_TEST_ASSERT((strncmp(df->data,"My name is  shajaafffdddkkklllmmmLLLpppG",df->len)==0),"pop data fragment error!");
    ch_assemble_data_fragment_free(df);  

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);

    tmp_data = "aaaaaaaaabbbbbbbbbcccccccccdddddddddfffffffffeeeeeeeeeggggggggghhhhhhhhh";
    ch_assemble_fragment_push(as_frag_ptr,(void*)tmp_data,strlen(tmp_data),16);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==1&&dfs_len==(tdf1.dlen+strlen(tmp_data)+tdf4.dlen-4),"only one data fragment!");
    df = ch_assemble_fragment_pop(as_frag_ptr,0);
    ch_assemble_fragment_info_get(as_frag_ptr,&df_count,&dfs_len);
    CH_TEST_ASSERT(df_count==0&&dfs_len==0&&df,"after pop,should no data fragments!");    
    CH_TEST_ASSERT((strncmp(df->data,
                    "My name is  shajf,aaaaaaabbbbbbbbbcccccccccdddddddddfffffffffeeeeeeeeeggggggggghhhhhhhI love my mather!",df->len)==0),"pop data fragment error!");
   
    ch_assemble_data_fragment_free(df);  

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);
    df = ch_assemble_fragment_pop(as_frag_ptr,2);
    CH_TEST_ASSERT(df&&(strncmp(df->data,tdf1.data+2,df->len)==0),"pop failed!");
    ch_assemble_data_fragment_free(df);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);
    df = ch_assemble_fragment_pop(as_frag_ptr,tdf1.dlen);
    CH_TEST_ASSERT(df==NULL,"pop failed!");
   
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf1.data,tdf1.dlen,tdf1.offset);
    df = ch_assemble_fragment_pop(as_frag_ptr,tdf1.dlen+1);
    CH_TEST_ASSERT(df==NULL,"pop failed!");
    ch_assemble_data_fragment_free(df);

    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf2.data,tdf2.dlen,tdf2.offset);
    ch_assemble_fragment_push(as_frag_ptr,(void*)tdf4.data,tdf4.dlen,tdf4.offset);

    df = ch_assemble_fragment_pop(as_frag_ptr,tdf3.offset+tdf3.dlen);
    CH_TEST_ASSERT(df&&(strncmp(df->data,tdf4.data,df->len)==0),"pop failed");
    ch_assemble_data_fragment_free(df);
    
    return 0;
}

static int test_assemble_fragment(void){

    assemble_fragment_init();

    return test_assemble_fragment_push();
}

static ch_test_command_t as_frag_cmd = {
    
    .command = "as_frag_test",
    .callback = test_assemble_fragment,
};

CH_REGISTER_TEST_COMMAND(as_frag_cmd);





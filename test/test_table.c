/*
 * =====================================================================================
 *
 *       Filename:  test_table.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月4日 17时25分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include "ch_test.h"
#include "ch_table.h"
#include "ch_jhash.h"
#include "ch_assemble_packet.h"

typedef struct _table_entry_test_t _table_entry_test_t;

#define TEST_N_ENTRIES_LIMIT 10
#define TEST_TBL_SIZE 8
#define TEST_ENTRY_TIMEOUT 5
#define TEST_N_CACHES_LIMIT 4


static ch_table_t *as_tbl;

struct _table_entry_test_t {

    ch_table_entry_t entry;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
};

static uint32_t _entry_hash(void *key,void *priv_data){

	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)key;

    return ch_jhash_4words_sort(as_pkt->src_ip,as_pkt->dst_ip,
            (uint32_t)(as_pkt->src_port),(uint32_t)(as_pkt->dst_port),0);
}

static int _entry_equal(ch_table_entry_t *entry,void *key,void *priv_data){


	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)key;
    _table_entry_test_t *tentry = (_table_entry_test_t*)entry;
    int rc1 = 0;
    int rc2 = 0;

    rc1 = (as_pkt->src_ip == tentry->src_ip && as_pkt->src_port == tentry->src_port)&&\
          (as_pkt->dst_ip == tentry->dst_ip && as_pkt->dst_port == tentry->dst_port);

    rc2 = (as_pkt->src_ip == tentry->dst_ip && as_pkt->src_port == tentry->dst_port)&&\
          (as_pkt->dst_ip == tentry->src_ip && as_pkt->dst_port == tentry->src_port);

    return rc1||rc2;
}

static void _entry_clean(ch_table_entry_t *entry,void *priv_data){

    fprintf(stdout,"entry clean timeout:\n");
}

static inline size_t  get_list_count(struct ch_list_head *h,int is_cache){

    size_t n = 0;
    ch_table_entry_t *entry;
    
    if(is_cache){
        
        list_for_each_entry(entry,h,lru){
        
            n++;
        }
    }else{
    
        list_for_each_entry(entry,h,entry){
        
            n++;
        }
    }
    return n;
}

static inline size_t get_tbl_list_count(void){

    uint32_t i;
    size_t n = 0;
    struct ch_list_head *h;

    for(i = 0; i<as_tbl->tbl_size;i++){
    
        h  =  as_tbl->entries+i;
        n+= get_list_count(h,0);
    }

    return n;
}

static inline int test_tbl_stats(uint32_t create_num,
        uint32_t free_num,
        uint32_t hits_num,
        uint32_t no_hits_num,
        uint32_t fail_num,
        uint32_t timeout_num){


    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_stats.create_num,create_num,"tbl_stats.create_num value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_stats.free_num,free_num,"tbl_stats.free_num value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_stats.hits_num,hits_num,"tbl_stats.hits_num value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_stats.no_hits_num,no_hits_num,"tbl_stats.no_hits_num value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_stats.fail_num,fail_num,"tbl_stats.fail_num value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_stats.timeout_num,timeout_num,"tbl_stats.timeout_num value is invalid!");

    return CH_TEST_SUCCESS;
}


static int test_table_create(void){

    apr_pool_t *mp;
    
    apr_pool_create(&mp,NULL);

    as_tbl = ch_table_create(mp,sizeof(_table_entry_test_t),
            TEST_N_ENTRIES_LIMIT,
            TEST_TBL_SIZE,
            TEST_ENTRY_TIMEOUT,
            TEST_N_CACHES_LIMIT,
            NULL,
            _entry_hash,_entry_equal,_entry_clean,NULL);

    CH_TEST_ASSERT_EQUAL(as_tbl->mp,mp," table's mempool is invalid!");
    CH_TEST_ASSERT_NULL(as_tbl->last_entry,"as_tbl->last_entry should been NULL,when table created!");
    CH_TEST_ASSERT_EQUAL(as_tbl->entry_timeout,TEST_ENTRY_TIMEOUT,"as_tbl->entry_timeout's value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->entry_size,sizeof(_table_entry_test_t),"as_tbl->entry_size value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,0,"as_tbl->n_entries value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries_limit,TEST_N_ENTRIES_LIMIT,"as_tbl->n_entries_limit value is invalid!");

    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_size,TEST_TBL_SIZE,"as_tbl->tbl_size value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->tbl_mask,TEST_TBL_SIZE-1,"as_tbl->tbl_mask value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,0,"as_tbl->n_caches value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches_limits,TEST_N_CACHES_LIMIT,"as_tbl->n_caches_limits value is invalid!");

    CH_TEST_ASSERT_EQUAL(as_tbl->entry_hash,_entry_hash,"as_tbl->entry_hash value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->entry_equal,_entry_equal,"as_tbl->entry_equal value is invalid!");
    CH_TEST_ASSERT_EQUAL(as_tbl->entry_clean,_entry_clean,"as_tbl->entry_clean value is invalid!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(0,0,0,0,0,0))
        return CH_TEST_FAILED;

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid!");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,0),0,"as_tbl->caches value is invalid!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),0,"as_tbl->entries value is invalid!");

    return CH_TEST_SUCCESS;
}

static inline void _packet_init(ch_assemble_packet_t *as_pkt,
        uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port){

   
    as_pkt->src_ip = src_ip;
    as_pkt->dst_ip = dst_ip;
    as_pkt->src_port = src_port;
    as_pkt->dst_port = dst_port;

}

static inline void _entry_init(_table_entry_test_t *entry,ch_assemble_packet_t *as_pkt){

    entry->src_ip = as_pkt->src_ip;
    entry->dst_ip = as_pkt->dst_ip;
    entry->src_port = as_pkt->src_port;
    entry->dst_port = as_pkt->dst_port;
}

static int test_table_entry_create(void){

    _table_entry_test_t *entry,*entry1;
    ch_assemble_packet_t as_pkt1,as_pkt2,as_pkt3;
    int i;

    _packet_init(&as_pkt1,
            IPv4(192,168,100,10),
            IPv4(192,168,100,11),
            1234,80);

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt1);
    _entry_init(entry,&as_pkt1);
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,1," table numbers of entries are not be incr!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry," talbe's last entry has not been update after create");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,1," table numbers of caches are not be incr!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after create !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),1,"as_tbl->caches value is invalid after create!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),1,"as_tbl->entries value is invalid after create!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(1,0,0,0,0,0))
        return CH_TEST_FAILED;

    _packet_init(&as_pkt2,
            IPv4(192,168,100,10),
            IPv4(192,168,100,12),
            1235,80);

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt2);
    _entry_init(entry,&as_pkt2);
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,2," table numbers of entries are not be incr!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry," talbe's last entry has not been update after create");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,2," table numbers of caches are not be incr!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after create !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),2,"as_tbl->caches value is invalid after create!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),2,"as_tbl->entries value is invalid after create!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(2,0,0,0,0,0))
        return CH_TEST_FAILED;

    ch_table_entry_free(as_tbl,(ch_table_entry_t*)entry);
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,2," table numbers of entries are invalid after free!");
    CH_TEST_ASSERT_NULL(as_tbl->last_entry," talbe's last entry has not been update after free");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,1," table numbers of caches are not be update after free!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),1,"as_tbl->free_entries value is invalid after create !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),1,"as_tbl->caches value is invalid after free!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),1,"as_tbl->entries value is invalid after free!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(2,1,0,0,0,0))
        return CH_TEST_FAILED;

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt2);
    _entry_init(entry,&as_pkt2);
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,2," table numbers of entries are invalid after free&create!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry," talbe's last entry has not been update after free&create");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,2," table numbers of caches are invalid after free&create!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after free&create !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),2,"as_tbl->caches value is invalid after free&create!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),2,"as_tbl->entries value is invalid after free&create!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(2,1,0,0,0,0))
        return CH_TEST_FAILED;

   /*create all*/
    for(i = 2; i<TEST_N_ENTRIES_LIMIT; i++){
    
    
        _packet_init(&as_pkt3,
            IPv4(192,168,100,10),
            IPv4(192,168,100,12+i+1),
            1235+i+1,80);
    
        entry1 = (_table_entry_test_t*)ch_table_entry_create(as_tbl,&as_pkt3);
    
        _entry_init(entry1,&as_pkt3);
    }
    
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,TEST_N_ENTRIES_LIMIT," table numbers of entries are invalid after create all!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1," talbe's last entry has not been update after create all");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,TEST_N_CACHES_LIMIT," table numbers of caches are invalid after create all!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after create all !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),TEST_N_CACHES_LIMIT,"as_tbl->caches value is invalid after create all!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),TEST_N_ENTRIES_LIMIT,"as_tbl->entries value is invalid after create all!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_ENTRIES_LIMIT,1,0,0,0,0))
        return CH_TEST_FAILED;
    
    entry1 = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt3);

    CH_TEST_ASSERT_NULL(entry1," table has no empty space to create entry!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_ENTRIES_LIMIT,1,0,0,1,0))
        return CH_TEST_FAILED;
    
    /*test timeout free entries*/
    fprintf(stdout,"test timeout free entries----------------------------------\n");
    sleep(TEST_ENTRY_TIMEOUT*2);
    entry1 = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt3);
    CH_TEST_ASSERT_NOT_NULL(entry1," table should can be get a free entry after timeout!");

    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,TEST_N_ENTRIES_LIMIT," table numbers of entries are invalid after timeout!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,1," table numbers of caches are invalid after timeout!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),TEST_N_ENTRIES_LIMIT-1,"as_tbl->free_entries value is invalid after timeout !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),1,"as_tbl->caches value is invalid after timeout!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),1,"as_tbl->entries value is invalid after timeout!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_ENTRIES_LIMIT,1,0,0,1,TEST_N_ENTRIES_LIMIT))
        return CH_TEST_FAILED;

    return CH_TEST_SUCCESS;
}

static int test_table_entry_find(void){

    _table_entry_test_t *entry,*entry1;
    ch_assemble_packet_t as_pkt1,as_pkt2,as_pkt3;
    int i;
    
    apr_pool_destroy(as_tbl->mp);
    if(CH_TEST_SUCCESS != test_table_create())
        return CH_TEST_FAILED;
    
    _packet_init(&as_pkt1,
            IPv4(192,168,100,10),
            IPv4(192,168,100,11),
            1234,80);

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt1);
    _entry_init(entry,&as_pkt1);

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,1," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),1,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),1,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(1,0,1,0,0,0))
        return CH_TEST_FAILED;

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,1," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),1,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),1,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(1,0,2,0,0,0))
        return CH_TEST_FAILED;

    _packet_init(&as_pkt2,
            IPv4(192,168,100,10),
            IPv4(192,168,100,12),
            1235,80);

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt2);
    _entry_init(entry,(void*)&as_pkt2);

    _packet_init(&as_pkt3,
            IPv4(192,168,100,10),
            IPv4(192,168,100,13),
            1236,80);

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt3);
    _entry_init(entry,&as_pkt3);

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt2);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,3," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),3,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),3,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(3,0,3,0,0,0))
        return CH_TEST_FAILED;

    /*create until to caches limits*/
    for(i=3;i<TEST_N_CACHES_LIMIT;i++){
        
        _packet_init(&as_pkt3,
                IPv4(192,168,100,10),
                IPv4(192,168,100,13+i+1),
                1236+i+1,80);
    
        entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt3);
        _entry_init(entry,&as_pkt3);
    }

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,TEST_N_CACHES_LIMIT," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),TEST_N_CACHES_LIMIT,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),TEST_N_CACHES_LIMIT,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_CACHES_LIMIT,0,4,0,0,0))
        return CH_TEST_FAILED;
    
    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt2);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,TEST_N_CACHES_LIMIT," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),TEST_N_CACHES_LIMIT,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),TEST_N_CACHES_LIMIT,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_CACHES_LIMIT,0,5,0,0,0))
        return CH_TEST_FAILED;
    
    /*create until to caches limits*/
    for(i=TEST_N_CACHES_LIMIT;i<TEST_N_ENTRIES_LIMIT;i++){
        
        _packet_init(&as_pkt3,
                IPv4(192,168,100,10),
                IPv4(192,168,100,13+i+1),
                1236+i+1,80);
    
        entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt3);
        _entry_init(entry,&as_pkt3);
    }

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,TEST_N_CACHES_LIMIT," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    
    int nn = (int)get_list_count(&as_tbl->caches,1);
    printf("nn==================%d\n",nn);

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),TEST_N_CACHES_LIMIT,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),TEST_N_ENTRIES_LIMIT,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_ENTRIES_LIMIT,0,5,1,0,0))
        return CH_TEST_FAILED;
    
    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt2);
    CH_TEST_ASSERT_NOT_NULL(entry1,"can not find  entry has been put!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry has not been update after find");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,TEST_N_CACHES_LIMIT," table numbers of caches are invalid after find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),TEST_N_CACHES_LIMIT,"as_tbl->caches value is invalid after find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),TEST_N_ENTRIES_LIMIT,"as_tbl->entries value is invalid after find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(TEST_N_ENTRIES_LIMIT,0,5,2,0,0))
        return CH_TEST_FAILED;

    return CH_TEST_SUCCESS;
}

static int test_table_entry_free(void){

    _table_entry_test_t *entry,*entry1;
    ch_assemble_packet_t as_pkt1,as_pkt2,as_pkt3;
    int i;
    
    apr_pool_destroy(as_tbl->mp);
    if(CH_TEST_SUCCESS != test_table_create())
        return CH_TEST_FAILED;
    
    _packet_init(&as_pkt1,
            IPv4(192,168,100,10),
            IPv4(192,168,100,11),
            1234,80);

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt1);
    _entry_init(entry,&as_pkt1);

    ch_table_entry_free(as_tbl,(ch_table_entry_t*)entry);

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NULL(entry1,"after free,entry should not been found!");
    CH_TEST_ASSERT_NULL(as_tbl->last_entry,"as_tbl->last_entry should been NULL after free");
     
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,1,"as_tbl->n_entries is invalid after free!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,0,"as_tbl->n_caches is invalid after free!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),1,"as_tbl->free_entries value is invalid after free !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),0,"as_tbl->caches value is invalid after free!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),0,"as_tbl->entries value is invalid after free!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(1,1,0,0,0,0))
        return CH_TEST_FAILED;

    entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,(void*)&as_pkt1);
    _entry_init(entry,&as_pkt1);

    entry1 = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NOT_NULL(entry1,"entry should been found after create&find!");
    CH_TEST_ASSERT_EQUAL(as_tbl->last_entry,(ch_table_entry_t*)entry1,"as_tbl->last_entry should been update after create &find");
     
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,1,"as_tbl->n_entries is invalid after create &find!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,1,"as_tbl->n_caches is invalid after create &find!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),0,"as_tbl->free_entries value is invalid after create &find !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),1,"as_tbl->caches value is invalid after create &find!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),1,"as_tbl->entries value is invalid after create &find!");

    if(CH_TEST_SUCCESS!=test_tbl_stats(1,1,1,0,0,0))
        return CH_TEST_FAILED;

    /*create until to entries limits*/
    for(i=1;i<TEST_N_ENTRIES_LIMIT;i++){
        
        _packet_init(&as_pkt3,
                IPv4(192,168,100,100),
                IPv4(192,168,100,50+i+1),
                1236+i+1,80);
    
        entry = (_table_entry_test_t*)ch_table_entry_create(as_tbl,&as_pkt3);
        _entry_init(entry,&as_pkt3);
    }

    ch_table_entry_free(as_tbl,(ch_table_entry_t*)entry1);

    entry = (_table_entry_test_t*)ch_table_entry_find(as_tbl,(void*)&as_pkt1);
    CH_TEST_ASSERT_NULL(entry,"after free,entry should not been found!");
    CH_TEST_ASSERT_NOT_NULL(as_tbl->last_entry,"as_tbl->last_entry should been not NULL after no found");
     
    CH_TEST_ASSERT_EQUAL(as_tbl->n_entries,TEST_N_ENTRIES_LIMIT,"as_tbl->n_entries is invalid after free!");
    CH_TEST_ASSERT_EQUAL(as_tbl->n_caches,TEST_N_CACHES_LIMIT,"as_tbl->n_caches is invalid after free!");

    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->free_entries,0),1,"as_tbl->free_entries value is invalid after free !");
    CH_TEST_ASSERT_EQUAL(get_list_count(&as_tbl->caches,1),TEST_N_CACHES_LIMIT,"as_tbl->caches value is invalid after free!");
    CH_TEST_ASSERT_EQUAL(get_tbl_list_count(),TEST_N_ENTRIES_LIMIT-1,"as_tbl->entries value is invalid after free!");

    return CH_TEST_SUCCESS;
}

static int test_table(void){

    if(CH_TEST_SUCCESS != test_table_create())
        return CH_TEST_FAILED;

    if(CH_TEST_SUCCESS != test_table_entry_create())
        return CH_TEST_FAILED;
   
    if(CH_TEST_SUCCESS != test_table_entry_find())
        return CH_TEST_FAILED;

    if(CH_TEST_SUCCESS!= test_table_entry_free())
        return CH_TEST_FAILED;

    return CH_TEST_SUCCESS;
}

static ch_test_command_t as_tbl_cmd = {

    .command = "as_tbl_test",
    .callback = test_table,
};

CH_REGISTER_TEST_COMMAND(as_tbl_cmd);


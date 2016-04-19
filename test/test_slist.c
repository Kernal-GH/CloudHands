/*
 * =====================================================================================
 *
 *       Filename:  test_slist.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月16日 14时49分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>




#define ch_slist_foreach_entry(h,entry,link) \
    for(entry = h;entry;entry=entry->link)

#define ch_slist_entry_next(entry,link) ((entry)?(entry)->link:NULL)

#define ch_slist_foreach_entry_safe(h,entry,tmp_ptr,link)\
    for(entry = h,tmp_ptr = ch_slist_entry_next(entry,link);entry;entry=tmp_ptr,tmp_ptr = ch_slist_entry_next(tmp_ptr,link))

#define ch_slist_foreach_entry_three(h,entry,pre,post,link)\
    for(entry = h,post = ch_slist_entry_next(entry,link);entry;pre=entry,entry=post,post = ch_slist_entry_next(post,link))

#define ch_slist_fast_del(h,pre,entry,post,link) do{ if(pre == NULL){h=post;}else{pre->link = post;} entry->link = NULL;}while(0)

#define ch_slist_entry_del(h,pre_entry,pos,entry,link) do{\
    pos = h;                                         \
    pre_entry = NULL;                                \
    while(pos){                                      \
        if(pos == entry)break;                       \
        pre_entry = pos;                             \
        pos = pos->link;                             \
    }                                                \
    if(pos){                                         \
        if(pre_entry){                               \
            pre_entry->link = pos->link;             \
        }else{                                       \
            h = pos->link;                           \
        }                                            \
        pos->link = NULL;                            \
    }                                                \
}while(0)


#define ch_slist_entry_add(h,entry,link) do{        \
    if(h==NULL){ (h)=(entry);}                      \
    else{                                           \
        (entry)->link = (h);                        \
        (h)=(entry);                                \
    }                                               \
}while(0)

typedef struct slist_entry_t slist_entry_t;

struct slist_entry_t {
 
    slist_entry_t *next;
    int a;
};

typedef struct {
    slist_entry_t *entries;
}slist_pool_t;


static inline void slist_print(slist_pool_t *h){
    slist_entry_t *entry,*post;
    ch_slist_foreach_entry_safe(h->entries,entry,post,next){

        printf("entry[%d]\n",entry->a);
    }
}

static inline void slist_for_del(slist_pool_t *h){

    slist_entry_t *entry,*pre = NULL,*post=NULL;
    int i = 0;

    ch_slist_foreach_entry_three(h->entries,entry,pre,post,next){

        if(i/2==0){

            ch_slist_fast_del(h->entries,pre,entry,post,next);
        }
        i++;
    }
}

int main(int argc,char **argv){

    slist_entry_t *entry,*pre,*post;

    slist_pool_t sp={NULL};

    slist_entry_t a={NULL,1};
    slist_entry_t b={NULL,2};
    slist_entry_t c={NULL,3};
    slist_entry_t d={NULL,4};
    slist_entry_t *a_ptr = &a;
    slist_entry_t *b_ptr = &b;
    slist_entry_t *c_ptr = &c;
    slist_entry_t *d_ptr = &d;

    printf("None:\n");
    slist_print(&sp);
    ch_slist_entry_add(sp.entries,&a,next);
    printf("One:\n");
    slist_print(&sp);
    ch_slist_entry_add(sp.entries,&b,next);
    ch_slist_entry_add(sp.entries,&c,next);
    ch_slist_entry_add(sp.entries,&d,next);

    printf("Four:\n");
    slist_print(&sp);

    printf("Test for_del:\n");
    slist_for_del(&sp);
    slist_print(&sp);

    printf("Del 4:\n");
    ch_slist_entry_del(sp.entries,pre,post,&d,next);
    slist_print(&sp);

    printf("Del 1:\n");
    ch_slist_entry_del(sp.entries,pre,post,&a,next);
    slist_print(&sp);

    printf("Del 2:\n");
    ch_slist_entry_del(sp.entries,pre,post,&b,next);
    slist_print(&sp);

    printf("Del 3:\n");
    ch_slist_entry_del(sp.entries,pre,post,&c,next);
    slist_print(&sp);
}


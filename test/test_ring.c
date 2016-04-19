/*
 * =====================================================================================
 *
 *       Filename:  test_ring.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月15日 18时14分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_ring.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct ch_ring_pool_t ch_ring_pool_t;
typedef struct ch_ring_entry_t ch_ring_entry_t;

struct ch_ring_entry_t {

    CH_RING_ENTRY(ch_ring_entry_t) link;

    int a;
};

struct ch_ring_pool_t {
    
    CH_RING_ENTRY(ch_ring_entry_t) head;

    int aa;
    float bb;
};

static inline void print_ring(ch_ring_pool_t *p){
   
    ch_ring_entry_t *ep;

    CH_RING_FOREACH(ep,&p->head,ch_ring_entry_t,link){

        printf("e:%d\n",ep->a);
    }

    
}

static ch_ring_entry_t *find_nb(ch_ring_pool_t *p,ch_ring_entry_t *e){

    ch_ring_entry_t *ep;

    CH_RING_FOREACH(ep,&p->head,ch_ring_entry_t,link){

        if(ep->a>=e->a){

            return ep; 
        }
    }

    return NULL;
}

static void find_neb(ch_ring_pool_t *p,ch_ring_entry_t *e,ch_ring_entry_t **bf,ch_ring_entry_t **af){

    ch_ring_entry_t *ep;
    *bf = NULL;
    *af = NULL;

    CH_RING_FOREACH(ep,&p->head,ch_ring_entry_t,link){

        if(ep->a>=e->a){

            *af = ep;
            *bf = CH_RING_PREV(ep,link) == CH_RING_SENTINEL(&p->head,ch_ring_entry_t,link)?NULL:CH_RING_PREV(ep,link);
            return;
        }
    }

    if(!CH_RING_EMPTY(&p->head,ch_ring_entry_t,link)){
        *bf = CH_RING_LAST(&p->head);
    }
}

static void insert_sort(ch_ring_pool_t *p,ch_ring_entry_t *e){

    ch_ring_entry_t *af = find_nb(p,e);
    if(af){

        CH_RING_INSERT_BEFORE(af,e,link);
    }else{
        CH_RING_INSERT_TAIL(&p->head,e,ch_ring_entry_t,link);
    }
}

static void test_for_del(ch_ring_pool_t *p){

    ch_ring_entry_t *e,*e1;
    int i =0;

    CH_RING_FOREACH_SAFE(e,e1,&p->head,ch_ring_entry_t,link){
        
        if((i%2)==0){

            printf("del-->%d,%d\n",i,e->a);
            CH_RING_REMOVE(e,link);
        }
        i++;
    }
}

int main(int argc,char ** argv){

    ch_ring_pool_t t,*p = &t;
    p->aa=1234;
    p->bb = 3.134;

    ch_ring_entry_t *bf,*af;

    ch_ring_entry_t a,b,c,d,e,f;
    a.a = 1;
    b.a = 2;
    c.a = 3;
    d.a = 4;
    e.a = -1;
    f.a = 6;

    CH_RING_INIT(&p->head,ch_ring_entry_t,link);
    if(CH_RING_EMPTY(&p->head,ch_ring_entry_t,link))
        printf("empty-----\n");

    find_neb(p,&a,&bf,&af);

    insert_sort(p,&a);
    if(!CH_RING_EMPTY(&p->head,ch_ring_entry_t,link))
        printf("no empty-----\n");
    
    find_neb(p,&a,&bf,&af);
    insert_sort(p,&c);
    insert_sort(p,&d);
    insert_sort(p,&e);
    insert_sort(p,&b);

    find_neb(p,&e,&bf,&af);

    find_neb(p,&c,&bf,&af);
    find_neb(p,&f,&bf,&af);
    print_ring(p);

    printf("test for del:\n");
    test_for_del(p);
    print_ring(p);
}


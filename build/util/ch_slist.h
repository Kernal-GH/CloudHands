/*
 * =====================================================================================
 *
 *       Filename:  ch_slist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月16日 14时29分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SLIST_H
#define CH_SLIST_H

#define ch_slist_foreach_entry(h,entry,link) \
    for(entry = h;entry;entry=entry->link)

#define ch_slist_entry_next(entry,link) ((entry)?(entry)->link:NULL)

#define ch_slist_foreach_entry_safe(h,entry,tmp_ptr,link)\
    for(entry = h,tmp_ptr = ch_slist_entry_next(entry,link);entry;entry=tmp_ptr,tmp_ptr = ch_slist_entry_next(tmp_ptr,link))

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

#endif /*CH_SLIST_H*/

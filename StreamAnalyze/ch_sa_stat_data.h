/*
 * =====================================================================================
 *
 *       Filename:  ch_sa_stat_data.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2018 03:32:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SA_STAT_DATA_H
#define CH_SA_STAT_DATA_H

typedef struct ch_sa_stat_data_t ch_sa_stat_data_t;

struct ch_sa_stat_data_t {

    const char *key;
    int is_req;
    uint64_t pkts;
    uint64_t bytes;

};


#endif /* CH_SA_STAT_DATA_H */


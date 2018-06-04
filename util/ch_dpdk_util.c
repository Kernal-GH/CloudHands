/*
 *
 *      Filename: ./util/ch_dpdk_util.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-21 17:33:46
 * Last Modified: 2016-07-21 17:34:11
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_dpdk_util.h"

char * ch_macaddr_str_get(char *buf,size_t blen,struct ether_addr *maddr){

    memset(buf,0,blen);

    snprintf(buf,blen,"%02X:%02X:%02X:%02X:%02X:%02X",
                maddr->addr_bytes[0],
                maddr->addr_bytes[1],
                maddr->addr_bytes[2],
                maddr->addr_bytes[3],
                maddr->addr_bytes[4],
                maddr->addr_bytes[5]);

    return buf;
}


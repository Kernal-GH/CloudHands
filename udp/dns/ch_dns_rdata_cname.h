/*
 *
 *      Filename: ch_dns_rdata_cname.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:04:40
 * Last Modified: 2018-05-02 17:04:40
 */

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_cname_t ch_dns_rdata_cname_t;


struct ch_dns_rdata_cname_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t name;

};

extern void ch_dns_rdata_cname_init(ch_dns_rdata_pool_t *rdata_pool);


/*
 *
 *      Filename: ch_dns_rdata_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-25 11:53:40
 * Last Modified: 2018-04-25 11:53:40
 */

#ifndef CH_DNS_RDATA_POOL_H
#define CH_DNS_RDATA_POOL_H

typedef struct ch_dns_rdata_pool_t ch_dns_rdata_pool_t;
typedef struct ch_dns_rdata_parser_t ch_dns_rdata_parser_t;

#include "ch_mpool.h"
#include "ch_list.h"
#include "ch_dns_rdata.h"

struct ch_dns_rdata_pool_t {

	struct list_head rdata_parsers;

	ch_pool_t *mp;


};

struct ch_dns_rdata_parser_t {

	struct list_head node;

	uint16_t dclass;
	uint16_t type;

	ch_dns_rdata_t * (*rdata_create)(ch_pool_t *mp,void *priv_data);

	int (*rdata_parse)(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data);

};

extern ch_dns_rdata_pool_t *ch_dns_rdata_pool_create(ch_pool_t *mp);

extern void ch_dns_rdata_parser_register(ch_dns_rdata_pool_t *rdata_pool,ch_dns_rdata_parser_t *parser);

extern ch_dns_rdata_parser_t * ch_dns_rdata_parser_find(ch_dns_rdata_pool_t *rdata_pool,uint16_t cls,uint16_t type);

extern ch_dns_rdata_t * ch_dns_rdata_parse(ch_pool_t *mp,ch_dns_rdata_pool_t *rdata_pool,ch_dns_data_input_t *din);

#endif /*CH_DNS_RDATA_POOL_H*/

/*
 *
 *      Filename: ch_dns_rdata_parser.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-31 23:58:44
 * Last Modified: 2016-10-31 23:58:44
 */

#ifndef CH_DNS_RDATA_PARSER_H
#define CH_DNS_RDATA_PARSER_H

typedef struct ch_dns_rdata_parse_t ch_dns_rdata_parse_t;
typedef struct ch_dns_rdata_t ch_dns_rdata_t;

#include <stdint.h>
#include <apr_pools.h>
#include <msgpack.h>

typedef ch_dns_rdata_t* rr_data_parser(apr_pool_t *,const uint8_t*, uint32_t, uint32_t, 
                              uint16_t, uint32_t);

struct ch_dns_rdata_parse_t {

    uint16_t cls;
    uint16_t rtype;
	
	rr_data_parser* rdata_parse;

	size_t count;

};

struct ch_dns_rdata_t {

	void *data_parsed;

	void (*msgpack_format)(msgpack_packer *pk,ch_dns_rdata_t *rdata);
};

// Add them to the list of data parsers in rtypes.c.
extern ch_dns_rdata_parse_t rr_parsers[];

rr_data_parser _rdata_opts_parse;
rr_data_parser _rdata_escape_parse;

// Add them to the list of data parsers in rtypes.c.
extern ch_dns_rdata_parse_t rr_parsers[];

extern ch_dns_rdata_parse_t* ch_dns_rdata_parser_find(uint16_t cls, uint16_t rtype);

#endif /* CH_DNS_RDATA_PARSER_H */

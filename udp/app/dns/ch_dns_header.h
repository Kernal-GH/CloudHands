/*
 *
 *      Filename: ch_dns_header.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-25 10:29:21
 * Last Modified: 2018-04-25 10:29:21
 */

#ifndef CH_DNS_HEADER_H
#define CH_DNS_HEADER_H

typedef struct ch_dns_header_t ch_dns_header_t;

#include "ch_dns_data_input.h"
#include "ch_data_output.h"
#include "ch_msgpack_store.h"

struct ch_dns_header_t {

	uint16_t id;
	uint16_t flags;

	uint16_t qcount;
	uint16_t ancount;
	uint16_t aucount;
	uint16_t adcount;	
};

#define ch_dns_hdr_qr(hdr) ((hdr)->flags>>15)
#define ch_dns_hdr_opcode(hdr) (((hdr)->flags&(0xF<<11))>>11)
#define ch_dns_hdr_aa(hdr) (((hdr)->flags&(1<<10))?1:0)
#define ch_dns_hdr_tc(hdr) (((hdr)->flags&(1<<9))?1:0)
#define ch_dns_hdr_rd(hdr) (((hdr)->flags&(1<<8))?1:0)
#define ch_dns_hdr_ra(hdr) (((hdr)->flags&(1<<7))?1:0)
#define ch_dns_hdr_rcode(hdr) ((hdr)->flags&0xF)


extern int ch_dns_header_parse(ch_dns_data_input_t *din,ch_dns_header_t *hdr);

static inline void ch_dns_header_dump(ch_dns_header_t *hdr,FILE *fp){

	fprintf(fp,"dns.hdr.id:%u\n",hdr->id);
	fprintf(fp,"dns.hdr.flags:%u\n",hdr->flags);
	fprintf(fp,"dns.hdr.qcount:%u\n",hdr->qcount);
	fprintf(fp,"dns.hdr.ancount:%u\n",hdr->ancount);
	fprintf(fp,"dns.hdr.aucount:%u\n",hdr->aucount);
	fprintf(fp,"dns.hdr.adcount:%u\n",hdr->adcount);

}

static inline void ch_dns_header_store(ch_dns_header_t *hdr,ch_msgpack_store_t *dstore){

    ch_msgpack_store_map_start(dstore,"header",6);

    ch_msgpack_store_write_uint16(dstore,"id",hdr->id);
    ch_msgpack_store_write_uint16(dstore,"flags",hdr->flags);
    ch_msgpack_store_write_uint16(dstore,"qc",hdr->qcount);
    ch_msgpack_store_write_uint16(dstore,"anc",hdr->ancount);
    ch_msgpack_store_write_uint16(dstore,"auc",hdr->aucount);
    ch_msgpack_store_write_uint16(dstore,"adc",hdr->adcount);

}

static inline ssize_t ch_dns_header_write(ch_dns_header_t *hdr,ch_data_output_t *dout){

	ssize_t rc,len=0;

	CH_DOUT_UINT16_WRITE(dout,hdr->id,len,rc);
	CH_DOUT_UINT16_WRITE(dout,hdr->flags,len,rc);
	CH_DOUT_UINT16_WRITE(dout,hdr->qcount,len,rc);
	CH_DOUT_UINT16_WRITE(dout,hdr->ancount,len,rc);
	CH_DOUT_UINT16_WRITE(dout,hdr->aucount,len,rc);
	CH_DOUT_UINT16_WRITE(dout,hdr->adcount,len,rc);

	return len;
}
#define CH_DNS_HEADER_WRITE(hdr,dout,len,rc) do {  \
	if(-1 == (rc = ch_dns_header_write(hdr,dout))) \
		return -1;                                 \
	len+=rc;									   \
}while(0)

#endif /*CH_DNS_HEADER_H*/

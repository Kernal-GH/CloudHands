/*
 *
 *      Filename: ch_dns_class.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-25 13:52:26
 * Last Modified: 2018-04-25 13:52:26
 */

#ifndef CH_DNS_CLASS_H
#define CH_DNS_CLASS_H

typedef uint16_t ch_dns_rdata_class_t;

enum {
	dns_rdataclass_reserved0 = 0,

	dns_rdataclass_in = 1,

	dns_rdataclass_chaos = 3,
	dns_rdataclass_ch = 3,
	dns_rdataclass_hs = 4,
	dns_rdataclass_none = 254,
	dns_rdataclass_any = 255
};

#define dns_rdataclass_reserved0 ((ch_dns_rdata_class_t)dns_rdataclass_reserved0)
#define dns_rdataclass_in		 ((ch_dns_rdata_class_t)dns_rdataclass_in)
#define dns_rdataclass_chaos	 ((ch_dns_rdata_class_t)dns_rdataclass_chaos)
#define dns_rdataclass_ch		 ((ch_dns_rdata_class_t)dns_rdataclass_ch)
#define dns_rdataclass_hs		 ((ch_dns_rdata_class_t)dns_rdataclass_hs)
#define dns_rdataclass_none	     ((ch_dns_rdata_class_t)dns_rdataclass_none)
#define dns_rdataclass_any	     ((ch_dns_rdata_class_t)dns_rdataclass_any)

#endif /*CH_DNS_CLASS_H*/

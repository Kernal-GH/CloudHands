/*************************************************************************
 > File Name: ch_test_util.h
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年05月10日 星期二 14时10分12秒
 ************************************************************************/

#ifndef CH_TEST_UTIL_H
#define CH_TEST_UTIL_H

#include <rte_mbuf.h>

extern struct rte_mbuf * alloc_a_mbuf(uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,
		void *data,uint16_t dlen,uint32_t seq,uint32_t seq_ack,uint16_t tcp_flags);


extern void free_a_mbuf(struct rte_mbuf *mbuf);

#endif /*CH_TEST_UTIL_H*/

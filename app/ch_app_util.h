/*
 * =====================================================================================
 *
 *       Filename:  ch_app_util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月22日 14时23分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_APP_UTIL_H
#define CH_APP_UTIL_H

#include <stdint.h>

static inline int ch_port_equal(uint16_t src_port,uint16_t dst_port,uint16_t v){

    return src_port == v || dst_port == v;
}

static inline int ch_ports_equal(uint16_t *ports,int n,uint16_t src_port,uint16_t dst_port){

    int i ;

    uint16_t v;

    for(i= 0;i<n;i++){
    
        v = ports[i];
		
		if(v == 0)
			break;

        if(ch_port_equal(src_port,dst_port,v))
            return 1;
        
    }

    return 0;
}

#endif /*CH_APP_UTIL_H*/

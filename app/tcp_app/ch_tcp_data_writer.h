/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_data_writer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月1日 11时50分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_TCP_DATA_WRITER_H
#define CH_TCP_DATA_WRITER_H

#include <stdint.h>
#include "ch_shm_format.h"

extern int ch_tcp_data_write(ch_shm_format_t *fmt,ch_shm_record_t *rcd,int proto_id);

#endif /*CH_TCP_DATA_WRITER_H*/

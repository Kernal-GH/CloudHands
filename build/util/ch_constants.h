/*
 * =====================================================================================
 *
 *       Filename:  ch_constants.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月14日 14时38分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_CONSTANTS_H
#define CH_CONSTANTS_H

#ifndef CR
#define CR '\r'
#endif 

#ifndef CRLF
#define CRLF "\r\n"
#endif

#ifndef LF
#define LF '\n'
#endif

#define MAX_STRING_LEN 8192

#define CH_OK 0
#define CH_ERROR -1

#define PROCESSOR_RET_OK 0
#define PROCESSOR_RET_DROP 1

#define CREATEMODE ( APR_UREAD | APR_UWRITE | APR_GREAD )
#define CREATEMODE_DIR ( APR_UREAD | APR_UWRITE | APR_UEXECUTE | APR_GREAD | APR_GEXECUTE )
#define CREATEMODE_UNISTD ( S_IRUSR | S_IWUSR | S_IRGRP )

#define ch_align_low(d,a) ((d)&~(a-1))
#define ch_align_up(d,a) (((d)+(a-1))&~(a-1))
#endif /*CH_CONSTANTS_H*/


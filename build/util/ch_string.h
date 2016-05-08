/*
 * =====================================================================================
 *
 *       Filename:  ch_string.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时23分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_STRING_H
#define CH_STRING_H

#include <stdio.h>

typedef struct {
    size_t len;
    char *data;
}ch_str_t;

#define ch_string(str) { sizeof(str) - 1, str}

#endif /*CH_STRING_H*/


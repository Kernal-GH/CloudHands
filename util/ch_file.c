/*
 * =====================================================================================
 *
 *       Filename:  ch_file.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月24日 11时23分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <errno.h>
#include <stdio.h>
#include "ch_file.h"

ssize_t ch_file_write(ch_file_t *file,void *data,size_t data_len,size_t offset){

    ssize_t n,written = 0;
    /*no data to write*/
    if(data == NULL || data_len == 0)
        return 0;

    if (file->sys_offset != offset) {
        if (lseek(file->fd, offset, SEEK_SET) == -1) {
            return -1;
        }

        file->sys_offset = offset;
    }

    for ( ;; ) {
        n = write(file->fd, data + written, data_len);

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        file->sys_offset += n;
        file->offset += n;
        written += n;

        if ((size_t) n == data_len) {
            return written;
        }

        data_len -= n;
    }

}

ssize_t
ch_file_read(ch_file_t *file, char *buf, size_t size, size_t offset)
{
    ssize_t  n;

    if (file->sys_offset != offset) {
        if (lseek(file->fd, offset, SEEK_SET) == -1) {
            return -1;
        }

        file->sys_offset = offset;
    }

    n = read(file->fd, buf, size);

    if (n == -1) {
        return -1;
    }

    file->sys_offset += n;

    file->offset += n;

    return n;
}


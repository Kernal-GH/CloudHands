/*
 * =====================================================================================
 *
 *       Filename:  ch_sfile.c
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
#include "ch_sfile.h"

ssize_t ch_sfile_write(ch_sfile_t *sfile,void *data,size_t data_len,size_t offset){

    ssize_t n,written = 0;
    /*no data to write*/
    if(data == NULL || data_len == 0)
        return 0;

    if (sfile->sys_offset != offset) {
        if (lseek(sfile->fd, offset, SEEK_SET) == -1) {
            return -1;
        }

        sfile->sys_offset = offset;
    }

    for ( ;; ) {
        n = write(sfile->fd, data + written, data_len);

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        sfile->sys_offset += n;
        sfile->offset += n;
        written += n;

        if ((size_t) n == data_len) {
            return written;
        }

        data_len -= n;
    }

}

ssize_t
ch_sfile_read(ch_sfile_t *sfile, char *buf, size_t size, size_t offset)
{
    ssize_t  n;

    if (sfile->sys_offset != offset) {
        if (lseek(sfile->fd, offset, SEEK_SET) == -1) {
            return -1;
        }

        sfile->sys_offset = offset;
    }

    n = read(sfile->fd, buf, size);

    if (n == -1) {
        return -1;
    }

    sfile->sys_offset += n;

    sfile->offset += n;

    return n;
}


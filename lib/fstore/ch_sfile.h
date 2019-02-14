/*
 * =====================================================================================
 *
 *       Filename:  ch_sfile.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月24日 11时02分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SFILE_H
#define CH_SFILE_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int fd;
    size_t sys_offset;
    size_t offset;
} ch_sfile_t ;

#define CH_SFILE_DEFAULT_ACCESS 0644

#define CH_SFILE_RDONLY          O_RDONLY
#define CH_SFILE_WRONLY          O_WRONLY
#define CH_SFILE_RDWR            O_RDWR
#define CH_SFILE_CREATE_OR_OPEN  O_CREAT
#define CH_SFILE_OPEN            0
#define CH_SFILE_TRUNCATE        (O_CREAT|O_TRUNC)
#define CH_SFILE_APPEND          (O_WRONLY|O_APPEND)
#define CH_SFILE_NONBLOCK        O_NONBLOCK

#define ch_sfile_open(name,mode,create,access)  \
    open((const char*)name,mode|create,access)

#define ch_sfile_delete(name) unlink((const char *) name)

#define ch_sfile_open_for_write_common(name)  \
    ch_sfile_open(name,CH_SFILE_APPEND,CH_SFILE_CREATE_OR_OPEN,CH_SFILE_DEFAULT_ACCESS)


/*write data into sfile
 * @sfile,the sfile instance writting
 * @data,the start address of data writting
 * @data_len,the data length writting
 * @offset,the sfile offset to start writting
 * return: if ok ,return the real size writted,otherwise return -1*/
extern ssize_t ch_sfile_write(ch_sfile_t *sfile,void *data,size_t data_len,size_t offset);


/*Read data from sfile
 * @sfile,the sfile instance reading
 * @buf,the buffer 
 * @size,the buffer length
 * @offset,the offset read in sfile
 * Return: if ok,return the size read,otherwise return -1*/
extern ssize_t ch_sfile_read(ch_sfile_t *sfile, char *buf, size_t size, size_t offset);

#endif /*CH_SFILE_H*/


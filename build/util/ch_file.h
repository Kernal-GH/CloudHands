/*
 * =====================================================================================
 *
 *       Filename:  ch_file.h
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

#ifndef CH_FILE_H
#define CH_FILE_H

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
} ch_file_t ;

#define CH_FILE_DEFAULT_ACCESS 0644

#define CH_FILE_RDONLY          O_RDONLY
#define CH_FILE_WRONLY          O_WRONLY
#define CH_FILE_RDWR            O_RDWR
#define CH_FILE_CREATE_OR_OPEN  O_CREAT
#define CH_FILE_OPEN            0
#define CH_FILE_TRUNCATE        (O_CREAT|O_TRUNC)
#define CH_FILE_APPEND          (O_WRONLY|O_APPEND)
#define CH_FILE_NONBLOCK        O_NONBLOCK

#define ch_file_open(name,mode,create,access)  \
    open((const char*)name,mode|create,access)

#define ch_file_delete(name) unlink((const char *) name)

#define ch_file_open_for_write_common(name)  \
    ch_file_open(name,CH_FILE_APPEND,CH_FILE_CREATE_OR_OPEN,CH_FILE_DEFAULT_ACCESS)


/*write data into file
 * @file,the file instance writting
 * @data,the start address of data writting
 * @data_len,the data length writting
 * @offset,the file offset to start writting
 * return: if ok ,return the real size writted,otherwise return -1*/
extern ssize_t ch_file_write(ch_file_t *file,void *data,size_t data_len,size_t offset);


/*Read data from file
 * @file,the file instance reading
 * @buf,the buffer 
 * @size,the buffer length
 * @offset,the offset read in file
 * Return: if ok,return the size read,otherwise return -1*/
extern ssize_t ch_file_read(ch_file_t *file, char *buf, size_t size, size_t offset);

#endif /*CH_FILE_H*/


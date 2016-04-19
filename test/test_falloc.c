/*
 * =====================================================================================
 *
 *       Filename:  test_falloc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月12日 14时13分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
int main(int argc,char **argv){
    int fd1,ret;
    int file_size=20971520; //20M
    fd1=open("/tmp/1",O_CREAT | O_RDWR,0777);
    ret = fallocate(fd1, 0, 0, file_size);
    close(fd1);
    return ret;
}

/*
 * =====================================================================================
 *
 *       Filename:  test_file_mapping.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月06日 14时20分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define ch_align_low(d,a) ((d)&~(a-1))
#define ch_align_up(d,a) (((d)+(a-1))&~(a-1))

static void test_read(int fd,size_t offset,size_t len){

   char *addr;
   struct stat sb;
   off_t pa_offset;
   ssize_t s;


   if (fstat(fd, &sb) == -1)           /* To obtain file size */
       handle_error("fstat");

   pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
       /* offset for mmap() must be page aligned */

   if (offset >= sb.st_size) {
       fprintf(stderr, "offset is past end of file\n");
       exit(EXIT_FAILURE);
   }

   if (offset + len > sb.st_size)
       len = sb.st_size - offset;
               /* Can't display bytes past end of file */


   addr = mmap(NULL, len + offset - pa_offset, PROT_READ,
               MAP_PRIVATE, fd, pa_offset);
   if (addr == MAP_FAILED)
       handle_error("mmap");

   s = write(STDOUT_FILENO, addr + offset - pa_offset-1, len);
   printf("\n");
   if (s != len) {
       if (s == -1)
           handle_error("write");

       fprintf(stderr, "partial write");
   }

   munmap(addr,len + offset - pa_offset);
}

static inline void set_fsize(int fd,size_t fsize){

    lseek(fd,fsize-1,SEEK_SET);
    write(fd,"",1);
}

static void test_write(int fd,const char *content,size_t offset){
   
    size_t pa_offset = ~(4096-1)&offset;
    size_t len = strlen(content);


    void *_des=mmap(NULL,offset+len-pa_offset,PROT_WRITE,MAP_SHARED,fd,pa_offset);

    memcpy(_des+offset-pa_offset,content,len);

    munmap(_des,offset+len-pa_offset);
}

int
main(int argc, char *argv[])
{

    if(argc<3){
        fprintf(stderr,"Usage:<fname><offset>");
        return -1;
    }

    int fd=open(argv[1],O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
    set_fsize(fd,ch_align_up(1024,4096));
    test_write(fd,"wo shi shajf",atoi(argv[2]));
    set_fsize(fd,ch_align_up(5000,4096));
    test_write(fd,"wo shi shage",4000);

    test_read(fd,atoi(argv[2]),12);
    test_read(fd,4000,12);
    close(fd);
}


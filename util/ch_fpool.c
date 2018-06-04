/*
 * =====================================================================================
 *
 *       Filename:  ch_fpool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月15日 16时45分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <apr_strings.h>
#include <time.h>
#include "ch_constants.h"
#include "ch_util.h"
#include "ch_log.h"
#include "ch_slist.h"
#include "ch_file.h"
#include "ch_fpool.h"

ch_fpool_t * ch_fpool_create(apr_pool_t *mp,const char *root_dir,size_t falloc_size,size_t fentry_size){

    ch_fpool_t *fp = NULL;
    size_t falloc_size_align,fentry_size_align,page_size;

    if(root_dir == NULL||falloc_size==0||fentry_size==0||fentry_size>=falloc_size){
        ch_log(CH_LOG_ERR,"Invalid args !");
        return NULL;
    }

    page_size = sysconf(_SC_PAGE_SIZE);

    falloc_size_align = ch_align_up(falloc_size,page_size);
    fentry_size_align = ch_align_up(fentry_size,page_size);

    fp = (ch_fpool_t*)apr_palloc(mp,sizeof(ch_fpool_t));

    if(!fp){
        ch_log(CH_LOG_ERR,"No memory used to create the file pool instance!");
        return NULL;
    }

    fp->mp = mp;
    fp->root_dir = root_dir;
    fp->fallocs = NULL;
    fp->fentries = NULL;
    fp->falloc_size = falloc_size_align;
    fp->fentry_size = fentry_size_align;
    fp->n_fallocs = 0;
    fp->n_free_fentries = 0;

    /*ok*/
    return fp;
}

static ch_fallocator_t * _falloc_create(ch_fpool_t *fp){

    ch_fallocator_t *falloc;
    const char *fname;
    int fd;

    /*make file allocator name*/
    {
        char tstr[64]={0};
        time_t rawtime;
        struct tm* timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(tstr,63,"/%Y%m%d/",timeinfo);
        fname = apr_psprintf(fp->mp,"%s%sfalloc-%lu-%lu",
                fp->root_dir,tstr,ch_get_current_timems(),fp->n_fallocs);

        /*make dirs*/
        ch_dir_make(fname);

        /*Create file */
        fd = ch_file_open_for_write_common(fname);
        if(fd <0){
            
            ch_log(CH_LOG_ERR,"Open file [%s] failed!",fname);
            return NULL;
        }
        /*Pre allocate file continue space*/

        if(fallocate(fd, 0, 0, fp->falloc_size)){
            ch_log(CH_LOG_ERR,"fallocate failed,file[%s],size[%d]",fname,fp->falloc_size);
            close(fd);
            return NULL;
        }
        /*make file ok*/
    }

    falloc = (ch_fallocator_t*)apr_palloc(fp->mp,sizeof(ch_fallocator_t));
    falloc->fname = fname;
    falloc->fd = fd;
    falloc->next = NULL;
    falloc->offset_start = 0;
    falloc->offset_pos = 0;
    falloc->offset_end = falloc->offset_start+fp->falloc_size;

    /*add it to file pool*/
    ch_slist_entry_add(fp->fallocs,falloc,next);
    fp->n_fallocs+=1;

    ch_log(CH_LOG_DEBUG,"Create a file allocator ok,file[%s],size[%d]",falloc->fname,fp->falloc_size);

    return falloc;
}

static void _falloc_destroy(ch_fpool_t *fp,ch_fallocator_t *falloc){

    close(falloc->fd);
    ch_file_delete(falloc->fname);
}

void ch_fpool_reset(ch_fpool_t *fp){
   
    ch_fallocator_t *falloc,*post;

    /*free all file allocator*/
    ch_slist_foreach_entry_safe(fp->fallocs,falloc,post,next){

        _falloc_destroy(fp,falloc);
    }

    fp->fallocs = NULL;
    fp->fentries = NULL;
    fp->n_fallocs = 0;
    fp->n_free_fentries = 0;

}

void ch_fpool_destroy(ch_fpool_t *fp){
    
    ch_fpool_reset(fp);
}

ch_fentry_t * ch_fpool_entry_alloc(ch_fpool_t *fp){

    ch_fentry_t *fentry = NULL;
    ch_fallocator_t *falloc = NULL;

    /*if file pool has free file entry ,then allocates a file entry from free entries*/
    if(fp->fentries){
        fentry = fp->fentries;
        fp->fentries = fentry->next;
        fentry->next = NULL;
        fentry->offset_pos = fentry->offset_start;
        fp->n_free_fentries-=1;
        return fentry;

    }else if(fp->fallocs&&ch_fallocator_space_enough(fp->fallocs,fp->fentry_size)){
        /*allocates a file entry from free spaces in file allocator*/
        falloc = fp->fallocs;
    }else{
        /*need create a new file allocator*/
        falloc = _falloc_create(fp);
        if(falloc == NULL){
            ch_log(CH_LOG_ERR,"Cannot create a new file allocator to allocate a file entry!");
            return NULL;
        }
    }

    /*allocate a new file entry from file allocator*/
    fentry = (ch_fentry_t*)apr_palloc(fp->mp,sizeof(ch_fentry_t));

    fentry->next = NULL;
    fentry->falloc = falloc;
    fentry->offset_start = falloc->offset_pos;
    fentry->offset_pos = fentry->offset_start;
    falloc->offset_pos+=fp->fentry_size;

    return fentry;
}

void ch_fpool_entry_free(ch_fpool_t *fp,ch_fentry_t *fentry){

    ch_slist_entry_add(fp->fentries,fentry,next);

    fp->n_free_fentries +=1;

}

int ch_fentry_data_append(ch_fpool_t *fp,ch_fentry_t *fentry,void *data,size_t data_len){

    ssize_t n,written = 0;
    int fd = fentry->falloc->fd;
    ch_fallocator_t *falloc = fentry->falloc;

    /*no data to write*/
    if(data == NULL || data_len == 0)
        return CH_OK;

    if(ch_fentry_space_enough(fentry,fp->fentry_size,data_len)==0){
        ch_log(CH_LOG_ERR,"No free space to store data_len[%lu] in this fentry!",data_len);
        return CH_ERROR;
    }

    if (lseek(fd, (size_t)fentry->offset_pos, SEEK_SET) == -1) {
        ch_log(CH_LOG_ERR,"lseek file pos[%lu] failed in file[%s]",fentry->offset_pos,falloc->fname);
        return CH_ERROR;
    }

    /*do write data into file */
    for ( ;; ) {
        n = write(fd, data + written, data_len);

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }

            return CH_ERROR;
        }

        written += n;

        if ((size_t)n == data_len) {
            break;
        }

        data_len -= n;
    }

    fentry->offset_pos = fentry->offset_pos+(uint32_t)written;

    return CH_OK;
}

int ch_fentry_data_fetch(ch_fpool_t *fp,ch_fentry_t *fentry,void *buf,size_t buf_len){
    
    ssize_t  n;
    size_t rsize;

    if(ch_fentry_content_size(fentry)==0||fentry->offset_pos_r>=fentry->offset_pos){
        /*no data to read*/
        return CH_ERROR;
    }

    rsize = (size_t)(fentry->offset_pos-fentry->offset_pos_r);
    if(rsize>buf_len){
        rsize = buf_len;
    }

    if (lseek(fentry->falloc->fd, fentry->offset_pos_r, SEEK_SET) == -1) {
        return CH_ERROR;
    }

    n = read(fentry->falloc->fd, buf, rsize);

    if (n == -1) {
        return CH_ERROR;
    }

    fentry->offset_pos_r = fentry->offset_pos_r+(uint32_t)n;

    return CH_OK;
}

/*
 * =====================================================================================
 *
 *       Filename:  ch_fpool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月15日 12时28分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_FPOOL_H
#define CH_FPOOL_H

typedef struct ch_fpool_t ch_fpool_t;
typedef struct ch_fallocator_t ch_fallocator_t;
typedef struct ch_fentry_t ch_fentry_t;

#include <apr_pools.h>

struct ch_fpool_t {

    /*The memory pool */
    apr_pool_t *mp;

    /*The root dir used to create file */
    const char *root_dir;

    /*The list header of all file pool allocators*/
    ch_fallocator_t *fallocs;

    /*The list header of all file entries freed*/
    ch_fentry_t *fentries;

    /*The every file allocator size */
    size_t falloc_size;

    /*The every file entry size*/
    size_t fentry_size;

    /*The number of file allocators*/
    size_t n_fallocs;
    
    /*The number of file entries freed*/
    size_t n_free_fentries;

};

struct ch_fallocator_t {
   
    /*file name of this file allocator*/
    const char *fname;

    /*file desc*/
    int fd;

    /*link to next file allocator*/
    ch_fallocator_t *next;

    /*the start offset of file allocator that can store payload data*/
    size_t offset_start;

    /*the offset of file allocator that can can be allocated*/
    size_t offset_pos;

    /*the end offset of file allocator*/
    size_t offset_end;
};

struct ch_fentry_t {

    /*link to next file entry*/
    ch_fentry_t *next;

    /*the file allocator that contains this file entry*/
    ch_fallocator_t *falloc; 

    uint32_t offset_start;

    uint32_t offset_pos;
    
    uint32_t offset_pos_r;
};


#define ch_fallocator_space_enough(falloc,sz) ((falloc)->offset_pos+(sz)>(falloc)->offset_end?0:1)

#define ch_fentry_write_reset(fentry) ((fentry)->offset_pos = (fentry)->offset_start)

#define ch_fentry_read_reset(fentry) ((fentry)->offset_pos_r = (fentry)->offset_start)

static inline size_t ch_fentry_content_size(ch_fentry_t *fentry){

    return (size_t)(fentry->offset_pos-fentry->offset_start);
}

static inline int ch_fentry_space_enough(ch_fentry_t *fentry,size_t fentry_size,size_t data_len){
    
    uint32_t data_len32 = (uint32_t)data_len;

    return (fentry->offset_pos+data_len>fentry->offset_start+fentry_size?0:1);
}

/*Create the instance of file pool
 * @mp,the memory pool used to create file pool instance
 * @root_dir,The root dir used to create file allocator in it
 * @falloc_size,the every file allocator
 * @fentry_size,the every file entry size
 * Return: if ok,return the file pool instance,otherwise return NULL. */
extern ch_fpool_t * ch_fpool_create(apr_pool_t *mp,const char *root_dir,size_t falloc_size,size_t fentry_size);


/*Destroy the instance of file pool
 * fp,the file pool instance
 * */
extern void ch_fpool_destroy(ch_fpool_t *fp);


/*Reset the instance of file pool,the file pool will back to 
 * the state created it
 *@fp,the file pool instance
 * */
extern void ch_fpool_reset(ch_fpool_t *fp);


/*
 * Alloc a file entry from file pool 
 * @fp,the file pool instance
 * Return,if ok,return the instance of file entry,otherwise return NULL
 * */
extern ch_fentry_t *ch_fpool_entry_alloc(ch_fpool_t *fp);


/*Free a file entry back to file pool
 * @fp,the file pool instance
 * @fentry,the file entry instance freed
 * */
extern void ch_fpool_entry_free(ch_fpool_t *fp,ch_fentry_t *fentry);


/*Append data into file entry
 * @fp,the file pool instance
 * @fentry,the file entry instance
 * @data,the data will be appended
 * @data_len,the data length will be appended
 * Return,if ok return CH_OK,otherwise return CH_ERROR*/
extern int ch_fentry_data_append(ch_fpool_t *fp,ch_fentry_t *fentry,void *data,size_t data_len);


/*Read the data from file entry into specify buf
 * @fp,the file pool instance
 * @fentry,the file entry instance
 * @buf,the buffer used to store data read
 * @buf_len,the buffer length
 * Return: if ok,return CH_OK,otherwise return CH_ERROR
 * */
extern int ch_fentry_data_fetch(ch_fpool_t *fp,ch_fentry_t *fentry,void *buf,size_t buf_len);

#endif /*CH_FPOOL_H*/

/*
 * =====================================================================================
 *
 *       Filename:  SHMInfoDump.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/08/2019 03:58:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "ch_shm_format.h"
#include "ch_packet_record.h"
#include "ch_shm_mmap.h"

static void print_usage(void) {

	const char* usage = "Usage:SHMInfoDump <mmap_fname> <field_index>\n"
				   "field_index as :\n"
				   "-1 all\n"
				   "0 shm_entries_start\n"
				   "1 shm_entries_count\n"
				   "2 shm_entry_size \n"
				   "3 shm_entries_count_cur \n"
				   "4 shm_write_entry_pos \n"
				   "5 shm_read_entry_pos \n"
				   "6 shm_priv_data_size \n"
				   "7 shm_last_write_time \n"
				   "8 shm_last_read_time \n"
				   "9 shm_last_write_ok_time \n"
				   "10 shm_last_read_ok_time \n";

	printf("%s",usage);

}

static void _dump_info(ch_shm_format_t *shm_fmt,int index){

    ch_shm_header_t *shm_int = shm_fmt->shm_int->shm_header;

    switch(index){

        case -1:
            printf("shm_entries_start:%lu\n",(unsigned long)shm_int->shm_entries_start);
            printf("shm_entries_count:%lu\n",(unsigned long)shm_int->shm_entries_count);
            printf("shm_entry_size:%lu\n",(unsigned long)shm_int->shm_entry_size);
            printf("shm_entries_count_cur:%lu\n",(unsigned long)shm_int->shm_entries_count_cur);
            printf("shm_write_entry_pos:%lu\n",(unsigned long)shm_int->shm_write_entry_pos);
            printf("shm_read_entry_pos:%lu\n",(unsigned long)shm_int->shm_read_entry_pos);
            printf("shm_priv_data_size:%lu\n",(unsigned long)shm_int->shm_priv_data_size);
            printf("shm_last_write_time:%lu\n",(unsigned long)shm_int->shm_last_write_time);
            printf("shm_last_read_time:%lu\n",(unsigned long)shm_int->shm_last_read_time);
            printf("shm_last_write_ok_time:%lu\n",(unsigned long)shm_int->shm_last_write_ok_time);
            printf("shm_last_read_ok_time:%lu\n",(unsigned long)shm_int->shm_last_read_ok_time);
            break;

        case 0:
            printf("shm_entries_start:%lu\n",(unsigned long)shm_int->shm_entries_start);
            break;

        case 1:
            printf("shm_entries_count:%lu\n",(unsigned long)shm_int->shm_entries_count);
            break;

        case 2:
            printf("shm_entry_size:%lu\n",(unsigned long)shm_int->shm_entry_size);
            break;

        case 3:
            printf("shm_entries_count_cur:%lu\n",(unsigned long)shm_int->shm_entries_count_cur);
            break;

        case 4:
            printf("shm_write_entry_pos:%lu\n",(unsigned long)shm_int->shm_write_entry_pos);
            break;

        case 5:
            printf("shm_read_entry_pos:%lu\n",(unsigned long)shm_int->shm_read_entry_pos);
            break;

        case 6:
            printf("shm_priv_data_size:%lu\n",(unsigned long)shm_int->shm_priv_data_size);
            break;

        case 7:
            printf("shm_last_write_time:%lu\n",(unsigned long)shm_int->shm_last_write_time);
            break;

        case 8:
            printf("shm_last_read_time:%lu\n",(unsigned long)shm_int->shm_last_read_time);
            break;

        case 9:
            printf("shm_last_write_ok_time:%lu\n",(unsigned long)shm_int->shm_last_write_ok_time);
            break;

        case 10:

            printf("shm_last_read_ok_time:%lu\n",(unsigned long)shm_int->shm_last_read_ok_time);
            break;

        default:
            print_usage();

    }
}

static void shm_close(ch_shm_format_t *shm_fmt){

     ch_shm_mmap_t *shm_mmap = (ch_shm_mmap_t*)shm_fmt->shm_int;
     munmap(shm_fmt->shm_int->shm_header,shm_fmt->shm_int->shm_header->shm_entries_start);
     close(shm_mmap->fd);

}

int  main(int argc,char **argv){

    int index;
    const char *mmap_fname;
    ch_shm_format_t tmp,*shm_fmt=&tmp;

    if(argc<3){

        print_usage();
        return -1;
    }

    mmap_fname = argv[1];
    index = atoi(argv[2]);

    if(index<-1||index>=11){

        printf("Error:invalid index:%d\n",index);
        print_usage();
        return -1;
    }

    ch_pool_t *mp = ch_pool_create(1024);
    if(mp == NULL){

        printf("Cannot create a memory pool to open shm fime:%s\n",mmap_fname);
        return -1;
    }

    shm_fmt = ch_shm_format_pkt_with_mmap_create(mp,mmap_fname,0,0,0,0);
    if(shm_fmt == NULL||shm_fmt->shm_int == NULL){

        printf("Cannot open shm file:%s\n",mmap_fname);
        return -1;
    }
    
    _dump_info(shm_fmt,index);

    shm_close(shm_fmt);

    return 0;
}




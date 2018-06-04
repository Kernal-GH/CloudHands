/*************************************************************************
 > File Name: ch_mmap_file_format.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月16日 星期四 11时25分29秒
 ************************************************************************/
#include <time.h>
#include "ch_log.h"
#include "ch_mmap_file_format.h"
#include "ch_util.h"

static ch_mmap_file_iterator_t g_fiter,*g_fiter_ptr = &g_fiter;
static ch_mmap_file_entry_t g_fentry,*g_fentry_ptr = &g_fentry;

ch_mmap_file_format_t * 
ch_mmap_file_format_create(apr_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,int is_write){

    ch_mmap_file_format_t *mmfmt = NULL;
    ch_mmap_t *mm = NULL;

    mm = ch_mmap_create(mp,fname,fsize,entry_size,0,is_write); 

    if(mm == NULL){
        ch_log(CH_LOG_ERR,"Cannot create mmap for file format!");
        return NULL;
    }

    mmfmt = (ch_mmap_file_format_t*)apr_pcalloc(mp,sizeof(ch_mmap_file_format_t));
    mmfmt->mp = mp;
    mmfmt->mm = mm;
    mmfmt->has_fheader = 0;
    mmfmt->entry_size = entry_size;
    return mmfmt;
}


void ch_mmap_file_format_destroy(ch_mmap_file_format_t *mmfmt){

    if(mmfmt->mm){

        ch_mmap_destroy(mmfmt->mm);
        mmfmt->mm = NULL;
    }
}

static inline int _is_fentry_size_over(ch_mmap_file_format_t *mmfmt,ch_mmap_file_entry_t *fentry){

    uint64_t rem_entry_size = mmfmt->entry_size-CH_MMAP_FILE_HEADER_LEN;
    uint64_t fentry_size = (uint64_t)fentry->entry_size;

    return fentry_size > rem_entry_size;
}

static inline void _mmap_file_header_init(ch_mmap_file_header_t *fheader){

    ch_bin_format_t *bfmt = &fheader->bfmt;
    ch_bf_init(bfmt,fheader->mmb.start,fheader->mmb.end);

    fheader->magic = FIMAGIC;
    fheader->major_version = MAJOR_VERSION;
    fheader->minor_version = MINJOR_VERSION;
    fheader->entries_size = 0;
    fheader->entries_number = 0;

    ch_bf_skip(bfmt,CH_MMAP_FILE_HEADER_LEN);
}

static inline void _mmap_file_header_commit(ch_mmap_file_format_t *mmfmt,ch_mmap_file_header_t *fheader){

    ch_bin_format_t *bfmt = &fheader->bfmt;
    ch_mmap_buf_t *mmb = &fheader->mmb;
    ch_bf_reset(bfmt);

    ch_bf_uint32_write(bfmt,fheader->magic);
    ch_bf_uint32_write(bfmt,fheader->major_version);
    ch_bf_uint32_write(bfmt,fheader->minor_version);
    ch_bf_uint32_write(bfmt,fheader->entries_number);
    ch_bf_uint32_write(bfmt,fheader->entries_size);

    ch_mmap_buf_commit(mmfmt->mm,mmb);
}

static int _mmap_file_header_create(ch_mmap_file_format_t *mmfmt){

    if(mmfmt->has_fheader){
        _mmap_file_header_commit(mmfmt,&mmfmt->cur_fheader);
        mmfmt->has_fheader = 0;
    }

    if(-1 == ch_mmap_buf_get(mmfmt->mm,&(mmfmt->cur_fheader.mmb))){
        return -1;
    }
    mmfmt->has_fheader = 1;
    _mmap_file_header_init(&mmfmt->cur_fheader);
    /*ok*/
    return 0;
}

static inline int _is_need_create_file_header(ch_mmap_file_format_t *mmfmt,ch_mmap_file_entry_t *fentry){

    ch_bin_format_t *bfmt = &mmfmt->cur_fheader.bfmt;
    if(mmfmt->has_fheader == 0)
        return 1;

    if(ch_bf_full(bfmt,(size_t)fentry->entry_size))
        return 1;

    return 0;
}

static void _mmap_file_entry_write(ch_mmap_file_format_t *mmfmt,ch_mmap_file_entry_t *fentry){

    unsigned char *wdata;

    ch_mmap_file_header_t *fheader = &mmfmt->cur_fheader;
    ch_bin_format_t *bfmt = &fheader->bfmt;

    uint32_t bsize = (uint32_t)ch_bf_content_size(bfmt);

    ch_bf_uint32_write(bfmt,EIMAGIC);
    ch_bf_uint32_write(bfmt,ch_mmap_file_entry_len(fentry));
    ch_bf_uint8_write(bfmt,fentry->packet_type);
    ch_bf_uint8_write(bfmt,fentry->session_direct);
    ch_bf_uint32_write(bfmt,fentry->protocol_id);
    ch_bf_uint64_write(bfmt,fentry->session_id);
    ch_bf_uint64_write(bfmt,fentry->time);
    ch_bf_uint32_write(bfmt,fentry->src_ip);
    ch_bf_uint32_write(bfmt,fentry->dst_ip);
    ch_bf_uint16_write(bfmt,fentry->src_port);
    ch_bf_uint16_write(bfmt,fentry->dst_port);
	ch_bf_uint32_write(bfmt,fentry->data_offset);

    wdata = fentry->data == NULL || fentry->data_len == 0?"":fentry->data;

    ch_bf_bytes_write(bfmt,wdata,fentry->data_len);

    uint32_t asize = (uint32_t)ch_bf_content_size(bfmt);
    fheader->entries_number+=1;
    fheader->entries_size += (asize-bsize);
}

static time_t last_log_timestamp = 0;
int ch_mmap_file_format_put(ch_mmap_file_format_t *mmfmt,ch_mmap_file_entry_t *fentry){
	time_t cur_timestamp = time(NULL);

    if(_is_fentry_size_over(mmfmt,fentry)){

        ch_log(CH_LOG_ERR,"The entry size:%lu is too large!",fentry->entry_size);
        return -2;
    }

    if(_is_need_create_file_header(mmfmt,fentry)){
		if (cur_timestamp < last_log_timestamp + 30) {
			return -1;
		}
		last_log_timestamp = cur_timestamp;

        if(-1 == _mmap_file_header_create(mmfmt)){
			

            ch_log(CH_LOG_ERR,"Cannot create a mmap buf to store fentry!");
            return -1;
        }
    }

    _mmap_file_entry_write(mmfmt,fentry);

    /*ok*/

    return 0;
}

static inline int _can_been_flush(ch_mmap_file_format_t *mmfmt){

    ch_bin_format_t *bfmt = &mmfmt->cur_fheader.bfmt;
	
	if(mmfmt->has_fheader == 0)
		return 0;

	if(ch_bf_empty(bfmt))
		return 0;

	return 1;
}

void ch_mmap_file_format_flush(ch_mmap_file_format_t *mmfmt){

	if(_can_been_flush(mmfmt)){
	
        if(-1 == _mmap_file_header_create(mmfmt)){

			mmfmt->has_fheader = 0;
        }

	}
}

static inline int has_next(ch_mmap_file_iterator_t *iter){

    if(iter->fheader == NULL)
        return 0;

    if(iter->entries_count>=iter->fheader->entries_number)
        return 0;

    return 1;
}

static inline ch_mmap_file_entry_t * _file_entry_read(ch_mmap_file_iterator_t *iter,ch_bin_format_t *bfmt){

    g_fentry_ptr->magic = ch_bf_uint32_read(bfmt);
    if(g_fentry_ptr->magic!=EIMAGIC){
        ch_log(CH_LOG_ERR,"File entry magic:%lu is invalid!",g_fentry_ptr->magic);
        iter->fheader = NULL;
        return NULL;
    }
    g_fentry_ptr->entry_size = ch_bf_uint32_read(bfmt);

    g_fentry_ptr->packet_type = ch_bf_uint8_read(bfmt);
    g_fentry_ptr->session_direct = ch_bf_uint8_read(bfmt);
    g_fentry_ptr->protocol_id = ch_bf_uint32_read(bfmt);
    g_fentry_ptr->session_id = ch_bf_uint64_read(bfmt);
    g_fentry_ptr->time = ch_bf_uint64_read(bfmt);
    g_fentry_ptr->src_ip = ch_bf_uint32_read(bfmt);
    g_fentry_ptr->dst_ip = ch_bf_uint32_read(bfmt);
    g_fentry_ptr->src_port = ch_bf_uint16_read(bfmt);
    g_fentry_ptr->dst_port = ch_bf_uint16_read(bfmt);
	g_fentry_ptr->data_offset = ch_bf_uint32_read(bfmt);

    ch_bf_bytes_read(bfmt,(unsigned char**)(&g_fentry_ptr->data),&g_fentry_ptr->data_len);

    return g_fentry_ptr;
}

static ch_mmap_file_entry_t * file_entry_next(ch_mmap_file_iterator_t *iter){

    ch_mmap_file_entry_t *fentry = NULL;
    ch_mmap_file_header_t *fheader = iter->fheader;
    ch_bin_format_t *bfmt = &fheader->bfmt;

    if(has_next(iter) == 0){
        return NULL;
    }

    fentry = _file_entry_read(iter,bfmt);

    if(fentry){

        iter->entries_count+=1;
    }

    return fentry;
}

static inline int file_header_read(ch_mmap_file_header_t *fheader,ch_bin_format_t *bfmt){

    fheader->magic = ch_bf_uint32_read(bfmt);
    if(fheader->magic!=FIMAGIC){
        ch_log(CH_LOG_ERR,"MMap file header Magic:%lu is invalid!",fheader->magic);
        return -1;
    }
    fheader->major_version = ch_bf_uint32_read(bfmt);
    fheader->minor_version = ch_bf_uint32_read(bfmt);
    fheader->entries_number = ch_bf_uint32_read(bfmt);
    fheader->entries_size = ch_bf_uint32_read(bfmt);

    return 0;
}

ch_mmap_file_iterator_t *ch_mmap_file_format_iterator_prefare(ch_mmap_file_format_t *mmfmt){

    ch_mmap_buf_t *mmb = &mmfmt->cur_fheader.mmb;
    g_fiter_ptr->fheader = NULL;
    g_fiter_ptr->next = file_entry_next;
    g_fiter_ptr->entries_count = 0;

    if( -1 == ch_mmap_buf_get(mmfmt->mm,mmb)){
        return NULL;
    }

    ch_bf_init(&mmfmt->cur_fheader.bfmt,mmb->start,mmb->end);

    if(-1 == file_header_read(&mmfmt->cur_fheader,&mmfmt->cur_fheader.bfmt))
        return NULL;

    g_fiter_ptr->fheader = &mmfmt->cur_fheader;

    return g_fiter_ptr;
}

void ch_mmap_file_format_iterator_commit(ch_mmap_file_format_t *mmfmt,ch_mmap_file_iterator_t *iter){

    if(iter->fheader){
        ch_mmap_buf_t *mmb = &iter->fheader->mmb;
        ch_mmap_buf_commit(mmfmt->mm,mmb);
        iter->fheader = NULL;
        iter->entries_count = 0;
    }

}

void ch_mmap_file_entry_dump(ch_mmap_file_entry_t *fe,FILE *out){

	char b[4096];
	char *bp = b;
	char ip[64];

	if(fe->data_len>4095){
	
		bp = (char*)malloc(fe->data_len+1);

	}

	fprintf(out,"Dump mmap file entry info:\n");
	fprintf(out,"Magic:%lu\n",(unsigned long)fe->magic);
	fprintf(out,"EntrySize:%lu\n",(unsigned long)fe->entry_size);
	fprintf(out,"packetType:%lu\n",(unsigned long)fe->packet_type);
	fprintf(out,"Direct:%lu\n",(unsigned long)fe->session_direct);
	fprintf(out,"ProtoID:%lu\n",(unsigned long)fe->protocol_id);
	fprintf(out,"SessionID:%lu\n",(unsigned long)fe->session_id);
	fprintf(out,"Time:%lu\n",(unsigned long)fe->time);
	fprintf(out,"srcIP:%s\n",ch_ipaddr_str_get(ip,64,fe->src_ip));
	fprintf(out,"dstIP:%s\n",ch_ipaddr_str_get(ip,64,fe->dst_ip));
	fprintf(out,"srcPort:%lu\n",(unsigned long)fe->src_port);
	fprintf(out,"dstPort:%lu\n",(unsigned long)fe->dst_port);
	fprintf(out,"Doffset:%lu\n",(unsigned long)fe->data_offset);
	fprintf(out,"Dlen:%lu\n",(unsigned long)fe->data_len);
	if(fe->data_len>0){
	
		memcpy(bp,fe->data,fe->data_len);
		fwrite(bp,fe->data_len,1,out);
		fprintf(out,"\n");

	}else{
	
		fprintf(out,"Data:%s\n","");

	}
	if(bp!=b){
	
		free(bp);
	}
}

/*************************************************************************
 > File Name: ch_mmap_packets_format.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月16日 星期四 11时25分29秒
 ************************************************************************/
#include "ch_log.h"
#include "ch_mmap_packets_format.h"
#include "ch_util.h"

static ch_mmap_packets_iterator_t g_piter,*g_piter_ptr = &g_piter;
static ch_mmap_packets_entry_t g_pentry,*g_pentry_ptr = &g_pentry;

ch_mmap_packets_format_t * 
ch_mmap_packets_format_create(apr_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,int is_write){

    ch_mmap_packets_format_t *mmfmt = NULL;
    ch_mmap_t *mm = NULL;

    mm = ch_mmap_create(mp,fname,fsize,entry_size,0,is_write); 

    if(mm == NULL){
        ch_log(CH_LOG_WARN,"Cannot create mmap for packets format!");
        return NULL;
    }

    mmfmt = (ch_mmap_packets_format_t*)apr_pcalloc(mp,sizeof(ch_mmap_packets_format_t));
    mmfmt->mp = mp;
    mmfmt->mm = mm;
    mmfmt->has_pheader = 0;
    mmfmt->entry_size = entry_size;
    return mmfmt;
}


void ch_mmap_packets_format_destroy(ch_mmap_packets_format_t *mmfmt){

    if(mmfmt->mm){

        ch_mmap_destroy(mmfmt->mm);
        mmfmt->mm = NULL;
    }
}

static inline int _is_pentry_size_over(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_entry_t *pentry){

    uint64_t rem_entry_size = mmfmt->entry_size-CH_MMAP_PACKETS_HEADER_LEN;
    uint64_t pentry_size = (uint64_t)pentry->entry_size;

    return pentry_size > rem_entry_size;
}

static inline void _mmap_packets_header_init(ch_mmap_packets_header_t *pheader){

    ch_bin_format_t *bfmt = &pheader->bfmt;
    ch_bf_init(bfmt,pheader->mmb.start,pheader->mmb.end);

    pheader->magic = PFIMAGIC;
    pheader->major_version = PMAJOR_VERSION;
    pheader->minor_version = PMINJOR_VERSION;
    pheader->entries_size = 0;
    pheader->entries_number = 0;

    ch_bf_skip(bfmt,CH_MMAP_PACKETS_HEADER_LEN);
}

static inline void _mmap_packets_header_commit(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_header_t *pheader){

    ch_bin_format_t *bfmt = &pheader->bfmt;
    ch_mmap_buf_t *mmb = &pheader->mmb;
    ch_bf_reset(bfmt);

    ch_bf_uint32_write(bfmt,pheader->magic);
    ch_bf_uint32_write(bfmt,pheader->major_version);
    ch_bf_uint32_write(bfmt,pheader->minor_version);
    ch_bf_uint32_write(bfmt,pheader->entries_number);
    ch_bf_uint32_write(bfmt,pheader->entries_size);

    ch_mmap_buf_commit(mmfmt->mm,mmb);
}

static int _mmap_packets_header_create(ch_mmap_packets_format_t *mmfmt){

    if(mmfmt->has_pheader){
        _mmap_packets_header_commit(mmfmt,&mmfmt->cur_pheader);
        mmfmt->has_pheader = 0;
    }

    if(-1 == ch_mmap_buf_get(mmfmt->mm,&(mmfmt->cur_pheader.mmb))){
        return -1;
    }
    mmfmt->has_pheader = 1;
    _mmap_packets_header_init(&mmfmt->cur_pheader);
    /*ok*/
    return 0;
}

static inline int _is_need_create_packets_header(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_entry_t *pentry){

    ch_bin_format_t *bfmt = &mmfmt->cur_pheader.bfmt;
    if(mmfmt->has_pheader == 0)
        return 1;

    if(ch_bf_full(bfmt,(size_t)pentry->entry_size))
        return 1;

    return 0;
}

static void _mmap_packets_entry_write(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_entry_t *pentry){

    unsigned char *wdata;

    ch_mmap_packets_header_t *pheader = &mmfmt->cur_pheader;
    ch_bin_format_t *bfmt = &pheader->bfmt;

    uint32_t bsize = (uint32_t)ch_bf_content_size(bfmt);

    ch_bf_uint32_write(bfmt,PEIMAGIC);
    ch_bf_uint32_write(bfmt,ch_mmap_packets_entry_len(pentry));
    ch_bf_uint8_write(bfmt,pentry->proto_id);
    ch_bf_uint8_write(bfmt,pentry->flags);
    ch_bf_uint32_write(bfmt,pentry->sent_seq);
    ch_bf_uint32_write(bfmt,pentry->recv_ack);
    ch_bf_uint64_write(bfmt,pentry->time);
    ch_bf_uint32_write(bfmt,pentry->src_ip);
    ch_bf_uint32_write(bfmt,pentry->dst_ip);
    ch_bf_uint16_write(bfmt,pentry->src_port);
    ch_bf_uint16_write(bfmt,pentry->dst_port);
	ch_bf_uint32_write(bfmt,pentry->p_size);

    wdata = pentry->data == NULL || pentry->data_len == 0?"":pentry->data;

    ch_bf_bytes_write(bfmt,wdata,pentry->data_len);

    uint32_t asize = (uint32_t)ch_bf_content_size(bfmt);
    pheader->entries_number+=1;
    pheader->entries_size += (asize-bsize);
}

int ch_mmap_packets_format_put(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_entry_t *pentry){

    if(_is_pentry_size_over(mmfmt,pentry)){

        ch_log(CH_LOG_ERR,"The entry size:%lu is too large!",pentry->entry_size);
        return -2;
    }

    if(_is_need_create_packets_header(mmfmt,pentry)){

        if(-1 == _mmap_packets_header_create(mmfmt)){

            ch_log(CH_LOG_WARN,"Cannot create a mmap buf to store pentry!");
            return -1;
        }
    }

    _mmap_packets_entry_write(mmfmt,pentry);

    /*ok*/

    return 0;
}

static inline int _can_been_flush(ch_mmap_packets_format_t *mmfmt){

    ch_bin_format_t *bfmt = &mmfmt->cur_pheader.bfmt;
	
	if(mmfmt->has_pheader == 0)
		return 0;

	if(ch_bf_empty(bfmt))
		return 0;

	return 1;
}

void ch_mmap_packets_format_flush(ch_mmap_packets_format_t *mmfmt){

	if(_can_been_flush(mmfmt)){
	
        if(-1 == _mmap_packets_header_create(mmfmt)){

			mmfmt->has_pheader = 0;
        }

	}
}

static inline int has_next(ch_mmap_packets_iterator_t *iter){

    if(iter->pheader == NULL)
        return 0;

    if(iter->entries_count>=iter->pheader->entries_number)
        return 0;

    return 1;
}

static inline ch_mmap_packets_entry_t * _packets_entry_read(ch_mmap_packets_iterator_t *iter,ch_bin_format_t *bfmt){

    g_pentry_ptr->magic = ch_bf_uint32_read(bfmt);
    if(g_pentry_ptr->magic!=PEIMAGIC){
        ch_log(CH_LOG_ERR,"Packets entry magic:%lu is invalid!",g_pentry_ptr->magic);
        iter->pheader = NULL;
        return NULL;
    }
    g_pentry_ptr->entry_size = ch_bf_uint32_read(bfmt);

    g_pentry_ptr->proto_id = ch_bf_uint8_read(bfmt);
    g_pentry_ptr->flags = ch_bf_uint8_read(bfmt);
    g_pentry_ptr->sent_seq = ch_bf_uint32_read(bfmt);
    g_pentry_ptr->recv_ack = ch_bf_uint32_read(bfmt);
    
	g_pentry_ptr->time = ch_bf_uint64_read(bfmt);
    g_pentry_ptr->src_ip = ch_bf_uint32_read(bfmt);
    g_pentry_ptr->dst_ip = ch_bf_uint32_read(bfmt);
    g_pentry_ptr->src_port = ch_bf_uint16_read(bfmt);
    g_pentry_ptr->dst_port = ch_bf_uint16_read(bfmt);
	g_pentry_ptr->p_size = ch_bf_uint32_read(bfmt);

    ch_bf_bytes_read(bfmt,(unsigned char**)(&g_pentry_ptr->data),&g_pentry_ptr->data_len);

    return g_pentry_ptr;
}

static ch_mmap_packets_entry_t * packets_entry_next(ch_mmap_packets_iterator_t *iter){

    ch_mmap_packets_entry_t *pentry = NULL;
    ch_mmap_packets_header_t *pheader = iter->pheader;
    ch_bin_format_t *bfmt = &pheader->bfmt;

    if(has_next(iter) == 0){
        return NULL;
    }

    pentry = _packets_entry_read(iter,bfmt);

    if(pentry){

        iter->entries_count+=1;
    }

    return pentry;
}

static inline int packets_header_read(ch_mmap_packets_header_t *pheader,ch_bin_format_t *bfmt){

    pheader->magic = ch_bf_uint32_read(bfmt);
    if(pheader->magic!=PFIMAGIC){
        ch_log(CH_LOG_ERR,"MMap packets header Magic:%lu is invalid!",pheader->magic);
        return -1;
    }
    pheader->major_version = ch_bf_uint32_read(bfmt);
    pheader->minor_version = ch_bf_uint32_read(bfmt);
    pheader->entries_number = ch_bf_uint32_read(bfmt);
    pheader->entries_size = ch_bf_uint32_read(bfmt);

    return 0;
}

ch_mmap_packets_iterator_t *ch_mmap_packets_format_iterator_prefare(ch_mmap_packets_format_t *mmfmt){

    ch_mmap_buf_t *mmb = &mmfmt->cur_pheader.mmb;
    g_piter_ptr->pheader = NULL;
    g_piter_ptr->next = packets_entry_next;
    g_piter_ptr->entries_count = 0;

    if( -1 == ch_mmap_buf_get(mmfmt->mm,mmb)){
        return NULL;
    }

    ch_bf_init(&mmfmt->cur_pheader.bfmt,mmb->start,mmb->end);

    if(-1 == packets_header_read(&mmfmt->cur_pheader,&mmfmt->cur_pheader.bfmt))
        return NULL;

    g_piter_ptr->pheader = &mmfmt->cur_pheader;

    return g_piter_ptr;
}

void ch_mmap_packets_format_iterator_commit(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_iterator_t *iter){

    if(iter->pheader){
        ch_mmap_buf_t *mmb = &iter->pheader->mmb;
        ch_mmap_buf_commit(mmfmt->mm,mmb);
        iter->pheader = NULL;
        iter->entries_count = 0;
    }

}

void ch_mmap_packets_entry_dump(ch_mmap_packets_entry_t *pe,FILE *out){

	char b[4096];
	char *bp = b;
	char ip[64];

	if(pe->data_len>4095){
	
		bp = (char*)malloc(pe->data_len+1);

	}

	fprintf(out,"Dump mmap packets entry info:\n");
	fprintf(out,"Magic:%lu\n",(unsigned long)pe->magic);
	fprintf(out,"EntrySize:%lu\n",(unsigned long)pe->entry_size);
	fprintf(out,"protoID:%lu\n",(unsigned long)pe->proto_id);
	fprintf(out,"flags:%lu\n",(unsigned long)pe->flags);
	fprintf(out,"sentSeq:%lu\n",(unsigned long)pe->sent_seq);
	fprintf(out,"recvACK:%lu\n",(unsigned long)pe->recv_ack);
	fprintf(out,"Time:%lu\n",(unsigned long)pe->time);
	fprintf(out,"srcIP:%s\n",ch_ipaddr_str_get(ip,64,pe->src_ip));
	fprintf(out,"dstIP:%s\n",ch_ipaddr_str_get(ip,64,pe->dst_ip));
	fprintf(out,"srcPort:%lu\n",(unsigned long)pe->src_port);
	fprintf(out,"dstPort:%lu\n",(unsigned long)pe->dst_port);
	fprintf(out,"packetSize:%lu\n",(unsigned long)pe->p_size);
	fprintf(out,"Dlen:%lu\n",(unsigned long)pe->data_len);
	if(pe->data_len>0){
	
		memcpy(bp,pe->data,pe->data_len);
		fwrite(bp,pe->data_len,1,out);
		fprintf(out,"\n");

	}else{
	
		fprintf(out,"Data:%s\n","");

	}
	if(bp!=b){
	
		free(bp);
	}
}

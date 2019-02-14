/*
 *
 *      Filename: ch_packet_record_reader.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-09 15:09:31
 * Last Modified: 2018-07-17 10:34:30
 */

#include "ch_packet_record_reader.h"
#include "ch_jni_util.h"
#include "ch_mpool.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"

#define MAX_PKT_RCD_CONTEXT_N 64

typedef struct ch_packet_record_reader_context_t ch_packet_record_reader_context_t;


struct ch_packet_record_reader_context_t {
	ch_pool_t *mp;
	ch_shm_format_t *shm_fmt;
	ch_shm_entry_iterator_t *eiter;
};

static ch_packet_record_reader_context_t pkt_rcd_contexts[MAX_PKT_RCD_CONTEXT_N];

static inline ch_packet_record_reader_context_t * _pkt_rcd_context_get(int id){

	if(id >= MAX_PKT_RCD_CONTEXT_N||id<0)
		return NULL;

	return &pkt_rcd_contexts[id];
}


/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    openMMap
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_openMMap
  (JNIEnv *jenv, jobject jthis, jint id, jstring jfname){

	  ch_packet_record_reader_context_t *pcontext;
	  ch_shm_format_t *shm_fmt;
	  ch_pool_t *mp;
	  const char *open_fname;
	  
	  pcontext = _pkt_rcd_context_get(id);
	  if(pcontext == NULL)
		  return -1;

	  open_fname = ch_string_arg_get(jenv,jfname);
	  if(open_fname == NULL || strlen(open_fname) == 0)
		  return -1;
	  /*create global apr memory pool*/
	  mp = ch_pool_create(1024);
	  if(mp == NULL){
		  return -1;
	  }
	  
	  shm_fmt = ch_shm_format_pkt_with_mmap_create(mp,open_fname,0,0,0,0);
	  if(shm_fmt == NULL)
		  return -1;


	  pcontext->mp = mp;

	  pcontext->shm_fmt = shm_fmt;

	  pcontext->eiter = NULL;

    
	  return 0;
  
  }

	  
/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    openSHM
 * Signature: (ILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_openSHM
  (JNIEnv *jenv, jobject jthis, jint id, jstring key, jint proj_id){

	  ch_packet_record_reader_context_t *pcontext;
	  ch_shm_format_t *shm_fmt;
	  ch_pool_t *mp;
	  
	  const char *open_key;
	  
	  pcontext = _pkt_rcd_context_get(id);
	  if(pcontext == NULL)
		  return -1;
	  
	  open_key = ch_string_arg_get(jenv,key);

	  if(open_key == NULL|| strlen(open_key) == 0)
		  return -1;
	  /*create global apr memory pool*/
	  
	  mp = ch_pool_create(1024);
	  if(mp == NULL){
		  return -1;
	  }
	  
	  shm_fmt = ch_shm_format_pkt_with_shm_create(mp,open_key,proj_id,0,0,0,0);
	  if(shm_fmt == NULL)
		  return -1;
	  
	  pcontext->mp = mp;

	  pcontext->shm_fmt = shm_fmt;

	  pcontext->eiter = NULL;
	  
	  return 0;
  
  }

static inline int _prepare_iter_ok(ch_packet_record_reader_context_t *pcontext){

	if(pcontext == NULL)
		return 0;

    if(pcontext->eiter)
		return 1;

    pcontext->eiter = ch_shm_entry_iterator_prefare(pcontext->shm_fmt);

    if(pcontext->eiter == NULL)
		return 0;
     
    return 1;
}

/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    read
 * Signature: (ILcom/antell/cloudhands/api/packet/PacketRecord;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_read
  (JNIEnv *jenv, jobject jthis, jint id, jobject jpacketRecord){

	  ch_packet_record_reader_context_t *pcontext = NULL;
	  ch_shm_format_t *shm_fmt;
	  ch_shm_entry_iterator_t *eiter;

	  ch_packet_record_t *pkt_rcd;
	  ch_shm_record_t *shm_rcd;
	  
	  pcontext = _pkt_rcd_context_get(id);
	  if(pcontext == NULL)
		  return -1;

     if(_prepare_iter_ok(pcontext) == 0){
		 /* no data to be read */
		 return -2;
     }

	 shm_fmt = pcontext->shm_fmt;
	 eiter = pcontext->eiter;

     shm_rcd = eiter->next(eiter);

	 if(shm_rcd == NULL){
		 /* read one data chunk completely*/
		 ch_shm_entry_iterator_commit(shm_fmt,eiter);

		 pcontext->eiter = NULL;
		 return -1;
	 } 

	 pkt_rcd = (ch_packet_record_t*)shm_rcd;
     /* read ok */
	 
	 ch_set_int_field(jenv,jpacketRecord,"type",(int)pkt_rcd->type);
	 ch_set_int_field(jenv,jpacketRecord,"dataSize",(int)shm_rcd->data_len);
	 ch_set_int_field(jenv,jpacketRecord,"metaDataSize",(int)pkt_rcd->meta_data_size);

	 ch_set_long_field(jenv,jpacketRecord,"time",(long)pkt_rcd->time);
	 
	 if(shm_rcd->data_len>0)
		 ch_send_data_to_java(jenv,jpacketRecord,shm_rcd->data,(long)shm_rcd->data_len);

	return 0;
  
  }

  
/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_close
  (JNIEnv *jenv, jobject jthis, jint id){

      ch_packet_record_reader_context_t *pcontext = NULL;

	  pcontext = _pkt_rcd_context_get(id);
	  if(pcontext == NULL)
		  return;

	  if(pcontext->eiter){
	  
		  ch_shm_entry_iterator_commit(pcontext->shm_fmt,pcontext->eiter);
	  }

	  ch_pool_destroy(pcontext->mp);

  }


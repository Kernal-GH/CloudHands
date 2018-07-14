/*
 *
 *      Filename: ch_packet_record_reader.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-09 15:09:31
 * Last Modified: 2018-07-14 18:58:28
 */

#include "ch_packet_record_reader.h"
#include "ch_jni_util.h"
#include "ch_mpool.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"
#include "ch_jni_context_pool.h"
#include <pthread.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct ch_packet_record_reader_context_t ch_packet_record_reader_context_t;


struct ch_packet_record_reader_context_t {
	ch_jni_context_t jcontext;

	ch_pool_t *mp;
	ch_shm_format_t *shm_fmt;
	ch_shm_entry_iterator_t *eiter;
};

static ch_jni_context_pool_t _pkt_reader_cp = {
	{
		.next = &_pkt_reader_cp.contexts,
		.prev = &_pkt_reader_cp.contexts
	},
	.context_size = sizeof(ch_packet_record_reader_context_t)
};

static ch_jni_context_pool_t *g_pkt_reader_context_pool = &_pkt_reader_cp;


/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    openMMap
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_openMMap
  (JNIEnv *jenv, jobject jthis, jstring jfname){
	  
	  ch_packet_record_reader_context_t *pcontext;
	  ch_shm_format_t *shm_fmt;
	  ch_pool_t *mp;
	  const char *open_fname = ch_string_arg_get(jenv,jfname);
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

	  pthread_mutex_lock(&lock);
	  pcontext = (ch_packet_record_reader_context_t*)ch_jni_context_pool_create(g_pkt_reader_context_pool,jenv);
	  pthread_mutex_unlock(&lock);
	
	  if(pcontext == NULL)
		  return -1;


	  pcontext->mp = mp;

	  pcontext->shm_fmt = shm_fmt;

	  pcontext->eiter = NULL;

    
	  return 0;
  
  }

/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    openSHM
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_openSHM
  (JNIEnv *jenv, jobject jthis, jstring key, jint proj_id){
	  
	  ch_packet_record_reader_context_t *pcontext;
	  ch_shm_format_t *shm_fmt;
	  ch_pool_t *mp;
	  
	  const char *open_key = ch_string_arg_get(jenv,key);

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
	  
	  pthread_mutex_lock(&lock);
	  pcontext = (ch_packet_record_reader_context_t*)ch_jni_context_pool_create(g_pkt_reader_context_pool,jenv);
	  pthread_mutex_unlock(&lock);
	
	  if(pcontext == NULL)
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
 * Signature: (Lcom/antell/cloudhands/api/packet/PacketRecord;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_read
  (JNIEnv *jenv, jobject jthis, jobject jpacketRecord){
	  
	  ch_packet_record_reader_context_t *pcontext = NULL;
	  ch_shm_format_t *shm_fmt;
	  ch_shm_entry_iterator_t *eiter;

	  ch_packet_record_t *pkt_rcd;
	  ch_shm_record_t *shm_rcd;
	  
	  pthread_mutex_lock(&lock);
	  pcontext = (ch_packet_record_reader_context_t*)ch_jni_context_pool_find(g_pkt_reader_context_pool,jenv);
	  pthread_mutex_unlock(&lock);

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
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_close
  (JNIEnv *jenv, jobject jthis){
  
	  ch_packet_record_reader_context_t *pcontext = NULL;

	  pthread_mutex_lock(&lock);
	  pcontext = (ch_packet_record_reader_context_t*)ch_jni_context_pool_find(g_pkt_reader_context_pool,jenv);
	  pthread_mutex_unlock(&lock);

	  if(pcontext == NULL)
		  return;

	  if(pcontext->eiter){
	  
		  ch_shm_entry_iterator_commit(pcontext->shm_fmt,pcontext->eiter);
	  }

	  ch_pool_destroy(pcontext->mp);

	  pthread_mutex_lock(&lock);
	  ch_jni_context_pool_remove(g_pkt_reader_context_pool,(ch_jni_context_t*)pcontext); 
	  pthread_mutex_unlock(&lock);
  }


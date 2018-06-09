/*
 *
 *      Filename: ch_packet_record_reader.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-09 15:09:31
 * Last Modified: 2018-06-09 15:20:32
 */

#include "ch_packet_record_reader.h"
#include "ch_jni_util.h"
#include "ch_mpool.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"

static ch_shm_format_t *shm_fmt = NULL;
static ch_shm_entry_iterator_t *eiter = NULL;

/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    openMMap
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_openMMap
  (JNIEnv *jenv, jobject jthis, jstring jfname){
  
	/*unused*/
	jthis = jthis;

    ch_pool_t *mp;
    const char *open_fname = (*jenv)->GetStringUTFChars(jenv,jfname, 0);
    
    /*create global apr memory pool*/
    mp = ch_pool_create(1024);

    if(mp == NULL){
		return -1;
    }

	shm_fmt = ch_shm_format_pkt_with_mmap_create(mp,open_fname,0,0,0,0);

    if(shm_fmt == NULL)
		return -1;

    return 0;
  
  }

/*
 * Class:     com_antell_cloudhands_api_packet_PacketRecordReader
 * Method:    openSHM
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_PacketRecordReader_openSHM
  (JNIEnv *jenv, jobject jthis, jstring key, jint proj_id){
  
	  /*unused*/
	  jthis = jthis;
    
	  ch_pool_t *mp;
	  
	  const char *open_key = (*jenv)->GetStringUTFChars(jenv,key, 0);
    
    /*create global apr memory pool*/
    mp = ch_pool_create(1024);

    if(mp == NULL){
		return -1;
    }

	shm_fmt = ch_shm_format_pkt_with_shm_create(mp,open_key,proj_id,0,0,0,0);

    if(shm_fmt == NULL)
		return -1;

    return 0;
  
  }

static inline int _prepare_iter_ok(void){

    if(eiter)
		return 1;

    eiter = ch_shm_entry_iterator_prefare(shm_fmt);

    if(eiter == NULL)
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
  
	  /*unused*/
	  jthis = jthis;

	  ch_packet_record_t *pkt_rcd;
	  ch_shm_record_t *shm_rcd;

     if(_prepare_iter_ok() == 0){
		 /* no data to be read */
		 return -2;
     }

     shm_rcd = eiter->next(eiter);

	 if(shm_rcd == NULL){
		 /* read one data chunk completely*/
		 ch_shm_entry_iterator_commit(shm_fmt,eiter);

		 eiter = NULL;
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
  
	  /*unused*/
	  jenv = jenv;
	  jthis = jthis;

	  if(eiter){
	  
		  ch_shm_entry_iterator_commit(shm_fmt,eiter);
	  }

	  ch_pool_destroy(shm_fmt->mp);
  
  }


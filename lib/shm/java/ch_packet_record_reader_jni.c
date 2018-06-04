/*
 *
 *      Filename: ch_packet_record_reader_jni.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-16 18:25:38
 * Last Modified: 2018-04-16 19:47:19
 */

#include "ch_mpool.h"
#include "ch_packet_record_reader_jni.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"

static ch_shm_format_t *shm_fmt = NULL;
static ch_shm_entry_iterator_t *eiter = NULL;


/*
 * Class:     com_antell_feye_jni_PacketRecordReader
 * Method:    openMMap
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_feye_jni_PacketRecordReader_openMMap
  (JNIEnv * jenv, jobject jthis, jstring jfname){
  
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
 * Class:     com_antell_feye_jni_PacketRecordReader
 * Method:    openSHM
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_feye_jni_PacketRecordReader_openSHM
  (JNIEnv * jenv, jobject jthis, jstring key, jint proj_id){
  


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

static inline void _set_int_field(JNIEnv *jenv,jobject jpacketRecord,const char *f,int v){

	jclass cls = (*jenv)->GetObjectClass(jenv,jpacketRecord);

	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, f, "I");
	(*jenv)->SetIntField(jenv,jpacketRecord,fid,v);
}

static inline void _set_long_field(JNIEnv *jenv,jobject jpacketRecord,const char *f,long v){

	jclass cls = (*jenv)->GetObjectClass(jenv,jpacketRecord);
	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, f, "J");
	(*jenv)->SetLongField(jenv,jpacketRecord,fid,v);
}

static inline void _send_data_to_java(JNIEnv *jenv,jobject jpacketRecord,void *data,long dlen){

	jclass cls = (*jenv)->GetObjectClass(jenv,jpacketRecord);
	/*call java function updateByteBuffer to adjust the buf*/
	jmethodID upfun = (*jenv)->GetMethodID(jenv,cls,"updateByteBuffer","(J)V");
	(*jenv)->CallVoidMethod(jenv,jpacketRecord,upfun,dlen);

	/*copy data into java bytebuffer*/
	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, "dataBuffer", "Ljava/nio/ByteBuffer;");
	jobject jb = (*jenv)->GetObjectField(jenv,jpacketRecord, fid);
	void *jb_ptr = (void*)(*jenv)->GetDirectBufferAddress(jenv,jb);

	memcpy(jb_ptr,data,dlen);

}

/*
 * Class:     com_antell_feye_jni_PacketRecordReader
 * Method:    read
 * Signature: (Lcom/antell/feye/jni/PacketRecord;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_feye_jni_PacketRecordReader_read
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
	 
	 _set_int_field(jenv,jpacketRecord,"type",(int)pkt_rcd->type);
	 _set_int_field(jenv,jpacketRecord,"dataSize",(int)shm_rcd->data_len);
	 _set_int_field(jenv,jpacketRecord,"metaDataSize",(int)pkt_rcd->meta_data_size);

	 _set_long_field(jenv,jpacketRecord,"time",(long)pkt_rcd->time);
	 
	 if(shm_rcd->data_len>0)
		 _send_data_to_java(jenv,jpacketRecord,shm_rcd->data,(long)shm_rcd->data_len);

	return 0;
  }

/*
 * Class:     com_antell_feye_jni_PacketRecordReader
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_antell_feye_jni_PacketRecordReader_close
  (JNIEnv *jenv, jobject jthis){
  
	  /*unused*/
	  jenv = jenv;
	  jthis = jthis;

	  if(eiter){
	  
		  ch_shm_entry_iterator_commit(shm_fmt,eiter);
	  }

	  ch_pool_destroy(shm_fmt->mp);

  
  }

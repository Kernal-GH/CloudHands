/*
 *
 *      Filename: ch_packet_reader_jni.c
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-06-02 10:40:25
 * Last Modified: 2017-06-02 17:06:19
 */


#include <apr_pools.h>
#include "ch_packet_reader_jni.h"
#include "ch_mmap_packets_format.h"

static ch_mmap_packets_format_t *pmft = NULL;
static ch_mmap_packets_iterator_t *piter = NULL;

/*
 * Class:     com_antell_security_stream_analysis_jni_PacketReader
 * Method:    open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_security_stream_analysis_jni_PacketReader_open
  (JNIEnv *jenv, jobject jthis, jstring jfname){

	jthis = jthis;
    apr_pool_t *mp;
    const char *open_fname = (*jenv)->GetStringUTFChars(jenv,jfname, 0);
    
    int argc =1;
    const char *argv[1]={"jni"};

    /*init apr app runtine*/
    apr_app_initialize(&argc,&argv,NULL);

    /*create global apr memory pool*/
    apr_pool_create(&mp,NULL);

    if(mp == NULL){
		return -1;
    }

    pmft = ch_mmap_packets_format_create(mp,open_fname,0,0,0);
    if(pmft == NULL)
		return -1;

    return 0;
}

static inline int _prepare_iter_ok(void){

    if(piter)
		return 1;

    piter = ch_mmap_packets_format_iterator_prefare(pmft);
    if(piter == NULL)
		return 0;
     
    return 1;
}

static inline void _set_int_field(JNIEnv *jenv,jobject jpentry,const char *f,int v){

	jclass cls = (*jenv)->GetObjectClass(jenv,jpentry);

	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, f, "I");
	(*jenv)->SetIntField(jenv,jpentry,fid,v);
}

static inline void _set_long_field(JNIEnv *jenv,jobject jpentry,const char *f,long v){

	jclass cls = (*jenv)->GetObjectClass(jenv,jpentry);
	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, f, "J");
	(*jenv)->SetLongField(jenv,jpentry,fid,v);
}

static inline void _send_data_to_java(JNIEnv *jenv,jobject jpentry,void *data,long dlen){

	jclass cls = (*jenv)->GetObjectClass(jenv,jpentry);
	/*call java function updateByteBufper to adjust the buf*/
	jmethodID upfun = (*jenv)->GetMethodID(jenv,cls,"updateByteBuffer","(J)V");
	(*jenv)->CallVoidMethod(jenv,jpentry,upfun,dlen);

	/*copy data into java bytebuffer*/
	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, "dataBuf", "Ljava/nio/ByteBuffer;");
	jobject jb = (*jenv)->GetObjectField(jenv,jpentry, fid);
	void *jb_ptr = (void*)(*jenv)->GetDirectBufferAddress(jenv,jb);

	memcpy(jb_ptr,data,dlen);

}

/*
 * Class:     com_antell_security_stream_analysis_jni_PacketReader
 * Method:    read
 * Signature: (Lcom/antell/security/stream/analysis/jni/PacketEntry;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_security_stream_analysis_jni_PacketReader_read
  (JNIEnv *jenv, jobject jthis, jobject jpentry){

     ch_mmap_packets_entry_t *pe;
	 jthis = jthis;

     if(_prepare_iter_ok() == 0){
		 /* no data to be read */
		 return -2;
     }

     pe = piter->next(piter);

	 if(pe == NULL){
		 /* read one data chunk */
		 ch_mmap_packets_format_iterator_commit(pmft,piter);
		 piter = NULL;
		 return -1;
	 } 
     /* read ok */
	 
	 _set_int_field(jenv,jpentry,"magic",(int)pe->magic);
	 _set_int_field(jenv,jpentry,"entrySize",(int)pe->entry_size);
	 _set_int_field(jenv,jpentry,"protoID",pe->proto_id);
	 _set_int_field(jenv,jpentry,"flags",(int)pe->flags);

	 _set_long_field(jenv,jpentry,"sentSeq",(long)pe->sent_seq);
	 _set_long_field(jenv,jpentry,"recvAck",(long)pe->recv_ack);
	 _set_long_field(jenv,jpentry,"time",(long)pe->time);
	 
	 _set_long_field(jenv,jpentry,"srcIP",(long)pe->src_ip);
	 _set_long_field(jenv,jpentry,"dstIP",(long)pe->dst_ip);
	 _set_int_field(jenv,jpentry,"srcPort",(int)pe->src_port);
	 _set_int_field(jenv,jpentry,"dstPort",(int)pe->dst_port);
     
	 _set_long_field(jenv,jpentry,"psize",(long)pe->p_size);
	 _set_long_field(jenv,jpentry,"dataSize",(long)pe->data_len);
	 
	 if(pe->data_len>0)
		 _send_data_to_java(jenv,jpentry,pe->data,(long)pe->data_len);

	return 0;
}
/*
 * Class:     com_antell_security_stream_analysis_jni_PacketReader
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_antell_security_stream_analysis_jni_PacketReader_close
  (JNIEnv * jenv, jobject jthis){

	  jenv = jenv;

	  jthis = jthis;
	  if(piter){
	  
		  ch_mmap_packets_format_iterator_commit(pmft,piter);
	  }

	  apr_pool_destroy(pmft->mp);

	  ch_mmap_packets_format_destroy(pmft);

}






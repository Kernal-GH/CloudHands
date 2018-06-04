/*
 * =====================================================================================
 *
 *       Filename:  ch_session_packet_reader_jni.c
 *
 *    Description:  for java to read assemble data!
 *
 *        Version:  1.0
 *        Created:  2017年03月24日 13时22分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), 15101157948@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <apr_pools.h>
#include "ch_session_packet_reader_jni.h"
#include "ch_mmap_file_format.h"

static ch_mmap_file_format_t *mff = NULL;
static ch_mmap_file_iterator_t *fiter = NULL;


JNIEXPORT jint JNICALL Java_com_antell_security_jni_SessionPacketReader_open
  (JNIEnv * jenv, jobject jthis, jstring jfname){

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

    mff = ch_mmap_file_format_create(mp,open_fname,0,0,0);
    if(mff == NULL)
		return -1;

    return 0;
}

static inline int _prepare_iter_ok(void){

    if(fiter)
		return 1;

    fiter = ch_mmap_file_format_iterator_prefare(mff);
    if(fiter == NULL)
		return 0;
     
    return 1;
}

static inline void _set_int_field(JNIEnv *jenv,jobject jsentry,const char *f,int v){

	jclass cls = (*jenv)->GetObjectClass(jenv,jsentry);

	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, f, "I");
	(*jenv)->SetIntField(jenv,jsentry,fid,v);
}

static inline void _set_long_field(JNIEnv *jenv,jobject jsentry,const char *f,long v){

	jclass cls = (*jenv)->GetObjectClass(jenv,jsentry);
	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, f, "J");
	(*jenv)->SetLongField(jenv,jsentry,fid,v);
}

static inline void _send_data_to_java(JNIEnv *jenv,jobject jsentry,void *data,long dlen){

	jclass cls = (*jenv)->GetObjectClass(jenv,jsentry);
	/*call java function updateByteBuffer to adjust the buf*/
	jmethodID upfun = (*jenv)->GetMethodID(jenv,cls,"updateByteBuffer","(J)V");
	(*jenv)->CallVoidMethod(jenv,jsentry,upfun,dlen);

	/*copy data into java bytebuffer*/
	jfieldID fid = (*jenv)->GetFieldID(jenv,cls, "dataBuf", "Ljava/nio/ByteBuffer;");
	jobject jb = (*jenv)->GetObjectField(jenv,jsentry, fid);
	void *jb_ptr = (void*)(*jenv)->GetDirectBufferAddress(jenv,jb);

	memcpy(jb_ptr,data,dlen);

}

JNIEXPORT jint JNICALL Java_com_antell_security_jni_SessionPacketReader_read
  (JNIEnv *jenv, jobject jthis, jobject jsentry){

     ch_mmap_file_entry_t *fe;
	 jthis = jthis;

     if(_prepare_iter_ok() == 0){
		 /* no data to be read */
		 return -2;
     }

     fe = fiter->next(fiter);

	 if(fe == NULL){
		 /* read one data chunk */
		 ch_mmap_file_format_iterator_commit(mff,fiter);
		 fiter = NULL;
		 return -1;
	 } 
     /* read ok */
	 
	 _set_int_field(jenv,jsentry,"magic",(int)fe->magic);
	 _set_int_field(jenv,jsentry,"entrySize",(int)fe->entry_size);
	 _set_int_field(jenv,jsentry,"packetType",(int)fe->packet_type);
	 _set_int_field(jenv,jsentry,"sessionDirect",(int)fe->session_direct);
	 _set_int_field(jenv,jsentry,"protoID",fe->protocol_id);

	 _set_long_field(jenv,jsentry,"sessionID",(long)fe->session_id);
	 _set_long_field(jenv,jsentry,"time",(long)fe->time);
	 
	 _set_long_field(jenv,jsentry,"srcIP",(long)fe->src_ip);
	 _set_long_field(jenv,jsentry,"dstIP",(long)fe->dst_ip);
	 _set_int_field(jenv,jsentry,"srcPort",(int)fe->src_port);
	 _set_int_field(jenv,jsentry,"dstPort",(int)fe->dst_port);
     
	 _set_long_field(jenv,jsentry,"dataOffset",(long)fe->data_offset);
	 _set_long_field(jenv,jsentry,"dataSize",(long)fe->data_len);
	 
	 if(fe->data_len>0)
		 _send_data_to_java(jenv,jsentry,fe->data,(long)fe->data_len);

	return 0;
}

JNIEXPORT void JNICALL Java_com_antell_security_jni_SessionPacketReader_close
  (JNIEnv * jenv, jobject jthis){

	  jenv = jenv;

	  jthis = jthis;
	  if(fiter){
	  
		  ch_mmap_file_format_iterator_commit(mff,fiter);
	  }

	  apr_pool_destroy(mff->mp);

	  ch_mmap_file_format_destroy(mff);

}

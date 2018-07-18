/*
 *
 *      Filename: ch_session_monitor_cmd.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-14 14:05:35
 * Last Modified: 2018-07-18 16:31:45
 */

#include "ch_session_monitor.h"
#include "ch_session_monitor_cmd.h"
#include "ch_jni_util.h"

#define MAX_SESSION_MONITOR_CONTEXT_N 32

typedef struct ch_session_monitor_context_t ch_session_monitor_context_t;

struct ch_session_monitor_context_t {

	ch_session_monitor_t monitor;

};

static ch_session_monitor_context_t smon_contexts[MAX_SESSION_MONITOR_CONTEXT_N];

static inline ch_session_monitor_context_t* _smon_context_get(int context_id){

	if(context_id>=MAX_SESSION_MONITOR_CONTEXT_N||context_id<0)
		return NULL;

	return &smon_contexts[context_id];
}



/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    open
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_open
  (JNIEnv *jenv, jobject jthis, jint context_id, jstring jmmap_fname){

	  ch_session_monitor_context_t *context;

	  const char *mmap_fname = ch_string_arg_get(jenv,jmmap_fname);

	  if(mmap_fname == NULL || strlen(mmap_fname) == 0)
		  return -1;

	  context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;

	  if(ch_session_monitor_load(&context->monitor,mmap_fname,0))
		  return -1;

	  return 0;

  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_close
  (JNIEnv *jenv, jobject jthis, jint context_id) {


	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return;

	  ch_session_monitor_exit(&context->monitor);

  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (IJJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__IJJ
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong ip, jlong tv){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;
	  
	  return ch_session_monitor_item_add_ip(&context->monitor,ip,tv);
  }

 
/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (IIJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__IIJ
  (JNIEnv *jenv, jobject jthis, jint context_id, jint port, jlong tv){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;

	  return ch_session_monitor_item_add_port(&context->monitor,port,tv);
  }

 
/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (IJIJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__IJIJ
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong ip, jint port, jlong tv){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;

	  return ch_session_monitor_item_add_ip_port(&context->monitor,ip,port,tv);
  }

  
/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (IJIJIJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__IJIJIJ
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong src_ip, jint src_port, jlong dst_ip, jint dst_port, jlong tv){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;

	  return ch_session_monitor_item_add_session(&context->monitor,src_ip,dst_ip,src_port,dst_port,tv);
  }


static inline void _item_fill(JNIEnv *jenv,jobject jmonitor_item,ch_session_monitor_item_t *item){

	ch_set_long_field(jenv,jmonitor_item,"id",(long)item->id);
	ch_set_long_field(jenv,jmonitor_item,"srcIP",(long)item->src_ip);
	ch_set_long_field(jenv,jmonitor_item,"dstIP",(long)item->dst_ip);

	ch_set_int_field(jenv,jmonitor_item,"srcPort",(int)item->src_port);
	ch_set_int_field(jenv,jmonitor_item,"dstPort",(int)item->dst_port);

	ch_set_long_field(jenv,jmonitor_item,"timeTV",(long)item->monitor_time_tv);
	ch_set_long_field(jenv,jmonitor_item,"startTime",(long)item->monitor_start_time);
	ch_set_long_field(jenv,jmonitor_item,"lastTime",(long)item->monitor_last_time);

	ch_set_int_field(jenv,jmonitor_item,"type",(int)item->monitor_type);
	ch_set_int_field(jenv,jmonitor_item,"state",(int)item->monitor_state);

}

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    find
 * Signature: (ILcom/antell/cloudhands/api/smon/SessionMonitorItem;J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_find__ILcom_antell_cloudhands_api_smon_SessionMonitorItem_2J
  (JNIEnv * jenv, jobject jthis, jint context_id, jobject jmonitor_item, jlong id){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;

	  ch_session_monitor_item_t *item = ch_session_monitor_item_findById(&context->monitor,id);
	  if(item == NULL)
		  return -1;

	  _item_fill(jenv,jmonitor_item,item);

	  return 0;
  }


/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    find
 * Signature: (ILcom/antell/cloudhands/api/smon/SessionMonitorItem;JIJI)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_find__ILcom_antell_cloudhands_api_smon_SessionMonitorItem_2JIJI
  (JNIEnv *jenv, jobject jthis, jint context_id, jobject jmonitor_item, jlong src_ip, jint src_port, jlong dst_ip, jint dst_port){


	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;
	  
	  ch_session_monitor_item_t *item = ch_session_monitor_item_find(&context->monitor,src_ip,dst_ip,src_port,dst_port);
	  if(item == NULL)
		  return -1;

	  _item_fill(jenv,jmonitor_item,item);

	  return 0;
  
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    delete
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_delete
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong id){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;
	  
	  ch_session_monitor_item_del(&context->monitor,id);

	  return 0;
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    stop
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_stop
  (JNIEnv * jenv, jobject jthis, jint context_id, jlong id){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return;

	  ch_session_monitor_item_stop(&context->monitor,id);

  }

  
/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    restart
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_restart
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong id){

	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return;

	  ch_session_monitor_item_restart(&context->monitor,id);

  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    get
 * Signature: (ILcom/antell/cloudhands/api/smon/SessionMonitorItem;I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_get
  (JNIEnv *jenv, jobject jthis, jint context_id, jobject jmonitor_item, jint index){
  
	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;

	  ch_session_monitor_item_t *item = ch_session_monitor_item_get(&context->monitor,index);
	  if(item == NULL)
		  return -1;

	  _item_fill(jenv,jmonitor_item,item);

	  return 0;

  }


/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    count
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_count
  (JNIEnv *jenv, jobject jthis, jint context_id){
  
	  ch_session_monitor_context_t *context = _smon_context_get(context_id);

	  if(context == NULL)
		  return -1;


	  return ch_session_monitor_item_count(&context->monitor);

  }


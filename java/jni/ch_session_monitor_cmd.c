/*
 *
 *      Filename: ch_session_monitor_cmd.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-14 14:05:35
 * Last Modified: 2018-07-14 14:56:15
 */

#include "ch_session_monitor.h"
#include "ch_session_monitor_cmd.h"
#include "ch_jni_util.h"

static ch_session_monitor_t tmp_monitor,*monitor = &tmp_monitor;

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_open
  (JNIEnv *jenv, jobject jthis, jstring jmmap_fname){
  
	  const char *mmap_fname = ch_string_arg_get(jenv,jmmap_fname);
	  if(mmap_fname == NULL || strlen(mmap_fname) == 0)
		  return -1;

	  return ch_session_monitor_load(monitor,mmap_fname,0);

  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_close
  (JNIEnv *jenv, jobject jthis){
  
	  ch_session_monitor_exit(monitor);

  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__JJ
  (JNIEnv *jenv, jobject jthis, jlong ip, jlong tv){
  
	  return ch_session_monitor_item_add_ip(monitor,ip,tv);
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (IJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__IJ
  (JNIEnv *jenv, jobject jthis, jint port, jlong tv){
  
	  return ch_session_monitor_item_add_port(monitor,port,tv);
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__JIJ
  (JNIEnv *jenv, jobject jthis, jlong ip, jint port, jlong tv){
  
	  return ch_session_monitor_item_add_ip_port(monitor,ip,port,tv);
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (JIJIJ)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__JIJIJ
  (JNIEnv *jenv, jobject jthis, jlong src_ip, jint src_port, jlong dst_ip, jint dst_port, jlong tv){
  

	  return ch_session_monitor_item_add_session(monitor,src_ip,dst_ip,src_port,dst_port,tv);
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
 * Signature: (Lcom/antell/cloudhands/api/smon/SessionMonitorItem;J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_find__Lcom_antell_cloudhands_api_smon_SessionMonitorItem_2J
  (JNIEnv *jenv, jobject jthis, jobject jmonitor_item , jlong id){
  
	  ch_session_monitor_item_t *item = ch_session_monitor_item_findById(monitor,id);
	  if(item == NULL)
		  return -1;

	  _item_fill(jenv,jmonitor_item,item);

	  return 0;
  }
/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    find
 * Signature: (Lcom/antell/cloudhands/api/smon/SessionMonitorItem;JIJI)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_find__Lcom_antell_cloudhands_api_smon_SessionMonitorItem_2JIJI
  (JNIEnv *jenv, jobject jthis, jobject jmonitor_item, jlong src_ip, jint src_port, jlong dst_ip, jint dst_port){
 
	  ch_session_monitor_item_t *item = ch_session_monitor_item_find(monitor,src_ip,dst_ip,src_port,dst_port);
	  if(item == NULL)
		  return -1;

	  _item_fill(jenv,jmonitor_item,item);

	  return 0;
  
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    delete
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_delete
  (JNIEnv *jenv, jobject jthis, jlong id){
  
	  ch_session_monitor_item_del(monitor,id);
	  return 0;
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    stop
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_stop
  (JNIEnv *jenv, jobject jthis, jlong id){
  
	  ch_session_monitor_item_stop(monitor,id);
	  return 0;
  }
/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    restart
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_restart
  (JNIEnv *jenv, jobject jthis, jlong id){
  
	  ch_session_monitor_item_restart(monitor,id);

	  return 0;
  }


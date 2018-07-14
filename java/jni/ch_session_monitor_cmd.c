/*
 *
 *      Filename: ch_stat_pool_reader.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-14 08:57:05
 * Last Modified: 2018-07-14 14:24:43
 */

#include "ch_session_monitor_cmd.h"
#include "ch_session_monitor.h"
#include "ch_jni_util.h"

static ch_session_monitor_t tmp_monitor,*monitor = &tmp_monitor;

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_open
  (JNIEnv *jenv, jobject jthis, jstring mmap_fname){
  
  
  }

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__I
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__JI
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    add
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_add__JIJI
  (JNIEnv *, jobject, jlong, jint, jlong, jint);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    find
 * Signature: (Lcom/antell/cloudhands/api/smon/SessionMonitorItem;J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_find__Lcom_antell_cloudhands_api_smon_SessionMonitorItem_2J
  (JNIEnv *, jobject, jobject, jlong);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    find
 * Signature: (Lcom/antell/cloudhands/api/smon/SessionMonitorItem;JIJI)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_find__Lcom_antell_cloudhands_api_smon_SessionMonitorItem_2JIJI
  (JNIEnv *, jobject, jobject, jlong, jint, jlong, jint);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    delete
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_delete
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    stop
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_stop
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_antell_cloudhands_api_smon_SessionMonitorCmd
 * Method:    restart
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_smon_SessionMonitorCmd_restart
  (JNIEnv *, jobject, jlong);




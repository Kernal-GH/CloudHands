/*
 *
 *      Filename: ch_stat_pool_reader.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-08 11:57:05
 * Last Modified: 2018-07-17 15:18:49
 */

#include "ch_stat_pool_reader.h"
#include "ch_mpool.h"
#include "ch_jni_util.h"
#include "ch_data_output.h"
#include "ch_stat_pool.h"

static ch_stat_pool_t *st_pool;
static ch_data_output_t tmp_dout,*dout = &tmp_dout;

/*
 * Class:     com_antell_cloudhands_api_statistic_StatDataReader
 * Method:    open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_statistic_StatDataReader_open
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

	st_pool = ch_stat_pool_create(mp,open_fname,0,0);

	if(st_pool == NULL)
		return -1;

	if(ch_dout_init(dout))
		return -1;

	return 0;
}

/*
 * Class:     com_antell_cloudhands_api_statistic_StatDataReader
 * Method:    read
 * Signature: (Lcom/antell/cloudhands/api/statistic/StatDataInput;J)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_statistic_StatDataReader_read__Lcom_antell_cloudhands_api_statistic_StatDataInput_2J
  (JNIEnv *jenv, jobject jthis, jobject jst_din, jlong time) {
 
	  /*unused*/
	  jthis = jthis;

	  ssize_t rc;

	  int dsize = 0; 
	  void *data;

	  CH_DOUT_RESET(dout);

	  rc = ch_stat_pool_out_all(st_pool,dout,time);
	  if(rc == -1)
		  return -1;

	  data = CH_DOUT_CONTENT(dout); 
	  dsize = CH_DOUT_CONTENT_SIZE(dout);

	  ch_set_int_field(jenv,jst_din,"dataSize",dsize);
	  ch_send_data_to_java2(jenv,jst_din,data,dsize);

	  return 0;
  }

/*
 * Class:     com_antell_cloudhands_api_statistic_StatDataReader
 * Method:    read
 * Signature: (Lcom/antell/cloudhands/api/statistic/StatDataInput;JI)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_statistic_StatDataReader_read__Lcom_antell_cloudhands_api_statistic_StatDataInput_2JI
  (JNIEnv * jenv, jobject jthis, jobject jst_din, jlong time, jint type) {
  
	  /*unused*/
	  jthis = jthis;

	  ssize_t rc;

	  int dsize = 0; 
	  void *data;

	  CH_DOUT_RESET(dout);

	  rc = ch_stat_pool_out(st_pool,dout,time,type);
	  if(rc == -1)
		  return -1;

	  data = CH_DOUT_CONTENT(dout); 
	  dsize = CH_DOUT_CONTENT_SIZE(dout);

	  ch_set_int_field(jenv,jst_din,"dataSize",dsize);
	  ch_send_data_to_java2(jenv,jst_din,data,dsize);

	  return 0;
  
  }

/*
 * Class:     com_antell_cloudhands_api_statistic_StatDataReader
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_statistic_StatDataReader_close
  (JNIEnv * jenv, jobject jthis) {
  
	  /*unused*/
	  jenv = jenv;
	  jthis = jthis;

	  if(st_pool){
	  
		  ch_stat_pool_destroy(st_pool);

		  ch_pool_destroy(st_pool->mp);
	  }

	  st_pool = NULL;

  }



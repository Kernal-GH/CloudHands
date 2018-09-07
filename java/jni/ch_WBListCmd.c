/*
 *
 *      Filename: ch_WBListCmd.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-06 16:08:04
 * Last Modified: 2018-09-07 16:14:31
 */

#include "ch_WBListCmd.h"
#include "ch_wb_list.h"
#include "ch_wb_list_ip.h"
#include "ch_wb_list_str.h"
#include "ch_jni_util.h"
#include "ch_jni_list.h"

static ch_wb_list_t wb_lists[5];

static inline ch_wb_list_t * _wb_list_get(int context_id){

	if(context_id>=5||context_id<0)
		return NULL;

	return &wb_lists[context_id];
}


/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    open
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_open
  (JNIEnv *jenv, jobject jthis, jint context_id, jstring jmmap_fname){
  

	  ch_wb_list_t *wb_list;

	  const char *mmap_fname = ch_string_arg_get(jenv,jmmap_fname);

	  if(mmap_fname == NULL || strlen(mmap_fname) == 0)
		  return -1;

	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return -1;

	  if(context_id<=1){
	  
		  /*ip wblist*/
		  return ch_wb_list_ip_init(wb_list,mmap_fname,0);

	  }else if(context_id == 4){
		  /*extName */
		  return ch_wb_list_str_init(wb_list,mmap_fname,0,32);
	  }else{
	  
		  /*host wblist*/
		  return ch_wb_list_str_init(wb_list,mmap_fname,0,64); 
	  }

  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_close
  (JNIEnv *jenv, jobject jthis, jint context_id){
  
	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return;

	  ch_wb_list_fin(wb_list);
  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    add
 * Signature: (IJJJII)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_add__IJJJII
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong ip_start, jlong ip_end, jlong netmask, jint port_start, jint port_end){
  
	  ch_wb_list_ip_add_value_t t,*av=&t; 
	  
	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return -1;

	  av->netmask = (uint32_t)netmask;
	  av->ip_start = (uint32_t)ip_start;
	  av->ip_end = (uint32_t)ip_end;
	  av->port_start = (uint16_t)port_start;
	  av->port_end = (uint16_t)port_end;

	  return ch_wb_list_add(wb_list,(void*)av);

  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    add
 * Signature: (ILjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_add__ILjava_lang_String_2
  (JNIEnv *jenv, jobject jthis, jint context_id, jstring value){
  
	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return -1;

	  const char *av = ch_string_arg_get(jenv,value);

	  return ch_wb_list_add(wb_list,(void*)av);
  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    delete
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_delete
  (JNIEnv *jenv, jobject jthis, jint context_id, jlong id){
  
	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return -1;

	  ch_wb_list_del(wb_list,id);

  }

static inline void addIPEntry(ch_jni_list_t *jlist,JNIEnv *jenv,ch_wb_list_ip_entry_t *ip_entry){

	   jobject obj = (*jenv)->NewObject(jenv,jlist->elem_cls,jlist->elem_costruct,
		   ip_entry->entry.id,
		   ip_entry->ip_start,
		   ip_entry->ip_end,
		   ip_entry->netmask,
		   ip_entry->port_start,
		   ip_entry->port_end);


	   ch_jni_list_addObj(jlist,jenv,obj);

}

static inline void addStrEntry(ch_jni_list_t *jlist,JNIEnv *jenv,ch_wb_list_str_entry_t *str_entry){

	   jstring str = (*jenv)->NewStringUTF(jenv,(const char*)str_entry->value);

	   jobject obj = (*jenv)->NewObject(jenv,jlist->elem_cls,jlist->elem_costruct,
		   str_entry->entry.id,str);

	   ch_jni_list_addObj(jlist,jenv,obj);

}

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    getWBIPList
 * Signature: (I)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_getWBIPList
  (JNIEnv *jenv, jobject jthis, jint context_id){

	  ch_jni_list_t t,*jlist = &t;
	  ch_wb_list_ip_entry_t *entry;

	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return NULL;

	  if(ch_wb_list_empty(wb_list))
		  return NULL;

	  if(ch_jni_list_init(jlist,jenv,"Lcom/antell/cloudhands/api/WBList/IPPortRangeValue;",
			  "(JJJJII)V"))
		  return NULL;

	  ch_wb_list_for_each_entry(wb_list,entry,ch_wb_list_ip_entry_t){
	  
		   addIPEntry(jlist,jenv,entry);
	  }

	  return jlist->list_obj;
  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    getWBStrList
 * Signature: (I)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_getWBStrList
  (JNIEnv *jenv, jobject jthis, jint context_id){
  
	  ch_jni_list_t t,*jlist = &t;
	  ch_wb_list_str_entry_t *entry;

	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return NULL;

	  if(ch_wb_list_empty(wb_list))
		  return NULL;

	  if(ch_jni_list_init(jlist,jenv,"Lcom/antell/cloudhands/api/WBList/StrValue;",
			  "(JLjava/lang/String;)V"))
		  return NULL;

	  ch_wb_list_for_each_entry(wb_list,entry,ch_wb_list_str_entry_t){
	  
		   addStrEntry(jlist,jenv,entry);
	  }

	  return jlist->list_obj;
  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    start
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_start
  (JNIEnv *jenv, jobject jthis, jint context_id){

	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return;

	  ch_wb_list_is_on_set(wb_list,1);

  }

/*
 * Class:     com_antell_cloudhands_api_WBList_WBListCmd
 * Method:    stop
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_antell_cloudhands_api_WBList_WBListCmd_stop
  (JNIEnv *jenv, jobject jthis, jint context_id){
  
	  ch_wb_list_t *wb_list;
	  wb_list = _wb_list_get(context_id);
	  if(wb_list == NULL)
		  return;

	  ch_wb_list_is_on_set(wb_list,0);

  
  }


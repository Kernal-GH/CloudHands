/*
 *
 *      Filename: ch_jni_list.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-06 16:59:51
 * Last Modified: 2018-09-06 16:59:51
 */

#ifndef CH_JNI_LIST_H
#define CH_JNI_LIST_H

#include <jni.h>

typedef struct ch_jni_list_t ch_jni_list_t;

struct ch_jni_list_t {

	 jclass list_cls;
	 jmethodID list_costruct;
	 jobject list_obj;

	 jmethodID list_add;
	 jclass elem_cls;
	 jmethodID elem_costruct;
};

#define PCHECK(v) do{ \
	if(v==NULL) \
		return -1;\
}while(0)

static inline int ch_jni_list_init(ch_jni_list_t *jlist,
	JNIEnv * env,
	const char * elem_class_sig,
	const char * elem_cost_sig){

	jlist->list_cls = (*env)->FindClass(env,"Ljava/util/ArrayList;");
	PCHECK(jlist->list_cls);
	
	jlist->list_costruct = (*env)->GetMethodID(env,jlist->list_cls , "<init>","()V");
	PCHECK(jlist->list_costruct);
	
	jlist->list_obj = (*env)->NewObject(env,jlist->list_cls , jlist->list_costruct);
	PCHECK(jlist->list_obj);

	jlist->list_add = (*env)->GetMethodID(env,jlist->list_cls,"add","(Ljava/lang/Object;)Z");
	PCHECK(jlist->list_add);

	jlist->elem_cls = (*env)->FindClass(env,elem_class_sig);
	PCHECK(jlist->elem_cls);

	jlist->elem_costruct = (*env)->GetMethodID(env,jlist->elem_cls,"<init>",elem_cost_sig);
	PCHECK(jlist->elem_costruct);

	return 0;
}

static inline void ch_jni_list_addObj(ch_jni_list_t *jlist,JNIEnv *env,jobject obj){
	
	(*env)->CallObjectMethod(env,jlist->list_obj,jlist->list_add,obj);

}

#endif /*CH_JNI_LIST_H*/

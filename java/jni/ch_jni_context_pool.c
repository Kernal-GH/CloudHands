/*
 *
 *      Filename: ch_jni_context_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-14 15:52:48
 * Last Modified: 2018-07-14 18:34:38
 */

#include "ch_jni_context_pool.h"

void ch_jni_context_pool_init(ch_jni_context_pool_t *jcontext_pool,size_t context_size){

	INIT_LIST_HEAD(&jcontext_pool->contexts);

	jcontext_pool->context_size = context_size;

}

ch_jni_context_t *ch_jni_context_pool_find(ch_jni_context_pool_t *jcontext_pool,JNIEnv *jenv){

	ch_jni_context_t *context = NULL;

	list_for_each_entry(context,&jcontext_pool->contexts,node){

		if(context->jenv == jenv)
			return context;
	}

	return NULL;
}

ch_jni_context_t *ch_jni_context_pool_create(ch_jni_context_pool_t *jcontext_pool,JNIEnv *jenv){


	ch_jni_context_t *context;

	context = ch_jni_context_pool_find(jcontext_pool,jenv);
	if(context)
		return context;

	context = (ch_jni_context_t*)malloc(jcontext_pool->context_size);
	if(context == NULL)
		return NULL;

	context->jenv = jenv;

	list_add_tail(&context->node,&jcontext_pool->contexts);

	return context;

}


void ch_jni_context_pool_remove(ch_jni_context_pool_t *jcontext_pool,ch_jni_context_t *context){


	if(context){
	
		list_del(&context->node);
		free(context);
	}

}

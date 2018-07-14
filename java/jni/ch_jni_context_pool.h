/*
 *
 *      Filename: ch_jni_context_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-14 15:32:02
 * Last Modified: 2018-07-14 15:32:02
 */

#ifndef CH_JNI_CONTEXT_POOL_H
#define CH_JNI_CONTEXT_POOL_H

#include <jni.h>
#include "ch_list.h"

typedef struct ch_jni_context_pool_t ch_jni_context_pool_t;
typedef struct ch_jni_context_t ch_jni_context_t;

struct ch_jni_context_pool_t {

	struct list_head contexts;
	
	size_t context_size;

};

struct ch_jni_context_t {

	struct list_head node;
	JNIEnv *jenv;

};


extern void ch_jni_context_pool_init(ch_jni_context_pool_t *jcontext_pool,size_t context_size);

extern ch_jni_context_t *ch_jni_context_pool_create(ch_jni_context_pool_t *jcontext_pool,JNIEnv *jenv);

extern ch_jni_context_t *ch_jni_context_pool_find(ch_jni_context_pool_t *jcontext_pool,JNIEnv *jenv);

extern void ch_jni_context_pool_remove(ch_jni_context_pool_t *jcontext_pool,ch_jni_context_t *context);

#endif /*CH_JNI_CONTEXT_POOL_H*/

/*
 *
 *      Filename: ch_object_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 14:28:25
 * Last Modified: 2017-12-29 09:47:23
 */

#include "ch_object_pool.h"
#include "ch_log.h"

static inline int _new_memory(ch_object_pool_t *omp){

	ch_object_mem_t *objm;
	ch_object_item_t *item;

	size_t r_item_size;
	void *addr;
	void *pos;
	void *end;

	addr = malloc(OBJECT_MEM_SIZE);
	if(addr == NULL){
	
		return -1;
	}

	objm = (ch_object_mem_t*)addr;

	pos = (void*)(objm+1);
	end = addr+OBJECT_MEM_SIZE;

	/*alloc object item and add to free list*/
	r_item_size = omp->object_size+sizeof(ch_object_item_t);

	while(pos+r_item_size<end){
	

		item = (ch_object_item_t*)pos;

		pos += r_item_size;

		list_add_tail(&item->node,&omp->free_list);
		omp->n_frees += 1;
		omp->n_objects+=1;
	}

	/*add memory to object memory pool's mm_List*/
	objm->next = omp->mm_list;
	omp->mm_list = objm;
	omp->n_mms += 1;

	/*ok*/
	return 0;
}

ch_object_pool_t * ch_object_pool_create(ch_pool_t *mp,size_t object_limits,
	size_t object_size,
	void *priv_data,
	void (*obj_init)(void *obj,void *priv_data)){

	if(object_size>=OBJECT_MEM_SIZE){
	
		ch_log(CH_LOG_ERR,"The object size is too large:%lu",object_size);
		return NULL;
	}

	ch_object_pool_t *objp = (ch_object_pool_t *) ch_pcalloc(mp,sizeof(*objp));

	objp->mm_list = NULL;

	INIT_LIST_HEAD(&objp->free_list);

	objp->priv_data = priv_data;
	objp->obj_init = obj_init;
	objp->object_limits = object_limits;
	objp->object_size = object_size;
	objp->n_objects = 0;
	objp->n_mms = 0;

	objp->n_frees = 0;
	objp->n_using = 0;

	return objp;
}


void ch_object_pool_destroy(ch_object_pool_t *omp){

	ch_object_mem_t *objm = omp->mm_list;
	ch_object_mem_t *prev;

	while(objm){
	
		prev = objm;
		objm = objm->next;

		free(prev);
	}

}

void * ch_object_pool_get(ch_object_pool_t *omp){

	ch_object_item_t *item;
	void *obj;

	if(list_empty(&omp->free_list)){
	
		if(omp->object_limits&&omp->n_objects>omp->object_limits){
		
			ch_log(CH_LOG_ERR,"Too many object has been alloced!");
			return NULL;
		}

		if(_new_memory(omp)){

			ch_log(CH_LOG_ERR,"No memory space to  new object!");
			return NULL;
		}
	}

	item = list_first_entry(&omp->free_list,ch_object_item_t,node);
	list_del(&item->node);

	obj = (void*)(item+1);

	if(omp->obj_init){
		
		omp->obj_init(obj,omp->priv_data);
	
	}

	omp->n_frees -= 1;
	omp->n_using += 1;

	return obj;
}

void  ch_object_pool_put(ch_object_pool_t *omp,void *obj){

	if(obj == NULL)
		return;

	ch_object_item_t *item = (ch_object_item_t*)(obj-sizeof(*item));

	list_add_tail(&item->node,&omp->free_list);

	omp->n_frees += 1;
	omp->n_using -= 1;

}

#define ROBJ_SIZE(omp,n) (unsigned long)(((omp)->object_size+sizeof(ch_object_item_t))*(n))

void ch_object_pool_dump(ch_object_pool_t *omp,FILE *fp){


	fprintf(fp,"Dump object pool info-------------------------------------------\n");
	
	fprintf(fp,"The object size:%lu\n",(unsigned long)(omp->object_size+sizeof(ch_object_item_t)));

	if(omp->object_limits)
		fprintf(fp,"The max object number can been alloced :%lu,size:%lu\n",(unsigned long)omp->object_limits,
			ROBJ_SIZE(omp,omp->object_limits));
	else
		fprintf(fp,"The max object number can been alloced :unlimited,size:unlimited\n");

	fprintf(fp,"The total number object has been alloced:%lu,size:%lu\n",(unsigned long)omp->n_objects,
		ROBJ_SIZE(omp,omp->n_objects));

	fprintf(fp,"The total number memory has been alloced:%lu,size:%luk\n",(unsigned long)omp->n_mms,
		(unsigned long)64*omp->n_mms);


	fprintf(fp,"The total number object in free list:%lu,size:%lu\n",(unsigned long)omp->n_frees,
		ROBJ_SIZE(omp,omp->n_frees));

	fprintf(fp,"The total number object is using:%lu,size:%lu\n",(unsigned long)omp->n_using,
		ROBJ_SIZE(omp,omp->n_using));
}

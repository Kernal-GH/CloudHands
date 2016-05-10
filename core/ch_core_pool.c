/*
 * =====================================================================================
 *
 *       Filename:  ch_core_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月25日 12时44分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_constants.h"
#include "ch_log.h"
#include "ch_core_pool.h"

static inline void _core_push(ch_core_pool_t *cpool,ch_core_t *core){
    *(ch_core_t **)apr_array_push(cpool->cores) = core;
}

static inline ch_core_t* _core_get(ch_core_pool_t *cpool){

    ch_core_t *core,**cores;
    int i;

    if(cpool->cores->nelts == 0){
        return NULL;
    }

    cores = (ch_core_t**)cpool->cores->elts;
    
    for(i = 0; i<cpool->cores->nelts; i++){
        core = cores[i];
        if(core->bound == 0)
            return core;
    }

    return NULL;

}

static ch_core_t * _core_create(ch_core_pool_t *cpool,unsigned int core_id){

    ch_core_t *core = (ch_core_t*)apr_palloc(cpool->context->mp,sizeof(ch_core_t));

    core->tsk = NULL;
    core->core_id = core_id;
    core->socket = (unsigned int) rte_lcore_to_socket_id(core_id);

    core->bound = 0;

    _core_push(cpool,core);

    return core;
}

ch_core_pool_t * ch_core_pool_create(ch_context_t *context){

    ch_core_t *core;
    unsigned int lcore_id;
    unsigned int core_n = rte_lcore_count();

    if(core_n == 0){
        ch_log(CH_LOG_ERR,"The number of cpu cores is zero!");
        return NULL;
    }

    apr_pool_t *mp = context->mp;
    ch_core_pool_t *cpool = (ch_core_pool_t*)apr_palloc(mp,sizeof(ch_core_pool_t));

    cpool->context = context;
    cpool->cores = apr_array_make(mp,16,sizeof(ch_core_t*));

    /*create all cores*/

    for(lcore_id = 0; lcore_id < core_n; lcore_id++){
        
        if(rte_lcore_is_enabled(lcore_id) == 0){
            continue;
        }

        core = _core_create(cpool,lcore_id);

        ch_log(CH_LOG_INFO,"Create core[id=%u,socket=%u]",lcore_id,core->socket);
    }

    ch_log(CH_LOG_INFO,"Create cpu cores[%u] in core pool OK",cpool->cores->nelts);

    return cpool;
}

int ch_core_pool_task_bind(ch_core_pool_t *cpool,ch_task_t *tsk){

    ch_core_t *core = _core_get(cpool);

    if(core == NULL){
        ch_log(CH_LOG_ERR,"no cpu core used to bind task[%u]!",tsk->tsk_id);
        return CH_ERROR;
    }

    core->tsk = tsk;
    core->bound = 1;
	tsk->core = core;

    ch_log(CH_LOG_INFO,"Cpu core[%u] was bound to task[%u] ok!",core->core_id,tsk->tsk_id);

    return CH_OK;
}

void ch_core_pool_task_unbind(ch_core_pool_t *cpool,ch_core_t *core){

    core->tsk = NULL;
    core->bound = 0;
	
}


static inline ch_core_t * _core_get_by_id(ch_core_pool_t *cpool,unsigned int core_id){

    ch_core_t **cores,*core;
    int i;

    cores = (ch_core_t**)cpool->cores->elts;
    
    for(i = 0; i<cpool->cores->nelts; i++){
        core = cores[i];
        if(core->core_id == core_id)
            return core;
    }

    return NULL;
}

static int core_setup_main(void *priv_data){

    ch_core_t *core;

    unsigned int core_id;

    ch_core_pool_t *cpool = (ch_core_pool_t*)priv_data;
    core_id = rte_lcore_id();

    core = _core_get_by_id(cpool,core_id);
    
    if(core == NULL||core->tsk == NULL||core->bound == 0){
        
        ch_log(CH_LOG_INFO,"Core was not be bound to any task,ignore it!");
        return 0;
    }

    /* run task! */
    if(CH_ERROR == ch_task_pool_task_run(cpool->context->tpool,core->tsk)){
        return -1;
    }

    return 0;
}

void ch_core_pool_cores_setup(ch_core_pool_t *cpool){
    
    unsigned int core_id;

    if(cpool->cores->nelts == 0){
        
        ch_log(CH_LOG_ERR,"No cpu core created,so nothing t!");
        return;
    }

   /* launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(core_setup_main, (void*)cpool, CALL_MASTER);

    RTE_LCORE_FOREACH_SLAVE(core_id) {
        if (rte_eal_wait_lcore(core_id) < 0){
            ch_log(CH_LOG_ERR,"wait all tasks failed!");
        }
    } 
}

/*
 *
 *      Filename: ch_fpath.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-25 02:01:00
 * Last Modified: 2016-11-05 23:43:15
 */

#include "ch_constants.h"
#include <time.h>
#include "ch_log.h"
#include "ch_fpath.h"
#include "ch_util.h"

static inline size_t _fpath_fname_buf_size_get(const char *root_dir,int create_dir_type){

	/* unused */
	create_dir_type = create_dir_type;

    /*BY DAY*/
    /*root_dir/yyyymmdd/yyyymmdd-HHMMSS-UINT64_MAX_LEN*/
 
    /*BY_HOUR*/
    /*root_dir/yyyymmdd/yyyymmdd-HH/yyyymmdd-HHMMSS-UINT64_MAX_LEN*/

    /*BY_MIN*/
    /*root_dir/yyyymmdd/yyyymmdd-HHMM/yyyymmdd-HHMMSS-UINT64_MAX_LEN*/

    /*BY_SEC*/
    /*root_dir/yyyymmdd/yyyymmdd-HHMMSS/yyyymmdd-HHMMSS-UINT64_MAX_LEN*/
    
    return strlen(root_dir)+3+40+2*CH_UINT64_MAX_VALUE_LEN+32;

}

int ch_fpath_init(ch_fpath_t *fpath,const char *root,int create_dir_type,int is_need_tmp_buf, int log_id){


	fpath->root = root;
	fpath->log_id = log_id;
	fpath->counter = 1;
	fpath->create_dir_type = create_dir_type;
	fpath->is_need_tmp_buf = is_need_tmp_buf;
	fpath->fname_buf_size = _fpath_fname_buf_size_get(root,create_dir_type);

    fpath->fname_buf = (char*)malloc(fpath->fname_buf_size);
	fpath->fname_buf_tmp = NULL;

	if(is_need_tmp_buf){
    
		fpath->fname_buf_tmp = (char*)malloc(fpath->fname_buf_size+4);
	
	}

    if(fpath->fname_buf == NULL || (is_need_tmp_buf&&fpath->fname_buf_tmp == NULL)){
    
        ch_log(CH_LOG_ERR,"no free memory used to create fname buffer !");
        return -1;
    }

	return 0;
}

void ch_fpath_fin(ch_fpath_t *fpath){

	if(fpath->fname_buf)
	{
		free(fpath->fname_buf);
	
		fpath->fname_buf = NULL;
	}

	if(fpath->is_need_tmp_buf&&fpath->fname_buf_tmp){
	
		free(fpath->fname_buf_tmp);
		fpath->fname_buf_tmp = NULL;
	}
}

ch_fpath_t* 
ch_fpath_create(apr_pool_t*mp,const char *root,int create_dir_type,int is_need_tmp_buf, int log_id){


	ch_fpath_t *fpath = (ch_fpath_t*)apr_palloc(mp,sizeof(ch_fpath_t));

	return ch_fpath_init(fpath,root,create_dir_type,is_need_tmp_buf, log_id)?NULL:fpath;
}

void ch_fpath_name_create(ch_fpath_t *fpath,char **fname,char **fname_tmp){

    char tstr[64] = {0};

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    
    switch(fpath->create_dir_type){

        case BY_DAY:
            strftime(tstr,63,"/%Y%m%d/%Y%m%d-%H%M%S",timeinfo);
            break;

        case BY_HOUR:
            strftime(tstr,63,"/%Y%m%d/%Y%m%d-%H/%Y%m%d-%H%M%S",timeinfo);
            break;

        case BY_MIN:
            strftime(tstr,63,"/%Y%m%d/%Y%m%d-%H%M/%Y%m%d-%H%M%S",timeinfo);
            break;

        case BY_SEC:
            strftime(tstr,63,"/%Y%m%d/%Y%m%d-%H%M%S/%Y%m%d-%H%M%S",timeinfo);
            break;

        default:
            strftime(tstr,63,"/%Y%m%d/%Y%m%d-%H%M%S",timeinfo);
    }

    snprintf(fpath->fname_buf,fpath->fname_buf_size-1,"%s/%d%s-%lu-%lu",fpath->root,
            fpath->log_id, tstr,
            (unsigned long)ch_get_current_timems(),(unsigned long)fpath->counter);

	if(fpath->is_need_tmp_buf){
    
		snprintf(fpath->fname_buf_tmp,fpath->fname_buf_size+4-1,"%s%s",fpath->fname_buf,".tmp");
	}

    ch_dir_make(fpath->fname_buf);
	
	ch_fpath_counter_update(fpath,1);

	*fname = fpath->fname_buf;
	*fname_tmp = fpath->fname_buf_tmp;
}

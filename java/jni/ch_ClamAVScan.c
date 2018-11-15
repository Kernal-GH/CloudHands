/*
 *
 *      Filename: ch_ClamAVScan.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-08-28 11:57:14
 * Last Modified: 2018-08-28 13:49:11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <clamav.h>
#include "ch_ClamAVScan.h"
#include "ch_jni_util.h"

static struct cl_engine *engine;

#define IS_STR_EMPTY(str) ((str)==NULL||strlen(str)==0)
/*
 * Class:     com_antell_cloudhands_api_packet_se
 * curity_clamav_ClamAVScan
 * Method:    init
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_antell_cloudhands_api_packet_security_clamav_ClamAVScan_init
  (JNIEnv *jenv, jobject jthis, jstring dbDir, jint loadOpt){
 
	  int ret;
	  unsigned int sigs = 0;

	  const char *av_db_path = ch_string_arg_get(jenv,dbDir);
	  if(IS_STR_EMPTY(av_db_path)){
	  
		  av_db_path = cl_retdbdir();
		  if(IS_STR_EMPTY(av_db_path)){
			  printf("Please specify the Clamav Database path!\n");
		  
			  return -1;
		  }
	  }

	  if(loadOpt <=0)
		  loadOpt = CL_DB_STDOPT;
	  
	  if((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) {
		  printf("Can't initialize libclamav: %s\n", cl_strerror(ret));
		  return -1;
	  
	  }
	  
	  if(!(engine = cl_engine_new())) {
		  printf("Can't create new engine\n");
		  return -1;
	  }
	  
	  /* load all available databases from default directory */
	  if((ret = cl_load(av_db_path, engine, &sigs,loadOpt)) != CL_SUCCESS) {
		  printf("cl_load: %s\n", cl_strerror(ret));
		  cl_engine_free(engine);
		  return -1;
	  }
	  
	  printf("Loaded %u signatures.\n", sigs);
	  
	  /* build engine */
	  if((ret = cl_engine_compile(engine)) != CL_SUCCESS) {
		  printf("Database initialization error: %s\n", cl_strerror(ret));
		  cl_engine_free(engine);
		  return -1;
		}

	  /*ok!*/
	  return 0;
  }

/*
 * Class:     com_antell_cloudhands_api_packet_security_clamav_ClamAVScan
 * Method:    scanFile
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_antell_cloudhands_api_packet_security_clamav_ClamAVScan_scanFile
  (JNIEnv *jenv, jobject jthis, jstring fname, jint scanOpt){

	  int ret;
	  int fd;
	  unsigned long int size = 0;
	  const char *virname;
	  const char *filename;
	  
	  if(scanOpt<=0)
		  scanOpt = CL_SCAN_STDOPT; 

	  filename = ch_string_arg_get(jenv,fname);
	  if(IS_STR_EMPTY(filename)){
	  
		  printf("Please Specify File Name!\n");

		  return (*jenv)->NewStringUTF(jenv,"");
	  }
	  
	  if((fd = open(filename, O_RDONLY)) == -1) {
		  printf("Can't open file %s\n",filename);
		  return (*jenv)->NewStringUTF(jenv,"");
	  }
	  
	  /* scan file descriptor */
	  if((ret = cl_scandesc(fd, &virname, &size, engine, scanOpt)) == CL_VIRUS) {
		  printf("Virus detected: %s\n", virname);
	  }else{
	  
		  virname = "";
	  }

	  close(fd);

	  return (*jenv)->NewStringUTF(jenv,virname);
  }


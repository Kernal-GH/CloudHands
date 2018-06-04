##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

CFLAGS  = ${BUILD_CFLAGS} -ggdb3 -O0 -rdynamic -D_GNU_SOURCE -pthread
CFLAGS += -fPIC -shared -I../../common -I../ -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux

jni_SOURCES = ../../common/ch_file_open.c \
		  ../../common/ch_errno.c  \
		  ../../common/ch_string.c \
		  ../../common/ch_mpool.c \
		  ../../common/ch_log.c    \
		  ../ch_shm_mmap.c \
		  ../ch_shm_memory.c \
		  ../ch_shm_interface.c \
		  ../ch_shm_format.c \
		  ../ch_packet_record.c \
		  ch_packet_record_reader_jni.c

jni_package = libCHPacketRecordJNI.so

.PHONY: all clean install

all:
	gcc ${CFLAGS} ${jni_SOURCES} -o ${jni_package} ${LDLIBS}

clean:
	rm -rf ${jni_package}

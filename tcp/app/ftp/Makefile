##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../../lib/common -I../../../packet -I../../../lib/ptables -I../../../lib/shm -I../../../lib/thread -I../../../MProcess -I../../../lib/fstore
CFLAGS += -I../../../WBList/ -I../../../SessionMonitor
CFLAGS += -I../ -I../../ -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_ftp_SOURCES = ch_ftp.c \
				  ch_ftp_data_connection_pool.c \
				  ch_ftp_data_session_entry.c \
				  ch_ftp_session.c \
				  ch_ftp_session_entry.c 

ch_ftp_OBJECTS = $(patsubst %.c,%.o,$(ch_ftp_SOURCES))
ch_ftp_DEPENDS = $(patsubst %.c,%.d,$(ch_ftp_SOURCES))
ch_ftp_ASMFILE = $(patsubst %.c,%.s,$(ch_ftp_SOURCES))

.PHONY: all clean

all: $(ch_ftp_OBJECTS)

clean:
	@rm -fr $(ch_ftp_OBJECTS) $(ch_ftp_DEPENDS) $(ch_ftp_ASMFILE) $(ch_ftp_package)
	@rm -fr *.d *.o *.s 


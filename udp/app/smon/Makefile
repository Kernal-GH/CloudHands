##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../../lib/common -I../../../packet -I../../../lib/ptables -I../../../lib/shm -I../../../lib/thread -I../../../SessionMonitor -I../../../lib/fstore 
CFLAGS += -I../../../MProcess -I../../../WBList
CFLAGS +=-I../ -I../../ -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_smon_app_SOURCES = ch_smon_app.c \
				 ch_smon_session.c 

ch_smon_app_OBJECTS = $(patsubst %.c,%.o,$(ch_smon_app_SOURCES))
ch_smon_app_DEPENDS = $(patsubst %.c,%.d,$(ch_smon_app_SOURCES))
ch_smon_app_ASMFILE = $(patsubst %.c,%.s,$(ch_smon_app_SOURCES))

.PHONY: all clean

all: $(ch_smon_app_OBJECTS)

clean:
	@rm -fr $(ch_smon_app_OBJECTS) $(ch_smon_app_DEPENDS) $(ch_smon_app_ASMFILE) $(ch_smon_app_package)
	@rm -fr *.d *.o *.s 


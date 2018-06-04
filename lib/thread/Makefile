##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS} -pthread
CFLAGS += -I../common  -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_thread_SOURCES = ch_core_pool.c \
					ch_task.c

ch_thread_OBJECTS = $(patsubst %.c,%.o,$(ch_thread_SOURCES))
ch_thread_DEPENDS = $(patsubst %.c,%.d,$(ch_thread_SOURCES))
ch_thread_ASMFILE = $(patsubst %.c,%.s,$(ch_thread_SOURCES))

.PHONY: all clean

all: $(ch_thread_OBJECTS)

clean:
	@rm -fr $(ch_thread_OBJECTS) $(ch_thread_DEPENDS) $(ch_thread_ASMFILE) $(ch_thread_package)
	@rm -fr *.d *.o *.s 


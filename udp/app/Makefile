##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../lib/common  -I../../lib/ptables -I../../packet -I../../lib/shm -I../../lib/thread -I../ -I./ 
CFLAGS += -I./dns -I./tftp -I./smon -I../../MProcess -I../../SessionMonitor -I../../WBList
CFLAGS +=-I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_udp_app_SOURCES = ch_udp_app_pool.c \
					 ch_udp_app_context.c

ch_udp_app_OBJECTS = $(patsubst %.c,%.o,$(ch_udp_app_SOURCES))
ch_udp_app_DEPENDS = $(patsubst %.c,%.d,$(ch_udp_app_SOURCES))
ch_udp_app_ASMFILE = $(patsubst %.c,%.s,$(ch_udp_app_SOURCES))

.PHONY: all clean

all: $(ch_udp_app_OBJECTS)

clean:
	@rm -fr $(ch_udp_app_OBJECTS) $(ch_udp_app_DEPENDS) $(ch_udp_app_ASMFILE) $(ch_udp_app_package)
	@rm -fr *.d *.o *.s 


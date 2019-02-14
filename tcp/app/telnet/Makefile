##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../../lib/common -I../../../packet -I../../../lib/ptables -I../../../lib/shm -I../../../lib/fstore -I../../../SessionMonitor
CFLAGS += -I../../../lib/thread -I../../../MProcess -I../../../WBList
CFLAGS += -I../ -I../../ -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_telnet_SOURCES = ch_telnet.c \
				  ch_telnet_session_entry.c 

ch_telnet_OBJECTS = $(patsubst %.c,%.o,$(ch_telnet_SOURCES))
ch_telnet_DEPENDS = $(patsubst %.c,%.d,$(ch_telnet_SOURCES))
ch_telnet_ASMFILE = $(patsubst %.c,%.s,$(ch_telnet_SOURCES))

.PHONY: all clean

all: $(ch_telnet_OBJECTS)

clean:
	@rm -fr $(ch_telnet_OBJECTS) $(ch_telnet_DEPENDS) $(ch_telnet_ASMFILE) $(ch_telnet_package)
	@rm -fr *.d *.o *.s 


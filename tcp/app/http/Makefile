##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../../lib/common -I../../../packet -I../../../lib/ptables -I../../../lib/shm -I../../../lib/fstore
CFLAGS += -I../../../WBList/ -I../../../SessionMonitor -I../../../lib/thread -I../../../MProcess
CFLAGS += -I../ -I../../ -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_http_SOURCES = ch_http.c \
				  ch_http_data_parse.c \
				  ch_http_session.c \
				  ch_http_session_body.c \
				  ch_http_session_entry.c  \
				  ch_http_ctypes.c

ch_http_OBJECTS = $(patsubst %.c,%.o,$(ch_http_SOURCES))
ch_http_DEPENDS = $(patsubst %.c,%.d,$(ch_http_SOURCES))
ch_http_ASMFILE = $(patsubst %.c,%.s,$(ch_http_SOURCES))

.PHONY: all clean

all: $(ch_http_OBJECTS)

clean:
	@rm -fr $(ch_http_OBJECTS) $(ch_http_DEPENDS) $(ch_http_ASMFILE) $(ch_http_package)
	@rm -fr *.d *.o *.s 


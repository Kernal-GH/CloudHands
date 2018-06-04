##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include

DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE  ${DPDK_FLAGS} -pthread
CFLAGS += -I../../../lib/common  -I../../../lib/ptables -I../../../lib/fstore -I../../../lib/shm -I../  -I${DPDK}/include -include ${DPDK}/include/rte_config.h

ch_http_SOURCES = ch_http_context.c \
				  ch_http_session.c \
				  ch_http_session_body.c \
				  ch_http_data_parse.c  \
				  ch_http_session_entry.c \
				  ch_http_parser.c

ch_http_OBJECTS = $(patsubst %.c,%.o,$(ch_http_SOURCES))
ch_http_DEPENDS = $(patsubst %.c,%.d,$(ch_http_SOURCES))
ch_http_ASMFILE = $(patsubst %.c,%.s,$(ch_http_SOURCES))

.PHONY: all clean

all: $(ch_http_OBJECTS)

clean:
	@rm -fr $(ch_http_OBJECTS) $(ch_http_DEPENDS) $(ch_http_ASMFILE) $(ch_http_package)
	@rm -fr *.d *.o *.s 


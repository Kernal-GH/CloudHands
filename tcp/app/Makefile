##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include

DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE  ${DPDK_FLAGS} -pthread
CFLAGS += -I../../lib/common  -I../../lib/shm -I../../lib/ptables -I../../packet

CFLAGS += -I./http -I./smtp -I./imap -I./pop3 -I./telnet -I./ftp -I./smon

CFLAGS += -I../ -I.  -I${DPDK}/include -include ${DPDK}/include/rte_config.h

ch_tcp_app_SOURCES = ch_tcp_app_context.c \
					 ch_tcp_app_pool.c \
					 http/ch_http.c \
					 smtp/ch_smtp.c

ch_tcp_app_OBJECTS = $(patsubst %.c,%.o,$(ch_tcp_app_SOURCES))
ch_tcp_app_DEPENDS = $(patsubst %.c,%.d,$(ch_tcp_app_SOURCES))
ch_tcp_app_ASMFILE = $(patsubst %.c,%.s,$(ch_tcp_app_SOURCES))

.PHONY: all clean

all: $(ch_tcp_app_OBJECTS)

clean:
	@rm -fr $(ch_tcp_app_OBJECTS) $(ch_tcp_app_DEPENDS) $(ch_tcp_app_ASMFILE) $(ch_tcp_app_package)
	@rm -fr *.d *.o *.s 


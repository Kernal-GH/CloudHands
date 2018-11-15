##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../lib/common  -I../packet -I../SessionMonitor -I../WBList
CFLAGS += -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_mprocess_SOURCES = ch_process_interface.c \
					  ch_process_interface_tcp_context.c \
					  ch_process_interface_udp_context.c \
					  ch_process_interface_sa_context.c

ch_mprocess_OBJECTS = $(patsubst %.c,%.o,$(ch_mprocess_SOURCES))
ch_mprocess_DEPENDS = $(patsubst %.c,%.d,$(ch_mprocess_SOURCES))
ch_mprocess_ASMFILE = $(patsubst %.c,%.s,$(ch_mprocess_SOURCES))

.PHONY: all clean

all: $(ch_mprocess_OBJECTS)

clean:
	@rm -fr $(ch_mprocess_OBJECTS) $(ch_mprocess_DEPENDS) $(ch_mprocess_ASMFILE) $(ch_mprocess_package)
	@rm -fr *.d *.o *.s 


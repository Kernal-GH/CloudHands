##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include

DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE  ${DPDK_FLAGS} -pthread
CFLAGS += -I../../../lib/common  -I../../../lib/ptables -I../../../lib/fstore -I../../../lib/shm -I../../../SessionMonitor -I../  -I${DPDK}/include -include ${DPDK}/include/rte_config.h

ch_smon_SOURCES = ch_smon_context.c \
				  ch_smon_session_entry.c \
				  ch_smon_parser.c

ch_smon_OBJECTS = $(patsubst %.c,%.o,$(ch_smon_SOURCES))
ch_smon_DEPENDS = $(patsubst %.c,%.d,$(ch_smon_SOURCES))
ch_smon_ASMFILE = $(patsubst %.c,%.s,$(ch_smon_SOURCES))

.PHONY: all clean

all: $(ch_smon_OBJECTS)

clean:
	@rm -fr $(ch_smon_OBJECTS) $(ch_smon_DEPENDS) $(ch_smon_ASMFILE) $(ch_smon_package)
	@rm -fr *.d *.o *.s 


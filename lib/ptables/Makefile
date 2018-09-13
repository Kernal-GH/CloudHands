##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS} -pthread
CFLAGS += -I../common  -I${DPDK}/include -include ${DPDK}/include/rte_config.h


ch_ptable_SOURCES = ch_ptable.c \
					ch_entry_pool.c \
					ch_memory.c \
					ch_memory_cache.c \
					ch_plist.c

ch_ptable_OBJECTS = $(patsubst %.c,%.o,$(ch_ptable_SOURCES))
ch_ptable_DEPENDS = $(patsubst %.c,%.d,$(ch_ptable_SOURCES))
ch_ptable_ASMFILE = $(patsubst %.c,%.s,$(ch_ptable_SOURCES))

.PHONY: all clean

all: $(ch_ptable_OBJECTS)

clean:
	@rm -fr $(ch_ptable_OBJECTS) $(ch_ptable_DEPENDS) $(ch_ptable_ASMFILE) $(ch_ptable_package)
	@rm -fr *.d *.o *.s 


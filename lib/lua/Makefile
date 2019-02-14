##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../lib/common  -I../../packet -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_lua_SOURCES = ch_lua_engine.c

ch_lua_OBJECTS = $(patsubst %.c,%.o,$(ch_lua_SOURCES))
ch_lua_DEPENDS = $(patsubst %.c,%.d,$(ch_lua_SOURCES))
ch_lua_ASMFILE = $(patsubst %.c,%.s,$(ch_lua_SOURCES))

.PHONY: all clean

all: $(ch_lua_OBJECTS)

clean:
	@rm -fr $(ch_lua_OBJECTS) $(ch_lua_DEPENDS) $(ch_lua_ASMFILE) $(ch_lua_package)
	@rm -fr *.d *.o *.s 


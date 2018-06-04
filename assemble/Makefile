include ../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS} -pthread
CFLAGS += -I../lib/common  -I../lib/shm -I../lib/thread -I../lib/ptables -I../packet -I../MProcess  -I../filter/lua_filter -I${DPDK}/include -include ${DPDK}/include/rte_config.h

LDFLAGS  = ${BUILD_LDFLAGS} ${DPDK_LIB_FLAGS} -lpthread -lluajit-5.1


common_package = ../lib/common/*.o
cpool_package = ../lib/thread/*.o
shm_package = ../lib/shm/*.o
ptables_package = ../lib/ptables/*.o
pkt_package = ../packet/*.o
mprocess_package = ../MProcess/*.o
luaf_package = ../filter/lua_filter/*.o

assemble_package= Assemble

assemble_SOURCES = Assemble.c \
			   ch_assemble_context.c 

assemble_OBJECTS =$(patsubst %.c,%.o,$(assemble_SOURCES))
assemble_DEPENDS =$(patsubst %.c,%.d,$(assemble_SOURCES))
assemble_ASMFILE =$(patsubst %.c,%.s,$(assemble_SOURCES))

quiet_cmd_dpdk = LINK   $@ 
	cmd_assemble = ${CC} ${CFLAGS} -o $@ $(assemble_OBJECTS)  \
				   $(common_package) $(cpool_package) $(shm_package) $(ptables_package)\
				   $(pkt_package) $(mprocess_package) $(luaf_package)\
				   $(LDFLAGS)

.PHONY: all clean

all: $(assemble_package)

$(assemble_package): $(assemble_OBJECTS) 
	$(call cmd,assemble)
-include $(assemble_DEPENDS)

$(common_package):
	$(call,make)

$(shm_package):
	$(call,make)

$(thread_package):
	$(call,make)

$(ptables_package):
	$(call,make)

$(pkt_package):
	$(call,make)

$(mprocess_package):
	$(call,make)

$(luaf_package):
	$(call,make)

clean:
	@rm -rf $(assemble_OBJECTS) $(assemble_DEPENDS) $(assemble_ASMFILE) $(assemble_package)



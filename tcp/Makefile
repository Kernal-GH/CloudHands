include ../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS} -pthread
CFLAGS += -I../lib/common  -I../lib/shm -I../lib/thread -I../lib/ptables -I../packet -I../MProcess -I./app -I. -I../SessionMonitor -I../WBList 
CFLAGS += -I../lib/fstore -I${DPDK}/include -include ${DPDK}/include/rte_config.h

LDFLAGS  = ${BUILD_LDFLAGS} ${DPDK_LIB_FLAGS} -lpthread


common_package = ../lib/common/*.o
cpool_package = ../lib/thread/*.o
shm_package = ../lib/shm/*.o
ptables_package = ../lib/ptables/*.o
pkt_package = ../packet/*.o
mprocess_package = ../MProcess/*.o
smon_package = ../SessionMonitor/*.o
app_package=./app/*.o
fstore_package=../lib/fstore/*.o
wblist_package=../WBList/*.o

tcp_package= TCPMain

tcp_SOURCES = TCPMain.c \
			   ch_assemble_fragment.c \
			   ch_tcp_work.c \
			   ch_tcp_context.c \
			   ch_tcp_session.c \
			   ch_tcp_session_pool.c \
			   ch_tcp_session_request_pool.c \
			   ch_tcp_session_endpoint.c \
			   ch_tcp_session_request_handler.c \
			   ch_tcp_session_handler.c \
			   ch_tcp_session_task.c \
			   ch_tcp_session_task_pool.c 

tcp_OBJECTS =$(patsubst %.c,%.o,$(tcp_SOURCES))
tcp_DEPENDS =$(patsubst %.c,%.d,$(tcp_SOURCES))
tcp_ASMFILE =$(patsubst %.c,%.s,$(tcp_SOURCES))

quiet_cmd_dpdk = LINK   $@ 
	cmd_tcp = ${CC} ${CFLAGS} -o $@ $(tcp_OBJECTS)  \
				   $(common_package) $(cpool_package) $(shm_package) $(ptables_package)\
				   $(pkt_package) $(app_package) $(mprocess_package) $(smon_package) $(fstore_package) $(wblist_package)\
				   $(LDFLAGS)

.PHONY: all clean

all: $(tcp_package)

$(tcp_package): $(tcp_OBJECTS) 
	$(call cmd,tcp)
-include $(tcp_DEPENDS)

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

$(app_package):
	$(call,make)

$(smon_package):
	$(call,make)

$(fstore_package):
	$(call,make)

$(wblist_package):
	$(call,make)

clean:
	@rm -rf $(tcp_OBJECTS) $(tcp_DEPENDS) $(tcp_ASMFILE) $(tcp_package)



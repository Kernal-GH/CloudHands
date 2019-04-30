include ../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS} -pthread
CFLAGS += -I../lib/common  -I../lib/shm -I../lib/thread -I../lib/ptables  -I../packet -I../MProcess -I./app/ -I../SessionMonitor -I. -I../WBList
CFLAGS += -I${DPDK}/include -include ${DPDK}/include/rte_config.h

LDFLAGS  = ${BUILD_LDFLAGS} ${DPDK_LIB_FLAGS} -lhiredis


common_package = ../lib/common/*.o
cpool_package = ../lib/thread/*.o
shm_package = ../lib/shm/*.o
ptables_package = ../lib/ptables/*.o
pkt_package = ../packet/*.o
mprocess_package = ../MProcess/*.o
smon_package = ../SessionMonitor/*.o
app_package = ./app/*.o
fstore_package = ../lib/fstore/*.o
redis_package = ../lib/redis/*.o
wblist_package = ../WBList/*.o


udp_package= UDPMain

udp_SOURCES = UDPMain.c \
			 ch_udp_context.c \
			 ch_udp_session.c \
			 ch_udp_session_handler.c \
			 ch_udp_session_pool.c \
			 ch_udp_session_task.c \
			 ch_udp_session_task_pool.c \
			 ch_udp_work.c \
			 ch_udp_session_request_pool.c \
			 ch_udp_session_request_handler.c

udp_OBJECTS =$(patsubst %.c,%.o,$(udp_SOURCES))
udp_DEPENDS =$(patsubst %.c,%.d,$(udp_SOURCES))
udp_ASMFILE =$(patsubst %.c,%.s,$(udp_SOURCES))

quiet_cmd_dpdk = LINK   $@ 
	cmd_udp = ${CC} ${CFLAGS} -o $@ $(udp_OBJECTS)  \
				   $(common_package) $(cpool_package) $(shm_package) $(ptables_package)\
				   $(pkt_package)  $(mprocess_package) $(app_package) $(smon_package) $(fstore_package) $(wblist_package)\
				   $(redis_package) \
				   $(LDFLAGS)

.PHONY: all clean

all: $(udp_package)

$(udp_package): $(udp_OBJECTS) 
	$(call cmd,udp)
-include $(udp_DEPENDS)

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
	@rm -rf $(udp_OBJECTS) $(udp_DEPENDS) $(udp_ASMFILE) $(udp_package)



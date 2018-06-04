include ../../make.include

DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)

CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE  ${DPDK_FLAGS} -pthread
CFLAGS += -I../../lib/common  -I../../lib/shm -I../../lib/thread -I../../lib/ptables -I../../lib/fstore 
CFLAGS += -I./http -I./mail
CFLAGS += -I./ -I${DPDK}/include -include ${DPDK}/include/rte_config.h


LDFLAGS  = ${BUILD_LDFLAGS} ${DPDK_LIB_FLAGS} -lmsgpack


common_package = ../../lib/common/*.o
cpool_package = ../../lib/thread/*.o
shm_package = ../../lib/shm/*.o
ptables_package = ../../lib/ptables/*.o
fstore_package = ../../lib/fstore/*.o
http_package = ./http/*.o
mail_package = ./mail/*.o
smtp_package = ./mail/smtp/*.o
imap_package = ./mail/imap/*.o
pop3_package = ./mail/pop3/*.o

pp_package= PPMain

pp_SOURCES = ProtoParse.c \
			 ch_pp_context.c \
			 ch_session_entry_pool.c \
			 ch_session_entry.c \
			 ch_session_store.c \
			 ch_pp_pool.c \
			 ch_pp_work.c

pp_OBJECTS =$(patsubst %.c,%.o,$(pp_SOURCES))
pp_DEPENDS =$(patsubst %.c,%.d,$(pp_SOURCES))
pp_ASMFILE =$(patsubst %.c,%.s,$(pp_SOURCES))

quiet_cmd_dpdk = LINK   $@ 
	cmd_pp = ${CC} ${CFLAGS} -o $@ $(pp_OBJECTS)  \
				   $(common_package) $(cpool_package) $(shm_package) $(ptables_package) $(fstore_package)\
				   $(http_package) $(mail_package) $(smtp_package) $(imap_package) $(pop3_package)\
				   $(LDFLAGS)

.PHONY: all clean

all: $(pp_package)

$(pp_package): $(pp_OBJECTS) 
	$(call cmd,pp)
-include $(pp_DEPENDS)

$(common_package):
	$(call,make)

$(shm_package):
	$(call,make)

$(thread_package):
	$(call,make)

$(ptables_package):
	$(call,make)

$(fstore_package):
	$(call,make)

$(http_package):
	$(call,make)

$(mail_package):
	$(call,make)

$(smtp_package):
	$(call,make)

$(imap_package):
	$(call,make)

$(pop3_package):
	$(call,make)



clean:
	@rm -rf $(pp_OBJECTS) $(pp_DEPENDS) $(pp_ASMFILE) $(pp_package)



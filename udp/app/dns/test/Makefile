include ../../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
DPDK_LIB_FLAGS = $(shell cat ${DPDK}/lib/ldflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS} -pthread
CFLAGS += -I../../../../lib/common  -I../  -I${DPDK}/include -include ${DPDK}/include/rte_config.h

LDFLAGS  = ${BUILD_LDFLAGS} ${DPDK_LIB_FLAGS}


common_package = ../../../../lib/common/*.o
dns_package = ../*.o

test_package= DNSTest

test_SOURCES = DNSTest.c 

test_OBJECTS =$(patsubst %.c,%.o,$(test_SOURCES))
test_DEPENDS =$(patsubst %.c,%.d,$(test_SOURCES))
test_ASMFILE =$(patsubst %.c,%.s,$(test_SOURCES))

quiet_cmd_dpdk = LINK   $@ 
	cmd_test = ${CC} ${CFLAGS} -o $@ $(test_OBJECTS)  \
				   $(common_package) $(dns_package)\
				   $(LDFLAGS)

.PHONY: all clean

all: $(test_package)

$(test_package): $(test_OBJECTS) 
	$(call cmd,test)
-include $(test_DEPENDS)

$(common_package):
	$(call,make)

$(dns_package):
	$(call,make)

clean:
	@rm -rf $(test_OBJECTS) $(test_DEPENDS) $(test_ASMFILE) $(test_package)



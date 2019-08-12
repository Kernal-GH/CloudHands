##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../lib/common  -I../lib/shm -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_packet_SOURCES = ch_packet.c \
					  ch_packet_ether.c \
					  ch_packet_ipv4.c \
					  ch_packet_ipv6.c \
					  ch_packet_tcp.c \
					  ch_packet_udp.c \
					  ch_packet_arp.c \
					  ch_packet_icmp.c

ch_packet_OBJECTS = $(patsubst %.c,%.o,$(ch_packet_SOURCES))
ch_packet_DEPENDS = $(patsubst %.c,%.d,$(ch_packet_SOURCES))
ch_packet_ASMFILE = $(patsubst %.c,%.s,$(ch_packet_SOURCES))

.PHONY: all clean

all: $(ch_packet_OBJECTS)

clean:
	@rm -fr $(ch_packet_OBJECTS) $(ch_packet_DEPENDS) $(ch_packet_ASMFILE) $(ch_packet_package)
	@rm -fr *.d *.o *.s 


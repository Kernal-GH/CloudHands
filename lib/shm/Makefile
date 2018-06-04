##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
CFLAGS += -I../common

ch_shm_SOURCES = ch_shm_interface.c \
		 ch_shm_mmap.c \
		 ch_shm_memory.c \
		 ch_shm_format.c \
		 ch_tcp_record.c \
		 ch_packet_record.c \
		 ch_packet_record_session_tcp.c \
		 ch_packet_record_session_udp.c \
		 ch_packet_record_arp.c \
		 ch_packet_record_icmp.c
ch_shm_OBJECTS = $(patsubst %.c,%.o,$(ch_shm_SOURCES))
ch_shm_DEPENDS = $(patsubst %.c,%.d,$(ch_shm_SOURCES))
ch_shm_ASMFILE = $(patsubst %.c,%.s,$(ch_shm_SOURCES))

.PHONY: all clean

all: $(ch_shm_OBJECTS)

clean:
	@rm -fr $(ch_shm_OBJECTS) $(ch_shm_DEPENDS) $(ch_shm_ASMFILE) $(ch_shm_package)
	@rm -fr *.d *.o *.s 


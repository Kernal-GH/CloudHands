##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../../lib/common -I../../../packet -I../../../lib/ptables -I../../../lib/shm -I../../../lib/fstore
CFLAGS += -I./smtp -I./imap -I./pop3 -I../../../MProcess -I../../../lib/thread -I../../../SessionMonitor -I../../../WBList
CFLAGS += -I../ -I../../ -I. -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_mail_SOURCES = ch_mail.c \
				  ch_mail_session.c \
				  ch_mail_auth.c \
				  ch_mail_common.c \
				  ch_mail_decode_mime.c \
				  ch_mail_file.c \
				  ch_mail_session_entry.c  \
				  smtp/smtp_parser.c \
				  pop3/pop3_parser.c \
				  imap/imap_parser.c

ch_mail_OBJECTS = $(patsubst %.c,%.o,$(ch_mail_SOURCES))
ch_mail_DEPENDS = $(patsubst %.c,%.d,$(ch_mail_SOURCES))
ch_mail_ASMFILE = $(patsubst %.c,%.s,$(ch_mail_SOURCES))

.PHONY: all clean

all: $(ch_mail_OBJECTS)

clean:
	@rm -fr $(ch_mail_OBJECTS) $(ch_mail_DEPENDS) $(ch_mail_ASMFILE) $(ch_mail_package)
	@rm -fr *.d *.o *.s 


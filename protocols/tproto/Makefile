##########################################################
#Author(A) shajianfeng
##########################################################

include ../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../
CFLAGS += -I.
CFLAGS += -I./http
CFLAGS += -I./mail
CFLAGS += -I./debug
CFLAGS += -I./telnet
CFLAGS += -I./ftp
CFLAGS += -I../../util

tproto_SOURCES = ch_proto_tcp.c

tproto_OBJECTS = $(patsubst %.c,%.o,$(tproto_SOURCES))
tproto_DEPENDS = $(patsubst %.c,%.d,$(tproto_SOURCES))
tproto_ASMFILE = $(patsubst %.c,%.s,$(tproto_SOURCES))

.PHONY: all clean

all: $(tproto_OBJECTS)

clean:
	@rm -fr $(tproto_OBJECTS) $(tproto_DEPENDS) $(tproto_ASMFILE)
	@rm -fr *.d *.o *.s

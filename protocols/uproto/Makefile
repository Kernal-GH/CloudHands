##########################################################
#Author(A) shajianfeng
##########################################################

include ../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../
CFLAGS += -I.
CFLAGS += -I./dns
CFLAGS += -I../../util

uproto_SOURCES = ch_proto_udp.c

uproto_OBJECTS = $(patsubst %.c,%.o,$(uproto_SOURCES))
uproto_DEPENDS = $(patsubst %.c,%.d,$(uproto_SOURCES))
uproto_ASMFILE = $(patsubst %.c,%.s,$(uproto_SOURCES))

.PHONY: all clean

all: $(uproto_OBJECTS)

clean:
	@rm -fr $(uproto_OBJECTS) $(uproto_DEPENDS) $(uproto_ASMFILE)
	@rm -fr *.d *.o *.s

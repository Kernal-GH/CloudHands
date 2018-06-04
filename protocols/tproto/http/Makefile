##########################################################
#Author(A) shajianfeng
##########################################################

include ../../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../../
CFLAGS += -I.
CFLAGS += -I../../../util

http_SOURCES = ch_http_context.c \
			ch_http_parser.c \
			ch_http_proto.c \
			ch_http_session.c \
			ch_http_session_body.c \
			ch_http_session_entry.c 

http_OBJECTS = $(patsubst %.c,%.o,$(http_SOURCES))
http_DEPENDS = $(patsubst %.c,%.d,$(http_SOURCES))
http_ASMFILE = $(patsubst %.c,%.s,$(http_SOURCES))

.PHONY: all clean

all: $(http_OBJECTS)

clean:
	@rm -fr $(http_OBJECTS) $(http_DEPENDS) $(http_ASMFILE)
	@rm -fr *.d *.o *.s

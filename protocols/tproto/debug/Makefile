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

debug_SOURCES = ch_debug_context.c \
			ch_debug_proto.c \
			ch_debug_session_entry.c 

debug_OBJECTS = $(patsubst %.c,%.o,$(debug_SOURCES))
debug_DEPENDS = $(patsubst %.c,%.d,$(debug_SOURCES))
debug_ASMFILE = $(patsubst %.c,%.s,$(debug_SOURCES))

.PHONY: all clean

all: $(debug_OBJECTS)

clean:
	@rm -fr $(debug_OBJECTS) $(debug_DEPENDS) $(debug_ASMFILE)
	@rm -fr *.d *.o *.s

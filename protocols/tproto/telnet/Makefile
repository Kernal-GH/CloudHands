
include ../../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread -DTELCMDS -DTELOPTS
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../../
CFLAGS += -I.
CFLAGS += -I../../../util

telnet_SOURCES = ch_telnet_context.c \
				 ch_telnet_parser_do.c  \
				 ch_telnet_proto.c  \
				 ch_telnet_session.c \
				 ch_telnet_session_entry.c \
				 ch_telnet_session_format_msgpack.c \
				 ch_telnet_session_tbl_ops.c \
				 ch_telnet_transfer_data.c

telnet_OBJECTS = $(patsubst %.c,%.o,$(telnet_SOURCES))
telnet_DEPENDS = $(patsubst %.c,%.d,$(telnet_SOURCES))
telnet_ASMFILE = $(patsubst %.c,%.s,$(telnet_SOURCES))

PHONY: all

all:$(telnet_OBJECTS)

clean:
	@rm -fr *.d *.o *.s $(TARGET_TEST)

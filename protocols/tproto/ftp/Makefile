
include ../../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread -DTELCMDS -DTELOPTS
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../../
CFLAGS += -I.
CFLAGS += -I../../../util

ftp_SOURCES = ch_ftp_context.c  \
				 ch_ftp_interactive_content_data.c  \
				 ch_ftp_parser_do.c  \
				 ch_ftp_proto.c  \
				 ch_ftp_session.c  \
				 ch_ftp_session_entry.c  \
				 ch_ftp_session_format_msgpack.c  \
				 ch_ftp_session_tbl_ops.c \
				 ch_ftp_tx_buffer.c

ftp_OBJECTS = $(patsubst %.c,%.o,$(ftp_SOURCES))
ftp_DEPENDS = $(patsubst %.c,%.d,$(ftp_SOURCES))
ftp_ASMFILE = $(patsubst %.c,%.s,$(ftp_SOURCES))

PHONY: all

all:$(ftp_OBJECTS)

clean:
	@rm -fr *.d *.o *.s $(TARGET_TEST)

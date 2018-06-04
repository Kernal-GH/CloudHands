##########################################################
#Author(A) shajianfeng
##########################################################

include ../../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../../
CFLAGS += -I.
CFLAGS += -I./imap
CFLAGS += -I./pop3
CFLAGS += -I./smtp
CFLAGS += -I../../../util

mail_SOURCES =ch_mail_proto.c \
			ch_mail_context.c \
			ch_mail_session_tbl_ops.c \
			ch_mail_session_format_msgpack.c \
			ch_mail_session_entry.c \
			ch_mail_session.c \
			ch_mail_parser_do.c \
			ch_mail_common.c \
			ch_mail_decode_mime.c \
			ch_mail_file.c \
			ch_mail_auth.c \
			imap/imap_parser.c \
			pop3/pop3_parser.c \
			smtp/smtp_parser.c

mail_OBJECTS = $(patsubst %.c,%.o,$(mail_SOURCES))
mail_DEPENDS = $(patsubst %.c,%.d,$(mail_SOURCES))
mail_ASMFILE = $(patsubst %.c,%.s,$(mail_SOURCES))

.PHONY: all clean

all: $(mail_OBJECTS)

clean:
	@rm -fr $(mail_OBJECTS) $(mail_DEPENDS) $(mail_ASMFILE)
	@rm -fr *.d *.o *.s

##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../make.include
CFLAGS += -I../lib/common -I../lib/shm -I./ -I./tcp_app/http -I./tcp_app/debug \
		  -I./tcp_app/ftp -I./tcp_app/imap -I./tcp_app/pop3 -I./tcp_app/smtp \
		  -I./tcp_app/telnet -I./tcp_app

ch_app_SOURCES = ch_app_context.c \
				 tcp_app/ch_app_context_tcp.c \
				 tcp_app/ch_tcp_data_writer.c \
				 tcp_app/debug/ch_debug.c \
				 tcp_app/ftp/ch_ftp.c \
				 tcp_app/http/ch_http.c \
				 tcp_app/imap/ch_imap.c \
				 tcp_app/pop3/ch_pop3.c \
				 tcp_app/smtp/ch_smtp.c \
				 tcp_app/telnet/ch_telnet.c

ch_app_OBJECTS = $(patsubst %.c,%.o,$(ch_app_SOURCES))
ch_app_DEPENDS = $(patsubst %.c,%.d,$(ch_app_SOURCES))
ch_app_ASMFILE = $(patsubst %.c,%.s,$(ch_app_SOURCES))

.PHONY: all clean

all: $(ch_app_OBJECTS)

clean:
	@rm -fr $(ch_app_OBJECTS) $(ch_app_DEPENDS) $(ch_app_ASMFILE) $(ch_app_package)
	@rm -fr *.d *.o *.s 


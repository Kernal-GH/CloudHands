# by shajf
#

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# library name
LIB = libapp.a

CFLAGS += -O3 -g -I$(SRCDIR)/../core \
		  -I$(SRCDIR)/../util \
		  -I$(SRCDIR)/../assemble \
		  -I$(SRCDIR)/../packets \
		  -I$(SRCDIR)/tcp_app \
		  -I$(SRCDIR)/udp_app \
		  -I$(SRCDIR)/udp_app/dns \
		  -I$(SRCDIR)/tcp_app/http \
		  -I$(SRCDIR)/tcp_app/debug \
		  -I$(SRCDIR)/tcp_app/smtp \
		  -I$(SRCDIR)/tcp_app/imap \
		  -I$(SRCDIR)/tcp_app/pop3 \
		  -I$(SRCDIR)/tcp_app/telnet \
		  -I$(SRCDIR)/tcp_app/ftp \
		  -I$(SRCDIR)

CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`

# all source are stored in SRCS-y
SRCS-y := ch_app_context.c \
		  tcp_app/ch_app_context_tcp.c \
		  tcp_app/ch_tcp_data_writer.c \
		  tcp_app/http/ch_http.c \
		  tcp_app/smtp/ch_smtp.c \
		  tcp_app/imap/ch_imap.c \
		  tcp_app/pop3/ch_pop3.c \
		  tcp_app/telnet/ch_telnet.c \
		  tcp_app/ftp/ch_ftp.c \
		  tcp_app/debug/ch_debug.c \
		  udp_app/ch_app_context_udp.c \
		  udp_app/ch_udp_data_writer.c \
		  udp_app/dns/ch_dns.c 
		  

include $(RTE_SDK)/mk/rte.lib.mk

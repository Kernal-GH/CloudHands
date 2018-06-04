
# by shajf
#

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# library name
LIB = libassemble.a

CFLAGS += -O3 -g -I$(SRCDIR)/../core  -I$(SRCDIR)/../util -I$(SRCDIR)/../app -I$(SRCDIR)/../packets -I$(SRCDIR)/../app/tcp_app -I$(SRCDIR)/../app/udp_app -I$(SRCDIR)
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`

# all source are stored in SRCS-y
SRCS-y := ch_assemble_fragment.c \
	      ch_assemble_task.c \
		  ch_assemble_session_endpoint.c \
		  ch_assemble.c \
		  ch_assemble_context_tcp.c \
		  ch_assemble_context_udp.c \
		  ch_assemble_pool.c \
		  ch_assemble_pool_tcp.c \
		  ch_assemble_pool_udp.c \
		  ch_assemble_session_pool_tcp.c \
		  ch_assemble_session_tcp.c \
		  ch_assemble_tcp.c \
		  ch_assemble_udp.c \
		  ch_tcp_session_request_pool.c \
		  ch_ptable.c \
		  ch_entry_pool.c

include $(RTE_SDK)/mk/rte.lib.mk

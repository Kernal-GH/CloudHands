# by shajf
#

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# library name
LIB = libpackets.a

CFLAGS += -O3 -g -I$(SRCDIR)/../core \
		  -I$(SRCDIR)/../util \
		  -I$(SRCDIR)/../assemble \
		  -I$(SRCDIR)

CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`

# all source are stored in SRCS-y
SRCS-y := ch_packets_context.c \
	ch_packets_interface.c

include $(RTE_SDK)/mk/rte.lib.mk

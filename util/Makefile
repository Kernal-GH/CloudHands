
# by shajf
#

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# library name
LIB = libutil.a

CFLAGS += -O3 -g  -I$(SRCDIR)
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`

# all source are stored in SRCS-y
SRCS-y := ch_bitmap.c \
	      ch_bitops.c \
		  ch_errno.c  \
		  ch_file.c   \
		  ch_log.c    \
		  ch_util.c   \
		  ch_mpool.c  \
		  ch_fpool.c \
		  ch_hash_pool.c

include $(RTE_SDK)/mk/rte.lib.mk

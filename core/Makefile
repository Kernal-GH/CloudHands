ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# binary name
APP = cloudhands

# all source are stored in SRCS-y
SRCS-y := cloudhands.c \
	ch_config.c \
	ch_context.c \
	ch_port_pool.c    \
	ch_core_pool.c    \
	ch_task_pool.c    \
	ch_rxtask.c  \
	ch_packet.c  \
	ch_ethertype.c \
	ch_ipv4.c      \
	ch_tcp.c       \
	ch_session_request_pool.c \
	ch_tcp_session.c


CFLAGS += -O3 -g -I$(SRCDIR) -I$(SRCDIR)/../util -I$(SRCDIR)/../assemble -I$(SRCDIR)/../app

CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apu/bin/apu-1-config --includes`
#CFLAGS += $(WERROR_FLAGS)
#CFLAGS_config.o := -D_GNU_SOURCE

#LDLIBS += `/usr/local/apr/bin/apr-1-config --link-ld --libs` 
#LDLIBS += `/usr/local/apu/bin/apu-1-config --link-ld --libs` 
LDLIBS += -L/usr/local/apr/lib -lapr-1 -luuid -lrt -lcrypt  -lpthread -ldl
LDLIBS +=  -L/usr/local/apu/lib -laprutil-1     -lexpat
LDLIBS += -L$(SRCDIR)/../util -L$(SRCDIR)/../assemble -L$(SRCDIR)/../app -L$(SRCDIR)/../app/http -lutil -lassemble -lapp -lhttp

# workaround for a gcc bug with noreturn attribute
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=12603
ifeq ($(CONFIG_RTE_TOOLCHAIN_GCC),y)
CFLAGS_cloudhands.o += -Wno-return-type
endif

DEP_$(APP) += $(SRCDIR)/../util/libutil.a $(SRCDIR)/../assemble/libassemble.a $(SRCDIR)/../app/libapp.a $(SRCDIR)/../app/http/libhttp.a

include $(RTE_SDK)/mk/rte.extapp.mk

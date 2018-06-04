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
	ch_port_context.c \
	ch_context.c \
	ch_port.c    \
	ch_task.c    \
	ch_core_pool.c    \
	ch_rxtask.c  \
	ch_rxtask_pool.c \
	ch_ethertype.c \
	ch_ipv4.c      \
	ch_tcp.c  \
	ch_udp.c


CFLAGS += -O3 -g -I$(SRCDIR) -I$(SRCDIR)/../util -I$(SRCDIR)/../assemble -I$(SRCDIR)/../app -I$(SRCDIR)/../packets -I$(SRCDIR)/../app/tcp_app

CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
#CFLAGS += $(WERROR_FLAGS)
#CFLAGS_config.o := -D_GNU_SOURCE

#LDLIBS += `/usr/local/apr/bin/apr-1-config --link-ld --libs` 
#LDLIBS += `/usr/local/apr/bin/apu-1-config --link-ld --libs` 
LDLIBS += -L/usr/local/apr/lib -lapr-1 -luuid -lrt -lcrypt  -lpthread -ldl
LDLIBS +=  -L/usr/local/apr/lib -laprutil-1     -lexpat
LDLIBS += -L$(SRCDIR)/../assemble -L$(SRCDIR)/../app -L$(SRCDIR)/../packets  -L$(SRCDIR)/../util  -lassemble -lapp -lpackets -lutil 

# workaround for a gcc bug with noreturn attribute
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=12603
ifeq ($(CONFIG_RTE_TOOLCHAIN_GCC),y)
CFLAGS_cloudhands.o += -Wno-return-type
endif

DEP_$(APP) += $(SRCDIR)/../assemble/libassemble.a  $(SRCDIR)/../app/libapp.a $(SRCDIR)/../packets/libpackets.a $(SRCDIR)/../util/libutil.a  

include $(RTE_SDK)/mk/rte.extapp.mk

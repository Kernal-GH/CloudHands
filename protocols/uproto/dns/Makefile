##########################################################
#Author(A) shajianfeng
##########################################################

include ../../../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../../
CFLAGS += -I.
CFLAGS += -I../../../util

dns_SOURCES =ch_dns_context.c \
			 ch_dns_rdata_parser.c \
			 ch_dns_parser.c \
			 ch_dns_session.c \
			 ch_dns_proto.c

dns_OBJECTS = $(patsubst %.c,%.o,$(dns_SOURCES))
dns_DEPENDS = $(patsubst %.c,%.d,$(dns_SOURCES))
dns_ASMFILE = $(patsubst %.c,%.s,$(dns_SOURCES))

.PHONY: all clean

all: $(dns_OBJECTS)

clean:
	@rm -fr $(dns_OBJECTS) $(dns_DEPENDS) $(dns_ASMFILE)
	@rm -fr *.d *.o *.s

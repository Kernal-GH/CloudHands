##########################################################
#Author(A) shajianfeng
##########################################################
include ../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../util
CFLAGS += -I.

LDFLAGS += `/usr/local/apr/bin/apr-1-config --link-ld --libs` 
LDFLAGS += `/usr/local/apr/bin/apu-1-config --link-ld --libs` 

util_package = ../util/libutil.a

main_package = preader 

prMain_SOURCES = ch_packets_reader.c

prMain_OBJECTS =$(patsubst %.c,%.o,$(prMain_SOURCES))
prMain_DEPENDS =$(patsubst %.c,%.d,$(prMain_SOURCES))
prMain_ASMFILE =$(patsubst %.c,%.s,$(prMain_SOURCES))

quiet_cmd_prMain = LINK   $@ 
	cmd_prMain = ${CC} ${CFLAGS} -o $@ $(prMain_OBJECTS)  \
				   $(mail_package) $(util_package)  \
				   $(LDFLAGS)

.PHONY: all clean

all: $(main_package)

$(main_package): $(prMain_OBJECTS) 
	$(call cmd,prMain)
-include $(prMain_DEPENDS)

clean:
	@rm -rf $(prMain_OBJECTS) $(prMain_DEPENDS) $(prMain_ASMFILE) $(main_package)


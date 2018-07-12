##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../make.include
CFLAGS += -I../lib/common

ch_smon_SOURCES = ch_session_monitor.c

ch_smon_OBJECTS = $(patsubst %.c,%.o,$(ch_smon_SOURCES))
ch_smon_DEPENDS = $(patsubst %.c,%.d,$(ch_smon_SOURCES))
ch_smon_ASMFILE = $(patsubst %.c,%.s,$(ch_smon_SOURCES))

.PHONY: all clean

all: $(ch_smon_OBJECTS)

clean:
	@rm -fr $(ch_smon_OBJECTS) $(ch_smon_DEPENDS) $(ch_smon_ASMFILE) $(ch_smon_package)
	@rm -fr *.d *.o *.s 


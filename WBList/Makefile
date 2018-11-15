##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../make.include
CFLAGS += -I../lib/common

ch_wbm_SOURCES = ch_wb_memory.c \
				 ch_wb_list.c \
				 ch_wb_list_ip.c \
				 ch_wb_list_str.c

ch_wbm_OBJECTS = $(patsubst %.c,%.o,$(ch_wbm_SOURCES))
ch_wbm_DEPENDS = $(patsubst %.c,%.d,$(ch_wbm_SOURCES))
ch_wbm_ASMFILE = $(patsubst %.c,%.s,$(ch_wbm_SOURCES))

.PHONY: all clean

all: $(ch_wbm_OBJECTS)

clean:
	@rm -fr $(ch_wbm_OBJECTS) $(ch_wbm_DEPENDS) $(ch_wbm_ASMFILE) $(ch_wbm_package)
	@rm -fr *.d *.o *.s 


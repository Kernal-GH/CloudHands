##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
CFLAGS += -I../common

ch_rdb_SOURCES = ch_rdb_store_pool.c \
				 ch_rdb_store.c

ch_rdb_OBJECTS = $(patsubst %.c,%.o,$(ch_rdb_SOURCES))
ch_rdb_DEPENDS = $(patsubst %.c,%.d,$(ch_rdb_SOURCES))
ch_rdb_ASMFILE = $(patsubst %.c,%.s,$(ch_rdb_SOURCES))

.PHONY: all clean

all: $(ch_rdb_OBJECTS)

clean:
	@rm -fr $(ch_rdb_OBJECTS) $(ch_rdb_DEPENDS) $(ch_rdb_ASMFILE) $(ch_rdb_package)
	@rm -fr *.d *.o *.s 


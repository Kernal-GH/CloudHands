##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
CFLAGS  = ${BUILD_CFLAGS}  -I../common -O0 -rdynamic -D_GNU_SOURCE -pthread
CFLAGS +=  -fPIC

ch_fstore_SOURCES = ch_fpath.c \
					ch_file_store.c \
					ch_sfile.c

ch_fstore_OBJECTS = $(patsubst %.c,%.o,$(ch_fstore_SOURCES))
ch_fstore_DEPENDS = $(patsubst %.c,%.d,$(ch_fstore_SOURCES))
ch_fstore_ASMFILE = $(patsubst %.c,%.s,$(ch_fstore_SOURCES))

.PHONY: all clean

all: $(ch_fstore_OBJECTS)

clean:
	@rm -fr $(ch_fstore_OBJECTS) $(ch_fstore_DEPENDS) $(ch_fstore_ASMFILE) $(ch_fstore_package)
	@rm -fr *.d *.o *.s 


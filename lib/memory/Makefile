##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -pthread
CFLAGS +=  -fPIC -I../common

ch_memory_SOURCES = ch_hugepage_info.c \
					ch_hugepage_file.c \
					ch_memseg.c \
					ch_memory_pool.c

ch_memory_OBJECTS = $(patsubst %.c,%.o,$(ch_memory_SOURCES))
ch_memory_DEPENDS = $(patsubst %.c,%.d,$(ch_memory_SOURCES))
ch_memory_ASMFILE = $(patsubst %.c,%.s,$(ch_memory_SOURCES))

.PHONY: all clean

all: $(ch_memory_OBJECTS)

clean:
	@rm -fr $(ch_memory_OBJECTS) $(ch_memory_DEPENDS) $(ch_memory_ASMFILE) $(ch_memory_package)
	@rm -fr *.d *.o *.s 


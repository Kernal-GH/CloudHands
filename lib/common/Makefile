##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -pthread
CFLAGS +=  -fPIC

ch_common_SOURCES = ch_mpool.c \
			 ch_printf.c \
			 ch_strtok.c \
			 ch_strnatcmp.c \
			 ch_cpystrn.c \
			 ch_tables.c \
			 ch_fnmatch.c \
			 ch_string.c \
			 ch_errno.c \
			 ch_file_readwrite.c \
			 ch_file_fullrw.c \
			 ch_dir.c \
			 ch_fileacc.c \
			 ch_filepath.c \
			 ch_config.c \
			 ch_time.c \
			 ch_strtime.c \
			 ch_filestat.c \
			 ch_file_open.c \
			 ch_getopt.c \
			 ch_signal.c \
			 ch_util.c \
			 ch_log.c \
			 ch_object_pool.c \
			 ch_net_util.c \
			 ch_uri.c \
			 ch_msgpack_store.c 

ch_common_OBJECTS = $(patsubst %.c,%.o,$(ch_common_SOURCES))
ch_common_DEPENDS = $(patsubst %.c,%.d,$(ch_common_SOURCES))
ch_common_ASMFILE = $(patsubst %.c,%.s,$(ch_common_SOURCES))

.PHONY: all clean

all: $(ch_common_OBJECTS)

clean:
	@rm -fr $(ch_common_OBJECTS) $(ch_common_DEPENDS) $(ch_common_ASMFILE) $(ch_common_package)
	@rm -fr *.d *.o *.s 


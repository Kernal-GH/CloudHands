##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../make.include
CFLAGS += -I../common

ch_redis_SOURCES = ch_redis.c \
				   ch_redis_ip_wblist.c

ch_redis_OBJECTS = $(patsubst %.c,%.o,$(ch_redis_SOURCES))
ch_redis_DEPENDS = $(patsubst %.c,%.d,$(ch_redis_SOURCES))
ch_redis_ASMFILE = $(patsubst %.c,%.s,$(ch_redis_SOURCES))

.PHONY: all clean

all: $(ch_redis_OBJECTS)

clean:
	@rm -fr $(ch_redis_OBJECTS) $(ch_redis_DEPENDS) $(ch_redis_ASMFILE) $(ch_redis_package)
	@rm -fr *.d *.o *.s 


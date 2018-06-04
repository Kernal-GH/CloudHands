##########################################################
#Author(A) shajianfeng
##########################################################
include ../make.include

CFLAGS  = ${BUILD_CFLAGS}  -g -rdynamic  -pthread 
CFLAGS += `/usr/local/apr/bin/apr-1-config --includes`
CFLAGS += `/usr/local/apr/bin/apu-1-config --includes`
CFLAGS += -I../util
CFLAGS += -I.
CFLAGS += -I./tproto
CFLAGS += -I./uproto

LDFLAGS  = ${BUILD_LDFLAGS} -lmsgpack -lz
LDFLAGS += `/usr/local/apr/bin/apr-1-config --link-ld --libs` 
LDFLAGS += `/usr/local/apr/bin/apu-1-config --link-ld --libs` 

util_package = ../util/libutil.a
mail_package = ./tproto/mail/*.o ./tproto/mail/imap/*.o ./tproto/mail/pop3/*.o ./tproto/mail/smtp/*.o
http_package = ./tproto/http/*.o
debug_package = ./tproto/debug/*.o
telnet_package = ./tproto/telnet/*.o
ftp_package = ./tproto/ftp/*.o
tproto_package=./tproto/*.o
uproto_package=./uproto/*.o
dns_package=./uproto/dns/*.o

main_package = proto_main 

protoMain_SOURCES = ch_proto_context.c \
					ch_proto_pool.c \
					ch_proto_work.c \
					ch_session_format.c \
					ch_session_format_factory.c \
					ch_session_format_msgpack.c \
					ch_proto_main.c

protoMain_OBJECTS =$(patsubst %.c,%.o,$(protoMain_SOURCES))
protoMain_DEPENDS =$(patsubst %.c,%.d,$(protoMain_SOURCES))
protoMain_ASMFILE =$(patsubst %.c,%.s,$(protoMain_SOURCES))

quiet_cmd_protoMain = LINK   $@ 
	cmd_protoMain = ${CC} ${CFLAGS} -o $@ $(protoMain_OBJECTS)  \
				   $(mail_package) $(http_package) $(debug_package) $(telnet_package) $(ftp_package) $(tproto_package) $(dns_package) $(uproto_package) $(util_package)  \
				   $(LDFLAGS)

.PHONY: all clean

all: $(main_package)

$(main_package): $(protoMain_OBJECTS) 
	$(call cmd,protoMain)
-include $(protoMain_DEPENDS)

$(tproto_package):
	$(call,make)

$(http_package):
	$(call,make)

$(debug_package):
	$(call,make)

$(telnet_package):
	$(call,make)

$(ftp_package):
	$(call,make)

$(mail_package):
	$(call,make)

$(uproto_package):
	$(call,make)

$(dns_package):
	$(call,make)

clean:
	@rm -rf $(protoMain_OBJECTS) $(protoMain_DEPENDS) $(protoMain_ASMFILE) $(main_package)


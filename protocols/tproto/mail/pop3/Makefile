CC=gcc

CFLAGS=-g

INCS=-I./ -I../ -I../../util -I./util/ -I/usr/local/apr/include/apr-1

LIBS=-L/usr/local/apr/lib -lapr-1 -laprutil-1 

OBJS= pop3_mail.o ../ch_parser_decode_mime.o pop3_parser.o \
	  ../app_layer_parser.o ../ch_parser_conf.o ../ch_parser_mail_common.o \
	  ../ch_parser_file.o ../ch_parser_common.o

OBJS += ../libutil.a 

OBJS_PRO=pop3test

POP3_OBJS=$(OBJS) pop3test.o

PHONY:all

all:$(OBJS_PRO)

pop3test:$(POP3_OBJS)
	${CC} -o $@ $^ $(CFLAGS) $(INCS) $(LIBS)

%.o:%.c
	${CC} -c -o $@ $^ $(CFLAGS) $(INCS) $(LIBS)

clean:
	rm -fr *.o $(OBJS_PRO)


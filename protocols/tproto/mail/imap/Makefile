CC=gcc

CFLAGS=-g

INCS=-I./ -I../ -I../../util -I/usr/local/apr/include/apr-1

LIBS=-L/usr/local/apr/lib -lapr-1 -laprutil-1 

OBJS=imap_parser.o ../app_layer_parser.o  ../ch_parser_decode_mime.o \
	 ../ch_parser_string.o ../ch_parser_base64.o ../ch_parser_mail_common.o \
	 ../ch_parser_conf.o ../ch_parser_common.o ../ch_parser_file.o \
	 ../ch_parser_network.o

OBJS += ../libutil.a 

OBJS_TEST=$(OBJS) test.o 

TEST_PROG=test_imap

all:$(TEST_PROG)

$(TEST_PROG):$(OBJS_TEST)
	$(CC) -o $@ $^ $(INCS) $(CFLAGS) $(LIBS)

%.o:%.c
	$(CC) -c -o $@ $^ $(INCS) $(CFLAGS) $(LIBS)

clean:
	rm -fr *.o $(TEST_PROG)

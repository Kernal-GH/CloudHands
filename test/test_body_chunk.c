/*
 * =====================================================================================
 *
 *       Filename:  test_body_chunk.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月24日 14时48分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define LF     (unsigned char) '\n'
#define CR     (unsigned char) '\r'
#define CRLF   "\r\n"


#define CH_AGAIN                          14
#define CH_DONE                           15
#define CH_ERROR                          -1
#define CH_OK                              0

#define CH_MAX_SIZE_T_VALUE (sizeof(size_t)==8?18446744073709551615L:4294967295)

typedef struct {

    unsigned char *start;       /*start address waited to parse*/
    unsigned char *pos;         /*the position waiting to handle*/
    unsigned char *end;    /* end address waited to parse */

    unsigned state; /*the current parsed state*/
}ch_http_session_parse_t;

typedef struct {
    ch_http_session_parse_t parser;
    size_t size;
    size_t length;
    void *data;
}ch_http_body_chunk_t;

static int http_session_body_chunked_parser(ch_http_body_chunk_t *chunk){

    unsigned char     *pos, ch, c;
    int   rc;
    enum {
        sw_chunk_start = 0,
        sw_chunk_size,
        sw_chunk_extension,
        sw_chunk_extension_almost_done,
        sw_chunk_data,
        sw_after_data,
        sw_after_data_almost_done,
        sw_last_chunk_extension,
        sw_last_chunk_extension_almost_done,
        sw_trailer,
        sw_trailer_almost_done,
        sw_trailer_header,
        sw_trailer_header_almost_done
    } state;

    ch_http_session_parse_t *parser = &chunk->parser;

    state = parser->state;

    if (state == sw_chunk_data && chunk->size == 0) {
        state = sw_after_data;
    }

    rc = CH_AGAIN;

    for (pos = parser->pos; pos < parser->end; pos++) {

        ch = *pos;

        switch (state) {

        case sw_chunk_start:
            if (ch >= '0' && ch <= '9') {
                state = sw_chunk_size;
                chunk->size = ch - '0';
                break;
            }

            c = (unsigned char) (ch | 0x20);

            if (c >= 'a' && c <= 'f') {
                state = sw_chunk_size;
                chunk->size = c - 'a' + 10;
                break;
            }

            goto invalid;

        case sw_chunk_size:
            if (chunk->size > CH_MAX_SIZE_T_VALUE / 16) {
                goto invalid;
            }

            if (ch >= '0' && ch <= '9') {
                chunk->size = chunk->size * 16 + (ch - '0');
                break;
            }

            c = (unsigned char) (ch | 0x20);

            if (c >= 'a' && c <= 'f') {
                chunk->size = chunk->size * 16 + (c - 'a' + 10);
                break;
            }

            if (chunk->size == 0) {

                switch (ch) {
                case CR:
                    state = sw_last_chunk_extension_almost_done;
                    break;
                case LF:
                    state = sw_trailer;
                    break;
                case ';':
                case ' ':
                case '\t':
                    state = sw_last_chunk_extension;
                    break;
                default:
                    goto invalid;
                }

                break;
            }

            switch (ch) {
            case CR:
                state = sw_chunk_extension_almost_done;
                break;
            case LF:
                state = sw_chunk_data;
                break;
            case ';':
            case ' ':
            case '\t':
                state = sw_chunk_extension;
                break;
            default:
                goto invalid;
            }

            break;

        case sw_chunk_extension:
            switch (ch) {
            case CR:
                state = sw_chunk_extension_almost_done;
                break;
            case LF:
                state = sw_chunk_data;
            }
            break;

        case sw_chunk_extension_almost_done:
            if (ch == LF) {
                state = sw_chunk_data;
                break;
            }
            goto invalid;

        case sw_chunk_data:
            rc = CH_OK;
            goto data;

        case sw_after_data:
            switch (ch) {
            case CR:
                state = sw_after_data_almost_done;
                break;
            case LF:
                state = sw_chunk_start;
            }
            break;

        case sw_after_data_almost_done:
            if (ch == LF) {
                state = sw_chunk_start;
                break;
            }
            goto invalid;

        case sw_last_chunk_extension:
            switch (ch) {
            case CR:
                state = sw_last_chunk_extension_almost_done;
                break;
            case LF:
                state = sw_trailer;
            }
            break;

        case sw_last_chunk_extension_almost_done:
            if (ch == LF) {
                state = sw_trailer;
                break;
            }
            goto invalid;

        case sw_trailer:
            switch (ch) {
            case CR:
                state = sw_trailer_almost_done;
                break;
            case LF:
                goto done;
            default:
                state = sw_trailer_header;
            }
            break;

        case sw_trailer_almost_done:
            if (ch == LF) {
                goto done;
            }
            goto invalid;

        case sw_trailer_header:
            switch (ch) {
            case CR:
                state = sw_trailer_header_almost_done;
                break;
            case LF:
                state = sw_trailer;
            }
            break;

        case sw_trailer_header_almost_done:
            if (ch == LF) {
                state = sw_trailer;
                break;
            }
            goto invalid;

        }
    }

data:

    parser->state = state;
    parser->pos = pos;

    if (chunk->size > CH_MAX_SIZE_T_VALUE - 5) {
        goto invalid;
    }

    switch (state) {

    case sw_chunk_start:
        chunk->length = 3 /* "0" LF LF */;
        break;
    case sw_chunk_size:
        chunk->length = 1 /* LF */
                      + (chunk->size ? chunk->size + 4 /* LF "0" LF LF */
                                   : 1 /* LF */);
        break;
    case sw_chunk_extension:
    case sw_chunk_extension_almost_done:
        chunk->length = 1 /* LF */ + chunk->size + 4 /* LF "0" LF LF */;
        break;
    case sw_chunk_data:
        chunk->length = chunk->size + 4 /* LF "0" LF LF */;
        break;
    case sw_after_data:
    case sw_after_data_almost_done:
        chunk->length = 4 /* LF "0" LF LF */;
        break;
    case sw_last_chunk_extension:
    case sw_last_chunk_extension_almost_done:
        chunk->length = 2 /* LF LF */;
        break;
    case sw_trailer:
    case sw_trailer_almost_done:
        chunk->length = 1 /* LF */;
        break;
    case sw_trailer_header:
    case sw_trailer_header_almost_done:
        chunk->length = 2 /* LF LF */;
        break;

    }

    return rc;

done:

    parser->state = 0;
    parser->pos = pos + 1;

    return CH_DONE;

invalid:

    return CH_ERROR;
}



static void parse_body_chunk(ch_http_body_chunk_t *chunk,FILE *fp){

    int rc;
    size_t size,writen=0;
    void *data;

    ch_http_session_parse_t *parser = &chunk->parser;

    for(;;){

        rc = http_session_body_chunked_parser(chunk);
        if(rc == CH_OK){
            /* a chunk has been parsed successfully */
            size = parser->end-parser->pos;
            if(size>chunk->size){
                writen = chunk->size;
                data = parser->pos;
                parser->pos += chunk->size;
                chunk->size = 0;
            }else{
                writen = size;
                data = parser->pos;
                chunk->size-=size;
                parser->pos = parser->end;
            }

            fwrite(data,1,writen,fp);

            continue;
        }

        if(rc == CH_DONE){
            fprintf(stdout,"Parse chunk body done!\n");
            break;
        }

        if(rc == CH_AGAIN){
            fprintf(stdout,"need some data to parse!\n");
            break;
        }

        /*invalid*/
        fprintf(stderr,"invalid chunk body!\n");
        exit(-1);
    }
}

int main(int argc,char ** argv){

    ch_http_body_chunk_t chunk;

    const char *fname;
    const char *out_fname;
    unsigned char buf[1024];
    FILE *fp,*fp_out;
    
    if(argc<3){

        fprintf(stderr,"Usage:<in file name> <out file name>\n");

        exit(-1);
    }

    fname = argv[1];
    out_fname = argv[2];

    fp = fopen(fname,"r");
    fp_out = fopen(out_fname,"a+");
    memset(&chunk,0,sizeof(ch_http_body_chunk_t));

    while(1){

        size_t n = fread(buf,1,1023,fp);
        chunk.parser.pos = buf;
        chunk.parser.start = buf;
        chunk.parser.end = buf+n;

        if(n!=1023){
            if(n){
                parse_body_chunk(&chunk,fp_out);
            }
            break;
        }
        parse_body_chunk(&chunk,fp_out);
    }

    fclose(fp);
    fclose(fp_out);

    return 0;
}

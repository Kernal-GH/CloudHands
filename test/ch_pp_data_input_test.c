/*
 *
 *      Filename: ch_pp_data_input_test.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-15 12:04:41
 * Last Modified: 2018-05-15 13:49:58
 */

#include <stdio.h>
#include <stdlib.h>

#include "ch_pp_data_input.h"
#include "ch_buffer.h"

static unsigned char *test_str = "wo shi yi ge shuaige\r\nni zhidaobuzhidao\ncao aaaa!";
static unsigned char *rl ="\r\n";

static void _print_line(ch_pp_data_input_t *din,ch_pp_data_line_t *dline){

	char b[1024];
	size_t rlen;

	while(1){
	
		if(ch_pp_data_input_line_read(din,dline))
			break;
	
		rlen = dline->len>1024?1023:dline->len;


		memcpy(b,dline->line,rlen);
		b[rlen] = 0;

		printf("%s  %lu\n",b,(unsigned long)dline->len);
	
	}

}

int main(int argc,char **argv){

	unsigned char b[4096] = {0};
	size_t dlen;

	const char *fname;
	FILE *fp;
	ch_pp_data_line_t dline;

	ch_buffer_t g_buffer,*g_buffer_ptr = &g_buffer;
	ch_pp_data_input_t tmp,*din=&tmp;

	ch_pool_t *mp = ch_pool_create(1024);

	ch_buffer_init(g_buffer_ptr);

	fname = argv[1];

	dlen = (size_t)atoi(argv[2]);

	fp = fopen(fname,"r");

	fread(b,4096,1,fp);

	ch_pp_din_init(din,g_buffer_ptr,mp);

	ch_pp_din_read_prefare(din,(void*)b,dlen);

	_print_line(din,&dline);

	ch_pp_din_read_end(din);

	ch_pp_din_read_prefare(din,(void*)test_str,strlen(test_str));

	_print_line(din,&dline);
	ch_pp_din_read_end(din);

	ch_pp_din_read_prefare(din,(void*)rl,strlen(rl));
	_print_line(din,&dline);
	ch_pp_din_read_end(din);

	return 0;
}

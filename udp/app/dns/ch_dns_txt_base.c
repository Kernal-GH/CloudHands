/*
 *
 *      Filename: ch_dns_txt_base.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-15 10:15:28
 * Last Modified: 2018-06-15 10:26:38
 */

#include "ch_dns_txt_base.h"

void ch_dns_txt_base_init(ch_pool_t *mp,ch_dns_txt_base_t *txt_base){

	txt_base->txts = ch_array_make(mp,8,sizeof(unsigned char*));

}

int ch_dns_txt_base_parse(ch_pool_t *mp,ch_dns_txt_base_t *txt_base,ch_dns_data_input_t *din){

	unsigned char *txt;

	while(!ch_dns_data_input_read_over(din,1)){
	
		txt = ch_dns_data_input_bstring_read_dup(din,mp);
		if(txt == NULL)
			break;

		ch_dns_txt_base_add(txt_base,txt);
	}

	return 0;

}

void ch_dns_txt_base_dump(ch_dns_txt_base_t *txt_base,FILE *fp){

	unsigned char ** texts,*str;
	int i;

	texts = (unsigned char**)txt_base->txts->elts;

	for(i = 0;i<txt_base->txts->nelts;i++){
	
		str = texts[i];
		fprintf(fp,"%s\n",str);
	}

}


ssize_t ch_dns_txt_base_write(ch_dns_txt_base_t *txt_base,ch_data_output_t *dout){

	ssize_t rc,len = 0;
	unsigned char ** texts,*str;
	int i;

	texts = (unsigned char**)txt_base->txts->elts;
	uint16_t n = txt_base->txts->nelts;

	CH_DOUT_UINT16_WRITE(dout,n,len,rc);
	
	for(i = 0;i<n;i++){
	
		str = texts[i];

		CH_DOUT_STRING16_WRITE2(dout,str,len,rc);

	}

	return len;

}

void ch_dns_txt_base_store(ch_dns_txt_base_t *txt_base,ch_msgpack_store_t *dstore){


    unsigned char ** texts,*str;
    int i;

    texts = (unsigned char**)txt_base->txts->elts;
    uint16_t n = txt_base->txts->nelts;  

    ch_msgpack_store_array_start(dstore,"texts",n);

    for(i = 0;i<n;i++){

        str = texts[i];  
        ch_msgpack_store_write_str(dstore,(const char*)str);
    }
}

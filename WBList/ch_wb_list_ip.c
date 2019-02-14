/*
 *
 *      Filename: ch_wb_list_ip.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-04 13:26:51
 * Last Modified: 2018-09-05 18:39:18
 */

#include "ch_wb_list_ip.h"
#include "ch_net_util.h"

#define NetMaskAnd(v,netmask) ((v)&(netmask))

static int list_ip_add(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *value){

	/*unused*/
	wb_list = wb_list;

	ch_wb_list_ip_entry_t *ip_entry = (ch_wb_list_ip_entry_t*)entry;
	ch_wb_list_ip_add_value_t *add_v = (ch_wb_list_ip_add_value_t*)value;

	if(add_v->ip_start>add_v->ip_end||add_v->port_start>add_v->port_end)
		return -1;

	if(NetMaskAnd(add_v->ip_start,add_v->netmask)!=NetMaskAnd(add_v->ip_end,add_v->netmask))
		return -1;

	ip_entry->netmask = add_v->netmask;
	ip_entry->ip_start = add_v->ip_start;
	ip_entry->ip_end = add_v->ip_end;
	ip_entry->port_start = add_v->port_start;
	ip_entry->port_end = add_v->port_end;

	return 0;
}

static void list_ip_entry_copy(ch_wb_list_t *wb_list,ch_wb_list_entry_t *d_entry,ch_wb_list_entry_t *s_entry){

	/*unused*/
	wb_list = wb_list;

	ch_wb_list_ip_entry_t *d_ip_entry = (ch_wb_list_ip_entry_t*)d_entry;
	ch_wb_list_ip_entry_t *s_ip_entry = (ch_wb_list_ip_entry_t*)s_entry;
	d_ip_entry->netmask = s_ip_entry->netmask;
	d_ip_entry->ip_start = s_ip_entry->ip_start;
	d_ip_entry->ip_end = s_ip_entry->ip_end;
	d_ip_entry->port_start = s_ip_entry->port_start;
	d_ip_entry->port_end = s_ip_entry->port_end;


}

#define RangeMatch(v,start,end) (((v)>=(start))&&((v)<=(end)))

static int list_ip_is_match(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *match_value){

	/*unused*/
	wb_list = wb_list;

	ch_wb_list_ip_entry_t *ip_entry = (ch_wb_list_ip_entry_t*)entry;
	ch_wb_list_ip_match_value_t *mv = (ch_wb_list_ip_match_value_t*)match_value;

	int ip_is_match=1,port_is_match=1;
	uint32_t netmask = ip_entry->netmask;

	if(ip_entry->ip_start!=0||ip_entry->ip_end!=0){

		if(NetMaskAnd(ip_entry->ip_start,netmask)!=NetMaskAnd(mv->ip,netmask))
			ip_is_match = 0;
		else
			ip_is_match = RangeMatch(mv->ip,ip_entry->ip_start,ip_entry->ip_end);
	}
	if(ip_entry->port_start!=0||ip_entry->port_end!=0){
	
		port_is_match = RangeMatch(mv->port,ip_entry->port_start,ip_entry->port_end);
	}

	return ip_is_match&&port_is_match;

}

static void list_ip_dump(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,FILE *out){

	/*unused*/
	wb_list = wb_list;

	char buf[64] = {0};

	ch_wb_list_ip_entry_t *ip_entry = (ch_wb_list_ip_entry_t*)entry;
	fprintf(out,"Dump The IP Entry Info:\n");
	
	fprintf(out,"IPEntry.netmask:%s\n",ch_ip_to_str(buf,63,ip_entry->netmask));
	fprintf(out,"IPEntry.startIP:%s\n",ch_ip_to_str(buf,63,ip_entry->ip_start));
	fprintf(out,"IPEntry.endIP:%s\n",ch_ip_to_str(buf,63,ip_entry->ip_end));

	fprintf(out,"IPEntry.startPort:%lu\n",(unsigned long)ip_entry->port_start);
	fprintf(out,"IPEntry.endPort:%lu\n",(unsigned long)ip_entry->port_end);

}

int ch_wb_list_ip_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize){


	return ch_wb_list_init(wb_list,mmap_fname,msize,sizeof(ch_wb_list_ip_entry_t),
		list_ip_add,
		list_ip_entry_copy,
		list_ip_is_match,
		list_ip_dump);

}

/*
 *
 *      Filename: ch_dns_name.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-26 19:08:33
 * Last Modified: 2018-05-11 20:12:02
 */

#include "ch_dns_name.h"
#include "ch_string.h"

typedef enum {
	fw_start = 0,
	fw_ordinary,
	fw_newcurrent
} fw_state;

int ch_dns_name_parse(ch_pool_t *mp,ch_dns_data_input_t *din,ch_dns_name_t *dname){

	unsigned char *cdata,*base;
	unsigned char tmp_data[DNS_NAME_MAXLEN] = {0},*ndata = tmp_data;

	unsigned int cused; /* Bytes of compressed name data used */
	unsigned int nused, labels, n, nmax,len;
	unsigned int current, new_current, biggest_pointer,last_offset;
	int done;
	fw_state state = fw_start;
	unsigned int c;
	int seen_pointer;

	n = 0;
	new_current = 0;

	/*
	 * Set up.
	 */
	labels = 0;
	done = 0;

	nused = 0;
	seen_pointer = 0;
	len = 0;

	/*
	 * Find the maximum number of uncompressed target name
	 * bytes we are willing to generate.  This is the smaller
	 * of the available target buffer length and the
	 * maximum legal domain name length (255).
	 */
	nmax = ch_dns_data_input_offset_last(din);
	if (nmax > DNS_NAME_MAXLEN)
		nmax = DNS_NAME_MAXLEN;

	base = (unsigned char*)ch_dns_data_input_base(din);
	cdata = (unsigned char*)ch_dns_data_input_pos(din);
	cused = 0;

	current = ch_dns_data_input_offset_pos(din);
	biggest_pointer = current;
	last_offset = ch_dns_data_input_offset_last(din);

	/*
	 * Note:  The following code is not optimized for speed, but
	 * rather for correctness.  Speed will be addressed in the future.
	 */

	while (current < last_offset && !done) {
		c = *cdata++;
		current++;
		if (!seen_pointer)
			cused++;

		switch (state) {
		case fw_start:
			if (c < 64) {
				
				if(c!=0)
					labels++;

				if (nused + c + 1 > nmax)
					goto full;
				nused += c + 1;
				len+=c;

				if(labels!=1&&c){
					*ndata++ = '.';
					len+=1;
				}

				if (c == 0)
					done = 1;
				n = c;
				state = fw_ordinary;
			} else if (c >= 128 && c < 192) {
				/*
				 * 14 bit local compression pointer.
				 * Local compression is no longer an
				 * IETF draft.
				 */
				return -1;
			} else if (c >= 192) {
				/*
				 * Ordinary 14-bit pointer.
				 */
				new_current = c & 0x3F;
				state = fw_newcurrent;
			} else
				return -1;
			break;

		case fw_ordinary:
			*ndata++ = c;
			n--;
			if (n == 0)
				state = fw_start;
			break;

		case fw_newcurrent:
			new_current *= 256;
			new_current += c;
			if (new_current >= biggest_pointer)
				return -1;

			biggest_pointer = new_current;
			current = new_current;
			cdata = base + current;
			seen_pointer = 1;
			state = fw_start;
			break;
		default:
			/* Does not return. */
			break;
		}
	}

	if (!done)
		return -1;

	dname->ndata = (unsigned char*)ch_psprintf(mp,"%s",tmp_data);
	dname->labels = labels;
	dname->length = len;

	ch_dns_data_input_pos_update(din,cused);

	return 0;

 full:
	if (nmax == DNS_NAME_MAXLEN)
		/*
		 * The name did not fit even though we had a buffer
		 * big enough to fit a maximum-length name.
		 */
		return -1;
	else
		/*
		 * The name might fit if only the caller could give us a
		 * big enough buffer.
		 */
		return -1;

}

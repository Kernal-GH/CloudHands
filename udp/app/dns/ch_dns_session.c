/*
 *
 *      Filename: ch_dns_session.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-10 14:48:56
 * Last Modified: 2018-05-10 14:54:34
 */

#include "ch_dns_session.h"

ch_dns_session_t * ch_dns_session_create(ch_pool_t *mp){


	ch_dns_session_t *dns_session = (ch_dns_session_t*)ch_palloc(mp,sizeof(*dns_session));

	dns_session->mp = mp;
	dns_session->dns_req = NULL;
	dns_session->dns_res = NULL;



	return dns_session;
}


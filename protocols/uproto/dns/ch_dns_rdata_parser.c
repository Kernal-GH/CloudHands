/*
 *
 *      Filename: ch_dns_rdata_parser.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-31 23:10:44
 * Last Modified: 2016-11-02 04:02:14
 */


// This is used when a parser isn't defined for a given class, rtypes.
// Note: The NULL in the name field is important, and used to distinguish
//       this from the rest of the parser containers.

#include <apr_strings.h>
#include "ch_session_format_msgpack.h"
#include "ch_dns_rdata_parser.h"
#include "ch_string.h"

static ch_dns_rdata_parse_t default_rr_parser = {0, 0, _rdata_escape_parse, 0};

//A records are simply an IPv4 address, and are formatted as such

static inline ch_dns_rdata_t * _make_dns_rdata(apr_pool_t *mp,void *data,
	void (*msgpack_format)(msgpack_packer *pk,ch_dns_rdata_t *rdata)){


	ch_dns_rdata_t *rdata = (ch_dns_rdata_t*)apr_palloc(mp,sizeof(*rdata));

	rdata->data_parsed = data;
	rdata->msgpack_format = msgpack_format;

	return rdata;
}

static void _rdata_ipv4_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	ch_msgpack_map_start(pk,"rdata",1);
	ch_msgpack_write_kv(pk,"ipv4",(const char*)rdata->data_parsed);
}

static ch_dns_rdata_t*  _rdata_ipv4_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t i,
         uint16_t rdlength, uint32_t plen) {

	/* unused */
	i = i;
	plen = plen;

	const char *value;

	if(rdlength!=4)
	{
	
		return _make_dns_rdata(mp,(void*)"0.0.0.0",_rdata_ipv4_format);
	}


    value =  apr_psprintf(mp, "%d.%d.%d.%d", packet[pos], packet[pos+1],
                                 packet[pos+2], packet[pos+3]);

	return _make_dns_rdata(mp,(void*)value,_rdata_ipv4_format);
}

//A DNS like name. This format is used for many record types

static void _rdata_domain_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	ch_msgpack_map_start(pk,"rdata",1);
	ch_msgpack_write_kv(pk,"domain",(const char*)rdata->data_parsed);
}

static ch_dns_rdata_t *  _rdata_domain_name_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                   uint16_t rdlength, uint32_t plen) {

	/* unused */
	rdlength = rdlength;

	char *name;

    name = ch_read_rr_name(mp,packet, &pos, id_pos, plen);

	if(name == NULL){
	
		name = "Bad Host";
	}
	return _make_dns_rdata(mp,(void*)name,_rdata_domain_format);
}

//Presented as a series of labeled SOA fields
typedef struct {
	
	const char *mname;
	const char *rname;
	uint32_t serial;
	uint32_t refresh;
	uint32_t retry;
	uint32_t expire;
	uint32_t minimum;
} _rdata_soa_t;

static void _rdata_soa_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	ch_msgpack_map_start(pk,"rdata",7);
	_rdata_soa_t *soa = (_rdata_soa_t*)rdata->data_parsed;

	ch_msgpack_write_kv(pk,"mname",soa->mname);
	ch_msgpack_write_kv(pk,"rname",soa->rname);
	ch_msgpack_write_uint32(pk,"serial",soa->serial);
	ch_msgpack_write_uint32(pk,"refresh",soa->refresh);
	ch_msgpack_write_uint32(pk,"retry",soa->retry);
	ch_msgpack_write_uint32(pk,"expire",soa->expire);
	ch_msgpack_write_uint32(pk,"minimum",soa->minimum);

}

static ch_dns_rdata_t * _rdata_soa_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                 uint16_t rdlength, uint32_t plen) {
   
	/* unused */
	rdlength = rdlength;

	_rdata_soa_t *soa = (_rdata_soa_t*)apr_palloc(mp,sizeof(*soa));
	soa->mname = "";
	soa->rname = "";
	soa->serial = 0;
	soa->refresh = 0;
	soa->retry = 0;
	soa->expire = 0;
	soa->minimum = 0;

    soa->mname = (const char*)ch_read_rr_name(mp,packet, &pos, id_pos, plen);

    if (soa->mname == NULL) 
		return _make_dns_rdata(mp,(void*)soa,_rdata_soa_format);

    soa->rname = (const char*)ch_read_rr_name(mp,packet, &pos, id_pos, plen);

    if (soa->rname == NULL) 
		return _make_dns_rdata(mp,(void*)soa,_rdata_soa_format);

    int i;
    for (i = 0; i < 4; i++) {
        soa->serial  <<= 8; soa->serial  |= packet[pos+(i+(4*0))];
        soa->refresh <<= 8; soa->refresh |= packet[pos+(i+(4*1))];
        soa->retry   <<= 8; soa->retry   |= packet[pos+(i+(4*2))];
        soa->expire  <<= 8; soa->expire  |= packet[pos+(i+(4*3))];
        soa->minimum <<= 8; soa->minimum |= packet[pos+(i+(4*4))];
    }
    
	return _make_dns_rdata(mp,(void*)soa,_rdata_soa_format);
    
}

//A standard dns name preceded by a preference number 
typedef struct {

	const char *name;
	uint16_t pref;

} _rdata_mx_t;

static void _rdata_mx_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_mx_t *mx = (_rdata_mx_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",2);
	ch_msgpack_write_kv(pk,"mx_name",mx->name);
	ch_msgpack_write_uint16(pk,"mx_pref",mx->pref);
}

static ch_dns_rdata_t * _rdata_mx_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                uint16_t rdlength, uint32_t plen) {

	/* unused */
	rdlength = rdlength;

	_rdata_mx_t *mx = (_rdata_mx_t*)apr_palloc(mp,sizeof(*mx));
	mx->name = "";
	mx->pref = 0;

    mx->pref = (packet[pos] << 8) + packet[pos+1];

    pos = pos + 2;
    mx->name = (const char*)ch_read_rr_name(mp,packet, &pos, id_pos, plen);

    return _make_dns_rdata(mp,(void*)mx,_rdata_mx_format);
}

/*  These records contain a size field for warning about extra large DNS 
packets, an extended rcode, and an optional set of dynamic fields.
The size and extended rcode are printed, but the dynamic fields are 
simply escaped. Note that the associated format function is non-standard,
as EDNS records modify the basic resourse record protocol (there is no
class field, for instance. RFC 2671
*/

typedef struct {

	uint16_t pload_size;
	const char *data;
	const char *rcode_hex;
} _rdata_opts_t;

static void _rdata_opts_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_opts_t *opts = (_rdata_opts_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",3);
	ch_msgpack_write_uint16(pk,"opt_psize",opts->pload_size);
	ch_msgpack_write_kv(pk,"opt_data",opts->data);
	ch_msgpack_write_kv(pk,"opt_rcode_hex",opts->rcode_hex);
}

ch_dns_rdata_t *  _rdata_opts_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                  uint16_t rdlength, uint32_t plen) {

	/* unused */
	id_pos = id_pos;
	plen = plen;

	_rdata_opts_t *opts = (_rdata_opts_t*)apr_palloc(mp,sizeof(*opts));
	opts->pload_size = 0;
	opts->data = "";
	opts->rcode_hex = "";

    opts->pload_size = (packet[pos] << 8) + packet[pos+1];
    
	const char * base_format = "0x%02x%02x%02x%02x";
    
	opts->data = (const char*)ch_escape_data(mp,packet, pos+6, pos + 6 + rdlength);

    
	opts->rcode_hex = apr_psprintf(mp, base_format, packet[2], packet[3],
                                 packet[4], packet[5]);

	return _make_dns_rdata(mp,(void*)opts,_rdata_opts_format);
}

/*  Service records are used to identify various network services and ports.
The format is: 'priority,weight,port target
The target is a somewhat standard DNS name.*/
typedef struct {

	uint16_t priority;
	uint16_t weight;
	uint16_t port;
	const char *target;

} _rdata_srv_t;

static void _rdata_srv_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_srv_t *srv = (_rdata_srv_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",4);

	ch_msgpack_write_uint16(pk,"srv_priority",srv->priority);
	ch_msgpack_write_uint16(pk,"srv_weight",srv->weight);
	ch_msgpack_write_uint16(pk,"srv_port",srv->port);
	ch_msgpack_write_kv(pk,"srv_target",srv->target);

}

static ch_dns_rdata_t * _rdata_srv_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                 uint16_t rdlength, uint32_t plen) {

	/* unused */
	plen = plen;

	_rdata_srv_t *srv = (_rdata_srv_t*)apr_palloc(mp,sizeof(*srv));
	srv->target = "";

	srv->priority = (packet[pos] << 8) + packet[pos+1];
    srv->weight = (packet[pos+2] << 8) + packet[pos+3];
    srv->port = (packet[pos+4] << 8) + packet[pos+5];
    
	pos = pos + 6;
    // Don't read beyond the end of the rr.
    srv->target  = ch_read_rr_name(mp,packet, &pos, id_pos, pos+rdlength-6);

	return _make_dns_rdata(mp,(void*)srv,_rdata_srv_format);
}

//A standard IPv6 address. No attempt is made to abbreviate the address

static void _rdata_ipv6_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	ch_msgpack_map_start(pk,"rdata",1);
	ch_msgpack_write_kv(pk,"ipv6",(const char*)rdata->data_parsed);
}

static ch_dns_rdata_t * _rdata_ipv6_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                  uint16_t rdlength, uint32_t plen) {

	/* unused */
	id_pos = id_pos;
	plen = plen;

	unsigned char ipv6[8];
    int i;

    if (rdlength != 16) { 
        return _make_dns_rdata(mp,(void*)"0:0:0:0:0:0:0:0",_rdata_ipv6_format); 
    }

    for (i=0; i < 8; i++) 
        ipv6[i] = (packet[pos+i*2] << 8) + packet[pos+i*2+1];

    
	return _make_dns_rdata(mp,
		(void*)apr_psprintf(mp, "%x:%x:%x:%x:%x:%x:%x:%x", ipv6[0], ipv6[1], ipv6[2],
                                               ipv6[3], ipv6[4], ipv6[5],
                                               ipv6[6], ipv6[7]),
			_rdata_ipv6_format
		); 
}

/*format: flags, proto, algorithm, key
All fields except the key are printed as decimal numbers.
The key is given in base64.*/

typedef struct {

	uint16_t flags;
	uint8_t proto;
	uint8_t algorithm;
	const char *key;
} _rdata_dnskey_t;

static void _rdata_dnskey_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_dnskey_t *dnskey = (_rdata_dnskey_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",4);
	ch_msgpack_write_uint16(pk,"dnskey_flags",dnskey->flags);
	ch_msgpack_write_uint8(pk,"dnskey_proto",dnskey->proto);
	ch_msgpack_write_uint8(pk,"dnskey_algorithm",dnskey->algorithm);
	ch_msgpack_write_kv(pk,"dnskey",dnskey->key);

}

static ch_dns_rdata_t* _rdata_dnskey_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                    uint16_t rdlength, uint32_t plen) {

	/* unused */
	plen = plen;
	id_pos = id_pos;

	_rdata_dnskey_t *dnskey = (_rdata_dnskey_t*)apr_palloc(mp,sizeof(*dnskey));

	dnskey->flags = (packet[pos] << 8) + packet[pos+1];
    dnskey->proto = packet[pos+2];
    dnskey->algorithm = packet[pos+3];

    dnskey->key = ch_encode_base64_packet(mp,packet, pos+4, rdlength-4);

    return _make_dns_rdata(mp,(void*)dnskey,_rdata_dnskey_format);
}

/*format: tc,alg,labels,ottl,expiration,inception,tag signer signature
All fields except the signer and signature are given as decimal numbers
The signer is a standard DNS name
The signature is base64 encoded.*/
typedef struct {

	uint16_t tc;
	uint8_t alg;
	uint8_t labels;
	uint32_t ottl;
	uint32_t expiration;
	uint32_t inception;
	uint16_t tag;
	const char *signer;
	const char *signature;
} _rdata_sig_t;

static void _rdata_sig_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_sig_t *sig = (_rdata_sig_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",9);
	ch_msgpack_write_uint16(pk,"sig_tc",sig->tc);
	ch_msgpack_write_uint8(pk,"sig_alg",sig->alg);
	ch_msgpack_write_uint8(pk,"sig_labels",sig->labels);

	ch_msgpack_write_uint32(pk,"sig_ottl",sig->ottl);
	ch_msgpack_write_uint32(pk,"sig_expiration",sig->expiration);
	ch_msgpack_write_uint32(pk,"sig_inception",sig->inception);
	ch_msgpack_write_uint16(pk,"sig_tag",sig->tag);

	ch_msgpack_write_kv(pk,"sig_signer",sig->signer);
	ch_msgpack_write_kv(pk,"sig_signature",sig->signature);

}

static ch_dns_rdata_t * _rdata_rrsig_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                   uint16_t rdlength, uint32_t plen) {

	/* unused */
	plen = plen;

	_rdata_sig_t *sig = (_rdata_sig_t*)apr_palloc(mp,sizeof(*sig));

    uint32_t o_pos = pos;
    sig->tc = (packet[pos] << 8) + packet[pos+1];
    sig->alg = packet[pos+2];
    sig->labels = packet[pos+3];
    sig->tag = (packet[pos+16] << 8) + packet[pos+17];
    
	pos = pos + 4;
    sig->ottl = (packet[pos] << 24) + (packet[pos+1] << 16) + 
           (packet[pos+2] << 8) + packet[pos+3];
    
	pos = pos + 4;
    sig->expiration = (packet[pos] << 24) + (packet[pos+1] << 16) + 
              (packet[pos+2] << 8) + packet[pos+3];
    
	pos = pos + 4; 
    sig->inception = (packet[pos] << 24) + (packet[pos+1] << 16) + 
              (packet[pos+2] << 8) + packet[pos+3];

    pos = pos + 6;
    sig->signer = ch_read_rr_name(mp,packet, &pos, id_pos, o_pos+rdlength);
        
    sig->signature = ch_encode_base64_packet(mp,packet, pos, o_pos+rdlength-pos);

	return _make_dns_rdata(mp,(void*)sig,_rdata_sig_format);
}

/*Format: domain bitmap 
domain is a DNS name, bitmap is hex escaped.*/
typedef struct {

	const char *domain;
	const char *bitmap;
} _rdata_nsec_t;

static void _rdata_nsec_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_nsec_t *nsec = (_rdata_nsec_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",2);

	ch_msgpack_write_kv(pk,"domain",nsec->domain);
	ch_msgpack_write_kv(pk,"bitmap",nsec->bitmap);

}

static ch_dns_rdata_t * _rdata_nsec_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t id_pos,
                  uint16_t rdlength, uint32_t plen) {

	/* unused */
	plen = plen;

	_rdata_nsec_t *nsec = (_rdata_nsec_t*)apr_palloc(mp,sizeof(*nsec));

    nsec->domain = ch_read_rr_name(mp,packet, &pos, id_pos, pos+rdlength);

    nsec->bitmap = ch_escape_data(mp,packet, pos, pos+rdlength);

	return _make_dns_rdata(mp,(void*)nsec,_rdata_nsec_format);
}

/*format: key_tag,algorithm,digest_type,digest
The keytag, algorithm, and digest type are given as base 10.
The digest is base64 encoded.*/
typedef struct {

	uint16_t tag;
	uint8_t alg;
	uint8_t dig_type;
	const char *digest;
} _rdata_ds_t;

static void _rdata_ds_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	_rdata_ds_t *ds = (_rdata_ds_t*)rdata->data_parsed;

	ch_msgpack_map_start(pk,"rdata",4);
	ch_msgpack_write_uint16(pk,"ds_tag",ds->tag);
	ch_msgpack_write_uint8(pk,"ds_alg",ds->alg);
	ch_msgpack_write_uint8(pk,"ds_type",ds->dig_type);
	ch_msgpack_write_kv(pk,"ds_digest",ds->digest);
}

static ch_dns_rdata_t * _rdata_ds_parse(apr_pool_t *mp,
	const unsigned char * packet, uint32_t pos, uint32_t id_pos,uint16_t rdlength, uint32_t plen) 
{
	/* unused */
	id_pos = id_pos;
	plen = plen;

    _rdata_ds_t *ds = (_rdata_ds_t*)apr_palloc(mp,sizeof(*ds));

	ds->tag = (packet[pos] << 8) + packet[pos+1];
    ds->alg = packet[pos+2];
    ds->dig_type = packet[pos+3];
    ds->digest = ch_encode_base64_packet(mp,packet,pos+4,rdlength-4);

	return _make_dns_rdata(mp,(void*)ds,_rdata_ds_format);
}

//Non printable characters are given as a hex value (\\x30), for example.

static void _rdata_any_format(msgpack_packer *pk,ch_dns_rdata_t *rdata){

	ch_msgpack_map_start(pk,"rdata",1);
	ch_msgpack_write_kv(pk,"any",(const char*)rdata->data_parsed);
}

ch_dns_rdata_t * _rdata_escape_parse(apr_pool_t *mp,const unsigned char * packet, uint32_t pos, uint32_t i,
              uint16_t rdlength, uint32_t plen) {

	/* unused */
	i = i;
	plen = plen;

    return _make_dns_rdata(mp,(void*)ch_escape_data(mp,packet, pos, pos + rdlength),
		
		_rdata_any_format
		);

}

// Add parser functions here
// then defined below.
// Some of the rtypes below use the escape parser.  This isn't
// because we don't know how to parse them, it's simply because that's
// the right parser for them anyway.
ch_dns_rdata_parse_t rr_parsers[] = {{1, 1, _rdata_ipv4_parse,0},
                                    {0, 2,  _rdata_domain_name_parse, 0},
                                    {0, 5,  _rdata_domain_name_parse, 0},
                                    {0, 6,  _rdata_soa_parse, 0},
                                    {0, 12, _rdata_domain_name_parse, 0},
                                    {1, 33, _rdata_srv_parse, 0}, 
                                    {1, 28, _rdata_ipv6_parse, 0},
                                    {0, 15, _rdata_mx_parse, 0},
                                    {0, 46, _rdata_rrsig_parse, 0},
                                    {0, 16, _rdata_escape_parse,  0}, 
                                    {0, 47, _rdata_nsec_parse, 0},
                                    {0, 43, _rdata_ds_parse,  0},
                                    {0, 10, _rdata_escape_parse, 0}, 
                                    {0, 48, _rdata_dnskey_parse, 0},
                                    {0, 255,_rdata_escape_parse,0}
                                   };

static inline size_t count_parsers(void) {
    return sizeof(rr_parsers)/sizeof(ch_dns_rdata_parse_t);
}

// We occasionally sort the parsers according to how they're used in order
// to speed up lookups.
static void _sort_parsers(void) {
    int m,n;
    int change = 1;
    int pcount = (int)count_parsers();

    ch_dns_rdata_parse_t tmp;

    for (m = 0; m < pcount - 1 && change == 1; m++) {
        change = 0;
        for (n = 0; n < pcount - 1; n++) {
            if (rr_parsers[n].count < rr_parsers[n+1].count) {
                tmp = rr_parsers[n];
                rr_parsers[n] = rr_parsers[n+1];
                rr_parsers[n+1] = tmp;
                change = 1;
            }
        }
    }
    // Reset the counts
    for (m = 0; m < pcount - 1; m++) {
        rr_parsers[m].count = 0;
    }
}

unsigned int PACKETS_SEEN = 0;
#define REORDER_LIMIT 100000
// Find the parser that corresponds to the given cls and rtype.
ch_dns_rdata_parse_t* ch_dns_rdata_parser_find(uint16_t cls, uint16_t rtype) {

    unsigned int i=0, pcount = count_parsers();
    ch_dns_rdata_parse_t * found = NULL;
   
    // Re-arrange the order of the parsers according to how often things are 
    // seen every REORDER_LIMIT packets.
    if (PACKETS_SEEN > REORDER_LIMIT) {
        PACKETS_SEEN = 0;
        _sort_parsers();
    } 
    PACKETS_SEEN++;

    while (i < pcount && found == NULL) {
        ch_dns_rdata_parse_t pc = rr_parsers[i];
        if ((pc.rtype == rtype || pc.rtype == 0) &&
            (pc.cls == cls || pc.cls == 0)) {
            rr_parsers[i].count++;
            found = &rr_parsers[i];
            break;
        }
        i++;
    }

    if (found == NULL) 
        found = &default_rr_parser;
    
    found->count++;
    return found;
}



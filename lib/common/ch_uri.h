
/*
 *
 *      Filename: ch_uri.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-10 18:34:50
 * Last Modified: 2018-04-10 18:34:50
 */

#ifndef CH_URI_H
#define CH_URI_H

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdint.h>
#include "ch_mpool.h"
#include "ch_string.h"

#define CH_URI_FTP_DEFAULT_PORT         21 /**< default FTP port */
#define CH_URI_SSH_DEFAULT_PORT         22 /**< default SSH port */
#define CH_URI_TELNET_DEFAULT_PORT      23 /**< default telnet port */
#define CH_URI_GOPHER_DEFAULT_PORT      70 /**< default Gopher port */
#define CH_URI_HTTP_DEFAULT_PORT        80 /**< default HTTP port */
#define CH_URI_POP_DEFAULT_PORT        110 /**< default POP port */
#define CH_URI_NNTP_DEFAULT_PORT       119 /**< default NNTP port */
#define CH_URI_IMAP_DEFAULT_PORT       143 /**< default IMAP port */
#define CH_URI_PROSPERO_DEFAULT_PORT   191 /**< default Prospero port */
#define CH_URI_WAIS_DEFAULT_PORT       210 /**< default WAIS port */
#define CH_URI_LDAP_DEFAULT_PORT       389 /**< default LDAP port */
#define CH_URI_HTTPS_DEFAULT_PORT      443 /**< default HTTPS port */
#define CH_URI_RTSP_DEFAULT_PORT       554 /**< default RTSP port */
#define CH_URI_SNEWS_DEFAULT_PORT      563 /**< default SNEWS port */
#define CH_URI_ACAP_DEFAULT_PORT       674 /**< default ACAP port */
#define CH_URI_NFS_DEFAULT_PORT       2049 /**< default NFS port */
#define CH_URI_TIP_DEFAULT_PORT       3372 /**< default TIP port */
#define CH_URI_SIP_DEFAULT_PORT       5060 /**< default SIP port */

/** Flags passed to unparse_uri_components(): */
/** suppress "scheme://user\@site:port" */
#define CH_URI_UNP_OMITSITEPART    (1U<<0)
/** Just omit user */
#define CH_URI_UNP_OMITUSER        (1U<<1)
/** Just omit password */
#define CH_URI_UNP_OMITPASSWORD    (1U<<2)
/** omit "user:password\@" part */
#define CH_URI_UNP_OMITUSERINFO    (CH_URI_UNP_OMITUSER | \
                                     CH_URI_UNP_OMITPASSWORD)
/** Show plain text password (default: show XXXXXXXX) */
#define CH_URI_UNP_REVEALPASSWORD  (1U<<3)
/** Show "scheme://user\@site:port" only */
#define CH_URI_UNP_OMITPATHINFO    (1U<<4)
/** Omit the "?queryarg" from the path */
#define CH_URI_UNP_OMITQUERY       (1U<<5)

/** @see ch_uri_t */
typedef struct ch_uri_t ch_uri_t;

/**
 * A structure to encompass all of the fields in a uri
 */
struct ch_uri_t {
    /** scheme ("http"/"ftp"/...) */
    char *scheme;
    /** combined [user[:password]\@]host[:port] */
    char *hostinfo;
    /** user name, as in http://user:passwd\@host:port/ */
    char *user;
    /** password, as in http://user:passwd\@host:port/ */
    char *password;
    /** hostname from URI (or from Host: header) */
    char *hostname;
    /** port string (integer representation is in "port") */
    char *port_str;
    /** the request path (or NULL if only scheme://host was given) */
    char *path;
    
    char *ext_name;

    /** Everything after a '?' in the path, if present */
    char *query;
    /** Trailing "#fragment" string, if present */
    char *fragment;

    /** The port number, numeric, valid only if port_str != NULL */
    uint16_t port;
    
};


/**
 * Parse a given URI, fill in all supplied fields of a ch_uri_t
 * structure. This eliminates the necessity of extracting host, port,
 * path, query info repeatedly in the modules.
 * @param p The pool to allocate out of
 * @param uri The uri to parse
 * @param uptr The ch_uri_t to fill out
 * @return 0 for success or error code -1
 */
extern int ch_uri_parse(ch_pool_t *p, const char *uri, 
                                        ch_uri_t *uptr);


#endif /*CH_URI_H*/

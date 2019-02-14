#include "ch_uri.h"

typedef struct schemes_t schemes_t;

/** Structure to store various schemes and their default ports */
struct schemes_t {
    /** The name of the scheme */
    const char *name;
    /** The default port for the scheme */
    uint16_t default_port;
};

/* Some WWW schemes and their default ports; this is basically /etc/services */
/* This will become global when the protocol abstraction comes */
/* As the schemes are searched by a linear search, */
/* they are sorted by their expected frequency */
static schemes_t schemes[] =
{
    {"http",     CH_URI_HTTP_DEFAULT_PORT},
    {"ftp",      CH_URI_FTP_DEFAULT_PORT},
    {"https",    CH_URI_HTTPS_DEFAULT_PORT},
    {"gopher",   CH_URI_GOPHER_DEFAULT_PORT},
    {"ldap",     CH_URI_LDAP_DEFAULT_PORT},
    {"nntp",     CH_URI_NNTP_DEFAULT_PORT},
    {"snews",    CH_URI_SNEWS_DEFAULT_PORT},
    {"imap",     CH_URI_IMAP_DEFAULT_PORT},
    {"pop",      CH_URI_POP_DEFAULT_PORT},
    {"sip",      CH_URI_SIP_DEFAULT_PORT},
    {"rtsp",     CH_URI_RTSP_DEFAULT_PORT},
    {"wais",     CH_URI_WAIS_DEFAULT_PORT},
    {"z39.50r",  CH_URI_WAIS_DEFAULT_PORT},
    {"z39.50s",  CH_URI_WAIS_DEFAULT_PORT},
    {"prospero", CH_URI_PROSPERO_DEFAULT_PORT},
    {"nfs",      CH_URI_NFS_DEFAULT_PORT},
    {"tip",      CH_URI_TIP_DEFAULT_PORT},
    {"acap",     CH_URI_ACAP_DEFAULT_PORT},
    {"telnet",   CH_URI_TELNET_DEFAULT_PORT},
    {"ssh",      CH_URI_SSH_DEFAULT_PORT},
    { NULL, 0xFFFF }     /* unknown port */
};

/* Here is the hand-optimized parse_uri_components().  There are some wild
 * tricks we could pull in assembly language that we don't pull here... like we
 * can do word-at-time scans for delimiter characters using the same technique
 * that fast memchr()s use.  But that would be way non-portable. -djg
 */

/* We have a ch_table_t that we can index by character and it tells us if the
 * character is one of the interesting delimiters.  Note that we even get
 * compares for NUL for free -- it's just another delimiter.
 */

#define T_SLASH           0x01        /* '/' */
#define T_QUESTION        0x02        /* '?' */
#define T_HASH            0x04        /* '#' */
#define T_ALPHA           0x08        /* 'A' ... 'Z', 'a' ... 'z' */
#define T_SCHEME          0x10        /* '0' ... '9', '-', '+', '.'
                                       * (allowed in scheme except first char)
                                       */
#define T_NUL             0x80        /* '\0' */

/* Delimiter table for the ASCII character set */
static const unsigned char uri_delims[256] = {
    T_NUL,                      /* 0x00     */
    0,                          /* 0x01     */
    0,                          /* 0x02     */
    0,                          /* 0x03     */
    0,                          /* 0x04     */
    0,                          /* 0x05     */
    0,                          /* 0x06     */
    0,                          /* 0x07     */
    0,                          /* 0x08     */
    0,                          /* 0x09     */
    0,                          /* 0x0a     */
    0,                          /* 0x0b     */
    0,                          /* 0x0c     */
    0,                          /* 0x0d     */
    0,                          /* 0x0e     */
    0,                          /* 0x0f     */
    0,                          /* 0x10     */
    0,                          /* 0x11     */
    0,                          /* 0x12     */
    0,                          /* 0x13     */
    0,                          /* 0x14     */
    0,                          /* 0x15     */
    0,                          /* 0x16     */
    0,                          /* 0x17     */
    0,                          /* 0x18     */
    0,                          /* 0x19     */
    0,                          /* 0x1a     */
    0,                          /* 0x1b     */
    0,                          /* 0x1c     */
    0,                          /* 0x1d     */
    0,                          /* 0x1e     */
    0,                          /* 0x1f     */
    0,                          /* 0x20 ' ' */
    0,                          /* 0x21 '!' */
    0,                          /* 0x22 '"' */
    T_HASH,                     /* 0x23 '#' */
    0,                          /* 0x24 '$' */
    0,                          /* 0x25 '%' */
    0,                          /* 0x26 '&' */
    0,                          /* 0x27 ''' */
    0,                          /* 0x28 '(' */
    0,                          /* 0x29 ')' */
    0,                          /* 0x2a '*' */
    T_SCHEME,                   /* 0x2b '+' */
    0,                          /* 0x2c ',' */
    T_SCHEME,                   /* 0x2d '-' */
    T_SCHEME,                   /* 0x2e '.' */
    T_SLASH,                    /* 0x2f '/' */
    T_SCHEME,                   /* 0x30 '0' */
    T_SCHEME,                   /* 0x31 '1' */
    T_SCHEME,                   /* 0x32 '2' */
    T_SCHEME,                   /* 0x33 '3' */
    T_SCHEME,                   /* 0x34 '4' */
    T_SCHEME,                   /* 0x35 '5' */
    T_SCHEME,                   /* 0x36 '6' */
    T_SCHEME,                   /* 0x37 '7' */
    T_SCHEME,                   /* 0x38 '8' */
    T_SCHEME,                   /* 0x39 '9' */
    0,                          /* 0x3a ':' */
    0,                          /* 0x3b ';' */
    0,                          /* 0x3c '<' */
    0,                          /* 0x3d '=' */
    0,                          /* 0x3e '>' */
    T_QUESTION,                 /* 0x3f '?' */
    0,                          /* 0x40 '@' */
    T_ALPHA,                    /* 0x41 'A' */
    T_ALPHA,                    /* 0x42 'B' */
    T_ALPHA,                    /* 0x43 'C' */
    T_ALPHA,                    /* 0x44 'D' */
    T_ALPHA,                    /* 0x45 'E' */
    T_ALPHA,                    /* 0x46 'F' */
    T_ALPHA,                    /* 0x47 'G' */
    T_ALPHA,                    /* 0x48 'H' */
    T_ALPHA,                    /* 0x49 'I' */
    T_ALPHA,                    /* 0x4a 'J' */
    T_ALPHA,                    /* 0x4b 'K' */
    T_ALPHA,                    /* 0x4c 'L' */
    T_ALPHA,                    /* 0x4d 'M' */
    T_ALPHA,                    /* 0x4e 'N' */
    T_ALPHA,                    /* 0x4f 'O' */
    T_ALPHA,                    /* 0x50 'P' */
    T_ALPHA,                    /* 0x51 'Q' */
    T_ALPHA,                    /* 0x52 'R' */
    T_ALPHA,                    /* 0x53 'S' */
    T_ALPHA,                    /* 0x54 'T' */
    T_ALPHA,                    /* 0x55 'U' */
    T_ALPHA,                    /* 0x56 'V' */
    T_ALPHA,                    /* 0x57 'W' */
    T_ALPHA,                    /* 0x58 'X' */
    T_ALPHA,                    /* 0x59 'Y' */
    T_ALPHA,                    /* 0x5a 'Z' */
    0,                          /* 0x5b '[' */
    0,                          /* 0x5c '\' */
    0,                          /* 0x5d ']' */
    0,                          /* 0x5e '^' */
    0,                          /* 0x5f '_' */
    0,                          /* 0x60 '`' */
    T_ALPHA,                    /* 0x61 'a' */
    T_ALPHA,                    /* 0x62 'b' */
    T_ALPHA,                    /* 0x63 'c' */
    T_ALPHA,                    /* 0x64 'd' */
    T_ALPHA,                    /* 0x65 'e' */
    T_ALPHA,                    /* 0x66 'f' */
    T_ALPHA,                    /* 0x67 'g' */
    T_ALPHA,                    /* 0x68 'h' */
    T_ALPHA,                    /* 0x69 'i' */
    T_ALPHA,                    /* 0x6a 'j' */
    T_ALPHA,                    /* 0x6b 'k' */
    T_ALPHA,                    /* 0x6c 'l' */
    T_ALPHA,                    /* 0x6d 'm' */
    T_ALPHA,                    /* 0x6e 'n' */
    T_ALPHA,                    /* 0x6f 'o' */
    T_ALPHA,                    /* 0x70 'p' */
    T_ALPHA,                    /* 0x71 'q' */
    T_ALPHA,                    /* 0x72 'r' */
    T_ALPHA,                    /* 0x73 's' */
    T_ALPHA,                    /* 0x74 't' */
    T_ALPHA,                    /* 0x75 'u' */
    T_ALPHA,                    /* 0x76 'v' */
    T_ALPHA,                    /* 0x77 'w' */
    T_ALPHA,                    /* 0x78 'x' */
    T_ALPHA,                    /* 0x79 'y' */
    T_ALPHA,                    /* 0x7a 'z' */
    0,                          /* 0x7b '{' */
    0,                          /* 0x7c '|' */
    0,                          /* 0x7d '}' */
    0,                          /* 0x7e '~' */
    0,                          /* 0x7f     */
    0,                          /* 0x80     */
    0,                          /* 0x81     */
    0,                          /* 0x82     */
    0,                          /* 0x83     */
    0,                          /* 0x84     */
    0,                          /* 0x85     */
    0,                          /* 0x86     */
    0,                          /* 0x87     */
    0,                          /* 0x88     */
    0,                          /* 0x89     */
    0,                          /* 0x8a     */
    0,                          /* 0x8b     */
    0,                          /* 0x8c     */
    0,                          /* 0x8d     */
    0,                          /* 0x8e     */
    0,                          /* 0x8f     */
    0,                          /* 0x90     */
    0,                          /* 0x91     */
    0,                          /* 0x92     */
    0,                          /* 0x93     */
    0,                          /* 0x94     */
    0,                          /* 0x95     */
    0,                          /* 0x96     */
    0,                          /* 0x97     */
    0,                          /* 0x98     */
    0,                          /* 0x99     */
    0,                          /* 0x9a     */
    0,                          /* 0x9b     */
    0,                          /* 0x9c     */
    0,                          /* 0x9d     */
    0,                          /* 0x9e     */
    0,                          /* 0x9f     */
    0,                          /* 0xa0     */
    0,                          /* 0xa1     */
    0,                          /* 0xa2     */
    0,                          /* 0xa3     */
    0,                          /* 0xa4     */
    0,                          /* 0xa5     */
    0,                          /* 0xa6     */
    0,                          /* 0xa7     */
    0,                          /* 0xa8     */
    0,                          /* 0xa9     */
    0,                          /* 0xaa     */
    0,                          /* 0xab     */
    0,                          /* 0xac     */
    0,                          /* 0xad     */
    0,                          /* 0xae     */
    0,                          /* 0xaf     */
    0,                          /* 0xb0     */
    0,                          /* 0xb1     */
    0,                          /* 0xb2     */
    0,                          /* 0xb3     */
    0,                          /* 0xb4     */
    0,                          /* 0xb5     */
    0,                          /* 0xb6     */
    0,                          /* 0xb7     */
    0,                          /* 0xb8     */
    0,                          /* 0xb9     */
    0,                          /* 0xba     */
    0,                          /* 0xbb     */
    0,                          /* 0xbc     */
    0,                          /* 0xbd     */
    0,                          /* 0xbe     */
    0,                          /* 0xbf     */
    0,                          /* 0xc0     */
    0,                          /* 0xc1     */
    0,                          /* 0xc2     */
    0,                          /* 0xc3     */
    0,                          /* 0xc4     */
    0,                          /* 0xc5     */
    0,                          /* 0xc6     */
    0,                          /* 0xc7     */
    0,                          /* 0xc8     */
    0,                          /* 0xc9     */
    0,                          /* 0xca     */
    0,                          /* 0xcb     */
    0,                          /* 0xcc     */
    0,                          /* 0xcd     */
    0,                          /* 0xce     */
    0,                          /* 0xcf     */
    0,                          /* 0xd0     */
    0,                          /* 0xd1     */
    0,                          /* 0xd2     */
    0,                          /* 0xd3     */
    0,                          /* 0xd4     */
    0,                          /* 0xd5     */
    0,                          /* 0xd6     */
    0,                          /* 0xd7     */
    0,                          /* 0xd8     */
    0,                          /* 0xd9     */
    0,                          /* 0xda     */
    0,                          /* 0xdb     */
    0,                          /* 0xdc     */
    0,                          /* 0xdd     */
    0,                          /* 0xde     */
    0,                          /* 0xdf     */
    0,                          /* 0xe0     */
    0,                          /* 0xe1     */
    0,                          /* 0xe2     */
    0,                          /* 0xe3     */
    0,                          /* 0xe4     */
    0,                          /* 0xe5     */
    0,                          /* 0xe6     */
    0,                          /* 0xe7     */
    0,                          /* 0xe8     */
    0,                          /* 0xe9     */
    0,                          /* 0xea     */
    0,                          /* 0xeb     */
    0,                          /* 0xec     */
    0,                          /* 0xed     */
    0,                          /* 0xee     */
    0,                          /* 0xef     */
    0,                          /* 0xf0     */
    0,                          /* 0xf1     */
    0,                          /* 0xf2     */
    0,                          /* 0xf3     */
    0,                          /* 0xf4     */
    0,                          /* 0xf5     */
    0,                          /* 0xf6     */
    0,                          /* 0xf7     */
    0,                          /* 0xf8     */
    0,                          /* 0xf9     */
    0,                          /* 0xfa     */
    0,                          /* 0xfb     */
    0,                          /* 0xfc     */
    0,                          /* 0xfd     */
    0,                          /* 0xfe     */
    0                           /* 0xff     */
};


/* it works like this:
    if (uri_delims[ch] & NOTEND_foobar) {
        then we're not at a delimiter for foobar
    }
*/

#define NOTEND_HOSTINFO   (T_SLASH | T_QUESTION | T_HASH | T_NUL)
#define NOTEND_PATH       (T_QUESTION | T_HASH | T_NUL)

uint16_t ch_uri_port_of_scheme(const char *scheme_str)
{
    schemes_t *scheme;

    if (scheme_str) {
        for (scheme = schemes; scheme->name != NULL; ++scheme) {
            if (strcasecmp(scheme_str, scheme->name) == 0) {
                return scheme->default_port;
            }
        }
    }
    return 0;
}

/* parse_uri_components():
 * Parse a given URI, fill in all supplied fields of a uri_components
 * structure. This eliminates the necessity of extracting host, port,
 * path, query info repeatedly in the modules.
 * Side effects:
 *  - fills in fields of uri_components *uptr
 *  - none on any of the r->* fields
 */
int ch_uri_parse(ch_pool_t *p, const char *uri, 
                                        ch_uri_t *uptr)
{
    const char *s;
    const char *s1;
    const char *hostinfo;
    char *endstr;
    int port;
    int v6_offset1 = 0, v6_offset2 = 0;

    /* Initialize the structure. parse_uri() and parse_uri_components()
     * can be called more than once per request.
     */
    memset (uptr, '\0', sizeof(*uptr));

    /* We assume the processor has a branch predictor like most --
     * it assumes forward branches are untaken and backwards are taken.  That's
     * the reason for the gotos.  -djg
     */
    if (uri[0] == '/') {
        /* RFC2396 #4.3 says that two leading slashes mean we have an
         * authority component, not a path!  Fixing this looks scary
         * with the gotos here.  But if the existing logic is valid,
         * then presumably a goto pointing to deal_with_authority works.
         *
         * RFC2396 describes this as resolving an ambiguity.  In the
         * case of three or more slashes there would seem to be no
         * ambiguity, so it is a path after all.
         */
        if (uri[1] == '/' && uri[2] != '/') {
            s = uri + 2 ;
            goto deal_with_authority ;
        }

deal_with_path:
        /* we expect uri to point to first character of path ... remember
         * that the path could be empty -- http://foobar?query for example
         */
        s = uri;
        while ((uri_delims[*(unsigned char *)s] & NOTEND_PATH) == 0) {
            ++s;
        }
        if (s != uri) {
            uptr->path = ch_pstrmemdup(p, uri, s - uri);
            
            char *pp = strchr(uptr->path,'.');
            if(pp){
                uptr->ext_name = pp+1;
            }
        }
        if (*s == 0) {
            return 0;
        }
        if (*s == '?') {
            ++s;
            s1 = strchr(s, '#');
            if (s1) {
                uptr->fragment = ch_pstrdup(p, s1 + 1);
                uptr->query = ch_pstrmemdup(p, s, s1 - s);
            }
            else {
                uptr->query = ch_pstrdup(p, s);
            }
            return 0;
        }
        /* otherwise it's a fragment */
        uptr->fragment = ch_pstrdup(p, s + 1);
        return 0;
    }

    /* find the scheme: */
    s = uri;
    /* first char must be letter */
    if (uri_delims[*(unsigned char *)s] & T_ALPHA) {
        ++s;
        while ((uri_delims[*(unsigned char *)s] & (T_ALPHA|T_SCHEME)))
            ++s;
    }
    /* scheme must be non-empty and followed by : */
    if (s != uri && s[0] == ':') {
        uptr->scheme = ch_pstrmemdup(p, uri, s - uri);
        s++;
    }
    else {
        /* No valid scheme, restart from the beginning */
        s = uri;
    }

    if (s[0] != '/' || s[1] != '/') {
        if (uri == s) {
            /*
             * RFC 3986 3.3: If we have no scheme and no authority,
             * the leading segment of a relative path must not contain a ':'.
             */
            char *first_slash = strchr(uri, '/');
            if (first_slash) {
                while (s < first_slash) {
                    if (s[0] == ':')
                        return -1;
                    ++s;
                }
                /* no scheme but relative path, e.g. '../image.jpg' */
            }
            else {
                if (strchr(uri, ':') != NULL)
                    return -1;
                /* no scheme, no slash, but relative path, e.g. 'image.jpg' */
            }
            goto deal_with_path;
        }
        /* scheme and relative path */
        uri = s;
        goto deal_with_path;
    }

    s += 2;

deal_with_authority:
    hostinfo = s;
    while ((uri_delims[*(unsigned char *)s] & NOTEND_HOSTINFO) == 0) {
        ++s;
    }
    uri = s;        /* whatever follows hostinfo is start of uri */
    uptr->hostinfo = ch_pstrmemdup(p, hostinfo, uri - hostinfo);

    /* If there's a username:password@host:port, the @ we want is the last @...
     * too bad there's no memrchr()... For the C purists, note that hostinfo
     * is definitely not the first character of the original uri so therefore
     * &hostinfo[-1] < &hostinfo[0] ... and this loop is valid C.
     */
    do {
        --s;
    } while (s >= hostinfo && *s != '@');
    if (s < hostinfo) {
        /* again we want the common case to be fall through */
deal_with_host:
        /* We expect hostinfo to point to the first character of
         * the hostname.  If there's a port it is the first colon,
         * except with IPv6.
         */
        if (*hostinfo == '[') {
            v6_offset1 = 1;
            v6_offset2 = 2;
            s = memchr(hostinfo, ']', uri - hostinfo);
            if (s == NULL) {
                return -1;
            }
            if (*++s != ':') {
                s = NULL; /* no port */
            }
        }
        else {
            s = memchr(hostinfo, ':', uri - hostinfo);
        }
        if (s == NULL) {
            /* we expect the common case to have no port */
            uptr->hostname = ch_pstrmemdup(p,
                                            hostinfo + v6_offset1,
                                            uri - hostinfo - v6_offset2);
            goto deal_with_path;
        }
        uptr->hostname = ch_pstrmemdup(p,
                                        hostinfo + v6_offset1,
                                        s - hostinfo - v6_offset2);
        ++s;
        uptr->port_str = ch_pstrmemdup(p, s, uri - s);
        if (uri != s) {
            port = strtol(uptr->port_str, &endstr, 10);
            uptr->port = port;
            if (*endstr == '\0') {
                goto deal_with_path;
            }
            /* Invalid characters after ':' found */
            return -1;
        }
        uptr->port = ch_uri_port_of_scheme(uptr->scheme);
        goto deal_with_path;
    }

    /* first colon delimits username:password */
    s1 = memchr(hostinfo, ':', s - hostinfo);
    if (s1) {
        uptr->user = ch_pstrmemdup(p, hostinfo, s1 - hostinfo);
        ++s1;
        uptr->password = ch_pstrmemdup(p, s1, s - s1);
    }
    else {
        uptr->user = ch_pstrmemdup(p, hostinfo, s - hostinfo);
    }
    hostinfo = s + 1;
    goto deal_with_host;
}

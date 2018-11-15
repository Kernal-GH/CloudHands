/*
 *
 *      Filename: ch_util.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:45:13
 * Last Modified: 2018-06-13 19:26:54
 */

#include "ch_util.h"
#include "ch_errno.h"
#include "ch_string.h"
#include "ch_time.h"

/* Base64 tables used in decodeBase64Ext */
static const char b64_pad = '=';

static const short b64_reverse_t[256] = {
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
  -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
  -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};



/** \brief Decode utf-8 to unicode format.
 *
 * \param mp Pointer to memory pool
 * \param input Pointer to input data
 * \param input_len Input data length
 * \param changed Set if data is changed
 *
 * \retval rval On Success
 */
char *ch_utf8_unicode_inplace_ex(ch_pool_t *mp, unsigned char *input, long int input_len, int *changed) {
    int unicode_len = 0;
    unsigned int length = 0;
    unsigned int d = 0, count = 0;
    unsigned char c, *utf;
    char *rval, *data;
    unsigned int i, len, j;
    unsigned int bytes_left = input_len;
    unsigned char *unicode = NULL;

    *changed = 0;

    len = input_len * 7 + 1;
    data = rval = ch_palloc(mp, len);
    if (rval == NULL) return NULL;


    if (input == NULL) return NULL;

    for(i = 0; i < bytes_left;)  {
        unicode_len = 0; d = 0;
        utf = (unsigned char *)&input[i];

        c = *utf;

        /* If first byte begins with binary 0 it is single byte encoding */
        if ((c & 0x80) == 0) {
            /* single byte unicode (7 bit ASCII equivilent) has no validation */
            count++;
            if(count <= len)    {
                if(c == 0)
                    *data = ch_x2c(&c);
                else
                    *data++ = c;
            }

        }
        /* If first byte begins with binary 110 it is two byte encoding*/
        else if ((c & 0xE0) == 0xC0) {
            /* check we have at least two bytes */
            if (bytes_left < 2) unicode_len = UNICODE_ERROR_CHARACTERS_MISSING;
            /* check second byte starts with binary 10 */
            else if (((*(utf + 1)) & 0xC0) != 0x80) unicode_len = UNICODE_ERROR_INVALID_ENCODING;
            else {
                unicode_len = 2;
                count+=6;
                if(count <= len) {
                    /* compute character number */
                    d = ((c & 0x1F) << 6) | (*(utf + 1) & 0x3F);
                    *data++ = '%';
                    *data++ = 'u';
                    unicode = (unsigned char*)ch_psprintf(mp, "%x", d);
                    length = strlen((const char*)unicode);

                    switch(length)  {
                        case 1:
                            *data++ = '0';
                            *data++ = '0';
                            *data++ = '0';
                            break;
                        case 2:
                            *data++ = '0';
                            *data++ = '0';
                            break;
                        case 3:
                            *data++ = '0';
                            break;
                        case 4:
                        case 5:
                            break;

                        default:
                            break;
                    }

                    for(j=0; j<length; j++) {
                        *data++ = unicode[j];
                    }

                    *changed = 1;
                }
            }
        }
        /* If first byte begins with binary 1110 it is three byte encoding */
        else if ((c & 0xF0) == 0xE0) {
            /* check we have at least three bytes */
            if (bytes_left < 3) unicode_len = UNICODE_ERROR_CHARACTERS_MISSING;
            /* check second byte starts with binary 10 */
            else if (((*(utf + 1)) & 0xC0) != 0x80) unicode_len = UNICODE_ERROR_INVALID_ENCODING;
            /* check third byte starts with binary 10 */
            else if (((*(utf + 2)) & 0xC0) != 0x80) unicode_len = UNICODE_ERROR_INVALID_ENCODING;
            else {
                unicode_len = 3;
                count+=6;
                if(count <= len) {
                    /* compute character number */
                    d = ((c & 0x0F) << 12) | ((*(utf + 1) & 0x3F) << 6) | (*(utf + 2) & 0x3F);
                    *data++ = '%';
                    *data++ = 'u';
                    unicode = (unsigned char*)ch_psprintf(mp, "%x", d);
                    length = strlen((const char*)unicode);

                    switch(length)  {
                        case 1:
                            *data++ = '0';
                            *data++ = '0';
                            *data++ = '0';
                            break;
                        case 2:
                            *data++ = '0';
                            *data++ = '0';
                            break;
                        case 3:
                            *data++ = '0';
                            break;
                        case 4:
                        case 5:
                            break;
                        default:
                            break;
                    }

                    for(j=0; j<length; j++) {
                        *data++ = unicode[j];
                    }

                    *changed = 1;

                }
            }
        }
        /* If first byte begins with binary 11110 it is four byte encoding */
        else if ((c & 0xF8) == 0xF0) {
            /* restrict characters to UTF-8 range (U+0000 - U+10FFFF)*/
            if (c >= 0xF5) {
                *data++ = c;
            }
            /* check we have at least four bytes */
            if (bytes_left < 4) unicode_len = UNICODE_ERROR_CHARACTERS_MISSING;
            /* check second byte starts with binary 10 */
            else if (((*(utf + 1)) & 0xC0) != 0x80) unicode_len = UNICODE_ERROR_INVALID_ENCODING;
            /* check third byte starts with binary 10 */
            else if (((*(utf + 2)) & 0xC0) != 0x80) unicode_len = UNICODE_ERROR_INVALID_ENCODING;
            /* check forth byte starts with binary 10 */
            else if (((*(utf + 3)) & 0xC0) != 0x80) unicode_len = UNICODE_ERROR_INVALID_ENCODING;
            else {
                unicode_len = 4;
                count+=7;
                if(count <= len) {
                    /* compute character number */
                    d = ((c & 0x07) << 18) | ((*(utf + 1) & 0x3F) << 12) | ((*(utf + 2) & 0x3F) << 6) | (*(utf + 3) & 0x3F);
                    *data++ = '%';
                    *data++ = 'u';
                    unicode = (unsigned char*)ch_psprintf(mp, "%x", d);
                    length = strlen((const char*)unicode);

                    switch(length)  {
                        case 1:
                            *data++ = '0';
                            *data++ = '0';
                            *data++ = '0';
                            break;
                        case 2:
                            *data++ = '0';
                            *data++ = '0';
                            break;
                        case 3:
                            *data++ = '0';
                            break;
                        case 4:
                        case 5:
                            break;
                        default:
                            break;
                    }

                    for(j=0; j<length; j++) {
                        *data++ = unicode[j];
                    }

                    *changed = 1;

                }
            }
        }
        /* any other first byte is invalid (RFC 3629) */
        else {
            count++;
            if(count <= len)
                *data++ = c;
        }

        /* invalid UTF-8 character number range (RFC 3629) */
        if ((d >= 0xD800) && (d <= 0xDFFF)) {
            count++;
            if(count <= len)
                *data++ = c;
        }

        /* check for overlong */
        if ((unicode_len == 4) && (d < 0x010000)) {
            /* four byte could be represented with less bytes */
            count++;
            if(count <= len)
                *data++ = c;
        }
        else if ((unicode_len == 3) && (d < 0x0800)) {
            /* three byte could be represented with less bytes */
            count++;
            if(count <= len)
                *data++ = c;
        }
        else if ((unicode_len == 2) && (d < 0x80)) {
            /* two byte could be represented with less bytes */
            count++;
            if(count <= len)
                *data++ = c;
        }

        if(unicode_len > 0) {
            i += unicode_len;
        } else {
            i++;
        }
    }

    *data ='\0';

    return rval;
}

/** \brief Validate IPv4 Netmask
 *
 * \param ip_strv6 Pointer to ipv6 address
 *
 * \retval netmask_v4 On Success
 */
unsigned char ch_is_netmask_v4(char *ip_strv4) {
    unsigned char netmask_v4 = 32;
    char *mask_str = NULL;
    int cidr;

    if(ip_strv4 == NULL)
        return netmask_v4;

    if ((mask_str = strchr(ip_strv4, '/'))) {
        *(mask_str++) = '\0';

        if (strchr(mask_str, '.') != NULL) {
            return 0;
        }

        cidr = atoi(mask_str);
        if ((cidr < 0) || (cidr > 32)) {
            return 0;
        }

        netmask_v4 = (unsigned char)cidr;
    }

    return netmask_v4;
}

/** \brief Validate IPv6 Netmask
 *
 * \param ip_strv6 Pointer to ipv6 address
 *
 * \retval netmask_v6 On Success
 */
unsigned char ch_is_netmask_v6(char *ip_strv6) {
    unsigned char netmask_v6 = 128;
    char *mask_str = NULL;
    int cidr;

    if(ip_strv6 == NULL)
        return netmask_v6;

    if ((mask_str = strchr(ip_strv6, '/'))) {
        *(mask_str++) = '\0';

        if (strchr(mask_str, ':') != NULL) {
            return 0;
        }

        cidr = atoi(mask_str);
        if ((cidr < 0) || (cidr > 128)) {
            return 0;
        }
        netmask_v6 = (unsigned char)cidr;
    }

    return netmask_v6;
}



/** \brief Remove quotes
 *
 * \param mptmp Pointer to the pool
 * \param input Pointer to input string
 * \param input_len Input data length
 *
 * \retval string On Success
 */
char *ch_remove_quotes(ch_pool_t *mptmp, const char *input, int input_len)  {
    char *parm = ch_palloc(mptmp, input_len);
    char *ret = parm;
    int len = input_len;

    for(; *input !='\0' && len >=0; input++, len--)    {
        if(*input != '\'' && *input != '\"')    {
            *parm++ = *input;
        }
    }

    *parm = '\0';
    return ret;
}

/** \brief Remove escape char
 *
 * \param mptmp Pointer to the pool
 * \param input Pointer to input string
 * \param input_len Input data length
 *
 * \retval string On Success
 */
char *ch_remove_escape(ch_pool_t *mptmp, const char *input, int input_len)  {
    char *parm = ch_palloc(mptmp, input_len);
    char *ret = parm;
    int len = input_len;

    for(; *input !='\0' && len >=0; input++, len--)    {
        if(*input != '\\')    {
            *parm++ = *input;
        }
    }

    *parm = '\0';
    return ret;
}

/**
 *
 */
int ch_parse_boolean(const char *input) {
    if (input == NULL) return -1;
    if (strcasecmp(input, "on") == 0) return 1;
    if (strcasecmp(input, "true") == 0) return 1;
    if (strcasecmp(input, "1") == 0) return 1;
    if (strcasecmp(input, "off") == 0) return 0;
    if (strcasecmp(input, "false") == 0) return 0;
    if (strcasecmp(input, "0") == 0) return 0;

    return -1;
}

/** \brief Decode Base64 data with special chars
 *
 * \param plain_text Pointer to plain text data
 * \param input Pointer to input data
 * \param input_len Input data length
 *
 * \retval 0 On failure
 * \retval string length On Success
 */
int ch_decode_base64_ext(char *plain_text, const unsigned char *input, int input_len)   {
    const unsigned char *encoded = input;
    int i = 0, j = 0, k = 0;
    int ch = 0;

    while ((ch = *encoded++) != '\0' && input_len-- > 0) {
        if (ch == b64_pad) {
            if (*encoded != '=' && (i % 4) == 1) {
                return 0;
            }
            continue;
        }

        ch = b64_reverse_t[ch];
        if (ch < 0 || ch == -1) {
            continue;
        } else if (ch == -2) {
            return 0;
        }
        switch(i % 4) {
            case 0:
                plain_text[j] = ch << 2;
                break;
            case 1:
                plain_text[j++] |= ch >> 4;
                plain_text[j] = (ch & 0x0f) << 4;
                break;
            case 2:
                plain_text[j++] |= ch >>2;
                plain_text[j] = (ch & 0x03) << 6;
                break;
            case 3:
                plain_text[j++] |= ch;
                break;
            default:
                break;
        }
        i++;
    }

    k = j;
    if (ch == b64_pad) {
        switch(i % 4) {
            case 1:
                return 0;
            case 2:
                k++;
            case 3:
                plain_text[k] = 0;
            default:
                break;
        }
    }

    plain_text[j] = '\0';

    return j;
}

/** \brief Convert const char to int
 *
 * \param c number string
 *
 * \retval n The converted number
 */
int ch_convert_to_int(const char c)
{
    int n;
    if ((c>='0') && (c<='9'))
        n = c - '0';
    else if ((c>='A') && (c<='F'))
        n = c - 'A' + 10;
    else if ((c>='a') && (c<='f'))
        n = c - 'a' + 10;
    else
        n = 0;
    return n;
}


/**
 * Parses a string that contains a name-value pair in the form "name=value".
 * IMP1 It does not check for whitespace between tokens.
 */
int ch_parse_name_eq_value(ch_pool_t *mp, const char *input, char **name, char **value) {
    char *p = NULL;

    if ((name == NULL)||(value == NULL)) return -1;
    if (input == NULL) return 0;

    *name = NULL;
    *value = NULL;
    p = (char *)input;

    while((*p != '=')&&(*p != '\0')) p++;
    if (*p == '\0') {
        *name = (char *)input;
        return 1;
    }

    *name = ch_pstrmemdup(mp, input, p - input);
    if (*name == NULL) return -1;
    p++;

    *value = ch_pstrdup(mp, p);
    if (*value == NULL) return -1;

    return 1;
}

/**
 *
 * IMP1 Assumes NUL-terminated
 */
char *ch_url_encode(ch_pool_t *mp, char *input, unsigned int input_len, int *changed) {
    char *rval, *d;
    unsigned int i, len;

    *changed = 0;

    len = input_len * 3 + 1;
    d = rval = ch_palloc(mp, len);
    if (rval == NULL) return NULL;

    /* ENH Only encode the characters that really need to be encoded. */

    for(i = 0; i < input_len; i++) {
        unsigned char c = input[i];

        if (c == ' ') {
            *d++ = '+';
            *changed = 1;
        } else
            if ( (c == 42) || ((c >= 48)&&(c <= 57)) || ((c >= 65)&&(c <= 90))
                    || ((c >= 97)&&(c <= 122))
               ) {
                *d++ = c;
            } else {
                *d++ = '%';
                ch_c2x(c, (unsigned char *)d);
                d += 2;
                *changed = 1;
            }
    }

    *d = '\0';

    return rval;
}



/**
 *
 */
char *ch_file_basename(ch_pool_t *mp, const char *filename) {
    char *d, *p;

    if (filename == NULL) return NULL;
    d = ch_pstrdup(mp, filename);
    if (d == NULL) return NULL;

    p = strrchr(d, '/');
    if (p != NULL) d = p + 1;
    p = strrchr(d, '\\');
    if (p != NULL) d = p + 1;

    return d;
}


char *ch_file_dirname(ch_pool_t *p, const char *filename) {
    char *b, *c;

    if (filename == NULL) return NULL;
    b = ch_pstrdup(p, filename);
    if (b == NULL) return NULL;

    c = strrchr(b, '/');
    if (c != NULL) *c = '\0';

    return b;
}


/**
 *
 */
int ch_sql_hex2bytes_inplace(unsigned char *data, int len) {
    unsigned char *d, *begin = data;

    if ((data == NULL)||(len == 0)) return 0;

    for( d = data; *data; *d++ = *data++) {
        if ( *data != '0' ) continue;
        if ( tolower(*++data) != 'x' ) {
            data--;
            continue;
        }

        data++;

        // Do we need to keep "0x" if no hexa after?
        if ( !VALID_HEX(data[0]) || !VALID_HEX(data[1]) ) {
            data-=2;
            continue;
        }

        while ( VALID_HEX(data[0]) && VALID_HEX(data[1]) )  {
            *d++ = ch_x2c(data);
            data += 2;
        }
    }

    *d = '\0';
    return strlen((char *)begin);
}

/**
 *
 *
 */
int ch_hex2bytes_inplace(unsigned char *data, int len) {
    unsigned char *d = data;
    int i, count = 0;

    if ((data == NULL)||(len == 0)) return 0;

    for(i = 0; i <= len - 2; i += 2) {
        *d++ = ch_x2c(&data[i]);
        count++;
    }
    *d = '\0';

    return count;
}

/**
 * Converts a series of bytes into its hexadecimal
 * representation.
 */
char *ch_bytes2hex(ch_pool_t *pool, unsigned char *data, int len) {
    static const unsigned char b2hex[] = "0123456789abcdef";
    char *hex = NULL;
    int i, j;

    hex = ch_palloc(pool, (len * 2) + 1);
    if (hex == NULL) return NULL;

    j = 0;
    for(i = 0; i < len; i++) {
        hex[j++] = b2hex[data[i] >> 4];
        hex[j++] = b2hex[data[i] & 0x0f];
    }
    hex[j] = 0;

    return hex;
}

/**
 * Converts a series of bytes into its hexadecimal
 * representation.
 */
char *ch_bytes2hex_malloc(unsigned char *data, int len) {
    static const unsigned char b2hex[] = "0123456789abcdef";
    char *hex = NULL;
    int i, j;

    hex = malloc((len * 2) + 1);
    if (hex == NULL) return NULL;

    j = 0;
    for(i = 0; i < len; i++) {
        hex[j++] = b2hex[data[i] >> 4];
        hex[j++] = b2hex[data[i] & 0x0f];
    }

    hex[j] = 0;

    return hex;
}

/**
 *
 */
int ch_is_token_char(unsigned char c) {
    /* ENH Is the performance important at all? We could use a table instead. */

    /* CTLs not allowed */
    if ((c <= 32)||(c >= 127)) return 0;

    switch(c) {
        case '(' :
        case ')' :
        case '<' :
        case '>' :
        case '@' :
        case ',' :
        case ';' :
        case ':' :
        case '\\' :
        case '"' :
        case '/' :
        case '[' :
        case ']' :
        case '?' :
        case '=' :
            return 0;
        default:
            break;
    }

    return 1;
}

/**
 *
 */
int ch_remove_lf_crlf_inplace(char *text) {
    char *p = text;
    int count = 0;

    if (text == NULL) return -1;

    while(*p != '\0') {
        count++;
        p++;
    }

    if (count > 0) {
        if (*(p - 1) == '\n') {
            *(p - 1) = '\0';
            if (count > 1) {
                if (*(p - 2) == '\r') {
                    *(p - 2) = '\0';
                }
            }
        }
    }

    return 1;
}

/**
 *
 */
char *ch_guess_tmp_dir(ch_pool_t *p) {
    
	/* unused */
	p = p;

	char *filename = NULL;

    /* ENH Use ch_temp_dir_get instead. */

    filename = getenv("TMPDIR");
    if (filename != NULL) return filename;

    filename = getenv("TEMP");
    if (filename != NULL) return filename;

    filename = getenv("TMP");
    if (filename != NULL) return filename;

    return("/tmp/");
}

/**
 *
 */
char *ch_current_logtime(ch_pool_t *mp) {
    ch_time_exp_t t;
    char tstr[100];
    size_t len;

    ch_time_exp_lt(&t, ch_time_now());

    ch_strftime(tstr, &len, 80, "%d/%b/%Y:%H:%M:%S ", &t);
    ch_snprintf(tstr + strlen(tstr), 80 - strlen(tstr), "%c%.2d%.2d",
            t.tm_gmtoff < 0 ? '-' : '+',
            t.tm_gmtoff / (60 * 60), (t.tm_gmtoff / 60) % 60);
    return ch_pstrdup(mp, tstr);
}

char  *ch_current_logtime_with_buf(char *buf,size_t buf_size){

    ch_time_exp_t t;
    size_t len;

    if(buf_size<100)
        return "";

    ch_time_exp_lt(&t, ch_time_now());

    ch_strftime(buf, &len, 80, "%d/%b/%Y:%H:%M:%S ", &t);

    ch_snprintf(buf + strlen(buf), 80 - strlen(buf), "%c%.2d%.2d",
            t.tm_gmtoff < 0 ? '-' : '+',
            t.tm_gmtoff / (60 * 60), (t.tm_gmtoff / 60) % 60);

    return buf;
}

/**
 *
 */
char *ch_current_filetime(ch_pool_t *mp) {
    ch_time_exp_t t;
    char tstr[100];
    size_t len;

    ch_time_exp_lt(&t, ch_time_now());

    ch_strftime(tstr, &len, 80, "%Y%m%d-%H%M%S", &t);
    return ch_pstrdup(mp, tstr);
}

/**
 *
 */
int ch_mkstemp_ex(char *templat, int mode) {
    int fd = -1;

    /* ENH Use ch_file_mktemp instead. */

    fd = mkstemp(templat);
    if ((fd != -1) && (mode != 0)) {
        if (fchmod(fd, mode) == -1) {
            return -1;
        }
    }

    return fd;
}

/**
 *
 */
int ch_mkstemp(char *templat) {
    return ch_mkstemp_ex(templat, CREATEMODE_UNISTD);
}

/**
 * Converts the input string to lowercase (in-place).
 */
char *ch_strtolower_inplace(unsigned char *str) {
    unsigned char *c = str;

    if (str == NULL) return NULL;

    while(*c != 0) {
        *c = tolower(*c);
        c++;
    }

    return (char *)str;
}

static char *_log_escape(ch_pool_t *p, const unsigned char *input,
        unsigned long int input_length, int escape_quotes, int escape_colon, int escape_re);

char *ch_log_escape_re(ch_pool_t *mp, const char *text) {
    return _log_escape(mp, (const unsigned char *)text, text ? strlen(text) : 0, 1, 1, 1);
}

char *ch_log_escape(ch_pool_t *mp, const char *text) {
    return _log_escape(mp, (const unsigned char *)text, text ? strlen(text) : 0, 1, 0, 0);
}

char *ch_log_escape_nq(ch_pool_t *mp, const char *text) {
    return _log_escape(mp, (const unsigned char *)text, text ? strlen(text) : 0, 0, 0, 0);
}

char *ch_log_escape_ex(ch_pool_t *mp, const char *text, unsigned long int text_length) {
    return _log_escape(mp, (const unsigned char *)text, text_length, 1, 0, 0);
}

char *ch_log_escape_nq_ex(ch_pool_t *mp, const char *text, unsigned long int text_length) {
    return _log_escape(mp, (const unsigned char *)text, text_length, 0, 0, 0);
}

char *ch_log_escape_raw(ch_pool_t *mp, const unsigned char *text, unsigned long int text_length) {
    unsigned char *ret = ch_palloc(mp, text_length * 4 + 1);
    unsigned long int i, j;

    for (i = 0, j = 0; i < text_length; i++, j += 4) {
        ret[j] = '\\';
        ret[j+1] = 'x';
        ch_c2x(text[i], ret+j+2);
    }
    ret[text_length * 4] = '\0';

    return (char *)ret;
}

char *ch_log_escape_nul(ch_pool_t *mp, const unsigned char *text, unsigned long int text_length) {
    unsigned char *ret = ch_palloc(mp, text_length * 4 + 1);
    unsigned long int i, j;

    for (i = 0, j = 0; i < text_length; i++) {
        if (text[i] == '\0') {
            ret[j] = '\\';
            ret[j+1] = 'x';
            ch_c2x(text[i], ret+j+2);
            j += 4;
        }
        else {
            ret[j] = text[i];
            j++;
        }
    }
    ret[j] = '\0';

    return (char *)ret;
}

/**
 * Transform text to ASCII printable or hex escaped
 */
char *ch_log_escape_hex(ch_pool_t *mp, const unsigned char *text, unsigned long int text_length) {
    unsigned char *ret = ch_palloc(mp, text_length * 4 + 1);
    unsigned long int i, j;

    for (i = 0, j = 0; i < text_length; i++) {
        if (  (text[i] == '"')
                ||(text[i] == '\\')
                ||(text[i] <= 0x1f)
                ||(text[i] >= 0x7f))
        {
            ret[j] = '\\';
            ret[j+1] = 'x';
            ch_c2x(text[i], ret+j+2);
            j += 4;
        }
        else {
            ret[j] = text[i];
            j ++;
        }
    }
    ret[j] = '\0';

    return (char *)ret;
}

/**
 * Transform input into a form safe for logging.
 */
static char *_log_escape(ch_pool_t *mp, const unsigned char *input, unsigned long int input_len,
        int escape_quotes, int escape_colon, int escape_re)
{
    unsigned char *d = NULL;
    char *ret = NULL;
    unsigned long int i;

    if (input == NULL) return NULL;

    ret = ch_palloc(mp, input_len * 4 + 1);
    if (ret == NULL) return NULL;
    d = (unsigned char *)ret;

    i = 0;
    while(i < input_len) {
        switch(input[i]) {
            case ':' :
                if (escape_colon) {
                    *d++ = '\\';
                    *d++ = ':';
                } else {
                    *d++ = input[i];
                }
                break;
            case '"' :
                if (escape_quotes) {
                    *d++ = '\\';
                    *d++ = '"';
                } else {
                    *d++ = input[i];
                }
                break;
            case '+' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = '+';
                } else {
                    *d++ = input[i];
                }
                break;
            case '.' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = '.';
                } else {
                    *d++ = input[i];
                }
                break;
            case ']' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = ']';
                } else {
                    *d++ = input[i];
                }
                break;
            case '[' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = '[';
                } else {
                    *d++ = input[i];
                }
                break;
            case '(' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = '(';
                } else {
                    *d++ = input[i];
                }
                break;
            case ')' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = ')';
                } else {
                    *d++ = input[i];
                }
                break;
            case '?' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = '?';
                } else {
                    *d++ = input[i];
                }
                break;
            case '/' :
                if (escape_re) {
                    *d++ = '\\';
                    *d++ = '/';
                } else {
                    *d++ = input[i];
                }
                break;
            case '\b' :
                *d++ = '\\';
                *d++ = 'b';
                break;
            case '\n' :
                *d++ = '\\';
                *d++ = 'n';
                break;
            case '\r' :
                *d++ = '\\';
                *d++ = 'r';
                break;
            case '\t' :
                *d++ = '\\';
                *d++ = 't';
                break;
            case '\v' :
                *d++ = '\\';
                *d++ = 'v';
                break;
            case '\\' :
                *d++ = '\\';
                *d++ = '\\';
                break;
            default :
                if ((input[i] <= 0x1f)||(input[i] >= 0x7f)) {
                    *d++ = '\\';
                    *d++ = 'x';
                    ch_c2x(input[i], d);
                    d += 2;
                } else {
                    *d++ = input[i];
                }
                break;
        }

        i++;
    }

    *d = 0;

    return ret;
}

/**
 * JavaScript decoding.
 * IMP1 Assumes NUL-terminated
 */

int ch_js_decode_nonstrict_inplace(unsigned char *input, long int input_len) {
    unsigned char *d = (unsigned char *)input;
    long int i, count;

    if (input == NULL) return -1;

    i = count = 0;
    while (i < input_len) {
        if (input[i] == '\\') {
            /* Character is an escape. */

            if (   (i + 5 < input_len) && (input[i + 1] == 'u')
                    && (VALID_HEX(input[i + 2])) && (VALID_HEX(input[i + 3]))
                    && (VALID_HEX(input[i + 4])) && (VALID_HEX(input[i + 5])) )
            {
                /* \uHHHH */

                /* Use only the lower byte. */
                *d = ch_x2c(&input[i + 4]);

                /* Full width ASCII (ff01 - ff5e) needs 0x20 added */
                if (   (*d > 0x00) && (*d < 0x5f)
                        && ((input[i + 2] == 'f') || (input[i + 2] == 'F'))
                        && ((input[i + 3] == 'f') || (input[i + 3] == 'F')))
                {
                    (*d) += 0x20;
                }

                d++;
                count++;
                i += 6;
            }
            else if (   (i + 3 < input_len) && (input[i + 1] == 'x')
                    && VALID_HEX(input[i + 2]) && VALID_HEX(input[i + 3])) {
                /* \xHH */
                *d++ = ch_x2c(&input[i + 2]);
                count++;
                i += 4;
            }
            else if ((i + 1 < input_len) && ISODIGIT(input[i + 1])) {
                /* \OOO (only one byte, \000 - \377) */
                char buf[4];
                int j = 0;

                while((i + 1 + j < input_len)&&(j < 3)) {
                    buf[j] = input[i + 1 + j];
                    j++;
                    if (!ISODIGIT(input[i + 1 + j])) break;
                }
                buf[j] = '\0';

                if (j > 0) {
                    /* Do not use 3 characters if we will be > 1 byte */
                    if ((j == 3) && (buf[0] > '3')) {
                        j = 2;
                        buf[j] = '\0';
                    }
                    *d++ = (unsigned char)strtol(buf, NULL, 8);
                    i += 1 + j;
                    count++;
                }
            }
            else if (i + 1 < input_len) {
                /* \C */
                unsigned char c = input[i + 1];
                switch(input[i + 1]) {
                    case 'a' :
                        c = '\a';
                        break;
                    case 'b' :
                        c = '\b';
                        break;
                    case 'f' :
                        c = '\f';
                        break;
                    case 'n' :
                        c = '\n';
                        break;
                    case 'r' :
                        c = '\r';
                        break;
                    case 't' :
                        c = '\t';
                        break;
                    case 'v' :
                        c = '\v';
                        break;
                        /* The remaining (\?,\\,\',\") are just a removal
                         * of the escape char which is default.
                         */
                    default:
                        break;
                }

                *d++ = c;
                i += 2;
                count++;
            }
            else {
                /* Not enough bytes */
                while(i < input_len) {
                    *d++ = input[i++];
                    count++;
                }
            }
        }
        else {
            *d++ = input[i++];
            count++;
        }
    }

    *d = '\0';

    return count;
}


/**
 *
 * IMP1 Assumes NUL-terminated
 */
int ch_urldecode_nonstrict_inplace_ex(unsigned char *input, long int input_len, int *invalid_count, int *changed) {
    unsigned char *d = (unsigned char *)input;
    long int i, count;

    *changed = 0;

    if (input == NULL) return -1;

    i = count = 0;
    while (i < input_len) {
        if (input[i] == '%') {
            /* Character is a percent sign. */

            /* Are there enough bytes available? */
            if (i + 2 < input_len) {
                char c1 = input[i + 1];
                char c2 = input[i + 2];

                if (VALID_HEX(c1) && VALID_HEX(c2)) {
                    /* Valid encoding - decode it. */
                    *d++ = ch_x2c(&input[i + 1]);
                    count++;
                    i += 3;
                    *changed = 1;
                } else {
                    /* Not a valid encoding, skip this % */
                    *d++ = input[i++];
                    count ++;
                    (*invalid_count)++;
                }
            } else {
                /* Not enough bytes available, copy the raw bytes. */
                *d++ = input[i++];
                count ++;
                (*invalid_count)++;
            }
        } else {
            /* Character is not a percent sign. */
            if (input[i] == '+') {
                *d++ = ' ';
                *changed = 1;
            } else {
                *d++ = input[i];
            }
            count++;
            i++;
        }
    }

    *d = '\0';

    return count;
}

/**
 *
 * IMP1 Assumes NUL-terminated
 */
int ch_html_entities_decode_inplace(ch_pool_t *mp, unsigned char *input, int input_len) {
    unsigned char *d = input;
    int i, count;

    if ((input == NULL)||(input_len <= 0)) return 0;

    i = count = 0;
    while((i < input_len)&&(count < input_len)) {
        int z, copy = 1;

        /* Require an ampersand and at least one character to
         * start looking into the entity.
         */
        if ((input[i] == '&')&&(i + 1 < input_len)) {
            int k, j = i + 1;

            if (input[j] == '#') {
                /* Numerical entity. */
                copy++;

                if (!(j + 1 < input_len)) goto HTML_ENT_OUT; /* Not enough bytes. */
                j++;

                if ((input[j] == 'x')||(input[j] == 'X')) {
                    /* Hexadecimal entity. */
                    copy++;

                    if (!(j + 1 < input_len)) goto HTML_ENT_OUT; /* Not enough bytes. */
                    j++; /* j is the position of the first digit now. */

                    k = j;
                    while((j < input_len)&&(isxdigit(input[j]))) j++;
                    if (j > k) { /* Do we have at least one digit? */
                        /* Decode the entity. */
                        char *x = ch_pstrmemdup(mp, (const char *)&input[k], j - k);
                        *d++ = (unsigned char)strtol(x, NULL, 16);
                        count++;

                        /* Skip over the semicolon if it's there. */
                        if ((j < input_len)&&(input[j] == ';')) i = j + 1;
                        else i = j;

                        continue;
                    } else {
                        goto HTML_ENT_OUT;
                    }
                } else {
                    /* Decimal entity. */
                    k = j;
                    while((j < input_len)&&(isdigit(input[j]))) j++;
                    if (j > k) { /* Do we have at least one digit? */
                        /* Decode the entity. */
                        char *x = ch_pstrmemdup(mp, (const char *)&input[k], j - k);
                        *d++ = (unsigned char)strtol(x, NULL, 10);
                        count++;

                        /* Skip over the semicolon if it's there. */
                        if ((j < input_len)&&(input[j] == ';')) i = j + 1;
                        else i = j;

                        continue;
                    } else {
                        goto HTML_ENT_OUT;
                    }
                }
            } else {
                /* Text entity. */

                k = j;
                while((j < input_len)&&(isalnum(input[j]))) j++;
                if (j > k) { /* Do we have at least one digit? */
                    char *x = ch_pstrmemdup(mp, (const char *)&input[k], j - k);

                    /* Decode the entity. */
                    /* ENH What about others? */
                    if (strcasecmp(x, "quot") == 0) *d++ = '"';
                    else
                        if (strcasecmp(x, "amp") == 0) *d++ = '&';
                        else
                            if (strcasecmp(x, "lt") == 0) *d++ = '<';
                            else
                                if (strcasecmp(x, "gt") == 0) *d++ = '>';
                                else
                                    if (strcasecmp(x, "nbsp") == 0) *d++ = NBSP;
                                    else {
                                        /* We do no want to convert this entity, copy the raw data over. */
                                        copy = j - k + 1;
                                        goto HTML_ENT_OUT;
                                    }

                    count++;

                    /* Skip over the semicolon if it's there. */
                    if ((j < input_len)&&(input[j] == ';')) i = j + 1;
                    else i = j;

                    continue;
                }
            }
        }

HTML_ENT_OUT:

        for(z = 0; ((z < copy) && (count < input_len)); z++) {
            *d++ = input[i++];
            count++;
        }
    }

    *d = '\0';

    return count;
}

/**
 *
 * IMP1 Assumes NUL-terminated
 */
int ch_ansi_c_sequences_decode_inplace(unsigned char *input, int input_len) {
    unsigned char *d = input;
    int i, count;

    i = count = 0;
    while(i < input_len) {
        if ((input[i] == '\\')&&(i + 1 < input_len)) {
            int c = -1;

            switch(input[i + 1]) {
                case 'a' :
                    c = '\a';
                    break;
                case 'b' :
                    c = '\b';
                    break;
                case 'f' :
                    c = '\f';
                    break;
                case 'n' :
                    c = '\n';
                    break;
                case 'r' :
                    c = '\r';
                    break;
                case 't' :
                    c = '\t';
                    break;
                case 'v' :
                    c = '\v';
                    break;
                case '\\' :
                    c = '\\';
                    break;
                case '?' :
                    c = '?';
                    break;
                case '\'' :
                    c = '\'';
                    break;
                case '"' :
                    c = '"';
                    break;

                default:
                    break;
            }

            if (c != -1) i += 2;

            /* Hexadecimal or octal? */
            if (c == -1) {
                if ((input[i + 1] == 'x')||(input[i + 1] == 'X')) {
                    /* Hexadecimal. */
                    if ((i + 3 < input_len)&&(isxdigit(input[i + 2]))&&(isxdigit(input[i + 3]))) {
                        /* Two digits. */
                        c = ch_x2c(&input[i + 2]);
                        i += 4;
                    } else {
                        /* Invalid encoding, do nothing. */
                    }
                }
                else
                    if (ISODIGIT(input[i + 1])) { /* Octal. */
                        char buf[4];
                        int j = 0;

                        while((i + 1 + j < input_len)&&(j < 3)) {
                            buf[j] = input[i + 1 + j];
                            j++;
                            if (!ISODIGIT(input[i + 1 + j])) break;
                        }
                        buf[j] = '\0';

                        if (j > 0) {
                            c = strtol(buf, NULL, 8);
                            i += 1 + j;
                        }
                    }
            }

            if (c == -1) {
                /* Didn't recognise encoding, copy raw bytes. */
                *d++ = input[i + 1];
                count++;
                i += 2;
            } else {
                /* Converted the encoding. */
                *d++ = c;
                count++;
            }
        } else {
            /* Input character not a backslash, copy it. */
            *d++ = input[i++];
            count++;
        }
    }

    *d = '\0';

    return count;
}

/**
 *
 * IMP1 Assumes NUL-terminated
 */
int ch_normalize_path_inplace(unsigned char *input, int input_len, int win, int *changed) {
    unsigned char *src;
    unsigned char *dst;
    unsigned char *end;
    int ldst = 0;
    int hitroot = 0;
    int done = 0;
    int relative;
    int trailing;

    *changed = 0;

    /* Need at least one byte to normalize */
    if (input_len <= 0) return 0;

    /*
     * ENH: Deal with UNC and drive letters?
     */

    src = dst = input;
    end = input + (input_len - 1);
    ldst = 1;

    relative = ((*input == '/') || (win && (*input == '\\'))) ? 0 : 1;
    trailing = ((*end == '/') || (win && (*end == '\\'))) ? 1 : 0;


    while (!done && (src <= end) && (dst <= end)) {
        /* Convert backslash to forward slash on Windows only. */
        if (win) {
            if (*src == '\\') {
                *src = '/';
                *changed = 1;
            }
            if ((src < end) && (*(src + 1) == '\\')) {
                *(src + 1) = '/';
                *changed = 1;
            }
        }

        /* Always normalize at the end of the input. */
        if (src == end) {
            done = 1;
        }

        /* Skip normalization if this is NOT the end of the path segment. */
        else if (*(src + 1) != '/') {
            goto copy; /* Skip normalization. */
        }

        /*** Normalize the path segment. ***/

        /* Could it be an empty path segment? */
        if ((src != end) && *src == '/') {
            /* Ignore */
            *changed = 1;
            goto copy; /* Copy will take care of this. */
        }

        /* Could it be a back or self reference? */
        else if (*src == '.') {

            /* Back-reference? */
            if ((dst > input) && (*(dst - 1) == '.')) {
                /* If a relative path and either our normalization has
                 * already hit the rootdir, or this is a backref with no
                 * previous path segment, then mark that the rootdir was hit
                 * and just copy the backref as no normilization is possible.
                 */
                if (relative && (hitroot || ((dst - 2) <= input))) {
                    hitroot = 1;

                    goto copy; /* Skip normalization. */
                }

                /* Remove backreference and the previous path segment. */
                dst -= 3;
                while ((dst > input) && (*dst != '/')) {
                    dst--;
                }

                /* But do not allow going above rootdir. */
                if (dst <= input) {
                    hitroot = 1;
                    dst = input;

                    /* Need to leave the root slash if this
                     * is not a relative path and the end was reached
                     * on a backreference.
                     */
                    if (!relative && (src == end)) {
                        dst++;
                    }
                }

                if (done) goto length; /* Skip the copy. */
                src++;

                *changed = 1;
            }

            /* Relative Self-reference? */
            else if (dst == input) {
                *changed = 1;

                /* Ignore. */

                if (done) goto length; /* Skip the copy. */
                src++;
            }

            /* Self-reference? */
            else if (*(dst - 1) == '/') {
                *changed = 1;

                /* Ignore. */

                if (done) goto length; /* Skip the copy. */
                dst--;
                src++;
            }
        }

        /* Found a regular path segment. */
        else if (dst > input) {
            hitroot = 0;
        }

copy:
        /*** Copy the byte if required. ***/

        /* Skip to the last forward slash when multiple are used. */
        if (*src == '/') {
            unsigned char *oldsrc = src;

            while (   (src < end)
                    && ((*(src + 1) == '/') || (win && (*(src + 1) == '\\'))) )
            {
                src++;
            }
            if (oldsrc != src) *changed = 1;

            /* Do not copy the forward slash to the root
             * if it is not a relative path.  Instead
             * move over the slash to the next segment.
             */
            if (relative && (dst == input)) {
                src++;
                goto length; /* Skip the copy */
            }
        }

        *(dst++) = *(src++);

length:
        ldst = (dst - input);
    }

    /* Make sure that there is not a trailing slash in the
     * normalized form if there was not one in the original form.
     */
    if (!trailing && (dst > input) && *(dst - 1) == '/') {
        ldst--;
        dst--;
    }

    /* Always NUL terminate */
    *dst = '\0';

    return ldst;
}



char *ch_resolve_relative_path(ch_pool_t *pool, const char *parent_filename, const char *filename) {
    if (filename == NULL) return NULL;
    // TODO Support paths on operating systems other than Unix.
    if (filename[0] == '/') return (char *)filename;

    return ch_pstrcat(pool, ch_pstrndup(pool, parent_filename,
                strlen(parent_filename) - strlen(ch_filepath_name_get(parent_filename))),
            filename, NULL);
}

/**
 * Decode a string that contains CSS-escaped characters.
 * 
 * References:
 *     http://www.w3.org/TR/REC-CSS2/syndata.html#q4
 *     http://www.unicode.org/roadmaps/
 */
int ch_css_decode_inplace(unsigned char *input, long int input_len) {
    unsigned char *d = (unsigned char *)input;
    long int i, j, count;

    if (input == NULL) return -1;

    i = count = 0;
    while (i < input_len) {

        /* Is the character a backslash? */
        if (input[i] == '\\') {

            /* Is there at least one more byte? */
            if (i + 1 < input_len) {
                i++; /* We are not going to need the backslash. */

                /* Check for 1-6 hex characters following the backslash */
                j = 0;
                while (    (j < 6)
                        && (i + j < input_len)
                        && (VALID_HEX(input[i + j])))
                {
                    j++;
                }

                if (j > 0) { /* We have at least one valid hexadecimal character. */
                    int fullcheck = 0;

                    /* For now just use the last two bytes. */
                    switch (j) {
                        /* Number of hex characters */
                        case 1:
                            *d++ = ch_xsingle2c(&input[i]);
                            break;

                        case 2:
                        case 3:
                            /* Use the last two from the end. */
                            *d++ = ch_x2c(&input[i + j - 2]);
                            break;

                        case 4:
                            /* Use the last two from the end, but request
                             * a full width check.
                             */
                            *d = ch_x2c(&input[i + j - 2]);
                            fullcheck = 1;
                            break;

                        case 5:
                            /* Use the last two from the end, but request
                             * a full width check if the number is greater
                             * or equal to 0xFFFF.
                             */
                            *d = ch_x2c(&input[i + j - 2]);

                            /* Do full check if first byte is 0 */
                            if (input[i] == '0') {
                                fullcheck = 1;
                            }
                            else {
                                d++;
                            }
                            break;

                        case 6:
                            /* Use the last two from the end, but request
                             * a full width check if the number is greater
                             * or equal to 0xFFFF.
                             */
                            *d = ch_x2c(&input[i + j - 2]);

                            /* Do full check if first/second bytes are 0 */
                            if (    (input[i] == '0')
                                    && (input[i + 1] == '0')
                               ) {
                                fullcheck = 1;
                            }
                            else {
                                d++;
                            }
                            break;
                        default:
                            break;
                    }

                    /* Full width ASCII (0xff01 - 0xff5e) needs 0x20 added */
                    if (fullcheck) {
                        if (   (*d > 0x00) && (*d < 0x5f)
                                && ((input[i + j - 3] == 'f') ||
                                    (input[i + j - 3] == 'F'))
                                && ((input[i + j - 4] == 'f') ||
                                    (input[i + j - 4] == 'F')))
                        {
                            (*d) += 0x20;
                        }

                        d++;
                    }

                    /* We must ignore a single whitespace after a hex escape */
                    if ((i + j < input_len) && isspace(input[i + j])) {
                        j++;
                    }

                    /* Move over. */
                    count++;
                    i += j;
                }

                /* No hexadecimal digits after backslash */
                else if (input[i] == '\n') {
                    /* A newline character following backslash is ignored. */
                    i++;
                }

                /* The character after backslash is not a hexadecimal digit, nor a newline. */
                else {
                    /* Use one character after backslash as is. */
                    *d++ = input[i++];
                    count++;
                }
            }

            /* No characters after backslash. */
            else {
                /* Do not include backslash in output (continuation to nothing) */
                i++; 
            }
        }

        /* Character is not a backslash. */
        else {
            /* Copy one normal character to output. */
            *d++ = input[i++];
            count++;
        }
    }

    /* Terminate output string. */
    *d = '\0';

    return count;
}

/**
 * @brief Transforms an ch_array_header_t to a text buffer
 *
 * Converts an ch_array_header_t into a plain/text buffer in a Key: Pair
 * format. The generated buffer is not null terminated.
 *
 * If called with `buffer_length` set to 0 or with `buffer` set to NULL,
 * it will _not_ fill any buffer, instead, it will return the length, that
 * will be needed to save the entire content of `arr` into a buffer.
 *
 * @warning return is not NULL-terminated.
 * @note memory management is in the responsibility of the caller.
 *
 * @param arr ch_array_header_t to be iterated.
 * @param buffer pointer to the destination buffer.
 * @param buffer_length length that will fully fill the buffer.
 * @retval -1 Something went wrong in the process. Do not trust in
 *            buffer content.
 * @retval n>0 size of the [needed|] buffer.
 *
 */
int ch_headers_to_buffer(const ch_array_header_t *arr, char *buffer,
        int buffer_length)
{
    int headers_length = 0;
    int write_to_buffer = 0;
    int i = 0;
    const ch_table_entry_t *te = NULL;

    if (buffer != NULL && buffer_length > 0) {
        write_to_buffer = 1;
    }

    te = (ch_table_entry_t *)arr->elts;
    for (i = 0; i < arr->nelts; i++) {
            char *value = te[i].val;
            char *key = te[i].key;
            headers_length = headers_length + strlen(value) + strlen(key) + /* \n: */ 1 +
               /* colum */ 1 + /* space: */ 1 ;

            if (write_to_buffer == 1) {
                if (buffer_length < headers_length) {
                    headers_length = -1;
                    goto not_enough_memory;
                }

                sprintf(buffer, "%s%s: %s\n", buffer, key, value);
            }
    }

    headers_length++; /* Save space for an extra '\n' between the hedaers and the request body */
    if (write_to_buffer) {
        if (buffer_length < headers_length) {
            headers_length = -1;
            goto not_enough_memory;
        }

        buffer[headers_length-1] = '\n';
    }

not_enough_memory:
    return headers_length;
}

int ch_read_line(char *buf, int len, FILE *fp)
{
    char *tmp;

    if (buf == NULL)
    {
        return -1;
    }

    memset(buf, '\0', len*sizeof(char));

    if (fgets(buf, len, fp) == NULL)
    {
        *buf = '\0';
        return 0;
    }
    else
    {
        if ((tmp = strrchr(buf, '\n')) != NULL)
        {
            *tmp = '\0';
        }
    }

    return 1;
}


unsigned long ch_get_current_timems(void)
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int ch_dir_make_r(const char *orig_path)
{
	char *s;
	char c;
	struct stat st;

    char path[1024]={0};

    char *p = strrchr(orig_path,'/');

    if(p==NULL) return 0;

    memcpy(path,orig_path,p-orig_path);

    s = path;

	while (1) {
		c = '\0';

        /* Bypass leading non-'/'s and then subsequent '/'s. */
        while (*s) {
            if (*s == '/') {
                do {
                    ++s;
                } while (*s == '/');
                c = *s; /* Save the current char */
                *s = '\0'; /* and replace it with nul. */
                break;
            }
            ++s;
        }

		if (mkdir(path, 0777) < 0) {
			/* If we failed for any other reason than the directory
			 * already exists, output a diagnostic and return -1. */
			if (errno != EEXIST
			 || ((stat(path, &st) < 0) || !S_ISDIR(st.st_mode))
			) {
				break;
			}
			/* Since the directory exists, don't attempt to change
			 * permissions if it was the full target.  Note that
			 * this is not an error condition. */
			if (!c) {
				return 0;
			}
		}

		if (!c) {
			/* Done. */
			return 0;
		}

		/* Remove any inserted nul from the path (recursive mode). */
		*s = c;
	} /* while (1) */

	return -1;
}

static int
_encode_base64_internal(unsigned char *dst, const unsigned char *s,int len,
    unsigned int padding)
{
    
	static char   basis[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	unsigned char *d = dst;

    while (len > 2) {
        *d++ = basis[(s[0] >> 2) & 0x3f];
        *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
        *d++ = basis[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = basis[s[2] & 0x3f];

        s += 3;
        len -= 3;
    }

    if (len) {
        *d++ = basis[(s[0] >> 2) & 0x3f];

        if (len == 1) {
            *d++ = basis[(s[0] & 3) << 4];
            if (padding) {
                *d++ = '=';
            }

        } else {
            *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = basis[(s[1] & 0x0f) << 2];
        }

        if (padding) {
            *d++ = '=';
        }
    }

    return d - dst;
}


#define BASE64_LEN(len) (((len)/3+1)*4+1)

unsigned char* ch_encode_base64(ch_pool_t *mp,const unsigned char *input,int input_len){

	size_t dst_len = BASE64_LEN(input_len);

	unsigned char *dst = (unsigned char*)ch_pcalloc(mp,dst_len);

	if(dst == NULL)
		return NULL;

	_encode_base64_internal(dst,input,input_len,1);

	return dst;
}

unsigned char* ch_encode_base64_malloc(const unsigned char *input,int input_len){

	size_t dst_len = BASE64_LEN(input_len);

	unsigned char *dst = (unsigned char*)malloc(dst_len);

	if(dst == NULL)
		return NULL;

	_encode_base64_internal(dst,input,input_len,1);

	return dst;
}


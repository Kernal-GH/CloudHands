/*
 *
 *      Filename: ch_string.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-10 14:44:39
 * Last Modified: 2018-05-18 16:16:11
 */

#include "ch_string.h"

/** this is used to cache lengths in ch_pstrcat */
#define MAX_SAVED_LENGTHS  6

char * ch_pstrdup(ch_pool_t *a, const char *s)
{
    char *res;
    size_t len;

    if (s == NULL) {
        return NULL;
    }
    len = strlen(s) + 1;
    res = ch_pcalloc(a,len);
    memcpy(res,s,len-1);

    return res;
}

char * ch_pstrndup(ch_pool_t *a, const char *s, size_t n)
{
    char *res;
    const char *end;

    if (s == NULL) {
        return NULL;
    }
    end = memchr(s, '\0', n);
    if (end != NULL)
        n = end - s;
    res = ch_palloc(a, n + 1);
    memcpy(res, s, n);
    res[n] = '\0';
    return res;
}

char * ch_pstrmemdup(ch_pool_t *a, const char *s, size_t n)
{
    char *res;

    if (s == NULL) {
        return NULL;
    }
    res = ch_palloc(a, n + 1);
    memcpy(res, s, n);
    res[n] = '\0';
    return res;
}

void * ch_pmemdup(ch_pool_t *a, const void *m, size_t n)
{
    void *res;

    if (m == NULL)
	return NULL;
    res = ch_palloc(a, n);
    memcpy(res, m, n);
    return res;
}

char * ch_pstrcat(ch_pool_t *a, ...)
{
    char *cp, *argp, *res;
    size_t saved_lengths[MAX_SAVED_LENGTHS];
    int nargs = 0;

    /* Pass one --- find length of required string */

    size_t len = 0;
    va_list adummy;

    va_start(adummy, a);

    while ((cp = va_arg(adummy, char *)) != NULL) {
        size_t cplen = strlen(cp);
        if (nargs < MAX_SAVED_LENGTHS) {
            saved_lengths[nargs++] = cplen;
        }
        len += cplen;
    }

    va_end(adummy);

    /* Allocate the required string */

    res = (char *) ch_palloc(a, len + 1);
    cp = res;

    /* Pass two --- copy the argument strings into the result space */

    va_start(adummy, a);

    nargs = 0;
    while ((argp = va_arg(adummy, char *)) != NULL) {
        if (nargs < MAX_SAVED_LENGTHS) {
            len = saved_lengths[nargs++];
        }
        else {
            len = strlen(argp);
        }
 
        memcpy(cp, argp, len);
        cp += len;
    }

    va_end(adummy);

    /* Return the result string */

    *cp = '\0';

    return res;
}

char * ch_pstrcatv(ch_pool_t *a, const struct iovec *vec,
                                 size_t nvec, size_t *nbytes)
{
    size_t i;
    size_t len;
    const struct iovec *src;
    char *res;
    char *dst;

    /* Pass one --- find length of required string */
    len = 0;
    src = vec;
    for (i = nvec; i; i--) {
        len += src->iov_len;
        src++;
    }
    if (nbytes) {
        *nbytes = len;
    }

    /* Allocate the required string */
    res = (char *) ch_palloc(a, len + 1);
    
    /* Pass two --- copy the argument strings into the result space */
    src = vec;
    dst = res;
    for (i = nvec; i; i--) {
        memcpy(dst, src->iov_base, src->iov_len);
        dst += src->iov_len;
        src++;
    }

    /* Return the result string */
    *dst = '\0';

    return res;
}

int ch_strtoff(off_t *offset, const char *nptr,
                                      char **endptr, int base)
{
    *offset = strtol(nptr, endptr, base);
    return 0;
}

int64_t ch_strtoi64(const char *nptr, char **endptr, int base)
{
    return strtoll(nptr, endptr, base);
}

int64_t ch_atoi64(const char *buf)
{
    return ch_strtoi64(buf, NULL, 10);
}

char * ch_itoa(ch_pool_t *p, int n)
{
    const int BUFFER_SIZE = sizeof(int) * 3 + 2;
    char *buf = ch_palloc(p, BUFFER_SIZE);
    char *start = buf + BUFFER_SIZE - 1;
    int negative;
    if (n < 0) {
	negative = 1;
	n = -n;
    }
    else {
	negative = 0;
    }
    *start = 0;
    do {
	*--start = '0' + (n % 10);
	n /= 10;
    } while (n);
    if (negative) {
	*--start = '-';
    }
    return start;
}

char * ch_ltoa(ch_pool_t *p, long n)
{
    const int BUFFER_SIZE = sizeof(long) * 3 + 2;
    char *buf = ch_palloc(p, BUFFER_SIZE);
    char *start = buf + BUFFER_SIZE - 1;
    int negative;
    if (n < 0) {
	negative = 1;
	n = -n;
    }
    else {
	negative = 0;
    }
    *start = 0;
    do {
	*--start = (char)('0' + (n % 10));
	n /= 10;
    } while (n);
    if (negative) {
	*--start = '-';
    }
    return start;
}

char * off_t_toa(ch_pool_t *p, off_t n)
{
    const int BUFFER_SIZE = sizeof(off_t) * 3 + 2;
    char *buf = ch_palloc(p, BUFFER_SIZE);
    char *start = buf + BUFFER_SIZE - 1;
    int negative;
    if (n < 0) {
	negative = 1;
	n = -n;
    }
    else {
	negative = 0;
    }
    *start = 0;
    do {
	*--start = '0' + (char)(n % 10);
	n /= 10;
    } while (n);
    if (negative) {
	*--start = '-';
    }
    return start;
}

char * ch_strfsize(off_t size, char *buf)
{
    const char ord[] = "KMGTPE";
    const char *o = ord;
    int remain;

    if (size < 0) {
        return strcpy(buf, "  - ");
    }
    if (size < 973) {
        if (ch_snprintf(buf, 5, "%3d ", (int) size) < 0)
            return strcpy(buf, "****");
        return buf;
    }
    do {
        remain = (int)(size & 1023);
        size >>= 10;
        if (size >= 973) {
            ++o;
            continue;
        }
        if (size < 9 || (size == 9 && remain < 973)) {
            if ((remain = ((remain * 5) + 256) / 512) >= 10)
                ++size, remain = 0;
            if (ch_snprintf(buf, 5, "%d.%d%c", (int) size, remain, *o) < 0)
                return strcpy(buf, "****");
            return buf;
        }
        if (remain >= 512)
            ++size;
        if (ch_snprintf(buf, 5, "%3d%c", (int) size, *o) < 0)
            return strcpy(buf, "****");
        return buf;
    } while (1);
}

static int
ch_decode_base64_internal(ch_str_t *dst, ch_str_t *src, const char *basis)
{
    size_t          len;
    unsigned char         *d, *s;

    for (len = 0; len < src->len; len++) {
        if (src->data[len] == '=') {
            break;
        }

        if (basis[src->data[len]] == 77) {
            return -1;
        }
    }

    if (len % 4 == 1) {
        return -1;
    }

    s = src->data;
    d = dst->data;

    while (len > 3) {
        *d++ = (char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
        *d++ = (char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
        *d++ = (char) (basis[s[2]] << 6 | basis[s[3]]);

        s += 4;
        len -= 4;
    }

    if (len > 1) {
        *d++ = (char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    }

    if (len > 2) {
        *d++ = (char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    }

    dst->len = d - dst->data;

    return 0;
}


int
ch_decode_base64(ch_str_t *dst, ch_str_t *src)
{
    static char   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return ch_decode_base64_internal(dst, src, basis64);
}

int
ch_strncasecmp(char *s1, char *s2, size_t n)
{
    char c1, c2;

    while (n) {
        c1 = (char) *s1++;
        c2 = (char) *s2++;

        c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
        c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;

        if (c1 == c2) {

            if (c1) {
                n--;
                continue;
            }

            return 0;
        }

        return c1 - c2;
    }

    return 0;
}

char *
ch_strlcasestrn(char *s1, char *last, char *s2, size_t n)
{
    char c1, c2;

    c2 = (char) *s2++;
    c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;
    last -= n;

    do {
        do {
            if (s1 >= last) {
                return NULL;
            }

            c1 = (char) *s1++;

            c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;

        } while (c1 != c2);

    } while (ch_strncasecmp(s1, s2, n) != 0);

    return --s1;
}

int ch_string_endsWith(const char *target,const char *match){

    if(match == NULL||target==NULL)
        return 0;

    size_t match_length = strlen(match);
    size_t target_length = strlen(target);

    /* The empty string always matches */
    if (match_length == 0) {
        /* Match. */
        return 1;
    }

    /* This is impossible to match */
    if (match_length > target_length) {
        /* No match. */
        return 0;
    }

    if (memcmp(match, (target + (target_length - match_length)), match_length) == 0) {
        /* Match. */
        return 1;
    }

    /* No match. */
    return 0;

}


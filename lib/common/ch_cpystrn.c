/*
 *
 *      Filename: ch_cpystrn.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 17:30:03
 * Last Modified: 2017-01-09 18:47:40
 */

#include "ch_string.h"

char * ch_cpystrn(char *dst, const char *src, size_t dst_size)
{

    char *d = dst, *end;

    if (dst_size == 0) {
        return (dst);
    }

    if (src) {
        end = dst + dst_size - 1;

        for (; d < end; ++d, ++src) {
            if (!(*d = *src)) {
                return (d);
            }
        }
    }

    *d = '\0';	/* always null terminate */

    return (d);
}


/*
 * This function provides a way to parse a generic argument string
 * into a standard argv[] form of argument list. It respects the 
 * usual "whitespace" and quoteing rules. In the future this could
 * be expanded to include support for the ch_call_exec command line
 * string processing (including converting '+' to ' ' and doing the 
 * url processing. It does not currently support this function.
 *
 *    token_context: Context from which pool allocations will occur.
 *    arg_str:       Input argument string for conversion to argv[].
 *    argv_out:      Output location. This is a pointer to an array
 *                   of pointers to strings (ie. &(char *argv[]).
 *                   This value will be allocated from the contexts
 *                   pool and filled in with copies of the tokens
 *                   found during parsing of the arg_str. 
 */

int ch_tokenize_to_argv(const char *arg_str, 
                                            char ***argv_out,
                                            ch_pool_t *token_context)
{
    const char *cp;
    const char *ct;
    char *cleaned, *dirty;
    int escaped;
    int isquoted, numargs = 0, argnum;

#define SKIP_WHITESPACE(cp) \
    for ( ; *cp == ' ' || *cp == '\t'; ) { \
        cp++; \
    };

#define CHECK_QUOTATION(cp,isquoted) \
    isquoted = 0; \
    if (*cp == '"') { \
        isquoted = 1; \
        cp++; \
    } \
    else if (*cp == '\'') { \
        isquoted = 2; \
        cp++; \
    }

/* DETERMINE_NEXTSTRING:
 * At exit, cp will point to one of the following:  NULL, SPACE, TAB or QUOTE.
 * NULL implies the argument string has been fully traversed.
 */
#define DETERMINE_NEXTSTRING(cp,isquoted) \
    for ( ; *cp != '\0'; cp++) { \
        if (   (*cp == '\\' && (*(cp+1) == ' ' || *(cp+1) == '\t' || \
                                *(cp+1) == '"' || *(cp+1) == '\''))) { \
            cp++; \
            continue; \
        } \
        if (   (!isquoted && (*cp == ' ' || *cp == '\t')) \
            || (isquoted == 1 && *cp == '"') \
            || (isquoted == 2 && *cp == '\'')                 ) { \
            break; \
        } \
    }
 
/* REMOVE_ESCAPE_CHARS:
 * Compresses the arg string to remove all of the '\' escape chars.
 * The final argv strings should not have any extra escape chars in it.
 */
#define REMOVE_ESCAPE_CHARS(cleaned, dirty, escaped) \
    escaped = 0; \
    while(*dirty) { \
        if (!escaped && *dirty == '\\') { \
            escaped = 1; \
        } \
        else { \
            escaped = 0; \
            *cleaned++ = *dirty; \
        } \
        ++dirty; \
    } \
    *cleaned = 0;        /* last line of macro... */

    cp = arg_str;
    SKIP_WHITESPACE(cp);
    ct = cp;

    /* This is ugly and expensive, but if anyone wants to figure a
     * way to support any number of args without counting and 
     * allocating, please go ahead and change the code.
     *
     * Must account for the trailing NULL arg.
     */
    numargs = 1;
    while (*ct != '\0') {
        CHECK_QUOTATION(ct, isquoted);
        DETERMINE_NEXTSTRING(ct, isquoted);
        if (*ct != '\0') {
            ct++;
        }
        numargs++;
        SKIP_WHITESPACE(ct);
    }
    *argv_out = ch_palloc(token_context, numargs * sizeof(char*));

    /*  determine first argument */
    for (argnum = 0; argnum < (numargs-1); argnum++) {
        SKIP_WHITESPACE(cp);
        CHECK_QUOTATION(cp, isquoted);
        ct = cp;
        DETERMINE_NEXTSTRING(cp, isquoted);
        cp++;
        (*argv_out)[argnum] = ch_palloc(token_context, cp - ct);
        ch_cpystrn((*argv_out)[argnum], ct, cp - ct);
        cleaned = dirty = (*argv_out)[argnum];
        REMOVE_ESCAPE_CHARS(cleaned, dirty, escaped);
    }
    (*argv_out)[argnum] = NULL;

    return 0;
}

/* Filepath_name_get returns the final element of the pathname.
 * Using the current platform's filename syntax.
 *   "/foo/bar/gum" -> "gum"
 *   "/foo/bar/gum/" -> ""
 *   "gum" -> "gum"
 *   "wi\\n32\\stuff" -> "stuff
 *
 * Corrected Win32 to accept "a/b\\stuff", "a:stuff"
 */

const char * ch_filepath_name_get(const char *pathname)
{
    const char path_separator = '/';
    const char *s = strrchr(pathname, path_separator);

    return s ? ++s : pathname;
}

/* length of dest assumed >= length of src
 * collapse in place (src == dest) is legal.
 * returns terminating null ptr to dest string.
 */
char * ch_collapse_spaces(char *dest, const char *src)
{
    while (*src) {
        if (!ch_isspace(*src)) 
            *dest++ = *src;
        ++src;
    }
    *dest = 0;
    return (dest);
}



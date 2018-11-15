/*
 *
 *      Filename: ch_fnmatch.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 18:49:15
 * Last Modified: 2017-01-10 14:40:54
 */

#include "ch_string.h"
#include "ch_fnmatch.h"
/* Most MBCS/collation/case issues handled here.  Wildcard '*' is not handled.
 * EOS '\0' and the FNM_PATHNAME '/' delimiters are not advanced over, 
 * however the "\/" sequence is advanced to '/'.
 *
 * Both pattern and string are **char to support pointer increment of arbitrary
 * multibyte characters for the given locale, in a later iteration of this code
 */

static ch_inline int fnmatch_ch(const char **pattern, const char **string, int flags)
{
    const char * const mismatch = *pattern;
    const int nocase = !!(flags & CH_FNM_CASE_BLIND);
    const int escape = !(flags & CH_FNM_NOESCAPE);
    const int slash = !!(flags & CH_FNM_PATHNAME);
    int result = CH_FNM_NOMATCH;
    const char *startch;
    int negate;

    if (**pattern == '[')
    {
        ++*pattern;

        /* Handle negation, either leading ! or ^ operators (never both) */
        negate = ((**pattern == '!') || (**pattern == '^'));
        if (negate)
            ++*pattern;

        /* ']' is an ordinary character at the start of the range pattern */
        if (**pattern == ']')
            goto leadingclosebrace;

        while (**pattern)
        {
            if (**pattern == ']') {
                ++*pattern;
                /* XXX: Fix for MBCS character width */
                ++*string;
                return (result ^ negate);
            }

            if (escape && (**pattern == '\\')) {
                ++*pattern;

                /* Patterns must be terminated with ']', not EOS */
                if (!**pattern)
                    break;
            }

            /* Patterns must be terminated with ']' not '/' */
            if (slash && (**pattern == '/'))
                break;

leadingclosebrace:
            /* Look at only well-formed range patterns; 
             * "x-]" is not allowed unless escaped ("x-\]")
             * XXX: Fix for locale/MBCS character width
             */
            if (((*pattern)[1] == '-') && ((*pattern)[2] != ']'))
            {
                startch = *pattern;
                *pattern += (escape && ((*pattern)[2] == '\\')) ? 3 : 2;

                /* NOT a properly balanced [expr] pattern, EOS terminated 
                 * or ranges containing a slash in FNM_PATHNAME mode pattern
                 * fall out to to the rewind and test '[' literal code path
                 */
                if (!**pattern || (slash && (**pattern == '/')))
                    break;

                /* XXX: handle locale/MBCS comparison, advance by MBCS char width */
                if ((**string >= *startch) && (**string <= **pattern))
                    result = 0;
                else if (nocase && (isupper(**string) || isupper(*startch)
                                                      || isupper(**pattern))
                            && (tolower(**string) >= tolower(*startch)) 
                            && (tolower(**string) <= tolower(**pattern)))
                    result = 0;

                ++*pattern;
                continue;
            }

            /* XXX: handle locale/MBCS comparison, advance by MBCS char width */
            if ((**string == **pattern))
                result = 0;
            else if (nocase && (isupper(**string) || isupper(**pattern))
                            && (tolower(**string) == tolower(**pattern)))
                result = 0;

            ++*pattern;
        }

        /* NOT a properly balanced [expr] pattern; Rewind
         * and reset result to test '[' literal
         */
        *pattern = mismatch;
        result = CH_FNM_NOMATCH;
    }
    else if (**pattern == '?') {
        /* Optimize '?' match before unescaping **pattern */
        if (!**string || (slash && (**string == '/')))
            return CH_FNM_NOMATCH;
        result = 0;
        goto fnmatch_ch_success;
    }
    else if (escape && (**pattern == '\\') && (*pattern)[1]) {
        ++*pattern;
    }

    /* XXX: handle locale/MBCS comparison, advance by the MBCS char width */
    if (**string == **pattern)
        result = 0;
    else if (nocase && (isupper(**string) || isupper(**pattern))
                    && (tolower(**string) == tolower(**pattern)))
        result = 0;

    /* Refuse to advance over trailing slash or nulls
     */
    if (!**string || !**pattern || (slash && ((**string == '/') || (**pattern == '/'))))
        return result;

fnmatch_ch_success:
    ++*pattern;
    ++*string;
    return result;
}


int ch_fnmatch(const char *pattern, const char *string, int flags)
{
    static const char dummystring[2] = {' ', 0};
    const int escape = !(flags & CH_FNM_NOESCAPE);
    const int slash = !!(flags & CH_FNM_PATHNAME);
    const char *strendseg;
    const char *dummyptr;
    const char *matchptr;
    int wild;
    /* For '*' wild processing only; surpress 'used before initialization'
     * warnings with dummy initialization values;
     */
    const char *strstartseg = NULL;
    const char *mismatch = NULL;
    int matchlen = 0;

    if (*pattern == '*')
        goto firstsegment;

    while (*pattern && *string)
    {
        /* Pre-decode "\/" which has no special significance, and
         * match balanced slashes, starting a new segment pattern
         */
        if (slash && escape && (*pattern == '\\') && (pattern[1] == '/'))
            ++pattern;
        if (slash && (*pattern == '/') && (*string == '/')) {
            ++pattern;
            ++string;
        }            

firstsegment:
        /* At the beginning of each segment, validate leading period behavior.
         */
        if ((flags & CH_FNM_PERIOD) && (*string == '.'))
        {
            if (*pattern == '.')
                ++pattern;
            else if (escape && (*pattern == '\\') && (pattern[1] == '.'))
                pattern += 2;
            else
                return CH_FNM_NOMATCH;
            ++string;
        }

        /* Determine the end of string segment
         *
         * Presumes '/' character is unique, not composite in any MBCS encoding
         */
        if (slash) {
            strendseg = strchr(string, '/');
            if (!strendseg)
                strendseg = strchr(string, '\0');
        }
        else {
            strendseg = strchr(string, '\0');
        }

        /* Allow pattern '*' to be consumed even with no remaining string to match
         */
        while (*pattern)
        {
            if ((string > strendseg)
                || ((string == strendseg) && (*pattern != '*')))
                break;

            if (slash && ((*pattern == '/')
                           || (escape && (*pattern == '\\')
                                      && (pattern[1] == '/'))))
                break;

            /* Reduce groups of '*' and '?' to n '?' matches
             * followed by one '*' test for simplicity
             */
            for (wild = 0; ((*pattern == '*') || (*pattern == '?')); ++pattern)
            {
                if (*pattern == '*') {
                    wild = 1;
                }
                else if (string < strendseg) {  /* && (*pattern == '?') */
                    /* XXX: Advance 1 char for MBCS locale */
                    ++string;
                }
                else {  /* (string >= strendseg) && (*pattern == '?') */
                    return CH_FNM_NOMATCH;
                }
            }

            if (wild)
            {
                strstartseg = string;
                mismatch = pattern;

                /* Count fixed (non '*') char matches remaining in pattern
                 * excluding '/' (or "\/") and '*'
                 */
                for (matchptr = pattern, matchlen = 0; 1; ++matchlen)
                {
                    if ((*matchptr == '\0') 
                        || (slash && ((*matchptr == '/')
                                      || (escape && (*matchptr == '\\')
                                                 && (matchptr[1] == '/')))))
                    {
                        /* Compare precisely this many trailing string chars,
                         * the resulting match needs no wildcard loop
                         */
                        /* XXX: Adjust for MBCS */
                        if (string + matchlen > strendseg)
                            return CH_FNM_NOMATCH;

                        string = strendseg - matchlen;
                        wild = 0;
                        break;
                    }

                    if (*matchptr == '*')
                    {
                        /* Ensure at least this many trailing string chars remain
                         * for the first comparison
                         */
                        /* XXX: Adjust for MBCS */
                        if (string + matchlen > strendseg)
                            return CH_FNM_NOMATCH;

                        /* Begin first wild comparison at the current position */
                        break;
                    }

                    /* Skip forward in pattern by a single character match
                     * Use a dummy fnmatch_ch() test to count one "[range]" escape
                     */ 
                    /* XXX: Adjust for MBCS */
                    if (escape && (*matchptr == '\\') && matchptr[1]) {
                        matchptr += 2;
                    }
                    else if (*matchptr == '[') {
                        dummyptr = dummystring;
                        fnmatch_ch(&matchptr, &dummyptr, flags);
                    }
                    else {
                        ++matchptr;
                    }
                }
            }

            /* Incrementally match string against the pattern
             */
            while (*pattern && (string < strendseg))
            {
                /* Success; begin a new wild pattern search
                 */
                if (*pattern == '*')
                    break;

                if (slash && ((*string == '/')
                              || (*pattern == '/')
                              || (escape && (*pattern == '\\')
                                         && (pattern[1] == '/'))))
                    break;

                /* Compare ch's (the pattern is advanced over "\/" to the '/',
                 * but slashes will mismatch, and are not consumed)
                 */
                if (!fnmatch_ch(&pattern, &string, flags))
                    continue;

                /* Failed to match, loop against next char offset of string segment 
                 * until not enough string chars remain to match the fixed pattern
                 */
                if (wild) {
                    /* XXX: Advance 1 char for MBCS locale */
                    string = ++strstartseg;
                    if (string + matchlen > strendseg)
                        return CH_FNM_NOMATCH;

                    pattern = mismatch;
                    continue;
                }
                else
                    return CH_FNM_NOMATCH;
            }
        }

        if (*string && !(slash && (*string == '/')))
            return CH_FNM_NOMATCH;

        if (*pattern && !(slash && ((*pattern == '/')
                                    || (escape && (*pattern == '\\')
                                               && (pattern[1] == '/')))))
            return CH_FNM_NOMATCH;
    }

    /* Where both pattern and string are at EOS, declare success
     */
    if (!*string && !*pattern)
        return 0;

    /* pattern didn't match to the end of string */
    return CH_FNM_NOMATCH;
}


/* This function is an Apache addition
 * return non-zero if pattern has any glob chars in it
 * @bug Function does not distinguish for FNM_PATHNAME mode, which renders
 * a false positive for test[/]this (which is not a range, but 
 * seperate test[ and ]this segments and no glob.)
 * @bug Function does not distinguish for non-FNM_ESCAPE mode.
 * @bug Function does not parse []] correctly
 * Solution may be to use fnmatch_ch() to walk the patterns?
 */
int ch_fnmatch_test(const char *pattern)
{
    int nesting;

    nesting = 0;
    while (*pattern) {
        switch (*pattern) {
        case '?':
        case '*':
            return 1;

        case '\\':
            if (*++pattern == '\0') {
                return 0;
            }
            break;

        case '[':         /* '[' is only a glob if it has a matching ']' */
            ++nesting;
            break;

        case ']':
            if (nesting) {
                return 1;
            }
            break;
        }
        ++pattern;    }
    return 0;
}


/* Find all files matching the specified pattern */
int ch_match_glob(const char *pattern, 
                                         ch_array_header_t **result,
                                         ch_pool_t *p)
{
    DIR *dir;
	struct dirent *next;
    char *path;

    /* XXX So, this is kind of bogus.  Basically, I need to strip any leading
     * directories off the pattern, but there is no portable way to do that.
     * So, for now we just find the last occurance of '/' and if that doesn't
     * return anything, then we look for '\'.  This means that we could
     * screw up on unix if the pattern is something like "foo\.*"  That '\'
     * isn't a directory delimiter, it is a part of the filename.  To fix this,
     * we really need ch_filepath_basename, which will be coming as soon as
     * I get to it.  rbb
     */
    char *idx = strrchr(pattern, '/');
    
    if (idx == NULL) {
        idx = strrchr(pattern, '\\');
    }
    if (idx == NULL) {
        path = ".";
    }
    else {
        path = ch_pstrndup(p, pattern, idx - pattern);
        pattern = idx + 1;
    }

    *result = ch_array_make(p, 0, sizeof(char *));
    dir = opendir(path);

    if (dir == NULL) {
        return -1;
    }

    while ((next = readdir(dir)) != NULL) {
        if (ch_fnmatch(pattern, next->d_name, 0) == 0) {
            *(const char **)ch_array_push(*result) = ch_pstrdup(p, next->d_name);
        }
    }

    closedir(dir);

    return 0;
}

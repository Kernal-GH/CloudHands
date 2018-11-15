/*
 *
 *      Filename: ch_fnmatch.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 19:00:29
 * Last Modified: 2017-01-09 19:00:29
 */

#ifndef	CH_FNMATCH_H
#define	CH_FNMATCH_H

#include "ch_mpool.h"
#include "ch_tables.h"
/**
 * @defgroup ch_fnmatch Filename Matching Functions
 * @ingroup GW 
 * @{
 */

#define CH_FNM_NOMATCH     1     /**< Match failed. */
 
#define CH_FNM_NOESCAPE    0x01  /**< Disable backslash escaping. */
#define CH_FNM_PATHNAME    0x02  /**< Slash must be matched by slash. */
#define CH_FNM_PERIOD      0x04  /**< Period must be matched by period. */
#define CH_FNM_CASE_BLIND  0x08  /**< Compare characters case-insensitively. */

/**
 * Try to match the string to the given pattern, return 0 if
 *    match, else return CH_FNM_NOMATCH.  Note that there is no such thing as
 *    an illegal pattern.
 *
 * With all flags unset, a pattern is interpreted as such:
 *
 * PATTERN: Backslash followed by any character, including another
 *          backslash.<br/>
 * MATCHES: That character exactly.
 * 
 * <p>
 * PATTERN: ?<br/>
 * MATCHES: Any single character.
 * </p>
 * 
 * <p>
 * PATTERN: *<br/>
 * MATCHES: Any sequence of zero or more characters. (Note that multiple
 *          *s in a row are equivalent to one.)
 * 
 * PATTERN: Any character other than \?*[ or a \ at the end of the pattern<br/>
 * MATCHES: That character exactly. (Case sensitive.)
 * 
 * PATTERN: [ followed by a class description followed by ]<br/>
 * MATCHES: A single character described by the class description.
 *          (Never matches, if the class description reaches until the
 *          end of the string without a ].) If the first character of
 *          the class description is ^ or !, the sense of the description
 *          is reversed.  The rest of the class description is a list of
 *          single characters or pairs of characters separated by -. Any
 *          of those characters can have a backslash in front of them,
 *          which is ignored; this lets you use the characters ] and -
 *          in the character class, as well as ^ and ! at the
 *          beginning.  The pattern matches a single character if it
 *          is one of the listed characters or falls into one of the
 *          listed ranges (inclusive, case sensitive).  Ranges with
 *          the first character larger than the second are legal but
 *          never match. Edge cases: [] never matches, and [^] and [!]
 *          always match without consuming a character.
 * 
 * Note that these patterns attempt to match the entire string, not
 * just find a substring matching the pattern.
 *
 * @param pattern The pattern to match to
 * @param strings The string we are trying to match
 * @param flags flags to use in the match.  Bitwise OR of:
 * <pre>
 *              CH_FNM_NOESCAPE       Disable backslash escaping
 *              CH_FNM_PATHNAME       Slash must be matched by slash
 *              CH_FNM_PERIOD         Period must be matched by period
 *              CH_FNM_CASE_BLIND     Compare characters case-insensitively.
 * </pre>
 */

int ch_fnmatch(const char *pattern,const char *strings, int flags);

/**
 * Determine if the given pattern is a regular expression.
 * @param pattern The pattern to search for glob characters.
 * @return non-zero if pattern has any glob characters in it
 */
int ch_fnmatch_test(const char *pattern);

/**
 * Find all files that match a specified pattern in a directory.
 * @param dir_pattern The pattern to use for finding files, appended
 * to the search directory.  The pattern is anything following the
 * final forward or backward slash in the parameter.  If no slash
 * is found, the current directory is searched.
 * @param result Array to use when storing the results
 * @param p The pool to use.
 * @return 0 if no processing errors occurred, GW error
 * code otherwise
 * @remark The returned array may be empty even if 0 was
 * returned.
 */
int ch_match_glob(const char *dir_pattern, 
                                         ch_array_header_t **result,
                                         ch_pool_t *p);

#endif /* CH_FNMATCH_H */

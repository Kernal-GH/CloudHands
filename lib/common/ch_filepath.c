/*
 *
 *      Filename: ch_filepath.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 13:23:02
 * Last Modified: 2017-01-11 13:33:37
 */

#include "ch_file_info.h"
#include "ch_errno.h"
#include "ch_string.h"

/* Any OS that requires/refuses trailing slashes should be dealt with here.
 */
int ch_filepath_get(char **defpath, int32_t flags,
                                           ch_pool_t *p)
{
	flags = flags;
    char path[CH_PATH_MAX];

    if (!getcwd(path, sizeof(path))) {
        if (errno == ERANGE)
            return CH_ENAMETOOLONG;
        else
            return errno;
    }
    *defpath = ch_pstrdup(p, path);

    return 0;
}


/* Any OS that requires/refuses trailing slashes should be dealt with here
 */
int ch_filepath_set(const char *path, ch_pool_t *p)
{
	p = p;

    if (chdir(path) != 0)
        return errno;

    return 0;
}

int ch_filepath_root(const char **rootpath,
                                            const char **inpath,
                                            int32_t flags,
                                            ch_pool_t *p)
{
	flags = flags;
    if (**inpath == '/') {
        *rootpath = ch_pstrdup(p, "/");
        do {
            ++(*inpath);
        } while (**inpath == '/');

        return 0;
    }

    return CH_ERELATIVE;
}

int ch_filepath_merge(char **newpath,
                                             const char *rootpath,
                                             const char *addpath,
                                             int32_t flags,
                                             ch_pool_t *p)
{
    char *path;
    size_t rootlen; /* is the length of the src rootpath */
    size_t maxlen;  /* maximum total path length */
    size_t keptlen; /* is the length of the retained rootpath */
    size_t pathlen; /* is the length of the result path */
    size_t seglen;  /* is the end of the current segment */
    int rv;

    /* Treat null as an empty path.
     */
    if (!addpath)
        addpath = "";

    if (addpath[0] == '/') {
        /* If addpath is rooted, then rootpath is unused.
         * Ths violates any CH_FILEPATH_SECUREROOTTEST and
         * CH_FILEPATH_NOTABSOLUTE flags specified.
         */
        if (flags & CH_FILEPATH_SECUREROOTTEST)
            return CH_EABOVEROOT;
        if (flags & CH_FILEPATH_NOTABSOLUTE)
            return CH_EABSOLUTE;

        /* If CH_FILEPATH_NOTABOVEROOT wasn't specified,
         * we won't test the root again, it's ignored.
         * Waste no CPU retrieving the working path.
         */
        if (!rootpath && !(flags & CH_FILEPATH_NOTABOVEROOT))
            rootpath = "";
    }
    else {
        /* If CH_FILEPATH_NOTABSOLUTE is specified, the caller
         * requires a relative result.  If the rootpath is
         * ommitted, we do not retrieve the working path,
         * if rootpath was supplied as absolute then fail.
         */
        if (flags & CH_FILEPATH_NOTABSOLUTE) {
            if (!rootpath)
                rootpath = "";
            else if (rootpath[0] == '/')
                return CH_EABSOLUTE;
        }
    }

    if (!rootpath) {
        /* Start with the current working path.  This is bass akwards,
         * but required since the compiler (at least vc) doesn't like
         * passing the address of a char const* for a char** arg.
         */
        char *getpath;
        rv = ch_filepath_get(&getpath, flags, p);
        rootpath = getpath;
        if (rv != 0)
            return errno;

        /* XXX: Any kernel subject to goofy, uncanonical results
         * must run the rootpath against the user's given flags.
         * Simplest would be a recursive call to ch_filepath_merge
         * with an empty (not null) rootpath and addpath of the cwd.
         */
    }

    rootlen = strlen(rootpath);
    maxlen = rootlen + strlen(addpath) + 4; /* 4 for slashes at start, after
                                             * root, and at end, plus trailing
                                             * null */
    if (maxlen > CH_PATH_MAX) {
        return CH_ENAMETOOLONG;
    }
    path = (char *)ch_palloc(p, maxlen);

    if (addpath[0] == '/') {
        /* Ignore the given root path, strip off leading
         * '/'s to a single leading '/' from the addpath,
         * and leave addpath at the first non-'/' character.
         */
        keptlen = 0;
        while (addpath[0] == '/')
            ++addpath;
        path[0] = '/';
        pathlen = 1;
    }
    else {
        /* If both paths are relative, fail early
         */
        if (rootpath[0] != '/' && (flags & CH_FILEPATH_NOTRELATIVE))
            return CH_ERELATIVE;

        /* Base the result path on the rootpath
         */
        keptlen = rootlen;
        memcpy(path, rootpath, rootlen);

        /* Always '/' terminate the given root path
         */
        if (keptlen && path[keptlen - 1] != '/') {
            path[keptlen++] = '/';
        }
        pathlen = keptlen;
    }

    while (*addpath) {
        /* Parse each segment, find the closing '/'
         */
        const char *next = addpath;
        while (*next && (*next != '/')) {
            ++next;
        }
        seglen = next - addpath;

        if (seglen == 0 || (seglen == 1 && addpath[0] == '.')) {
            /* noop segment (/ or ./) so skip it
             */
        }
        else if (seglen == 2 && addpath[0] == '.' && addpath[1] == '.') {
            /* backpath (../) */
            if (pathlen == 1 && path[0] == '/') {
                /* Attempt to move above root.  Always die if the
                 * CH_FILEPATH_SECUREROOTTEST flag is specified.
                 */
                if (flags & CH_FILEPATH_SECUREROOTTEST) {
                    return CH_EABOVEROOT;
                }

                /* Otherwise this is simply a noop, above root is root.
                 * Flag that rootpath was entirely replaced.
                 */
                keptlen = 0;
            }
            else if (pathlen == 0
                     || (pathlen == 3
                         && !memcmp(path + pathlen - 3, "../", 3))
                     || (pathlen  > 3
                         && !memcmp(path + pathlen - 4, "/../", 4))) {
                /* Path is already backpathed or empty, if the
                 * CH_FILEPATH_SECUREROOTTEST.was given die now.
                 */
                if (flags & CH_FILEPATH_SECUREROOTTEST) {
                    return CH_EABOVEROOT;
                }

                /* Otherwise append another backpath, including
                 * trailing slash if present.
                 */
                memcpy(path + pathlen, "../", *next ? 3 : 2);
                pathlen += *next ? 3 : 2;
            }
            else {
                /* otherwise crop the prior segment
                 */
                do {
                    --pathlen;
                } while (pathlen && path[pathlen - 1] != '/');
            }

            /* Now test if we are above where we started and back up
             * the keptlen offset to reflect the added/altered path.
             */
            if (pathlen < keptlen) {
                if (flags & CH_FILEPATH_SECUREROOTTEST) {
                    return CH_EABOVEROOT;
                }
                keptlen = pathlen;
            }
        }
        else {
            /* An actual segment, append it to the destination path
             */
            if (*next) {
                seglen++;
            }
            memcpy(path + pathlen, addpath, seglen);
            pathlen += seglen;
        }

        /* Skip over trailing slash to the next segment
         */
        if (*next) {
            ++next;
        }

        addpath = next;
    }
    path[pathlen] = '\0';

    /* keptlen will be the rootlen unless the addpath contained
     * backpath elements.  If so, and CH_FILEPATH_NOTABOVEROOT
     * is specified (CH_FILEPATH_SECUREROOTTEST was caught above),
     * compare the original root to assure the result path is
     * still within given root path.
     */
    if ((flags & CH_FILEPATH_NOTABOVEROOT) && keptlen < rootlen) {
        if (strncmp(rootpath, path, rootlen)) {
            return CH_EABOVEROOT;
        }
        if (rootpath[rootlen - 1] != '/'
            && path[rootlen] && path[rootlen] != '/') {
            return CH_EABOVEROOT;
        }
    }

    *newpath = path;
    return 0;
}

int ch_filepath_list_split_impl(ch_array_header_t **pathelts,
                                          const char *liststr,
                                          char separator,
                                          ch_pool_t *p)
{
    char *path, *part, *ptr;
    char separator_string[2] = { '\0', '\0' };
    ch_array_header_t *elts;
    int nelts;

    separator_string[0] = separator;
    /* Count the number of path elements. We know there'll be at least
       one even if path is an empty string. */
    path = ch_pstrdup(p, liststr);
    for (nelts = 0, ptr = path; ptr != NULL; ++nelts)
    {
        ptr = strchr(ptr, separator);
        if (ptr)
            ++ptr;
    }

    /* Split the path into the array. */
    elts = ch_array_make(p, nelts, sizeof(char*));
    while ((part = ch_strtok(path, separator_string, &ptr)) != NULL)
    {
        if (*part == '\0')      /* Ignore empty path components. */
            continue;

        *(char**)ch_array_push(elts) = part;
        path = NULL;            /* For the next call to ch_strtok */
    }

    *pathelts = elts;
    return 0;
}


int ch_filepath_list_merge_impl(char **liststr,
                                          ch_array_header_t *pathelts,
                                          char separator,
                                          ch_pool_t *p)
{
    size_t path_size = 0;
    char *path;
    int i;

    /* This test isn't 100% certain, but it'll catch at least some
       invalid uses... */
    if (pathelts->elt_size != sizeof(char*))
        return CH_EINVAL;

    /* Calculate the size of the merged path */
    for (i = 0; i < pathelts->nelts; ++i)
        path_size += strlen(((char**)pathelts->elts)[i]);

    if (path_size == 0)
    {
        *liststr = NULL;
        return 0;
    }

    if (i > 0)                  /* Add space for the separators */
        path_size += (i - 1);

    /* Merge the path components */
    path = *liststr = ch_palloc(p, path_size + 1);
    for (i = 0; i < pathelts->nelts; ++i)
    {
        /* ### Hmmmm. Calling strlen twice on the same string. Yuck.
               But is is better than reallocation in ch_pstrcat? */
        const char *part = ((char**)pathelts->elts)[i];
        size_t part_size = strlen(part);
        if (part_size == 0)     /* Ignore empty path components. */
            continue;

        if (i > 0)
            *path++ = separator;
        memcpy(path, part, part_size);
        path += part_size;
    }
    *path = '\0';
    return 0;
}

int ch_filepath_list_split(ch_array_header_t **pathelts,
                                                  const char *liststr,
                                                  ch_pool_t *p)
{
    return ch_filepath_list_split_impl(pathelts, liststr, ':', p);
}

int ch_filepath_list_merge(char **liststr,
                                                  ch_array_header_t *pathelts,
                                                  ch_pool_t *p)
{
    return ch_filepath_list_merge_impl(liststr, pathelts, ':', p);
}

int ch_filepath_encoding(int *style, ch_pool_t *p)
{
	p = p;
    *style = CH_FILEPATH_ENCODING_LOCALE;
    return 0;
}

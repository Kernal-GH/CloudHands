/*
 *
 *      Filename: ch_dir.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 12:39:57
 * Last Modified: 2017-01-11 13:08:59
 */

#include "ch_file_info.h"
#include "ch_errno.h"
#include "ch_constants.h"
#include "ch_string.h"
#include "ch_file.h"

#define PATH_SEPARATOR '/'

/* Remove trailing separators that don't affect the meaning of PATH. */
static const char *path_canonicalize (const char *path, ch_pool_t *pool)
{
    /* At some point this could eliminate redundant components.  For
     * now, it just makes sure there is no trailing slash. */
    size_t len = strlen (path);
    size_t orig_len = len;
    
    while ((len > 0) && (path[len - 1] == PATH_SEPARATOR))
        len--;
    
    if (len != orig_len)
        return ch_pstrndup (pool, path, len);
    else
        return path;
}

/* Remove one component off the end of PATH. */
static char *path_remove_last_component (const char *path, ch_pool_t *pool)
{
    const char *newpath = path_canonicalize (path, pool);
    int i;
    
    for (i = (strlen(newpath) - 1); i >= 0; i--) {
        if (path[i] == PATH_SEPARATOR)
            break;
    }

    return ch_pstrndup (pool, path, (i < 0) ? 0 : i);
}

int ch_dir_open(ch_dir_t **new, const char *dirname, 
                          ch_pool_t *pool)
{
    /* On some platforms (e.g., Linux+GNU libc), d_name[] in struct 
     * dirent is declared with enough storage for the name.  On other
     * platforms (e.g., Solaris 8 for Intel), d_name is declared as a
     * one-byte array.  Note: gcc evaluates this at compile time.
     */
    size_t dirent_size = 
        sizeof(*(*new)->entry) +
        (sizeof((*new)->entry->d_name) > 1 ? 0 : 255);
    DIR *dir = opendir(dirname);

    if (!dir) {
        return errno;
    }

    (*new) = (ch_dir_t *)ch_palloc(pool, sizeof(ch_dir_t));

    (*new)->pool = pool;
    (*new)->dirname = ch_pstrdup(pool, dirname);
    (*new)->dirstruct = dir;
    (*new)->entry = ch_pcalloc(pool, dirent_size);

    return 0;
}

int ch_dir_close(ch_dir_t *dir)
{
    if (closedir(dir->dirstruct) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

#ifdef DIRENT_TYPE
static ch_filetype_e filetype_from_dirent_type(int type)
{
    switch (type) {
    case DT_REG:
        return CH_REG;
    case DT_DIR:
        return CH_DIR;
    case DT_LNK:
        return CH_LNK;
    case DT_CHR:
        return CH_CHR;
    case DT_BLK:
        return CH_BLK;
#if defined(DT_FIFO)
    case DT_FIFO:
        return CH_PIPE;
#endif
#if !defined(BEOS) && defined(DT_SOCK)
    case DT_SOCK:
        return CH_SOCK;
#endif
    default:
        return CH_UNKFILE;
    }
}
#endif

int ch_dir_read(ch_finfo_t *finfo, int32_t wanted,
                          ch_dir_t *thedir)
{
    int ret = 0;
#ifdef DIRENT_TYPE
    ch_filetype_e type;
#endif



    /* We're about to call a non-thread-safe readdir() that may
       possibly set `errno', and the logic below actually cares about
       errno after the call.  Therefore we need to clear errno first. */
    errno = 0;
    thedir->entry = readdir(thedir->dirstruct);
    if (thedir->entry == NULL) {
        /* If NULL was returned, this can NEVER be a success. Can it?! */
        if (errno == 0) {
            ret = CH_ENOENT;
        }
        else
            ret = errno;
    }

    /* No valid bit flag to test here - do we want one? */
    finfo->fname = NULL;

    if (ret) {
        finfo->valid = 0;
        return ret;
    }

#ifdef DIRENT_TYPE
    type = filetype_from_dirent_type(thedir->entry->DIRENT_TYPE);
    if (type != CH_UNKFILE) {
        wanted &= ~CH_FINFO_TYPE;
    }
#endif
#ifdef DIRENT_INODE
    if (thedir->entry->DIRENT_INODE && thedir->entry->DIRENT_INODE != -1) {

        wanted &= ~CH_FINFO_INODE;
    }
#endif /* DIRENT_INODE */

    wanted &= ~CH_FINFO_NAME;

    if (wanted)
    {
        char fspec[CH_PATH_MAX];
        char *end;

        end = ch_cpystrn(fspec, thedir->dirname, sizeof fspec);

        if (end > fspec && end[-1] != '/' && (end < fspec + CH_PATH_MAX))
            *end++ = '/';

        ch_cpystrn(end, thedir->entry->d_name, 
                    sizeof fspec - (end - fspec));

        ret = ch_stat(finfo, fspec, CH_FINFO_LINK | wanted, thedir->pool);
        /* We passed a stack name that will disappear */
        finfo->fname = NULL;
    }

    if (wanted && (ret == 0 || ret == CH_INCOMPLETE)) {
        wanted &= ~finfo->valid;
    }
    else {
        /* We don't bail because we fail to stat, when we are only -required-
         * to readdir... but the result will be CH_INCOMPLETE
         */
        finfo->pool = thedir->pool;
        finfo->valid = 0;
#ifdef DIRENT_TYPE
        if (type != CH_UNKFILE) {
            finfo->filetype = type;
            finfo->valid |= CH_FINFO_TYPE;
        }
#endif
#ifdef DIRENT_INODE
        if (thedir->entry->DIRENT_INODE && thedir->entry->DIRENT_INODE != -1) {
            finfo->inode = thedir->entry->DIRENT_INODE;
            finfo->valid |= CH_FINFO_INODE;
        }
#endif
    }

    finfo->name = ch_pstrdup(thedir->pool, thedir->entry->d_name);
    finfo->valid |= CH_FINFO_NAME;

    if (wanted)
        return CH_INCOMPLETE;

    return 0;
}

int ch_dir_rewind(ch_dir_t *thedir)
{
    rewinddir(thedir->dirstruct);
    return 0;
}

int ch_dir_make(const char *path, int32_t perm, 
                          ch_pool_t *pool)
{
	pool = pool;

    mode_t mode = ch_perms2mode(perm);

    if (mkdir(path, mode) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

int ch_dir_make_recursive(const char *path, int32_t perm,
                                           ch_pool_t *pool) 
{
    int ch_err = 0;

    ch_err = ch_dir_make (path, perm, pool); /* Try to make PATH right out */
    
    if (ch_err == ENOENT) { /* Missing an intermediate dir */
        char *dir;
        
        dir = path_remove_last_component(path, pool);
        /* If there is no path left, give up. */
        if (dir[0] == '\0') {
            return ch_err;
        }

        ch_err = ch_dir_make_recursive(dir, perm, pool);
        
        if (!ch_err) 
            ch_err = ch_dir_make (path, perm, pool);
    }

    /*
     * It's OK if PATH exists. Timing issues can lead to the second
     * ch_dir_make being called on existing dir, therefore this check
     * has to come last.
     */
    if (CH_STATUS_IS_EEXIST(ch_err))
        return 0;

    return ch_err;
}

int ch_dir_remove(const char *path, ch_pool_t *pool)
{
	pool = pool;

    if (rmdir(path) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}



  

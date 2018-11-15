/*
 *
 *      Filename: ch_file_open.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 15:15:44
 * Last Modified: 2017-01-11 15:32:20
 */

#include "ch_file.h"
#include "ch_file_info.h"
#include "ch_errno.h"

static int file_cleanup(ch_file_t *file, int is_child)
{
    int rv = 0;
    int fd = file->filedes;

    /* Set file descriptor to -1 before close(), so that there is no
     * chance of returning an already closed FD from ch_os_file_get().
     */
    file->filedes = -1;

    if (close(fd) == 0) {
        /* Only the parent process should delete the file! */
        if (!is_child && (file->flags & CH_FOPEN_DELONCLOSE)) {
            unlink(file->fname);
        }
    }
    else {
        /* Restore, close() was not successful. */
        file->filedes = fd;

        /* Are there any error conditions other than EINTR or EBADF? */
        rv = errno;
    }
    return rv;
}

int ch_unix_file_cleanup(void *thefile)
{
    ch_file_t *file = thefile;
    int flush_rv = 0, rv = 0;

    if (file->buffered) {
        flush_rv = ch_file_flush(file);
    }

    rv = file_cleanup(file, 0);

    return rv != 0 ? rv : flush_rv;
}

int ch_unix_child_file_cleanup(void *thefile)
{
    return file_cleanup(thefile, 1);
}

int ch_file_open(ch_file_t **new, 
                                        const char *fname, 
                                        int32_t flag, 
                                        int32_t perm, 
                                        ch_pool_t *pool)
{
    int fd;
    int oflags = 0;

    if ((flag & CH_FOPEN_READ) && (flag & CH_FOPEN_WRITE)) {
        oflags = O_RDWR;
    }
    else if (flag & CH_FOPEN_READ) {
        oflags = O_RDONLY;
    }
    else if (flag & CH_FOPEN_WRITE) {
        oflags = O_WRONLY;
    }
    else {
        return CH_EACCES; 
    }

    if (flag & CH_FOPEN_CREATE) {
        oflags |= O_CREAT;
        if (flag & CH_FOPEN_EXCL) {
            oflags |= O_EXCL;
        }
    }
    if ((flag & CH_FOPEN_EXCL) && !(flag & CH_FOPEN_CREATE)) {
        return CH_EACCES;
    }   

    if (flag & CH_FOPEN_APPEND) {
        oflags |= O_APPEND;
    }
    if (flag & CH_FOPEN_TRUNCATE) {
        oflags |= O_TRUNC;
    }
#ifdef O_BINARY
    if (flag & CH_FOPEN_BINARY) {
        oflags |= O_BINARY;
    }
#endif

    if (flag & CH_FOPEN_NONBLOCK) {
#ifdef O_NONBLOCK
        oflags |= O_NONBLOCK;
#else
        return CH_ENOTIMPL;
#endif
    }

#ifdef O_CLOEXEC
    /* Introduced in Linux 2.6.23. Silently ignored on earlier Linux kernels.
     */
    if (!(flag & CH_FOPEN_NOCLEANUP)) {
        oflags |= O_CLOEXEC;
}
#endif
 
#if CH_HAS_LARGE_FILES && defined(_LARGEFILE64_SOURCE)
    oflags |= O_LARGEFILE;
#elif defined(O_LARGEFILE)
    if (flag & CH_FOPEN_LARGEFILE) {
        oflags |= O_LARGEFILE;
    }
#endif

    if (perm == CH_OS_DEFAULT) {
        fd = open(fname, oflags, 0666);
    }
    else {
        fd = open(fname, oflags, ch_perms2mode(perm));
    } 
    if (fd < 0) {
       return errno;
    }
    if (!(flag & CH_FOPEN_NOCLEANUP)) {
#ifdef O_CLOEXEC
        static int has_o_cloexec = 0;
        if (!has_o_cloexec)
#endif
        {
            int flags;

            if ((flags = fcntl(fd, F_GETFD)) == -1) {
                close(fd);
                return errno;
            }
            if ((flags & FD_CLOEXEC) == 0) {
                flags |= FD_CLOEXEC;
                if (fcntl(fd, F_SETFD, flags) == -1) {
                    close(fd);
                    return errno;
                }
            }
#ifdef O_CLOEXEC
            else {
                has_o_cloexec = 1;
            }
#endif
        }
    }

    (*new) = (ch_file_t *)ch_pcalloc(pool, sizeof(ch_file_t));
    (*new)->pool = pool;
    (*new)->flags = flag;
    (*new)->filedes = fd;

    (*new)->fname = ch_pstrdup(pool, fname);

    (*new)->blocking = BLK_ON;
    (*new)->buffered = (flag & CH_FOPEN_BUFFERED) > 0;

    if ((*new)->buffered) {
        (*new)->buffer = ch_palloc(pool, CH_FILE_DEFAULT_BUFSIZE);
        (*new)->bufsize = CH_FILE_DEFAULT_BUFSIZE;
    }
    else {
        (*new)->buffer = NULL;
    }

    (*new)->is_pipe = 0;
    (*new)->timeout = -1;
    (*new)->ungetchar = -1;
    (*new)->eof_hit = 0;
    (*new)->filePtr = 0;
    (*new)->bufpos = 0;
    (*new)->dataRead = 0;
    (*new)->direction = 0;
    return 0;
}

int ch_file_close(ch_file_t *file)
{
    return close(file->filedes);
}

int ch_file_remove(const char *path, ch_pool_t *pool)
{
	pool = pool;

    if (unlink(path) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

int ch_file_rename(const char *from_path, 
                                          const char *to_path,
                                          ch_pool_t *p)
{
	p = p;

    if (rename(from_path, to_path) != 0) {
        return errno;
    }
    return 0;
}

int ch_os_file_put(ch_file_t **file, 
                                          int *thefile,
                                          int32_t flags, ch_pool_t *pool)
{
    int *dafile = thefile;
    
    (*file) = ch_pcalloc(pool, sizeof(ch_file_t));
    (*file)->pool = pool;
    (*file)->eof_hit = 0;
    (*file)->blocking = BLK_UNKNOWN; /* in case it is a pipe */
    (*file)->timeout = -1;
    (*file)->ungetchar = -1; /* no char avail */
    (*file)->filedes = *dafile;
    (*file)->flags = flags | CH_FOPEN_NOCLEANUP;
    (*file)->buffered = (flags & CH_FOPEN_BUFFERED) > 0;

    if ((*file)->buffered) {
        (*file)->buffer = ch_palloc(pool, CH_FILE_DEFAULT_BUFSIZE);
        (*file)->bufsize = CH_FILE_DEFAULT_BUFSIZE;
    }
    return 0;
}    

int ch_file_eof(ch_file_t *fptr)
{
    if (fptr->eof_hit == 1) {
        return CH_EOF;
    }
    return 0;
}   

int ch_file_open_flags_stderr(ch_file_t **thefile, 
                                                     int32_t flags,
                                                     ch_pool_t *pool)
{
    int fd = STDERR_FILENO;

    return ch_os_file_put(thefile, &fd, flags | CH_FOPEN_WRITE, pool);
}

int ch_file_open_flags_stdout(ch_file_t **thefile, 
                                                     int32_t flags,
                                                     ch_pool_t *pool)
{
    int fd = STDOUT_FILENO;

    return ch_os_file_put(thefile, &fd, flags | CH_FOPEN_WRITE, pool);
}

int ch_file_open_flags_stdin(ch_file_t **thefile, 
                                                    int32_t flags,
                                                    ch_pool_t *pool)
{
    int fd = STDIN_FILENO;

    return ch_os_file_put(thefile, &fd, flags | CH_FOPEN_READ, pool);
}

int ch_file_open_stderr(ch_file_t **thefile, 
                                               ch_pool_t *pool)
{
    return ch_file_open_flags_stderr(thefile, 0, pool);
}

int ch_file_open_stdout(ch_file_t **thefile, 
                                               ch_pool_t *pool)
{
    return ch_file_open_flags_stdout(thefile, 0, pool);
}

int ch_file_open_stdin(ch_file_t **thefile, 
                                              ch_pool_t *pool)
{
    return ch_file_open_flags_stdin(thefile, 0, pool);
}




int ch_file_link(const char *from_path, 
                                          const char *to_path)
{
    if (link(from_path, to_path) == -1) {
        return errno;
    }

    return 0;
}

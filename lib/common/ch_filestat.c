/*
 *
 *      Filename: ch_filestat.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:20:18
 * Last Modified: 2017-01-11 15:10:33
 */

#include "ch_file_info.h"
#include "ch_file.h"
#include "ch_errno.h"
#include "ch_time.h"

static ch_filetype_e filetype_from_mode(mode_t mode)
{
    ch_filetype_e type;

    switch (mode & S_IFMT) {
    case S_IFREG:
        type = CH_REG;  break;
    case S_IFDIR:
        type = CH_DIR;  break;
    case S_IFLNK:
        type = CH_LNK;  break;
    case S_IFCHR:
        type = CH_CHR;  break;
    case S_IFBLK:
        type = CH_BLK;  break;
#if defined(S_IFFIFO)
    case S_IFFIFO:
        type = CH_PIPE; break;
#endif
#if !defined(BEOS) && defined(S_IFSOCK)
    case S_IFSOCK:
        type = CH_SOCK; break;
#endif

    default:
	/* Work around missing S_IFxxx values above
         * for Linux et al.
         */
#if !defined(S_IFFIFO) && defined(S_ISFIFO)
    	if (S_ISFIFO(mode)) {
            type = CH_PIPE;
	} else
#endif
#if !defined(BEOS) && !defined(S_IFSOCK) && defined(S_ISSOCK)
    	if (S_ISSOCK(mode)) {
            type = CH_SOCK;
	} else
#endif
        type = CH_UNKFILE;
    }
    return type;
}

static void fill_out_finfo(ch_finfo_t *finfo, struct_stat *info,
                           int32_t wanted)
{
	wanted = wanted;

    finfo->valid = CH_FINFO_MIN | CH_FINFO_IDENT | CH_FINFO_NLINK
                 | CH_FINFO_OWNER | CH_FINFO_PROT;
    finfo->protection = ch_mode2perms(info->st_mode);
    finfo->filetype = filetype_from_mode(info->st_mode);
    finfo->user = info->st_uid;
    finfo->group = info->st_gid;
    finfo->size = info->st_size;
    finfo->device = info->st_dev;
    finfo->nlink = info->st_nlink;

    /* Check for overflow if storing a 64-bit st_ino in a 32-bit
     * ino_t for LFS builds: */
    if (sizeof(ino_t) >= sizeof(info->st_ino)
        || (ino_t)info->st_ino == info->st_ino) {
        finfo->inode = info->st_ino;
    } else {
        finfo->valid &= ~CH_FINFO_INODE;
    }

    ch_time_ansi_put(&finfo->atime, info->st_atime);
    finfo->atime += info->st_atim.tv_nsec / 1000L;

    ch_time_ansi_put(&finfo->mtime, info->st_mtime);
    finfo->mtime += info->st_mtim.tv_nsec / 1000L;

    ch_time_ansi_put(&finfo->ctime, info->st_ctime);
    finfo->ctime += info->st_ctim.tv_nsec / 1000L;

#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
#ifdef DEV_BSIZE
    finfo->csize = (ch_off_t)info->st_blocks * (ch_off_t)DEV_BSIZE;
#else
    finfo->csize = (ch_off_t)info->st_blocks * (ch_off_t)512;
#endif
    finfo->valid |= CH_FINFO_CSIZE;
#endif
}

int ch_file_info_get_locked(ch_finfo_t *finfo, int32_t wanted,
                                      ch_file_t *thefile)
{
    struct_stat info;

    if (thefile->buffered) {
        int rv = ch_file_flush_locked(thefile);
        if (rv != 0)
            return rv;
    }

    if (fstat(thefile->filedes, &info) == 0) {
        finfo->pool = thefile->pool;
        finfo->fname = thefile->fname;
        fill_out_finfo(finfo, &info, wanted);
        return (wanted & ~finfo->valid) ? CH_INCOMPLETE : 0;
    }
    else {
        return errno;
    }
}

int ch_file_info_get(ch_finfo_t *finfo, 
                                            int32_t wanted,
                                            ch_file_t *thefile)
{
    struct_stat info;

    if (thefile->buffered) {
        int rv = ch_file_flush(thefile);
        if (rv != 0)
            return rv;
    }

    if (fstat(thefile->filedes, &info) == 0) {
        finfo->pool = thefile->pool;
        finfo->fname = thefile->fname;
        fill_out_finfo(finfo, &info, wanted);
        return (wanted & ~finfo->valid) ? CH_INCOMPLETE : 0;
    }
    else {
        return errno;
    }
}

int ch_file_perms_set(const char *fname, 
                                             int32_t perms)
{
    mode_t mode = ch_perms2mode(perms);

    if (chmod(fname, mode) == -1)
        return errno;
    return 0;
}

int ch_file_attrs_set(const char *fname,
                                             ch_fileattrs_t attributes,
                                             ch_fileattrs_t attr_mask,
                                             ch_pool_t *pool)
{
    int status;
    ch_finfo_t finfo;

    /* Don't do anything if we can't handle the requested attributes */
    if (!(attr_mask & (CH_FILE_ATTR_READONLY
                       | CH_FILE_ATTR_EXECUTABLE)))
        return 0;

    status = ch_stat(&finfo, fname, CH_FINFO_PROT, pool);
    if (status)
        return status;

    /* ### TODO: should added bits be umask'd? */
    if (attr_mask & CH_FILE_ATTR_READONLY)
    {
        if (attributes & CH_FILE_ATTR_READONLY)
        {
            finfo.protection &= ~CH_UWRITE;
            finfo.protection &= ~CH_GWRITE;
            finfo.protection &= ~CH_WWRITE;
        }
        else
        {
            /* ### umask this! */
            finfo.protection |= CH_UWRITE;
            finfo.protection |= CH_GWRITE;
            finfo.protection |= CH_WWRITE;
        }
    }

    if (attr_mask & CH_FILE_ATTR_EXECUTABLE)
    {
        if (attributes & CH_FILE_ATTR_EXECUTABLE)
        {
            /* ### umask this! */
            finfo.protection |= CH_UEXECUTE;
            finfo.protection |= CH_GEXECUTE;
            finfo.protection |= CH_WEXECUTE;
        }
        else
        {
            finfo.protection &= ~CH_UEXECUTE;
            finfo.protection &= ~CH_GEXECUTE;
            finfo.protection &= ~CH_WEXECUTE;
        }
    }

    return ch_file_perms_set(fname, finfo.protection);
}


int ch_file_mtime_set(const char *fname,
                                              ch_time_t mtime,
                                              ch_pool_t *pool)
{
    int status;
    ch_finfo_t finfo;

    status = ch_stat(&finfo, fname, CH_FINFO_ATIME, pool);
    if (status) {
        return status;
    }

    {
      struct timeval tvp[2];
    
      tvp[0].tv_sec = ch_time_sec(finfo.atime);
      tvp[0].tv_usec = ch_time_usec(finfo.atime);
      tvp[1].tv_sec = ch_time_sec(mtime);
      tvp[1].tv_usec = ch_time_usec(mtime);
      
      if (utimes(fname, tvp) == -1) {
        return errno;
      }
    }

    return 0;
}


int ch_stat(ch_finfo_t *finfo, 
                                   const char *fname, 
                                   int32_t wanted, ch_pool_t *pool)
{
    struct_stat info;
    int srv;

    if (wanted & CH_FINFO_LINK)
        srv = lstat(fname, &info);
    else
        srv = stat(fname, &info);

    if (srv == 0) {
        finfo->pool = pool;
        finfo->fname = fname;
        fill_out_finfo(finfo, &info, wanted);
        if (wanted & CH_FINFO_LINK)
            wanted &= ~CH_FINFO_LINK;
        return (wanted & ~finfo->valid) ? CH_INCOMPLETE : 0;
    }
    else {
#if !defined(ENOENT) || !defined(ENOTDIR)
#error ENOENT || ENOTDIR not defined; please see the
#error comments at this line in the source for a workaround.
        /*
         * If ENOENT || ENOTDIR is not defined in one of the your OS's
         * include files, GW cannot report a good reason why the stat()
         * of the file failed; there are cases where it can fail even though
         * the file exists.  This opens holes in Apache, for example, because
         * it becomes possible for someone to get a directory listing of a 
         * directory even though there is an index (eg. index.html) file in 
         * it.  If you do not have a problem with this, delete the above 
         * #error lines and start the compile again.  If you need to do this,
         * please submit a bug report to http://www.apache.org/bug_report.html
         * letting us know that you needed to do this.  Please be sure to 
         * include the operating system you are using.
         */
        /* WARNING: All errors will be handled as not found
         */
#if !defined(ENOENT) 
        return CH_ENOENT;
#else
        /* WARNING: All errors but not found will be handled as not directory
         */
        if (errno != ENOENT)
            return CH_ENOENT;
        else
            return errno;
#endif
#else /* All was defined well, report the usual: */
        return errno;
#endif
    }
}



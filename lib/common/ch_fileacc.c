/*
 *
 *      Filename: ch_fileacc.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 13:10:06
 * Last Modified: 2017-01-11 13:18:40
 */

#include "ch_file_info.h"

mode_t ch_perms2mode(int32_t perms)
{
    mode_t mode = 0;

    if (perms & CH_USETID)
        mode |= S_ISUID;
    if (perms & CH_UREAD)
        mode |= S_IRUSR;
    if (perms & CH_UWRITE)
        mode |= S_IWUSR;
    if (perms & CH_UEXECUTE)
        mode |= S_IXUSR;

    if (perms & CH_GSETID)
        mode |= S_ISGID;
    if (perms & CH_GREAD)
        mode |= S_IRGRP;
    if (perms & CH_GWRITE)
        mode |= S_IWGRP;
    if (perms & CH_GEXECUTE)
        mode |= S_IXGRP;

#ifdef S_ISVTX
    if (perms & CH_WSTICKY)
        mode |= S_ISVTX;
#endif
    if (perms & CH_WREAD)
        mode |= S_IROTH;
    if (perms & CH_WWRITE)
        mode |= S_IWOTH;
    if (perms & CH_WEXECUTE)
        mode |= S_IXOTH;

    return mode;
}

int32_t ch_mode2perms(mode_t mode)
{
    int32_t perms = 0;

    if (mode & S_ISUID)
        perms |= CH_USETID;
    if (mode & S_IRUSR)
        perms |= CH_UREAD;
    if (mode & S_IWUSR)
        perms |= CH_UWRITE;
    if (mode & S_IXUSR)
        perms |= CH_UEXECUTE;

    if (mode & S_ISGID)
        perms |= CH_GSETID;
    if (mode & S_IRGRP)
        perms |= CH_GREAD;
    if (mode & S_IWGRP)
        perms |= CH_GWRITE;
    if (mode & S_IXGRP)
        perms |= CH_GEXECUTE;

#ifdef S_ISVTX
    if (mode & S_ISVTX)
        perms |= CH_WSTICKY;
#endif
    if (mode & S_IROTH)
        perms |= CH_WREAD;
    if (mode & S_IWOTH)
        perms |= CH_WWRITE;
    if (mode & S_IXOTH)
        perms |= CH_WEXECUTE;

    return perms;
}



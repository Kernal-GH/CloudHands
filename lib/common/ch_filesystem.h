/*
 *
 *      Filename: ch_filesystem.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 16:54:03
 * Last Modified: 2017-02-10 16:54:03
 */

#ifndef CH_FILESYSTEM_H
#define CH_FILESYSTEM_H

#include "ch_constants.h"
/** String format for hugepage map files. */
#define HUGEFILE_FMT "%s/%smap_%d"
#define TEMP_HUGEFILE_FMT "%s/%smap_temp_%d"

static inline const char *
ch_get_hugefile_path(char *buffer, size_t buflen, const char *hugedir, const char *hugefile_prefix,int f_id)
{
	snprintf(buffer, buflen, HUGEFILE_FMT, hugedir,
			hugefile_prefix, f_id);

	buffer[buflen - 1] = '\0';
	return buffer;
}

/** define the default filename prefix for the %s values above */
#define HUGEFILE_PREFIX_DEFAULT "ch_"

/** Function to read a single numeric value from a file on the filesystem.
 * Used to read information from files on /sys */
int ch_parse_sysfs_value(const char *filename, unsigned long *val);

#endif /*CH_FILESYSTEM_H */

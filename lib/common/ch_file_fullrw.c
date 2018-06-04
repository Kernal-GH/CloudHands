/*
 *
 *      Filename: ch_file_fullrw.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:58:43
 * Last Modified: 2017-01-11 11:03:32
 */

#include "ch_file.h"


int ch_file_read_full(ch_file_t *thefile, void *buf,
                                             size_t nbytes,
                                             size_t *bytes_read)
{
    int status;
    size_t total_read = 0;

    do {
	size_t amt = nbytes;

	status = ch_file_read(thefile, buf, &amt);
	buf = (char *)buf + amt;
        nbytes -= amt;
        total_read += amt;
    } while (status == 0 && nbytes > 0);

    if (bytes_read != NULL)
        *bytes_read = total_read;

    return status;
}

int ch_file_write_full(ch_file_t *thefile,
                                              const void *buf,
                                              size_t nbytes,
                                              size_t *bytes_written)
{
    int status;
    size_t total_written = 0;

    do {
	size_t amt = nbytes;

	status = ch_file_write(thefile, buf, &amt);
	buf = (char *)buf + amt;
        nbytes -= amt;
        total_written += amt;
    } while (status == 0 && nbytes > 0);

    if (bytes_written != NULL)
        *bytes_written = total_written;

    return status;
}

int ch_file_writev_full(ch_file_t *thefile,
                                               const struct iovec *vec,
                                               size_t nvec,
                                               size_t *bytes_written)
{
    int rv = 0;
    size_t i;
    size_t amt = 0;
    size_t total = 0;

    for (i = 0; i < nvec; i++) {
        total += vec[i].iov_len;
    }

    rv = ch_file_writev(thefile, vec, nvec, &amt);

    if (bytes_written != NULL)
        *bytes_written = amt;

    if (rv != 0 || (amt == total)) {
        return rv;
    }

    for (i = 0; i < nvec && amt; i++) {
        if (amt >= vec[i].iov_len) {
            amt -= vec[i].iov_len;
        }
        else {
            break;
        }
    }

    if (amt) {
        rv = ch_file_write_full(thefile, (const char *)vec[i].iov_base + amt,
                                 vec[i].iov_len - amt, NULL);
    }

    for (; i < nvec && rv == 0; i++) {
        rv = ch_file_write_full(thefile, vec[i].iov_base,
                                 vec[i].iov_len, &amt);
    }

    if (bytes_written != NULL)
        *bytes_written = total;

    return rv;
}

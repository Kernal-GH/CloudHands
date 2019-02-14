/*
 *
 *      Filename: ch_file.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 09:24:06
 * Last Modified: 2017-01-11 09:24:06
 */

#ifndef CH_FILE_H
#define CH_FILE_H

#include "ch_mpool.h"
#include "ch_constants.h"
#include "ch_string.h"

/** File attributes */
typedef uint32_t ch_fileattrs_t;

/** Type to pass as whence argument to ch_file_seek. */
typedef int       ch_seek_where_t;

typedef struct stat struct_stat;

/**
 * Structure for referencing files.
 */
typedef struct ch_file_t         ch_file_t;


#include "ch_file_info.h"

#define CH_FILE_DEFAULT_BUFSIZE 4096
/* For backwards-compat */
#define CH_FILE_BUFSIZE  CH_FILE_DEFAULT_BUFSIZE

/* Note to implementors: Values in the range 0x00100000--0x80000000
   are reserved for platform-specific values. */

#define CH_FOPEN_READ       0x00001  /**< Open the file for reading */
#define CH_FOPEN_WRITE      0x00002  /**< Open the file for writing */
#define CH_FOPEN_CREATE     0x00004  /**< Create the file if not there */
#define CH_FOPEN_APPEND     0x00008  /**< Append to the end of the file */
#define CH_FOPEN_TRUNCATE   0x00010  /**< Open the file and truncate
                                         to 0 length */
#define CH_FOPEN_BINARY     0x00020  /**< Open the file in binary mode
				         (This flag is ignored on UNIX 
					 because it has no meaning)*/
#define CH_FOPEN_EXCL       0x00040  /**< Open should fail if #CH_FOPEN_CREATE
                                         and file exists. */
#define CH_FOPEN_BUFFERED   0x00080  /**< Open the file for buffered I/O */
#define CH_FOPEN_DELONCLOSE 0x00100  /**< Delete the file after close */
#define CH_FOPEN_XTHREAD    0x00200  /**< Platform dependent tag to open
                                         the file for use across multiple
                                         threads */
#define CH_FOPEN_SHARELOCK  0x00400  /**< Platform dependent support for
                                         higher level locked read/write
                                         access to support writes across
                                         process/machines */
#define CH_FOPEN_NOCLEANUP  0x00800  /**< Do not register a cleanup
                                         when the file is opened. The
					 ch_os_file_t handle in ch_file_t
					 will not be closed when the pool
					 is destroyed. */
#define CH_FOPEN_SENDFILE_ENABLED 0x01000 /**< Advisory flag that this
                                             file should support
                                             ch_socket_sendfile operation */
#define CH_FOPEN_LARGEFILE   0x04000 /**< Platform dependent flag to enable
                                       * large file support, see WARNING below
                                       */
#define CH_FOPEN_SPARSE      0x08000 /**< Platform dependent flag to enable
                                       * sparse file support, see WARNING below
                                       */
#define CH_FOPEN_NONBLOCK    0x40000 /**< Platform dependent flag to enable
                                       * non blocking file io */


/* backcompat */
#define CH_READ             CH_FOPEN_READ       /**< @deprecated @see CH_FOPEN_READ */
#define CH_WRITE            CH_FOPEN_WRITE      /**< @deprecated @see CH_FOPEN_WRITE */   
#define CH_CREATE           CH_FOPEN_CREATE     /**< @deprecated @see CH_FOPEN_CREATE */   
#define CH_APPEND           CH_FOPEN_APPEND     /**< @deprecated @see CH_FOPEN_APPEND */   
#define CH_TRUNCATE         CH_FOPEN_TRUNCATE   /**< @deprecated @see CH_FOPEN_TRUNCATE */   
#define CH_BINARY           CH_FOPEN_BINARY     /**< @deprecated @see CH_FOPEN_BINARY */   
#define CH_EXCL             CH_FOPEN_EXCL       /**< @deprecated @see CH_FOPEN_EXCL */   
#define CH_BUFFERED         CH_FOPEN_BUFFERED   /**< @deprecated @see CH_FOPEN_BUFFERED */   
#define CH_DELONCLOSE       CH_FOPEN_DELONCLOSE /**< @deprecated @see CH_FOPEN_DELONCLOSE */   
#define CH_XTHREAD          CH_FOPEN_XTHREAD    /**< @deprecated @see CH_FOPEN_XTHREAD */   
#define CH_SHARELOCK        CH_FOPEN_SHARELOCK  /**< @deprecated @see CH_FOPEN_SHARELOCK */   
#define CH_FILE_NOCLEANUP   CH_FOPEN_NOCLEANUP  /**< @deprecated @see CH_FOPEN_NOCLEANUP */   
#define CH_SENDFILE_ENABLED CH_FOPEN_SENDFILE_ENABLED /**< @deprecated @see CH_FOPEN_SENDFILE_ENABLED */   
#define CH_LARGEFILE        CH_FOPEN_LARGEFILE  /**< @deprecated @see CH_FOPEN_LARGEFILE */   

/** @def CH_FOPEN_LARGEFILE 
 * @warning CH_FOPEN_LARGEFILE flag only has effect on some
 * platforms where sizeof(off_t) == 4.  Where implemented, it
 * allows opening and writing to a file which exceeds the size which
 * can be represented by off_t (2 gigabytes).  When a file's size
 * does exceed 2Gb, ch_file_info_get() will fail with an error on the
 * descriptor, likewise ch_stat()/ch_lstat() will fail on the
 * filename.  ch_dir_read() will fail with #CH_INCOMPLETE on a
 * directory entry for a large file depending on the particular
 * CH_FINFO_* flags.  Generally, it is not recommended to use this
 * flag.
 *
 * @def CH_FOPEN_SPARSE
 * @warning CH_FOPEN_SPARSE may, depending on platform, convert a
 * normal file to a sparse file.  Some applications may be unable
 * to decipher a sparse file, so it's critical that the sparse file
 * flag should only be used for files accessed only by GW or other
 * applications known to be able to decipher them.  GW does not
 * guarantee that it will compress the file into sparse segments
 * if it was previously created and written without the sparse flag.
 * On platforms which do not understand, or on file systems which
 * cannot handle sparse files, the flag is ignored by ch_file_open().
 *
 * @def CH_FOPEN_NONBLOCK
 * @warning CH_FOPEN_NONBLOCK is not implemented on all platforms.
 * Callers should be prepared for it to fail with #CH_ENOTIMPL.
 */

/** @} */

/**
 * @defgroup ch_file_seek_flags File Seek Flags
 * @{
 */

/* flags for ch_file_seek */
/** Set the file position */
#define CH_SET SEEK_SET
/** Current */
#define CH_CUR SEEK_CUR
/** Go to end of file */
#define CH_END SEEK_END
/** @} */

/**
 * @defgroup ch_file_attrs_set_flags File Attribute Flags
 * @{
 */

/* flags for ch_file_attrs_set */
#define CH_FILE_ATTR_READONLY   0x01          /**< File is read-only */
#define CH_FILE_ATTR_EXECUTABLE 0x02          /**< File is executable */
#define CH_FILE_ATTR_HIDDEN     0x04          /**< File is hidden */
/** @} */

/**
 * @defgroup ch_file_writev{_full} max iovec size
 * @{
 */
#if defined(DOXYGEN)
#define CH_MAX_IOVEC_SIZE 1024                /**< System dependent maximum 
                                                    size of an iovec array */
#elif defined(IOV_MAX)
#define CH_MAX_IOVEC_SIZE IOV_MAX
#elif defined(MAX_IOVEC)
#define CH_MAX_IOVEC_SIZE MAX_IOVEC
#else
#define CH_MAX_IOVEC_SIZE 1024
#endif
/** @} */


/* File lock types/flags */
/**
 * @defgroup ch_file_lock_types File Lock Types
 * @{
 */

#define CH_FLOCK_SHARED        1       /**< Shared lock. More than one process
                                           or thread can hold a shared lock
                                           at any given time. Essentially,
                                           this is a "read lock", preventing
                                           writers from establishing an
                                           exclusive lock. */
#define CH_FLOCK_EXCLUSIVE     2       /**< Exclusive lock. Only one process
                                           may hold an exclusive lock at any
                                           given time. This is analogous to
                                           a "write lock". */

#define CH_FLOCK_TYPEMASK      0x000F  /**< mask to extract lock type */
#define CH_FLOCK_NONBLOCK      0x0010  /**< do not block while acquiring the
                                           file lock */

struct ch_file_t {
    ch_pool_t *pool;
    int filedes;
    char *fname;
    int32_t flags;
    int eof_hit;
    int is_pipe;
    int64_t timeout;
    int buffered;
    enum {BLK_UNKNOWN, BLK_OFF, BLK_ON } blocking;
    int ungetchar;    /* Last char provided by an unget op. (-1 = no char)*/
    /* Stuff for buffered mode */
    char *buffer;
    size_t bufpos;        /* Read/Write position in buffer */
    size_t bufsize;       /* The size of the buffer */
    unsigned long dataRead;   /* amount of valid data read into buffer */
    int direction;            /* buffer being used for 0 = read, 1 = write */
    off_t filePtr;        /* position in file of handle */
};

/** @} */

/**
 * Open the specified file.
 * @param newf The opened file descriptor.
 * @param fname The full path to the file (using / on all systems)
 * @param flag Or'ed value of:
 * @li #CH_FOPEN_READ           open for reading
 * @li #CH_FOPEN_WRITE          open for writing
 * @li #CH_FOPEN_CREATE         create the file if not there
 * @li #CH_FOPEN_APPEND         file ptr is set to end prior to all writes
 * @li #CH_FOPEN_TRUNCATE       set length to zero if file exists
 * @li #CH_FOPEN_BINARY         not a text file
 * @li #CH_FOPEN_BUFFERED       buffer the data.  Default is non-buffered
 * @li #CH_FOPEN_EXCL           return error if #CH_FOPEN_CREATE and file exists
 * @li #CH_FOPEN_DELONCLOSE     delete the file after closing
 * @li #CH_FOPEN_XTHREAD        Platform dependent tag to open the file
 *                               for use across multiple threads
 * @li #CH_FOPEN_SHARELOCK      Platform dependent support for higher
 *                               level locked read/write access to support
 *                               writes across process/machines
 * @li #CH_FOPEN_NOCLEANUP      Do not register a cleanup with the pool 
 *                               passed in on the @a pool argument (see below)
 * @li #CH_FOPEN_SENDFILE_ENABLED  Open with appropriate platform semantics
 *                               for sendfile operations.  Advisory only,
 *                               ch_socket_sendfile does not check this flag
 * @li #CH_FOPEN_LARGEFILE      Platform dependent flag to enable large file
 *                               support, see WARNING below 
 * @li #CH_FOPEN_SPARSE         Platform dependent flag to enable sparse file
 *                               support, see WARNING below
 * @li #CH_FOPEN_NONBLOCK       Platform dependent flag to enable
 *                               non blocking file io
 * @param perm Access permissions for file.
 * @param pool The pool to use.
 * @remark If perm is #CH_FPROT_OS_DEFAULT and the file is being created,
 * appropriate default permissions will be used.
 * @remark By default, the returned file descriptor will not be
 * inherited by child processes created by ch_proc_create().  This
 * can be changed using ch_file_inherit_set().
 */
int ch_file_open(ch_file_t **newf, const char *fname,
                                        int32_t flag, int32_t perm,
                                        ch_pool_t *pool);

/**
 * Close the specified file.
 * @param file The file descriptor to close.
 */
int ch_file_close(ch_file_t *file);

/**
 * Delete the specified file.
 * @param path The full path to the file (using / on all systems)
 * @param pool The pool to use.
 * @remark If the file is open, it won't be removed until all
 * instances are closed.
 */
int ch_file_remove(const char *path, ch_pool_t *pool);

/**
 * Rename the specified file.
 * @param from_path The full path to the original file (using / on all systems)
 * @param to_path The full path to the new file (using / on all systems)
 * @param pool The pool to use.
 * @warning If a file exists at the new location, then it will be
 * overwritten.  Moving files or directories across devices may not be
 * possible.
 */
int ch_file_rename(const char *from_path, 
                                          const char *to_path,
                                          ch_pool_t *pool);

/**
 * Create a hard link to the specified file.
 * @param from_path The full path to the original file (using / on all systems)
 * @param to_path The full path to the new file (using / on all systems)
 * @remark Both files must reside on the same device.
 */
int ch_file_link(const char *from_path, 
                                          const char *to_path);

/**
 * Copy the specified file to another file.
 * @param from_path The full path to the original file (using / on all systems)
 * @param to_path The full path to the new file (using / on all systems)
 * @param perms Access permissions for the new file if it is created.
 *     In place of the usual or'd combination of file permissions, the
 *     value #CH_FPROT_FILE_SOURCE_PERMS may be given, in which case the source
 *     file's permissions are copied.
 * @param pool The pool to use.
 * @remark The new file does not need to exist, it will be created if required.
 * @warning If the new file already exists, its contents will be overwritten.
 */
int ch_file_copy(const char *from_path, 
                                        const char *to_path,
                                        int32_t perms,
                                        ch_pool_t *pool);

/**
 * Append the specified file to another file.
 * @param from_path The full path to the source file (use / on all systems)
 * @param to_path The full path to the destination file (use / on all systems)
 * @param perms Access permissions for the destination file if it is created.
 *     In place of the usual or'd combination of file permissions, the
 *     value #CH_FPROT_FILE_SOURCE_PERMS may be given, in which case the source
 *     file's permissions are copied.
 * @param pool The pool to use.
 * @remark The new file does not need to exist, it will be created if required.
 */
int ch_file_append(const char *from_path, 
                                          const char *to_path,
                                          int32_t perms,
                                          ch_pool_t *pool);

/**
 * Are we at the end of the file
 * @param fptr The gw file we are testing.
 * @remark Returns #CH_EOF if we are at the end of file, #CH_SUCCESS otherwise.
 */
int ch_file_eof(ch_file_t *fptr);

/**
 * Open standard error as an gw file pointer.
 * @param thefile The gw file to use as stderr.
 * @param pool The pool to allocate the file out of.
 * 
 * @remark The only reason that the ch_file_open_std* functions exist
 * is that you may not always have a stderr/out/in on Windows.  This
 * is generally a problem with newer versions of Windows and services.
 * 
 * @remark The other problem is that the C library functions generally work
 * differently on Windows and Unix.  So, by using ch_file_open_std*
 * functions, you can get a handle to an GW struct that works with
 * the GW functions which are supposed to work identically on all
 * platforms.
 */
int ch_file_open_stderr(ch_file_t **thefile,
                                               ch_pool_t *pool);

/**
 * open standard output as an gw file pointer.
 * @param thefile The gw file to use as stdout.
 * @param pool The pool to allocate the file out of.
 * 
 * @remark See remarks for ch_file_open_stderr().
 */
int ch_file_open_stdout(ch_file_t **thefile,
                                               ch_pool_t *pool);

/**
 * open standard input as an gw file pointer.
 * @param thefile The gw file to use as stdin.
 * @param pool The pool to allocate the file out of.
 * 
 * @remark See remarks for ch_file_open_stderr().
 */
int ch_file_open_stdin(ch_file_t **thefile,
                                              ch_pool_t *pool);

/**
 * open standard error as an gw file pointer, with flags.
 * @param thefile The gw file to use as stderr.
 * @param flags The flags to open the file with. Only the 
 *              @li #CH_FOPEN_EXCL
 *              @li #CH_FOPEN_BUFFERED
 *              @li #CH_FOPEN_XTHREAD
 *              @li #CH_FOPEN_SHARELOCK 
 *              @li #CH_FOPEN_SENDFILE_ENABLED
 *              @li #CH_FOPEN_LARGEFILE
 *
 *              flags should be used. The #CH_FOPEN_WRITE flag will
 *              be set unconditionally.
 * @param pool The pool to allocate the file out of.
 * 
 * @remark See remarks for ch_file_open_stderr().
 */
int ch_file_open_flags_stderr(ch_file_t **thefile,
                                                     int32_t flags,
                                                     ch_pool_t *pool);

/**
 * open standard output as an gw file pointer, with flags.
 * @param thefile The gw file to use as stdout.
 * @param flags The flags to open the file with. Only the 
 *              @li #CH_FOPEN_EXCL
 *              @li #CH_FOPEN_BUFFERED
 *              @li #CH_FOPEN_XTHREAD
 *              @li #CH_FOPEN_SHARELOCK 
 *              @li #CH_FOPEN_SENDFILE_ENABLED
 *              @li #CH_FOPEN_LARGEFILE
 *
 *              flags should be used. The #CH_FOPEN_WRITE flag will
 *              be set unconditionally.
 * @param pool The pool to allocate the file out of.
 * 
 * @remark See remarks for ch_file_open_stderr().
 */
int ch_file_open_flags_stdout(ch_file_t **thefile,
                                                     int32_t flags,
                                                     ch_pool_t *pool);

/**
 * open standard input as an gw file pointer, with flags.
 * @param thefile The gw file to use as stdin.
 * @param flags The flags to open the file with. Only the 
 *              @li #CH_FOPEN_EXCL
 *              @li #CH_FOPEN_BUFFERED
 *              @li #CH_FOPEN_XTHREAD
 *              @li #CH_FOPEN_SHARELOCK 
 *              @li #CH_FOPEN_SENDFILE_ENABLED
 *              @li #CH_FOPEN_LARGEFILE
 *
 *              flags should be used. The #CH_FOPEN_WRITE flag will
 *              be set unconditionally.
 * @param pool The pool to allocate the file out of.
 * 
 * @remark See remarks for ch_file_open_stderr().
 */
int ch_file_open_flags_stdin(ch_file_t **thefile,
                                                     int32_t flags,
                                                     ch_pool_t *pool);

/**
 * Read data from the specified file.
 * @param thefile The file descriptor to read from.
 * @param buf The buffer to store the data to.
 * @param nbytes On entry, the number of bytes to read; on exit, the number
 * of bytes read.
 *
 * @remark ch_file_read() will read up to the specified number of
 * bytes, but never more.  If there isn't enough data to fill that
 * number of bytes, all of the available data is read.  The third
 * argument is modified to reflect the number of bytes read.  If a
 * char was put back into the stream via ungetc, it will be the first
 * character returned.
 *
 * @remark It is not possible for both bytes to be read and an #CH_EOF
 * or other error to be returned.  #CH_EINTR is never returned.
 */
int ch_file_read(ch_file_t *thefile, void *buf,
                                        size_t *nbytes);

/**
 * Write data to the specified file.
 * @param thefile The file descriptor to write to.
 * @param buf The buffer which contains the data.
 * @param nbytes On entry, the number of bytes to write; on exit, the number 
 *               of bytes written.
 *
 * @remark ch_file_write() will write up to the specified number of
 * bytes, but never more.  If the OS cannot write that many bytes, it
 * will write as many as it can.  The third argument is modified to
 * reflect the * number of bytes written.
 *
 * @remark It is possible for both bytes to be written and an error to
 * be returned.  #CH_EINTR is never returned.
 */
int ch_file_write(ch_file_t *thefile, const void *buf,
                                         size_t *nbytes);

/**
 * Write data from iovec array to the specified file.
 * @param thefile The file descriptor to write to.
 * @param vec The array from which to get the data to write to the file.
 * @param nvec The number of elements in the struct iovec array. This must 
 *             be smaller than #CH_MAX_IOVEC_SIZE.  If it isn't, the function 
 *             will fail with #CH_EINVAL.
 * @param nbytes The number of bytes written.
 *
 * @remark It is possible for both bytes to be written and an error to
 * be returned.  #CH_EINTR is never returned.
 *
 * @remark ch_file_writev() is available even if the underlying
 * operating system doesn't provide writev().
 */
int ch_file_writev(ch_file_t *thefile,
                                          const struct iovec *vec,
                                          size_t nvec, size_t *nbytes);

/**
 * Read data from the specified file, ensuring that the buffer is filled
 * before returning.
 * @param thefile The file descriptor to read from.
 * @param buf The buffer to store the data to.
 * @param nbytes The number of bytes to read.
 * @param bytes_read If non-NULL, this will contain the number of bytes read.
 *
 * @remark ch_file_read_full() will read up to the specified number of
 * bytes, but never more.  If there isn't enough data to fill that
 * number of bytes, then the process/thread will block until it is
 * available or EOF is reached.  If a char was put back into the
 * stream via ungetc, it will be the first character returned.
 *
 * @remark It is possible for both bytes to be read and an error to be
 * returned.  And if *bytes_read is less than nbytes, an accompanying
 * error is _always_ returned.
 *
 * @remark #CH_EINTR is never returned.
 */
int ch_file_read_full(ch_file_t *thefile, void *buf,
                                             size_t nbytes,
                                             size_t *bytes_read);

/**
 * Write data to the specified file, ensuring that all of the data is
 * written before returning.
 * @param thefile The file descriptor to write to.
 * @param buf The buffer which contains the data.
 * @param nbytes The number of bytes to write.
 * @param bytes_written If non-NULL, set to the number of bytes written.
 * 
 * @remark ch_file_write_full() will write up to the specified number of
 * bytes, but never more.  If the OS cannot write that many bytes, the
 * process/thread will block until they can be written. Exceptional
 * error such as "out of space" or "pipe closed" will terminate with
 * an error.
 *
 * @remark It is possible for both bytes to be written and an error to
 * be returned.  And if *bytes_written is less than nbytes, an
 * accompanying error is _always_ returned.
 *
 * @remark #CH_EINTR is never returned.
 */
int ch_file_write_full(ch_file_t *thefile, 
                                              const void *buf,
                                              size_t nbytes, 
                                              size_t *bytes_written);


/**
 * Write data from iovec array to the specified file, ensuring that all of the
 * data is written before returning.
 * @param thefile The file descriptor to write to.
 * @param vec The array from which to get the data to write to the file.
 * @param nvec The number of elements in the struct iovec array. This must 
 *             be smaller than #CH_MAX_IOVEC_SIZE.  If it isn't, the function 
 *             will fail with #CH_EINVAL.
 * @param nbytes The number of bytes written.
 *
 * @remark ch_file_writev_full() is available even if the underlying
 * operating system doesn't provide writev().
 */
int ch_file_writev_full(ch_file_t *thefile,
                                               const struct iovec *vec,
                                               size_t nvec,
                                               size_t *nbytes);
/**
 * Write a character into the specified file.
 * @param ch The character to write.
 * @param thefile The file descriptor to write to
 */
int ch_file_putc(char ch, ch_file_t *thefile);

/**
 * Read a character from the specified file.
 * @param ch The character to read into
 * @param thefile The file descriptor to read from
 */
int ch_file_getc(char *ch, ch_file_t *thefile);

/**
 * Put a character back onto a specified stream.
 * @param ch The character to write.
 * @param thefile The file descriptor to write to
 */
int ch_file_ungetc(char ch, ch_file_t *thefile);

/**
 * Read a line from the specified file
 * @param str The buffer to store the string in. 
 * @param len The length of the string
 * @param thefile The file descriptor to read from
 * @remark The buffer will be NUL-terminated if any characters are stored.
 *         The newline at the end of the line will not be stripped.
 */
int ch_file_gets(char *str, int len, 
                                        ch_file_t *thefile);

/**
 * Write the string into the specified file.
 * @param str The string to write. 
 * @param thefile The file descriptor to write to
 */
int ch_file_puts(const char *str, ch_file_t *thefile);

/**
 * Flush the file's buffer.
 * @param thefile The file descriptor to flush
 */
int ch_file_flush(ch_file_t *thefile);

/**
 * Transfer all file modified data and metadata to disk.
 * @param thefile The file descriptor to sync
 */
int ch_file_sync(ch_file_t *thefile);

/**
 * Transfer all file modified data to disk.
 * @param thefile The file descriptor to sync
 */
int ch_file_datasync(ch_file_t *thefile);

/**
 * Duplicate the specified file descriptor.
 * @param new_file The structure to duplicate into. 
 * @param old_file The file to duplicate.
 * @param p The pool to use for the new file.
 * @remark *new_file must point to a valid ch_file_t, or point to NULL.
 */         
int ch_file_dup(ch_file_t **new_file,
                                       ch_file_t *old_file,
                                       ch_pool_t *p);

/**
 * Duplicate the specified file descriptor and close the original
 * @param new_file The old file that is to be closed and reused
 * @param old_file The file to duplicate
 * @param p        The pool to use for the new file
 *
 * @remark new_file MUST point at a valid ch_file_t. It cannot be NULL.
 */
int ch_file_dup2(ch_file_t *new_file,
                                        ch_file_t *old_file,
                                        ch_pool_t *p);

/**
 * Move the specified file descriptor to a new pool
 * @param new_file Pointer in which to return the new ch_file_t
 * @param old_file The file to move
 * @param p        The pool to which the descriptor is to be moved
 * @remark Unlike ch_file_dup2(), this function doesn't do an
 *         OS dup() operation on the underlying descriptor; it just
 *         moves the descriptor's ch_file_t wrapper to a new pool.
 * @remark The new pool need not be an ancestor of old_file's pool.
 * @remark After calling this function, old_file may not be used
 */
int ch_file_setaside(ch_file_t **new_file,
                                            ch_file_t *old_file,
                                            ch_pool_t *p);

/**
 * Give the specified gw file handle a new buffer 
 * @param thefile  The file handle that is to be modified
 * @param buffer   The buffer
 * @param bufsize  The size of the buffer
 * @remark It is possible to add a buffer to previously unbuffered
 *         file handles, the #CH_FOPEN_BUFFERED flag will be added to
 *         the file handle's flags. Likewise, with buffer=NULL and
 *         bufsize=0 arguments it is possible to make a previously
 *         buffered file handle unbuffered.
 */
int ch_file_buffer_set(ch_file_t *thefile,
                                              char * buffer,
                                              size_t bufsize);

/**
 * Get the size of any buffer for the specified gw file handle 
 * @param thefile  The file handle 
 */
size_t ch_file_buffer_size_get(ch_file_t *thefile);

/**
 * Move the read/write file offset to a specified byte within a file.
 * @param thefile The file descriptor
 * @param where How to move the pointer, one of:
 *              @li #CH_SET  --  set the offset to offset
 *              @li #CH_CUR  --  add the offset to the current position 
 *              @li #CH_END  --  add the offset to the current file size 
 * @param offset The offset to move the pointer to.
 * @remark The third argument is modified to be the offset the pointer
          was actually moved to.
 */
int ch_file_seek(ch_file_t *thefile, 
                                   ch_seek_where_t where,
                                   off_t *offset);

/**
 * Create an anonymous pipe.
 * @param in The newly created pipe's file for reading.
 * @param out The newly created pipe's file for writing.
 * @param pool The pool to operate on.
 * @remark By default, the returned file descriptors will be inherited
 * by child processes created using ch_proc_create().  This can be
 * changed using ch_file_inherit_unset().
 * @bug  Some platforms cannot toggle between blocking and nonblocking,
 * and when passing a pipe as a standard handle to an application which
 * does not expect it, a non-blocking stream will fluxor the client app.
 * @deprecated @see ch_file_pipe_create_ex()
 */
int ch_file_pipe_create(ch_file_t **in, 
                                               ch_file_t **out,
                                               ch_pool_t *pool);

/**
 * Create an anonymous pipe which portably supports async timeout options.
 * @param in The newly created pipe's file for reading.
 * @param out The newly created pipe's file for writing.
 * @param blocking one of these values defined in ch_thread_proc.h;
 *                 @li #CH_FULL_BLOCK
 *                 @li #CH_READ_BLOCK
 *                 @li #CH_WRITE_BLOCK
 *                 @li #CH_FULL_NONBLOCK
 * @param pool The pool to operate on.
 * @remark By default, the returned file descriptors will be inherited
 * by child processes created using ch_proc_create().  This can be
 * changed using ch_file_inherit_unset().
 * @remark Some platforms cannot toggle between blocking and nonblocking,
 * and when passing a pipe as a standard handle to an application which
 * does not expect it, a non-blocking stream will fluxor the client app.
 * Use this function rather than ch_file_pipe_create() to create pipes 
 * where one or both ends require non-blocking semantics.
 */
int ch_file_pipe_create_ex(ch_file_t **in, 
                                                  ch_file_t **out, 
                                                  int32_t blocking, 
                                                  ch_pool_t *pool);

/**
 * Create a named pipe.
 * @param filename The filename of the named pipe
 * @param perm The permissions for the newly created pipe.
 * @param pool The pool to operate on.
 */
int ch_file_namedpipe_create(const char *filename, 
                                                    int32_t perm, 
                                                    ch_pool_t *pool);

/**
 * Get the timeout value for a pipe or manipulate the blocking state.
 * @param thepipe The pipe we are getting a timeout for.
 * @param timeout The current timeout value in microseconds. 
 */
int ch_file_pipe_timeout_get(ch_file_t *thepipe, 
                                               int64_t *timeout);

/**
 * Set the timeout value for a pipe or manipulate the blocking state.
 * @param thepipe The pipe we are setting a timeout on.
 * @param timeout The timeout value in microseconds.  Values < 0 mean wait 
 *        forever, 0 means do not wait at all.
 */
int ch_file_pipe_timeout_set(ch_file_t *thepipe, 
                                                  int64_t timeout);

/** file (un)locking functions. */

/**
 * Establish a lock on the specified, open file. The lock may be advisory
 * or mandatory, at the discretion of the platform. The lock applies to
 * the file as a whole, rather than a specific range. Locks are established
 * on a per-thread/process basis; a second lock by the same thread will not
 * block.
 * @param thefile The file to lock.
 * @param type The type of lock to establish on the file.
 */
int ch_file_lock(ch_file_t *thefile, int type);

/**
 * Remove any outstanding locks on the file.
 * @param thefile The file to unlock.
 */
int ch_file_unlock(ch_file_t *thefile);

/**accessor and general file_io functions. */

/**
 * return the file name of the current file.
 * @param new_path The path of the file.  
 * @param thefile The currently open file.
 */                     
int ch_file_name_get(const char **new_path, 
                                            ch_file_t *thefile);
    
/**
 * Return the data associated with the current file.
 * @param data The user data associated with the file.  
 * @param key The key to use for retrieving data associated with this file.
 * @param file The currently open file.
 */                     
int ch_file_data_get(void **data, const char *key, 
                                            ch_file_t *file);

/**
 * Set the data associated with the current file.
 * @param file The currently open file.
 * @param data The user data to associate with the file.  
 * @param key The key to use for associating data with the file.
 * @param cleanup The cleanup routine to use when the file is destroyed.
 */                     
int ch_file_data_set(ch_file_t *file, void *data,
                                            const char *key,
                                            int (*cleanup)(void *));

/**
 * Write a string to a file using a printf format.
 * @param fptr The file to write to.
 * @param format The format string
 * @param ... The values to substitute in the format string
 * @return The number of bytes written
 */ 
int ch_file_printf(ch_file_t *fptr, 
                                        const char *format, ...)
        __attribute__((format(printf,2,3)));

/**
 * set the specified file's permission bits.
 * @param fname The file (name) to apply the permissions to.
 * @param perms The permission bits to apply to the file.
 *
 * @warning Some platforms may not be able to apply all of the
 * available permission bits; #CH_INCOMPLETE will be returned if some
 * permissions are specified which could not be set.
 *
 * @warning Platforms which do not implement this feature will return
 * #CH_ENOTIMPL.
 */
int ch_file_perms_set(const char *fname,
                                             int32_t perms);

/**
 * Set attributes of the specified file.
 * @param fname The full path to the file (using / on all systems)
 * @param attributes Or'd combination of
 *            @li #CH_FILE_ATTR_READONLY   - make the file readonly
 *            @li #CH_FILE_ATTR_EXECUTABLE - make the file executable
 *            @li #CH_FILE_ATTR_HIDDEN     - make the file hidden
 * @param attr_mask Mask of valid bits in attributes.
 * @param pool the pool to use.
 * @remark This function should be used in preference to explicit manipulation
 *      of the file permissions, because the operations to provide these
 *      attributes are platform specific and may involve more than simply
 *      setting permission bits.
 * @warning Platforms which do not implement this feature will return
 *      #CH_ENOTIMPL.
 */
int ch_file_attrs_set(const char *fname,
                                             ch_fileattrs_t attributes,
                                             ch_fileattrs_t attr_mask,
                                             ch_pool_t *pool);



/**
 * Create a new directory on the file system.
 * @param path the path for the directory to be created. (use / on all systems)
 * @param perm Permissions for the new directory.
 * @param pool the pool to use.
 */                        
int ch_dir_make(const char *path, int32_t perm, 
                                       ch_pool_t *pool);

/** Creates a new directory on the file system, but behaves like
 * 'mkdir -p'. Creates intermediate directories as required. No error
 * will be reported if PATH already exists.
 * @param path the path for the directory to be created. (use / on all systems)
 * @param perm Permissions for the new directory.
 * @param pool the pool to use.
 */
int ch_dir_make_recursive(const char *path,
                                                 int32_t perm,
                                                 ch_pool_t *pool);

/**
 * Remove directory from the file system.
 * @param path the path for the directory to be removed. (use / on all systems)
 * @param pool the pool to use.
 * @remark Removing a directory which is in-use (e.g., the current working
 * directory, or during ch_dir_read, or with an open file) is not portable.
 */                        
int ch_dir_remove(const char *path, ch_pool_t *pool);

/**
 * get the specified file's stats.
 * @param finfo Where to store the information about the file.
 * @param wanted The desired ch_finfo_t fields, as a bit flag of CH_FINFO_* values 
 * @param thefile The file to get information about.
 */ 
int ch_file_info_get(ch_finfo_t *finfo, 
                                            int32_t wanted,
                                            ch_file_t *thefile);
    

/**
 * Truncate the file's length to the specified offset
 * @param fp The file to truncate
 * @param offset The offset to truncate to.
 * @remark The read/write file offset is repositioned to offset.
 */
int ch_file_trunc(ch_file_t *fp, off_t offset);

/**
 * Retrieve the flags that were passed into ch_file_open()
 * when the file was opened.
 * @return int32_t the flags
 */
int32_t ch_file_flags_get(ch_file_t *f);

/**
 * Open a temporary file
 * @param fp The gw file to use as a temporary file.
 * @param templ The template to use when creating a temp file.
 * @param flags The flags to open the file with. If this is zero,
 *              the file is opened with 
 *              #CH_FOPEN_CREATE | #CH_FOPEN_READ | #CH_FOPEN_WRITE |
 *              #CH_FOPEN_EXCL | #CH_FOPEN_DELONCLOSE
 * @param p The pool to allocate the file out of.
 * @remark   
 * This function  generates  a unique temporary file name from template.  
 * The last six characters of template must be XXXXXX and these are replaced 
 * with a string that makes the filename unique. Since it will  be  modified,
 * template must not be a string constant, but should be declared as a character
 * array.  
 *
 */
int ch_file_mktemp(ch_file_t **fp, char *templ,
                                          int32_t flags, ch_pool_t *p);


/**
 * Find an existing directory suitable as a temporary storage location.
 * @param temp_dir The temp directory.
 * @param p The pool to use for any necessary allocations.
 * @remark   
 * This function uses an algorithm to search for a directory that an
 * an application can use for temporary storage.
 *
 */
int ch_temp_dir_get(const char **temp_dir, 
                                           ch_pool_t *p);

int ch_file_flush_locked(ch_file_t *thefile);


#endif /* CH_FILE_H */

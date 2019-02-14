/*
 *
 *      Filename: ch_file_info.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 11:10:18
 * Last Modified: 2017-01-11 11:10:18
 */

#ifndef CH_FILE_INFO_H
#define CH_FILE_INFO_H

#include "ch_tables.h"
/**
 * @defgroup ch_file_info File Information
 * @ingroup GW 
 * @{
 */

/* Many applications use the type member to determine the
 * existance of a file or initialization of the file info,
 * so the CH_NOFILE value must be distinct from CH_UNKFILE.
 */

/** ch_filetype_e values for the filetype member of the 
 * ch_file_info_t structure
 * @warning: Not all of the filetypes below can be determined.
 * For example, a given platform might not correctly report 
 * a socket descriptor as CH_SOCK if that type isn't 
 * well-identified on that platform.  In such cases where
 * a filetype exists but cannot be described by the recognized
 * flags below, the filetype will be CH_UNKFILE.  If the
 * filetype member is not determined, the type will be CH_NOFILE.
 */

typedef enum {
    CH_NOFILE = 0,     /**< no file type determined */
    CH_REG,            /**< a regular file */
    CH_DIR,            /**< a directory */
    CH_CHR,            /**< a character device */
    CH_BLK,            /**< a block device */
    CH_PIPE,           /**< a FIFO / pipe */
    CH_LNK,            /**< a symbolic link */
    CH_SOCK,           /**< a [unix domain] socket */
    CH_UNKFILE = 127   /**< a file of some other unknown type */
} ch_filetype_e; 

#define CH_PATH_MAX 8192
/**
 * @defgroup ch_file_permissions File Permissions flags 
 * @{
 */

#define CH_FPROT_USETID      0x8000 /**< Set user id */
#define CH_FPROT_UREAD       0x0400 /**< Read by user */
#define CH_FPROT_UWRITE      0x0200 /**< Write by user */
#define CH_FPROT_UEXECUTE    0x0100 /**< Execute by user */

#define CH_FPROT_GSETID      0x4000 /**< Set group id */
#define CH_FPROT_GREAD       0x0040 /**< Read by group */
#define CH_FPROT_GWRITE      0x0020 /**< Write by group */
#define CH_FPROT_GEXECUTE    0x0010 /**< Execute by group */

#define CH_FPROT_WSTICKY     0x2000 /**< Sticky bit */
#define CH_FPROT_WREAD       0x0004 /**< Read by others */
#define CH_FPROT_WWRITE      0x0002 /**< Write by others */
#define CH_FPROT_WEXECUTE    0x0001 /**< Execute by others */

#define CH_FPROT_OS_DEFAULT  0x0FFF /**< use OS's default permissions */

/* additional permission flags for ch_file_copy  and ch_file_append */
#define CH_FPROT_FILE_SOURCE_PERMS 0x1000 /**< Copy source file's permissions */
    
/* backcompat */
#define CH_USETID     CH_FPROT_USETID     /**< @deprecated @see CH_FPROT_USETID     */
#define CH_UREAD      CH_FPROT_UREAD      /**< @deprecated @see CH_FPROT_UREAD      */
#define CH_UWRITE     CH_FPROT_UWRITE     /**< @deprecated @see CH_FPROT_UWRITE     */
#define CH_UEXECUTE   CH_FPROT_UEXECUTE   /**< @deprecated @see CH_FPROT_UEXECUTE   */
#define CH_GSETID     CH_FPROT_GSETID     /**< @deprecated @see CH_FPROT_GSETID     */
#define CH_GREAD      CH_FPROT_GREAD      /**< @deprecated @see CH_FPROT_GREAD      */
#define CH_GWRITE     CH_FPROT_GWRITE     /**< @deprecated @see CH_FPROT_GWRITE     */
#define CH_GEXECUTE   CH_FPROT_GEXECUTE   /**< @deprecated @see CH_FPROT_GEXECUTE   */
#define CH_WSTICKY    CH_FPROT_WSTICKY    /**< @deprecated @see CH_FPROT_WSTICKY    */
#define CH_WREAD      CH_FPROT_WREAD      /**< @deprecated @see CH_FPROT_WREAD      */
#define CH_WWRITE     CH_FPROT_WWRITE     /**< @deprecated @see CH_FPROT_WWRITE     */
#define CH_WEXECUTE   CH_FPROT_WEXECUTE   /**< @deprecated @see CH_FPROT_WEXECUTE   */
#define CH_OS_DEFAULT CH_FPROT_OS_DEFAULT /**< @deprecated @see CH_FPROT_OS_DEFAULT */
#define CH_FILE_SOURCE_PERMS CH_FPROT_FILE_SOURCE_PERMS /**< @deprecated @see CH_FPROT_FILE_SOURCE_PERMS */
    
/** @} */


/**
 * Structure for referencing directories.
 */
typedef struct ch_dir_t          ch_dir_t;
/**
 * Structure for determining file permissions.
 */

/**
 * @defgroup ch_file_stat Stat Functions
 * @{
 */
/** file info structure */
typedef struct ch_finfo_t        ch_finfo_t;

#define CH_FINFO_LINK   0x00000001 /**< Stat the link not the file itself if it is a link */
#define CH_FINFO_MTIME  0x00000010 /**< Modification Time */
#define CH_FINFO_CTIME  0x00000020 /**< Creation or inode-changed time */
#define CH_FINFO_ATIME  0x00000040 /**< Access Time */
#define CH_FINFO_SIZE   0x00000100 /**< Size of the file */
#define CH_FINFO_CSIZE  0x00000200 /**< Storage size consumed by the file */
#define CH_FINFO_DEV    0x00001000 /**< Device */
#define CH_FINFO_INODE  0x00002000 /**< Inode */
#define CH_FINFO_NLINK  0x00004000 /**< Number of links */
#define CH_FINFO_TYPE   0x00008000 /**< Type */
#define CH_FINFO_USER   0x00010000 /**< User */
#define CH_FINFO_GROUP  0x00020000 /**< Group */
#define CH_FINFO_UPROT  0x00100000 /**< User protection bits */
#define CH_FINFO_GPROT  0x00200000 /**< Group protection bits */
#define CH_FINFO_WPROT  0x00400000 /**< World protection bits */
#define CH_FINFO_ICASE  0x01000000 /**< if dev is case insensitive */
#define CH_FINFO_NAME   0x02000000 /**< ->name in proper case */

#define CH_FINFO_MIN    0x00008170 /**< type, mtime, ctime, atime, size */
#define CH_FINFO_IDENT  0x00003000 /**< dev and inode */
#define CH_FINFO_OWNER  0x00030000 /**< user and group */
#define CH_FINFO_PROT   0x00700000 /**<  all protections */
#define CH_FINFO_NORM   0x0073b170 /**<  an atomic unix ch_stat() */
#define CH_FINFO_DIRENT 0x02000000 /**<  an atomic unix ch_dir_read() */

/**
 * The file information structure.  This is analogous to the POSIX
 * stat structure.
 */
struct ch_finfo_t {
    /** Allocates memory and closes lingering handles in the specified pool */
    ch_pool_t *pool;
    /** The bitmask describing valid fields of this ch_finfo_t structure 
     *  including all available 'wanted' fields and potentially more */
    int32_t valid;
    /** The access permissions of the file.  Mimics Unix access rights. */
    int32_t protection;
    /** The type of file.  One of CH_REG, CH_DIR, CH_CHR, CH_BLK, CH_PIPE, 
     * CH_LNK or CH_SOCK.  If the type is undetermined, the value is CH_NOFILE.
     * If the type cannot be determined, the value is CH_UNKFILE.
     */
    ch_filetype_e filetype;
    /** The user id that owns the file */
    uid_t user;
    /** The group id that owns the file */
    gid_t group;
    /** The inode of the file. */
    ino_t inode;
    /** The id of the device the file is on. */
    dev_t device;
    /** The number of hard links to the file. */
    int32_t nlink;
    /** The size of the file */
    off_t size;
    /** The storage size consumed by the file */
    off_t csize;
    /** The time the file was last accessed */
    int64_t atime;
    /** The time the file was last modified */
    int64_t mtime;
    /** The time the file was created, or the inode was last changed */
    int64_t ctime;
    /** The pathname of the file (possibly unrooted) */
    const char *fname;
    /** The file's name (no path) in filesystem case */
    const char *name;
    /** Unused */
    struct ch_file_t *filehand;
};

struct ch_dir_t {                                                                                                              
     ch_pool_t *pool;                                                                                                           
     char *dirname;                                                                                                              
     DIR *dirstruct;                                                                                                             
     struct dirent *entry;                                                                                                       
 };     

/**
 * get the specified file's stats.  The file is specified by filename, 
 * instead of using a pre-opened file.
 * @param finfo Where to store the information about the file, which is
 * never touched if the call fails.
 * @param fname The name of the file to stat.
 * @param wanted The desired ch_finfo_t fields, as a bit flag of CH_FINFO_
                 values 
 * @param pool the pool to use to allocate the new file. 
 *
 * @note If @c CH_INCOMPLETE is returned all the fields in @a finfo may
 *       not be filled in, and you need to check the @c finfo->valid bitmask
 *       to verify that what you're looking for is there.
 */ 
int ch_stat(ch_finfo_t *finfo, const char *fname,
                                   int32_t wanted, ch_pool_t *pool);

/** @} */
/**
 * @defgroup ch_dir Directory Manipulation Functions
 * @{
 */

/**
 * Open the specified directory.
 * @param new_dir The opened directory descriptor.
 * @param dirname The full path to the directory (use / on all systems)
 * @param pool The pool to use.
 */                        
int ch_dir_open(ch_dir_t **new_dir, 
                                       const char *dirname, 
                                       ch_pool_t *pool);

/**
 * close the specified directory. 
 * @param thedir the directory descriptor to close.
 */                        
int ch_dir_close(ch_dir_t *thedir);

/**
 * Read the next entry from the specified directory. 
 * @param finfo the file info structure and filled in by ch_dir_read
 * @param wanted The desired ch_finfo_t fields, as a bit flag of CH_FINFO_
                 values 
 * @param thedir the directory descriptor returned from ch_dir_open
 * @remark No ordering is guaranteed for the entries read.
 *
 * @note If @c CH_INCOMPLETE is returned all the fields in @a finfo may
 *       not be filled in, and you need to check the @c finfo->valid bitmask
 *       to verify that what you're looking for is there. When no more
 *       entries are available, CH_ENOENT is returned.
 */                        
int ch_dir_read(ch_finfo_t *finfo, int32_t wanted,
                                       ch_dir_t *thedir);

/**
 * Rewind the directory to the first entry.
 * @param thedir the directory descriptor to rewind.
 */                        
int ch_dir_rewind(ch_dir_t *thedir);
/** @} */

/**
 * @defgroup ch_filepath Filepath Manipulation Functions
 * @{
 */

/** Cause ch_filepath_merge to fail if addpath is above rootpath 
 * @bug in GW 0.9 and 1.x, this flag's behavior is undefined
 * if the rootpath is NULL or empty.  In GW 2.0 this should be
 * changed to imply NOTABSOLUTE if the rootpath is NULL or empty.
 */
#define CH_FILEPATH_NOTABOVEROOT   0x01

/** internal: Only meaningful with CH_FILEPATH_NOTABOVEROOT */
#define CH_FILEPATH_SECUREROOTTEST 0x02

/** Cause ch_filepath_merge to fail if addpath is above rootpath,
 * even given a rootpath /foo/bar and an addpath ../bar/bash
 */
#define CH_FILEPATH_SECUREROOT     0x03

/** Fail ch_filepath_merge if the merged path is relative */
#define CH_FILEPATH_NOTRELATIVE    0x04

/** Fail ch_filepath_merge if the merged path is absolute */
#define CH_FILEPATH_NOTABSOLUTE    0x08

/** Return the file system's native path format (e.g. path delimiters
 * of ':' on MacOS9, '\' on Win32, etc.) */
#define CH_FILEPATH_NATIVE         0x10

/** Resolve the true case of existing directories and file elements
 * of addpath, (resolving any aliases on Win32) and append a proper 
 * trailing slash if a directory
 */
#define CH_FILEPATH_TRUENAME       0x20

/**
 * Extract the rootpath from the given filepath
 * @param rootpath the root file path returned with CH_SUCCESS or CH_EINCOMPLETE
 * @param filepath the pathname to parse for its root component
 * @param flags the desired rules to apply, from
 * <PRE>
 *      CH_FILEPATH_NATIVE    Use native path separators (e.g. '\' on Win32)
 *      CH_FILEPATH_TRUENAME  Tests that the root exists, and makes it proper
 * </PRE>
 * @param p the pool to allocate the new path string from
 * @remark on return, filepath points to the first non-root character in the
 * given filepath.  In the simplest example, given a filepath of "/foo", 
 * returns the rootpath of "/" and filepath points at "foo".  This is far 
 * more complex on other platforms, which will canonicalize the root form
 * to a consistant format, given the CH_FILEPATH_TRUENAME flag, and also
 * test for the validity of that root (e.g., that a drive d:/ or network 
 * share //machine/foovol/). 
 * The function returns CH_ERELATIVE if filepath isn't rooted (an
 * error), CH_EINCOMPLETE if the root path is ambiguous (but potentially
 * legitimate, e.g. "/" on Windows is incomplete because it doesn't specify
 * the drive letter), or CH_EBADPATH if the root is simply invalid.
 * CH_SUCCESS is returned if filepath is an absolute path.
 */
int ch_filepath_root(const char **rootpath, 
                                            const char **filepath, 
                                            int32_t flags,
                                            ch_pool_t *p);

/**
 * Merge additional file path onto the previously processed rootpath
 * @param newpath the merged paths returned
 * @param rootpath the root file path (NULL uses the current working path)
 * @param addpath the path to add to the root path
 * @param flags the desired CH_FILEPATH_ rules to apply when merging
 * @param p the pool to allocate the new path string from
 * @remark if the flag CH_FILEPATH_TRUENAME is given, and the addpath 
 * contains wildcard characters ('*', '?') on platforms that don't support 
 * such characters within filenames, the paths will be merged, but the 
 * result code will be CH_EPATHWILD, and all further segments will not
 * reflect the true filenames including the wildcard and following segments.
 */                        
int ch_filepath_merge(char **newpath, 
                                             const char *rootpath,
                                             const char *addpath, 
                                             int32_t flags,
                                             ch_pool_t *p);

/**
 * Split a search path into separate components
 * @param pathelts the returned components of the search path
 * @param liststr the search path (e.g., <tt>getenv("PATH")</tt>)
 * @param p the pool to allocate the array and path components from
 * @remark empty path components do not become part of @a pathelts.
 * @remark the path separator in @a liststr is system specific;
 * e.g., ':' on Unix, ';' on Windows, etc.
 */
int ch_filepath_list_split(ch_array_header_t **pathelts,
                                                  const char *liststr,
                                                  ch_pool_t *p);

/**
 * Merge a list of search path components into a single search path
 * @param liststr the returned search path; may be NULL if @a pathelts is empty
 * @param pathelts the components of the search path
 * @param p the pool to allocate the search path from
 * @remark emtpy strings in the source array are ignored.
 * @remark the path separator in @a liststr is system specific;
 * e.g., ':' on Unix, ';' on Windows, etc.
 */
int ch_filepath_list_merge(char **liststr,
                                                  ch_array_header_t *pathelts,
                                                  ch_pool_t *p);

/**
 * Return the default file path (for relative file names)
 * @param path the default path string returned
 * @param flags optional flag CH_FILEPATH_NATIVE to retrieve the
 *              default file path in os-native format.
 * @param p the pool to allocate the default path string from
 */
int ch_filepath_get(char **path, int32_t flags,
                                           ch_pool_t *p);

/**
 * Set the default file path (for relative file names)
 * @param path the default path returned
 * @param p the pool to allocate any working storage
 */
int ch_filepath_set(const char *path, ch_pool_t *p);

/** The FilePath character encoding is unknown */
#define CH_FILEPATH_ENCODING_UNKNOWN  0

/** The FilePath character encoding is locale-dependent */
#define CH_FILEPATH_ENCODING_LOCALE   1

/** The FilePath character encoding is UTF-8 */
#define CH_FILEPATH_ENCODING_UTF8     2

/**
 * Determine the encoding used internally by the FilePath functions
 * @param style points to a variable which receives the encoding style flag
 * @param p the pool to allocate any working storage
 * @remark Use @c ch_os_locale_encoding and/or @c ch_os_default_encoding
 * to get the name of the path encoding if it's not UTF-8.
 */
int ch_filepath_encoding(int *style, ch_pool_t *p);
/** @} */

/** @} */

mode_t ch_perms2mode(int32_t perm);

int32_t ch_mode2perms(mode_t mode);


#endif /* CH_FILE_INFO_H */

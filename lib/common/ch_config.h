/*
 *
 *      Filename: ch_config.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-10 18:01:33
 * Last Modified: 2017-01-10 18:01:33
 */

#ifndef CH_CONFIG_H
#define CH_CONFIG_H

#include "ch_mpool.h"
#include "ch_tables.h"

enum cmd_how {
    RAW_ARGS,           /**< cmd_func parses command line itself */
    TAKE1,              /**< one argument only */
    TAKE2,              /**< two arguments only */
    ITERATE,            /**< one argument, occuring multiple times
                         * (e.g., IndexIgnore)
                         */
    ITERATE2,           /**< two arguments, 2nd occurs multiple times
                         * (e.g., AddIcon)
                         */
    FLAG,               /**< One of 'On' or 'Off' */
    NO_ARGS,            /**< No args at all, e.g. &lt;/Directory&gt; */
    TAKE12,             /**< one or two arguments */
    TAKE3,              /**< thrgw arguments only */
    TAKE23,             /**< two or thrgw arguments */
    TAKE123,            /**< one, two or thrgw arguments */
    TAKE13,             /**< one or thrgw arguments */
    TAKE_ARGV           /**< an argc and argv are passed */
};

/**
 * This structure is passed to a command which is being invoked,
 * to carry a large variety of miscellaneous data which is all of
 * use to *somebody*...
 */
typedef struct cmd_parms_struct cmd_parms;


/**
 * All the types of functions that can be used in directives
 * @internal
 */
typedef union {
    /** function to call for a no-args */
    const char *(*no_args) (cmd_parms *parms, void *mconfig);
    /** function to call for a raw-args */
    const char *(*raw_args) (cmd_parms *parms, void *mconfig,
                             const char *args);
    /** function to call for a argv/argc */
    const char *(*take_argv) (cmd_parms *parms, void *mconfig,
                             int argc, char *const argv[]);
    /** function to call for a take1 */
    const char *(*take1) (cmd_parms *parms, void *mconfig, const char *w);
    /** function to call for a take2 */
    const char *(*take2) (cmd_parms *parms, void *mconfig, const char *w,
                          const char *w2);
    /** function to call for a take3 */
    const char *(*take3) (cmd_parms *parms, void *mconfig, const char *w,
                          const char *w2, const char *w3);
    /** function to call for a flag */
    const char *(*flag) (cmd_parms *parms, void *mconfig, int on);
} cmd_func;

/** This configuration directive does not take any arguments */
# define CH_NO_ARGS     func.no_args
/** This configuration directive will handle its own parsing of arguments*/
# define CH_RAW_ARGS    func.raw_args
/** This configuration directive will handle its own parsing of arguments*/
# define CH_TAKE_ARGV   func.take_argv
/** This configuration directive takes 1 argument*/
# define CH_TAKE1       func.take1
/** This configuration directive takes 2 arguments */
# define CH_TAKE2       func.take2
/** This configuration directive takes 3 arguments */
# define CH_TAKE3       func.take3
/** This configuration directive takes a flag (on/off) as a argument*/
# define CH_FLAG        func.flag

/** mechanism for declaring a directive with no arguments */
# define CH_INIT_NO_ARGS(directive, func, mconfig, where, help) \
    { directive, { .no_args=func }, mconfig, where, RAW_ARGS, help }
/** mechanism for declaring a directive with raw argument parsing */
# define CH_INIT_RAW_ARGS(directive, func, mconfig, where, help) \
    { directive, { .raw_args=func }, mconfig, where, RAW_ARGS, help }
/** mechanism for declaring a directive with raw argument parsing */
# define CH_INIT_TAKE_ARGV(directive, func, mconfig, where, help) \
    { directive, { .take_argv=func }, mconfig, where, TAKE_ARGV, help }
/** mechanism for declaring a directive which takes 1 argument */
# define CH_INIT_TAKE1(directive, func, mconfig, where, help) \
    { directive, { .take1=func }, mconfig, where, TAKE1, help }
/** mechanism for declaring a directive which takes multiple arguments */
# define CH_INIT_ITERATE(directive, func, mconfig, where, help) \
    { directive, { .take1=func }, mconfig, where, ITERATE, help }
/** mechanism for declaring a directive which takes 2 arguments */
# define CH_INIT_TAKE2(directive, func, mconfig, where, help) \
    { directive, { .take2=func }, mconfig, where, TAKE2, help }
/** mechanism for declaring a directive which takes 1 or 2 arguments */
# define CH_INIT_TAKE12(directive, func, mconfig, where, help) \
    { directive, { .take2=func }, mconfig, where, TAKE12, help }
/** mechanism for declaring a directive which takes multiple 2 arguments */
# define CH_INIT_ITERATE2(directive, func, mconfig, where, help) \
    { directive, { .take2=func }, mconfig, where, ITERATE2, help }
/** mechanism for declaring a directive which takes 1 or 3 arguments */
# define CH_INIT_TAKE13(directive, func, mconfig, where, help) \
    { directive, { .take3=func }, mconfig, where, TAKE13, help }
/** mechanism for declaring a directive which takes 2 or 3 arguments */
# define CH_INIT_TAKE23(directive, func, mconfig, where, help) \
    { directive, { .take3=func }, mconfig, where, TAKE23, help }
/** mechanism for declaring a directive which takes 1 to 3 arguments */
# define CH_INIT_TAKE123(directive, func, mconfig, where, help) \
    { directive, { .take3=func }, mconfig, where, TAKE123, help }
/** mechanism for declaring a directive which takes 3 arguments */
# define CH_INIT_TAKE3(directive, func, mconfig, where, help) \
    { directive, { .take3=func }, mconfig, where, TAKE3, help }
/** mechanism for declaring a directive which takes a flag (on/off) argument */
# define CH_INIT_FLAG(directive, func, mconfig, where, help) \
    { directive, { .flag=func }, mconfig, where, FLAG, help }


/**
 * The command record structure.  Each modules can define a table of these
 * to define the directives it will implement.
 */
typedef struct command_struct command_rec;
struct command_struct {
    /** Name of this command */
    const char *name;
    /** The function to be called when this directive is parsed */
    cmd_func func;
    /** Extra data, for functions which implement multiple commands... */
    void *cmd_data;
    /** What overrides ngwd to be allowed to enable this command. */
    int req_override;
    /** What the command expects as arguments */
    enum cmd_how args_how;

    /** 'usage' message, in case of syntax errors */
    const char *errmsg;
};

/**
 * This can be returned by a function if they don't wish to handle
 * a command. Make it something not likely someone will actually use
 * as an error code.
 */
#define DECLINE_CMD "\a\b"

/** Common structure for reading of config files / passwd files etc. */
typedef struct ch_configfile_t ch_configfile_t;
struct ch_configfile_t {
    /**< an ch_file_getc()-like function */
    int (*getch) (char *ch, void *param);
    /**< an ch_file_gets()-like function */
    int (*getstr) (void *buf, size_t bufsiz, void *param);
    /**< a close handler function */
    int (*close) (void *param);
    /**< the argument passed to getch/getstr/close */
    void *param;
    /**< the filename / description */
    const char *name;
    /**< current line number, starting at 1 */
    unsigned line_number;
};

/**
 * This structure is passed to a command which is being invoked,
 * to carry a large variety of miscellaneous data which is all of
 * use to *somebody*...
 */
struct cmd_parms_struct {
    /** Argument to command from cmd_table */
    void *info;

    /** Config file structure. */
    ch_configfile_t *config_file;

    /** Pool to allocate new storage in */
    ch_pool_t *pool;
    /** Pool for scratch memory; persists during configuration, but
     *  wiped before the first request is served...  */
    ch_pool_t *temp_pool;
    /** If configuring for a directory, pathname of that directory.
     *  NOPE!  That's what it meant previous to the existence of &lt;Files&gt;,
     * &lt;Location&gt; and regex matching.  Now the only usefulness that can be
     * derived from this field is whether a command is being called in a
     * server context (path == NULL) or being called in a dir context
     * (path != NULL).  */
    char *path;
    /** configuration command */
    const command_rec *cmd;

};


/**
 * Open a ch_configfile_t as ch_file_t
 * @param ret_cfg open ch_configfile_t struct pointer
 * @param p The pool to allocate the structure from
 * @param name the name of the file to open
 */
int ch_pcfg_openfile(ch_configfile_t **ret_cfg,
                                          ch_pool_t *p, const char *name);


/**
 * Read one line from open ch_configfile_t, strip leading and trailing
 * whitespace, increase line number
 * @param buf place to store the line read
 * @param bufsize size of the buffer
 * @param cfp File to read from
 * @return error status, GWR_ENOSPC if bufsize is too small for the line
 */
int ch_cfg_getline(char *buf, size_t bufsize, ch_configfile_t *cfp);


/**
 * Detach from open ch_configfile_t, calling the close handler
 * @param cfp The file to close
 * @return 1 on sucess, 0 on failure
 */
int ch_cfg_closefile(ch_configfile_t *cfp);


/**
 * Find a given directive in a command_rec table
 * @param name The directive to search for
 * @param cmds The table to search
 * @return The directive definition of the specified directive
 */
const command_rec * ch_find_command(const char *name,
                                                     const command_rec *cmds);


char * ch_getword_conf(ch_pool_t *p, const char **line);

int ch_is_directory(ch_pool_t *p, const char *path);

char * ch_make_full_path(ch_pool_t *a, const char *src1,
                                  const char *src2);

const char * ch_process_resource_config(const char *fname,
                                                    ch_array_header_t *ari,
                                                    ch_pool_t *ptemp);

const char * ch_process_fnmatch_configs(ch_array_header_t *ari,
                                                    const char *fname,
                                                    ch_pool_t *p,
                                                    ch_pool_t *ptemp,
                                                    int optional);

const char *ch_populate_include_files(ch_pool_t *p, ch_pool_t *ptemp, ch_array_header_t *ari, const char *fname, int optional);

const char *ch_process_command_config(const command_rec *cmds,
                                        void *mconfig,
                                          ch_pool_t *p,
                                          ch_pool_t *ptemp,
										  const char *filename);


#endif /* CH_CONFIG_H */

/*
 * =====================================================================================
 *
 *       Filename:  ch_config.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月14日 14时09分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <apr_fnmatch.h>
#include <apr_file_io.h> 
#include <apr_strings.h>
#include "ch_constants.h"
#include "ch_config.h"

static cmd_parms default_parms =
{NULL, NULL, NULL, NULL, NULL, NULL};

int ch_cfg_closefile(ch_configfile_t *cfp)
{
    return (cfp->close == NULL) ? 0 : cfp->close(cfp->param);
}

/* we can't use apr_file_* directly because of linking issues on Windows */
static int cfg_close(void *param)
{
    return apr_file_close((apr_file_t*)param);
}

static int cfg_getch(char *ch, void *param)
{
    return apr_file_getc(ch, (apr_file_t*)param);
}

static int cfg_getstr(void *buf, size_t bufsiz, void *param)
{
    return apr_file_gets((char*)buf, bufsiz, (apr_file_t*)param);
}

static int ch_cfg_getline_core(char *buf, size_t bufsize,
                                        ch_configfile_t *cfp)
{
    int rc;
    /* If a "get string" function is defined, use it */
    if (cfp->getstr != NULL) {
        char *cp;
        char *cbuf = buf;
        size_t cbufsize = bufsize;

        while (1) {
            ++cfp->line_number;
            rc = cfp->getstr(cbuf, cbufsize, cfp->param);
            if (rc == APR_EOF) {
                if (cbuf != buf) {
                    *cbuf = '\0';
                    break;
                }
                else {
                    return APR_EOF;
                }
            }
            if (rc != APR_SUCCESS) {
                return rc;
            }

            /*
             *  check for line continuation,
             *  i.e. match [^\\]\\[\r]\n only
             */
            cp = cbuf;
            cp += strlen(cp);
            if (cp > cbuf && cp[-1] == LF) {
                cp--;
                if (cp > cbuf && cp[-1] == CR)
                    cp--;
                if (cp > cbuf && cp[-1] == '\\') {
                    cp--;
                    /*
                     * line continuation requested -
                     * then remove backslash and continue
                     */
                    cbufsize -= (cp-cbuf);
                    cbuf = cp;
                    continue;
                }
            }
            else if ((size_t)(cp - buf) >= bufsize - 1) {
                return APR_ENOSPC;
            }
            break;
        }
    } else {
        /* No "get string" function defined; read character by character */
        size_t i = 0;

        if (bufsize < 2) {
            /* too small, assume caller is crazy */
            return APR_EINVAL;
        }
        buf[0] = '\0';

        while (1) {
            char c;
            rc = cfp->getch(&c, cfp->param);
            if (rc == APR_EOF) {
                if (i > 0)
                    break;
                else
                    return APR_EOF;
            }
            if (rc != APR_SUCCESS)
                return rc;
            if (c == LF) {
                ++cfp->line_number;
                /* check for line continuation */
                if (i > 0 && buf[i-1] == '\\') {
                    i--;
                    continue;
                }
                else {
                    break;
                }
            }
            else if (i >= bufsize - 2) {
                return APR_ENOSPC;
            }
            buf[i] = c;
            ++i;
        }
        buf[i] = '\0';
    }
    return APR_SUCCESS;
}

static int cfg_trim_line(char *buf)
{
    char *start, *end;
    /*
     * Leading and trailing white space is eliminated completely
     */
    start = buf;
    while (isspace(*start))
        ++start;
    /* blast trailing whitespace */
    end = &start[strlen(start)];
    while (--end >= start && isspace(*end))
        *end = '\0';
    /* Zap leading whitespace by shifting */
    if (start != buf)
        memmove(buf, start, end - start + 2);
    return end - start + 1;
}

int ch_cfg_getline(char *buf, size_t bufsize,
		ch_configfile_t *cfp)
{
	int rc = ch_cfg_getline_core(buf, bufsize, cfp);
	if (rc == APR_SUCCESS)
		cfg_trim_line(buf);
	return rc;
}

static char *substring_conf(apr_pool_t *p, const char *start, int len,
                            char quote)
{
    char *result = (char*)apr_palloc(p, len + 2);
    char *resp = result;
    int i;

    for (i = 0; i < len; ++i) {
        if (start[i] == '\\' && (start[i + 1] == '\\'
                                 || (quote && start[i + 1] == quote)))
            *resp++ = start[++i];
        else
            *resp++ = start[i];
    }

    *resp++ = '\0';
    return result;
}

char * ch_getword_conf(apr_pool_t *p, const char **line)
{
    const char *str = *line, *strend;
    char *res;
    char quote;

    while (*str && isspace(*str))
        ++str;

    if (!*str) {
        *line = str;
        return "";
    }

    if ((quote = *str) == '"' || quote == '\'') {
        strend = str + 1;
        while (*strend && *strend != quote) {
            if (*strend == '\\' && strend[1] &&
                (strend[1] == quote || strend[1] == '\\')) {
                strend += 2;
            }
            else {
                ++strend;
            }
        }
        res = substring_conf(p, str + 1, strend - str - 1, quote);

        if (*strend == quote)
            ++strend;
    }
    else {
        strend = str;
        while (*strend && !isspace(*strend))
            ++strend;

        res = substring_conf(p, str, strend - str, 0);
    }

    while (*strend && isspace(*strend))
        ++strend;
    *line = strend;
    return res;
}

/* Open a ch_configfile_t as FILE, return open ch_configfile_t struct pointer */
int ch_pcfg_openfile(ch_configfile_t **ret_cfg,
                                          apr_pool_t *p, const char *name)
{
    ch_configfile_t *new_cfg;
    apr_file_t *file = NULL;
    apr_finfo_t finfo;
    int status;

    if (name == NULL) {
        return APR_EBADF;
    }

    status = apr_file_open(&file, name, APR_READ | APR_BUFFERED,
                           APR_OS_DEFAULT, p);
    if (status != APR_SUCCESS)
        return status;

    status = apr_file_info_get(&finfo, APR_FINFO_TYPE, file);
    if (status != APR_SUCCESS)
        return status;

    if (finfo.filetype != APR_REG &&
        strcmp(name, "/dev/null") != 0) {
        apr_file_close(file);
        return APR_EBADF;
    }


    new_cfg = (ch_configfile_t*)apr_palloc(p, sizeof(*new_cfg));
    new_cfg->param = file;
    new_cfg->name = apr_pstrdup(p, name);
    new_cfg->getch = cfg_getch;
    new_cfg->getstr = cfg_getstr;
    new_cfg->close = cfg_close; 
    new_cfg->line_number = 0;
    *ret_cfg = new_cfg;
    return APR_SUCCESS;
}

const command_rec * ch_find_command(const char *name,
                                                     const command_rec *cmds)
{
    while (cmds->name) {
        if (!strcasecmp(name, cmds->name))
            return cmds;

        ++cmds;
    }

    return NULL;
}

#define CH_MAX_ARGC 64

static const char *invoke_cmd(const command_rec *cmd, cmd_parms *parms,
                              void *mconfig, const char *args)
{
    char *w, *w2, *w3;
    const char *errmsg = NULL;


    parms->info = cmd->cmd_data;
    parms->cmd = cmd;

    switch (cmd->args_how) {
    case RAW_ARGS:
        return cmd->CH_RAW_ARGS(parms, mconfig, args);

    case TAKE_ARGV:
        {
            char *argv[CH_MAX_ARGC];
            int argc = 0;

            do {
                w = ch_getword_conf(parms->pool, &args);
                if (*w == '\0' && *args == '\0') {
                    break;
                }
                argv[argc] = w;
                argc++;
            } while (argc < CH_MAX_ARGC && *args != '\0');

            return cmd->CH_TAKE_ARGV(parms, mconfig, argc, argv);
        }

    case NO_ARGS:
        if (*args != 0)
            return apr_pstrcat(parms->pool, cmd->name, " takes no arguments",
                               NULL);

        return cmd->CH_NO_ARGS(parms, mconfig);

    case TAKE1:
        w = ch_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name, " takes one argument",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE1(parms, mconfig, w);

    case TAKE2:
        w = ch_getword_conf(parms->pool, &args);
        w2 = ch_getword_conf(parms->pool, &args);

        if (*w == '\0' || *w2 == '\0' || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name, " takes two arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE2(parms, mconfig, w, w2);

    case TAKE12:
        w = ch_getword_conf(parms->pool, &args);
        w2 = ch_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name, " takes 1-2 arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE2(parms, mconfig, w, *w2 ? w2 : NULL);

    case TAKE3:
        w = ch_getword_conf(parms->pool, &args);
        w2 = ch_getword_conf(parms->pool, &args);
        w3 = ch_getword_conf(parms->pool, &args);

        if (*w == '\0' || *w2 == '\0' || *w3 == '\0' || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name, " takes three arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE23:
        w = ch_getword_conf(parms->pool, &args);
        w2 = ch_getword_conf(parms->pool, &args);
        w3 = *args ? ch_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || *w2 == '\0' || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name,
                               " takes two or three arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE123:
        w = ch_getword_conf(parms->pool, &args);
        w2 = *args ? ch_getword_conf(parms->pool, &args) : NULL;
        w3 = *args ? ch_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name,
                               " takes one, two or three arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE13:
        w = ch_getword_conf(parms->pool, &args);
        w2 = *args ? ch_getword_conf(parms->pool, &args) : NULL;
        w3 = *args ? ch_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || (w2 && *w2 && !w3) || *args != 0)
            return apr_pstrcat(parms->pool, cmd->name,
                               " takes one or three arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->CH_TAKE3(parms, mconfig, w, w2, w3);

    case ITERATE:
        while (*(w = ch_getword_conf(parms->pool, &args)) != '\0') {

            errmsg = cmd->CH_TAKE1(parms, mconfig, w);

            if (errmsg && strcmp(errmsg, DECLINE_CMD) != 0)
                return errmsg;
        }

        return errmsg;

    case ITERATE2:
        w = ch_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args == 0)
            return apr_pstrcat(parms->pool, cmd->name,
                               " requires at least two arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        while (*(w2 = ch_getword_conf(parms->pool, &args)) != '\0') {

            errmsg = cmd->CH_TAKE2(parms, mconfig, w, w2);

            if (errmsg && strcmp(errmsg, DECLINE_CMD) != 0)
                return errmsg;
        }

        return errmsg;

    case FLAG:
        w = ch_getword_conf(parms->pool, &args);

        if (*w == '\0' || (strcasecmp(w, "on") && strcasecmp(w, "off")))
            return apr_pstrcat(parms->pool, cmd->name, " must be On or Off",
                               NULL);

        return cmd->CH_FLAG(parms, mconfig, strcasecmp(w, "off") != 0);

    default:
        return apr_pstrcat(parms->pool, cmd->name,
                           " is improperly configured internally (server bug)",
                           NULL);
    }
}


typedef struct {
    const char *fname;
} fnames;

int ch_is_directory(apr_pool_t *p, const char *path)
{
    apr_finfo_t finfo;

    if (apr_stat(&finfo, path, APR_FINFO_TYPE, p) != APR_SUCCESS)
        return 0;                /* in error condition, just return no */

    return (finfo.filetype == APR_DIR);
}

char * ch_make_full_path(apr_pool_t *a, const char *src1,
                                  const char *src2)
{
    size_t len1, len2;
    char *path;

    len1 = strlen(src1);
    len2 = strlen(src2);
     /* allocate +3 for '/' delimiter, trailing NULL and overallocate
      * one extra byte to allow the caller to add a trailing '/'
      */
    path = (char *)apr_palloc(a, len1 + len2 + 3);
    if (len1 == 0) {
        *path = '/';
        memcpy(path + 1, src2, len2 + 1);
    }
    else {
        char *next;
        memcpy(path, src1, len1);
        next = path + len1;
        if (next[-1] != '/') {
            *next++ = '/';
        }
        memcpy(next, src2, len2 + 1);
    }
    return path;
}

static int fname_alphasort(const void *fn1, const void *fn2)
{
    const fnames *f1 = (const fnames*)fn1;
    const fnames *f2 = (const fnames*)fn2;

    return strcmp(f1->fname,f2->fname);
}


const char * ch_process_resource_config(const char *fname,
                                                    apr_array_header_t *ari,
                                                    apr_pool_t *ptemp)
{
    ptemp = ptemp;

	*(char **)apr_array_push(ari) = (char *)fname;

    return NULL;
}

static const char *ch_process_resource_config_nofnmatch(const char *fname,
                                                     apr_array_header_t *ari,
                                                     apr_pool_t *p,
                                                     apr_pool_t *ptemp,
													 unsigned depth,
                                                     int optional)
{
    const char *error;
    int rv;

    if (ch_is_directory(ptemp, fname)) {
        apr_dir_t *dirp;
        apr_finfo_t dirent;
        int current;
        apr_array_header_t *candidates = NULL;
        fnames *fnew;
        char *path = apr_pstrdup(ptemp, fname);

        if (++depth > 100) {
            return apr_psprintf(p, "Directory %s exceeds the maximum include "
                                "directory nesting level of %u. You have "
                                "probably a recursion somewhere.", path,
                                100);
        }

        /*
         * first course of business is to grok all the directory
         * entries here and store 'em away. Recall we need full pathnames
         * for this.
         */
        rv = apr_dir_open(&dirp, path, ptemp);
        if (rv != APR_SUCCESS) {
            char errmsg[120];
            return apr_psprintf(p, "Could not open config directory %s: %s",
                                path, apr_strerror(rv, errmsg, sizeof errmsg));
        }

        candidates = apr_array_make(ptemp, 1, sizeof(fnames));
        while (apr_dir_read(&dirent, APR_FINFO_DIRENT, dirp) == APR_SUCCESS) {
            /* strip out '.' and '..' */
            if (strcmp(dirent.name, ".")
                && strcmp(dirent.name, "..")) {
                fnew = (fnames *) apr_array_push(candidates);
                fnew->fname = ch_make_full_path(ptemp, path, dirent.name);
            }
        }

        apr_dir_close(dirp);
        if (candidates->nelts != 0) {
            qsort((void *) candidates->elts, candidates->nelts,
                  sizeof(fnames), fname_alphasort);

            /*
             * Now recurse these... we handle errors and subdirectories
             * via the recursion, which is nice
             */
            for (current = 0; current < candidates->nelts; ++current) {
                fnew = &((fnames *) candidates->elts)[current];
                error = ch_process_resource_config_nofnmatch(fnew->fname,
                                                          ari, p, ptemp,
                                                          depth, optional);
                if (error) {
                    return error;
                }
            }
        }

        return NULL;
    }

    return ch_process_resource_config(fname, ari, ptemp);
}

int fnmatch_test(const char *pattern)
{
    int nesting;

    nesting = 0;
    while (*pattern) {
        switch (*pattern) {
        case '?':
        case '*':
            return 1;

/*        case '\\':
            if (*++pattern == '\0') {
                return 0;
            }
            break;*/        // this breaks on Windows

        case '[':         /* '[' is only a glob if it has a matching ']' */
            ++nesting;
            break;

        case ']':
            if (nesting) {
                return 1;
            }
            break;

        default:
            break;
        }
        ++pattern;    }
    return 0;
}


static const char *ch_process_resource_config_fnmatch(const char *path,
                                                   const char *fname,
                                                   apr_array_header_t *ari,
                                                   apr_pool_t *p,
                                                   apr_pool_t *ptemp,
                                                   unsigned depth,
                                                   int optional)
{
    depth = depth;

    const char *rest;
    int rv;
    apr_dir_t *dirp;
    apr_finfo_t dirent;
    apr_array_header_t *candidates = NULL;
    fnames *fnew;
    int current;

    /* find the first part of the filename */
    rest = strchr(fname, '/');

	if(rest == NULL)
		rest = strchr(fname, '\\');

	if (rest) {
        fname = apr_pstrndup(ptemp, fname, rest - fname);
        rest++;
    }

    /* optimisation - if the filename isn't a wildcard, process it directly */
    if (!fnmatch_test(fname)) {
        path = ch_make_full_path(ptemp, path, fname);
        if (!rest) {
            return ch_process_resource_config_nofnmatch(path,
                                                     ari, p,
                                                     ptemp, 0, optional);
        }
        else {
            return ch_process_resource_config_fnmatch(path, rest,
                                                   ari, p,
                                                   ptemp, 0, optional);
        }
    }

    /*
     * first course of business is to grok all the directory
     * entries here and store 'em away. Recall we need full pathnames
     * for this.
     */
    rv = apr_dir_open(&dirp, path, ptemp);
    if (rv != APR_SUCCESS) {
        char errmsg[120];
        return apr_psprintf(p, "Could not open config directory %s: %s",
                            path, apr_strerror(rv, errmsg, sizeof errmsg));
    }

    candidates = apr_array_make(ptemp, 1, sizeof(fnames));
    while (apr_dir_read(&dirent, APR_FINFO_DIRENT | APR_FINFO_TYPE, dirp) == APR_SUCCESS) {
        /* strip out '.' and '..' */
        if (strcmp(dirent.name, ".")
            && strcmp(dirent.name, "..")
            && (apr_fnmatch(fname, dirent.name,
                            APR_FNM_PERIOD | APR_FNM_NOESCAPE | APR_FNM_PATHNAME) == APR_SUCCESS)) {
            const char *full_path = ch_make_full_path(ptemp, path, dirent.name);
            /* If matching internal to path, and we happen to match something
             * other than a directory, skip it
             */
            if (rest && (rv == APR_SUCCESS) && (dirent.filetype != APR_DIR)) {
                continue;
            }
            fnew = (fnames *) apr_array_push(candidates);
            fnew->fname = full_path;
        }
    }

    apr_dir_close(dirp);
    if (candidates->nelts != 0) {
        const char *error;

        qsort((void *) candidates->elts, candidates->nelts,
              sizeof(fnames), fname_alphasort);

        /*
         * Now recurse these... we handle errors and subdirectories
         * via the recursion, which is nice
         */
        for (current = 0; current < candidates->nelts; ++current) {
            fnew = &((fnames *) candidates->elts)[current];
            if (!rest) {
                error = ch_process_resource_config_nofnmatch(fnew->fname,
                                                          ari, p,
                                                          ptemp, 0, optional);
            }
            else {
                error = ch_process_resource_config_fnmatch(fnew->fname, rest,
                                                        ari, p,
                                                        ptemp, 0, optional);
            }
            if (error) {
                return error;
            }
        }
    }
    else {

        if (!optional) {
            return apr_psprintf(p, "No matches for the wildcard '%s' in '%s', failing "
                                   "(use IncludeOptional if required)", fname, path);
        }
    }

    return NULL;
}

const char * ch_process_fnmatch_configs(apr_array_header_t *ari,
                                                    const char *fname,
                                                    apr_pool_t *p,
                                                    apr_pool_t *ptemp,
                                                    int optional)
{
    if (!fnmatch_test(fname)) {
        return ch_process_resource_config(fname, ari, p);
    }
    else {
        int status;
        const char *rootpath, *filepath = fname;

        /* locate the start of the directories proper */
        status = apr_filepath_root(&rootpath, &filepath, APR_FILEPATH_TRUENAME | APR_FILEPATH_NATIVE, ptemp);

        /* we allow APR_SUCCESS and APR_EINCOMPLETE */
        if (APR_ERELATIVE == status) {
            return apr_pstrcat(p, "Include must have an absolute path, ", fname, NULL);
        }
        else if (APR_EBADPATH == status) {
            return apr_pstrcat(p, "Include has a bad path, ", fname, NULL);
        }

        /* walk the filepath */
        return ch_process_resource_config_fnmatch(rootpath, filepath, ari, p, ptemp,
                                               0, optional);
    }
}

const char *ch_populate_include_files(apr_pool_t *p, apr_pool_t *ptemp, apr_array_header_t *ari, const char *fname, int optional)
{
	return ch_process_fnmatch_configs(ari, fname, p, ptemp, optional);
}

const char *ch_process_command_config(const command_rec *cmds,
                                        void *mconfig,
                                          apr_pool_t *p,
                                          apr_pool_t *ptemp,
										  const char *filename)
{
    const char *errmsg;
    char *l = apr_palloc (ptemp, MAX_STRING_LEN);
    const char *args = l;
    char *cmd_name, *w;
	const command_rec *cmd;
	apr_array_header_t *arr = apr_array_make(p, 1, sizeof(cmd_parms));
	apr_array_header_t *ari = apr_array_make(p, 1, sizeof(char *));
    cmd_parms *parms;
	int status;
	int optional;
	char *err = NULL;
	char *rootpath, *incpath;
	int li;

	errmsg = ch_populate_include_files(p, ptemp, ari, filename, 0);

	if(errmsg != NULL)
		goto Exit;

	while(ari->nelts != 0 || arr->nelts != 0)
	{
		if(ari->nelts > 0)
		{
			char *fn = *(char **)apr_array_pop(ari);

			parms = (cmd_parms *)apr_array_push(arr);
			*parms = default_parms;
			parms->pool = p;
			parms->temp_pool = ptemp;

			status = ch_pcfg_openfile(&parms->config_file, p, fn);

			if(status != APR_SUCCESS)
			{
				apr_array_pop(arr);
				errmsg = apr_pstrcat(p, "Cannot open config file: ", fn, NULL);
				goto Exit;
			}
		}

		if (arr->nelts > 1024) {
            errmsg = "Exceeded the maximum include directory nesting level. You have "
                                "probably a recursion somewhere.";
			goto Exit;
        }

		parms = (cmd_parms *)apr_array_pop(arr);

		if(parms == NULL)
			break;

		while (!(ch_cfg_getline(l, MAX_STRING_LEN, parms->config_file))) {
			if (*l == '#' || *l == '\0')
				continue;

			args = l;

			cmd_name = ch_getword_conf(p, &args);

			if (*cmd_name == '\0')
				continue;

			if (!strcasecmp(cmd_name, "IncludeOptional"))
			{
				optional = 1;
				goto ProcessInclude;
			}

			if (!strcasecmp(cmd_name, "Include"))
			{
				optional = 0;
ProcessInclude:
				w = ch_getword_conf(parms->pool, &args);

				if (*w == '\0' || *args != 0)
				{
					ch_cfg_closefile(parms->config_file);
					errmsg = apr_pstrcat(parms->pool, "Include takes one argument", NULL);
					goto Exit;
				}

				incpath = w;

				/* locate the start of the directories proper */
				status = apr_filepath_root((const char**)&rootpath, (const char**)&incpath, APR_FILEPATH_TRUENAME | APR_FILEPATH_NATIVE, ptemp);

				/* we allow APR_SUCCESS and APR_EINCOMPLETE */
				if (APR_ERELATIVE == status) {
					rootpath = apr_pstrdup(ptemp, parms->config_file->name);
					li = strlen(rootpath) - 1;

					while(li >= 0 && rootpath[li] != '/' && rootpath[li] != '\\')
						rootpath[li--] = 0;

					w = apr_pstrcat(p, rootpath, w, NULL);
				}
				else if (APR_EBADPATH == status) {
					ch_cfg_closefile(parms->config_file);
					errmsg = apr_pstrcat(p, "Include file has a bad path, ", w, NULL);
					goto Exit;
				}

				errmsg = ch_populate_include_files(p, ptemp, ari, w, optional);

				*(cmd_parms *)apr_array_push(arr) = *parms;

				if(errmsg != NULL)
					goto Exit;

				// we don't want to close the current file yet
				//
				parms = NULL;
				break;
			}

			cmd = ch_find_command(cmd_name, cmds);

			if(cmd == NULL)
			{
				// unknown command, should error
				//
				ch_cfg_closefile(parms->config_file);
				errmsg = apr_pstrcat(p, "Unknown command in config: ", cmd_name, NULL);
				goto Exit;
			}

				errmsg = invoke_cmd(cmd, parms, mconfig, args);

			if(errmsg != NULL)
				break;
		}

		if(parms != NULL)
			ch_cfg_closefile(parms->config_file);

		if(errmsg != NULL)
			break;
	}

    if (errmsg) {
		err = (char *)apr_palloc(p, 1024);

		if(parms != NULL)
			apr_snprintf(err, 1024, "Syntax error in config file %s, line %d: %s", parms->config_file->name,
							parms->config_file->line_number, errmsg);
		else
			apr_snprintf(err, 1024, "Syntax error in config file: %s", errmsg);
    }

    errmsg = err;

Exit:
	while((parms = (cmd_parms *)apr_array_pop(arr)) != NULL)
	{
		ch_cfg_closefile(parms->config_file);
	}

	return errmsg;
}

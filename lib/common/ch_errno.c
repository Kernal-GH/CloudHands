/*
 *
 *      Filename: ch_errno.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:35:16
 * Last Modified: 2017-01-11 10:46:51
 */

#include "ch_errno.h"
#include "ch_string.h"
/*
 * stuffbuffer - like ch_cpystrn() but returns the address of the
 * dest buffer instead of the address of the terminating '\0'
 */
static char *stuffbuffer(char *buf, size_t bufsize, const char *s)
{
    ch_cpystrn(buf,s,bufsize);
    return buf;
}

static char *ch_error_string(int statcode)
{
    switch (statcode) {
    case CH_ENOSTAT:
        return "Could not perform a stat on the file.";
    case CH_ENOPOOL:
        return "A new pool could not be created.";
    case CH_EBADDATE:
        return "An invalid date has been provided";
    case CH_EINVALSOCK:
        return "An invalid socket was returned";
    case CH_ENOPROC:
        return "No process was provided and one was required.";
    case CH_ENOTIME:
        return "No time was provided and one was required.";
    case CH_ENODIR:
        return "No directory was provided and one was required.";
    case CH_ENOLOCK:
        return "No lock was provided and one was required.";
    case CH_ENOPOLL:
        return "No poll structure was provided and one was required.";
    case CH_ENOSOCKET:
        return "No socket was provided and one was required.";
    case CH_ENOTHREAD:
        return "No thread was provided and one was required.";
    case CH_ENOTHDKEY:
        return "No thread key structure was provided and one was required.";
    case CH_ENOSHMAVAIL:
        return "No shared memory is currently available";
    case CH_EDSOOPEN:
        return "DSO load failed";
    case CH_EBADIP:
        return "The specified IP address is invalid.";
    case CH_EBADMASK:
        return "The specified network mask is invalid.";
    case CH_ESYMNOTFOUND:
        return "Could not find the requested symbol.";
    case CH_ENOTENOUGHENTROPY:
        return "Not enough entropy to continue.";
    case CH_INCHILD:
        return
	    "Your code just forked, and you are currently executing in the "
	    "child process";
    case CH_INPARENT:
        return
	    "Your code just forked, and you are currently executing in the "
	    "parent process";
    case CH_DETACH:
        return "The specified thread is detached";
    case CH_NOTDETACH:
        return "The specified thread is not detached";
    case CH_CHILD_DONE:
        return "The specified child process is done executing";
    case CH_CHILD_NOTDONE:
        return "The specified child process is not done executing";
    case CH_TIMEUP:
        return "The timeout specified has expired";
    case CH_INCOMPLETE:
        return "Partial results are valid but processing is incomplete";
    case CH_BADCH:
        return "Bad character specified on command line";
    case CH_BADARG:
        return "Missing parameter for the specified command line option";
    case CH_EOF:
        return "End of file found";
    case CH_NOTFOUND:
        return "Could not find specified socket in poll list.";
    case CH_ANONYMOUS:
        return "Shared memory is implemented anonymously";
    case CH_FILEBASED:
        return "Shared memory is implemented using files";
    case CH_KEYBASED:
        return "Shared memory is implemented using a key system";
    case CH_EINIT:
        return
	    "There is no error, this value signifies an initialized "
	    "error code";
    case CH_ENOTIMPL:
        return "This function has not been implemented on this platform";
    case CH_EMISMATCH:
        return "passwords do not match";
    case CH_EABSOLUTE:
        return "The given path is absolute";
    case CH_ERELATIVE:
        return "The given path is relative";
    case CH_EINCOMPLETE:
        return "The given path is incomplete";
    case CH_EABOVEROOT:
        return "The given path was above the root path";
    case CH_EBADPATH:
        return "The given path is misformatted or contained invalid characters";
    case CH_EPATHWILD:
        return "The given path contained wildcard characters";
    case CH_EBUSY:
        return "The given lock was busy.";
    case CH_EPROC_UNKNOWN:
        return "The process is not recognized.";
    case CH_EGENERAL:
        return "Internal error (specific information not available)";
    default:
        return "Error string not specified yet";
    }
}



/* On Unix, ch_os_strerror() handles error codes from the resolver 
 * (h_errno). 
 */
static char *ch_os_strerror(char* buf, size_t bufsize, int err) 
{
    const char *msg = "";
	err = err;

    return stuffbuffer(buf, bufsize, msg);
}


/* plain old strerror(); 
 * thread-safe on some platforms (e.g., Solaris, OS/390)
 */
static char *native_strerror(int statcode, char *buf,
                             size_t bufsize)
{
    const char *err = strerror(statcode);
    if (err) {
        return stuffbuffer(buf, bufsize, err);
    } else {
        return stuffbuffer(buf, bufsize, 
                           "GW does not understand this error code");
    }
}

char * ch_strerror(int statcode, char *buf,
                                 size_t bufsize)
{
    if (statcode < CH_OS_START_ERROR) {
        return native_strerror(statcode, buf, bufsize);
    }
    else if (statcode < CH_OS_START_USERERR) {
        return stuffbuffer(buf, bufsize, ch_error_string(statcode));
    }
    else if (statcode < CH_OS_START_EAIERR) {
        return stuffbuffer(buf, bufsize, "GW does not understand this error code");
    }
    else if (statcode < CH_OS_START_SYSERR) {
#if defined(HAVE_GAI_STRERROR)
        statcode -= CH_OS_START_EAIERR;
#if defined(NEGATIVE_EAI)
        statcode = -statcode;
#endif
        return stuffbuffer(buf, bufsize, gai_strerror(statcode));
#else
        return stuffbuffer(buf, bufsize, "GW does not understand this error code");
#endif
    }
    else {
        return ch_os_strerror(buf, bufsize, statcode - CH_OS_START_SYSERR);
    }
}



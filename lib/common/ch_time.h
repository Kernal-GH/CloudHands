/*
 *
 *      Filename: ch_time.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:26:54
 * Last Modified: 2017-01-11 14:26:54
 */

#ifndef CH_TIME_H
#define CH_TIME_H

#include "ch_constants.h"
#include "ch_mpool.h"
/** month names */
extern const char ch_month_snames[12][4];
/** day names */
extern const char ch_day_snames[7][4];


/** number of microseconds since 00:00:00 January 1, 1970 UTC */
typedef int64_t ch_time_t;


/** mechanism to properly type ch_time_t literals */
#define CH_TIME_C(val) (val ## L)

/** mechanism to properly print ch_time_t values */
#define CH_TIME_T_FMT CH_INT64_T_FMT

/** intervals for I/O timeouts, in microseconds */
typedef int64_t ch_interval_time_t;
/** short interval for I/O timeouts, in microseconds */
typedef int32_t ch_short_interval_time_t;

/** number of microseconds per second */
#define CH_USEC_PER_SEC 1000000L

/** @return ch_time_t as a second */
#define ch_time_sec(time) ((time) / CH_USEC_PER_SEC)

/** @return ch_time_t as a usec */
#define ch_time_usec(time) ((time) % CH_USEC_PER_SEC)

/** @return ch_time_t as a msec */
#define ch_time_msec(time) (((time) / 1000) % 1000)

/** @return ch_time_t as a msec */
#define ch_time_as_msec(time) ((time) / 1000)

/** @return milliseconds as an ch_time_t */
#define ch_time_from_msec(msec) ((ch_time_t)(msec) * 1000)

/** @return seconds as an ch_time_t */
#define ch_time_from_sec(sec) ((ch_time_t)(sec) * CH_USEC_PER_SEC)

/** @return a second and usec combination as an ch_time_t */
#define ch_time_make(sec, usec) ((ch_time_t)(sec) * CH_USEC_PER_SEC \
                                + (ch_time_t)(usec))

/**
 * @return the current time
 */
ch_time_t ch_time_now(void);

/** @see ch_time_exp_t */
typedef struct ch_time_exp_t ch_time_exp_t;

/**
 * a structure similar to ANSI struct tm with the following differences:
 *  - tm_usec isn't an ANSI field
 *  - tm_gmtoff isn't an ANSI field (it's a BSDism)
 */
struct ch_time_exp_t {
    /** microseconds past tm_sec */
    int32_t tm_usec;
    /** (0-61) seconds past tm_min */
    int32_t tm_sec;
    /** (0-59) minutes past tm_hour */
    int32_t tm_min;
    /** (0-23) hours past midnight */
    int32_t tm_hour;
    /** (1-31) day of the month */
    int32_t tm_mday;
    /** (0-11) month of the year */
    int32_t tm_mon;
    /** year since 1900 */
    int32_t tm_year;
    /** (0-6) days since Sunday */
    int32_t tm_wday;
    /** (0-365) days since January 1 */
    int32_t tm_yday;
    /** daylight saving time */
    int32_t tm_isdst;
    /** seconds east of UTC */
    int32_t tm_gmtoff;
};

/**
 * Convert an ansi time_t to an ch_time_t
 * @param result the resulting ch_time_t
 * @param input the time_t to convert
 */
int ch_time_ansi_put(ch_time_t *result, 
                                                    time_t input);

/**
 * Convert a time to its human readable components using an offset
 * from GMT.
 * @param result the exploded time
 * @param input the time to explode
 * @param offs the number of seconds offset to apply
 */
int ch_time_exp_tz(ch_time_exp_t *result,
                                          ch_time_t input,
                                          int32_t offs);

/**
 * Convert a time to its human readable components (GMT).
 * @param result the exploded time
 * @param input the time to explode
 */
int ch_time_exp_gmt(ch_time_exp_t *result, 
                                           ch_time_t input);

/**
 * Convert a time to its human readable components in the local timezone.
 * @param result the exploded time
 * @param input the time to explode
 */
int ch_time_exp_lt(ch_time_exp_t *result, 
                                          ch_time_t input);

/**
 * Convert time value from human readable format to a numeric ch_time_t
 * (elapsed microseconds since the epoch).
 * @param result the resulting imploded time
 * @param input the input exploded time
 */
int ch_time_exp_get(ch_time_t *result, 
                                           ch_time_exp_t *input);

/**
 * Convert time value from human readable format to a numeric ch_time_t that
 * always represents GMT.
 * @param result the resulting imploded time
 * @param input the input exploded time
 */
int ch_time_exp_gmt_get(ch_time_t *result, 
                                               ch_time_exp_t *input);

/**
 * Sleep for the specified number of micro-seconds.
 * @param t desired amount of time to sleep.
 * @warning May sleep for longer than the specified time. 
 */
void ch_sleep(ch_interval_time_t t);

/** length of a RFC822 Date */
#define CH_RFC822_DATE_LEN (30)
/**
 * ch_rfc822_date formats dates in the RFC822
 * format in an efficient manner.  It is a fixed length
 * format which requires CH_RFC822_DATA_LEN bytes of storage,
 * including the trailing NUL terminator.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
int ch_rfc822_date(char *date_str, ch_time_t t);

/** length of a CTIME date */
#define CH_CTIME_LEN (25)
/**
 * ch_ctime formats dates in the ctime() format
 * in an efficient manner.  It is a fixed length format
 * and requires CH_CTIME_LEN bytes of storage including
 * the trailing NUL terminator.
 * Unlike ANSI/ISO C ctime(), ch_ctime() does not include
 * a \\n at the end of the string.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
int ch_ctime(char *date_str, ch_time_t t);

/**
 * Formats the exploded time according to the format specified
 * @param s string to write to
 * @param retsize The length of the returned string
 * @param max The maximum length of the string
 * @param format The format for the time string
 * @param tm The time to convert
 */
int ch_strftime(char *s, size_t *retsize, 
                                       size_t max, const char *format, 
                                       ch_time_exp_t *tm);

/**
 * Improve the clock resolution for the lifetime of the given pool.
 * Generally this is only desirable on benchmarking and other very
 * time-sensitive applications, and has no impact on most platforms.
 * @param p The pool to associate the finer clock resolution 
 */
void ch_time_clock_hires(ch_pool_t *p);


#endif /* CH_TIME_H */

/*
 *
 *      Filename: ch_time.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:33:45
 * Last Modified: 2017-01-11 14:54:46
 */

#include "ch_time.h"
#include "ch_mpool.h"
#include "ch_errno.h"

#if !defined(HAVE_STRUCT_TM_TM_GMTOFF) && !defined(HAVE_STRUCT_TM___TM_GMTOFF)
static int32_t server_gmt_offset;
#define NO_GMTOFF_IN_STRUCT_TM
#endif          

static int32_t get_offset(struct tm *tm)
{
#if defined(HAVE_STRUCT_TM_TM_GMTOFF)
    return tm->tm_gmtoff;
#elif defined(HAVE_STRUCT_TM___TM_GMTOFF)
    return tm->__tm_gmtoff;
#else

    if (tm->tm_isdst)
        return server_gmt_offset + 3600;
    return server_gmt_offset;
#endif
}

int ch_time_ansi_put(ch_time_t *result,
                                            time_t input)
{
    *result = (ch_time_t)input * CH_USEC_PER_SEC;
    return 0;
}

/* NB NB NB NB This returns GMT!!!!!!!!!! */
ch_time_t ch_time_now(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * CH_USEC_PER_SEC + tv.tv_usec;
}

static void explode_time(ch_time_exp_t *xt, ch_time_t t,
                         int32_t offset, int use_localtime)
{
    struct tm tm;
    time_t tt = (t / CH_USEC_PER_SEC) + offset;
    xt->tm_usec = t % CH_USEC_PER_SEC;

    if (use_localtime)
        tm = *localtime(&tt);
    else
        tm = *gmtime(&tt);

    xt->tm_sec  = tm.tm_sec;
    xt->tm_min  = tm.tm_min;
    xt->tm_hour = tm.tm_hour;
    xt->tm_mday = tm.tm_mday;
    xt->tm_mon  = tm.tm_mon;
    xt->tm_year = tm.tm_year;
    xt->tm_wday = tm.tm_wday;
    xt->tm_yday = tm.tm_yday;
    xt->tm_isdst = tm.tm_isdst;
    xt->tm_gmtoff = get_offset(&tm);
}

int ch_time_exp_tz(ch_time_exp_t *result,
                                          ch_time_t input, int32_t offs)
{
    explode_time(result, input, offs, 0);
    result->tm_gmtoff = offs;
    return 0;
}

int ch_time_exp_gmt(ch_time_exp_t *result,
                                           ch_time_t input)
{
    return ch_time_exp_tz(result, input, 0);
}

int ch_time_exp_lt(ch_time_exp_t *result,
                                                ch_time_t input)
{
#if defined(__EMX__)
    /* EMX gcc (OS/2) has a timezone global we can use */
    return ch_time_exp_tz(result, input, -timezone);
#else
    explode_time(result, input, 0, 1);
    return 0;
#endif /* __EMX__ */
}

int ch_time_exp_get(ch_time_t *t, ch_time_exp_t *xt)
{
    ch_time_t year = xt->tm_year;
    ch_time_t days;
    static const int dayoffset[12] =
    {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

    /* shift new year to 1st March in order to make leap year calc easy */

    if (xt->tm_mon < 2)
        year--;

    /* Find number of days since 1st March 1900 (in the Gregorian calendar). */

    days = year * 365 + year / 4 - year / 100 + (year / 100 + 3) / 4;
    days += dayoffset[xt->tm_mon] + xt->tm_mday - 1;
    days -= 25508;              /* 1 jan 1970 is 25508 days since 1 mar 1900 */
    days = ((days * 24 + xt->tm_hour) * 60 + xt->tm_min) * 60 + xt->tm_sec;

    if (days < 0) {
        return CH_EBADDATE;
    }
    *t = days * CH_USEC_PER_SEC + xt->tm_usec;
    return 0;
}

int ch_time_exp_gmt_get(ch_time_t *t, 
                                               ch_time_exp_t *xt)
{
    int status = ch_time_exp_get(t, xt);
    if (status == 0)
        *t -= (ch_time_t) xt->tm_gmtoff * CH_USEC_PER_SEC;
    return status;
}



void ch_sleep(ch_interval_time_t t)
{
    struct timeval tv;
    tv.tv_usec = t % CH_USEC_PER_SEC;
    tv.tv_sec = t / CH_USEC_PER_SEC;
    select(0, NULL, NULL, NULL, &tv);
}


void ch_unix_setup_time(void)
{
#ifdef NO_GMTOFF_IN_STRUCT_TM
    /* Precompute the offset from GMT on systems where it's not
       in struct tm.

       Note: This offset is normalized to be independent of daylight
       savings time; if the calculation happens to be done in a
       time/place where a daylight savings adjustment is in effect,
       the returned offset has the same value that it would have
       in the same location if daylight savings were not in effect.
       The reason for this is that the returned offset can be
       applied to a past or future timestamp in explode_time(),
       so the DST adjustment obtained from the current time won't
       necessarily be applicable.

       mktime() is the inverse of localtime(); so, presumably,
       passing in a struct tm made by gmtime() let's us calculate
       the true GMT offset. However, there's a catch: if daylight
       savings is in effect, gmtime()will set the tm_isdst field
       and confuse mktime() into returning a time that's offset
       by one hour. In that case, we must adjust the calculated GMT
       offset.

     */

    struct timeval now;
    time_t t1, t2;
    struct tm t;

    gettimeofday(&now, NULL);
    t1 = now.tv_sec;
    t2 = 0;

#if CH_HAS_THREADS && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    gmtime_r(&t1, &t);
#else
    t = *gmtime(&t1);
#endif
    t.tm_isdst = 0; /* we know this GMT time isn't daylight-savings */
    t2 = mktime(&t);
    server_gmt_offset = (int32_t) difftime(t1, t2);
#endif /* NO_GMTOFF_IN_STRUCT_TM */
}


/* A noop on all known Unix implementations */
void ch_time_clock_hires(ch_pool_t *p)
{
	p = p;
    return;
}



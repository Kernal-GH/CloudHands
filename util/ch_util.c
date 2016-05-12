/*
 * =====================================================================================
 *
 *       Filename:  ch_util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月26日 16时45分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <apr_time.h>
#include <apr_strings.h>
#include "ch_util.h"

unsigned long ch_get_current_timems(void)
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

char *ch_current_filetime(apr_pool_t *mp) {
    apr_time_exp_t t;
    char tstr[100];
    size_t len;

    apr_time_exp_lt(&t, apr_time_now());

    apr_strftime(tstr, &len, 80, "%Y%m%d-%H%M%S", &t);
    return apr_pstrdup(mp, tstr);
}

char  *ch_current_logtime_with_buf(char *buf,size_t buf_size){

	apr_time_exp_t t;

    size_t len;

    if(buf_size<100)
        return "";

    apr_time_exp_lt(&t, apr_time_now());

    apr_strftime(buf, &len, 80, "%d/%b/%Y:%H:%M:%S ", &t);

    apr_snprintf(buf + strlen(buf), 80 - strlen(buf), "%c%.2d%.2d",
            t.tm_gmtoff < 0 ? '-' : '+',
            t.tm_gmtoff / (60 * 60), (t.tm_gmtoff / 60) % 60);

    return buf;
}

char *ch_current_logtime(apr_pool_t *mp) {
    apr_time_exp_t t;
    char tstr[100];
    size_t len;

    apr_time_exp_lt(&t, apr_time_now());

    apr_strftime(tstr, &len, 80, "%d/%b/%Y:%H:%M:%S ", &t);
    apr_snprintf(tstr + strlen(tstr), 80 - strlen(tstr), "%c%.2d%.2d",
            t.tm_gmtoff < 0 ? '-' : '+',
            t.tm_gmtoff / (60 * 60), (t.tm_gmtoff / 60) % 60);
    return apr_pstrdup(mp, tstr);
}

char * ch_macaddr_str_get(char *buf,size_t blen,struct ether_addr *maddr){

    memset(buf,0,blen);

    snprintf(buf,blen,"%02X:%02X:%02X:%02X:%02X:%02X",
                maddr->addr_bytes[0],
                maddr->addr_bytes[1],
                maddr->addr_bytes[2],
                maddr->addr_bytes[3],
                maddr->addr_bytes[4],
                maddr->addr_bytes[5]);

    return buf;
}

static const char * const fast_strings[] = {
"0", "1", "2", "3", "4", "5", "6", "7",
"8", "9", "10", "11", "12", "13", "14", "15",
"16", "17", "18", "19", "20", "21", "22", "23",
"24", "25", "26", "27", "28", "29", "30", "31",
"32", "33", "34", "35", "36", "37", "38", "39",
"40", "41", "42", "43", "44", "45", "46", "47",
"48", "49", "50", "51", "52", "53", "54", "55",
"56", "57", "58", "59", "60", "61", "62", "63",
"64", "65", "66", "67", "68", "69", "70", "71",
"72", "73", "74", "75", "76", "77", "78", "79",
"80", "81", "82", "83", "84", "85", "86", "87",
"88", "89", "90", "91", "92", "93", "94", "95",
"96", "97", "98", "99", "100", "101", "102", "103",
"104", "105", "106", "107", "108", "109", "110", "111",
"112", "113", "114", "115", "116", "117", "118", "119",
"120", "121", "122", "123", "124", "125", "126", "127",
"128", "129", "130", "131", "132", "133", "134", "135",
"136", "137", "138", "139", "140", "141", "142", "143",
"144", "145", "146", "147", "148", "149", "150", "151",
"152", "153", "154", "155", "156", "157", "158", "159",
"160", "161", "162", "163", "164", "165", "166", "167",
"168", "169", "170", "171", "172", "173", "174", "175",
"176", "177", "178", "179", "180", "181", "182", "183",
"184", "185", "186", "187", "188", "189", "190", "191",
"192", "193", "194", "195", "196", "197", "198", "199",
"200", "201", "202", "203", "204", "205", "206", "207",
"208", "209", "210", "211", "212", "213", "214", "215",
"216", "217", "218", "219", "220", "221", "222", "223",
"224", "225", "226", "227", "228", "229", "230", "231",
"232", "233", "234", "235", "236", "237", "238", "239",
"240", "241", "242", "243", "244", "245", "246", "247",
"248", "249", "250", "251", "252", "253", "254", "255"
};

static void
ip_to_str_buf(const uint8_t *ad, char *buf, const int buf_len)
{
    register char const *p;
    register char *b=buf;

    p=fast_strings[*ad++];
    do {
        *b++=*p;
        p++;
    } while(*p);
    *b++='.';

    p=fast_strings[*ad++];
    do {
        *b++=*p;
        p++;
    } while(*p);
    *b++='.';

    p=fast_strings[*ad++];
    do {
        *b++=*p;
        p++;
    } while(*p);
    *b++='.';

    p=fast_strings[*ad];
    do {
        *b++=*p;
        p++;
    } while(*p);
    *b=0;
}

#define MAX_IP_STR_LEN 16

char * ch_ipaddr_str_get(char *buf,size_t blen,uint32_t addr){

    if(blen < MAX_IP_STR_LEN ){
        return "0.0.0.0";
    }
    ip_to_str_buf((const uint8_t*)&addr,buf,blen);
}


static inline unsigned short from32to16(unsigned int x)
{
    /* add up 16-bit and 16-bit for 16+c bit */
    x = (x & 0xffff) + (x >> 16);
    /* add up carry.. */
    x = (x & 0xffff) + (x >> 16);
    return x;
}

static unsigned int do_csum(const unsigned char *buff, int len)
{
    int odd;
    unsigned int result = 0;
    if (len <= 0)
          goto out;
    odd = 1 & (unsigned long) buff;
    
    if (odd) {
          result += (*buff << 8);
          len--;
          buff++;
    }
    
    if (len >= 2) {
        if (2 & (unsigned long) buff) {
            result += *(unsigned short *) buff;
            len -= 2;
            buff += 2;
        }
        if (len >= 4) {
            const unsigned char *end = buff + ((unsigned)len & ~3);
            unsigned int carry = 0;
            do {
                unsigned int w = *(unsigned int *) buff;
                buff += 4;
                result += carry;
                result += w;
                carry = (w > result);
          } while (buff < end);
            result += carry;
            result = (result & 0xffff) + (result >> 16);
        }
        if (len & 2) {
            result += *(unsigned short *) buff;
            buff += 2;
        }
    }
    
    if (len & 1)
        result += *buff;
    
    result = from32to16(result);
    if (odd)
        result = ((result >> 8) & 0xff) | ((result & 0xff) << 8);
out:
  return result;
}

/*
 * compute ip header check sum!  
*/
uint16_t ch_ip_fast_csum(const void *iph, unsigned int ihl)
{
    return (uint16_t)~do_csum(iph, ihl*4);
}

int ch_dir_make(const char *orig_path)
{
	char *s;
	char c;
	struct stat st;

    char path[1024]={0};

    char *p = strrchr(orig_path,'/');

    if(p==NULL) return 0;

    memcpy(path,orig_path,p-orig_path);

    s = path;

	while (1) {
		c = '\0';

        /* Bypass leading non-'/'s and then subsequent '/'s. */
        while (*s) {
            if (*s == '/') {
                do {
                    ++s;
                } while (*s == '/');
                c = *s; /* Save the current char */
                *s = '\0'; /* and replace it with nul. */
                break;
            }
            ++s;
        }

		if (mkdir(path, 0777) < 0) {
			/* If we failed for any other reason than the directory
			 * already exists, output a diagnostic and return -1. */
			if (errno != EEXIST
			 || ((stat(path, &st) < 0) || !S_ISDIR(st.st_mode))
			) {
				break;
			}
			/* Since the directory exists, don't attempt to change
			 * permissions if it was the full target.  Note that
			 * this is not an error condition. */
			if (!c) {
				return 0;
			}
		}

		if (!c) {
			/* Done. */
			return 0;
		}

		/* Remove any inserted nul from the path (recursive mode). */
		*s = c;
	} /* while (1) */

	return -1;
}

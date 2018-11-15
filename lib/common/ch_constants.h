/*
 *
 *      Filename: ch_constants.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 10:16:13
 * Last Modified: 2017-01-09 10:16:13
 */

#ifndef CH_CONSTANTS
#define CH_CONSTANTS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#ifndef asm
#define asm __asm__
#endif

#define CH_OK 0
#define CH_ERROR -1
#define CH_AGAIN -2
#define CH_DONE  -3

#define CH_MAX_SIZE_T_VALUE  9223372036854775807LL
#define CH_MAX_OFF_T_VALUE  9223372036854775807LL
#define CH_UINT64_MAX_VALUE_LEN sizeof("9223372036854775807")

#define PKT_POOL_NAME "pkt_pool_name"
#define SA_POOL_NAME  "sa_pool_name"

typedef struct ch_memory_pool_t ch_memory_pool_t;
typedef uint64_t phys_addr_t;

/**
 * container_of - 根据锚成员指针获取包含其的结构的指针
 *
 * @param ptr    锚成员指针
 * @param type   包含锚成员的结构类型
 * @param member 锚成员在包含其的结构中的名字
 */

#ifndef container_of
#define container_of(ptr, type, member) ({          \
    typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif
#define ch_align(d,a) (((d) + (a - 1)) & ~(a - 1))

#define ch_align_ptr(p, a) \
	(void *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))   

#define ch_inline __attribute__ ((always_inline))

#ifndef CH_ALIGNMENT
#define CH_ALIGNMENT sizeof(unsigned long)
#endif

#ifndef CR
#define CR '\r'
#endif 

#ifndef CRLF
#define CRLF "\r\n"
#endif

#ifndef LF
#define LF '\n'
#endif

#define SOCKET_ID_ANY -1                    /**< Any NUMA socket. */
#define CH_CACHE_LINE_SIZE 64
#define CH_CACHE_LINE_MASK (CH_CACHE_LINE_SIZE-1) /**< Cache line mask. */

#define CH_CACHE_LINE_ROUNDUP(size) \
	(CH_CACHE_LINE_SIZE * ((size + CH_CACHE_LINE_SIZE - 1) / CH_CACHE_LINE_SIZE))
/**< Return the first cache-aligned value greater or equal to size. */

/**< Cache line size in terms of log2 */
#if CH_CACHE_LINE_SIZE == 64
#define CH_CACHE_LINE_SIZE_LOG2 6
#elif CH_CACHE_LINE_SIZE == 128
#define CH_CACHE_LINE_SIZE_LOG2 7
#else
#error "Unsupported cache line size"
#endif

#define CH_CACHE_LINE_MIN_SIZE 64	/**< Minimum Cache line size. */

enum ch_page_sizes {
	CH_PGSIZE_4K    = 1ULL << 12,
	CH_PGSIZE_64K   = 1ULL << 16,
	CH_PGSIZE_256K  = 1ULL << 18,
	CH_PGSIZE_2M    = 1ULL << 21,
	CH_PGSIZE_16M   = 1ULL << 24,
	CH_PGSIZE_256M  = 1ULL << 28,
	CH_PGSIZE_512M  = 1ULL << 29,
	CH_PGSIZE_1G    = 1ULL << 30,
	CH_PGSIZE_4G    = 1ULL << 32,
	CH_PGSIZE_16G   = 1ULL << 34,
};


/**
 * Force alignment to cache line.
 */
#define __ch_cache_aligned __ch_aligned(CH_CACHE_LINE_SIZE)

/**
 * Force minimum cache line alignment.
 */
#define __ch_cache_min_aligned __ch_aligned(CH_CACHE_LINE_MIN_SIZE)

#define CH_BAD_PHYS_ADDR ((phys_addr_t)-1)


/** C extension macro for environments lacking C11 features. */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define CH_STD_C11 __extension__
#else
#define CH_STD_C11
#endif

#define CH_MAX_MEMSEG 8096
#define CH_MAX_NUMA_NODES 64
#define MAX_HUGEPAGE_SIZES 3  /**< support up to 3 page sizes */

/**
 * Force alignment
 */
#define __ch_aligned(a) __attribute__((__aligned__(a)))

/**
 * Force a structure to be packed
 */
#define __ch_packed __attribute__((__packed__))

/******* Macro to mark functions and fields scheduled for removal *****/
#define __ch_deprecated	__attribute__((__deprecated__))

/*********** Macros to eliminate unused variable warnings ********/

/**
 * short definition to mark a function parameter unused
 */
#define ch_unused __attribute__((__unused__))

/**
 * definition to mark a variable or function parameter as used so
 * as to avoid a compiler warning
 */
#define CH_SET_USED(x) (void)(x)

/*********** Macros for pointer arithmetic ********/

/**
 * add a byte-value offset from a pointer
 */
#define CH_PTR_ADD(ptr, x) ((void*)((uintptr_t)(ptr) + (x)))

/**
 * subtract a byte-value offset from a pointer
 */
#define CH_PTR_SUB(ptr, x) ((void*)((uintptr_t)ptr - (x)))

/**
 * get the difference between two pointer values, i.e. how far apart
 * in bytes are the locations they point two. It is assumed that
 * ptr1 is greater than ptr2.
 */
#define CH_PTR_DIFF(ptr1, ptr2) ((uintptr_t)(ptr1) - (uintptr_t)(ptr2))

/*********** Macros/static functions for doing alignment ********/


/**
 * Macro to align a pointer to a given power-of-two. The resultant
 * pointer will be a pointer of the same type as the first parameter, and
 * point to an address no higher than the first parameter. Second parameter
 * must be a power-of-two value.
 */
#define CH_PTR_ALIGN_FLOOR(ptr, align) \
	((typeof(ptr))CH_ALIGN_FLOOR((uintptr_t)ptr, align))

/**
 * Macro to align a value to a given power-of-two. The resultant value
 * will be of the same type as the first parameter, and will be no
 * bigger than the first parameter. Second parameter must be a
 * power-of-two value.
 */
#define CH_ALIGN_FLOOR(val, align) \
	(typeof(val))((val) & (~((typeof(val))((align) - 1))))

/**
 * Macro to align a pointer to a given power-of-two. The resultant
 * pointer will be a pointer of the same type as the first parameter, and
 * point to an address no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 */
#define CH_PTR_ALIGN_CEIL(ptr, align) \
	CH_PTR_ALIGN_FLOOR((typeof(ptr))CH_PTR_ADD(ptr, (align) - 1), align)

/**
 * Macro to align a value to a given power-of-two. The resultant value
 * will be of the same type as the first parameter, and will be no lower
 * than the first parameter. Second parameter must be a power-of-two
 * value.
 */
#define CH_ALIGN_CEIL(val, align) \
	CH_ALIGN_FLOOR(((val) + ((typeof(val)) (align) - 1)), align)

/**
 * Macro to align a pointer to a given power-of-two. The resultant
 * pointer will be a pointer of the same type as the first parameter, and
 * point to an address no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 * This function is the same as CH_PTR_ALIGN_CEIL
 */
#define CH_PTR_ALIGN(ptr, align) CH_PTR_ALIGN_CEIL(ptr, align)

/**
 * Macro to align a value to a given power-of-two. The resultant
 * value will be of the same type as the first parameter, and
 * will be no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 * This function is the same as CH_ALIGN_CEIL
 */
#define CH_ALIGN(val, align) CH_ALIGN_CEIL(val, align)

#define ch_align_up(d,a) (((d)+(a-1))&~(a-1))

/**
 * Checks if a pointer is aligned to a given power-of-two value
 *
 * @param ptr
 *   The pointer whose alignment is to be checked
 * @param align
 *   The power-of-two value to which the ptr should be aligned
 *
 * @return
 *   True(1) where the pointer is correctly aligned, false(0) otherwise
 */
static inline int
ch_is_aligned(void *ptr, unsigned align)
{
	return CH_PTR_ALIGN(ptr, align) == ptr;
}


#ifdef __SSE2__                                                                                                                 
#include <emmintrin.h>
/**
 * PAUSE instruction for tight loops (avoid busy waiting)
 */
static inline void
ch_pause (void)
{
	_mm_pause();
}
#else
static inline void
ch_pause(void) {}
#endif

/*********** Macros to work with powers of 2 ********/

/**
 * Returns true if n is a power of 2
 * @param n
 *     Number to check
 * @return 1 if true, 0 otherwise
 */
static inline int
ch_is_power_of_2(uint32_t n)
{
	return n && !(n & (n - 1));
}

/**
 * Aligns input parameter to the next power of 2
 *
 * @param x
 *   The integer value to algin
 *
 * @return
 *   Input parameter aligned to the next power of 2
 */
static inline uint32_t
ch_align32pow2(uint32_t x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return x + 1;
}

/**
 * Aligns 64b input parameter to the next power of 2
 *
 * @param v
 *   The 64b value to align
 *
 * @return
 *   Input parameter aligned to the next power of 2
 */
static inline uint64_t
ch_align64pow2(uint64_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;

	return v + 1;
}

/*********** Macros for calculating min and max **********/

/**
 * Macro to return the minimum of two numbers
 */
#define CH_MIN(a, b) \
	__extension__ ({ \
		typeof (a) _a = (a); \
		typeof (b) _b = (b); \
		_a < _b ? _a : _b; \
	})

/**
 * Macro to return the maximum of two numbers
 */
#define CH_MAX(a, b) \
	__extension__ ({ \
		typeof (a) _a = (a); \
		typeof (b) _b = (b); \
		_a > _b ? _a : _b; \
	})

 /**                                                                                                                             
 * Converts a numeric string to the equivalent uint64_t value.                                                                  
 * As well as straight number conversion, also recognises the suffixes                                                          
 * k, m and g for kilobytes, megabytes and gigabytes respectively.                                                              
 *                                                                                                                              
 * If a negative number is passed in  i.e. a string with the first non-black                                                    
 * character being "-", zero is returned. Zero is also returned in the case of                                                  
 * an error with the strtoull call in the function.                                                                             
 *                                                                                                                              
 * @param str                                                                                                                   
 *     String containing number to convert.                                                                                     
 * @return                                                                                                                      
 *     Number.                                                                                                                  
 */                                                                                                                             
static inline uint64_t                                                                                                          
ch_str_to_size(const char *str)                                                                                                
{                                                                                                                               
    char *endptr;                                                                                                               
    unsigned long long size;                                                                                                    
                                                                                                                                
    while (isspace((int)*str))                                                                                                  
        str++;                                                                                                                  
    if (*str == '-')                                                                                                            
        return 0;                                                                                                               
                                                                                                                                
    errno = 0;                                                                                                                  
    size = strtoull(str, &endptr, 0);                                                                                           
    if (errno)                                                                                                                  
        return 0;                                                                                                               
                                                                                                                                
    if (*endptr == ' ')                                                                                                         
        endptr++; /* allow 1 space gap */                                                                                       
                                                                                                                                
    switch (*endptr){                                                                                                           
    case 'G': case 'g': size *= 1024; /* fall-through */                                                                        
    case 'M': case 'm': size *= 1024; /* fall-through */                                                                        
    case 'K': case 'k': size *= 1024; /* fall-through */                                                                        
    default:                                                                                                                    
        break;                                                                                                                  
    }                                                                                                                           
    return size;                                                                                                                
}
 
#endif /* CH_CONSTANTS */

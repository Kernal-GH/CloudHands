/*
 * =====================================================================================
 *
 *       Filename:  ch_compiler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月26日 12时43分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_COMPILER_H
#define CH_COMPILER_H

#ifdef __GNUC__

// Tacking on "\n\t#" to the section name makes gcc put it's bogus
// section attributes on what looce like a comment to the assembler.
#ifdef HAVE_SECTION_QUOTES
    #define __sec_comment "\"\n\t#\""
#else
    #define __sec_comment "\n\t#"
#endif

// unallocate .gnu.warning.SYMBOL section
#define HAVE_ASM_PREVIOUS_DIRECTIVE
#ifdef HAVE_ASM_PREVIOUS_DIRECTIVE
    #define __make_section_unallocated(section_string) asm (".section " section_string "\n\t.previous");
#elif defined HAVE_ASM_POPSECTION_DIRECTIVE
    #define __make_section_unallocated(section_string) asm (".pushsection " section_string "\n\t.popsection");
#else
    #define __make_section_unallocated(section_string)
#endif

#define link_warning(symbol, msg) \
    __make_section_unallocated(".gnu.warning." #symbol) \
    static const char __evoke_link_warning_##symbol[] \
    __attribute__ ((used, section (".gnu.warning." #symbol __sec_comment))) \
    = msg;

#ifdef __cplusplus
//#warning "TODO: implement __must_be_array in C++!"
#define __must_be_array(a) 0
#else
/**
 * 如果 a 为数组，则 __builtin_types_compatible_p(typeof(a), typeof(&a[0])) 返回 0
 */
#define __must_be_array(a) \
  BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0]))) //!< 确保 a 一定是数组：若 a 为数组，则 &a[0] 退化为指针
#endif

#if __GNUC__ >= 4
# define __compiler_offsetof(a,b) __builtin_offsetof(a,b)     //!< b 在 a 中的偏移量，gcc 内置函数
# define __wafd_inline__ __attribute__ ((__always_inline__))  //!< 总是内联
#elif __GNUC__ == 3 && __GNUC_MINOR__ >= 2
# define __wafd_inline__ __attribute__ ((always_inline))      //!< 总是内联
#else
# error Sorry, your compiler is too old/not recognized.
#endif

#define __wafd_deprecated__ __attribute__ ((deprecated))      //!< 过时的函数或类型
#define __wafd_unused__ __attribute__ ((unused))              //!< 函数或变量可能不被使用
#define __wafd_warn_unused_result__ \
    __attribute__ ((warn_unused_result))                      //!< 警告如果函数返回值未被使用
#endif

#define unused_var(x)        ((x) = (x)) //!< 技巧： 阻止编译器对未使用的变量告警，实际上不生成任何代码

/**
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted). 
 */
#ifndef BUILD_BUG_ON_ZERO
#define BUILD_BUG_ON_ZERO(e) (sizeof(char[1 - 2 * !!(e)]) - 1)                  //!< 如果 e 为真则编译会报错
#endif
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr)) //!< 数组的大小

#undef offsetof
#ifdef __compiler_offsetof
# define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)    //!< MEMBER 在 TYPE 中的偏移量
#else
# define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)    //!< MEMBER 在 TYPE 中的偏移量
#endif

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

#define prefetch(x)  __builtin_prefetch(x)   //!< 尝试读预取地址 x 对应的内存数据到CPU L1 cache
#define prefetchw(x) __builtin_prefetch(x,1) //!< 尝试写预取地址 x 对应的内存数据到CPU L1 cache

#endif /*CH_COMPILER_H*/

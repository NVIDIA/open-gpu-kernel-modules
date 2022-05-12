/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  S390 version
 *    Copyright IBM Corp. 1999
 *    Author(s): Martin Schwidefsky (schwidefsky@de.ibm.com),
 */

#ifndef _S390_STRING_H_
#define _S390_STRING_H_

#ifndef _LINUX_TYPES_H
#include <linux/types.h>
#endif

#define __HAVE_ARCH_MEMCPY	/* gcc builtin & arch function */
#define __HAVE_ARCH_MEMMOVE	/* gcc builtin & arch function */
#define __HAVE_ARCH_MEMSET	/* gcc builtin & arch function */
#define __HAVE_ARCH_MEMSET16	/* arch function */
#define __HAVE_ARCH_MEMSET32	/* arch function */
#define __HAVE_ARCH_MEMSET64	/* arch function */

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);

#ifndef CONFIG_KASAN
#define __HAVE_ARCH_MEMCHR	/* inline & arch function */
#define __HAVE_ARCH_MEMCMP	/* arch function */
#define __HAVE_ARCH_MEMSCAN	/* inline & arch function */
#define __HAVE_ARCH_STRCAT	/* inline & arch function */
#define __HAVE_ARCH_STRCMP	/* arch function */
#define __HAVE_ARCH_STRCPY	/* inline & arch function */
#define __HAVE_ARCH_STRLCAT	/* arch function */
#define __HAVE_ARCH_STRLCPY	/* arch function */
#define __HAVE_ARCH_STRLEN	/* inline & arch function */
#define __HAVE_ARCH_STRNCAT	/* arch function */
#define __HAVE_ARCH_STRNCPY	/* arch function */
#define __HAVE_ARCH_STRNLEN	/* inline & arch function */
#define __HAVE_ARCH_STRRCHR	/* arch function */
#define __HAVE_ARCH_STRSTR	/* arch function */

/* Prototypes for non-inlined arch strings functions. */
int memcmp(const void *s1, const void *s2, size_t n);
int strcmp(const char *s1, const char *s2);
size_t strlcat(char *dest, const char *src, size_t n);
size_t strlcpy(char *dest, const char *src, size_t size);
char *strncat(char *dest, const char *src, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *strrchr(const char *s, int c);
char *strstr(const char *s1, const char *s2);
#endif /* !CONFIG_KASAN */

#undef __HAVE_ARCH_STRCHR
#undef __HAVE_ARCH_STRNCHR
#undef __HAVE_ARCH_STRNCMP
#undef __HAVE_ARCH_STRPBRK
#undef __HAVE_ARCH_STRSEP
#undef __HAVE_ARCH_STRSPN

#if defined(CONFIG_KASAN) && !defined(__SANITIZE_ADDRESS__)

extern void *__memcpy(void *dest, const void *src, size_t n);
extern void *__memset(void *s, int c, size_t n);
extern void *__memmove(void *dest, const void *src, size_t n);

/*
 * For files that are not instrumented (e.g. mm/slub.c) we
 * should use not instrumented version of mem* functions.
 */

#define memcpy(dst, src, len) __memcpy(dst, src, len)
#define memmove(dst, src, len) __memmove(dst, src, len)
#define memset(s, c, n) __memset(s, c, n)
#define strlen(s) __strlen(s)

#define __no_sanitize_prefix_strfunc(x) __##x

#ifndef __NO_FORTIFY
#define __NO_FORTIFY /* FORTIFY_SOURCE uses __builtin_memcpy, etc. */
#endif

#else
#define __no_sanitize_prefix_strfunc(x) x
#endif /* defined(CONFIG_KASAN) && !defined(__SANITIZE_ADDRESS__) */

void *__memset16(uint16_t *s, uint16_t v, size_t count);
void *__memset32(uint32_t *s, uint32_t v, size_t count);
void *__memset64(uint64_t *s, uint64_t v, size_t count);

static inline void *memset16(uint16_t *s, uint16_t v, size_t count)
{
	return __memset16(s, v, count * sizeof(v));
}

static inline void *memset32(uint32_t *s, uint32_t v, size_t count)
{
	return __memset32(s, v, count * sizeof(v));
}

static inline void *memset64(uint64_t *s, uint64_t v, size_t count)
{
	return __memset64(s, v, count * sizeof(v));
}

#if !defined(IN_ARCH_STRING_C) && (!defined(CONFIG_FORTIFY_SOURCE) || defined(__NO_FORTIFY))

#ifdef __HAVE_ARCH_MEMCHR
static inline void *memchr(const void * s, int c, size_t n)
{
	register int r0 asm("0") = (char) c;
	const void *ret = s + n;

	asm volatile(
		"0:	srst	%0,%1\n"
		"	jo	0b\n"
		"	jl	1f\n"
		"	la	%0,0\n"
		"1:"
		: "+a" (ret), "+&a" (s) : "d" (r0) : "cc", "memory");
	return (void *) ret;
}
#endif

#ifdef __HAVE_ARCH_MEMSCAN
static inline void *memscan(void *s, int c, size_t n)
{
	register int r0 asm("0") = (char) c;
	const void *ret = s + n;

	asm volatile(
		"0:	srst	%0,%1\n"
		"	jo	0b\n"
		: "+a" (ret), "+&a" (s) : "d" (r0) : "cc", "memory");
	return (void *) ret;
}
#endif

#ifdef __HAVE_ARCH_STRCAT
static inline char *strcat(char *dst, const char *src)
{
	register int r0 asm("0") = 0;
	unsigned long dummy;
	char *ret = dst;

	asm volatile(
		"0:	srst	%0,%1\n"
		"	jo	0b\n"
		"1:	mvst	%0,%2\n"
		"	jo	1b"
		: "=&a" (dummy), "+a" (dst), "+a" (src)
		: "d" (r0), "0" (0) : "cc", "memory" );
	return ret;
}
#endif

#ifdef __HAVE_ARCH_STRCPY
static inline char *strcpy(char *dst, const char *src)
{
	register int r0 asm("0") = 0;
	char *ret = dst;

	asm volatile(
		"0:	mvst	%0,%1\n"
		"	jo	0b"
		: "+&a" (dst), "+&a" (src) : "d" (r0)
		: "cc", "memory");
	return ret;
}
#endif

#if defined(__HAVE_ARCH_STRLEN) || (defined(CONFIG_KASAN) && !defined(__SANITIZE_ADDRESS__))
static inline size_t __no_sanitize_prefix_strfunc(strlen)(const char *s)
{
	register unsigned long r0 asm("0") = 0;
	const char *tmp = s;

	asm volatile(
		"0:	srst	%0,%1\n"
		"	jo	0b"
		: "+d" (r0), "+a" (tmp) :  : "cc", "memory");
	return r0 - (unsigned long) s;
}
#endif

#ifdef __HAVE_ARCH_STRNLEN
static inline size_t strnlen(const char * s, size_t n)
{
	register int r0 asm("0") = 0;
	const char *tmp = s;
	const char *end = s + n;

	asm volatile(
		"0:	srst	%0,%1\n"
		"	jo	0b"
		: "+a" (end), "+a" (tmp) : "d" (r0)  : "cc", "memory");
	return end - s;
}
#endif
#else /* IN_ARCH_STRING_C */
void *memchr(const void * s, int c, size_t n);
void *memscan(void *s, int c, size_t n);
char *strcat(char *dst, const char *src);
char *strcpy(char *dst, const char *src);
size_t strlen(const char *s);
size_t strnlen(const char * s, size_t n);
#endif /* !IN_ARCH_STRING_C */

#endif /* __S390_STRING_H_ */

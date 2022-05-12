// SPDX-License-Identifier: GPL-2.0
/*
 *    Optimized string functions
 *
 *  S390 version
 *    Copyright IBM Corp. 2004
 *    Author(s): Martin Schwidefsky (schwidefsky@de.ibm.com)
 */

#define IN_ARCH_STRING_C 1

#include <linux/types.h>
#include <linux/string.h>
#include <linux/export.h>

/*
 * Helper functions to find the end of a string
 */
static inline char *__strend(const char *s)
{
	register unsigned long r0 asm("0") = 0;

	asm volatile ("0: srst  %0,%1\n"
		      "   jo    0b"
		      : "+d" (r0), "+a" (s) :  : "cc", "memory");
	return (char *) r0;
}

static inline char *__strnend(const char *s, size_t n)
{
	register unsigned long r0 asm("0") = 0;
	const char *p = s + n;

	asm volatile ("0: srst  %0,%1\n"
		      "   jo    0b"
		      : "+d" (p), "+a" (s) : "d" (r0) : "cc", "memory");
	return (char *) p;
}

/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 *
 * returns the length of @s
 */
#ifdef __HAVE_ARCH_STRLEN
size_t strlen(const char *s)
{
	return __strend(s) - s;
}
EXPORT_SYMBOL(strlen);
#endif

/**
 * strnlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @n: The maximum number of bytes to search
 *
 * returns the minimum of the length of @s and @n
 */
#ifdef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *s, size_t n)
{
	return __strnend(s, n) - s;
}
EXPORT_SYMBOL(strnlen);
#endif

/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 *
 * returns a pointer to @dest
 */
#ifdef __HAVE_ARCH_STRCPY
char *strcpy(char *dest, const char *src)
{
	register int r0 asm("0") = 0;
	char *ret = dest;

	asm volatile ("0: mvst  %0,%1\n"
		      "   jo    0b"
		      : "+&a" (dest), "+&a" (src) : "d" (r0)
		      : "cc", "memory" );
	return ret;
}
EXPORT_SYMBOL(strcpy);
#endif

/**
 * strlcpy - Copy a %NUL terminated string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @size: size of destination buffer
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
#ifdef __HAVE_ARCH_STRLCPY
size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = __strend(src) - src;

	if (size) {
		size_t len = (ret >= size) ? size-1 : ret;
		dest[len] = '\0';
		memcpy(dest, src, len);
	}
	return ret;
}
EXPORT_SYMBOL(strlcpy);
#endif

/**
 * strncpy - Copy a length-limited, %NUL-terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @n: The maximum number of bytes to copy
 *
 * The result is not %NUL-terminated if the source exceeds
 * @n bytes.
 */
#ifdef __HAVE_ARCH_STRNCPY
char *strncpy(char *dest, const char *src, size_t n)
{
	size_t len = __strnend(src, n) - src;
	memset(dest + len, 0, n - len);
	memcpy(dest, src, len);
	return dest;
}
EXPORT_SYMBOL(strncpy);
#endif

/**
 * strcat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 *
 * returns a pointer to @dest
 */
#ifdef __HAVE_ARCH_STRCAT
char *strcat(char *dest, const char *src)
{
	register int r0 asm("0") = 0;
	unsigned long dummy;
	char *ret = dest;

	asm volatile ("0: srst  %0,%1\n"
		      "   jo    0b\n"
		      "1: mvst  %0,%2\n"
		      "   jo    1b"
		      : "=&a" (dummy), "+a" (dest), "+a" (src)
		      : "d" (r0), "0" (0UL) : "cc", "memory" );
	return ret;
}
EXPORT_SYMBOL(strcat);
#endif

/**
 * strlcat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @n: The size of the destination buffer.
 */
#ifdef __HAVE_ARCH_STRLCAT
size_t strlcat(char *dest, const char *src, size_t n)
{
	size_t dsize = __strend(dest) - dest;
	size_t len = __strend(src) - src;
	size_t res = dsize + len;

	if (dsize < n) {
		dest += dsize;
		n -= dsize;
		if (len >= n)
			len = n - 1;
		dest[len] = '\0';
		memcpy(dest, src, len);
	}
	return res;
}
EXPORT_SYMBOL(strlcat);
#endif

/**
 * strncat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @n: The maximum numbers of bytes to copy
 *
 * returns a pointer to @dest
 *
 * Note that in contrast to strncpy, strncat ensures the result is
 * terminated.
 */
#ifdef __HAVE_ARCH_STRNCAT
char *strncat(char *dest, const char *src, size_t n)
{
	size_t len = __strnend(src, n) - src;
	char *p = __strend(dest);

	p[len] = '\0';
	memcpy(p, src, len);
	return dest;
}
EXPORT_SYMBOL(strncat);
#endif

/**
 * strcmp - Compare two strings
 * @s1: One string
 * @s2: Another string
 *
 * returns   0 if @s1 and @s2 are equal,
 *	   < 0 if @s1 is less than @s2
 *	   > 0 if @s1 is greater than @s2
 */
#ifdef __HAVE_ARCH_STRCMP
int strcmp(const char *s1, const char *s2)
{
	register int r0 asm("0") = 0;
	int ret = 0;

	asm volatile ("0: clst %2,%3\n"
		      "   jo   0b\n"
		      "   je   1f\n"
		      "   ic   %0,0(%2)\n"
		      "   ic   %1,0(%3)\n"
		      "   sr   %0,%1\n"
		      "1:"
		      : "+d" (ret), "+d" (r0), "+a" (s1), "+a" (s2)
		      : : "cc", "memory");
	return ret;
}
EXPORT_SYMBOL(strcmp);
#endif

/**
 * strrchr - Find the last occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
#ifdef __HAVE_ARCH_STRRCHR
char *strrchr(const char *s, int c)
{
       size_t len = __strend(s) - s;

       if (len)
	       do {
		       if (s[len] == (char) c)
			       return (char *) s + len;
	       } while (--len > 0);
       return NULL;
}
EXPORT_SYMBOL(strrchr);
#endif

static inline int clcle(const char *s1, unsigned long l1,
			const char *s2, unsigned long l2)
{
	register unsigned long r2 asm("2") = (unsigned long) s1;
	register unsigned long r3 asm("3") = (unsigned long) l1;
	register unsigned long r4 asm("4") = (unsigned long) s2;
	register unsigned long r5 asm("5") = (unsigned long) l2;
	int cc;

	asm volatile ("0: clcle %1,%3,0\n"
		      "   jo    0b\n"
		      "   ipm   %0\n"
		      "   srl   %0,28"
		      : "=&d" (cc), "+a" (r2), "+a" (r3),
			"+a" (r4), "+a" (r5) : : "cc", "memory");
	return cc;
}

/**
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */
#ifdef __HAVE_ARCH_STRSTR
char *strstr(const char *s1, const char *s2)
{
	int l1, l2;

	l2 = __strend(s2) - s2;
	if (!l2)
		return (char *) s1;
	l1 = __strend(s1) - s1;
	while (l1-- >= l2) {
		int cc;

		cc = clcle(s1, l2, s2, l2);
		if (!cc)
			return (char *) s1;
		s1++;
	}
	return NULL;
}
EXPORT_SYMBOL(strstr);
#endif

/**
 * memchr - Find a character in an area of memory.
 * @s: The memory area
 * @c: The byte to search for
 * @n: The size of the area.
 *
 * returns the address of the first occurrence of @c, or %NULL
 * if @c is not found
 */
#ifdef __HAVE_ARCH_MEMCHR
void *memchr(const void *s, int c, size_t n)
{
	register int r0 asm("0") = (char) c;
	const void *ret = s + n;

	asm volatile ("0: srst  %0,%1\n"
		      "   jo    0b\n"
		      "   jl	1f\n"
		      "   la    %0,0\n"
		      "1:"
		      : "+a" (ret), "+&a" (s) : "d" (r0) : "cc", "memory");
	return (void *) ret;
}
EXPORT_SYMBOL(memchr);
#endif

/**
 * memcmp - Compare two areas of memory
 * @s1: One area of memory
 * @s2: Another area of memory
 * @n: The size of the area.
 */
#ifdef __HAVE_ARCH_MEMCMP
int memcmp(const void *s1, const void *s2, size_t n)
{
	int ret;

	ret = clcle(s1, n, s2, n);
	if (ret)
		ret = ret == 1 ? -1 : 1;
	return ret;
}
EXPORT_SYMBOL(memcmp);
#endif

/**
 * memscan - Find a character in an area of memory.
 * @s: The memory area
 * @c: The byte to search for
 * @n: The size of the area.
 *
 * returns the address of the first occurrence of @c, or 1 byte past
 * the area if @c is not found
 */
#ifdef __HAVE_ARCH_MEMSCAN
void *memscan(void *s, int c, size_t n)
{
	register int r0 asm("0") = (char) c;
	const void *ret = s + n;

	asm volatile ("0: srst  %0,%1\n"
		      "   jo    0b\n"
		      : "+a" (ret), "+&a" (s) : "d" (r0) : "cc", "memory");
	return (void *) ret;
}
EXPORT_SYMBOL(memscan);
#endif

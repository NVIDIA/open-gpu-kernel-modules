/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_


#include <linux/compiler.h>	/* for inline */
#include <linux/types.h>	/* for size_t */
#include <linux/stddef.h>	/* for NULL */
#include <linux/errno.h>	/* for E2BIG */
#include <stdarg.h>
#include <uapi/linux/string.h>

extern char *strndup_user(const char __user *, long);
extern void *memdup_user(const void __user *, size_t);
extern void *vmemdup_user(const void __user *, size_t);
extern void *memdup_user_nul(const void __user *, size_t);

/*
 * Include machine specific inline routines
 */
#include <asm/string.h>

#ifndef __HAVE_ARCH_STRCPY
extern char * strcpy(char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRNCPY
extern char * strncpy(char *,const char *, __kernel_size_t);
#endif
#ifndef __HAVE_ARCH_STRLCPY
size_t strlcpy(char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRSCPY
ssize_t strscpy(char *, const char *, size_t);
#endif

/* Wraps calls to strscpy()/memset(), no arch specific code required */
ssize_t strscpy_pad(char *dest, const char *src, size_t count);

#ifndef __HAVE_ARCH_STRCAT
extern char * strcat(char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCAT
extern char * strncat(char *, const char *, __kernel_size_t);
#endif
#ifndef __HAVE_ARCH_STRLCAT
extern size_t strlcat(char *, const char *, __kernel_size_t);
#endif
#ifndef __HAVE_ARCH_STRCMP
extern int strcmp(const char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRNCMP
extern int strncmp(const char *,const char *,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_STRCASECMP
extern int strcasecmp(const char *s1, const char *s2);
#endif
#ifndef __HAVE_ARCH_STRNCASECMP
extern int strncasecmp(const char *s1, const char *s2, size_t n);
#endif
#ifndef __HAVE_ARCH_STRCHR
extern char * strchr(const char *,int);
#endif
#ifndef __HAVE_ARCH_STRCHRNUL
extern char * strchrnul(const char *,int);
#endif
extern char * strnchrnul(const char *, size_t, int);
#ifndef __HAVE_ARCH_STRNCHR
extern char * strnchr(const char *, size_t, int);
#endif
#ifndef __HAVE_ARCH_STRRCHR
extern char * strrchr(const char *,int);
#endif
extern char * __must_check skip_spaces(const char *);

extern char *strim(char *);

static inline __must_check char *strstrip(char *str)
{
	return strim(str);
}

#ifndef __HAVE_ARCH_STRSTR
extern char * strstr(const char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNSTR
extern char * strnstr(const char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRLEN
extern __kernel_size_t strlen(const char *);
#endif
#ifndef __HAVE_ARCH_STRNLEN
extern __kernel_size_t strnlen(const char *,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_STRPBRK
extern char * strpbrk(const char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRSEP
extern char * strsep(char **,const char *);
#endif
#ifndef __HAVE_ARCH_STRSPN
extern __kernel_size_t strspn(const char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRCSPN
extern __kernel_size_t strcspn(const char *,const char *);
#endif

#ifndef __HAVE_ARCH_MEMSET
extern void * memset(void *,int,__kernel_size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET16
extern void *memset16(uint16_t *, uint16_t, __kernel_size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET32
extern void *memset32(uint32_t *, uint32_t, __kernel_size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET64
extern void *memset64(uint64_t *, uint64_t, __kernel_size_t);
#endif

static inline void *memset_l(unsigned long *p, unsigned long v,
		__kernel_size_t n)
{
	if (BITS_PER_LONG == 32)
		return memset32((uint32_t *)p, v, n);
	else
		return memset64((uint64_t *)p, v, n);
}

static inline void *memset_p(void **p, void *v, __kernel_size_t n)
{
	if (BITS_PER_LONG == 32)
		return memset32((uint32_t *)p, (uintptr_t)v, n);
	else
		return memset64((uint64_t *)p, (uintptr_t)v, n);
}

extern void **__memcat_p(void **a, void **b);
#define memcat_p(a, b) ({					\
	BUILD_BUG_ON_MSG(!__same_type(*(a), *(b)),		\
			 "type mismatch in memcat_p()");	\
	(typeof(*a) *)__memcat_p((void **)(a), (void **)(b));	\
})

#ifndef __HAVE_ARCH_MEMCPY
extern void * memcpy(void *,const void *,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_MEMMOVE
extern void * memmove(void *,const void *,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_MEMSCAN
extern void * memscan(void *,int,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_MEMCMP
extern int memcmp(const void *,const void *,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_BCMP
extern int bcmp(const void *,const void *,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_MEMCHR
extern void * memchr(const void *,int,__kernel_size_t);
#endif
#ifndef __HAVE_ARCH_MEMCPY_FLUSHCACHE
static inline void memcpy_flushcache(void *dst, const void *src, size_t cnt)
{
	memcpy(dst, src, cnt);
}
#endif

void *memchr_inv(const void *s, int c, size_t n);
char *strreplace(char *s, char old, char new);

extern void kfree_const(const void *x);

extern char *kstrdup(const char *s, gfp_t gfp) __malloc;
extern const char *kstrdup_const(const char *s, gfp_t gfp);
extern char *kstrndup(const char *s, size_t len, gfp_t gfp);
extern void *kmemdup(const void *src, size_t len, gfp_t gfp);
extern char *kmemdup_nul(const char *s, size_t len, gfp_t gfp);

extern char **argv_split(gfp_t gfp, const char *str, int *argcp);
extern void argv_free(char **argv);

extern bool sysfs_streq(const char *s1, const char *s2);
extern int kstrtobool(const char *s, bool *res);
static inline int strtobool(const char *s, bool *res)
{
	return kstrtobool(s, res);
}

int match_string(const char * const *array, size_t n, const char *string);
int __sysfs_match_string(const char * const *array, size_t n, const char *s);

/**
 * sysfs_match_string - matches given string in an array
 * @_a: array of strings
 * @_s: string to match with
 *
 * Helper for __sysfs_match_string(). Calculates the size of @a automatically.
 */
#define sysfs_match_string(_a, _s) __sysfs_match_string(_a, ARRAY_SIZE(_a), _s)

#ifdef CONFIG_BINARY_PRINTF
int vbin_printf(u32 *bin_buf, size_t size, const char *fmt, va_list args);
int bstr_printf(char *buf, size_t size, const char *fmt, const u32 *bin_buf);
int bprintf(u32 *bin_buf, size_t size, const char *fmt, ...) __printf(3, 4);
#endif

extern ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos,
				       const void *from, size_t available);

int ptr_to_hashval(const void *ptr, unsigned long *hashval_out);

/**
 * strstarts - does @str start with @prefix?
 * @str: string to examine
 * @prefix: prefix to look for.
 */
static inline bool strstarts(const char *str, const char *prefix)
{
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

size_t memweight(const void *ptr, size_t bytes);

/**
 * memzero_explicit - Fill a region of memory (e.g. sensitive
 *		      keying data) with 0s.
 * @s: Pointer to the start of the area.
 * @count: The size of the area.
 *
 * Note: usually using memset() is just fine (!), but in cases
 * where clearing out _local_ data at the end of a scope is
 * necessary, memzero_explicit() should be used instead in
 * order to prevent the compiler from optimising away zeroing.
 *
 * memzero_explicit() doesn't need an arch-specific version as
 * it just invokes the one of memset() implicitly.
 */
static inline void memzero_explicit(void *s, size_t count)
{
	memset(s, 0, count);
	barrier_data(s);
}

/**
 * kbasename - return the last part of a pathname.
 *
 * @path: path to extract the filename from.
 */
static inline const char *kbasename(const char *path)
{
	const char *tail = strrchr(path, '/');
	return tail ? tail + 1 : path;
}

#define __FORTIFY_INLINE extern __always_inline __attribute__((gnu_inline))
#define __RENAME(x) __asm__(#x)

void fortify_panic(const char *name) __noreturn __cold;
void __read_overflow(void) __compiletime_error("detected read beyond size of object passed as 1st parameter");
void __read_overflow2(void) __compiletime_error("detected read beyond size of object passed as 2nd parameter");
void __read_overflow3(void) __compiletime_error("detected read beyond size of object passed as 3rd parameter");
void __write_overflow(void) __compiletime_error("detected write beyond size of object passed as 1st parameter");

#if !defined(__NO_FORTIFY) && defined(__OPTIMIZE__) && defined(CONFIG_FORTIFY_SOURCE)
#include <linux/fortify-string.h>
#endif

/**
 * memcpy_and_pad - Copy one buffer to another with padding
 * @dest: Where to copy to
 * @dest_len: The destination buffer size
 * @src: Where to copy from
 * @count: The number of bytes to copy
 * @pad: Character to use for padding if space is left in destination.
 */
static inline void memcpy_and_pad(void *dest, size_t dest_len,
				  const void *src, size_t count, int pad)
{
	if (dest_len > count) {
		memcpy(dest, src, count);
		memset(dest + count, pad,  dest_len - count);
	} else
		memcpy(dest, src, dest_len);
}

/**
 * str_has_prefix - Test if a string has a given prefix
 * @str: The string to test
 * @prefix: The string to see if @str starts with
 *
 * A common way to test a prefix of a string is to do:
 *  strncmp(str, prefix, sizeof(prefix) - 1)
 *
 * But this can lead to bugs due to typos, or if prefix is a pointer
 * and not a constant. Instead use str_has_prefix().
 *
 * Returns:
 * * strlen(@prefix) if @str starts with @prefix
 * * 0 if @str does not start with @prefix
 */
static __always_inline size_t str_has_prefix(const char *str, const char *prefix)
{
	size_t len = strlen(prefix);
	return strncmp(str, prefix, len) == 0 ? len : 0;
}

#endif /* _LINUX_STRING_H_ */

/*
 * include/asm-xtensa/uaccess.h
 *
 * User space memory access functions
 *
 * These routines provide basic accessing functions to the user memory
 * space for the kernel. This header file provides functions such as:
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001 - 2005 Tensilica Inc.
 */

#ifndef _XTENSA_UACCESS_H
#define _XTENSA_UACCESS_H

#include <linux/prefetch.h>
#include <asm/types.h>
#include <asm/extable.h>

/*
 * The fs value determines whether argument validity checking should
 * be performed or not.  If get_fs() == USER_DS, checking is
 * performed, with get_fs() == KERNEL_DS, checking is bypassed.
 *
 * For historical reasons (Data Segment Register?), these macros are
 * grossly misnamed.
 */

#define KERNEL_DS	((mm_segment_t) { 0 })
#define USER_DS		((mm_segment_t) { 1 })

#define get_fs()	(current->thread.current_ds)
#define set_fs(val)	(current->thread.current_ds = (val))

#define uaccess_kernel() (get_fs().seg == KERNEL_DS.seg)

#define __kernel_ok (uaccess_kernel())
#define __user_ok(addr, size) \
		(((size) <= TASK_SIZE)&&((addr) <= TASK_SIZE-(size)))
#define __access_ok(addr, size) (__kernel_ok || __user_ok((addr), (size)))
#define access_ok(addr, size) __access_ok((unsigned long)(addr), (size))

#define user_addr_max() (uaccess_kernel() ? ~0UL : TASK_SIZE)

/*
 * These are the main single-value transfer routines.  They
 * automatically use the right size if we just have the right pointer
 * type.
 *
 * This gets kind of ugly. We want to return _two_ values in
 * "get_user()" and yet we don't want to do any pointers, because that
 * is too much of a performance impact. Thus we have a few rather ugly
 * macros here, and hide all the uglyness from the user.
 *
 * Careful to not
 * (a) re-use the arguments for side effects (sizeof is ok)
 * (b) require any knowledge of processes at this stage
 */
#define put_user(x, ptr)	__put_user_check((x), (ptr), sizeof(*(ptr)))
#define get_user(x, ptr) __get_user_check((x), (ptr), sizeof(*(ptr)))

/*
 * The "__xxx" versions of the user access functions are versions that
 * do not verify the address space, that must have been done previously
 * with a separate "access_ok()" call (this is used when we do multiple
 * accesses to the same area of user memory).
 */
#define __put_user(x, ptr) __put_user_nocheck((x), (ptr), sizeof(*(ptr)))
#define __get_user(x, ptr) __get_user_nocheck((x), (ptr), sizeof(*(ptr)))


extern long __put_user_bad(void);

#define __put_user_nocheck(x, ptr, size)		\
({							\
	long __pu_err;					\
	__put_user_size((x), (ptr), (size), __pu_err);	\
	__pu_err;					\
})

#define __put_user_check(x, ptr, size)					\
({									\
	long __pu_err = -EFAULT;					\
	__typeof__(*(ptr)) __user *__pu_addr = (ptr);			\
	if (access_ok(__pu_addr, size))			\
		__put_user_size((x), __pu_addr, (size), __pu_err);	\
	__pu_err;							\
})

#define __put_user_size(x, ptr, size, retval)				\
do {									\
	int __cb;							\
	retval = 0;							\
	switch (size) {							\
	case 1: __put_user_asm(x, ptr, retval, 1, "s8i", __cb);  break;	\
	case 2: __put_user_asm(x, ptr, retval, 2, "s16i", __cb); break;	\
	case 4: __put_user_asm(x, ptr, retval, 4, "s32i", __cb); break;	\
	case 8: {							\
		     __typeof__(*ptr) __v64 = x;			\
		     retval = __copy_to_user(ptr, &__v64, 8) ? -EFAULT : 0;	\
		     break;						\
	        }							\
	default: __put_user_bad();					\
	}								\
} while (0)


/*
 * Consider a case of a user single load/store would cause both an
 * unaligned exception and an MMU-related exception (unaligned
 * exceptions happen first):
 *
 * User code passes a bad variable ptr to a system call.
 * Kernel tries to access the variable.
 * Unaligned exception occurs.
 * Unaligned exception handler tries to make aligned accesses.
 * Double exception occurs for MMU-related cause (e.g., page not mapped).
 * do_page_fault() thinks the fault address belongs to the kernel, not the
 * user, and panics.
 *
 * The kernel currently prohibits user unaligned accesses.  We use the
 * __check_align_* macros to check for unaligned addresses before
 * accessing user space so we don't crash the kernel.  Both
 * __put_user_asm and __get_user_asm use these alignment macros, so
 * macro-specific labels such as 0f, 1f, %0, %2, and %3 must stay in
 * sync.
 */

#define __check_align_1  ""

#define __check_align_2				\
	"   _bbci.l %[mem] * 0, 1f	\n"	\
	"   movi    %[err], %[efault]	\n"	\
	"   _j      2f			\n"

#define __check_align_4				\
	"   _bbsi.l %[mem] * 0, 0f	\n"	\
	"   _bbci.l %[mem] * 0 + 1, 1f	\n"	\
	"0: movi    %[err], %[efault]	\n"	\
	"   _j      2f			\n"


/*
 * We don't tell gcc that we are accessing memory, but this is OK
 * because we do not write to any memory gcc knows about, so there
 * are no aliasing issues.
 *
 * WARNING: If you modify this macro at all, verify that the
 * __check_align_* macros still work.
 */
#define __put_user_asm(x_, addr_, err_, align, insn, cb)\
__asm__ __volatile__(					\
	__check_align_##align				\
	"1: "insn"  %[x], %[mem]	\n"		\
	"2:				\n"		\
	"   .section  .fixup,\"ax\"	\n"		\
	"   .align 4			\n"		\
	"   .literal_position		\n"		\
	"5:				\n"		\
	"   movi   %[tmp], 2b		\n"		\
	"   movi   %[err], %[efault]	\n"		\
	"   jx     %[tmp]		\n"		\
	"   .previous			\n"		\
	"   .section  __ex_table,\"a\"	\n"		\
	"   .long	1b, 5b		\n"		\
	"   .previous"					\
	:[err] "+r"(err_), [tmp] "=r"(cb), [mem] "=m"(*(addr_))		\
	:[x] "r"(x_), [efault] "i"(-EFAULT))

#define __get_user_nocheck(x, ptr, size)			\
({								\
	long __gu_err;						\
	__get_user_size((x), (ptr), (size), __gu_err);		\
	__gu_err;						\
})

#define __get_user_check(x, ptr, size)					\
({									\
	long __gu_err = -EFAULT;					\
	const __typeof__(*(ptr)) __user *__gu_addr = (ptr);		\
	if (access_ok(__gu_addr, size))					\
		__get_user_size((x), __gu_addr, (size), __gu_err);	\
	else								\
		(x) = (__typeof__(*(ptr)))0;				\
	__gu_err;							\
})

extern long __get_user_bad(void);

#define __get_user_size(x, ptr, size, retval)				\
do {									\
	int __cb;							\
	retval = 0;							\
	switch (size) {							\
	case 1: __get_user_asm(x, ptr, retval, 1, "l8ui", __cb);  break;\
	case 2: __get_user_asm(x, ptr, retval, 2, "l16ui", __cb); break;\
	case 4: __get_user_asm(x, ptr, retval, 4, "l32i", __cb);  break;\
	case 8: {							\
		u64 __x;						\
		if (unlikely(__copy_from_user(&__x, ptr, 8))) {		\
			retval = -EFAULT;				\
			(x) = (__typeof__(*(ptr)))0;			\
		} else {						\
			(x) = *(__force __typeof__(*(ptr)) *)&__x;	\
		}							\
		break;							\
	}								\
	default:							\
		(x) = (__typeof__(*(ptr)))0;				\
		__get_user_bad();					\
	}								\
} while (0)


/*
 * WARNING: If you modify this macro at all, verify that the
 * __check_align_* macros still work.
 */
#define __get_user_asm(x_, addr_, err_, align, insn, cb) \
do {							\
	u32 __x = 0;					\
	__asm__ __volatile__(				\
		__check_align_##align			\
		"1: "insn"  %[x], %[mem]	\n"	\
		"2:				\n"	\
		"   .section  .fixup,\"ax\"	\n"	\
		"   .align 4			\n"	\
		"   .literal_position		\n"	\
		"5:				\n"	\
		"   movi   %[tmp], 2b		\n"	\
		"   movi   %[err], %[efault]	\n"	\
		"   jx     %[tmp]		\n"	\
		"   .previous			\n"	\
		"   .section  __ex_table,\"a\"	\n"	\
		"   .long	1b, 5b		\n"	\
		"   .previous"				\
		:[err] "+r"(err_), [tmp] "=r"(cb), [x] "+r"(__x) \
		:[mem] "m"(*(addr_)), [efault] "i"(-EFAULT)); \
	(x_) = (__force __typeof__(*(addr_)))__x;	\
} while (0)


/*
 * Copy to/from user space
 */

extern unsigned __xtensa_copy_user(void *to, const void *from, unsigned n);

static inline unsigned long
raw_copy_from_user(void *to, const void __user *from, unsigned long n)
{
	prefetchw(to);
	return __xtensa_copy_user(to, (__force const void *)from, n);
}
static inline unsigned long
raw_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	prefetch(from);
	return __xtensa_copy_user((__force void *)to, from, n);
}
#define INLINE_COPY_FROM_USER
#define INLINE_COPY_TO_USER

/*
 * We need to return the number of bytes not cleared.  Our memset()
 * returns zero if a problem occurs while accessing user-space memory.
 * In that event, return no memory cleared.  Otherwise, zero for
 * success.
 */

static inline unsigned long
__xtensa_clear_user(void __user *addr, unsigned long size)
{
	if (!__memset((void __force *)addr, 0, size))
		return size;
	return 0;
}

static inline unsigned long
clear_user(void __user *addr, unsigned long size)
{
	if (access_ok(addr, size))
		return __xtensa_clear_user(addr, size);
	return size ? -EFAULT : 0;
}

#define __clear_user  __xtensa_clear_user


#ifndef CONFIG_GENERIC_STRNCPY_FROM_USER

extern long __strncpy_user(char *dst, const char __user *src, long count);

static inline long
strncpy_from_user(char *dst, const char __user *src, long count)
{
	if (access_ok(src, 1))
		return __strncpy_user(dst, src, count);
	return -EFAULT;
}
#else
long strncpy_from_user(char *dst, const char __user *src, long count);
#endif

/*
 * Return the size of a string (including the ending 0!)
 */
extern long __strnlen_user(const char __user *str, long len);

static inline long strnlen_user(const char __user *str, long len)
{
	if (!access_ok(str, 1))
		return 0;
	return __strnlen_user(str, len);
}

#endif	/* _XTENSA_UACCESS_H */

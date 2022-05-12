/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __M68K_UACCESS_H
#define __M68K_UACCESS_H

#ifdef CONFIG_MMU

/*
 * User space memory access functions
 */
#include <linux/compiler.h>
#include <linux/types.h>
#include <asm/segment.h>
#include <asm/extable.h>

/* We let the MMU do all checking */
static inline int access_ok(const void __user *addr,
			    unsigned long size)
{
	return 1;
}

/*
 * Not all varients of the 68k family support the notion of address spaces.
 * The traditional 680x0 parts do, and they use the sfc/dfc registers and
 * the "moves" instruction to access user space from kernel space. Other
 * family members like ColdFire don't support this, and only have a single
 * address space, and use the usual "move" instruction for user space access.
 *
 * Outside of this difference the user space access functions are the same.
 * So lets keep the code simple and just define in what we need to use.
 */
#ifdef CONFIG_CPU_HAS_ADDRESS_SPACES
#define	MOVES	"moves"
#else
#define	MOVES	"move"
#endif

extern int __put_user_bad(void);
extern int __get_user_bad(void);

#define __put_user_asm(res, x, ptr, bwl, reg, err)	\
asm volatile ("\n"					\
	"1:	"MOVES"."#bwl"	%2,%1\n"		\
	"2:\n"						\
	"	.section .fixup,\"ax\"\n"		\
	"	.even\n"				\
	"10:	moveq.l	%3,%0\n"			\
	"	jra 2b\n"				\
	"	.previous\n"				\
	"\n"						\
	"	.section __ex_table,\"a\"\n"		\
	"	.align	4\n"				\
	"	.long	1b,10b\n"			\
	"	.long	2b,10b\n"			\
	"	.previous"				\
	: "+d" (res), "=m" (*(ptr))			\
	: #reg (x), "i" (err))

/*
 * These are the main single-value transfer routines.  They automatically
 * use the right size if we just have the right pointer type.
 */

#define __put_user(x, ptr)						\
({									\
	typeof(*(ptr)) __pu_val = (x);					\
	int __pu_err = 0;						\
	__chk_user_ptr(ptr);						\
	switch (sizeof (*(ptr))) {					\
	case 1:								\
		__put_user_asm(__pu_err, __pu_val, ptr, b, d, -EFAULT);	\
		break;							\
	case 2:								\
		__put_user_asm(__pu_err, __pu_val, ptr, w, r, -EFAULT);	\
		break;							\
	case 4:								\
		__put_user_asm(__pu_err, __pu_val, ptr, l, r, -EFAULT);	\
		break;							\
	case 8:								\
 	    {								\
 		const void __user *__pu_ptr = (ptr);			\
		asm volatile ("\n"					\
			"1:	"MOVES".l	%2,(%1)+\n"		\
			"2:	"MOVES".l	%R2,(%1)\n"		\
			"3:\n"						\
			"	.section .fixup,\"ax\"\n"		\
			"	.even\n"				\
			"10:	movel %3,%0\n"				\
			"	jra 3b\n"				\
			"	.previous\n"				\
			"\n"						\
			"	.section __ex_table,\"a\"\n"		\
			"	.align 4\n"				\
			"	.long 1b,10b\n"				\
			"	.long 2b,10b\n"				\
			"	.long 3b,10b\n"				\
			"	.previous"				\
			: "+d" (__pu_err), "+a" (__pu_ptr)		\
			: "r" (__pu_val), "i" (-EFAULT)			\
			: "memory");					\
		break;							\
	    }								\
	default:							\
		__pu_err = __put_user_bad();				\
		break;							\
	}								\
	__pu_err;							\
})
#define put_user(x, ptr)	__put_user(x, ptr)


#define __get_user_asm(res, x, ptr, type, bwl, reg, err) ({		\
	type __gu_val;							\
	asm volatile ("\n"						\
		"1:	"MOVES"."#bwl"	%2,%1\n"			\
		"2:\n"							\
		"	.section .fixup,\"ax\"\n"			\
		"	.even\n"					\
		"10:	move.l	%3,%0\n"				\
		"	sub.l	%1,%1\n"				\
		"	jra	2b\n"					\
		"	.previous\n"					\
		"\n"							\
		"	.section __ex_table,\"a\"\n"			\
		"	.align	4\n"					\
		"	.long	1b,10b\n"				\
		"	.previous"					\
		: "+d" (res), "=&" #reg (__gu_val)			\
		: "m" (*(ptr)), "i" (err));				\
	(x) = (__force typeof(*(ptr)))(__force unsigned long)__gu_val;	\
})

#define __get_user(x, ptr)						\
({									\
	int __gu_err = 0;						\
	__chk_user_ptr(ptr);						\
	switch (sizeof(*(ptr))) {					\
	case 1:								\
		__get_user_asm(__gu_err, x, ptr, u8, b, d, -EFAULT);	\
		break;							\
	case 2:								\
		__get_user_asm(__gu_err, x, ptr, u16, w, r, -EFAULT);	\
		break;							\
	case 4:								\
		__get_user_asm(__gu_err, x, ptr, u32, l, r, -EFAULT);	\
		break;							\
	case 8: {							\
		const void __user *__gu_ptr = (ptr);			\
		union {							\
			u64 l;						\
			__typeof__(*(ptr)) t;				\
		} __gu_val;						\
		asm volatile ("\n"					\
			"1:	"MOVES".l	(%2)+,%1\n"		\
			"2:	"MOVES".l	(%2),%R1\n"		\
			"3:\n"						\
			"	.section .fixup,\"ax\"\n"		\
			"	.even\n"				\
			"10:	move.l	%3,%0\n"			\
			"	sub.l	%1,%1\n"			\
			"	sub.l	%R1,%R1\n"			\
			"	jra	3b\n"				\
			"	.previous\n"				\
			"\n"						\
			"	.section __ex_table,\"a\"\n"		\
			"	.align	4\n"				\
			"	.long	1b,10b\n"			\
			"	.long	2b,10b\n"			\
			"	.previous"				\
			: "+d" (__gu_err), "=&r" (__gu_val.l),		\
			  "+a" (__gu_ptr)				\
			: "i" (-EFAULT)					\
			: "memory");					\
		(x) = __gu_val.t;					\
		break;							\
	}								\
	default:							\
		__gu_err = __get_user_bad();				\
		break;							\
	}								\
	__gu_err;							\
})
#define get_user(x, ptr) __get_user(x, ptr)

unsigned long __generic_copy_from_user(void *to, const void __user *from, unsigned long n);
unsigned long __generic_copy_to_user(void __user *to, const void *from, unsigned long n);

#define __suffix0
#define __suffix1 b
#define __suffix2 w
#define __suffix4 l

#define ____constant_copy_from_user_asm(res, to, from, tmp, n1, n2, n3, s1, s2, s3)\
	asm volatile ("\n"						\
		"1:	"MOVES"."#s1"	(%2)+,%3\n"			\
		"	move."#s1"	%3,(%1)+\n"			\
		"	.ifnc	\""#s2"\",\"\"\n"			\
		"2:	"MOVES"."#s2"	(%2)+,%3\n"			\
		"	move."#s2"	%3,(%1)+\n"			\
		"	.ifnc	\""#s3"\",\"\"\n"			\
		"3:	"MOVES"."#s3"	(%2)+,%3\n"			\
		"	move."#s3"	%3,(%1)+\n"			\
		"	.endif\n"					\
		"	.endif\n"					\
		"4:\n"							\
		"	.section __ex_table,\"a\"\n"			\
		"	.align	4\n"					\
		"	.long	1b,10f\n"				\
		"	.ifnc	\""#s2"\",\"\"\n"			\
		"	.long	2b,20f\n"				\
		"	.ifnc	\""#s3"\",\"\"\n"			\
		"	.long	3b,30f\n"				\
		"	.endif\n"					\
		"	.endif\n"					\
		"	.previous\n"					\
		"\n"							\
		"	.section .fixup,\"ax\"\n"			\
		"	.even\n"					\
		"10:	addq.l #"#n1",%0\n"				\
		"	.ifnc	\""#s2"\",\"\"\n"			\
		"20:	addq.l #"#n2",%0\n"				\
		"	.ifnc	\""#s3"\",\"\"\n"			\
		"30:	addq.l #"#n3",%0\n"				\
		"	.endif\n"					\
		"	.endif\n"					\
		"	jra	4b\n"					\
		"	.previous\n"					\
		: "+d" (res), "+&a" (to), "+a" (from), "=&d" (tmp)	\
		: : "memory")

#define ___constant_copy_from_user_asm(res, to, from, tmp, n1, n2, n3, s1, s2, s3)\
	____constant_copy_from_user_asm(res, to, from, tmp, n1, n2, n3, s1, s2, s3)
#define __constant_copy_from_user_asm(res, to, from, tmp, n1, n2, n3)	\
	___constant_copy_from_user_asm(res, to, from, tmp, n1, n2, n3,  \
					__suffix##n1, __suffix##n2, __suffix##n3)

static __always_inline unsigned long
__constant_copy_from_user(void *to, const void __user *from, unsigned long n)
{
	unsigned long res = 0, tmp;

	switch (n) {
	case 1:
		__constant_copy_from_user_asm(res, to, from, tmp, 1, 0, 0);
		break;
	case 2:
		__constant_copy_from_user_asm(res, to, from, tmp, 2, 0, 0);
		break;
	case 3:
		__constant_copy_from_user_asm(res, to, from, tmp, 2, 1, 0);
		break;
	case 4:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 0, 0);
		break;
	case 5:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 1, 0);
		break;
	case 6:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 2, 0);
		break;
	case 7:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 2, 1);
		break;
	case 8:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 4, 0);
		break;
	case 9:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 4, 1);
		break;
	case 10:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 4, 2);
		break;
	case 12:
		__constant_copy_from_user_asm(res, to, from, tmp, 4, 4, 4);
		break;
	default:
		/* we limit the inlined version to 3 moves */
		return __generic_copy_from_user(to, from, n);
	}

	return res;
}

#define __constant_copy_to_user_asm(res, to, from, tmp, n, s1, s2, s3)	\
	asm volatile ("\n"						\
		"	move."#s1"	(%2)+,%3\n"			\
		"11:	"MOVES"."#s1"	%3,(%1)+\n"			\
		"12:	move."#s2"	(%2)+,%3\n"			\
		"21:	"MOVES"."#s2"	%3,(%1)+\n"			\
		"22:\n"							\
		"	.ifnc	\""#s3"\",\"\"\n"			\
		"	move."#s3"	(%2)+,%3\n"			\
		"31:	"MOVES"."#s3"	%3,(%1)+\n"			\
		"32:\n"							\
		"	.endif\n"					\
		"4:\n"							\
		"\n"							\
		"	.section __ex_table,\"a\"\n"			\
		"	.align	4\n"					\
		"	.long	11b,5f\n"				\
		"	.long	12b,5f\n"				\
		"	.long	21b,5f\n"				\
		"	.long	22b,5f\n"				\
		"	.ifnc	\""#s3"\",\"\"\n"			\
		"	.long	31b,5f\n"				\
		"	.long	32b,5f\n"				\
		"	.endif\n"					\
		"	.previous\n"					\
		"\n"							\
		"	.section .fixup,\"ax\"\n"			\
		"	.even\n"					\
		"5:	moveq.l	#"#n",%0\n"				\
		"	jra	4b\n"					\
		"	.previous\n"					\
		: "+d" (res), "+a" (to), "+a" (from), "=&d" (tmp)	\
		: : "memory")

static __always_inline unsigned long
__constant_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	unsigned long res = 0, tmp;

	switch (n) {
	case 1:
		__put_user_asm(res, *(u8 *)from, (u8 __user *)to, b, d, 1);
		break;
	case 2:
		__put_user_asm(res, *(u16 *)from, (u16 __user *)to, w, r, 2);
		break;
	case 3:
		__constant_copy_to_user_asm(res, to, from, tmp, 3, w, b,);
		break;
	case 4:
		__put_user_asm(res, *(u32 *)from, (u32 __user *)to, l, r, 4);
		break;
	case 5:
		__constant_copy_to_user_asm(res, to, from, tmp, 5, l, b,);
		break;
	case 6:
		__constant_copy_to_user_asm(res, to, from, tmp, 6, l, w,);
		break;
	case 7:
		__constant_copy_to_user_asm(res, to, from, tmp, 7, l, w, b);
		break;
	case 8:
		__constant_copy_to_user_asm(res, to, from, tmp, 8, l, l,);
		break;
	case 9:
		__constant_copy_to_user_asm(res, to, from, tmp, 9, l, l, b);
		break;
	case 10:
		__constant_copy_to_user_asm(res, to, from, tmp, 10, l, l, w);
		break;
	case 12:
		__constant_copy_to_user_asm(res, to, from, tmp, 12, l, l, l);
		break;
	default:
		/* limit the inlined version to 3 moves */
		return __generic_copy_to_user(to, from, n);
	}

	return res;
}

static inline unsigned long
raw_copy_from_user(void *to, const void __user *from, unsigned long n)
{
	if (__builtin_constant_p(n))
		return __constant_copy_from_user(to, from, n);
	return __generic_copy_from_user(to, from, n);
}

static inline unsigned long
raw_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	if (__builtin_constant_p(n))
		return __constant_copy_to_user(to, from, n);
	return __generic_copy_to_user(to, from, n);
}
#define INLINE_COPY_FROM_USER
#define INLINE_COPY_TO_USER

#define user_addr_max() \
	(uaccess_kernel() ? ~0UL : TASK_SIZE)

extern long strncpy_from_user(char *dst, const char __user *src, long count);
extern __must_check long strnlen_user(const char __user *str, long n);

unsigned long __clear_user(void __user *to, unsigned long n);

#define clear_user	__clear_user

#else /* !CONFIG_MMU */
#include <asm-generic/uaccess.h>
#endif

#endif /* _M68K_UACCESS_H */

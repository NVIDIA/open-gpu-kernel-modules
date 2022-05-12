/* SPDX-License-Identifier: LGPL-2.1 OR MIT */
/*
 * Author: Paul Burton <paul.burton@mips.com>
 * (C) Copyright 2018 MIPS Tech LLC
 *
 * Based on rseq-arm.h:
 * (C) Copyright 2016-2018 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

/*
 * RSEQ_SIG uses the break instruction. The instruction pattern is:
 *
 * On MIPS:
 *	0350000d        break     0x350
 *
 * On nanoMIPS:
 *      00100350        break     0x350
 *
 * On microMIPS:
 *      0000d407        break     0x350
 *
 * For nanoMIPS32 and microMIPS, the instruction stream is encoded as 16-bit
 * halfwords, so the signature halfwords need to be swapped accordingly for
 * little-endian.
 */
#if defined(__nanomips__)
# ifdef __MIPSEL__
#  define RSEQ_SIG	0x03500010
# else
#  define RSEQ_SIG	0x00100350
# endif
#elif defined(__mips_micromips)
# ifdef __MIPSEL__
#  define RSEQ_SIG	0xd4070000
# else
#  define RSEQ_SIG	0x0000d407
# endif
#elif defined(__mips__)
# define RSEQ_SIG	0x0350000d
#else
/* Unknown MIPS architecture. */
#endif

#define rseq_smp_mb()	__asm__ __volatile__ ("sync" ::: "memory")
#define rseq_smp_rmb()	rseq_smp_mb()
#define rseq_smp_wmb()	rseq_smp_mb()

#define rseq_smp_load_acquire(p)					\
__extension__ ({							\
	__typeof(*p) ____p1 = RSEQ_READ_ONCE(*p);			\
	rseq_smp_mb();							\
	____p1;								\
})

#define rseq_smp_acquire__after_ctrl_dep()	rseq_smp_rmb()

#define rseq_smp_store_release(p, v)					\
do {									\
	rseq_smp_mb();							\
	RSEQ_WRITE_ONCE(*p, v);						\
} while (0)

#ifdef RSEQ_SKIP_FASTPATH
#include "rseq-skip.h"
#else /* !RSEQ_SKIP_FASTPATH */

#if _MIPS_SZLONG == 64
# define LONG			".dword"
# define LONG_LA		"dla"
# define LONG_L			"ld"
# define LONG_S			"sd"
# define LONG_ADDI		"daddiu"
# define U32_U64_PAD(x)		x
#elif _MIPS_SZLONG == 32
# define LONG			".word"
# define LONG_LA		"la"
# define LONG_L			"lw"
# define LONG_S			"sw"
# define LONG_ADDI		"addiu"
# ifdef __BIG_ENDIAN
#  define U32_U64_PAD(x)	"0x0, " x
# else
#  define U32_U64_PAD(x)	x ", 0x0"
# endif
#else
# error unsupported _MIPS_SZLONG
#endif

#define __RSEQ_ASM_DEFINE_TABLE(label, version, flags, start_ip, \
				post_commit_offset, abort_ip) \
		".pushsection __rseq_cs, \"aw\"\n\t" \
		".balign 32\n\t" \
		__rseq_str(label) ":\n\t"					\
		".word " __rseq_str(version) ", " __rseq_str(flags) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(start_ip)) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(post_commit_offset)) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(abort_ip)) "\n\t" \
		".popsection\n\t" \
		".pushsection __rseq_cs_ptr_array, \"aw\"\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(label) "b") "\n\t" \
		".popsection\n\t"

#define RSEQ_ASM_DEFINE_TABLE(label, start_ip, post_commit_ip, abort_ip) \
	__RSEQ_ASM_DEFINE_TABLE(label, 0x0, 0x0, start_ip, \
				(post_commit_ip - start_ip), abort_ip)

/*
 * Exit points of a rseq critical section consist of all instructions outside
 * of the critical section where a critical section can either branch to or
 * reach through the normal course of its execution. The abort IP and the
 * post-commit IP are already part of the __rseq_cs section and should not be
 * explicitly defined as additional exit points. Knowing all exit points is
 * useful to assist debuggers stepping over the critical section.
 */
#define RSEQ_ASM_DEFINE_EXIT_POINT(start_ip, exit_ip) \
		".pushsection __rseq_exit_point_array, \"aw\"\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(start_ip)) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(exit_ip)) "\n\t" \
		".popsection\n\t"

#define RSEQ_ASM_STORE_RSEQ_CS(label, cs_label, rseq_cs) \
		RSEQ_INJECT_ASM(1) \
		LONG_LA " $4, " __rseq_str(cs_label) "\n\t" \
		LONG_S  " $4, %[" __rseq_str(rseq_cs) "]\n\t" \
		__rseq_str(label) ":\n\t"

#define RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, label) \
		RSEQ_INJECT_ASM(2) \
		"lw  $4, %[" __rseq_str(current_cpu_id) "]\n\t" \
		"bne $4, %[" __rseq_str(cpu_id) "], " __rseq_str(label) "\n\t"

#define __RSEQ_ASM_DEFINE_ABORT(table_label, label, teardown, \
				abort_label, version, flags, \
				start_ip, post_commit_offset, abort_ip) \
		".balign 32\n\t" \
		__rseq_str(table_label) ":\n\t" \
		".word " __rseq_str(version) ", " __rseq_str(flags) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(start_ip)) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(post_commit_offset)) "\n\t" \
		LONG " " U32_U64_PAD(__rseq_str(abort_ip)) "\n\t" \
		".word " __rseq_str(RSEQ_SIG) "\n\t" \
		__rseq_str(label) ":\n\t" \
		teardown \
		"b %l[" __rseq_str(abort_label) "]\n\t"

#define RSEQ_ASM_DEFINE_ABORT(table_label, label, teardown, abort_label, \
			      start_ip, post_commit_ip, abort_ip) \
	__RSEQ_ASM_DEFINE_ABORT(table_label, label, teardown, \
				abort_label, 0x0, 0x0, start_ip, \
				(post_commit_ip - start_ip), abort_ip)

#define RSEQ_ASM_DEFINE_CMPFAIL(label, teardown, cmpfail_label) \
		__rseq_str(label) ":\n\t" \
		teardown \
		"b %l[" __rseq_str(cmpfail_label) "]\n\t"

#define rseq_workaround_gcc_asm_size_guess()	__asm__ __volatile__("")

static inline __attribute__((always_inline))
int rseq_cmpeqv_storev(intptr_t *v, intptr_t expect, intptr_t newv, int cpu)
{
	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
#endif
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[cmpfail]\n\t"
		RSEQ_INJECT_ASM(4)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, %l[error1])
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[error2]\n\t"
#endif
		/* final store */
		LONG_S " %[newv], %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(5)
		"b 5f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4, "", abort, 1b, 2b, 4f)
		"5:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  [v]			"m" (*v),
		  [expect]		"r" (expect),
		  [newv]		"r" (newv)
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_bug("expected value comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_cmpnev_storeoffp_load(intptr_t *v, intptr_t expectnot,
			       off_t voffp, intptr_t *load, int cpu)
{
	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
#endif
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"beq $4, %[expectnot], %l[cmpfail]\n\t"
		RSEQ_INJECT_ASM(4)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, %l[error1])
		LONG_L " $4, %[v]\n\t"
		"beq $4, %[expectnot], %l[error2]\n\t"
#endif
		LONG_S " $4, %[load]\n\t"
		LONG_ADDI " $4, %[voffp]\n\t"
		LONG_L " $4, 0($4)\n\t"
		/* final store */
		LONG_S " $4, %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(5)
		"b 5f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4, "", abort, 1b, 2b, 4f)
		"5:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  /* final store input */
		  [v]			"m" (*v),
		  [expectnot]		"r" (expectnot),
		  [voffp]		"Ir" (voffp),
		  [load]		"m" (*load)
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_bug("expected value comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_addv(intptr_t *v, intptr_t count, int cpu)
{
	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
#endif
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, %l[error1])
#endif
		LONG_L " $4, %[v]\n\t"
		LONG_ADDI " $4, %[count]\n\t"
		/* final store */
		LONG_S " $4, %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(4)
		"b 5f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4, "", abort, 1b, 2b, 4f)
		"5:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  [v]			"m" (*v),
		  [count]		"Ir" (count)
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort
#ifdef RSEQ_COMPARE_TWICE
		  , error1
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_bug("cpu_id comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_cmpeqv_trystorev_storev(intptr_t *v, intptr_t expect,
				 intptr_t *v2, intptr_t newv2,
				 intptr_t newv, int cpu)
{
	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
#endif
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[cmpfail]\n\t"
		RSEQ_INJECT_ASM(4)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, %l[error1])
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[error2]\n\t"
#endif
		/* try store */
		LONG_S " %[newv2], %[v2]\n\t"
		RSEQ_INJECT_ASM(5)
		/* final store */
		LONG_S " %[newv], %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(6)
		"b 5f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4, "", abort, 1b, 2b, 4f)
		"5:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  /* try store input */
		  [v2]			"m" (*v2),
		  [newv2]		"r" (newv2),
		  /* final store input */
		  [v]			"m" (*v),
		  [expect]		"r" (expect),
		  [newv]		"r" (newv)
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_bug("expected value comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_cmpeqv_trystorev_storev_release(intptr_t *v, intptr_t expect,
					 intptr_t *v2, intptr_t newv2,
					 intptr_t newv, int cpu)
{
	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
#endif
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[cmpfail]\n\t"
		RSEQ_INJECT_ASM(4)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, %l[error1])
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[error2]\n\t"
#endif
		/* try store */
		LONG_S " %[newv2], %[v2]\n\t"
		RSEQ_INJECT_ASM(5)
		"sync\n\t"	/* full sync provides store-release */
		/* final store */
		LONG_S " %[newv], %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(6)
		"b 5f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4, "", abort, 1b, 2b, 4f)
		"5:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  /* try store input */
		  [v2]			"m" (*v2),
		  [newv2]		"r" (newv2),
		  /* final store input */
		  [v]			"m" (*v),
		  [expect]		"r" (expect),
		  [newv]		"r" (newv)
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_bug("expected value comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_cmpeqv_cmpeqv_storev(intptr_t *v, intptr_t expect,
			      intptr_t *v2, intptr_t expect2,
			      intptr_t newv, int cpu)
{
	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error3])
#endif
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[cmpfail]\n\t"
		RSEQ_INJECT_ASM(4)
		LONG_L " $4, %[v2]\n\t"
		"bne $4, %[expect2], %l[cmpfail]\n\t"
		RSEQ_INJECT_ASM(5)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, %l[error1])
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], %l[error2]\n\t"
		LONG_L " $4, %[v2]\n\t"
		"bne $4, %[expect2], %l[error3]\n\t"
#endif
		/* final store */
		LONG_S " %[newv], %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(6)
		"b 5f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4, "", abort, 1b, 2b, 4f)
		"5:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  /* cmp2 input */
		  [v2]			"m" (*v2),
		  [expect2]		"r" (expect2),
		  /* final store input */
		  [v]			"m" (*v),
		  [expect]		"r" (expect),
		  [newv]		"r" (newv)
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2, error3
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_bug("1st expected value comparison failed");
error3:
	rseq_bug("2nd expected value comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_cmpeqv_trymemcpy_storev(intptr_t *v, intptr_t expect,
				 void *dst, void *src, size_t len,
				 intptr_t newv, int cpu)
{
	uintptr_t rseq_scratch[3];

	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
#endif
		LONG_S " %[src], %[rseq_scratch0]\n\t"
		LONG_S "  %[dst], %[rseq_scratch1]\n\t"
		LONG_S " %[len], %[rseq_scratch2]\n\t"
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], 5f\n\t"
		RSEQ_INJECT_ASM(4)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 6f)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], 7f\n\t"
#endif
		/* try memcpy */
		"beqz %[len], 333f\n\t" \
		"222:\n\t" \
		"lb   $4, 0(%[src])\n\t" \
		"sb   $4, 0(%[dst])\n\t" \
		LONG_ADDI " %[src], 1\n\t" \
		LONG_ADDI " %[dst], 1\n\t" \
		LONG_ADDI " %[len], -1\n\t" \
		"bnez %[len], 222b\n\t" \
		"333:\n\t" \
		RSEQ_INJECT_ASM(5)
		/* final store */
		LONG_S " %[newv], %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(6)
		/* teardown */
		LONG_L " %[len], %[rseq_scratch2]\n\t"
		LONG_L " %[dst], %[rseq_scratch1]\n\t"
		LONG_L " %[src], %[rseq_scratch0]\n\t"
		"b 8f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4,
				      /* teardown */
				      LONG_L " %[len], %[rseq_scratch2]\n\t"
				      LONG_L " %[dst], %[rseq_scratch1]\n\t"
				      LONG_L " %[src], %[rseq_scratch0]\n\t",
				      abort, 1b, 2b, 4f)
		RSEQ_ASM_DEFINE_CMPFAIL(5,
					/* teardown */
					LONG_L " %[len], %[rseq_scratch2]\n\t"
					LONG_L " %[dst], %[rseq_scratch1]\n\t"
					LONG_L " %[src], %[rseq_scratch0]\n\t",
					cmpfail)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_CMPFAIL(6,
					/* teardown */
					LONG_L " %[len], %[rseq_scratch2]\n\t"
					LONG_L " %[dst], %[rseq_scratch1]\n\t"
					LONG_L " %[src], %[rseq_scratch0]\n\t",
					error1)
		RSEQ_ASM_DEFINE_CMPFAIL(7,
					/* teardown */
					LONG_L " %[len], %[rseq_scratch2]\n\t"
					LONG_L " %[dst], %[rseq_scratch1]\n\t"
					LONG_L " %[src], %[rseq_scratch0]\n\t",
					error2)
#endif
		"8:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  /* final store input */
		  [v]			"m" (*v),
		  [expect]		"r" (expect),
		  [newv]		"r" (newv),
		  /* try memcpy input */
		  [dst]			"r" (dst),
		  [src]			"r" (src),
		  [len]			"r" (len),
		  [rseq_scratch0]	"m" (rseq_scratch[0]),
		  [rseq_scratch1]	"m" (rseq_scratch[1]),
		  [rseq_scratch2]	"m" (rseq_scratch[2])
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_workaround_gcc_asm_size_guess();
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_workaround_gcc_asm_size_guess();
	rseq_bug("expected value comparison failed");
#endif
}

static inline __attribute__((always_inline))
int rseq_cmpeqv_trymemcpy_storev_release(intptr_t *v, intptr_t expect,
					 void *dst, void *src, size_t len,
					 intptr_t newv, int cpu)
{
	uintptr_t rseq_scratch[3];

	RSEQ_INJECT_C(9)

	rseq_workaround_gcc_asm_size_guess();
	__asm__ __volatile__ goto (
		RSEQ_ASM_DEFINE_TABLE(9, 1f, 2f, 4f) /* start, commit, abort */
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[cmpfail])
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error1])
		RSEQ_ASM_DEFINE_EXIT_POINT(1f, %l[error2])
#endif
		LONG_S " %[src], %[rseq_scratch0]\n\t"
		LONG_S " %[dst], %[rseq_scratch1]\n\t"
		LONG_S " %[len], %[rseq_scratch2]\n\t"
		/* Start rseq by storing table entry pointer into rseq_cs. */
		RSEQ_ASM_STORE_RSEQ_CS(1, 3f, rseq_cs)
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 4f)
		RSEQ_INJECT_ASM(3)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], 5f\n\t"
		RSEQ_INJECT_ASM(4)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_CMP_CPU_ID(cpu_id, current_cpu_id, 6f)
		LONG_L " $4, %[v]\n\t"
		"bne $4, %[expect], 7f\n\t"
#endif
		/* try memcpy */
		"beqz %[len], 333f\n\t" \
		"222:\n\t" \
		"lb   $4, 0(%[src])\n\t" \
		"sb   $4, 0(%[dst])\n\t" \
		LONG_ADDI " %[src], 1\n\t" \
		LONG_ADDI " %[dst], 1\n\t" \
		LONG_ADDI " %[len], -1\n\t" \
		"bnez %[len], 222b\n\t" \
		"333:\n\t" \
		RSEQ_INJECT_ASM(5)
		"sync\n\t"	/* full sync provides store-release */
		/* final store */
		LONG_S " %[newv], %[v]\n\t"
		"2:\n\t"
		RSEQ_INJECT_ASM(6)
		/* teardown */
		LONG_L " %[len], %[rseq_scratch2]\n\t"
		LONG_L " %[dst], %[rseq_scratch1]\n\t"
		LONG_L " %[src], %[rseq_scratch0]\n\t"
		"b 8f\n\t"
		RSEQ_ASM_DEFINE_ABORT(3, 4,
				      /* teardown */
				      LONG_L " %[len], %[rseq_scratch2]\n\t"
				      LONG_L " %[dst], %[rseq_scratch1]\n\t"
				      LONG_L " %[src], %[rseq_scratch0]\n\t",
				      abort, 1b, 2b, 4f)
		RSEQ_ASM_DEFINE_CMPFAIL(5,
					/* teardown */
					LONG_L " %[len], %[rseq_scratch2]\n\t"
					LONG_L " %[dst], %[rseq_scratch1]\n\t"
					LONG_L " %[src], %[rseq_scratch0]\n\t",
					cmpfail)
#ifdef RSEQ_COMPARE_TWICE
		RSEQ_ASM_DEFINE_CMPFAIL(6,
					/* teardown */
					LONG_L " %[len], %[rseq_scratch2]\n\t"
					LONG_L " %[dst], %[rseq_scratch1]\n\t"
					LONG_L " %[src], %[rseq_scratch0]\n\t",
					error1)
		RSEQ_ASM_DEFINE_CMPFAIL(7,
					/* teardown */
					LONG_L " %[len], %[rseq_scratch2]\n\t"
					LONG_L " %[dst], %[rseq_scratch1]\n\t"
					LONG_L " %[src], %[rseq_scratch0]\n\t",
					error2)
#endif
		"8:\n\t"
		: /* gcc asm goto does not allow outputs */
		: [cpu_id]		"r" (cpu),
		  [current_cpu_id]	"m" (__rseq_abi.cpu_id),
		  [rseq_cs]		"m" (__rseq_abi.rseq_cs),
		  /* final store input */
		  [v]			"m" (*v),
		  [expect]		"r" (expect),
		  [newv]		"r" (newv),
		  /* try memcpy input */
		  [dst]			"r" (dst),
		  [src]			"r" (src),
		  [len]			"r" (len),
		  [rseq_scratch0]	"m" (rseq_scratch[0]),
		  [rseq_scratch1]	"m" (rseq_scratch[1]),
		  [rseq_scratch2]	"m" (rseq_scratch[2])
		  RSEQ_INJECT_INPUT
		: "$4", "memory"
		  RSEQ_INJECT_CLOBBER
		: abort, cmpfail
#ifdef RSEQ_COMPARE_TWICE
		  , error1, error2
#endif
	);
	rseq_workaround_gcc_asm_size_guess();
	return 0;
abort:
	rseq_workaround_gcc_asm_size_guess();
	RSEQ_INJECT_FAILED
	return -1;
cmpfail:
	rseq_workaround_gcc_asm_size_guess();
	return 1;
#ifdef RSEQ_COMPARE_TWICE
error1:
	rseq_workaround_gcc_asm_size_guess();
	rseq_bug("cpu_id comparison failed");
error2:
	rseq_workaround_gcc_asm_size_guess();
	rseq_bug("expected value comparison failed");
#endif
}

#endif /* !RSEQ_SKIP_FASTPATH */

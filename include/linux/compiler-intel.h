/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_TYPES_H
#error "Please don't include <linux/compiler-intel.h> directly, include <linux/compiler.h> instead."
#endif

#ifdef __ECC

/* Compiler specific definitions for Intel ECC compiler */

#include <asm/intrinsics.h>

/* Intel ECC compiler doesn't support gcc specific asm stmts.
 * It uses intrinsics to do the equivalent things.
 */

#define barrier() __memory_barrier()
#define barrier_data(ptr) barrier()

#define RELOC_HIDE(ptr, off)					\
  ({ unsigned long __ptr;					\
     __ptr = (unsigned long) (ptr);				\
    (typeof(ptr)) (__ptr + (off)); })

/* This should act as an optimization barrier on var.
 * Given that this compiler does not have inline assembly, a compiler barrier
 * is the best we can do.
 */
#define OPTIMIZER_HIDE_VAR(var) barrier()

#endif

/* icc has this, but it's called _bswap16 */
#define __HAVE_BUILTIN_BSWAP16__
#define __builtin_bswap16 _bswap16

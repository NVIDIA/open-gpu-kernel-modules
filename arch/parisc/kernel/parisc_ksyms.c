// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *    Architecture-specific kernel symbols
 *
 *    Copyright (C) 2000-2001 Richard Hirst <rhirst with parisc-linux.org>
 *    Copyright (C) 2001 Dave Kennedy
 *    Copyright (C) 2001 Paul Bame <bame at parisc-linux.org>
 *    Copyright (C) 2001-2003 Grant Grundler <grundler with parisc-linux.org>
 *    Copyright (C) 2002-2003 Matthew Wilcox <willy at parisc-linux.org>
 *    Copyright (C) 2002 Randolph Chung <tausq at parisc-linux.org>
 *    Copyright (C) 2002-2007 Helge Deller <deller with parisc-linux.org>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

#include <linux/string.h>
EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(strlen);
EXPORT_SYMBOL(strcpy);
EXPORT_SYMBOL(strncpy);
EXPORT_SYMBOL(strcat);

#include <linux/atomic.h>
EXPORT_SYMBOL(__xchg8);
EXPORT_SYMBOL(__xchg32);
EXPORT_SYMBOL(__cmpxchg_u32);
EXPORT_SYMBOL(__cmpxchg_u64);
#ifdef CONFIG_SMP
EXPORT_SYMBOL(__atomic_hash);
#endif
#ifdef CONFIG_64BIT
EXPORT_SYMBOL(__xchg64);
#endif

#include <linux/uaccess.h>
EXPORT_SYMBOL(lclear_user);
EXPORT_SYMBOL(lstrnlen_user);

#ifndef CONFIG_64BIT
/* Needed so insmod can set dp value */
extern int $global$;
EXPORT_SYMBOL($global$);
#endif

#include <asm/io.h>
EXPORT_SYMBOL(memcpy_toio);
EXPORT_SYMBOL(memcpy_fromio);
EXPORT_SYMBOL(memset_io);

extern void $$divI(void);
extern void $$divU(void);
extern void $$remI(void);
extern void $$remU(void);
extern void $$mulI(void);
extern void $$divU_3(void);
extern void $$divU_5(void);
extern void $$divU_6(void);
extern void $$divU_9(void);
extern void $$divU_10(void);
extern void $$divU_12(void);
extern void $$divU_7(void);
extern void $$divU_14(void);
extern void $$divU_15(void);
extern void $$divI_3(void);
extern void $$divI_5(void);
extern void $$divI_6(void);
extern void $$divI_7(void);
extern void $$divI_9(void);
extern void $$divI_10(void);
extern void $$divI_12(void);
extern void $$divI_14(void);
extern void $$divI_15(void);

EXPORT_SYMBOL($$divI);
EXPORT_SYMBOL($$divU);
EXPORT_SYMBOL($$remI);
EXPORT_SYMBOL($$remU);
EXPORT_SYMBOL($$mulI);
EXPORT_SYMBOL($$divU_3);
EXPORT_SYMBOL($$divU_5);
EXPORT_SYMBOL($$divU_6);
EXPORT_SYMBOL($$divU_9);
EXPORT_SYMBOL($$divU_10);
EXPORT_SYMBOL($$divU_12);
EXPORT_SYMBOL($$divU_7);
EXPORT_SYMBOL($$divU_14);
EXPORT_SYMBOL($$divU_15);
EXPORT_SYMBOL($$divI_3);
EXPORT_SYMBOL($$divI_5);
EXPORT_SYMBOL($$divI_6);
EXPORT_SYMBOL($$divI_7);
EXPORT_SYMBOL($$divI_9);
EXPORT_SYMBOL($$divI_10);
EXPORT_SYMBOL($$divI_12);
EXPORT_SYMBOL($$divI_14);
EXPORT_SYMBOL($$divI_15);

extern void __ashrdi3(void);
extern void __ashldi3(void);
extern void __lshrdi3(void);
extern void __muldi3(void);
extern void __ucmpdi2(void);

EXPORT_SYMBOL(__ashrdi3);
EXPORT_SYMBOL(__ashldi3);
EXPORT_SYMBOL(__lshrdi3);
EXPORT_SYMBOL(__muldi3);
EXPORT_SYMBOL(__ucmpdi2);

asmlinkage void * __canonicalize_funcptr_for_compare(void *);
EXPORT_SYMBOL(__canonicalize_funcptr_for_compare);

#ifdef CONFIG_64BIT
extern void __divdi3(void);
extern void __udivdi3(void);
extern void __umoddi3(void);
extern void __moddi3(void);

EXPORT_SYMBOL(__divdi3);
EXPORT_SYMBOL(__udivdi3);
EXPORT_SYMBOL(__umoddi3);
EXPORT_SYMBOL(__moddi3);
#endif

#ifndef CONFIG_64BIT
extern void $$dyncall(void);
EXPORT_SYMBOL($$dyncall);
#endif

#ifdef CONFIG_FUNCTION_TRACER
extern void _mcount(void);
EXPORT_SYMBOL(_mcount);
#endif

/* from pacache.S -- needed for clear/copy_page */
EXPORT_SYMBOL(clear_page_asm);
EXPORT_SYMBOL(copy_page_asm);

/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_MCE_H
#define AMDKCL_MCE_H

#ifdef CONFIG_X86

#include <asm/mce.h>

/* Copied from asm/mce.h */
#ifndef XEC
#define XEC(x, mask)			(((x) >> 16) & mask)
#endif

#endif /* CONFIG_X86 */
#endif

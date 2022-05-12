/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _ASM_IA64_MMIOWB_H
#define _ASM_IA64_MMIOWB_H

/**
 * mmiowb - I/O write barrier
 *
 * Ensure ordering of I/O space writes.  This will make sure that writes
 * following the barrier will arrive after all previous writes.  For most
 * ia64 platforms, this is a simple 'mf.a' instruction.
 */
#define mmiowb()	ia64_mfa()

#include <asm-generic/mmiowb.h>

#endif	/* _ASM_IA64_MMIOWB_H */

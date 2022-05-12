/*
 * arch/arm/mach-spear3xx/include/mach/misc_regs.h
 *
 * Miscellaneous registers definitions for SPEAr3xx machine family
 *
 * Copyright (C) 2009 ST Microelectronics
 * Viresh Kumar <vireshk@kernel.org>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MACH_MISC_REGS_H
#define __MACH_MISC_REGS_H

#include <mach/spear.h>

#define MISC_BASE		(VA_SPEAR_ICM3_MISC_REG_BASE)
#define DMA_CHN_CFG		(MISC_BASE + 0x0A0)

#endif /* __MACH_MISC_REGS_H */

/*
 * AM33XX PRM_XXX register bits
 *
 * Copyright (C) 2011-2012 Texas Instruments Incorporated - https://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_ARM_MACH_OMAP2_PRM_REGBITS_33XX_H
#define __ARCH_ARM_MACH_OMAP2_PRM_REGBITS_33XX_H

#include "prm.h"

#define AM33XX_GFX_MEM_ONSTATE_MASK			(0x3 << 17)
#define AM33XX_GFX_MEM_RETSTATE_MASK			(1 << 6)
#define AM33XX_GFX_MEM_STATEST_MASK			(0x3 << 4)
#define AM33XX_GLOBAL_WARM_SW_RST_MASK			(1 << 1)
#define AM33XX_RST_GLOBAL_WARM_SW_MASK			(1 << 0)
#define AM33XX_PRUSS_MEM_ONSTATE_MASK			(0x3 << 5)
#define AM33XX_PRUSS_MEM_RETSTATE_MASK			(1 << 7)
#define AM33XX_PRUSS_MEM_STATEST_MASK			(0x3 << 23)
#define AM33XX_LASTPOWERSTATEENTERED_SHIFT		24
#define AM33XX_LASTPOWERSTATEENTERED_MASK		(0x3 << 24)
#define AM33XX_LOGICRETSTATE_MASK			(1 << 2)
#define AM33XX_LOGICRETSTATE_3_3_MASK			(1 << 3)
#define AM33XX_LOGICSTATEST_SHIFT			2
#define AM33XX_LOGICSTATEST_MASK			(1 << 2)
#define AM33XX_LOWPOWERSTATECHANGE_SHIFT		4
#define AM33XX_LOWPOWERSTATECHANGE_MASK			(1 << 4)
#define AM33XX_MPU_L1_ONSTATE_MASK			(0x3 << 18)
#define AM33XX_MPU_L1_RETSTATE_MASK			(1 << 22)
#define AM33XX_MPU_L1_STATEST_MASK			(0x3 << 6)
#define AM33XX_MPU_L2_ONSTATE_MASK			(0x3 << 20)
#define AM33XX_MPU_L2_RETSTATE_MASK			(1 << 23)
#define AM33XX_MPU_L2_STATEST_MASK			(0x3 << 8)
#define AM33XX_MPU_RAM_ONSTATE_MASK			(0x3 << 16)
#define AM33XX_MPU_RAM_RETSTATE_MASK			(1 << 24)
#define AM33XX_MPU_RAM_STATEST_MASK			(0x3 << 4)
#define AM33XX_PER_MEM_ONSTATE_MASK			(0x3 << 25)
#define AM33XX_PER_MEM_RETSTATE_MASK			(1 << 29)
#define AM33XX_PER_MEM_STATEST_MASK			(0x3 << 17)
#define AM33XX_RAM_MEM_ONSTATE_MASK			(0x3 << 30)
#define AM33XX_RAM_MEM_RETSTATE_MASK			(1 << 27)
#define AM33XX_RAM_MEM_STATEST_MASK			(0x3 << 21)
#endif

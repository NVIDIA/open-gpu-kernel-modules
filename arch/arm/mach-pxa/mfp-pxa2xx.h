/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_ARCH_MFP_PXA2XX_H
#define __ASM_ARCH_MFP_PXA2XX_H

#include <plat/mfp.h>

/*
 * the following MFP_xxx bit definitions in mfp.h are re-used for pxa2xx:
 *
 *  MFP_PIN(x)
 *  MFP_AFx
 *  MFP_LPM_DRIVE_{LOW, HIGH}
 *  MFP_LPM_EDGE_x
 *
 * other MFP_x bit definitions will be ignored
 *
 * and adds the below two bits specifically for pxa2xx:
 *
 * bit     23 - Input/Output (PXA2xx specific)
 * bit     24 - Wakeup Enable(PXA2xx specific)
 * bit     25 - Keep Output  (PXA2xx specific)
 */

#define MFP_DIR_IN		(0x0 << 23)
#define MFP_DIR_OUT		(0x1 << 23)
#define MFP_DIR_MASK		(0x1 << 23)
#define MFP_DIR(x)		(((x) >> 23) & 0x1)

#define MFP_LPM_CAN_WAKEUP	(0x1 << 24)

/*
 * MFP_LPM_KEEP_OUTPUT must be specified for pins that need to
 * retain their last output level (low or high).
 * Note: MFP_LPM_KEEP_OUTPUT has no effect on pins configured for input.
 */
#define MFP_LPM_KEEP_OUTPUT	(0x1 << 25)

#define WAKEUP_ON_EDGE_RISE	(MFP_LPM_CAN_WAKEUP | MFP_LPM_EDGE_RISE)
#define WAKEUP_ON_EDGE_FALL	(MFP_LPM_CAN_WAKEUP | MFP_LPM_EDGE_FALL)
#define WAKEUP_ON_EDGE_BOTH	(MFP_LPM_CAN_WAKEUP | MFP_LPM_EDGE_BOTH)

/* specifically for enabling wakeup on keypad GPIOs */
#define WAKEUP_ON_LEVEL_HIGH	(MFP_LPM_CAN_WAKEUP)

#define MFP_CFG_IN(pin, af)		\
	((MFP_CFG_DEFAULT & ~(MFP_AF_MASK | MFP_DIR_MASK)) |\
	 (MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DIR_IN))

/* NOTE:  pins configured as output _must_ provide a low power state,
 * and this state should help to minimize the power dissipation.
 */
#define MFP_CFG_OUT(pin, af, state)	\
	((MFP_CFG_DEFAULT & ~(MFP_AF_MASK | MFP_DIR_MASK | MFP_LPM_STATE_MASK)) |\
	 (MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DIR_OUT | MFP_LPM_##state))

/* Common configurations for pxa25x and pxa27x
 *
 * Note: pins configured as GPIO are always initialized to input
 * so not to cause any side effect
 */
#define GPIO0_GPIO	MFP_CFG_IN(GPIO0, AF0)
#define GPIO1_GPIO	MFP_CFG_IN(GPIO1, AF0)
#define GPIO9_GPIO	MFP_CFG_IN(GPIO9, AF0)
#define GPIO10_GPIO	MFP_CFG_IN(GPIO10, AF0)
#define GPIO11_GPIO	MFP_CFG_IN(GPIO11, AF0)
#define GPIO12_GPIO	MFP_CFG_IN(GPIO12, AF0)
#define GPIO13_GPIO	MFP_CFG_IN(GPIO13, AF0)
#define GPIO14_GPIO	MFP_CFG_IN(GPIO14, AF0)
#define GPIO15_GPIO	MFP_CFG_IN(GPIO15, AF0)
#define GPIO16_GPIO	MFP_CFG_IN(GPIO16, AF0)
#define GPIO17_GPIO	MFP_CFG_IN(GPIO17, AF0)
#define GPIO18_GPIO	MFP_CFG_IN(GPIO18, AF0)
#define GPIO19_GPIO	MFP_CFG_IN(GPIO19, AF0)
#define GPIO20_GPIO	MFP_CFG_IN(GPIO20, AF0)
#define GPIO21_GPIO	MFP_CFG_IN(GPIO21, AF0)
#define GPIO22_GPIO	MFP_CFG_IN(GPIO22, AF0)
#define GPIO23_GPIO	MFP_CFG_IN(GPIO23, AF0)
#define GPIO24_GPIO	MFP_CFG_IN(GPIO24, AF0)
#define GPIO25_GPIO	MFP_CFG_IN(GPIO25, AF0)
#define GPIO26_GPIO	MFP_CFG_IN(GPIO26, AF0)
#define GPIO27_GPIO	MFP_CFG_IN(GPIO27, AF0)
#define GPIO28_GPIO	MFP_CFG_IN(GPIO28, AF0)
#define GPIO29_GPIO	MFP_CFG_IN(GPIO29, AF0)
#define GPIO30_GPIO	MFP_CFG_IN(GPIO30, AF0)
#define GPIO31_GPIO	MFP_CFG_IN(GPIO31, AF0)
#define GPIO32_GPIO	MFP_CFG_IN(GPIO32, AF0)
#define GPIO33_GPIO	MFP_CFG_IN(GPIO33, AF0)
#define GPIO34_GPIO	MFP_CFG_IN(GPIO34, AF0)
#define GPIO35_GPIO	MFP_CFG_IN(GPIO35, AF0)
#define GPIO36_GPIO	MFP_CFG_IN(GPIO36, AF0)
#define GPIO37_GPIO	MFP_CFG_IN(GPIO37, AF0)
#define GPIO38_GPIO	MFP_CFG_IN(GPIO38, AF0)
#define GPIO39_GPIO	MFP_CFG_IN(GPIO39, AF0)
#define GPIO40_GPIO	MFP_CFG_IN(GPIO40, AF0)
#define GPIO41_GPIO	MFP_CFG_IN(GPIO41, AF0)
#define GPIO42_GPIO	MFP_CFG_IN(GPIO42, AF0)
#define GPIO43_GPIO	MFP_CFG_IN(GPIO43, AF0)
#define GPIO44_GPIO	MFP_CFG_IN(GPIO44, AF0)
#define GPIO45_GPIO	MFP_CFG_IN(GPIO45, AF0)
#define GPIO46_GPIO	MFP_CFG_IN(GPIO46, AF0)
#define GPIO47_GPIO	MFP_CFG_IN(GPIO47, AF0)
#define GPIO48_GPIO	MFP_CFG_IN(GPIO48, AF0)
#define GPIO49_GPIO	MFP_CFG_IN(GPIO49, AF0)
#define GPIO50_GPIO	MFP_CFG_IN(GPIO50, AF0)
#define GPIO51_GPIO	MFP_CFG_IN(GPIO51, AF0)
#define GPIO52_GPIO	MFP_CFG_IN(GPIO52, AF0)
#define GPIO53_GPIO	MFP_CFG_IN(GPIO53, AF0)
#define GPIO54_GPIO	MFP_CFG_IN(GPIO54, AF0)
#define GPIO55_GPIO	MFP_CFG_IN(GPIO55, AF0)
#define GPIO56_GPIO	MFP_CFG_IN(GPIO56, AF0)
#define GPIO57_GPIO	MFP_CFG_IN(GPIO57, AF0)
#define GPIO58_GPIO	MFP_CFG_IN(GPIO58, AF0)
#define GPIO59_GPIO	MFP_CFG_IN(GPIO59, AF0)
#define GPIO60_GPIO	MFP_CFG_IN(GPIO60, AF0)
#define GPIO61_GPIO	MFP_CFG_IN(GPIO61, AF0)
#define GPIO62_GPIO	MFP_CFG_IN(GPIO62, AF0)
#define GPIO63_GPIO	MFP_CFG_IN(GPIO63, AF0)
#define GPIO64_GPIO	MFP_CFG_IN(GPIO64, AF0)
#define GPIO65_GPIO	MFP_CFG_IN(GPIO65, AF0)
#define GPIO66_GPIO	MFP_CFG_IN(GPIO66, AF0)
#define GPIO67_GPIO	MFP_CFG_IN(GPIO67, AF0)
#define GPIO68_GPIO	MFP_CFG_IN(GPIO68, AF0)
#define GPIO69_GPIO	MFP_CFG_IN(GPIO69, AF0)
#define GPIO70_GPIO	MFP_CFG_IN(GPIO70, AF0)
#define GPIO71_GPIO	MFP_CFG_IN(GPIO71, AF0)
#define GPIO72_GPIO	MFP_CFG_IN(GPIO72, AF0)
#define GPIO73_GPIO	MFP_CFG_IN(GPIO73, AF0)
#define GPIO74_GPIO	MFP_CFG_IN(GPIO74, AF0)
#define GPIO75_GPIO	MFP_CFG_IN(GPIO75, AF0)
#define GPIO76_GPIO	MFP_CFG_IN(GPIO76, AF0)
#define GPIO77_GPIO	MFP_CFG_IN(GPIO77, AF0)
#define GPIO78_GPIO	MFP_CFG_IN(GPIO78, AF0)
#define GPIO79_GPIO	MFP_CFG_IN(GPIO79, AF0)
#define GPIO80_GPIO	MFP_CFG_IN(GPIO80, AF0)
#define GPIO81_GPIO	MFP_CFG_IN(GPIO81, AF0)
#define GPIO82_GPIO	MFP_CFG_IN(GPIO82, AF0)
#define GPIO83_GPIO	MFP_CFG_IN(GPIO83, AF0)
#define GPIO84_GPIO	MFP_CFG_IN(GPIO84, AF0)

extern void pxa2xx_mfp_config(unsigned long *mfp_cfgs, int num);
extern void pxa2xx_mfp_set_lpm(int mfp, unsigned long lpm);
extern int gpio_set_wake(unsigned int gpio, unsigned int on);
#endif /* __ASM_ARCH_MFP_PXA2XX_H */

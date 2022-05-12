/*
 * Platform specific functions
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 *
 * Copyright (C) 2001 - 2005 Tensilica Inc.
 */

#ifndef _XTENSA_PLATFORM_H
#define _XTENSA_PLATFORM_H

#include <linux/types.h>
#include <asm/bootparam.h>

/*
 * platform_init is called before the mmu is initialized to give the
 * platform a early hook-up. bp_tag_t is a list of configuration tags
 * passed from the boot-loader.
 */
extern void platform_init(bp_tag_t*);

/*
 * platform_setup is called from setup_arch with a pointer to the command-line
 * string.
 */
extern void platform_setup (char **);

/*
 * platform_restart is called to restart the system.
 */
extern void platform_restart (void);

/*
 * platform_halt is called to stop the system and halt.
 */
extern void platform_halt (void);

/*
 * platform_power_off is called to stop the system and power it off.
 */
extern void platform_power_off (void);

/*
 * platform_idle is called from the idle function.
 */
extern void platform_idle (void);

/*
 * platform_heartbeat is called every HZ
 */
extern void platform_heartbeat (void);

/*
 * platform_calibrate_ccount calibrates cpu clock freq (CONFIG_XTENSA_CALIBRATE)
 */
extern void platform_calibrate_ccount (void);

/*
 * Flush and reset the mmu, simulate a processor reset, and
 * jump to the reset vector.
 */
void cpu_reset(void) __attribute__((noreturn));

#endif	/* _XTENSA_PLATFORM_H */

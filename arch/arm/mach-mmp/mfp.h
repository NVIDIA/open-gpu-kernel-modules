/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_MACH_MFP_H
#define __ASM_MACH_MFP_H

#include <plat/mfp.h>

/*
 * NOTE: the MFPR register bit definitions on PXA168 processor lines are a
 * bit different from those on PXA3xx.  Bit [7:10] are now reserved, which
 * were SLEEP_OE_N, SLEEP_DATA, SLEEP_SEL and the LSB of DRIVE bits.
 *
 * To cope with this difference and re-use the pxa3xx mfp code as much as
 * possible, we make the following compromise:
 *
 * 1. SLEEP_OE_N will always be programmed to '1' (by MFP_LPM_FLOAT)
 * 2. DRIVE strength definitions redefined to include the reserved bit
 *    - the reserved bit differs between pxa168 and pxa910, and the
 *      MFP_DRIVE_* macros are individually defined in mfp-pxa{168,910}.h
 * 3. Override MFP_CFG() and MFP_CFG_DRV()
 * 4. Drop the use of MFP_CFG_LPM() and MFP_CFG_X()
 */

#undef MFP_CFG
#undef MFP_CFG_DRV
#undef MFP_CFG_LPM
#undef MFP_CFG_X
#undef MFP_CFG_DEFAULT

#define MFP_CFG(pin, af)		\
	(MFP_LPM_FLOAT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM)

#define MFP_CFG_DRV(pin, af, drv)	\
	(MFP_LPM_FLOAT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_##drv)

#endif /* __ASM_MACH_MFP_H */

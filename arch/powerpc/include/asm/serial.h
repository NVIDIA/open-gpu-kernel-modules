/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 */
#ifndef _ASM_POWERPC_SERIAL_H
#define _ASM_POWERPC_SERIAL_H

/*
 * Serial ports are not listed here, because they are discovered
 * through the device tree.
 */

/* Default baud base if not found in device-tree */
#define BASE_BAUD ( 1843200 / 16 )

#ifdef CONFIG_PPC_UDBG_16550
extern void find_legacy_serial_ports(void);
#else
#define find_legacy_serial_ports()	do { } while (0)
#endif

#endif /* _PPC64_SERIAL_H */

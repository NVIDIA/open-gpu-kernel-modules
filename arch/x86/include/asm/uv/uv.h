/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_UV_UV_H
#define _ASM_X86_UV_UV_H

enum uv_system_type {UV_NONE, UV_LEGACY_APIC, UV_X2APIC};

#ifdef CONFIG_X86_UV
#include <linux/efi.h>

#define	UV_PROC_NODE	"sgi_uv"

static inline int uv(int uvtype)
{
	/* uv(0) is "any" */
	if (uvtype >= 0 && uvtype <= 30)
		return 1 << uvtype;
	return 1;
}

extern unsigned long uv_systab_phys;

extern enum uv_system_type get_uv_system_type(void);
static inline bool is_early_uv_system(void)
{
	return uv_systab_phys && uv_systab_phys != EFI_INVALID_TABLE_ADDR;
}
extern int is_uv_system(void);
extern int is_uv_hubbed(int uvtype);
extern void uv_cpu_init(void);
extern void uv_nmi_init(void);
extern void uv_system_init(void);

#else	/* !X86_UV */

static inline enum uv_system_type get_uv_system_type(void) { return UV_NONE; }
static inline bool is_early_uv_system(void)	{ return 0; }
static inline int is_uv_system(void)	{ return 0; }
static inline int is_uv_hubbed(int uv)	{ return 0; }
static inline void uv_cpu_init(void)	{ }
static inline void uv_system_init(void)	{ }

#endif	/* X86_UV */

#endif	/* _ASM_X86_UV_UV_H */

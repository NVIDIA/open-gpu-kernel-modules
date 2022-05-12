/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 Western Digital Corporation or its affiliates.
 */
#ifndef _ASM_EFI_H
#define _ASM_EFI_H

#include <asm/csr.h>
#include <asm/io.h>
#include <asm/mmu_context.h>
#include <asm/ptrace.h>
#include <asm/tlbflush.h>

#ifdef CONFIG_EFI
extern void efi_init(void);
#else
#define efi_init()
#endif

int efi_create_mapping(struct mm_struct *mm, efi_memory_desc_t *md);
int efi_set_mapping_permissions(struct mm_struct *mm, efi_memory_desc_t *md);

#define arch_efi_call_virt_setup()      efi_virtmap_load()
#define arch_efi_call_virt_teardown()   efi_virtmap_unload()

#define arch_efi_call_virt(p, f, args...) p->f(args)

#define ARCH_EFI_IRQ_FLAGS_MASK (SR_IE | SR_SPIE)

/* Load initrd at enough distance from DRAM start */
static inline unsigned long efi_get_max_initrd_addr(unsigned long image_addr)
{
	return image_addr + SZ_256M;
}

#define alloc_screen_info(x...)		(&screen_info)

static inline void free_screen_info(struct screen_info *si)
{
}

static inline void efifb_setup_from_dmi(struct screen_info *si, const char *opt)
{
}

void efi_virtmap_load(void);
void efi_virtmap_unload(void);

#endif /* _ASM_EFI_H */

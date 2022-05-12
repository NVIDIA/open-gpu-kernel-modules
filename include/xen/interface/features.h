/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 * features.h
 *
 * Feature flags, reported by XENVER_get_features.
 *
 * Copyright (c) 2006, Keir Fraser <keir@xensource.com>
 */

#ifndef __XEN_PUBLIC_FEATURES_H__
#define __XEN_PUBLIC_FEATURES_H__

/*
 * If set, the guest does not need to write-protect its pagetables, and can
 * update them via direct writes.
 */
#define XENFEAT_writable_page_tables       0

/*
 * If set, the guest does not need to write-protect its segment descriptor
 * tables, and can update them via direct writes.
 */
#define XENFEAT_writable_descriptor_tables 1

/*
 * If set, translation between the guest's 'pseudo-physical' address space
 * and the host's machine address space are handled by the hypervisor. In this
 * mode the guest does not need to perform phys-to/from-machine translations
 * when performing page table operations.
 */
#define XENFEAT_auto_translated_physmap    2

/* If set, the guest is running in supervisor mode (e.g., x86 ring 0). */
#define XENFEAT_supervisor_mode_kernel     3

/*
 * If set, the guest does not need to allocate x86 PAE page directories
 * below 4GB. This flag is usually implied by auto_translated_physmap.
 */
#define XENFEAT_pae_pgdir_above_4gb        4

/* x86: Does this Xen host support the MMU_PT_UPDATE_PRESERVE_AD hypercall? */
#define XENFEAT_mmu_pt_update_preserve_ad  5

/* x86: Does this Xen host support the MMU_{CLEAR,COPY}_PAGE hypercall? */
#define XENFEAT_highmem_assist             6

/*
 * If set, GNTTABOP_map_grant_ref honors flags to be placed into guest kernel
 * available pte bits.
 */
#define XENFEAT_gnttab_map_avail_bits      7

/* x86: Does this Xen host support the HVM callback vector type? */
#define XENFEAT_hvm_callback_vector        8

/* x86: pvclock algorithm is safe to use on HVM */
#define XENFEAT_hvm_safe_pvclock           9

/* x86: pirq can be used by HVM guests */
#define XENFEAT_hvm_pirqs           10

/* operation as Dom0 is supported */
#define XENFEAT_dom0                      11

/* Xen also maps grant references at pfn = mfn.
 * This feature flag is deprecated and should not be used.
#define XENFEAT_grant_map_identity        12
 */

/* Guest can use XENMEMF_vnode to specify virtual node for memory op. */
#define XENFEAT_memory_op_vnode_supported 13

/* arm: Hypervisor supports ARM SMC calling convention. */
#define XENFEAT_ARM_SMCCC_supported       14

/*
 * x86/PVH: If set, ACPI RSDP can be placed at any address. Otherwise RSDP
 * must be located in lower 1MB, as required by ACPI Specification for IA-PC
 * systems.
 * This feature flag is only consulted if XEN_ELFNOTE_GUEST_OS contains
 * the "linux" string.
 */
#define XENFEAT_linux_rsdp_unrestricted   15

/*
 * A direct-mapped (or 1:1 mapped) domain is a domain for which its
 * local pages have gfn == mfn. If a domain is direct-mapped,
 * XENFEAT_direct_mapped is set; otherwise XENFEAT_not_direct_mapped
 * is set.
 *
 * If neither flag is set (e.g. older Xen releases) the assumptions are:
 * - not auto_translated domains (x86 only) are always direct-mapped
 * - on x86, auto_translated domains are not direct-mapped
 * - on ARM, Dom0 is direct-mapped, DomUs are not
 */
#define XENFEAT_not_direct_mapped         16
#define XENFEAT_direct_mapped             17

#define XENFEAT_NR_SUBMAPS 1

#endif /* __XEN_PUBLIC_FEATURES_H__ */

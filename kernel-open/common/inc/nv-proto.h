/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _NV_PROTO_H_
#define _NV_PROTO_H_

#include "nv-pci.h"

extern const char *nv_device_name;

void        nv_acpi_register_notifier   (nv_linux_state_t *);
void        nv_acpi_unregister_notifier (nv_linux_state_t *);

NvU8        nv_find_pci_capability      (struct pci_dev *, NvU8);

int         nv_procfs_init              (void);
void        nv_procfs_exit              (void);
void        nv_procfs_add_warning       (const char *, const char *);
int         nv_procfs_add_gpu           (nv_linux_state_t *);
void        nv_procfs_remove_gpu        (nv_linux_state_t *);

int         nvidia_mmap                 (struct file *, struct vm_area_struct *);
int         nvidia_mmap_helper          (nv_state_t *, nv_linux_file_private_t *, nvidia_stack_t *, struct vm_area_struct *, void *);
int         nv_encode_caching           (pgprot_t *, NvU32, NvU32);
void        nv_revoke_gpu_mappings_locked(nv_state_t *);

NvUPtr      nv_vm_map_pages             (struct page **, NvU32, NvBool, NvBool);
void        nv_vm_unmap_pages           (NvUPtr, NvU32);

NV_STATUS   nv_alloc_contig_pages       (nv_state_t *, nv_alloc_t *);
void        nv_free_contig_pages        (nv_alloc_t *);
NV_STATUS   nv_alloc_system_pages       (nv_state_t *, nv_alloc_t *);
void        nv_free_system_pages        (nv_alloc_t *);

int         nv_uvm_init                 (void);
void        nv_uvm_exit                 (void);
NV_STATUS   nv_uvm_suspend              (void);
NV_STATUS   nv_uvm_resume               (void);
void        nv_uvm_notify_start_device  (const NvU8 *uuid);
void        nv_uvm_notify_stop_device   (const NvU8 *uuid);
NV_STATUS   nv_uvm_event_interrupt      (const NvU8 *uuid);

/* Move these to nv.h once implemented by other UNIX platforms */
NvBool      nvidia_get_gpuid_list       (NvU32 *gpu_ids, NvU32 *gpu_count);
int         nvidia_dev_get              (NvU32, nvidia_stack_t *);
void        nvidia_dev_put              (NvU32, nvidia_stack_t *);
int         nvidia_dev_get_uuid         (const NvU8 *, nvidia_stack_t *);
void        nvidia_dev_put_uuid         (const NvU8 *, nvidia_stack_t *);
int         nvidia_dev_block_gc6        (const NvU8 *, nvidia_stack_t *);
int         nvidia_dev_unblock_gc6      (const NvU8 *, nvidia_stack_t *);

#if defined(CONFIG_PM)
NV_STATUS     nv_set_system_power_state (nv_power_state_t, nv_pm_action_depth_t);
#endif

void          nvidia_modeset_suspend           (NvU32 gpuId);
void          nvidia_modeset_resume            (NvU32 gpuId);
NvBool        nv_is_uuid_in_gpu_exclusion_list (const char *);

NV_STATUS     nv_parse_per_device_option_string(nvidia_stack_t *sp);
nv_linux_state_t * find_uuid(const NvU8 *uuid);
void          nv_report_error(struct pci_dev *dev, NvU32 error_number, const char *format, va_list ap);
void          nv_shutdown_adapter(nvidia_stack_t *, nv_state_t *, nv_linux_state_t *);
void          nv_dev_free_stacks(nv_linux_state_t *);
NvBool        nv_lock_init_locks(nvidia_stack_t *, nv_state_t *);
void          nv_lock_destroy_locks(nvidia_stack_t *, nv_state_t *);
int           nv_linux_add_device_locked(nv_linux_state_t *);
void          nv_linux_remove_device_locked(nv_linux_state_t *);
NvBool        nv_acpi_power_resource_method_present(struct pci_dev *);

int           nv_linux_init_open_q(nv_linux_state_t *);
void          nv_linux_stop_open_q(nv_linux_state_t *);

#endif /* _NV_PROTO_H_ */

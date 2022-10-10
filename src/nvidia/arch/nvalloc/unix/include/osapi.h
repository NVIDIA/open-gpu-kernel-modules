/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _OSAPI_H_
#define _OSAPI_H_

#include "core/system.h"
#include "gpu/gpu.h"

#include <os-interface.h> // NV_DBG_ERRORS
#include <rmapi/rmapi.h>
#include <core/thread_state.h>

#if defined(__use_altstack__)
#if defined(QA_BUILD)
//---------------------------------------------------------------------------
//
// 32 bit debug marker values.
//
//---------------------------------------------------------------------------

#define NV_MARKER1 (NvU32)(('M' << 24) | ('R' << 16) | ('V' << 8) | 'N')
#define NV_MARKER2 (NvU32)(('N' << 24) | ('V' << 16) | ('R' << 8) | 'M')

//
// The two macros below implement a simple alternate stack usage sanity
// check for QA_BUILD RM builds. NV_ALTSTACK_WRITE_MARKERS() fills
// altstacks with NV_MARKER1, which enables NV_ALTSTACK_CHECK_MARKERS()
// to determine the stack usage fairly reliably by looking for the
// first clobbered marker. If more than 7/8 of the alternate stack were
// used, NV_ALTSTACK_CHECK_MARKERS() prints an error and asserts.
//
#define NV_ALTSTACK_WRITE_MARKERS(sp)                                           \
{                                                                               \
    NvU32 i, *stack = (void *)(sp)->stack;                                      \
    for (i = 0; i < ((sp)->size / sizeof(NvU32)); i++)                          \
        stack[i] = NV_MARKER1;                                                  \
}

#define NV_ALTSTACK_CHECK_MARKERS(sp)                                           \
{                                                                               \
    NvU32 i, *stack = (void *)(sp)->stack;                                      \
    for (i = 0; i < ((sp)->size / sizeof(NvU32)); i++)                          \
    {                                                                           \
        if (stack[i] != NV_MARKER1)                                             \
            break;                                                              \
    }                                                                           \
    if ((i * sizeof(NvU32)) < ((sp)->size / 8))                                 \
    {                                                                           \
        nv_printf(NV_DBG_ERRORS, "NVRM: altstack: used %d of %d bytes!\n",      \
                  ((sp)->size - (i * sizeof(NvU32))), (sp)->size);              \
        NV_ASSERT_PRECOMP((i * sizeof(NvU32)) >= ((sp)->size / 8));             \
    }                                                                           \
}
#else
#define NV_ALTSTACK_WRITE_MARKERS(sp)
#define NV_ALTSTACK_CHECK_MARKERS(sp)
#endif
#if defined(NVCPU_X86_64)
#define NV_ENTER_RM_RUNTIME(sp,fp)                                              \
{                                                                               \
    NV_ALTSTACK_WRITE_MARKERS(sp);                                              \
    __asm__ __volatile__ ("movq %%rbp,%0" : "=r" (fp)); /* save %rbp */         \
    __asm__ __volatile__ ("movq %0,%%rbp" :: "r" ((sp)->top));                  \
}

#define NV_EXIT_RM_RUNTIME(sp,fp)                                               \
{                                                                               \
    register void *__rbp __asm__ ("rbp");                                       \
    if (__rbp != (sp)->top)                                                     \
    {                                                                           \
        nv_printf(NV_DBG_ERRORS, "NVRM: detected corrupted runtime stack!\n");  \
        NV_ASSERT_PRECOMP(__rbp == (sp)->top);                                  \
    }                                                                           \
    NV_ALTSTACK_CHECK_MARKERS(sp);                                              \
    __asm__ __volatile__ ("movq %0,%%rbp" :: "r" (fp)); /* restore %rbp */      \
}
#else
#error "gcc \"altstacks\" support is not implemented on this platform!"
#endif
#else
#define NV_ENTER_RM_RUNTIME(sp,fp) { (void)sp; (void)fp; }
#define NV_EXIT_RM_RUNTIME(sp,fp)
#endif

void       RmShutdownRm           (void);

NvBool     RmInitPrivateState     (nv_state_t *);
void       RmFreePrivateState     (nv_state_t *);

NvBool     RmInitAdapter          (nv_state_t *);
NvBool     RmPartiallyInitAdapter (nv_state_t *);
void       RmShutdownAdapter      (nv_state_t *);
void       RmDisableAdapter       (nv_state_t *);
void       RmPartiallyDisableAdapter(nv_state_t *);
NV_STATUS  RmGetAdapterStatus     (nv_state_t *, NvU32 *);
NV_STATUS  RmExcludeAdapter       (nv_state_t *);

NvBool     RmGpuHasIOSpaceEnabled (nv_state_t *);

void       RmFreeUnusedClients    (nv_state_t *, nv_file_private_t *);
NV_STATUS  RmIoctl                (nv_state_t *, nv_file_private_t *, NvU32, void *, NvU32);

NV_STATUS  RmAllocOsEvent         (NvHandle, nv_file_private_t *, NvU32);
NV_STATUS  RmFreeOsEvent          (NvHandle, NvU32);

void       RmI2cAddGpuPorts(nv_state_t *);

NV_STATUS  RmInitX86EmuState(OBJGPU *);
void       RmFreeX86EmuState(OBJGPU *);
NV_STATUS  RmSystemEvent(nv_state_t *, NvU32, NvU32);

const NvU8 *RmGetGpuUuidRaw(nv_state_t *);

NV_STATUS  nv_vbios_call(OBJGPU *, NvU32 *, NvU32 *);

int        amd_adv_spec_cache_feature(OBJOS *);
int        amd_msr_c0011022_incompatible(OBJOS *);

NV_STATUS  rm_get_adapter_status    (nv_state_t *, NvU32 *);

NV_STATUS  rm_alloc_os_event        (NvHandle, nv_file_private_t *, NvU32);
NV_STATUS  rm_free_os_event         (NvHandle, NvU32);
NV_STATUS  rm_get_event_data        (nv_file_private_t *, NvP64, NvU32 *);
void       rm_client_free_os_events (NvHandle);

NV_STATUS  rm_create_mmap_context   (nv_state_t *, NvHandle, NvHandle, NvHandle, NvP64, NvU64, NvU64, NvU32, NvU32);
NV_STATUS  rm_update_device_mapping_info  (NvHandle, NvHandle, NvHandle, void *, void *);

NV_STATUS  rm_access_registry       (NvHandle, NvHandle, NvU32, NvP64, NvU32, NvP64, NvU32, NvP64, NvU32 *, NvU32 *, NvU32 *);

// registry management
NV_STATUS  RmInitRegistry           (void);
NV_STATUS  RmDestroyRegistry        (nv_state_t *);

NV_STATUS  RmWriteRegistryDword     (nv_state_t *, const char *, NvU32 );
NV_STATUS  RmReadRegistryDword      (nv_state_t *, const char *, NvU32 *);
NV_STATUS  RmWriteRegistryString    (nv_state_t *, const char *, const char *, NvU32);
NV_STATUS  RmReadRegistryBinary     (nv_state_t *, const char *, NvU8 *, NvU32 *);
NV_STATUS  RmWriteRegistryBinary    (nv_state_t *, const char *, NvU8 *, NvU32);
NV_STATUS  RmReadRegistryString     (nv_state_t *, const char *, NvU8 *, NvU32 *);

NV_STATUS  RmPackageRegistry        (nv_state_t *, PACKED_REGISTRY_TABLE *, NvU32 *);

NvBool     RmIsNvifFunctionSupported(NvU32, NvU32);
void       RmInitAcpiMethods        (OBJOS *, OBJSYS *, OBJGPU *);
void       RmUnInitAcpiMethods      (OBJSYS *);

void       RmInflateOsToRmPageArray (RmPhysAddr *, NvU64);
void       RmDeflateRmToOsPageArray (RmPhysAddr *, NvU64);

void       RmInitS0ixPowerManagement              (nv_state_t *);
void       RmInitDeferredDynamicPowerManagement   (nv_state_t *);
void       RmDestroyDeferredDynamicPowerManagement(nv_state_t *);

NV_STATUS  os_ref_dynamic_power     (nv_state_t *, nv_dynamic_power_mode_t);
void       os_unref_dynamic_power   (nv_state_t *, nv_dynamic_power_mode_t);
void       RmHandleDisplayChange    (nvidia_stack_t *, nv_state_t *);
void       RmUpdateGc6ConsoleRefCount (nv_state_t *, NvBool);

NvBool     rm_get_uefi_console_status (nv_state_t *);
NvU64      rm_get_uefi_console_size (nv_state_t *, NvU64 *);

RM_API    *RmUnixRmApiPrologue      (nv_state_t *, THREAD_STATE_NODE *, NvU32 module);
void       RmUnixRmApiEpilogue      (nv_state_t *, THREAD_STATE_NODE *);

static inline NvBool rm_is_system_notebook(void)
{
    return (nv_is_chassis_notebook() || nv_acpi_is_battery_present());
}

#endif // _OSAPI_H_

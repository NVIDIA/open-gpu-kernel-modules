/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "core/core.h"

#include "resserv/rs_resource.h"
#include "resserv/resserv.h"

#include "utils/nvmacro.h"
#include "rmapi/param_copy.h"

struct NVOC_EXPORTED_METHOD_DEF;
typedef RS_RES_CONTROL_PARAMS_INTERNAL RmCtrlParams;

// RMCTRL_API_COPPY_FLAGS is used to specify control api copy behavior.
#define RMCTRL_API_COPY_FLAGS_NONE                              0x00000000

// skip memory copy in api copy in and zero the buffer
#define RMCTRL_API_COPY_FLAGS_SKIP_COPYIN_ZERO_BUFFER           NVBIT(0)

// set control cache on api copy out
#define RMCTRL_API_COPY_FLAGS_SET_CONTROL_CACHE                 NVBIT(1)

// skip copy out even for controls with RMCTRL_FLAGS_COPYOUT_ON_ERROR
#define RMCTRL_API_COPY_FLAGS_FORCE_SKIP_COPYOUT_ON_ERROR       NVBIT(2)

//
// RmCtrlExecuteCookie
//
// This typedef describes the data used by the rmctrl cmd execution
// path.  The data is filled at the beginning of rmControlCmdExecute()
// and used as necessary in the other stages.
//
struct RS_CONTROL_COOKIE
{
    // Rmctrl Command ID
    NvU32        cmd;

    // Rmctrl Flags
    NvU32        ctrlFlags;

    // API Copy Flags
    NvU32        apiCopyFlags;

    // Required Access Rights for this command
    const RS_ACCESS_MASK rightsRequired;

    NvBool              bFreeParamCopy;    ///< Indicates that param copies should be cleaned up
    NvBool              bFreeEmbeddedCopy; ///< Indicates embedded param copies should be cleaned up

    RMAPI_PARAM_COPY    paramCopy;
    RMAPI_PARAM_COPY    embeddedParamCopies[4];   // Up to 4 embedded pointers per one RmControl identified
};
typedef RS_CONTROL_COOKIE RmCtrlExecuteCookie;

// values for RmCtrlDeferredCmd.pending
#define RMCTRL_DEFERRED_FREE        0            // buffer is free
#define RMCTRL_DEFERRED_ACQUIRED    1            // buffer is acquired to fill in data
#define RMCTRL_DEFERRED_READY       2            // buffer is acquired and data has been copied.

#define RMCTRL_DEFERRED_MAX_PARAM_SIZE       128 // 128 bytes internal buffer for rmctrl param

typedef struct
{
   NvS32 volatile pending;
   NvU32 cpuInst;
   RmCtrlParams rmCtrlDeferredParams;
   NvU8 paramBuffer[RMCTRL_DEFERRED_MAX_PARAM_SIZE];    // buffer to hold rmCtrlDeferredParams.pParams
} RmCtrlDeferredCmd;

// catch commands misdirected to non-existent engines
#define VERIFY_OBJ_PTR(p)   if (p == NULL) return NV_ERR_INVALID_ARGUMENT

// macros to get/set/clear cap bits
#define RMCTRL_GET_CAP(tbl,cap,field)             (((NvU8)tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field))
#define RMCTRL_SET_CAP(tbl,cap,field)             ((tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) |= (0?cap##field))
#define RMCTRL_CLEAR_CAP(tbl,cap,field)           ((tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) &= ~(0?cap##field))

// macros to AND/OR caps between two tables
#define RMCTRL_AND_CAP(finaltbl,tmptbl,tmp,cap,field)                                                                                                               \
    tmp = ((finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] & tmptbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field)); \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] &= ~(0?cap##field);                                                                           \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] |= tmp;

#define RMCTRL_OR_CAP(finaltbl,tmptbl,tmp,cap,field)                                                                                                                \
    tmp = ((finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] | tmptbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field)); \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] &= ~(0?cap##field);                                                                           \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] |= tmp;

// Whether the command ID is a NULL command?
//    We allow NVXXXX_CTRL_CMD_NULL (0x00000000) as well as the
//    per-class NULL cmd ( _CATEGORY==0x00 and _INDEX==0x00 )
#define RMCTRL_IS_NULL_CMD(cmd)             ((cmd == NVXXXX_CTRL_CMD_NULL) ||                            \
                                             (FLD_TEST_DRF_NUM(XXXX, _CTRL_CMD, _CATEGORY, 0x00, cmd) && \
                                              FLD_TEST_DRF_NUM(XXXX, _CTRL_CMD, _INDEX,    0x00, cmd)))

// top-level internal RM Control interface
NV_STATUS   rmControl_Deferred(RmCtrlDeferredCmd *pRmCtrlDeferredCmd);

// Helper functions for handling embedded parameter copies
NV_STATUS embeddedParamCopyIn(RMAPI_PARAM_COPY  *pParamCopy, RmCtrlParams *pRmCtrlParams);
NV_STATUS embeddedParamCopyOut(RMAPI_PARAM_COPY  *pParamCopy, RmCtrlParams *pRmCtrlParams);

#define RM_CLIENT_PTR_ACCESS_CHECK_READ     NVBIT(0)
#define RM_CLIENT_PTR_ACCESS_CHECK_WRITE    NVBIT(1)

//
// For NVOC Exported functions
//
//   RMCTRL_FLAGS(A, B, C) is expanded to
//      0 | RMCTRL_FLAGS_A | RMCTRL_FLAGS_B | RMCTRL_FLAGS_C
//
//   ACCESS_RIGHTS(A, B, C) is expanded to
//      0 | NVBIT(RS_ACCESS_A) | NVBIT(RS_ACCESS_B) | NVBIT(RS_ACCESS_C)
//
#define RMCTRL_EXPORT(cmdId, ...)      [[nvoc::export(cmdId, __VA_ARGS__)]]
#define _RMCTRL_PREP_FLAG_ARG(x)       | NV_CONCATENATE(RMCTRL_FLAGS_, x)
#define RMCTRL_FLAGS(...)              (0 NV_FOREACH_ARG_NOCOMMA(_RMCTRL_PREP_FLAG_ARG, __VA_ARGS__))
#define _RMCTRL_PREP_ACCESS_ARG(x)     | NVBIT(NV_CONCATENATE(RS_ACCESS_, x))
#define ACCESS_RIGHTS(...)             (0 NV_FOREACH_ARG_NOCOMMA(_RMCTRL_PREP_ACCESS_ARG, __VA_ARGS__))

// This define is currently unused.
// In the future it will be used by NVOC to validate control flags.
// 1. PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST should be set only if ROUTE_TO_PHYSICAL is set
// 2. PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST and ROUTE_TO_VGPU_HOST shouldn't be set at the same time
#define NVOC_EXPORTED_METHOD_FLAGS_VALID(ctrlFlags) \
    ((ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL) || !(ctrlFlags & RMCTRL_FLAGS_PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST)) && \
    (!(ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_VGPU_HOST) || !(ctrlFlags & RMCTRL_FLAGS_PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST))

#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(ctrlFlags) \
    ((ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL) && \
     (!(ctrlFlags & RMCTRL_FLAGS_PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST)))

//
// 'FLAGS' Attribute
// -----------------
//
// RMCTRL_FLAGS is used to specify per-command state.
//

#define RMCTRL_FLAGS_NONE                                     0x000000000

//
// If the KERNEL_PRIVILEGED flag is specified, the call will only be allowed
// for kernel mode callers (such as other kernel drivers) using a privileged
// kernel RM client (CliCheckIsKernelClient() returning true). Otherwise,
// NV_ERR_INSUFFICIENT_PERMISSIONS is returned.
//
#define RMCTRL_FLAGS_KERNEL_PRIVILEGED                        0x000000000

//
// The resman rmcontrol handler will not grab the "gpus lock"
// before executing the implementing function.
//
// Please be sure you know what you're doing before using this!
//
#define RMCTRL_FLAGS_NO_GPUS_LOCK                             0x000000001

//
// Indicate to resman that this rmcontrol does not access any gpu
// resources and can therefore run even when the gpu is powered down.
//
// Please be sure you know what you're doing before using this!
//
#define RMCTRL_FLAGS_NO_GPUS_ACCESS                           0x000000002

//
// If the PRIVILEGED flag is specified, the call will only be allowed for
//  a) user contexts with admin privleges (osIsAdministrator() returning true), or
//  b) kernel mode callers, such as other kernel drivers.
// Otherwise, NV_ERR_INSUFFICIENT_PERMISSIONS is returned.
//
#define RMCTRL_FLAGS_PRIVILEGED                               0x000000004

//
// If the NON_PRIVILEGED flag is specified, the call will be allowed from any
// client.
//
#define RMCTRL_FLAGS_NON_PRIVILEGED                           0x000000008

//
// The resman rmcontrol handler will grab the per-device lock instead
// of the "gpus lock" before executing the implementing function.
//
// Please be sure you know what you're doing before using this!
//
#define RMCTRL_FLAGS_GPU_LOCK_DEVICE_ONLY                     0x000000010

//
// This flag is equivalent to PRIVILEGED when the RM access rights
// implementation is disabled. Otherwise, it has no effect.
//
// The purpose of this flag is to aid in the transition to the access rights
// system, so that access rights can be used for control calls that were
// previously PRIVILEGED. Once access rights are enabled, this flag will no
// longer be necessary.
//
#define RMCTRL_FLAGS_PRIVILEGED_IF_RS_ACCESS_DISABLED         0x000000020 // for Resserv Access Rights migration

//
// This flag specifies that the control shall be directly forwarded to the
// physical object if called on the CPU-RM kernel.
//
#define RMCTRL_FLAGS_ROUTE_TO_PHYSICAL                        0x000000040

//
// If the INTERNAL flag is specified, the call will only be allowed
// to be issued from RM itself. Otherwise, NV_ERR_NOT_SUPPORTED is returned.
//
#define RMCTRL_FLAGS_INTERNAL                                 0x000000080

//
//  If the API_LOCK_READONLY flag is specified, the call will acquire the
//  read-only API lock and may run concurrently with other operations that have
//  also taken the read-only API lock. This flag is ignored if read-only API
//  locking is disabled in RM.
//
#define RMCTRL_FLAGS_API_LOCK_READONLY                        0x000000100

//
// This flag specifies that the control shall be directly forwarded to the
// the VGPU host if called from a guest (where IS_VIRTUAL() is true)
//
#define RMCTRL_FLAGS_ROUTE_TO_VGPU_HOST                       0x000000200

//
// This flag specifies that the control output does not depend on the input
// parameters and can be cached on the receiving end.
// The cache is transparent and may not exist on all platforms.
//
#define RMCTRL_FLAGS_CACHEABLE                                0x000000400

//
// This flag specifies that the control parameters will be
// copied out back to the caller even if the control call fails.
//
#define RMCTRL_FLAGS_COPYOUT_ON_ERROR                         0x000000800

// ??
#define RMCTRL_FLAGS_ALLOW_WITHOUT_SYSMEM_ACCESS              0x000001000

//
// This flag specifies that the control can be run by an admin privileged
// client running in a full SRIOV, vGPU-GSP-ENABLED hypervisor environment.
// Overrides regular privilege level flags.
//
#define RMCTRL_FLAGS_CPU_PLUGIN_FOR_VGPU_GSP                  0x000002000

//
// This flag specifies that the control can be run by an admin privileged
// client running in a full SRIOV, vGPU-GSP-DISABLED hypervisor environment.
// Overrides regular privilege level flags.
//
#define RMCTRL_FLAGS_CPU_PLUGIN_FOR_SRIOV                     0x000004000

//
// This flag specifies that the control can be run by an admin privileged
// client running in a non-SRIOV or SRIOV-Heavy hypervisor environment.
// Overrides regular privilege level flags.
//
#define RMCTRL_FLAGS_CPU_PLUGIN_FOR_LEGACY                    0x000008000

//
// This flag specifies that the control can be run by an unprivileged
// client running in GSP-RM when SRIOV and vGPU-GSP are ENABLED.
// Overrides regular privilege level flags.
//
#define RMCTRL_FLAGS_GSP_PLUGIN_FOR_VGPU_GSP                  0x000010000

//
// This flag specifies that the control output depends on the input
// parameters but can be cached on receiving end. Since the control
// result depends on the input and the input varifies with controls,
// the cache should be handled in a per-control bases.
//
#define RMCTRL_FLAGS_CACHEABLE_BY_INPUT                       0x000020000


//
// This flag specifies that ROUTE_TO_PHYSICAL control is implemented on vGPU Guest RM.
// If a ROUTE_TO_PHYSICAL control is supported within vGPU Guest RM,
// it should either have this flag set (indicating the implementation in the vGPU Guest RM) or
// the ROUTE_TO_VGPU_HOST flag set (indicating the implementation in vGPU Host RM).
// Without either of these flags set, the control will return NV_ERR_NOT_SUPPORTED.
//
#define RMCTRL_FLAGS_PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST       0x000040000

// The combination of cacheable flags
#define RMCTRL_FLAGS_CACHEABLE_ANY (RMCTRL_FLAGS_CACHEABLE | RMCTRL_FLAGS_CACHEABLE_BY_INPUT)

//
// This flag specifies that two client handles need to be locked.
// An entry is required for any control calls that set this in
// serverControlLookupSecondClient or Resource Server will NV_ASSERT(0).
//
#define RMCTRL_FLAGS_DUAL_CLIENT_LOCK                         0x000080000

//
// This flag specifies that the control call is for RM test only code.
//
#define RMCTRL_FLAGS_RM_TEST_ONLY_CODE                        0x000100000

//
// This flag specifies that all client handles in RM need to be locked.
// This flag should almost never be used, the only cases where it is required
// are cases where an RM API loops accessed several/arbitrary clients in RM using
// something like serverutilGetFirstClientUnderLock. The RW API lock is required
// to use this flag
//
#define RMCTRL_FLAGS_ALL_CLIENT_LOCK                          0x000200000

//
// This flag specifies that the API lock should not be acquired for this
// RM Control. DO NOT use this flag without consulting Locking/Resource Server
// experts first and please consider other alternatives as much as possible
// before resorting to using this flag!
//
#define RMCTRL_FLAGS_NO_API_LOCK                              0x000400000

//
//  'ACCESS_RIGHTS' Attribute
//  ------------------------
//
//    Used to specify a set of access rights that the client must hold on the
//    target resource to execute this control call. Note that this can only check
//    access rights on the target object; for other objects, such as those
//    specified by handles in parameter structs, checks must be done manually.
//
//    The definition of each access right and its meaning is provided in the
//    README located at drivers/common/shared/accessrights/README. The prefix
//    "RS_ACCESS" is appended to each entry in the control call definition;
//    for example, :NICE -> RS_ACCESS_NICE.
//
//    This attribute only has an effect when the RM access rights implementation
//    is enabled; see g_bRsAccessEnabled.
//


/*
 * On T234, RM is in kernel mode, so when RM is running in kernel mode it
 * does not allow usermode clients like MODs to call control calls that are
 * marked as KERNEL_PRIVILEGED.
 * So defining new macro DISPLAY_PRIVILEGED(i.e PRIVILEGED) for Tegra and mark
 * control calls needed by MODs with this so that MODs running as root can call
 * these control calls. However keeping same privilege level for DGPUs which
 * does not change the current behaviour.
 */
#define DISPLAY_PRIVILEGED KERNEL_PRIVILEGED

#endif // _CONTROL_H_



/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>
#include <rmconfig.h>
#include <gpu/subdevice/subdevice.h>
#include <ctrl/ctrl0080/ctrl0080unix.h>
#include <ctrl/ctrl2080/ctrl2080unix.h>

NV_STATUS NV_API_CALL rm_schedule_gpu_wakeup(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
    return NV_OK;
}

void NV_API_CALL rm_init_dynamic_power_management(
    nvidia_stack_t *sp,
    nv_state_t     *nv,
    NvBool          bPr3AcpiMethodPresent
)
{
}

void NV_API_CALL rm_cleanup_dynamic_power_management(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
}

void NV_API_CALL rm_enable_dynamic_power_management(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
}

NV_STATUS NV_API_CALL rm_ref_dynamic_power(
    nvidia_stack_t          *sp,
    nv_state_t              *nv,
    nv_dynamic_power_mode_t  mode
)
{
    return NV_OK;
}

void NV_API_CALL rm_unref_dynamic_power(
    nvidia_stack_t          *sp,
    nv_state_t              *nv,
    nv_dynamic_power_mode_t mode
)
{
}

NV_STATUS NV_API_CALL rm_transition_dynamic_power(
    nvidia_stack_t *sp,
    nv_state_t     *nv,
    NvBool          bEnter
)
{
    return NV_OK;
}

NV_STATUS NV_API_CALL rm_power_management(
     nvidia_stack_t *sp,
     nv_state_t *pNv,
     nv_pm_action_t pmAction
)
{
    return NV_OK;
}

const char* NV_API_CALL rm_get_vidmem_power_status(
    nvidia_stack_t *sp,
    nv_state_t     *pNv
)
{
    return "?";
}

const char* NV_API_CALL rm_get_dynamic_power_management_status(
    nvidia_stack_t *sp,
    nv_state_t     *pNv
)
{
    return "?";
}

const char* NV_API_CALL rm_get_gpu_gcx_support(
    nvidia_stack_t *sp,
    nv_state_t     *pNv,
    NvBool          bGcxTypeGC6
)
{
    return "?";
}

NV_STATUS
subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams
)
{
    return NV_OK;
}

void
RmUpdateGc6ConsoleRefCount
(
    nv_state_t *nv,
    NvBool bIncrease
)
{
}

void
RmInitS0ixPowerManagement
(
    nv_state_t *nv
)
{
}

void
RmInitDeferredDynamicPowerManagement
(
    nv_state_t *nv
)
{
}

void
RmDestroyDeferredDynamicPowerManagement
(
    nv_state_t *nv
)
{
}

void RmHandleDisplayChange
(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
}

NV_STATUS
os_ref_dynamic_power
(
    nv_state_t *nv,
    nv_dynamic_power_mode_t mode
)
{
    return NV_OK;
}

void
os_unref_dynamic_power
(
    nv_state_t *nv,
    nv_dynamic_power_mode_t mode
)
{
}

NV_STATUS  NV_API_CALL rm_get_clientnvpcf_power_limits(
    nvidia_stack_t *sp,
    nv_state_t     *nv,
    NvU32          *limitRated,
    NvU32          *limitCurr
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
deviceCtrlCmdOsUnixVTSwitch_IMPL
(
    Device *pDevice,
    NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS NV_API_CALL rm_save_low_res_mode(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL rm_gpu_copy_mmu_faults(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvU32 *faultsCopied
)
{
    return NV_OK;
}

NV_STATUS NV_API_CALL rm_gpu_copy_mmu_faults_unlocked(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvU32 *faultsCopied
)
{
    return NV_OK;
}

NV_STATUS RmInitX86EmuState(OBJGPU *pGpu)
{
    return NV_OK;
}

void RmFreeX86EmuState(OBJGPU *pGpu)
{
}

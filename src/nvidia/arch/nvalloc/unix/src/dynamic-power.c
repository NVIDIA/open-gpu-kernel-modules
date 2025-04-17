/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <rmconfig.h>

/*
 * NOTE:
 * The driver disables dynamic power management by effectively setting
 * nv_dynamic_power_t::mode to NV_DYNAMIC_PM_NEVER at two places:
 * 1) During module probe, when the _PRx methods are not supported
 * by the SBIOS or when the kernel does not support runtime PM.
 * 2) During RM Init, when the GPU is not Turing+ or when the system is not
 * a laptop or when the VBIOS does not support RTD3/GC6.
 *
 * Thus, after RMInit, if the nv_dynamic_power_t::mode is still
 * NV_DYNAMIC_PM_FINE, then we know for sure that the fine grained power
 * control is active and running. In this case, the runtime D3 status is
 * shown as "Enabled (fine-grained)".
 *
 * If the nv_dynamic_power_t::mode is NV_DYNAMIC_PM_NEVER, we have to
 * distinguish between four cases:
 * 1) The driver disabled dynamic power management due to lack of kernel/SBIOS
 * support.
 * 2) The driver disabled dynamic power management because the GPU does not
 * support it or the system is not a laptop.
 * 3) The user has explicitly disabled the feature.
 * 4) Configuration is not supported by default.
 *
 * In order to differentiate between these four cases,
 * we use the value of regkey NVreg_DynamicPowerManagement. If this value is
 * set to 0, then we know that the user has explicitly disabled the
 * feature. In this case we show the runtime D3 status as "Disabled". This
 * handles case (3) above.
 *
 * For case (4), we look at the combination of nv_dynamic_power_t::mode and
 * NVreg_DynamicPowerManagement value. If mode is never and regkey value is
 * default, then status is shown as "Disabled by default".
 *
 * For remaining cases, (case (1) and (2)) we show status as "Not Supported".
 *
 * When the regkey is set to 0 and the driver disables the feature due to lack
 * of some support, then precedence will be given to regkey value. Thus, when
 * both "Not supported" and "Disabled" are possible values, precedence will be
 * given to "Disabled" over "Not Supported".
 *
 * For coarse-grain power control, we show runtime D3 status as
 * "Enabled (coarse-grained)".
 *
 * The runtime D3 status is shown as "?" when RM has not been initialized.
 * This is because we do not have full knowledge regarding driver triggered
 * disablement until RM is initialized.
 */

#include <nvlog_inc.h>
#include <nv.h>
#include <nv-priv.h>
#include <nv-reg.h>
#include <nv_ref.h>

#include <osapi.h>

#include <gpu/mem_mgr/mem_mgr.h>
#include <gpu/kern_gpu_power.h>
#include <gpu_mgr/gpu_mgr.h>
#include <core/locks.h>
#include <pex.h>
#include <acpidsmguids.h>
#include "kernel/gpu/intr/intr.h"

#include <gpu/mem_sys/kern_mem_sys.h>
#include <gpu/subdevice/subdevice.h>
#include <ctrl/ctrl0080/ctrl0080gpu.h>
#include <ctrl/ctrl2080/ctrl2080unix.h>
#include <gpu/timer/objtmr.h>


//
// Schedule timer based callback, to check for the complete GPU Idleness.
// Windows has idle time from 70msec to 10sec, we opted for present duration
// considering windows limit. Duration is not much aggressive or slow, hence
// less thrashing.
//
#define GC6_PRECONDITION_CHECK_TIME    ((NvU64)5 * 1000 * 1000 * 1000)

//
// Timeout needed for back to back GC6 cycles.
// Timeout is kept same as the timeout selected for GC6 precondition check.
// There are cases where GPU is in GC6 and then kernel wakes GPU out of GC6
// as part of say accessing pci tree through lspci and then again ask driver
// to put GPU in GC6 state after access to device info is done.
// Below are the reasons for having timeout limit for back to back GC6 cycle.
// 1> Less chip life cycle.
// 2> P-state is not reached to GC6 supported P-state, in some cycles.
// P-state case returns error to kernel, resulting in corrupted sysfs entry
// and then kernel never calls driver to put device in low power state.
//
#define GC6_CYCLE_IDLE_HOLDOFF_CHECK_TIME    GC6_PRECONDITION_CHECK_TIME

//
// Once GPU is found to be idle, driver will schedule another callback of
// smaller duration. Driver needs to be sure that methods that are present
// in host pipeline are flushed to the respective engines and engines become
// idle upon consumption.
//
#define GC6_BAR1_BLOCKER_CHECK_AND_METHOD_FLUSH_TIME (200 * 1000 * 1000)

//
// Cap Maximum FB allocation size for GCOFF. If regkey value is greater
// than this value then it will be capped to this value.
//
#define GCOFF_DYNAMIC_PM_MAX_FB_SIZE_MB      1024

//
// Safe/Fallback value of PEX reset delay for GCOFF/GPUOFF.
// Used when programming upstream port as per sku specific PEX reset delay from vbios fails
//
#define DEFAULT_GCOFF_PEXRST_DELAY 0

static void RmScheduleCallbackForIdlePreConditions(OBJGPU *);
static void RmScheduleCallbackForIdlePreConditionsUnderGpuLock(OBJGPU *);
static void RmScheduleCallbackToIndicateIdle(OBJGPU *);
static NvBool RmCheckForGcxSupportOnCurrentState(OBJGPU *);
static void RmScheduleCallbackToRemoveIdleHoldoff(OBJGPU *);
static void RmQueueIdleSustainedWorkitem(OBJGPU *);

/*!
 * @brief Wrapper that checks lock order for the dynamic power mutex.  Locking
 * order dictates that the GPU lock must not be taken before taking the dynamic
 * power mutex.
 *
 * The GPU lock must not be held when calling this function.
 *
 * @param[in]   nvp     nv_priv_t pointer.
 */
static void acquireDynamicPowerMutex(nv_priv_t *nvp)
{
    /*
     * Note that this may be called before nvp->pGpu has been initialized, for
     * example at the very beginning of nv_start_device.
     */
    NV_ASSERT_CHECKED(!nvp->pGpu || !rmDeviceGpuLockIsOwner(nvp->pGpu->gpuInstance));

    portSyncMutexAcquire(nvp->dynamic_power.mutex);
}

/*!
 * @brief Wrapper to release the mutex, just for consistency with
 * acquireDynamicPowerMutex() above.
 *
 * @param[in]   nvp     nv_priv_t pointer.
 */
static void releaseDynamicPowerMutex(nv_priv_t *nvp)
{
    portSyncMutexRelease(nvp->dynamic_power.mutex);
}

/*!
 * @brief: Helper function to get a string for the given state.
 *
 * @param[in]   state   Dynamic power state.
 *
 * @return      String for the given state.
 */
const char *nv_dynamic_power_state_string(
    nv_dynamic_power_state_t state
)
{
    switch (state)
    {
#define DPCASE(_case) \
    case NV_DYNAMIC_POWER_STATE_ ## _case: return # _case;

    DPCASE(IN_USE);
    DPCASE(IDLE_INSTANT);
    DPCASE(IDLE_SUSTAINED);
    DPCASE(IDLE_INDICATED);
    DPCASE(UNKNOWN);
#undef DPCASE
    }
    NV_ASSERT(0);
    return "UNEXPECTED";
}

/*!
 * @brief: Helper function to transition the GPU represented by the 'nv'
 * pointer from the power state 'old_state' to the power state 'new_state'.
 *
 * An atomic compare-and-swap is used; the function returns true if the
 * operation was successful.  If the function returns false, the state is
 * unchanged.
 *
 * @param[in]   nv          nv_state_t pointer.
 * @param[in]   old_state   Previous (current) state.
 * @param[in]   new_state   New (requested) state.
 *
 * @return      TRUE if the operation was successful; FALSE otherwise.
 */
NvBool nv_dynamic_power_state_transition(
    nv_state_t *nv,
    nv_dynamic_power_state_t old_state,
    nv_dynamic_power_state_t new_state
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);

    NV_ASSERT(old_state != new_state);

    ct_assert(sizeof(nv_dynamic_power_state_t) == sizeof(NvS32));
    NvBool ret = portAtomicCompareAndSwapS32((NvS32*)&nvp->dynamic_power.state,
                                             new_state, old_state);

    if (ret)
    {
        NV_PRINTF(LEVEL_INFO, "%s: state transition %s -> %s\n",
                  __FUNCTION__,
                  nv_dynamic_power_state_string(old_state),
                  nv_dynamic_power_state_string(new_state));
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "%s: FAILED state transition %s -> %s\n",
                  __FUNCTION__,
                  nv_dynamic_power_state_string(old_state),
                  nv_dynamic_power_state_string(new_state));
    }

    return ret;

}

/*!
 * @brief: Check if the GPU hardware appears to be idle.
 *
 * Assumes the GPU lock is held.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 *
 * @return      TRUE if the GPU appears to be currently idle; FALSE otherwise.
 */
static NvBool RmCanEnterGcxUnderGpuLock(
    OBJGPU *pGpu
)
{
    nv_state_t    *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t     *nvp = NV_GET_NV_PRIV(nv);

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    /*
     * If GC6 cannot be achieved (either GC6 is unsupported or the upstream port is not configured),
     * Check for GCOFF prerequisites
     */
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED) ||
        !nvp->gc6_upstream_port_configured)
    {
        NvU64          usedFbSize     = 0;
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        /*
         * In order to enter GCOFF, the FB memory threshold should be less than gcoff_max_fb_size
         * and clients of RM should not hold the refcount to prevent GCOFF.
         */
        if (!((memmgrGetUsedRamSize(pGpu, pMemoryManager, &usedFbSize) == NV_OK) &&
              (usedFbSize <= nvp->dynamic_power.gcoff_max_fb_size) &&
              (nvp->dynamic_power.clients_gcoff_disallow_refcount == 0)))
        {
            return NV_FALSE;
        }
    }

    // Check the instantaneous engine level idleness.
    return RmCheckForGcxSupportOnCurrentState(pGpu);
}

/*!
 * @brief: Check the feasibility of GPU engaging in a GCx cycle.
 *
 * Takes the GPU lock.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 *
 * @return      TRUE if the GPU appears to be currently idle; FALSE otherwise.
 */
static NvBool RmCanEnterGcx(
    OBJGPU *pGpu
)
{
    NvBool      result = NV_FALSE;
    NV_STATUS   status;
    GPU_MASK    gpuMask;

    // LOCK: acquire per device lock
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                   GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER,
                                   &gpuMask);
    if (status == NV_OK)
    {
        result = RmCanEnterGcxUnderGpuLock(pGpu);
        // UNLOCK: release per device lock
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
    }

    return result;
}

/*!
 * @brief: Force the GPU to be "not idle", if it had previously been indicated
 * idle.  This is triggered by GPU usage (i.e., writing through a revoked
 * mapping), not tracked through the refcount maintained through
 * os_{ref,unref}_dynamic_power.  So, if we'd previously indicated the GPU was
 * idle, we transition to the IDLE_INSTANT state.
 *
 * @param[in]   gpuInstance     GPU instance ID.
 * @param[in]   pArgs           Unused callback closure.
 */
static void RmForceGpuNotIdle(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    nv_dynamic_power_state_t old_state;
    NvBool ret;

    acquireDynamicPowerMutex(nvp);

    old_state = nvp->dynamic_power.state;

    switch (old_state)
    {
    case NV_DYNAMIC_POWER_STATE_IDLE_INDICATED:
        nv_indicate_not_idle(nv);
        NV_ASSERT(nvp->dynamic_power.deferred_idle_enabled);
        RmScheduleCallbackForIdlePreConditions(pGpu);
        /* fallthrough */
    case NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED:
        ret = nv_dynamic_power_state_transition(nv, old_state, NV_DYNAMIC_POWER_STATE_IDLE_INSTANT);
        NV_ASSERT(ret);
        break;
    case NV_DYNAMIC_POWER_STATE_IDLE_INSTANT:
    case NV_DYNAMIC_POWER_STATE_IN_USE:
        break;
    case NV_DYNAMIC_POWER_STATE_UNKNOWN:
        NV_ASSERT(0);
        break;
    }

    /*
     * Now that the GPU is guaranteed to not be powered off, we can resume
     * servicing mmap requests as usual.
     */
    nv_acquire_mmap_lock(nv);
    nv_set_safe_to_mmap_locked(nv, NV_TRUE);
    nv_release_mmap_lock(nv);

    releaseDynamicPowerMutex(nvp);
}

/*!
 * @brief: Work item to actually indicate the GPU is idle.  This rechecks the
 * preconditions one last time and verifies nothing has attempted to write
 * through any mappings which were revoked.
 *
 * @param[in]   gpuInstance     GPU instance ID.
 * @param[in]   pArgs           Unused callback closure.
 */
static void RmIndicateIdle(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);

    acquireDynamicPowerMutex(nvp);

    nv_acquire_mmap_lock(nv);

    if (nv_get_all_mappings_revoked_locked(nv) &&
        nvp->dynamic_power.state == NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED &&
        RmCanEnterGcx(pGpu))
    {
        nv_set_safe_to_mmap_locked(nv, NV_FALSE);
        nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED,
                                              NV_DYNAMIC_POWER_STATE_IDLE_INDICATED);
        nv_indicate_idle(nv);
    }
    else
    {
        RmScheduleCallbackForIdlePreConditions(pGpu);
    }

    nv_release_mmap_lock(nv);

    releaseDynamicPowerMutex(nvp);
}

/*!
 * @brief: Helper function to trigger the GPU to be forced indicated
 * "non-idle", which powers it on if it had been previously powered off, and
 * allows mmap processing to continue.
 *
 * Triggered when a revoked mapping is accessed.
 *
 * @param[in]   sp  nvidia_stack_t pointer.
 * @param[in]   nv  nv_state_t pointer.
 */
NV_STATUS NV_API_CALL rm_schedule_gpu_wakeup(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    void *fp;
    NV_STATUS ret;
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

    NV_ENTER_RM_RUNTIME(sp, fp);

    ret = osQueueWorkItem(pGpu, RmForceGpuNotIdle, NULL);

    NV_EXIT_RM_RUNTIME(sp, fp);

    return ret;
}

/*!
 * @brief Function to increment/decrement refcount for GPU driving console.
 *
 * @params[in]  nv         nv_state_t pointer.
 * @params[in]  bIncrease  If true, increase refcount else decrease.
 */
void
RmUpdateGc6ConsoleRefCount
(
    nv_state_t *nv,
    NvBool      bIncrease
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);

    if (nv->console_device == NV_FALSE)
    {
        return;
    }

    acquireDynamicPowerMutex(nvp);

    if (bIncrease)
    {
        /*
         * Here refcount value should atleast be 2 due to below:
         * 1> Refcount increased by NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT.
         * 2> Refcount increased by the Rm entry points for current thread.
         */
        if (nvp->dynamic_power.refcount <= 1)
        {
            NV_PRINTF(LEVEL_INFO,
                  "%s: Unexpected dynamic power refcount value\n", __FUNCTION__);
        }
        else
        {
            nvp->dynamic_power.refcount++;
        }
    }
    else
    {
        /*
         * Here refcount value should atleast be 3 due to below:
         * 1> Refcount set by RM at the probe time for console device.
         * 2> Refcount increased by NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT.
         * 3> Refcount increased by the Rm entry points for current thread.
         */
        if (nvp->dynamic_power.refcount <= 2)
        {
            NV_PRINTF(LEVEL_INFO,
                  "%s: Unexpected dynamic power refcount value\n", __FUNCTION__);
        }
        else
        {
            nvp->dynamic_power.refcount--;
        }
    }

    releaseDynamicPowerMutex(nvp);
}

/*!
 * @brief Implements the NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT RmControl
 * request.
 *
 * @param[in]     pSubdevice
 * @param[in,out] pParams
 *
 * @return
 *      NV_OK                       Success
 *      NV_ERR_INVALID_ARGUMENT     Invalid pParams->action
 */
NV_STATUS
subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    /* TODO: keep track of inc/dec on a per-client basis, clean up after
     * clients which are unbalanced. */
    if (pParams->action == NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_INC)
    {
        return osRefGpuAccessNeeded(pGpu->pOsGpuInfo);
    }
    else if (pParams->action == NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_DEC)
    {
        osUnrefGpuAccessNeeded(pGpu->pOsGpuInfo);
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief Implements the NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER RmControl
 *        request. It will perform dynamic power management for GPU
 *        HDA contoller.
 *
 * @param[in]     pSubdevice
 * @param[in,out] pParams
 *
 * @return
 *      NV_OK   Success
 */
NV_STATUS
subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    /*
     * Ideally, we need to honor pParams->bEnter. If it is true, then send the
     * request for suspending the HDA controller and if is false, then send the
     * request for waking up the HDA controller. Currently, for unix based OS,
     * we don't have any proper way to do this and it is implemented with
     * workaround. This workaround will wake up the HDA controller for few
     * seconds and then bring it back to suspended state. We are using this
     * workaround for both the cases (bEnter is true or false). It will help
     * in keeping HDA controller active for the duration, when GPU is
     * processing HDA controller commands (like writing ELD data).
     */
    nv_audio_dynamic_power(NV_GET_NV_STATE(pGpu));

    return NV_OK;
}

/*!
 * @brief: Function to indicate if GC6/GC-OFF is supported
 * or not on the SKU.
 *
 * @param[in]   pGpu           OBJGPU pointer.
 * @param[in]   bGcxTypeGc6    If true, returns string indicating GC6 support
 *                             otherwise returns GC-OFF support.
 *
 * @return      String indicating GC6/GC-OFF support status.
 */
static const char* RmGetGpuGcxSupport(
    OBJGPU *pGpu,
    NvBool  bGcxTypeGC6
)
{
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED))
    {
        return "Not Supported";
    }

    if (bGcxTypeGC6)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED))
        {
            return "Supported";
        }
    }
    else
    {
        if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED)) ||
            (pGpu->getProperty(pGpu, PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED)))
        {
            return "Supported";
        }
    }

    return "Not Supported";
}

/*!
 * @brief Function to increment/decrement global Gcoff disallow refcount.
 *
 * @params[in]  pOsGpuInfo        OS specific GPU information pointer.
 * @params[in]  bIsGcoffDisallow  If true, increase refcount else decrease.
 */
void osClientGcoffDisallowRefcount(
    OS_GPU_INFO *pOsGpuInfo,
    NvBool       bIsGcoffDisallow
)
{
    nv_priv_t    *nvp = NV_GET_NV_PRIV(pOsGpuInfo);

    if (nvp->dynamic_power.mode == NV_DYNAMIC_PM_NEVER)
    {
        return;
    }

    acquireDynamicPowerMutex(nvp);

    if (bIsGcoffDisallow)
    {
        nvp->dynamic_power.clients_gcoff_disallow_refcount++;
    }
    else
    {
        nvp->dynamic_power.clients_gcoff_disallow_refcount--;
    }

    releaseDynamicPowerMutex(nvp);
}

/*!
 * @brief Implements the NV2080_CTRL_CMD_OS_UNIX_ALLOW_DISALLOW_GCOFF RmControl
 * request.
 *
 * @param[in]     pSubdevice
 * @param[in,out] pParams
 *
 * @return
 *      NV_OK                           Success
 *      NV_ERR_INVALID_ARGUMENT         Invalid pParams->action
 *      NV_ERR_INVALID_OBJECT_HANDLE    Invalid Object handle
 *      NV_ERR_OBJECT_NOT_FOUND         Object not found
 */
NV_STATUS
subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);

    if (pParams->action == NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_ALLOW)
    {
        if (pSubdevice->bGcoffDisallowed)
        {
            pSubdevice->bGcoffDisallowed = NV_FALSE;
            osClientGcoffDisallowRefcount(pGpu->pOsGpuInfo, NV_FALSE);
            status = NV_OK;
       }
       else
       {
            NV_ASSERT(0);
            status = NV_ERR_INVALID_ARGUMENT;
       }
    }
    else if (pParams->action == NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_DISALLOW)
    {
        if (!pSubdevice->bGcoffDisallowed)
        {
            pSubdevice->bGcoffDisallowed = NV_TRUE;
            osClientGcoffDisallowRefcount(pGpu->pOsGpuInfo, NV_TRUE);
            status = NV_OK;
        }
        else
        {
            NV_ASSERT(0);
            status = NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
}

/*!
 * @brief Read and Parse the Dynamic Power Management regkey.
 * This function modifies the default value of regkey to either enable
 * or disable RTD3 as per System form factor and GPU architecture.
 *
 * @param[in]    pNv             nv_state_t pointer.
 * @param[out]   pRegkeyValue    Original value of regkey.
 * @param[out]   pOption         Modified value of regkey.
 *
 * @return       NV_STATUS code.
 */

#define NV_PMC_BOOT_42_CHIP_ID_GA102                     0x00000172

static NV_STATUS
rmReadAndParseDynamicPowerRegkey
(
    nv_state_t *pNv,
    NvU32      *pRegkeyValue,
    NvU32      *pOption
)
{
    NV_STATUS  status;
    NvU32      chipId;
    nv_priv_t *pNvp = NV_GET_NV_PRIV(pNv);

    status = osReadRegistryDword(NULL, NV_REG_DYNAMIC_POWER_MANAGEMENT, pRegkeyValue);

    if (status != NV_OK)
        return status;

    // If User has set some value, honor that value
    if (*pRegkeyValue != NV_REG_DYNAMIC_POWER_MANAGEMENT_DEFAULT)
    {
        *pOption = *pRegkeyValue;
        return NV_OK;
    }

    chipId = decodePmcBoot42ChipId(pNvp->pmc_boot_42);

    // From GA102+, we enable RTD3 only if system is found to be Notebook
    if ((chipId >= NV_PMC_BOOT_42_CHIP_ID_GA102) &&
        (rm_is_system_notebook()))
    {
        *pOption = NV_REG_DYNAMIC_POWER_MANAGEMENT_FINE;
        return NV_OK;
    }

    *pOption = NV_REG_DYNAMIC_POWER_MANAGEMENT_NEVER;
    return NV_OK;
}
#undef NV_PMC_BOOT_42_CHIP_ID_GA102

/*!
 * @brief Initialize state related to dynamic power management.
 * Called once per GPU during driver initialization.
 *
 * @param[in]   sp  nvidia_stack_t pointer.
 * @param[in]   nv  nv_state_t pointer.
 */
void NV_API_CALL rm_init_dynamic_power_management(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvBool bPr3AcpiMethodPresent
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NvU32 option   = NV_REG_DYNAMIC_POWER_MANAGEMENT_NEVER;
    NvU32 gcOffMaxFbSizeMb;
    void *fp;
    NvBool bUefiConsole;
    NvU32 status;
    NvU32 regkeyValue;

    NV_ENTER_RM_RUNTIME(sp,fp);

    portMemSet(&nvp->dynamic_power, 0, sizeof(nvp->dynamic_power));

    /*
     * Program an impossible value so that we show correct status
     * during procfs read of runtime D3 status.
     */
    nvp->dynamic_power.dynamic_power_regkey = 0xFFFFFFFFu;

    /*
     * Dynamic power management will be disabled in either of
     * following 3 cases:
     * 1. The driver does not support dynamic PM for this
     *    distro/kernel version.
     * 2. _PR3 ACPI method is not supported by upstream port of GPU.
     * 3. This feature is not enabled by regkey.
     * Also see NOTE
     */
    status = rmReadAndParseDynamicPowerRegkey(nv, &regkeyValue, &option);

    if (status == NV_OK)
    {
        nvp->dynamic_power.dynamic_power_regkey = regkeyValue;
    }

    nvp->pr3_acpi_method_present = bPr3AcpiMethodPresent;
    if (!nv_dynamic_power_available(nv) || !bPr3AcpiMethodPresent ||
        (status != NV_OK))
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "%s: Disabling dynamic power management either due to lack"
                  " of system support or due to error (%d) in reading regkey."
                  "\n", __FUNCTION__, status);
        option = NV_REG_DYNAMIC_POWER_MANAGEMENT_NEVER;
    }

    /*
     * Read the OS registry for Maximum FB size during GCOFF based dynamic PM.
     * This will be capped to GCOFF_DYNAMIC_PM_MAX_FB_SIZE_MB.
     */
    if (osReadRegistryDword(NULL,
                            NV_REG_DYNAMIC_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD,
                            &gcOffMaxFbSizeMb) == NV_OK)
    {
        if (gcOffMaxFbSizeMb > GCOFF_DYNAMIC_PM_MAX_FB_SIZE_MB)
            gcOffMaxFbSizeMb = GCOFF_DYNAMIC_PM_MAX_FB_SIZE_MB;
    }
    else
    {
        gcOffMaxFbSizeMb = GCOFF_DYNAMIC_PM_MAX_FB_SIZE_MB;
    }

    nvp->dynamic_power.gcoff_max_fb_size =
            (NvU64)gcOffMaxFbSizeMb * 1024 * 1024;

    nvp->dynamic_power.mutex = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (nvp->dynamic_power.mutex == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: failed to create power mutex\n", __FUNCTION__);
        nvp->dynamic_power.mode = NV_DYNAMIC_PM_NEVER;
        goto done;
    }

    switch (option)
    {
    case NV_REG_DYNAMIC_POWER_MANAGEMENT_FINE:
        nvp->dynamic_power.mode = NV_DYNAMIC_PM_FINE;
        break;
    case NV_REG_DYNAMIC_POWER_MANAGEMENT_COARSE:
        nvp->dynamic_power.mode = NV_DYNAMIC_PM_COARSE;
        break;
    default:
        nv_printf(LEVEL_ERROR,
                  "NVRM: Unknown DynamicPowerManagement value '%u' specified; "
                  "disabling dynamic power management.\n", option);
        // fallthrough
    case NV_REG_DYNAMIC_POWER_MANAGEMENT_NEVER:
        nvp->dynamic_power.mode = NV_DYNAMIC_PM_NEVER;
        break;
    }

    // Enable RTD3 infrastructure from OS side.
    if ((nvp->dynamic_power.mode == NV_DYNAMIC_PM_FINE) &&
        (nvp->dynamic_power.dynamic_power_regkey == NV_REG_DYNAMIC_POWER_MANAGEMENT_DEFAULT))
    {
        nv_allow_runtime_suspend(nv);
    }

    // Legacy case: check if device is primary and driven by VBIOS or fb driver.
    nv->primary_vga = NV_FALSE;

    //
    // Below function always return NV_OK and depends upon kernel flags
    // IORESOURCE_ROM_SHADOW & PCI_ROM_RESOURCE for Primary VGA detection.
    //
    nv_set_primary_vga_status(nv);

    // UEFI case: where console is driven by GOP driver.
    bUefiConsole = rm_get_uefi_console_status(nv);

    nv->console_device = bUefiConsole || nv->primary_vga;

    //
    // Initialize refcount as 1 and state as IN_USE.
    // rm_enable_dynamic_power_management() will decrease the refcount
    // and change state to IDLE_INDICATED, if all prerequisites are met.
    //
    nvp->dynamic_power.refcount = 1;
    nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_UNKNOWN,
                                      NV_DYNAMIC_POWER_STATE_IN_USE);
done:
    NV_EXIT_RM_RUNTIME(sp,fp);
}

/*!
 * @brief Clean up state related to dynamic power management.
 * Called during driver removal.
 *
 * @param[in]   sp  nvidia_stack_t pointer.
 * @param[in]   nv  nv_state_t pointer.
 */
void NV_API_CALL rm_cleanup_dynamic_power_management(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    void *fp;
    NvBool ret;

    NV_ENTER_RM_RUNTIME(sp,fp);

    // Disable RTD3 infrastructure from OS side.
    if ((nvp->dynamic_power.mode == NV_DYNAMIC_PM_FINE) &&
        (nvp->dynamic_power.dynamic_power_regkey == NV_REG_DYNAMIC_POWER_MANAGEMENT_DEFAULT))
    {
        nv_disallow_runtime_suspend(nv);
    }

    nv_dynamic_power_state_t old_state = nvp->dynamic_power.state;

    switch (old_state)
    {
    case NV_DYNAMIC_POWER_STATE_IDLE_INDICATED:
        nv_indicate_not_idle(nv);
        /* fallthrough */
    case NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED:
    case NV_DYNAMIC_POWER_STATE_IDLE_INSTANT:
        ret = nv_dynamic_power_state_transition(nv, old_state,
                                                NV_DYNAMIC_POWER_STATE_IN_USE);
        /*
         * Nothing else should be running asynchronous to teardown that could
         * change the state so this should always succeed.
         */
        NV_ASSERT(ret);
        break;
    case NV_DYNAMIC_POWER_STATE_IN_USE:
        break;
    case NV_DYNAMIC_POWER_STATE_UNKNOWN:
        NV_ASSERT(0);
        break;
    }

    if (nvp->dynamic_power.mutex)
    {
        portSyncMutexDestroy(nvp->dynamic_power.mutex);
        nvp->dynamic_power.mutex = NULL;
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
}

/*!
 * @brief Initialize the dynamic power management refcount and enable GPUOFF,
 * if all prerequisites are met.
 * Called once per GPU during driver initialization.
 *
 * @param[in]   sp  nvidia_stack_t pointer.
 * @param[in]   nv  nv_state_t pointer.
 */
void NV_API_CALL rm_enable_dynamic_power_management(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    /*
     * If dynamic power management is enabled and device is not driving console,
     * then dynamic power management can be engaged (since we're not
     * using the GPU yet, so it's okay to power it off).
     */
    if ((nvp->dynamic_power.mode != NV_DYNAMIC_PM_NEVER) && !nv->console_device)
    {
        os_unref_dynamic_power(nv, NV_DYNAMIC_PM_COARSE);
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
}

/*
 * @brief Increment the dynamic power refcount to prevent the GPU from being
 * powered off until a corresponding os_unref_dynamic_power() call is made.
 * These two calls must always be balanced in pairs.
 *
 * This call may block until the GPU is powered on.
 *
 * The caller must not be holding the GPU lock, but may be holding the API
 * lock.
 *
 * @param[in]   sp      nvidia_stack_t pointer.
 * @param[in]   mode    Dynamic power mode this refcount bump is used for.
 */
NV_STATUS
os_ref_dynamic_power(
    nv_state_t *nv,
    nv_dynamic_power_mode_t mode
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NV_STATUS status = NV_OK;
    NvS32 ref;

    if (nvp == NULL)
    {
        NV_ASSERT(nv->removed);
        return NV_OK;
    }

    if (mode > nvp->dynamic_power.mode)
    {
        return NV_OK;
    }

    acquireDynamicPowerMutex(nvp);

    ref = nvp->dynamic_power.refcount++;

    NV_ASSERT(ref >= 0);

    if (ref > 0)
    {
        NV_ASSERT(nvp->dynamic_power.state == NV_DYNAMIC_POWER_STATE_IN_USE);
    }
    else
    {
        /*
         * Just transitioned from 0 -> 1.
         * The current dynamic power state could be any of:
         *
         * - IDLE_INDICATED: call nv_indicate_not_idle() to ensure
         *   the GPU is at full power and ready to be used.
         *
         * - IDLE_INSTANT/IDLE_SUSTAINED: transition to IN_USE.
         *   Note that in these states, callbacks may be operating
         *   asynchronously, so care has to be taken to retry the write if it
         *   fails.
         */

        nv_dynamic_power_state_t old_state = nvp->dynamic_power.state;
        NvBool ret;

        switch (old_state)
        {
        default:
        case NV_DYNAMIC_POWER_STATE_IN_USE:
            NV_PRINTF(LEVEL_ERROR, "Unexpected dynamic power state 0x%x\n",
                      old_state);
            /* fallthrough */
        case NV_DYNAMIC_POWER_STATE_IDLE_INDICATED:
            status = nv_indicate_not_idle(nv);
            if (status != NV_OK)
            {
                nvp->dynamic_power.refcount--;
                break;
            }
            if (nvp->dynamic_power.deferred_idle_enabled)
            {
                RmScheduleCallbackForIdlePreConditions(NV_GET_NV_PRIV_PGPU(nv));
            }
            /* fallthrough */
        case NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED:
        case NV_DYNAMIC_POWER_STATE_IDLE_INSTANT:
            do {
                ret = nv_dynamic_power_state_transition(nv, old_state, NV_DYNAMIC_POWER_STATE_IN_USE);
                if (!ret)
                {
                    old_state = nvp->dynamic_power.state;
                    NV_ASSERT(old_state == NV_DYNAMIC_POWER_STATE_IDLE_INSTANT ||
                              old_state == NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED);
                }
            } while (!ret);
            break;
        }
    }

    releaseDynamicPowerMutex(nvp);

    return status;
}

/*
 * @brief This function queries the dynamic boost support.
 * This function returns -1 in case it failed to query the
 * dynamic boost support. It returns 1 if dynamic boost is
 * supported and returns 0 if dynamic boost is not supported.
 *
 * The caller must hold the API lock before calling this
 * function.
 *
 * @param[in]   nv      nv_state_t pointer.
 */
static int os_get_dynamic_boost_support(
    nv_state_t *nv    
)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pNvpcfParams;
    OBJGPU *pGpu;
    int ret;

    status = os_alloc_mem((void**)&pNvpcfParams, sizeof(*pNvpcfParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory\n");
        ret = -1;
        goto end;
    }

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    pNvpcfParams->gpuId = pGpu->gpuId;
    pNvpcfParams->subFunc = 
        NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_SUPPORTED_CASE;
    status = pRmApi->Control(pRmApi,
                           nv->rmapi.hClient,
                           nv->rmapi.hClient,
                           NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO,
                           (void*)pNvpcfParams,
                           sizeof(*pNvpcfParams));
    if (status == NV_OK)
    {
        ret = 1;
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    os_free_mem(pNvpcfParams);
end:
    return ret;
}

/*!
 * @brief Wrapper around os_ref_dynamic_power() suitable for use from the
 * per-OS layers.
 *
 * @param[in]   sp      nvidia_stack_t pointer.
 * @param[in]   nv      nv_state_t pointer.
 * @param[in]   mode    Dynamic power mode this refcount bump is used for.
 */
NV_STATUS NV_API_CALL rm_ref_dynamic_power(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    nv_dynamic_power_mode_t mode
)
{
    NV_STATUS status;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    status = os_ref_dynamic_power(nv, mode);

    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

/*!
 * @brief Decrement the dynamic power refcount to release an earlier
 * requirement that the GPU be powered on made by calling
 * os_ref_dynamic_power().
 * These two calls must always be balanced in pairs.
 *
 * This call may block until the GPU is powered on.
 *
 * The caller must not be holding the GPU lock, but may be holding the API
 * lock.
 *
 * @param[in]   sp      nvidia_stack_t pointer.
 * @param[in]   mode    Dynamic power mode this refcount bump is used for.
 */
void
os_unref_dynamic_power(
    nv_state_t *nv,
    nv_dynamic_power_mode_t mode
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NvS32 ref;

    if (nvp == NULL)
    {
        NV_ASSERT(nv->removed);
        return;
    }

    if (mode > nvp->dynamic_power.mode)
    {
        return;
    }

    acquireDynamicPowerMutex(nvp);

    ref = --nvp->dynamic_power.refcount;

    NV_ASSERT(ref >= 0);

    if (ref == 0) {
        NvBool ret;

        /*
         * Just transitioned from 1 -> 0.  We should have been in the IN_USE
         * state previously; now we transition to an idle state: "instant" idle
         * if deferred idle is enabled; otherwise indicate idle immediately.
         *
         * Nothing should be asynchronously changing the state from IN_USE,
         * so the state transitions are not expected to fail.
         */

        NV_ASSERT(nvp->dynamic_power.state == NV_DYNAMIC_POWER_STATE_IN_USE);

        if (nvp->dynamic_power.deferred_idle_enabled)
        {
            ret = nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_IN_USE,
                                                        NV_DYNAMIC_POWER_STATE_IDLE_INSTANT);
            NV_ASSERT(ret);
        }
        else
        {
            nv_indicate_idle(nv);
            ret = nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_IN_USE,
                                                        NV_DYNAMIC_POWER_STATE_IDLE_INDICATED);
            NV_ASSERT(ret);
        }
    }

    releaseDynamicPowerMutex(nvp);
}

/*!
 * @brief Wrapper around os_unref_dynamic_power() suitable for use from the
 * per-OS layers.
 *
 * @param[in]   sp      nvidia_stack_t pointer.
 * @param[in]   nv      nv_state_t pointer.
 * @param[in]   mode    Dynamic power mode this refcount bump is used for.
 */
void NV_API_CALL rm_unref_dynamic_power(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    nv_dynamic_power_mode_t mode
)
{
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    os_unref_dynamic_power(nv, mode);

    NV_EXIT_RM_RUNTIME(sp,fp);
}

/*!
 * @brief Wrapper around os_ref_dynamic_power() suitable for use from core RM.
 *
 * @params[in]  pOsGpuInfo        OS specific GPU information pointer
 */
NV_STATUS osRefGpuAccessNeeded(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return os_ref_dynamic_power(pOsGpuInfo, NV_DYNAMIC_PM_FINE);
}

/*!
 * @brief Wrapper around os_unref_dynamic_power() suitable for use from core RM.
 *
 * @params[in]  pOsGpuInfo        OS specific GPU information pointer
 */
void osUnrefGpuAccessNeeded(
    OS_GPU_INFO *pOsGpuInfo
)
{
    os_unref_dynamic_power(pOsGpuInfo, NV_DYNAMIC_PM_FINE);
}

/*!
 * @brief Check if GCx is supported on current pstate and if engines are idle.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static NvBool RmCheckForGcxSupportOnCurrentState(
    OBJGPU *pGpu
)
{
    NV_STATUS   status = NV_OK;
    nv_state_t *nv     = NV_GET_NV_STATE(pGpu);
    RM_API     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS entryPrerequisiteParams;

    portMemSet(&entryPrerequisiteParams, 0, sizeof(entryPrerequisiteParams));

    status = pRmApi->Control(pRmApi,
                             nv->rmapi.hClient,
                             nv->rmapi.hSubDevice,
                             NV2080_CTRL_CMD_INTERNAL_GCX_ENTRY_PREREQUISITE,
                             (void*)&entryPrerequisiteParams,
                             sizeof(entryPrerequisiteParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM, Failed to get GCx pre-requisite, status=0x%x\n",
                  status);
        return NV_FALSE;
    }

    return entryPrerequisiteParams.bIsGC6Satisfied || entryPrerequisiteParams.bIsGCOFFSatisfied;
}

/*!
 * @brief: Work item to actually remove the idle holdoff which was applied
 * on GC6 exit.
 * This workitem is scheduled after the timeout limit for idle holdoff is
 * finished.
 *
 * Queue with lock flags:
 *     OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE
 *
 * @param[in]   gpuInstance     GPU instance ID.
 * @param[in]   pArgs           Unused callback closure.
 */
static void RmRemoveIdleHoldoff(
   NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU     *pGpu = gpumgrGetGpu(gpuInstance);
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);

    if (nvp->dynamic_power.b_idle_holdoff == NV_TRUE)
    {
        if ((RmCheckForGcxSupportOnCurrentState(pGpu) == NV_TRUE) ||
            (nvp->dynamic_power.idle_precondition_check_callback_scheduled))
        {
            nv_indicate_idle(nv);
            nvp->dynamic_power.b_idle_holdoff = NV_FALSE;
        }
        else
        {
            RmScheduleCallbackToRemoveIdleHoldoff(pGpu);
        }
    }
}

/*!
 * @brief Timer callback to schedule a work item to remove idle hold off
 * that was applied after GC6 exit.
 *
 * @param[in]   pCallbackData   OBJGPU pointer (with void pointer type).
 */
static void timerCallbackToRemoveIdleHoldoff(
    void *pCallbackData
)
{
    NV_STATUS  status = NV_OK;
    OBJGPU    *pGpu   = reinterpretCast(pCallbackData, OBJGPU *);

    status = osQueueWorkItemWithFlags(pGpu,
                                      RmRemoveIdleHoldoff,
                                      NULL,
                                      OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Queuing of remove idle holdoff work item failed with status : 0x%x\n",
                  status);
    }
}

/*!
 * @brief Timer callback to check if all idle conditions remain met and
 * if so schedule a work item to indicate the GPU is idle.
 *
 * @param[in]   pCallbackData   OBJGPU pointer (with void pointer type).
 */
static void timerCallbackToIndicateIdle(
    void *pCallbackData
)
{
    OBJGPU     *pGpu    = reinterpretCast(pCallbackData, OBJGPU *);
    nv_state_t *nv      = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp     = NV_GET_NV_PRIV(nv);

    nv_acquire_mmap_lock(nv);

    if (nv_get_all_mappings_revoked_locked(nv) &&
        nvp->dynamic_power.state == NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED &&
        RmCanEnterGcxUnderGpuLock(pGpu))
    {
        osQueueWorkItem(pGpu, RmIndicateIdle, NULL);
    }
    else
    {
        if (nvp->dynamic_power.state == NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED)
        {
            /*
             * Something used the GPU between when we last sampled and now.
             * Go back to the "instant" state to start waiting from scratch.
             */
            nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED,
                                                  NV_DYNAMIC_POWER_STATE_IDLE_INSTANT);
        }

        RmScheduleCallbackForIdlePreConditionsUnderGpuLock(pGpu);
    }

    nv_release_mmap_lock(nv);
}

/*!
 * @brief Timer callback to periodically check if all idle preconditions are
 * met.  If so, the dynamic power state is advanced from IDLE_INSTANT to
 * IDLE_SUSTAINED, or (if already in the IDLE_SUSTAINED state) mappings are
 * revoked and timerCallbackToIndicateIdle() is scheduled.
 *
 * @param[in]   pCallbackData   OBJGPU pointer (with void pointer type).
 */
static void timerCallbackForIdlePreConditions(
    void *pCallbackData
)
{
    OBJGPU     *pGpu    = reinterpretCast(pCallbackData, OBJGPU *);
    nv_state_t *nv      = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp     = NV_GET_NV_PRIV(nv);

    nvp->dynamic_power.idle_precondition_check_callback_scheduled = NV_FALSE;

    /*
     * Note: It is not safe to take the dynamic power mutex here.
     * Lock ordering dictates that the dynamic power mutex must be taken before
     * the GPU lock, and this function is called with the GPU lock held.
     *
     * Therefore, this function is careful about *only* triggering specific
     * transitions:
     * NV_DYNAMIC_POWER_STATE_IDLE_INSTANT -> NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED
     * NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED -> NV_DYNAMIC_POWER_STATE_IDLE_INSTANT
     *
     * Other codepaths are resilient to these state transitions occurring
     * asynchronously.  The state transition itself is done with an atomic
     * compare and swap, so there should be no danger of inadvertently
     * triggering a transition from a different state due to a
     * read-modify-write or "time of check to time of use" (TOCTTOU) race
     * condition.
     */

    if (nvp->dynamic_power.state != NV_DYNAMIC_POWER_STATE_IN_USE)
    {
        if (RmCanEnterGcxUnderGpuLock(pGpu))
        {
            switch (nvp->dynamic_power.state)
            {
            case NV_DYNAMIC_POWER_STATE_UNKNOWN:
                NV_ASSERT(0);
                /* fallthrough */
            case NV_DYNAMIC_POWER_STATE_IDLE_INDICATED:
                NV_PRINTF(LEVEL_ERROR, "unexpected dynamic power state 0x%x\n",
                          nvp->dynamic_power.state);
                /* fallthrough */
            case NV_DYNAMIC_POWER_STATE_IN_USE:
                break;
            case NV_DYNAMIC_POWER_STATE_IDLE_INSTANT:
                nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_IDLE_INSTANT,
                                                      NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED);
                break;
            case NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED:
                RmQueueIdleSustainedWorkitem(pGpu);
                return;
            }

        }
        else
        {
            if (nvp->dynamic_power.state == NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED)
            {
                /*
                 * Something used the GPU between when we last sampled and now.
                 * Go back to the "instant" state to start waiting from scratch.
                 */
                nv_dynamic_power_state_transition(nv, NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED,
                                                      NV_DYNAMIC_POWER_STATE_IDLE_INSTANT);
            }
        }
    }
    RmScheduleCallbackForIdlePreConditionsUnderGpuLock(pGpu);
}

/*!
 * @brief Cancel any in-flight callback to remove idle holdoff.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmCancelCallbackToRemoveIdleHoldoff(
    OBJGPU *pGpu
)
{
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_GENERAL_PARAMS cancelParams;

    if (NvP64_VALUE(nvp->dynamic_power.remove_idle_holdoff) != NULL)
    {
        portMemSet(&cancelParams, 0, sizeof(cancelParams));

        cancelParams.pEvent = nvp->dynamic_power.remove_idle_holdoff;

        tmrCtrlCmdEventCancel(pGpu, &cancelParams);
    }
}

/*!
 * @brief Cancel any in-flight timer callbacks.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmCancelDynamicPowerCallbacks(
    OBJGPU *pGpu
)
{
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_GENERAL_PARAMS cancelParams;

    if (NvP64_VALUE(nvp->dynamic_power.idle_precondition_check_event) != NULL)
    {
        // Cancel precondition check callback.
        portMemSet(&cancelParams, 0, sizeof(cancelParams));

        cancelParams.pEvent = nvp->dynamic_power.idle_precondition_check_event;

        tmrCtrlCmdEventCancel(pGpu, &cancelParams);
    }

    if (NvP64_VALUE(nvp->dynamic_power.indicate_idle_event) != NULL)
    {
        // Cancel callback to indicate idle
        portMemSet(&cancelParams, 0, sizeof(cancelParams));

        cancelParams.pEvent = nvp->dynamic_power.indicate_idle_event;

        tmrCtrlCmdEventCancel(pGpu, &cancelParams);
    }

    RmCancelCallbackToRemoveIdleHoldoff(pGpu);

    nvp->dynamic_power.idle_precondition_check_callback_scheduled = NV_FALSE;
}

/*!
 * @brief Free timer events allocated by CreateDynamicPowerCallbacks()
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmDestroyDynamicPowerCallbacks(
    OBJGPU *pGpu
)
{
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_GENERAL_PARAMS destroyParams;

    if (NvP64_VALUE(nvp->dynamic_power.idle_precondition_check_event) != NULL)
    {
        // Destroy precondition check callback.
        portMemSet(&destroyParams, 0, sizeof(destroyParams));

        destroyParams.pEvent = nvp->dynamic_power.idle_precondition_check_event;

        tmrCtrlCmdEventDestroy(pGpu, &destroyParams);
        nvp->dynamic_power.idle_precondition_check_event = NvP64_NULL;
    }

    if (NvP64_VALUE(nvp->dynamic_power.indicate_idle_event) != NULL)
    {
        // Destroy callback to indicate idle
        portMemSet(&destroyParams, 0, sizeof(destroyParams));

        destroyParams.pEvent = nvp->dynamic_power.indicate_idle_event;

        tmrCtrlCmdEventDestroy(pGpu, &destroyParams);
        nvp->dynamic_power.indicate_idle_event = NvP64_NULL;
    }

    if (NvP64_VALUE(nvp->dynamic_power.remove_idle_holdoff) != NULL)
    {
        // Destroy callback to decrease kernel refcount
        portMemSet(&destroyParams, 0, sizeof(destroyParams));

        destroyParams.pEvent = nvp->dynamic_power.remove_idle_holdoff;

        tmrCtrlCmdEventDestroy(pGpu, &destroyParams);
        nvp->dynamic_power.remove_idle_holdoff = NvP64_NULL;
    }

    nvp->dynamic_power.deferred_idle_enabled = NV_FALSE;
}

/*
 * @brief Adds a GPU to OBJOS::dynamicPowerSupportGpuMask
 *
 * @param[in]                  instance
 */
void osAddGpuDynPwrSupported
(
    NvU32 instance
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);

    pOS->dynamicPowerSupportGpuMask |= (1 << instance);
}

/*
 * @brief Removes a GPU from OBJOS::dynamicPowerSupportGpuMask
 *
 * @param[in]                  instance
 */
void osRemoveGpuDynPwrSupported
(
    NvU32 instance
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);

    pOS->dynamicPowerSupportGpuMask &= ~(1 << instance);
}

/*
 * @brief queries  OBJOS::dynamicPowerSupportGpuMask
 *
 * @param[in]                  void
 */
NvU32 osGetDynamicPowerSupportMask
(
    void
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);

    return  pOS->dynamicPowerSupportGpuMask;
}

/*!
 * @brief Cancel any outstanding callbacks, and free RM resources allocated by
 * RmInitDeferredDynamicPowerManagement().
 *
 * @param[in]   nv      nv_state_t pointer.
 */
void RmDestroyDeferredDynamicPowerManagement(
    nv_state_t *nv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    OBJGPU    *pGpu = NV_GET_NV_PRIV_PGPU(nv);

    //
    // Re-instate the dynamic power mode to fine if it was
    // disabled due to lack of platform support
    //
    if (nvp->dynamic_power.b_fine_not_supported == NV_TRUE)
    {
        nvp->dynamic_power.mode = NV_DYNAMIC_PM_FINE;
        nvp->dynamic_power.b_fine_not_supported = NV_FALSE;
    }

    //
    // Before cancelling and destroying the callbacks, make sure to
    // remove the idle holfoff that was applied during gc6 exit.
    //
    if (nvp->dynamic_power.b_idle_holdoff == NV_TRUE)
    {
        nv_indicate_idle(nv);
        nvp->dynamic_power.b_idle_holdoff = NV_FALSE;
    }

    RmCancelDynamicPowerCallbacks(pGpu);
    RmDestroyDynamicPowerCallbacks(pGpu);
    osRemoveGpuDynPwrSupported(gpuGetInstance(pGpu));
}

/*!
 * @brief Allocate timer events to call the functions:
 * timerCallbackForIdlePreConditions()
 * timerCallbackToIndicateIdle()
 *
 * The callbacks must be scheduled separately.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static NV_STATUS CreateDynamicPowerCallbacks(
    OBJGPU *pGpu
)
{
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_SET_PARAMS createEventParams;
    NV_STATUS status;

    // Creating PreCondition check callback.
    portMemSet(&createEventParams, 0, sizeof(createEventParams));

    createEventParams.ppEvent = &(nvp->dynamic_power.idle_precondition_check_event);
    createEventParams.pTimeProc = NV_PTR_TO_NvP64(timerCallbackForIdlePreConditions);
    createEventParams.pCallbackData = NV_PTR_TO_NvP64(pGpu);
    createEventParams.flags = TMR_FLAG_USE_OS_TIMER;

    status = tmrCtrlCmdEventCreate(pGpu, &createEventParams);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error creating dynamic power precondition check callback\n");
        nvp->dynamic_power.idle_precondition_check_event = NvP64_NULL;
        nvp->dynamic_power.indicate_idle_event = NvP64_NULL;
        nvp->dynamic_power.remove_idle_holdoff = NvP64_NULL;
        return status;
    }

    // Create callback to indicate idle..
    portMemSet(&createEventParams, 0, sizeof(createEventParams));

    createEventParams.ppEvent = &(nvp->dynamic_power.indicate_idle_event);
    createEventParams.pTimeProc = NV_PTR_TO_NvP64(timerCallbackToIndicateIdle);
    createEventParams.pCallbackData = NV_PTR_TO_NvP64(pGpu);
    createEventParams.flags = TMR_FLAG_USE_OS_TIMER;

    status = tmrCtrlCmdEventCreate(pGpu, &createEventParams);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error creating callback to indicate GPU idle\n");
        nvp->dynamic_power.idle_precondition_check_event = NvP64_NULL;
        nvp->dynamic_power.indicate_idle_event = NvP64_NULL;
        nvp->dynamic_power.remove_idle_holdoff = NvP64_NULL;
        return status;
    }

    // Create callback to remove idle holdoff
    portMemSet(&createEventParams, 0, sizeof(createEventParams));

    createEventParams.ppEvent = &(nvp->dynamic_power.remove_idle_holdoff);
    createEventParams.pTimeProc = NV_PTR_TO_NvP64(timerCallbackToRemoveIdleHoldoff);
    createEventParams.pCallbackData = NV_PTR_TO_NvP64(pGpu);
    createEventParams.flags = TMR_FLAG_USE_OS_TIMER;

    status = tmrCtrlCmdEventCreate(pGpu, &createEventParams);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error creating callback to decrease kernel refcount\n");
        nvp->dynamic_power.idle_precondition_check_event = NvP64_NULL;
        nvp->dynamic_power.indicate_idle_event = NvP64_NULL;
        nvp->dynamic_power.remove_idle_holdoff = NvP64_NULL;
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Schedule the timerCallbackForIdlePreConditions() function to be called.
 *
 * If it is already scheduled, the function is a noop.
 *
 * The GPU lock must be held when calling this function.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmScheduleCallbackForIdlePreConditionsUnderGpuLock(
    OBJGPU *pGpu
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_SCHEDULE_PARAMS scheduleEventParams;
    NV_STATUS status;

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    if (nvp->dynamic_power.idle_precondition_check_callback_scheduled)
    {
        return;
    }

    if (NvP64_VALUE(nvp->dynamic_power.idle_precondition_check_event) != NULL)
    {
        portMemSet(&scheduleEventParams, 0, sizeof(scheduleEventParams));

        scheduleEventParams.pEvent = nvp->dynamic_power.idle_precondition_check_event;
        scheduleEventParams.timeNs = GC6_PRECONDITION_CHECK_TIME;
        scheduleEventParams.bUseTimeAbs = NV_FALSE;

        status = tmrCtrlCmdEventSchedule(pGpu, &scheduleEventParams);

        if (status == NV_OK)
        {
            nvp->dynamic_power.idle_precondition_check_callback_scheduled = NV_TRUE;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Error scheduling precondition callback\n");
        }
    }
}

/*!
 * @brief Schedule the timerCallbackForIdlePreConditions() function to be called.
 *
 * If it is already scheduled, the function is a noop.
 *
 * This function takes the GPU lock.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmScheduleCallbackForIdlePreConditions(
    OBJGPU *pGpu
)
{
    NV_STATUS   status;
    GPU_MASK    gpuMask;

    // LOCK: acquire per device lock
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                   GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER,
                                   &gpuMask);
    if (status == NV_OK)
    {
        RmScheduleCallbackForIdlePreConditionsUnderGpuLock(pGpu);
        // UNLOCK: release per device lock
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
    }
}

/*!
 * @brief Schedule the timerCallbackToIndicateIdle() function to be called.
 *
 * The GPU lock must be held when calling this function.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmScheduleCallbackToIndicateIdle(
    OBJGPU *pGpu
)
{
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_SCHEDULE_PARAMS scheduleEventParams;
    NV_STATUS status;

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    if (NvP64_VALUE(nvp->dynamic_power.indicate_idle_event) != NULL)
    {
        portMemSet(&scheduleEventParams, 0, sizeof(scheduleEventParams));

        scheduleEventParams.pEvent = nvp->dynamic_power.indicate_idle_event;
        scheduleEventParams.timeNs = GC6_BAR1_BLOCKER_CHECK_AND_METHOD_FLUSH_TIME;
        scheduleEventParams.bUseTimeAbs = NV_FALSE;

        status = tmrCtrlCmdEventSchedule(pGpu, &scheduleEventParams);

        if (status != NV_OK)
            NV_PRINTF(LEVEL_ERROR, "Error scheduling indicate idle callback\n");
    }
}

/*!
 * @brief Schedule the timerCallbackToRemoveIdleHoldoff() function.
 *
 * The GPU lock must be held when calling this function.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static void RmScheduleCallbackToRemoveIdleHoldoff(
    OBJGPU *pGpu
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    TMR_EVENT_SCHEDULE_PARAMS scheduleEventParams;
    NV_STATUS status;

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    if (NvP64_VALUE(nvp->dynamic_power.remove_idle_holdoff) != NULL)
    {
        portMemSet(&scheduleEventParams, 0, sizeof(scheduleEventParams));

        scheduleEventParams.pEvent = nvp->dynamic_power.remove_idle_holdoff;
        scheduleEventParams.timeNs = GC6_CYCLE_IDLE_HOLDOFF_CHECK_TIME;
        scheduleEventParams.bUseTimeAbs = NV_FALSE;

        status = tmrCtrlCmdEventSchedule(pGpu, &scheduleEventParams);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Error scheduling kernel refcount decrement callback\n");
        }
        else
        {
            nvp->dynamic_power.b_idle_holdoff = NV_TRUE;
        }
    }
}

/*!
 * @brief Check if the system supports RTD3-GC6.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 */
static NvBool RmCheckRtd3GcxSupport(
    nv_state_t *pNv,
    NvBool     *bGC6Support,
    NvBool     *bGCOFFSupport
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(pNv);
    OBJGPU    *pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    RM_API    *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS  status;
    NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS virtModeParams = { 0 };
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED))
    {
        NV_PRINTF(LEVEL_NOTICE, "RTD3 is not supported for this arch\n");
        return NV_FALSE;
    }

    *bGC6Support = pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED);
    *bGCOFFSupport = nvp->b_mobile_config_enabled ? *bGC6Support :
                     pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED);

    if (!(*bGC6Support) && !(*bGCOFFSupport))
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "Disabling RTD3. [GC6 support=%d GCOFF support=%d]\n",
                  *bGC6Support, *bGCOFFSupport);
        return NV_FALSE;
    }

    status = pRmApi->Control(pRmApi, pNv->rmapi.hClient, pNv->rmapi.hDevice,
                             NV0080_CTRL_CMD_GPU_GET_VIRTUALIZATION_MODE,
                             &virtModeParams, sizeof(virtModeParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get Virtualization mode, status=0x%x\n",
                  status);
        return NV_FALSE;
    }

    if ((virtModeParams.virtualizationMode != NV0080_CTRL_GPU_VIRTUALIZATION_MODE_NONE) &&
        (virtModeParams.virtualizationMode != NV0080_CTRL_GPU_VIRTUALIZATION_MODE_NMOS))
    {
        NV_PRINTF(LEVEL_NOTICE, "RTD3 is not supported on VM\n");
        return NV_FALSE;
    }

    if (!nvp->pr3_acpi_method_present)
    {
        NV_PRINTF(LEVEL_NOTICE, "RTD3 ACPI support is not available.\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

/*!
 * @brief Allocate resources needed to track deferred power management, and
 * schedule the initial periodic callbacks.
 *
 * @param[in]   nv      nv_state_t pointer.
 */
static void RmInitDeferredDynamicPowerManagement(
    nv_state_t *nv,
    NvBool      bRtd3Support
)
{
    NV_STATUS  status;
    nv_priv_t *nvp  = NV_GET_NV_PRIV(nv);

    if (nvp->dynamic_power.mode == NV_DYNAMIC_PM_FINE)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

        if (!bRtd3Support)
        {
             nvp->dynamic_power.mode = NV_DYNAMIC_PM_NEVER;
             nvp->dynamic_power.b_fine_not_supported = NV_TRUE;
             NV_PRINTF(LEVEL_NOTICE, "RTD3 is not supported.\n");
             return;
        }
        osAddGpuDynPwrSupported(gpuGetInstance(pGpu));
        nvp->dynamic_power.b_fine_not_supported = NV_FALSE;
        status = CreateDynamicPowerCallbacks(pGpu);

        if (status == NV_OK)
        {
            RmScheduleCallbackForIdlePreConditionsUnderGpuLock(pGpu);

            nvp->dynamic_power.deferred_idle_enabled = NV_TRUE;
            // RM's default is GCOFF allow
            nvp->dynamic_power.clients_gcoff_disallow_refcount = 0;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to register for dynamic power callbacks\n");
        }
    }
}

/*!
 * @brief Check if GCOFF state can be used for Power Management.
 *
 * @param[in]   pGpu            OBJGPU pointer.
 * @param[in]   usedFbSize      Used FB Allocation size.
 * @param[in]   bIsDynamicPM    If true, check for dynamic PM; otherwise, check
 *                              for system PM (suspend/resume).
 *
 * returns NV_TRUE if GCOFF state can be used, otherwise NV_FALSE.
 *
 */
static NvBool RmCheckForGcOffPM(
    OBJGPU *pGpu,
    NvU64   usedFbSize,
    NvBool  bIsDynamicPM
)
{
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    NvU64       gcoff_max_fb_size;

    if (bIsDynamicPM)
    {
        /*
         * GCOFF won't be enaged for Dynamic PM if X server does not allow.
         * The dynamic_power.clients_gcoff_disallow_refcount is being accessed
         * without acquiring dynamic power mutex since this value can be
         * modified only when GPU is in active state. The os_ref_dynamic_power()
         * function will be called to hold a reference to the GPU whenever
         * RM is processing osClientGcoffDisallowRefcount() calls. This either
         * prevents the GPU from going into a low power state or will wait for
         * it to be resumed before proceeding.
         */
        if (nvp->dynamic_power.clients_gcoff_disallow_refcount != 0)
            return NV_FALSE;

        gcoff_max_fb_size = nvp->dynamic_power.gcoff_max_fb_size;
    }
    else
    {
        gcoff_max_fb_size = nvp->s0ix_gcoff_max_fb_size;
    }

    /*
     * GCOFF will be engaged whenever the following necessary preconditions
     * are met:
     *
     * 1. The GCOFF has not been disabled with regkey by setting it to zero.
     * 2. Used FB allocation size are within limits.
     */
    return (gcoff_max_fb_size > 0) &&
           (usedFbSize <= gcoff_max_fb_size);
}

//
// Function to update fixed fbsr modes to support multiple vairants such as
// GCOFF and cuda S3/resume.
//
static void
RmUpdateFixedFbsrModes(OBJGPU *pGpu)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    nv_state_t    *nv             = NV_GET_NV_STATE(pGpu);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING))
    {
        pMemoryManager->fixedFbsrModesMask = NVBIT(FBSR_TYPE_DMA);
    }
    else if (nv->preserve_vidmem_allocations)
    {
        pMemoryManager->fixedFbsrModesMask = NVBIT(FBSR_TYPE_FILE);
    }
}

static NV_STATUS
RmPowerManagement(
    OBJGPU *pGpu,
    nv_pm_action_t pmAction
)
{
    // default to NV_OK. there may cases where resman is loaded, but
    // no devices are allocated (we're still at the console). in these
    // cases, it's fine to let the system do whatever it wants.
    NV_STATUS rmStatus = NV_OK;

    if (pGpu)
    {
        nv_state_t *nv = NV_GET_NV_STATE(pGpu);
        nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
        NvBool bcState = gpumgrGetBcEnabledStatus(pGpu);
        Intr *pIntr = GPU_GET_INTR(pGpu);

        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        if ((pmAction == NV_PM_ACTION_HIBERNATE) || (pmAction == NV_PM_ACTION_STANDBY))
        {
            //
            // pFb object store the FBSR mode through which FB state unload has happened,
            // so os layer doesn't need to set FBSR mode on resume.
            //
            RmUpdateFixedFbsrModes(pGpu);
        }

        switch (pmAction)
        {
            case NV_PM_ACTION_HIBERNATE:
                nvp->pm_state.InHibernate = NV_TRUE;
                nvp->pm_state.IntrEn = intrGetIntrEn(pIntr);
                intrSetIntrEn(pIntr, INTERRUPT_TYPE_DISABLED);
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

                rmStatus = gpuEnterHibernate(pGpu);
                gpumgrSetBcEnabledStatus(pGpu, bcState);

                break;

            case NV_PM_ACTION_STANDBY:
                nvp->pm_state.InHibernate = NV_FALSE;
                nvp->pm_state.IntrEn = intrGetIntrEn(pIntr);
                intrSetIntrEn(pIntr, INTERRUPT_TYPE_DISABLED);
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

                rmStatus = gpuEnterStandby(pGpu);
                gpumgrSetBcEnabledStatus(pGpu, bcState);

                break;

            case NV_PM_ACTION_RESUME:
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

                if (nvp->pm_state.InHibernate)
                {
                    gpuResumeFromHibernate(pGpu);
                }
                else
                {
                    gpuResumeFromStandby(pGpu);
                }
                intrSetIntrEn(pIntr, nvp->pm_state.IntrEn);
                gpumgrSetBcEnabledStatus(pGpu, bcState);
                NvU32 ac_plugged = 0;

                if (IsMobile(pGpu))
                {
                    if (nv_acpi_get_powersource(&ac_plugged) == NV_OK)
                    {
                        //
                        // As we have already acquired API and GPU lock here, we are
                        // directly calling RmPowerSourceChangeEvent.
                        //
                        RmPowerSourceChangeEvent(nv, !ac_plugged);
                    }
                    RmRequestDNotifierState(nv);
                }
                break;

            default:
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                break;
        }
        pMemoryManager->fixedFbsrModesMask = 0;
    }

    return rmStatus;
}

/*!
 * @brief Call core RM to perform the GCx (GC6/GCOF) entry sequence (before
 * powering off the GPU) or exit sequence (after powering the GPU back on).
 *
 * The GPU lock should be held when calling this function.
 *
 * @param[in]   pGpu            OBJGPU pointer.
 * @param[in]   bEnter          If true, perform entry sequence; exit sequence
 *                              otherwise.
 * @param[in]   bIsDynamicPM    If true, it is for dynamic PM; otherwise, it is
 *                              for system PM (suspend/resume).
 *
 * @return      NV_STATUS code indicating if the operation was successful.
 */
NV_STATUS RmGcxPowerManagement(
    OBJGPU *pGpu,
    NvBool  bEnter,
    NvBool  bIsDynamicPM,
    NvBool *bTryAgain
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    nv_state_t         *nv                  = NV_GET_NV_STATE(pGpu);
    nv_priv_t          *nvp                 = NV_GET_NV_PRIV(nv);
    NV_STATUS           status              = NV_OK;

    if (pGpu->acpiMethodData.jtMethodData.bSBIOSCaps &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_MOBILE))
    {
        //
        // AML override is expected only on NB platforms to support GCOFF-1.0(
        // which is legacy GCOFF) and GC6-3.0
        // GC6 3.0 is possible without AML override but due to changes required
        // for GCOFF-1.0 in SBIOS & HW, AML override is needed for GC6-3.0 also.
        //
        NV_PRINTF(LEVEL_INFO,"AML overrides present in Desktop");
    }

    nv->d0_state_in_suspend = NV_FALSE;

    if (bEnter)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        NvU64          usedFbSize = 0;
        NvBool         bCanUseGc6 = NV_FALSE;
        NV_STATUS      fbsrStatus = NV_ERR_GENERIC;

        //
        // If the GPU supports GC6, then check if GC6 can be used for
        // the current power management request.
        // 1. For dynamic PM, GC6 can be used if it is supported by the GPU.
        // 2. For system PM with s2idle, GC6 can be used if it is
        //    supported by the GPU.
        //
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED) &&
            nvp->gc6_upstream_port_configured)
        {
            bCanUseGc6 = bIsDynamicPM ? NV_TRUE : nv_s2idle_pm_configured();
        }

        //
        // If GC6 cannot be used, then no need to compare the used FB size with
        // threshold value and select GCOFF irrespective of FB size.
        //
        if ((memmgrGetUsedRamSize(pGpu, pMemoryManager, &usedFbSize) == NV_OK) &&
            (!bCanUseGc6 || RmCheckForGcOffPM(pGpu, usedFbSize, bIsDynamicPM)) &&
            ((fbsrStatus = fbsrReserveSysMemoryForPowerMgmt(pGpu, pMemoryManager->pFbsr[FBSR_TYPE_DMA],
                                             usedFbSize)) == NV_OK))
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING, NV_TRUE);

            //
            // Set 'bPreserveComptagBackingStoreOnSuspend' so that comptag
            // related handling can be done during state unload/load.
            //
            if (!pMemoryManager->bUseVirtualCopyOnSuspend)
            {
                pKernelMemorySystem->bPreserveComptagBackingStoreOnSuspend = NV_TRUE;
            }

            status = RmPowerManagement(pGpu, NV_PM_ACTION_STANDBY);
            pGpu->setProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING, NV_FALSE);

            if (status == NV_OK)
            {
                pGpu->setProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERED, NV_TRUE);
            }
            else
            {
                /*
                 * Normally the NV_PM_ACTION_RESUME resume should free the
                 * the reserved memory while doing FB state load. But if the
                 * failure has happened at very early stage in FB state unload
                 * while doing NV_PM_ACTION_STANDBY, then reserved system memory
                 * might not be freed.
                 */
                fbsrFreeReservedSysMemoryForPowerMgmt(pMemoryManager->pFbsr[FBSR_TYPE_DMA]);
                pKernelMemorySystem->bPreserveComptagBackingStoreOnSuspend = NV_FALSE;
            }
        }
        else if (bCanUseGc6)
        {
            NV2080_CTRL_GC6_ENTRY_PARAMS entryParams;
            portMemSet(&entryParams, 0, sizeof(entryParams));

            entryParams.flavorId = NV2080_CTRL_GC6_FLAVOR_ID_MSHYBRID;
            entryParams.stepMask = NVBIT(NV2080_CTRL_GC6_STEP_ID_GPU_OFF);
            entryParams.params.bIsRTD3Transition = NV_TRUE;

            //
            // Currently if the GPU is not in the Lowest P-State, then the
            // GC6 entry will fail in its internal sanity check. The system
            // suspend can be triggered in any P-State, so sanity check
            // for the lowest P-State needs to be skipped. In this case, during
            // Perf state unload time, the forceful P-State switch will
            // happen which will bring the GPU into lowest P-State.
            //
            entryParams.params.bSkipPstateSanity = !bIsDynamicPM;

            status = gpuGc6Entry(pGpu, &entryParams);

            //
            // The GC6 state requires SW involvement before each transition
            // from D0 to D3cold. In case of system suspend with pass-through
            // mode, the VM suspend should happen before host suspend. During
            // VM suspend, the NVIDIA driver running in the VM side will be
            // involved in D3cold transition, but in the host side, it will
            // go through D3cold->D0->D3cold transition and will make the
            // second D3cold transition without NVIDIA driver. To handle this
            // use-case, during VM suspend, use the D0 state and the host linux
            // kernel will put the device into D3cold state.
            //
            if ((status == NV_OK) && ((nv->flags & NV_FLAG_PASSTHRU) != 0))
            {
                nv->d0_state_in_suspend = NV_TRUE;
            }
        }
        //
        // The else condition below will hit in the following cases: 
        // Case 1. During system suspend transition: For systems that support s2idle but are configured
        // for deep sleep, this "else" condition will be hit when the system memory 
        // is not sufficient. In this case, we should unset bTryAgain to abort the current suspend entry. 
        // Case 2. During runtime suspend transition: For systems that do not support GC6 but support 
        // GCOFF, this "else" condition will be hit when system memory is not sufficent, In this case, we 
        // should set bTryagain so that the kernel can reschedule the callback later. 
        //
        else
        {
            if (bIsDynamicPM)
            {
                if (fbsrStatus == NV_ERR_NO_MEMORY)
                {
                    *bTryAgain = NV_TRUE;
                }
                status = fbsrStatus;
            }
            else
            {
                status = NV_ERR_NOT_SUPPORTED;
                NV_PRINTF(LEVEL_ERROR,
                          "System suspend failed with current system suspend configuration. "
                          "Please change the system suspend configuration to s2idle in /sys/power/mem_sleep.\n");
            }
        }
    }
    else
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERED))
        {
            status = RmPowerManagement(pGpu, NV_PM_ACTION_RESUME);
            pGpu->setProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERED, NV_FALSE);
            pKernelMemorySystem->bPreserveComptagBackingStoreOnSuspend = NV_FALSE;
        }
        else
        {
            NV2080_CTRL_GC6_EXIT_PARAMS exitParams;
            portMemSet(&exitParams, 0, sizeof(exitParams));

            exitParams.flavorId = NV2080_CTRL_GC6_FLAVOR_ID_MSHYBRID;
            exitParams.params.bIsRTD3Transition = NV_TRUE;

            status = gpuGc6Exit(pGpu, &exitParams);
        }
    }

    return status;
}

NV_STATUS NV_API_CALL rm_power_management(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    nv_pm_action_t pmAction
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_OK;
    void *fp;
    NvBool bTryAgain = NV_FALSE;
    NvBool bConsoleDisabled = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_DEVICE_INIT);

    NV_ASSERT_OK(os_flush_work_queue(pNv->queue, pmAction != NV_PM_ACTION_RESUME));

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER)) == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(pNv);

        if (pGpu != NULL)
        {
            if ((rmStatus = os_ref_dynamic_power(pNv, NV_DYNAMIC_PM_FINE)) == NV_OK)
            {
                // LOCK: acquire GPUs lock
                if ((rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER)) == NV_OK)
                {
                    //
                    // For GPU driving console, disable console access here, to ensure no console
                    // writes through BAR1 can interfere with physical RM's setup of BAR1
                    //
                    if (pNv->client_managed_console)
                    {
                        os_disable_console_access();
                        bConsoleDisabled = NV_TRUE;
                    }

                    nv_priv_t *nvp = NV_GET_NV_PRIV(pNv);

                    //
                    // Before going to S3 or S4, remove idle holdoff which was
                    // applied during gc6 exit.
                    //
                    if ((pmAction != NV_PM_ACTION_RESUME) &&
                        (nvp->dynamic_power.b_idle_holdoff == NV_TRUE))
                    {
                        nv_indicate_idle(pNv);
                        RmCancelCallbackToRemoveIdleHoldoff(pGpu);
                        nvp->dynamic_power.b_idle_holdoff = NV_FALSE;
                    }

                    //
                    // Use GCx (GCOFF/GC6) power management if S0ix-based PM is
                    // enabled and the request is for system suspend/resume.
                    // Otherwise, use the existing mechanism.
                    //
                    if (nvp->s0ix_pm_enabled &&
                        (pmAction == NV_PM_ACTION_STANDBY ||
                        (pmAction == NV_PM_ACTION_RESUME &&
                         !nvp->pm_state.InHibernate)))
                    {
                        rmStatus = RmGcxPowerManagement(pGpu,
                                        pmAction == NV_PM_ACTION_STANDBY,
                                        NV_FALSE, &bTryAgain);

                    }
                    else
                    {
                        rmStatus = RmPowerManagement(pGpu, pmAction);
                    }

                    if (bConsoleDisabled)
                    {
                        os_enable_console_access();
                    }

                    // UNLOCK: release GPUs lock
                    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
                }
                os_unref_dynamic_power(pNv, NV_DYNAMIC_PM_FINE);
            }
        }
        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    NV_ASSERT_OK(os_flush_work_queue(pNv->queue, pmAction != NV_PM_ACTION_RESUME));

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

/*!
 * @brief Wrapper around RmGcxPowerManagement() and removes the Idle holdoff
 * during exit sequence.
 *
 * The GPU lock should be held when calling this function.
 *
 * @param[in]   pGpu    OBJGPU pointer.
 * @param[in]   bEnter  If true, perform entry sequence; exit sequence
 *                      otherwise.
 *
 * @return      NV_STATUS code indicating if the operation was successful.
 */
static NV_STATUS RmTransitionDynamicPower(
    OBJGPU *pGpu,
    NvBool  bEnter,
    NvBool *bTryAgain
)
{
    nv_state_t *nv   = NV_GET_NV_STATE(pGpu);
    NV_STATUS   status;

    status = RmGcxPowerManagement(pGpu, bEnter, NV_TRUE, bTryAgain);

    if (!bEnter && status == NV_OK)
    {
        nv_idle_holdoff(nv);
        RmScheduleCallbackToRemoveIdleHoldoff(pGpu);
    }

    return status;
}

/*!
 * @brief Wrapper around RmTransitionDynamicPower() which sets up the RM
 * runtime (thread state and altstack), and takes the GPU lock for the duration
 * of the operation.
 *
 * @param[in]   sp      nvidia_stack_t pointer.
 * @param[in]   nv      nv_state_t pointer.
 * @param[in]   bEnter  If true, perform entry sequence; exit sequence
 *                      otherwise.
 *
 * @return      NV_STATUS code indicating if the operation was successful.
 */
NV_STATUS NV_API_CALL rm_transition_dynamic_power(
    nvidia_stack_t *sp,
    nv_state_t     *nv,
    NvBool          bEnter,
    NvBool         *bTryAgain
)
{
    OBJGPU             *pGpu = NV_GET_NV_PRIV_PGPU(nv);
    NV_STATUS           status = NV_OK;
    THREAD_STATE_NODE   threadState;
    void               *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire GPUs lock
    status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER);
    if (status == NV_OK)
    {
        rmapiEnterRtd3PmPath();

        status = RmTransitionDynamicPower(pGpu, bEnter, bTryAgain);

        rmapiLeaveRtd3PmPath();

        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

/*!
 * @brief: Notify client about hot-plugged/unplugged displays.
 * Called after GPU exits GC6 because of display hot-plug/unplug.
 *
 * @param[in]   nv    nv_state_t pointer.
 */
static void RmNotifyClientAboutHotplug(
    nv_state_t *nv
)
{
    NV_STATUS status;
    GPU_MASK gpuMask;
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

    Nv2080HotplugNotification hotplugNotificationParams;

    portMemSet(&hotplugNotificationParams, 0, sizeof(hotplugNotificationParams));

    // LOCK: acquire per device lock
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                   GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HOTPLUG,
                                   &gpuMask);
    if (status == NV_OK)
    {
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_HOTPLUG,
                                &hotplugNotificationParams,
                                sizeof(hotplugNotificationParams), 0, 0);
        // UNLOCK: release per device lock
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
    }
}

/*!
 * @brief If GPU is in GC6, exit GC6 state and
 * notify client about display hot-plug/unplug.
 *
 * @param[in]   sp    nvidia_stack_t pointer.
 * @param[in]   nv    nv_state_t pointer.
 */
void RmHandleDisplayChange(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

    if (pGpu &&
        (IS_GPU_GC6_STATE_ENTERED(pGpu) ||
         pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERED)))
    {
        if (rm_ref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE) == NV_OK)
        {
            RmNotifyClientAboutHotplug(nv);
            rm_unref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE);
        }
        else
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to increment dynamic power refcount\n");
    }
}

/*!
 * @brief RmGetDynamicBoostSupport() gets the notebook DB
 * support. DB support is static information for vast majority
 * of notebooks and hence we fetch it only once (during the first 
 * call to this function). This way, the GPU remains in RTD3 state
 * when procfs read is done.
 *
 *  @param[in]   nv    nv_state_t pointer.
 */
static const char* RmGetDynamicBoostSupport(
    nv_state_t *pNv
)
{
    nv_priv_t *pNvp = NV_GET_NV_PRIV(pNv);
    const char* DbStatus = "?";

    if (pNvp->db_supported == -1)
    {
        pNvp->db_supported = os_get_dynamic_boost_support(pNv);
    }
    
    if (pNvp->db_supported == 0)
    {
        DbStatus = "Not Supported";
    }
    else if (pNvp->db_supported == 1)
    { 
        DbStatus = "Supported";
    }
    return DbStatus;
}

/*!
 * @brief: Function to query Dynamic Power Management
 *
 * @param[in]   nvp     nv_priv_t pointer.
 *
 * @return      String indicating Dynamic Power Management status.
 */
static const char* RmGetDynamicPowerManagementStatus(
        nv_priv_t *nvp
)
{
   /*
    * If the platform/driver does not support Dynamic Power Management,
    * we set mode as NV_DYNAMIC_PM_NEVER. Hence, after RmInit if the
    * mode is still NV_DYNAMIC_PM_FINE, we are sure that it is
    * supported and enabled. Also see NOTE.
    */
    if (nvp->dynamic_power.mode == NV_DYNAMIC_PM_FINE)
    {
        return "Enabled (fine-grained)";
    }
    else if (nvp->dynamic_power.mode == NV_DYNAMIC_PM_COARSE)
    {
        return "Enabled (coarse-grained)";
    }
    else if (nvp->dynamic_power.mode == NV_DYNAMIC_PM_NEVER)
    {

        if (nvp->dynamic_power.dynamic_power_regkey ==
            NV_REG_DYNAMIC_POWER_MANAGEMENT_NEVER)
            return "Disabled";
        else if (nvp->dynamic_power.dynamic_power_regkey ==
                 NV_REG_DYNAMIC_POWER_MANAGEMENT_DEFAULT)
            return "Disabled by default";
    }

    return "Not supported";
}

static void RmHandleIdleSustained(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU     *pGpu = gpumgrGetGpu(gpuInstance);
    nv_state_t *nv   = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp  = NV_GET_NV_PRIV(nv);

    nv_revoke_gpu_mappings(NV_GET_NV_STATE(pGpu));
    RmScheduleCallbackToIndicateIdle(pGpu);
    nvp->dynamic_power.b_idle_sustained_workitem_queued = NV_FALSE;
}

/*
 * Queue a workitem for revoking GPU mappings, and scheduling a callback to indicate idle.
 */
static void RmQueueIdleSustainedWorkitem(
    OBJGPU *pGpu
)
{
    nv_state_t *nv   = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp  = NV_GET_NV_PRIV(nv);
    NV_STATUS status = NV_OK;

    if (!nvp->dynamic_power.b_idle_sustained_workitem_queued)
    {
        status = osQueueWorkItemWithFlags(pGpu,
                                          RmHandleIdleSustained,
                                          NULL,
                                          OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Failed to queue RmHandleIdleSustained() workitem.\n");
            RmScheduleCallbackForIdlePreConditionsUnderGpuLock(pGpu);
            return;
        }
        nvp->dynamic_power.b_idle_sustained_workitem_queued = NV_TRUE;
    }
}

/*
 * Allocate resources needed for S0ix-based system power management.
 */
static void
RmInitS0ixPowerManagement(
    nv_state_t *nv,
    NvBool bRtd3Support,
    NvBool bGC6Support
)
{
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    NvU32       data;

    // S0ix-based S2Idle, on desktops, is not supported yet. Return early for desktop SKUs
    if (!nvp->b_mobile_config_enabled)
    {
        return;
    }

    /*
     * The GPU S0ix-based system power management will be enabled
     * only if all the following necessary requirements are met:
     *
     * 1. The GPU has RTD3 support.
     * 2. The platform has support for s0ix.
     * 3. Feature regkey EnableS0ixPowerManagement is enabled.
     */
    if (bRtd3Support && bGC6Support &&
        nv_platform_supports_s0ix() &&
        (osReadRegistryDword(NULL, NV_REG_ENABLE_S0IX_POWER_MANAGEMENT,
                             &data) == NV_OK) && (data == 1))
    {
        nvp->s0ix_pm_enabled = NV_TRUE;

        /*
         * Read the OS registry for Maximum FB size for S0ix-based PM
         * which will be expressed in Megabytes (1048576 bytes) and
         * convert it into bytes.
         */
        if ((osReadRegistryDword(NULL,
                                 NV_REG_S0IX_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD,
                                 &data) == NV_OK))
        {
            nvp->s0ix_gcoff_max_fb_size = (NvU64)data * 1024 * 1024;
        }
    }
}

/*!
 * @brief: Helper function to abstract the execution and return value verification
 *         of PEX DSM methods, necessary for GC6 to work in Desktops.
 *
 * This function is capable to invoke three _DSM methods, as listed below:
 *   0x0: _DSM for "Query supported functions"
 *        (for more details, refer to section 9.1.1 of the ACPI 6.3 Specification).
 *   0xA: _DSM for "Requesting D3cold Aux Power Limit"
 *        (for more details, refer to section 4.6.10 of PCI Firmware Specification v3.3)
 *   0xB: _DSM for "Adding PERST# Assertion Delay"
 *        (for more details, refer to section 4.6.11 of PCI Firmware Specification v3.3)
 *
 * @param[in]   nv                 nv_state_t pointer.
 * @param[in]   dsmSubFunc         subfunction to invoke.
 * @param[in]   data               input data for dsmSubFunc.
 *
 * @return      TRUE if dsmSubFunc executed successfully and return value is as expected;
 *              FALSE otherwise.
 */
static NvBool RmAcpiD3ColdDsm
(
    nv_state_t *nv,
    NvU32       dsmSubFunc,
    NvU32       data
)
{
    NvU32 inData = data;

    /*
     * Third argument (i.e. acpiDsmRev) is sent as 4 because PEX_FUNC_AUXPOWERLIMIT and
     * PEX_FUNC_PEXRST_DELAY sub-functions were introduced in PCI Firmware Specification 3.2
     * and they are supported only for _DSM revision 4 onwards. But, RM still use _DSM
     * revision 2 (see drivers/common/inc/pex.h). Hence, the need to hardcode 4 here.
     */
    if (nv_acpi_d3cold_dsm_for_upstream_port(nv, (NvU8 *)&PEX_DSM_GUID, 4, dsmSubFunc, &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_NOTICE, "%s: PEX _DSM subfunction: 0x%X failed.\n", __FUNCTION__, dsmSubFunc);
        return NV_FALSE;
    }

    switch (dsmSubFunc)
    {
        case NV_ACPI_ALL_FUNC_SUPPORT:
            return ((data & NVBIT(PEX_FUNC_PEXRST_DELAY)) &&
                    (data & NVBIT(PEX_FUNC_AUXPOWERLIMIT)));

        case PEX_FUNC_PEXRST_DELAY:
            return (inData == data);

        case PEX_FUNC_AUXPOWERLIMIT:
            return ((data == NV_AUX_POWER_REQUEST_STATUS_GRANTED_WITH_12V_POWER) ||
                    (data == NV_AUX_POWER_REQUEST_STATUS_GRANTED_WITHOUT_12V_POWER));

        default:
            return NV_FALSE;
    }

    return NV_FALSE;
}

static NvBool RmConfigureUpstreamPortForRTD3(
    nv_state_t *nv,
    NvBool      bConfigureForGC6
)
{
    nv_priv_t *nvp  = NV_GET_NV_PRIV(nv);
    OBJGPU    *pGpu = NV_GET_NV_PRIV_PGPU(nv);

    if (nvp->b_mobile_config_enabled)
        return NV_TRUE;

    if (!RmAcpiD3ColdDsm(nv, NV_ACPI_ALL_FUNC_SUPPORT, 0))
    {
        return NV_FALSE;
    }

    if (!RmAcpiD3ColdDsm(nv, PEX_FUNC_PEXRST_DELAY,
                         bConfigureForGC6 ? pGpu->gc6State.GC6PerstDelay : DEFAULT_GCOFF_PEXRST_DELAY))
    {
        return NV_FALSE;
    }

    if (!RmAcpiD3ColdDsm(nv, PEX_FUNC_AUXPOWERLIMIT,
                         bConfigureForGC6 ? pGpu->gc6State.GC6TotalBoardPower : 0))
    {
        if (bConfigureForGC6)
            RmAcpiD3ColdDsm(nv, PEX_FUNC_PEXRST_DELAY, DEFAULT_GCOFF_PEXRST_DELAY);

        return NV_FALSE;
    }

    NV_PRINTF(LEVEL_NOTICE,
              "Aux Power and Pex delay settings %s successfully.\n",
              bConfigureForGC6? "applied": "cleared");

    return NV_TRUE;
}

void RmInitPowerManagement(
    nv_state_t *nv
)
{
    // LOCK: acquire GPUs lock
    if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT) == NV_OK)
    {
        NvBool bGC6Support   = NV_FALSE;
        NvBool bGCOFFSupport = NV_FALSE;
        NvBool bRtd3Support  = RmCheckRtd3GcxSupport(nv, &bGC6Support, &bGCOFFSupport);
        nv_priv_t *nvp       = NV_GET_NV_PRIV(nv);

        RmInitDeferredDynamicPowerManagement(nv, bRtd3Support);
        RmInitS0ixPowerManagement(nv, bRtd3Support, bGC6Support);

        if (bRtd3Support && bGC6Support &&
            (nvp->s0ix_pm_enabled || (nvp->dynamic_power.mode == NV_DYNAMIC_PM_FINE)))
        {
            nvp->gc6_upstream_port_configured = RmConfigureUpstreamPortForRTD3(nv, NV_TRUE);
        }

        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }
}

void RmDestroyPowerManagement(
    nv_state_t *nv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);

    if (nvp->gc6_upstream_port_configured)
    {
        RmConfigureUpstreamPortForRTD3(nv, NV_FALSE);
    }

    RmDestroyDeferredDynamicPowerManagement(nv);
}

void NV_API_CALL rm_get_power_info(
    nvidia_stack_t  *sp,
    nv_state_t      *pNv,
    nv_power_info_t *powerInfo
)
{
    THREAD_STATE_NODE threadState;
    void              *fp;
    GPU_MASK          gpuMask;
    const char       *pVidmemPowerStatus = "?";
    const char       *pGc6Supported = "?";
    const char       *pGcoffSupported = "?";
    const char       *pDynamicPowerStatus = "?";
    const char       *pS0ixStatus = "?";
    const char       *pDbStatus = "?";

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER)) == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(pNv);

        // LOCK: acquire per device lock
        if ((pGpu != NULL) &&
           ((rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                   GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER,
                                   &gpuMask)) == NV_OK))
        {
            nv_priv_t *pNvp = NV_GET_NV_PRIV(pNv);

            pVidmemPowerStatus = pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERED) ?
                                    "Off" : "Active";
            pDynamicPowerStatus = RmGetDynamicPowerManagementStatus(pNvp);
            pGc6Supported = RmGetGpuGcxSupport(pGpu, NV_TRUE);
            pGcoffSupported = RmGetGpuGcxSupport(pGpu, NV_FALSE);
            pS0ixStatus = pNvp->s0ix_pm_enabled ? "Enabled" : "Disabled";
            pDbStatus = RmGetDynamicBoostSupport(pNv);
            // UNLOCK: release per device lock
            rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
        }

        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    powerInfo->vidmem_power_status = pVidmemPowerStatus;
    powerInfo->dynamic_power_status = pDynamicPowerStatus;
    powerInfo->gc6_support = pGc6Supported;
    powerInfo->gcoff_support = pGcoffSupported;
    powerInfo->s0ix_status = pS0ixStatus;
    powerInfo->db_support = pDbStatus;
}


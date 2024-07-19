/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include <nv-priv.h>
#include <osapi.h>
#include <core/thread_state.h>
#include <core/locks.h>
#include <gpu_mgr/gpu_mgr.h>
#include <gpu/gpu.h>
#include "kernel/gpu/intr/intr.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include <nv_sriov_defines.h>

static NvBool osInterruptPending(
    OBJGPU            *pGpu,
    NvBool            *serviced,
    THREAD_STATE_NODE *pThreadState
)
{
    OBJDISP *pDisp;
    KernelDisplay   *pKernelDisplay;
    NvBool pending, sema_release;
    THREAD_STATE_NODE threadState;
    NvU32 gpuMask, gpuInstance;
    Intr *pIntr = NULL;
    MC_ENGINE_BITVECTOR intr0Pending;
    MC_ENGINE_BITVECTOR intr1Pending;

    *serviced = NV_FALSE;
    pending = NV_FALSE;
    sema_release = NV_TRUE;
    OBJGPU *pDeviceLockGpu = pGpu;
    NvU8 stackAllocator[TLS_ISR_ALLOCATOR_SIZE]; // ISR allocations come from this buffer
    PORT_MEM_ALLOCATOR *pIsrAllocator;

    //
    // GPU interrupt servicing ("top half")
    //
    // Top-level processing of GPU interrupts is performed using the
    // steps below; although the code is straight forward, there
    // are a few points to be aware of:
    //
    //  1) The GPUs lock is acquired for two reasons: to allow
    //       looping over GPUs atomically in SLI and to sanity
    //       check the PCI configuration space of any initialized
    //       GPUs. If the acquisition fails, the early return
    //       is acceptable since GPU interrupts are disabled while
    //       the lock is held; note that returning success
    //       in this case could interfere with the processing
    //       of third-party device interrupts if the IRQ is shared.
    //       Due to the above, some interrupts may be reported as
    //       unhandled if invocations of the ISR registered with
    //       the kernel are not serialized. This is bad, but
    //       ignored by currently supported kernels, provided most
    //       interrupts are handled.
    //
    //  2) Since acquisition of the lock disables interrupts
    //       on all initialized GPUs, NV_PMC_INTR_EN_0 can not be
    //       relied up on to determine whether interrupts are
    //       expected from a given GPU. The code below is therefore
    //       forced to rely on software state. NV_PMC_INTR_EN_0
    //       is read only as a sanity check to guard against
    //       invalid GPU state (lack of PCI memory access, etc.).
    //
    //  3) High priority interrupts (VBLANK, etc.), are serviced in
    //       this function, service of all other interrupts is
    //       deferred until a bottom half. If a bottom half needs
    //       to be scheduled, release of the GPUs lock is
    //       likewise deferred until completion of the bottom half.
    //
    //  4) To reduce the risk of starvation, an effort is made to
    //       consolidate processing of interrupts pending on
    //       all GPUs sharing a given IRQ.
    //
    //  5) Care is taken to ensure that the consolidated interrupt
    //       processing is performed in the context of a GPU
    //       that has interrupts pending. Else if additional ISR
    //       processing via a bottom-half is required, this
    //       bottom-half ISR might race against the GPU's shut-down
    //       path.
    //

    pIsrAllocator = portMemAllocatorCreateOnExistingBlock(stackAllocator, sizeof(stackAllocator));
    tlsIsrInit(pIsrAllocator);

    //
    // Service nonstall interrupts before possibly acquiring GPUs lock
    // so that we don't unnecesarily hold the lock while servicing them.
    //
    // Also check if we need to acquire the GPU lock at all and get critical interrupts
    // This should not violate (1) from above since we are not servicing the GPUs in SLI,
    // only checking their state.
    // 
    // To do so, two steps are required:
    // Step 1: Check if we can service nonstall interrupts outside the GPUs lock. This is true
    // if the two PDBs are true. Otherwise we have to acquire the GPUs lock to service the nonstall
    // interrupts anyway, and we can't get around acquiring the GPUs lock.
    //
    // Step 2 is inline below
    //
    NvBool bIsAnyStallIntrPending = NV_TRUE;
    NvBool bIsAnyBottomHalfStallPending = NV_FALSE;
    NvU32  isDispPendingPerGpu    = 0;
    NvU32  isTmrPendingPerGpu     = 0;

    if (pDeviceLockGpu->getProperty(pDeviceLockGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED) &&
        pDeviceLockGpu->getProperty(pDeviceLockGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS))
    {
        threadStateInitISRLockless(&threadState, pDeviceLockGpu, THREAD_STATE_FLAGS_IS_ISR_LOCKLESS);
        bIsAnyStallIntrPending = NV_FALSE;

        gpuMask = gpumgrGetGpuMask(pDeviceLockGpu);
        gpuInstance = 0;
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
        {
            pIntr = GPU_GET_INTR(pGpu);
            if ((pIntr != NULL) && (INTERRUPT_TYPE_HARDWARE == intrGetIntrEn(pIntr)))
            {
                NvBool bCtxswLog = NV_FALSE;
                intrGetPendingNonStall_HAL(pGpu, pIntr, &intr1Pending, &threadState);
                intrCheckFecsEventbufferPending(pGpu, pIntr, &intr1Pending, &bCtxswLog);
                if (!bitVectorTestAllCleared(&intr1Pending))
                {
                    intrServiceNonStall_HAL(pGpu, pIntr, &intr1Pending, &threadState);
                    *serviced = NV_TRUE;
                }

                //
                // Step 2: Service nonstall interrupts and check if there are any stall interrupts
                // pending on any GPUs. If there are, then we will take the GPU lock.
                //
                // Bug 4223192: calling intrGetPendingStall_HAL is rather expensive,
                // so save off the critical interrupts to be handled in the top half.
                //
                intrGetPendingStall_HAL(pGpu, pIntr, &intr0Pending, &threadState);
                if (!bitVectorTestAllCleared(&intr0Pending))
                {
                    if (bitVectorTest(&intr0Pending, MC_ENGINE_IDX_DISP))
                    {
                        isDispPendingPerGpu |= NVBIT32(gpuGetInstance(pGpu));

                        // We will attempt to handle this separately from the other stall interrupts
                        bitVectorClr(&intr0Pending, MC_ENGINE_IDX_DISP);
                    }

                    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) &&
                        !IS_VIRTUAL(pGpu) && bitVectorTest(&intr0Pending, MC_ENGINE_IDX_TMR))
                    {
                        isTmrPendingPerGpu |= NVBIT32(gpuGetInstance(pGpu));

                        // We will clear this in the top half
                        bitVectorClr(&intr0Pending, MC_ENGINE_IDX_TMR);
                    }

                    if (!bitVectorTestAllCleared(&intr0Pending))
                    {
                        //
                        // If any other stall interrupts are pending,
                        // they will be handled in the bottom half.
                        //
                        bIsAnyBottomHalfStallPending = NV_TRUE;
                    }

                    bIsAnyStallIntrPending = NV_TRUE;
                }
            }
        }
        threadStateFreeISRLockless(&threadState, pDeviceLockGpu, THREAD_STATE_FLAGS_IS_ISR_LOCKLESS);
    }

    // LOCK: try to acquire GPUs lock
    if (bIsAnyStallIntrPending &&
        (rmDeviceGpuLocksAcquire(pDeviceLockGpu, GPU_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_ISR) == NV_OK))
    {
        threadStateInitISRAndDeferredIntHandler(&threadState,
            pDeviceLockGpu, THREAD_STATE_FLAGS_IS_ISR);

        gpuMask = gpumgrGetGpuMask(pDeviceLockGpu);

        gpuInstance = 0;
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
        {
            pIntr = GPU_GET_INTR(pGpu);
            pDisp = GPU_GET_DISP(pGpu);
            pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

            if ((pDisp != NULL) && pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
            {
            }
            else if ((pIntr != NULL) && INTERRUPT_TYPE_HARDWARE == intrGetIntrEn(pIntr))
            {
                // If interrupt enable is garbage the GPU is probably in a bad state
                if (intrGetIntrEnFromHw_HAL(pGpu, pIntr, &threadState) > INTERRUPT_TYPE_MAX)
                   continue;

                //
                // If lockless interrupt handling was enabled, we have already cached which critical
                // interrupts are pending. We only set the appropriate bits in the intr0Pending bitvector
                // for the subsequent code to service them in the same manner as the lockless nonstall
                // interrupt handling disabled case. But we also clear them from intr0Pending and
                // for disp, check if they're still pending afterwards. We already checked whether any
                // other bottom half stall interrupts are pending in bIsAnyBottomHalfStallPending above.
                //
                // After all this, the combination of bIsAnyBottomHalfStallPending and intr0Pending 
                // contains whether any stall interrupts are still pending, so check both to determine if
                // we need a bottom half.
                //
                if (pDeviceLockGpu->getProperty(pDeviceLockGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED) &&
                    pDeviceLockGpu->getProperty(pDeviceLockGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS))
                {
                    bitVectorClrAll(&intr0Pending);
                    if ((isDispPendingPerGpu & NVBIT32(gpuGetInstance(pGpu))) != 0)
                    {
                        bitVectorSet(&intr0Pending, MC_ENGINE_IDX_DISP);
                    }

                    if ((isTmrPendingPerGpu & NVBIT32(gpuGetInstance(pGpu))) != 0)
                    {
                        bitVectorSet(&intr0Pending, MC_ENGINE_IDX_TMR);
                    }
                }
                else
                {
                    intrGetPendingStall_HAL(pGpu, pIntr, &intr0Pending, &threadState);
                }

                if (bitVectorTest(&intr0Pending, MC_ENGINE_IDX_DISP))
                {
                    if (pKernelDisplay != NULL)
                    {
                        NvU32 head = 0;
                        NvU32 headIntrMask = 0;
                        MC_ENGINE_BITVECTOR intrDispPending;

                        kdispServiceVblank_HAL(pGpu, pKernelDisplay, 0,
                                               (VBLANK_STATE_PROCESS_LOW_LATENCY |
                                                VBLANK_STATE_PROCESS_CALLED_FROM_ISR),
                                               &threadState);

                        // handle RG line interrupt, if it is forwared from GSP.
                        if (IS_GSP_CLIENT(pGpu))
                        {
                            for (head = 0; head < kdispGetNumHeads(pKernelDisplay); ++head)
                            {
                                KernelHead *pKernelHead = KDISP_GET_HEAD(pKernelDisplay, head);

                                headIntrMask = kheadReadPendingRgLineIntr_HAL(pGpu, pKernelHead, &threadState);
                                if (headIntrMask != 0)
                                {
                                    NvU32 clearIntrMask = 0;

                                    kheadProcessRgLineCallbacks_HAL(pGpu,
                                                                    pKernelHead,
                                                                    head,
                                                                    &headIntrMask,
                                                                    &clearIntrMask,
                                                                    osIsISR());
                                    if (clearIntrMask != 0)
                                    {
                                        kheadResetRgLineIntrMask_HAL(pGpu, pKernelHead, clearIntrMask, &threadState);
                                    }
                                }
                            }
                        }

                        *serviced = NV_TRUE;
                        bitVectorClr(&intr0Pending, MC_ENGINE_IDX_DISP);

                        intrGetPendingDisplayIntr_HAL(pGpu, pIntr, &intrDispPending, &threadState);
                        bitVectorOr(&intr0Pending, &intr0Pending, &intrDispPending);
                    }
                }

                if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) &&
                    !IS_VIRTUAL(pGpu) && bitVectorTest(&intr0Pending, MC_ENGINE_IDX_TMR))
                {
                    // We have to clear the top level interrupt bit here since otherwise
                    // the bottom half will attempt to service the interrupt on the CPU
                    // side before GSP receives the notification and services it
                    intrClearLeafVector_HAL(pGpu, pIntr, intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_TMR, NV_FALSE), &threadState);
                    bitVectorClr(&intr0Pending, MC_ENGINE_IDX_TMR);

                    NV_ASSERT_OK(intrTriggerPrivDoorbell_HAL(pGpu, pIntr, NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_HANDLE));
                }

                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED) &&
                    !pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS))
                {
                    pIntr = GPU_GET_INTR(pGpu);
                    if (pIntr != NULL)
                    {
                        NvBool bCtxswLog = NV_FALSE;
                        intrGetPendingNonStall_HAL(pGpu, pIntr, &intr1Pending, &threadState);
                        intrCheckFecsEventbufferPending(pGpu, pIntr, &intr1Pending, &bCtxswLog);
                    }
                }
                else
                {
                    bitVectorClrAll(&intr1Pending);
                }

                if (bIsAnyBottomHalfStallPending ||
                    !bitVectorTestAllCleared(&intr0Pending) ||
                    !bitVectorTestAllCleared(&intr1Pending))
                {
                    pending = NV_TRUE;
                    sema_release = NV_FALSE;
                }
            }
        }
        threadStateFreeISRAndDeferredIntHandler(&threadState,
                pDeviceLockGpu, THREAD_STATE_FLAGS_IS_ISR);

        if (sema_release)
        {
            NV_ASSERT(!pending);

            // UNLOCK: release GPUs lock
            rmDeviceGpuLocksRelease(pDeviceLockGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }
        else
        {
            rmDeviceGpuLockSetOwner(pDeviceLockGpu, GPUS_LOCK_OWNER_PENDING_DPC_REFRESH);
        }
    }

    tlsIsrDestroy(pIsrAllocator);
    portMemAllocatorRelease(pIsrAllocator);

    return pending;
}

NV_STATUS osIsr(
    OBJGPU *pGpu
)
{
    NV_STATUS status = NV_OK;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NvBool pending = NV_FALSE;
    NvBool serviced = NV_FALSE;
    Intr *pIntr;

    if (nvp->flags & NV_INIT_FLAG_GPU_STATE_LOAD)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
        {
            pending = osInterruptPending(pGpu, &serviced, NULL /* threadstate */);
        }
        else
        {
            pIntr = GPU_GET_INTR(pGpu);
            if (INTERRUPT_TYPE_HARDWARE == intrGetIntrEn(pIntr))
            {
                KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
                pending = osInterruptPending(pGpu, &serviced, NULL /* threadstate */);
                kbifCheckAndRearmMSI(pGpu, pKernelBif);
            }
        }
    }

    if (!pending && (IS_VIRTUAL(pGpu) || !serviced))
        status = NV_ERR_NO_INTR_PENDING;
    else if (pending)
        status = NV_WARN_MORE_PROCESSING_REQUIRED;

    return status;
}

/*
 * Helper function to determine when the RM SEMA/GPUS LOCK should toggle
 * interrupts.  Based on the state of the GPU - we must add cases here as we
 * discover them.
 *
 * Noteworthy special cases:
 *
 *   - Suspend/resume: the GPU could still be suspended and not accessible
 *     on the bus, while passive-level threads need to grab the GPUs
 *     lock, or other GPUs are being resumed and triggering interrupts.
 *
 *   - SLI state transitions: interrupts are disabled manually prior to
 *     removing GPUs from the lock mask leading up to SLI link/unlink
 *     operations on UNIX, but since the GPUs lock is not held by design in
 *     these paths, it needs to be ensured that GPUs lock acquisitions
 *     occurring aynchronously do not re-enable interrupts on any of the
 *     GPUs undergoing the SLI state transition.
 *
 * @param[in] pGpu  OBJGPU pointer
 *
 * @return NV_TRUE if the RM SEMA/GPUS LOCK should toggle interrupts, NV_FALSE
 *         otherwise.
 */
NvBool osLockShouldToggleInterrupts(OBJGPU *pGpu)
{
    return (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH) &&
             gpuIsStateLoaded(pGpu) &&
            !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SLI_LINK_CODEPATH));
}

void osEnableInterrupts(OBJGPU *pGpu)
{
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        // enable irq through os call
        nv_control_soc_irqs(NV_GET_NV_STATE(pGpu), NV_TRUE);
        return;
    }
    else
    {
        Intr *pIntr = GPU_GET_INTR(pGpu);
        NvU32 intrEn;

        if (!pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
            NV_ASSERT(intrGetIntrEnFromHw_HAL(pGpu, pIntr, NULL) == INTERRUPT_TYPE_DISABLED);

        intrEn = intrGetIntrEn(pIntr);
        intrSetIntrEnInHw_HAL(pGpu, pIntr, intrEn, NULL);
        intrSetStall_HAL(pGpu, pIntr, intrEn, NULL);

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED))
        {
            intrRestoreNonStall_HAL(pGpu, pIntr, intrGetIntrEn(pIntr), NULL);
        }

    }
}

void osDisableInterrupts(
    OBJGPU *pGpu,
    NvBool  bIsr
)
{
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        // disable irq through os call
        nv_control_soc_irqs(NV_GET_NV_STATE(pGpu), NV_FALSE);
        return;
    }
    else
    {
        Intr *pIntr = GPU_GET_INTR(pGpu);
        NvU32 new_intr_en_0 = INTERRUPT_TYPE_DISABLED;

        intrSetIntrEnInHw_HAL(pGpu, pIntr, new_intr_en_0, NULL);

        intrSetStall_HAL(pGpu, pIntr, new_intr_en_0, NULL);

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED))
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS))
            {
                intrRestoreNonStall_HAL(pGpu, pIntr, intrGetIntrEn(pIntr), NULL);
            }
            else
            {
                intrRestoreNonStall_HAL(pGpu, pIntr, new_intr_en_0, NULL);
            }
        }
    }
}

static void RmIsrBottomHalf(
    nv_state_t *pNv
)
{
    OBJGPU    *pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    THREAD_STATE_NODE threadState;
    OS_THREAD_HANDLE threadId;
    NvU32 gpuMask, gpuInstance;
    OBJGPU *pDeviceLockGpu = pGpu;
    Intr *pIntr = NULL;
    OBJDISP *pDisp = NULL;
    NvU8 stackAllocator[TLS_ISR_ALLOCATOR_SIZE]; // ISR allocations come from this buffer
    PORT_MEM_ALLOCATOR *pIsrAllocator;

    pIsrAllocator = portMemAllocatorCreateOnExistingBlock(stackAllocator, sizeof(stackAllocator));
    tlsIsrInit(pIsrAllocator);

    //
    // The owning thread changes as the ISR acquires the GPUs lock,
    // but the bottom half releases it.  Refresh the ThreadId owner to be
    // correct here for the bottom half context.
    //
    osGetCurrentThread(&threadId);
    rmDeviceGpuLockSetOwner(pDeviceLockGpu, threadId);

    gpuMask = gpumgrGetGpuMask(pGpu);

    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {

        threadStateInitISRAndDeferredIntHandler(&threadState,
            pGpu, THREAD_STATE_FLAGS_IS_ISR_DEFERRED_INT_HANDLER);

        pIntr = GPU_GET_INTR(pGpu);
        pDisp = GPU_GET_DISP(pGpu);

        //
        // Call disp service incase of SOC Display,
        // TODO : with multi interrupt handling based on irq aux interrupts are serviced by dpAuxService
        // See JIRA task TDS-4253.
        //
        if ((pDisp != NULL) && pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
        {
        }
        else if ((pIntr != NULL) && (INTERRUPT_TYPE_HARDWARE == intrGetIntrEn(pIntr)))
        {
            intrServiceStall_HAL(pGpu, pIntr);

            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED) &&
                !pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS))
            {
                MC_ENGINE_BITVECTOR intrPending;
                intrServiceNonStall_HAL(pGpu, pIntr, &intrPending, &threadState);
            }
        }

        threadStateFreeISRAndDeferredIntHandler(&threadState,
            pGpu, THREAD_STATE_FLAGS_IS_ISR_DEFERRED_INT_HANDLER);
    }

    // UNLOCK: release GPUs lock
    rmDeviceGpuLocksRelease(pDeviceLockGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    tlsIsrDestroy(pIsrAllocator);
    portMemAllocatorRelease(pIsrAllocator);
}

static void RmIsrBottomHalfUnlocked(
    nv_state_t *pNv
)
{
    OBJGPU  *pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    Intr *pIntr;
    THREAD_STATE_NODE threadState;

    // In the GSP client scenario, the fatal fault interrupt is not shared
    // by UVM and CPU-RM.  Instead, it is handled entirely by GSP-RM.  We
    // therefore do not expect this function to be called.  But if it is, bail
    // without attempting to service interrupts.
    if (IS_GSP_CLIENT(pGpu))
    {
        return;
    }

    // Grab GPU lock here as this kthread-item was enqueued without grabbing GPU lock
    if (rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DPC) == NV_OK)
    {
        if (FULL_GPU_SANITY_CHECK(pGpu))
        {
            pIntr = GPU_GET_INTR(pGpu);

            threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

            if (intrGetIntrEn(pIntr) != INTERRUPT_TYPE_DISABLED)
            {
                MC_ENGINE_BITVECTOR intrPending;
                intrGetPendingStall_HAL(pGpu, pIntr, &intrPending, &threadState);
                intrServiceNonStallBottomHalf(pGpu, pIntr, &intrPending, &threadState);
            }

            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        }

        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
    }
}

NvBool NV_API_CALL rm_isr(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvU32      *NeedBottomHalf
)
{
    NV_STATUS status;
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    OBJGPU *pGpu;
    NvBool retval;
    void *fp;

    if ((nvp->flags & NV_INIT_FLAG_GPU_STATE_LOAD) == 0)
    {
        return NV_FALSE;
    }

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    if (pGpu == NULL)
    {
        return NV_FALSE;
    }

    NV_ENTER_RM_RUNTIME(sp,fp);

    // call actual isr function here
    status = isrWrapper(pGpu->testIntr, pGpu);

    switch (status)
    {
        case NV_OK:
            *NeedBottomHalf = NV_FALSE;
            retval = NV_TRUE;
            break;
        case NV_WARN_MORE_PROCESSING_REQUIRED:
            *NeedBottomHalf = NV_TRUE;
            retval = NV_TRUE;
            break;
        case NV_ERR_NO_INTR_PENDING:
        default:
            *NeedBottomHalf = NV_FALSE;
            retval = NV_FALSE;
            break;
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return retval;
}

void NV_API_CALL rm_isr_bh(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    void    *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    RmIsrBottomHalf(pNv);

    NV_EXIT_RM_RUNTIME(sp,fp);
}

void NV_API_CALL rm_isr_bh_unlocked(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    void    *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    RmIsrBottomHalfUnlocked(pNv);

    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL rm_gpu_copy_mmu_faults(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvU32 *faultsCopied
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu;
    void   *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    if (pGpu == NULL || faultsCopied == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        // Non-replayable faults are copied to the client shadow buffer by GSP-RM.
        status = NV_OK;
        goto done;
    }
    else
    {
        KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        PORT_MEM_ALLOCATOR *pIsrAllocator;
        THREAD_STATE_NODE   threadState;
        NvU8 stackAllocator[TLS_ISR_ALLOCATOR_SIZE]; // ISR allocations come from this buffer

        if (pKernelGmmu == NULL)
        {
            status = NV_ERR_OBJECT_NOT_FOUND;
            goto done;
        }

        // If MMU fault buffer is not enabled, return early
        if (!gpuIsVoltaHubIntrSupported(pGpu))
            goto done;

        pIsrAllocator = portMemAllocatorCreateOnExistingBlock(stackAllocator, sizeof(stackAllocator));
        tlsIsrInit(pIsrAllocator);
        threadStateInitISRAndDeferredIntHandler(&threadState, pGpu, THREAD_STATE_FLAGS_IS_ISR);

        // Copies all valid packets in RM's and client's shadow buffer
        status  = kgmmuCopyMmuFaults_HAL(pGpu, pKernelGmmu, &threadState, faultsCopied,
                                         NON_REPLAYABLE_FAULT_BUFFER, NV_FALSE);

        threadStateFreeISRAndDeferredIntHandler(&threadState, pGpu, THREAD_STATE_FLAGS_IS_ISR);
        tlsIsrDestroy(pIsrAllocator);
        portMemAllocatorRelease(pIsrAllocator);

    }

done:
    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

//
// Use this call when MMU faults needs to be copied
// outisde of RM lock.
//
static NV_STATUS _rm_gpu_copy_mmu_faults_unlocked(
    OBJGPU *pGpu,
    NvU32 *pFaultsCopied,
    THREAD_STATE_NODE *pThreadState
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    if (pKernelGmmu == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    // If MMU fault buffer is not enabled, return early
    if (!gpuIsVoltaHubIntrSupported(pGpu))
        return NV_OK;

    // Copies all valid packets in RM's and client's shadow buffer
    return kgmmuCopyMmuFaults_HAL(pGpu, pKernelGmmu, pThreadState, pFaultsCopied,
                                  NON_REPLAYABLE_FAULT_BUFFER, NV_FALSE);

    return NV_OK;
}

//
// Wrapper to handle calls to copy mmu faults
//
NV_STATUS rm_gpu_handle_mmu_faults(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvU32 *faultsCopied
)
{
    NvU32 status = NV_OK;
    OBJGPU *pGpu;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    *faultsCopied = 0;
    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    
    if (pGpu == NULL)
    {
        NV_EXIT_RM_RUNTIME(sp,fp);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    {
        KernelGmmu *pKernelGmmu;
        PORT_MEM_ALLOCATOR *pIsrAllocator;
        THREAD_STATE_NODE threadState;
        NvU8 stackAllocator[TLS_ISR_ALLOCATOR_SIZE]; // ISR allocations come from this buffer

        pIsrAllocator = portMemAllocatorCreateOnExistingBlock(stackAllocator, sizeof(stackAllocator));
        tlsIsrInit(pIsrAllocator);
        threadStateInitISRLockless(&threadState, pGpu, THREAD_STATE_FLAGS_IS_ISR_LOCKLESS);

        pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && !IS_VIRTUAL(pGpu))
        {
            Intr *pIntr = GPU_GET_INTR(pGpu);

            if (kgmmuIsNonReplayableFaultPending_HAL(pGpu, pKernelGmmu, &threadState))
            {
                // We have to clear the top level interrupt bit here since otherwise
                // the bottom half will attempt to service the interrupt on the CPU
                // side before GSP receives the notification and services it
                kgmmuClearNonReplayableFaultIntr_HAL(pGpu, pKernelGmmu, &threadState);
                status = intrTriggerPrivDoorbell_HAL(pGpu, pIntr, NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_HANDLE);

            }
        }
        else if (IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            if (kgmmuIsNonReplayableFaultPending_HAL(pGpu, pKernelGmmu, &threadState))
            {
                status = _rm_gpu_copy_mmu_faults_unlocked(pGpu, faultsCopied, &threadState);
            }
        }
        else
        {
            if (IS_GSP_CLIENT(pGpu))
            {
                // Non-replayable faults are copied to the client shadow buffer by GSP-RM.
                status = NV_OK;
            }
            else
            {
                status = _rm_gpu_copy_mmu_faults_unlocked(pGpu, faultsCopied, &threadState);
            }
        }

        threadStateFreeISRLockless(&threadState, pGpu, THREAD_STATE_FLAGS_IS_ISR_LOCKLESS);
        tlsIsrDestroy(pIsrAllocator);
        portMemAllocatorRelease(pIsrAllocator);
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
    return status;
}

NvBool NV_API_CALL rm_is_msix_allowed(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
    nv_priv_t           *pNvp = NV_GET_NV_PRIV(nv);
    THREAD_STATE_NODE    threadState;
    void                *fp;
    NvBool               ret = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT) == NV_OK)
    {
        ret = gpumgrIsDeviceMsixAllowed(nv->regs->cpu_address,
                                        pNvp->pmc_boot_1, pNvp->pmc_boot_42);
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return ret;
}

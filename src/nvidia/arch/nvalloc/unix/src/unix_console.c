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

#include <core/locks.h>
#include <ctrl/ctrl0080/ctrl0080unix.h>
#include <gpu/device/device.h>
#include <gpu/gpu.h>
#include <nv-priv.h>
#include <nv.h>
#include <osapi.h>
#include <gpu/mem_mgr/mem_mgr.h>
#include <platform/sli/sli.h>

#include <vgpu/rpc.h>
#include "vgpu/vgpu_events.h"

static NV_STATUS
unixCallVideoBIOS
(
    OBJGPU *pGpu,
    NvU32  *eax,
    NvU32  *ebx
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (NVCPU_IS_X86_64)
    {
        NvU32         eax_in = *eax;
        NvU32         ebx_in = *ebx;

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_UEFI))
        {
            return NV_ERR_NOT_SUPPORTED;
        }

        NV_PRINTF(LEVEL_INFO, "unixCallVideoBIOS: 0x%x 0x%x, vga_satus = %d\n", *eax, *ebx, NV_PRIMARY_VGA(NV_GET_NV_STATE(pGpu)));

        status = nv_vbios_call(pGpu, eax, ebx);

        // this was originally changed for nt in changelist 644223
        if (*eax != 0x4f)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "int10h(%04x, %04x) vesa call failed! (%04x, %04x)\n",
                      eax_in, ebx_in, *eax, *ebx);
            status = NV_ERR_GENERIC;
        }
    }

    return status;
}

static void
RmSaveDisplayState
(
    OBJGPU *pGpu
)
{
    nv_state_t     *nv             = NV_GET_NV_STATE(pGpu);
    nv_priv_t      *nvp            = NV_GET_NV_PRIV(nv);
    RM_API         *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvBool          use_vbios      = NV_PRIMARY_VGA(nv) && RmGpuHasIOSpaceEnabled(nv);
    NvU32           eax, ebx;
    NV_STATUS       status;
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS  preUnixConsoleParams = {0};
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS  postUnixConsoleParams = {0};

    if (IS_VIRTUAL(pGpu) || (pKernelDisplay == NULL) || nv->client_managed_console)
    {
        return;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_UEFI))
    {
        NV_PRINTF(LEVEL_INFO, "RM fallback doesn't support saving of efifb console\n");
        return;
    }

    os_disable_console_access();

    preUnixConsoleParams.bSave     = NV_TRUE;
    preUnixConsoleParams.bUseVbios = use_vbios;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, pRmApi->Control(pRmApi, nv->rmapi.hClient, nv->rmapi.hSubDevice,
                        NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE,
                        &preUnixConsoleParams, sizeof(preUnixConsoleParams)), done);

    if (preUnixConsoleParams.bReturnEarly)
    {
        goto done;
    }

    if (use_vbios)
    {
        //
        // Attempt to identify the currently set VESA mode; assume
        // vanilla VGA text if the VBIOS call fails.
        //
        eax = 0x4f03;
        ebx = 0;
        if (NV_OK == unixCallVideoBIOS(pGpu, &eax, &ebx))
        {
            nvp->vga.vesaMode = (ebx & 0x3fff);
        }
        else
        {
            nvp->vga.vesaMode = 3;
        }
    }

    postUnixConsoleParams.bSave     = NV_TRUE;
    postUnixConsoleParams.bUseVbios = use_vbios;

    NV_CHECK_OK(status, LEVEL_ERROR,
                pRmApi->Control(pRmApi, nv->rmapi.hClient,nv->rmapi.hSubDevice,
                                NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE,
                                &postUnixConsoleParams, sizeof(postUnixConsoleParams)));

done:
    os_enable_console_access();
}

static void RmRestoreDisplayState
(
    OBJGPU *pGpu
)
{
    nv_state_t     *nv             = NV_GET_NV_STATE(pGpu);
    nv_priv_t      *nvp            = NV_GET_NV_PRIV(nv);
    RM_API         *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool          use_vbios      = NV_PRIMARY_VGA(nv) && RmGpuHasIOSpaceEnabled(nv);;
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NV_STATUS       status;
    NvU32           eax, ebx;
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS  preUnixConsoleParams   = {0};
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS  postUnixConsoleParams = {0};

    //
    // Since vGPU does all real hardware management in the host,
    // there is nothing to do at this point in the guest OS.
    //
    if (IS_VIRTUAL(pGpu) || (pKernelDisplay == NULL) || nv->client_managed_console)
    {
        return;
    }

    //
    // Fix up DCB index VBIOS scratch registers.
    // The strategies employed are:
    //
    // SBIOS/VBIOS:
    // Clear the DCB index, and set the previous DCB index to the original
    // value. This allows the VBIOS (during the int10h mode-set) to
    // determine which display to enable, and to set the head-enabled bit
    // as needed (see bugs #264873 and #944398).
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_UEFI))
    {
        NV_PRINTF(LEVEL_INFO, "RM fallback doesn't support efifb console restore\n");
        return;
    }

    os_disable_console_access();

    preUnixConsoleParams.bUseVbios = use_vbios;
    preUnixConsoleParams.bSave = NV_FALSE;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, pRmApi->Control(pRmApi, nv->rmapi.hClient,
                        nv->rmapi.hSubDevice,
                        NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE,
                        &preUnixConsoleParams, sizeof(preUnixConsoleParams)), done);

    if (preUnixConsoleParams.bReturnEarly)
    {
        goto done;
    }

    if (use_vbios)
    {
        eax = 0x4f02;
        ebx = nvp->vga.vesaMode;

        if (NV_OK == unixCallVideoBIOS(pGpu, &eax, &ebx))
        {
            postUnixConsoleParams.bVbiosCallSuccessful = NV_TRUE;
        }
    }

    postUnixConsoleParams.bSave = NV_FALSE;
    postUnixConsoleParams.bUseVbios = use_vbios;

    NV_CHECK_OK(status, LEVEL_ERROR,
                pRmApi->Control(pRmApi, nv->rmapi.hClient, nv->rmapi.hSubDevice,
                                NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE,
                                &postUnixConsoleParams, sizeof(postUnixConsoleParams)));

done:
    os_enable_console_access();
}

static void
RmChangeResMode
(
    OBJGPU *pGpu,
    NvBool  hires
)
{
    if (hires)
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)

        RmSaveDisplayState(pGpu);

        SLI_LOOP_END
    } 
    else
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)

        RmRestoreDisplayState(pGpu);
        //
        // vGPU:
        //
        // Since vGPU does all real hardware management in the host, if we
        // are in guest OS (where IS_VIRTUAL(pGpu) is true), do an RPC to
        // the host to trigger switch from HIRES to (LORES)VGA.
        //
        if (IS_VIRTUAL(pGpu))
        {
            NV_STATUS status = NV_OK;
            NV_RM_RPC_SWITCH_TO_VGA(pGpu, status);
        }

        SLI_LOOP_END
    }
}

NV_STATUS NV_API_CALL
rm_save_low_res_mode
(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    RmSaveDisplayState(pGpu);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return NV_OK;
}

NV_STATUS
deviceCtrlCmdOsUnixVTSwitch_IMPL
(
    Device *pDevice,
    NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *pParams
)
{
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pDevice);
    nv_state_t *nv   = NV_GET_NV_STATE(pGpu);
    NvBool      hires;
    NvBool      bChangeResMode = NV_TRUE;

    switch (pParams->cmd)
    {
        case NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_SAVE_VT_STATE:
            hires = NV_TRUE;
            break;

        case NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_RESTORE_VT_STATE:
            hires = NV_FALSE;
            break;

        case NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_CONSOLE_RESTORED:
            bChangeResMode = NV_FALSE;
            break;

        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    RmUpdateGc6ConsoleRefCount(nv,
                               pParams->cmd != NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_SAVE_VT_STATE);

    if (!bChangeResMode)
    {
        return NV_OK;
    }

    if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FB) == NV_OK)
    {
        RmChangeResMode(pGpu, hires);
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,"%s: Failed to acquire GPU lock",  __FUNCTION__);
    }
    return NV_OK;
}

NV_STATUS deviceCtrlCmdOsUnixVTGetFBInfo_IMPL
(
    Device *pDevice,
    NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *pParams
)
{
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pDevice);
    NvBool bContinue = NV_TRUE;

    if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FB) == NV_OK)
    {
        // See if the console is on one of the subdevices of this device.
        portMemSet(pParams, 0, sizeof(*pParams));

        SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)

            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            nv_state_t *nv = NV_GET_NV_STATE(pGpu);

            if ((memmgrGetReservedConsoleMemDesc(pGpu, pMemoryManager) != NULL) && bContinue)
            {
                NvU64 baseAddr, size;
                NvU32 width, height, depth, pitch;

                // There should only be one.
                NV_ASSERT(pParams->width == 0);

                pParams->subDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

                // Console is either mapped to BAR1 or BAR2 + 16 MB
                nv_get_screen_info(nv, &baseAddr, &width, &height, &depth,
                                   &pitch, &size);

                pParams->baseAddress = baseAddr;
                pParams->size = size;
                pParams->width = (NvU16)width;
                pParams->height = (NvU16)height;
                pParams->depth = (NvU16)depth;
                pParams->pitch = (NvU16)pitch;

                if (baseAddr != 0)
                {
                    bContinue = NV_FALSE; 
                }
            }

        SLI_LOOP_END

        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,"%s: Failed to acquire GPU lock",  __FUNCTION__);
    }

    return NV_OK;
}


void 
dispdeviceFillVgaSavedDisplayState
(
    OBJGPU *pGpu,
    NvU64   vgaAddr,
    NvU8    vgaMemType,
    NvBool  vgaValid,
    NvU64   workspaceAddr,
    NvU8    workspaceMemType,
    NvBool  workspaceValid,
    NvBool  baseValid,
    NvBool  workspaceBaseValid
)
{
    nv_state_t *nv        = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp       = NV_GET_NV_PRIV(nv);
    NvBool      use_vbios = NV_PRIMARY_VGA(nv) && RmGpuHasIOSpaceEnabled(nv);

    if (use_vbios)
    {
        nvp->vga.base.addr = vgaAddr;
        nvp->vga.base.memTarget = vgaMemType;
        nvp->vga.base.valid = vgaValid;
        nvp->vga.baseValid = baseValid;

        nvp->vga.workspaceBase.addr = workspaceAddr;
        nvp->vga.workspaceBase.memTarget = workspaceMemType;
        nvp->vga.workspaceBase.valid = workspaceValid;
        nvp->vga.workspaceBaseValid = workspaceBaseValid;
    }
}


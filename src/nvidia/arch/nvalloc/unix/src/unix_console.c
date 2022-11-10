/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
                NvU64 baseAddr;

                // There should only be one.
                NV_ASSERT(pParams->width == 0);

                pParams->subDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

                // Console is either mapped to BAR1 or BAR2 + 16 MB
                os_get_screen_info(&baseAddr, &pParams->width,
                                   &pParams->height, &pParams->depth,
                                   &pParams->pitch,
                                   nv->bars[NV_GPU_BAR_INDEX_FB].cpu_address,
                                   nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address + 0x1000000);

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


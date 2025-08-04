/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                           *
*         Implementation specific Descriptor List management functions      *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/eng_desc.h"
#include "gpu/gpu_child_class_defs.h"
#include <published/nv_arch.h>

#include "swref/published/t23x/t234/dev_fuse.h"

#include "class/cl0080.h"
#include "class/cl2080.h"

#include "ctrl/ctrl0080/ctrl0080dma.h"
#include "ctrl/ctrl0080/ctrl0080fb.h"
#include "ctrl/ctrl0080/ctrl0080gpu.h"
#include "ctrl/ctrl0080/ctrl0080unix.h"

#include "ctrl/ctrl2080/ctrl2080bus.h"
#include "ctrl/ctrl2080/ctrl2080event.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "ctrl/ctrl2080/ctrl2080unix.h"

/*!
 * @brief fill in the GPU ID information
 */
void
gpuGetIdInfo_T234D
(
    OBJGPU   *pGpu
)
{
    pGpu->chipInfo.platformId       = GPU_ARCHITECTURE_T23X;
    pGpu->chipInfo.implementationId = GPU_IMPLEMENTATION_T234D;
    pGpu->chipInfo.revisionId = 0;
}

// See gpuChildOrderList_GM200 for documentation
static const GPUCHILDORDER
gpuChildOrderList_T234D[] =
{
    {classId(OBJDCECLIENTRM),       GCO_ALL},
    {classId(MemorySystem),         GCO_ALL},
    {classId(KernelMemorySystem),   GCO_ALL},
    {classId(MemoryManager),        GCO_ALL},
    {classId(OBJDCB),               GCO_ALL},
    {classId(OBJDISP),              GCO_ALL},
    {classId(KernelDisplay),        GCO_ALL},
    {classId(OBJDPAUX),             GCO_ALL},
    {classId(I2c),                  GCO_ALL},
    {classId(OBJGPIO),              GCO_ALL},
    {classId(OBJHDACODEC),          GCO_ALL},
};

// See gpuChildrenPresent_GM200 for documentation on GPUCHILDPRESENT
static const GPUCHILDPRESENT gpuChildrenPresent_T234D[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJDCECLIENTRM, 1),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
};


const GPUCHILDORDER *
gpuGetChildrenOrder_T234D(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildOrderList_T234D);
    return gpuChildOrderList_T234D;
}

const GPUCHILDPRESENT *
gpuGetChildrenPresent_T234D(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_T234D);
    return gpuChildrenPresent_T234D;
}

// list of valid NV01_DEVICE (0x0080) rmctrl commands for SOC Display GPUs
static const NvU32
gpuDeviceRmctrlAllowlist_T234D[] =
{
    NV0080_CTRL_CMD_GPU_GET_CLASSLIST,
    NV0080_CTRL_CMD_GPU_GET_NUM_SUBDEVICES,
    NV0080_CTRL_CMD_GPU_GET_CLASSLIST_V2,
    NV0080_CTRL_CMD_FB_GET_CAPS_V2,
    NV0080_CTRL_CMD_DMA_GET_CAPS,
    NV0080_CTRL_CMD_OS_UNIX_VT_SWITCH,
    NV0080_CTRL_CMD_OS_UNIX_VT_GET_FB_INFO,
};

// list of valid NV20_SUBDEVICE (0x2080) rmctrl commands for SOC Display GPUs
static const NvU32
gpuSubdeviceRmctrlAllowlist_T234D[] =
{
    NV2080_CTRL_CMD_GPU_GET_INFO_V2,
    NV2080_CTRL_CMD_GPU_GET_NAME_STRING,
    NV2080_CTRL_CMD_GPU_GET_SHORT_NAME_STRING,
    NV2080_CTRL_CMD_GPU_GET_SIMULATION_INFO,
    NV2080_CTRL_CMD_GPU_GET_ID,
    NV2080_CTRL_CMD_GPU_GET_GID_INFO,
    NV2080_CTRL_CMD_GPU_GET_ENGINES_V2,
    NV2080_CTRL_CMD_GPU_GET_CONSTRUCTED_FALCON_INFO,
    NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_STATIC_INFO,
    NV2080_CTRL_CMD_INTERNAL_GPU_GET_CHIP_INFO,
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_IP_VERSION,
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_WRITE_INST_MEM,
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_SET_IMP_INIT_INFO,
    NV2080_CTRL_CMD_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER,
    NV2080_CTRL_CMD_FB_GET_INFO_V2,
    NV2080_CTRL_CMD_BUS_GET_INFO_V2,
    NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT,
    NV2080_CTRL_CMD_OS_UNIX_ALLOW_DISALLOW_GCOFF,
    NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER,
};

static NV_STATUS
gpuCheckRmctrlAllowList(const NvU32 *pAllowList, NvU32 allowListSize, NvU32 cmd)
{
    NvU32 i;
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    for (i = 0; i < allowListSize; i++)
    {
        if (pAllowList[i] == cmd)
        {
            status = NV_OK;
            break;
        }
    }

    return status;
}

NV_STATUS
gpuValidateRmctrlCmd_T234D(OBJGPU *pGpu, NvU32 cmd)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY),
                        NV_ERR_INVALID_STATE);

    // only validate device/subdevice rmctrls currently
    switch (DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd))
    {
        case NV01_DEVICE_0:
            status = gpuCheckRmctrlAllowList(gpuDeviceRmctrlAllowlist_T234D,
                                             NV_ARRAY_ELEMENTS(gpuDeviceRmctrlAllowlist_T234D),
                                             cmd);
            break;
        case NV20_SUBDEVICE_0:
            status = gpuCheckRmctrlAllowList(gpuSubdeviceRmctrlAllowlist_T234D,
                                             NV_ARRAY_ELEMENTS(gpuSubdeviceRmctrlAllowlist_T234D),
                                             cmd);
            break;
        default:
            status = NV_OK;
            break;
    }

    return status;
}

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/gpu_device_mapping.h"
#include "core/thread_state.h"
#include "nv_ref.h"

/**
 * @brief Finds the device mapping matching the specified address and device index
 *
 * @param[in] pGpu
 * @param[in] deviceIndex device specific device enum (DEVICE_INDEX_*)
 * @param[in] addr        device register address
 *
 * @returns matching mapping, or NULL if not found.
 */
static DEVICE_MAPPING *
_gpuFindDeviceMapping
(
    OBJGPU *pGpu,
    DEVICE_INDEX deviceIndex,
    NvU32   instance
)
{
    NvU32 i;
    NvU32 devId = 0;
    DEVICE_ID_MAPPING *deviceIdMapping;
    NvU32 numDeviceIDs;

    numDeviceIDs = gpuGetDeviceIDList_HAL(pGpu, &deviceIdMapping);

    // Find the devID that matches the requested device index
    for (i = 0; i < numDeviceIDs; i++)
    {
        if (deviceIdMapping[i].deviceIndex == deviceIndex)
        {
            devId = deviceIdMapping[i].devId;
            break;
        }
    }

    if (devId == 0)
    {
        // For discrete GPU, just return BAR0 mapping
        if (deviceIndex == DEVICE_INDEX_GPU)
        {
            return &pGpu->deviceMappings[0];
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not find mapping for deviceIndex=%d\n",
                      deviceIndex);
            return NULL;
        }
    }
    return gpuGetDeviceMappingFromDeviceID(pGpu, devId, instance);
}

DEVICE_MAPPING *
gpuGetDeviceMapping_IMPL
(
    OBJGPU *pGpu,
    DEVICE_INDEX deviceIndex,
    NvU32   instance
)
{
    // Fast lookup path for first instance of a device
    if ((deviceIndex < DEVICE_INDEX_MAX) && (instance == 0))
    {
        if (!pGpu->pDeviceMappingsByDeviceInstance[deviceIndex])
        {
            pGpu->pDeviceMappingsByDeviceInstance[deviceIndex] = _gpuFindDeviceMapping(pGpu, deviceIndex, instance);
        }
        return pGpu->pDeviceMappingsByDeviceInstance[deviceIndex];
    }

    return _gpuFindDeviceMapping(pGpu, deviceIndex, instance);
}

/**
 * @brief Returns the device mapping matching the specified device ID from
 * project relocation table
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] deviceId    device ID from project relocation table
 * @param[in] instance    instance of the particular device ID
 *
 * @returns matching mapping, or NULL if not found.
 */

DEVICE_MAPPING *
gpuGetDeviceMappingFromDeviceID_IMPL
(
    OBJGPU *pGpu,
    NvU32 deviceId,
    NvU32 instance
)
{
    NvU32 i;

    //
    // For SOC, walk the list of devices to find the device/instance requested.
    // For GPU (legacy), only NV_DEVID_GPU(0) is expected & allowed
    //
    if (pGpu->bIsSOC)
    {
        for (i = 0; i < pGpu->gpuDeviceMapCount; i++)
        {
            if (pGpu->deviceMappings[i].gpuDeviceEnum == deviceId)
            {
                // Find the Nth instance of the requested device
                if (instance)
                    instance--;
                else
                    return &pGpu->deviceMappings[i];
            }
        }

        NV_PRINTF(LEVEL_ERROR, "Could not find mapping for deviceId=%d\n",
                  deviceId);
    }
    else
    {
        // For GPU, always assume NV_DEVID_GPU instance 0.
        NV_ASSERT(instance == 0);
        NV_ASSERT(pGpu->gpuDeviceMapCount == 1);

        return &pGpu->deviceMappings[0];
    }

    return NULL;
}

static NvBool _gpuCheckIsBar0OffByN(OBJGPU *pGpu)
{
    NvU32 i, pmcBoot0;

    // Check to see if we can find PMC_BOOT_0
    for (i = 0; i < 20; i++)
    {
        pmcBoot0 = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0 + (i * 4));
        if (pmcBoot0 == pGpu->chipId0)
        {
            break;
        }
    }

    if ((i != 0) && (i != 20))
    {
        // We are off by N
        return NV_TRUE;
    }

    // Everything looks ok
    return NV_FALSE;
}

static NvBool _gpuCheckDoesPciSpaceMatch(OBJGPU *pGpu)
{
    NvU16 VendorId;
    NvU16 DeviceId;
    NvU8  bus = gpuGetBus(pGpu);
    NvU8  device = gpuGetDevice(pGpu);
    NvU32 domain = gpuGetDomain(pGpu);

    osPciInitHandle(domain, bus, device, 0, &VendorId, &DeviceId);
    if ((DeviceId == 0xFFFF) ||
        (VendorId != 0x10DE))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

static NvBool _gpuCheckIsPciMemSpaceEnabled(OBJGPU *pGpu)
{
    NvU16 VendorId;
    NvU16 DeviceId;
    NvU8  bus = gpuGetBus(pGpu);
    NvU8  device = gpuGetDevice(pGpu);
    NvU32 domain = gpuGetDomain(pGpu);
    void *Handle = osPciInitHandle(domain, bus, device, 0, &VendorId, &DeviceId);
    NvU32 Enabled = osPciReadDword(Handle, NV_CONFIG_PCI_NV_1);

    // Is Memory Spaced Enabled
    if (DRF_VAL(_CONFIG, _PCI_NV_1, _MEMORY_SPACE, Enabled) != NV_CONFIG_PCI_NV_1_MEMORY_SPACE_ENABLED)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

NV_STATUS gpuSanityCheck_IMPL
(
    OBJGPU *pGpu,
    NvU32 flags,
    NvU32 *pFlagsFailed
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 flagsFailed = GPU_SANITY_CHECK_FLAGS_NONE;
    THREAD_STATE_NODE *pThreadNode = NULL;

    if (pFlagsFailed != NULL)
    {
        *pFlagsFailed = GPU_SANITY_CHECK_FLAGS_NONE;
    }

    if (pGpu->bIsSOC)
    {
        flags &= ~(
                  GPU_SANITY_CHECK_FLAGS_BOOT_0                  |
                  GPU_SANITY_CHECK_FLAGS_OFF_BY_N                |
                  GPU_SANITY_CHECK_FLAGS_PCI_SPACE_MATCH         |
                  GPU_SANITY_CHECK_FLAGS_PCI_MEM_SPACE_ENABLED   |
                  GPU_SANITY_CHECK_FLAGS_FB);

    }

    //
    // Check to make sure the lock is held for this thread as the underlying
    // functions can touch state and lists that expect exclusive access.
    //
    rmStatus = threadStateGetCurrent(&pThreadNode, pGpu);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }
    if (pThreadNode->flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Check to make sure we are powered on first
    if (gpuIsGpuFullPower(pGpu) == NV_FALSE)
    {
        NV_ASSERT(0);
        return NV_ERR_GPU_NOT_FULL_POWER;
    }

    if (flags & GPU_SANITY_CHECK_FLAGS_BOOT_0)
    {
        //
        // When GPU is in reset reg reads will return 0xFFFFFFFF.
        // Without this check RM would keep hitting assert during TDR recovery.
        //
        if (!API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        {
            NvU32 pmcBoot0 = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);
            if (pmcBoot0 != pGpu->chipId0)
            {
                flagsFailed |= GPU_SANITY_CHECK_FLAGS_BOOT_0;
                NV_ASSERT(0);
            }
        }
    }

    if (flags & GPU_SANITY_CHECK_FLAGS_OFF_BY_N)
    {
        if (_gpuCheckIsBar0OffByN(pGpu))
        {
            flagsFailed |= GPU_SANITY_CHECK_FLAGS_OFF_BY_N;
            NV_ASSERT(0);
        }
    }

    if (flags & GPU_SANITY_CHECK_FLAGS_PCI_SPACE_MATCH)
    {
        if (!_gpuCheckDoesPciSpaceMatch(pGpu))
        {
            flagsFailed |= GPU_SANITY_CHECK_FLAGS_PCI_SPACE_MATCH;
            NV_ASSERT(0);
        }
    }

    if (flags & GPU_SANITY_CHECK_FLAGS_PCI_MEM_SPACE_ENABLED)
    {
        if (!_gpuCheckIsPciMemSpaceEnabled(pGpu))
        {
            flagsFailed |= GPU_SANITY_CHECK_FLAGS_PCI_MEM_SPACE_ENABLED;
            NV_ASSERT(0);
        }
    }

    if (flags & GPU_SANITY_CHECK_FLAGS_FB)
    {
        if (!gpuIsGpuFullPower(pGpu))
        {
            NV_ASSERT(0);
        }
    }

    if (flagsFailed != GPU_SANITY_CHECK_FLAGS_NONE)
    {
        rmStatus = NV_ERR_GENERIC;
        NV_PRINTF(LEVEL_ERROR, "Failed test flags: 0x%x\n", flagsFailed);
    }

    if (pFlagsFailed != NULL)
    {
        *pFlagsFailed = flagsFailed;
    }

    return rmStatus;
}

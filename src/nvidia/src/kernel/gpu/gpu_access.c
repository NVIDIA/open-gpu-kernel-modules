/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/diagnostics/journal.h"

#include "core/thread_state.h"
#include "platform/sli/sli.h"
#include "nv_ref.h"

// Following enums are duplicated in 'apps/nvbucket/oca/ocarm.h'.
typedef enum {
    BAD_READ_GPU_OFF_BUS = 1,
    BAD_READ_LOW_POWER,
    BAD_READ_PCI_DEVICE_DISABLED,
    BAD_READ_GPU_RESET,
    BAD_READ_DWORD_SHIFT,
    BAD_READ_UNKNOWN,
} RMCD_BAD_READ_REASON;

static void   _gpuCleanRegisterFilterList(DEVICE_REGFILTER_INFO *);
static NvU32  _gpuHandleReadRegisterFilter(OBJGPU *, DEVICE_INDEX devIndex, NvU32 devInstance, NvU32 addr, NvU32 accessSize, NvU32 *pFlags, THREAD_STATE_NODE *pThreadState);
static void   _gpuHandleWriteRegisterFilter(OBJGPU *, DEVICE_INDEX devIndex, NvU32 devInstance, NvU32 addr, NvU32 val, NvU32 accessSize, NvU32 *pFlags, THREAD_STATE_NODE *pThreadState);

static void   ioaprtWriteRegUnicast(OBJGPU *, IoAperture *pAperture, NvU32 addr, NvV32 val, NvU32 size);
static NvU32  ioaprtReadReg(IoAperture *pAperture, NvU32 addr, NvU32 size);

static REGISTER_FILTER * _findGpuRegisterFilter(DEVICE_INDEX devIndex, NvU32 devInstance, NvU32 addr, REGISTER_FILTER *);
static NV_STATUS _gpuInitIOAperture(OBJGPU *pGpu, NvU32 deviceIndex, DEVICE_MAPPING *pMapping);

NV_STATUS
regAccessConstruct
(
    RegisterAccess *pRegisterAccess,
    OBJGPU *pGpu
)
{
    NV_STATUS    rmStatus = NV_OK;
    DEVICE_INDEX deviceIndex, minDeviceIndex, maxDeviceIndex;

    pRegisterAccess->pGpu = pGpu;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        // DEVICE_INDEX_GPU aperture is of GPU, as Tegra SOC NvDisplay constructs
        // display device IO aperture as part of objdisp construction so its safe to
        // skip this function.
        return NV_OK;
    }

    // Check that GPU is the first device
    ct_assert(DEVICE_INDEX_GPU == 0);

    minDeviceIndex = DEVICE_INDEX_GPU;
    maxDeviceIndex = pGpu->bIsSOC ? (DEVICE_INDEX_MAX - 1) : (DEVICE_INDEX_GPU);

    for (deviceIndex = minDeviceIndex; deviceIndex <= maxDeviceIndex; deviceIndex++)
    {
        // Initialize IO Device and Aperture
        DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, deviceIndex, 0);
        if (pMapping != NULL)
        {
            rmStatus = _gpuInitIOAperture(pGpu, deviceIndex, pMapping);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to initialize pGpu IO device/aperture for deviceIndex=%d.\n", deviceIndex);
                return rmStatus;
            }
        }
    }

    return rmStatus;
}

void
regAccessDestruct
(
    RegisterAccess *pRegisterAccess
)
{
    OBJGPU         *pGpu = pRegisterAccess->pGpu;
    DEVICE_INDEX    deviceIndex;
    NvU32           mappingNum;
    IoAperture     *pIOAperture;
    REGISTER_FILTER *pNode;

    // Ignore attempt to destruct a not-fully-constructed RegisterAccess
    if (pGpu == NULL)
    {
        return;
    }

    for (deviceIndex = 0; deviceIndex < DEVICE_INDEX_MAX; deviceIndex++)
    {
        pIOAperture = pGpu->pIOApertures[deviceIndex];
        if (pIOAperture != NULL)
        {
            objDelete(pIOAperture);
        }
    }

    for (mappingNum = 0; mappingNum < pGpu->gpuDeviceMapCount; mappingNum++)
    {
        // Device-specific register filter list
        NV_ASSERT(!pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterList);
        if (NULL != pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterLock)
        {
            portSyncSpinlockDestroy(pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterLock);
            pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterLock = NULL;
        }

        while (pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterRecycleList)
        {
            pNode = pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterRecycleList;

            pGpu->deviceMappings[mappingNum].devRegFilterInfo.pRegFilterRecycleList = pNode->pNext;
            portMemFree(pNode);
        }
    }
}

//
// The following register I/O functions are organized into two groups;
// a multi-chip unaware group and a multi-chip aware group.
// The multi-chip aware group of register I/O functions is also split
// into two groups; one that really does multi-chip logic and another
// that has the same interface but doesn't do any of the multi-chip
// logic.
//
// In the interests of performance, the determination as to whether
// multi-chip logic is necessary is done at two levels; the upper-level
// functions use 'MC' register I/O macros where multi-chip considerations
// are required, and when the 'MC' register I/O macros are used they
// call through GPU object pointers that are polymorphic - they contain
// pointers to one of the two groups of multi-chip aware functions
// depending on whether the multi-chip condition actually exists.
// This avoids a run-time SLI LOOP call.
//
static void
_regWriteUnicast
(
    RegisterAccess    *pRegisterAccess,
    DEVICE_INDEX       deviceIndex,
    NvU32              instance,
    NvU32              addr,
    NvU32              val,
    NvU32              size,
    THREAD_STATE_NODE *pThreadState
)
{
    OBJGPU   *pGpu    = pRegisterAccess->pGpu;
    NvU32     flags   = 0;
    NV_STATUS status;
    DEVICE_MAPPING *pMapping;

    pRegisterAccess->regWriteCount++;

    pMapping = gpuGetDeviceMapping(pGpu, deviceIndex, instance);
    if (pMapping == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Could not find mapping for reg %x, deviceIndex=0x%x instance=%d\n",
                  addr, deviceIndex, instance);
        NV_ASSERT(0);
        return;
    }

    status = gpuSanityCheckRegisterAccess(pGpu, addr, NULL);
    if (status != NV_OK)
    {
        return;
    }

    _gpuHandleWriteRegisterFilter(pGpu, deviceIndex, instance, addr, val, size, &flags, pThreadState);

    if (!(flags & REGISTER_FILTER_FLAGS_WRITE))
    {
        switch (size)
        {
            case 8:
                osDevWriteReg008(pGpu, pMapping, addr, 0xFFU & (val));
                break;
            case 16:
                osDevWriteReg016(pGpu, pMapping, addr, 0xFFFFU & (val));
                break;
            case 32:
                osDevWriteReg032(pGpu, pMapping, addr, val);
                break;
        }
    }
}

/*!
 * @brief: Initialize an IoAperture instance in-place.
 *
 * @param[out] pAperture        pointer to the IoAperture.
 * @param[in]  pParentAperture  pointer to the parent of the new IoAperture.
 * @param[in]  offset           offset from the parent APERTURE's baseAddress.
 * @param[in]  length           length of the APERTURE.
 *
 * @return NV_OK upon success
 *         NV_ERR* otherwise.
 */
NV_STATUS
ioaprtInit
(
    IoAperture     *pAperture,
    IoAperture     *pParentAperture,
    NvU32           offset,
    NvU32           length
)
{
    return objCreateWithFlags(&pAperture, NVOC_NULL_OBJECT, IoAperture, NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT, pParentAperture, NULL, 0, 0, NULL, 0, offset, length);
}

/*!
 * Initialize an IoAperture instance.
 *
 * @param[in,out] pAperture        pointer to IoAperture instance to be initialized.
 * @param[in]     pParentAperture  pointer to parent of the new IoAperture.
 * @param[in]     deviceIndex      device index
 * @param[in]     deviceInstance   device instance
 * @param[in]     pMapping         device register mapping
 * @param[in]     mappingStartAddr register address corresponding to the start of the mapping
 * @param[in]     offset           offset from the parent APERTURE's baseAddress.
 * @param[in]     length           length of the APERTURE.
 *
 * @return NV_OK when inputs are valid.
 */
NV_STATUS
ioaprtConstruct_IMPL
(
    IoAperture      *pAperture,
    IoAperture      *pParentAperture,
    OBJGPU          *pGpu,
    NvU32            deviceIndex,
    NvU32            deviceInstance,
    DEVICE_MAPPING  *pMapping,
    NvU32            mappingStartAddr,
    NvU32            offset,
    NvU32            length
)
{
    if (pParentAperture != NULL)
    {
        NV_ASSERT_OR_RETURN(pMapping == NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pGpu == NULL || pGpu == pParentAperture->pGpu, NV_ERR_INVALID_ARGUMENT);

        pAperture->pGpu = pParentAperture->pGpu;
        pAperture->deviceIndex = pParentAperture->deviceIndex;
        pAperture->deviceInstance = pParentAperture->deviceInstance;
        pAperture->pMapping    = pParentAperture->pMapping;
        pAperture->baseAddress = pParentAperture->baseAddress;
        pAperture->mappingStartAddr = pParentAperture->mappingStartAddr;

        // Check if the child Aperture strides beyond the parent's boundary.
        if ((length + offset) > pParentAperture->length)
        {
            NV_PRINTF(LEVEL_WARNING,
                "Child aperture crosses parent's boundary, length %u offset %u, Parent's length %u\n",
                length, offset, pParentAperture->length);
        }

    }
    else
    {
        NV_ASSERT_OR_RETURN(pMapping != NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

        pAperture->pGpu = pGpu;
        pAperture->deviceIndex = deviceIndex;
        pAperture->deviceInstance = deviceInstance;
        pAperture->pMapping = pMapping;
        pAperture->baseAddress = 0;
        pAperture->mappingStartAddr = mappingStartAddr;
    }

    pAperture->baseAddress += offset;
    pAperture->length       = length;

    return NV_OK;
}

static void
ioaprtWriteRegUnicast
(
    OBJGPU         *pGpu,
    IoAperture     *pAperture,
    NvU32           addr,
    NvV32           val,
    NvU32           size
)
{
    NvU32              deviceIndex = pAperture->deviceIndex;
    NvU32              instance    = pAperture->deviceInstance;
    NvU32              regAddr     = pAperture->baseAddress + addr;
    NvU32              mappingRegAddr = regAddr - pAperture->mappingStartAddr;
    DEVICE_MAPPING    *pMapping    = pAperture->pMapping;
    NvU32              flags       = 0;
    NV_STATUS          status;
    THREAD_STATE_NODE *pThreadState;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        status = gpuSanityCheckRegisterAccess(pGpu, regAddr, NULL);
        if (status != NV_OK)
        {
            return;
        }

        threadStateGetCurrentUnchecked(&pThreadState, pGpu);

        _gpuHandleWriteRegisterFilter(pGpu, deviceIndex, instance, regAddr,
                                      val, size, &flags, pThreadState);
    }

    if (!(flags & REGISTER_FILTER_FLAGS_WRITE))
    {
        switch (size)
        {
            case 8:
                osDevWriteReg008(pGpu, pMapping, mappingRegAddr, 0xFFU & (val));
                break;
            case 16:
                osDevWriteReg016(pGpu, pMapping, mappingRegAddr, 0xFFFFU & (val));
                break;
            case 32:
                osDevWriteReg032(pGpu, pMapping, mappingRegAddr, val);
                break;
        }
    }
}

void
ioaprtWriteReg08_IMPL
(
    IoAperture     *pAperture,
    NvU32           addr,
    NvV8            val
)
{
    NV_ASSERT(!gpumgrGetBcEnabledStatus(pAperture->pGpu));

    ioaprtWriteRegUnicast(pAperture->pGpu, pAperture, addr, val, 8 /* size */);
}

void
ioaprtWriteReg16_IMPL
(
    IoAperture        *pAperture,
    NvU32              addr,
    NvV16              val
)
{
    NV_ASSERT(!gpumgrGetBcEnabledStatus(pAperture->pGpu));

    ioaprtWriteRegUnicast(pAperture->pGpu, pAperture, addr, val, 16 /* size */);
}

void
ioaprtWriteReg32_IMPL
(
    IoAperture        *pAperture,
    NvU32              addr,
    NvV32              val
)
{
    NV_ASSERT(!gpumgrGetBcEnabledStatus(pAperture->pGpu));

    ioaprtWriteRegUnicast(pAperture->pGpu, pAperture, addr, val, 32 /* size */);
}

void
ioaprtWriteReg32Uc_IMPL
(
    IoAperture        *pAperture,
    NvU32              addr,
    NvV32              val
)
{
    ioaprtWriteRegUnicast(pAperture->pGpu, pAperture, addr, val, 32 /* size */);
}

void
regWrite008
(
    RegisterAccess *pRegisterAccess,
    DEVICE_INDEX    deviceIndex,
    NvU32           instance,
    NvU32           addr,
    NvV8            val
)
{
    OBJGPU *pGpu = pRegisterAccess->pGpu;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
        _regWriteUnicast(GPU_GET_REGISTER_ACCESS(pGpu), deviceIndex, instance, addr, val, 8, NULL);
    SLI_LOOP_END;
}
void
regWrite016
(
    RegisterAccess *pRegisterAccess,
    DEVICE_INDEX    deviceIndex,
    NvU32           instance,
    NvU32           addr,
    NvV16           val
)
{
    OBJGPU *pGpu = pRegisterAccess->pGpu;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
        _regWriteUnicast(GPU_GET_REGISTER_ACCESS(pGpu), deviceIndex, instance, addr, val, 16, NULL);
    SLI_LOOP_END;
}

void
regWrite032
(
    RegisterAccess    *pRegisterAccess,
    DEVICE_INDEX       deviceIndex,
    NvU32              instance,
    NvU32              addr,
    NvV32              val,
    THREAD_STATE_NODE *pThreadState
)
{
    OBJGPU *pGpu = pRegisterAccess->pGpu;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
        regWrite032Unicast(GPU_GET_REGISTER_ACCESS(pGpu), deviceIndex, instance, addr, val, pThreadState);
    SLI_LOOP_END
}

void
regWrite032Unicast
(
    RegisterAccess    *pRegisterAccess,
    DEVICE_INDEX       deviceIndex,
    NvU32              instance,
    NvU32              addr,
    NvV32              val,
    THREAD_STATE_NODE *pThreadState
)
{

    _regWriteUnicast(pRegisterAccess, deviceIndex, instance, addr, val, 32, pThreadState);
}

static NvU32
ioaprtReadReg
(
    IoAperture     *pAperture,
    NvU32           addr,
    NvU32           size
)
{
    NvU32              flags       = 0;
    NvU32              returnValue = 0;
    OBJGPU            *pGpu        = pAperture->pGpu;
    NV_STATUS          status      = NV_OK;
    NvU32              regAddr     = pAperture->baseAddress + addr;
    NvU32              mappingRegAddr = regAddr - pAperture->mappingStartAddr;
    NvU32              deviceIndex = pAperture->deviceIndex;
    NvU32              instance    = pAperture->deviceInstance;
    DEVICE_MAPPING    *pMapping    = pAperture->pMapping;
    THREAD_STATE_NODE *pThreadState;

    pGpu->registerAccess.regReadCount++;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        status = gpuSanityCheckRegisterAccess(pGpu, regAddr, NULL);
        if (status != NV_OK)
        {
            return (~0);
        }

        threadStateGetCurrentUnchecked(&pThreadState, pGpu);

        returnValue = _gpuHandleReadRegisterFilter(pGpu, deviceIndex, instance,
                                                   regAddr, size, &flags, pThreadState);
    }

    if (!(flags & REGISTER_FILTER_FLAGS_READ))
    {
        switch (size)
        {
            case 8:
                returnValue = osDevReadReg008(pGpu, pMapping, mappingRegAddr);
                break;
            case 16:
                returnValue = osDevReadReg016(pGpu, pMapping, mappingRegAddr);
                break;
            case 32:
                returnValue = osDevReadReg032(pGpu, pMapping, mappingRegAddr);
                break;
        }
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        // Make sure the value read is sane before we party on it.
        gpuSanityCheckRegRead(pGpu, regAddr, size, &returnValue);
    }

    return returnValue;
}

NvU8
ioaprtReadReg08_IMPL
(
    IoAperture     *pAperture,
    NvU32           addr
)
{
    return (NvU8) ioaprtReadReg(pAperture, addr, 8 /* size */);
}

NvU16
ioaprtReadReg16_IMPL
(
    IoAperture     *pAperture,
    NvU32           addr
)
{
    return (NvU16) ioaprtReadReg(pAperture, addr, 16 /* size */);
}

NvU32
ioaprtReadReg32_IMPL
(
    IoAperture        *pAperture,
    NvU32              addr

)
{
    return ioaprtReadReg(pAperture, addr, 32 /* size */);
}

/*!
 * Checks if the register address is valid for a particular aperture
 *
 * @param[in]       pAperture       IoAperture pointer
 * @param[in]       addr            register address
 *
 * @returns         NV_TRUE         Register offset is valid
 */
NvBool
ioaprtIsRegValid_IMPL
(
    IoAperture     *pAperture,
    NvU32           addr
)
{
    NV_ASSERT_OR_RETURN(pAperture != NULL, NV_FALSE);

    return addr < pAperture->length;
}

static NvU32
_regRead
(
    RegisterAccess    *pRegisterAccess,
    DEVICE_INDEX       deviceIndex,
    NvU32              instance,
    NvU32              addr,
    NvU32              size,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32     flags       = 0;
    NvU32     returnValue = 0;
    OBJGPU   *pGpu        = pRegisterAccess->pGpu;
    DEVICE_MAPPING *pMapping;
    NV_STATUS status      = NV_OK;

    pRegisterAccess->regReadCount++;

    pMapping = gpuGetDeviceMapping(pGpu, deviceIndex, instance);
    if (pMapping == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Could not find mapping for reg %x, deviceIndex=0x%x instance=%d\n",
                  addr, deviceIndex, instance);
        NV_ASSERT(0);
        return 0xd0d0d0d0;
    }

    if ((size == 32) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_BUGCHECK_CALLBACK_ROUTINE))
    {
        return osDevReadReg032(pGpu, pMapping, addr);
    }

    status = gpuSanityCheckRegisterAccess(pGpu, addr, &returnValue);
    if (status != NV_OK)
        return returnValue;

    returnValue = _gpuHandleReadRegisterFilter(pGpu, deviceIndex, instance,
                                               addr, size, &flags, pThreadState);

    if (!(flags & REGISTER_FILTER_FLAGS_READ))
    {
        switch (size)
        {
            case 8:
                returnValue = osDevReadReg008(pGpu, pMapping, addr);
                break;
            case 16:
                returnValue = osDevReadReg016(pGpu, pMapping, addr);
                break;
            case 32:
                returnValue = osDevReadReg032(pGpu, pMapping, addr);
                break;
        }
    }

    // Make sure the value read is sane before we party on it.
    gpuSanityCheckRegRead(pGpu, addr, size, &returnValue);

    return returnValue;
}

NvU8
regRead008
(
    RegisterAccess *pRegisterAccess,
    DEVICE_INDEX    deviceIndex,
    NvU32           instance,
    NvU32           addr
)
{
    return _regRead(pRegisterAccess, deviceIndex, instance, addr, 8, NULL);
}

NvU16
regRead016
(
    RegisterAccess *pRegisterAccess,
    DEVICE_INDEX    deviceIndex,
    NvU32           instance,
    NvU32           addr
)
{
    return _regRead(pRegisterAccess, deviceIndex, instance, addr, 16, NULL);
}

/*!
 * This function is used for converting do-while read register constructs in RM to
 * equivalent PMU sequencer handling. The idea is to construct seq instruction
 * which polls on a field in the given register.
 *
 * @param[in]       pRegisterAccess RegisterAccess object pointer
 * @param[in]       deviceIndex     deviceIndex
 * @param[in]       addr            register address
 * @param[in]       mask            required mask for the field
 * @param[in]       val             value to poll for
 *
 * @returns         NV_OK           if val is found
 *                  NV_ERR_TIMEOUT  if val is not found within timeout limit
 */
NV_STATUS
regRead032_AndPoll
(
    RegisterAccess   *pRegisterAccess,
    DEVICE_INDEX      deviceIndex,
    NvU32             addr,
    NvU32             mask,
    NvU32             val
)
{
    RMTIMEOUT  timeout;
    OBJGPU    *pGpu = pRegisterAccess->pGpu;
    NvU32      data = 0;
    NV_STATUS  status = NV_OK;

    {
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

        do
        {
            data = GPU_REG_RD32(pGpu, addr);

            if ((data & mask) == val)
            {
                status = NV_OK;
                break;
            }

            // Loosen this loop
            osSpinLoop();

            status = gpuCheckTimeout(pGpu, &timeout);
        } while (status != NV_ERR_TIMEOUT);
    }

    return status;
}

NvU32
regRead032
(
    RegisterAccess    *pRegisterAccess,
    DEVICE_INDEX       deviceIndex,
    NvU32              instance,
    NvU32              addr,
    THREAD_STATE_NODE *pThreadState
)
{
    if (pRegisterAccess == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    return _regRead(pRegisterAccess, deviceIndex, instance, addr, 32, pThreadState);
}

/*!
 * @brief Allocates and initializes GPU_IO_DEVICE and IO Aperture.
 *
 * @param     pGpu
 * @param[in] deviceIndex   DEVICE_INDEX enum value for identifying device type
 * @param[in] gpuDeviceEnum Device ID NV_DEVID_*
 * @param[in] gpuNvPAddr    Physical Base Address
 * @param[in] gpuNvLength   Length of Aperture
 *
 * @return NV_OK if IO Aperture is successfully initialized, error otherwise.
 */
static NV_STATUS
_gpuInitIOAperture
(
    OBJGPU         *pGpu,
    NvU32           deviceIndex,
    DEVICE_MAPPING *pMapping
)
{
    NV_STATUS rmStatus;

    rmStatus = objCreate(&pGpu->pIOApertures[deviceIndex], NVOC_NULL_OBJECT, IoAperture,
                         NULL, // no parent aperture
                         pGpu,
                         deviceIndex,
                         0, // GPU register operations are always on instance 0
                         pMapping, 0, // mapping, mappingStartAddr
                         0, pMapping->gpuNvLength); // offset, length
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to initialize pGpu IO aperture for devIdx %d.\n",
                  deviceIndex);

        return rmStatus;
    }

    return NV_OK;
}


NV_STATUS
regAddRegisterFilter
(
    RegisterAccess *pRegisterAccess,
    NvU32 flags,
    DEVICE_INDEX devIndex, NvU32 devInstance,
    NvU32 rangeStart, NvU32 rangeEnd,
    GpuWriteRegCallback pWriteCallback,
    GpuReadRegCallback pReadCallback,
    void *pParam,
    REGISTER_FILTER **ppFilter
)
{
    DEVICE_REGFILTER_INFO *pRegFilter;
    REGISTER_FILTER     *pNode;
    REGISTER_FILTER     *pTmpNode;
    DEVICE_MAPPING      *pMapping;

    NV_ASSERT_OR_RETURN(devIndex < DEVICE_INDEX_MAX, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRegisterAccess != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ppFilter != NULL, NV_ERR_INVALID_ARGUMENT);

    // Get the device filter
    pMapping = gpuGetDeviceMapping(pRegisterAccess->pGpu, devIndex, devInstance);
    NV_ASSERT_OR_RETURN(pMapping != NULL, NV_ERR_INVALID_ARGUMENT);

    pRegFilter = &pMapping->devRegFilterInfo;

    if (!pWriteCallback && !pReadCallback)
    {
        // At least one register callback needs to be passed.
        NV_PRINTF(LEVEL_ERROR,
                  "Need to specify at least one callback function.\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT(!(flags & REGISTER_FILTER_FLAGS_INVALID));

    if ((flags & REGISTER_FILTER_FLAGS_READ) && !pReadCallback)
    {
        // If REGISTER_FILTER_FLAGS_READ is specified, then a read
        // callback must also be specified.
        NV_PRINTF(LEVEL_ERROR,
                  "REGISTER_FILTER_FLAGS_READ requires a read callback function.\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((flags & REGISTER_FILTER_FLAGS_WRITE) && !pWriteCallback)
    {
        // If REGISTER_FILTER_FLAGS_WRITE is specified, then a write
        // callback must also be specified.
        NV_PRINTF(LEVEL_ERROR,
                  "REGISTER_FILTER_FLAGS_WRITE requires a write callback function.\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    // If the regfilter hasn't been used yet, then allocate a lock
    if (NULL == pRegFilter->pRegFilterLock)
    {
        // Allocate spinlock for reg filter access
        pRegFilter->pRegFilterLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
        NV_ASSERT_OR_RETURN(pRegFilter->pRegFilterLock != NULL, NV_ERR_INSUFFICIENT_RESOURCES);
    }

    portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);

    if (NULL != pRegFilter->pRegFilterRecycleList)
    {
        pNode = pRegFilter->pRegFilterRecycleList;
        pRegFilter->pRegFilterRecycleList = pNode->pNext;
    }
    else
    {
        portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
        pNode = portMemAllocNonPaged(sizeof(REGISTER_FILTER));
        if (NULL == pNode)
        {
            return NV_ERR_NO_MEMORY;
        }
        portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);
    }

    // Print a warning if there's another register filter already registered.
    if (((pTmpNode = _findGpuRegisterFilter(devIndex, devInstance, rangeStart, pRegFilter->pRegFilterList)) != NULL) ||
        ((pTmpNode = _findGpuRegisterFilter(devIndex, devInstance, rangeEnd,   pRegFilter->pRegFilterList)) != NULL))
    {
            NV_PRINTF(LEVEL_WARNING,
                      "WARNING!! Previously registered reg filter found. Handle: %p, dev: "
                      "%d(%d) Range : 0x%x - 0x%x, WR/RD Callback: %p/%p, flags : %x\n",
                      pTmpNode, pTmpNode->devIndex, pTmpNode->devInstance,
                      pTmpNode->rangeStart, pTmpNode->rangeEnd,
                      pTmpNode->pWriteCallback, pTmpNode->pReadCallback,
                      pTmpNode->flags);
    }

    // Populate structure
    pNode->flags          = flags;
    pNode->devIndex       = devIndex;
    pNode->devInstance    = devInstance;
    pNode->rangeStart     = rangeStart;
    pNode->rangeEnd       = rangeEnd;
    pNode->pWriteCallback = pWriteCallback;
    pNode->pReadCallback  = pReadCallback;
    pNode->pParam         = pParam;

    // Link in
    pNode->pNext = pRegFilter->pRegFilterList;
    pRegFilter->pRegFilterList = pNode;

    // return pNode
    *ppFilter = pNode;

    portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
    return NV_OK;
}

void
regRemoveRegisterFilter
(
    RegisterAccess *pRegisterAccess,
    REGISTER_FILTER *pFilter
)
{
    REGISTER_FILTER       *pNode;
    REGISTER_FILTER       *pPrev = NULL;
    REGISTER_FILTER       *pNext = NULL;
    DEVICE_REGFILTER_INFO *pRegFilter;
    DEVICE_MAPPING        *pMapping;

    // Get the device filter
    pMapping = gpuGetDeviceMapping(pRegisterAccess->pGpu, pFilter->devIndex, pFilter->devInstance);
    NV_ASSERT_OR_RETURN_VOID(pMapping != NULL);

    pRegFilter = &pMapping->devRegFilterInfo;

    portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);
    pNode = pRegFilter->pRegFilterList;
    while (pNode)
    {
        //
        // we could have used a doubly linked list to do a quick removal, but
        // iterating the list to find the match serves as sanity test, so let's
        // stick with a singly linked list.
        //
        if (pNode == pFilter)
        {
            if (pRegFilter->regFilterRefCnt > 0)
            {
                // defer removal if another thread is working on the list
                pNode->flags |= REGISTER_FILTER_FLAGS_INVALID;
                pRegFilter->bRegFilterNeedRemove = NV_TRUE;
                portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
                return;
            }

            // Unlink
            pNext = pNode->pNext;

            // place on recycle list
            pNode->pNext = pRegFilter->pRegFilterRecycleList;
            pRegFilter->pRegFilterRecycleList = pNode;

            if (pPrev)
            {
                pPrev->pNext = pNext;
            }
            else
            {
                pRegFilter->pRegFilterList = pNext;
            }

            portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
            return;
        }

        pPrev = pNode;
        pNode = pNode->pNext;
    }
    NV_ASSERT_FAILED("Attempted to remove a nonexistent filter");
    portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
}

// called with lock held
static void
_gpuCleanRegisterFilterList
(
    DEVICE_REGFILTER_INFO *pRegFilter
)
{
    REGISTER_FILTER *pNode = pRegFilter->pRegFilterList;
    REGISTER_FILTER *pPrev = NULL;
    REGISTER_FILTER *pNext = NULL;

    while (pNode)
    {
        if (pNode->flags & REGISTER_FILTER_FLAGS_INVALID)
        {
            // Unlink
            pNext = pNode->pNext;

            // place on recycle list
            pNode->pNext = pRegFilter->pRegFilterRecycleList;
            pRegFilter->pRegFilterRecycleList = pNode;

            if (pPrev)
            {
                pPrev->pNext = pNext;
            }
            else
            {
                pRegFilter->pRegFilterList = pNext;
            }

            pNode = pNext;
            continue;
        }

        pPrev = pNode;
        pNode = pNode->pNext;
    }
}

static NvU32
_gpuHandleReadRegisterFilter
(
    OBJGPU            *pGpu,
    DEVICE_INDEX       devIndex,
    NvU32              devInstance,
    NvU32              addr,
    NvU32              accessSize,
    NvU32             *pFlags,
    THREAD_STATE_NODE *pThreadState
)
{
    REGISTER_FILTER       *pFilter;
    NvU32                  returnValue = 0;
    NvU32                  tempVal     = 0;
    DEVICE_REGFILTER_INFO *pRegFilter;
    DEVICE_MAPPING        *pMapping;

    // Get the device filter
    pMapping = gpuGetDeviceMapping(pGpu, devIndex, devInstance);
    NV_ASSERT_OR_RETURN(pMapping != NULL, returnValue);

    pRegFilter = &pMapping->devRegFilterInfo;

    // if there is no filter, do nothing. just bail out.
    if (pRegFilter->pRegFilterList == NULL)
    {
        return returnValue;
    }

    if (pThreadState != NULL)
    {
        // Filters should be only used with GPU lock is held.
        if (pThreadState->flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS)
        {
            return returnValue;
        }
    }
#ifdef DEBUG
    else
    {
        THREAD_STATE_NODE *pCurThread;

        if (NV_OK == threadStateGetCurrentUnchecked(&pCurThread, pGpu))
        {
            // Filters should be only used with GPU lock is held.
            // Assert because ISRs are expected to pass threadstate down the stack.
            // Don't bale out to keep release and debug path behavior identical.
            if (pCurThread->flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS)
            {
                NV_ASSERT(0);
            }
        }
    }
#endif

    //
    // NOTE: we can't simply grab the lock and release it after
    //       the search since it is not safe to assume that
    //       callbacks can be called with spinlock held
    //
    portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);
    pRegFilter->regFilterRefCnt++;
    portSyncSpinlockRelease(pRegFilter->pRegFilterLock);

    //
    // Note there is potential thread race condition where a filter may be
    // being added or removed in one thread (dispatch) while another thread
    // is searching the list.  This search should have a lock in place.
    //
    pFilter = pRegFilter->pRegFilterList;
    while ((pFilter) && (pFilter = _findGpuRegisterFilter(devIndex, devInstance, addr, pFilter)))
    {
        if (pFilter->pReadCallback)
        {
            tempVal = pFilter->pReadCallback(pGpu, pFilter->pParam, addr,
                                             accessSize, *pFlags);
            //
            // if there are multiple filters, we use the last filter found to
            // save returnValue
            //
            if (pFilter->flags & REGISTER_FILTER_FLAGS_READ)
            {
                returnValue = tempVal;
            }
        }
        *pFlags |= pFilter->flags;
        pFilter = pFilter->pNext;
    }

    portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);
    pRegFilter->regFilterRefCnt--;
    if (pRegFilter->regFilterRefCnt == 0 && pRegFilter->bRegFilterNeedRemove)
    {
        // no other thread can be touching the list. remove invalid entries
        _gpuCleanRegisterFilterList(pRegFilter);
        pRegFilter->bRegFilterNeedRemove = NV_FALSE;
    }
    portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
    return returnValue;
}

static void
_gpuHandleWriteRegisterFilter
(
    OBJGPU            *pGpu,
    DEVICE_INDEX       devIndex,
    NvU32              devInstance,
    NvU32              addr,
    NvU32              val,
    NvU32              accessSize,
    NvU32             *pFlags,
    THREAD_STATE_NODE *pThreadState
)
{
    REGISTER_FILTER       *pFilter;
    DEVICE_REGFILTER_INFO *pRegFilter;
    DEVICE_MAPPING        *pMapping;

    // Get the device filter
    pMapping = gpuGetDeviceMapping(pGpu, devIndex, devInstance);
    NV_ASSERT_OR_RETURN_VOID(pMapping != NULL);

    pRegFilter = &pMapping->devRegFilterInfo;

    // if there is no filter, do nothing. just bail out.
    if (pRegFilter->pRegFilterList == NULL)
    {
        return;
    }

    if (pThreadState != NULL)
    {
        // Filters should be only used with GPU lock is held.
        if (pThreadState->flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS)
        {
            return;
        }
    }
#ifdef DEBUG
    else
    {
        THREAD_STATE_NODE *pCurThread;

        if (NV_OK == threadStateGetCurrentUnchecked(&pCurThread, pGpu))
        {
            // Filters should be only used with GPU lock is held.
            // Assert because ISRs are expected to pass threadstate down the stack.
            // Don't bale out to keep release and debug path behavior identical.
            if (pCurThread->flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS)
            {
                NV_ASSERT(0);
            }
        }
    }
#endif

    //
    // NOTE: we can't simply grab the lock and release it after
    //       the search since it is not safe to assume that
    //       callbacks can be called with spinlock held
    //
    portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);
    pRegFilter->regFilterRefCnt++;
    portSyncSpinlockRelease(pRegFilter->pRegFilterLock);

    //
    // Note there is potential thread race condition where a filter may be
    // being added or removed in one thread (dispatch) while another thread
    // is searching the list.  This search should have a lock in place.
    //
    pFilter = pRegFilter->pRegFilterList;
    while ((pFilter) && (pFilter = _findGpuRegisterFilter(devIndex, devInstance, addr, pFilter)))
    {
        if (pFilter->pWriteCallback)
        {
            pFilter->pWriteCallback(pGpu, pFilter->pParam, addr, val,
                                    accessSize, *pFlags);
        }
        *pFlags |= pFilter->flags;
        pFilter = pFilter->pNext;
    }

    portSyncSpinlockAcquire(pRegFilter->pRegFilterLock);
    pRegFilter->regFilterRefCnt--;
    if (pRegFilter->regFilterRefCnt == 0 && pRegFilter->bRegFilterNeedRemove)
    {
        // no other thread can be touching the list. remove invalid entries
        _gpuCleanRegisterFilterList(pRegFilter);
        pRegFilter->bRegFilterNeedRemove = NV_FALSE;
    }
    portSyncSpinlockRelease(pRegFilter->pRegFilterLock);
}

static REGISTER_FILTER *
_findGpuRegisterFilter
(
    DEVICE_INDEX     devIndex,
    NvU32            devInstance,
    NvU32            addr,
    REGISTER_FILTER *pFilter
)
{
    while (pFilter != NULL)
    {
        if (!(pFilter->flags & REGISTER_FILTER_FLAGS_INVALID) &&
            (devIndex == pFilter->devIndex) &&
            (devInstance == pFilter->devInstance) &&
            (addr >= pFilter->rangeStart) && (addr <= pFilter->rangeEnd))
        {
            break;
        }

        pFilter = pFilter->pNext;
    }

    return pFilter;
}

static NvBool
_gpuEnablePciMemSpaceAndCheckPmcBoot0Match
(
    OBJGPU *pGpu
)
{
    NvU16 VendorId;
    NvU16 DeviceId;
    NvU8  bus = gpuGetBus(pGpu);
    NvU8  device = gpuGetDevice(pGpu);
    NvU32 domain = gpuGetDomain(pGpu);
    void *Handle = osPciInitHandle(domain, bus, device, 0, &VendorId, &DeviceId);
    NvU32 Enabled = osPciReadDword(Handle, NV_CONFIG_PCI_NV_1);
    NvU32 pmcBoot0;

    // If Memory Spaced is not enabled, enable it
    if (DRF_VAL(_CONFIG, _PCI_NV_1, _MEMORY_SPACE, Enabled) != NV_CONFIG_PCI_NV_1_MEMORY_SPACE_ENABLED)
    {
        osPciWriteDword(Handle, NV_CONFIG_PCI_NV_1,
                        Enabled |
                        (DRF_DEF(_CONFIG, _PCI_NV_1, _MEMORY_SPACE, _ENABLED) |
                        DRF_DEF(_CONFIG, _PCI_NV_1, _BUS_MASTER, _ENABLED)));
    }

    // Check PMC_ENABLE to make sure that it matches
    pmcBoot0 = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);
    if (pmcBoot0 == pGpu->chipId0)
    {
       return NV_TRUE;
    }

    return NV_FALSE;
}

static NvU32
_regCheckReadFailure
(
    OBJGPU *pGpu,
    NvU32   value
)
{
    NvU32 flagsFailed;
    NvU32 reason = BAD_READ_UNKNOWN;

    if ((!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH)) &&
        (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST)))
    {
        gpuSanityCheck(pGpu, GPU_SANITY_CHECK_FLAGS_ALL, &flagsFailed);

        // This is where we need to determine why we might be seeing this failure
        if (value == GPU_REG_VALUE_INVALID)
        {
            // Does PCI Space Match
            if (flagsFailed & GPU_SANITY_CHECK_FLAGS_PCI_SPACE_MATCH)
            {
                reason = BAD_READ_GPU_OFF_BUS;
                goto exit;
            }

            // Is Memory Spaced Enabled
            if (flagsFailed & GPU_SANITY_CHECK_FLAGS_PCI_MEM_SPACE_ENABLED)
            {
                reason = BAD_READ_PCI_DEVICE_DISABLED;

                if (!_gpuEnablePciMemSpaceAndCheckPmcBoot0Match(pGpu))
                {
                    // We have been reset!
                    reason = BAD_READ_GPU_RESET;
                    goto exit;
                }
            }
        }

        // Are we off by N
        if (flagsFailed & GPU_SANITY_CHECK_FLAGS_OFF_BY_N)
        {
            reason = BAD_READ_DWORD_SHIFT;
        }
    }
    else
    {
        reason = BAD_READ_LOW_POWER;
    }

exit:
    return reason;
}

void
regCheckAndLogReadFailure
(
    RegisterAccess *pRegisterAccess,
    NvU32 addr,
    NvU32 mask,
    NvU32 value
)
{
    OBJGPU *pGpu = pRegisterAccess->pGpu;
    const NvU32 failureReason = _regCheckReadFailure(pGpu, value);
    PRmRC2BadRead2_RECORD pBadRead = NULL;
    OBJSYS *pSys = SYS_GET_INSTANCE();

    // Record a Journal Entry about this failure
    if (rcdbAllocNextJournalRec(SYS_GET_RCDB(pSys),
                                (NVCD_RECORD **)&pBadRead,
                                RmGroup,
                                RmBadRead_V2,
                                sizeof *pBadRead) == NV_OK)
    {
        rcdbSetCommonJournalRecord(pGpu, &pBadRead->common);
        pBadRead->MemorySpace = MEMORY_BAR0;
        pBadRead->Offset = addr;
        pBadRead->Mask = mask;
        pBadRead->Value = value;
        pBadRead->Reason = failureReason;

        // We are seeing some misreads in DVS runs.  Adding this so that we can get
        // stack traces of why this is happening
        if ((NV_DEBUG_BREAK_ATTRIBUTES_CRASH) &
            DRF_VAL(_DEBUG, _BREAK, _ATTRIBUTES, pSys->debugFlags))
        {
            osBugCheck(OS_BUG_CHECK_BUGCODE_INTERNAL_TEST);
        }
    }

    PORT_UNREFERENCED_VARIABLE(failureReason);
}

NvU32
regCheckRead032
(
    RegisterAccess    *pRegisterAccess,
    NvU32              addr,
    NvU32              mask,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 returnValue;
    OBJGPU *pGpu = pRegisterAccess->pGpu;

    returnValue = GPU_REG_RD32_EX(pGpu, addr, pThreadState);
    if (returnValue & mask)
    {
        if (!API_GPU_IN_RESET_SANITY_CHECK(pGpu))
            regCheckAndLogReadFailure(pRegisterAccess, addr, mask, returnValue);
        returnValue = 0;
    }

    return returnValue;
}

#if GPU_REGISTER_ACCESS_DUMP

NvU8
gpuRegRd08_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr)
{
    NvU8 val = REG_INST_RD08(pGpu, GPU, 0, addr);
    // filter out duplicate read
    static NvU32 prev_addr = 0;
    static NvU8  prev_val  = 0;
    if (addr != prev_addr || val != prev_val)
    {
        // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
        if ((addr & 0xFFF00000) != 0x00700000)
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "READ   func: %s, reg name: %s, addr: %08x, val: %02x\n",
                      func, addrStr, addr, val);
        }
        prev_addr = addr;
        prev_val = val;
    }
    return val;
}

NvU16
gpuRegRd16_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr)
{
    NvU16 val = REG_INST_RD16(pGpu, GPU, 0, addr);
    // filter out duplicate read
    static NvU32 prev_addr = 0;
    static NvU16 prev_val  = 0;
    if (addr != prev_addr || val != prev_val)
    {
        // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
        if ((addr & 0xFFF00000) != 0x00700000)
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "READ   func: %s, reg name: %s, addr: %08x, val: %04x\n",
                      func, addrStr, addr, val);
        }
        prev_addr = addr;
        prev_val = val;
    }
    return val;
}

NvU32
gpuRegRd32_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr)
{
    NvU32 val = REG_INST_RD32(pGpu, GPU, 0, addr);
    // filter out duplicate read
    static NvU32 prev_addr = 0;
    static NvU32 prev_val  = 0;
    if (addr != prev_addr || val != prev_val)
    {
        // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
        if ((addr & 0xFFF00000) != 0x00700000)
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "READ  %s func: %s, reg name: %s, addr: %08x, val: %08x\n",
                      vreg, func, addrStr, addr, val);
        }
        prev_addr = addr;
        prev_val = val;
    }
    return val;
}

void
gpuRegWr08_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV8 val)
{
    // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
    if ((addr & 0xFFF00000) != 0x00700000)
    {
        NV_PRINTF(LEVEL_NOTICE,
                 "WRITE  func: %s, reg name: %s, addr: %08x, val: %02x\n",
                 func, addrStr, addr, val);
    }
    REG_INST_WR08(pGpu, GPU, 0, addr, val);
}

void
gpuRegWr16_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV16 val)
{
    // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
    if ((addr & 0xFFF00000) != 0x00700000)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "WRITE  func: %s, reg name: %s, addr: %08x, val: %04x\n",
                  func, addrStr, addr, val);
    }
    REG_INST_WR16(pGpu, GPU, 0, addr, val);
}

void
gpuRegWr32_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV32 val)
{
    // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
    if ((addr & 0xFFF00000) != 0x00700000)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "WRITE %s func: %s, reg name: %s, addr: %08x, val: %08x\n",
                  vreg, func, addrStr, addr, val);
    }
    REG_INST_WR32(pGpu, GPU, 0, addr, val);
}

void
gpuRegWr32Uc_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV32 val)
{
    // filter out bar0 windows registers (NV_PRAMIN – range 0x007FFFFF:0x00700000 )
    if ((addr & 0xFFF00000) != 0x00700000)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "WRITE  func: %s, reg name: %s, addr: %08x, val: %08x\n",
                  func, addrStr, addr, val);
    }
    REG_INST_WR32_UC(pGpu, GPU, 0, addr, val);
}

#endif // GPU_REGISTER_ACCESS_DUMP

/*!
 * @brief Do any sanity checks for the GPU's state before actually reading/writing to the chip.
 *
 * @param[in]  pGpu       OBJGPU pointer
 * @param[in]  addr       Address of the register to be sanity checked
 * @param[out] pRetVal    Default return value for read accesses incase of sanity check failure. Only for U032 hals.
 *
 * @returns NV_ERR_GPU_IN_FULLCHIP_RESET    if GPU is in reset
 *          NV_ERR_GPU_IS_LOST              if GPU is inaccessible
 *          NV_ERR_GPU_NOT_FULL_POWER       if GPU is not at full power AND
 *                                             GPU is not in resume codepath
 *                                             sim low power reg access is disabled
 *          NV_OK                           Otherwise
 */
NV_STATUS
gpuSanityCheckRegisterAccess_IMPL
(
    OBJGPU     *pGpu,
    NvU32       addr,
    NvU32       *pRetVal
)
{
    NV_STATUS status = NV_OK;
    NvU32     retVal = ~0;

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        status = NV_ERR_GPU_IN_FULLCHIP_RESET;
        goto done;
    }

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        status = NV_ERR_GPU_IS_LOST;
        goto done;
    }

    if ((status = gpuSanityCheckVirtRegAccess_HAL(pGpu, addr)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid register access on VF, addr: 0x%x\n", addr);
        osAssertFailed();

        // Return 0 to match with HW behavior
        retVal = 0;
        goto done;
    }

    //
    // Make sure the GPU is in full power or resuming.  When the OS has put the
    // GPU in suspend (i.e. any of the D3 variants) there's no guarantee the GPU is
    // accessible over PCI-E: the GPU may be completely powered off, the
    // upstream bridges may not be properly configured, etc.  Attempts to access
    // the GPU may then result in PCI-E errors and/or bugchecks.  For examples,
    // see Bugs 440565 and 479003.
    // On Mshybrid, the OS will make sure we are up and alive before calling
    // into the driver. So we can skip this check on MsHybrid.
    //
    // DO NOT IGNORE OR REMOVE THIS ASSERT.  It is a warning that improperly
    // written RM code further up the stack is trying to access a GPU which is
    // in suspend (i.e. low power).  Any entry points into the RM (especially
    // those between GPUs or for asynchronous callbacks) should always check
    // that the GPU is in full power via gpuIsGpuFullPower(), bailing out in the
    // appropriate manner when it returns NV_FALSE.
    //
    // If you are not an RM engineer and are encountering this assert, please
    // file a bug against the RM.
    //
    if ((gpuIsGpuFullPower(pGpu) == NV_FALSE)                        &&
        !IS_GPU_GC6_STATE_ENTERING(pGpu)                             &&
        !(IS_GPU_GC6_STATE_ENTERED(pGpu)
         )                                                           &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE)   &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_REG_ACCESS_IN_LOW_POWER_FOR_SIM_SRTEST) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
    {
        DBG_BREAKPOINT();
        status = NV_ERR_GPU_NOT_FULL_POWER;
        goto done;
    }

    // TODO: More complete sanity checking

done:
    // Assign the return value
    if ((status != NV_OK) && (pRetVal != NULL))
    {
        *pRetVal = retVal;
    }
    return status;
}

/**
 * @brief checks if the register offset is valid
 *
 * @param[in] pGpu
 * @param[in] offset
 *
 * @returns NV_OK if valid
 * @returns NV_ERR_INVALID_ARGUMENT if offset is too large for bar
 * @returns NV_ERR_INSUFFICIENT_PERMISSIONS if user is not authorized to access register
 */
NV_STATUS
gpuValidateRegOffset_IMPL
(
    OBJGPU *pGpu,
    NvU32   offset
)
{
    NvU64 maxBar0Size = pGpu->deviceMappings[0].gpuNvLength;

    // The register offset should be 4 bytes smaller than the max bar size
    if (offset > (maxBar0Size - 4))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!osIsAdministrator() &&
        !gpuGetUserRegisterAccessPermissions(pGpu, offset))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "User does not have permission to access register offset 0x%x\n",
                  offset);
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return NV_OK;
}

/*!
 * @brief Verify existence function.
 *
 * @param[in] pGpu
 *
 * @returns NV_OK if GPU is still accessible
 *          NV_ERR_INVALID_STATE if GPU is inaccessible
 */
NV_STATUS
gpuVerifyExistence_IMPL
(
    OBJGPU *pGpu
)
{
    NvU32 regVal = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);

    if (regVal != pGpu->chipId0)
    {
        osHandleGpuLost(pGpu);
        regVal = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);
        if (regVal != pGpu->chipId0)
        {
            return NV_ERR_GPU_IS_LOST;
        }
    }

    return NV_OK;
}

/*!
 * @brief Perform a sanity check on a register read value
 * Starts with gpu-independent check, then calls into HAL for specific cases
 *
 * @param[in]       pGpu        GPU object pointer
 * @param[in]       addr        Value address
 * @param[in]       size        Access size
 * @param[in/out]   pValue      Value to sanity check
 */
NV_STATUS
gpuSanityCheckRegRead_IMPL
(
    OBJGPU *pGpu,
    NvU32 addr,
    NvU32 size,
    void *pValue
)
{
    NvU8       *pValue8;
    NvU16      *pValue16;
    NvU32      *pValue32;
    NvU32       value;

    switch (size)
    {
        case 8:
            {
                pValue8 = ((NvU8 *) pValue);
                if (*pValue8 == (NvU8) (~0))
                {
                    //
                    // The result looks suspicious, let's check if the GPU is still attached.
                    //
                    NvU32 testValue = osGpuReadReg032(pGpu, NV_PMC_BOOT_0);
                    if (testValue == GPU_REG_VALUE_INVALID)
                    {
                        osHandleGpuLost(pGpu);
                        *pValue8 = osGpuReadReg008(pGpu, addr);
                    }
                }
                break;
            }
        case 16:
            {
                pValue16 = ((NvU16 *) pValue);
                if (*pValue16 == (NvU16) (~0))
                {
                    //
                    // The result looks suspicious, let's check if the GPU is still attached.
                    //
                    NvU32 testValue = osGpuReadReg032(pGpu, NV_PMC_BOOT_0);
                    if (testValue == GPU_REG_VALUE_INVALID)
                    {
                        osHandleGpuLost(pGpu);
                        *pValue16 = osGpuReadReg016(pGpu, addr);
                    }
                }
                break;
            }
        case 32:
            {
                pValue32 = ((NvU32 *) pValue);
                if (*pValue32 == (NvU32) (~0))
                {
                    //
                    // The result looks suspicious, let's check if the GPU is still attached.
                    //
                    NvU32 testValue = osGpuReadReg032(pGpu, NV_PMC_BOOT_0);
                    if (testValue == GPU_REG_VALUE_INVALID)
                    {
                        osHandleGpuLost(pGpu);
                        *pValue32 = osGpuReadReg032(pGpu, addr);
                    }
                }

                value = *((NvU32 *)pValue);

                //
                // HW will return 0xbad in the upper 3 nibbles
                // when there is a possible issue.
                //
                if ((value & GPU_READ_PRI_ERROR_MASK) == GPU_READ_PRI_ERROR_CODE)
                {
                    gpuHandleSanityCheckRegReadError_HAL(pGpu, addr, value);
                }
                break;
            }
        default:
            {
                NV_ASSERT_FAILED("Invalid access size");
                break;
            }
    }

    return NV_OK;
}


NV_STATUS swbcaprtConstruct_IMPL
(
    SwBcAperture      *pAperture,
    IoAperture        *pApertures,
    NvU32              numApertures
)
{
    NV_ASSERT_OR_RETURN(numApertures != 0, NV_ERR_INVALID_ARGUMENT);

    pAperture->pApertures = pApertures;
    pAperture->numApertures = numApertures;

    return NV_OK;
}

NvU8
swbcaprtReadReg08_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr
)
{
    NvU8 val = REG_RD08(&pAperture->pApertures[0], addr);

#if defined(DEBUG)
    NvU32 i;
    for (i = 1; i < pAperture->numApertures; i++)
        NV_ASSERT(REG_RD08(&pAperture->pApertures[i], addr) == val);
#endif // defined(DEBUG)

    return val;
}

NvU16
swbcaprtReadReg16_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr
)
{
    NvU16 val = REG_RD16(&pAperture->pApertures[0], addr);

#if defined(DEBUG)
    NvU32 i;
    for (i = 1; i < pAperture->numApertures; i++)
        NV_ASSERT(REG_RD16(&pAperture->pApertures[i], addr) == val);
#endif // defined(DEBUG)

    return val;
}

NvU32
swbcaprtReadReg32_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr
)
{
    NvU32 val = REG_RD32(&pAperture->pApertures[0], addr);

#if defined(DEBUG)
    NvU32 i;
    for (i = 1; i < pAperture->numApertures; i++)
        NV_ASSERT(REG_RD32(&pAperture->pApertures[i], addr) == val);
#endif // defined(DEBUG)

    return val;
}

void
swbcaprtWriteReg08_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr,
    NvV8          value
)
{
    NvU32 i;

    for (i = 0; i < pAperture->numApertures; i++)
        REG_WR08(&pAperture->pApertures[i], addr, value);
}

void
swbcaprtWriteReg16_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr,
    NvV16         value
)
{
    NvU32 i;

    for (i = 0; i < pAperture->numApertures; i++)
        REG_WR16(&pAperture->pApertures[i], addr, value);
}

void
swbcaprtWriteReg32_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr,
    NvV32         value
)
{
    NvU32 i;

    for (i = 0; i < pAperture->numApertures; i++)
        REG_WR32(&pAperture->pApertures[i], addr, value);
}

void
swbcaprtWriteReg32Uc_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr,
    NvV32         value
)
{
    NvU32 i;

    for (i = 0; i < pAperture->numApertures; i++)
        REG_WR32_UC(&pAperture->pApertures[i], addr, value);
}

NvBool
swbcaprtIsRegValid_IMPL
(
    SwBcAperture *pAperture,
    NvU32         addr
)
{

    NvU32 i;

    for (i = 0; i < pAperture->numApertures; i++)
    {
        if (!REG_VALID(&pAperture->pApertures[i], addr))
            return NV_FALSE;
    }

    return NV_TRUE;
}

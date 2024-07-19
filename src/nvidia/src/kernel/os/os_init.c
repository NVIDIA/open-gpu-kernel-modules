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

/***************************** HW State Routines ***************************\
*                                                                           *
*         Common Operating System Object Function Pointer Initializations.  *
*         All the function pointers in the OS object are initialized here.  *
*         The initializations are broken into 'bite-sized' sub-functions    *
*         for ease of reading. Any functions that are common among all OS's *
*         are directly initialized to the common function name. However,    *
*         the actual code for that function may be different from one OS    *
*         to the other; each OS compiles separately. Any function pointers  *
*         that are either not used by some OS's or are initialized to       *
*         different functions by different OS's are 'stubbed' out by        *
*         initializing them to a 'stub' function.                           *
\***************************************************************************/

#include "os/os.h"
#include "os/os_stub.h"
#include "core/system.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "nv_ref.h"
#include "virtualization/hypervisor/hypervisor.h"


#include "gpu/bif/kernel_bif.h"

#include "kernel/gpu/rc/kernel_rc.h"
#include "platform/nbsi/nbsi_read.h"

// Bug check code string common to all OS
const char *ppOsBugCheckBugcodeStr[] = OS_BUG_CHECK_BUGCODE_STR;

NV_STATUS
constructObjOS(OBJOS *pOS)
{
    // Now call the OS specific initialization
    osInitObjOS(pOS);

    return NV_OK;
}

//
// Function to find the maximum number of cores in the system
//
NvU32 osGetMaximumCoreCount(void)
{
    //
    // Windows provides an API to query this that supports CPU hotadd that our
    // cpuid() didn't catch, so favor that.
    //
#if NVOS_IS_WINDOWS && PORT_IS_KERNEL_BUILD && !defined(NV_MODS)
    extern unsigned long KeQueryMaximumProcessorCountEx(unsigned short);
    return KeQueryMaximumProcessorCountEx(0xFFFF); // All groups.
#else
    OBJSYS *pSys = SYS_GET_INSTANCE();
    return pSys ? pSys->cpuInfo.maxLogicalCpus : 0;
#endif
}

/*!
 * @brief Generic OS 8-bit GPU register write function.
 *
 * This function first obtains the pointer to the mapping for the GPU
 * registers and then calls the OS specific osDevWriteReg008 function.
 *
 * @param[in] pGpu          - The GPU context specific to this call.
 * @param[in] thisAddress   - Address of the register to be written
 * @param[in] thisValue     - Value to be written
 *
 */
void osGpuWriteReg008(
     OBJGPU  *pGpu,
     NvU32    thisAddress,
     NvU8     thisValue
)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    osDevWriteReg008(pGpu, pMapping, thisAddress, thisValue);
}

/*!
 * @brief Generic OS 16-bit GPU register write function.
 *
 * This function first obtains the pointer to the mapping for the GPU
 * registers and then calls the OS specific osDevWriteReg016 function.
 *
 * @param[in] pGpu          - The GPU context specific to this call.
 * @param[in] thisAddress   - Address of the register to be written
 * @param[in] thisValue     - Value to be written
 *
 */
void osGpuWriteReg016(
     OBJGPU  *pGpu,
     NvU32    thisAddress,
     NvV16    thisValue
)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    osDevWriteReg016(pGpu, pMapping, thisAddress, thisValue);
}

/*!
 * @brief Generic OS 32-bit GPU register write function.
 *
 * This function first obtains the pointer to the mapping for the GPU
 * registers and then calls the OS specific osDevWriteReg032 function.
 *
 * @param[in] pGpu          - The GPU context specific to this call.
 * @param[in] thisAddress   - Address of the register to be written
 * @param[in] thisValue     - Value to be written
 *
 */
void osGpuWriteReg032(
     OBJGPU  *pGpu,
     NvU32    thisAddress,
     NvV32    thisValue
)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    osDevWriteReg032(pGpu, pMapping, thisAddress, thisValue);
}

/*!
 * @brief Generic OS 8-bit GPU register read function.
 *
 * This function first obtains the pointer to the mapping for the GPU
 * registers and then calls the OS specific osDevReadReg008 function.
 *
 * @param[in] pGpu          - The GPU context specific to this call.
 * @param[in] thisAddress   - Address of the register to be read.
 *
 * @return  The value read from the register
 */
NvU8 osGpuReadReg008(
    OBJGPU *pGpu,
    NvU32   thisAddress
)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    return osDevReadReg008(pGpu, pMapping, thisAddress);
}

/*!
 * @brief Generic OS 16-bit GPU register read function.
 *
 * This function first obtains the pointer to the mapping for the GPU
 * registers and then calls the OS specific osDevReadReg016 function.
 *
 * @param[in] pGpu          - The GPU context specific to this call.
 * @param[in] thisAddress   - Address of the register to be read.
 *
 * @return  The value read from the register
 */
NvU16 osGpuReadReg016(
    OBJGPU  *pGpu,
    NvU32    thisAddress
)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    return osDevReadReg016(pGpu, pMapping, thisAddress);
}

/*!
 * @brief Generic OS 32-bit GPU register read function.
 *
 * This function first obtains the pointer to the mapping for the GPU
 * registers and then calls the OS specific osDevReadReg032 function.
 *
 * @param[in] pGpu          - The GPU context specific to this call.
 * @param[in] thisAddress   - Address of the register to be read.
 *
 * @return  The value read from the register
 */
NvU32 osGpuReadReg032(
    OBJGPU *pGpu,
    NvU32   thisAddress
)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    return osDevReadReg032(pGpu, pMapping, thisAddress);
}

void vgpuDevWriteReg032(
        OBJGPU  *pGpu,
        NvU32    thisAddress,
        NvV32    thisValue,
        NvBool   *vgpuHandled
)
{
    NvBool         isPCIConfigAccess = NV_FALSE;
    NvU32          offAddr = 0;
    NvU32          configSpaceSize;
    NvU32          configSpaceMirrorBase = 0;
    NvU32          configSpaceMirrorSize = 0;
    OBJSYS        *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    if(!pGpu ||
       !pHypervisor || !pHypervisor->bDetected || !pHypervisor->bIsHVMGuest ||
       !GPU_GET_KERNEL_BIF(pGpu))
    {
        *vgpuHandled = NV_FALSE;
        return;
    }

    NV_ASSERT_OK(kbifGetPciConfigSpacePriMirror_HAL(pGpu, GPU_GET_KERNEL_BIF(pGpu),
                                                    &configSpaceMirrorBase, &configSpaceMirrorSize));

    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        configSpaceSize = configSpaceMirrorSize;
    }
    else
    {
        configSpaceSize = NV_CONFIG_PCI_NV_12;
    }

    if ((thisAddress >= configSpaceMirrorBase) &&
        (thisAddress + sizeof(NvU32) <=  configSpaceMirrorBase + configSpaceSize))
    {
        isPCIConfigAccess = NV_TRUE;
        offAddr = thisAddress - configSpaceMirrorBase;
    }

    if ((isPCIConfigAccess) && (pGpu->hPci))
    {
        if (IS_PASSTHRU(pGpu) &&
            !gpuIsBar1Size64Bit(pGpu) &&
            (offAddr == NV_CONFIG_PCI_NV_6) &&
            !gpuIsBar2MovedByVtd(pGpu))
        {
            //
            // Xen doesn't move the BAR2 according to the BAR1 size, so
            // we have to manually jump to 0x1C.
            //
            osPciWriteDword(pGpu->hPci, NV_CONFIG_PCI_NV_7(0), thisValue);
            *vgpuHandled = NV_TRUE;
            return;
        }

        //
        // Avoid calling OS Pci config functions during ISR
        // As this is not allowed in Windows.
        //
        if (IS_VIRTUAL_WITH_SRIOV(pGpu) && osIsISR())
        {
            *vgpuHandled = NV_TRUE;
            return;
        }

        // use config cycles to write these PCI offsets
        osPciWriteDword(pGpu->hPci, offAddr, thisValue);
        *vgpuHandled = NV_TRUE;
        return;
    }

    *vgpuHandled = NV_FALSE;
}

NvU32 vgpuDevReadReg032(
        OBJGPU *pGpu,
        NvU32   thisAddress,
        NvBool  *vgpuHandled
)
{
    NvBool         isPCIConfigAccess = NV_FALSE;
    NvU32          offAddr = 0;
    NvU32          configSpaceSize;
    NvU32          configSpaceMirrorBase = 0;
    NvU32          configSpaceMirrorSize = 0;
    OBJSYS        *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    if(!pGpu ||
       !pHypervisor || !pHypervisor->bDetected || !pHypervisor->bIsHVMGuest ||
       !GPU_GET_KERNEL_BIF(pGpu))
    {
        *vgpuHandled = NV_FALSE;
        return 0;
    }

    NV_ASSERT_OK(kbifGetPciConfigSpacePriMirror_HAL(pGpu, GPU_GET_KERNEL_BIF(pGpu),
                                                    &configSpaceMirrorBase, &configSpaceMirrorSize));

    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        configSpaceSize = configSpaceMirrorSize;
    }
    else
    {
        configSpaceSize = NV_CONFIG_PCI_NV_12;
    }

    if ((thisAddress >= configSpaceMirrorBase) &&
        (thisAddress + sizeof(NvU32) <=  configSpaceMirrorBase + configSpaceSize))
    {
        isPCIConfigAccess = NV_TRUE;
        offAddr = thisAddress - configSpaceMirrorBase;
    }

    if ((isPCIConfigAccess) && (pGpu->hPci))
    {
        if (IS_PASSTHRU(pGpu) &&
            !gpuIsBar1Size64Bit(pGpu) &&
            (offAddr == NV_CONFIG_PCI_NV_6) &&
            !gpuIsBar2MovedByVtd(pGpu))
        {
            //
            // Xen doesn't move the BAR2 according to the BAR1 size, so
            // we have to manually jump to 0x1C.
            //
            *vgpuHandled = NV_TRUE;
            return osPciReadDword(pGpu->hPci, NV_CONFIG_PCI_NV_7(0));
        }

        //
        // Avoid calling OS Pci config functions during ISR
        // As this is not allowed in Windows.
        //
        if (IS_VIRTUAL_WITH_SRIOV(pGpu) && osIsISR())
        {
            *vgpuHandled = NV_TRUE;
            return 0;
        }
        // use config cycles to read these PCI offsets
        *vgpuHandled = NV_TRUE;
        return  osPciReadDword(pGpu->hPci, offAddr);
    }

    *vgpuHandled = NV_FALSE;
    return 0;
}

NvU64 osGetMaxUserVa(void);

/**
 * @brief Get the Max User VA address shift
 *
 * The max user VA address shift may not be power-2 aligned,
 * so do some math to round it up.
 *
 * @return max user VA address shift
 */
NvU32
osGetCpuVaAddrShift(void)
{
    NvU64 maxUserVa = osGetMaxUserVa();

    //
    // Add 1 to account for kernel VA space, on the assumption
    // that kernel VA space is the top half of the address space.
    //
    return (64 - portUtilCountLeadingZeros64(maxUserVa - 1)) + 1;
}

/**
 * @brief Adds a filter to trap a certain CPU virtual address range
 *
 * Sets up a filter so all accesses to an address range are sent through the
 * specified callback.
 *
 * Only one filter is allowed for any given address.
 *
 * @param[in] rangeStart  start of CPU address range (inclusive)
 * @param[in] rangeEnd end of CPU address range (inclusive)
 * @param[in] pCb Callback function
 * @param[in] pPriv opaque point to data pass to callback
 *
 * @return NV_OK is success, appropriate error otherwise.
 */
NV_STATUS
osMemAddFilter
(
    NvU64           rangeStart,
    NvU64           rangeEnd,
    OSMemFilterCb  *pCb,
    void           *pPriv
)
{
    OBJSYS             *pSys = SYS_GET_INSTANCE();
    POSMEMFILTERDATA    pFilterData = NULL;

    pFilterData = portMemAllocNonPaged(sizeof(OSMEMFILTERDATA));
    if (pFilterData == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to alloc mem for os mem filter data!\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pFilterData->node.keyStart = rangeStart;
    pFilterData->node.keyEnd   = rangeEnd;
    pFilterData->pPriv         = pPriv;
    pFilterData->pFilterCb     = pCb;
    pFilterData->node.Data     = (void *)pFilterData;

    return btreeInsert(&pFilterData->node, &pSys->pMemFilterList);
}

/**
 * @brief Remove a filter added with @ref osMemAddFilter
 *
 * @param[in] rangeStart memory address to remove filter from.
 */
NV_STATUS
osMemRemoveFilter
(
    NvU64       rangeStart
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    PNODE       pNode = NULL;

    if (btreeSearch(rangeStart, &pNode, pSys->pMemFilterList) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Failed to find filter data for the given range start address!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT(pNode);
    NV_ASSERT(pNode->keyStart == rangeStart);

    if (btreeUnlink(pNode, &pSys->pMemFilterList) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to unlink filter data!\n");
        return NV_ERR_INVALID_STATE;
    }

    portMemFree(pNode->Data);
    pNode = NULL;

    return NV_OK;
}

/**
 * @brief Retrieves a filter added with @ref osMemAddFilter.
 *
 * @param[in] address Address to search for
 *
 * @return Appropriate filter data if a filter exists, NULL otherwise.
 */
POSMEMFILTERDATA
osMemGetFilter(NvUPtr address)
{
    OBJSYS     *pSys;
    PNODE       pNode = NULL;

    pSys = SYS_GET_INSTANCE();
    if (!pSys)
        return NULL;

    if (btreeSearch(address, &pNode, pSys->pMemFilterList) != NV_OK)
        return NULL;

    return pNode->Data;
}

/*!
 * Some data such as Bindata array are placed on paged memory.  Access to paged segment
 * on high IRQL is not allowed on some platform (e.g. Windows).  The issue could be
 * difficult to debug as the repro rate is random.  The failure only happens when the
 * target segment is paged out.
 *
 * This utility function checks whether it is safe to access paged segments.  When the
 * function is called at high IRQL path, it gives an assertion with a message.  On
 * developer branches, such as chips_a, it triggers an intended Bugcheck.
 *
 * @param[in]   void             No input required
 *
 * @return      void             To avoid random failure, do not return and check the error
 *                               code of this function.  BSOD D1 or internal BSOD provides
 *                               full call stack that is much helpful for debugging.
 */

void osPagedSegmentAccessCheck(void)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJOS     *pOS  = SYS_GET_OS(pSys);

    // check whether it is safe to access/alloc Paged memory
    if (! portMemExSafeForPagedAlloc() || pOS->getProperty(pOS, PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS))
    {
        NV_ASSERT_FAILED("Paged memory access is prohibited");

    }
}

/*!
 * @brief Retrieves a registry key DWORD value and returns the best result
 *        from both nbsi and os registry tables.
 *
 * @param[in]  OBJGPU pointer
 * @param[in]  pRegParmStr Registry key string
 * @param[out] pData Registry key DWORD value
 *
 * @return NV_OK if key was found and data returned in pData
 * @return Other unexpected errors
 */
NV_STATUS osReadRegistryDword
(
    OBJGPU *pGpu,
    const char *pRegParmStr,
    NvU32  *pData
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pRegParmStr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pData != NULL, NV_ERR_INVALID_ARGUMENT);

    status = osReadRegistryDwordBase(pGpu, pRegParmStr, pData);
    if (status != NV_OK)
    {
        status = nbsiReadRegistryDword(pGpu, pRegParmStr, pData);
    }

    return status;
}

/*!
 * @brief Retrieves a registry key STRING value and returns the best result
 *        from both nbsi and os registry tables.
 *
 * @param[in]  OBJGPU pointer
 * @param[in]  pRegParmStr Registry key string
 * @param[out] pData Registry key STRING value
 * @param[in]  pCbLen Count of bytes in registry value.
 *
 * @return NV_OK if key was found and data returned in pData
 * @return Other unexpected errors
 */
NV_STATUS osReadRegistryString
(
    OBJGPU  *pGpu,
    const char *pRegParmStr,
    NvU8    *pData,
    NvU32   *pCbLen
)
{
    NV_STATUS status;
    NV_ASSERT_OR_RETURN(pRegParmStr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCbLen != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!(*pCbLen != 0 && pData == NULL), NV_ERR_INVALID_ARGUMENT);

    status = osReadRegistryStringBase(pGpu, pRegParmStr, pData, pCbLen);
    if (status != NV_OK)
    {
        status = nbsiReadRegistryString(pGpu, pRegParmStr, pData, pCbLen);
    }

    return status;
}

void nvErrorLog(void *pVoid, NvU32 num, const char *pFormat, va_list arglist)
{
    if ((pFormat == NULL) || (*pFormat == '\0'))
    {
        return;
    }

    OBJGPU    *pGpu    = reinterpretCast(pVoid, OBJGPU *);

#if RMCFG_MODULE_SMBPBI || \
    (RMCFG_MODULE_KERNEL_RC && !RMCFG_FEATURE_PLATFORM_GSP)
    char *errorString = portMemAllocNonPaged(MAX_ERROR_STRING);
    if (errorString == NULL)
        goto done;

    unsigned msglen;
    va_list arglistCpy;

    va_copy(arglistCpy, arglist);
    msglen = nvDbgVsnprintf(errorString, MAX_ERROR_STRING, pFormat, arglistCpy);
    va_end(arglistCpy);

    if (msglen == 0)
        goto done;

    {
        KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
        if (pKernelRc != NULL)
            krcReportXid(pGpu, pKernelRc, num, errorString);
    }

done:
    portMemFree(errorString);
#endif // RMCFG_MODULE_SMBPBI || (RMCFG_MODULE_KERNEL_RC &&
       // !RMCFG_FEATURE_PLATFORM_GSP)

    osErrorLogV(pGpu, num, pFormat, arglist);
}

void
nvErrorLog_va
(
    void * pVoid,
    NvU32 num,
    const char * pFormat,
    ...
)
{
    va_list arglist;

    va_start(arglist, pFormat);
    nvErrorLog(pVoid, num, pFormat, arglist);
    va_end(arglist);
}

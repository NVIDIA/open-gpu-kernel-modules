/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: os_stubs.c                                                        *
*           Stubs for all the public stub routines                          *
\***************************************************************************/

#include "os/os_stub.h"

//
// Here's a little debugging tool. It is possible that some code is stubbed for
// certain OS's that shouldn't be. In debug mode, the stubs below will dump out
// a stub 'number' to help you identify any stubs that are getting called. You
// can then evaluate whether or not that is correct.
//
// Highest used STUB_CHECK is 237.
//
#if defined(DEBUG)
#define STUB_CHECK(n) _stubCallCheck(n)

int enableOsStubCallCheck = 0;

static void _stubCallCheck(int funcNumber)
{
    if (enableOsStubCallCheck) {
        NV_PRINTF(LEVEL_INFO, "STUB CALL: %d \r\n", funcNumber);
    }
}

#else
#define STUB_CHECK(n)
#endif // DEBUG

struct OBJCL;

void stubOsQADbgRegistryInit(OBJOS *pOS)
{
    STUB_CHECK(61);
}

NvU32 stubOsnv_rdcr4(OBJOS *pOS)
{
    STUB_CHECK(76);
    return 0;
}

NvU64 stubOsnv_rdxcr0(OBJOS *pOs)
{
    STUB_CHECK(237);
    return 0;
}

int stubOsnv_cpuid(OBJOS *pOS, int arg1, int arg2, NvU32 *arg3,
                   NvU32 *arg4, NvU32 *arg5, NvU32 *arg6)
{
    STUB_CHECK(77);
    return 0;
}

NvU32 stubOsnv_rdmsr(OBJOS *pOS, NvU32 arg1, NvU32 *arg2, NvU32 *arg3)
{
    STUB_CHECK(122);
    return 0;
}

NvU32 stubOsnv_wrmsr(OBJOS *pOS, NvU32 arg1, NvU32 arg2, NvU32 arg3)
{
    STUB_CHECK(123);
    return 0;
}

NvU32 stubOsRobustChannelsDefaultState(OBJOS *pOS)
{
    STUB_CHECK(128);
    return 0;
}

NV_STATUS stubOsQueueWorkItem(OBJGPU *pGpu, OSWorkItemFunction pFunction, void * pParms)
{
    STUB_CHECK(180);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsQueueSystemWorkItem(OSSystemWorkItemFunction pFunction, void *pParms)
{
    STUB_CHECK(181);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsQueueWorkItemWithFlags(OBJGPU *pGpu, OSWorkItemFunction pFunction, void * pParms, NvU32 flags)
{
    STUB_CHECK(182);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsSimEscapeWrite(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, NvU32 Value)
{
    STUB_CHECK(195);
    return NV_ERR_GENERIC;
}

NV_STATUS stubOsSimEscapeWriteBuffer(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, void* pBuffer)
{
    STUB_CHECK(197);
    return NV_ERR_GENERIC;
}

NV_STATUS stubOsSimEscapeRead(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, NvU32 *Value)
{
    STUB_CHECK(196);
    return NV_ERR_GENERIC;
}

NV_STATUS stubOsSimEscapeReadBuffer(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, void* pBuffer)
{
    STUB_CHECK(198);
    return NV_ERR_GENERIC;
}

NV_STATUS stubOsCallACPI_MXMX(OBJGPU *pGpu, NvU32 AcpiId, NvU8 *pInOut)
{
    STUB_CHECK(220);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_DDC(OBJGPU *pGpu, NvU32 ulAcpiId, NvU8 *pOut, NvU32 *size, NvBool bReadMultiBlock)
{
    STUB_CHECK(221);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_BCL(OBJGPU *pGpu, NvU32 acpiId, NvU32 *pOut, NvU16 *size)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_ON(OBJGPU *pGpu, NvU32 uAcpiId)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_OFF(OBJGPU *pGpu, NvU32 uAcpiId)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_NBPS(OBJGPU *pGpu, NvU8 *pOut, NvU32 *pOutSize)
{
    *pOutSize = 0;
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_NBSL(OBJGPU *pGpu, NvU32 val)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_OPTM_GPUON(OBJGPU *pGpu)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_GPUON(OBJGPU *pGpu, NvU32 *pInOut)
{
    //STUB_CHECK(225);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_GPUOFF(OBJGPU *pGpu, NvU32 *pInOut)
{
    //STUB_CHECK(226);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_GPUSTA(OBJGPU *pGpu, NvU32 *pInOut)
{
    //STUB_CHECK(227);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_MXDS(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    //STUB_CHECK(228);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_MXMX(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    //STUB_CHECK(229);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_DOS(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    //STUB_CHECK(230);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_ROM(OBJGPU *pGpu, NvU32 *pIn, NvU32 *pOut)
{
    //STUB_CHECK(231);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_NVHG_DCS(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    //STUB_CHECK(232);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallWMI_DOD(OBJGPU *pGpu, NvU32 *pInOut, NvU32 *pOutSize)
{
    //STUB_CHECK(233);
    return NV_ERR_NOT_SUPPORTED;
}


NV_STATUS stubOsCallACPI_DSM(OBJGPU *pGpu, ACPI_DSM_FUNCTION  acpiDSMFunction, NvU32 NVHGDSMSubfunction, NvU32 *pInOut, NvU16 *size)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_MXDS(OBJGPU *pGpu, NvU32 ulAcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_MXDM(OBJGPU *pGpu, NvU32 ulAcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_MXID(OBJGPU *pGpu, NvU32 ulAcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS stubOsCallACPI_LRST(OBJGPU *pGpu, NvU32 ulAcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool stubOsCheckCallback(OBJGPU *pGpu)
{
    return NV_FALSE;
}

RC_CALLBACK_STATUS
stubOsRCCallback
(
    OBJGPU  *pGpu,
    NvHandle hClient,    // IN The client RC is on
    NvHandle hDevice,    // IN The device RC is on
    NvHandle hFifo,      // IN The channel or TSG RC is on
    NvHandle hChannel,   // IN The channel RC is on
    NvU32    errorLevel, // IN Error Level
    NvU32    errorType,  // IN Error type
    NvU32   *data,      // IN/OUT context of RC handler
    void    *pfnRmRCReenablePusher
)
{
    return RC_CALLBACK_IGNORE;
}

NV_STATUS stubOsSetupVBlank(OBJGPU *pGpu, void * pProc,
                       void * pParm1, void * pParm2, NvU32 Head, void * pParm3)
{
    return NV_OK;
}

NV_STATUS stubOsObjectEventNotification(NvHandle hClient, NvHandle hObject, NvU32 hClass, PEVENTNOTIFICATION pNotifyEvent,
                                    NvU32 notifyIndex, void * pEventData, NvU32 eventDataSize)
{
    return NV_ERR_NOT_SUPPORTED;
}

RmPhysAddr
stubOsPageArrayGetPhysAddr(OS_GPU_INFO *pOsGpuInfo, void* pPageData, NvU32 pageIndex)
{
    NV_ASSERT(0);
    return 0;
}

void stubOsInternalReserveAllocCallback(NvU64 offset, NvU64 size, NvU32 gpuId)
{
    return;
}

void stubOsInternalReserveFreeCallback(NvU64 offset, NvU32 gpuId)
{
    return;
}


NV_STATUS osVgpuAllocVmbusEventDpc(void **ppArg1)
{
    return NV_OK;
}

void osVgpuScheduleVmbusEventDpc(void *pArg1, void *pArg2)
{
}

NV_STATUS osLockPageableDataSection(RM_PAGEABLE_SECTION *pSection)
{
    return NV_OK;
}

NV_STATUS osUnlockPageableDataSection(RM_PAGEABLE_SECTION *pSection)
{
    return NV_OK;
}

NV_STATUS osIsKernelBuffer(void *pArg1, NvU32 arg2)
{
    return NV_OK;
}

NV_STATUS osMapViewToSection(OS_GPU_INFO  *pArg1,
                             void         *pSectionHandle,
                             void         **ppAddress,
                             NvU64         actualSize,
                             NvU64         sectionOffset,
                             NvBool        bIommuEnabled)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osUnmapViewFromSection(OS_GPU_INFO *pArg1,
                                 void *pAddress,
                                 NvBool bIommuEnabled)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osSrPinSysmem(
    OS_GPU_INFO  *pArg1,
    NvU64         commitSize,
    void         *pMdl
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osSrUnpinSysmem(OS_GPU_INFO  *pArg1)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCreateMemFromOsDescriptorInternal(
    OBJGPU       *pGpu,
    void         *pAddress,
    NvU32         flags,
    NvU64         size,
    MEMORY_DESCRIPTOR **ppMemDesc,
    NvBool        bCachedKernel,
    RS_PRIV_LEVEL privilegeLevel
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osReserveCpuAddressSpaceUpperBound(void **ppSectionHandle,
                                             NvU64 maxSectionSize)
{
    return NV_ERR_NOT_SUPPORTED;
}

void osReleaseCpuAddressSpaceUpperBound(void *pSectionHandle)
{
}

void osIoWriteDword(
    NvU32         port,
    NvU32         data
)
{
}

NvU32 osIoReadDword(
    NvU32         port
)
{
    return 0;
}

NvBool osIsVga(
    OS_GPU_INFO  *pArg1,
    NvBool        bIsGpuPrimaryDevice
)
{
    return bIsGpuPrimaryDevice;
}

void osInitOSHwInfo(
    OBJGPU       *pGpu
)
{
}

void osDestroyOSHwInfo(
    OBJGPU       *pGpu
)
{
}

NV_STATUS osDoFunctionLevelReset(
    OBJGPU *pGpu
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool osGrService(
    OS_GPU_INFO    *pOsGpuInfo,
    NvU32           grIdx,
    NvU32           intr,
    NvU32           nstatus,
    NvU32           addr,
    NvU32           dataLo
)
{
    return NV_FALSE;
}

NvBool osDispService(
    NvU32         Intr0,
    NvU32         Intr1
)
{
    return NV_FALSE;
}

NV_STATUS osDeferredIsr(
    OBJGPU       *pGpu
)
{
    return NV_OK;
}

NV_STATUS osGetAcpiTable(
    NvU32         tableSignature,
    void         **ppTable,
    NvU32         tableSize,
    NvU32        *retSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osInitGetAcpiTable(void)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 osGetReleaseAssertBehavior(void)
{
    return 0;
}

void osDbgBugCheckOnAssert(void)
{
    return;
}

NvBool osQueueDpc(OBJGPU *pGpu)
{
    return NV_FALSE;
}

NvBool osBugCheckOnTimeoutEnabled(void)
{
    return NV_FALSE;
}

NV_STATUS osNvifMethod(
    OBJGPU       *pGpu,
    NvU32         func,
    NvU32         subFunc,
    void         *pInParam,
    NvU16         inParamSize,
    NvU32        *pOutStatus,
    void         *pOutData,
    NvU16        *pOutDataSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osNvifInitialize(
    OBJGPU       *pGpu
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
stubOsGetUefiVariable
(
    OBJGPU *pGpu,
    char   *pName,
    LPGUID  pGuid,
    NvU8   *pBuffer,
    NvU32  *pSize,
    NvU32  *pAttributes
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 osGetDynamicPowerSupportMask(void)
{
    return 0;
}

void osUnrefGpuAccessNeeded(OS_GPU_INFO *pOsGpuInfo)
{
    return;
}

NV_STATUS osRefGpuAccessNeeded(OS_GPU_INFO *pOsGpuInfo)
{
    return NV_OK;
}

void osClientGcoffDisallowRefcount(
    OS_GPU_INFO  *pArg1,
    NvBool        arg2
)
{
}

#if !RMCFG_FEATURE_PLATFORM_DCE /* dce_core_rm_clk_reset.c */ && \
    (!RMCFG_FEATURE_PLATFORM_UNIX || !RMCFG_FEATURE_TEGRA_SOC_NVDISPLAY || \
     RMCFG_FEATURE_DCE_CLIENT_RM /* osSocNvDisp.c */ )
NV_STATUS
osTegraSocEnableClk
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32     whichClkRM
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocDisableClk
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                   whichClkRM
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocGetCurrFreqKHz
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                   whichClkRM,
    NvU32                   *pCurrFreqKHz
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocGetMaxFreqKHz
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                    whichClkRM,
    NvU32                   *pMaxFreqKHz
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocGetMinFreqKHz
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                    whichClkRM,
    NvU32                   *pMinFreqKHz
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocSetFreqKHz
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                    whichClkRM,
    NvU32                    reqFreqKHz
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocSetParent
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                    whichClkRMsource,
    NvU32                    whichClkRMparent
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocGetParent
(
    OS_GPU_INFO             *pOsGpuInfo,
    NvU32                    whichClkRMsource,
    NvU32                   *pWhichClkRMparent
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocDeviceReset
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocPmPowergate
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocPmUnpowergate
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}
#endif


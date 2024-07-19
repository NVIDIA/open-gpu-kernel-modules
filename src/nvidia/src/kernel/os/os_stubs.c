/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

void osQADbgRegistryInit(void)
{
    return;
}

#if !(RMCFG_FEATURE_PLATFORM_WINDOWS && NVCPU_IS_X86_64) &&            \
    !(RMCFG_FEATURE_PLATFORM_UNIX && NVCPU_IS_X86_64) &&               \
    !RMCFG_FEATURE_PLATFORM_MODS
NvU32 osNv_rdcr4(void)
{
    return 0;
}
#endif

NvU64 osNv_rdxcr0(void)
{
    return 0;
}

#if !(RMCFG_FEATURE_PLATFORM_WINDOWS && NVCPU_IS_X86_64) &&            \
    !(RMCFG_FEATURE_PLATFORM_UNIX && NVCPU_IS_X86_64) &&               \
    !RMCFG_FEATURE_PLATFORM_MODS
int osNv_cpuid(int arg1, int arg2, NvU32 *arg3,
               NvU32 *arg4, NvU32 *arg5, NvU32 *arg6)
{
    return 0;
}
#endif

NV_STATUS osSimEscapeWrite(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, NvU32 Value)
{
    return NV_ERR_GENERIC;
}

NV_STATUS osSimEscapeWriteBuffer(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, void* pBuffer)
{
    return NV_ERR_GENERIC;
}

NV_STATUS osSimEscapeRead(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, NvU32 *Value)
{
    return NV_ERR_GENERIC;
}

NV_STATUS osSimEscapeReadBuffer(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, void* pBuffer)
{
    return NV_ERR_GENERIC;
}

NV_STATUS osCallACPI_MXMX(OBJGPU *pGpu, NvU32 AcpiId, NvU8 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_BCL(OBJGPU *pGpu, NvU32 acpiId, NvU32 *pOut, NvU16 *size)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_OPTM_GPUON(OBJGPU *pGpu)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_GPUON(OBJGPU *pGpu, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_GPUOFF(OBJGPU *pGpu, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_GPUSTA(OBJGPU *pGpu, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_MXDS(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_MXMX(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_DOS(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_NVHG_DCS(OBJGPU *pGpu, NvU32 AcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_MXID(OBJGPU *pGpu, NvU32 ulAcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osCallACPI_LRST(OBJGPU *pGpu, NvU32 ulAcpiId, NvU32 *pInOut)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool osCheckCallback(OBJGPU *pGpu)
{
    return NV_FALSE;
}

RC_CALLBACK_STATUS
osRCCallback
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

NvBool osCheckCallback_v2(OBJGPU *pGpu)
{
    return NV_FALSE;
}

RC_CALLBACK_STATUS
osRCCallback_v2
(
    OBJGPU  *pGpu,
    NvHandle hClient,    // IN The client RC is on
    NvHandle hDevice,    // IN The device RC is on
    NvHandle hFifo,      // IN The channel or TSG RC is on
    NvHandle hChannel,   // IN The channel RC is on
    NvU32    errorLevel, // IN Error Level
    NvU32    errorType,  // IN Error type
    NvBool   bDeferRcRequested, // IN defer RC state
    NvU32   *data,      // IN/OUT context of RC handler
    void    *pfnRmRCReenablePusher
)
{
    return RC_CALLBACK_IGNORE;
}

NV_STATUS osSetupVBlank(OBJGPU *pGpu, void * pProc,
                        void * pParm1, void * pParm2, NvU32 Head, void * pParm3)
{
    return NV_OK;
}

RmPhysAddr
osPageArrayGetPhysAddr(OS_GPU_INFO *pOsGpuInfo, void* pPageData, NvU32 pageIndex)
{
    NV_ASSERT(0);
    return 0;
}

void osInternalReserveAllocCallback(NvU64 offset, NvU64 size, NvU32 gpuId)
{
}

void osInternalReserveFreeCallback(NvU64 offset, NvU32 gpuId)
{
}

NV_STATUS osGetCurrentProcessGfid(NvU32 *pGfid)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osSetRegistryList(nv_reg_entry_t *pRegList)
{
    return NV_ERR_NOT_SUPPORTED;
}

nv_reg_entry_t *osGetRegistryList(void)
{
    return NULL;
}

#if !(RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_PLATFORM_DCE) || \
    (RMCFG_FEATURE_PLATFORM_UNIX && !RMCFG_FEATURE_TEGRA_SOC_NVDISPLAY)
NV_STATUS osTegraSocGpioGetPinState(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32        *pArg3
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void osTegraSocGpioSetPinState(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32         arg3
)
{
}

NV_STATUS osTegraSocGpioSetPinDirection(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32         arg3
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osTegraSocGpioGetPinDirection(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32        *pArg3
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osTegraSocGpioGetPinNumber(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32        *pArg3
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osTegraSocGpioGetPinInterruptStatus(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32         arg3,
    NvBool       *pArg4
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osTegraSocGpioSetPinInterrupt(
    OS_GPU_INFO  *pArg1,
    NvU32         arg2,
    NvU32         arg3
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocResetMipiCal
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osGetValidWindowHeadMask
(
    OS_GPU_INFO *pArg1,
    NvU64 *pWindowHeadMask
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool
osTegraSocIsDsiPanelConnected
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return NV_FALSE;
}

NV_STATUS
osTegraSocDsiParsePanelProps
(
    OS_GPU_INFO *pOsGpuInfo,
    void        *dsiPanelInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocDsiPanelEnable
(
    OS_GPU_INFO *pOsGpuInfo,
    void        *dsiPanelInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocDsiPanelReset
(
    OS_GPU_INFO *pOsGpuInfo,
    void        *dsiPanelInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void
osTegraSocDsiPanelDisable
(
    OS_GPU_INFO *pOsGpuInfo,
    void        *dsiPanelInfo
)
{
    return;
}

void
osTegraSocDsiPanelCleanup
(
    OS_GPU_INFO *pOsGpuInfo,
    void        *dsiPanelInfo
)
{
    return;
}

NV_STATUS
osTegraSocHspSemaphoreAcquire
(
    NvU32 ownerId,
    NvBool bAcquire,
    NvU64 timeout
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool
osTegraSocGetHdcpEnabled(OS_GPU_INFO *pOsGpuInfo)
{
    return NV_TRUE;
}

void
osTegraGetDispSMMUStreamIds
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32       *dispIsoStreamId,
    NvU32       *dispNisoStreamId
)
{
    /* NV_U32_MAX is used to indicate that the platform does not support SMMU */
    *dispIsoStreamId = NV_U32_MAX;
    *dispNisoStreamId = NV_U32_MAX;
}
#endif

NV_STATUS
osTegraSocParseFixedModeTimings
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32 dcbIndex,
    NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pTimingsPerStream,
    NvU8 *pNumTimings
)
{
    return NV_OK;
}

NV_STATUS
osTegraSocPowerManagement
(
    OS_GPU_INFO *pOsGpuInfo,
    NvBool bInPMTransition,
    NvU32 newPMLevel
)
{
    return NV_OK;
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

void osSetSurfaceName(
    void *pDescriptor,
    char *name
)
{
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

NvBool osDbgBreakpointEnabled(void)
{
    return NV_TRUE;
}

NV_STATUS
osGetSysmemInfo
(
    OBJGPU *pGpu,
    NvU64  *pSysmemBaseAddr,
    NvU64  *pSysmemTotalSize
)
{
    // Bug 4377373 - TODO: Need to add proper implementation for non MODS platform.
    *pSysmemBaseAddr = 0;
    *pSysmemTotalSize = (1ULL << 32);

    return NV_OK;
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
osGetUefiVariable
(
    const char *pName,
    LPGUID      pGuid,
    NvU8       *pBuffer,
    NvU32      *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osGetNvGlobalRegistryDword
(
    OBJGPU     *pGpu,
    const char *pRegParmStr,
    NvU32      *pData
)
{
    return NV_ERR_NOT_SUPPORTED;
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

#if (RMCFG_FEATURE_PLATFORM_WINDOWS && !RMCFG_FEATURE_TEGRA_BPMP) || \
    (!RMCFG_FEATURE_PLATFORM_WINDOWS && !RMCFG_FEATURE_TEGRA_SOC_NVDISPLAY)
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

NvU32
osTegraSocFuseRegRead(NvU32 addr)
{
    return 0;
}
#endif

#if !(RMCFG_FEATURE_PLATFORM_UNIX) || \
    (RMCFG_FEATURE_PLATFORM_UNIX && !RMCFG_FEATURE_TEGRA_SOC_NVDISPLAY)
NV_STATUS
osTegraSocDpUphyPllInit(OS_GPU_INFO *pOsGpuInfo, NvU32 link_rate, NvU32 lanes)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraSocDpUphyPllDeInit(OS_GPU_INFO *pOsGpuInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

#endif


NV_STATUS osGetPcieCplAtomicsCaps
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32       *pMask
)
{
    return NV_ERR_NOT_SUPPORTED;
}

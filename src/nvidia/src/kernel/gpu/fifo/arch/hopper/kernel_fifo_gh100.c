/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "published/hopper/gh100/dev_fault.h"
#include "published/hopper/gh100/dev_vm.h"

/*!
 * Checks the USERD and GPFIFO/PushBuffer location attributes
 * to make sure that when USERD is in sysmem, GPFIFO or
 * PushBuffer is not in vidmem.
 *
 * @param[in]   pKernelFifo         KernelFifo pointer
 * @param[in]   userdAddrSpace      USERD address space
 * @param[in]   pushBuffAddrSpace   PushBuffer address space
 * @param[in]   gpFifoAddrSpace     GPFIFO address space
 *
 */
NV_STATUS
kfifoCheckChannelAllocAddrSpaces_GH100
(
    KernelFifo       *pKernelFifo,
    NV_ADDRESS_SPACE  userdAddrSpace,
    NV_ADDRESS_SPACE  pushBuffAddrSpace,
    NV_ADDRESS_SPACE  gpFifoAddrSpace
)
{
    if ((userdAddrSpace == ADDR_SYSMEM) &&
        ((pushBuffAddrSpace == ADDR_FBMEM || gpFifoAddrSpace == ADDR_FBMEM)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

/*
 * Returns VA alloc flags to be passed into dmaMapBuffer.
 *
 * @param[in]   pGpu               OBJGPU pointer
 * @param[in]   pKernelFifo        KernelFifo pointer
 * @param[in]   engine             engine tells us which engine's ctx buffer are we getting the flags for
 * @param[out]  pflags             NvU32* returned alloc flags to pass into dmaMapBuffer.
 */
void
kfifoGetCtxBufferMapFlags_GH100
(
    OBJGPU      *pGpu,
    KernelFifo  *pKernelFifo,
    NvU32        engine,
    NvU32       *pflags
)
{
    if (IS_GR(engine))
        *pflags |= DMA_VA_LIMIT_57B;

    return;
}

/*
 * Allocate Memory Descriptors for BAR1 VF pages
 *
 * @param[in]   pGpu               OBJGPU pointer
 * @param[in]   pKernelFifo        KernelFifo pointer
 */
NV_STATUS
kfifoConstructUsermodeMemdescs_GH100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32 attr                    = 0;
    NvU32 attr2                   = 0;
    NvU32 i                       = 0;
    NV_STATUS status              = NV_OK;

    attr = FLD_SET_DRF(OS32, _ATTR,  _PHYSICALITY, _CONTIGUOUS, attr);
    attr = FLD_SET_DRF(OS32, _ATTR,  _COHERENCY, _CACHED, attr);

    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, attr2 );

    for(i = 0; i < 2; i++)
    {
        NvBool bPriv = (i==0);
        NvU64 offset = bPriv ? DRF_BASE(NV_VIRTUAL_FUNCTION_PRIV) :  DRF_BASE(NV_VIRTUAL_FUNCTION);
        NvU64 size   = bPriv ? DRF_SIZE(NV_VIRTUAL_FUNCTION_PRIV) : DRF_SIZE(NV_VIRTUAL_FUNCTION);
        MEMORY_DESCRIPTOR **ppMemDesc = bPriv ? &(pKernelFifo->pBar1PrivVF) : &(pKernelFifo->pBar1VF);
        NV_ASSERT_OK_OR_GOTO(status,
            memCreateMemDesc(pGpu, ppMemDesc, ADDR_SYSMEM, offset, size, attr, attr2),
            err);
        memdescSetPteKind(*ppMemDesc, memmgrGetMessageKind_HAL(pGpu, pMemoryManager));
        memdescSetFlag(*ppMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1, NV_TRUE);
        memdescSetPageSize(*ppMemDesc, AT_GPU, RM_PAGE_SIZE);
    }

    NV_ASSERT_OK_OR_GOTO(status,
        kfifoConstructUsermodeMemdescs_GV100(pGpu, pKernelFifo),
        err);
    return NV_OK;
err:
    memdescDestroy(pKernelFifo->pBar1VF);
    memdescDestroy(pKernelFifo->pBar1PrivVF);
    return status;
}

/*!
 * @brief Converts an mmu engine id (NV_PFAULT_MMU_ENG_ID_*) into a string.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] engineID NV_PFAULT_MMU_ENG_ID_*
 *
 * @returns a string (always non-null)
 */
const char*
kfifoPrintInternalEngine_GH100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    NV_STATUS status = NV_OK;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32 pbdmaId;
    NvU32 engTag;

    if (kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, engineID))
    {
        NV_ASSERT_OR_RETURN(kfifoGetPbdmaIdFromMmuFaultId(pGpu, pKernelFifo, engineID, &pbdmaId) == NV_OK, "UNKNOWN");
        return kfifoPrintPbdmaId_HAL(pGpu, pKernelFifo, pbdmaId);
    }

    if (kgmmuIsFaultEngineBar1_HAL(pKernelGmmu, engineID))
    {
        return "BAR1";
    }
    else if (kgmmuIsFaultEngineBar2_HAL(pKernelGmmu, engineID))
    {
        return "BAR2";
    }

    switch (engineID)
    {
        case NV_PFAULT_MMU_ENG_ID_DISPLAY:
            return "DISPLAY";
        case NV_PFAULT_MMU_ENG_ID_PERF:
            return "PERF";
        case NV_PFAULT_MMU_ENG_ID_PWR_PMU:
            return "PMU";
        case NV_PFAULT_MMU_ENG_ID_PTP:
            return "PTP";
        case NV_PFAULT_MMU_ENG_ID_FLA:
            return "FLA";
        case NV_PFAULT_MMU_ENG_ID_GSP:
            return "GSP";
        case NV_PFAULT_MMU_ENG_ID_FSP:
            return "FSP";
        default:
            break;
    }

    status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_MMU_FAULT_ID,
                                      engineID, ENGINE_INFO_TYPE_ENG_DESC, &engTag);
    if (status == NV_OK)
    {
        switch (engTag)
        {
            case ENG_GR(0):
                return "GRAPHICS";
            default:
                return kfifoGetEngineName_HAL(pKernelFifo, ENGINE_INFO_TYPE_ENG_DESC, engTag);
        }
    }

    return "UNKNOWN";
}

/**
 * @brief Converts a subid/clientid into a client string.
 *        This common function may be used by subsequent chips
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pMmuExceptData

 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdStringCommon_GH100
(
    OBJGPU                  *pGpu,
    KernelFifo              *pKernelFifo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptInfo
)
{
    if (pMmuExceptInfo->bGpc)
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_GPC_T1_0:
                return "GPCCLIENT_T1_0";
            case NV_PFAULT_CLIENT_GPC_T1_1:
                return "GPCCLIENT_T1_1";
            case NV_PFAULT_CLIENT_GPC_T1_2:
                return "GPCCLIENT_T1_2";
            case NV_PFAULT_CLIENT_GPC_T1_3:
                return "GPCCLIENT_T1_3";
            case NV_PFAULT_CLIENT_GPC_T1_4:
                return "GPCCLIENT_T1_4";
            case NV_PFAULT_CLIENT_GPC_T1_5:
                return "GPCCLIENT_T1_5";
            case NV_PFAULT_CLIENT_GPC_T1_6:
                return "GPCCLIENT_T1_6";
            case NV_PFAULT_CLIENT_GPC_T1_7:
                return "GPCCLIENT_T1_7";
            case NV_PFAULT_CLIENT_GPC_PE_0:
                return "GPCCLIENT_PE_0";
            case NV_PFAULT_CLIENT_GPC_PE_1:
                return "GPCCLIENT_PE_1";
            case NV_PFAULT_CLIENT_GPC_PE_2:
                return "GPCCLIENT_PE_2";
            case NV_PFAULT_CLIENT_GPC_PE_3:
                return "GPCCLIENT_PE_3";
            case NV_PFAULT_CLIENT_GPC_PE_4:
                return "GPCCLIENT_PE_4";
            case NV_PFAULT_CLIENT_GPC_PE_5:
                return "GPCCLIENT_PE_5";
            case NV_PFAULT_CLIENT_GPC_PE_6:
                return "GPCCLIENT_PE_6";
            case NV_PFAULT_CLIENT_GPC_PE_7:
                return "GPCCLIENT_PE_7";
            case NV_PFAULT_CLIENT_GPC_RAST:
                return "GPCCLIENT_RAST";
            case NV_PFAULT_CLIENT_GPC_GCC:
                return "GPCCLIENT_GCC";
            case NV_PFAULT_CLIENT_GPC_GPCCS:
                return "GPCCLIENT_GPCCS";
            case NV_PFAULT_CLIENT_GPC_PROP_0:
                return "GPCCLIENT_PROP_0";
            case NV_PFAULT_CLIENT_GPC_PROP_1:
                return "GPCCLIENT_PROP_1";
            case NV_PFAULT_CLIENT_GPC_T1_8:
                return "GPCCLIENT_T1_8";
            case NV_PFAULT_CLIENT_GPC_T1_9:
                return "GPCCLIENT_T1_9";
            case NV_PFAULT_CLIENT_GPC_T1_10:
                return "GPCCLIENT_T1_10";
            case NV_PFAULT_CLIENT_GPC_T1_11:
                return "GPCCLIENT_T1_11";
            case NV_PFAULT_CLIENT_GPC_T1_12:
                return "GPCCLIENT_T1_12";
            case NV_PFAULT_CLIENT_GPC_T1_13:
                return "GPCCLIENT_T1_13";
            case NV_PFAULT_CLIENT_GPC_T1_14:
                return "GPCCLIENT_T1_14";
            case NV_PFAULT_CLIENT_GPC_T1_15:
                return "GPCCLIENT_T1_15";
            case NV_PFAULT_CLIENT_GPC_TPCCS_0:
                return "GPCCLIENT_TPCCS_0";
            case NV_PFAULT_CLIENT_GPC_TPCCS_1:
                return "GPCCLIENT_TPCCS_1";
            case NV_PFAULT_CLIENT_GPC_TPCCS_2:
                return "GPCCLIENT_TPCCS_2";
            case NV_PFAULT_CLIENT_GPC_TPCCS_3:
                return "GPCCLIENT_TPCCS_3";
            case NV_PFAULT_CLIENT_GPC_TPCCS_4:
                return "GPCCLIENT_TPCCS_4";
            case NV_PFAULT_CLIENT_GPC_TPCCS_5:
                return "GPCCLIENT_TPCCS_5";
            case NV_PFAULT_CLIENT_GPC_TPCCS_6:
                return "GPCCLIENT_TPCCS_6";
            case NV_PFAULT_CLIENT_GPC_TPCCS_7:
                return "GPCCLIENT_TPCCS_7";
            case NV_PFAULT_CLIENT_GPC_PE_8:
                return "GPCCLIENT_PE_8";
            case NV_PFAULT_CLIENT_GPC_TPCCS_8:
                return "GPCCLIENT_TPCCS_8";
            case NV_PFAULT_CLIENT_GPC_T1_16:
                return "GPCCLIENT_T1_16";
            case NV_PFAULT_CLIENT_GPC_T1_17:
                return "GPCCLIENT_T1_17";
            case NV_PFAULT_CLIENT_GPC_ROP_0:
                return "GPCCLIENT_ROP_0";
            case NV_PFAULT_CLIENT_GPC_ROP_1:
                return "GPCCLIENT_ROP_1";
            case NV_PFAULT_CLIENT_GPC_GPM:
                return "GPCCLIENT_GPM";
            default:
                return "UNRECOGNIZED_CLIENT";
        }
    }
    else
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_HUB_VIP:
                return "HUBCLIENT_VIP";
            case NV_PFAULT_CLIENT_HUB_CE0:
                return "HUBCLIENT_CE0";
            case NV_PFAULT_CLIENT_HUB_CE1:
                return "HUBCLIENT_CE1";
            case NV_PFAULT_CLIENT_HUB_CE2:
                return "HUBCLIENT_CE2";
            case NV_PFAULT_CLIENT_HUB_CE3:
                return "HUBCLIENT_CE3";
            case NV_PFAULT_CLIENT_HUB_DNISO:
                return "HUBCLIENT_DNISO";
            case NV_PFAULT_CLIENT_HUB_FE:
                return "HUBCLIENT_FE";
            case NV_PFAULT_CLIENT_HUB_FECS:
                return "HUBCLIENT_FECS";
            case NV_PFAULT_CLIENT_HUB_HOST:
                return "HUBCLIENT_HOST";
            case NV_PFAULT_CLIENT_HUB_HOST_CPU:
                return "HUBCLIENT_HOST_CPU";
            case NV_PFAULT_CLIENT_HUB_HOST_CPU_NB:
                return "HUBCLIENT_HOST_CPU_NB";
            case NV_PFAULT_CLIENT_HUB_ISO:
                return "HUBCLIENT_ISO";
            case NV_PFAULT_CLIENT_HUB_MMU:
                return "HUBCLIENT_MMU";
            case NV_PFAULT_CLIENT_HUB_NVDEC0:
                return "HUBCLIENT_NVDEC0";
            case NV_PFAULT_CLIENT_HUB_NVENC1:
                return "HUBCLIENT_NVENC1";
            case NV_PFAULT_CLIENT_HUB_NISO:
                return "HUBCLIENT_NISO";
            case NV_PFAULT_CLIENT_HUB_P2P:
                return "HUBCLIENT_P2P";
            case NV_PFAULT_CLIENT_HUB_PD:
                return "HUBCLIENT_PD";
            case NV_PFAULT_CLIENT_HUB_PERF0:
                return "HUBCLIENT_PERF";
            case NV_PFAULT_CLIENT_HUB_PMU:
                return "HUBCLIENT_PMU";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD:
                return "HUBCLIENT_RASTERTWOD";
            case NV_PFAULT_CLIENT_HUB_SCC:
                return "HUBCLIENT_SCC";
            case NV_PFAULT_CLIENT_HUB_SCC_NB:
                return "HUBCLIENT_SCC_NB";
            case NV_PFAULT_CLIENT_HUB_SEC:
                return "HUBCLIENT_SEC";
            case NV_PFAULT_CLIENT_HUB_SSYNC:
                return "HUBCLIENT_SSYNC";
            case NV_PFAULT_CLIENT_HUB_XV:
                return "HUBCLIENT_XV";
            case NV_PFAULT_CLIENT_HUB_MMU_NB:
                return "HUBCLIENT_MMU_NB";
            case NV_PFAULT_CLIENT_HUB_DFALCON:
                return "HUBCLIENT_DFALCON";
            case NV_PFAULT_CLIENT_HUB_SKED:
                return "HUBCLIENT_SKED";
            case NV_PFAULT_CLIENT_HUB_DONT_CARE:
                return "HUBCLIENT_DONT_CARE";
            case NV_PFAULT_CLIENT_HUB_HSCE0:
                return "HUBCLIENT_HSCE0";
            case NV_PFAULT_CLIENT_HUB_HSCE1:
                return "HUBCLIENT_HSCE1";
            case NV_PFAULT_CLIENT_HUB_HSCE2:
                return "HUBCLIENT_HSCE2";
            case NV_PFAULT_CLIENT_HUB_HSCE3:
                return "HUBCLIENT_HSCE3";
            case NV_PFAULT_CLIENT_HUB_HSCE4:
                return "HUBCLIENT_HSCE4";
            case NV_PFAULT_CLIENT_HUB_HSCE5:
                return "HUBCLIENT_HSCE5";
            case NV_PFAULT_CLIENT_HUB_HSCE6:
                return "HUBCLIENT_HSCE6";
            case NV_PFAULT_CLIENT_HUB_HSCE7:
                return "HUBCLIENT_HSCE7";
            case NV_PFAULT_CLIENT_HUB_HSHUB:
                return "HUBCLIENT_HSHUB";
            case NV_PFAULT_CLIENT_HUB_PTP_X0:
                return "HUBCLIENT_PTP_X0";
            case NV_PFAULT_CLIENT_HUB_PTP_X1:
                return "HUBCLIENT_PTP_X1";
            case NV_PFAULT_CLIENT_HUB_PTP_X2:
                return "HUBCLIENT_PTP_X2";
            case NV_PFAULT_CLIENT_HUB_PTP_X3:
                return "HUBCLIENT_PTP_X3";
            case NV_PFAULT_CLIENT_HUB_PTP_X4:
                return "HUBCLIENT_PTP_X4";
            case NV_PFAULT_CLIENT_HUB_PTP_X5:
                return "HUBCLIENT_PTP_X5";
            case NV_PFAULT_CLIENT_HUB_PTP_X6:
                return "HUBCLIENT_PTP_X6";
            case NV_PFAULT_CLIENT_HUB_PTP_X7:
                return "HUBCLIENT_PTP_X7";
            case NV_PFAULT_CLIENT_HUB_NVENC2:
                return "HUBCLIENT_NVENC2";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER0:
                return "HUBCLIENT_VPR_SCRUBBER0";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER1:
                return "HUBCLIENT_VPR_SCRUBBER1";
            case NV_PFAULT_CLIENT_HUB_FBFALCON:
                return "HUBCLIENT_FBFALCON";
            case NV_PFAULT_CLIENT_HUB_CE_SHIM:
                return "HUBCLIENT_CE_SHIM";
            case NV_PFAULT_CLIENT_HUB_GSP:
                return "HUBCLIENT_GSP";
            case NV_PFAULT_CLIENT_HUB_FSP:
                return "HUBCLIENT_FSP";
            case NV_PFAULT_CLIENT_HUB_NVDEC1:
                return "HUBCLIENT_NVDEC1";
            case NV_PFAULT_CLIENT_HUB_NVDEC2:
                return "HUBCLIENT_NVDEC2";
            case NV_PFAULT_CLIENT_HUB_NVJPG0:
                return "HUBCLIENT_NVJPG0";
            case NV_PFAULT_CLIENT_HUB_NVJPG1:
                return "HUBCLIENT_NVJPG1";
            case NV_PFAULT_CLIENT_HUB_NVJPG2:
                return "HUBCLIENT_NVJPG2";
            case NV_PFAULT_CLIENT_HUB_NVJPG3:
                return "HUBCLIENT_NVJPG3";
            case NV_PFAULT_CLIENT_HUB_NVJPG4:
                return "HUBCLIENT_NVJPG4";
            case NV_PFAULT_CLIENT_HUB_NVJPG5:
                return "HUBCLIENT_NVJPG5";
            case NV_PFAULT_CLIENT_HUB_NVJPG6:
                return "HUBCLIENT_NVJPG6";
            case NV_PFAULT_CLIENT_HUB_NVJPG7:
                return "HUBCLIENT_NVJPG7";
            case NV_PFAULT_CLIENT_HUB_NVDEC3:
                return "HUBCLIENT_NVDEC3";
            case NV_PFAULT_CLIENT_HUB_NVDEC4:
                return "HUBCLIENT_NVDEC4";
            case NV_PFAULT_CLIENT_HUB_NVDEC5:
                return "HUBCLIENT_NVDEC5";
            case NV_PFAULT_CLIENT_HUB_NVDEC6:
                return "HUBCLIENT_NVDEC6";
            case NV_PFAULT_CLIENT_HUB_NVDEC7:
                return "HUBCLIENT_NVDEC7";
            case NV_PFAULT_CLIENT_HUB_OFA0:
                return "HUBCLIENT_OFA0";
            case NV_PFAULT_CLIENT_HUB_FE1:
                return "HUBCLIENT_FE1";
            case NV_PFAULT_CLIENT_HUB_FE2:
                return "HUBCLIENT_FE2";
            case NV_PFAULT_CLIENT_HUB_FE3:
                return "HUBCLIENT_FE3";
            case NV_PFAULT_CLIENT_HUB_FE4:
                return "HUBCLIENT_FE4";
            case NV_PFAULT_CLIENT_HUB_FE5:
                return "HUBCLIENT_FE5";
            case NV_PFAULT_CLIENT_HUB_FE6:
                return "HUBCLIENT_FE6";
            case NV_PFAULT_CLIENT_HUB_FE7:
                return "HUBCLIENT_FE7";
            case NV_PFAULT_CLIENT_HUB_FECS1:
                return "HUBCLIENT_FECS1";
            case NV_PFAULT_CLIENT_HUB_FECS2:
                return "HUBCLIENT_FECS2";
            case NV_PFAULT_CLIENT_HUB_FECS3:
                return "HUBCLIENT_FECS3";
            case NV_PFAULT_CLIENT_HUB_FECS4:
                return "HUBCLIENT_FECS4";
            case NV_PFAULT_CLIENT_HUB_FECS5:
                return "HUBCLIENT_FECS5";
            case NV_PFAULT_CLIENT_HUB_FECS6:
                return "HUBCLIENT_FECS6";
            case NV_PFAULT_CLIENT_HUB_FECS7:
                return "HUBCLIENT_FECS7";
            case NV_PFAULT_CLIENT_HUB_SKED1:
                return "HUBCLIENT_SKED1";
            case NV_PFAULT_CLIENT_HUB_SKED2:
                return "HUBCLIENT_SKED2";
            case NV_PFAULT_CLIENT_HUB_SKED3:
                return "HUBCLIENT_SKED3";
            case NV_PFAULT_CLIENT_HUB_SKED4:
                return "HUBCLIENT_SKED4";
            case NV_PFAULT_CLIENT_HUB_SKED5:
                return "HUBCLIENT_SKED5";
            case NV_PFAULT_CLIENT_HUB_SKED6:
                return "HUBCLIENT_SKED6";
            case NV_PFAULT_CLIENT_HUB_SKED7:
                return "HUBCLIENT_SKED7";
            case NV_PFAULT_CLIENT_HUB_ESC:
                return "HUBCLIENT_ESC";
            default:
                return kfifoGetClientIdStringCheck_HAL(pGpu, pKernelFifo, pMmuExceptInfo->clientId);
        }
    }
}

/**
 * @brief Converts a subid/clientid into a client string
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pMmuExceptData
 *
 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdString_GH100
(
    OBJGPU                  *pGpu,
    KernelFifo              *pKernelFifo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionInfo
)
{
    if (pMmuExceptionInfo->bGpc)
    {
        return kfifoGetClientIdStringCommon_HAL(pGpu, pKernelFifo, pMmuExceptionInfo);
    }
    else
    {
        switch (pMmuExceptionInfo->clientId)
        {
            case NV_PFAULT_CLIENT_HUB_AFALCON:
                return "HUBCLIENT_AFALCON";
            case NV_PFAULT_CLIENT_HUB_HSCE8:
                return "HUBCLIENT_HSCE8";
            case NV_PFAULT_CLIENT_HUB_HSCE9:
                return "HUBCLIENT_HSCE9";
            case NV_PFAULT_CLIENT_HUB_DWBIF:
                return "HUBCLIENT_DWBIF";
            case NV_PFAULT_CLIENT_HUB_HSCE10:
                return "HUBCLIENT_HSCE10";
            case NV_PFAULT_CLIENT_HUB_HSCE11:
                return "HUBCLIENT_HSCE11";
            case NV_PFAULT_CLIENT_HUB_HSCE12:
                return "HUBCLIENT_HSCE12";
            case NV_PFAULT_CLIENT_HUB_HSCE13:
                return "HUBCLIENT_HSCE13";
            case NV_PFAULT_CLIENT_HUB_HSCE14:
                return "HUBCLIENT_HSCE14";
            case NV_PFAULT_CLIENT_HUB_HSCE15:
                return "HUBCLIENT_HSCE15";
            default:
                return kfifoGetClientIdStringCommon_HAL(pGpu, pKernelFifo, pMmuExceptionInfo);
        }
    }
}

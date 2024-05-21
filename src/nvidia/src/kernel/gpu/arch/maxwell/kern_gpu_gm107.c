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

#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/gpu_child_class_defs.h"
#include "gpu_mgr/gpu_mgr_sli.h"
#include "gpu/bif/kernel_bif.h"
#include "jt.h"
#include "published/maxwell/gm107/dev_bus.h"
#include "published/maxwell/gm107/dev_nv_xve.h"
#include "published/maxwell/gm107/dev_nv_xve1.h"
#include "published/maxwell/gm107/dev_fuse.h"
#include "published/maxwell/gm107/dev_pri_ringstation_sys.h"

/*!
 * @brief Returns SR-IOV capabilities
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[out] pParams        Pointer for get_sriov_caps params
 *
 * @returns NV_OK always
 */
NV_STATUS
gpuGetSriovCaps_GM107
(
    OBJGPU *pGpu,
    NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams
)
{
    pParams->bSriovEnabled = NV_FALSE;
    return NV_OK;
}

/*!
 * @brief Read fuse for display supported status.
 *        Some chips not marked displayless do not support display
 */
NvBool
gpuFuseSupportsDisplay_GM107
(
    OBJGPU *pGpu
)
{
    return GPU_FLD_TEST_DRF_DEF(pGpu, _FUSE, _STATUS_OPT_DISPLAY, _DATA, _ENABLE);
}

/*!
 * @brief gpuReadBusConfigRegEx_GM107
 *
 * param[in] pGpu         The GPU object pointer
 * param[in] index        NvU32
 * param[in] *data        NvU32 *
 * param[in] pThreadState thread state node pointer
 */
NV_STATUS
gpuReadBusConfigRegEx_GM107
(
    OBJGPU            *pGpu,
    NvU32              index,
    NvU32             *data,
    THREAD_STATE_NODE *pThreadState
)
{

    if (index > (PCIE_CONFIG_SPACE_SIZE - sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Offset 0x%08x exceeds range!\n",
                  index);
        NV_ASSERT(0);
        return NV_ERR_GENERIC;
    }

    *data = GPU_REG_RD32_EX(pGpu, DEVICE_BASE(NV_PCFG) + index, pThreadState);

    return NV_OK;
}

/*!
 * @brief gpuReadBusConfigReg_GM107()
 *
 * param[in] pGpu       The GPU object pointer
 * param[in] index      NvU32
 * param[in] *data      NvU32 *
 */
NV_STATUS
gpuReadBusConfigReg_GM107
(
    OBJGPU *pGpu,
    NvU32   index,
    NvU32  *data
)
{
    return gpuReadBusConfigRegEx_HAL(pGpu, index, data, NULL);
}

/*!
 * @brief gpuWriteBusConfigReg_GM107
 *
 * param[in] pGpu       The GPU object pointer
 * param[in] index      NvU32
 * param[in] value      NvU32
 */
NV_STATUS
gpuWriteBusConfigReg_GM107
(
    OBJGPU *pGpu,
    NvU32   index,
    NvU32   value
)
{

    if (index > (PCIE_CONFIG_SPACE_SIZE - sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Offset 0x%08x exceeds range!\n",
                  index);
        NV_ASSERT(0);
        return NV_ERR_GENERIC;
    }

    GPU_REG_WR32(pGpu, DEVICE_BASE(NV_PCFG) + index, value);

    return NV_OK;
}

NV_STATUS
gpuReadFunctionConfigReg_GM107
(
    OBJGPU *pGpu,
    NvU32   function,
    NvU32   index,
    NvU32  *data
)
{
    NvU32  retval;

    if (index > (PCIE_CONFIG_SPACE_SIZE - sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_ERROR, "Offset 0x%08x exceeds range!\n", index);

        return NV_ERR_GENERIC;
    }

    switch (function)
    {
        case 0:
        {
            retval = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + index);
            break;
        }

        case 1:
        {
            if (IS_FMODEL(pGpu))
            {
                //
                // Function 1 is not modeled on fmodel
                //
                *data = 0;
                return NV_OK;
            }
            else
            {
                retval = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG1) + index);
            }
            break;
        }

        default:
            NV_PRINTF(LEVEL_ERROR,
                      "attempt to read cfg space of non-existant function %x\n",
                      function);
            return NV_ERR_GENERIC;
    }

    *data = retval;
    return NV_OK;
}


NV_STATUS
gpuWriteFunctionConfigReg_GM107
(
    OBJGPU *pGpu,
    NvU32   function,
    NvU32   index,
    NvU32   data
)
{
    return gpuWriteFunctionConfigRegEx_HAL(pGpu, function, index, data,
                                           NULL /* threadstate */);
}

NV_STATUS
gpuWriteFunctionConfigRegEx_GM107
(
    OBJGPU            *pGpu,
    NvU32              function,
    NvU32              index,
    NvU32              data,
    THREAD_STATE_NODE *pThreadState
)
{
    if (index > (PCIE_CONFIG_SPACE_SIZE - sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_ERROR, "Offset 0x%08x exceeds range!\n", index);

        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (function)
    {
        case 0:
        {
            GPU_REG_WR32_EX(pGpu, DEVICE_BASE(NV_PCFG) + index, data, pThreadState);
            break;
        }

        case 1:
        {
            //
            // Function 1 is not modeled on fmodel
            //
            if (!IS_FMODEL(pGpu))
            {
                GPU_REG_WR32_EX(pGpu, DEVICE_BASE(NV_PCFG1) + index, data, pThreadState);
            }
            break;
        }

        default:
            NV_PRINTF(LEVEL_ERROR,
                      "attempt to read cfg space of non-existant function %x\n",
                      function);
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief Perform gpu-dependent error handling for error during register read sanity check
 *
 * @param[in]       pGpu        GPU object pointer
 * @param[in]       addr        Value address
 * @param[in]       value       Value read during check
 */
void
gpuHandleSanityCheckRegReadError_GM107
(
    OBJGPU *pGpu,
    NvU32 addr,
    NvU32 value
)
{
#if NV_PRINTF_ENABLED
    //
    // Read the interrupt status using the direct OS reg read call so we don't recurs
    // if we happen to see GPU_READ_PRI_ERROR_CODE there as well (bug 799876)
    //
    NvU32 intr = osGpuReadReg032(pGpu, NV_PBUS_INTR_0);

    // To be sure, filter this down further by checking the related pri interrupts:
    if (FLD_TEST_DRF(_PBUS, _INTR_0, _PRI_SQUASH,  _PENDING, intr) ||
        FLD_TEST_DRF(_PBUS, _INTR_0, _PRI_FECSERR, _PENDING, intr) ||
        FLD_TEST_DRF(_PBUS, _INTR_0, _PRI_TIMEOUT, _PENDING, intr))
    {
#if NV_PRINTF_STRINGS_ALLOWED
        const char *errorString = "Unknown SYS_PRI_ERROR_CODE";

        gpuGetSanityCheckRegReadError_HAL(pGpu, value,
                                          &errorString);
        NV_PRINTF(LEVEL_ERROR,
                  "Possible bad register read: addr: 0x%x,  regvalue: 0x%x,  error code: %s\n",
                  addr, value, errorString);
#else // NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_ERROR,
                  "Possible bad register read: addr: 0x%x,  regvalue: 0x%x\n",
                  addr, value);
#endif // NV_PRINTF_STRINGS_ALLOWED
    }
#endif // NV_PRINTF_ENABLED
}

void
gpuGetIdInfo_GM107(OBJGPU *pGpu)
{
    NvU32 data;

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_REV_ID, &data))
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_XVE_REV_ID\n");
        return;
    }

    // we only need the FIB and MASK values
    pGpu->idInfo.PCIRevisionID = (data & ~GPU_DRF_SHIFTMASK(NV_XVE_REV_ID_CLASS_CODE));

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_SUBSYSTEM, &data))
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_XVE_SUBSYSTEM\n");
        return;
    }
    pGpu->idInfo.PCISubDeviceID = data;

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_ID, &data))
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_XVE_ID\n");
        return;
    }

    pGpu->idInfo.PCIDeviceID = data;

}

// GM200 used on all later GPUs

//
// Lists the order of GPU children for engine state transitions (StateInit, StateLoad,
// StateUnload and StateDestroy). This list controls only the engine order. Engine
// presence is defined by gpuGetChildrenPresent_HAL. Engines in this list that aren't in the
// gpuGetChildrenPresent_HAL list are ignored.
//
// List entries contain {CLASS-ID, flags} pairs.
//
// Valid flags are:
//   GCO_ALL - entry is used for all list types
//   GCO_LIST_INIT - entry is used for init ordering (DO NOT USE)
//   GCO_LIST_LOAD - entry is used for load and postload ordering (DO NOT USE)
//   GCO_LIST_UNLOAD - entry is used for unload and preunload ordering (DO NOT USE)
//   GCO_LIST_DESTROY - entry is used for destroy order (DO NOT USE)
//
// For UNLOAD and DESTROY the list is played back in reverse from LOAD and INIT.
//
// IMPORTANT:
//   <1> GCO_ALL is the recommended flag to use for all engine types. Engines should
//       always have a consist order. If there are complicated dependencies that cannot
//       be resolved using this list, please use callbacks (such as fifoAddSchedulingHandler)
//   <1> DO NOT FORK THIS LIST. The goal is to have a single ordered list across all
//       chips. Inconsistent ordering makes it challenging to modify shared code to work
//       across all variations.
//
static const GPUCHILDORDER
gpuChildOrderList_GM200[] =
{
    {classId(OBJVBIOS),           GCO_ALL},
    {classId(ConfidentialCompute),      GCO_ALL},
    {classId(Pxuc),               GCO_ALL},
    {classId(OBJBIF),             GCO_ALL},
    {classId(KernelBif),          GCO_ALL},
    {classId(Nne),                GCO_ALL},
    {classId(NvDebugDump),        GCO_ALL},
    {classId(ClockManager),       GCO_ALL},
    {classId(Pmgr),               GCO_ALL},
    {classId(OBJVOLT),            GCO_ALL},
    {classId(OBJMC),              GCO_ALL},
    {classId(KernelMc),           GCO_ALL},
    {classId(PrivRing),           GCO_ALL},
    {classId(SwIntr),             GCO_ALL},
    {classId(Intr),               GCO_ALL},
    {classId(OBJTMR),             GCO_ALL},
    {classId(Therm),              GCO_ALL},
    {classId(OBJHSHUBMANAGER),    GCO_ALL},
    {classId(Hshub),              GCO_ALL},
    {classId(MemorySystem),       GCO_ALL},
    {classId(KernelMemorySystem), GCO_ALL},
    {classId(MemoryManager),      GCO_ALL},
    {classId(Nvlink),             GCO_ALL},
    {classId(KernelNvlink),       GCO_ALL},
    {classId(OBJHDACODEC),        GCO_ALL},
    {classId(OBJGMMU),            GCO_ALL},
    {classId(KernelGmmu),         GCO_ALL},
    {classId(OBJVMMU),            GCO_ALL},
    {classId(KernelSec2),         GCO_ALL},
    {classId(KernelGsp),          GCO_ALL},
    {classId(OBJBUS),             GCO_ALL},
    {classId(KernelBus),          GCO_ALL},
    {classId(OBJLSFM),            GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY}, // LOAD  LSFM must be before ACR and any managed Falcon.
    {classId(OBJACR),             GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY},
    {classId(Pmu),                GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY},
    {classId(KernelPmu),          GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY},
    {classId(Gsp),                GCO_ALL},
    {classId(OBJFSP),             GCO_ALL},
    {classId(KernelFsp),          GCO_ALL},
    {classId(OBJFBFLCN),          GCO_ALL},
    {classId(Lpwr   ),            GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY},
    {classId(Perf),               GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY}, // LOAD Perf is after PMU for perfmon_sampling to work
    {classId(KernelPerf),         GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY},
    {classId(OBJDISP),            GCO_LIST_DESTROY},
    {classId(KernelDisplay),      GCO_LIST_DESTROY},
    {classId(OBJHDA),             GCO_LIST_DESTROY},
    {classId(Fan),             GCO_LIST_DESTROY},
    {classId(VirtMemAllocator),   GCO_ALL},
    {classId(OBJDISP),            GCO_LIST_INIT},
    {classId(KernelDisplay),      GCO_LIST_INIT},
    {classId(OBJHDA),             GCO_LIST_INIT},
    {classId(Fan),             GCO_LIST_INIT},
    {classId(GraphicsManager),    GCO_ALL},
    {classId(MIGManager),         GCO_ALL},
    {classId(KernelMIGManager),   GCO_ALL},
    {classId(KernelGraphicsManager), GCO_ALL},
    {classId(Graphics),           GCO_ALL},                   // INIT GR has to be initialized before LSFM because
                                                            // the ucode image pointers needed by LSFM are only
                                                            // known after GR has loaded the netlist.

    {classId(KernelGraphics),     GCO_ALL},
    {classId(OBJLSFM),            GCO_LIST_INIT},
    {classId(OBJACR),             GCO_LIST_INIT},
    {classId(Pmu),                GCO_LIST_INIT},
    {classId(KernelPmu),          GCO_LIST_INIT},
    {classId(Lpwr   ),            GCO_LIST_INIT},
    {classId(Perf),               GCO_LIST_INIT},
    {classId(KernelPerf),         GCO_LIST_INIT},
    {classId(OBJBSP),             GCO_ALL},
    {classId(OBJCIPHER),          GCO_ALL},
    {classId(OBJDISP),            GCO_LIST_LOAD | GCO_LIST_UNLOAD},    // LOAD Display is *after* cipher so that hdcp keys can be loaded .
    {classId(KernelDisplay),      GCO_LIST_LOAD | GCO_LIST_UNLOAD},    // LOAD Display is *after* cipher so that hdcp keys can be loaded .
    {classId(OBJHDA),             GCO_LIST_LOAD | GCO_LIST_UNLOAD},
    {classId(Fan),             GCO_LIST_LOAD | GCO_LIST_UNLOAD},
    {classId(OBJCE),              GCO_ALL},
    {classId(KernelCE),           GCO_ALL},
    {classId(OBJMSENC),           GCO_ALL},
    {classId(OBJNVJPG),           GCO_ALL},
    {classId(OBJOFA),             GCO_ALL},
    {classId(OBJSEC2),            GCO_ALL},
    {classId(KernelFifo),              GCO_ALL},
    {classId(OBJFIFO),            GCO_ALL},
    {classId(OBJDPAUX),           GCO_ALL},
    {classId(OBJINFOROM),         GCO_ALL},
    {classId(OBJUVM),             GCO_ALL},
    {classId(OBJGPULOG),          GCO_LIST_INIT | GCO_LIST_LOAD},
    {classId(OBJGPUMON),          GCO_ALL},
    {classId(OBJGPULOG),          GCO_LIST_UNLOAD | GCO_LIST_DESTROY},
    {classId(KernelHwpm),         GCO_ALL},
    {classId(OBJHWPM),            GCO_ALL},
    {classId(OBJSWENG),           GCO_ALL},
    {classId(OBJGRIDDISPLAYLESS), GCO_ALL},
    {classId(KernelCcu),      GCO_ALL},
};

const GPUCHILDORDER *
gpuGetChildrenOrder_GM200(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildOrderList_GM200);
    return gpuChildOrderList_GM200;
}

//
// List of GPU children that present for the chip. List entries contain
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This
// list controls only engine presence. Order is defined by
// gpuGetChildrenOrder_HAL.
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT
// FORK THIS LIST!
//
// List entries contain {CLASS-ID, # of instances} pairs.
//
static const GPUCHILDPRESENT gpuChildrenPresent_GM200[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 3),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GM200(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GM200);
    return gpuChildrenPresent_GM200;
}

/*!
 * @brief   checks for each type of bridge to deterimne what is available,
 *          then selects the SLI bridge to use.
 *
 * @param[In]   gpuCount    The number of GPUs to be checked for SLI links.
 * @param[In]   gpuMaskArg  A mask of the GPUs that are to be tested for SLI links.
 * @param[Out]  pSliLinkOutputMask  a mask of the GPUs that are attached to the type of
 *                  SLI link that is being used.
 * @param[Out]  pSliLinkCircular    a boolean indicating if teh SLI link is circular.
 * @param[Out]  pSliLinkEndsMask    a mask indicating the endpoints of the SLI link,
 *                   if there are any.
.*/
void
gpuDetectSliLinkFromGpus_GK104
(
    OBJGPU *pGpu,
    NvU32   gpuCount,
    NvU32   gpuMaskArg,
    NvU32  *pSliLinkOutputMask,
    NvBool *pSliLinkCircular,
    NvU32  *pSliLinkEndsMask,
    NvU32  *pVidLinkCount
)
{
    NvU32       i;
    NvU32       sliLinkOutputMask[SLI_MAX_BRIDGE_TYPES] = {0, 0};
    NvBool      bSliLinkCircular[SLI_MAX_BRIDGE_TYPES]  = {NV_FALSE, NV_FALSE};
    NvU32       sliLinkEndsMask[SLI_MAX_BRIDGE_TYPES]   = {0, 0};
    NvU32       vidLinkCount[SLI_MAX_BRIDGE_TYPES]      = {0, 0};
    OBJSYS     *pSys                                    = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr                                 = SYS_GET_GPUMGR(pSys);
    OBJGPU     *pGpuLoop;
    OBJGPU     *pGpuSaved;
    NvU32       gpuMask;
    // Array to store the link detection HAL flag of GpuDetectVidLinkFromGpus_HAL and GpuDetectNvlinkLinkFromGpus_HAL.
    NvU32       linkHalImpl[SLI_MAX_BRIDGE_TYPES];
    NvBool      bFoundBridge = NV_FALSE;

    // set the return values assuming we will not find an SLI link.
    *pSliLinkOutputMask = 0;
    *pSliLinkCircular   = NV_FALSE;
    *pSliLinkEndsMask   = 0;

    pGpuMgr->gpuBridgeType = SLI_BT_VIDLINK;

    //
    // Link detection HAL should have same HAL implementation as HAL flag.
    // This checks for mismatched HAL implementation flag.
    //
    NV_ASSERT_OR_RETURN_VOID(gpuGetSliLinkDetectionHalFlag_HAL(pGpu) == GPU_LINK_DETECTION_HAL_GK104);

    i = 0;
    gpuMask = gpuMaskArg;
    pGpuLoop = gpumgrGetNextGpu(gpuMask, &i);
    if (pGpuLoop != NULL)
    {
        pGpuSaved = pGpuLoop;
        linkHalImpl[SLI_BT_NVLINK]  = gpuGetNvlinkLinkDetectionHalFlag_HAL(pGpuLoop);

    }
    else
    {
         return;
    }

    // run thru the GPUs and see if they are all using the same HAL functions.
    // if they are different, we can't use the function to check for a bridge
    pGpuLoop = gpumgrGetNextGpu(gpuMask, &i);
    while (NULL != pGpuLoop)
    {
        if (linkHalImpl[SLI_BT_NVLINK] != gpuGetNvlinkLinkDetectionHalFlag_HAL(pGpuLoop))
        {
            linkHalImpl[SLI_BT_NVLINK] = GPU_LINK_DETECTION_HAL_STUB;
        }
        pGpuLoop = gpumgrGetNextGpu(gpuMask, &i);
    }

    if (linkHalImpl[SLI_BT_NVLINK] != GPU_LINK_DETECTION_HAL_STUB)
    {
        gpuDetectNvlinkLinkFromGpus_HAL(pGpuSaved, gpuCount, gpuMaskArg,
                                        &sliLinkOutputMask[SLI_BT_NVLINK],
                                        &bSliLinkCircular[SLI_BT_NVLINK],
                                        &sliLinkEndsMask[SLI_BT_NVLINK],
                                        &vidLinkCount[SLI_BT_NVLINK]);
    }

    //
    // Determine which type of bridge we are going to support.
    // Currently we only support a single type of SLI bridge in the system.
    //
    for (i = 0; i < SLI_MAX_BRIDGE_TYPES; ++i)
    {
        if (sliLinkOutputMask[i] != 0)
        {
            if (bFoundBridge)
            {
                NV_PRINTF(LEVEL_ERROR, "More than one type of SLI bridge detected!\n");
                NV_ASSERT(0);
                break;
            }
            else
            {
                pGpuMgr->gpuBridgeType = (NvU8)i;
                *pSliLinkOutputMask = sliLinkOutputMask[i];
                *pSliLinkCircular = bSliLinkCircular[i];
                *pSliLinkEndsMask = sliLinkEndsMask[i];
                *pVidLinkCount = vidLinkCount[i];
                bFoundBridge = NV_TRUE;
            }
        }
    }
}

/*!
 * @brief Get error that arises during sanity check on a register read value
 *
 * @param[in]       pGpu             GPU object pointer
 * @param[in]       value            Register value
 * @param[out]      pErrorString     Error string pointer
 *
 * @return void
 */
void
gpuGetSanityCheckRegReadError_GM107
(
    OBJGPU *pGpu,
    NvU32 value,
    const char **pErrorString
)
{
#define PRI_ERROR(err)                                    \
if (DRF_VAL(_PPRIV, _SYS_PRI_ERROR, _CODE, value) == err) \
{                                                         \
    *pErrorString = #err;                                 \
}
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_HOST_FECS_ERR);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_HOST_PRI_TIMEOUT);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_HOST_FB_ACK_TIMEOUT);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_TIMEOUT);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_DECODE);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_RESET);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_FLOORSWEEP);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_STUCK_ACK);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_0_EXPECTED_ACK);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_FENCE_ERROR);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_SUBID_ERROR);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_ORPHAN);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_DEAD_RING);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_TRAP);
    PRI_ERROR(NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_CLIENT_ERR);
}

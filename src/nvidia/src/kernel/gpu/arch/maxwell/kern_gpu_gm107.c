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

#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"

#include "published/maxwell/gm107/dev_bus.h"
#include "published/maxwell/gm107/dev_nv_xve.h"
#include "published/maxwell/gm107/dev_nv_xve1.h"
#include "published/maxwell/gm107/dev_fuse.h"

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
    {classId(OBJBIF),             GCO_ALL},
    {classId(KernelBif),          GCO_ALL},
    {classId(OBJNNE),             GCO_ALL},
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
    {classId(OBJHSHUB),           GCO_ALL},
    {classId(MemorySystem),       GCO_ALL},
    {classId(KernelMemorySystem), GCO_ALL},
    {classId(MemoryManager),      GCO_ALL},
    {classId(Nvlink),             GCO_ALL},
    {classId(KernelNvlink),       GCO_ALL},
    {classId(OBJHDACODEC),        GCO_ALL},
    {classId(OBJGMMU),            GCO_ALL},
    {classId(KernelGmmu),         GCO_ALL},
    {classId(OBJVMMU),            GCO_ALL},
    {classId(KernelNvdec),        GCO_ALL},
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
    {classId(OBJFAN),             GCO_LIST_DESTROY},
    {classId(VirtMemAllocator),   GCO_ALL},
    {classId(OBJDISP),            GCO_LIST_INIT},
    {classId(KernelDisplay),      GCO_LIST_INIT},
    {classId(OBJHDA),             GCO_LIST_INIT},
    {classId(OBJFAN),             GCO_LIST_INIT},
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
    {classId(OBJFAN),             GCO_LIST_LOAD | GCO_LIST_UNLOAD},
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
    {classId(OBJHWPM),            GCO_ALL},
    {classId(OBJSWENG),           GCO_ALL},
    {classId(OBJGRIDDISPLAYLESS), GCO_ALL},
};

const GPUCHILDORDER *
gpuGetChildrenOrder_GM200(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildOrderList_GM200);
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
    {classId(OBJSWENG), 1},
    {classId(OBJACR), 1},
    {classId(OBJBIF), 1},
    {classId(KernelBif), 1},
    {classId(OBJBSP), 1},
    {classId(OBJBUS), 1},
    {classId(KernelBus), 1},
    {classId(OBJCE), 3},
    {classId(KernelCE), 3},
    {classId(OBJCIPHER), 1},
    {classId(ClockManager), 1},
    {classId(OBJDISP), 1},
    {classId(KernelDisplay), 1},
    {classId(VirtMemAllocator), 1},
    {classId(OBJDPAUX), 1},
    {classId(OBJFAN), 1},
    {classId(OBJHSHUB), 1},
    {classId(MemorySystem), 1},
    {classId(KernelMemorySystem), 1},
    {classId(MemoryManager), 1},
    {classId(KernelFifo), 1},
    {classId(OBJFIFO), 1},
    {classId(OBJGMMU), 1},
    {classId(KernelGmmu), 1},
    {classId(OBJGPULOG), 1},
    {classId(OBJGPUMON), 1},
    {classId(GraphicsManager), 1},
    {classId(MIGManager), 1},
    {classId(KernelMIGManager), 1},
    {classId(KernelGraphicsManager), 1},
    {classId(Graphics), 1},
    {classId(KernelGraphics), 1},
    {classId(OBJHDA), 1},
    {classId(OBJHDACODEC), 1},
    {classId(OBJHWPM), 1},
    {classId(OBJINFOROM), 1},
    {classId(Intr), 1},
    {classId(Lpwr   ), 1},
    {classId(OBJLSFM), 1},
    {classId(OBJMC), 1},
    {classId(KernelMc), 1},
    {classId(PrivRing), 1},
    {classId(SwIntr), 1},
    {classId(OBJMSENC), 2},
    {classId(NvDebugDump), 1},
    {classId(Perf), 1},
    {classId(KernelPerf), 1},
    {classId(Pmgr), 1},
    {classId(Pmu), 1},
    {classId(KernelPmu), 1},
    {classId(OBJSEC2), 1},
    {classId(Therm), 1},
    {classId(OBJTMR), 1},
    {classId(OBJVOLT), 1},
    {classId(OBJGRIDDISPLAYLESS), 1},
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GM200(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildrenPresent_GM200);
    return gpuChildrenPresent_GM200;
}


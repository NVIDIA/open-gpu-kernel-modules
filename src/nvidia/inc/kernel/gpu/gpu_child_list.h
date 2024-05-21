/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// No include guards - this file is included multiple times, each time with a
// different definition for GPU_CHILD_SINGLE_INST and GPU_CHILD_GPU_CHILD_MULTI_INST
//
// Callers that will use the same definition for single- and multi- instance
// can define GPU_CHILD that will be used for both
//
#if defined(GPU_CHILD)
#if !defined(GPU_CHILD_SINGLE_INST) && !defined(GPU_CHILD_MULTI_INST)
#define GPU_CHILD_SINGLE_INST GPU_CHILD
#define GPU_CHILD_MULTI_INST GPU_CHILD
#else
#error "Must not define GPU_CHILD_{SINGLE,MULTI}_INST and GPU_CHILD at the same time"
#endif
#endif

//
// GPU child list. All objects must inherit from OBJENGSTATE. Objects are
// constructed in the listed order and destructed in reverse order. Storage in
// OBJGPU and accessor macros (i.e.: GET_GPU_XXX) are generated from this list.
//

//
// Temporarily needed to generate stubs for disabled modules
// To be removed when the references to these modules are gone
//
#if defined(GPU_CHILD_LIST_DISABLED_ONLY)
#define GPU_CHILD_MODULE(_rmcfgModule) !RMCFG_MODULE_ENABLED(_rmcfgModule)
#else
#define GPU_CHILD_MODULE(_rmcfgModule) RMCFG_MODULE_ENABLED(_rmcfgModule)
#endif

                        /* Class Name            Accessor  Name                       Max Instances     bConstructEarly  OBJGPU Field */
#if GPU_CHILD_MODULE(FUSE)
    GPU_CHILD_SINGLE_INST( OBJFUSE,              GPU_GET_FUSE,                        1,                NV_TRUE,         pFuse            )
#endif
#if GPU_CHILD_MODULE(BIF)
    GPU_CHILD_SINGLE_INST( OBJBIF,               GPU_GET_BIF,                         1,                NV_TRUE,         pBif             )
#endif
#if GPU_CHILD_MODULE(KERNEL_BIF)
    GPU_CHILD_SINGLE_INST( KernelBif,            GPU_GET_KERNEL_BIF,                  1,                NV_TRUE,         pKernelBif       )
#endif
#if GPU_CHILD_MODULE(NNE)
    GPU_CHILD_SINGLE_INST( Nne,                  GPU_GET_NNE,                         1,                NV_TRUE,         pNne             )
#endif
#if GPU_CHILD_MODULE(MC)
    GPU_CHILD_SINGLE_INST( OBJMC,                GPU_GET_MC,                          1,                NV_FALSE,        pMc              )
#endif
#if GPU_CHILD_MODULE(KERNEL_MC)
    GPU_CHILD_SINGLE_INST( KernelMc,             GPU_GET_KERNEL_MC,                   1,                NV_FALSE,        pKernelMc        )
#endif
#if GPU_CHILD_MODULE(PRIV_RING)
    GPU_CHILD_SINGLE_INST( PrivRing,             GPU_GET_PRIV_RING,                   1,                NV_FALSE,        pPrivRing        )
#endif
#if GPU_CHILD_MODULE(INTR)
    GPU_CHILD_SINGLE_INST( SwIntr,               GPU_GET_SW_INTR,                     1,                NV_FALSE,        pSwIntr          )
#endif
#if GPU_CHILD_MODULE(MEMORY_SYSTEM)
    GPU_CHILD_SINGLE_INST( MemorySystem,         GPU_GET_MEMORY_SYSTEM,               1,                NV_FALSE,        pMemorySystem    )
#endif
#if GPU_CHILD_MODULE(KERNEL_MEMORY_SYSTEM)
    GPU_CHILD_SINGLE_INST( KernelMemorySystem,   GPU_GET_KERNEL_MEMORY_SYSTEM,        1,                NV_FALSE,        pKernelMemorySystem )
#endif
#if GPU_CHILD_MODULE(MEMORY_MANAGER)
    GPU_CHILD_SINGLE_INST( MemoryManager,        GPU_GET_MEMORY_MANAGER,              1,                NV_FALSE,        pMemoryManager   )
#endif
#if GPU_CHILD_MODULE(FBFLCN)
    GPU_CHILD_SINGLE_INST( OBJFBFLCN,            GPU_GET_FBFLCN,                      1,                NV_FALSE,        pFbflcn          )
#endif
#if GPU_CHILD_MODULE(HSHUBMANAGER)
    GPU_CHILD_SINGLE_INST( OBJHSHUBMANAGER,      GPU_GET_HSHUBMANAGER,                 1,               NV_FALSE,        pHshMgr          )
#endif
#if GPU_CHILD_MODULE(HSHUB)
    GPU_CHILD_MULTI_INST ( Hshub,                GPU_GET_HSHUB,                       GPU_MAX_HSHUBS,   NV_FALSE,        pHshub           )
#endif
#if GPU_CHILD_MODULE(SEQ)
    GPU_CHILD_SINGLE_INST( OBJSEQ,               GPU_GET_SEQ,                         1,                NV_FALSE,        pSeq             )
#endif
#if GPU_CHILD_MODULE(GpuMutexMgr)
    GPU_CHILD_SINGLE_INST( GpuMutexMgr,          GPU_GET_MUTEX_MGR,                   1,                NV_FALSE,        pMutexMgr        )
#endif
#if GPU_CHILD_MODULE(KERNEL_DISPLAY)
    GPU_CHILD_SINGLE_INST( KernelDisplay,        GPU_GET_KERNEL_DISPLAY,              1,                NV_FALSE,        pKernelDisplay   )
#endif
#if GPU_CHILD_MODULE(TMR)
    GPU_CHILD_SINGLE_INST( OBJTMR,               GPU_GET_TIMER,                       1,                NV_TRUE,         pTmr             )
#endif
#if GPU_CHILD_MODULE(DISP)
    GPU_CHILD_SINGLE_INST( OBJDISP,              GPU_GET_DISP,                        1,                NV_FALSE,        pDisp            )
#endif
#if GPU_CHILD_MODULE(BUS)
    GPU_CHILD_SINGLE_INST( OBJBUS,               GPU_GET_BUS,                         1,                NV_FALSE,        pBus             )
#endif
#if GPU_CHILD_MODULE(KERNEL_BUS)
    GPU_CHILD_SINGLE_INST( KernelBus,            GPU_GET_KERNEL_BUS,                  1,                NV_FALSE,        pKernelBus       )
#endif
#if GPU_CHILD_MODULE(GMMU)
    GPU_CHILD_SINGLE_INST( OBJGMMU,              GPU_GET_GMMU,                        1,                NV_FALSE,        pGmmu            )
#endif
#if GPU_CHILD_MODULE(KERNEL_GMMU)
    GPU_CHILD_SINGLE_INST( KernelGmmu,           GPU_GET_KERNEL_GMMU,                 1,                NV_FALSE,        pKernelGmmu      )
#endif
#if GPU_CHILD_MODULE(KERNEL_SEC2)
    GPU_CHILD_SINGLE_INST( KernelSec2,           GPU_GET_KERNEL_SEC2,                 1,                NV_FALSE,        pKernelSec2      )
#endif
#if GPU_CHILD_MODULE(KERNEL_GSP)
    GPU_CHILD_SINGLE_INST( KernelGsp,            GPU_GET_KERNEL_GSP,                  1,                NV_FALSE,        pKernelGsp       )
#endif
#if GPU_CHILD_MODULE(DCECLIENTRM)
    GPU_CHILD_SINGLE_INST( OBJDCECLIENTRM,       GPU_GET_DCECLIENTRM,                 1,                NV_FALSE,        pDceclientrm     )
#endif
#if GPU_CHILD_MODULE(VIRT_MEM_ALLOCATOR)
    GPU_CHILD_SINGLE_INST( VirtMemAllocator,     GPU_GET_DMA,                         1,                NV_FALSE,        pDma             )
#endif
#if GPU_CHILD_MODULE(GRMGR)
    GPU_CHILD_SINGLE_INST( GraphicsManager,      GPU_GET_GRMGR,                       1,                NV_FALSE,        pGrMgr           )
#endif
#if GPU_CHILD_MODULE(MIG_MANAGER)
    GPU_CHILD_SINGLE_INST( MIGManager,           GPU_GET_MIG_MANAGER,                 1,                NV_FALSE,        pMIGManager      )
#endif
#if GPU_CHILD_MODULE(KERNEL_MIG_MANAGER)
    GPU_CHILD_SINGLE_INST( KernelMIGManager,     GPU_GET_KERNEL_MIG_MANAGER,          1,                NV_FALSE,        pKernelMIGManager )
#endif
#if GPU_CHILD_MODULE(KERNEL_GRAPHICS_MANAGER)
    GPU_CHILD_SINGLE_INST( KernelGraphicsManager, GPU_GET_KERNEL_GRAPHICS_MANAGER,    1,                NV_FALSE,        pKernelGraphicsManager )
#endif
#if GPU_CHILD_MODULE(GR)
    GPU_CHILD_MULTI_INST ( Graphics,             GPU_GET_GR_UNSAFE,                   GPU_MAX_GRS,      NV_FALSE,        pGr              )
#endif
#if GPU_CHILD_MODULE(KERNEL_GRAPHICS)
    GPU_CHILD_MULTI_INST ( KernelGraphics,       GPU_GET_KERNEL_GRAPHICS,             GPU_MAX_GRS,      NV_FALSE,        pKernelGraphics  )
#endif
#if GPU_CHILD_MODULE(ClockManager)
    GPU_CHILD_SINGLE_INST( ClockManager,         GPU_GET_CLK_MGR,                     1,                NV_FALSE,        pClk             )
#endif
#if GPU_CHILD_MODULE(FAN)
    GPU_CHILD_SINGLE_INST( Fan,                  GPU_GET_FAN,                         1,                NV_FALSE,        pFan             )
#endif
#if GPU_CHILD_MODULE(PERF)
    GPU_CHILD_SINGLE_INST( Perf,                 GPU_GET_PERF,                        1,                NV_FALSE,        pPerf            )
#endif
#if GPU_CHILD_MODULE(KERNEL_PERF)
    GPU_CHILD_SINGLE_INST( KernelPerf,           GPU_GET_KERNEL_PERF,                 1,                NV_FALSE,        pKernelPerf      )
#endif
#if GPU_CHILD_MODULE(THERM)
    GPU_CHILD_SINGLE_INST( Therm,                GPU_GET_THERM,                       1,                NV_FALSE,        pTherm           )
#endif
#if GPU_CHILD_MODULE(BSP)
    GPU_CHILD_MULTI_INST ( OBJBSP,               GPU_GET_BSP,                         GPU_MAX_NVDECS,   NV_FALSE,        pBsp             )
#endif
#if GPU_CHILD_MODULE(CIPHER)
    GPU_CHILD_SINGLE_INST( OBJCIPHER,            GPU_GET_CIPHER,                      1,                NV_FALSE,        pCipher          )
#endif
#if GPU_CHILD_MODULE(VBIOS)
    GPU_CHILD_SINGLE_INST( OBJVBIOS,             GPU_GET_VBIOS,                       1,                NV_FALSE,        pVbios           )
#endif
#if GPU_CHILD_MODULE(DCB)
    GPU_CHILD_SINGLE_INST( OBJDCB,               GPU_GET_DCB,                         1,                NV_FALSE,        pDcb             )
#endif
#if GPU_CHILD_MODULE(GPIO)
    GPU_CHILD_SINGLE_INST( OBJGPIO,              GPU_GET_GPIO,                        1,                NV_FALSE,        pGpio            )
#endif
#if GPU_CHILD_MODULE(VOLT)
    GPU_CHILD_SINGLE_INST( OBJVOLT,              GPU_GET_VOLT,                        1,                NV_FALSE,        pVolt            )
#endif
#if GPU_CHILD_MODULE(I2C)
    GPU_CHILD_SINGLE_INST( I2c,                  GPU_GET_I2C,                         1,                NV_FALSE,        pI2c             )
#endif
#if GPU_CHILD_MODULE(SPI)
    GPU_CHILD_SINGLE_INST( Spi,               GPU_GET_SPI,                            1,                NV_FALSE,        pSpi             )
#endif
#if GPU_CHILD_MODULE(KERNEL_RC)
    GPU_CHILD_SINGLE_INST( KernelRc,             GPU_GET_KERNEL_RC,                   1,                NV_FALSE,        pKernelRc        )
#endif
#if GPU_CHILD_MODULE(RC)
    GPU_CHILD_SINGLE_INST( OBJRC,                GPU_GET_RC,                          1,                NV_FALSE,        pRC              )
#endif
#if GPU_CHILD_MODULE(STEREO)
    GPU_CHILD_SINGLE_INST( Stereo,            GPU_GET_STEREO,                      1,                NV_FALSE,        pStereo          )
#endif
#if GPU_CHILD_MODULE(INTR)
    GPU_CHILD_SINGLE_INST( Intr,                 GPU_GET_INTR,                        1,                NV_FALSE,        pIntr            )
#endif
#if GPU_CHILD_MODULE(DPAUX)
    GPU_CHILD_SINGLE_INST( OBJDPAUX,             GPU_GET_DPAUX,                       1,                NV_FALSE,        pDpAux           )
#endif
#if GPU_CHILD_MODULE(PMU)
    GPU_CHILD_SINGLE_INST( Pmu,                  GPU_GET_PMU,                         1,                NV_FALSE,        pPmu             )
#endif
#if GPU_CHILD_MODULE(KERNEL_PMU)
    GPU_CHILD_SINGLE_INST( KernelPmu,            GPU_GET_KERNEL_PMU,                  1,                NV_FALSE,        pKernelPmu       )
#endif
#if GPU_CHILD_MODULE(CE)
    GPU_CHILD_MULTI_INST ( OBJCE,                GPU_GET_CE,                          GPU_MAX_CES,      NV_FALSE,        pCe              )
#endif
#if GPU_CHILD_MODULE(KERNEL_CE)
    GPU_CHILD_MULTI_INST ( KernelCE,             GPU_GET_KCE,                         GPU_MAX_CES,      NV_FALSE,        pKCe             )
#endif
#if GPU_CHILD_MODULE(MSENC)
    GPU_CHILD_MULTI_INST ( OBJMSENC,             GPU_GET_MSENC,                       GPU_MAX_MSENCS,   NV_FALSE,        pMsenc           )
#endif
#if GPU_CHILD_MODULE(HDA)
    GPU_CHILD_SINGLE_INST( OBJHDA,               GPU_GET_HDA,                         1,                NV_FALSE,        pHda             )
#endif
#if GPU_CHILD_MODULE(HDACODEC)
    GPU_CHILD_SINGLE_INST( OBJHDACODEC,          GPU_GET_HDACODEC,                    1,                NV_FALSE,        pHdacodec        )
#endif
#if GPU_CHILD_MODULE(LPWR)
    GPU_CHILD_SINGLE_INST( Lpwr,              GPU_GET_LPWR,                        1,                NV_FALSE,           pLpwr            )
#endif
#if GPU_CHILD_MODULE(KERNEL_FIFO)
    GPU_CHILD_SINGLE_INST( KernelFifo,           GPU_GET_KERNEL_FIFO_UC,              1,                NV_FALSE,        pKernelFifo      )
#endif
#if GPU_CHILD_MODULE(FIFO)
    GPU_CHILD_SINGLE_INST( OBJFIFO,              GPU_GET_FIFO_UC,                     1,                NV_FALSE,        pFifo            )
#endif
#if GPU_CHILD_MODULE(INFOROM)
    GPU_CHILD_SINGLE_INST( OBJINFOROM,           GPU_GET_INFOROM,                     1,                NV_FALSE,        pInforom         )
#endif
#if GPU_CHILD_MODULE(PMGR)
    GPU_CHILD_SINGLE_INST( Pmgr,                 GPU_GET_PMGR,                        1,                NV_FALSE,        pPmgr            )
#endif
#if GPU_CHILD_MODULE(UVM)
    GPU_CHILD_SINGLE_INST( OBJUVM,               GPU_GET_UVM,                         1,                NV_FALSE,        pUvm             )
#endif
#if GPU_CHILD_MODULE(NV_DEBUG_DUMP)
    GPU_CHILD_SINGLE_INST( NvDebugDump,          GPU_GET_NVD,                         1,                NV_FALSE,        pNvd             )
#endif
#if GPU_CHILD_MODULE(GRDBG)
    GPU_CHILD_SINGLE_INST( SMDebugger,           GPU_GET_GRDBG,                       1,                NV_FALSE,        pGrdbg           )
#endif
#if GPU_CHILD_MODULE(SEC2)
    GPU_CHILD_SINGLE_INST( OBJSEC2,              GPU_GET_SEC2,                        1,                NV_FALSE,        pSec2            )
#endif
#if GPU_CHILD_MODULE(LSFM)
    GPU_CHILD_SINGLE_INST( OBJLSFM,              GPU_GET_LSFM,                        1,                NV_FALSE,        pLsfm            )
#endif
#if GPU_CHILD_MODULE(ACR)
    GPU_CHILD_SINGLE_INST( OBJACR,               GPU_GET_ACR,                         1,                NV_FALSE,        pAcr             )
#endif
#if GPU_CHILD_MODULE(KERNEL_NVLINK)
    GPU_CHILD_SINGLE_INST( KernelNvlink,         GPU_GET_KERNEL_NVLINK,               1,                NV_FALSE,        pKernelNvlink    )
#endif
#if GPU_CHILD_MODULE(NVLINK)
    GPU_CHILD_SINGLE_INST( Nvlink,               GPU_GET_NVLINK,                      1,                NV_FALSE,        pNvLink          )
#endif
#if GPU_CHILD_MODULE(GPULOG)
    GPU_CHILD_SINGLE_INST( OBJGPULOG,            GPU_GET_GPULOG,                      1,                NV_FALSE,        pGpuLog          )
#endif
#if GPU_CHILD_MODULE(GPUMON)
    GPU_CHILD_SINGLE_INST( OBJGPUMON,            GPU_GET_GPUMON,                      1,                NV_FALSE,        pGpuMon          )
#endif
#if GPU_CHILD_MODULE(HWPM)
    GPU_CHILD_SINGLE_INST( OBJHWPM,              GPU_GET_HWPM,                        1,                NV_FALSE,        pHwpm            )
#endif
#if GPU_CHILD_MODULE(KERNEL_HWPM)
    GPU_CHILD_SINGLE_INST( KernelHwpm,           GPU_GET_KERNEL_HWPM,                 1,                NV_FALSE,        pKernelHwpm      )
#endif
#if GPU_CHILD_MODULE(GRIDDISPLAYLESS)
    GPU_CHILD_SINGLE_INST( OBJGRIDDISPLAYLESS,   GPU_GET_GRIDDISPLAYLESS,             1,                NV_FALSE,        pGridDisplayless )
#endif
#if GPU_CHILD_MODULE(SWENG)
    GPU_CHILD_SINGLE_INST( OBJSWENG,             GPU_GET_SWENG,                       1,                NV_FALSE,        pSwEng           )
#endif
#if GPU_CHILD_MODULE(VMMU)
    GPU_CHILD_SINGLE_INST( OBJVMMU,              GPU_GET_VMMU,                        1,                NV_FALSE,        pVmmu            )
#endif
#if GPU_CHILD_MODULE(NVJPG)
    GPU_CHILD_MULTI_INST( OBJNVJPG,              GPU_GET_NVJPG,                       GPU_MAX_NVJPGS,   NV_FALSE,        pNvjpg           )
#endif
#if GPU_CHILD_MODULE(GSP)
    GPU_CHILD_SINGLE_INST( Gsp,                  GPU_GET_GSP,                         1,                NV_FALSE,        pGsp             )
#endif
#if RMCFG_MODULE_KERNEL_FSP && GPU_CHILD_MODULE(KERNEL_FSP)
    GPU_CHILD_SINGLE_INST( KernelFsp,            GPU_GET_KERNEL_FSP,                  1,                NV_FALSE,        pKernelFsp       )
#endif
#if GPU_CHILD_MODULE(OFA)
    GPU_CHILD_MULTI_INST( OBJOFA,                GPU_GET_OFA,                         GPU_MAX_OFAS,     NV_FALSE,        pOfa             )
#endif
#if RMCFG_MODULE_CONF_COMPUTE && GPU_CHILD_MODULE(CONF_COMPUTE)
    GPU_CHILD_SINGLE_INST( ConfidentialCompute,  GPU_GET_CONF_COMPUTE,                1,                NV_TRUE,         pConfCompute      )
#endif
#if RMCFG_MODULE_KERNEL_CCU && GPU_CHILD_MODULE(KERNEL_CCU)
    GPU_CHILD_SINGLE_INST( KernelCcu,            GPU_GET_KERNEL_CCU,                  1,                NV_FALSE,        pKernelCcu        )
#endif

// Undefine the entry macros to simplify call sites
#undef GPU_CHILD
#undef GPU_CHILD_SINGLE_INST
#undef GPU_CHILD_MULTI_INST
#undef GPU_CHILD_MODULE
#undef GPU_CHILD_LIST_DISABLED_ONLY

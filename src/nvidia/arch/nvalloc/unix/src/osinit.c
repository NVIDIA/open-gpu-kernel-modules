/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**************************************************************************************************************
*
*   Description:
*       UNIX-general, device-independent initialization code for
*       the resource manager.
*
*
**************************************************************************************************************/

#include <nv_ref.h>
#include <nv.h>                 // NV device driver interface
#include <nv-reg.h>
#include <nv-priv.h>
#include <nvos.h>
#include <nvrm_registry.h>
#include <platform/sli/sli.h>
#include <nverror.h>
#include <osapi.h>
#include <core/system.h>
#include <os/os.h>
#include "gpu/gpu.h"
#include <gpu/timer/objtmr.h>
#include "gpu/bus/kern_bus.h"
#include "nverror.h"
#include <gpu/bif/kernel_bif.h>
#include <nv-nb-regs.h>
#include <gpu/mem_mgr/mem_mgr.h>
#include <gpu/mem_sys/kern_mem_sys.h>
#include "kernel/gpu/intr/intr.h"
#include <mem_mgr/io_vaspace.h>
#include <gpu/disp/kern_disp.h>
#include <platform/chipset/chipset.h>
#include <kernel/gpu/rc/kernel_rc.h>
#include <kernel/gpu/fifo/kernel_fifo.h>
#include <nv-firmware-chip-family-select.h>
#include <gpu/gsp/kernel_gsp.h>
#include "liblogdecode.h"
#include <gpu/fsp/kern_fsp.h>

#include <mem_mgr/virt_mem_mgr.h>
#include <virtualization/kernel_vgpu_mgr.h>

#include <rmosxfac.h>
#include <gpu_mgr/gpu_mgr.h>
#include <core/thread_state.h>
#include <core/locks.h>
#include <rmapi/client.h>
#include <ctrl/ctrl2080/ctrl2080gpu.h>
#include <class/cl00f2.h>

#include <platform/chipset/pci_pbi.h>

#include "platform/nbsi/nbsi_read.h"
#include "gpu_mgr/gpu_db.h"
#include <class/cl0080.h>
#include <class/cl0073.h>
#include <class/cl2080.h>
#include <class/cl402c.h>

#include <gpu/dce_client/dce_client.h>
// RMCONFIG: need definition of REGISTER_ALL_HALS()
#include "g_hal_register.h"

typedef enum
{
   RM_INIT_OK,

   /* general os errors */
   RM_INIT_REG_SETUP_FAILED            =  0x10,
   RM_INIT_SYS_ENVIRONMENT_FAILED,

   /* gpu errors */
   RM_INIT_GPU_GPUMGR_ALLOC_GPU_FAILED =  0x20,
   RM_INIT_GPU_GPUMGR_CREATE_DEV_FAILED,
   RM_INIT_GPU_GPUMGR_ATTACH_GPU_FAILED,
   RM_INIT_GPU_PRE_INIT_FAILED,
   RM_INIT_GPU_STATE_INIT_FAILED,
   RM_INIT_GPU_LOAD_FAILED,
   RM_INIT_GPU_DMA_CONFIGURATION_FAILED,
   RM_INIT_GPU_GPUMGR_EXPANDED_VISIBILITY_FAILED,

   /* vbios errors */
   RM_INIT_VBIOS_FAILED                =  0x30,
   RM_INIT_VBIOS_POST_FAILED,
   RM_INIT_VBIOS_X86EMU_FAILED,

   /* scalability errors */
   RM_INIT_SCALABILITY_FAILED          =  0x40,

   /* general core rm errors */
   RM_INIT_WATCHDOG_FAILED,
   RM_FIFO_GET_UD_BAR1_MAP_INFO_FAILED,
   RM_GPUDB_REGISTER_FAILED,

   RM_INIT_ALLOC_RMAPI_FAILED,
   RM_INIT_GPUINFO_WITH_RMAPI_FAILED,

   /* rm firmware errors */
   RM_INIT_FIRMWARE_POLICY_FAILED      = 0x60,
   RM_INIT_FIRMWARE_FETCH_FAILED,
   RM_INIT_FIRMWARE_INIT_FAILED,

   RM_INIT_MAX_FAILURES
} rm_init_status;

typedef rm_init_status RM_INIT_STATUS;

typedef struct {
    RM_INIT_STATUS initStatus;
    NV_STATUS      rmStatus;
    NvU32          line;
} UNIX_STATUS;

#define INIT_UNIX_STATUS   { RM_INIT_OK, NV_OK, 0 }
#define RM_INIT_SUCCESS(init)  ((init) == RM_INIT_OK)

#define RM_SET_ERROR(status, err)  { (status).initStatus = (err); \
                                     (status).line = __LINE__; }


//
// GPU architectures support DMA addressing up to a certain address width,
// above which all other bits in any given DMA address must not vary
// (e.g., all 0). This value is the minimum of the DMA addressing
// capabilities, in number of physical address bits, for all supported
// GPU architectures.
//
#define NV_GPU_MIN_SUPPORTED_DMA_ADDR_WIDTH                36

//
// All GPU architectures with GSP support at least 47 physical address bits.
//
#define NV_GSP_GPU_MIN_SUPPORTED_DMA_ADDR_WIDTH            47

static inline NvU64 nv_encode_pci_info(nv_pci_info_t *pci_info)
{
    return gpuEncodeDomainBusDevice(pci_info->domain, pci_info->bus, pci_info->slot);
}

static inline NvU32 nv_generate_id_from_pci_info(nv_pci_info_t *pci_info)
{
    return gpuGenerate32BitId(pci_info->domain, pci_info->bus, pci_info->slot);
}

static inline void nv_os_map_kernel_space(nv_state_t *nv, nv_aperture_t *aperture)
{
    NV_ASSERT(aperture->map == NULL);

    // let's start off assuming a standard device and map the registers
    // normally. It is unfortunate to hard-code the register size here, but we don't
    // want to fail trying to map all of a multi-devices' register space
    aperture->map = osMapKernelSpace(aperture->cpu_address,
                                     aperture->size,
                                     NV_MEMORY_UNCACHED,
                                     NV_PROTECT_READ_WRITE);
    aperture->map_u = (nv_phwreg_t)aperture->map;
}

// local prototypes
static NV_STATUS   initCoreLogic(OBJGPU *);
static void        initVendorSpecificRegistry(OBJGPU *, NvU16);
static NV_STATUS   teardownCoreLogic(OBJOS *, OBJGPU *);
static void        initUnixSpecificRegistry(OBJGPU *);

NV_STATUS osRmInitRm(void)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    NV_STATUS  status;

    if (os_is_efi_enabled())
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_IS_UEFI, NV_TRUE);
    }

    // have to init this before the debug subsystem, which will
    // try to check the value of ResmanDebugLevel
    RmInitRegistry();

    NvU32 data = 0;
    if ((osReadRegistryDword(NULL,
                NV_REG_ENABLE_PCIE_GEN3, &data) == NV_OK) &&
            (data != 0))
    {
        data = DRF_DEF(_REG_STR, _RM_PCIE, _LINK_SPEED_ALLOW_GEN3, _ENABLE);

        osWriteRegistryDword(NULL,
                NV_REG_STR_RM_PCIE_LINK_SPEED, data);
    }

    if (!(osReadRegistryDword(NULL,
                NV_REG_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS,
                &data) == NV_OK) || data)
    {
        pSys->setProperty(pSys,
                PDB_PROP_SYS_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS,
                NV_TRUE);
    }

    if ((osReadRegistryDword(NULL,
                NV_REG_ENABLE_STREAM_MEMOPS,
                &data) == NV_OK) && (data != 0))
    {
        data = DRF_DEF(_REG_STR, _RM_STREAM_MEMOPS, _ENABLE, _YES);
        osWriteRegistryDword(NULL, NV_REG_STR_RM_STREAM_MEMOPS, data);
    }

    if ((osReadRegistryDword(NULL,
                NV_REG_STR_RM_FORCE_BAR_PATH,
                &data) == NV_OK) && (data == 1))
    {
        //
        // Exposing the GPU memory as NUMA node memory requires coherent CPU
        // mappings over NVLink. If those mappings are disabled, also disable
        // NUMA-onlining of the device memory.
        //
        osWriteRegistryDword(NULL,
                NV_REG_ENABLE_USER_NUMA_MANAGEMENT, 0);
    }

    if ((osReadRegistryDword(NULL,
                NV_REG_NVLINK_DISABLE, &data) == NV_OK) &&
            (data == 1))
    {
        osWriteRegistryDword(NULL, NV_REG_STR_RM_NVLINK_CONTROL, 0x1);
    }

    if ((osReadRegistryDword(NULL,
                NV_REG_ENABLE_PCIE_RELAXED_ORDERING_MODE, &data) == NV_OK) &&
            (data == 1))
    {
        osWriteRegistryDword(NULL,
                             NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING,
                             NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING_ENABLE);
    }

    // init the debug subsystem if necessary
    os_dbg_init();
    nvDbgInitRmMsg(NULL);

    // Force nvlog reinit since module params are now available
    NVLOG_UPDATE();

    // Register all supported hals
    status = REGISTER_ALL_HALS();
    if (status != NV_OK)
    {
        RmDestroyRegistry(NULL);
        return status;
    }

    // Setup any ThreadState defaults
    threadStateInitSetupFlags(THREAD_STATE_SETUP_FLAGS_ENABLED |
                              THREAD_STATE_SETUP_FLAGS_TIMEOUT_ENABLED |
                              THREAD_STATE_SETUP_FLAGS_SLI_LOGIC_ENABLED |
                              THREAD_STATE_SETUP_FLAGS_DO_NOT_INCLUDE_SLEEP_TIME_ENABLED);

    return NV_OK;
}

void RmShutdownRm(void)
{
    NV_PRINTF(LEVEL_INFO, "shutdown rm\n");

    RmDestroyRegistry(NULL);

    // Free objects created with RmInitRm, including the system object
    RmDestroyRm();
}

//
// osAttachGpu
//
// This routine is used as a callback by the gpumgrAttachGpu
// interface to allow os-dependent code to set up any state
// before engine construction begins.
//
NV_STATUS osAttachGpu(
    OBJGPU    *pGpu,
    void      *pOsGpuInfo
)
{
    nv_state_t *nv = (nv_state_t *)pOsGpuInfo;
    nv_priv_t  *nvp;

    nvp = NV_GET_NV_PRIV(nv);

    nvp->pGpu = pGpu;

    NV_SET_NV_STATE(pGpu, (void *)nv);

    initUnixSpecificRegistry(pGpu);

    // Assign default values to Registry keys for VGX
    if (os_is_vgx_hyper())
    {
        initVGXSpecificRegistry(pGpu);
    }

    return NV_OK;
}

NV_STATUS osDpcAttachGpu(
    OBJGPU    *pGpu,
    void      *pOsGpuInfo
)
{
    return NV_OK; // Nothing to do for unix
}

void osDpcDetachGpu(
    OBJGPU    *pGpu
)
{
    return; // Nothing to do for unix
}

NV_STATUS
osHandleGpuLost
(
    OBJGPU *pGpu
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NvU32 pmc_boot_0;

    // Determine if we've already run the handler
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED))
    {
        return NV_OK;
    }

    pmc_boot_0 = NV_PRIV_REG_RD32(nv->regs->map_u, NV_PMC_BOOT_0);
    if (pmc_boot_0 != nvp->pmc_boot_0)
    {
        //
        // This doesn't support PEX Reset and Recovery yet.
        // This will help to prevent accessing registers of a GPU
        // which has fallen off the bus.
        //
        nvErrorLog_va((void *)pGpu, ROBUST_CHANNEL_GPU_HAS_FALLEN_OFF_THE_BUS,
                      "GPU has fallen off the bus.");

        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_RC_ERROR, NULL, 0, ROBUST_CHANNEL_GPU_HAS_FALLEN_OFF_THE_BUS, 0);

        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "GPU has fallen off the bus.\n");

        if (pGpu->boardInfo != NULL && pGpu->boardInfo->serialNumber[0] != '\0')
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "GPU serial number is %s.\n",
                          pGpu->boardInfo->serialNumber);
        }

        gpuSetDisconnectedProperties(pGpu);

        // Trigger the OS's PCI recovery mechanism
        if (nv_pci_trigger_recovery(nv) != NV_OK)
        {
            //
            // Initiate a crash dump immediately, since the OS doesn't appear
            // to have a mechanism wired up for attempted recovery.
            //
            (void) RmLogGpuCrash(pGpu);
        }
        else
        {
            //
            // Make the SW state stick around until the recovery can start, but
            // don't change the PDB property: this is only used to report to
            // clients whether or not persistence mode is enabled, and we'll
            // need it after the recovery callbacks to restore the correct
            // persistence mode for the GPU.
            //
            osModifyGpuSwStatePersistence(pGpu->pOsGpuInfo, NV_TRUE);
        }

        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJCL  *pCl  = SYS_GET_CL(pSys);
        // Set SURPRISE_REMOVAL flag for eGPU to help in device removal.
        if ((pCl != NULL) &&
            pCl->getProperty(pCl, PDB_PROP_CL_IS_EXTERNAL_GPU))
        {
            nv->flags |= NV_FLAG_IN_SURPRISE_REMOVAL;
        }
        DBG_BREAKPOINT();
    }

    return NV_OK;
}

/*!
 * @brief Traverse bus topology till Gpu's root port.
 * If any of the intermediate bridge has TB3 supported vendorId and hotplug
 * capability(not necessarily same bridge), mark the Gpu as External Gpu.
 *
 * @params[in]    pGpu    OBJGPU pointer
 * @params[in]    pCl     OBJCL pointer
 *
* @return NV_OK
*      Identified to be eGPU
* @return others
 *     Not an eGPU / error on identfying
 *
 */
NvBool
RmCheckForExternalGpu
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    NvU8 bus;
    NvU32 domain;
    void *handleUp;
    NvU8 busUp, devUp, funcUp;
    NvU16 vendorIdUp, deviceIdUp;
    NvU32 portCaps, pciCaps, slotCaps;
    NvU32 PCIECapPtr;
    RM_API *pRmApi;
    NV_STATUS status, rmStatus;
    NvBool bTb3Bridge = NV_FALSE, bSlotHotPlugSupport = NV_FALSE;
    NvBool iseGPUBridge = NV_FALSE;

    pRmApi  = GPU_GET_PHYSICAL_RMAPI(pGpu);
    domain  = gpuGetDomain(pGpu);
    bus     = gpuGetBus(pGpu);
    do
    {
        // Find the upstream bridge
        handleUp = clFindP2PBrdg(pCl, domain, bus, &busUp, &devUp, &funcUp, &vendorIdUp, &deviceIdUp);
        if (!handleUp)
        {
            return iseGPUBridge;
        }

        if (vendorIdUp == PCI_VENDOR_ID_INTEL)
        {
            // Check for the supported TB3(ThunderBolt 3) bridges.
            NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS params = { 0 };

            // LOCK: acquire GPUs lock
            rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                         RM_LOCK_MODULES_INIT);
            if (rmStatus != NV_OK)
            {
                return iseGPUBridge;
            }
            params.pciDeviceId = deviceIdUp;
            status = pRmApi->Control(pRmApi,
                                     pGpu->hInternalClient,
                                     pGpu->hInternalSubdevice,
                                     NV2080_CTRL_CMD_INTERNAL_GET_EGPU_BRIDGE_INFO,
                                     &params,
                                     sizeof(params));
            // UNLOCK: release GPUs lock
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Error 0x%08x on eGPU Approval for Bridge ID: 0x%08x\n", status, deviceIdUp);
                DBG_BREAKPOINT();
                return iseGPUBridge;
            }
            else
            {
                // Check for the approved eGPU BUS TB3
                if (params.iseGPUBridge &&
                    params.approvedBusType == NV2080_CTRL_INTERNAL_EGPU_BUS_TYPE_TB3)
                {
                    bTb3Bridge =  NV_TRUE;
                }
            }
        }

        if (NV_OK != clSetPortPcieCapOffset(pCl, handleUp, &PCIECapPtr))
        {
            // PCIE bridge but no cap pointer.
            break;
        }

        // Get the PCIE capabilities.
        pciCaps = osPciReadDword(handleUp, CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);
        if (CL_PCIE_CAP_SLOT & pciCaps)
        {
            // Get the slot capabilities.
            slotCaps = osPciReadDword(handleUp, CL_PCIE_SLOT_CAP - CL_PCIE_BEGIN + PCIECapPtr);

            if ((CL_PCIE_SLOT_CAP_HOTPLUG_CAPABLE & slotCaps) &&
                (CL_PCIE_SLOT_CAP_HOTPLUG_SURPRISE & slotCaps))
            {
                bSlotHotPlugSupport = NV_TRUE;
            }
        }

        if (bTb3Bridge && bSlotHotPlugSupport)
        {
            iseGPUBridge = NV_TRUE;
            break;
        }

        bus = busUp;

        // Get port caps to check if PCIE bridge is the root port
        portCaps = osPciReadDword(handleUp, CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);

    } while (!CL_IS_ROOT_PORT(portCaps));
    return iseGPUBridge;
}

/*
 * Initialize the required GPU information by doing RMAPI control calls
 * and store the same in the UNIX specific data structures.
 */
static NV_STATUS
RmInitGpuInfoWithRmApi
(
    OBJGPU *pGpu
)
{
    RM_API     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    nv_state_t *nv     = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp    = NV_GET_NV_PRIV(nv);
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams = { 0 };
    NV_STATUS   status;

    // LOCK: acquire GPUs lock
    status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT);
    if (status != NV_OK)
    {
        return status;
    }

    pGpuInfoParams = portMemAllocNonPaged(sizeof(*pGpuInfoParams));
    if (pGpuInfoParams == NULL)
    {
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

        return NV_ERR_NO_MEMORY;
    }


    portMemSet(pGpuInfoParams, 0, sizeof(*pGpuInfoParams));

    pGpuInfoParams->gpuInfoListSize = 3;
    pGpuInfoParams->gpuInfoList[0].index = NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED;
    pGpuInfoParams->gpuInfoList[1].index = NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED;
    pGpuInfoParams->gpuInfoList[2].index = NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY;

    status = pRmApi->Control(pRmApi, nv->rmapi.hClient,
                             nv->rmapi.hSubDevice,
                             NV2080_CTRL_CMD_GPU_GET_INFO_V2,
                             pGpuInfoParams, sizeof(*pGpuInfoParams));

    if (status == NV_OK)
    {
        nvp->b_4k_page_isolation_required =
            (pGpuInfoParams->gpuInfoList[0].data ==
             NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED_YES);
        nvp->b_mobile_config_enabled =
            (pGpuInfoParams->gpuInfoList[1].data ==
             NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED_YES);
        nv->dma_buf_supported =
            (pGpuInfoParams->gpuInfoList[2].data ==
             NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY_YES);
    }

    nv->coherent = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    portMemFree(pGpuInfoParams);

    // UNLOCK: release GPUs lock
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    return status;
}

static void RmSetSocDispDeviceMappings(
    GPUATTACHARG *gpuAttachArg,
    nv_state_t *nv
)
{
    gpuAttachArg->socDeviceArgs.deviceMapping[SOC_DEV_MAPPING_DISP].gpuNvAddr = (GPUHWREG*) nv->regs->map;
    gpuAttachArg->socDeviceArgs.deviceMapping[SOC_DEV_MAPPING_DISP].gpuNvPAddr = nv->regs->cpu_address;
    gpuAttachArg->socDeviceArgs.deviceMapping[SOC_DEV_MAPPING_DISP].gpuNvLength = (NvU32) nv->regs->size;
}

static void RmSetSocDpauxDeviceMappings(
    GPUATTACHARG *gpuAttachArg,
    nv_state_t *nv
)
{
}

static void RmSetSocHdacodecDeviceMappings(
    GPUATTACHARG *gpuAttachArg,
    nv_state_t *nv
)
{
}

static void RmSetSocMipiCalDeviceMappings(
    GPUATTACHARG *gpuAttachArg,
    nv_state_t *nv
)
{
}

static void
osInitNvMapping(
    nv_state_t *nv,
    NvU32 *pDeviceReference,
    UNIX_STATUS *status
)
{
    OBJGPU *pGpu;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GPUATTACHARG *gpuAttachArg;
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NvU32 deviceInstance;
    NvU32 data = 0;
    NvU32 dispIsoStreamId;
    NvU32 dispNisoStreamId;

    NV_PRINTF(LEVEL_INFO, "osInitNvMapping:\n");

    // allocate the next available gpu device number
    status->rmStatus = gpumgrAllocGpuInstance(pDeviceReference);
    if (status->rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot get valid gpu instance\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_GPUMGR_ALLOC_GPU_FAILED);
        return;
    }

    // RM_BASIC_LOCK_MODEL: allocate GPU lock
    status->rmStatus = rmGpuLockAlloc(*pDeviceReference);
    if (status->rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** cannot allocate GPU lock\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_GPUMGR_ALLOC_GPU_FAILED);
        return;
    }

    // attach default single-entry broadcast device for this gpu
    status->rmStatus = gpumgrCreateDevice(&deviceInstance, NVBIT(*pDeviceReference), NULL);
    if (status->rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot attach bc gpu\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_GPUMGR_CREATE_DEV_FAILED);
        // RM_BASIC_LOCK_MODEL: free GPU lock
        rmGpuLockFree(*pDeviceReference);
        return;
    }

    // init attach state
    gpuAttachArg = portMemAllocNonPaged(sizeof(GPUATTACHARG));
    if (gpuAttachArg == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot allocate gpuAttachArg\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_GPUMGR_ALLOC_GPU_FAILED);
        // RM_BASIC_LOCK_MODEL: free GPU lock
        rmGpuLockFree(*pDeviceReference);
        return;
    }

    portMemSet(gpuAttachArg, 0, sizeof(GPUATTACHARG));

    if (NV_IS_SOC_DISPLAY_DEVICE(nv))
    {
        gpuAttachArg->socDeviceArgs.specified = NV_TRUE;

        RmSetSocDispDeviceMappings(gpuAttachArg, nv);

        RmSetSocDpauxDeviceMappings(gpuAttachArg, nv);

        RmSetSocHdacodecDeviceMappings(gpuAttachArg, nv);

        RmSetSocMipiCalDeviceMappings(gpuAttachArg, nv);

        gpuAttachArg->socDeviceArgs.socChipId0 = nv->disp_sw_soc_chip_id;

        gpuAttachArg->socDeviceArgs.iovaspaceId = nv->iovaspace_id;
    }
    else
    {
        gpuAttachArg->fbPhysAddr      = nv->fb->cpu_address;
        gpuAttachArg->fbBaseAddr      = (GPUHWREG*) 0; // not mapped
        gpuAttachArg->devPhysAddr     = nv->regs->cpu_address;
        gpuAttachArg->regBaseAddr     = (GPUHWREG*) nv->regs->map;
        gpuAttachArg->intLine         = 0;             // don't know yet
        gpuAttachArg->instPhysAddr    = nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address;
        gpuAttachArg->instBaseAddr    = (GPUHWREG*) 0; // not mapped

        gpuAttachArg->regLength       = nv->regs->size;
        gpuAttachArg->fbLength        = nv->fb->size;
        gpuAttachArg->instLength      = nv->bars[NV_GPU_BAR_INDEX_IMEM].size;

        gpuAttachArg->iovaspaceId     = nv->iovaspace_id;
        gpuAttachArg->cpuNumaNodeId   = nv->cpu_numa_node_id;
    }

    //
    // we need this to check if we are running on virtual GPU
    // in gpuBindHal function later.
    //
    gpuAttachArg->nvDomainBusDeviceFunc = nv_encode_pci_info(&nv->pci_info);

    gpuAttachArg->bRequestFwClientRm = nv->request_fw_client_rm;

    gpuAttachArg->pOsAttachArg    = (void *)nv;

    // use gpu manager to attach gpu
    status->rmStatus = gpumgrAttachGpu(*pDeviceReference, gpuAttachArg);
    portMemFree(gpuAttachArg);
    if (status->rmStatus != NV_OK)
    {
        gpumgrDestroyDevice(deviceInstance);
        RM_SET_ERROR(*status, RM_INIT_GPU_GPUMGR_ATTACH_GPU_FAILED);
        NV_PRINTF(LEVEL_ERROR, "*** Cannot attach gpu\n");
        // RM_BASIC_LOCK_MODEL: free GPU lock
        rmGpuLockFree(*pDeviceReference);
        return;
    }
    nvp->flags |= NV_INIT_FLAG_GPUMGR_ATTACH;

    pGpu = gpumgrGetGpu(*pDeviceReference);

    sysInitRegistryOverrides(pSys);

    sysApplyLockingPolicy(pSys);

    pGpu->busInfo.IntLine = nv->interrupt_line;

    //
    // Set the DMA address size as soon as we have the HAL to call to
    // determine the precise number of physical address bits supported
    // by the architecture. DMA allocations should not be made before
    // this point.
    //
    nv_set_dma_address_size(nv, gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM));

    pGpu->dmaStartAddress = (RmPhysAddr)nv_get_dma_start_address(nv);
    if (nv->fb != NULL)
    {
        pGpu->registerAccess.gpuFbAddr = (GPUHWREG*) nv->fb->map;
        pGpu->busInfo.gpuPhysFbAddr = nv->fb->cpu_address;
    }

    // set default parent gpu
    gpumgrSetParentGPU(pGpu, pGpu);

    NV_PRINTF(LEVEL_INFO, "device instance          : %d\n", *pDeviceReference);
    NV_PRINTF(LEVEL_INFO, "NV regs using linear address  : 0x%p\n",
              pGpu->deviceMappings[SOC_DEV_MAPPING_DISP].gpuNvAddr);
    NV_PRINTF(LEVEL_INFO,
              "NV fb using linear address  : 0x%p\n", pGpu->registerAccess.gpuFbAddr);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED, NV_TRUE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS, NV_TRUE);

    if (osReadRegistryDword(pGpu,
                NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR, &data) == NV_OK)
    {
        if (data == NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR_DISABLE)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS, NV_FALSE);
        }
    }

    if (!os_is_vgx_hyper())
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT, NV_TRUE);
    }
    else
    {
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT, NV_TRUE);
        }
    }

    if ((osReadRegistryDword(NULL,
                             NV_REG_PRESERVE_VIDEO_MEMORY_ALLOCATIONS,
                             &data) == NV_OK) && data)
    {
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        memmgrSetPmaForcePersistence(pMemoryManager, NV_TRUE);
        pKernelMemorySystem->bPreserveComptagBackingStoreOnSuspend = NV_TRUE;

        nv->preserve_vidmem_allocations = NV_TRUE;
    }

    // Check if SMMU can be enabled on PushBuffer Aperture
    nv_get_disp_smmu_stream_ids(nv, &dispIsoStreamId, &dispNisoStreamId);
    if (dispNisoStreamId != NV_U32_MAX)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS, NV_FALSE);
    }
    else
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS, NV_TRUE);
    }
}

void osInitScalabilityOptions
(
    OBJGPU  *pGpu,
    void     *pDeviceExtension
)
{
}


static NV_STATUS
osInitScalability(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);

    osInitScalabilityOptions(pGpu, NULL);

    // We need PCI-E chipset information very early.
    if (pCl != NULL)
    {
        return clInitPcie(pGpu, pCl);
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }
}

static NV_STATUS
osTeardownScalability(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);

    return clTeardownPcie(pGpu, pCl);
}

#define NV_DBG_PRINT_VGA_STATUS(nv, src)    \
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "%s reports GPU is %s VGA\n", \
              src, NV_PRIMARY_VGA(nv) ? "primary" : "not primary");

static void
RmAssignPrimaryVga(
    nv_state_t *nv,
    OBJGPU     *pGpu
)
{
    //
    // Check with the OS for the primary VGA status of the adapter. If it knows
    // definitively (nv_set_primary_vga_status() returns NV_OK), then we should
    // use that value.
    //
    // Otherwise, check the I/O access and VGA decoding along the path from the
    // adapter to the root. We expect that the primary VGA will be the only
    // non-3D controller with these properties enabled along the entire path.
    //
    if (nv_set_primary_vga_status(nv) != NV_OK)
    {
        KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        OBJSYS    *pSys       = SYS_GET_INSTANCE();
        OBJCL     *pCl        = SYS_GET_CL(pSys);

        nv->primary_vga = (kbifIsPciIoAccessEnabled_HAL(pGpu, pKernelBif) &&
                           !kbifIs3dController_HAL(pGpu, pKernelBif) && (pCl != NULL) &&
                           clUpstreamVgaDecodeEnabled(pGpu, pCl));

        NV_DBG_PRINT_VGA_STATUS(nv, "PCI config space");
    }
    else
    {
        NV_DBG_PRINT_VGA_STATUS(nv, "OS");
    }
}

static void
RmDeterminePrimaryDevice(OBJGPU *pGpu)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    NvBool bFrameBufferConsoleDevice = NV_FALSE;

    // Skip updating nv->primary_vga while RM is recovering after GPU reset
    if (nv->flags & NV_FLAG_IN_RECOVERY)
    {
        return;
    }

    nv->primary_vga = NV_FALSE;

    //
    // In case of Passthru, GPU will always be secondary
    //
    if (IS_PASSTHRU(pGpu))
    {
        return;
    }

    //
    // In case of VIRTUAL GPU, there is no display, hence it will be secondary
    //
    if (IS_VIRTUAL(pGpu))
    {
        return;
    }

    RmAssignPrimaryVga(nv, pGpu);

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, " is %s VGA\n",
              !!nv->primary_vga ? "primary" : "not primary");

    //
    // If GPU is driving any frame buffer console(vesafb, efifb etc)
    // mark the GPU as Primary.
    //
    bFrameBufferConsoleDevice = rm_get_uefi_console_status(nv);

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, " is %s UEFI console device\n",
              bFrameBufferConsoleDevice ? "primary" : "not primary");

    pGpu->setProperty(pGpu, PDB_PROP_GPU_PRIMARY_DEVICE,
                      (bFrameBufferConsoleDevice || !!nv->primary_vga));
}

static void
RmSetConsolePreservationParams(OBJGPU *pGpu)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    NvU64 fbBaseAddress = 0;
    NvU64 fbConsoleSize = 0;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    //
    // PDB_PROP_GPU_PRIMARY_DEVICE should be NV_FALSE for vGPU configuration so
    // return early
    //
    if (os_is_vgx_hyper() || IS_VIRTUAL(pGpu))
        return;

    if (!gpuFuseSupportsDisplay_HAL(pGpu))
    {
        return;
    }

    //
    // Check the OS layer for any video memory used by a console
    // driver that should be reserved.
    //
    // If the console driver is using some amount of memory, its
    // mapping is presumably either pointing at the start of BAR1
    // (this is the mapping the EFI GOP driver provides for
    // efifb), or the upper 16MB of BAR2 (this is the mapping the
    // VBIOS provides for, e.g., vesafb).
    //
    // In the BAR2 case, there is nothing more to do: the upper
    // 16MB of BAR2 will remain in physical mode
    // (see the documentation for NV_PBUS_BAR2_BLOCK in dev_bus.ref),
    // and the console driver can continue to access it.
    //
    // In the BAR1 case, RM will put all of BAR1 into virtual
    // mode, so we need to create a BAR1 mapping for the console
    // driver to use.
    //
    fbConsoleSize = rm_get_uefi_console_size(nv, &fbBaseAddress);

    if ((fbConsoleSize > 0) && (fbBaseAddress != 0))
    {
        KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        pKernelBus->bPreserveBar1ConsoleEnabled =
                          (fbBaseAddress == nv->fb->cpu_address);
    }

    //
    // Case for legacy console, where OS will not give console size
    // and primary_vga will be set for the device.
    // Hence, reserving default size for VGA text modes.
    //
    if ((fbConsoleSize == 0) && nv->primary_vga)
    {
        fbConsoleSize = 0x40000;
    }

    pMemoryManager->Ram.ReservedConsoleDispMemSize = NV_ALIGN_UP(fbConsoleSize, 0x10000);
}

static NV_STATUS
RmInitDeviceDma(
    nv_state_t *nv
)
{
    if (nv->iovaspace_id != NV_IOVA_DOMAIN_NONE)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJVMM *pVmm = SYS_GET_VMM(pSys);
        OBJVASPACE *pIOVAS;
        NV_STATUS status = vmmCreateVaspace(pVmm, IO_VASPACE_A,
                                            nv->iovaspace_id, 0, 0ULL, ~0ULL,
                                            0ULL, 0ULL,
                                            NULL, VASPACE_FLAGS_ENABLE_VMM,
                                            &pIOVAS);
        if (status != NV_OK)
        {
            return status;
        }
    }

    return NV_OK;
}

static void
RmTeardownDeviceDma(
    nv_state_t *nv
)
{
    if (nv->iovaspace_id != NV_IOVA_DOMAIN_NONE)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJVMM *pVmm = SYS_GET_VMM(pSys);
        OBJVASPACE *pIOVAS;

        if (NV_OK == vmmGetVaspaceFromId(pVmm, nv->iovaspace_id, IO_VASPACE_A, &pIOVAS))
        {
            vmmDestroyVaspace(pVmm, pIOVAS);
        }
    }
}

static void
RmInitNvDevice(
    NvU32 deviceReference,
    UNIX_STATUS *status
)
{
    // set the device context
    OBJGPU *pGpu = gpumgrGetGpu(deviceReference);
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);

    NV_PRINTF(LEVEL_INFO, "RmInitNvDevice:\n");

    NV_PRINTF(LEVEL_INFO,
              "device instance          : 0x%08x\n", deviceReference);

    // initialize all engines -- calls back osInitMapping()
    status->rmStatus = gpumgrStatePreInitGpu(pGpu);
    if (status->rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot pre-initialize the device\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_PRE_INIT_FAILED);
        return;
    }

    // Configure eGPU setting
    if (RmCheckForExternalGpu(pGpu, pCl))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_IS_EXTERNAL_GPU, NV_TRUE);
        nv->is_external_gpu = NV_TRUE;
    }
    status->rmStatus = gpumgrStateInitGpu(pGpu);
    if (status->rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot initialize the device\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_STATE_INIT_FAILED);
        return;
    }
    nvp->flags |= NV_INIT_FLAG_GPU_STATE;

    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    //
    // Make sure bifIsMSIEnabled() gets a chance to look up the OS PCI
    // handle for this GPU. This must happen after GPU state
    // initialization.
    //
    if (pKernelBif != NULL)
    {
        kbifCheckAndRearmMSI(pGpu, pKernelBif);
    }

    // Set RM's interrupt enable state to zero here so that interrupts
    // won't be enabled during loading
    Intr *pIntr = GPU_GET_INTR(pGpu);
    if (pIntr != NULL)
    {
        intrSetIntrEn(pIntr, INTERRUPT_TYPE_DISABLED);
    }

    status->rmStatus = gpumgrStateLoadGpu(pGpu, GPU_STATE_DEFAULT);
    if (status->rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "*** Cannot load state into the device\n");
        RM_SET_ERROR(*status, RM_INIT_GPU_LOAD_FAILED);
        return;
    }
    nvp->flags |= NV_INIT_FLAG_GPU_STATE_LOAD;

    // Setup GPU scalability
    (void) RmInitScalability(pGpu);

    return;
}

NV_STATUS osInitMapping(
    OBJGPU *pGpu
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJCL      *pCl = SYS_GET_CL(pSys);
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);

    NV_PRINTF(LEVEL_INFO, "osInitMapping:\n");

    // Some PCI BIOSs leave the ROM mapped.  This causes problems if it overlays system RAM.  Just disable it.
    GPU_BUS_CFG_WR32(pGpu, NV_CONFIG_PCI_NV_12, 0);

    // make sure our PCI latency timer is sufficient (max it out)
    GPU_BUS_CFG_WR32(pGpu, NV_CONFIG_PCI_NV_3,
        DRF_DEF(_CONFIG, _PCI_NV_3, _LATENCY_TIMER, _248_CLOCKS));

    if (pCl != NULL)
    {
        initCoreLogic(pGpu);
    }
    nvp->flags |= NV_INIT_FLAG_CORE_LOGIC;

    return NV_OK;

} // end of osInitMapping()

static NV_STATUS
initCoreLogic(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);

#if defined(NVCPU_X86_64)
    OBJOS *pOS = SYS_GET_OS(pSys);
    if (!os_pat_supported())
        pOS->setProperty(pOS, PDB_PROP_OS_PAT_UNSUPPORTED, NV_TRUE);
#endif

    return clInit(pGpu, pCl);
}

static NV_STATUS
teardownCoreLogic(
    OBJOS  *pOS,
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);

    return clTeardown(pGpu, pCl);
}

static void RmTeardownDpauxRegisters(
    nv_state_t *nv
)
{
}

static void RmTeardownHdacodecRegisters(
    nv_state_t *nv
)
{
}

static void RmTeardownMipiCalRegisters(
    nv_state_t *nv
)
{
}

static NV_STATUS
RmTeardownRegisters(
    nv_state_t *nv
)
{
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "Tearing down registers\n");

    if (nv->regs && nv->regs->map)
    {
        osUnmapKernelSpace(nv->regs->map, nv->regs->size);
        nv->regs->map = 0;
        nv->regs->map_u = NULL;
    }

    RmTeardownDpauxRegisters(nv);

    RmTeardownHdacodecRegisters(nv);

    RmTeardownMipiCalRegisters(nv);

    return NV_OK;
}

static NV_STATUS
RmSetupDpauxRegisters(
    nv_state_t *nv,
    UNIX_STATUS *status
)
{

    return NV_OK;
}

static NV_STATUS
RmSetupHdacodecRegisters(
    nv_state_t *nv,
    UNIX_STATUS *status
)
{

    return NV_OK;
}

static NV_STATUS
RmSetupMipiCalRegisters(
    nv_state_t *nv,
    UNIX_STATUS *status
)
{

    return NV_OK;
}

static void
RmSetupRegisters(
    nv_state_t *nv,
    UNIX_STATUS *status
)
{
    NV_STATUS ret;

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "RmSetupRegisters for 0x%x:0x%x\n",
              nv->pci_info.vendor_id, nv->pci_info.device_id);
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "pci config info:\n");
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "   registers look  like: 0x%" NvU64_fmtx " 0x%" NvU64_fmtx,
              nv->regs->cpu_address, nv->regs->size);

    if (nv->fb != NULL)
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv, "   fb        looks like: 0x%" NvU64_fmtx " 0x%" NvU64_fmtx,
                nv->fb->cpu_address, nv->fb->size);
    }

    {
        nv_os_map_kernel_space(nv, nv->regs);
    }

    if (nv->regs->map == NULL)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to map regs registers!!\n");
        RM_SET_ERROR(*status, RM_INIT_REG_SETUP_FAILED);
        status->rmStatus   = NV_ERR_OPERATING_SYSTEM;
        return;
    }
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "Successfully mapped framebuffer and registers\n");
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "final mappings:\n");
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "    regs: 0x%" NvU64_fmtx " 0x%" NvU64_fmtx " 0x%p\n",
              nv->regs->cpu_address, nv->regs->size, nv->regs->map);

    ret = RmSetupDpauxRegisters(nv, status);
    if (ret != NV_OK)
        goto err_unmap_disp_regs;

    ret = RmSetupHdacodecRegisters(nv, status);
    if (ret != NV_OK)
    {
        RmTeardownDpauxRegisters(nv);
        goto err_unmap_disp_regs;
    }

    ret = RmSetupMipiCalRegisters(nv, status);
    if (ret != NV_OK)
    {
        RmTeardownHdacodecRegisters(nv);
        RmTeardownDpauxRegisters(nv);
        goto err_unmap_disp_regs;
    }

    return;

err_unmap_disp_regs:
    if (nv->regs && nv->regs->map)
    {
        osUnmapKernelSpace(nv->regs->map, nv->regs->size);
        nv->regs->map = 0;
    }

    return;
}

NvBool RmInitPrivateState(
    nv_state_t *pNv
)
{
    nv_priv_t *nvp;
    NvU32 gpuId;
    NvU32 pmc_boot_0 = 0;
    NvU32 pmc_boot_1 = 0;
    NvU32 pmc_boot_42 = 0;

    NV_SET_NV_PRIV(pNv, NULL);

    if (!NV_IS_SOC_DISPLAY_DEVICE(pNv) && !NV_IS_SOC_IGPU_DEVICE(pNv))
    {
        pNv->regs->map_u = os_map_kernel_space(pNv->regs->cpu_address,
                                               os_page_size,
                                               NV_MEMORY_UNCACHED);
        if (pNv->regs->map_u == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to map GPU registers (DISABLE_INTERRUPTS).\n");
            return NV_FALSE;
        }

        pmc_boot_0 = NV_PRIV_REG_RD32(pNv->regs->map_u, NV_PMC_BOOT_0);
        pmc_boot_1 = NV_PRIV_REG_RD32(pNv->regs->map_u, NV_PMC_BOOT_1);
        pmc_boot_42 = NV_PRIV_REG_RD32(pNv->regs->map_u, NV_PMC_BOOT_42);

        os_unmap_kernel_space(pNv->regs->map_u, os_page_size);
        pNv->regs->map_u = NULL;
    }

    if (os_alloc_mem((void **)&nvp, sizeof(*nvp)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to allocate private device state.\n");
        return NV_FALSE;
    }

    gpuId = nv_generate_id_from_pci_info(&pNv->pci_info);

    if (gpumgrRegisterGpuId(gpuId, nv_encode_pci_info(&pNv->pci_info)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to register GPU with GPU manager.\n");
        os_free_mem(nvp);
        return NV_FALSE;
    }

    pNv->gpu_id = gpuId;

    pNv->iovaspace_id = nv_requires_dma_remap(pNv) ? gpuId :
                                                     NV_IOVA_DOMAIN_NONE;
    pNv->cpu_numa_node_id = NV0000_CTRL_NO_NUMA_NODE;

    kvgpumgrAttachGpu(pNv->gpu_id);

    //
    // Set up a reasonable default DMA address size, based on the minimum
    // possible on currently supported GPUs.
    //
    nv_set_dma_address_size(pNv, NV_GPU_MIN_SUPPORTED_DMA_ADDR_WIDTH);

    os_mem_set(nvp, 0, sizeof(*nvp));
    nvp->status = NV_ERR_INVALID_STATE;
    nvp->pmc_boot_0 = pmc_boot_0;
    nvp->pmc_boot_1 = pmc_boot_1;
    nvp->pmc_boot_42 = pmc_boot_42;
    NV_SET_NV_PRIV(pNv, nvp);

    return NV_TRUE;
}

void RmClearPrivateState(
    nv_state_t *pNv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(pNv);
    NvU32 status;
    void *pVbiosCopy = NULL;
    void *pRegistryCopy = NULL;
    NvU32 vbiosSize;
    nv_i2c_adapter_entry_t i2c_adapters[MAX_I2C_ADAPTERS];
    nv_dynamic_power_t dynamicPowerCopy;
    NvU32 x = 0;
    NvU32 pmc_boot_0, pmc_boot_1, pmc_boot_42;

    //
    // Do not clear private state after GPU resets, it is used while
    // recovering the GPU. Only clear the pGpu pointer, which is
    // restored during next initialization cycle.
    //
    if (pNv->flags & NV_FLAG_IN_RECOVERY)
    {
        nvp->pGpu = NULL;
    }

    status = nvp->status;
    pVbiosCopy = nvp->pVbiosCopy;
    vbiosSize = nvp->vbiosSize;
    pRegistryCopy = nvp->pRegistry;
    dynamicPowerCopy = nvp->dynamic_power;
    pmc_boot_0 = nvp->pmc_boot_0;
    pmc_boot_1 = nvp->pmc_boot_1;
    pmc_boot_42 = nvp->pmc_boot_42;

    for (x = 0; x < MAX_I2C_ADAPTERS; x++)
    {
        i2c_adapters[x] = nvp->i2c_adapters[x];
    }

    portMemSet(nvp, 0, sizeof(nv_priv_t));

    nvp->status = status;
    nvp->pVbiosCopy = pVbiosCopy;
    nvp->vbiosSize = vbiosSize;
    nvp->pRegistry = pRegistryCopy;
    nvp->dynamic_power = dynamicPowerCopy;
    nvp->pmc_boot_0 = pmc_boot_0;
    nvp->pmc_boot_1 = pmc_boot_1;
    nvp->pmc_boot_42 = pmc_boot_42;

    for (x = 0; x < MAX_I2C_ADAPTERS; x++)
    {
        nvp->i2c_adapters[x] = i2c_adapters[x];
    }

    nvp->flags |= NV_INIT_FLAG_PUBLIC_I2C;
}

void RmFreePrivateState(
    nv_state_t *pNv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(pNv);

    gpumgrUnregisterGpuId(pNv->gpu_id);

    kvgpumgrDetachGpu(pNv->gpu_id);

    RmDestroyRegistry(pNv);

    if (nvp != NULL)
    {
        portMemFree(nvp->pVbiosCopy);
        os_free_mem(nvp);
    }

    NV_SET_NV_PRIV(pNv, NULL);
}

NvBool RmPartiallyInitAdapter(
    nv_state_t *nv
)
{
    NV_PRINTF(LEVEL_INFO, "%s: %04x:%02x:%02x.0\n", __FUNCTION__,
              nv->pci_info.domain, nv->pci_info.bus, nv->pci_info.slot);

    nv_start_rc_timer(nv);

    return NV_TRUE;
}

static NV_STATUS
RmInitX86Emu(
    OBJGPU *pGpu
)
{
    NV_STATUS status = NV_OK;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    PORT_UNREFERENCED_VARIABLE(nv);

#if NVCPU_IS_X86_64
    status = RmInitX86EmuState(pGpu);
#else
    // We don't expect a "primary VGA" adapter on non-amd64 platforms
    NV_ASSERT(!NV_PRIMARY_VGA(nv));
#endif

    return status;
}

static NV_STATUS RmRegisterGpudb(
    OBJGPU       *pGpu
)
{
    NV_STATUS     rmStatus;
    const NvU8   *pGid;
    nv_state_t   *pNv = NV_GET_NV_STATE(pGpu);

    pGid = RmGetGpuUuidRaw(pNv);
    if (pGid == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get UUID\n");
        return NV_ERR_OPERATING_SYSTEM;
    }

    rmStatus = gpudbRegisterGpu(pGid, &pGpu->gpuClData.upstreamPort.addr,
                                pGpu->busInfo.nvDomainBusDeviceFunc);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to register GPU with GPU data base\n");
    }

    return rmStatus;
}

static void RmUnixFreeRmApi(
    nv_state_t *nv
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);

    if (nv->rmapi.hClient != 0)
    {
        pRmApi->Free(pRmApi, nv->rmapi.hClient, nv->rmapi.hClient);
    }

    portMemSet(&nv->rmapi, 0, sizeof(nv->rmapi));
}

static NvBool RmUnixAllocRmApi(
    nv_state_t *nv,
    NvU32 deviceId
)
{
    NV0080_ALLOC_PARAMETERS deviceParams = { 0 };
    NV2080_ALLOC_PARAMETERS subDeviceParams = { 0 };
    RM_API *pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);

    portMemSet(&nv->rmapi, 0, sizeof(nv->rmapi));

    if (pRmApi->AllocWithHandle(
                    pRmApi,
                    NV01_NULL_OBJECT,
                    NV01_NULL_OBJECT,
                    NV01_NULL_OBJECT,
                    NV01_ROOT,
                    &nv->rmapi.hClient,
                    sizeof(nv->rmapi.hClient)) != NV_OK)
    {
        goto fail;
    }

    //
    // Any call to rmapiDelPendingDevices() will internally delete the UNIX OS
    // layer RMAPI handles. Set this flag to preserve these handles. These
    // handles will be freed explicitly by RmUnixFreeRmApi().
    //
    if (!rmclientSetClientFlagsByHandle(nv->rmapi.hClient,
                                        RMAPI_CLIENT_FLAG_RM_INTERNAL_CLIENT))
    {
        goto fail;
    }

    deviceParams.deviceId = deviceId;

    if (pRmApi->Alloc(
                    pRmApi,
                    nv->rmapi.hClient,
                    nv->rmapi.hClient,
                    &nv->rmapi.hDevice,
                    NV01_DEVICE_0,
                    &deviceParams,
                    sizeof(deviceParams)) != NV_OK)
    {
        goto fail;
    }

    subDeviceParams.subDeviceId = 0;

    if (pRmApi->Alloc(
                    pRmApi,
                    nv->rmapi.hClient,
                    nv->rmapi.hDevice,
                    &nv->rmapi.hSubDevice,
                    NV20_SUBDEVICE_0,
                    &subDeviceParams,
                    sizeof(subDeviceParams)) != NV_OK)
    {
        goto fail;
    }

    //
    // The NV40_I2C allocation expected to fail, if it is disabled
    // with RM config.
    //
    if (pRmApi->Alloc(
                    pRmApi,
                    nv->rmapi.hClient,
                    nv->rmapi.hSubDevice,
                    &nv->rmapi.hI2C,
                    NV40_I2C,
                    NULL,
                    0) != NV_OK)
    {
        nv->rmapi.hI2C = 0;
    }

    //
    // The NV04_DISPLAY_COMMON allocation expected to fail for displayless
    // system. nv->rmapi.hDisp value needs to be checked before doing display
    // related control calls.
    //
    if (pRmApi->Alloc(
                    pRmApi,
                    nv->rmapi.hClient,
                    nv->rmapi.hDevice,
                    &nv->rmapi.hDisp,
                    NV04_DISPLAY_COMMON,
                    NULL,
                    0) != NV_OK)
    {
        nv->rmapi.hDisp = 0;
    }

    return NV_TRUE;

fail:
    RmUnixFreeRmApi(nv);
    return NV_FALSE;
}

static NV_STATUS RmFetchGspRmImages
(
    nv_state_t    *nv,
    GSP_FIRMWARE  *pGspFw,
    const void   **gspFwHandle,
    const void   **gspFwLogHandle
)
{
    nv_firmware_chip_family_t chipFamily;
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    NvU32 gpuArch = (gpuGetArchitectureFromPmcBoot42(nvp->pmc_boot_42) <<
                     GPU_ARCH_SHIFT);
    NvU32 gpuImpl = DRF_VAL(_PMC, _BOOT_42, _IMPLEMENTATION, nvp->pmc_boot_42);

    chipFamily = nv_firmware_get_chip_family(gpuArch, gpuImpl);

    portMemSet(pGspFw, 0, sizeof(*pGspFw));

    *gspFwHandle = nv_get_firmware(nv, NV_FIRMWARE_TYPE_GSP,
                                   chipFamily,
                                   &pGspFw->pBuf,
                                   &pGspFw->size);
    if (*gspFwHandle == NULL &&
        !nv->allow_fallback_to_monolithic_rm)
    {
        NV_PRINTF(LEVEL_ERROR, "No firmware image found\n");
        return NV_ERR_NOT_SUPPORTED;
    }
    else if (*gspFwHandle != NULL)
    {
#if LIBOS_LOG_DECODE_ENABLE
        if (nv->enable_firmware_logs)
        {
            *gspFwLogHandle = nv_get_firmware(nv, NV_FIRMWARE_TYPE_GSP_LOG,
                                              chipFamily,
                                              &pGspFw->pLogElf,
                                              &pGspFw->logElfSize);
            if (*gspFwLogHandle == NULL)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to load gsp_log_*.bin, no GSP-RM logs will be printed (non-fatal)\n");
            }
        }
#endif
        nv->request_fw_client_rm = NV_TRUE;
    }
    else
    {
        nv->request_fw_client_rm = NV_FALSE;
    }

    return NV_OK;
}

NvBool RmInitAdapter(
    nv_state_t *nv
)
{
    NvU32           devicereference = 0;
    UNIX_STATUS     status = INIT_UNIX_STATUS;
    nv_priv_t      *nvp;
    NvBool          retVal = NV_FALSE;
    OBJSYS         *pSys;
    OBJGPU         *pGpu = NULL;
    OBJOS          *pOS;
    KernelDisplay  *pKernelDisplay;
    const void     *gspFwHandle = NULL;
    const void     *gspFwLogHandle = NULL;
    NvBool          consoleDisabled = NV_FALSE;

    GSP_FIRMWARE    gspFw = {0};
    PORT_UNREFERENCED_VARIABLE(gspFw);

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "RmInitAdapter\n");

    nv->flags &= ~NV_FLAG_PASSTHRU;

    RmSetupRegisters(nv, &status);
    if (! RM_INIT_SUCCESS(status.initStatus) )
        goto failed;

    nvp = NV_GET_NV_PRIV(nv);
    nvp->status = NV_ERR_OPERATING_SYSTEM;

    status.rmStatus = RmInitDeviceDma(nv);
    if (status.rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot configure the device for DMA\n");
        RM_SET_ERROR(status, RM_INIT_GPU_DMA_CONFIGURATION_FAILED);
        goto shutdown;
    }

    nvp->flags |= NV_INIT_FLAG_DMA;

    pSys = SYS_GET_INSTANCE();

    //
    // Get firmware from the OS, if requested, and decide if RM will run as a
    // firmware client.
    //
    if (nv->request_firmware)
    {
        nv_set_dma_address_size(nv, NV_GSP_GPU_MIN_SUPPORTED_DMA_ADDR_WIDTH);

        status.rmStatus = RmFetchGspRmImages(nv, &gspFw, &gspFwHandle, &gspFwLogHandle);
        if (status.rmStatus != NV_OK)
        {
            RM_SET_ERROR(status, RM_INIT_FIRMWARE_FETCH_FAILED);
            goto shutdown;
        }
    }

    //
    // Initialization path requires expanded GPU visibility in GPUMGR
    // in order to access the GPU undergoing initialization.
    //
    status.rmStatus = gpumgrThreadEnableExpandedGpuVisibility();
    if (status.rmStatus != NV_OK)
    {
        RM_SET_ERROR(status, RM_INIT_GPU_GPUMGR_EXPANDED_VISIBILITY_FAILED);
        goto shutdown;
    }

    // initialize the RM device register mapping
    osInitNvMapping(nv, &devicereference, &status);
    if (! RM_INIT_SUCCESS(status.initStatus) )
    {
        switch (status.rmStatus)
        {
            case NV_ERR_NOT_SUPPORTED:
                nvp->status = NV_ERR_NOT_SUPPORTED;
                break;
        }
        NV_PRINTF(LEVEL_ERROR,
                  "osInitNvMapping failed, bailing out of RmInitAdapter\n");
        goto shutdown;
    }

    //
    // now we can have a pdev for the first time...
    //
    pGpu   = gpumgrGetGpu(devicereference);

    pOS    = SYS_GET_OS(pSys);

    status.rmStatus = osInitScalability(pGpu);
    if (status.rmStatus == NV_OK)
    {
        nvp->flags |= NV_INIT_FLAG_SCALABILITY;
    }
    else if (status.rmStatus != NV_ERR_NOT_SUPPORTED)
    {
        RM_SET_ERROR(status, RM_INIT_SCALABILITY_FAILED);
        goto shutdown;
    }

    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    if ((pKernelFsp != NULL) && !IS_GSP_CLIENT(pGpu) && !IS_VIRTUAL(pGpu))
    {
        status.rmStatus = kfspPrepareAndSendBootCommands_HAL(pGpu, pKernelFsp);
        if (status.rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "FSP boot command failed.\n");
            RM_SET_ERROR(status, RM_INIT_FIRMWARE_INIT_FAILED);
            goto shutdown;
        }
    }

    RmDeterminePrimaryDevice(pGpu);

    RmSetConsolePreservationParams(pGpu);

    RmInitAcpiMethods(pOS, pSys, pGpu);

    //
    // For GPU driving console, disable console access here, to ensure no console
    // writes through BAR1 can interfere with physical RM's setup of BAR1
    //
    if (rm_get_uefi_console_status(nv))
    {
        os_disable_console_access();
        consoleDisabled = NV_TRUE;
    }

    //
    // If GSP fw RM support is enabled then start the GSP microcode
    // (including the task running the full instance of the RM) and
    // exchange the necessary initial RPC messages before continuing
    // with GPU initialization here.
    //
    if (IS_GSP_CLIENT(pGpu))
    {
        status.rmStatus = kgspInitRm(pGpu, GPU_GET_KERNEL_GSP(pGpu), &gspFw);
        if (status.rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Cannot initialize GSP firmware RM\n");
            RM_SET_ERROR(status, RM_INIT_FIRMWARE_INIT_FAILED);
            goto shutdown;
        }
    }
    else if (nv->request_fw_client_rm)
    {
        // We were expecting to enable GSP-RM but something went wrong.
        if (!nv->allow_fallback_to_monolithic_rm)
        {
            RM_SET_ERROR(status, RM_INIT_FIRMWARE_POLICY_FAILED);
            goto shutdown;
        }
        else
        {
            NV_PRINTF(LEVEL_NOTICE, "Falling back to monolithic RM\n");
        }
    }

    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    if (pKernelDisplay != NULL)
    {
        kdispSetWarPurgeSatellitesOnCoreFree(pKernelDisplay, NV_TRUE);
    }

    if (IS_PASSTHRU(pGpu))
        nv->flags |= NV_FLAG_PASSTHRU;

    status.rmStatus = RmInitX86Emu(pGpu);
    if (status.rmStatus != NV_OK)
    {
        RM_SET_ERROR(status, RM_INIT_VBIOS_X86EMU_FAILED);
        NV_PRINTF(LEVEL_ERROR,
                  "RmInitX86Emu failed, bailing out of RmInitAdapter\n");
        goto shutdown;
    }

    initVendorSpecificRegistry(pGpu, nv->pci_info.device_id);
    if (!IS_VIRTUAL(pGpu))
    {
        initNbsiTable(pGpu);
    }

    // finally, initialize the device
    RmInitNvDevice(devicereference, &status);
    if (! RM_INIT_SUCCESS(status.initStatus) )
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RmInitNvDevice failed, bailing out of RmInitAdapter\n");
        switch (status.rmStatus)
        {
            case NV_ERR_INSUFFICIENT_POWER:
                nvp->status = NV_ERR_INSUFFICIENT_POWER;
                NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                    "GPU does not have the necessary power cables connected.\n");
                break;
        }
        goto shutdown;
    }

    if (consoleDisabled)
    {
        os_enable_console_access();
        consoleDisabled = NV_FALSE;
    }

    //
    // Expanded GPU visibility in GPUMGR is no longer needed once the
    // GPU is initialized.
    //
    gpumgrThreadDisableExpandedGpuVisibility();

    // LOCK: acquire GPUs lock
    status.rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                        RM_LOCK_MODULES_INIT);
    if (status.rmStatus != NV_OK)
    {
        goto shutdown;
    }

    status.rmStatus = osVerifySystemEnvironment(pGpu);

    // UNLOCK: release GPUs lock
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    if (status.rmStatus != NV_OK)
    {
        RM_SET_ERROR(status, RM_INIT_SYS_ENVIRONMENT_FAILED);
        switch (status.rmStatus)
        {
            case NV_ERR_IRQ_NOT_FIRING:
                nvp->status = NV_ERR_IRQ_NOT_FIRING;
                break;
        }
        NV_PRINTF(LEVEL_ERROR, "osVerifySystemEnvironment failed, bailing!\n");
        goto shutdown;
    }

    Intr *pIntr = GPU_GET_INTR(pGpu);
    if (pIntr != NULL)
    {
        intrSetIntrEn(pIntr, INTERRUPT_TYPE_HARDWARE);
    }

    // LOCK: acquire GPUs lock
    status.rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                        RM_LOCK_MODULES_INIT);
    if (status.rmStatus != NV_OK)
    {
        goto shutdown;
    }

    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    // initialize the watchdog (disabled by default)
    status.rmStatus = pKernelRc != NULL ? krcWatchdogInit_HAL(pGpu, pKernelRc) :
                                          NV_ERR_NOT_SUPPORTED;
    if (status.rmStatus == NV_OK)
    {
        krcWatchdogDisable(pKernelRc);
        nvp->flags |= NV_INIT_FLAG_FIFO_WATCHDOG;
    }
    else if (status.rmStatus == NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_INFO,
            "krcWatchdogInit returned _NOT_SUPPORTED. For Kepler GPUs in PGOB mode, this is normal\n");
    }
    else
    {
        RM_SET_ERROR(status, RM_INIT_WATCHDOG_FAILED);
        NV_PRINTF(LEVEL_ERROR,
                  "krcWatchdogInit failed, bailing out of RmInitAdapter\n");
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        goto shutdown;
    }

    // UNLOCK: release GPUs lock
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    nv_start_rc_timer(nv);

    nvp->status = NV_OK;

    if (!RmUnixAllocRmApi(nv, devicereference)) {
        RM_SET_ERROR(status, RM_INIT_ALLOC_RMAPI_FAILED);
        status.rmStatus = NV_ERR_GENERIC;
        goto shutdown;
    }

    status.rmStatus = RmInitGpuInfoWithRmApi(pGpu);
    if (status.rmStatus != NV_OK)
    {
        RM_SET_ERROR(status, RM_INIT_GPUINFO_WITH_RMAPI_FAILED);
        goto shutdown;
    }

    // i2c only on master device??
    RmI2cAddGpuPorts(nv);
    nvp->flags |= NV_INIT_FLAG_PUBLIC_I2C;

    // This fifo hal call will fail on pre-fermi gpus. In that case, userd
    // info will remain 0 and skipped by fb mmap code
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU64       udaddr = 0;
    NvU32       udsize = 0;
    if (pKernelFifo != NULL)
    {
        status.rmStatus = kfifoGetUserdBar1MapInfo_HAL(pGpu, pKernelFifo, &udaddr, &udsize);
        if (status.rmStatus == NV_OK)
        {
            nv->ud.cpu_address = (nv->fb->cpu_address + udaddr);
            nv->ud.size = udsize;
        }
        else if (status.rmStatus != NV_ERR_NOT_SUPPORTED)
        {
            RM_SET_ERROR(status, RM_FIFO_GET_UD_BAR1_MAP_INFO_FAILED);
            NV_PRINTF(LEVEL_ERROR,
                    "kfifoGetUserdBar1MapInfo failed, bailing out of RmInitAdapter\n");
            goto shutdown;
        }
    }

    nv->flags &= ~NV_FLAG_IN_RECOVERY;

    pOS->setProperty(pOS, PDB_PROP_OS_SYSTEM_EVENTS_SUPPORTED, NV_TRUE);

    RmInitPowerManagement(nv);

    if (!NV_IS_SOC_DISPLAY_DEVICE(nv) && !NV_IS_SOC_IGPU_DEVICE(nv))
    {
        status.rmStatus = RmRegisterGpudb(pGpu);
        if (status.rmStatus != NV_OK)
        {
            RM_SET_ERROR(status, RM_GPUDB_REGISTER_FAILED);
            goto shutdown;
        }
    }

    if (nvp->b_mobile_config_enabled)
    {
        NvU32 ac_plugged = 0;
        if (nv_acpi_get_powersource(&ac_plugged) == NV_OK)
        {
            // LOCK: acquire GPU lock
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_NONE) == NV_OK)
            {
                //
                // As we have already acquired the API Lock here, we are calling
                // RmPowerSourceChangeEvent directly instead of rm_power_source_change_event.
                //
                RmPowerSourceChangeEvent(nv, !ac_plugged);

                // UNLOCK: release GPU lock
                rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            }
        }
    }

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "RmInitAdapter succeeded!\n");

    retVal = NV_TRUE;
    goto done;

 shutdown:
    nv->flags &= ~NV_FLAG_IN_RECOVERY;

    gpumgrThreadDisableExpandedGpuVisibility();

    if (consoleDisabled)
    {
        os_enable_console_access();
    }

    // call ShutdownAdapter to undo anything we've done above
    RmShutdownAdapter(nv);

 failed:
    NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "RmInitAdapter failed! (0x%x:0x%x:%d)\n",
        status.initStatus, status.rmStatus, status.line);

done:
    nv_put_firmware(gspFwHandle);
    nv_put_firmware(gspFwLogHandle);

    return retVal;
}

void RmShutdownAdapter(
    nv_state_t *nv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);
    NV_STATUS rmStatus;

    if ((pGpu != NULL) && (nvp->flags & NV_INIT_FLAG_GPUMGR_ATTACH))
    {
        NvU32 gpuInstance    = gpuGetInstance(pGpu);
        NvU32 deviceInstance = gpuGetDeviceInstance(pGpu);
        OBJSYS         *pSys = SYS_GET_INSTANCE();

        RmUnixFreeRmApi(nv);

        nv->ud.cpu_address = 0;
        nv->ud.size = 0;

        //
        // LOCK: lock all clients in case of eGPU hot unplug, which
        // will not wait for all existing RM clients to stop using the GPU.
        //
        if (!nv->is_external_gpu || serverLockAllClients(&g_resServ) == NV_OK)
        {
            // LOCK: acquire GPUs lock
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DESTROY) == NV_OK)
            {
                //
                // Shutdown path requires expanded GPU visibility in GPUMGR in order
                // to access the GPU undergoing shutdown which may not be fully
                // initialized, and to continue accessing the GPU undergoing shutdown
                // after state destroy.
                //
                NV_ASSERT_OK(gpumgrThreadEnableExpandedGpuVisibility());

                RmDestroyPowerManagement(nv);

                freeNbsiTable(pGpu);

                gpuFreeEventHandle(pGpu);

                OBJCL          *pCl  = SYS_GET_CL(pSys);
                OBJOS          *pOS  = GPU_GET_OS(pGpu);
                if (pCl != NULL)
                {
                    if (nvp->flags & NV_INIT_FLAG_CORE_LOGIC)
                    {
                        teardownCoreLogic(pOS, pGpu);
                    }
                    else if (nvp->flags & NV_INIT_FLAG_SCALABILITY)
                    {
                        osTeardownScalability(pGpu);
                    }
                }

                rmapiSetDelPendingClientResourcesFromGpuMask(NVBIT(gpuInstance));
                rmapiDelPendingDevices(NVBIT(gpuInstance));

                os_disable_console_access();

                if (nvp->flags & NV_INIT_FLAG_GPU_STATE_LOAD)
                {
                    rmStatus = gpuStateUnload(pGpu, GPU_STATE_DEFAULT);
                    NV_ASSERT(rmStatus == NV_OK);
                }

                if (nvp->flags & NV_INIT_FLAG_GPU_STATE)
                {
                    rmStatus = gpuStateDestroy(pGpu);
                    NV_ASSERT(rmStatus == NV_OK);
                }

                os_enable_console_access();

                //if (nvp->flags & NV_INIT_FLAG_HAL)
                //  destroyHal(pDev);

#if NVCPU_IS_X86_64
                RmFreeX86EmuState(pGpu);
#endif

                gpumgrDetachGpu(gpuInstance);
                gpumgrDestroyDevice(deviceInstance);

                //
                // Expanded GPU visibility in GPUMGR is no longer needed once the
                // GPU is removed from GPUMGR.
                //
                gpumgrThreadDisableExpandedGpuVisibility();

                if (nvp->flags & NV_INIT_FLAG_DMA)
                {
                    RmTeardownDeviceDma(nv);
                }

                RmClearPrivateState(nv);

                RmUnInitAcpiMethods(pSys);

                // UNLOCK: release GPUs lock
                rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

                // RM_BASIC_LOCK_MODEL: free GPU lock
                rmGpuLockFree(deviceInstance);
            }

            // UNLOCK: unlock all clients for eGPU hot unplug path
            if (nv->is_external_gpu)
                serverUnlockAllClients(&g_resServ);
        }
    }
    else
    {
        RmClearPrivateState(nv);
    }

    RmTeardownRegisters(nv);
}

void RmPartiallyDisableAdapter(
    nv_state_t *nv
)
{
    NV_PRINTF(LEVEL_INFO, "%s: RM is in SW Persistence mode\n", __FUNCTION__);

    nv_stop_rc_timer(nv);
}

void RmDisableAdapter(
    nv_state_t *nv
)
{
    NV_STATUS  rmStatus;
    OBJGPU    *pGpu = NV_GET_NV_PRIV_PGPU(nv);
    NvU32      gpuMask;
    nv_priv_t *nvp  = NV_GET_NV_PRIV(nv);

    //
    // Normally, we re-enable interrupts when we release the lock
    // after RmDisableAdapter(), before disabling the bottom-half ISR.
    // If an interrupt came in in that time window, but the bottom half
    // wasn't scheduled to run in time, the semaphore acquired by the
    // top-half would never be released afterwards (since the bottom-half
    // would never run). This call to intrSetIntrEn() ensures that this won't
    // happen, by making sure interrupts remain disabled on the GPU in question after
    // RmDisableAdapter().
    //
    Intr *pIntr = GPU_GET_INTR(pGpu);
    if (pIntr != NULL)
    {
        intrSetIntrEn(pIntr, INTERRUPT_TYPE_DISABLED);
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET))
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_TIMEOUT_RECOVERY, NV_TRUE);
        nv->flags |= NV_FLAG_IN_RECOVERY;
    }

    //
    // LOCK: lock all clients in case of eGPU hot unplug, which
    // will not wait for all existing RM clients to stop using the GPU.
    //
    if (!nv->is_external_gpu || serverLockAllClients(&g_resServ) == NV_OK)
    {

        // LOCK: acquire GPUs lock
        if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DESTROY) == NV_OK)
        {
            //
            // Free the client allocated resources.
            //
            // This needs to happen prior to tearing down SLI state when SLI is enabled.
            //
            // Note this doesn't free RM internal resource allocations. Those are
            // freed during (gpumgrUpdateSLIConfig->...->)gpuStateUnload.
            //
            // We need to free resources for all GPUs linked in a group as
            // gpumgrUpdateSLIConfig will teardown GPU state for the entire set.
            //
            gpuMask = gpumgrGetGpuMask(pGpu);

            rmapiSetDelPendingClientResourcesFromGpuMask(gpuMask);
            rmapiDelPendingDevices(gpuMask);

            nv_stop_rc_timer(nv);

            OBJSYS    *pSys = SYS_GET_INSTANCE();
            OBJCL     *pCl  = SYS_GET_CL(pSys);
            OBJOS     *pOS  = SYS_GET_OS(pSys);
            if (pCl != NULL)
            {
                teardownCoreLogic(pOS, pGpu);
                nvp->flags &= ~NV_INIT_FLAG_CORE_LOGIC;
            }

            if (nvp->flags & NV_INIT_FLAG_FIFO_WATCHDOG)
            {
                krcWatchdogShutdown(pGpu, GPU_GET_KERNEL_RC(pGpu));
                nvp->flags &= ~NV_INIT_FLAG_FIFO_WATCHDOG;
            }

            os_disable_console_access();

            if (nvp->flags & NV_INIT_FLAG_GPU_STATE_LOAD)
            {
                rmStatus = gpuStateUnload(pGpu, GPU_STATE_DEFAULT);
                NV_ASSERT(rmStatus == NV_OK);
                nvp->flags &= ~NV_INIT_FLAG_GPU_STATE_LOAD;
            }

            os_enable_console_access();

            // UNLOCK: release GPUs lock
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        }

        // UNLOCK: unlock all clients for eGPU hot unplug path
        if (nv->is_external_gpu)
            serverUnlockAllClients(&g_resServ);
    }
}

NV_STATUS RmGetAdapterStatus(
    nv_state_t *pNv,
    NvU32      *pStatus
)
{
    //
    // This status is determined in RmInitAdapter(); the glue layer
    // requests it when the adapter failed to initialize to learn
    // more about the error condition. This is currently limited to
    // osVerifySystemEnvironment() failures.
    //
    nv_priv_t *nvp;

    nvp = NV_GET_NV_PRIV(pNv);
    if (nvp == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    *pStatus = nvp->status;
    return NV_OK;
}

static void initVendorSpecificRegistry(
    OBJGPU *pGpu,
    NvU16 device_id
)
{
    NV_STATUS rmStatus;
    NvU32 i;
    NvU32 subsystem_id;
    NvU32 subsystem_vendor_id;
    NvU32 subsystem_device_id;
    NvU32 vendor_id = 0;

    if (!pGpu)
        return;

    rmStatus = GPU_BUS_CFG_RD32(pGpu,
            NV_CONFIG_PCI_NV_11, &subsystem_id);
    if (rmStatus != NV_OK)
    {
       NV_PRINTF(LEVEL_ERROR,
                 "%s: Cannot read NV_CONFIG_PCI_NV_11\n", __FUNCTION__);
       return;
    }

    subsystem_vendor_id = (subsystem_id & 0xffff);
    subsystem_device_id = (subsystem_id >> 16);

    for (i = 0; (nb_id_table[i].subsystem_vendor_id) != 0; i++)
    {
        if ((nb_id_table[i].subsystem_vendor_id == subsystem_vendor_id) &&
            (nb_id_table[i].subsystem_device_id == subsystem_device_id) &&
            (nb_id_table[i].gpu_device_id == device_id))
        {
            vendor_id = subsystem_vendor_id;
            break;
        }
    }

    if (vendor_id != 0)
    {
        for (i = 0; nb_reg_table[i].vendor_id != 0; i++)
        {
            if (nb_reg_table[i].vendor_id == vendor_id)
            {
                osWriteRegistryDword(pGpu, nb_reg_table[i].name,
                        nb_reg_table[i].data);
            }
        }
    }
}

static void initUnixSpecificRegistry(
    OBJGPU *pGpu
)
{
    // By default, enable GPU reset on Unix
    osWriteRegistryDword(pGpu, "RMSecBusResetEnable", 1);
    osWriteRegistryDword(pGpu, "RMForcePcieConfigSave", 1);

}

void
osRemoveGpu(
    NvU32 domain,
    NvU8 bus,
    NvU8 device
)
{
    void   *handle;

    handle = os_pci_init_handle(domain, bus, device, 0, NULL, NULL);
    if (handle != NULL)
    {
        os_pci_remove(handle);
    }
}

/*
 * Check to see if this board supports GPU exclusion.
 */
static NvBool RmIsExcludingAllowed(
    nv_state_t *pNv
)
{
    NvU32 feature = 0;
    NvU32 brand;

    // DGX-2/HGX-2 systems pre-date the PBI call
    if (pNv->pci_info.device_id == 0x1db8)
        return NV_TRUE;

    if (pciPbiGetFeature(pNv->handle, &feature) != NV_OK)
        return NV_FALSE;

    brand = DRF_VAL(_PBI, _EXECUTE_ROUTINE, _GET_FEATURE_EXCLUSION, feature);

    return (brand == NV_PBI_EXECUTE_ROUTINE_GET_FEATURE_EXCLUSION_ALLOWED);
}

NV_STATUS RmExcludeAdapter(
    nv_state_t *nv
)
{
    NV_STATUS rmStatus;

    if (!RmIsExcludingAllowed(nv))
        return NV_ERR_NOT_SUPPORTED;

    rmStatus = gpumgrExcludeGpuId(nv->gpu_id);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to exclude GPU: 0x%x\n", rmStatus);
        return rmStatus;
    }

    /*
     * GPU exclusion only happens during initialization, and therefore there
     * is no need to stop activity on the GPU
     */

    return rmStatus;
}

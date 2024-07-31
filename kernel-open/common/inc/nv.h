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


#ifndef _NV_H_
#define _NV_H_



#include <nvlimits.h>

#if defined(NV_KERNEL_INTERFACE_LAYER) && defined(__FreeBSD__)
  #include <sys/stddef.h>   // NULL
#elif defined(NV_KERNEL_INTERFACE_LAYER) && defined(NV_LINUX)
  #include <linux/stddef.h> // NULL
#else
  #include <stddef.h>       // NULL
#endif

#include <nvstatus.h>
#include "nv_stdarg.h"
#include <nv-caps.h>
#include <nv-firmware.h>
#include <nv-ioctl.h>
#include <nv-ioctl-numa.h>
#include <nvmisc.h>

extern nv_cap_t *nvidia_caps_root;

extern const NvBool nv_is_rm_firmware_supported_os;

#include <nv-kernel-interface-api.h>

#define GPU_UUID_LEN    (16)

/*
 * Buffer size for an ASCII UUID: We need 2 digits per byte, plus space
 * for "GPU", 5 dashes, and '\0' termination:
 */
#define GPU_UUID_ASCII_LEN  (GPU_UUID_LEN * 2 + 9)

/*
 * #define an absolute maximum used as a sanity check for the
 * NV_ESC_IOCTL_XFER_CMD ioctl() size argument.
 */
#define NV_ABSOLUTE_MAX_IOCTL_SIZE  16384

/*
 * Solaris provides no more than 8 bits for the argument size in
 * the ioctl() command encoding; make sure we don't exceed this
 * limit.
 */
#define __NV_IOWR_ASSERT(type) ((sizeof(type) <= NV_PLATFORM_MAX_IOCTL_SIZE) ? 1 : -1)
#define __NV_IOWR(nr, type) ({                                        \
    typedef char __NV_IOWR_TYPE_SIZE_ASSERT[__NV_IOWR_ASSERT(type)];  \
    _IOWR(NV_IOCTL_MAGIC, (nr), type);                                \
})

#define NV_PCI_DEV_FMT          "%04x:%02x:%02x.%x"
#define NV_PCI_DEV_FMT_ARGS(nv) (nv)->pci_info.domain, (nv)->pci_info.bus, \
                                (nv)->pci_info.slot, (nv)->pci_info.function

#define NV_RM_DEVICE_INTR_ADDRESS 0x100

/*!
 * @brief The order of the display clocks in the below defined enum
 * should be synced with below mapping array and macro.
 * All four should be updated simultaneously in case
 * of removal or addition of clocks in below order.
 * Also, TEGRASOC_WHICH_CLK_MAX is used in various places
 * in below mentioned files.
 * arch/nvalloc/unix/Linux/nv-linux.h
 *
 * arch/nvalloc/unix/src/os.c
 * dispClkMapRmToOsArr[] = {...};
 *
 * arch/nvalloc/unix/Linux/nv-clk.c
 * osMapClk[] = {...};
 *
 */
typedef enum _TEGRASOC_WHICH_CLK
{
    TEGRASOC_WHICH_CLK_NVDISPLAYHUB,
    TEGRASOC_WHICH_CLK_NVDISPLAY_DISP,
    TEGRASOC_WHICH_CLK_NVDISPLAY_P0,
    TEGRASOC_WHICH_CLK_NVDISPLAY_P1,
    TEGRASOC_WHICH_CLK_DPAUX0,
    TEGRASOC_WHICH_CLK_FUSE,
    TEGRASOC_WHICH_CLK_DSIPLL_VCO,
    TEGRASOC_WHICH_CLK_DSIPLL_CLKOUTPN,
    TEGRASOC_WHICH_CLK_DSIPLL_CLKOUTA,
    TEGRASOC_WHICH_CLK_SPPLL0_VCO,
    TEGRASOC_WHICH_CLK_SPPLL0_CLKOUTA,
    TEGRASOC_WHICH_CLK_SPPLL0_CLKOUTB,
    TEGRASOC_WHICH_CLK_SPPLL0_CLKOUTPN,
    TEGRASOC_WHICH_CLK_SPPLL1_CLKOUTPN,
    TEGRASOC_WHICH_CLK_SPPLL0_DIV27,
    TEGRASOC_WHICH_CLK_SPPLL1_DIV27,
    TEGRASOC_WHICH_CLK_SPPLL0_DIV10,
    TEGRASOC_WHICH_CLK_SPPLL0_DIV25,
    TEGRASOC_WHICH_CLK_SPPLL1_VCO,
    TEGRASOC_WHICH_CLK_VPLL0_REF,
    TEGRASOC_WHICH_CLK_VPLL0,
    TEGRASOC_WHICH_CLK_VPLL1,
    TEGRASOC_WHICH_CLK_NVDISPLAY_P0_REF,
    TEGRASOC_WHICH_CLK_RG0,
    TEGRASOC_WHICH_CLK_RG1,
    TEGRASOC_WHICH_CLK_DISPPLL,
    TEGRASOC_WHICH_CLK_DISPHUBPLL,
    TEGRASOC_WHICH_CLK_DSI_LP,
    TEGRASOC_WHICH_CLK_DSI_CORE,
    TEGRASOC_WHICH_CLK_DSI_PIXEL,
    TEGRASOC_WHICH_CLK_PRE_SOR0,
    TEGRASOC_WHICH_CLK_PRE_SOR1,
    TEGRASOC_WHICH_CLK_DP_LINKA_REF,
    TEGRASOC_WHICH_CLK_SOR_LINKA_INPUT,
    TEGRASOC_WHICH_CLK_SOR_LINKA_AFIFO,
    TEGRASOC_WHICH_CLK_SOR_LINKA_AFIFO_M,
    TEGRASOC_WHICH_CLK_RG0_M,
    TEGRASOC_WHICH_CLK_RG1_M,
    TEGRASOC_WHICH_CLK_SOR0_M,
    TEGRASOC_WHICH_CLK_SOR1_M,
    TEGRASOC_WHICH_CLK_PLLHUB,
    TEGRASOC_WHICH_CLK_SOR0,
    TEGRASOC_WHICH_CLK_SOR1,
    TEGRASOC_WHICH_CLK_SOR_PADA_INPUT,
    TEGRASOC_WHICH_CLK_PRE_SF0,
    TEGRASOC_WHICH_CLK_SF0,
    TEGRASOC_WHICH_CLK_SF1,
    TEGRASOC_WHICH_CLK_DSI_PAD_INPUT,
    TEGRASOC_WHICH_CLK_PRE_SOR0_REF,
    TEGRASOC_WHICH_CLK_PRE_SOR1_REF,
    TEGRASOC_WHICH_CLK_SOR0_PLL_REF,
    TEGRASOC_WHICH_CLK_SOR1_PLL_REF,
    TEGRASOC_WHICH_CLK_SOR0_REF,
    TEGRASOC_WHICH_CLK_SOR1_REF,
    TEGRASOC_WHICH_CLK_OSC,
    TEGRASOC_WHICH_CLK_DSC,
    TEGRASOC_WHICH_CLK_MAUD,
    TEGRASOC_WHICH_CLK_AZA_2XBIT,
    TEGRASOC_WHICH_CLK_AZA_BIT,
    TEGRASOC_WHICH_CLK_MIPI_CAL,
    TEGRASOC_WHICH_CLK_UART_FST_MIPI_CAL,
    TEGRASOC_WHICH_CLK_SOR0_DIV,
    TEGRASOC_WHICH_CLK_DISP_ROOT,
    TEGRASOC_WHICH_CLK_HUB_ROOT,
    TEGRASOC_WHICH_CLK_PLLA_DISP,
    TEGRASOC_WHICH_CLK_PLLA_DISPHUB,
    TEGRASOC_WHICH_CLK_PLLA,
    TEGRASOC_WHICH_CLK_MAX, // TEGRASOC_WHICH_CLK_MAX is defined for boundary checks only.
} TEGRASOC_WHICH_CLK;

#ifdef NVRM

extern const char *pNVRM_ID;

/*
 * ptr arithmetic convenience
 */

typedef union
{
    volatile NvV8 Reg008[1];
    volatile NvV16 Reg016[1];
    volatile NvV32 Reg032[1];
} nv_hwreg_t, * nv_phwreg_t;


#define NVRM_PCICFG_NUM_BARS            6
#define NVRM_PCICFG_BAR_OFFSET(i)       (0x10 + (i) * 4)
#define NVRM_PCICFG_BAR_REQTYPE_MASK    0x00000001
#define NVRM_PCICFG_BAR_REQTYPE_MEMORY  0x00000000
#define NVRM_PCICFG_BAR_MEMTYPE_MASK    0x00000006
#define NVRM_PCICFG_BAR_MEMTYPE_64BIT   0x00000004
#define NVRM_PCICFG_BAR_ADDR_MASK       0xfffffff0

#define NVRM_PCICFG_NUM_DWORDS          16

#define NV_GPU_NUM_BARS                 3
#define NV_GPU_BAR_INDEX_REGS           0
#define NV_GPU_BAR_INDEX_FB             1
#define NV_GPU_BAR_INDEX_IMEM           2

typedef struct
{
    NvU64 cpu_address;
    NvU64 size;
    NvU32 offset;
    NvU32 *map;
    nv_phwreg_t map_u;
} nv_aperture_t;

typedef struct
{
    char *name;
    NvU32 *data;
} nv_parm_t;

#define NV_RM_PAGE_SHIFT    12
#define NV_RM_PAGE_SIZE     (1 << NV_RM_PAGE_SHIFT)
#define NV_RM_PAGE_MASK     (NV_RM_PAGE_SIZE - 1)

#define NV_RM_TO_OS_PAGE_SHIFT      (os_page_shift - NV_RM_PAGE_SHIFT)
#define NV_RM_PAGES_TO_OS_PAGES(count) \
    ((((NvUPtr)(count)) >> NV_RM_TO_OS_PAGE_SHIFT) + \
     ((((count) & ((1 << NV_RM_TO_OS_PAGE_SHIFT) - 1)) != 0) ? 1 : 0))

#if defined(NVCPU_X86_64)
#define NV_STACK_SIZE (NV_RM_PAGE_SIZE * 3)
#else
#define NV_STACK_SIZE (NV_RM_PAGE_SIZE * 2)
#endif

typedef struct nvidia_stack_s
{
    NvU32 size;
    void *top;
    NvU8  stack[NV_STACK_SIZE-16] __attribute__ ((aligned(16)));
} nvidia_stack_t;

/*
 * TODO: Remove once all UNIX layers have been converted to use nvidia_stack_t
 */
typedef nvidia_stack_t nv_stack_t;

typedef struct nv_file_private_t nv_file_private_t;

/*
 * this is a wrapper for unix events
 * unlike the events that will be returned to clients, this includes
 * kernel-specific data, such as file pointer, etc..
 */
typedef struct nv_event_s
{
    NvHandle            hParent;
    NvHandle            hObject;
    NvU32               index;
    NvU32               info32;
    NvU16               info16;
    nv_file_private_t  *nvfp;  /* per file-descriptor data pointer */
    NvU32               fd;
    NvBool              active; /* whether the event should be signaled */
    NvU32               refcount; /* count of associated RM events */
    struct nv_event_s  *next;
} nv_event_t;

typedef struct nv_kern_mapping_s
{
    void  *addr;
    NvU64 size;
    NvU32 modeFlag;
    struct nv_kern_mapping_s *next;
} nv_kern_mapping_t;

typedef struct nv_usermap_access_params_s
{
    NvU64    addr;
    NvU64    size;
    NvU64    offset;
    NvU64   *page_array;
    NvU64    num_pages;
    NvU64    mmap_start;
    NvU64    mmap_size;
    NvU64    access_start;
    NvU64    access_size;
    NvU64    remap_prot_extra;
    NvBool   contig;
    NvU32    caching;
} nv_usermap_access_params_t;

/*
 * It stores mapping context per mapping
 */
typedef struct nv_alloc_mapping_context_s {
    void  *alloc;
    NvU64  page_index;
    NvU64 *page_array;
    NvU64  num_pages;
    NvU64  mmap_start;
    NvU64  mmap_size;
    NvU64  access_start;
    NvU64  access_size;
    NvU64  remap_prot_extra;
    NvU32  prot;
    NvBool valid;
    NvU32  caching;
} nv_alloc_mapping_context_t;

typedef enum
{
    NV_SOC_IRQ_DISPLAY_TYPE = 0x1,
    NV_SOC_IRQ_DPAUX_TYPE,
    NV_SOC_IRQ_GPIO_TYPE,
    NV_SOC_IRQ_HDACODEC_TYPE,
    NV_SOC_IRQ_TCPC2DISP_TYPE,
    NV_SOC_IRQ_INVALID_TYPE
} nv_soc_irq_type_t;

/*
 * It stores interrupt numbers and interrupt type and private data
 */
typedef struct nv_soc_irq_info_s {
    NvU32 irq_num;
    nv_soc_irq_type_t irq_type;
    NvBool bh_pending;
    union {
        NvU32 gpio_num;
        NvU32 dpaux_instance;
    } irq_data;
    NvS32 ref_count;
} nv_soc_irq_info_t;

#define NV_MAX_SOC_IRQS              6
#define NV_MAX_DPAUX_NUM_DEVICES     4

#define NV_MAX_SOC_DPAUX_NUM_DEVICES 2


#define NV_IGPU_LEGACY_STALL_IRQ     70
#define NV_IGPU_MAX_STALL_IRQS       3
#define NV_IGPU_MAX_NONSTALL_IRQS    1
/*
 * per device state
 */

/* DMA-capable device data, defined by kernel interface layer */
typedef struct nv_dma_device nv_dma_device_t;

typedef struct nv_phys_addr_range
{
    NvU64 addr;
    NvU64 len;
} nv_phys_addr_range_t;

typedef struct nv_state_t
{
    void  *priv;                    /* private data */
    void  *os_state;                /* os-specific device state */

    int    flags;

    /* PCI config info */
    nv_pci_info_t pci_info;
    NvU16 subsystem_id;
    NvU16 subsystem_vendor;
    NvU32 gpu_id;
    NvU32 iovaspace_id;
    struct
    {
        NvBool         valid;
        NvU8           uuid[GPU_UUID_LEN];
    } nv_uuid_cache;
    void *handle;

    NvU32 pci_cfg_space[NVRM_PCICFG_NUM_DWORDS];

    /* physical characteristics */
    nv_aperture_t bars[NV_GPU_NUM_BARS];
    nv_aperture_t *regs;
    nv_aperture_t *dpaux[NV_MAX_DPAUX_NUM_DEVICES];
    nv_aperture_t *hdacodec_regs;
    nv_aperture_t *mipical_regs;
    nv_aperture_t *fb, ud;
    nv_aperture_t *simregs;
    nv_aperture_t *emc_regs;

    NvU32  num_dpaux_instance;
    NvU32  interrupt_line;
    NvU32  dpaux_irqs[NV_MAX_DPAUX_NUM_DEVICES];
    nv_soc_irq_info_t soc_irq_info[NV_MAX_SOC_IRQS];
    NvS32 current_soc_irq;
    NvU32 num_soc_irqs;
    NvU32 hdacodec_irq;
    NvU32 tcpc2disp_irq;
    NvU8 *soc_dcb_blob;
    NvU32 soc_dcb_size;
    NvU32 disp_sw_soc_chip_id;
    NvBool soc_is_dpalt_mode_supported;

    NvU32 igpu_stall_irq[NV_IGPU_MAX_STALL_IRQS];
    NvU32 igpu_nonstall_irq;
    NvU32 num_stall_irqs;
    NvU64 dma_mask;
    
    NvBool primary_vga;

    NvU32 sim_env;

    NvU32 rc_timer_enabled;

    /* list of events allocated for this device */
    nv_event_t *event_list;

    /* lock to protect event_list */
    void *event_spinlock;

    nv_kern_mapping_t *kern_mappings;

    /* Kernel interface DMA device data */
    nv_dma_device_t *dma_dev;
    nv_dma_device_t *niso_dma_dev;

    /*
     * Per-GPU queue.  The actual queue object is usually allocated in the
     * arch-specific parent structure (e.g. nv_linux_state_t), and this
     * pointer just points to it.
     */
    struct os_work_queue *queue;

    /* For loading RM as a firmware (DCE or GSP) client */
    NvBool request_firmware;                /* request firmware from the OS */
    NvBool request_fw_client_rm;            /* attempt to init RM as FW a client */
    NvBool allow_fallback_to_monolithic_rm; /* allow fallback to monolithic RM if FW client RM doesn't work out */
    NvBool enable_firmware_logs;            /* attempt to enable firmware log decoding/printing */

    /* Variable to track, if nvidia_remove is called */
    NvBool removed;

    NvBool console_device;

    /* Variable to track, if GPU is external GPU */
    NvBool is_external_gpu;

    /* Variable to track, if regkey PreserveVideoMemoryAllocations is set */
    NvBool preserve_vidmem_allocations;

    /* Variable to force allocation of 32-bit addressable memory */
    NvBool force_dma32_alloc;

    /* PCI power state should be D0 during system suspend */
    NvBool d0_state_in_suspend;

    /* Current cyclestats client and context */
    NvU32 profiler_owner;
    void *profiler_context;

    /*
     * RMAPI objects to use in the OS layer to talk to core RM.
     *
     * Note that we only need to store one subdevice handle: in SLI, we will
     * have a separate nv_state_t per physical GPU.
     */
    struct {
        NvHandle hClient;
        NvHandle hDevice;
        NvHandle hSubDevice;
        NvHandle hI2C;
        NvHandle hDisp;
    } rmapi;

    /* Bool to check if dma-buf is supported */
    NvBool dma_buf_supported;

    /* Check if NVPCF DSM function is implemented under NVPCF or GPU device scope */
    NvBool nvpcf_dsm_in_gpu_scope;

    /* Bool to check if the device received a shutdown notification */
    NvBool is_shutdown;

    /* Bool to check if the GPU has a coherent sysmem link */
    NvBool coherent;

    /*
     * NUMA node ID of the CPU to which the GPU is attached.
     * Holds NUMA_NO_NODE on platforms that don't support NUMA configuration.
     */
    NvS32 cpu_numa_node_id;

    struct {
        /* Bool to check if ISO iommu enabled */
        NvBool iso_iommu_present;
        /* Bool to check if NISO iommu enabled */
        NvBool niso_iommu_present;
        /* Display SMMU Stream IDs */
        NvU32 dispIsoStreamId;
        NvU32 dispNisoStreamId;
    } iommus;
} nv_state_t;

#define NVFP_TYPE_NONE       0x0
#define NVFP_TYPE_REFCOUNTED 0x1
#define NVFP_TYPE_REGISTERED 0x2

struct nv_file_private_t
{
    NvHandle *handles;
    NvU16 maxHandles;
    NvU32 deviceInstance;
    NvU32 gpuInstanceId;
    NvU8 metadata[64];

    nv_file_private_t *ctl_nvfp;
    void *ctl_nvfp_priv;
    NvU32 register_or_refcount;

    //
    // True if a client or an event was ever allocated on this fd.
    // If false, RMAPI cleanup is skipped.
    //
    NvBool bCleanupRmapi;
};

// Forward define the gpu ops structures
typedef struct gpuSession                           *nvgpuSessionHandle_t;
typedef struct gpuDevice                            *nvgpuDeviceHandle_t;
typedef struct gpuAddressSpace                      *nvgpuAddressSpaceHandle_t;
typedef struct gpuTsg                               *nvgpuTsgHandle_t;
typedef struct UvmGpuTsgAllocParams_tag              nvgpuTsgAllocParams_t;
typedef struct gpuChannel                           *nvgpuChannelHandle_t;
typedef struct UvmGpuChannelInfo_tag                *nvgpuChannelInfo_t;
typedef struct UvmGpuChannelAllocParams_tag          nvgpuChannelAllocParams_t;
typedef struct UvmGpuCaps_tag                       *nvgpuCaps_t;
typedef struct UvmGpuCopyEnginesCaps_tag            *nvgpuCesCaps_t;
typedef struct UvmGpuAddressSpaceInfo_tag           *nvgpuAddressSpaceInfo_t;
typedef struct UvmGpuAllocInfo_tag                  *nvgpuAllocInfo_t;
typedef struct UvmGpuP2PCapsParams_tag              *nvgpuP2PCapsParams_t;
typedef struct UvmGpuFbInfo_tag                     *nvgpuFbInfo_t;
typedef struct UvmGpuEccInfo_tag                    *nvgpuEccInfo_t;
typedef struct UvmGpuFaultInfo_tag                  *nvgpuFaultInfo_t;
typedef struct UvmGpuAccessCntrInfo_tag             *nvgpuAccessCntrInfo_t;
typedef struct UvmGpuAccessCntrConfig_tag           *nvgpuAccessCntrConfig_t;
typedef struct UvmGpuInfo_tag                       nvgpuInfo_t;
typedef struct UvmGpuClientInfo_tag                 nvgpuClientInfo_t;
typedef struct UvmPmaAllocationOptions_tag          *nvgpuPmaAllocationOptions_t;
typedef struct UvmPmaStatistics_tag                 *nvgpuPmaStatistics_t;
typedef struct UvmGpuMemoryInfo_tag                 *nvgpuMemoryInfo_t;
typedef struct UvmGpuExternalMappingInfo_tag        *nvgpuExternalMappingInfo_t;
typedef struct UvmGpuChannelResourceInfo_tag        *nvgpuChannelResourceInfo_t;
typedef struct UvmGpuChannelInstanceInfo_tag        *nvgpuChannelInstanceInfo_t;
typedef struct UvmGpuChannelResourceBindParams_tag  *nvgpuChannelResourceBindParams_t;
typedef struct UvmGpuPagingChannelAllocParams_tag    nvgpuPagingChannelAllocParams_t;
typedef struct UvmGpuPagingChannel_tag              *nvgpuPagingChannelHandle_t;
typedef struct UvmGpuPagingChannelInfo_tag          *nvgpuPagingChannelInfo_t;
typedef enum   UvmPmaGpuMemoryType_tag               nvgpuGpuMemoryType_t;
typedef NV_STATUS (*nvPmaEvictPagesCallback)(void *, NvU64, NvU64 *, NvU32, NvU64, NvU64, nvgpuGpuMemoryType_t);
typedef NV_STATUS (*nvPmaEvictRangeCallback)(void *, NvU64, NvU64, nvgpuGpuMemoryType_t);

/*
 * flags
 */

#define NV_FLAG_OPEN                   0x0001
#define NV_FLAG_EXCLUDE                0x0002
#define NV_FLAG_CONTROL                0x0004
// Unused                              0x0008
#define NV_FLAG_SOC_DISPLAY            0x0010
#define NV_FLAG_USES_MSI               0x0020
#define NV_FLAG_USES_MSIX              0x0040
#define NV_FLAG_PASSTHRU               0x0080
#define NV_FLAG_SUSPENDED              0x0100
#define NV_FLAG_SOC_IGPU               0x0200
// Unused                              0x0400
#define NV_FLAG_PERSISTENT_SW_STATE    0x0800
#define NV_FLAG_IN_RECOVERY            0x1000
// Unused                              0x2000
#define NV_FLAG_UNBIND_LOCK            0x4000
/* To be set when GPU is not present on the bus, to help device teardown */
#define NV_FLAG_IN_SURPRISE_REMOVAL    0x8000

typedef enum
{
    NV_PM_ACTION_HIBERNATE,
    NV_PM_ACTION_STANDBY,
    NV_PM_ACTION_RESUME
} nv_pm_action_t;

typedef enum
{
    NV_PM_ACTION_DEPTH_DEFAULT,
    NV_PM_ACTION_DEPTH_MODESET,
    NV_PM_ACTION_DEPTH_UVM
} nv_pm_action_depth_t;

typedef enum
{
    NV_DYNAMIC_PM_NEVER,
    NV_DYNAMIC_PM_COARSE,
    NV_DYNAMIC_PM_FINE
} nv_dynamic_power_mode_t;

typedef enum
{
    NV_POWER_STATE_IN_HIBERNATE,
    NV_POWER_STATE_IN_STANDBY,
    NV_POWER_STATE_RUNNING
} nv_power_state_t;

typedef struct
{
    const char *vidmem_power_status;
    const char *dynamic_power_status;
    const char *gc6_support;
    const char *gcoff_support;
    const char *s0ix_status;
} nv_power_info_t;

#define NV_PRIMARY_VGA(nv)      ((nv)->primary_vga)

#define NV_IS_CTL_DEVICE(nv)    ((nv)->flags & NV_FLAG_CONTROL)
#define NV_IS_SOC_DISPLAY_DEVICE(nv)    \
        ((nv)->flags & NV_FLAG_SOC_DISPLAY)

#define NV_IS_SOC_IGPU_DEVICE(nv)    \
        ((nv)->flags & NV_FLAG_SOC_IGPU)

#define NV_IS_DEVICE_IN_SURPRISE_REMOVAL(nv)    \
        (((nv)->flags & NV_FLAG_IN_SURPRISE_REMOVAL) != 0)

/*
 * For console setup by EFI GOP, the base address is BAR1.
 * For console setup by VBIOS, the base address is BAR2 + 16MB.
 */
#define NV_IS_CONSOLE_MAPPED(nv, addr)  \
        (((addr) == (nv)->bars[NV_GPU_BAR_INDEX_FB].cpu_address) || \
         ((addr) == ((nv)->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address + 0x1000000)))

#define NV_SOC_IS_ISO_IOMMU_PRESENT(nv)     \
        ((nv)->iommus.iso_iommu_present)

#define NV_SOC_IS_NISO_IOMMU_PRESENT(nv)     \
        ((nv)->iommus.niso_iommu_present)
/*
 * GPU add/remove events
 */
#define NV_SYSTEM_GPU_ADD_EVENT             0x9001
#define NV_SYSTEM_GPU_REMOVE_EVENT          0x9002

/*
 * NVIDIA ACPI sub-event IDs (event types) to be passed into
 * to core NVIDIA driver for ACPI events.
 */
#define NV_SYSTEM_ACPI_EVENT_VALUE_DISPLAY_SWITCH_DEFAULT    0
#define NV_SYSTEM_ACPI_EVENT_VALUE_DOCK_EVENT_UNDOCKED       0
#define NV_SYSTEM_ACPI_EVENT_VALUE_DOCK_EVENT_DOCKED         1

#define NV_ACPI_NVIF_HANDLE_PRESENT 0x01
#define NV_ACPI_DSM_HANDLE_PRESENT  0x02
#define NV_ACPI_WMMX_HANDLE_PRESENT 0x04

#define NV_EVAL_ACPI_METHOD_NVIF     0x01
#define NV_EVAL_ACPI_METHOD_WMMX     0x02

typedef enum {
    NV_I2C_CMD_READ = 1,
    NV_I2C_CMD_WRITE,
    NV_I2C_CMD_SMBUS_READ,
    NV_I2C_CMD_SMBUS_WRITE,
    NV_I2C_CMD_SMBUS_QUICK_WRITE,
    NV_I2C_CMD_SMBUS_QUICK_READ,
    NV_I2C_CMD_SMBUS_BLOCK_READ,
    NV_I2C_CMD_SMBUS_BLOCK_WRITE,
    NV_I2C_CMD_BLOCK_READ,
    NV_I2C_CMD_BLOCK_WRITE
} nv_i2c_cmd_t;

// Flags needed by OSAllocPagesNode
#define NV_ALLOC_PAGES_NODE_NONE                0x0
#define NV_ALLOC_PAGES_NODE_SKIP_RECLAIM        0x1

/*
** where we hide our nv_state_t * ...
*/
#define NV_SET_NV_STATE(pgpu,p) ((pgpu)->pOsGpuInfo = (p))
#define NV_GET_NV_STATE(pGpu) \
    (nv_state_t *)((pGpu) ? (pGpu)->pOsGpuInfo : NULL)

static inline NvBool IS_REG_OFFSET(nv_state_t *nv, NvU64 offset, NvU64 length)
{
    return ((offset >= nv->regs->cpu_address) &&
            ((offset + (length - 1)) >= offset) &&
            ((offset + (length - 1)) <= (nv->regs->cpu_address + (nv->regs->size - 1))));
}

static inline NvBool IS_FB_OFFSET(nv_state_t *nv, NvU64 offset, NvU64 length)
{
    return  ((nv->fb) && (nv->fb->size != 0) &&
             (offset >= nv->fb->cpu_address) &&
             ((offset + (length - 1)) >= offset) &&
             ((offset + (length - 1)) <= (nv->fb->cpu_address + (nv->fb->size - 1))));
}

static inline NvBool IS_UD_OFFSET(nv_state_t *nv, NvU64 offset, NvU64 length)
{
    return ((nv->ud.cpu_address != 0) && (nv->ud.size != 0) &&
            (offset >= nv->ud.cpu_address) &&
            ((offset + (length - 1)) >= offset) &&
            ((offset + (length - 1)) <= (nv->ud.cpu_address + (nv->ud.size - 1))));
}

static inline NvBool IS_IMEM_OFFSET(nv_state_t *nv, NvU64 offset, NvU64 length)
{
    return ((nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address != 0) &&
            (nv->bars[NV_GPU_BAR_INDEX_IMEM].size != 0) &&
            (offset >= nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address) &&
            ((offset + (length - 1)) >= offset) &&
            ((offset + (length - 1)) <= (nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address +
                                         (nv->bars[NV_GPU_BAR_INDEX_IMEM].size - 1))));
}

#define NV_RM_MAX_MSIX_LINES  8

#define NV_MAX_ISR_DELAY_US           20000
#define NV_MAX_ISR_DELAY_MS           (NV_MAX_ISR_DELAY_US / 1000)

#define NV_TIMERCMP(a, b, CMP)                                              \
    (((a)->tv_sec == (b)->tv_sec) ?                                         \
        ((a)->tv_usec CMP (b)->tv_usec) : ((a)->tv_sec CMP (b)->tv_sec))

#define NV_TIMERADD(a, b, result)                                           \
    {                                                                       \
        (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                       \
        (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                    \
        if ((result)->tv_usec >= 1000000)                                   \
        {                                                                   \
            ++(result)->tv_sec;                                             \
            (result)->tv_usec -= 1000000;                                   \
        }                                                                   \
    }

#define NV_TIMERSUB(a, b, result)                                           \
    {                                                                       \
        (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                       \
        (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                    \
        if ((result)->tv_usec < 0)                                          \
        {                                                                   \
          --(result)->tv_sec;                                               \
          (result)->tv_usec += 1000000;                                     \
        }                                                                   \
    }

#define NV_TIMEVAL_TO_US(tv)    ((NvU64)(tv).tv_sec * 1000000 + (tv).tv_usec)

#ifndef NV_ALIGN_UP
#define NV_ALIGN_UP(v,g) (((v) + ((g) - 1)) & ~((g) - 1))
#endif
#ifndef NV_ALIGN_DOWN
#define NV_ALIGN_DOWN(v,g) ((v) & ~((g) - 1))
#endif

/*
 * driver internal interfaces
 */

/*
 * ---------------------------------------------------------------------------
 *
 * Function prototypes for UNIX specific OS interface.
 *
 * ---------------------------------------------------------------------------
 */

NvU32      NV_API_CALL  nv_get_dev_minor         (nv_state_t *);
void*      NV_API_CALL  nv_alloc_kernel_mapping  (nv_state_t *, void *, NvU64, NvU32, NvU64, void **);
NV_STATUS  NV_API_CALL  nv_free_kernel_mapping   (nv_state_t *, void *, void *, void *);
NV_STATUS  NV_API_CALL  nv_alloc_user_mapping    (nv_state_t *, void *, NvU64, NvU32, NvU64, NvU32, NvU64 *, void **);
NV_STATUS  NV_API_CALL  nv_free_user_mapping     (nv_state_t *, void *, NvU64, void *);
NV_STATUS  NV_API_CALL  nv_add_mapping_context_to_file (nv_state_t *, nv_usermap_access_params_t*, NvU32, void *, NvU64, NvU32);

NvU64  NV_API_CALL  nv_get_kern_phys_address     (NvU64);
NvU64  NV_API_CALL  nv_get_user_phys_address     (NvU64);
nv_state_t*  NV_API_CALL  nv_get_adapter_state   (NvU32, NvU8, NvU8);
nv_state_t*  NV_API_CALL  nv_get_ctl_state       (void);

void   NV_API_CALL  nv_set_dma_address_size      (nv_state_t *, NvU32 );

NV_STATUS  NV_API_CALL  nv_alias_pages           (nv_state_t *, NvU32, NvU64, NvU32, NvU32, NvU64, NvU64 *, void **);
NV_STATUS  NV_API_CALL  nv_alloc_pages           (nv_state_t *, NvU32, NvU64, NvBool, NvU32, NvBool, NvBool, NvS32, NvU64 *, void **);
NV_STATUS  NV_API_CALL  nv_free_pages            (nv_state_t *, NvU32, NvBool, NvU32, void *);

NV_STATUS  NV_API_CALL  nv_register_user_pages   (nv_state_t *, NvU64, NvU64 *, void *, void **);
void       NV_API_CALL  nv_unregister_user_pages (nv_state_t *, NvU64, void **, void **);

NV_STATUS NV_API_CALL   nv_register_peer_io_mem  (nv_state_t *, NvU64 *, NvU64, void **);
void      NV_API_CALL   nv_unregister_peer_io_mem(nv_state_t *, void *);

struct sg_table;

NV_STATUS NV_API_CALL   nv_register_sgt          (nv_state_t *, NvU64 *, NvU64, NvU32, void **, struct sg_table *, void *);
void      NV_API_CALL   nv_unregister_sgt        (nv_state_t *, struct sg_table **, void **, void *);
NV_STATUS NV_API_CALL   nv_register_phys_pages   (nv_state_t *, NvU64 *, NvU64, NvU32, void **);
void      NV_API_CALL   nv_unregister_phys_pages (nv_state_t *, void *);

NV_STATUS  NV_API_CALL  nv_dma_map_sgt           (nv_dma_device_t *, NvU64, NvU64 *, NvU32, void **);

NV_STATUS  NV_API_CALL  nv_dma_map_alloc         (nv_dma_device_t *, NvU64, NvU64 *, NvBool, void **);
NV_STATUS  NV_API_CALL  nv_dma_unmap_alloc       (nv_dma_device_t *, NvU64, NvU64 *, void **);

NV_STATUS  NV_API_CALL  nv_dma_map_peer          (nv_dma_device_t *, nv_dma_device_t *, NvU8, NvU64, NvU64 *);
void       NV_API_CALL  nv_dma_unmap_peer        (nv_dma_device_t *, NvU64, NvU64);

NV_STATUS  NV_API_CALL  nv_dma_map_mmio          (nv_dma_device_t *, NvU64, NvU64 *);
void       NV_API_CALL  nv_dma_unmap_mmio        (nv_dma_device_t *, NvU64, NvU64);

void       NV_API_CALL  nv_dma_cache_invalidate  (nv_dma_device_t *, void *);
void       NV_API_CALL  nv_dma_enable_nvlink     (nv_dma_device_t *);

NvS32  NV_API_CALL  nv_start_rc_timer            (nv_state_t *);
NvS32  NV_API_CALL  nv_stop_rc_timer             (nv_state_t *);

void   NV_API_CALL  nv_post_event                (nv_event_t *, NvHandle, NvU32, NvU32, NvU16, NvBool);
NvS32  NV_API_CALL  nv_get_event                 (nv_file_private_t *, nv_event_t *, NvU32 *);

void*  NV_API_CALL  nv_i2c_add_adapter           (nv_state_t *, NvU32);
void   NV_API_CALL  nv_i2c_del_adapter           (nv_state_t *, void *);

void   NV_API_CALL  nv_acpi_methods_init         (NvU32 *);
void   NV_API_CALL  nv_acpi_methods_uninit       (void);

NV_STATUS  NV_API_CALL  nv_acpi_method           (NvU32, NvU32, NvU32, void *, NvU16, NvU32 *, void *, NvU16 *);
NV_STATUS  NV_API_CALL  nv_acpi_d3cold_dsm_for_upstream_port (nv_state_t *, NvU8 *, NvU32, NvU32, NvU32 *);
NV_STATUS  NV_API_CALL  nv_acpi_dsm_method       (nv_state_t *, NvU8 *, NvU32, NvBool, NvU32, void *, NvU16, NvU32 *, void *, NvU16 *);
NV_STATUS  NV_API_CALL  nv_acpi_ddc_method       (nv_state_t *, void *, NvU32 *, NvBool);
NV_STATUS  NV_API_CALL  nv_acpi_dod_method       (nv_state_t *, NvU32 *, NvU32 *);
NV_STATUS  NV_API_CALL  nv_acpi_rom_method       (nv_state_t *, NvU32 *, NvU32 *);
NV_STATUS  NV_API_CALL  nv_acpi_get_powersource  (NvU32 *);
NvBool     NV_API_CALL  nv_acpi_is_battery_present(void);

NV_STATUS  NV_API_CALL  nv_acpi_mux_method       (nv_state_t *, NvU32 *, NvU32, const char *);

NV_STATUS  NV_API_CALL  nv_log_error             (nv_state_t *, NvU32, const char *, va_list);

NvU64      NV_API_CALL  nv_get_dma_start_address (nv_state_t *);
NV_STATUS  NV_API_CALL  nv_set_primary_vga_status(nv_state_t *);
NV_STATUS  NV_API_CALL  nv_pci_trigger_recovery  (nv_state_t *);
NvBool     NV_API_CALL  nv_requires_dma_remap    (nv_state_t *);

NvBool     NV_API_CALL  nv_is_rm_firmware_active(nv_state_t *);
const void*NV_API_CALL  nv_get_firmware(nv_state_t *, nv_firmware_type_t, nv_firmware_chip_family_t, const void **, NvU32 *);
void       NV_API_CALL  nv_put_firmware(const void *);

nv_file_private_t* NV_API_CALL nv_get_file_private(NvS32, NvBool, void **);
void               NV_API_CALL nv_put_file_private(void *);

NV_STATUS NV_API_CALL nv_get_device_memory_config(nv_state_t *, NvU64 *, NvU64 *, NvU64 *, NvU32 *, NvS32 *);
NV_STATUS NV_API_CALL nv_get_egm_info(nv_state_t *, NvU64 *, NvU64 *, NvS32 *);

NV_STATUS NV_API_CALL nv_get_ibmnpu_genreg_info(nv_state_t *, NvU64 *, NvU64 *, void**);
NV_STATUS NV_API_CALL nv_get_ibmnpu_relaxed_ordering_mode(nv_state_t *nv, NvBool *mode);

void      NV_API_CALL nv_wait_for_ibmnpu_rsync(nv_state_t *nv);

void      NV_API_CALL nv_ibmnpu_cache_flush_range(nv_state_t *nv, NvU64, NvU64);

void      NV_API_CALL nv_p2p_free_platform_data(void *data);

#if defined(NVCPU_PPC64LE)
NV_STATUS NV_API_CALL nv_get_nvlink_line_rate    (nv_state_t *, NvU32 *);
#endif

NV_STATUS NV_API_CALL nv_revoke_gpu_mappings     (nv_state_t *);
void      NV_API_CALL nv_acquire_mmap_lock       (nv_state_t *);
void      NV_API_CALL nv_release_mmap_lock       (nv_state_t *);
NvBool    NV_API_CALL nv_get_all_mappings_revoked_locked (nv_state_t *);
void      NV_API_CALL nv_set_safe_to_mmap_locked (nv_state_t *, NvBool);

NV_STATUS NV_API_CALL nv_indicate_idle           (nv_state_t *);
NV_STATUS NV_API_CALL nv_indicate_not_idle       (nv_state_t *);
void      NV_API_CALL nv_idle_holdoff            (nv_state_t *);

NvBool    NV_API_CALL nv_dynamic_power_available (nv_state_t *);
void      NV_API_CALL nv_audio_dynamic_power     (nv_state_t *);

void      NV_API_CALL nv_control_soc_irqs        (nv_state_t *, NvBool bEnable);
NV_STATUS NV_API_CALL nv_get_current_irq_priv_data(nv_state_t *, NvU32 *);

NV_STATUS NV_API_CALL nv_acquire_fabric_mgmt_cap (int, int*);
int       NV_API_CALL nv_cap_drv_init(void);
void      NV_API_CALL nv_cap_drv_exit(void);
NvBool    NV_API_CALL nv_is_gpu_accessible(nv_state_t *);
NvBool    NV_API_CALL nv_match_gpu_os_info(nv_state_t *, void *);

void      NV_API_CALL nv_get_updated_emu_seg(NvU32 *start, NvU32 *end);
void      NV_API_CALL nv_get_screen_info(nv_state_t *, NvU64 *, NvU32 *, NvU32 *, NvU32 *, NvU32 *, NvU64 *);

struct dma_buf;
typedef struct nv_dma_buf nv_dma_buf_t;
struct drm_gem_object;

NV_STATUS NV_API_CALL nv_dma_import_sgt  (nv_dma_device_t *, struct sg_table *, struct drm_gem_object *);
void NV_API_CALL nv_dma_release_sgt(struct sg_table *, struct drm_gem_object *);
NV_STATUS NV_API_CALL nv_dma_import_dma_buf      (nv_dma_device_t *, struct dma_buf *, NvU32 *, struct sg_table **, nv_dma_buf_t **);
NV_STATUS NV_API_CALL nv_dma_import_from_fd      (nv_dma_device_t *, NvS32, NvU32 *, struct sg_table **, nv_dma_buf_t **);
void      NV_API_CALL nv_dma_release_dma_buf     (nv_dma_buf_t *);

void      NV_API_CALL nv_schedule_uvm_isr        (nv_state_t *);

NvBool    NV_API_CALL nv_platform_supports_s0ix  (void);
NvBool    NV_API_CALL nv_s2idle_pm_configured    (void);

NvBool    NV_API_CALL nv_is_chassis_notebook      (void);
void      NV_API_CALL nv_allow_runtime_suspend    (nv_state_t *nv);
void      NV_API_CALL nv_disallow_runtime_suspend (nv_state_t *nv);

typedef void (*nvTegraDceClientIpcCallback)(NvU32, NvU32, NvU32, void *, void *);

NV_STATUS NV_API_CALL nv_get_num_phys_pages      (void *, NvU32 *);
NV_STATUS NV_API_CALL nv_get_phys_pages          (void *, void *, NvU32 *);

void      NV_API_CALL nv_get_disp_smmu_stream_ids (nv_state_t *, NvU32 *, NvU32 *);

/*
 * ---------------------------------------------------------------------------
 *
 * Function prototypes for Resource Manager interface.
 *
 * ---------------------------------------------------------------------------
 */

NvBool     NV_API_CALL  rm_init_rm               (nvidia_stack_t *);
void       NV_API_CALL  rm_shutdown_rm           (nvidia_stack_t *);
NvBool     NV_API_CALL  rm_init_private_state    (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_free_private_state    (nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL  rm_init_adapter          (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_disable_adapter       (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_shutdown_adapter      (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_exclude_adapter       (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_acquire_api_lock      (nvidia_stack_t *);
NV_STATUS  NV_API_CALL  rm_release_api_lock      (nvidia_stack_t *);
NV_STATUS  NV_API_CALL  rm_acquire_gpu_lock      (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_release_gpu_lock      (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_acquire_all_gpus_lock (nvidia_stack_t *);
NV_STATUS  NV_API_CALL  rm_release_all_gpus_lock (nvidia_stack_t *);
NV_STATUS  NV_API_CALL  rm_ioctl                 (nvidia_stack_t *, nv_state_t *, nv_file_private_t *, NvU32, void *, NvU32);
NvBool     NV_API_CALL  rm_isr                   (nvidia_stack_t *, nv_state_t *, NvU32 *);
void       NV_API_CALL  rm_isr_bh                (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_isr_bh_unlocked       (nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL  rm_is_msix_allowed       (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_power_management      (nvidia_stack_t *, nv_state_t *, nv_pm_action_t);
NV_STATUS  NV_API_CALL  rm_stop_user_channels    (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_restart_user_channels (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_save_low_res_mode     (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_get_vbios_version     (nvidia_stack_t *, nv_state_t *, char *);
char*      NV_API_CALL  rm_get_gpu_uuid          (nvidia_stack_t *, nv_state_t *);
const NvU8* NV_API_CALL rm_get_gpu_uuid_raw      (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_set_rm_firmware_requested(nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_get_firmware_version  (nvidia_stack_t *, nv_state_t *, char *, NvLength);
void       NV_API_CALL  rm_cleanup_file_private  (nvidia_stack_t *, nv_state_t *, nv_file_private_t *);
void       NV_API_CALL  rm_unbind_lock           (nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_read_registry_dword   (nvidia_stack_t *, nv_state_t *, const char *, NvU32 *);
NV_STATUS  NV_API_CALL  rm_write_registry_dword  (nvidia_stack_t *, nv_state_t *, const char *, NvU32);
NV_STATUS  NV_API_CALL  rm_write_registry_binary (nvidia_stack_t *, nv_state_t *, const char *, NvU8 *, NvU32);
NV_STATUS  NV_API_CALL  rm_write_registry_string (nvidia_stack_t *, nv_state_t *, const char *, const char *, NvU32);
void       NV_API_CALL  rm_parse_option_string   (nvidia_stack_t *, const char *);
char*      NV_API_CALL  rm_remove_spaces         (const char *);
char*      NV_API_CALL  rm_string_token          (char **, const char);
void       NV_API_CALL  rm_vgpu_vfio_set_driver_vm(nvidia_stack_t *, NvBool);
NV_STATUS  NV_API_CALL  rm_get_adapter_status_external(nvidia_stack_t *, nv_state_t *);

NV_STATUS  NV_API_CALL  rm_run_rc_callback       (nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_execute_work_item     (nvidia_stack_t *, void *);
const char* NV_API_CALL rm_get_device_name       (NvU16, NvU16, NvU16);

NV_STATUS  NV_API_CALL  rm_is_supported_device   (nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL  rm_is_supported_pci_device(NvU8   pci_class,
                                                   NvU8   pci_subclass,
                                                   NvU16  vendor,
                                                   NvU16  device,
                                                   NvU16  subsystem_vendor,
                                                   NvU16  subsystem_device,
                                                   NvBool print_legacy_warning);

void       NV_API_CALL  rm_i2c_remove_adapters    (nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL  rm_i2c_is_smbus_capable   (nvidia_stack_t *, nv_state_t *, void *);
NV_STATUS  NV_API_CALL  rm_i2c_transfer           (nvidia_stack_t *, nv_state_t *, void *, nv_i2c_cmd_t, NvU8, NvU8, NvU32, NvU8 *);

NV_STATUS  NV_API_CALL  rm_perform_version_check  (nvidia_stack_t *, void *, NvU32);

void       NV_API_CALL  rm_power_source_change_event        (nvidia_stack_t *, NvU32);

void       NV_API_CALL  rm_request_dnotifier_state          (nvidia_stack_t *, nv_state_t *);

void       NV_API_CALL  rm_disable_gpu_state_persistence    (nvidia_stack_t *sp, nv_state_t *);
NV_STATUS  NV_API_CALL  rm_p2p_init_mapping       (nvidia_stack_t *, NvU64, NvU64 *, NvU64 *, NvU64 *, NvU64 *, NvU64, NvU64, NvU64, NvU64, void (*)(void *), void *);
NV_STATUS  NV_API_CALL  rm_p2p_destroy_mapping    (nvidia_stack_t *, NvU64);
NV_STATUS  NV_API_CALL  rm_p2p_get_pages          (nvidia_stack_t *, NvU64, NvU32, NvU64, NvU64, NvU64 *, NvU32 *, NvU32 *, NvU32 *, NvU8 **, void *);
NV_STATUS  NV_API_CALL  rm_p2p_get_gpu_info       (nvidia_stack_t *, NvU64, NvU64, NvU8 **, void **);
NV_STATUS  NV_API_CALL  rm_p2p_get_pages_persistent (nvidia_stack_t *,  NvU64, NvU64, void **, NvU64 *, NvU32 *, void *, void *, void **);
NV_STATUS  NV_API_CALL  rm_p2p_register_callback  (nvidia_stack_t *, NvU64, NvU64, NvU64, void *, void (*)(void *), void *);
NV_STATUS  NV_API_CALL  rm_p2p_put_pages          (nvidia_stack_t *, NvU64, NvU32, NvU64, void *);
NV_STATUS  NV_API_CALL  rm_p2p_put_pages_persistent(nvidia_stack_t *, void *, void *, void *);
NV_STATUS  NV_API_CALL  rm_p2p_dma_map_pages      (nvidia_stack_t *, nv_dma_device_t *, NvU8 *, NvU64, NvU32, NvU64 *, void **);
NV_STATUS  NV_API_CALL  rm_dma_buf_dup_mem_handle (nvidia_stack_t *, nv_state_t *, NvHandle, NvHandle, NvHandle, NvHandle, void *, NvHandle, NvU64, NvU64, NvHandle *, void **);
void       NV_API_CALL  rm_dma_buf_undup_mem_handle(nvidia_stack_t *, nv_state_t *, NvHandle, NvHandle);
NV_STATUS  NV_API_CALL  rm_dma_buf_map_mem_handle (nvidia_stack_t *, nv_state_t *, NvHandle, NvHandle, NvU64, NvU64, void *, nv_phys_addr_range_t **, NvU32 *);
void       NV_API_CALL  rm_dma_buf_unmap_mem_handle(nvidia_stack_t *, nv_state_t *, NvHandle, NvHandle, NvU64, nv_phys_addr_range_t **, NvU32);
NV_STATUS  NV_API_CALL  rm_dma_buf_get_client_and_device(nvidia_stack_t *, nv_state_t *, NvHandle, NvHandle, NvHandle *, NvHandle *, NvHandle *, void **, NvBool *);
void       NV_API_CALL  rm_dma_buf_put_client_and_device(nvidia_stack_t *, nv_state_t *, NvHandle, NvHandle, NvHandle, void *);
NV_STATUS  NV_API_CALL  rm_log_gpu_crash          (nv_stack_t *, nv_state_t *);

void       NV_API_CALL rm_kernel_rmapi_op(nvidia_stack_t *sp, void *ops_cmd);
NvBool     NV_API_CALL rm_get_device_remove_flag(nvidia_stack_t *sp, NvU32 gpu_id);
NV_STATUS  NV_API_CALL rm_gpu_copy_mmu_faults(nvidia_stack_t *, nv_state_t *, NvU32 *);
NV_STATUS  NV_API_CALL rm_gpu_handle_mmu_faults(nvidia_stack_t *, nv_state_t *, NvU32 *);
NvBool     NV_API_CALL rm_gpu_need_4k_page_isolation(nv_state_t *);
NvBool     NV_API_CALL rm_is_chipset_io_coherent(nv_stack_t *);
NvBool     NV_API_CALL rm_init_event_locks(nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL rm_destroy_event_locks(nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL rm_get_gpu_numa_info(nvidia_stack_t *, nv_state_t *, nv_ioctl_numa_info_t *);
NV_STATUS  NV_API_CALL rm_gpu_numa_online(nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL rm_gpu_numa_offline(nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL rm_is_device_sequestered(nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL rm_check_for_gpu_surprise_removal(nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL rm_set_external_kernel_client_count(nvidia_stack_t *, nv_state_t *, NvBool);
NV_STATUS  NV_API_CALL rm_schedule_gpu_wakeup(nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL rm_is_iommu_needed_for_sriov(nvidia_stack_t *, nv_state_t *);
NvBool     NV_API_CALL rm_disable_iomap_wc(void);

void       NV_API_CALL rm_init_dynamic_power_management(nvidia_stack_t *, nv_state_t *, NvBool);
void       NV_API_CALL rm_cleanup_dynamic_power_management(nvidia_stack_t *, nv_state_t *);
void       NV_API_CALL rm_enable_dynamic_power_management(nvidia_stack_t *, nv_state_t *);
NV_STATUS  NV_API_CALL rm_ref_dynamic_power(nvidia_stack_t *, nv_state_t *, nv_dynamic_power_mode_t);
void       NV_API_CALL rm_unref_dynamic_power(nvidia_stack_t *, nv_state_t *, nv_dynamic_power_mode_t);
NV_STATUS  NV_API_CALL rm_transition_dynamic_power(nvidia_stack_t *, nv_state_t *, NvBool, NvBool *);
void       NV_API_CALL rm_get_power_info(nvidia_stack_t *, nv_state_t *, nv_power_info_t *);

void       NV_API_CALL rm_acpi_notify(nvidia_stack_t *, nv_state_t *, NvU32);
void       NV_API_CALL rm_acpi_nvpcf_notify(nvidia_stack_t *);

NvBool     NV_API_CALL rm_is_altstack_in_use(void);

/* vGPU VFIO specific functions */
NV_STATUS  NV_API_CALL  nv_vgpu_create_request(nvidia_stack_t *, nv_state_t *, const NvU8 *, NvU32, NvU16 *, NvU32);
NV_STATUS  NV_API_CALL  nv_vgpu_delete(nvidia_stack_t *, const NvU8 *, NvU16);
NV_STATUS  NV_API_CALL  nv_vgpu_get_type_ids(nvidia_stack_t *, nv_state_t *, NvU32 *, NvU32 *, NvBool, NvU8, NvBool);
NV_STATUS  NV_API_CALL  nv_vgpu_get_type_info(nvidia_stack_t *, nv_state_t *, NvU32, char *, int, NvU8);
NV_STATUS  NV_API_CALL  nv_vgpu_get_bar_info(nvidia_stack_t *, nv_state_t *, const NvU8 *, NvU64 *,
                                             NvU64 *, NvU64 *, NvU32 *, NvBool *, NvU8 *);
NV_STATUS  NV_API_CALL  nv_vgpu_get_hbm_info(nvidia_stack_t *, nv_state_t *, const NvU8 *, NvU64 *, NvU64 *);
NV_STATUS  NV_API_CALL  nv_vgpu_process_vf_info(nvidia_stack_t *, nv_state_t *, NvU8, NvU32, NvU8, NvU8, NvU8, NvBool, void *);
NV_STATUS  NV_API_CALL  nv_gpu_bind_event(nvidia_stack_t *, NvU32, NvBool *);
NV_STATUS  NV_API_CALL  nv_gpu_unbind_event(nvidia_stack_t *, NvU32, NvBool *);

NV_STATUS NV_API_CALL nv_get_usermap_access_params(nv_state_t*, nv_usermap_access_params_t*);
nv_soc_irq_type_t NV_API_CALL nv_get_current_irq_type(nv_state_t*);
void       NV_API_CALL  nv_flush_coherent_cpu_cache_range(nv_state_t *nv, NvU64 cpu_virtual, NvU64 size);

#if defined(NV_VMWARE)
const void* NV_API_CALL rm_get_firmware(nv_firmware_type_t fw_type, const void **fw_buf, NvU32 *fw_size);
#endif

/* Callbacks should occur roughly every 10ms. */
#define NV_SNAPSHOT_TIMER_HZ 100
void NV_API_CALL nv_start_snapshot_timer(void (*snapshot_callback)(void *context));
void NV_API_CALL nv_flush_snapshot_timer(void);
void NV_API_CALL nv_stop_snapshot_timer(void);

static inline const NvU8 *nv_get_cached_uuid(nv_state_t *nv)
{
    return nv->nv_uuid_cache.valid ? nv->nv_uuid_cache.uuid : NULL;
}

/* nano second resolution timer callback structure */
typedef struct nv_nano_timer nv_nano_timer_t;

/* nano timer functions */
void        NV_API_CALL nv_create_nano_timer(nv_state_t *, void *pTmrEvent, nv_nano_timer_t **);
void        NV_API_CALL nv_start_nano_timer(nv_state_t *nv, nv_nano_timer_t *, NvU64 timens);
NV_STATUS   NV_API_CALL rm_run_nano_timer_callback(nvidia_stack_t *, nv_state_t *, void *pTmrEvent);
void        NV_API_CALL nv_cancel_nano_timer(nv_state_t *, nv_nano_timer_t *);
void        NV_API_CALL nv_destroy_nano_timer(nv_state_t *nv, nv_nano_timer_t *);

// Host1x specific functions.
NV_STATUS NV_API_CALL nv_get_syncpoint_aperture(NvU32, NvU64 *, NvU64 *, NvU32 *);

#if defined(NVCPU_X86_64)

static inline NvU64 nv_rdtsc(void)
{
    NvU64 val;
    __asm__ __volatile__ ("rdtsc               \t\n"
                          "shlq   $0x20,%%rdx  \t\n"
                          "orq    %%rdx,%%rax  \t\n"
                          : "=A" (val));
    return val;
}

#endif

#endif /* NVRM */

static inline int nv_count_bits(NvU64 word)
{
    NvU64 bits;

    bits = (word & 0x5555555555555555ULL) + ((word >>  1) & 0x5555555555555555ULL);
    bits = (bits & 0x3333333333333333ULL) + ((bits >>  2) & 0x3333333333333333ULL);
    bits = (bits & 0x0f0f0f0f0f0f0f0fULL) + ((bits >>  4) & 0x0f0f0f0f0f0f0f0fULL);
    bits = (bits & 0x00ff00ff00ff00ffULL) + ((bits >>  8) & 0x00ff00ff00ff00ffULL);
    bits = (bits & 0x0000ffff0000ffffULL) + ((bits >> 16) & 0x0000ffff0000ffffULL);
    bits = (bits & 0x00000000ffffffffULL) + ((bits >> 32) & 0x00000000ffffffffULL);

    return (int)(bits);
}

#endif

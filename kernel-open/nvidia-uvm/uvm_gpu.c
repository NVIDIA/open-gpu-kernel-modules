/*******************************************************************************
    Copyright (c) 2015-2025 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "nv_uvm_interface.h"
#include "uvm_api.h"
#include "uvm_channel.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_gpu_semaphore.h"
#include "uvm_hal.h"
#include "uvm_procfs.h"
#include "uvm_pmm_gpu.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_va_space.h"
#include "uvm_user_channel.h"
#include "uvm_perf_events.h"
#include "uvm_perf_heuristics.h"
#include "uvm_common.h"
#include "ctrl2080mc.h"
#include "nv-kthread-q.h"
#include "uvm_gpu_access_counters.h"
#include "uvm_ats.h"
#include "uvm_test.h"
#include "uvm_conf_computing.h"

#include "uvm_linux.h"

#define UVM_PROC_GPUS_PEER_DIR_NAME "peers"

// The uvm_peer_copy module parameter enables to choose from "phys" or "virt".
// It determines the addressing mode for P2P copies.
#define UVM_PARAM_PEER_COPY_VIRTUAL "virt"
#define UVM_PARAM_PEER_COPY_PHYSICAL "phys"
static char *uvm_peer_copy = UVM_PARAM_PEER_COPY_PHYSICAL;
module_param(uvm_peer_copy, charp, S_IRUGO);
MODULE_PARM_DESC(uvm_peer_copy, "Choose the addressing mode for peer copying, options: "
                                UVM_PARAM_PEER_COPY_PHYSICAL " [default] or " UVM_PARAM_PEER_COPY_VIRTUAL ". "
                                "Valid for Ampere+ GPUs.");

static uvm_user_channel_t *get_user_channel(uvm_rb_tree_node_t *node)
{
    return container_of(node, uvm_user_channel_t, instance_ptr.node);
}

static uvm_gpu_link_type_t get_gpu_link_type(UVM_LINK_TYPE link_type)
{
    switch (link_type) {
        case UVM_LINK_TYPE_PCIE:
            return UVM_GPU_LINK_PCIE;
        case UVM_LINK_TYPE_NVLINK_1:
            return UVM_GPU_LINK_NVLINK_1;
        case UVM_LINK_TYPE_NVLINK_2:
            return UVM_GPU_LINK_NVLINK_2;
        case UVM_LINK_TYPE_NVLINK_3:
            return UVM_GPU_LINK_NVLINK_3;
        case UVM_LINK_TYPE_NVLINK_4:
            return UVM_GPU_LINK_NVLINK_4;
        case UVM_LINK_TYPE_NVLINK_5:
            return UVM_GPU_LINK_NVLINK_5;
        case UVM_LINK_TYPE_C2C:
            return UVM_GPU_LINK_C2C;
        default:
            return UVM_GPU_LINK_INVALID;
    }
}

static void fill_parent_gpu_info(uvm_parent_gpu_t *parent_gpu, const UvmGpuInfo *gpu_info)
{
    char uuid_buffer[UVM_UUID_STRING_LENGTH];

    parent_gpu->rm_info = *gpu_info;

    parent_gpu->system_bus.link = get_gpu_link_type(gpu_info->sysmemLink);
    UVM_ASSERT(parent_gpu->system_bus.link != UVM_GPU_LINK_INVALID);

    parent_gpu->system_bus.link_rate_mbyte_per_s = gpu_info->sysmemLinkRateMBps;

    if (gpu_info->systemMemoryWindowSize > 0) {
        // memory_window_end is inclusive but uvm_parent_gpu_is_coherent()
        // checks memory_window_end > memory_window_start as its condition.
        UVM_ASSERT(gpu_info->systemMemoryWindowSize > 1);
        parent_gpu->system_bus.memory_window_start = gpu_info->systemMemoryWindowStart;
        parent_gpu->system_bus.memory_window_end   = gpu_info->systemMemoryWindowStart +
                                                     gpu_info->systemMemoryWindowSize - 1;
    }

    parent_gpu->nvswitch_info.is_nvswitch_connected = gpu_info->connectedToSwitch;

    // nvswitch is routed via physical pages, where the upper 13-bits of the
    // 47-bit address space holds the routing information for each peer.
    // Currently, this is limited to a 16GB framebuffer window size.
    if (parent_gpu->nvswitch_info.is_nvswitch_connected) {
        parent_gpu->nvswitch_info.fabric_memory_window_start = gpu_info->nvswitchMemoryWindowStart;
        parent_gpu->nvswitch_info.egm_fabric_memory_window_start = gpu_info->nvswitchEgmMemoryWindowStart;
    }

    uvm_uuid_string(uuid_buffer, &parent_gpu->uuid);
    snprintf(parent_gpu->name,
             sizeof(parent_gpu->name),
             "ID %u: %s: " UVM_PARENT_GPU_UUID_PREFIX "%s",
             uvm_parent_id_value(parent_gpu->id),
             parent_gpu->rm_info.name,
             uuid_buffer);
}

static NV_STATUS get_gpu_caps(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    UvmGpuCaps gpu_caps;

    memset(&gpu_caps, 0, sizeof(gpu_caps));

    status = uvm_rm_locked_call(nvUvmInterfaceQueryCaps(uvm_gpu_device_handle(gpu), &gpu_caps));
    if (status != NV_OK)
        return status;

    if (gpu_caps.numaEnabled) {
        UVM_ASSERT(uvm_parent_gpu_is_coherent(gpu->parent));

        gpu->mem_info.numa.enabled = true;
        gpu->mem_info.numa.node_id = gpu_caps.numaNodeId;
    }
    else {
        UVM_ASSERT(!uvm_parent_gpu_is_coherent(gpu->parent));
        gpu->mem_info.numa.node_id = NUMA_NO_NODE;
    }

    return NV_OK;
}

// Return a PASID to use with the internal address space (AS), or -1 if not
// supported. This PASID is needed to enable ATS in the internal AS, but it is
// not used in address translation requests, which only translate GPA->SPA.
// The buffer management thus remains the same: DMA mapped GPA addresses can
// be accessed by the GPU, while unmapped addresses can not and any access is
// blocked and potentially unrecoverable to the engine that made it.
static int gpu_get_internal_pasid(const uvm_gpu_t *gpu)
{
#if UVM_ATS_SVA_SUPPORTED() && defined(NV_IOMMU_IS_DMA_DOMAIN_PRESENT)
// iommu_is_dma_domain() was added in Linux 5.15 by commit bf3aed4660c6
// ("iommu: Introduce explicit type for non-strict DMA domains")
//
// SVA is not required for enabling ATS for internal UVM address spaces.
// However, it conveniently combines the necessary check for permissive license
// and the special behaviour of PASID 0 on SMMUv3 described below.
//
// PASID 0 is reserved on aarch64 SMMUv3 (see section 3.3.2 of the SMMU spec)
// because the corresponding page table is used to translate requests and
// transactions without an associated PASID.
// Linux 6.6+ generalized this value as IOMMU_NO_PASID for all architectures
// commit 4298780126c2 ("iommu: Generalize PASID 0 for normal DMA w/o PASID")
#ifdef IOMMU_NO_PASID
#define UVM_INTERNAL_PASID IOMMU_NO_PASID
#else
#define UVM_INTERNAL_PASID 0
#endif

    // Enable internal ATS only if ATS is enabled in general and we are using
    // 64kB base page size. The base page size limitation is needed to avoid
    // GH180 MMU behaviour which does not refetch invalid 4K ATS translations
    // on access (see bug 3949400). This also works in virtualized environments
    // because the entire 64kB guest page has to be mapped and pinned by the
    // hypervisor for device access.
    if (g_uvm_global.ats.enabled && PAGE_SIZE == UVM_PAGE_SIZE_64K) {
        struct device *dev = &gpu->parent->pci_dev->dev;
        struct iommu_domain *domain = iommu_get_domain_for_dev(dev);

        // Check that the iommu domain is controlled by linux DMA API and
        // return a valid reserved PASID. Using a reserved PASID is OK since
        // this value is only used for the internal UVM address space that
        // uses ATS only for GPA->SPA translations that don't use PASID.
        //
        // If a general reserved PASID is not available (e.g. non-smmuv3, <6.6)
        // we'd need to to reserve a PASID from the IOMMU driver here, or risk
        // PASID collision. Note that since the PASID should not be used during
        // normal operation, the collision would only manifest in error paths.
        if (domain && iommu_is_dma_domain(domain))
            return UVM_INTERNAL_PASID;
    }
#endif

    /* Invalid PASID for internal RM address space */
    return -1;
}

static NV_STATUS alloc_and_init_address_space(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    UvmGpuAddressSpaceInfo gpu_address_space_info = {0};

    status = uvm_rm_locked_call(nvUvmInterfaceAddressSpaceCreate(uvm_gpu_device_handle(gpu),
                                                                 gpu->parent->rm_va_base,
                                                                 gpu->parent->rm_va_size,
                                                                 gpu_get_internal_pasid(gpu) != -1,
                                                                 &gpu->rm_address_space,
                                                                 &gpu_address_space_info));
    if (status != NV_OK)
        return status;

    UVM_ASSERT(gpu_address_space_info.bigPageSize <= NV_U32_MAX);

    gpu->big_page.internal_size = gpu_address_space_info.bigPageSize;
    gpu->time.time0_register = gpu_address_space_info.time0Offset;
    gpu->time.time1_register = gpu_address_space_info.time1Offset;

    gpu->max_subcontexts = gpu_address_space_info.maxSubctxCount;

    return NV_OK;
}

int uvm_device_p2p_static_bar(uvm_gpu_t *gpu)
{
    return nv_bar_index_to_os_bar_index(gpu->parent->pci_dev, NV_GPU_BAR_INDEX_FB);
}

static NV_STATUS get_gpu_fb_info(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    UvmGpuFbInfo fb_info = {0};
    unsigned long pci_bar1_addr = pci_resource_start(gpu->parent->pci_dev, uvm_device_p2p_static_bar(gpu));

    status = uvm_rm_locked_call(nvUvmInterfaceGetFbInfo(uvm_gpu_device_handle(gpu), &fb_info));
    if (status != NV_OK)
        return status;

    if (!fb_info.bZeroFb) {
        gpu->mem_info.size = ((NvU64)fb_info.heapSize + fb_info.reservedHeapSize) * 1024;
        gpu->mem_info.max_allocatable_address = fb_info.maxAllocatableAddress;
        gpu->mem_info.phys_start = (NvU64)fb_info.heapStart * 1024;
    }

    gpu->mem_info.max_vidmem_page_size = fb_info.maxVidmemPageSize;
    gpu->mem_info.static_bar1_start = pci_bar1_addr + fb_info.staticBar1StartOffset;
    gpu->mem_info.static_bar1_size = fb_info.staticBar1Size;

    return NV_OK;
}

static NV_STATUS get_gpu_ecc_info(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    UvmGpuEccInfo ecc_info = {0};

    status = uvm_rm_locked_call(nvUvmInterfaceGetEccInfo(uvm_gpu_device_handle(gpu), &ecc_info));
    if (status != NV_OK)
        return status;

    gpu->ecc.enabled = ecc_info.bEccEnabled;
    if (gpu->ecc.enabled) {
        gpu->ecc.hw_interrupt_tree_location = (volatile NvU32*)((char*)ecc_info.eccReadLocation + ecc_info.eccOffset);
        UVM_ASSERT(gpu->ecc.hw_interrupt_tree_location != NULL);

        gpu->ecc.mask = ecc_info.eccMask;
        UVM_ASSERT(gpu->ecc.mask != 0);

        gpu->ecc.error_notifier = ecc_info.eccErrorNotifier;
        UVM_ASSERT(gpu->ecc.error_notifier != NULL);
    }

    return NV_OK;
}

static NV_STATUS get_gpu_nvlink_info(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    UvmGpuNvlinkInfo nvlink_info = {0};

    status = uvm_rm_locked_call(nvUvmInterfaceGetNvlinkInfo(uvm_gpu_device_handle(gpu), &nvlink_info));
    if (status != NV_OK)
        return status;

    atomic_set(&gpu->nvlink_status.injected_error, UVM_TEST_NVLINK_ERROR_NONE);

    gpu->nvlink_status.enabled = nvlink_info.bNvlinkRecoveryEnabled;
    if (gpu->nvlink_status.enabled) {
        gpu->nvlink_status.hw_interrupt_tree_location =
             (volatile NvU32*)((char*)nvlink_info.nvlinkReadLocation + nvlink_info.nvlinkOffset);
        UVM_ASSERT(gpu->nvlink_status.hw_interrupt_tree_location != NULL);

        gpu->nvlink_status.mask = nvlink_info.nvlinkMask;
        UVM_ASSERT(gpu->nvlink_status.mask != 0);

        gpu->nvlink_status.error_notifier = nvlink_info.nvlinkErrorNotifier;
        UVM_ASSERT(gpu->nvlink_status.error_notifier != NULL);
    }

    return NV_OK;
}

NV_STATUS uvm_gpu_inject_nvlink_error(uvm_gpu_t *gpu, UVM_TEST_NVLINK_ERROR_TYPE error_type)
{
    UVM_ASSERT(gpu);

    if (!uvm_enable_builtin_tests)
        return NV_ERR_NOT_SUPPORTED;

    switch (error_type)
    {
    case UVM_TEST_NVLINK_ERROR_NONE:
    case UVM_TEST_NVLINK_ERROR_RESOLVED:
    case UVM_TEST_NVLINK_ERROR_UNRESOLVED:
        break;

    default:
        return NV_ERR_INVALID_ARGUMENT;
    }

    atomic_set(&gpu->nvlink_status.injected_error, error_type);
    return NV_OK;
}

static bool gpu_supports_uvm(uvm_parent_gpu_t *parent_gpu)
{
    // TODO: Bug 1757136: Add Linux SLI support. Until then, explicitly disable
    //       UVM on SLI.
    return parent_gpu->rm_info.subdeviceCount == 1;
}

bool uvm_gpu_can_address(uvm_gpu_t *gpu, NvU64 addr, NvU64 size)
{
    // Lower and upper address spaces are typically found in platforms that use
    // the canonical address form.
    NvU64 max_va_lower;
    NvU64 min_va_upper;
    NvU64 addr_end = addr + size - 1;
    NvU8 gpu_addr_shift;
    NvU8 cpu_addr_shift;
    NvU8 addr_shift;

    // Watch out for calling this too early in init
    UVM_ASSERT(gpu->address_space_tree.hal);
    UVM_ASSERT(gpu->address_space_tree.hal->num_va_bits() < 64);
    UVM_ASSERT(addr <= addr_end);
    UVM_ASSERT(size > 0);

    gpu_addr_shift = gpu->address_space_tree.hal->num_va_bits();
    cpu_addr_shift = uvm_cpu_num_va_bits();
    addr_shift = gpu_addr_shift;

    // Pascal+ GPUs are capable of accessing kernel pointers in various modes
    // by applying the same upper-bit checks that x86 or ARM CPU processors do.
    // The x86 and ARM platforms use canonical form addresses. For ARM, even
    // with Top-Byte Ignore enabled, the following logic validates addresses
    // from the kernel VA range.
    //
    // The following diagram illustrates the valid (V) VA regions that can be
    // mapped (or addressed) by the GPU/CPU when the CPU uses canonical form.
    // (C) regions are only accessible by the CPU. Similarly, (G) regions
    // are only accessible by the GPU. (X) regions are not addressible.
    // Note that we only consider (V) regions, i.e., address ranges that are
    // addressable by both, the CPU and GPU.
    //
    //               GPU MAX VA < CPU MAX VA           GPU MAX VA >= CPU MAX VA
    //          0xF..F +----------------+          0xF..F +----------------+
    //                 |VVVVVVVVVVVVVVVV|                 |VVVVVVVVVVVVVVVV|
    //                 |VVVVVVVVVVVVVVVV|                 |VVVVVVVVVVVVVVVV|
    //                 |VVVVVVVVVVVVVVVV|                 |VVVVVVVVVVVVVVVV|
    // GPU MIN UPPER VA|----------------| CPU MIN UPPER VA|----------------|
    //                 |CCCCCCCCCCCCCCCC|                 |GGGGGGGGGGGGGGGG|
    //                 |CCCCCCCCCCCCCCCC|                 |GGGGGGGGGGGGGGGG|
    // CPU MIN UPPER VA|----------------| GPU MIN UPPER VA|----------------|
    //                 |XXXXXXXXXXXXXXXX|                 |XXXXXXXXXXXXXXXX|
    //                 |XXXXXXXXXXXXXXXX|                 |XXXXXXXXXXXXXXXX|
    // CPU MAX LOWER VA|----------------| GPU MAX LOWER VA|----------------|
    //                 |CCCCCCCCCCCCCCCC|                 |GGGGGGGGGGGGGGGG|
    //                 |CCCCCCCCCCCCCCCC|                 |GGGGGGGGGGGGGGGG|
    // GPU MAX LOWER VA|----------------| CPU MAX LOWER VA|----------------|
    //                 |VVVVVVVVVVVVVVVV|                 |VVVVVVVVVVVVVVVV|
    //                 |VVVVVVVVVVVVVVVV|                 |VVVVVVVVVVVVVVVV|
    //                 |VVVVVVVVVVVVVVVV|                 |VVVVVVVVVVVVVVVV|
    //               0 +----------------+               0 +----------------+

    // On Pascal+ GPUs.
    if (gpu_addr_shift > 40) {
        // On x86, when cpu_addr_shift > gpu_addr_shift, it means the CPU uses
        // 5-level paging and the GPU is pre-Hopper. On Pascal-Ada GPUs (49b
        // wide VA) we set addr_shift to match a 4-level paging x86 (48b wide).
        // See more details on uvm_parent_gpu_canonical_address(..);
        if (cpu_addr_shift > gpu_addr_shift)
            addr_shift = NVCPU_IS_X86_64 ? 48 : 49;
        else if (gpu_addr_shift == 57)
            addr_shift = gpu_addr_shift;
        else
            addr_shift = cpu_addr_shift;
    }

    uvm_get_unaddressable_range(addr_shift, &max_va_lower, &min_va_upper);

    return (addr_end < max_va_lower) || (addr >= min_va_upper);
}

// The internal UVM VAS does not use canonical form addresses.
bool uvm_gpu_can_address_kernel(uvm_gpu_t *gpu, NvU64 addr, NvU64 size)
{
    NvU64 addr_end = addr + size - 1;
    NvU64 max_gpu_va;

    // Watch out for calling this too early in init
    UVM_ASSERT(gpu->address_space_tree.hal);
    UVM_ASSERT(gpu->address_space_tree.hal->num_va_bits() < 64);
    UVM_ASSERT(addr <= addr_end);
    UVM_ASSERT(size > 0);

    max_gpu_va = 1ULL << gpu->address_space_tree.hal->num_va_bits();
    return addr_end < max_gpu_va;
}

NvU64 uvm_parent_gpu_canonical_address(uvm_parent_gpu_t *parent_gpu, NvU64 addr)
{
    NvU8 gpu_addr_shift;
    NvU8 cpu_addr_shift;
    NvU8 addr_shift;
    NvU64 input_addr = addr;

    // When the CPU VA width is larger than GPU's, it means that:
    // On ARM: the CPU is on LVA mode and the GPU is pre-Hopper.
    // On x86: the CPU uses 5-level paging and the GPU is pre-Hopper.
    // We sign-extend on the 48b on ARM and on the 47b on x86 to mirror the
    // behavior of CPUs with smaller (than GPU) VA widths.
    gpu_addr_shift = parent_gpu->arch_hal->mmu_mode_hal(UVM_PAGE_SIZE_64K)->num_va_bits();
    cpu_addr_shift = uvm_cpu_num_va_bits();

    if (cpu_addr_shift > gpu_addr_shift)
        addr_shift = NVCPU_IS_X86_64 ? 48 : 49;
    else if (gpu_addr_shift == 57)
        addr_shift = gpu_addr_shift;
    else
        addr_shift = cpu_addr_shift;

    addr = (NvU64)((NvS64)(addr << (64 - addr_shift)) >> (64 - addr_shift));

    // This protection acts on when the address is not covered by the GPU's
    // OOR_ADDR_CHECK. This can only happen when OOR_ADDR_CHECK is in
    // permissive (NO_CHECK) mode.
    if ((addr << (64 - gpu_addr_shift)) != (input_addr << (64 - gpu_addr_shift)))
        return input_addr;

    return addr;
}

static void gpu_info_print_ce_caps(uvm_gpu_t *gpu, struct seq_file *s)
{
    NvU32 i;
    UvmGpuCopyEnginesCaps *ces_caps;
    NV_STATUS status;

    ces_caps = uvm_kvmalloc_zero(sizeof(*ces_caps));
    if (!ces_caps) {
        UVM_SEQ_OR_DBG_PRINT(s, "supported_ces: unavailable (no memory)\n");
        return;
    }

    status = uvm_rm_locked_call(nvUvmInterfaceQueryCopyEnginesCaps(uvm_gpu_device_handle(gpu), ces_caps));
    if (status != NV_OK) {
        UVM_SEQ_OR_DBG_PRINT(s, "supported_ces: unavailable (query failed)\n");
        goto out;
    }

    UVM_SEQ_OR_DBG_PRINT(s, "supported_ces:\n");
    for (i = 0; i < UVM_COPY_ENGINE_COUNT_MAX; ++i) {
        UvmGpuCopyEngineCaps *ce_caps = ces_caps->copyEngineCaps + i;

        if (!ce_caps->supported)
            continue;

        UVM_SEQ_OR_DBG_PRINT(s, " ce %u pce mask 0x%08x grce %u shared %u sysmem read %u sysmem write %u sysmem %u "
                             "nvlink p2p %u p2p %u secure %u\n",
                             i,
                             ce_caps->cePceMask,
                             ce_caps->grce,
                             ce_caps->shared,
                             ce_caps->sysmemRead,
                             ce_caps->sysmemWrite,
                             ce_caps->sysmem,
                             ce_caps->nvlinkP2p,
                             ce_caps->p2p,
                             ce_caps->secure);
    }

out:
    uvm_kvfree(ces_caps);
}

static const char *uvm_gpu_virt_type_string(UVM_VIRT_MODE virtMode)
{
    BUILD_BUG_ON(UVM_VIRT_MODE_COUNT != 4);

    switch (virtMode) {
        UVM_ENUM_STRING_CASE(UVM_VIRT_MODE_NONE);
        UVM_ENUM_STRING_CASE(UVM_VIRT_MODE_LEGACY);
        UVM_ENUM_STRING_CASE(UVM_VIRT_MODE_SRIOV_HEAVY);
        UVM_ENUM_STRING_CASE(UVM_VIRT_MODE_SRIOV_STANDARD);
        UVM_ENUM_STRING_DEFAULT();
    }
}

static const char *uvm_gpu_link_type_string(uvm_gpu_link_type_t link_type)
{

    BUILD_BUG_ON(UVM_GPU_LINK_MAX != 8);

    switch (link_type) {
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_INVALID);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_PCIE);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_NVLINK_1);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_NVLINK_2);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_NVLINK_3);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_NVLINK_4);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_NVLINK_5);
        UVM_ENUM_STRING_CASE(UVM_GPU_LINK_C2C);
        UVM_ENUM_STRING_DEFAULT();
    }
}

static void gpu_info_print_common(uvm_gpu_t *gpu, struct seq_file *s)
{
    const UvmGpuInfo *gpu_info = &gpu->parent->rm_info;
    NvU64 num_pages_in;
    NvU64 num_pages_out;
    NvU64 mapped_cpu_pages_size;
    NvU32 get;
    NvU32 put;
    NvU32 i;
    unsigned int cpu;

    UVM_SEQ_OR_DBG_PRINT(s, "GPU %s\n", uvm_gpu_name(gpu));
    UVM_SEQ_OR_DBG_PRINT(s, "retained_count                         %llu\n", uvm_gpu_retained_count(gpu));
    UVM_SEQ_OR_DBG_PRINT(s, "ecc                                    %s\n", gpu->ecc.enabled ? "enabled" : "disabled");
    UVM_SEQ_OR_DBG_PRINT(s, "nvlink status and recovery             %s\n", gpu->nvlink_status.enabled ? "enabled" : "disabled");
    if (gpu->parent->closest_cpu_numa_node == -1)
        UVM_SEQ_OR_DBG_PRINT(s, "closest_cpu_numa_node                  n/a\n");
    else
        UVM_SEQ_OR_DBG_PRINT(s, "closest_cpu_numa_node                  %d\n", gpu->parent->closest_cpu_numa_node);

    if (!uvm_procfs_is_debug_enabled())
        return;

    UVM_SEQ_OR_DBG_PRINT(s, "CPU link type                          %s\n",
                         uvm_gpu_link_type_string(gpu->parent->system_bus.link));
    UVM_SEQ_OR_DBG_PRINT(s, "CPU link bandwidth                     %uMBps\n",
                         gpu->parent->system_bus.link_rate_mbyte_per_s);

    UVM_SEQ_OR_DBG_PRINT(s, "architecture                           0x%X\n", gpu_info->gpuArch);
    UVM_SEQ_OR_DBG_PRINT(s, "implementation                         0x%X\n", gpu_info->gpuImplementation);
    UVM_SEQ_OR_DBG_PRINT(s, "gpcs                                   %u\n", gpu_info->gpcCount);
    UVM_SEQ_OR_DBG_PRINT(s, "max_gpcs                               %u\n", gpu_info->maxGpcCount);
    UVM_SEQ_OR_DBG_PRINT(s, "tpcs                                   %u\n", gpu_info->tpcCount);
    UVM_SEQ_OR_DBG_PRINT(s, "max_tpcs_per_gpc                       %u\n", gpu_info->maxTpcPerGpcCount);
    UVM_SEQ_OR_DBG_PRINT(s, "host_class                             0x%X\n", gpu_info->hostClass);
    UVM_SEQ_OR_DBG_PRINT(s, "ce_class                               0x%X\n", gpu_info->ceClass);
    UVM_SEQ_OR_DBG_PRINT(s, "virtualization_mode                    %s\n",
                         uvm_gpu_virt_type_string(gpu_info->virtMode));
    UVM_SEQ_OR_DBG_PRINT(s, "big_page_size                          %u\n", gpu->big_page.internal_size);
    UVM_SEQ_OR_DBG_PRINT(s, "rm_va_base                             0x%llx\n", gpu->parent->rm_va_base);
    UVM_SEQ_OR_DBG_PRINT(s, "rm_va_size                             0x%llx\n", gpu->parent->rm_va_size);
    UVM_SEQ_OR_DBG_PRINT(s, "vidmem_start                           %llu (%llu MBs)\n",
                         gpu->mem_info.phys_start,
                         gpu->mem_info.phys_start / (1024 * 1024));
    UVM_SEQ_OR_DBG_PRINT(s, "vidmem_size                            %llu (%llu MBs)\n",
                         gpu->mem_info.size,
                         gpu->mem_info.size / (1024 * 1024));
    UVM_SEQ_OR_DBG_PRINT(s, "vidmem_max_allocatable                 0x%llx (%llu MBs)\n",
                         gpu->mem_info.max_allocatable_address,
                         gpu->mem_info.max_allocatable_address / (1024 * 1024));

    if (gpu->mem_info.numa.enabled) {
        NvU64 window_size = gpu->parent->system_bus.memory_window_end - gpu->parent->system_bus.memory_window_start + 1;
        UVM_SEQ_OR_DBG_PRINT(s, "numa_node_id                           %u\n", uvm_gpu_numa_node(gpu));
        UVM_SEQ_OR_DBG_PRINT(s, "memory_window_start                    0x%llx\n",
                             gpu->parent->system_bus.memory_window_start);
        UVM_SEQ_OR_DBG_PRINT(s, "memory_window_end                      0x%llx\n",
                             gpu->parent->system_bus.memory_window_end);
        UVM_SEQ_OR_DBG_PRINT(s, "system_memory_window_size              0x%llx (%llu MBs)\n",
                             window_size,
                             window_size / (1024 * 1024));
    }

    UVM_SEQ_OR_DBG_PRINT(s, "interrupts                             %llu\n", gpu->parent->isr.interrupt_count);

    if (gpu->parent->isr.replayable_faults.handling) {
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_bh                   %llu\n",
                             gpu->parent->isr.replayable_faults.stats.bottom_half_count);
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_bh/cpu\n");
        for_each_cpu(cpu, &gpu->parent->isr.replayable_faults.stats.cpus_used_mask) {
            UVM_SEQ_OR_DBG_PRINT(s, "    cpu%02u                              %llu\n",
                                 cpu,
                                 gpu->parent->isr.replayable_faults.stats.cpu_exec_count[cpu]);
        }
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_buffer_entries       %u\n",
                             gpu->parent->fault_buffer.replayable.max_faults);
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_cached_get           %u\n",
                             gpu->parent->fault_buffer.replayable.cached_get);
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_cached_put           %u\n",
                             gpu->parent->fault_buffer.replayable.cached_put);
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_get                  %u\n",
                             gpu->parent->fault_buffer_hal->read_get(gpu->parent));
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_put                  %u\n",
                             gpu->parent->fault_buffer_hal->read_put(gpu->parent));
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_fault_batch_size     %u\n",
                             gpu->parent->fault_buffer.max_batch_size);
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_replay_policy        %s\n",
                             uvm_perf_fault_replay_policy_string(gpu->parent->fault_buffer.replayable.replay_policy));
        UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults_num_faults           %llu\n",
                             gpu->parent->stats.num_replayable_faults);
    }
    if (gpu->parent->isr.non_replayable_faults.handling) {
        UVM_SEQ_OR_DBG_PRINT(s, "non_replayable_faults_bh               %llu\n",
                             gpu->parent->isr.non_replayable_faults.stats.bottom_half_count);
        UVM_SEQ_OR_DBG_PRINT(s, "non_replayable_faults_bh/cpu\n");
        for_each_cpu(cpu, &gpu->parent->isr.non_replayable_faults.stats.cpus_used_mask) {
            UVM_SEQ_OR_DBG_PRINT(s, "    cpu%02u                              %llu\n",
                                 cpu,
                                 gpu->parent->isr.non_replayable_faults.stats.cpu_exec_count[cpu]);
        }
        UVM_SEQ_OR_DBG_PRINT(s, "non_replayable_faults_buffer_entries   %u\n",
                             gpu->parent->fault_buffer.non_replayable.max_faults);
        UVM_SEQ_OR_DBG_PRINT(s, "non_replayable_faults_num_faults       %llu\n",
                             gpu->parent->stats.num_non_replayable_faults);
    }

    for (i = 0; i < gpu_info->accessCntrBufferCount; i++) {
        if (gpu->parent->access_counters_supported && gpu->parent->isr.access_counters[i].handling_ref_count > 0) {
            UVM_SEQ_OR_DBG_PRINT(s, "access_counters_notif_buffer_index     %u\n", i);
            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_bh                   %llu\n",
                                 gpu->parent->isr.access_counters[i].stats.bottom_half_count);
            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_bh/cpu\n");
            for_each_cpu(cpu, &gpu->parent->isr.access_counters[i].stats.cpus_used_mask) {
                UVM_SEQ_OR_DBG_PRINT(s, "    cpu%02u                              %llu\n",
                                     cpu,
                                     gpu->parent->isr.access_counters[i].stats.cpu_exec_count[cpu]);
            }
            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_buffer_entries       %u\n",
                                 gpu->parent->access_counter_buffer[i].max_notifications);
            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_cached_get           %u\n",
                                 gpu->parent->access_counter_buffer[i].cached_get);
            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_cached_put           %u\n",
                                 gpu->parent->access_counter_buffer[i].cached_put);

            get = UVM_GPU_READ_ONCE(*gpu->parent->access_counter_buffer[i].rm_info.pAccessCntrBufferGet);
            put = UVM_GPU_READ_ONCE(*gpu->parent->access_counter_buffer[i].rm_info.pAccessCntrBufferPut);

            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_get                  %u\n", get);
            UVM_SEQ_OR_DBG_PRINT(s, "  access_counters_put                  %u\n", put);
        }
    }

    num_pages_out = atomic64_read(&gpu->parent->stats.num_pages_out);
    num_pages_in = atomic64_read(&gpu->parent->stats.num_pages_in);
    mapped_cpu_pages_size = atomic64_read(&gpu->parent->mapped_cpu_pages_size);

    UVM_SEQ_OR_DBG_PRINT(s, "migrated_pages_in                      %llu (%llu MB)\n",
                         num_pages_in,
                         (num_pages_in * (NvU64)PAGE_SIZE) / (1024u * 1024u));
    UVM_SEQ_OR_DBG_PRINT(s, "migrated_pages_out                     %llu (%llu MB)\n",
                         num_pages_out,
                         (num_pages_out * (NvU64)PAGE_SIZE) / (1024u * 1024u));
    UVM_SEQ_OR_DBG_PRINT(s, "mapped_cpu_pages_dma                   %llu (%llu MB)\n",
                         mapped_cpu_pages_size / PAGE_SIZE,
                         mapped_cpu_pages_size / (1024u * 1024u));

    gpu_info_print_ce_caps(gpu, s);

    if (g_uvm_global.conf_computing_enabled) {
        UVM_SEQ_OR_DBG_PRINT(s, "dma_buffer_pool_num_buffers             %lu\n",
                             gpu->conf_computing.dma_buffer_pool.num_dma_buffers);
    }
}

static void
gpu_fault_stats_print_common(uvm_parent_gpu_t *parent_gpu, struct seq_file *s)
{
    NvU64 num_pages_in;
    NvU64 num_pages_out;

    UVM_ASSERT(uvm_procfs_is_debug_enabled());

    UVM_SEQ_OR_DBG_PRINT(s, "replayable_faults      %llu\n", parent_gpu->stats.num_replayable_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "duplicates             %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_duplicate_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "faults_by_access_type:\n");
    UVM_SEQ_OR_DBG_PRINT(s, "  prefetch             %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_prefetch_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "  read                 %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_read_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "  write                %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_write_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "  atomic               %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_atomic_faults);
    num_pages_out = atomic64_read(&parent_gpu->fault_buffer.replayable.stats.num_pages_out);
    num_pages_in = atomic64_read(&parent_gpu->fault_buffer.replayable.stats.num_pages_in);
    UVM_SEQ_OR_DBG_PRINT(s, "migrations:\n");
    UVM_SEQ_OR_DBG_PRINT(s, "  num_pages_in         %llu (%llu MB)\n", num_pages_in,
                         (num_pages_in * (NvU64)PAGE_SIZE) / (1024u * 1024u));
    UVM_SEQ_OR_DBG_PRINT(s, "  num_pages_out        %llu (%llu MB)\n", num_pages_out,
                         (num_pages_out * (NvU64)PAGE_SIZE) / (1024u * 1024u));
    UVM_SEQ_OR_DBG_PRINT(s, "replays:\n");
    UVM_SEQ_OR_DBG_PRINT(s, "  start                %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_replays);
    UVM_SEQ_OR_DBG_PRINT(s, "  start_ack_all        %llu\n",
                         parent_gpu->fault_buffer.replayable.stats.num_replays_ack_all);
    UVM_SEQ_OR_DBG_PRINT(s, "non_replayable_faults  %llu\n", parent_gpu->stats.num_non_replayable_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "faults_by_access_type:\n");
    UVM_SEQ_OR_DBG_PRINT(s, "  read                 %llu\n",
                         parent_gpu->fault_buffer.non_replayable.stats.num_read_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "  write                %llu\n",
                         parent_gpu->fault_buffer.non_replayable.stats.num_write_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "  atomic               %llu\n",
                         parent_gpu->fault_buffer.non_replayable.stats.num_atomic_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "faults_by_addressing:\n");
    UVM_SEQ_OR_DBG_PRINT(s, "  virtual              %llu\n",
                         parent_gpu->stats.num_non_replayable_faults -
                         parent_gpu->fault_buffer.non_replayable.stats.num_physical_faults);
    UVM_SEQ_OR_DBG_PRINT(s, "  physical             %llu\n",
                         parent_gpu->fault_buffer.non_replayable.stats.num_physical_faults);
    num_pages_out = atomic64_read(&parent_gpu->fault_buffer.non_replayable.stats.num_pages_out);
    num_pages_in = atomic64_read(&parent_gpu->fault_buffer.non_replayable.stats.num_pages_in);
    UVM_SEQ_OR_DBG_PRINT(s, "migrations:\n");
    UVM_SEQ_OR_DBG_PRINT(s, "  num_pages_in         %llu (%llu MB)\n", num_pages_in,
                         (num_pages_in * (NvU64)PAGE_SIZE) / (1024u * 1024u));
    UVM_SEQ_OR_DBG_PRINT(s, "  num_pages_out        %llu (%llu MB)\n", num_pages_out,
                         (num_pages_out * (NvU64)PAGE_SIZE) / (1024u * 1024u));
}

static void gpu_access_counters_print_common(uvm_parent_gpu_t *parent_gpu, struct seq_file *s)
{
    NvU64 num_pages_in;
    NvU64 num_pages_out;
    NvU32 i;

    UVM_ASSERT(uvm_procfs_is_debug_enabled());

    // procfs_files are created before gpu_init_isr, we need to check if the
    // access_counter_buffer is allocated.
    if (parent_gpu->access_counter_buffer) {
        for (i = 0; i < parent_gpu->rm_info.accessCntrBufferCount; i++) {
            uvm_access_counter_buffer_t *access_counters = &parent_gpu->access_counter_buffer[i];

            num_pages_out = atomic64_read(&access_counters->stats.num_pages_out);
            num_pages_in = atomic64_read(&access_counters->stats.num_pages_in);
            UVM_SEQ_OR_DBG_PRINT(s, "migrations - buffer index %u:\n", i);
            UVM_SEQ_OR_DBG_PRINT(s, "  num_pages_in         %llu (%llu MB)\n", num_pages_in,
                                 (num_pages_in * (NvU64)PAGE_SIZE) / (1024u * 1024u));
            UVM_SEQ_OR_DBG_PRINT(s, "  num_pages_out        %llu (%llu MB)\n", num_pages_out,
                                 (num_pages_out * (NvU64)PAGE_SIZE) / (1024u * 1024u));
        }
    }
}

// This function converts an index of 2D array of size [N x N] into an index
// of upper triangular array of size [((N - 1) * ((N - 1) + 1)) / 2] which
// does not include diagonal elements.
static NvU32 peer_table_index(NvU32 index0, NvU32 index1, NvU32 N)
{
    NvU32 square_index, triangular_index, min_index;

    UVM_ASSERT(index0 != index1);
    UVM_ASSERT(index0 < N);
    UVM_ASSERT(index1 < N);

    // Calculate an index of 2D array by re-ordering indices to always point
    // to the same entry.
    min_index = min(index0, index1);
    square_index = min_index * N + max(index0, index1);

    // Calculate and subtract number of lower triangular matrix elements till
    // the current row (which includes diagonal elements) to get the correct
    // index in an upper triangular matrix.
    triangular_index = square_index - SUM_FROM_0_TO_N(min_index + 1);

    return triangular_index;
}

// This function converts an index of 2D array of size [N x N] into an index
// of upper triangular array of size [(N * (N + 1)) / 2] which does include
// diagonal elements.
static NvU32 sub_processor_peer_table_index(NvU32 index0, NvU32 index1)
{
    NvU32 square_index, triangular_index, min_index;

    UVM_ASSERT(index0 < UVM_PARENT_ID_MAX_SUB_PROCESSORS);
    UVM_ASSERT(index1 < UVM_PARENT_ID_MAX_SUB_PROCESSORS);

    // Calculate an index of 2D array by re-ordering indices to always point
    // to the same entry.
    min_index = min(index0, index1);
    square_index = min_index * UVM_PARENT_ID_MAX_SUB_PROCESSORS + max(index0, index1);

    // Calculate and subtract number of lower triangular matrix elements till
    // the current row (which doesn't include diagonal elements) to get the
    // correct index in an upper triangular matrix.
    triangular_index = square_index - SUM_FROM_0_TO_N(min_index);

    return triangular_index;
}

NvU32 uvm_gpu_pair_index(const uvm_gpu_id_t id0, const uvm_gpu_id_t id1)
{
    NvU32 index = peer_table_index(uvm_id_gpu_index(id0), uvm_id_gpu_index(id1), UVM_ID_MAX_GPUS);

    UVM_ASSERT(index < UVM_MAX_UNIQUE_GPU_PAIRS);

    return index;
}

static NvU32 parent_gpu_peer_table_index(const uvm_parent_gpu_id_t id0, const uvm_parent_gpu_id_t id1)
{
    NvU32 index = peer_table_index(uvm_parent_id_gpu_index(id0), uvm_parent_id_gpu_index(id1), UVM_PARENT_ID_MAX_GPUS);

    UVM_ASSERT(index < UVM_MAX_UNIQUE_PARENT_GPU_PAIRS);

    return index;
}

static NvU32 sub_processor_pair_index(const uvm_gpu_id_t id0, const uvm_gpu_id_t id1)
{
    NvU32 index = sub_processor_peer_table_index(uvm_id_sub_processor_index(id0), uvm_id_sub_processor_index(id1));

    UVM_ASSERT(index < UVM_MAX_UNIQUE_SUB_PROCESSOR_PAIRS);

    return index;
}

// Get the parent P2P capabilities between the given parent gpus.
static uvm_parent_gpu_peer_t *parent_gpu_peer_caps(const uvm_parent_gpu_t *parent_gpu0,
                                                   const uvm_parent_gpu_t *parent_gpu1)
{
    return &g_uvm_global.parent_gpu_peers[parent_gpu_peer_table_index(parent_gpu0->id, parent_gpu1->id)];
}

// Get the P2P capabilities between the given gpus.
static uvm_gpu_peer_t *gpu_peer_caps(const uvm_gpu_t *gpu0, const uvm_gpu_t *gpu1)
{
    uvm_parent_gpu_peer_t *parent_peer_caps = parent_gpu_peer_caps(gpu0->parent, gpu1->parent);

    return &parent_peer_caps->gpu_peers[sub_processor_pair_index(gpu0->id, gpu1->id)];
}

static uvm_aperture_t parent_gpu_peer_aperture(uvm_parent_gpu_t *local,
                                               uvm_parent_gpu_t *remote,
                                               uvm_parent_gpu_peer_t *parent_peer_caps)
{
    size_t peer_index;

    UVM_ASSERT(parent_peer_caps->ref_count);
    UVM_ASSERT(parent_peer_caps->link_type != UVM_GPU_LINK_INVALID);

    if (uvm_parent_id_value(local->id) < uvm_parent_id_value(remote->id))
        peer_index = 0;
    else
        peer_index = 1;

    return UVM_APERTURE_PEER(parent_peer_caps->peer_ids[peer_index]);
}

static void parent_gpu_peer_caps_print(uvm_parent_gpu_t **parent_gpu_pair, struct seq_file *s)
{
    uvm_parent_gpu_peer_t *parent_peer_caps;
    bool nvswitch_connected;
    uvm_aperture_t aperture;
    uvm_parent_gpu_t *local;
    uvm_parent_gpu_t *remote;
    const char *link_type;
    NvU32 bandwidth;

    UVM_ASSERT(uvm_procfs_is_debug_enabled());

    local = parent_gpu_pair[0];
    remote = parent_gpu_pair[1];
    parent_peer_caps = parent_gpu_peer_caps(local, remote);
    link_type = uvm_gpu_link_type_string(parent_peer_caps->link_type);
    bandwidth = parent_peer_caps->total_link_line_rate_mbyte_per_s;
    aperture = parent_gpu_peer_aperture(local, remote, parent_peer_caps);
    nvswitch_connected = uvm_parent_gpus_are_nvswitch_connected(local, remote);
    UVM_SEQ_OR_DBG_PRINT(s, "Link type                      %s\n", link_type);
    UVM_SEQ_OR_DBG_PRINT(s, "Bandwidth                      %uMBps\n", bandwidth);
    UVM_SEQ_OR_DBG_PRINT(s, "Aperture                       %s\n", uvm_aperture_string(aperture));
    UVM_SEQ_OR_DBG_PRINT(s, "Connected through NVSWITCH     %s\n", nvswitch_connected ? "True" : "False");
    UVM_SEQ_OR_DBG_PRINT(s, "Refcount                       %llu\n", READ_ONCE(parent_peer_caps->ref_count));
}

static int nv_procfs_read_gpu_info(struct seq_file *s, void *v)
{
    uvm_gpu_t *gpu = (uvm_gpu_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    gpu_info_print_common(gpu, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_gpu_info_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_gpu_info(s, v));
}

static int nv_procfs_read_gpu_fault_stats(struct seq_file *s, void *v)
{
    uvm_parent_gpu_t *parent_gpu = (uvm_parent_gpu_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    gpu_fault_stats_print_common(parent_gpu, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_gpu_fault_stats_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_gpu_fault_stats(s, v));
}

static int nv_procfs_read_gpu_access_counters(struct seq_file *s, void *v)
{
    uvm_parent_gpu_t *parent_gpu = (uvm_parent_gpu_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    gpu_access_counters_print_common(parent_gpu, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_gpu_access_counters_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_gpu_access_counters(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(gpu_info_entry);
UVM_DEFINE_SINGLE_PROCFS_FILE(gpu_fault_stats_entry);
UVM_DEFINE_SINGLE_PROCFS_FILE(gpu_access_counters_entry);

static void uvm_parent_gpu_uuid_string(char *buffer, const NvProcessorUuid *uuid)
{
    memcpy(buffer, UVM_PARENT_GPU_UUID_PREFIX, sizeof(UVM_PARENT_GPU_UUID_PREFIX) - 1);
    uvm_uuid_string(buffer + sizeof(UVM_PARENT_GPU_UUID_PREFIX) - 1, uuid);
}

static void uvm_gpu_uuid_string(char *buffer, const NvProcessorUuid *uuid)
{
    memcpy(buffer, UVM_GPU_UUID_PREFIX, sizeof(UVM_GPU_UUID_PREFIX) - 1);
    uvm_uuid_string(buffer + sizeof(UVM_GPU_UUID_PREFIX) - 1, uuid);
}

static NV_STATUS init_parent_procfs_dir(uvm_parent_gpu_t *parent_gpu)
{
    struct proc_dir_entry *gpu_base_dir_entry;
    char gpu_dir_name[UVM_PARENT_GPU_UUID_STRING_LENGTH];

    if (!uvm_procfs_is_enabled())
        return NV_OK;

    gpu_base_dir_entry = uvm_procfs_get_gpu_base_dir();

    // Create GPU-${physical-UUID} directory.
    uvm_parent_gpu_uuid_string(gpu_dir_name, &parent_gpu->uuid);

    parent_gpu->procfs.dir = NV_CREATE_PROC_DIR(gpu_dir_name, gpu_base_dir_entry);
    if (parent_gpu->procfs.dir == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    // GPU peer files are debug only.
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    parent_gpu->procfs.dir_peers = NV_CREATE_PROC_DIR(UVM_PROC_GPUS_PEER_DIR_NAME, parent_gpu->procfs.dir);
    if (parent_gpu->procfs.dir_peers == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static void deinit_parent_procfs_dir(uvm_parent_gpu_t *parent_gpu)
{
    proc_remove(parent_gpu->procfs.dir_peers);
    proc_remove(parent_gpu->procfs.dir);
}

static NV_STATUS init_parent_procfs_files(uvm_parent_gpu_t *parent_gpu)
{
    // Fault and access counter files are debug only
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    parent_gpu->procfs.fault_stats_file = NV_CREATE_PROC_FILE("fault_stats",
                                                              parent_gpu->procfs.dir,
                                                              gpu_fault_stats_entry,
                                                              parent_gpu);
    if (parent_gpu->procfs.fault_stats_file == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    parent_gpu->procfs.access_counters_file = NV_CREATE_PROC_FILE("access_counters",
                                                                  parent_gpu->procfs.dir,
                                                                  gpu_access_counters_entry,
                                                                  parent_gpu);
    if (parent_gpu->procfs.access_counters_file == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static void deinit_parent_procfs_files(uvm_parent_gpu_t *parent_gpu)
{
    proc_remove(parent_gpu->procfs.access_counters_file);
    proc_remove(parent_gpu->procfs.fault_stats_file);
}

static NV_STATUS init_procfs_dirs(uvm_gpu_t *gpu)
{
    struct proc_dir_entry *gpu_base_dir_entry;
    char symlink_name[16]; // Hold a uvm_gpu_id_t value in decimal.
    char uuid_buffer[NV_MAX(UVM_PARENT_GPU_UUID_STRING_LENGTH, UVM_GPU_UUID_STRING_LENGTH)];
    char gpu_dir_name[sizeof(symlink_name) + sizeof(uuid_buffer) + 1];

    if (!uvm_procfs_is_enabled())
        return NV_OK;

    uvm_parent_gpu_uuid_string(uuid_buffer, &gpu->parent->uuid);

    gpu_base_dir_entry = uvm_procfs_get_gpu_base_dir();

    // Create GPU-${physical-UUID}/${sub_processor_index} directory
    snprintf(gpu_dir_name, sizeof(gpu_dir_name), "%u", uvm_id_sub_processor_index(gpu->id));

    gpu->procfs.dir = NV_CREATE_PROC_DIR(gpu_dir_name, gpu->parent->procfs.dir);
    if (gpu->procfs.dir == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    // Create symlink from ${gpu_id} to
    // GPU-${physical-UUID}/${sub_processor_index}
    snprintf(symlink_name, sizeof(symlink_name), "%u", uvm_id_value(gpu->id));
    snprintf(gpu_dir_name,
             sizeof(gpu_dir_name),
             "%s/%u",
             uuid_buffer,
             uvm_id_sub_processor_index(gpu->id));

    gpu->procfs.dir_symlink = proc_symlink(symlink_name, gpu_base_dir_entry, gpu_dir_name);
    if (gpu->procfs.dir_symlink == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    // Create symlink from GI-${GI-UUID} to
    // GPU-${physical-UUID}/${sub_processor_index}
    uvm_gpu_uuid_string(uuid_buffer, &gpu->uuid);

    gpu->procfs.gpu_instance_uuid_symlink = proc_symlink(uuid_buffer, gpu_base_dir_entry, gpu_dir_name);
    if (gpu->procfs.gpu_instance_uuid_symlink == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

// The kernel waits on readers to finish before returning from those calls
static void deinit_procfs_dirs(uvm_gpu_t *gpu)
{
    proc_remove(gpu->procfs.gpu_instance_uuid_symlink);
    proc_remove(gpu->procfs.dir_symlink);
    proc_remove(gpu->procfs.dir);
}

static NV_STATUS init_procfs_files(uvm_gpu_t *gpu)
{
    gpu->procfs.info_file = NV_CREATE_PROC_FILE("info", gpu->procfs.dir, gpu_info_entry, gpu);
    if (gpu->procfs.info_file == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static void deinit_procfs_files(uvm_gpu_t *gpu)
{
    proc_remove(gpu->procfs.info_file);
}

static void deinit_procfs_parent_peer_cap_files(uvm_parent_gpu_peer_t *parent_peer_caps)
{
    proc_remove(parent_peer_caps->procfs.peer_symlink_file[0]);
    proc_remove(parent_peer_caps->procfs.peer_symlink_file[1]);
    proc_remove(parent_peer_caps->procfs.peer_file[0]);
    proc_remove(parent_peer_caps->procfs.peer_file[1]);
}

static NV_STATUS init_semaphore_pools(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_gpu_t *other_gpu;

    status = uvm_gpu_semaphore_pool_create(gpu, &gpu->semaphore_pool);
    if (status != NV_OK)
        return status;

    // When the Confidential Computing feature is enabled, a separate secure
    // pool is created that holds page allocated in the CPR of vidmem.
    if (g_uvm_global.conf_computing_enabled)
        return uvm_gpu_semaphore_secure_pool_create(gpu, &gpu->secure_semaphore_pool);

    for_each_gpu(other_gpu) {
        if (other_gpu == gpu)
            continue;
        status = uvm_gpu_semaphore_pool_map_gpu(other_gpu->semaphore_pool, gpu);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static void deinit_semaphore_pools(uvm_gpu_t *gpu)
{
    uvm_gpu_t *other_gpu;

    for_each_gpu(other_gpu) {
        if (other_gpu == gpu)
            continue;
        uvm_gpu_semaphore_pool_unmap_gpu(other_gpu->semaphore_pool, gpu);
    }

    uvm_gpu_semaphore_pool_destroy(gpu->semaphore_pool);
    uvm_gpu_semaphore_pool_destroy(gpu->secure_semaphore_pool);
}

static NV_STATUS find_unused_gpu_id(uvm_parent_gpu_t *parent_gpu, uvm_gpu_id_t *out_id)
{
    NvU32 i;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    if (!parent_gpu) {
        for (i = 0; i < UVM_PARENT_ID_MAX_GPUS; i++) {
            if (!g_uvm_global.parent_gpus[i]) {
                *out_id = uvm_gpu_id_from_sub_processor_index(i, 0);
                return NV_OK;
            }
        }
    }
    else {
        NvU32 sub_processor_index = find_first_zero_bit(parent_gpu->valid_gpus, UVM_PARENT_ID_MAX_SUB_PROCESSORS);

        if (sub_processor_index < UVM_PARENT_ID_MAX_SUB_PROCESSORS) {
            i = uvm_parent_id_gpu_index(parent_gpu->id);
            *out_id = uvm_gpu_id_from_sub_processor_index(i, sub_processor_index);
            return NV_OK;
        }
    }

    return NV_ERR_INSUFFICIENT_RESOURCES;
}

// Allocates a uvm_parent_gpu_t, assigns the GPU ID, and sets up basic data
// structures, but leaves all other initialization up to the caller.
static NV_STATUS alloc_parent_gpu(const NvProcessorUuid *gpu_uuid,
                                  uvm_parent_gpu_id_t gpu_id,
                                  uvm_parent_gpu_t **parent_gpu_out)
{
    uvm_parent_gpu_t *parent_gpu;
    NV_STATUS status;

    parent_gpu = uvm_kvmalloc_zero(sizeof(*parent_gpu));
    if (!parent_gpu)
        return NV_ERR_NO_MEMORY;

    parent_gpu->id = gpu_id;

    uvm_uuid_copy(&parent_gpu->uuid, gpu_uuid);
    uvm_sema_init(&parent_gpu->isr.replayable_faults.service_lock, 1, UVM_LOCK_ORDER_ISR);
    uvm_sema_init(&parent_gpu->isr.non_replayable_faults.service_lock, 1, UVM_LOCK_ORDER_ISR);
    uvm_mutex_init(&parent_gpu->access_counters_enablement_lock, UVM_LOCK_ORDER_ACCESS_COUNTERS);
    uvm_mutex_init(&parent_gpu->access_counters_clear_tracker_lock, UVM_LOCK_ACCESS_COUNTERS_CLEAR_OPS);
    uvm_tracker_init(&parent_gpu->access_counters_clear_tracker);
    uvm_spin_lock_irqsave_init(&parent_gpu->isr.interrupts_lock, UVM_LOCK_ORDER_LEAF);
    uvm_spin_lock_init(&parent_gpu->instance_ptr_table_lock, UVM_LOCK_ORDER_LEAF);
    uvm_rb_tree_init(&parent_gpu->instance_ptr_table);
    uvm_rb_tree_init(&parent_gpu->tsg_table);

    // TODO: Bug 3881835: revisit whether to use nv_kthread_q_t or workqueue.
    status = errno_to_nv_status(nv_kthread_q_init(&parent_gpu->lazy_free_q, "vidmem lazy free"));
    if (status != NV_OK)
        goto cleanup;

    nv_kref_init(&parent_gpu->gpu_kref);

    *parent_gpu_out = parent_gpu;

    return NV_OK;

cleanup:
    uvm_tracker_deinit(&parent_gpu->access_counters_clear_tracker);
    uvm_kvfree(parent_gpu);

    return status;
}

// Allocates a uvm_gpu_t struct and initializes the basic fields and leaves all
// other initialization up to the caller.
static uvm_gpu_t *alloc_gpu(uvm_parent_gpu_t *parent_gpu, uvm_gpu_id_t gpu_id)
{
    NvU32 sub_processor_index;
    uvm_gpu_t *gpu;

    gpu = uvm_kvmalloc_zero(sizeof(*gpu));
    if (!gpu)
        return gpu;

    gpu->id = gpu_id;
    gpu->parent = parent_gpu;

    // Initialize enough of the gpu struct for remove_gpu to be called
    gpu->magic = UVM_GPU_MAGIC_VALUE;
    uvm_spin_lock_init(&gpu->peer_info.peer_gpu_lock, UVM_LOCK_ORDER_LEAF);

    sub_processor_index = uvm_id_sub_processor_index(gpu_id);
    parent_gpu->gpus[sub_processor_index] = gpu;

    return gpu;
}

static NV_STATUS configure_address_space(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    NvU32 num_entries;
    NvU64 va_size;
    NvU64 va_per_entry;
    NvU64 physical_address;
    NvU64 dma_address;
    uvm_mmu_page_table_alloc_t *tree_alloc;

    status = uvm_page_tree_init(gpu,
                                NULL,
                                UVM_PAGE_TREE_TYPE_KERNEL,
                                gpu->big_page.internal_size,
                                uvm_get_page_tree_location(gpu),
                                &gpu->address_space_tree);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Initializing the page tree failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    num_entries = uvm_mmu_page_tree_entries(&gpu->address_space_tree, 0, UVM_PAGE_SIZE_AGNOSTIC);

    UVM_ASSERT(gpu->address_space_tree.hal->num_va_bits() < 64);
    va_size = 1ull << gpu->address_space_tree.hal->num_va_bits();
    va_per_entry = va_size / num_entries;

    // Make sure that RM's part of the VA is aligned to the VA covered by a
    // single top level PDE.
    UVM_ASSERT_MSG(gpu->parent->rm_va_base % va_per_entry == 0,
                   "va_base 0x%llx va_per_entry 0x%llx\n",
                   gpu->parent->rm_va_base,
                   va_per_entry);
    UVM_ASSERT_MSG(gpu->parent->rm_va_size % va_per_entry == 0,
                   "va_size 0x%llx va_per_entry 0x%llx\n",
                   gpu->parent->rm_va_size,
                   va_per_entry);

    tree_alloc = uvm_page_tree_pdb_internal(&gpu->address_space_tree);
    if (tree_alloc->addr.aperture == UVM_APERTURE_VID)
        physical_address = tree_alloc->addr.address;
    else
        physical_address = page_to_phys(tree_alloc->handle.page);
    status = uvm_rm_locked_call(nvUvmInterfaceSetPageDirectory(gpu->rm_address_space,
                                                               physical_address,
                                                               num_entries,
                                                               tree_alloc->addr.aperture == UVM_APERTURE_VID,
                                                               gpu_get_internal_pasid(gpu),
                                                               &dma_address));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceSetPageDirectory() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    if (tree_alloc->addr.aperture == UVM_APERTURE_SYS)
        gpu->address_space_tree.pdb_rm_dma_address = uvm_gpu_phys_address(UVM_APERTURE_SYS, dma_address);

    gpu->rm_address_space_moved_to_page_tree = true;

    return NV_OK;
}

static void deconfigure_address_space(uvm_gpu_t *gpu)
{
    if (gpu->rm_address_space_moved_to_page_tree)
        uvm_rm_locked_call_void(nvUvmInterfaceUnsetPageDirectory(gpu->rm_address_space));

    if (gpu->address_space_tree.root)
        uvm_page_tree_deinit(&gpu->address_space_tree);
}

static NV_STATUS service_interrupts(uvm_parent_gpu_t *parent_gpu)
{
    // Asking RM to service interrupts from top half interrupt handler would
    // very likely deadlock.
    UVM_ASSERT(!in_interrupt());

    return uvm_rm_locked_call(nvUvmInterfaceServiceDeviceInterruptsRM(parent_gpu->rm_device));
}

NV_STATUS uvm_gpu_check_ecc_error(uvm_gpu_t *gpu)
{
    NV_STATUS status = uvm_gpu_check_ecc_error_no_rm(gpu);

    if (status == NV_OK || status != NV_WARN_MORE_PROCESSING_REQUIRED)
        return status;

    // An interrupt that might mean an ECC error needs to be serviced.
    UVM_ASSERT(status == NV_WARN_MORE_PROCESSING_REQUIRED);

    status = service_interrupts(gpu->parent);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Servicing interrupts failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    // After servicing interrupts the ECC error notifier should be current.
    if (*gpu->ecc.error_notifier) {
        UVM_ERR_PRINT("ECC error encountered, GPU %s\n", uvm_gpu_name(gpu));
        uvm_global_set_fatal_error(NV_ERR_ECC_ERROR);
        return NV_ERR_ECC_ERROR;
    }

    return NV_OK;
}

NV_STATUS uvm_gpu_check_nvlink_error(uvm_gpu_t *gpu)
{
    NV_STATUS status = uvm_gpu_check_nvlink_error_no_rm(gpu);

    // Either NV_OK, or a resolved error code.
    if (status != NV_WARN_MORE_PROCESSING_REQUIRED)
        return status;

    // An interrupt that might mean an NVLINK error needs to be serviced.
    UVM_ASSERT(status == NV_WARN_MORE_PROCESSING_REQUIRED);

    status = service_interrupts(gpu->parent);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Servicing interrupts failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    // Check injected error if present, this works even if nvlink_status is not
    // supported.
    if (uvm_enable_builtin_tests) {
        if (atomic_read(&gpu->nvlink_status.injected_error) != UVM_TEST_NVLINK_ERROR_NONE)
            return NV_ERR_MEMORY_ERROR;

        UVM_ASSERT(gpu->nvlink_status.error_notifier);
    }

    // After servicing interrupts, the NVLINK error notifier should be current.
    if (*gpu->nvlink_status.error_notifier) {
        UVM_ERR_PRINT("NVLINK error encountered, GPU %s\n", uvm_gpu_name(gpu));
        return NV_ERR_MEMORY_ERROR;
    }

    return NV_OK;
}
static NV_STATUS init_parent_gpu(uvm_parent_gpu_t *parent_gpu,
                                 const NvProcessorUuid *gpu_uuid,
                                 const UvmGpuInfo *gpu_info,
                                 const UvmGpuPlatformInfo *gpu_platform_info)
{
    NV_STATUS status;
    UvmGpuFbInfo fb_info = {0};

    status = uvm_rm_locked_call(nvUvmInterfaceDeviceCreate(uvm_global_session_handle(),
                                                           gpu_info,
                                                           gpu_uuid,
                                                           &parent_gpu->rm_device,
                                                           NV_FALSE));
    if (status != NV_OK) {
        UVM_ERR_PRINT("Creating RM device failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    uvm_conf_computing_check_parent_gpu(parent_gpu);

    parent_gpu->pci_dev = gpu_platform_info->pci_dev;
    parent_gpu->closest_cpu_numa_node = dev_to_node(&parent_gpu->pci_dev->dev);
    parent_gpu->dma_addressable_start = gpu_platform_info->dma_addressable_start;
    parent_gpu->dma_addressable_limit = gpu_platform_info->dma_addressable_limit;
    parent_gpu->egm.enabled = gpu_info->egmEnabled;
    parent_gpu->egm.local_peer_id = gpu_info->egmPeerId;
    parent_gpu->egm.base_address = gpu_info->egmBaseAddr;
    parent_gpu->access_counters_supported = (gpu_info->accessCntrBufferCount != 0);

    status = uvm_rm_locked_call(nvUvmInterfaceGetFbInfo(parent_gpu->rm_device, &fb_info));
    if (status != NV_OK)
        return status;

    if (!fb_info.bZeroFb)
        parent_gpu->max_allocatable_address = fb_info.maxAllocatableAddress;

    parent_gpu->virt_mode = gpu_info->virtMode;
    if (parent_gpu->virt_mode == UVM_VIRT_MODE_LEGACY) {
        UVM_ERR_PRINT("Failed to init GPU %s. UVM is not supported in legacy virtualization mode\n",
                      uvm_parent_gpu_name(parent_gpu));
        return NV_ERR_NOT_SUPPORTED;
    }

    if (gpu_info->isSimulated)
        ++g_uvm_global.num_simulated_devices;

    status = init_parent_procfs_dir(parent_gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init parent procfs dir: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    status = uvm_hal_init_gpu(parent_gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init GPU hal: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    uvm_hal_init_properties(parent_gpu);

    UVM_ASSERT(!parent_gpu->rm_info.smcEnabled || parent_gpu->smc.supported);
    parent_gpu->smc.enabled = !!parent_gpu->rm_info.smcEnabled;

    uvm_mmu_init_gpu_chunk_sizes(parent_gpu);

    status = uvm_pmm_devmem_init(parent_gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("failed to intialize device private memory: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    status = uvm_ats_add_gpu(parent_gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_ats_add_gpu failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    status = init_parent_procfs_files(parent_gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init parent procfs files: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    status = uvm_parent_gpu_init_isr(parent_gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init ISR: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
        return status;
    }

    return NV_OK;
}

static NV_STATUS init_gpu(uvm_gpu_t *gpu, const UvmGpuInfo *gpu_info)
{
    char parent_uuid_buffer[UVM_UUID_STRING_LENGTH];
    char gi_uuid_buffer[UVM_UUID_STRING_LENGTH];
    NV_STATUS status;

    if (gpu->parent->smc.enabled) {
        status = uvm_rm_locked_call(nvUvmInterfaceDeviceCreate(uvm_global_session_handle(),
                                                               gpu_info,
                                                               &gpu->parent->uuid,
                                                               &gpu->smc.rm_device,
                                                               NV_TRUE));
        if (status != NV_OK) {
            UVM_ERR_PRINT("Creating RM device failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
            return status;
        }
    }

    uvm_uuid_copy(&gpu->uuid, &gpu_info->uuid);
    gpu->smc.swizz_id = gpu_info->smcSwizzId;

    uvm_uuid_string(parent_uuid_buffer, &gpu->parent->uuid);
    uvm_uuid_string(gi_uuid_buffer, &gpu->uuid);
    snprintf(gpu->name,
             sizeof(gpu->name),
             "ID %u: " UVM_PARENT_GPU_UUID_PREFIX "%s " UVM_GPU_UUID_PREFIX "%s",
             uvm_id_value(gpu->id),
             parent_uuid_buffer,
             gi_uuid_buffer);

    // Initialize the per-GPU procfs dirs as early as possible so that other
    // parts of the driver can add files in them as part of their per-GPU init.
    status = init_procfs_dirs(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init procfs dirs: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = get_gpu_caps(gpu);
    if (status != NV_OK) {
        if (status != NV_ERR_NVLINK_FABRIC_NOT_READY)
            UVM_ERR_PRINT("Failed to get GPU caps: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    uvm_mmu_init_gpu_peer_addresses(gpu);

    status = alloc_and_init_address_space(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Creating RM address space failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = get_gpu_fb_info(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to get GPU FB info: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = get_gpu_ecc_info(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to get GPU ECC info: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = get_gpu_nvlink_info(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to get GPU NVLINK RECOVERY info: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_pmm_gpu_init(&gpu->pmm);
    if (status != NV_OK) {
        UVM_ERR_PRINT("PMM initialization failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_pmm_sysmem_mappings_init(gpu, &gpu->pmm_reverse_sysmem_mappings);
    if (status != NV_OK) {
        UVM_ERR_PRINT("CPU PMM MMIO initialization failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    uvm_pmm_gpu_device_p2p_init(gpu);

    status = init_semaphore_pools(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to initialize the semaphore pool: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_channel_manager_create(gpu, &gpu->channel_manager);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to initialize the channel manager: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    status = configure_address_space(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to configure the GPU address space: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_mmu_create_flat_mappings(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Creating flat mappings failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_conf_computing_gpu_init(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to initialize Confidential Compute: %s for GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    status = init_procfs_files(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init procfs files: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_perf_heuristics_add_gpu(gpu);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init heuristics: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    return NV_OK;
}

static void sync_parent_gpu_trackers(uvm_parent_gpu_t *parent_gpu,
                                     bool sync_replay_tracker,
                                     bool sync_clear_faulted_tracker)
{
    NV_STATUS status;

    // Sync the replay tracker since it inherits dependencies from the VA block
    // trackers.
    if (sync_replay_tracker) {
        uvm_parent_gpu_replayable_faults_isr_lock(parent_gpu);
        status = uvm_tracker_wait(&parent_gpu->fault_buffer.replayable.replay_tracker);
        uvm_parent_gpu_replayable_faults_isr_unlock(parent_gpu);

        if (status != NV_OK)
            UVM_ASSERT(status == uvm_global_get_status());
    }

    // Sync the clear_faulted tracker since it inherits dependencies from the
    // VA block trackers, too.
    if (sync_clear_faulted_tracker) {
        uvm_parent_gpu_non_replayable_faults_isr_lock(parent_gpu);
        status = uvm_tracker_wait(&parent_gpu->fault_buffer.non_replayable.clear_faulted_tracker);
        uvm_parent_gpu_non_replayable_faults_isr_unlock(parent_gpu);

        if (status != NV_OK)
            UVM_ASSERT(status == uvm_global_get_status());
    }

    // Sync the access counter clear tracker too.
    if (parent_gpu->access_counters_supported && parent_gpu->access_counter_buffer) {
        uvm_mutex_lock(&parent_gpu->access_counters_clear_tracker_lock);
        status = uvm_tracker_wait(&parent_gpu->access_counters_clear_tracker);
        uvm_mutex_unlock(&parent_gpu->access_counters_clear_tracker_lock);

        if (status != NV_OK)
            UVM_ASSERT(status == uvm_global_get_status());
    }
}

// Remove all references the given GPU has to other GPUs, since one of those
// other GPUs is getting removed. This involves waiting for any unfinished
// trackers contained by this GPU.
static void remove_gpus_from_gpu(uvm_gpu_t *gpu)
{
    sync_parent_gpu_trackers(gpu->parent,
                             gpu->parent->isr.replayable_faults.handling,
                             gpu->parent->isr.non_replayable_faults.handling);

    // Sync all trackers in PMM
    uvm_pmm_gpu_sync(&gpu->pmm);

    // Sync all trackers in the GPU's DMA allocation pool
    uvm_conf_computing_dma_buffer_pool_sync(&gpu->conf_computing.dma_buffer_pool);
}

// Remove all references to the given GPU from its parent, since it is being
// removed.  This involves waiting for any unfinished trackers contained
// by the parent GPU.
static void remove_gpu_from_parent_gpu(uvm_gpu_t *gpu)
{
    // We use *.was_handling instead of *.handling here since this function is
    // called after uvm_gpu_disable_isr(), and the *.handling flags will
    // already have been copied to *.was_handling, and then set to false.
    sync_parent_gpu_trackers(gpu->parent,
                             gpu->parent->isr.replayable_faults.was_handling,
                             gpu->parent->isr.non_replayable_faults.was_handling);
}

static void deinit_parent_gpu(uvm_parent_gpu_t *parent_gpu)
{
    // All channels should have been removed before the retained count went to 0
    UVM_ASSERT(uvm_rb_tree_empty(&parent_gpu->instance_ptr_table));
    UVM_ASSERT(uvm_rb_tree_empty(&parent_gpu->tsg_table));

    deinit_parent_procfs_files(parent_gpu);

    // Return ownership to RM
    uvm_parent_gpu_deinit_isr(parent_gpu);

    uvm_pmm_devmem_deinit(parent_gpu);
    uvm_ats_remove_gpu(parent_gpu);

    UVM_ASSERT(atomic64_read(&parent_gpu->mapped_cpu_pages_size) == 0);

    // After calling nvUvmInterfaceUnregisterGpu() the reference to pci_dev may
    // not be valid any more so clear it ahead of time.
    parent_gpu->pci_dev = NULL;

    deinit_parent_procfs_dir(parent_gpu);

    if (parent_gpu->rm_info.isSimulated)
        --g_uvm_global.num_simulated_devices;

    if (parent_gpu->rm_device != 0)
        uvm_rm_locked_call_void(nvUvmInterfaceDeviceDestroy(parent_gpu->rm_device));

    uvm_parent_gpu_kref_put(parent_gpu);
}

static void deinit_gpu(uvm_gpu_t *gpu)
{
    uvm_gpu_t *other_gpu;

    // Remove any pointers to this GPU from other GPUs' trackers.
    for_each_gpu(other_gpu) {
        UVM_ASSERT(other_gpu != gpu);
        remove_gpus_from_gpu(other_gpu);
    }

    // Further, remove any pointers to this GPU from its parent's trackers.
    remove_gpu_from_parent_gpu(gpu);

    uvm_perf_heuristics_remove_gpu(gpu);

    deinit_procfs_files(gpu);

    // TODO Bug 3429163: [UVM] Move uvm_mmu_destroy_flat_mapping() to the
    // correct spot
    uvm_mmu_destroy_flat_mappings(gpu);

    // Wait for any deferred frees and their associated trackers to be finished
    // before tearing down channels.
    uvm_pmm_gpu_sync(&gpu->pmm);

    uvm_channel_manager_destroy(gpu->channel_manager);

    // Deconfigure the address space only after destroying all the channels as
    // in case any of them hit fatal errors, RM will assert that they are not
    // idle during nvUvmInterfaceUnsetPageDirectory() and that's an unnecessary
    // pain during development.
    deconfigure_address_space(gpu);

    deinit_semaphore_pools(gpu);

    uvm_pmm_gpu_device_p2p_deinit(gpu);

    uvm_pmm_sysmem_mappings_deinit(&gpu->pmm_reverse_sysmem_mappings);

    uvm_pmm_gpu_deinit(&gpu->pmm);

    if (gpu->rm_address_space != 0)
        uvm_rm_locked_call_void(nvUvmInterfaceAddressSpaceDestroy(gpu->rm_address_space));

    deinit_procfs_dirs(gpu);

    if (gpu->parent->smc.enabled) {
        if (gpu->smc.rm_device != 0)
            uvm_rm_locked_call_void(nvUvmInterfaceDeviceDestroy(gpu->smc.rm_device));
    }

    gpu->magic = 0;
}

// Do not not call this directly. It is called by nv_kref_put, when the
// GPU's ref count drops to zero.
static void uvm_parent_gpu_destroy(nv_kref_t *nv_kref)
{
    uvm_parent_gpu_t *parent_gpu = container_of(nv_kref, uvm_parent_gpu_t, gpu_kref);
    NvU32 sub_processor_index;

    UVM_ASSERT(parent_gpu->num_retained_gpus == 0);
    UVM_ASSERT(bitmap_empty(parent_gpu->valid_gpus, UVM_PARENT_ID_MAX_SUB_PROCESSORS));

    nv_kthread_q_stop(&parent_gpu->lazy_free_q);

    for_each_sub_processor_index(sub_processor_index)
        UVM_ASSERT(!parent_gpu->gpus[sub_processor_index]);

    uvm_tracker_deinit(&parent_gpu->access_counters_clear_tracker);

    uvm_kvfree(parent_gpu);
}

void uvm_parent_gpu_kref_put(uvm_parent_gpu_t *parent_gpu)
{
    nv_kref_put(&parent_gpu->gpu_kref, uvm_parent_gpu_destroy);
}

static void update_stats_parent_gpu_fault_instance(uvm_parent_gpu_t *parent_gpu,
                                                   const uvm_fault_buffer_entry_t *fault_entry,
                                                   bool is_duplicate)
{
    if (!fault_entry->is_replayable) {
        switch (fault_entry->fault_access_type)
        {
            case UVM_FAULT_ACCESS_TYPE_READ:
                ++parent_gpu->fault_buffer.non_replayable.stats.num_read_faults;
                break;
            case UVM_FAULT_ACCESS_TYPE_WRITE:
                ++parent_gpu->fault_buffer.non_replayable.stats.num_write_faults;
                break;
            case UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK:
            case UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG:
                ++parent_gpu->fault_buffer.non_replayable.stats.num_atomic_faults;
                break;
            default:
                UVM_ASSERT_MSG(false, "Invalid access type for non-replayable faults\n");
                break;
        }

        if (!fault_entry->is_virtual)
            ++parent_gpu->fault_buffer.non_replayable.stats.num_physical_faults;

        ++parent_gpu->stats.num_non_replayable_faults;

        return;
    }

    UVM_ASSERT(fault_entry->is_virtual);

    switch (fault_entry->fault_access_type)
    {
        case UVM_FAULT_ACCESS_TYPE_PREFETCH:
            ++parent_gpu->fault_buffer.replayable.stats.num_prefetch_faults;
            break;
        case UVM_FAULT_ACCESS_TYPE_READ:
            ++parent_gpu->fault_buffer.replayable.stats.num_read_faults;
            break;
        case UVM_FAULT_ACCESS_TYPE_WRITE:
            ++parent_gpu->fault_buffer.replayable.stats.num_write_faults;
            break;
        case UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK:
        case UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG:
            ++parent_gpu->fault_buffer.replayable.stats.num_atomic_faults;
            break;
        default:
            break;
    }
    if (is_duplicate || fault_entry->filtered)
        ++parent_gpu->fault_buffer.replayable.stats.num_duplicate_faults;

    ++parent_gpu->stats.num_replayable_faults;
}

static void update_stats_fault_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_parent_gpu_t *parent_gpu;
    const uvm_fault_buffer_entry_t *fault_entry, *fault_instance;

    UVM_ASSERT(event_id == UVM_PERF_EVENT_FAULT);

    if (UVM_ID_IS_CPU(event_data->fault.proc_id))
        return;

    // The reported fault entry must be the "representative" fault entry
    UVM_ASSERT(!event_data->fault.gpu.buffer_entry->filtered);

    parent_gpu = uvm_gpu_get(event_data->fault.proc_id)->parent;

    fault_entry = event_data->fault.gpu.buffer_entry;

    // Update the stats using the representative fault entry and the rest of
    // instances
    update_stats_parent_gpu_fault_instance(parent_gpu, fault_entry, event_data->fault.gpu.is_duplicate);

    list_for_each_entry(fault_instance, &fault_entry->merged_instances_list, merged_instances_list)
        update_stats_parent_gpu_fault_instance(parent_gpu, fault_instance, event_data->fault.gpu.is_duplicate);
}

static void update_stats_migration_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_gpu_t *gpu_dst = NULL;
    uvm_gpu_t *gpu_src = NULL;
    NvU64 pages;
    bool is_replayable_fault;
    bool is_non_replayable_fault;
    bool is_access_counter;

    UVM_ASSERT(event_id == UVM_PERF_EVENT_MIGRATION);

    if (UVM_ID_IS_GPU(event_data->migration.dst))
        gpu_dst = uvm_gpu_get(event_data->migration.dst);

    if (UVM_ID_IS_GPU(event_data->migration.src))
        gpu_src = uvm_gpu_get(event_data->migration.src);

    if (!gpu_dst && !gpu_src)
        return;

    // Page prefetching is also triggered by faults
    is_replayable_fault =
        event_data->migration.make_resident_context->cause == UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT;
    is_non_replayable_fault =
        event_data->migration.make_resident_context->cause == UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT;
    is_access_counter =
        event_data->migration.make_resident_context->cause == UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER;

    pages = event_data->migration.bytes / PAGE_SIZE;
    UVM_ASSERT(event_data->migration.bytes % PAGE_SIZE == 0);
    UVM_ASSERT(pages > 0);

    if (gpu_dst) {
        atomic64_add(pages, &gpu_dst->parent->stats.num_pages_in);
        if (is_replayable_fault) {
            atomic64_add(pages, &gpu_dst->parent->fault_buffer.replayable.stats.num_pages_in);
        }
        else if (is_non_replayable_fault) {
            atomic64_add(pages, &gpu_dst->parent->fault_buffer.non_replayable.stats.num_pages_in);
        }
        else if (is_access_counter) {
            NvU32 index = event_data->migration.make_resident_context->access_counters_buffer_index;
            atomic64_add(pages, &gpu_dst->parent->access_counter_buffer[index].stats.num_pages_in);
        }
    }
    if (gpu_src) {
        atomic64_add(pages, &gpu_src->parent->stats.num_pages_out);
        if (is_replayable_fault) {
            atomic64_add(pages, &gpu_src->parent->fault_buffer.replayable.stats.num_pages_out);
        }
        else if (is_non_replayable_fault) {
            atomic64_add(pages, &gpu_src->parent->fault_buffer.non_replayable.stats.num_pages_out);
        }
        else if (is_access_counter) {
            NvU32 index = event_data->migration.make_resident_context->access_counters_buffer_index;
            atomic64_add(pages, &gpu_src->parent->access_counter_buffer[index].stats.num_pages_out);
        }
    }
}

// Override the UVM driver and GPU settings from the module loader
static void uvm_param_conf(void)
{
    // uvm_peer_copy: Valid entries are "phys" and "virt" for Ampere+ GPUs.
    // No effect in pre-Ampere GPUs
    if (strcmp(uvm_peer_copy, UVM_PARAM_PEER_COPY_VIRTUAL) == 0) {
        g_uvm_global.peer_copy_mode = UVM_GPU_PEER_COPY_MODE_VIRTUAL;
    }
    else {
        if (strcmp(uvm_peer_copy, UVM_PARAM_PEER_COPY_PHYSICAL) != 0) {
            UVM_INFO_PRINT("Invalid value for uvm_peer_copy = %s, using %s instead.\n",
                           uvm_peer_copy,
                           UVM_PARAM_PEER_COPY_PHYSICAL);
        }

        g_uvm_global.peer_copy_mode = UVM_GPU_PEER_COPY_MODE_PHYSICAL;
    }
}

NV_STATUS uvm_gpu_init(void)
{
    NV_STATUS status;

    uvm_param_conf();

    status = uvm_hal_init_table();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_hal_init_table() failed: %s\n", nvstatusToString(status));
        return status;
    }

    return NV_OK;
}

void uvm_gpu_exit(void)
{
    uvm_parent_gpu_t *parent_gpu;

    for_each_parent_gpu(parent_gpu)
        UVM_ASSERT_MSG(false, "GPU still present: %s\n", uvm_parent_gpu_name(parent_gpu));

    // CPU should never be in the retained GPUs mask
    UVM_ASSERT(!uvm_processor_mask_test(&g_uvm_global.retained_gpus, UVM_ID_CPU));
}

NV_STATUS uvm_gpu_init_va_space(uvm_va_space_t *va_space)
{
    NV_STATUS status;

    if (uvm_procfs_is_debug_enabled()) {
        status = uvm_perf_register_event_callback(&va_space->perf_events,
                                                  UVM_PERF_EVENT_FAULT,
                                                  update_stats_fault_cb);
        if (status != NV_OK)
            return status;

        status = uvm_perf_register_event_callback(&va_space->perf_events,
                                                  UVM_PERF_EVENT_MIGRATION,
                                                  update_stats_migration_cb);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

uvm_parent_gpu_t *uvm_parent_gpu_get_by_uuid_locked(const NvProcessorUuid *gpu_uuid)
{
    uvm_parent_gpu_t *parent_gpu;

    for_each_parent_gpu(parent_gpu) {
        if (uvm_uuid_eq(&parent_gpu->uuid, gpu_uuid))
            return parent_gpu;
    }

    return NULL;
}

uvm_parent_gpu_t *uvm_parent_gpu_get_by_uuid(const NvProcessorUuid *gpu_uuid)
{
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    return uvm_parent_gpu_get_by_uuid_locked(gpu_uuid);
}

uvm_gpu_t *uvm_gpu_get_by_uuid(const NvProcessorUuid *gpu_uuid)
{
    uvm_gpu_id_t gpu_id;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    for_each_gpu_id(gpu_id) {
        uvm_gpu_t *gpu = uvm_gpu_get(gpu_id);

        if (gpu) {
            if (uvm_uuid_eq(&gpu->uuid, gpu_uuid))
                return gpu;
        }
    }

    return NULL;
}

static uvm_gpu_t *uvm_gpu_get_by_parent_and_swizz_id(uvm_parent_gpu_t *parent_gpu, NvU32 swizz_id)
{
    uvm_gpu_t *gpu;

    UVM_ASSERT(parent_gpu);
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    for_each_gpu_in_parent(gpu, parent_gpu) {
        if (gpu->smc.swizz_id == swizz_id)
            return gpu;
    }

    return NULL;
}

void uvm_gpu_retain(uvm_gpu_t *gpu)
{
    UVM_ASSERT(uvm_gpu_retained_count(gpu) > 0);
    atomic64_inc(&gpu->retained_count);
}

bool uvm_parent_gpus_are_nvswitch_connected(const uvm_parent_gpu_t *parent_gpu0, const uvm_parent_gpu_t *parent_gpu1)
{
    if (parent_gpu0 != parent_gpu1 &&
        parent_gpu0->nvswitch_info.is_nvswitch_connected &&
        parent_gpu1->nvswitch_info.is_nvswitch_connected) {
        UVM_ASSERT(parent_gpu_peer_caps(parent_gpu0, parent_gpu1)->link_type >= UVM_GPU_LINK_NVLINK_2);
        return true;
    }

    return false;
}

NV_STATUS uvm_gpu_check_ecc_error_no_rm(uvm_gpu_t *gpu)
{
    // We may need to call service_interrupts() which cannot be done in the top
    // half interrupt handler so assert here as well to catch improper use as
    // early as possible.
    UVM_ASSERT(!in_interrupt());

    if (!gpu->ecc.enabled)
        return NV_OK;

    // Early out If a global ECC error is already set to not spam the logs with
    // the same error.
    if (uvm_global_get_status() == NV_ERR_ECC_ERROR)
        return NV_ERR_ECC_ERROR;

    if (*gpu->ecc.error_notifier) {
        UVM_ERR_PRINT("ECC error encountered, GPU %s\n", uvm_gpu_name(gpu));
        uvm_global_set_fatal_error(NV_ERR_ECC_ERROR);
        return NV_ERR_ECC_ERROR;
    }

    // RM hasn't seen an ECC error yet, check whether there is a pending
    // interrupt that might indicate one. We might get false positives because
    // the interrupt bits we read are not ECC-specific. They're just the
    // top-level bits for any interrupt on all engines which support ECC. On
    // Pascal for example, RM returns us a mask with the bits for GR, L2, and
    // FB, because any of those might raise an ECC interrupt. So if they're set
    // we have to ask RM to check whether it was really an ECC error (and a
    // double-bit ECC error at that), in which case it sets the notifier.
    if ((*gpu->ecc.hw_interrupt_tree_location & gpu->ecc.mask) == 0) {
        // No pending interrupts.
        return NV_OK;
    }

    // An interrupt that might mean an ECC error needs to be serviced, signal
    // that to the caller.
    return NV_WARN_MORE_PROCESSING_REQUIRED;
}

NV_STATUS uvm_gpu_get_injected_nvlink_error(uvm_gpu_t *gpu)
{
    if (uvm_enable_builtin_tests) {
        UVM_TEST_NVLINK_ERROR_TYPE error_type = atomic_read(&gpu->nvlink_status.injected_error);

        if (error_type == UVM_TEST_NVLINK_ERROR_UNRESOLVED)
            return NV_WARN_MORE_PROCESSING_REQUIRED;

        if (error_type == UVM_TEST_NVLINK_ERROR_RESOLVED)
            return NV_ERR_MEMORY_ERROR;
    }

    return NV_OK;
}

NV_STATUS uvm_gpu_check_nvlink_error_no_rm(uvm_gpu_t *gpu)
{
    NV_STATUS status;

    // We may need to call service_interrupts() which cannot be done in the top
    // half interrupt handler so assert here as well to catch improper use as
    // early as possible.
    UVM_ASSERT(!in_interrupt());

    status = uvm_gpu_get_injected_nvlink_error(gpu);
    if (status != NV_OK)
        return status;

    if (!gpu->nvlink_status.enabled)
        return NV_OK;

    if (*gpu->nvlink_status.error_notifier) {
        UVM_ERR_PRINT("NVLINK error encountered, GPU %s\n", uvm_gpu_name(gpu));
        return NV_ERR_MEMORY_ERROR;
    }

    // RM hasn't seen an NVLINK error yet, check whether there is a pending
    // interrupt that might indicate one. We might get false positives because
    // the interrupt bits we read are not NVLINK-specific. They're just the
    // top-level bits for any interrupt on all engines which support NVLINK
    // errors.
    if ((*gpu->nvlink_status.hw_interrupt_tree_location & gpu->nvlink_status.mask) == 0) {
        // No pending interrupts.
        return NV_OK;
    }

    // An interrupt that might mean an NVLINK error needs to be serviced, signal
    // that to the caller.
    return NV_WARN_MORE_PROCESSING_REQUIRED;
}
static NV_STATUS get_parent_p2p_caps(uvm_parent_gpu_t *parent_gpu0,
                                     uvm_parent_gpu_t *parent_gpu1,
                                     UvmGpuP2PCapsParams *p2p_caps_params)
{
    NV_STATUS status;
    uvmGpuDeviceHandle rm_device0, rm_device1;

    if (uvm_parent_id_value(parent_gpu0->id) < uvm_parent_id_value(parent_gpu1->id)) {
        rm_device0 = parent_gpu0->rm_device;
        rm_device1 = parent_gpu1->rm_device;
    }
    else {
        rm_device0 = parent_gpu1->rm_device;
        rm_device1 = parent_gpu0->rm_device;
    }

    memset(p2p_caps_params, 0, sizeof(*p2p_caps_params));
    status = uvm_rm_locked_call(nvUvmInterfaceGetP2PCaps(rm_device0, rm_device1, p2p_caps_params));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceGetP2PCaps() failed with error: %s, for GPU0:%s and GPU1:%s\n",
                       nvstatusToString(status),
                       uvm_parent_gpu_name(parent_gpu0),
                       uvm_parent_gpu_name(parent_gpu1));
        return status;
    }

    return NV_OK;
}

static NV_STATUS create_parent_p2p_object(uvm_parent_gpu_t *parent_gpu0,
                                          uvm_parent_gpu_t *parent_gpu1,
                                          NvHandle *p2p_handle)
{
    NV_STATUS status;
    uvmGpuDeviceHandle rm_device0, rm_device1;

    if (uvm_parent_id_value(parent_gpu0->id) < uvm_parent_id_value(parent_gpu1->id)) {
        rm_device0 = parent_gpu0->rm_device;
        rm_device1 = parent_gpu1->rm_device;
    }
    else {
        rm_device0 = parent_gpu1->rm_device;
        rm_device1 = parent_gpu0->rm_device;
    }

    *p2p_handle = 0;

    status = uvm_rm_locked_call(nvUvmInterfaceP2pObjectCreate(rm_device0, rm_device1, p2p_handle));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceP2pObjectCreate() failed with error: %s, for GPU0:%s and GPU1:%s\n",
                       nvstatusToString(status),
                       uvm_parent_gpu_name(parent_gpu0),
                       uvm_parent_gpu_name(parent_gpu1));
        return status;
    }

    UVM_ASSERT(*p2p_handle);
    return NV_OK;
}

static void set_optimal_p2p_write_ces(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_parent_gpu_peer_t *parent_peer_caps = parent_gpu_peer_caps(gpu0->parent, gpu1->parent);
    bool sorted;
    NvU32 ce0, ce1;

    UVM_ASSERT(parent_peer_caps->ref_count);
    UVM_ASSERT(gpu0->parent->peer_copy_mode == gpu1->parent->peer_copy_mode);

    if (gpu0->parent->peer_copy_mode == UVM_GPU_PEER_COPY_MODE_UNSUPPORTED)
        return;

    if (parent_peer_caps->link_type < UVM_GPU_LINK_NVLINK_1)
        return;

    sorted = uvm_id_value(gpu0->id) < uvm_id_value(gpu1->id);
    ce0 = parent_peer_caps->optimalNvlinkWriteCEs[sorted ? 0 : 1];
    ce1 = parent_peer_caps->optimalNvlinkWriteCEs[sorted ? 1 : 0];

    uvm_channel_manager_set_p2p_ce(gpu0->channel_manager, gpu1, ce0);
    uvm_channel_manager_set_p2p_ce(gpu1->channel_manager, gpu0, ce1);
}

static int nv_procfs_read_parent_gpu_peer_caps(struct seq_file *s, void *v)
{
    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    parent_gpu_peer_caps_print((uvm_parent_gpu_t **)s->private, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_parent_gpu_peer_caps_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_parent_gpu_peer_caps(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(parent_gpu_peer_caps_entry);

static NV_STATUS init_procfs_parent_peer_cap_files(uvm_parent_gpu_t *local, uvm_parent_gpu_t *remote, size_t local_idx)
{
    // This needs to hold a uvm_parent_gpu_id_t in decimal
    char gpu_dir_name[16];

    // This needs to hold a GPU UUID
    char symlink_name[UVM_GPU_UUID_STRING_LENGTH];
    uvm_parent_gpu_peer_t *parent_peer_caps;

    UVM_ASSERT(uvm_procfs_is_debug_enabled());

    parent_peer_caps = parent_gpu_peer_caps(local, remote);
    parent_peer_caps->procfs.pairs[local_idx][0] = local;
    parent_peer_caps->procfs.pairs[local_idx][1] = remote;

    // Create gpus/gpuA/peers/gpuB
    snprintf(gpu_dir_name, sizeof(gpu_dir_name), "%u", uvm_parent_id_value(remote->id));
    parent_peer_caps->procfs.peer_file[local_idx] = NV_CREATE_PROC_FILE(gpu_dir_name,
                                                                        local->procfs.dir_peers,
                                                                        parent_gpu_peer_caps_entry,
                                                                        &parent_peer_caps->procfs.pairs[local_idx]);

    if (parent_peer_caps->procfs.peer_file[local_idx] == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    // Create a symlink from UVM GPU UUID (GPU-...) to the UVM GPU ID gpuB
    uvm_parent_gpu_uuid_string(symlink_name, &remote->uuid);
    parent_peer_caps->procfs.peer_symlink_file[local_idx] = proc_symlink(symlink_name,
                                                                         local->procfs.dir_peers,
                                                                         gpu_dir_name);
    if (parent_peer_caps->procfs.peer_symlink_file[local_idx] == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static NV_STATUS init_procfs_parent_peer_files(uvm_parent_gpu_t *parent_gpu0, uvm_parent_gpu_t *parent_gpu1)
{
    NV_STATUS status;

    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    status = init_procfs_parent_peer_cap_files(parent_gpu0, parent_gpu1, 0);
    if (status != NV_OK)
        return status;

    status = init_procfs_parent_peer_cap_files(parent_gpu1, parent_gpu0, 1);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

static NV_STATUS parent_peers_init(uvm_parent_gpu_t *parent_gpu0,
                                   uvm_parent_gpu_t *parent_gpu1,
                                   uvm_parent_gpu_peer_t *parent_peer_caps)
{
    UvmGpuP2PCapsParams p2p_caps_params;
    uvm_gpu_link_type_t link_type;
    NvHandle p2p_handle;
    NV_STATUS status;

    UVM_ASSERT(parent_peer_caps->ref_count == 0);

    status = create_parent_p2p_object(parent_gpu0, parent_gpu1, &p2p_handle);
    if (status != NV_OK)
        return status;

    status = get_parent_p2p_caps(parent_gpu0, parent_gpu1, &p2p_caps_params);
    if (status != NV_OK)
        goto cleanup;

    // check for peer-to-peer compatibility (PCI-E or NvLink).
    link_type = get_gpu_link_type(p2p_caps_params.p2pLink);
    if (link_type == UVM_GPU_LINK_INVALID || link_type == UVM_GPU_LINK_C2C) {
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup;
    }

    status = init_procfs_parent_peer_files(parent_gpu0, parent_gpu1);
    if (status != NV_OK)
        goto cleanup;

    parent_peer_caps->ref_count = 1;
    parent_peer_caps->p2p_handle = p2p_handle;
    parent_peer_caps->link_type = link_type;
    parent_peer_caps->total_link_line_rate_mbyte_per_s = p2p_caps_params.totalLinkLineRateMBps;

    if (parent_gpu0->egm.enabled || parent_gpu1->egm.enabled)
        UVM_ASSERT(parent_peer_caps->link_type >= UVM_GPU_LINK_NVLINK_2);

    // Initialize peer ids and establish peer mappings
    // Peer id from min(gpu_id0, gpu_id1) -> max(gpu_id0, gpu_id1)
    parent_peer_caps->peer_ids[0] = p2p_caps_params.peerIds[0];
    parent_peer_caps->egm_peer_ids[0] = p2p_caps_params.egmPeerIds[0];

    // Peer id from max(gpu_id0, gpu_id1) -> min(gpu_id0, gpu_id1)
    parent_peer_caps->peer_ids[1] = p2p_caps_params.peerIds[1];
    parent_peer_caps->egm_peer_ids[1] = p2p_caps_params.egmPeerIds[1];

    parent_peer_caps->optimalNvlinkWriteCEs[0] = p2p_caps_params.optimalNvlinkWriteCEs[0];
    parent_peer_caps->optimalNvlinkWriteCEs[1] = p2p_caps_params.optimalNvlinkWriteCEs[1];

    return NV_OK;

cleanup:
    uvm_rm_locked_call_void(nvUvmInterfaceP2pObjectDestroy(uvm_global_session_handle(), p2p_handle));

    return status;
}

static NV_STATUS parent_peers_retain(uvm_parent_gpu_t *parent_gpu0, uvm_parent_gpu_t *parent_gpu1)
{
    uvm_parent_gpu_peer_t *parent_peer_caps = parent_gpu_peer_caps(parent_gpu0, parent_gpu1);
    NV_STATUS status = NV_OK;

    if (parent_peer_caps->ref_count == 0)
        status = parent_peers_init(parent_gpu0, parent_gpu1, parent_peer_caps);
    else
        parent_peer_caps->ref_count++;

    return status;
}

static void parent_peers_destroy(uvm_parent_gpu_t *parent_gpu0,
                                 uvm_parent_gpu_t *parent_gpu1,
                                 uvm_parent_gpu_peer_t *parent_peer_caps)
{
    UVM_ASSERT(parent_peer_caps->p2p_handle);

    deinit_procfs_parent_peer_cap_files(parent_peer_caps);

    uvm_rm_locked_call_void(nvUvmInterfaceP2pObjectDestroy(uvm_global_session_handle(), parent_peer_caps->p2p_handle));

    memset(parent_peer_caps, 0, sizeof(*parent_peer_caps));
}

static void parent_peers_release(uvm_parent_gpu_t *parent_gpu0, uvm_parent_gpu_t *parent_gpu1)
{
    uvm_parent_gpu_peer_t *parent_peer_caps = parent_gpu_peer_caps(parent_gpu0, parent_gpu1);

    UVM_ASSERT(parent_peer_caps->ref_count);

    if (--parent_peer_caps->ref_count == 0)
        parent_peers_destroy(parent_gpu0, parent_gpu1, parent_peer_caps);
}

static NV_STATUS peers_init(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1, uvm_gpu_peer_t *peer_caps)
{
    NV_STATUS status;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    UVM_ASSERT(peer_caps->ref_count == 0);

    status = parent_peers_retain(gpu0->parent, gpu1->parent);
    if (status != NV_OK)
        return status;

    // Establish peer mappings from each GPU to the other.
    status = uvm_mmu_create_peer_identity_mappings(gpu0, gpu1);
    if (status != NV_OK)
        goto cleanup_parent;

    status = uvm_mmu_create_peer_identity_mappings(gpu1, gpu0);
    if (status != NV_OK)
        goto cleanup_mappings;

    peer_caps->ref_count = 1;

    set_optimal_p2p_write_ces(gpu0, gpu1);

    UVM_ASSERT(uvm_gpu_get(gpu0->id) == gpu0);
    UVM_ASSERT(uvm_gpu_get(gpu1->id) == gpu1);

    uvm_spin_lock(&gpu0->peer_info.peer_gpu_lock);
    uvm_processor_mask_set(&gpu0->peer_info.peer_gpu_mask, gpu1->id);
    uvm_spin_unlock(&gpu0->peer_info.peer_gpu_lock);

    uvm_spin_lock(&gpu1->peer_info.peer_gpu_lock);
    uvm_processor_mask_set(&gpu1->peer_info.peer_gpu_mask, gpu0->id);
    uvm_spin_unlock(&gpu1->peer_info.peer_gpu_lock);

    return NV_OK;

cleanup_mappings:
    uvm_mmu_destroy_peer_identity_mappings(gpu0, gpu1);

cleanup_parent:
    parent_peers_release(gpu0->parent, gpu1->parent);

    return status;
}

static NV_STATUS peers_retain(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_gpu_peer_t *peer_caps = gpu_peer_caps(gpu0, gpu1);
    NV_STATUS status = NV_OK;

    if (peer_caps->ref_count == 0)
        status = peers_init(gpu0, gpu1, peer_caps);
    else
        peer_caps->ref_count++;

    return status;
}

static void peers_destroy(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1, uvm_gpu_peer_t *peer_caps)
{
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    uvm_mmu_destroy_peer_identity_mappings(gpu0, gpu1);
    uvm_mmu_destroy_peer_identity_mappings(gpu1, gpu0);

    uvm_spin_lock(&gpu0->peer_info.peer_gpu_lock);
    uvm_processor_mask_clear(&gpu0->peer_info.peer_gpu_mask, gpu1->id);
    uvm_spin_unlock(&gpu0->peer_info.peer_gpu_lock);

    uvm_spin_lock(&gpu1->peer_info.peer_gpu_lock);
    uvm_processor_mask_clear(&gpu1->peer_info.peer_gpu_mask, gpu0->id);
    uvm_spin_unlock(&gpu1->peer_info.peer_gpu_lock);

    // Flush the access counter buffer to avoid getting stale notifications for
    // accesses to GPUs to which peer access is being disabled. This is also
    // needed in the case of disabling automatic (NVLINK) peers on GPU
    // unregister, because access counter processing might still be using GPU
    // IDs queried from the peer table above which are about to be removed from
    // the global table.
    if (gpu0->parent->access_counters_supported)
        uvm_parent_gpu_access_counter_buffer_flush(gpu0->parent);
    if (gpu1->parent->access_counters_supported)
        uvm_parent_gpu_access_counter_buffer_flush(gpu1->parent);

    parent_peers_release(gpu0->parent, gpu1->parent);

    memset(peer_caps, 0, sizeof(*peer_caps));
}

static void peers_release(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_gpu_peer_t *peer_caps = gpu_peer_caps(gpu0, gpu1);

    UVM_ASSERT(peer_caps->ref_count);

    if (--peer_caps->ref_count == 0)
        peers_destroy(gpu0, gpu1, peer_caps);
}

static void parent_peers_destroy_nvlink(uvm_parent_gpu_t *parent_gpu)
{
    uvm_parent_gpu_t *other_parent_gpu;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    for_each_parent_gpu(other_parent_gpu) {
        uvm_parent_gpu_peer_t *parent_peer_caps;

        if (other_parent_gpu == parent_gpu)
            continue;

        parent_peer_caps = parent_gpu_peer_caps(parent_gpu, other_parent_gpu);

        // PCIe peers need to be explicitly destroyed via UvmDisablePeerAccess
        if (parent_peer_caps->ref_count == 0 || parent_peer_caps->link_type == UVM_GPU_LINK_PCIE)
            continue;

        parent_peers_release(parent_gpu, other_parent_gpu);
    }
}

static NV_STATUS parent_peers_discover_nvlink(uvm_parent_gpu_t *parent_gpu)
{
    uvm_parent_gpu_t *other_parent_gpu;
    NV_STATUS status;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    for_each_parent_gpu(other_parent_gpu) {
        uvm_parent_gpu_peer_t *parent_peer_caps;
        UvmGpuP2PCapsParams p2p_caps_params;

        if (other_parent_gpu == parent_gpu)
            continue;

        status = get_parent_p2p_caps(parent_gpu, other_parent_gpu, &p2p_caps_params);
        if (status != NV_OK)
            goto cleanup;

        // PCIe peers need to be explicitly enabled via UvmEnablePeerAccess
        if (p2p_caps_params.p2pLink == UVM_LINK_TYPE_NONE || p2p_caps_params.p2pLink == UVM_LINK_TYPE_PCIE)
            continue;

        parent_peer_caps = parent_gpu_peer_caps(parent_gpu, other_parent_gpu);
        UVM_ASSERT(parent_peer_caps->ref_count == 0);
        status = parent_peers_init(parent_gpu, other_parent_gpu, parent_peer_caps);
        if (status != NV_OK)
            goto cleanup;
    }

    return NV_OK;

cleanup:
    parent_peers_destroy_nvlink(parent_gpu);

    return status;
}

static void peers_destroy_nvlink(uvm_gpu_t *gpu)
{
    uvm_parent_gpu_t *other_parent_gpu;
    uvm_parent_gpu_t *parent_gpu;

    UVM_ASSERT(gpu);
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    parent_gpu = gpu->parent;

    for_each_parent_gpu(other_parent_gpu) {
        uvm_parent_gpu_peer_t *parent_peer_caps;
        uvm_gpu_t *other_gpu;

        if (other_parent_gpu == parent_gpu)
            continue;

        parent_peer_caps = parent_gpu_peer_caps(parent_gpu, other_parent_gpu);

        // PCIe peers need to be explicitly destroyed via UvmDisablePeerAccess
        if (parent_peer_caps->ref_count == 0 || parent_peer_caps->link_type == UVM_GPU_LINK_PCIE)
            continue;

        for_each_gpu_in_parent(other_gpu, other_parent_gpu) {
            uvm_gpu_peer_t *peer_caps = gpu_peer_caps(gpu, other_gpu);

            if (peer_caps->ref_count == 0)
                continue;

            peers_release(gpu, other_gpu);
        }
    }
}

static NV_STATUS peers_discover_nvlink(uvm_gpu_t *gpu)
{
    uvm_parent_gpu_t *parent_gpu = gpu->parent;
    uvm_parent_gpu_t *other_parent_gpu;
    uvm_gpu_t *other_gpu;
    NV_STATUS status;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    for_each_parent_gpu(other_parent_gpu) {
        uvm_parent_gpu_peer_t *parent_peer_caps;

        if (other_parent_gpu == parent_gpu)
            continue;

        parent_peer_caps = parent_gpu_peer_caps(parent_gpu, other_parent_gpu);
        if (parent_peer_caps->ref_count == 0 || parent_peer_caps->link_type == UVM_GPU_LINK_PCIE)
            continue;

        for_each_gpu_in_parent(other_gpu, other_parent_gpu) {
            uvm_gpu_peer_t *peer_caps = gpu_peer_caps(gpu, other_gpu);

            UVM_ASSERT(peer_caps->ref_count == 0);
            status = peers_init(gpu, other_gpu, peer_caps);
            if (status != NV_OK)
                goto cleanup;
        }
    }

    return NV_OK;

cleanup:
    peers_destroy_nvlink(gpu);

    return status;
}

// Remove a gpu and unregister it from RM
// Note that this is also used in most error paths in add_gpu()
static void remove_gpu(uvm_gpu_t *gpu)
{
    NvU32 sub_processor_index;
    uvm_parent_gpu_t *parent_gpu;
    bool free_parent;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    sub_processor_index = uvm_id_sub_processor_index(gpu->id);
    parent_gpu = gpu->parent;

    UVM_ASSERT_MSG(uvm_gpu_retained_count(gpu) == 0,
                   "gpu_id %u retained_count %llu\n",
                   uvm_id_value(gpu->id),
                   uvm_gpu_retained_count(gpu));

    UVM_ASSERT(parent_gpu->num_retained_gpus > 0);
    parent_gpu->num_retained_gpus--;

    free_parent = (parent_gpu->num_retained_gpus == 0);

    // NVLINK peers must be removed and the relevant access counter buffers must
    // be flushed before removing this GPU from the global table.
    peers_destroy_nvlink(gpu);

    if (free_parent)
        parent_peers_destroy_nvlink(parent_gpu);

    // uvm_mem_free and other uvm_mem APIs invoked by the Confidential Compute
    // deinitialization must be called before the GPU is removed from the global
    // table.
    //
    // TODO: Bug 2008200: Add and remove the GPU in a more reasonable spot.
    uvm_conf_computing_gpu_deinit(gpu);

    // If the parent is not being freed, the following gpu_table_lock is only
    // needed to protect concurrent uvm_parent_gpu_find_first_valid_gpu() in BH
    // from the __clear_bit here.
    // In the free_parent case, gpu_table_lock protects the top half from the
    // uvm_global_remove_parent_gpu()
    uvm_spin_lock_irqsave(&g_uvm_global.gpu_table_lock);

    // Mark the GPU as invalid in the parent GPU's GPU table.
    __clear_bit(sub_processor_index, parent_gpu->valid_gpus);

    // Remove the GPU from the table.
    if (free_parent)
        uvm_global_remove_parent_gpu(parent_gpu);

    uvm_spin_unlock_irqrestore(&g_uvm_global.gpu_table_lock);

    uvm_processor_mask_clear(&g_uvm_global.retained_gpus, gpu->id);

    // If the parent is being freed, stop scheduling new bottom halves and
    // update relevant software state. Else flush any pending bottom halves
    // before continuing.
    if (free_parent)
        uvm_parent_gpu_disable_isr(parent_gpu);
    else
        uvm_parent_gpu_flush_bottom_halves(parent_gpu);

    deinit_gpu(gpu);

    UVM_ASSERT(parent_gpu->gpus[sub_processor_index] == gpu);
    parent_gpu->gpus[sub_processor_index] = NULL;
    uvm_kvfree(gpu);

    if (free_parent)
        deinit_parent_gpu(parent_gpu);
}

// Add a new gpu and register it with RM
static NV_STATUS add_gpu(const NvProcessorUuid *gpu_uuid,
                         const uvm_gpu_id_t gpu_id,
                         const UvmGpuInfo *gpu_info,
                         const UvmGpuPlatformInfo *gpu_platform_info,
                         uvm_parent_gpu_t *parent_gpu,
                         const uvm_test_parent_gpu_inject_error_t *parent_gpu_error,
                         uvm_gpu_t **gpu_out)
{
    NV_STATUS status;
    bool alloc_parent = (parent_gpu == NULL);
    uvm_gpu_t *gpu = NULL;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    if (alloc_parent) {
        status = alloc_parent_gpu(gpu_uuid, uvm_parent_gpu_id_from_gpu_id(gpu_id), &parent_gpu);
        if (status != NV_OK)
            return status;

        if (uvm_enable_builtin_tests)
            parent_gpu->test = *parent_gpu_error;
    }

    gpu = alloc_gpu(parent_gpu, gpu_id);
    if (!gpu) {
        if (alloc_parent)
            uvm_parent_gpu_kref_put(parent_gpu);

        return NV_ERR_NO_MEMORY;
    }

    parent_gpu->num_retained_gpus++;

    if (alloc_parent)
        fill_parent_gpu_info(parent_gpu, gpu_info);

    // After this point all error clean up should be handled by remove_gpu()

    if (!gpu_supports_uvm(parent_gpu)) {
        UVM_DBG_PRINT("Registration of non-UVM-capable GPU attempted: GPU %s\n", uvm_gpu_name(gpu));
        status = NV_ERR_NOT_SUPPORTED;
        goto error;
    }

    if (alloc_parent) {
        status = init_parent_gpu(parent_gpu, gpu_uuid, gpu_info, gpu_platform_info);
        if (status != NV_OK)
            goto error;
    }

    status = init_gpu(gpu, gpu_info);
    if (status != NV_OK)
        goto error;

    status = uvm_gpu_check_ecc_error(gpu);
    if (status != NV_OK)
        goto error;

    atomic64_set(&gpu->retained_count, 1);
    uvm_processor_mask_set(&g_uvm_global.retained_gpus, gpu->id);

    uvm_spin_lock_irqsave(&g_uvm_global.gpu_table_lock);

    if (alloc_parent)
        uvm_global_add_parent_gpu(parent_gpu);

    // Mark the GPU as valid in the parent GPU's GPU table.
    UVM_ASSERT(!test_bit(uvm_id_sub_processor_index(gpu->id), parent_gpu->valid_gpus));
    __set_bit(uvm_id_sub_processor_index(gpu->id), parent_gpu->valid_gpus);

    // Although locking correctness does not, at this early point (before the
    // GPU is visible in the table) strictly require holding the gpu_table_lock
    // in order to read gpu->isr.replayable_faults.handling, nor to enable page
    // fault interrupts (this could have been done earlier), it is best to do it
    // here, in order to avoid an interrupt storm. That way, we take advantage
    // of the spinlock_irqsave side effect of turning off local CPU interrupts,
    // part of holding the gpu_table_lock. That means that the local CPU won't
    // receive any of these interrupts, until the GPU is safely added to the
    // table (where the top half ISR can find it).
    //
    // As usual with spinlock_irqsave behavior, *other* CPUs can still handle
    // these interrupts, but the local CPU will not be slowed down (interrupted)
    // by such handling, and can quickly release the gpu_table_lock, thus
    // unblocking any other CPU's top half (which waits for the gpu_table_lock).
    if (alloc_parent && parent_gpu->isr.replayable_faults.handling) {
        parent_gpu->fault_buffer_hal->enable_replayable_faults(parent_gpu);

        // Clear the interrupt bit and force the re-evaluation of the interrupt
        // condition to ensure that we don't miss any pending interrupt
        parent_gpu->fault_buffer_hal->clear_replayable_faults(parent_gpu,
                                                              parent_gpu->fault_buffer.replayable.cached_get);
    }

    // Access counters are enabled on demand

    uvm_spin_unlock_irqrestore(&g_uvm_global.gpu_table_lock);

    if (alloc_parent) {
        status = parent_peers_discover_nvlink(parent_gpu);
        if (status != NV_OK)
            goto error_retained;
    }

    status = peers_discover_nvlink(gpu);
    if (status != NV_OK)
        goto error_retained;

    *gpu_out = gpu;

    return NV_OK;

error_retained:
    UVM_ERR_PRINT("Failed to discover NVLINK peers: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));

    // Nobody can have retained the GPU yet, since we still hold the
    // global lock.
    UVM_ASSERT(uvm_gpu_retained_count(gpu) == 1);
    atomic64_set(&gpu->retained_count, 0);
error:
    remove_gpu(gpu);

    return status;
}

// Increment the refcount for the GPU with the given UUID. If this is the first
// time that this UUID is retained, the GPU is added to UVM.
// When SMC partitioning is enabled, user_rm_device contains the user handles
// that were created by the caller, and that can be used to identify and
// obtain information about the partition. nvUvmInterfaceGetGpuInfo returns, in
// gpu_info, whether SMC is enabled, the swizzId, and GI UUID corresponding to
// the partition.
static NV_STATUS gpu_retain_by_uuid_locked(const NvProcessorUuid *gpu_uuid,
                                           const uvm_rm_user_object_t *user_rm_device,
                                           const uvm_test_parent_gpu_inject_error_t *parent_gpu_error,
                                           uvm_gpu_t **gpu_out)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu = NULL;
    uvm_parent_gpu_t *parent_gpu;
    UvmGpuInfo *gpu_info = NULL;
    UvmGpuClientInfo client_info = {0};
    UvmGpuPlatformInfo gpu_platform_info = {0};
    uvm_gpu_id_t gpu_id;

    client_info.hClient = user_rm_device->user_client;
    client_info.hSmcPartRef = user_rm_device->user_object;

    gpu_info = uvm_kvmalloc_zero(sizeof(*gpu_info));
    if (!gpu_info)
        return NV_ERR_NO_MEMORY;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    parent_gpu = uvm_parent_gpu_get_by_uuid_locked(gpu_uuid);

    if (parent_gpu == NULL) {
        // If this is the first time the UUID is seen, register it on RM
        status = uvm_rm_locked_call(nvUvmInterfaceRegisterGpu(gpu_uuid, &gpu_platform_info));
        if (status != NV_OK)
            goto error_free_gpu_info;
    }

    status = uvm_rm_locked_call(nvUvmInterfaceGetGpuInfo(gpu_uuid, &client_info, gpu_info));
    if (status != NV_OK)
        goto error_unregister;

    // TODO: Bug 5262806: Remove this WAR once the bug is fixed.
    if (gpu_info->accessCntrBufferCount > 1)
        gpu_info->accessCntrBufferCount = 1;

    if (parent_gpu != NULL) {
        // If the UUID has been seen before, and if SMC is enabled, then check
        // if this specific partition has been seen previously. The UUID-based
        // look-up above may have succeeded for a different partition with the
        // same parent GPU.
        if (gpu_info->smcEnabled) {
            gpu = uvm_gpu_get_by_parent_and_swizz_id(parent_gpu, gpu_info->smcSwizzId);
        }
        else {
            gpu = parent_gpu->gpus[0];
            UVM_ASSERT(gpu != NULL);
        }
    }

    if (gpu == NULL) {
        status = find_unused_gpu_id(parent_gpu, &gpu_id);
        if (status != NV_OK)
            goto error_unregister;

        status = add_gpu(gpu_uuid, gpu_id, gpu_info, &gpu_platform_info, parent_gpu, parent_gpu_error, &gpu);
        if (status != NV_OK)
            goto error_unregister;
    }
    else {
        atomic64_inc(&gpu->retained_count);
    }

    *gpu_out = gpu;

    uvm_kvfree(gpu_info);

    return status;

error_unregister:
    if (parent_gpu == NULL)
        uvm_rm_locked_call_void(nvUvmInterfaceUnregisterGpu(gpu_uuid));
error_free_gpu_info:
    uvm_kvfree(gpu_info);

    return status;
}

NV_STATUS uvm_gpu_retain_by_uuid(const NvProcessorUuid *gpu_uuid,
                                 const uvm_rm_user_object_t *user_rm_device,
                                 const uvm_test_parent_gpu_inject_error_t *parent_gpu_error,
                                 uvm_gpu_t **gpu_out)
{
    NV_STATUS status;
    uvm_mutex_lock(&g_uvm_global.global_lock);
    status = gpu_retain_by_uuid_locked(gpu_uuid, user_rm_device, parent_gpu_error, gpu_out);
    uvm_mutex_unlock(&g_uvm_global.global_lock);
    return status;
}

void uvm_gpu_release_locked(uvm_gpu_t *gpu)
{
    uvm_parent_gpu_t *parent_gpu = gpu->parent;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    UVM_ASSERT(uvm_gpu_retained_count(gpu) > 0);

    if (atomic64_dec_and_test(&gpu->retained_count)) {
        nv_kref_get(&parent_gpu->gpu_kref);
        remove_gpu(gpu);
        if (parent_gpu->num_retained_gpus == 0)
            uvm_rm_locked_call_void(nvUvmInterfaceUnregisterGpu(&parent_gpu->uuid));
        uvm_parent_gpu_kref_put(parent_gpu);
    }
}

void uvm_gpu_release(uvm_gpu_t *gpu)
{
    uvm_mutex_lock(&g_uvm_global.global_lock);
    uvm_gpu_release_locked(gpu);
    uvm_mutex_unlock(&g_uvm_global.global_lock);
}

NV_STATUS uvm_gpu_retain_pcie_peer_access(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    NV_STATUS status;

    UVM_ASSERT(gpu0);
    UVM_ASSERT(gpu1);
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    status = peers_retain(gpu0, gpu1);
    if (status != NV_OK)
        return status;

    if (uvm_parent_gpu_peer_link_type(gpu0->parent, gpu1->parent) != UVM_GPU_LINK_PCIE) {
        peers_release(gpu0, gpu1);
        return NV_ERR_INVALID_DEVICE;
    }

    // GPUs can't be destroyed until their peer pairings have also been
    // destroyed.
    uvm_gpu_retain(gpu0);
    uvm_gpu_retain(gpu1);

    return NV_OK;
}

void uvm_gpu_release_pcie_peer_access(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    UVM_ASSERT(gpu0);
    UVM_ASSERT(gpu1);
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    UVM_ASSERT(uvm_parent_gpu_peer_link_type(gpu0->parent, gpu1->parent) == UVM_GPU_LINK_PCIE);

    peers_release(gpu0, gpu1);

    uvm_gpu_release_locked(gpu0);
    uvm_gpu_release_locked(gpu1);
}

uvm_gpu_link_type_t uvm_parent_gpu_peer_link_type(uvm_parent_gpu_t *parent_gpu0, uvm_parent_gpu_t *parent_gpu1)
{
    uvm_parent_gpu_peer_t *parent_peer_caps;

    if (parent_gpu0 == parent_gpu1)
        return UVM_GPU_LINK_INVALID;

    parent_peer_caps = parent_gpu_peer_caps(parent_gpu0, parent_gpu1);
    if (parent_peer_caps->ref_count == 0)
        return UVM_GPU_LINK_INVALID;

    return parent_peer_caps->link_type;
}

uvm_aperture_t uvm_gpu_peer_aperture(uvm_gpu_t *local_gpu, uvm_gpu_t *remote_gpu)
{
    uvm_parent_gpu_peer_t *parent_peer_caps;

    // MIG instances in the same physical GPU have vidmem addresses
    if (uvm_gpus_are_smc_peers(local_gpu, remote_gpu))
        return UVM_APERTURE_VID;

    parent_peer_caps = parent_gpu_peer_caps(local_gpu->parent, remote_gpu->parent);
    return parent_gpu_peer_aperture(local_gpu->parent, remote_gpu->parent, parent_peer_caps);
}

uvm_gpu_phys_address_t uvm_gpu_peer_phys_address(uvm_gpu_t *owning_gpu, NvU64 address, uvm_gpu_t *accessing_gpu)
{
    uvm_aperture_t aperture = uvm_gpu_peer_aperture(accessing_gpu, owning_gpu);

    if (uvm_parent_gpus_are_nvswitch_connected(accessing_gpu->parent, owning_gpu->parent))
        address += owning_gpu->parent->nvswitch_info.fabric_memory_window_start;

    return uvm_gpu_phys_address(aperture, address);
}

uvm_gpu_address_t uvm_gpu_peer_copy_address(uvm_gpu_t *owning_gpu, NvU64 address, uvm_gpu_t *accessing_gpu)
{
    uvm_gpu_identity_mapping_t *gpu_peer_mapping;

    if (accessing_gpu->parent->peer_copy_mode == UVM_GPU_PEER_COPY_MODE_PHYSICAL)
        return uvm_gpu_address_from_phys(uvm_gpu_peer_phys_address(owning_gpu, address, accessing_gpu));

    UVM_ASSERT(accessing_gpu->parent->peer_copy_mode == UVM_GPU_PEER_COPY_MODE_VIRTUAL);
    gpu_peer_mapping = uvm_gpu_get_peer_mapping(accessing_gpu, owning_gpu->id);

    return uvm_gpu_address_virtual(gpu_peer_mapping->base + address);
}

uvm_aperture_t uvm_gpu_egm_peer_aperture(uvm_parent_gpu_t *local_gpu, uvm_parent_gpu_t *remote_gpu)
{
    uvm_parent_gpu_peer_t *peer_caps;
    NvU8 peer_id;

    if (local_gpu == remote_gpu) {
        UVM_ASSERT(local_gpu->egm.enabled);
        peer_id = local_gpu->egm.local_peer_id;
    }
    else {
        UVM_ASSERT(remote_gpu->egm.enabled);

        peer_caps = parent_gpu_peer_caps(local_gpu, remote_gpu);

        // Comparison is based on min(gpu1, gpu2) logic for peer GPUs.
        if (uvm_parent_id_cmp(local_gpu->id, remote_gpu->id) < 0)
            peer_id = peer_caps->egm_peer_ids[0];
        else
            peer_id = peer_caps->egm_peer_ids[1];
    }

    return UVM_APERTURE_PEER(peer_id);
}

NvU64 uvm_gpu_peer_ref_count(const uvm_gpu_t *gpu0, const uvm_gpu_t *gpu1)
{
    UVM_ASSERT(!uvm_gpus_are_smc_peers(gpu0, gpu1));

    return gpu_peer_caps(gpu0, gpu1)->ref_count;
}

bool uvm_gpu_address_is_peer(uvm_gpu_t *gpu, uvm_gpu_address_t address)
{
    if (address.is_virtual) {
        // Virtual addresses can be used for peer copies only if
        // peer_copy_mode == UVM_GPU_PEER_COPY_MODE_VIRTUAL
        if (gpu->parent->peer_copy_mode == UVM_GPU_PEER_COPY_MODE_VIRTUAL) {
            return (address.address >= gpu->parent->peer_va_base &&
                    address.address < (gpu->parent->peer_va_base + gpu->parent->peer_va_size));
        }
    }
    else {
        uvm_parent_gpu_t *parent_gpu;
        phys_addr_t phys_addr;

        if (uvm_aperture_is_peer(address.aperture)) {
            uvm_parent_processor_mask_t parent_gpus;
            uvm_parent_gpu_t *parent_peer_gpu;

            // Local EGM accesses don't go over NVLINK
            if (gpu->parent->egm.enabled && address.aperture == gpu->parent->egm.local_peer_id)
                return false;

            uvm_spin_lock(&gpu->peer_info.peer_gpu_lock);
            uvm_parent_gpus_from_processor_mask(&parent_gpus, &gpu->peer_info.peer_gpu_mask);
            for_each_parent_gpu_in_mask(parent_peer_gpu, &parent_gpus) {
                if (!parent_peer_gpu->egm.enabled)
                    continue;

                // EGM uses peer IDs but they are different from VIDMEM peer
                // IDs.
                // Check if the address aperture is an EGM aperture.
                // We should not use remote EGM addresses internally until
                // NVLINK STO handling is updated to handle EGM.
                // TODO: Bug: 5068688 [UVM] Detect STO and prevent data leaks
                //                    when accessing EGM memory
                // TODO: Bug: 5007527 [UVM] Extend STO recovery to EGM enabled
                //                    systems
                UVM_ASSERT(address.aperture != uvm_gpu_egm_peer_aperture(gpu->parent, parent_peer_gpu));
            }

            uvm_spin_unlock(&gpu->peer_info.peer_gpu_lock);

            return true;
        } else if (address.aperture == UVM_APERTURE_SYS) {
            bool is_peer = false;

            // GPU uses DMA addresses, which might be translated by IOMMU/SMMU,
            // either inline, or via ATS.
            phys_addr = dma_to_phys(&gpu->parent->pci_dev->dev, (dma_addr_t)address.address);

            // Exposed coherent vidmem can be accessed via sys aperture
            uvm_spin_lock_irqsave(&g_uvm_global.gpu_table_lock);
            for_each_parent_gpu(parent_gpu) {
                if (parent_gpu == gpu->parent)
                    continue;

                if (phys_addr >= parent_gpu->system_bus.memory_window_start &&
                    phys_addr <= parent_gpu->system_bus.memory_window_end) {
                    is_peer = true;
                }
            }
            uvm_spin_unlock_irqrestore(&g_uvm_global.gpu_table_lock);
            return is_peer;
        }

        UVM_ASSERT(address.aperture == UVM_APERTURE_VID);
    }

    return false;
}

uvm_aperture_t uvm_get_page_tree_location(const uvm_gpu_t *gpu)
{
    // See comments in page_tree_set_location
    if (uvm_parent_gpu_is_virt_mode_sriov_heavy(gpu->parent) || g_uvm_global.conf_computing_enabled)
        return UVM_APERTURE_VID;

    if (!gpu->mem_info.size)
        return UVM_APERTURE_SYS;


    return UVM_APERTURE_DEFAULT;
}

static NvU64 instance_ptr_to_key(uvm_gpu_phys_address_t instance_ptr)
{
    NvU64 key;
    int is_sys = (instance_ptr.aperture == UVM_APERTURE_SYS);

    // Instance pointers must be 4k aligned and they must have either VID or SYS
    // apertures. Compress them as much as we can both to guarantee that the key
    // fits within 64 bits, and to make the key space as small as possible.
    UVM_ASSERT(IS_ALIGNED(instance_ptr.address, UVM_PAGE_SIZE_4K));
    UVM_ASSERT(instance_ptr.aperture == UVM_APERTURE_VID || instance_ptr.aperture == UVM_APERTURE_SYS);

    key = (instance_ptr.address >> 11) | is_sys;

    return key;
}

static NV_STATUS parent_gpu_add_user_channel_subctx_info(uvm_parent_gpu_t *parent_gpu,
                                                         uvm_user_channel_t *user_channel)
{
    uvm_gpu_phys_address_t instance_ptr = user_channel->instance_ptr.addr;
    NV_STATUS status = NV_OK;
    uvm_rb_tree_node_t *channel_tree_node;
    NvU64 node_key;
    uvm_user_channel_subctx_info_t *channel_subctx_info;
    uvm_user_channel_subctx_info_t *new_channel_subctx_info = NULL;
    uvm_gpu_va_space_t *gpu_va_space = user_channel->gpu_va_space;

    if (!user_channel->in_subctx)
        return NV_OK;

    // Pre-allocate a subcontext info descriptor out of the lock, in case we
    // need to add a new entry to the tree
    new_channel_subctx_info = uvm_kvmalloc_zero(sizeof(*new_channel_subctx_info));

    // Don't check for the result of the allocation since it is only needed
    // if the TSG has not been registered yet, and we do that under the lock
    // below
    if (new_channel_subctx_info) {
        new_channel_subctx_info->subctxs =
            uvm_kvmalloc_zero(sizeof(*new_channel_subctx_info->subctxs) * user_channel->tsg.max_subctx_count);
    }

    uvm_spin_lock(&parent_gpu->instance_ptr_table_lock);

    // Check if the subcontext information for the channel already exists
    node_key = ((NvU64)user_channel->hw_runlist_id << 32) | user_channel->tsg.id;
    channel_tree_node = uvm_rb_tree_find(&parent_gpu->tsg_table, node_key);

    if (!channel_tree_node) {
        // We could not allocate the descriptor before taking the lock. Exiting
        if (!new_channel_subctx_info || !new_channel_subctx_info->subctxs) {
            status = NV_ERR_NO_MEMORY;
            goto exit_unlock;
        }

        // Insert the new subcontext information descriptor
        new_channel_subctx_info->node.key = node_key;
        status = uvm_rb_tree_insert(&parent_gpu->tsg_table, &new_channel_subctx_info->node);
        UVM_ASSERT(status == NV_OK);

        channel_subctx_info = new_channel_subctx_info;
        channel_subctx_info->smc_engine_id = user_channel->smc_engine_id;
    }
    else {
        channel_subctx_info = container_of(channel_tree_node, uvm_user_channel_subctx_info_t, node);
        UVM_ASSERT(channel_subctx_info->smc_engine_id == user_channel->smc_engine_id);
    }

    user_channel->subctx_info = channel_subctx_info;

    // Register the GPU VA space of the channel subcontext info descriptor, or
    // check that the existing one matches the channel's
    if (channel_subctx_info->subctxs[user_channel->subctx_id].refcount++ > 0) {
        UVM_ASSERT_MSG(channel_subctx_info->subctxs[user_channel->subctx_id].gpu_va_space == gpu_va_space,
                       "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: expected GPU VA space 0x%llx but got 0x%llx instead\n",
                       user_channel->hw_runlist_id,
                       user_channel->hw_channel_id,
                       instance_ptr.address,
                       uvm_aperture_string(instance_ptr.aperture),
                       user_channel->subctx_id,
                       user_channel->tsg.id,
                       (NvU64)gpu_va_space,
                       (NvU64)channel_subctx_info->subctxs[user_channel->subctx_id].gpu_va_space);
        UVM_ASSERT_MSG(channel_subctx_info->subctxs[user_channel->subctx_id].gpu_va_space != NULL,
                       "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: GPU VA space is NULL\n",
                       user_channel->hw_runlist_id,
                       user_channel->hw_channel_id,
                       instance_ptr.address,
                       uvm_aperture_string(instance_ptr.aperture),
                       user_channel->subctx_id,
                       user_channel->tsg.id);
        UVM_ASSERT_MSG(channel_subctx_info->total_refcount > 0,
                       "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: TSG refcount is 0\n",
                       user_channel->hw_runlist_id,
                       user_channel->hw_channel_id,
                       instance_ptr.address,
                       uvm_aperture_string(instance_ptr.aperture),
                       user_channel->subctx_id,
                       user_channel->tsg.id);
    }
    else {
        UVM_ASSERT_MSG(channel_subctx_info->subctxs[user_channel->subctx_id].gpu_va_space == NULL,
                       "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: expected GPU VA space NULL but got 0x%llx instead\n",
                       user_channel->hw_runlist_id,
                       user_channel->hw_channel_id,
                       instance_ptr.address,
                       uvm_aperture_string(instance_ptr.aperture),
                       user_channel->subctx_id,
                       user_channel->tsg.id,
                       (NvU64)channel_subctx_info->subctxs[user_channel->subctx_id].gpu_va_space);

        channel_subctx_info->subctxs[user_channel->subctx_id].gpu_va_space = gpu_va_space;
    }

    ++channel_subctx_info->total_refcount;

exit_unlock:
    uvm_spin_unlock(&parent_gpu->instance_ptr_table_lock);

    // Remove the pre-allocated per-TSG subctx information struct if there was
    // some error or it was not used
    if (status != NV_OK || user_channel->subctx_info != new_channel_subctx_info) {
        if (new_channel_subctx_info)
            uvm_kvfree(new_channel_subctx_info->subctxs);

        uvm_kvfree(new_channel_subctx_info);
    }

    return status;
}

static void parent_gpu_remove_user_channel_subctx_info_locked(uvm_parent_gpu_t *parent_gpu,
                                                              uvm_user_channel_t *user_channel)
{
    uvm_gpu_phys_address_t instance_ptr = user_channel->instance_ptr.addr;
    uvm_gpu_va_space_t *gpu_va_space = user_channel->gpu_va_space;

    uvm_assert_spinlock_locked(&parent_gpu->instance_ptr_table_lock);

    // Channel subcontext info descriptor may not have been registered in
    // tsg_table since this function is called in some teardown paths during
    // channel creation
    if (!user_channel->subctx_info)
        return;

    UVM_ASSERT_MSG(&user_channel->subctx_info->node ==
                   uvm_rb_tree_find(&parent_gpu->tsg_table, user_channel->subctx_info->node.key),
                   "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: SubCTX not found in TSG table\n",
                   user_channel->hw_runlist_id,
                   user_channel->hw_channel_id,
                   instance_ptr.address,
                   uvm_aperture_string(instance_ptr.aperture),
                   user_channel->subctx_id,
                   user_channel->tsg.id);

    UVM_ASSERT_MSG(user_channel->subctx_info->subctxs[user_channel->subctx_id].refcount > 0,
                   "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: SubCTX refcount is 0\n",
                   user_channel->hw_runlist_id,
                   user_channel->hw_channel_id,
                   instance_ptr.address,
                   uvm_aperture_string(instance_ptr.aperture),
                   user_channel->subctx_id,
                   user_channel->tsg.id);

    UVM_ASSERT_MSG(user_channel->subctx_info->subctxs[user_channel->subctx_id].gpu_va_space == gpu_va_space,
                   "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: "
                   "expected GPU VA space 0x%llx but got 0x%llx instead\n",
                   user_channel->hw_runlist_id,
                   user_channel->hw_channel_id,
                   instance_ptr.address,
                   uvm_aperture_string(instance_ptr.aperture),
                   user_channel->subctx_id,
                   user_channel->tsg.id,
                   (NvU64)gpu_va_space,
                   (NvU64)user_channel->subctx_info->subctxs[user_channel->subctx_id].gpu_va_space);

    UVM_ASSERT_MSG(user_channel->subctx_info->total_refcount > 0,
                   "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: TSG refcount is 0\n",
                   user_channel->hw_runlist_id,
                   user_channel->hw_channel_id,
                   instance_ptr.address,
                   uvm_aperture_string(instance_ptr.aperture),
                   user_channel->subctx_id,
                   user_channel->tsg.id);

    // Decrement VA space refcount. If it gets to zero, unregister the pointer
    if (--user_channel->subctx_info->subctxs[user_channel->subctx_id].refcount == 0)
        user_channel->subctx_info->subctxs[user_channel->subctx_id].gpu_va_space = NULL;

    if (--user_channel->subctx_info->total_refcount == 0) {
        uvm_rb_tree_remove(&parent_gpu->tsg_table, &user_channel->subctx_info->node);
        uvm_kvfree(user_channel->subctx_info->subctxs);
        uvm_kvfree(user_channel->subctx_info);
    }

    user_channel->subctx_info = NULL;
}

static void parent_gpu_add_user_channel_instance_ptr(uvm_parent_gpu_t *parent_gpu,
                                                     uvm_user_channel_t *user_channel)
{
    uvm_gpu_phys_address_t instance_ptr = user_channel->instance_ptr.addr;
    NvU64 instance_ptr_key = instance_ptr_to_key(instance_ptr);
    NV_STATUS status;

    uvm_spin_lock(&parent_gpu->instance_ptr_table_lock);

    // Insert the instance_ptr -> user_channel mapping
    user_channel->instance_ptr.node.key = instance_ptr_key;
    status = uvm_rb_tree_insert(&parent_gpu->instance_ptr_table, &user_channel->instance_ptr.node);

    uvm_spin_unlock(&parent_gpu->instance_ptr_table_lock);

    UVM_ASSERT_MSG(status == NV_OK, "CH %u:%u instance_ptr {0x%llx:%s} SubCTX %u in TSG %u: error %s\n",
                   user_channel->hw_runlist_id,
                   user_channel->hw_channel_id,
                   instance_ptr.address,
                   uvm_aperture_string(instance_ptr.aperture),
                   user_channel->subctx_id,
                   user_channel->tsg.id,
                   nvstatusToString(status));
}

static void parent_gpu_remove_user_channel_instance_ptr_locked(uvm_parent_gpu_t *parent_gpu,
                                                               uvm_user_channel_t *user_channel)
{
    uvm_assert_spinlock_locked(&parent_gpu->instance_ptr_table_lock);

    if (UVM_RB_TREE_EMPTY_NODE(&user_channel->instance_ptr.node))
        return;

    uvm_rb_tree_remove(&parent_gpu->instance_ptr_table, &user_channel->instance_ptr.node);
}

NV_STATUS uvm_parent_gpu_add_user_channel(uvm_parent_gpu_t *parent_gpu, uvm_user_channel_t *user_channel)
{
    uvm_va_space_t *va_space;
    uvm_gpu_va_space_t *gpu_va_space = user_channel->gpu_va_space;
    NV_STATUS status;

    UVM_ASSERT(user_channel->rm_retained_channel);
    UVM_ASSERT(gpu_va_space);
    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);
    va_space = gpu_va_space->va_space;
    uvm_assert_rwsem_locked(&va_space->lock);

    status = parent_gpu_add_user_channel_subctx_info(parent_gpu, user_channel);
    if (status != NV_OK)
        return status;

    parent_gpu_add_user_channel_instance_ptr(parent_gpu, user_channel);

    return NV_OK;
}

static uvm_user_channel_t *instance_ptr_to_user_channel(uvm_parent_gpu_t *parent_gpu,
                                                        uvm_gpu_phys_address_t instance_ptr)
{
    NvU64 key = instance_ptr_to_key(instance_ptr);
    uvm_rb_tree_node_t *instance_node;

    uvm_assert_spinlock_locked(&parent_gpu->instance_ptr_table_lock);

    instance_node = uvm_rb_tree_find(&parent_gpu->instance_ptr_table, key);
    if (!instance_node)
        return NULL;

    return get_user_channel(instance_node);
}

static uvm_gpu_va_space_t *user_channel_and_subctx_to_gpu_va_space(uvm_user_channel_t *user_channel, NvU32 subctx_id)
{
    uvm_user_channel_subctx_info_t *channel_subctx_info;

    UVM_ASSERT(user_channel);
    UVM_ASSERT(user_channel->in_subctx);
    UVM_ASSERT(user_channel->subctx_info);

    uvm_assert_spinlock_locked(&user_channel->gpu->parent->instance_ptr_table_lock);

    channel_subctx_info = user_channel->subctx_info;

    UVM_ASSERT_MSG(subctx_id < user_channel->tsg.max_subctx_count,
                   "instance_ptr {0x%llx:%s} in TSG %u. Invalid SubCTX %u\n",
                   user_channel->instance_ptr.addr.address,
                   uvm_aperture_string(user_channel->instance_ptr.addr.aperture),
                   user_channel->tsg.id,
                   subctx_id);
    UVM_ASSERT_MSG(channel_subctx_info->total_refcount > 0,
                   "instance_ptr {0x%llx:%s} in TSG %u: TSG refcount is 0\n",
                   user_channel->instance_ptr.addr.address,
                   uvm_aperture_string(user_channel->instance_ptr.addr.aperture),
                   user_channel->tsg.id);

    // A subcontext's refcount can be zero if that subcontext is torn down
    // uncleanly and work from that subcontext continues running with work from
    // other subcontexts.
    if (channel_subctx_info->subctxs[subctx_id].refcount == 0) {
        UVM_ASSERT(channel_subctx_info->subctxs[subctx_id].gpu_va_space == NULL);
    }
    else {
        UVM_ASSERT_MSG(channel_subctx_info->subctxs[subctx_id].gpu_va_space,
                       "instance_ptr {0x%llx:%s} in TSG %u: no GPU VA space for SubCTX %u\n",
                       user_channel->instance_ptr.addr.address,
                       uvm_aperture_string(user_channel->instance_ptr.addr.aperture),
                       user_channel->tsg.id,
                       subctx_id);
    }

    return channel_subctx_info->subctxs[subctx_id].gpu_va_space;
}

NV_STATUS uvm_parent_gpu_fault_entry_to_va_space(uvm_parent_gpu_t *parent_gpu,
                                                 const uvm_fault_buffer_entry_t *fault,
                                                 uvm_va_space_t **out_va_space,
                                                 uvm_gpu_t **out_gpu)
{
    uvm_user_channel_t *user_channel;
    uvm_gpu_va_space_t *gpu_va_space;
    NV_STATUS status = NV_OK;

    *out_va_space = NULL;
    *out_gpu = NULL;

    uvm_spin_lock(&parent_gpu->instance_ptr_table_lock);

    user_channel = instance_ptr_to_user_channel(parent_gpu, fault->instance_ptr);
    if (!user_channel) {
        status = NV_ERR_INVALID_CHANNEL;
        goto exit_unlock;
    }

    // Faults from HUB clients will always report VEID 0 even if the channel
    // belongs a TSG with many subcontexts. Therefore, we cannot use the per-TSG
    // subctx table and we need to directly return the channel's VA space
    if (!user_channel->in_subctx || (fault->fault_source.client_type == UVM_FAULT_CLIENT_TYPE_HUB)) {
        UVM_ASSERT_MSG(fault->fault_source.ve_id == 0,
                       "Fault packet contains SubCTX %u for channel not in subctx\n",
                       fault->fault_source.ve_id);

        // We can safely access user_channel->gpu_va_space under the
        // instance_ptr_table_lock since gpu_va_space is set to NULL after this
        // function is called in uvm_user_channel_detach
        gpu_va_space = user_channel->gpu_va_space;
        UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);
        *out_va_space = gpu_va_space->va_space;
        *out_gpu = gpu_va_space->gpu;
    }
    else {
        NvU32 ve_id = fault->fault_source.ve_id;

        // Compute the SMC engine-local VEID
        UVM_ASSERT(ve_id >= user_channel->smc_engine_ve_id_offset);

        ve_id -= user_channel->smc_engine_ve_id_offset;

        gpu_va_space = user_channel_and_subctx_to_gpu_va_space(user_channel, ve_id);

        // Instance pointer is valid but the fault targets a non-existent
        // subcontext.
        if (gpu_va_space) {
            *out_va_space = gpu_va_space->va_space;
            *out_gpu = gpu_va_space->gpu;
        }
        else {
            status = NV_ERR_PAGE_TABLE_NOT_AVAIL;
        }
    }

exit_unlock:
    uvm_spin_unlock(&parent_gpu->instance_ptr_table_lock);

    return status;
}

NV_STATUS uvm_parent_gpu_access_counter_entry_to_va_space(uvm_parent_gpu_t *parent_gpu,
                                                          const uvm_access_counter_buffer_entry_t *entry,
                                                          uvm_va_space_t **out_va_space,
                                                          uvm_gpu_t **out_gpu)
{
    uvm_user_channel_t *user_channel;
    uvm_gpu_va_space_t *gpu_va_space;
    NV_STATUS status = NV_OK;

    *out_va_space = NULL;
    *out_gpu = NULL;

    uvm_spin_lock(&parent_gpu->instance_ptr_table_lock);

    user_channel = instance_ptr_to_user_channel(parent_gpu, entry->instance_ptr);
    if (!user_channel) {
        status = NV_ERR_INVALID_CHANNEL;
        goto exit_unlock;
    }

    if (!user_channel->in_subctx) {
        UVM_ASSERT_MSG(entry->ve_id == 0,
                       "Access counter packet contains SubCTX %u for channel not in subctx\n",
                       entry->ve_id);

        gpu_va_space = user_channel->gpu_va_space;
        UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);
        *out_va_space = gpu_va_space->va_space;
        *out_gpu = gpu_va_space->gpu;
    }
    else {
        gpu_va_space = user_channel_and_subctx_to_gpu_va_space(user_channel, entry->ve_id);
        if (gpu_va_space) {
            *out_va_space = gpu_va_space->va_space;
            *out_gpu = gpu_va_space->gpu;
        }
        else {
            status = NV_ERR_PAGE_TABLE_NOT_AVAIL;
        }
    }

exit_unlock:
    uvm_spin_unlock(&parent_gpu->instance_ptr_table_lock);

    return status;
}

void uvm_parent_gpu_remove_user_channel(uvm_parent_gpu_t *parent_gpu, uvm_user_channel_t *user_channel)
{
    uvm_va_space_t *va_space;
    uvm_gpu_va_space_t *gpu_va_space = user_channel->gpu_va_space;

    UVM_ASSERT(user_channel->rm_retained_channel);
    UVM_ASSERT(gpu_va_space);
    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);
    va_space = gpu_va_space->va_space;
    uvm_assert_rwsem_locked_write(&va_space->lock);

    uvm_spin_lock(&parent_gpu->instance_ptr_table_lock);
    parent_gpu_remove_user_channel_subctx_info_locked(parent_gpu, user_channel);
    parent_gpu_remove_user_channel_instance_ptr_locked(parent_gpu, user_channel);
    uvm_spin_unlock(&parent_gpu->instance_ptr_table_lock);
}

static NvU64 gpu_addr_to_dma_addr(uvm_parent_gpu_t *parent_gpu, NvU64 gpu_addr)
{
    NvU64 dma_addr = gpu_addr;
    UVM_ASSERT(dma_addr <= dma_addr + parent_gpu->dma_addressable_start);

    dma_addr += parent_gpu->dma_addressable_start;

    return dma_addr;
}

// The GPU has its NV_PFB_XV_UPPER_ADDR register set by RM to
// dma_addressable_start (in bifSetupDmaWindow_IMPL()) and hence when
// referencing sysmem from the GPU, dma_addressable_start should be
// subtracted from the DMA address we get from the OS.
NvU64 uvm_parent_gpu_dma_addr_to_gpu_addr(uvm_parent_gpu_t *parent_gpu, NvU64 dma_addr)
{
    NvU64 gpu_addr = dma_addr - parent_gpu->dma_addressable_start;
    UVM_ASSERT(dma_addr >= gpu_addr);

    return gpu_addr;
}

static void *parent_gpu_dma_alloc_page(uvm_parent_gpu_t *parent_gpu, gfp_t gfp_flags, NvU64 *dma_address_out)
{
    NvU64 dma_addr;
    void *cpu_addr;

    cpu_addr = dma_alloc_coherent(&parent_gpu->pci_dev->dev, PAGE_SIZE, &dma_addr, gfp_flags);
    if (!cpu_addr)
        return cpu_addr;

    *dma_address_out = uvm_parent_gpu_dma_addr_to_gpu_addr(parent_gpu, dma_addr);
    atomic64_add(PAGE_SIZE, &parent_gpu->mapped_cpu_pages_size);
    return cpu_addr;
}

NV_STATUS uvm_gpu_dma_alloc_page(uvm_gpu_t *gpu, gfp_t gfp_flags, void **cpu_addr_out, NvU64 *dma_address_out)
{
    void *cpu_addr = parent_gpu_dma_alloc_page(gpu->parent, gfp_flags, dma_address_out);
    if (!cpu_addr)
        return NV_ERR_NO_MEMORY;

    // TODO: Bug 4868590: Issue GPA invalidate here

    *cpu_addr_out = cpu_addr;
    return NV_OK;
}

void uvm_parent_gpu_dma_free_page(uvm_parent_gpu_t *parent_gpu, void *cpu_addr, NvU64 dma_address)
{
    dma_address = gpu_addr_to_dma_addr(parent_gpu, dma_address);
    dma_free_coherent(&parent_gpu->pci_dev->dev, PAGE_SIZE, cpu_addr, dma_address);
    atomic64_sub(PAGE_SIZE, &parent_gpu->mapped_cpu_pages_size);
}

static NV_STATUS parent_gpu_map_cpu_pages(uvm_parent_gpu_t *parent_gpu, struct page *page, size_t size, NvU64 *dma_address_out)
{
    NvU64 dma_addr;

    UVM_ASSERT(PAGE_ALIGNED(size));

    dma_addr = dma_map_page(&parent_gpu->pci_dev->dev, page, 0, size, DMA_BIDIRECTIONAL);
    if (dma_mapping_error(&parent_gpu->pci_dev->dev, dma_addr))
        return NV_ERR_OPERATING_SYSTEM;

    if (dma_addr < parent_gpu->dma_addressable_start ||
        dma_addr + size - 1 > parent_gpu->dma_addressable_limit) {
        dma_unmap_page(&parent_gpu->pci_dev->dev, dma_addr, size, DMA_BIDIRECTIONAL);
        UVM_ERR_PRINT_RL("PCI mapped range [0x%llx, 0x%llx) not in the addressable range [0x%llx, 0x%llx), GPU %s\n",
                         dma_addr,
                         dma_addr + (NvU64)size,
                         parent_gpu->dma_addressable_start,
                         parent_gpu->dma_addressable_limit + 1,
                         uvm_parent_gpu_name(parent_gpu));
        return NV_ERR_INVALID_ADDRESS;
    }

    atomic64_add(size, &parent_gpu->mapped_cpu_pages_size);
    *dma_address_out = uvm_parent_gpu_dma_addr_to_gpu_addr(parent_gpu, dma_addr);

    return NV_OK;
}

NV_STATUS uvm_gpu_map_cpu_pages(uvm_gpu_t *gpu, struct page *page, size_t size, NvU64 *dma_address_out)
{
    NV_STATUS status = parent_gpu_map_cpu_pages(gpu->parent, page, size, dma_address_out);

    // TODO: Bug 4868590: Issue GPA invalidate here

    return status;
}

void uvm_parent_gpu_unmap_cpu_pages(uvm_parent_gpu_t *parent_gpu, NvU64 dma_address, size_t size)
{
    UVM_ASSERT(PAGE_ALIGNED(size));

    dma_address = gpu_addr_to_dma_addr(parent_gpu, dma_address);
    dma_unmap_page(&parent_gpu->pci_dev->dev, dma_address, size, DMA_BIDIRECTIONAL);
    atomic64_sub(size, &parent_gpu->mapped_cpu_pages_size);
}

// This function implements the UvmRegisterGpu API call, as described in uvm.h.
// Notes:
//
// 1. The UVM VA space has a 1-to-1 relationship with an open instance of
// /dev/nvidia-uvm. That, in turn, has a 1-to-1 relationship with a process,
// because the user-level UVM code (os-user-linux.c, for example) enforces an
// "open /dev/nvidia-uvm only once per process" policy. So a UVM VA space is
// very close to a process's VA space.
//
// If that user space code fails or is not used, then the relationship is no
// longer 1-to-1. That situation requires that this code should avoid crashing,
// leaking resources, exhibiting security holes, etc, but it does not have to
// provide correct UVM API behavior. Correct UVM API behavior requires doing
// the right things in user space before calling into the kernel.
//
// 2. The uvm_api*() routines are invoked directly from the top-level ioctl
// handler. They are considered "API routing routines", because they are
// responsible for providing the behavior that is described in the UVM
// user-to-kernel API documentation, in uvm.h.
//
// 3. A GPU VA space, which you'll see in other parts of the driver,
// is something different: there may be more than one
// GPU VA space within a process, and therefore within a UVM VA space.
//
NV_STATUS uvm_api_register_gpu(UVM_REGISTER_GPU_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_rm_user_object_t user_rm_va_space = {
        .rm_control_fd = params->rmCtrlFd,
        .user_client   = params->hClient,
        .user_object   = params->hSmcPartRef,
    };
    NvProcessorUuid gpu_instance_uuid;
    NV_STATUS status;

    status = uvm_va_space_register_gpu(va_space,
                                       &params->gpu_uuid,
                                       &user_rm_va_space,
                                       &params->numaEnabled,
                                       &params->numaNodeId,
                                       &gpu_instance_uuid);
    if (status == NV_OK)
        uvm_uuid_copy(&params->gpu_uuid, &gpu_instance_uuid);

    return status;
}

NV_STATUS uvm_api_unregister_gpu(UVM_UNREGISTER_GPU_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    return uvm_va_space_unregister_gpu(va_space, &params->gpu_uuid);
}

NV_STATUS uvm_api_register_gpu_va_space(UVM_REGISTER_GPU_VASPACE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_rm_user_object_t user_rm_va_space = {
        .rm_control_fd = params->rmCtrlFd,
        .user_client   = params->hClient,
        .user_object   = params->hVaSpace
    };
    return uvm_va_space_register_gpu_va_space(va_space, &user_rm_va_space, &params->gpuUuid);
}

NV_STATUS uvm_api_unregister_gpu_va_space(UVM_UNREGISTER_GPU_VASPACE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    return uvm_va_space_unregister_gpu_va_space(va_space, &params->gpuUuid);
}

NV_STATUS uvm_api_pageable_mem_access_on_gpu(UVM_PAGEABLE_MEM_ACCESS_ON_GPU_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_t *gpu;

    uvm_va_space_down_read(va_space);
    gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpu_uuid);

    if (!gpu) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_INVALID_DEVICE;
    }

    if (uvm_va_space_pageable_mem_access_supported(va_space) && gpu->parent->replayable_faults_supported)
        params->pageableMemAccess = NV_TRUE;
    else
        params->pageableMemAccess = NV_FALSE;

    uvm_va_space_up_read(va_space);
    return NV_OK;
}

NV_STATUS uvm_test_set_prefetch_filtering(UVM_TEST_SET_PREFETCH_FILTERING_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_t *gpu = NULL;
    NV_STATUS status = NV_OK;

    uvm_mutex_lock(&g_uvm_global.global_lock);

    uvm_va_space_down_read(va_space);

    gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpu_uuid);

    if (!gpu) {
        status = NV_ERR_INVALID_DEVICE;
        goto done;
    }

    if (!gpu->parent->isr.replayable_faults.handling || !gpu->parent->prefetch_fault_supported) {
        status = NV_ERR_INVALID_DEVICE;
        goto done;
    }

    switch (params->filtering_mode) {
        case UVM_TEST_PREFETCH_FILTERING_MODE_FILTER_ALL:
            uvm_parent_gpu_disable_prefetch_faults(gpu->parent);
            break;
        case UVM_TEST_PREFETCH_FILTERING_MODE_FILTER_NONE:
            uvm_parent_gpu_enable_prefetch_faults(gpu->parent);
            break;
        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

done:
    uvm_va_space_up_read(va_space);

    uvm_mutex_unlock(&g_uvm_global.global_lock);
    return status;
}

NV_STATUS uvm_test_get_gpu_time(UVM_TEST_GET_GPU_TIME_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_t *gpu = NULL;
    NV_STATUS status = NV_OK;

    uvm_va_space_down_read(va_space);

    gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpu_uuid);

    if (gpu)
        params->timestamp_ns = gpu->parent->host_hal->get_time(gpu);
    else
        status = NV_ERR_INVALID_DEVICE;

    uvm_va_space_up_read(va_space);

    return status;
}

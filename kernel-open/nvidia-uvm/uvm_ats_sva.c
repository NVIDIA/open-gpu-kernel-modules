/*******************************************************************************
    Copyright (c) 2018-2023 NVIDIA Corporation

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

#include "uvm_ats_sva.h"

#if UVM_ATS_SVA_SUPPORTED()

#include "uvm_gpu.h"
#include "uvm_va_space.h"
#include "uvm_va_space_mm.h"

#include <asm/io.h>
#include <linux/log2.h>
#include <linux/iommu.h>
#include <linux/mm_types.h>
#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/mmu_context.h>

// linux/sched/mm.h is needed for mmget_not_zero and mmput to get the mm
// reference required for the iommu_sva_bind_device() call. This header is not
// present in all the supported versions. Instead of adding a conftest just for
// this header file, use UVM_ATS_SVA_SUPPORTED().
#include <linux/sched/mm.h>

// iommu_sva_bind_device() removed drvdata paramter with commit
// 942fd5435dccb273f90176b046ae6bbba60cfbd8 (10/31/2022).
#if defined(NV_IOMMU_SVA_BIND_DEVICE_HAS_DRVDATA_ARG)
#define UVM_IOMMU_SVA_BIND_DEVICE(dev, mm) iommu_sva_bind_device(dev, mm, NULL)
#else
#define UVM_IOMMU_SVA_BIND_DEVICE(dev, mm) iommu_sva_bind_device(dev, mm)
#endif

// Type to represent a 128-bit SMMU command queue command.
struct smmu_cmd {
    NvU64 low;
    NvU64 high;
};

// Base address of SMMU CMDQ-V for GSMMU0.
#define SMMU_CMDQV_BASE_ADDR(smmu_base) (smmu_base + 0x200000)
#define SMMU_CMDQV_BASE_LEN 0x00830000

// CMDQV configuration is done by firmware but we check status here.
#define SMMU_CMDQV_CONFIG 0x0
#define SMMU_CMDQV_CONFIG_CMDQV_EN BIT(0)

// Used to map a particular VCMDQ to a VINTF.
#define SMMU_CMDQV_CMDQ_ALLOC_MAP(vcmdq_id) (0x200 + 0x4 * (vcmdq_id))
#define SMMU_CMDQV_CMDQ_ALLOC_MAP_ALLOC BIT(0)

// Shift for the field containing the index of the virtual interface
// owning the VCMDQ.
#define SMMU_CMDQV_CMDQ_ALLOC_MAP_VIRT_INTF_INDX_SHIFT 15

// Base address for the VINTF registers.
#define SMMU_VINTF_BASE_ADDR(cmdqv_base_addr, vintf_id) (cmdqv_base_addr + 0x1000 + 0x100 * (vintf_id))

// Virtual interface (VINTF) configuration registers. The WAR only
// works on baremetal so we need to configure ourselves as the
// hypervisor owner.
#define SMMU_VINTF_CONFIG 0x0
#define SMMU_VINTF_CONFIG_ENABLE BIT(0)
#define SMMU_VINTF_CONFIG_HYP_OWN BIT(17)

#define SMMU_VINTF_STATUS 0x0
#define SMMU_VINTF_STATUS_ENABLED BIT(0)

// Caclulates the base address for a particular VCMDQ instance.
#define SMMU_VCMDQ_BASE_ADDR(cmdqv_base_addr, vcmdq_id) (cmdqv_base_addr + 0x10000 + 0x80 * (vcmdq_id))

// SMMU command queue consumer index register. Updated by SMMU
// when commands are consumed.
#define SMMU_VCMDQ_CONS 0x0

// SMMU command queue producer index register. Updated by UVM when
// commands are added to the queue.
#define SMMU_VCMDQ_PROD 0x4

// Configuration register used to enable a VCMDQ.
#define SMMU_VCMDQ_CONFIG 0x8
#define SMMU_VCMDQ_CONFIG_ENABLE BIT(0)

// Status register used to check the VCMDQ is enabled.
#define SMMU_VCMDQ_STATUS 0xc
#define SMMU_VCMDQ_STATUS_ENABLED BIT(0)

// Base address offset for the VCMDQ registers.
#define SMMU_VCMDQ_CMDQ_BASE 0x10000

// Size of the command queue. Each command is 16 bytes and we can't
// have a command queue greater than one page in size.
#define SMMU_VCMDQ_CMDQ_BASE_LOG2SIZE (PAGE_SHIFT - ilog2(sizeof(struct smmu_cmd)))
#define SMMU_VCMDQ_CMDQ_ENTRIES (1UL << SMMU_VCMDQ_CMDQ_BASE_LOG2SIZE)

// We always use VINTF63 for the WAR
#define VINTF 63
static void smmu_vintf_write32(void __iomem *smmu_cmdqv_base, int reg, NvU32 val)
{
    iowrite32(val, SMMU_VINTF_BASE_ADDR(smmu_cmdqv_base, VINTF) + reg);
}

static NvU32 smmu_vintf_read32(void __iomem *smmu_cmdqv_base, int reg)
{
    return ioread32(SMMU_VINTF_BASE_ADDR(smmu_cmdqv_base, VINTF) + reg);
}

// We always use VCMDQ127 for the WAR
#define VCMDQ 127
static void smmu_vcmdq_write32(void __iomem *smmu_cmdqv_base, int reg, NvU32 val)
{
    iowrite32(val, SMMU_VCMDQ_BASE_ADDR(smmu_cmdqv_base, VCMDQ) + reg);
}

static NvU32 smmu_vcmdq_read32(void __iomem *smmu_cmdqv_base, int reg)
{
    return ioread32(SMMU_VCMDQ_BASE_ADDR(smmu_cmdqv_base, VCMDQ) + reg);
}

static void smmu_vcmdq_write64(void __iomem *smmu_cmdqv_base, int reg, NvU64 val)
{
#if NV_IS_EXPORT_SYMBOL_PRESENT___iowrite64_lo_hi
    __iowrite64_lo_hi(val, SMMU_VCMDQ_BASE_ADDR(smmu_cmdqv_base, VCMDQ) + reg);
#else
    iowrite64(val, SMMU_VCMDQ_BASE_ADDR(smmu_cmdqv_base, VCMDQ) + reg);
#endif
}

// Fix for Bug 4130089: [GH180][r535] WAR for kernel not issuing SMMU
// TLB invalidates on read-only to read-write upgrades
static NV_STATUS uvm_ats_smmu_war_init(uvm_parent_gpu_t *parent_gpu)
{
    uvm_spin_loop_t spin;
    NV_STATUS status;
    unsigned long cmdqv_config;
    void __iomem *smmu_cmdqv_base;
    struct acpi_iort_node *node;
    struct acpi_iort_smmu_v3 *iort_smmu;

    node = *(struct acpi_iort_node **) dev_get_platdata(parent_gpu->pci_dev->dev.iommu->iommu_dev->dev->parent);
    iort_smmu = (struct acpi_iort_smmu_v3 *) node->node_data;

    smmu_cmdqv_base = ioremap(SMMU_CMDQV_BASE_ADDR(iort_smmu->base_address), SMMU_CMDQV_BASE_LEN);
    if (!smmu_cmdqv_base)
        return NV_ERR_NO_MEMORY;

    parent_gpu->smmu_war.smmu_cmdqv_base = smmu_cmdqv_base;
    cmdqv_config = ioread32(smmu_cmdqv_base + SMMU_CMDQV_CONFIG);
    if (!(cmdqv_config & SMMU_CMDQV_CONFIG_CMDQV_EN)) {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto out;
    }

    // Allocate SMMU CMDQ pages for WAR
    parent_gpu->smmu_war.smmu_cmdq = alloc_page(NV_UVM_GFP_FLAGS | __GFP_ZERO);
    if (!parent_gpu->smmu_war.smmu_cmdq) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    // Initialise VINTF for the WAR
    smmu_vintf_write32(smmu_cmdqv_base, SMMU_VINTF_CONFIG, SMMU_VINTF_CONFIG_ENABLE | SMMU_VINTF_CONFIG_HYP_OWN);
    UVM_SPIN_WHILE(!(smmu_vintf_read32(smmu_cmdqv_base, SMMU_VINTF_STATUS) & SMMU_VINTF_STATUS_ENABLED), &spin);

    // Allocate VCMDQ to VINTF
    iowrite32((VINTF << SMMU_CMDQV_CMDQ_ALLOC_MAP_VIRT_INTF_INDX_SHIFT) | SMMU_CMDQV_CMDQ_ALLOC_MAP_ALLOC,
              smmu_cmdqv_base + SMMU_CMDQV_CMDQ_ALLOC_MAP(VCMDQ));

    smmu_vcmdq_write64(smmu_cmdqv_base, SMMU_VCMDQ_CMDQ_BASE,
                       page_to_phys(parent_gpu->smmu_war.smmu_cmdq) | SMMU_VCMDQ_CMDQ_BASE_LOG2SIZE);
    smmu_vcmdq_write32(smmu_cmdqv_base, SMMU_VCMDQ_CONS, 0);
    smmu_vcmdq_write32(smmu_cmdqv_base, SMMU_VCMDQ_PROD, 0);
    smmu_vcmdq_write32(smmu_cmdqv_base, SMMU_VCMDQ_CONFIG, SMMU_VCMDQ_CONFIG_ENABLE);
    UVM_SPIN_WHILE(!(smmu_vcmdq_read32(smmu_cmdqv_base, SMMU_VCMDQ_STATUS) & SMMU_VCMDQ_STATUS_ENABLED), &spin);

    uvm_mutex_init(&parent_gpu->smmu_war.smmu_lock, UVM_LOCK_ORDER_LEAF);
    parent_gpu->smmu_war.smmu_prod = 0;
    parent_gpu->smmu_war.smmu_cons = 0;

    return NV_OK;

out:
    iounmap(parent_gpu->smmu_war.smmu_cmdqv_base);
    parent_gpu->smmu_war.smmu_cmdqv_base = NULL;

    return status;
}

static void uvm_ats_smmu_war_deinit(uvm_parent_gpu_t *parent_gpu)
{
    void __iomem *smmu_cmdqv_base = parent_gpu->smmu_war.smmu_cmdqv_base;
    NvU32 cmdq_alloc_map;

    if (parent_gpu->smmu_war.smmu_cmdqv_base) {
        smmu_vcmdq_write32(smmu_cmdqv_base, SMMU_VCMDQ_CONFIG, 0);
        cmdq_alloc_map = ioread32(smmu_cmdqv_base + SMMU_CMDQV_CMDQ_ALLOC_MAP(VCMDQ));
        iowrite32(cmdq_alloc_map & SMMU_CMDQV_CMDQ_ALLOC_MAP_ALLOC, smmu_cmdqv_base + SMMU_CMDQV_CMDQ_ALLOC_MAP(VCMDQ));
        smmu_vintf_write32(smmu_cmdqv_base, SMMU_VINTF_CONFIG, 0);
    }

    if (parent_gpu->smmu_war.smmu_cmdq)
        __free_page(parent_gpu->smmu_war.smmu_cmdq);

    if (parent_gpu->smmu_war.smmu_cmdqv_base)
        iounmap(parent_gpu->smmu_war.smmu_cmdqv_base);
}

// The SMMU on ARM64 can run under different translation regimes depending on
// what features the OS and CPU variant support. The CPU for GH180 supports
// virtualisation extensions and starts the kernel at EL2 meaning SMMU operates
// under the NS-EL2-E2H translation regime. Therefore we need to use the
// TLBI_EL2_* commands which invalidate TLB entries created under this
// translation regime.
#define CMDQ_OP_TLBI_EL2_ASID 0x21;
#define CMDQ_OP_TLBI_EL2_VA 0x22;
#define CMDQ_OP_CMD_SYNC 0x46

// Use the same maximum as used for MAX_TLBI_OPS in the upstream
// kernel.
#define UVM_MAX_TLBI_OPS (1UL << (PAGE_SHIFT - 3))

#if UVM_ATS_SMMU_WAR_REQUIRED()
void uvm_ats_smmu_invalidate_tlbs(uvm_gpu_va_space_t *gpu_va_space, NvU64 addr, size_t size)
{
    struct mm_struct *mm = gpu_va_space->va_space->va_space_mm.mm;
    uvm_parent_gpu_t *parent_gpu = gpu_va_space->gpu->parent;
    struct {
        NvU64 low;
        NvU64 high;
    } *vcmdq;
    unsigned long vcmdq_prod;
    NvU64 end;
    uvm_spin_loop_t spin;
    NvU16 asid;

    if (!parent_gpu->smmu_war.smmu_cmdqv_base)
        return;

    asid = arm64_mm_context_get(mm);
    vcmdq = kmap(parent_gpu->smmu_war.smmu_cmdq);
    uvm_mutex_lock(&parent_gpu->smmu_war.smmu_lock);
    vcmdq_prod = parent_gpu->smmu_war.smmu_prod;

    // Our queue management is very simple. The mutex prevents multiple
    // producers writing to the queue and all our commands require waiting for
    // the queue to drain so we know it's empty. If we can't fit enough commands
    // in the queue we just invalidate the whole ASID.
    //
    // The command queue is a cirular buffer with the MSB representing a wrap
    // bit that must toggle on each wrap. See the SMMU architecture
    // specification for more details.
    //
    // SMMU_VCMDQ_CMDQ_ENTRIES - 1 because we need to leave space for the
    // CMD_SYNC.
    if ((size >> PAGE_SHIFT) > min(UVM_MAX_TLBI_OPS, SMMU_VCMDQ_CMDQ_ENTRIES - 1)) {
        vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].low = CMDQ_OP_TLBI_EL2_ASID;
        vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].low |= (NvU64) asid << 48;
        vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].high = 0;
        vcmdq_prod++;
    }
    else {
        for (end = addr + size; addr < end; addr += PAGE_SIZE) {
            vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].low = CMDQ_OP_TLBI_EL2_VA;
            vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].low |= (NvU64) asid << 48;
            vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].high = addr & ~((1UL << 12) - 1);
            vcmdq_prod++;
        }
    }

    vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].low = CMDQ_OP_CMD_SYNC;
    vcmdq[vcmdq_prod % SMMU_VCMDQ_CMDQ_ENTRIES].high = 0x0;
    vcmdq_prod++;

    // MSB is the wrap bit
    vcmdq_prod &= (1UL << (SMMU_VCMDQ_CMDQ_BASE_LOG2SIZE + 1)) - 1;
    parent_gpu->smmu_war.smmu_prod = vcmdq_prod;
    smmu_vcmdq_write32(parent_gpu->smmu_war.smmu_cmdqv_base, SMMU_VCMDQ_PROD, parent_gpu->smmu_war.smmu_prod);

    UVM_SPIN_WHILE(
        (smmu_vcmdq_read32(parent_gpu->smmu_war.smmu_cmdqv_base, SMMU_VCMDQ_CONS) & GENMASK(19, 0)) != vcmdq_prod,
        &spin);

    uvm_mutex_unlock(&parent_gpu->smmu_war.smmu_lock);
    kunmap(parent_gpu->smmu_war.smmu_cmdq);
    arm64_mm_context_put(mm);
}
#endif

NV_STATUS uvm_ats_sva_add_gpu(uvm_parent_gpu_t *parent_gpu)
{
#if NV_IS_EXPORT_SYMBOL_GPL_iommu_dev_enable_feature
    int ret;

    ret = iommu_dev_enable_feature(&parent_gpu->pci_dev->dev, IOMMU_DEV_FEAT_SVA);
    if (ret)
        return errno_to_nv_status(ret);
#endif
    if (UVM_ATS_SMMU_WAR_REQUIRED())
        return uvm_ats_smmu_war_init(parent_gpu);
    else
        return NV_OK;
}

void uvm_ats_sva_remove_gpu(uvm_parent_gpu_t *parent_gpu)
{
    if (UVM_ATS_SMMU_WAR_REQUIRED())
        uvm_ats_smmu_war_deinit(parent_gpu);

#if NV_IS_EXPORT_SYMBOL_GPL_iommu_dev_disable_feature
    iommu_dev_disable_feature(&parent_gpu->pci_dev->dev, IOMMU_DEV_FEAT_SVA);
#endif
}

NV_STATUS uvm_ats_sva_bind_gpu(uvm_gpu_va_space_t *gpu_va_space)
{
    NV_STATUS status = NV_OK;
    struct iommu_sva *iommu_handle;
    struct pci_dev *pci_dev = gpu_va_space->gpu->parent->pci_dev;
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = &gpu_va_space->ats.sva;
    struct mm_struct *mm = gpu_va_space->va_space->va_space_mm.mm;

    UVM_ASSERT(gpu_va_space->ats.enabled);
    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_INIT);
    UVM_ASSERT(mm);

    // The mmput() below may trigger the kernel's mm teardown with exit_mmap()
    // and uvm_va_space_mm_shutdown() and uvm_vm_close_managed() in that path
    // will try to grab the va_space lock and deadlock if va_space was already
    // locked.
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_VA_SPACE);

    // iommu_sva_bind_device() requires the mm reference to be acquired. Since
    // the mm is already retained, mm is still valid but may be inactive since
    // mm_users can still be zero since UVM doesn't use mm_users and maintains a
    // separate refcount (retained_count) for the mm in va_space_mm. See the
    // block comment in va_space_mm.c for more details. So, return an error if
    // mm_users is zero.
    if (!mmget_not_zero(mm))
        return NV_ERR_PAGE_TABLE_NOT_AVAIL;

    // Multiple calls for the {same pci_dev, mm} pair are refcounted by the ARM
    // SMMU Layer.
    iommu_handle = UVM_IOMMU_SVA_BIND_DEVICE(&pci_dev->dev, mm);
    if (IS_ERR(iommu_handle)) {
        status = errno_to_nv_status(PTR_ERR(iommu_handle));
        goto out;
    }

    // If this is not the first bind of the gpu in the mm, then the previously
    // stored iommu_handle in the gpu_va_space must match the handle returned by
    // iommu_sva_bind_device().
    if (sva_gpu_va_space->iommu_handle) {
        UVM_ASSERT(sva_gpu_va_space->iommu_handle == iommu_handle);
        nv_kref_get(&sva_gpu_va_space->kref);
    }
    else {
        sva_gpu_va_space->iommu_handle = iommu_handle;
        nv_kref_init(&sva_gpu_va_space->kref);
    }

out:
    mmput(mm);
    return status;
}

static void uvm_sva_reset_iommu_handle(nv_kref_t *nv_kref)
{
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = container_of(nv_kref, uvm_sva_gpu_va_space_t, kref);
    sva_gpu_va_space->iommu_handle = NULL;
}

void uvm_ats_sva_unbind_gpu(uvm_gpu_va_space_t *gpu_va_space)
{
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = &gpu_va_space->ats.sva;

    // ARM SMMU layer decrements the refcount for the {pci_dev, mm} pair.
    // The actual unbind happens only when the refcount reaches zero.
    if (sva_gpu_va_space->iommu_handle) {
        iommu_sva_unbind_device(sva_gpu_va_space->iommu_handle);
        nv_kref_put(&sva_gpu_va_space->kref, uvm_sva_reset_iommu_handle);
    }
}

NV_STATUS uvm_ats_sva_register_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
{
    NvU32 pasid;
    NV_STATUS status = NV_OK;
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = &gpu_va_space->ats.sva;

    // A successful iommu_sva_bind_device() should have preceded this call.
    UVM_ASSERT(sva_gpu_va_space->iommu_handle);

    pasid = iommu_sva_get_pasid(sva_gpu_va_space->iommu_handle);
    if (pasid == IOMMU_PASID_INVALID)
        status = errno_to_nv_status(ENODEV);
    else
        gpu_va_space->ats.pasid = pasid;

    return status;
}

void uvm_ats_sva_unregister_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
{
    gpu_va_space->ats.pasid = -1U;
}

#endif // UVM_ATS_SVA_SUPPORTED()

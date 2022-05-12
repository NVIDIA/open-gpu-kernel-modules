/*
 * Copyright 2008 Advanced Micro Devices, Inc.
 * Copyright 2008 Red Hat Inc.
 * Copyright 2009 Jerome Glisse.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Dave Airlie
 *          Alex Deucher
 *          Jerome Glisse
 */
#include <linux/power_supply.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/reboot.h>
#include <linux/slab.h>
#include <linux/iommu.h>
#include <linux/pci.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_probe_helper.h>
#include <drm/amdgpu_drm.h>
#include <linux/vgaarb.h>
#include <linux/vga_switcheroo.h>
#include <linux/efi.h>
#include "amdgpu.h"
#include "amdgpu_trace.h"
#include "amdgpu_i2c.h"
#include "atom.h"
#include "amdgpu_atombios.h"
#include "amdgpu_atomfirmware.h"
#include "amd_pcie.h"
#ifdef CONFIG_DRM_AMDGPU_SI
#include "si.h"
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
#include "cik.h"
#endif
#include "vi.h"
#include "soc15.h"
#include "nv.h"
#include "bif/bif_4_1_d.h"
#include <linux/firmware.h>
#include "amdgpu_vf_error.h"

#include "amdgpu_amdkfd.h"
#include "amdgpu_pm.h"

#include "amdgpu_xgmi.h"
#include "amdgpu_ras.h"
#include "amdgpu_pmu.h"
#include "amdgpu_fru_eeprom.h"
#include "amdgpu_reset.h"

#include <linux/suspend.h>
#include <drm/task_barrier.h>
#include <linux/pm_runtime.h>

#include <drm/drm_drv.h>

MODULE_FIRMWARE("amdgpu/vega10_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/vega12_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/raven_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/picasso_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/raven2_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/arcturus_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/renoir_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/navi10_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/navi14_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/navi12_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/vangogh_gpu_info.bin");
MODULE_FIRMWARE("amdgpu/yellow_carp_gpu_info.bin");

#define AMDGPU_RESUME_MS		2000
#define AMDGPU_MAX_RETRY_LIMIT		2
#define AMDGPU_RETRY_SRIOV_RESET(r) ((r) == -EBUSY || (r) == -ETIMEDOUT || (r) == -EINVAL)

const char *amdgpu_asic_name[] = {
	"TAHITI",
	"PITCAIRN",
	"VERDE",
	"OLAND",
	"HAINAN",
	"BONAIRE",
	"KAVERI",
	"KABINI",
	"HAWAII",
	"MULLINS",
	"TOPAZ",
	"TONGA",
	"FIJI",
	"CARRIZO",
	"STONEY",
	"POLARIS10",
	"POLARIS11",
	"POLARIS12",
	"VEGAM",
	"VEGA10",
	"VEGA12",
	"VEGA20",
	"RAVEN",
	"ARCTURUS",
	"RENOIR",
	"ALDEBARAN",
	"NAVI10",
	"CYAN_SKILLFISH",
	"NAVI14",
	"NAVI12",
	"SIENNA_CICHLID",
	"NAVY_FLOUNDER",
	"VANGOGH",
	"DIMGREY_CAVEFISH",
	"BEIGE_GOBY",
	"YELLOW_CARP",
	"IP DISCOVERY",
	"LAST",
};

extern bool pcie_p2p;

/**
 * DOC: pcie_replay_count
 *
 * The amdgpu driver provides a sysfs API for reporting the total number
 * of PCIe replays (NAKs)
 * The file pcie_replay_count is used for this and returns the total
 * number of replays as a sum of the NAKs generated and NAKs received
 */

static ssize_t amdgpu_device_get_pcie_replay_count(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);
	uint64_t cnt = amdgpu_asic_get_pcie_replay_count(adev);

	return sysfs_emit(buf, "%llu\n", cnt);
}

static DEVICE_ATTR(pcie_replay_count, S_IRUGO,
		amdgpu_device_get_pcie_replay_count, NULL);

static void amdgpu_device_get_pcie_info(struct amdgpu_device *adev);

/**
 * DOC: product_name
 *
 * The amdgpu driver provides a sysfs API for reporting the product name
 * for the device
 * The file serial_number is used for this and returns the product name
 * as returned from the FRU.
 * NOTE: This is only available for certain server cards
 */

static ssize_t amdgpu_device_get_product_name(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);

	return sysfs_emit(buf, "%s\n", adev->product_name);
}

static DEVICE_ATTR(product_name, S_IRUGO,
		amdgpu_device_get_product_name, NULL);

/**
 * DOC: product_number
 *
 * The amdgpu driver provides a sysfs API for reporting the part number
 * for the device
 * The file serial_number is used for this and returns the part number
 * as returned from the FRU.
 * NOTE: This is only available for certain server cards
 */

static ssize_t amdgpu_device_get_product_number(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);

	return sysfs_emit(buf, "%s\n", adev->product_number);
}

static DEVICE_ATTR(product_number, S_IRUGO,
		amdgpu_device_get_product_number, NULL);

/**
 * DOC: serial_number
 *
 * The amdgpu driver provides a sysfs API for reporting the serial number
 * for the device
 * The file serial_number is used for this and returns the serial number
 * as returned from the FRU.
 * NOTE: This is only available for certain server cards
 */

static ssize_t amdgpu_device_get_serial_number(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);

	return sysfs_emit(buf, "%s\n", adev->serial);
}

static DEVICE_ATTR(serial_number, S_IRUGO,
		amdgpu_device_get_serial_number, NULL);

int amdgpu_read_lock(struct drm_device *dev, bool interruptible)
{
	struct amdgpu_device *adev = drm_to_adev(dev);
	int ret = 0;

	/**
	 * if a thread hold the read lock, but recovery thread has started,
	 * it should release the read lock and wait for recovery thread finished
	 * Because pre-reset functions have begun, which stops old threads but no
	 * include the current thread.
	*/
	if (interruptible) {
#ifdef HAVE_DOWN_READ_KILLABLE
		while (!(ret = down_read_killable(&adev->reset_sem)) &&
			amdgpu_in_reset(adev)) {
			up_read(&adev->reset_sem);
			ret = wait_event_interruptible(adev->recovery_fini_event,
							!amdgpu_in_reset(adev));
			if (ret)
				break;
		}
#else
		down_read(&adev->reset_sem);
		while (amdgpu_in_reset(adev)) {
			up_read(&adev->reset_sem);
			ret = wait_event_interruptible(adev->recovery_fini_event,
				   !amdgpu_in_reset(adev));
			if (ret)
				break;
			down_read(&adev->reset_sem);
		}
#endif
	} else {
		down_read(&adev->reset_sem);
		while (amdgpu_in_reset(adev)) {
			up_read(&adev->reset_sem);
			wait_event(adev->recovery_fini_event,
				   !amdgpu_in_reset(adev));
			down_read(&adev->reset_sem);
		}
	}

	return ret;
}

void amdgpu_read_unlock(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);

	up_read(&adev->reset_sem);
}

static void amdgpu_write_lock(struct amdgpu_device *adev, struct amdgpu_hive_info *hive)
{
	if (hive) {
		down_write_nest_lock(&adev->reset_sem, &hive->hive_lock);
	} else {
		down_write(&adev->reset_sem);
	}
}

static void amdgpu_write_unlock(struct amdgpu_device *adev)
{
	up_write(&adev->reset_sem);
}

/**
 * amdgpu_device_supports_px - Is the device a dGPU with ATPX power control
 *
 * @dev: drm_device pointer
 *
 * Returns true if the device is a dGPU with ATPX power control,
 * otherwise return false.
 */
bool amdgpu_device_supports_px(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);

	if ((adev->flags & AMD_IS_PX) && !amdgpu_is_atpx_hybrid())
		return true;
	return false;
}

/**
 * amdgpu_device_supports_boco - Is the device a dGPU with ACPI power resources
 *
 * @dev: drm_device pointer
 *
 * Returns true if the device is a dGPU with ACPI power control,
 * otherwise return false.
 */
bool amdgpu_device_supports_boco(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);

	if (adev->has_pr3 ||
	    ((adev->flags & AMD_IS_PX) && amdgpu_is_atpx_hybrid()))
		return true;
	return false;
}

/**
 * amdgpu_device_supports_baco - Does the device support BACO
 *
 * @dev: drm_device pointer
 *
 * Returns true if the device supporte BACO,
 * otherwise return false.
 */
bool amdgpu_device_supports_baco(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);

	return amdgpu_asic_supports_baco(adev);
}

/**
 * amdgpu_device_supports_smart_shift - Is the device dGPU with
 * smart shift support
 *
 * @dev: drm_device pointer
 *
 * Returns true if the device is a dGPU with Smart Shift support,
 * otherwise returns false.
 */
bool amdgpu_device_supports_smart_shift(struct drm_device *dev)
{
	return (amdgpu_device_supports_boco(dev) &&
		amdgpu_acpi_is_power_shift_control_supported());
}

/*
 * VRAM access helper functions
 */

/**
 * amdgpu_device_mm_access - access vram by MM_INDEX/MM_DATA
 *
 * @adev: amdgpu_device pointer
 * @pos: offset of the buffer in vram
 * @buf: virtual address of the buffer in system memory
 * @size: read/write size, sizeof(@buf) must > @size
 * @write: true - write to vram, otherwise - read from vram
 */
void amdgpu_device_mm_access(struct amdgpu_device *adev, loff_t pos,
			     void *buf, size_t size, bool write)
{
	unsigned long flags;
	uint32_t hi = ~0, tmp = 0;
	uint32_t *data = buf;
	uint64_t last;
	int idx;

	if (!drm_dev_enter(adev_to_drm(adev), &idx))
		return;

	BUG_ON(!IS_ALIGNED(pos, 4) || !IS_ALIGNED(size, 4));

	spin_lock_irqsave(&adev->mmio_idx_lock, flags);
	for (last = pos + size; pos < last; pos += 4) {
		tmp = pos >> 31;

		WREG32_NO_KIQ(mmMM_INDEX, ((uint32_t)pos) | 0x80000000);
		if (tmp != hi) {
			WREG32_NO_KIQ(mmMM_INDEX_HI, tmp);
			hi = tmp;
		}
		if (write)
			WREG32_NO_KIQ(mmMM_DATA, *data++);
		else
			*data++ = RREG32_NO_KIQ(mmMM_DATA);
	}

	spin_unlock_irqrestore(&adev->mmio_idx_lock, flags);
	drm_dev_exit(idx);
}

/**
 * amdgpu_device_aper_access - access vram by vram aperature
 *
 * @adev: amdgpu_device pointer
 * @pos: offset of the buffer in vram
 * @buf: virtual address of the buffer in system memory
 * @size: read/write size, sizeof(@buf) must > @size
 * @write: true - write to vram, otherwise - read from vram
 *
 * The return value means how many bytes have been transferred.
 */
size_t amdgpu_device_aper_access(struct amdgpu_device *adev, loff_t pos,
				 void *buf, size_t size, bool write)
{
#ifdef CONFIG_64BIT
	void __iomem *addr;
	size_t count = 0;
	uint64_t last;

	if (!adev->mman.aper_base_kaddr)
		return 0;

	last = min(pos + size, adev->gmc.visible_vram_size);
	if (last > pos) {
		addr = adev->mman.aper_base_kaddr + pos;
		count = last - pos;

		if (write) {
			memcpy_toio(addr, buf, count);
			mb();
			amdgpu_device_flush_hdp(adev, NULL);
		} else {
			amdgpu_device_invalidate_hdp(adev, NULL);
			mb();
			memcpy_fromio(buf, addr, count);
		}

	}

	return count;
#else
	return 0;
#endif
}

/**
 * amdgpu_device_vram_access - read/write a buffer in vram
 *
 * @adev: amdgpu_device pointer
 * @pos: offset of the buffer in vram
 * @buf: virtual address of the buffer in system memory
 * @size: read/write size, sizeof(@buf) must > @size
 * @write: true - write to vram, otherwise - read from vram
 */
void amdgpu_device_vram_access(struct amdgpu_device *adev, loff_t pos,
			       void *buf, size_t size, bool write)
{
	size_t count;

	/* try to using vram apreature to access vram first */
	count = amdgpu_device_aper_access(adev, pos, buf, size, write);
	size -= count;
	if (size) {
		/* using MM to access rest vram */
		pos += count;
		buf += count;
		amdgpu_device_mm_access(adev, pos, buf, size, write);
	}
}

/*
 * register access helper functions.
 */

/* Check if hw access should be skipped because of hotplug or device error */
bool amdgpu_device_skip_hw_access(struct amdgpu_device *adev)
{
	if (adev->no_hw_access)
		return true;

#ifdef CONFIG_LOCKDEP
	/*
	 * This is a bit complicated to understand, so worth a comment. What we assert
	 * here is that the GPU reset is not running on another thread in parallel.
	 *
	 * For this we trylock the read side of the reset semaphore, if that succeeds
	 * we know that the reset is not running in paralell.
	 *
	 * If the trylock fails we assert that we are either already holding the read
	 * side of the lock or are the reset thread itself and hold the write side of
	 * the lock.
	 */
	if (in_task()) {
		if (down_read_trylock(&adev->reset_sem))
			up_read(&adev->reset_sem);
		else
			lockdep_assert_held(&adev->reset_sem);
	}
#endif
	return false;
}

/**
 * amdgpu_device_rreg - read a memory mapped IO or indirect register
 *
 * @adev: amdgpu_device pointer
 * @reg: dword aligned register offset
 * @acc_flags: access flags which require special behavior
 *
 * Returns the 32 bit value from the offset specified.
 */
uint32_t amdgpu_device_rreg(struct amdgpu_device *adev,
			    uint32_t reg, uint32_t acc_flags)
{
	uint32_t ret;

	if (amdgpu_device_skip_hw_access(adev))
		return 0;

	if ((reg * 4) < adev->rmmio_size) {
		if (!(acc_flags & AMDGPU_REGS_NO_KIQ) &&
		    amdgpu_sriov_runtime(adev))
			ret = amdgpu_kiq_rreg(adev, reg);
		else
			ret = readl(((void __iomem *)adev->rmmio) + (reg * 4));
	} else {
		ret = adev->pcie_rreg(adev, reg * 4);
	}

	trace_amdgpu_device_rreg(adev->pdev->device, reg, ret);

	return ret;
}

/*
 * MMIO register read with bytes helper functions
 * @offset:bytes offset from MMIO start
 *
*/

/**
 * amdgpu_mm_rreg8 - read a memory mapped IO register
 *
 * @adev: amdgpu_device pointer
 * @offset: byte aligned register offset
 *
 * Returns the 8 bit value from the offset specified.
 */
uint8_t amdgpu_mm_rreg8(struct amdgpu_device *adev, uint32_t offset)
{
	if (amdgpu_device_skip_hw_access(adev))
		return 0;

	if (offset < adev->rmmio_size)
		return (readb(adev->rmmio + offset));
	BUG();
}

/*
 * MMIO register write with bytes helper functions
 * @offset:bytes offset from MMIO start
 * @value: the value want to be written to the register
 *
*/
/**
 * amdgpu_mm_wreg8 - read a memory mapped IO register
 *
 * @adev: amdgpu_device pointer
 * @offset: byte aligned register offset
 * @value: 8 bit value to write
 *
 * Writes the value specified to the offset specified.
 */
void amdgpu_mm_wreg8(struct amdgpu_device *adev, uint32_t offset, uint8_t value)
{
	if (amdgpu_device_skip_hw_access(adev))
		return;

	if (offset < adev->rmmio_size)
		writeb(value, adev->rmmio + offset);
	else
		BUG();
}

/**
 * amdgpu_device_wreg - write to a memory mapped IO or indirect register
 *
 * @adev: amdgpu_device pointer
 * @reg: dword aligned register offset
 * @v: 32 bit value to write to the register
 * @acc_flags: access flags which require special behavior
 *
 * Writes the value specified to the offset specified.
 */
void amdgpu_device_wreg(struct amdgpu_device *adev,
			uint32_t reg, uint32_t v,
			uint32_t acc_flags)
{
	if (amdgpu_device_skip_hw_access(adev))
		return;

	if ((reg * 4) < adev->rmmio_size) {
		if (!(acc_flags & AMDGPU_REGS_NO_KIQ) &&
		    amdgpu_sriov_runtime(adev))
			amdgpu_kiq_wreg(adev, reg, v);
		else
			writel(v, ((void __iomem *)adev->rmmio) + (reg * 4));
	} else {
		adev->pcie_wreg(adev, reg * 4, v);
	}

	trace_amdgpu_device_wreg(adev->pdev->device, reg, v);
}

/**
 * amdgpu_mm_wreg_mmio_rlc -  write register either with direct/indirect mmio or with RLC path if in range
 *
 * @adev: amdgpu_device pointer
 * @reg: mmio/rlc register
 * @v: value to write
 *
 * this function is invoked only for the debugfs register access
 */
void amdgpu_mm_wreg_mmio_rlc(struct amdgpu_device *adev,
			     uint32_t reg, uint32_t v)
{
	if (amdgpu_device_skip_hw_access(adev))
		return;

	if (amdgpu_sriov_fullaccess(adev) &&
	    adev->gfx.rlc.funcs &&
	    adev->gfx.rlc.funcs->is_rlcg_access_range) {
		if (adev->gfx.rlc.funcs->is_rlcg_access_range(adev, reg))
			return amdgpu_sriov_wreg(adev, reg, v, 0, 0);
	} else if ((reg * 4) >= adev->rmmio_size) {
		adev->pcie_wreg(adev, reg * 4, v);
	} else {
		writel(v, ((void __iomem *)adev->rmmio) + (reg * 4));
	}
}

/**
 * amdgpu_mm_rdoorbell - read a doorbell dword
 *
 * @adev: amdgpu_device pointer
 * @index: doorbell index
 *
 * Returns the value in the doorbell aperture at the
 * requested doorbell index (CIK).
 */
u32 amdgpu_mm_rdoorbell(struct amdgpu_device *adev, u32 index)
{
	if (amdgpu_device_skip_hw_access(adev))
		return 0;

	if (index < adev->doorbell.num_doorbells) {
		return readl(adev->doorbell.ptr + index);
	} else {
		DRM_ERROR("reading beyond doorbell aperture: 0x%08x!\n", index);
		return 0;
	}
}

/**
 * amdgpu_mm_wdoorbell - write a doorbell dword
 *
 * @adev: amdgpu_device pointer
 * @index: doorbell index
 * @v: value to write
 *
 * Writes @v to the doorbell aperture at the
 * requested doorbell index (CIK).
 */
void amdgpu_mm_wdoorbell(struct amdgpu_device *adev, u32 index, u32 v)
{
	if (amdgpu_device_skip_hw_access(adev))
		return;

	if (index < adev->doorbell.num_doorbells) {
		writel(v, adev->doorbell.ptr + index);
	} else {
		DRM_ERROR("writing beyond doorbell aperture: 0x%08x!\n", index);
	}
}

/**
 * amdgpu_mm_rdoorbell64 - read a doorbell Qword
 *
 * @adev: amdgpu_device pointer
 * @index: doorbell index
 *
 * Returns the value in the doorbell aperture at the
 * requested doorbell index (VEGA10+).
 */
u64 amdgpu_mm_rdoorbell64(struct amdgpu_device *adev, u32 index)
{
	if (amdgpu_device_skip_hw_access(adev))
		return 0;

	if (index < adev->doorbell.num_doorbells) {
		return atomic64_read((atomic64_t *)(adev->doorbell.ptr + index));
	} else {
		DRM_ERROR("reading beyond doorbell aperture: 0x%08x!\n", index);
		return 0;
	}
}

/**
 * amdgpu_mm_wdoorbell64 - write a doorbell Qword
 *
 * @adev: amdgpu_device pointer
 * @index: doorbell index
 * @v: value to write
 *
 * Writes @v to the doorbell aperture at the
 * requested doorbell index (VEGA10+).
 */
void amdgpu_mm_wdoorbell64(struct amdgpu_device *adev, u32 index, u64 v)
{
	if (amdgpu_device_skip_hw_access(adev))
		return;

	if (index < adev->doorbell.num_doorbells) {
		atomic64_set((atomic64_t *)(adev->doorbell.ptr + index), v);
	} else {
		DRM_ERROR("writing beyond doorbell aperture: 0x%08x!\n", index);
	}
}

/**
 * amdgpu_device_indirect_rreg - read an indirect register
 *
 * @adev: amdgpu_device pointer
 * @pcie_index: mmio register offset
 * @pcie_data: mmio register offset
 * @reg_addr: indirect register address to read from
 *
 * Returns the value of indirect register @reg_addr
 */
u32 amdgpu_device_indirect_rreg(struct amdgpu_device *adev,
				u32 pcie_index, u32 pcie_data,
				u32 reg_addr)
{
	unsigned long flags;
	u32 r;
	void __iomem *pcie_index_offset;
	void __iomem *pcie_data_offset;

	spin_lock_irqsave(&adev->pcie_idx_lock, flags);
	pcie_index_offset = (void __iomem *)adev->rmmio + pcie_index * 4;
	pcie_data_offset = (void __iomem *)adev->rmmio + pcie_data * 4;

	writel(reg_addr, pcie_index_offset);
	readl(pcie_index_offset);
	r = readl(pcie_data_offset);
	spin_unlock_irqrestore(&adev->pcie_idx_lock, flags);

	return r;
}

/**
 * amdgpu_device_indirect_rreg64 - read a 64bits indirect register
 *
 * @adev: amdgpu_device pointer
 * @pcie_index: mmio register offset
 * @pcie_data: mmio register offset
 * @reg_addr: indirect register address to read from
 *
 * Returns the value of indirect register @reg_addr
 */
u64 amdgpu_device_indirect_rreg64(struct amdgpu_device *adev,
				  u32 pcie_index, u32 pcie_data,
				  u32 reg_addr)
{
	unsigned long flags;
	u64 r;
	void __iomem *pcie_index_offset;
	void __iomem *pcie_data_offset;

	spin_lock_irqsave(&adev->pcie_idx_lock, flags);
	pcie_index_offset = (void __iomem *)adev->rmmio + pcie_index * 4;
	pcie_data_offset = (void __iomem *)adev->rmmio + pcie_data * 4;

	/* read low 32 bits */
	writel(reg_addr, pcie_index_offset);
	readl(pcie_index_offset);
	r = readl(pcie_data_offset);
	/* read high 32 bits */
	writel(reg_addr + 4, pcie_index_offset);
	readl(pcie_index_offset);
	r |= ((u64)readl(pcie_data_offset) << 32);
	spin_unlock_irqrestore(&adev->pcie_idx_lock, flags);

	return r;
}

/**
 * amdgpu_device_indirect_wreg - write an indirect register address
 *
 * @adev: amdgpu_device pointer
 * @pcie_index: mmio register offset
 * @pcie_data: mmio register offset
 * @reg_addr: indirect register offset
 * @reg_data: indirect register data
 *
 */
void amdgpu_device_indirect_wreg(struct amdgpu_device *adev,
				 u32 pcie_index, u32 pcie_data,
				 u32 reg_addr, u32 reg_data)
{
	unsigned long flags;
	void __iomem *pcie_index_offset;
	void __iomem *pcie_data_offset;

	spin_lock_irqsave(&adev->pcie_idx_lock, flags);
	pcie_index_offset = (void __iomem *)adev->rmmio + pcie_index * 4;
	pcie_data_offset = (void __iomem *)adev->rmmio + pcie_data * 4;

	writel(reg_addr, pcie_index_offset);
	readl(pcie_index_offset);
	writel(reg_data, pcie_data_offset);
	readl(pcie_data_offset);
	spin_unlock_irqrestore(&adev->pcie_idx_lock, flags);
}

/**
 * amdgpu_device_indirect_wreg64 - write a 64bits indirect register address
 *
 * @adev: amdgpu_device pointer
 * @pcie_index: mmio register offset
 * @pcie_data: mmio register offset
 * @reg_addr: indirect register offset
 * @reg_data: indirect register data
 *
 */
void amdgpu_device_indirect_wreg64(struct amdgpu_device *adev,
				   u32 pcie_index, u32 pcie_data,
				   u32 reg_addr, u64 reg_data)
{
	unsigned long flags;
	void __iomem *pcie_index_offset;
	void __iomem *pcie_data_offset;

	spin_lock_irqsave(&adev->pcie_idx_lock, flags);
	pcie_index_offset = (void __iomem *)adev->rmmio + pcie_index * 4;
	pcie_data_offset = (void __iomem *)adev->rmmio + pcie_data * 4;

	/* write low 32 bits */
	writel(reg_addr, pcie_index_offset);
	readl(pcie_index_offset);
	writel((u32)(reg_data & 0xffffffffULL), pcie_data_offset);
	readl(pcie_data_offset);
	/* write high 32 bits */
	writel(reg_addr + 4, pcie_index_offset);
	readl(pcie_index_offset);
	writel((u32)(reg_data >> 32), pcie_data_offset);
	readl(pcie_data_offset);
	spin_unlock_irqrestore(&adev->pcie_idx_lock, flags);
}

/**
 * amdgpu_invalid_rreg - dummy reg read function
 *
 * @adev: amdgpu_device pointer
 * @reg: offset of register
 *
 * Dummy register read function.  Used for register blocks
 * that certain asics don't have (all asics).
 * Returns the value in the register.
 */
static uint32_t amdgpu_invalid_rreg(struct amdgpu_device *adev, uint32_t reg)
{
	DRM_ERROR("Invalid callback to read register 0x%04X\n", reg);
	BUG();
	return 0;
}

/**
 * amdgpu_invalid_wreg - dummy reg write function
 *
 * @adev: amdgpu_device pointer
 * @reg: offset of register
 * @v: value to write to the register
 *
 * Dummy register read function.  Used for register blocks
 * that certain asics don't have (all asics).
 */
static void amdgpu_invalid_wreg(struct amdgpu_device *adev, uint32_t reg, uint32_t v)
{
	DRM_ERROR("Invalid callback to write register 0x%04X with 0x%08X\n",
		  reg, v);
	BUG();
}

/**
 * amdgpu_invalid_rreg64 - dummy 64 bit reg read function
 *
 * @adev: amdgpu_device pointer
 * @reg: offset of register
 *
 * Dummy register read function.  Used for register blocks
 * that certain asics don't have (all asics).
 * Returns the value in the register.
 */
static uint64_t amdgpu_invalid_rreg64(struct amdgpu_device *adev, uint32_t reg)
{
	DRM_ERROR("Invalid callback to read 64 bit register 0x%04X\n", reg);
	BUG();
	return 0;
}

/**
 * amdgpu_invalid_wreg64 - dummy reg write function
 *
 * @adev: amdgpu_device pointer
 * @reg: offset of register
 * @v: value to write to the register
 *
 * Dummy register read function.  Used for register blocks
 * that certain asics don't have (all asics).
 */
static void amdgpu_invalid_wreg64(struct amdgpu_device *adev, uint32_t reg, uint64_t v)
{
	DRM_ERROR("Invalid callback to write 64 bit register 0x%04X with 0x%08llX\n",
		  reg, v);
	BUG();
}

/**
 * amdgpu_block_invalid_rreg - dummy reg read function
 *
 * @adev: amdgpu_device pointer
 * @block: offset of instance
 * @reg: offset of register
 *
 * Dummy register read function.  Used for register blocks
 * that certain asics don't have (all asics).
 * Returns the value in the register.
 */
static uint32_t amdgpu_block_invalid_rreg(struct amdgpu_device *adev,
					  uint32_t block, uint32_t reg)
{
	DRM_ERROR("Invalid callback to read register 0x%04X in block 0x%04X\n",
		  reg, block);
	BUG();
	return 0;
}

/**
 * amdgpu_block_invalid_wreg - dummy reg write function
 *
 * @adev: amdgpu_device pointer
 * @block: offset of instance
 * @reg: offset of register
 * @v: value to write to the register
 *
 * Dummy register read function.  Used for register blocks
 * that certain asics don't have (all asics).
 */
static void amdgpu_block_invalid_wreg(struct amdgpu_device *adev,
				      uint32_t block,
				      uint32_t reg, uint32_t v)
{
	DRM_ERROR("Invalid block callback to write register 0x%04X in block 0x%04X with 0x%08X\n",
		  reg, block, v);
	BUG();
}

/**
 * amdgpu_device_asic_init - Wrapper for atom asic_init
 *
 * @adev: amdgpu_device pointer
 *
 * Does any asic specific work and then calls atom asic init.
 */
static int amdgpu_device_asic_init(struct amdgpu_device *adev)
{
	amdgpu_asic_pre_asic_init(adev);

	return amdgpu_atom_asic_init(adev->mode_info.atom_context);
}

/**
 * amdgpu_device_vram_scratch_init - allocate the VRAM scratch page
 *
 * @adev: amdgpu_device pointer
 *
 * Allocates a scratch page of VRAM for use by various things in the
 * driver.
 */
static int amdgpu_device_vram_scratch_init(struct amdgpu_device *adev)
{
	return amdgpu_bo_create_kernel(adev, AMDGPU_GPU_PAGE_SIZE,
				       PAGE_SIZE, AMDGPU_GEM_DOMAIN_VRAM,
				       &adev->vram_scratch.robj,
				       &adev->vram_scratch.gpu_addr,
				       (void **)&adev->vram_scratch.ptr);
}

/**
 * amdgpu_device_vram_scratch_fini - Free the VRAM scratch page
 *
 * @adev: amdgpu_device pointer
 *
 * Frees the VRAM scratch page.
 */
static void amdgpu_device_vram_scratch_fini(struct amdgpu_device *adev)
{
	amdgpu_bo_free_kernel(&adev->vram_scratch.robj, NULL, NULL);
}

/**
 * amdgpu_device_program_register_sequence - program an array of registers.
 *
 * @adev: amdgpu_device pointer
 * @registers: pointer to the register array
 * @array_size: size of the register array
 *
 * Programs an array or registers with and and or masks.
 * This is a helper for setting golden registers.
 */
void amdgpu_device_program_register_sequence(struct amdgpu_device *adev,
					     const u32 *registers,
					     const u32 array_size)
{
	u32 tmp, reg, and_mask, or_mask;
	int i;

	if (array_size % 3)
		return;

	for (i = 0; i < array_size; i +=3) {
		reg = registers[i + 0];
		and_mask = registers[i + 1];
		or_mask = registers[i + 2];

		if (and_mask == 0xffffffff) {
			tmp = or_mask;
		} else {
			tmp = RREG32(reg);
			tmp &= ~and_mask;
			if (adev->family >= AMDGPU_FAMILY_AI)
				tmp |= (or_mask & and_mask);
			else
				tmp |= or_mask;
		}
		WREG32(reg, tmp);
	}
}

/**
 * amdgpu_device_pci_config_reset - reset the GPU
 *
 * @adev: amdgpu_device pointer
 *
 * Resets the GPU using the pci config reset sequence.
 * Only applicable to asics prior to vega10.
 */
void amdgpu_device_pci_config_reset(struct amdgpu_device *adev)
{
	pci_write_config_dword(adev->pdev, 0x7c, AMDGPU_ASIC_RESET_DATA);
}

/**
 * amdgpu_device_pci_reset - reset the GPU using generic PCI means
 *
 * @adev: amdgpu_device pointer
 *
 * Resets the GPU using generic pci reset interfaces (FLR, SBR, etc.).
 */
int amdgpu_device_pci_reset(struct amdgpu_device *adev)
{
	return pci_reset_function(adev->pdev);
}

/*
 * GPU doorbell aperture helpers function.
 */
/**
 * amdgpu_device_doorbell_init - Init doorbell driver information.
 *
 * @adev: amdgpu_device pointer
 *
 * Init doorbell driver information (CIK)
 * Returns 0 on success, error on failure.
 */
static int amdgpu_device_doorbell_init(struct amdgpu_device *adev)
{

	/* No doorbell on SI hardware generation */
	if (adev->asic_type < CHIP_BONAIRE) {
		adev->doorbell.base = 0;
		adev->doorbell.size = 0;
		adev->doorbell.num_doorbells = 0;
		adev->doorbell.ptr = NULL;
		return 0;
	}

	if (pci_resource_flags(adev->pdev, 2) & IORESOURCE_UNSET)
		return -EINVAL;

	amdgpu_asic_init_doorbell_index(adev);

	/* doorbell bar mapping */
	adev->doorbell.base = pci_resource_start(adev->pdev, 2);
	adev->doorbell.size = pci_resource_len(adev->pdev, 2);

	adev->doorbell.num_doorbells = min_t(u32, adev->doorbell.size / sizeof(u32),
					     adev->doorbell_index.max_assignment+1);
	if (adev->doorbell.num_doorbells == 0)
		return -EINVAL;

	/* For Vega, reserve and map two pages on doorbell BAR since SDMA
	 * paging queue doorbell use the second page. The
	 * AMDGPU_DOORBELL64_MAX_ASSIGNMENT definition assumes all the
	 * doorbells are in the first page. So with paging queue enabled,
	 * the max num_doorbells should + 1 page (0x400 in dword)
	 */
	if (adev->asic_type >= CHIP_VEGA10)
		adev->doorbell.num_doorbells += 0x400;

	adev->doorbell.ptr = ioremap(adev->doorbell.base,
				     adev->doorbell.num_doorbells *
				     sizeof(u32));
	if (adev->doorbell.ptr == NULL)
		return -ENOMEM;

	return 0;
}

/**
 * amdgpu_device_doorbell_fini - Tear down doorbell driver information.
 *
 * @adev: amdgpu_device pointer
 *
 * Tear down doorbell driver information (CIK)
 */
static void amdgpu_device_doorbell_fini(struct amdgpu_device *adev)
{
	iounmap(adev->doorbell.ptr);
	adev->doorbell.ptr = NULL;
}



/*
 * amdgpu_device_wb_*()
 * Writeback is the method by which the GPU updates special pages in memory
 * with the status of certain GPU events (fences, ring pointers,etc.).
 */

/**
 * amdgpu_device_wb_fini - Disable Writeback and free memory
 *
 * @adev: amdgpu_device pointer
 *
 * Disables Writeback and frees the Writeback memory (all asics).
 * Used at driver shutdown.
 */
static void amdgpu_device_wb_fini(struct amdgpu_device *adev)
{
	if (adev->wb.wb_obj) {
		amdgpu_bo_free_kernel(&adev->wb.wb_obj,
				      &adev->wb.gpu_addr,
				      (void **)&adev->wb.wb);
		adev->wb.wb_obj = NULL;
	}
}

/**
 * amdgpu_device_wb_init - Init Writeback driver info and allocate memory
 *
 * @adev: amdgpu_device pointer
 *
 * Initializes writeback and allocates writeback memory (all asics).
 * Used at driver startup.
 * Returns 0 on success or an -error on failure.
 */
static int amdgpu_device_wb_init(struct amdgpu_device *adev)
{
	int r;

	if (adev->wb.wb_obj == NULL) {
		/* AMDGPU_MAX_WB * sizeof(uint32_t) * 8 = AMDGPU_MAX_WB 256bit slots */
		r = amdgpu_bo_create_kernel(adev, AMDGPU_MAX_WB * sizeof(uint32_t) * 8,
					    PAGE_SIZE, AMDGPU_GEM_DOMAIN_GTT,
					    &adev->wb.wb_obj, &adev->wb.gpu_addr,
					    (void **)&adev->wb.wb);
		if (r) {
			dev_warn(adev->dev, "(%d) create WB bo failed\n", r);
			return r;
		}

		adev->wb.num_wb = AMDGPU_MAX_WB;
		memset(&adev->wb.used, 0, sizeof(adev->wb.used));

		/* clear wb memory */
		memset((char *)adev->wb.wb, 0, AMDGPU_MAX_WB * sizeof(uint32_t) * 8);
	}

	return 0;
}

/**
 * amdgpu_device_wb_get - Allocate a wb entry
 *
 * @adev: amdgpu_device pointer
 * @wb: wb index
 *
 * Allocate a wb slot for use by the driver (all asics).
 * Returns 0 on success or -EINVAL on failure.
 */
int amdgpu_device_wb_get(struct amdgpu_device *adev, u32 *wb)
{
	unsigned long offset = find_first_zero_bit(adev->wb.used, adev->wb.num_wb);

	if (offset < adev->wb.num_wb) {
		__set_bit(offset, adev->wb.used);
		*wb = offset << 3; /* convert to dw offset */
		return 0;
	} else {
		return -EINVAL;
	}
}

/**
 * amdgpu_device_wb_free - Free a wb entry
 *
 * @adev: amdgpu_device pointer
 * @wb: wb index
 *
 * Free a wb slot allocated for use by the driver (all asics)
 */
void amdgpu_device_wb_free(struct amdgpu_device *adev, u32 wb)
{
	wb >>= 3;
	if (wb < adev->wb.num_wb)
		__clear_bit(wb, adev->wb.used);
}

#ifdef AMDKCL_ENABLE_RESIZE_FB_BAR
/**
 * amdgpu_device_resize_fb_bar - try to resize FB BAR
 *
 * @adev: amdgpu_device pointer
 *
 * Try to resize FB BAR to make all VRAM CPU accessible. We try very hard not
 * to fail, but if any of the BARs is not accessible after the size we abort
 * driver loading by returning -ENODEV.
 */
int amdgpu_device_resize_fb_bar(struct amdgpu_device *adev)
{
	int rbar_size = pci_rebar_bytes_to_size(adev->gmc.real_vram_size);
	struct pci_bus *root;
	struct resource *res;
	unsigned i;
	u16 cmd;
	int r;

	/* Bypass for VF */
	if (amdgpu_sriov_vf(adev))
		return 0;

	/* skip if the bios has already enabled large BAR */
	if (adev->gmc.real_vram_size &&
	    (pci_resource_len(adev->pdev, 0) >= adev->gmc.real_vram_size))
		return 0;

	/* Check if the root BUS has 64bit memory resources */
	root = adev->pdev->bus;
	while (root->parent)
		root = root->parent;

	pci_bus_for_each_resource(root, res, i) {
		if (res && res->flags & (IORESOURCE_MEM | IORESOURCE_MEM_64) &&
		    res->start > 0x100000000ull)
			break;
	}

	/* Trying to resize is pointless without a root hub window above 4GB */
	if (!res)
		return 0;

	/* Limit the BAR size to what is available */
	rbar_size = min(fls(pci_rebar_get_possible_sizes(adev->pdev, 0)) - 1,
			rbar_size);

	/* Disable memory decoding while we change the BAR addresses and size */
	pci_read_config_word(adev->pdev, PCI_COMMAND, &cmd);
	pci_write_config_word(adev->pdev, PCI_COMMAND,
			      cmd & ~PCI_COMMAND_MEMORY);

	/* Free the VRAM and doorbell BAR, we most likely need to move both. */
	amdgpu_device_doorbell_fini(adev);
	if (adev->asic_type >= CHIP_BONAIRE)
		pci_release_resource(adev->pdev, 2);

	pci_release_resource(adev->pdev, 0);

	r = pci_resize_resource(adev->pdev, 0, rbar_size);
	if (r == -ENOSPC)
		DRM_INFO("Not enough PCI address space for a large BAR.");
	else if (r && r != -ENOTSUPP)
		DRM_ERROR("Problem resizing BAR0 (%d).", r);

	pci_assign_unassigned_bus_resources(adev->pdev->bus);

	/* When the doorbell or fb BAR isn't available we have no chance of
	 * using the device.
	 */
	r = amdgpu_device_doorbell_init(adev);
	if (r || (pci_resource_flags(adev->pdev, 0) & IORESOURCE_UNSET))
		return -ENODEV;

	pci_write_config_word(adev->pdev, PCI_COMMAND, cmd);

	return 0;
}
#endif

/*
 * GPU helpers function.
 */
/**
 * amdgpu_device_need_post - check if the hw need post or not
 *
 * @adev: amdgpu_device pointer
 *
 * Check if the asic has been initialized (all asics) at driver startup
 * or post is needed if  hw reset is performed.
 * Returns true if need or false if not.
 */
bool amdgpu_device_need_post(struct amdgpu_device *adev)
{
	uint32_t reg;

	if (amdgpu_sriov_vf(adev))
		return false;

	if (amdgpu_passthrough(adev)) {
		/* for FIJI: In whole GPU pass-through virtualization case, after VM reboot
		 * some old smc fw still need driver do vPost otherwise gpu hang, while
		 * those smc fw version above 22.15 doesn't have this flaw, so we force
		 * vpost executed for smc version below 22.15
		 */
		if (adev->asic_type == CHIP_FIJI) {
			int err;
			uint32_t fw_ver;
			err = request_firmware(&adev->pm.fw, "amdgpu/fiji_smc.bin", adev->dev);
			/* force vPost if error occured */
			if (err)
				return true;

			fw_ver = *((uint32_t *)adev->pm.fw->data + 69);
			if (fw_ver < 0x00160e00)
				return true;
		}
	}

	/* Don't post if we need to reset whole hive on init */
	if (adev->gmc.xgmi.pending_reset)
		return false;

	if (adev->has_hw_reset) {
		adev->has_hw_reset = false;
		return true;
	}

	/* bios scratch used on CIK+ */
	if (adev->asic_type >= CHIP_BONAIRE)
		return amdgpu_atombios_scratch_need_asic_init(adev);

	/* check MEM_SIZE for older asics */
	reg = amdgpu_asic_get_config_memsize(adev);

	if ((reg != 0) && (reg != 0xffffffff))
		return false;

	return true;
}

/* if we get transitioned to only one device, take VGA back */
/**
 * amdgpu_device_vga_set_decode - enable/disable vga decode
 *
 * @cookie: amdgpu_device pointer
 * @state: enable/disable vga decode
 *
 * Enable/disable vga decode (all asics).
 * Returns VGA resource flags.
 */
static unsigned int amdgpu_device_vga_set_decode(void *cookie, bool state)
{
	struct amdgpu_device *adev = cookie;
	amdgpu_asic_set_vga_state(adev, state);
	if (state)
		return VGA_RSRC_LEGACY_IO | VGA_RSRC_LEGACY_MEM |
		       VGA_RSRC_NORMAL_IO | VGA_RSRC_NORMAL_MEM;
	else
		return VGA_RSRC_NORMAL_IO | VGA_RSRC_NORMAL_MEM;
}

/**
 * amdgpu_device_check_block_size - validate the vm block size
 *
 * @adev: amdgpu_device pointer
 *
 * Validates the vm block size specified via module parameter.
 * The vm block size defines number of bits in page table versus page directory,
 * a page is 4KB so we have 12 bits offset, minimum 9 bits in the
 * page table and the remaining bits are in the page directory.
 */
static void amdgpu_device_check_block_size(struct amdgpu_device *adev)
{
	/* defines number of bits in page table versus page directory,
	 * a page is 4KB so we have 12 bits offset, minimum 9 bits in the
	 * page table and the remaining bits are in the page directory */
	if (amdgpu_vm_block_size == -1)
		return;

	if (amdgpu_vm_block_size < 9) {
		dev_warn(adev->dev, "VM page table size (%d) too small\n",
			 amdgpu_vm_block_size);
		amdgpu_vm_block_size = -1;
	}
}

/**
 * amdgpu_device_check_vm_size - validate the vm size
 *
 * @adev: amdgpu_device pointer
 *
 * Validates the vm size in GB specified via module parameter.
 * The VM size is the size of the GPU virtual memory space in GB.
 */
static void amdgpu_device_check_vm_size(struct amdgpu_device *adev)
{
	/* no need to check the default value */
	if (amdgpu_vm_size == -1)
		return;

	if (amdgpu_vm_size < 1) {
		dev_warn(adev->dev, "VM size (%d) too small, min is 1GB\n",
			 amdgpu_vm_size);
		amdgpu_vm_size = -1;
	}
}

static void amdgpu_device_check_smu_prv_buffer_size(struct amdgpu_device *adev)
{
	struct sysinfo si;
	bool is_os_64 = (sizeof(void *) == 8);
	uint64_t total_memory;
	uint64_t dram_size_seven_GB = 0x1B8000000;
	uint64_t dram_size_three_GB = 0xB8000000;

	if (amdgpu_smu_memory_pool_size == 0)
		return;

	if (!is_os_64) {
		DRM_WARN("Not 64-bit OS, feature not supported\n");
		goto def_value;
	}
	si_meminfo(&si);
	total_memory = (uint64_t)si.totalram * si.mem_unit;

	if ((amdgpu_smu_memory_pool_size == 1) ||
		(amdgpu_smu_memory_pool_size == 2)) {
		if (total_memory < dram_size_three_GB)
			goto def_value1;
	} else if ((amdgpu_smu_memory_pool_size == 4) ||
		(amdgpu_smu_memory_pool_size == 8)) {
		if (total_memory < dram_size_seven_GB)
			goto def_value1;
	} else {
		DRM_WARN("Smu memory pool size not supported\n");
		goto def_value;
	}
	adev->pm.smu_prv_buffer_size = amdgpu_smu_memory_pool_size << 28;

	return;

def_value1:
	DRM_WARN("No enough system memory\n");
def_value:
	adev->pm.smu_prv_buffer_size = 0;
}

static int amdgpu_device_init_apu_flags(struct amdgpu_device *adev)
{
	if (!(adev->flags & AMD_IS_APU) ||
	    adev->asic_type < CHIP_RAVEN)
		return 0;

	switch (adev->asic_type) {
	case CHIP_RAVEN:
		if (adev->pdev->device == 0x15dd)
			adev->apu_flags |= AMD_APU_IS_RAVEN;
		if (adev->pdev->device == 0x15d8)
			adev->apu_flags |= AMD_APU_IS_PICASSO;
		break;
	case CHIP_RENOIR:
		if ((adev->pdev->device == 0x1636) ||
		    (adev->pdev->device == 0x164c))
			adev->apu_flags |= AMD_APU_IS_RENOIR;
		else
			adev->apu_flags |= AMD_APU_IS_GREEN_SARDINE;
		break;
	case CHIP_VANGOGH:
		adev->apu_flags |= AMD_APU_IS_VANGOGH;
		break;
	case CHIP_YELLOW_CARP:
		break;
	case CHIP_CYAN_SKILLFISH:
		if ((adev->pdev->device == 0x13FE) ||
		    (adev->pdev->device == 0x143F))
			adev->apu_flags |= AMD_APU_IS_CYAN_SKILLFISH2;
		break;
	default:
		break;
	}

	return 0;
}

/**
 * amdgpu_device_check_arguments - validate module params
 *
 * @adev: amdgpu_device pointer
 *
 * Validates certain module parameters and updates
 * the associated values used by the driver (all asics).
 */
static int amdgpu_device_check_arguments(struct amdgpu_device *adev)
{
	if (amdgpu_sched_jobs < 4) {
		dev_warn(adev->dev, "sched jobs (%d) must be at least 4\n",
			 amdgpu_sched_jobs);
		amdgpu_sched_jobs = 4;
	} else if (!is_power_of_2(amdgpu_sched_jobs)){
		dev_warn(adev->dev, "sched jobs (%d) must be a power of 2\n",
			 amdgpu_sched_jobs);
		amdgpu_sched_jobs = roundup_pow_of_two(amdgpu_sched_jobs);
	}

	if (amdgpu_gart_size != -1 && amdgpu_gart_size < 32) {
		/* gart size must be greater or equal to 32M */
		dev_warn(adev->dev, "gart size (%d) too small\n",
			 amdgpu_gart_size);
		amdgpu_gart_size = -1;
	}

	if (amdgpu_gtt_size != -1 && amdgpu_gtt_size < 32) {
		/* gtt size must be greater or equal to 32M */
		dev_warn(adev->dev, "gtt size (%d) too small\n",
				 amdgpu_gtt_size);
		amdgpu_gtt_size = -1;
	}

	/* valid range is between 4 and 9 inclusive */
	if (amdgpu_vm_fragment_size != -1 &&
	    (amdgpu_vm_fragment_size > 9 || amdgpu_vm_fragment_size < 4)) {
		dev_warn(adev->dev, "valid range is between 4 and 9\n");
		amdgpu_vm_fragment_size = -1;
	}

	if (amdgpu_sched_hw_submission < 2) {
		dev_warn(adev->dev, "sched hw submission jobs (%d) must be at least 2\n",
			 amdgpu_sched_hw_submission);
		amdgpu_sched_hw_submission = 2;
	} else if (!is_power_of_2(amdgpu_sched_hw_submission)) {
		dev_warn(adev->dev, "sched hw submission jobs (%d) must be a power of 2\n",
			 amdgpu_sched_hw_submission);
		amdgpu_sched_hw_submission = roundup_pow_of_two(amdgpu_sched_hw_submission);
	}

	amdgpu_device_check_smu_prv_buffer_size(adev);

	amdgpu_device_check_vm_size(adev);

	amdgpu_device_check_block_size(adev);

	adev->firmware.load_type = amdgpu_ucode_get_load_type(adev, amdgpu_fw_load_type);
	amdgpu_direct_gma_size = min(amdgpu_direct_gma_size, 96);

	amdgpu_gmc_tmz_set(adev);

	amdgpu_gmc_noretry_set(adev);

	return 0;
}

/**
 * amdgpu_switcheroo_set_state - set switcheroo state
 *
 * @pdev: pci dev pointer
 * @state: vga_switcheroo state
 *
 * Callback for the switcheroo driver.  Suspends or resumes the
 * the asics before or after it is powered up using ACPI methods.
 */
static void amdgpu_switcheroo_set_state(struct pci_dev *pdev,
					enum vga_switcheroo_state state)
{
	struct drm_device *dev = pci_get_drvdata(pdev);
	int r;

	if (amdgpu_device_supports_px(dev) && state == VGA_SWITCHEROO_OFF)
		return;

	if (state == VGA_SWITCHEROO_ON) {
		pr_info("switched on\n");
		/* don't suspend or resume card normally */
		dev->switch_power_state = DRM_SWITCH_POWER_CHANGING;

		pci_set_power_state(pdev, PCI_D0);
		amdgpu_device_load_pci_state(pdev);
		r = pci_enable_device(pdev);
		if (r)
			DRM_WARN("pci_enable_device failed (%d)\n", r);
		amdgpu_device_resume(dev, true);

		dev->switch_power_state = DRM_SWITCH_POWER_ON;
	} else {
		pr_info("switched off\n");
		dev->switch_power_state = DRM_SWITCH_POWER_CHANGING;
		amdgpu_device_suspend(dev, true);
		amdgpu_device_cache_pci_state(pdev);
		/* Shut down the device */
		pci_disable_device(pdev);
		pci_set_power_state(pdev, PCI_D3cold);
		dev->switch_power_state = DRM_SWITCH_POWER_OFF;
	}
}

/**
 * amdgpu_switcheroo_can_switch - see if switcheroo state can change
 *
 * @pdev: pci dev pointer
 *
 * Callback for the switcheroo driver.  Check of the switcheroo
 * state can be changed.
 * Returns true if the state can be changed, false if not.
 */
static bool amdgpu_switcheroo_can_switch(struct pci_dev *pdev)
{
	struct drm_device *dev = pci_get_drvdata(pdev);

	/*
	* FIXME: open_count is protected by drm_global_mutex but that would lead to
	* locking inversion with the driver load path. And the access here is
	* completely racy anyway. So don't bother with locking for now.
	*/
#ifdef HAVE_DRM_DEVICE_OPEN_COUNT_INT
	return dev->open_count == 0;
#else
	return atomic_read(&dev->open_count) == 0;
#endif
}

static const struct vga_switcheroo_client_ops amdgpu_switcheroo_ops = {
	.set_gpu_state = amdgpu_switcheroo_set_state,
	.reprobe = NULL,
	.can_switch = amdgpu_switcheroo_can_switch,
};

/**
 * amdgpu_device_ip_set_clockgating_state - set the CG state
 *
 * @dev: amdgpu_device pointer
 * @block_type: Type of hardware IP (SMU, GFX, UVD, etc.)
 * @state: clockgating state (gate or ungate)
 *
 * Sets the requested clockgating state for all instances of
 * the hardware IP specified.
 * Returns the error code from the last instance.
 */
int amdgpu_device_ip_set_clockgating_state(void *dev,
					   enum amd_ip_block_type block_type,
					   enum amd_clockgating_state state)
{
	struct amdgpu_device *adev = dev;
	int i, r = 0;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].version->type != block_type)
			continue;
		if (!adev->ip_blocks[i].version->funcs->set_clockgating_state)
			continue;
		r = adev->ip_blocks[i].version->funcs->set_clockgating_state(
			(void *)adev, state);
		if (r)
			DRM_ERROR("set_clockgating_state of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
	}
	return r;
}

/**
 * amdgpu_device_ip_set_powergating_state - set the PG state
 *
 * @dev: amdgpu_device pointer
 * @block_type: Type of hardware IP (SMU, GFX, UVD, etc.)
 * @state: powergating state (gate or ungate)
 *
 * Sets the requested powergating state for all instances of
 * the hardware IP specified.
 * Returns the error code from the last instance.
 */
int amdgpu_device_ip_set_powergating_state(void *dev,
					   enum amd_ip_block_type block_type,
					   enum amd_powergating_state state)
{
	struct amdgpu_device *adev = dev;
	int i, r = 0;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].version->type != block_type)
			continue;
		if (!adev->ip_blocks[i].version->funcs->set_powergating_state)
			continue;
		r = adev->ip_blocks[i].version->funcs->set_powergating_state(
			(void *)adev, state);
		if (r)
			DRM_ERROR("set_powergating_state of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
	}
	return r;
}

/**
 * amdgpu_device_ip_get_clockgating_state - get the CG state
 *
 * @adev: amdgpu_device pointer
 * @flags: clockgating feature flags
 *
 * Walks the list of IPs on the device and updates the clockgating
 * flags for each IP.
 * Updates @flags with the feature flags for each hardware IP where
 * clockgating is enabled.
 */
void amdgpu_device_ip_get_clockgating_state(struct amdgpu_device *adev,
					    u32 *flags)
{
	int i;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].version->funcs->get_clockgating_state)
			adev->ip_blocks[i].version->funcs->get_clockgating_state((void *)adev, flags);
	}
}

/**
 * amdgpu_device_ip_wait_for_idle - wait for idle
 *
 * @adev: amdgpu_device pointer
 * @block_type: Type of hardware IP (SMU, GFX, UVD, etc.)
 *
 * Waits for the request hardware IP to be idle.
 * Returns 0 for success or a negative error code on failure.
 */
int amdgpu_device_ip_wait_for_idle(struct amdgpu_device *adev,
				   enum amd_ip_block_type block_type)
{
	int i, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].version->type == block_type) {
			r = adev->ip_blocks[i].version->funcs->wait_for_idle((void *)adev);
			if (r)
				return r;
			break;
		}
	}
	return 0;

}

/**
 * amdgpu_device_ip_is_idle - is the hardware IP idle
 *
 * @adev: amdgpu_device pointer
 * @block_type: Type of hardware IP (SMU, GFX, UVD, etc.)
 *
 * Check if the hardware IP is idle or not.
 * Returns true if it the IP is idle, false if not.
 */
bool amdgpu_device_ip_is_idle(struct amdgpu_device *adev,
			      enum amd_ip_block_type block_type)
{
	int i;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].version->type == block_type)
			return adev->ip_blocks[i].version->funcs->is_idle((void *)adev);
	}
	return true;

}

/**
 * amdgpu_device_ip_get_ip_block - get a hw IP pointer
 *
 * @adev: amdgpu_device pointer
 * @type: Type of hardware IP (SMU, GFX, UVD, etc.)
 *
 * Returns a pointer to the hardware IP block structure
 * if it exists for the asic, otherwise NULL.
 */
struct amdgpu_ip_block *
amdgpu_device_ip_get_ip_block(struct amdgpu_device *adev,
			      enum amd_ip_block_type type)
{
	int i;

	for (i = 0; i < adev->num_ip_blocks; i++)
		if (adev->ip_blocks[i].version->type == type)
			return &adev->ip_blocks[i];

	return NULL;
}

/**
 * amdgpu_device_ip_block_version_cmp
 *
 * @adev: amdgpu_device pointer
 * @type: enum amd_ip_block_type
 * @major: major version
 * @minor: minor version
 *
 * return 0 if equal or greater
 * return 1 if smaller or the ip_block doesn't exist
 */
int amdgpu_device_ip_block_version_cmp(struct amdgpu_device *adev,
				       enum amd_ip_block_type type,
				       u32 major, u32 minor)
{
	struct amdgpu_ip_block *ip_block = amdgpu_device_ip_get_ip_block(adev, type);

	if (ip_block && ((ip_block->version->major > major) ||
			((ip_block->version->major == major) &&
			(ip_block->version->minor >= minor))))
		return 0;

	return 1;
}

/**
 * amdgpu_device_ip_block_add
 *
 * @adev: amdgpu_device pointer
 * @ip_block_version: pointer to the IP to add
 *
 * Adds the IP block driver information to the collection of IPs
 * on the asic.
 */
int amdgpu_device_ip_block_add(struct amdgpu_device *adev,
			       const struct amdgpu_ip_block_version *ip_block_version)
{
	if (!ip_block_version)
		return -EINVAL;

	switch (ip_block_version->type) {
	case AMD_IP_BLOCK_TYPE_VCN:
		if (adev->harvest_ip_mask & AMD_HARVEST_IP_VCN_MASK)
			return 0;
		break;
	case AMD_IP_BLOCK_TYPE_JPEG:
		if (adev->harvest_ip_mask & AMD_HARVEST_IP_JPEG_MASK)
			return 0;
		break;
	default:
		break;
	}

	DRM_INFO("add ip block number %d <%s>\n", adev->num_ip_blocks,
		  ip_block_version->funcs->name);

	adev->ip_blocks[adev->num_ip_blocks++].version = ip_block_version;

	return 0;
}

/**
 * amdgpu_device_enable_virtual_display - enable virtual display feature
 *
 * @adev: amdgpu_device pointer
 *
 * Enabled the virtual display feature if the user has enabled it via
 * the module parameter virtual_display.  This feature provides a virtual
 * display hardware on headless boards or in virtualized environments.
 * This function parses and validates the configuration string specified by
 * the user and configues the virtual display configuration (number of
 * virtual connectors, crtcs, etc.) specified.
 */
static void amdgpu_device_enable_virtual_display(struct amdgpu_device *adev)
{
	adev->enable_virtual_display = false;

	if (amdgpu_virtual_display) {
		const char *pci_address_name = pci_name(adev->pdev);
		char *pciaddstr, *pciaddstr_tmp, *pciaddname_tmp, *pciaddname;

		pciaddstr = kstrdup(amdgpu_virtual_display, GFP_KERNEL);
		pciaddstr_tmp = pciaddstr;
		while ((pciaddname_tmp = strsep(&pciaddstr_tmp, ";"))) {
			pciaddname = strsep(&pciaddname_tmp, ",");
			if (!strcmp("all", pciaddname)
			    || !strcmp(pci_address_name, pciaddname)) {
				long num_crtc;
				int res = -1;

				adev->enable_virtual_display = true;

				if (pciaddname_tmp)
					res = kstrtol(pciaddname_tmp, 10,
						      &num_crtc);

				if (!res) {
					if (num_crtc < 1)
						num_crtc = 1;
					if (num_crtc > 6)
						num_crtc = 6;
					adev->mode_info.num_crtc = num_crtc;
				} else {
					adev->mode_info.num_crtc = 1;
				}
				break;
			}
		}

		DRM_INFO("virtual display string:%s, %s:virtual_display:%d, num_crtc:%d\n",
			 amdgpu_virtual_display, pci_address_name,
			 adev->enable_virtual_display, adev->mode_info.num_crtc);

		kfree(pciaddstr);
	}
}

/**
 * amdgpu_device_parse_gpu_info_fw - parse gpu info firmware
 *
 * @adev: amdgpu_device pointer
 *
 * Parses the asic configuration parameters specified in the gpu info
 * firmware and makes them availale to the driver for use in configuring
 * the asic.
 * Returns 0 on success, -EINVAL on failure.
 */
static int amdgpu_device_parse_gpu_info_fw(struct amdgpu_device *adev)
{
	const char *chip_name;
	char fw_name[40];
	int err;
	const struct gpu_info_firmware_header_v1_0 *hdr;

	adev->firmware.gpu_info_fw = NULL;

	if (adev->mman.discovery_bin) {
		amdgpu_discovery_get_gfx_info(adev);

		/*
		 * FIXME: The bounding box is still needed by Navi12, so
		 * temporarily read it from gpu_info firmware. Should be droped
		 * when DAL no longer needs it.
		 */
		if (adev->asic_type != CHIP_NAVI12)
			return 0;
	}

	switch (adev->asic_type) {
#ifdef CONFIG_DRM_AMDGPU_SI
	case CHIP_VERDE:
	case CHIP_TAHITI:
	case CHIP_PITCAIRN:
	case CHIP_OLAND:
	case CHIP_HAINAN:
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
	case CHIP_BONAIRE:
	case CHIP_HAWAII:
	case CHIP_KAVERI:
	case CHIP_KABINI:
	case CHIP_MULLINS:
#endif
	case CHIP_TOPAZ:
	case CHIP_TONGA:
	case CHIP_FIJI:
	case CHIP_POLARIS10:
	case CHIP_POLARIS11:
	case CHIP_POLARIS12:
	case CHIP_VEGAM:
	case CHIP_CARRIZO:
	case CHIP_STONEY:
	case CHIP_VEGA20:
	case CHIP_ALDEBARAN:
	case CHIP_SIENNA_CICHLID:
	case CHIP_NAVY_FLOUNDER:
	case CHIP_DIMGREY_CAVEFISH:
	case CHIP_BEIGE_GOBY:
	default:
		return 0;
	case CHIP_VEGA10:
		chip_name = "vega10";
		break;
	case CHIP_VEGA12:
		chip_name = "vega12";
		break;
	case CHIP_RAVEN:
		if (adev->apu_flags & AMD_APU_IS_RAVEN2)
			chip_name = "raven2";
		else if (adev->apu_flags & AMD_APU_IS_PICASSO)
			chip_name = "picasso";
		else
			chip_name = "raven";
		break;
	case CHIP_ARCTURUS:
		chip_name = "arcturus";
		break;
	case CHIP_RENOIR:
		if (adev->apu_flags & AMD_APU_IS_RENOIR)
			chip_name = "renoir";
		else
			chip_name = "green_sardine";
		break;
	case CHIP_NAVI10:
		chip_name = "navi10";
		break;
	case CHIP_NAVI14:
		chip_name = "navi14";
		break;
	case CHIP_NAVI12:
		chip_name = "navi12";
		break;
	case CHIP_VANGOGH:
		chip_name = "vangogh";
		break;
	case CHIP_YELLOW_CARP:
		chip_name = "yellow_carp";
		break;
	}

	snprintf(fw_name, sizeof(fw_name), "amdgpu/%s_gpu_info.bin", chip_name);
	err = request_firmware(&adev->firmware.gpu_info_fw, fw_name, adev->dev);
	if (err) {
		dev_err(adev->dev,
			"Failed to load gpu_info firmware \"%s\"\n",
			fw_name);
		goto out;
	}
	err = amdgpu_ucode_validate(adev->firmware.gpu_info_fw);
	if (err) {
		dev_err(adev->dev,
			"Failed to validate gpu_info firmware \"%s\"\n",
			fw_name);
		goto out;
	}

	hdr = (const struct gpu_info_firmware_header_v1_0 *)adev->firmware.gpu_info_fw->data;
	amdgpu_ucode_print_gpu_info_hdr(&hdr->header);

	switch (hdr->version_major) {
	case 1:
	{
		const struct gpu_info_firmware_v1_0 *gpu_info_fw =
			(const struct gpu_info_firmware_v1_0 *)(adev->firmware.gpu_info_fw->data +
								le32_to_cpu(hdr->header.ucode_array_offset_bytes));

		/*
		 * Should be droped when DAL no longer needs it.
		 */
		if (adev->asic_type == CHIP_NAVI12)
			goto parse_soc_bounding_box;

		adev->gfx.config.max_shader_engines = le32_to_cpu(gpu_info_fw->gc_num_se);
		adev->gfx.config.max_cu_per_sh = le32_to_cpu(gpu_info_fw->gc_num_cu_per_sh);
		adev->gfx.config.max_sh_per_se = le32_to_cpu(gpu_info_fw->gc_num_sh_per_se);
		adev->gfx.config.max_backends_per_se = le32_to_cpu(gpu_info_fw->gc_num_rb_per_se);
		adev->gfx.config.max_texture_channel_caches =
			le32_to_cpu(gpu_info_fw->gc_num_tccs);
		adev->gfx.config.max_gprs = le32_to_cpu(gpu_info_fw->gc_num_gprs);
		adev->gfx.config.max_gs_threads = le32_to_cpu(gpu_info_fw->gc_num_max_gs_thds);
		adev->gfx.config.gs_vgt_table_depth = le32_to_cpu(gpu_info_fw->gc_gs_table_depth);
		adev->gfx.config.gs_prim_buffer_depth = le32_to_cpu(gpu_info_fw->gc_gsprim_buff_depth);
		adev->gfx.config.double_offchip_lds_buf =
			le32_to_cpu(gpu_info_fw->gc_double_offchip_lds_buffer);
		adev->gfx.cu_info.wave_front_size = le32_to_cpu(gpu_info_fw->gc_wave_size);
		adev->gfx.cu_info.max_waves_per_simd =
			le32_to_cpu(gpu_info_fw->gc_max_waves_per_simd);
		adev->gfx.cu_info.max_scratch_slots_per_cu =
			le32_to_cpu(gpu_info_fw->gc_max_scratch_slots_per_cu);
		adev->gfx.cu_info.lds_size = le32_to_cpu(gpu_info_fw->gc_lds_size);
		if (hdr->version_minor >= 1) {
			const struct gpu_info_firmware_v1_1 *gpu_info_fw =
				(const struct gpu_info_firmware_v1_1 *)(adev->firmware.gpu_info_fw->data +
									le32_to_cpu(hdr->header.ucode_array_offset_bytes));
			adev->gfx.config.num_sc_per_sh =
				le32_to_cpu(gpu_info_fw->num_sc_per_sh);
			adev->gfx.config.num_packer_per_sc =
				le32_to_cpu(gpu_info_fw->num_packer_per_sc);
		}

parse_soc_bounding_box:
#ifdef CONFIG_DRM_AMD_DC_DCN2_x
		/*
		 * soc bounding box info is not integrated in disocovery table,
		 * we always need to parse it from gpu info firmware if needed.
		 */
		if (hdr->version_minor == 2) {
			const struct gpu_info_firmware_v1_2 *gpu_info_fw =
				(const struct gpu_info_firmware_v1_2 *)(adev->firmware.gpu_info_fw->data +
									le32_to_cpu(hdr->header.ucode_array_offset_bytes));
			adev->dm.soc_bounding_box = &gpu_info_fw->soc_bounding_box;
		}
#endif
		break;
	}
	default:
		dev_err(adev->dev,
			"Unsupported gpu_info table %d\n", hdr->header.ucode_version);
		err = -EINVAL;
		goto out;
	}
out:
	return err;
}

/**
 * amdgpu_device_ip_early_init - run early init for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * Early initialization pass for hardware IPs.  The hardware IPs that make
 * up each asic are discovered each IP's early_init callback is run.  This
 * is the first stage in initializing the asic.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_early_init(struct amdgpu_device *adev)
{
	struct drm_device *dev = adev_to_drm(adev);
	struct pci_dev *parent;
	int i, r;

	amdgpu_device_enable_virtual_display(adev);

	if (amdgpu_sriov_vf(adev)) {
		r = amdgpu_virt_request_full_gpu(adev, true);
		if (r)
			return r;
	}

	switch (adev->asic_type) {
#ifdef CONFIG_DRM_AMDGPU_SI
	case CHIP_VERDE:
	case CHIP_TAHITI:
	case CHIP_PITCAIRN:
	case CHIP_OLAND:
	case CHIP_HAINAN:
		adev->family = AMDGPU_FAMILY_SI;
		r = si_set_ip_blocks(adev);
		if (r)
			return r;
		break;
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
	case CHIP_BONAIRE:
	case CHIP_HAWAII:
	case CHIP_KAVERI:
	case CHIP_KABINI:
	case CHIP_MULLINS:
		if (adev->flags & AMD_IS_APU)
			adev->family = AMDGPU_FAMILY_KV;
		else
			adev->family = AMDGPU_FAMILY_CI;

		r = cik_set_ip_blocks(adev);
		if (r)
			return r;
		break;
#endif
	case CHIP_TOPAZ:
	case CHIP_TONGA:
	case CHIP_FIJI:
	case CHIP_POLARIS10:
	case CHIP_POLARIS11:
	case CHIP_POLARIS12:
	case CHIP_VEGAM:
	case CHIP_CARRIZO:
	case CHIP_STONEY:
		if (adev->flags & AMD_IS_APU)
			adev->family = AMDGPU_FAMILY_CZ;
		else
			adev->family = AMDGPU_FAMILY_VI;

		r = vi_set_ip_blocks(adev);
		if (r)
			return r;
		break;
	default:
		r = amdgpu_discovery_set_ip_blocks(adev);
		if (r)
			return r;
		break;
	}

	if (amdgpu_has_atpx() &&
	    (amdgpu_is_atpx_hybrid() ||
	     amdgpu_has_atpx_dgpu_power_cntl()) &&
	    ((adev->flags & AMD_IS_APU) == 0) &&
	    !pci_is_thunderbolt_attached(to_pci_dev(dev->dev)))
		adev->flags |= AMD_IS_PX;

	parent = pci_upstream_bridge(adev->pdev);
	adev->has_pr3 = parent ? pci_pr3_present(parent) : false;

	amdgpu_amdkfd_device_probe(adev);

	adev->pm.pp_feature = amdgpu_pp_feature_mask;
	if (amdgpu_sriov_vf(adev) || sched_policy == KFD_SCHED_POLICY_NO_HWS)
		adev->pm.pp_feature &= ~PP_GFXOFF_MASK;
	if (amdgpu_sriov_vf(adev) && adev->asic_type == CHIP_SIENNA_CICHLID)
		adev->pm.pp_feature &= ~PP_OVERDRIVE_MASK;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if ((amdgpu_ip_block_mask & (1 << i)) == 0) {
			DRM_ERROR("disabled ip block: %d <%s>\n",
				  i, adev->ip_blocks[i].version->funcs->name);
			adev->ip_blocks[i].status.valid = false;
		} else {
			if (adev->ip_blocks[i].version->funcs->early_init) {
				r = adev->ip_blocks[i].version->funcs->early_init((void *)adev);
				if (r == -ENOENT) {
					adev->ip_blocks[i].status.valid = false;
				} else if (r) {
					DRM_ERROR("early_init of IP block <%s> failed %d\n",
						  adev->ip_blocks[i].version->funcs->name, r);
					return r;
				} else {
					adev->ip_blocks[i].status.valid = true;
				}
			} else {
				adev->ip_blocks[i].status.valid = true;
			}
		}
		/* get the vbios after the asic_funcs are set up */
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_COMMON) {
			r = amdgpu_device_parse_gpu_info_fw(adev);
			if (r)
				return r;

			/* Read BIOS */
			if (!amdgpu_get_bios(adev))
				return -EINVAL;

			r = amdgpu_atombios_init(adev);
			if (r) {
				dev_err(adev->dev, "amdgpu_atombios_init failed\n");
				amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_ATOMBIOS_INIT_FAIL, 0, 0);
				return r;
			}

			/*get pf2vf msg info at it's earliest time*/
			if (amdgpu_sriov_vf(adev))
				amdgpu_virt_init_data_exchange(adev);

		}
	}

	adev->cg_flags &= amdgpu_cg_mask;
	adev->pg_flags &= amdgpu_pg_mask;

	return 0;
}

static int amdgpu_device_ip_hw_init_phase1(struct amdgpu_device *adev)
{
	int i, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.sw)
			continue;
		if (adev->ip_blocks[i].status.hw)
			continue;
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_COMMON ||
		    (amdgpu_sriov_vf(adev) && (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_PSP)) ||
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_IH) {
			r = adev->ip_blocks[i].version->funcs->hw_init(adev);
			if (r) {
				DRM_ERROR("hw_init of IP block <%s> failed %d\n",
					  adev->ip_blocks[i].version->funcs->name, r);
				return r;
			}
			adev->ip_blocks[i].status.hw = true;
		}
	}

	return 0;
}

static int amdgpu_device_ip_hw_init_phase2(struct amdgpu_device *adev)
{
	int i, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.sw)
			continue;
		if (adev->ip_blocks[i].status.hw)
			continue;
		r = adev->ip_blocks[i].version->funcs->hw_init(adev);
		if (r) {
			DRM_ERROR("hw_init of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
			return r;
		}
		adev->ip_blocks[i].status.hw = true;
	}

	return 0;
}

static int amdgpu_device_fw_loading(struct amdgpu_device *adev)
{
	int r = 0;
	int i;
	uint32_t smu_version;

	if (adev->asic_type >= CHIP_VEGA10) {
		for (i = 0; i < adev->num_ip_blocks; i++) {
			if (adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_PSP)
				continue;

			if (!adev->ip_blocks[i].status.sw)
				continue;

			/* no need to do the fw loading again if already done*/
			if (adev->ip_blocks[i].status.hw == true)
				break;

			if (amdgpu_in_reset(adev) || adev->in_suspend) {
				r = adev->ip_blocks[i].version->funcs->resume(adev);
				if (r) {
					DRM_ERROR("resume of IP block <%s> failed %d\n",
							  adev->ip_blocks[i].version->funcs->name, r);
					return r;
				}
			} else {
				r = adev->ip_blocks[i].version->funcs->hw_init(adev);
				if (r) {
					DRM_ERROR("hw_init of IP block <%s> failed %d\n",
							  adev->ip_blocks[i].version->funcs->name, r);
					return r;
				}
			}

			adev->ip_blocks[i].status.hw = true;
			break;
		}
	}

	if (!amdgpu_sriov_vf(adev) || adev->asic_type == CHIP_TONGA)
		r = amdgpu_pm_load_smu_firmware(adev, &smu_version);

	return r;
}

/**
 * amdgpu_device_ip_init - run init for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * Main initialization pass for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked and the sw_init and hw_init callbacks
 * are run.  sw_init initializes the software state associated with each IP
 * and hw_init initializes the hardware associated with each IP.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_init(struct amdgpu_device *adev)
{
	int i, r;

	r = amdgpu_ras_init(adev);
	if (r)
		return r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		r = adev->ip_blocks[i].version->funcs->sw_init((void *)adev);
		if (r) {
			DRM_ERROR("sw_init of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
			goto init_failed;
		}
		adev->ip_blocks[i].status.sw = true;

		/* need to do gmc hw init early so we can allocate gpu mem */
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC) {
			/* Try to reserve bad pages early */
			if (amdgpu_sriov_vf(adev))
				amdgpu_virt_exchange_data(adev);

			r = amdgpu_device_vram_scratch_init(adev);
			if (r) {
				DRM_ERROR("amdgpu_vram_scratch_init failed %d\n", r);
				goto init_failed;
			}
			r = adev->ip_blocks[i].version->funcs->hw_init((void *)adev);
			if (r) {
				DRM_ERROR("hw_init %d failed %d\n", i, r);
				goto init_failed;
			}
			r = amdgpu_device_wb_init(adev);
			if (r) {
				DRM_ERROR("amdgpu_device_wb_init failed %d\n", r);
				goto init_failed;
			}
			adev->ip_blocks[i].status.hw = true;

			/* right after GMC hw init, we create CSA */
			if (amdgpu_mcbp || amdgpu_sriov_vf(adev)) {
				r = amdgpu_allocate_static_csa(adev, &adev->virt.csa_obj,
								AMDGPU_GEM_DOMAIN_VRAM,
								AMDGPU_CSA_SIZE);
				if (r) {
					DRM_ERROR("allocate CSA failed %d\n", r);
					goto init_failed;
				}
			}
		}
	}

	if (amdgpu_sriov_vf(adev))
		amdgpu_virt_init_data_exchange(adev);

	r = amdgpu_ib_pool_init(adev);
	if (r) {
		dev_err(adev->dev, "IB initialization failed (%d).\n", r);
		amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_IB_INIT_FAIL, 0, r);
		goto init_failed;
	}

	r = amdgpu_ucode_create_bo(adev); /* create ucode bo when sw_init complete*/
	if (r)
		goto init_failed;

	r = amdgpu_device_ip_hw_init_phase1(adev);
	if (r)
		goto init_failed;

	r = amdgpu_device_fw_loading(adev);
	if (r)
		goto init_failed;

	r = amdgpu_device_ip_hw_init_phase2(adev);
	if (r)
		goto init_failed;

	/*
	 * retired pages will be loaded from eeprom and reserved here,
	 * it should be called after amdgpu_device_ip_hw_init_phase2  since
	 * for some ASICs the RAS EEPROM code relies on SMU fully functioning
	 * for I2C communication which only true at this point.
	 *
	 * amdgpu_ras_recovery_init may fail, but the upper only cares the
	 * failure from bad gpu situation and stop amdgpu init process
	 * accordingly. For other failed cases, it will still release all
	 * the resource and print error message, rather than returning one
	 * negative value to upper level.
	 *
	 * Note: theoretically, this should be called before all vram allocations
	 * to protect retired page from abusing
	 */
	r = amdgpu_ras_recovery_init(adev);
	if (r)
		goto init_failed;

	if (adev->gmc.xgmi.num_physical_nodes > 1)
		amdgpu_xgmi_add_device(adev);

	/* Don't init kfd if whole hive need to be reset during init */
	if (!adev->gmc.xgmi.pending_reset)
		amdgpu_amdkfd_device_init(adev);

	amdgpu_fru_get_product_info(adev);

init_failed:
	if (amdgpu_sriov_vf(adev))
		amdgpu_virt_release_full_gpu(adev, true);

	return r;
}

/**
 * amdgpu_device_fill_reset_magic - writes reset magic to gart pointer
 *
 * @adev: amdgpu_device pointer
 *
 * Writes a reset magic value to the gart pointer in VRAM.  The driver calls
 * this function before a GPU reset.  If the value is retained after a
 * GPU reset, VRAM has not been lost.  Some GPU resets may destry VRAM contents.
 */
static void amdgpu_device_fill_reset_magic(struct amdgpu_device *adev)
{
	memcpy(adev->reset_magic, adev->gart.ptr, AMDGPU_RESET_MAGIC_NUM);
}

/**
 * amdgpu_device_check_vram_lost - check if vram is valid
 *
 * @adev: amdgpu_device pointer
 *
 * Checks the reset magic value written to the gart pointer in VRAM.
 * The driver calls this after a GPU reset to see if the contents of
 * VRAM is lost or now.
 * returns true if vram is lost, false if not.
 */
static bool amdgpu_device_check_vram_lost(struct amdgpu_device *adev)
{
	if (memcmp(adev->gart.ptr, adev->reset_magic,
			AMDGPU_RESET_MAGIC_NUM))
		return true;

	if (!amdgpu_in_reset(adev))
		return false;

	/*
	 * For all ASICs with baco/mode1 reset, the VRAM is
	 * always assumed to be lost.
	 */
	switch (amdgpu_asic_reset_method(adev)) {
	case AMD_RESET_METHOD_BACO:
	case AMD_RESET_METHOD_MODE1:
		return true;
	default:
		return false;
	}
}

/**
 * amdgpu_device_set_cg_state - set clockgating for amdgpu device
 *
 * @adev: amdgpu_device pointer
 * @state: clockgating state (gate or ungate)
 *
 * The list of all the hardware IPs that make up the asic is walked and the
 * set_clockgating_state callbacks are run.
 * Late initialization pass enabling clockgating for hardware IPs.
 * Fini or suspend, pass disabling clockgating for hardware IPs.
 * Returns 0 on success, negative error code on failure.
 */

int amdgpu_device_set_cg_state(struct amdgpu_device *adev,
			       enum amd_clockgating_state state)
{
	int i, j, r;

	if (amdgpu_emu_mode == 1)
		return 0;

	for (j = 0; j < adev->num_ip_blocks; j++) {
		i = state == AMD_CG_STATE_GATE ? j : adev->num_ip_blocks - j - 1;
		if (!adev->ip_blocks[i].status.late_initialized)
			continue;
		/* skip CG for GFX on S0ix */
		if (adev->in_s0ix &&
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GFX)
			continue;
		/* skip CG for VCE/UVD, it's handled specially */
		if (adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_UVD &&
		    adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_VCE &&
		    adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_VCN &&
		    adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_JPEG &&
		    adev->ip_blocks[i].version->funcs->set_clockgating_state) {
			/* enable clockgating to save power */
			r = adev->ip_blocks[i].version->funcs->set_clockgating_state((void *)adev,
										     state);
			if (r) {
				DRM_ERROR("set_clockgating_state(gate) of IP block <%s> failed %d\n",
					  adev->ip_blocks[i].version->funcs->name, r);
				return r;
			}
		}
	}

	return 0;
}

int amdgpu_device_set_pg_state(struct amdgpu_device *adev,
			       enum amd_powergating_state state)
{
	int i, j, r;

	if (amdgpu_emu_mode == 1)
		return 0;

	for (j = 0; j < adev->num_ip_blocks; j++) {
		i = state == AMD_PG_STATE_GATE ? j : adev->num_ip_blocks - j - 1;
		if (!adev->ip_blocks[i].status.late_initialized)
			continue;
		/* skip PG for GFX on S0ix */
		if (adev->in_s0ix &&
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GFX)
			continue;
		/* skip CG for VCE/UVD, it's handled specially */
		if (adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_UVD &&
		    adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_VCE &&
		    adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_VCN &&
		    adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_JPEG &&
		    adev->ip_blocks[i].version->funcs->set_powergating_state) {
			/* enable powergating to save power */
			r = adev->ip_blocks[i].version->funcs->set_powergating_state((void *)adev,
											state);
			if (r) {
				DRM_ERROR("set_powergating_state(gate) of IP block <%s> failed %d\n",
					  adev->ip_blocks[i].version->funcs->name, r);
				return r;
			}
		}
	}
	return 0;
}

static int amdgpu_device_enable_mgpu_fan_boost(void)
{
	struct amdgpu_gpu_instance *gpu_ins;
	struct amdgpu_device *adev;
	int i, ret = 0;

	mutex_lock(&mgpu_info.mutex);

	/*
	 * MGPU fan boost feature should be enabled
	 * only when there are two or more dGPUs in
	 * the system
	 */
	if (mgpu_info.num_dgpu < 2)
		goto out;

	for (i = 0; i < mgpu_info.num_dgpu; i++) {
		gpu_ins = &(mgpu_info.gpu_ins[i]);
		adev = gpu_ins->adev;
		if (!(adev->flags & AMD_IS_APU) &&
		    !gpu_ins->mgpu_fan_enabled) {
			ret = amdgpu_dpm_enable_mgpu_fan_boost(adev);
			if (ret)
				break;

			gpu_ins->mgpu_fan_enabled = 1;
		}
	}

out:
	mutex_unlock(&mgpu_info.mutex);

	return ret;
}

/**
 * amdgpu_device_ip_late_init - run late init for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * Late initialization pass for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked and the late_init callbacks are run.
 * late_init covers any special initialization that an IP requires
 * after all of the have been initialized or something that needs to happen
 * late in the init process.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_late_init(struct amdgpu_device *adev)
{
	struct amdgpu_gpu_instance *gpu_instance;
	int i = 0, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.hw)
			continue;
		if (adev->ip_blocks[i].version->funcs->late_init) {
			r = adev->ip_blocks[i].version->funcs->late_init((void *)adev);
			if (r) {
				DRM_ERROR("late_init of IP block <%s> failed %d\n",
					  adev->ip_blocks[i].version->funcs->name, r);
				return r;
			}
		}
		adev->ip_blocks[i].status.late_initialized = true;
	}

	r = amdgpu_ras_late_init(adev);
	if (r) {
		DRM_ERROR("amdgpu_ras_late_init failed %d", r);
		return r;
	}

	amdgpu_ras_set_error_query_ready(adev, true);

	amdgpu_device_set_cg_state(adev, AMD_CG_STATE_GATE);
	amdgpu_device_set_pg_state(adev, AMD_PG_STATE_GATE);

	amdgpu_device_fill_reset_magic(adev);

	r = amdgpu_device_enable_mgpu_fan_boost();
	if (r)
		DRM_ERROR("enable mgpu fan boost failed (%d).\n", r);

	/* For passthrough configuration on arcturus and aldebaran, enable special handling SBR */
	if (amdgpu_passthrough(adev) && ((adev->asic_type == CHIP_ARCTURUS && adev->gmc.xgmi.num_physical_nodes > 1)||
			       adev->asic_type == CHIP_ALDEBARAN ))
		amdgpu_dpm_handle_passthrough_sbr(adev, true);

	if (adev->gmc.xgmi.num_physical_nodes > 1) {
		mutex_lock(&mgpu_info.mutex);

		/*
		 * Reset device p-state to low as this was booted with high.
		 *
		 * This should be performed only after all devices from the same
		 * hive get initialized.
		 *
		 * However, it's unknown how many device in the hive in advance.
		 * As this is counted one by one during devices initializations.
		 *
		 * So, we wait for all XGMI interlinked devices initialized.
		 * This may bring some delays as those devices may come from
		 * different hives. But that should be OK.
		 */
		if (mgpu_info.num_dgpu == adev->gmc.xgmi.num_physical_nodes) {
			for (i = 0; i < mgpu_info.num_gpu; i++) {
				gpu_instance = &(mgpu_info.gpu_ins[i]);
				if (gpu_instance->adev->flags & AMD_IS_APU)
					continue;

				r = amdgpu_xgmi_set_pstate(gpu_instance->adev,
						AMDGPU_XGMI_PSTATE_MIN);
				if (r) {
					DRM_ERROR("pstate setting failed (%d).\n", r);
					break;
				}
			}
		}

		mutex_unlock(&mgpu_info.mutex);
	}

	return 0;
}

/**
 * amdgpu_device_smu_fini_early - smu hw_fini wrapper
 *
 * @adev: amdgpu_device pointer
 *
 * For ASICs need to disable SMC first
 */
static void amdgpu_device_smu_fini_early(struct amdgpu_device *adev)
{
	int i, r;

	if (adev->ip_versions[GC_HWIP][0] > IP_VERSION(9, 0, 0))
		return;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.hw)
			continue;
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_SMC) {
			r = adev->ip_blocks[i].version->funcs->hw_fini((void *)adev);
			/* XXX handle errors */
			if (r) {
				DRM_DEBUG("hw_fini of IP block <%s> failed %d\n",
					  adev->ip_blocks[i].version->funcs->name, r);
			}
			adev->ip_blocks[i].status.hw = false;
			break;
		}
	}
}

static int amdgpu_device_ip_fini_early(struct amdgpu_device *adev)
{
	int i, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].version->funcs->early_fini)
			continue;

		r = adev->ip_blocks[i].version->funcs->early_fini((void *)adev);
		if (r) {
			DRM_DEBUG("early_fini of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
		}
	}

	amdgpu_device_set_pg_state(adev, AMD_PG_STATE_UNGATE);
	amdgpu_device_set_cg_state(adev, AMD_CG_STATE_UNGATE);

	amdgpu_amdkfd_suspend(adev, false);

	/* Workaroud for ASICs need to disable SMC first */
	amdgpu_device_smu_fini_early(adev);

	for (i = adev->num_ip_blocks - 1; i >= 0; i--) {
		if (!adev->ip_blocks[i].status.hw)
			continue;

		r = adev->ip_blocks[i].version->funcs->hw_fini((void *)adev);
		/* XXX handle errors */
		if (r) {
			DRM_DEBUG("hw_fini of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
		}

		adev->ip_blocks[i].status.hw = false;
	}

	if (amdgpu_sriov_vf(adev)) {
		if (amdgpu_virt_release_full_gpu(adev, false))
			DRM_ERROR("failed to release exclusive mode on fini\n");
	}

	return 0;
}

/**
 * amdgpu_device_ip_fini - run fini for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * Main teardown pass for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked and the hw_fini and sw_fini callbacks
 * are run.  hw_fini tears down the hardware associated with each IP
 * and sw_fini tears down any software state associated with each IP.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_fini(struct amdgpu_device *adev)
{
	int i, r;

	if (amdgpu_sriov_vf(adev) && adev->virt.ras_init_done)
		amdgpu_virt_release_ras_err_handler_data(adev);

	if (adev->gmc.xgmi.num_physical_nodes > 1)
		amdgpu_xgmi_remove_device(adev);

	amdgpu_amdkfd_device_fini_sw(adev);

	for (i = adev->num_ip_blocks - 1; i >= 0; i--) {
		if (!adev->ip_blocks[i].status.sw)
			continue;

		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC) {
			amdgpu_ucode_free_bo(adev);
			amdgpu_free_static_csa(&adev->virt.csa_obj);
			amdgpu_device_wb_fini(adev);
			amdgpu_device_vram_scratch_fini(adev);
			amdgpu_ib_pool_fini(adev);
		}

		r = adev->ip_blocks[i].version->funcs->sw_fini((void *)adev);
		/* XXX handle errors */
		if (r) {
			DRM_DEBUG("sw_fini of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
		}
		adev->ip_blocks[i].status.sw = false;
		adev->ip_blocks[i].status.valid = false;
	}

	for (i = adev->num_ip_blocks - 1; i >= 0; i--) {
		if (!adev->ip_blocks[i].status.late_initialized)
			continue;
		if (adev->ip_blocks[i].version->funcs->late_fini)
			adev->ip_blocks[i].version->funcs->late_fini((void *)adev);
		adev->ip_blocks[i].status.late_initialized = false;
	}

	amdgpu_ras_fini(adev);

	return 0;
}

/**
 * amdgpu_device_delayed_init_work_handler - work handler for IB tests
 *
 * @work: work_struct.
 */
static void amdgpu_device_delayed_init_work_handler(struct work_struct *work)
{
	struct amdgpu_device *adev =
		container_of(work, struct amdgpu_device, delayed_init_work.work);
	int r;

	r = amdgpu_ib_ring_tests(adev);
	if (r)
		DRM_ERROR("ib ring test failed (%d).\n", r);
}

static void amdgpu_device_delay_enable_gfx_off(struct work_struct *work)
{
	struct amdgpu_device *adev =
		container_of(work, struct amdgpu_device, gfx.gfx_off_delay_work.work);

	WARN_ON_ONCE(adev->gfx.gfx_off_state);
	WARN_ON_ONCE(adev->gfx.gfx_off_req_count);

	if (!amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_GFX, true))
		adev->gfx.gfx_off_state = true;
}

/**
 * amdgpu_device_ip_suspend_phase1 - run suspend for hardware IPs (phase 1)
 *
 * @adev: amdgpu_device pointer
 *
 * Main suspend function for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked, clockgating is disabled and the
 * suspend callbacks are run.  suspend puts the hardware and software state
 * in each IP into a state suitable for suspend.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_suspend_phase1(struct amdgpu_device *adev)
{
	int i, r;

	amdgpu_device_set_pg_state(adev, AMD_PG_STATE_UNGATE);
	amdgpu_device_set_cg_state(adev, AMD_CG_STATE_UNGATE);

	for (i = adev->num_ip_blocks - 1; i >= 0; i--) {
		if (!adev->ip_blocks[i].status.valid)
			continue;

		/* displays are handled separately */
		if (adev->ip_blocks[i].version->type != AMD_IP_BLOCK_TYPE_DCE)
			continue;

		/* XXX handle errors */
		r = adev->ip_blocks[i].version->funcs->suspend(adev);
		/* XXX handle errors */
		if (r) {
			DRM_ERROR("suspend of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
			return r;
		}

		adev->ip_blocks[i].status.hw = false;
	}

	return 0;
}

/**
 * amdgpu_device_ip_suspend_phase2 - run suspend for hardware IPs (phase 2)
 *
 * @adev: amdgpu_device pointer
 *
 * Main suspend function for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked, clockgating is disabled and the
 * suspend callbacks are run.  suspend puts the hardware and software state
 * in each IP into a state suitable for suspend.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_suspend_phase2(struct amdgpu_device *adev)
{
	int i, r;

	if (adev->in_s0ix)
		amdgpu_dpm_gfx_state_change(adev, sGpuChangeState_D3Entry);

	for (i = adev->num_ip_blocks - 1; i >= 0; i--) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		/* displays are handled in phase1 */
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_DCE)
			continue;
		/* PSP lost connection when err_event_athub occurs */
		if (amdgpu_ras_intr_triggered() &&
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_PSP) {
			adev->ip_blocks[i].status.hw = false;
			continue;
		}

		/* skip unnecessary suspend if we do not initialize them yet */
		if (adev->gmc.xgmi.pending_reset &&
		    !(adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC ||
		      adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_SMC ||
		      adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_COMMON ||
		      adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_IH)) {
			adev->ip_blocks[i].status.hw = false;
			continue;
		}

		/* skip suspend of gfx and psp for S0ix
		 * gfx is in gfxoff state, so on resume it will exit gfxoff just
		 * like at runtime. PSP is also part of the always on hardware
		 * so no need to suspend it.
		 */
		if (adev->in_s0ix &&
		    (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_PSP ||
		     adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GFX))
			continue;

		/* XXX handle errors */
		r = adev->ip_blocks[i].version->funcs->suspend(adev);
		/* XXX handle errors */
		if (r) {
			DRM_ERROR("suspend of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
		}
		adev->ip_blocks[i].status.hw = false;
		/* handle putting the SMC in the appropriate state */
		if(!amdgpu_sriov_vf(adev)){
			if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_SMC) {
				r = amdgpu_dpm_set_mp1_state(adev, adev->mp1_state);
				if (r) {
					DRM_ERROR("SMC failed to set mp1 state %d, %d\n",
							adev->mp1_state, r);
					return r;
				}
			}
		}
	}

	return 0;
}

/**
 * amdgpu_device_ip_suspend - run suspend for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * Main suspend function for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked, clockgating is disabled and the
 * suspend callbacks are run.  suspend puts the hardware and software state
 * in each IP into a state suitable for suspend.
 * Returns 0 on success, negative error code on failure.
 */
int amdgpu_device_ip_suspend(struct amdgpu_device *adev)
{
	int r;

	if (amdgpu_sriov_vf(adev)) {
		amdgpu_virt_fini_data_exchange(adev);
		amdgpu_virt_request_full_gpu(adev, false);
	}

	r = amdgpu_device_ip_suspend_phase1(adev);
	if (r)
		return r;
	r = amdgpu_device_ip_suspend_phase2(adev);

	if (amdgpu_sriov_vf(adev))
		amdgpu_virt_release_full_gpu(adev, false);

	return r;
}

static int amdgpu_device_ip_reinit_early_sriov(struct amdgpu_device *adev)
{
	int i, r;

	static enum amd_ip_block_type ip_order[] = {
		AMD_IP_BLOCK_TYPE_GMC,
		AMD_IP_BLOCK_TYPE_COMMON,
		AMD_IP_BLOCK_TYPE_PSP,
		AMD_IP_BLOCK_TYPE_IH,
	};

	for (i = 0; i < adev->num_ip_blocks; i++) {
		int j;
		struct amdgpu_ip_block *block;

		block = &adev->ip_blocks[i];
		block->status.hw = false;

		for (j = 0; j < ARRAY_SIZE(ip_order); j++) {

			if (block->version->type != ip_order[j] ||
				!block->status.valid)
				continue;

			r = block->version->funcs->hw_init(adev);
			DRM_INFO("RE-INIT-early: %s %s\n", block->version->funcs->name, r?"failed":"succeeded");
			if (r)
				return r;
			block->status.hw = true;
		}
	}

	return 0;
}

static int amdgpu_device_ip_reinit_late_sriov(struct amdgpu_device *adev)
{
	int i, r;

	static enum amd_ip_block_type ip_order[] = {
		AMD_IP_BLOCK_TYPE_SMC,
		AMD_IP_BLOCK_TYPE_DCE,
		AMD_IP_BLOCK_TYPE_GFX,
		AMD_IP_BLOCK_TYPE_SDMA,
		AMD_IP_BLOCK_TYPE_UVD,
		AMD_IP_BLOCK_TYPE_VCE,
		AMD_IP_BLOCK_TYPE_VCN
	};

	for (i = 0; i < ARRAY_SIZE(ip_order); i++) {
		int j;
		struct amdgpu_ip_block *block;

		for (j = 0; j < adev->num_ip_blocks; j++) {
			block = &adev->ip_blocks[j];

			if (block->version->type != ip_order[i] ||
				!block->status.valid ||
				block->status.hw)
				continue;

			if (block->version->type == AMD_IP_BLOCK_TYPE_SMC)
				r = block->version->funcs->resume(adev);
			else
				r = block->version->funcs->hw_init(adev);

			DRM_INFO("RE-INIT-late: %s %s\n", block->version->funcs->name, r?"failed":"succeeded");
			if (r)
				return r;
			block->status.hw = true;
		}
	}

	return 0;
}

/**
 * amdgpu_device_ip_resume_phase1 - run resume for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * First resume function for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked and the resume callbacks are run for
 * COMMON, GMC, and IH.  resume puts the hardware into a functional state
 * after a suspend and updates the software state as necessary.  This
 * function is also used for restoring the GPU after a GPU reset.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_resume_phase1(struct amdgpu_device *adev)
{
	int i, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid || adev->ip_blocks[i].status.hw)
			continue;
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_COMMON ||
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC ||
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_IH) {

			r = adev->ip_blocks[i].version->funcs->resume(adev);
			if (r) {
				DRM_ERROR("resume of IP block <%s> failed %d\n",
					  adev->ip_blocks[i].version->funcs->name, r);
				return r;
			}
			adev->ip_blocks[i].status.hw = true;
		}
	}

	return 0;
}

/**
 * amdgpu_device_ip_resume_phase2 - run resume for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * First resume function for hardware IPs.  The list of all the hardware
 * IPs that make up the asic is walked and the resume callbacks are run for
 * all blocks except COMMON, GMC, and IH.  resume puts the hardware into a
 * functional state after a suspend and updates the software state as
 * necessary.  This function is also used for restoring the GPU after a GPU
 * reset.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_resume_phase2(struct amdgpu_device *adev)
{
	int i, r;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid || adev->ip_blocks[i].status.hw)
			continue;
		if (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_COMMON ||
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC ||
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_IH ||
		    adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_PSP)
			continue;
		r = adev->ip_blocks[i].version->funcs->resume(adev);
		if (r) {
			DRM_ERROR("resume of IP block <%s> failed %d\n",
				  adev->ip_blocks[i].version->funcs->name, r);
			return r;
		}
		adev->ip_blocks[i].status.hw = true;
	}

	return 0;
}

/**
 * amdgpu_device_ip_resume - run resume for hardware IPs
 *
 * @adev: amdgpu_device pointer
 *
 * Main resume function for hardware IPs.  The hardware IPs
 * are split into two resume functions because they are
 * are also used in in recovering from a GPU reset and some additional
 * steps need to be take between them.  In this case (S3/S4) they are
 * run sequentially.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_resume(struct amdgpu_device *adev)
{
	int r;

	r = amdgpu_amdkfd_resume_iommu(adev);
	if (r)
		return r;

	r = amdgpu_device_ip_resume_phase1(adev);
	if (r)
		return r;

	r = amdgpu_device_fw_loading(adev);
	if (r)
		return r;

	r = amdgpu_device_ip_resume_phase2(adev);

	return r;
}

/**
 * amdgpu_device_detect_sriov_bios - determine if the board supports SR-IOV
 *
 * @adev: amdgpu_device pointer
 *
 * Query the VBIOS data tables to determine if the board supports SR-IOV.
 */
static void amdgpu_device_detect_sriov_bios(struct amdgpu_device *adev)
{
	if (amdgpu_sriov_vf(adev)) {
		if (adev->is_atom_fw) {
			if (amdgpu_atomfirmware_gpu_virtualization_supported(adev))
				adev->virt.caps |= AMDGPU_SRIOV_CAPS_SRIOV_VBIOS;
		} else {
			if (amdgpu_atombios_has_gpu_virtualization_table(adev))
				adev->virt.caps |= AMDGPU_SRIOV_CAPS_SRIOV_VBIOS;
		}

		if (!(adev->virt.caps & AMDGPU_SRIOV_CAPS_SRIOV_VBIOS))
			amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_NO_VBIOS, 0, 0);
	}
}

/**
 * amdgpu_device_asic_has_dc_support - determine if DC supports the asic
 *
 * @asic_type: AMD asic type
 *
 * Check if there is DC (new modesetting infrastructre) support for an asic.
 * returns true if DC has support, false if not.
 */
bool amdgpu_device_asic_has_dc_support(enum amd_asic_type asic_type)
{
	switch (asic_type) {
#ifdef CONFIG_DRM_AMDGPU_SI
	case CHIP_HAINAN:
#endif
	case CHIP_TOPAZ:
		/* chips with no display hardware */
		return false;
#if defined(CONFIG_DRM_AMD_DC)
	case CHIP_TAHITI:
	case CHIP_PITCAIRN:
	case CHIP_VERDE:
	case CHIP_OLAND:
		/*
		 * We have systems in the wild with these ASICs that require
		 * LVDS and VGA support which is not supported with DC.
		 *
		 * Fallback to the non-DC driver here by default so as not to
		 * cause regressions.
		 */
#if defined(CONFIG_DRM_AMD_DC_SI)
		return amdgpu_dc > 0;
#else
		return false;
#endif
	case CHIP_BONAIRE:
	case CHIP_KAVERI:
	case CHIP_KABINI:
	case CHIP_MULLINS:
		/*
		 * We have systems in the wild with these ASICs that require
		 * LVDS and VGA support which is not supported with DC.
		 *
		 * Fallback to the non-DC driver here by default so as not to
		 * cause regressions.
		 */
		return amdgpu_dc > 0;
	case CHIP_HAWAII:
	case CHIP_CARRIZO:
	case CHIP_STONEY:
	case CHIP_POLARIS10:
	case CHIP_POLARIS11:
	case CHIP_POLARIS12:
	case CHIP_VEGAM:
	case CHIP_TONGA:
	case CHIP_FIJI:
	case CHIP_VEGA10:
	case CHIP_VEGA12:
	case CHIP_VEGA20:
#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
	case CHIP_RAVEN:
#endif
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	case CHIP_NAVI10:
	case CHIP_NAVI14:
	case CHIP_NAVI12:
	case CHIP_RENOIR:
#endif
#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	case CHIP_CYAN_SKILLFISH:
	case CHIP_SIENNA_CICHLID:
	case CHIP_NAVY_FLOUNDER:
	case CHIP_DIMGREY_CAVEFISH:
	case CHIP_BEIGE_GOBY:
	case CHIP_VANGOGH:
	case CHIP_YELLOW_CARP:
#endif
	default:
		return amdgpu_dc != 0;
#else
	default:
		if (amdgpu_dc > 0)
			DRM_INFO_ONCE("Display Core has been requested via kernel parameter "
					 "but isn't supported by ASIC, ignoring\n");
		return false;
#endif
	}
}

/**
 * amdgpu_device_has_dc_support - check if dc is supported
 *
 * @adev: amdgpu_device pointer
 *
 * Returns true for supported, false for not supported
 */
bool amdgpu_device_has_dc_support(struct amdgpu_device *adev)
{
	if (amdgpu_sriov_vf(adev) || 
	    adev->enable_virtual_display ||
	    (adev->harvest_ip_mask & AMD_HARVEST_IP_DMU_MASK))
		return false;

	return amdgpu_device_asic_has_dc_support(adev->asic_type);
}

static void amdgpu_device_xgmi_reset_func(struct work_struct *__work)
{
	struct amdgpu_device *adev =
		container_of(__work, struct amdgpu_device, xgmi_reset_work);
	struct amdgpu_hive_info *hive = amdgpu_get_xgmi_hive(adev);

	/* It's a bug to not have a hive within this function */
	if (WARN_ON(!hive))
		return;

	/*
	 * Use task barrier to synchronize all xgmi reset works across the
	 * hive. task_barrier_enter and task_barrier_exit will block
	 * until all the threads running the xgmi reset works reach
	 * those points. task_barrier_full will do both blocks.
	 */
	if (amdgpu_asic_reset_method(adev) == AMD_RESET_METHOD_BACO) {

		task_barrier_enter(&hive->tb);
		adev->asic_reset_res = amdgpu_device_baco_enter(adev_to_drm(adev));

		if (adev->asic_reset_res)
			goto fail;

		task_barrier_exit(&hive->tb);
		adev->asic_reset_res = amdgpu_device_baco_exit(adev_to_drm(adev));

		if (adev->asic_reset_res)
			goto fail;

		if (adev->mmhub.ras && adev->mmhub.ras->ras_block.hw_ops &&
		    adev->mmhub.ras->ras_block.hw_ops->reset_ras_error_count)
			adev->mmhub.ras->ras_block.hw_ops->reset_ras_error_count(adev);
	} else {

		task_barrier_full(&hive->tb);
		adev->asic_reset_res =  amdgpu_asic_reset(adev);
	}

fail:
	if (adev->asic_reset_res)
		DRM_WARN("ASIC reset failed with error, %d for drm dev, %s",
			 adev->asic_reset_res, adev_to_drm(adev)->unique);
	amdgpu_put_xgmi_hive(hive);
}

static int amdgpu_device_get_job_timeout_settings(struct amdgpu_device *adev)
{
	char *input = amdgpu_lockup_timeout;
	char *timeout_setting = NULL;
	int index = 0;
	long timeout;
	int ret = 0;

	/*
	 * By default timeout for non compute jobs is 10000
	 * and 60000 for compute jobs.
	 * In SR-IOV or passthrough mode, timeout for compute
	 * jobs are 60000 by default.
	 */
	adev->gfx_timeout = msecs_to_jiffies(10000);
	adev->sdma_timeout = adev->video_timeout = adev->gfx_timeout;
	if (amdgpu_sriov_vf(adev))
		adev->compute_timeout = amdgpu_sriov_is_pp_one_vf(adev) ?
					msecs_to_jiffies(60000) : msecs_to_jiffies(10000);
	else
		adev->compute_timeout =  msecs_to_jiffies(60000);

	if (strnlen(input, AMDGPU_MAX_TIMEOUT_PARAM_LENGTH)) {
		while ((timeout_setting = strsep(&input, ",")) &&
				strnlen(timeout_setting, AMDGPU_MAX_TIMEOUT_PARAM_LENGTH)) {
			ret = kstrtol(timeout_setting, 0, &timeout);
			if (ret)
				return ret;

			if (timeout == 0) {
				index++;
				continue;
			} else if (timeout < 0) {
				timeout = MAX_SCHEDULE_TIMEOUT;
				dev_warn(adev->dev, "lockup timeout disabled");
				add_taint(TAINT_SOFTLOCKUP, LOCKDEP_STILL_OK);
			} else {
				timeout = msecs_to_jiffies(timeout);
			}

			switch (index++) {
			case 0:
				adev->gfx_timeout = timeout;
				break;
			case 1:
				adev->compute_timeout = timeout;
				break;
			case 2:
				adev->sdma_timeout = timeout;
				break;
			case 3:
				adev->video_timeout = timeout;
				break;
			default:
				break;
			}
		}
		/*
		 * There is only one value specified and
		 * it should apply to all non-compute jobs.
		 */
		if (index == 1) {
			adev->sdma_timeout = adev->video_timeout = adev->gfx_timeout;
			if (amdgpu_sriov_vf(adev) || amdgpu_passthrough(adev))
				adev->compute_timeout = adev->gfx_timeout;
		}
	}

	return ret;
}

/**
 * amdgpu_device_check_iommu_direct_map - check if RAM direct mapped to GPU
 *
 * @adev: amdgpu_device pointer
 *
 * RAM direct mapped to GPU if IOMMU is not enabled or is pass through mode
 */
static void amdgpu_device_check_iommu_direct_map(struct amdgpu_device *adev)
{
	struct iommu_domain *domain;

	domain = iommu_get_domain_for_dev(adev->dev);
	if (!domain || domain->type == IOMMU_DOMAIN_IDENTITY)
		adev->ram_is_direct_mapped = true;
}

static const struct attribute *amdgpu_dev_attributes[] = {
	&dev_attr_product_name.attr,
	&dev_attr_product_number.attr,
	&dev_attr_serial_number.attr,
	&dev_attr_pcie_replay_count.attr,
	NULL
};

/**
 * amdgpu_device_init - initialize the driver
 *
 * @adev: amdgpu_device pointer
 * @flags: driver flags
 *
 * Initializes the driver info and hw (all asics).
 * Returns 0 for success or an error on failure.
 * Called at driver startup.
 */
int amdgpu_device_init(struct amdgpu_device *adev,
		       uint32_t flags)
{
	struct drm_device *ddev = adev_to_drm(adev);
	struct pci_dev *pdev = adev->pdev;
	int r, i;
	bool px = false;
	u32 max_MBps;

	adev->shutdown = false;
	adev->flags = flags;

	if (amdgpu_force_asic_type >= 0 && amdgpu_force_asic_type < CHIP_LAST)
		adev->asic_type = amdgpu_force_asic_type;
	else
		adev->asic_type = flags & AMD_ASIC_MASK;

	adev->usec_timeout = AMDGPU_MAX_USEC_TIMEOUT;
	if (amdgpu_emu_mode == 1)
		adev->usec_timeout *= 10;
	adev->gmc.gart_size = 512 * 1024 * 1024;
	adev->accel_working = false;
	adev->num_rings = 0;
	adev->mman.buffer_funcs = NULL;
	adev->mman.buffer_funcs_ring = NULL;
	adev->vm_manager.vm_pte_funcs = NULL;
	adev->vm_manager.vm_pte_num_scheds = 0;
	adev->gmc.gmc_funcs = NULL;
	adev->harvest_ip_mask = 0x0;
	adev->fence_context = dma_fence_context_alloc(AMDGPU_MAX_RINGS);
	bitmap_zero(adev->gfx.pipe_reserve_bitmap, AMDGPU_MAX_COMPUTE_QUEUES);

	adev->smc_rreg = &amdgpu_invalid_rreg;
	adev->smc_wreg = &amdgpu_invalid_wreg;
	adev->pcie_rreg = &amdgpu_invalid_rreg;
	adev->pcie_wreg = &amdgpu_invalid_wreg;
	adev->pciep_rreg = &amdgpu_invalid_rreg;
	adev->pciep_wreg = &amdgpu_invalid_wreg;
	adev->pcie_rreg64 = &amdgpu_invalid_rreg64;
	adev->pcie_wreg64 = &amdgpu_invalid_wreg64;
	adev->uvd_ctx_rreg = &amdgpu_invalid_rreg;
	adev->uvd_ctx_wreg = &amdgpu_invalid_wreg;
	adev->didt_rreg = &amdgpu_invalid_rreg;
	adev->didt_wreg = &amdgpu_invalid_wreg;
	adev->gc_cac_rreg = &amdgpu_invalid_rreg;
	adev->gc_cac_wreg = &amdgpu_invalid_wreg;
	adev->audio_endpt_rreg = &amdgpu_block_invalid_rreg;
	adev->audio_endpt_wreg = &amdgpu_block_invalid_wreg;

	DRM_INFO("initializing kernel modesetting (%s 0x%04X:0x%04X 0x%04X:0x%04X 0x%02X).\n",
		 amdgpu_asic_name[adev->asic_type], pdev->vendor, pdev->device,
		 pdev->subsystem_vendor, pdev->subsystem_device, pdev->revision);

	/* mutex initialization are all done here so we
	 * can recall function without having locking issues */
	mutex_init(&adev->firmware.mutex);
	mutex_init(&adev->pm.mutex);
	mutex_init(&adev->gfx.gpu_clock_mutex);
	mutex_init(&adev->srbm_mutex);
	mutex_init(&adev->gfx.pipe_reserve_mutex);
	mutex_init(&adev->gfx.gfx_off_mutex);
	mutex_init(&adev->grbm_idx_mutex);
	mutex_init(&adev->mn_lock);
	mutex_init(&adev->virt.vf_errors.lock);
	hash_init(adev->mn_hash);
	atomic_set(&adev->in_gpu_reset, 0);
	init_rwsem(&adev->reset_sem);
	init_waitqueue_head(&adev->recovery_fini_event);
	mutex_init(&adev->psp.mutex);
#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	mutex_init(&adev->notifier_lock);
#endif
	mutex_init(&adev->pm.stable_pstate_ctx_lock);

	amdgpu_device_init_apu_flags(adev);

	r = amdgpu_device_check_arguments(adev);
	if (r)
		return r;

	spin_lock_init(&adev->mmio_idx_lock);
	spin_lock_init(&adev->smc_idx_lock);
	spin_lock_init(&adev->pcie_idx_lock);
	spin_lock_init(&adev->uvd_ctx_idx_lock);
	spin_lock_init(&adev->didt_idx_lock);
	spin_lock_init(&adev->gc_cac_idx_lock);
	spin_lock_init(&adev->se_cac_idx_lock);
	spin_lock_init(&adev->audio_endpt_idx_lock);
	spin_lock_init(&adev->mm_stats.lock);

	INIT_LIST_HEAD(&adev->shadow_list);
	mutex_init(&adev->shadow_list_lock);

	INIT_LIST_HEAD(&adev->reset_list);

	INIT_LIST_HEAD(&adev->ras_list);

	INIT_DELAYED_WORK(&adev->delayed_init_work,
			  amdgpu_device_delayed_init_work_handler);
	INIT_DELAYED_WORK(&adev->gfx.gfx_off_delay_work,
			  amdgpu_device_delay_enable_gfx_off);

	INIT_WORK(&adev->xgmi_reset_work, amdgpu_device_xgmi_reset_func);

	adev->gfx.gfx_off_req_count = 1;
	adev->pm.ac_power = power_supply_is_system_supplied() > 0;

	atomic_set(&adev->throttling_logging_enabled, 1);
	/*
	 * If throttling continues, logging will be performed every minute
	 * to avoid log flooding. "-1" is subtracted since the thermal
	 * throttling interrupt comes every second. Thus, the total logging
	 * interval is 59 seconds(retelimited printk interval) + 1(waiting
	 * for throttling interrupt) = 60 seconds.
	 */
	ratelimit_state_init(&adev->throttling_logging_rs, (60 - 1) * HZ, 1);
#ifdef RATELIMIT_MSG_ON_RELEASE
	ratelimit_set_flags(&adev->throttling_logging_rs, RATELIMIT_MSG_ON_RELEASE);
#endif

	/* Registers mapping */
	/* TODO: block userspace mapping of io register */
	if (adev->asic_type >= CHIP_BONAIRE) {
		adev->rmmio_base = pci_resource_start(adev->pdev, 5);
		adev->rmmio_size = pci_resource_len(adev->pdev, 5);
	} else {
		adev->rmmio_base = pci_resource_start(adev->pdev, 2);
		adev->rmmio_size = pci_resource_len(adev->pdev, 2);
	}

	for (i = 0; i < AMD_IP_BLOCK_TYPE_NUM; i++)
		atomic_set(&adev->pm.pwr_state[i], POWER_STATE_UNKNOWN);

	adev->rmmio = ioremap(adev->rmmio_base, adev->rmmio_size);
	if (adev->rmmio == NULL) {
		return -ENOMEM;
	}
	DRM_INFO("register mmio base: 0x%08X\n", (uint32_t)adev->rmmio_base);
	DRM_INFO("register mmio size: %u\n", (unsigned)adev->rmmio_size);

	amdgpu_device_get_pcie_info(adev);

	if (amdgpu_mcbp)
		DRM_INFO("MCBP is enabled\n");

	if (amdgpu_mes && adev->asic_type >= CHIP_NAVI10)
		adev->enable_mes = true;

	/* detect hw virtualization here */
	amdgpu_detect_virtualization(adev);

	r = amdgpu_device_get_job_timeout_settings(adev);
	if (r) {
		dev_err(adev->dev, "invalid lockup_timeout parameter syntax\n");
		return r;
	}

	/* early init functions */
	r = amdgpu_device_ip_early_init(adev);
	if (r)
		return r;

	/* Need to get xgmi info early to decide the reset behavior*/
	if (adev->gmc.xgmi.supported) {
		r = adev->gfxhub.funcs->get_xgmi_info(adev);
		if (r)
			return r;
	}

	/* enable PCIE atomic ops */
	if (amdgpu_sriov_vf(adev))
		adev->have_atomics_support = ((struct amd_sriov_msg_pf2vf_info *)
			adev->virt.fw_reserve.p_pf2vf)->pcie_atomic_ops_enabled_flags ==
			(PCI_EXP_DEVCAP2_ATOMIC_COMP32 | PCI_EXP_DEVCAP2_ATOMIC_COMP64);
	else
		adev->have_atomics_support =
			!pci_enable_atomic_ops_to_root(adev->pdev,
					  PCI_EXP_DEVCAP2_ATOMIC_COMP32 |
					  PCI_EXP_DEVCAP2_ATOMIC_COMP64);
	if (!adev->have_atomics_support)
		dev_info(adev->dev, "PCIE atomic ops is not supported\n");

	/* doorbell bar mapping and doorbell index init*/
	amdgpu_device_doorbell_init(adev);

	if (amdgpu_emu_mode == 1) {
		/* post the asic on emulation mode */
		emu_soc_asic_init(adev);
		goto fence_driver_init;
	}

	amdgpu_reset_init(adev);

	/* detect if we are with an SRIOV vbios */
	amdgpu_device_detect_sriov_bios(adev);

	/* check if we need to reset the asic
	 *  E.g., driver was not cleanly unloaded previously, etc.
	 */
	if (!amdgpu_sriov_vf(adev) && amdgpu_asic_need_reset_on_init(adev)) {
		if (adev->gmc.xgmi.num_physical_nodes) {
			dev_info(adev->dev, "Pending hive reset.\n");
			adev->gmc.xgmi.pending_reset = true;
			/* Only need to init necessary block for SMU to handle the reset */
			for (i = 0; i < adev->num_ip_blocks; i++) {
				if (!adev->ip_blocks[i].status.valid)
					continue;
				if (!(adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC ||
				      adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_COMMON ||
				      adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_IH ||
				      adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_SMC)) {
					DRM_DEBUG("IP %s disabled for hw_init.\n",
						adev->ip_blocks[i].version->funcs->name);
					adev->ip_blocks[i].status.hw = true;
				}
			}
		} else {
			r = amdgpu_asic_reset(adev);
			if (r) {
				dev_err(adev->dev, "asic reset on init failed\n");
				goto failed;
			}
		}
	}

	pci_enable_pcie_error_reporting(adev->pdev);

	/* Post card if necessary */
	if (amdgpu_device_need_post(adev)) {
		if (!adev->bios) {
			dev_err(adev->dev, "no vBIOS found\n");
			r = -EINVAL;
			goto failed;
		}
		DRM_INFO("GPU posting now...\n");
		r = amdgpu_device_asic_init(adev);
		if (r) {
			dev_err(adev->dev, "gpu post error!\n");
			goto failed;
		}
	}

	if (adev->is_atom_fw) {
		/* Initialize clocks */
		r = amdgpu_atomfirmware_get_clock_info(adev);
		if (r) {
			dev_err(adev->dev, "amdgpu_atomfirmware_get_clock_info failed\n");
			amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_ATOMBIOS_GET_CLOCK_FAIL, 0, 0);
			goto failed;
		}
	} else {
		/* Initialize clocks */
		r = amdgpu_atombios_get_clock_info(adev);
		if (r) {
			dev_err(adev->dev, "amdgpu_atombios_get_clock_info failed\n");
			amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_ATOMBIOS_GET_CLOCK_FAIL, 0, 0);
			goto failed;
		}
		/* init i2c buses */
		if (!amdgpu_device_has_dc_support(adev))
			amdgpu_atombios_i2c_init(adev);
	}

fence_driver_init:
	/* Fence driver */
	r = amdgpu_fence_driver_sw_init(adev);
	if (r) {
		dev_err(adev->dev, "amdgpu_fence_driver_sw_init failed\n");
		amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_FENCE_INIT_FAIL, 0, 0);
		goto failed;
	}

	/* init the mode config */
	drm_mode_config_init(adev_to_drm(adev));

	if (amdgpu_sriov_vf(adev)) {
		adev->timeout_wq = alloc_ordered_workqueue("amdgpu_ring_timeout_wq", 0);
		if (!adev->timeout_wq)
			dev_warn(adev->dev, "alloc_ordered_workqueue failed\n");
	}

	r = amdgpu_device_ip_init(adev);
	if (r) {
		/* failed in exclusive mode due to timeout */
		if (amdgpu_sriov_vf(adev) &&
		    !amdgpu_sriov_runtime(adev) &&
		    amdgpu_virt_mmio_blocked(adev) &&
		    !amdgpu_virt_wait_reset(adev)) {
			dev_err(adev->dev, "VF exclusive mode timeout\n");
			/* Don't send request since VF is inactive. */
			adev->virt.caps &= ~AMDGPU_SRIOV_CAPS_RUNTIME;
			adev->virt.ops = NULL;
			r = -EAGAIN;
			goto release_ras_con;
		}
		dev_err(adev->dev, "amdgpu_device_ip_init failed\n");
		amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_AMDGPU_INIT_FAIL, 0, 0);
		goto release_ras_con;
	}

	amdgpu_fence_driver_hw_init(adev);

	dev_info(adev->dev,
		"SE %d, SH per SE %d, CU per SH %d, active_cu_number %d\n",
			adev->gfx.config.max_shader_engines,
			adev->gfx.config.max_sh_per_se,
			adev->gfx.config.max_cu_per_sh,
			adev->gfx.cu_info.number);

	adev->accel_working = true;

	amdgpu_vm_check_compute_bug(adev);

	/* Initialize the buffer migration limit. */
	if (amdgpu_moverate >= 0)
		max_MBps = amdgpu_moverate;
	else
		max_MBps = 8; /* Allow 8 MB/s. */
	/* Get a log2 for easy divisions. */
	adev->mm_stats.log2_max_MBps = ilog2(max(1u, max_MBps));

#ifndef HAVE_DRM_FBDEV_GENERIC_SETUP
	amdgpu_fbdev_init(adev);
#endif

	r = amdgpu_pm_sysfs_init(adev);
	if (r) {
		adev->pm_sysfs_en = false;
		DRM_ERROR("registering pm debugfs failed (%d).\n", r);
	} else
		adev->pm_sysfs_en = true;

	r = amdgpu_ucode_sysfs_init(adev);
	if (r) {
		adev->ucode_sysfs_en = false;
		DRM_ERROR("Creating firmware sysfs failed (%d).\n", r);
	} else
		adev->ucode_sysfs_en = true;

	if ((amdgpu_testing & 1)) {
		if (adev->accel_working)
			amdgpu_test_moves(adev);
		else
			DRM_INFO("amdgpu: acceleration disabled, skipping move tests\n");
	}
	if (amdgpu_benchmarking) {
		if (adev->accel_working)
			amdgpu_benchmark(adev, amdgpu_benchmarking);
		else
			DRM_INFO("amdgpu: acceleration disabled, skipping benchmarks\n");
	}

	/*
	 * Register gpu instance before amdgpu_device_enable_mgpu_fan_boost.
	 * Otherwise the mgpu fan boost feature will be skipped due to the
	 * gpu instance is counted less.
	 */
	amdgpu_register_gpu_instance(adev);

	/* enable clockgating, etc. after ib tests, etc. since some blocks require
	 * explicit gating rather than handling it automatically.
	 */
	if (!adev->gmc.xgmi.pending_reset) {
		r = amdgpu_device_ip_late_init(adev);
		if (r) {
			dev_err(adev->dev, "amdgpu_device_ip_late_init failed\n");
			amdgpu_vf_error_put(adev, AMDGIM_ERROR_VF_AMDGPU_LATE_INIT_FAIL, 0, r);
			goto release_ras_con;
		}
		/* must succeed. */
		amdgpu_ras_resume(adev);
		queue_delayed_work(system_wq, &adev->delayed_init_work,
				   msecs_to_jiffies(AMDGPU_RESUME_MS));
	}

	if (amdgpu_sriov_vf(adev))
		flush_delayed_work(&adev->delayed_init_work);

	r = sysfs_create_files(&adev->dev->kobj, amdgpu_dev_attributes);
	if (r)
		dev_err(adev->dev, "Could not create amdgpu device attr\n");

	if (IS_ENABLED(CONFIG_PERF_EVENTS))
		r = amdgpu_pmu_init(adev);
	if (r)
		dev_err(adev->dev, "amdgpu_pmu_init failed\n");

	/* Have stored pci confspace at hand for restore in sudden PCI error */
	if (amdgpu_device_cache_pci_state(adev->pdev))
		pci_restore_state(pdev);

	/* if we have > 1 VGA cards, then disable the amdgpu VGA resources */
	/* this will fail for cards that aren't VGA class devices, just
	 * ignore it */
	if ((adev->pdev->class >> 8) == PCI_CLASS_DISPLAY_VGA)
		vga_client_register(adev->pdev, adev, NULL, amdgpu_device_vga_set_decode);

	if (amdgpu_device_supports_px(ddev)) {
		px = true;
		vga_switcheroo_register_client(adev->pdev,
					       &amdgpu_switcheroo_ops, px);
		vga_switcheroo_init_domain_pm_ops(adev->dev, &adev->vga_pm_domain);
	}

	if (adev->gmc.xgmi.pending_reset)
		queue_delayed_work(system_wq, &mgpu_info.delayed_reset_work,
				   msecs_to_jiffies(AMDGPU_RESUME_MS));

	amdgpu_device_check_iommu_direct_map(adev);

	return 0;

release_ras_con:
	amdgpu_release_ras_context(adev);

failed:
	amdgpu_vf_error_trans_all(adev);

	return r;
}

static void amdgpu_device_unmap_mmio(struct amdgpu_device *adev)
{

	/* Clear all CPU mappings pointing to this device */
#ifdef HAVE_DRM_DRIVER_RELEASE
	unmap_mapping_range(adev->ddev.anon_inode->i_mapping, 0, 0, 1);
#else
	unmap_mapping_range(adev->ddev->anon_inode->i_mapping, 0, 0, 1);
#endif

	/* Unmap all mapped bars - Doorbell, registers and VRAM */
	amdgpu_device_doorbell_fini(adev);

	iounmap(adev->rmmio);
	adev->rmmio = NULL;
	if (adev->mman.aper_base_kaddr)
		iounmap(adev->mman.aper_base_kaddr);
	adev->mman.aper_base_kaddr = NULL;

	/* Memory manager related */
	if (!adev->gmc.xgmi.connected_to_cpu) {
		arch_phys_wc_del(adev->gmc.vram_mtrr);
		arch_io_free_memtype_wc(adev->gmc.aper_base, adev->gmc.aper_size);
	}
}

/**
 * amdgpu_device_fini_hw - tear down the driver
 *
 * @adev: amdgpu_device pointer
 *
 * Tear down the driver info (all asics).
 * Called at driver shutdown.
 */
void amdgpu_device_fini_hw(struct amdgpu_device *adev)
{
	dev_info(adev->dev, "amdgpu: finishing device.\n");
	flush_delayed_work(&adev->delayed_init_work);
	if (adev->mman.initialized) {
		flush_delayed_work(&adev->mman.bdev.wq);
		ttm_bo_lock_delayed_workqueue(&adev->mman.bdev);
	}
	adev->shutdown = true;

	/* make sure IB test finished before entering exclusive mode
	 * to avoid preemption on IB test
	 * */
	if (amdgpu_sriov_vf(adev)) {
		amdgpu_virt_request_full_gpu(adev, false);
		amdgpu_virt_fini_data_exchange(adev);
	}

	/* disable all interrupts */
	amdgpu_irq_disable_all(adev);
	if (adev->mode_info.mode_config_initialized){
		if (!drm_drv_uses_atomic_modeset(adev_to_drm(adev)))
			drm_helper_force_disable_all(adev_to_drm(adev));
#ifdef HAVE_DRM_ATOMIC_HELPER_SHUTDOWN
		else
			drm_atomic_helper_shutdown(adev_to_drm(adev));
#endif
	}
	amdgpu_fence_driver_hw_fini(adev);

	if (adev->pm_sysfs_en)
		amdgpu_pm_sysfs_fini(adev);
	if (adev->ucode_sysfs_en)
		amdgpu_ucode_sysfs_fini(adev);
	sysfs_remove_files(&adev->dev->kobj, amdgpu_dev_attributes);

#ifndef HAVE_DRM_FBDEV_GENERIC_SETUP
	amdgpu_fbdev_fini(adev);
#endif
	/* disable ras feature must before hw fini */
	amdgpu_ras_pre_fini(adev);

	amdgpu_device_ip_fini_early(adev);

	amdgpu_irq_fini_hw(adev);

	if (adev->mman.initialized)
		ttm_device_clear_dma_mappings(&adev->mman.bdev);

	amdgpu_gart_dummy_page_fini(adev);

	if (drm_dev_is_unplugged(adev_to_drm(adev)))
		amdgpu_device_unmap_mmio(adev);

}

void amdgpu_device_fini_sw(struct amdgpu_device *adev)
{
	int idx;

	amdgpu_fence_driver_sw_fini(adev);
	amdgpu_device_ip_fini(adev);
	release_firmware(adev->firmware.gpu_info_fw);
	adev->firmware.gpu_info_fw = NULL;
	adev->accel_working = false;

	amdgpu_reset_fini(adev);

	/* free i2c buses */
	if (!amdgpu_device_has_dc_support(adev))
		amdgpu_i2c_fini(adev);

	if (amdgpu_emu_mode != 1)
		amdgpu_atombios_fini(adev);

	kfree(adev->bios);
	adev->bios = NULL;
	if (amdgpu_device_supports_px(adev_to_drm(adev))) {
		vga_switcheroo_unregister_client(adev->pdev);
		vga_switcheroo_fini_domain_pm_ops(adev->dev);
	}
	if ((adev->pdev->class >> 8) == PCI_CLASS_DISPLAY_VGA)
		vga_client_register(adev->pdev, NULL, NULL, NULL);

	if (drm_dev_enter(adev_to_drm(adev), &idx)) {

		iounmap(adev->rmmio);
		adev->rmmio = NULL;
		amdgpu_device_doorbell_fini(adev);
		drm_dev_exit(idx);
	}

	if (IS_ENABLED(CONFIG_PERF_EVENTS))
		amdgpu_pmu_fini(adev);
	if (adev->mman.discovery_bin)
		amdgpu_discovery_fini(adev);

	kfree(adev->pci_state);

}

/**
 * amdgpu_device_evict_resources - evict device resources
 * @adev: amdgpu device object
 *
 * Evicts all ttm device resources(vram BOs, gart table) from the lru list
 * of the vram memory type. Mainly used for evicting device resources
 * at suspend time.
 *
 */
static void amdgpu_device_evict_resources(struct amdgpu_device *adev)
{
	/* No need to evict vram on APUs for suspend to ram or s2idle */
	if ((adev->in_s3 || adev->in_s0ix) && (adev->flags & AMD_IS_APU))
		return;

	if (amdgpu_ttm_evict_resources(adev, TTM_PL_VRAM))
		DRM_WARN("evicting device resources failed\n");

}

/*
 * Suspend & resume.
 */
/**
 * amdgpu_device_suspend - initiate device suspend
 *
 * @dev: drm dev pointer
 * @fbcon : notify the fbdev of suspend
 *
 * Puts the hw in the suspend state (all asics).
 * Returns 0 for success or an error on failure.
 * Called at driver suspend.
 */
int amdgpu_device_suspend(struct drm_device *dev, bool fbcon)
{
	struct amdgpu_device *adev = drm_to_adev(dev);

	if (dev->switch_power_state == DRM_SWITCH_POWER_OFF)
		return 0;

	adev->in_suspend = true;

	if (amdgpu_acpi_smart_shift_update(dev, AMDGPU_SS_DEV_D3))
		DRM_WARN("smart shift update failed\n");

	drm_kms_helper_poll_disable(dev);

	if (fbcon)
#ifdef HAVE_DRM_FBDEV_GENERIC_SETUP
		drm_fb_helper_set_suspend_unlocked(adev_to_drm(adev)->fb_helper, true);
#else
		amdgpu_fbdev_set_suspend(adev, 1);
#endif

	cancel_delayed_work_sync(&adev->delayed_init_work);

	amdgpu_ras_suspend(adev);

	amdgpu_device_ip_suspend_phase1(adev);

	if (!adev->in_s0ix)
		amdgpu_amdkfd_suspend(adev, adev->in_runpm);

	amdgpu_device_evict_resources(adev);

	amdgpu_fence_driver_hw_fini(adev);

	amdgpu_device_ip_suspend_phase2(adev);

	return 0;
}

/**
 * amdgpu_device_resume - initiate device resume
 *
 * @dev: drm dev pointer
 * @fbcon : notify the fbdev of resume
 *
 * Bring the hw back to operating state (all asics).
 * Returns 0 for success or an error on failure.
 * Called at driver resume.
 */
int amdgpu_device_resume(struct drm_device *dev, bool fbcon)
{
	struct amdgpu_device *adev = drm_to_adev(dev);
	int r = 0;

	if (dev->switch_power_state == DRM_SWITCH_POWER_OFF)
		return 0;

	if (adev->in_s0ix)
		amdgpu_dpm_gfx_state_change(adev, sGpuChangeState_D0Entry);

	/* post card */
	if (amdgpu_device_need_post(adev)) {
		r = amdgpu_device_asic_init(adev);
		if (r)
			dev_err(adev->dev, "amdgpu asic init failed\n");
	}

	r = amdgpu_device_ip_resume(adev);
	if (r) {
		dev_err(adev->dev, "amdgpu_device_ip_resume failed (%d).\n", r);
		return r;
	}
	amdgpu_fence_driver_hw_init(adev);

	r = amdgpu_device_ip_late_init(adev);
	if (r)
		return r;

	queue_delayed_work(system_wq, &adev->delayed_init_work,
			   msecs_to_jiffies(AMDGPU_RESUME_MS));

	if (!adev->in_s0ix) {
		r = amdgpu_amdkfd_resume(adev, adev->in_runpm, false);
		if (r)
			return r;
	}

	/* Make sure IB tests flushed */
	flush_delayed_work(&adev->delayed_init_work);

	if (fbcon)
#ifdef HAVE_DRM_FBDEV_GENERIC_SETUP
		drm_fb_helper_set_suspend_unlocked(adev_to_drm(adev)->fb_helper, false);
#else
		amdgpu_fbdev_set_suspend(adev, 0);
#endif

	drm_kms_helper_poll_enable(dev);

	amdgpu_ras_resume(adev);

	/*
	 * Most of the connector probing functions try to acquire runtime pm
	 * refs to ensure that the GPU is powered on when connector polling is
	 * performed. Since we're calling this from a runtime PM callback,
	 * trying to acquire rpm refs will cause us to deadlock.
	 *
	 * Since we're guaranteed to be holding the rpm lock, it's safe to
	 * temporarily disable the rpm helpers so this doesn't deadlock us.
	 */
#ifdef CONFIG_PM
	dev->dev->power.disable_depth++;
#endif
	if (!amdgpu_device_has_dc_support(adev))
		drm_helper_hpd_irq_event(dev);
	else
		drm_kms_helper_hotplug_event(dev);
#ifdef CONFIG_PM
	dev->dev->power.disable_depth--;
#endif
	adev->in_suspend = false;

	if (amdgpu_acpi_smart_shift_update(dev, AMDGPU_SS_DEV_D0))
		DRM_WARN("smart shift update failed\n");

	return 0;
}

/**
 * amdgpu_device_ip_check_soft_reset - did soft reset succeed
 *
 * @adev: amdgpu_device pointer
 *
 * The list of all the hardware IPs that make up the asic is walked and
 * the check_soft_reset callbacks are run.  check_soft_reset determines
 * if the asic is still hung or not.
 * Returns true if any of the IPs are still in a hung state, false if not.
 */
static bool amdgpu_device_ip_check_soft_reset(struct amdgpu_device *adev)
{
	int i;
	bool asic_hang = false;

	if (amdgpu_sriov_vf(adev))
		return true;

	if (amdgpu_asic_need_full_reset(adev))
		return true;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].version->funcs->check_soft_reset)
			adev->ip_blocks[i].status.hang =
				adev->ip_blocks[i].version->funcs->check_soft_reset(adev);
		if (adev->ip_blocks[i].status.hang) {
			dev_info(adev->dev, "IP block:%s is hung!\n", adev->ip_blocks[i].version->funcs->name);
			asic_hang = true;
		}
	}
	return asic_hang;
}

/**
 * amdgpu_device_ip_pre_soft_reset - prepare for soft reset
 *
 * @adev: amdgpu_device pointer
 *
 * The list of all the hardware IPs that make up the asic is walked and the
 * pre_soft_reset callbacks are run if the block is hung.  pre_soft_reset
 * handles any IP specific hardware or software state changes that are
 * necessary for a soft reset to succeed.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_pre_soft_reset(struct amdgpu_device *adev)
{
	int i, r = 0;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].status.hang &&
		    adev->ip_blocks[i].version->funcs->pre_soft_reset) {
			r = adev->ip_blocks[i].version->funcs->pre_soft_reset(adev);
			if (r)
				return r;
		}
	}

	return 0;
}

/**
 * amdgpu_device_ip_need_full_reset - check if a full asic reset is needed
 *
 * @adev: amdgpu_device pointer
 *
 * Some hardware IPs cannot be soft reset.  If they are hung, a full gpu
 * reset is necessary to recover.
 * Returns true if a full asic reset is required, false if not.
 */
static bool amdgpu_device_ip_need_full_reset(struct amdgpu_device *adev)
{
	int i;

	if (amdgpu_asic_need_full_reset(adev))
		return true;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if ((adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_GMC) ||
		    (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_SMC) ||
		    (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_ACP) ||
		    (adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_DCE) ||
		     adev->ip_blocks[i].version->type == AMD_IP_BLOCK_TYPE_PSP) {
			if (adev->ip_blocks[i].status.hang) {
				dev_info(adev->dev, "Some block need full reset!\n");
				return true;
			}
		}
	}
	return false;
}

/**
 * amdgpu_device_ip_soft_reset - do a soft reset
 *
 * @adev: amdgpu_device pointer
 *
 * The list of all the hardware IPs that make up the asic is walked and the
 * soft_reset callbacks are run if the block is hung.  soft_reset handles any
 * IP specific hardware or software state changes that are necessary to soft
 * reset the IP.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_soft_reset(struct amdgpu_device *adev)
{
	int i, r = 0;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].status.hang &&
		    adev->ip_blocks[i].version->funcs->soft_reset) {
			r = adev->ip_blocks[i].version->funcs->soft_reset(adev);
			if (r)
				return r;
		}
	}

	return 0;
}

/**
 * amdgpu_device_ip_post_soft_reset - clean up from soft reset
 *
 * @adev: amdgpu_device pointer
 *
 * The list of all the hardware IPs that make up the asic is walked and the
 * post_soft_reset callbacks are run if the asic was hung.  post_soft_reset
 * handles any IP specific hardware or software state changes that are
 * necessary after the IP has been soft reset.
 * Returns 0 on success, negative error code on failure.
 */
static int amdgpu_device_ip_post_soft_reset(struct amdgpu_device *adev)
{
	int i, r = 0;

	for (i = 0; i < adev->num_ip_blocks; i++) {
		if (!adev->ip_blocks[i].status.valid)
			continue;
		if (adev->ip_blocks[i].status.hang &&
		    adev->ip_blocks[i].version->funcs->post_soft_reset)
			r = adev->ip_blocks[i].version->funcs->post_soft_reset(adev);
		if (r)
			return r;
	}

	return 0;
}

/**
 * amdgpu_device_recover_vram - Recover some VRAM contents
 *
 * @adev: amdgpu_device pointer
 *
 * Restores the contents of VRAM buffers from the shadows in GTT.  Used to
 * restore things like GPUVM page tables after a GPU reset where
 * the contents of VRAM might be lost.
 *
 * Returns:
 * 0 on success, negative error code on failure.
 */
static int amdgpu_device_recover_vram(struct amdgpu_device *adev)
{
	struct dma_fence *fence = NULL, *next = NULL;
	struct amdgpu_bo *shadow;
	struct amdgpu_bo_vm *vmbo;
	long r = 1, tmo;

	if (amdgpu_sriov_runtime(adev))
		tmo = msecs_to_jiffies(8000);
	else
		tmo = msecs_to_jiffies(100);

	dev_info(adev->dev, "recover vram bo from shadow start\n");
	mutex_lock(&adev->shadow_list_lock);
	list_for_each_entry(vmbo, &adev->shadow_list, shadow_list) {
		shadow = &vmbo->bo;
		/* No need to recover an evicted BO */
		if (shadow->tbo.resource->mem_type != TTM_PL_TT ||
		    shadow->tbo.resource->start == AMDGPU_BO_INVALID_OFFSET ||
		    shadow->parent->tbo.resource->mem_type != TTM_PL_VRAM)
			continue;

		r = amdgpu_bo_restore_shadow(shadow, &next);
		if (r)
			break;

		if (fence) {
			tmo = dma_fence_wait_timeout(fence, false, tmo);
			dma_fence_put(fence);
			fence = next;
			if (tmo == 0) {
				r = -ETIMEDOUT;
				break;
			} else if (tmo < 0) {
				r = tmo;
				break;
			}
		} else {
			fence = next;
		}
	}
	mutex_unlock(&adev->shadow_list_lock);

	if (fence)
		tmo = dma_fence_wait_timeout(fence, false, tmo);
	dma_fence_put(fence);

	if (r < 0 || tmo <= 0) {
		dev_err(adev->dev, "recover vram bo from shadow failed, r is %ld, tmo is %ld\n", r, tmo);
		return -EIO;
	}

	dev_info(adev->dev, "recover vram bo from shadow done\n");
	return 0;
}


/**
 * amdgpu_device_reset_sriov - reset ASIC for SR-IOV vf
 *
 * @adev: amdgpu_device pointer
 * @from_hypervisor: request from hypervisor
 *
 * do VF FLR and reinitialize Asic
 * return 0 means succeeded otherwise failed
 */
static int amdgpu_device_reset_sriov(struct amdgpu_device *adev,
				     bool from_hypervisor)
{
	int r;
	struct amdgpu_hive_info *hive = NULL;
	int retry_limit = 0;

retry:
	amdgpu_amdkfd_pre_reset(adev);

	if (from_hypervisor)
		r = amdgpu_virt_request_full_gpu(adev, true);
	else
		r = amdgpu_virt_reset_gpu(adev);
	if (r)
		return r;

	/* Resume IP prior to SMC */
	r = amdgpu_device_ip_reinit_early_sriov(adev);
	if (r)
		goto error;

	amdgpu_virt_init_data_exchange(adev);

	r = amdgpu_device_fw_loading(adev);
	if (r)
		return r;

	/* now we are okay to resume SMC/CP/SDMA */
	r = amdgpu_device_ip_reinit_late_sriov(adev);
	if (r)
		goto error;

	hive = amdgpu_get_xgmi_hive(adev);
	/* Update PSP FW topology after reset */
	if (hive && adev->gmc.xgmi.num_physical_nodes > 1)
		r = amdgpu_xgmi_update_topology(hive, adev);

	if (hive)
		amdgpu_put_xgmi_hive(hive);

	if (!r) {
		amdgpu_irq_gpu_reset_resume_helper(adev);
		r = amdgpu_ib_ring_tests(adev);
		amdgpu_amdkfd_post_reset(adev);
	}

error:
	if (!r && adev->virt.gim_feature & AMDGIM_FEATURE_GIM_FLR_VRAMLOST) {
		amdgpu_inc_vram_lost(adev);
		r = amdgpu_device_recover_vram(adev);
	}
	amdgpu_virt_release_full_gpu(adev, true);

	if (AMDGPU_RETRY_SRIOV_RESET(r)) {
		if (retry_limit < AMDGPU_MAX_RETRY_LIMIT) {
			retry_limit++;
			goto retry;
		} else
			DRM_ERROR("GPU reset retry is beyond the retry limit\n");
	}

	return r;
}

/**
 * amdgpu_device_has_job_running - check if there is any job in mirror list
 *
 * @adev: amdgpu_device pointer
 *
 * check if there is any job in mirror list
 */
bool amdgpu_device_has_job_running(struct amdgpu_device *adev)
{
	int i;
	struct drm_sched_job *job;

	for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
		struct amdgpu_ring *ring = adev->rings[i];

		if (!ring || !ring->sched.thread)
			continue;

		spin_lock(&ring->sched.job_list_lock);
		job = list_first_entry_or_null(&ring->sched.pending_list,
					       struct drm_sched_job, list);
		spin_unlock(&ring->sched.job_list_lock);
		if (job)
			return true;
	}
	return false;
}

/**
 * amdgpu_device_should_recover_gpu - check if we should try GPU recovery
 *
 * @adev: amdgpu_device pointer
 *
 * Check amdgpu_gpu_recovery and SRIOV status to see if we should try to recover
 * a hung GPU.
 */
bool amdgpu_device_should_recover_gpu(struct amdgpu_device *adev)
{
	if (!amdgpu_device_ip_check_soft_reset(adev)) {
		dev_info(adev->dev, "Timeout, but no hardware hang detected.\n");
		return false;
	}

	if (amdgpu_gpu_recovery == 0)
		goto disabled;

	if (amdgpu_sriov_vf(adev))
		return true;

	if (amdgpu_gpu_recovery == -1) {
		switch (adev->asic_type) {
#ifdef CONFIG_DRM_AMDGPU_SI
		case CHIP_VERDE:
		case CHIP_TAHITI:
		case CHIP_PITCAIRN:
		case CHIP_OLAND:
		case CHIP_HAINAN:
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
		case CHIP_KAVERI:
		case CHIP_KABINI:
		case CHIP_MULLINS:
#endif
		case CHIP_CARRIZO:
		case CHIP_STONEY:
		case CHIP_CYAN_SKILLFISH:
			goto disabled;
		default:
			break;
		}
	}

	return true;

disabled:
		dev_info(adev->dev, "GPU recovery disabled.\n");
		return false;
}

int amdgpu_device_mode1_reset(struct amdgpu_device *adev)
{
        u32 i;
        int ret = 0;

        amdgpu_atombios_scratch_regs_engine_hung(adev, true);

        dev_info(adev->dev, "GPU mode1 reset\n");

        /* disable BM */
        pci_clear_master(adev->pdev);

        amdgpu_device_cache_pci_state(adev->pdev);

        if (amdgpu_dpm_is_mode1_reset_supported(adev)) {
                dev_info(adev->dev, "GPU smu mode1 reset\n");
                ret = amdgpu_dpm_mode1_reset(adev);
        } else {
                dev_info(adev->dev, "GPU psp mode1 reset\n");
                ret = psp_gpu_reset(adev);
        }

        if (ret)
                dev_err(adev->dev, "GPU mode1 reset failed\n");

        amdgpu_device_load_pci_state(adev->pdev);

        /* wait for asic to come out of reset */
        for (i = 0; i < adev->usec_timeout; i++) {
                u32 memsize = adev->nbio.funcs->get_memsize(adev);

                if (memsize != 0xffffffff)
                        break;
                udelay(1);
        }

        amdgpu_atombios_scratch_regs_engine_hung(adev, false);
        return ret;
}

int amdgpu_device_pre_asic_reset(struct amdgpu_device *adev,
				 struct amdgpu_reset_context *reset_context)
{
	int i, r = 0;
	struct amdgpu_job *job = NULL;
	bool need_full_reset =
		test_bit(AMDGPU_NEED_FULL_RESET, &reset_context->flags);

	if (reset_context->reset_req_dev == adev)
		job = reset_context->job;

	if (amdgpu_sriov_vf(adev)) {
		/* stop the data exchange thread */
		amdgpu_virt_fini_data_exchange(adev);
	}

	/* block all schedulers and reset given job's ring */
	for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
		struct amdgpu_ring *ring = adev->rings[i];

		if (!ring || !ring->sched.thread)
			continue;

		/*clear job fence from fence drv to avoid force_completion
		 *leave NULL and vm flush fence in fence drv */
		amdgpu_fence_driver_clear_job_fences(ring);

		/* after all hw jobs are reset, hw fence is meaningless, so force_completion */
		amdgpu_fence_driver_force_completion(ring);
	}

	if (job && job->vm)
		drm_sched_increase_karma(&job->base);

	r = amdgpu_reset_prepare_hwcontext(adev, reset_context);
	/* If reset handler not implemented, continue; otherwise return */
	if (r == -ENOSYS)
		r = 0;
	else
		return r;

	/* Don't suspend on bare metal if we are not going to HW reset the ASIC */
	if (!amdgpu_sriov_vf(adev)) {

		if (!need_full_reset)
			need_full_reset = amdgpu_device_ip_need_full_reset(adev);

		if (!need_full_reset) {
			amdgpu_device_ip_pre_soft_reset(adev);
			r = amdgpu_device_ip_soft_reset(adev);
			amdgpu_device_ip_post_soft_reset(adev);
			if (r || amdgpu_device_ip_check_soft_reset(adev)) {
				dev_info(adev->dev, "soft reset failed, will fallback to full reset!\n");
				need_full_reset = true;
			}
		}

		if (need_full_reset)
			r = amdgpu_device_ip_suspend(adev);
		if (need_full_reset)
			set_bit(AMDGPU_NEED_FULL_RESET, &reset_context->flags);
		else
			clear_bit(AMDGPU_NEED_FULL_RESET,
				  &reset_context->flags);
	}

	return r;
}

int amdgpu_do_asic_reset(struct list_head *device_list_handle,
			 struct amdgpu_reset_context *reset_context)
{
	struct amdgpu_device *tmp_adev = NULL;
	bool need_full_reset, skip_hw_reset, vram_lost = false;
	int r = 0;

	/* Try reset handler method first */
	tmp_adev = list_first_entry(device_list_handle, struct amdgpu_device,
				    reset_list);
	r = amdgpu_reset_perform_reset(tmp_adev, reset_context);
	/* If reset handler not implemented, continue; otherwise return */
	if (r == -ENOSYS)
		r = 0;
	else
		return r;

	/* Reset handler not implemented, use the default method */
	need_full_reset =
		test_bit(AMDGPU_NEED_FULL_RESET, &reset_context->flags);
	skip_hw_reset = test_bit(AMDGPU_SKIP_HW_RESET, &reset_context->flags);

	/*
	 * ASIC reset has to be done on all XGMI hive nodes ASAP
	 * to allow proper links negotiation in FW (within 1 sec)
	 */
	if (!skip_hw_reset && need_full_reset) {
		list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
			/* For XGMI run all resets in parallel to speed up the process */
			if (tmp_adev->gmc.xgmi.num_physical_nodes > 1) {
				tmp_adev->gmc.xgmi.pending_reset = false;
				if (!queue_work(system_unbound_wq, &tmp_adev->xgmi_reset_work))
					r = -EALREADY;
			} else
				r = amdgpu_asic_reset(tmp_adev);

			if (r) {
				dev_err(tmp_adev->dev, "ASIC reset failed with error, %d for drm dev, %s",
					 r, adev_to_drm(tmp_adev)->unique);
				break;
			}
		}

		/* For XGMI wait for all resets to complete before proceed */
		if (!r) {
			list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
				if (tmp_adev->gmc.xgmi.num_physical_nodes > 1) {
					flush_work(&tmp_adev->xgmi_reset_work);
					r = tmp_adev->asic_reset_res;
					if (r)
						break;
				}
			}
		}
	}

	if (!r && amdgpu_ras_intr_triggered()) {
		list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
			if (tmp_adev->mmhub.ras && tmp_adev->mmhub.ras->ras_block.hw_ops &&
			    tmp_adev->mmhub.ras->ras_block.hw_ops->reset_ras_error_count)
				tmp_adev->mmhub.ras->ras_block.hw_ops->reset_ras_error_count(tmp_adev);
		}

		amdgpu_ras_intr_cleared();
	}

	list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
		if (need_full_reset) {
			/* post card */
			r = amdgpu_device_asic_init(tmp_adev);
			if (r) {
				dev_warn(tmp_adev->dev, "asic atom init failed!");
			} else {
				dev_info(tmp_adev->dev, "GPU reset succeeded, trying to resume\n");
				r = amdgpu_amdkfd_resume_iommu(tmp_adev);
				if (r)
					goto out;

				r = amdgpu_device_ip_resume_phase1(tmp_adev);
				if (r)
					goto out;

				vram_lost = amdgpu_device_check_vram_lost(tmp_adev);
				if (vram_lost) {
					DRM_INFO("VRAM is lost due to GPU reset!\n");
					amdgpu_inc_vram_lost(tmp_adev);
				}

				r = amdgpu_device_fw_loading(tmp_adev);
				if (r)
					return r;

				r = amdgpu_device_ip_resume_phase2(tmp_adev);
				if (r)
					goto out;

				if (vram_lost)
					amdgpu_device_fill_reset_magic(tmp_adev);

				/*
				 * Add this ASIC as tracked as reset was already
				 * complete successfully.
				 */
				amdgpu_register_gpu_instance(tmp_adev);

				if (!reset_context->hive &&
				    tmp_adev->gmc.xgmi.num_physical_nodes > 1)
					amdgpu_xgmi_add_device(tmp_adev);

				r = amdgpu_device_ip_late_init(tmp_adev);
				if (r)
					goto out;

#ifdef HAVE_DRM_FBDEV_GENERIC_SETUP
				drm_fb_helper_set_suspend_unlocked(adev_to_drm(tmp_adev)->fb_helper, false);
#else
				amdgpu_fbdev_set_suspend(tmp_adev, 0);
#endif

				/*
				 * The GPU enters bad state once faulty pages
				 * by ECC has reached the threshold, and ras
				 * recovery is scheduled next. So add one check
				 * here to break recovery if it indeed exceeds
				 * bad page threshold, and remind user to
				 * retire this GPU or setting one bigger
				 * bad_page_threshold value to fix this once
				 * probing driver again.
				 */
				if (!amdgpu_ras_eeprom_check_err_threshold(tmp_adev)) {
					/* must succeed. */
					amdgpu_ras_resume(tmp_adev);
				} else {
					r = -EINVAL;
					goto out;
				}

				/* Update PSP FW topology after reset */
				if (reset_context->hive &&
				    tmp_adev->gmc.xgmi.num_physical_nodes > 1)
					r = amdgpu_xgmi_update_topology(
						reset_context->hive, tmp_adev);
			}
		}

out:
		if (!r) {
			amdgpu_irq_gpu_reset_resume_helper(tmp_adev);
			r = amdgpu_ib_ring_tests(tmp_adev);
			if (r) {
				dev_err(tmp_adev->dev, "ib ring test failed (%d).\n", r);
				need_full_reset = true;
				r = -EAGAIN;
				goto end;
			}
		}

		if (!r)
			r = amdgpu_device_recover_vram(tmp_adev);
		else
			tmp_adev->asic_reset_res = r;
	}

end:
	if (need_full_reset)
		set_bit(AMDGPU_NEED_FULL_RESET, &reset_context->flags);
	else
		clear_bit(AMDGPU_NEED_FULL_RESET, &reset_context->flags);
	return r;
}

static bool amdgpu_device_recovery_enter(struct amdgpu_device *adev)
{
	if (atomic_cmpxchg(&adev->in_gpu_reset, 0, 1) != 0)
		return false;

	return true;
}

static void amdgpu_device_recovery_exit(struct amdgpu_device *adev)
{
	atomic_set(&adev->in_gpu_reset, 0);
	wake_up_all(&adev->recovery_fini_event);
}

/*
 * to lockup a list of amdgpu devices in a hive safely, if not a hive
 * with multiple nodes, it will be similar as amdgpu_device_lock_adev.
 *
 * unlock won't require roll back.
 */
static int amdgpu_hive_recovery_enter(struct amdgpu_device *adev, struct amdgpu_hive_info *hive)
{
	struct amdgpu_device *tmp_adev = NULL;

	if (!amdgpu_sriov_vf(adev) && (adev->gmc.xgmi.num_physical_nodes > 1)) {
		if (!hive) {
			dev_err(adev->dev, "Hive is NULL while device has multiple xgmi nodes");
			return -ENODEV;
		}
		list_for_each_entry(tmp_adev, &hive->device_list, gmc.xgmi.head) {
			if (!amdgpu_device_recovery_enter(tmp_adev))
				goto roll_back;
		}
	} else if (!amdgpu_device_recovery_enter(adev))
		return -EAGAIN;

	return 0;
roll_back:
	if (!list_is_first(&tmp_adev->gmc.xgmi.head, &hive->device_list)) {
		/*
		 * if the lockup iteration break in the middle of a hive,
		 * it may means there may has a race issue,
		 * or a hive device locked up independently.
		 * we may be in trouble and may not, so will try to roll back
		 * the lock and give out a warnning.
		 */
		dev_warn(tmp_adev->dev, "Hive lock iteration broke in the middle. Rolling back to unlock");
		list_for_each_entry_continue_reverse(tmp_adev, &hive->device_list, gmc.xgmi.head) {
			amdgpu_device_recovery_exit(tmp_adev);
		}
	}
	return -EAGAIN;
}

static void amdgpu_device_lock_adev(struct amdgpu_device *adev,
				struct amdgpu_hive_info *hive)
{
	amdgpu_write_lock(adev, hive);

	switch (amdgpu_asic_reset_method(adev)) {
	case AMD_RESET_METHOD_MODE1:
		adev->mp1_state = PP_MP1_STATE_SHUTDOWN;
		break;
	case AMD_RESET_METHOD_MODE2:
		adev->mp1_state = PP_MP1_STATE_RESET;
		break;
	default:
		adev->mp1_state = PP_MP1_STATE_NONE;
		break;
	}
}

static void amdgpu_device_unlock_adev(struct amdgpu_device *adev)
{
	amdgpu_vf_error_trans_all(adev);
	adev->mp1_state = PP_MP1_STATE_NONE;
	amdgpu_write_unlock(adev);
}

/*
 * to lockup a list of amdgpu devices in a hive safely, if not a hive
 * with multiple nodes, it will be similar as amdgpu_device_lock_adev.
 *
 * unlock won't require roll back.
 */
static void amdgpu_device_lock_hive_adev(struct amdgpu_device *adev, struct amdgpu_hive_info *hive)
{
	struct amdgpu_device *tmp_adev = NULL;

	if (!amdgpu_sriov_vf(adev) && (adev->gmc.xgmi.num_physical_nodes > 1)) {
		if (!hive) {
			dev_err(adev->dev, "Hive is NULL while device has multiple xgmi nodes");
			return;
		}

		list_for_each_entry(tmp_adev, &hive->device_list, gmc.xgmi.head) {
			amdgpu_device_lock_adev(tmp_adev, hive);
		}
	} else {
		amdgpu_device_lock_adev(adev, hive);
	}
}

static void amdgpu_device_resume_display_audio(struct amdgpu_device *adev)
{
	struct pci_dev *p = NULL;

	p = pci_get_domain_bus_and_slot(pci_domain_nr(adev->pdev->bus),
			adev->pdev->bus->number, 1);
	if (p) {
		pm_runtime_enable(&(p->dev));
		pm_runtime_resume(&(p->dev));
	}
}

static int amdgpu_device_suspend_display_audio(struct amdgpu_device *adev)
{
	enum amd_reset_method reset_method;
	struct pci_dev *p = NULL;
	u64 expires;

	/*
	 * For now, only BACO and mode1 reset are confirmed
	 * to suffer the audio issue without proper suspended.
	 */
	reset_method = amdgpu_asic_reset_method(adev);
	if ((reset_method != AMD_RESET_METHOD_BACO) &&
	     (reset_method != AMD_RESET_METHOD_MODE1))
		return -EINVAL;

	p = pci_get_domain_bus_and_slot(pci_domain_nr(adev->pdev->bus),
			adev->pdev->bus->number, 1);
	if (!p)
		return -ENODEV;

	expires = pm_runtime_autosuspend_expiration(&(p->dev));
	if (!expires)
		/*
		 * If we cannot get the audio device autosuspend delay,
		 * a fixed 4S interval will be used. Considering 3S is
		 * the audio controller default autosuspend delay setting.
		 * 4S used here is guaranteed to cover that.
		 */
		expires = ktime_get_mono_fast_ns() + NSEC_PER_SEC * 4ULL;

	while (!pm_runtime_status_suspended(&(p->dev))) {
		if (!pm_runtime_suspend(&(p->dev)))
			break;

		if (expires < ktime_get_mono_fast_ns()) {
			dev_warn(adev->dev, "failed to suspend display audio\n");
			/* TODO: abort the succeeding gpu reset? */
			return -ETIMEDOUT;
		}
	}

	pm_runtime_disable(&(p->dev));

	return 0;
}

static void amdgpu_device_recheck_guilty_jobs(
	struct amdgpu_device *adev, struct list_head *device_list_handle,
	struct amdgpu_reset_context *reset_context)
{
	int i, r = 0;

	for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
		struct amdgpu_ring *ring = adev->rings[i];
		int ret = 0;
		struct drm_sched_job *s_job;

		if (!ring || !ring->sched.thread)
			continue;

		s_job = list_first_entry_or_null(&ring->sched.pending_list,
				struct drm_sched_job, list);
		if (s_job == NULL)
			continue;

		/* clear job's guilty and depend the folowing step to decide the real one */
		drm_sched_reset_karma(s_job);
		/* for the real bad job, it will be resubmitted twice, adding a dma_fence_get
		 * to make sure fence is balanced */
		dma_fence_get(s_job->s_fence->parent);
		drm_sched_resubmit_jobs_ext(&ring->sched, 1);

		ret = dma_fence_wait_timeout(s_job->s_fence->parent, false, ring->sched.timeout);
		if (ret == 0) { /* timeout */
			DRM_ERROR("Found the real bad job! ring:%s, job_id:%llx\n",
						ring->sched.name, s_job->id);

			/* set guilty */
			drm_sched_increase_karma(s_job);
retry:
			/* do hw reset */
			if (amdgpu_sriov_vf(adev)) {
				amdgpu_virt_fini_data_exchange(adev);
				r = amdgpu_device_reset_sriov(adev, false);
				if (r)
					adev->asic_reset_res = r;
			} else {
				clear_bit(AMDGPU_SKIP_HW_RESET,
					  &reset_context->flags);
				r = amdgpu_do_asic_reset(device_list_handle,
							 reset_context);
				if (r && r == -EAGAIN)
					goto retry;
			}

			/*
			 * add reset counter so that the following
			 * resubmitted job could flush vmid
			 */
			atomic_inc(&adev->gpu_reset_counter);
			continue;
		}

		/* got the hw fence, signal finished fence */
		atomic_dec(ring->sched.score);
		dma_fence_put(s_job->s_fence->parent);
		dma_fence_get(&s_job->s_fence->finished);
		dma_fence_signal(&s_job->s_fence->finished);
		dma_fence_put(&s_job->s_fence->finished);

		/* remove node from list and free the job */
		spin_lock(&ring->sched.job_list_lock);
		list_del_init(&s_job->list);
		spin_unlock(&ring->sched.job_list_lock);
		ring->sched.ops->free_job(s_job);
	}
}

/**
 * amdgpu_device_gpu_recover - reset the asic and recover scheduler
 *
 * @adev: amdgpu_device pointer
 * @job: which job trigger hang
 *
 * Attempt to reset the GPU if it has hung (all asics).
 * Attempt to do soft-reset or full-reset and reinitialize Asic
 * Returns 0 for success or an error on failure.
 */

int amdgpu_device_gpu_recover(struct amdgpu_device *adev,
			      struct amdgpu_job *job)
{
	struct list_head device_list, *device_list_handle =  NULL;
	bool job_signaled = false;
	struct amdgpu_hive_info *hive = NULL;
	struct amdgpu_device *tmp_adev = NULL;
	int i, r = 0;
	bool need_emergency_restart = false;
	bool audio_suspended = false;
	bool locked = false;
	int tmp_vram_lost_counter;
	struct amdgpu_reset_context reset_context;

	memset(&reset_context, 0, sizeof(reset_context));

	/*
	 * Special case: RAS triggered and full reset isn't supported
	 */
	need_emergency_restart = amdgpu_ras_need_emergency_restart(adev);

	/*
	 * Flush RAM to disk so that after reboot
	 * the user can read log and see why the system rebooted.
	 */
	if (need_emergency_restart && amdgpu_ras_get_context(adev)->reboot) {
		DRM_WARN("Emergency reboot.");

		ksys_sync_helper();
		emergency_restart();
	}

	dev_info(adev->dev, "GPU %s begin!\n",
		need_emergency_restart ? "jobs stop":"reset");

	/*
	 * Here we trylock to avoid chain of resets executing from
	 * either trigger by jobs on different adevs in XGMI hive or jobs on
	 * different schedulers for same device while this TO handler is running.
	 * We always reset all schedulers for device and all devices for XGMI
	 * hive so that should take care of them too.
	 */
	if (!amdgpu_sriov_vf(adev))
		hive = amdgpu_get_xgmi_hive(adev);
	if (hive) {
		if (atomic_cmpxchg(&hive->in_reset, 0, 1) != 0) {
			DRM_INFO("Bailing on TDR for s_job:%llx, hive: %llx as another already in progress",
				job ? job->base.id : -1, hive->hive_id);
			amdgpu_put_xgmi_hive(hive);
			if (job && job->vm)
				drm_sched_increase_karma(&job->base);
			return 0;
		}
		mutex_lock(&hive->hive_lock);
	}

	reset_context.method = AMD_RESET_METHOD_NONE;
	reset_context.reset_req_dev = adev;
	reset_context.job = job;
	reset_context.hive = hive;
	clear_bit(AMDGPU_NEED_FULL_RESET, &reset_context.flags);

	/*
	 * lock the device before we try to operate the linked list
	 * if didn't get the device lock, don't touch the linked list since
	 * others may iterating it.
	 */
	r = amdgpu_hive_recovery_enter(adev, hive);
	if (r) {
		dev_info(adev->dev, "Bailing on TDR for s_job:%llx, as another already in progress",
					job ? job->base.id : -1);

		/* even we skipped this reset, still need to set the job to guilty */
		if (job && job->vm)
			drm_sched_increase_karma(&job->base);
		goto skip_recovery;
	}

	/*
	 * Build list of devices to reset.
	 * In case we are in XGMI hive mode, resort the device list
	 * to put adev in the 1st position.
	 */
	INIT_LIST_HEAD(&device_list);
	if (!amdgpu_sriov_vf(adev) && (adev->gmc.xgmi.num_physical_nodes > 1)) {
		list_for_each_entry(tmp_adev, &hive->device_list, gmc.xgmi.head)
			list_add_tail(&tmp_adev->reset_list, &device_list);
		if (!list_is_first(&adev->reset_list, &device_list))
			list_rotate_to_front(&adev->reset_list, &device_list);
		device_list_handle = &device_list;
	} else {
		list_add_tail(&adev->reset_list, &device_list);
		device_list_handle = &device_list;
	}

	/* block all schedulers and reset given job's ring */
	list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
		/*
		 * Try to put the audio codec into suspend state
		 * before gpu reset started.
		 *
		 * Due to the power domain of the graphics device
		 * is shared with AZ power domain. Without this,
		 * we may change the audio hardware from behind
		 * the audio driver's back. That will trigger
		 * some audio codec errors.
		 */
		if (!amdgpu_device_suspend_display_audio(tmp_adev))
			audio_suspended = true;

		amdgpu_ras_set_error_query_ready(tmp_adev, false);

		cancel_delayed_work_sync(&tmp_adev->delayed_init_work);

		if (!amdgpu_sriov_vf(tmp_adev))
			amdgpu_amdkfd_pre_reset(tmp_adev);

		/*
		 * Mark these ASICs to be reseted as untracked first
		 * And add them back after reset completed
		 */
		amdgpu_unregister_gpu_instance(tmp_adev);

#ifdef HAVE_DRM_FBDEV_GENERIC_SETUP
		drm_fb_helper_set_suspend_unlocked(adev_to_drm(adev)->fb_helper, true);
#else
		amdgpu_fbdev_set_suspend(tmp_adev, 1);
#endif

		/* disable ras on ALL IPs */
		if (!need_emergency_restart &&
		      amdgpu_device_ip_need_full_reset(tmp_adev))
			amdgpu_ras_suspend(tmp_adev);

		for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
			struct amdgpu_ring *ring = tmp_adev->rings[i];

			if (!ring || !ring->sched.thread)
				continue;

			drm_sched_stop(&ring->sched, job ? &job->base : NULL);

			if (need_emergency_restart)
				amdgpu_job_stop_all_jobs_on_sched(&ring->sched);
		}
		atomic_inc(&tmp_adev->gpu_reset_counter);
	}

	if (need_emergency_restart)
		goto skip_sched_resume;

	/*
	 * Must check guilty signal here since after this point all old
	 * HW fences are force signaled.
	 *
	 * job->base holds a reference to parent fence
	 */
	if (job && job->base.s_fence->parent &&
	    dma_fence_is_signaled(job->base.s_fence->parent)) {
		job_signaled = true;
		dev_info(adev->dev, "Guilty job already signaled, skipping HW reset");
		goto skip_hw_reset;
	}

retry:	/* Rest of adevs pre asic reset from XGMI hive. */
	list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
		r = amdgpu_device_pre_asic_reset(tmp_adev, &reset_context);
		/*TODO Should we stop ?*/
		if (r) {
			dev_err(tmp_adev->dev, "GPU pre asic reset failed with err, %d for drm dev, %s ",
				  r, adev_to_drm(tmp_adev)->unique);
			tmp_adev->asic_reset_res = r;
		}
	}

	/*
	 * Pre reset functions called before lock, which make sure other threads
	 * who own reset lock exit successfully. No other thread runs in the driver
	 * while the recovery thread runs
	 */
	if (!locked) {
		amdgpu_device_lock_hive_adev(adev, hive);
		locked = true;
	}

	tmp_vram_lost_counter = atomic_read(&((adev)->vram_lost_counter));
	/* Actual ASIC resets if needed.*/
	/* Host driver will handle XGMI hive reset for SRIOV */
	if (amdgpu_sriov_vf(adev)) {
		r = amdgpu_device_reset_sriov(adev, job ? false : true);
		if (r)
			adev->asic_reset_res = r;
	} else {
		r = amdgpu_do_asic_reset(device_list_handle, &reset_context);
		if (r && r == -EAGAIN)
			goto retry;
	}

skip_hw_reset:

	/* Post ASIC reset for all devs .*/
	list_for_each_entry(tmp_adev, device_list_handle, reset_list) {

		/*
		 * Sometimes a later bad compute job can block a good gfx job as gfx
		 * and compute ring share internal GC HW mutually. We add an additional
		 * guilty jobs recheck step to find the real guilty job, it synchronously
		 * submits and pends for the first job being signaled. If it gets timeout,
		 * we identify it as a real guilty job.
		 */
		if (amdgpu_gpu_recovery == 2 &&
			!(tmp_vram_lost_counter < atomic_read(&adev->vram_lost_counter)))
			amdgpu_device_recheck_guilty_jobs(
				tmp_adev, device_list_handle, &reset_context);

		for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
			struct amdgpu_ring *ring = tmp_adev->rings[i];

			if (!ring || !ring->sched.thread)
				continue;

			/* No point to resubmit jobs if we didn't HW reset*/
			if (!tmp_adev->asic_reset_res && !job_signaled)
				drm_sched_resubmit_jobs(&ring->sched);

			drm_sched_start(&ring->sched, !tmp_adev->asic_reset_res);
		}

		if (!drm_drv_uses_atomic_modeset(adev_to_drm(tmp_adev)) && !job_signaled) {
			drm_helper_resume_force_mode(adev_to_drm(tmp_adev));
		}

		if (tmp_adev->asic_reset_res)
			r = tmp_adev->asic_reset_res;

		tmp_adev->asic_reset_res = 0;

		if (r) {
			/* bad news, how to tell it to userspace ? */
			dev_info(tmp_adev->dev, "GPU reset(%d) failed\n", atomic_read(&tmp_adev->gpu_reset_counter));
			amdgpu_vf_error_put(tmp_adev, AMDGIM_ERROR_VF_GPU_RESET_FAIL, 0, r);
		} else {
			dev_info(tmp_adev->dev, "GPU reset(%d) succeeded!\n", atomic_read(&tmp_adev->gpu_reset_counter));
			if (amdgpu_acpi_smart_shift_update(adev_to_drm(tmp_adev), AMDGPU_SS_DEV_D0))
				DRM_WARN("smart shift update failed\n");
		}
	}

skip_sched_resume:
	list_for_each_entry(tmp_adev, device_list_handle, reset_list) {
		/* unlock kfd: SRIOV would do it separately */
		if (!need_emergency_restart && !amdgpu_sriov_vf(tmp_adev))
			amdgpu_amdkfd_post_reset(tmp_adev);

		/* kfd_post_reset will do nothing if kfd device is not initialized,
		 * need to bring up kfd here if it's not be initialized before
		 */
		if (!adev->kfd.init_complete)
			amdgpu_amdkfd_device_init(adev);

		if (audio_suspended)
			amdgpu_device_resume_display_audio(tmp_adev);
		amdgpu_device_recovery_exit(tmp_adev);
		if (locked)
			amdgpu_device_unlock_adev(tmp_adev);
	}

skip_recovery:
	if (hive) {
		atomic_set(&hive->in_reset, 0);
		mutex_unlock(&hive->hive_lock);
		amdgpu_put_xgmi_hive(hive);
	}

	if (r && r != -EAGAIN)
		dev_info(adev->dev, "GPU reset end with ret = %d\n", r);
	return r;
}

/**
 * amdgpu_device_get_pcie_info - fence pcie info about the PCIE slot
 *
 * @adev: amdgpu_device pointer
 *
 * Fetchs and stores in the driver the PCIE capabilities (gen speed
 * and lanes) of the slot the device is in. Handles APUs and
 * virtualized environments where PCIE config space may not be available.
 */
static void amdgpu_device_get_pcie_info(struct amdgpu_device *adev)
{
	struct pci_dev *pdev;
	enum pci_bus_speed speed_cap, platform_speed_cap;
	enum pcie_link_width platform_link_width;

	if (amdgpu_pcie_gen_cap)
		adev->pm.pcie_gen_mask = amdgpu_pcie_gen_cap;

	if (amdgpu_pcie_lane_cap)
		adev->pm.pcie_mlw_mask = amdgpu_pcie_lane_cap;

	/* covers APUs as well */
	if (pci_is_root_bus(adev->pdev->bus)) {
		if (adev->pm.pcie_gen_mask == 0)
			adev->pm.pcie_gen_mask = AMDGPU_DEFAULT_PCIE_GEN_MASK;
		if (adev->pm.pcie_mlw_mask == 0)
			adev->pm.pcie_mlw_mask = AMDGPU_DEFAULT_PCIE_MLW_MASK;
		return;
	}

	if (adev->pm.pcie_gen_mask && adev->pm.pcie_mlw_mask)
		return;

	pcie_bandwidth_available(adev->pdev, NULL,
				 &platform_speed_cap, &platform_link_width);

	if (adev->pm.pcie_gen_mask == 0) {
		/* asic caps */
		pdev = adev->pdev;
		speed_cap = pcie_get_speed_cap(pdev);
		if (speed_cap == PCI_SPEED_UNKNOWN) {
			adev->pm.pcie_gen_mask |= (CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN1 |
						  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN2 |
						  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN3);
		} else {
			if (speed_cap == PCIE_SPEED_32_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN2 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN3 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN4 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN5);
			else if (speed_cap == PCIE_SPEED_16_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN2 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN3 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN4);
			else if (speed_cap == PCIE_SPEED_8_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN2 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN3);
			else if (speed_cap == PCIE_SPEED_5_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							  CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN2);
			else
				adev->pm.pcie_gen_mask |= CAIL_ASIC_PCIE_LINK_SPEED_SUPPORT_GEN1;
		}
		/* platform caps */
		if (platform_speed_cap == PCI_SPEED_UNKNOWN) {
			adev->pm.pcie_gen_mask |= (CAIL_PCIE_LINK_SPEED_SUPPORT_GEN1 |
						   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN2);
		} else {
			if (platform_speed_cap == PCIE_SPEED_32_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN2 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN3 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN4 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN5);
			else if (platform_speed_cap == PCIE_SPEED_16_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN2 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN3 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN4);
			else if (platform_speed_cap == PCIE_SPEED_8_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN2 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN3);
			else if (platform_speed_cap == PCIE_SPEED_5_0GT)
				adev->pm.pcie_gen_mask |= (CAIL_PCIE_LINK_SPEED_SUPPORT_GEN1 |
							   CAIL_PCIE_LINK_SPEED_SUPPORT_GEN2);
			else
				adev->pm.pcie_gen_mask |= CAIL_PCIE_LINK_SPEED_SUPPORT_GEN1;

		}
	}
	if (adev->pm.pcie_mlw_mask == 0) {
		if (platform_link_width == PCIE_LNK_WIDTH_UNKNOWN) {
			adev->pm.pcie_mlw_mask |= AMDGPU_DEFAULT_PCIE_MLW_MASK;
		} else {
			switch (platform_link_width) {
			case PCIE_LNK_X32:
				adev->pm.pcie_mlw_mask = (CAIL_PCIE_LINK_WIDTH_SUPPORT_X32 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X16 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X12 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X8 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X4 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X2 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X1);
				break;
			case PCIE_LNK_X16:
				adev->pm.pcie_mlw_mask = (CAIL_PCIE_LINK_WIDTH_SUPPORT_X16 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X12 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X8 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X4 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X2 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X1);
				break;
			case PCIE_LNK_X12:
				adev->pm.pcie_mlw_mask = (CAIL_PCIE_LINK_WIDTH_SUPPORT_X12 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X8 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X4 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X2 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X1);
				break;
			case PCIE_LNK_X8:
				adev->pm.pcie_mlw_mask = (CAIL_PCIE_LINK_WIDTH_SUPPORT_X8 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X4 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X2 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X1);
				break;
			case PCIE_LNK_X4:
				adev->pm.pcie_mlw_mask = (CAIL_PCIE_LINK_WIDTH_SUPPORT_X4 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X2 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X1);
				break;
			case PCIE_LNK_X2:
				adev->pm.pcie_mlw_mask = (CAIL_PCIE_LINK_WIDTH_SUPPORT_X2 |
							  CAIL_PCIE_LINK_WIDTH_SUPPORT_X1);
				break;
			case PCIE_LNK_X1:
				adev->pm.pcie_mlw_mask = CAIL_PCIE_LINK_WIDTH_SUPPORT_X1;
				break;
			default:
				break;
			}
		}
	}
}

/**
 * amdgpu_device_is_peer_accessible - Check peer access through PCIe BAR
 *
 * @adev: amdgpu_device pointer
 * @peer_adev: amdgpu_device pointer for peer device trying to access @adev
 *
 * Return true if @peer_adev can access (DMA) @adev through the PCIe
 * BAR, i.e. @adev is "large BAR" and the BAR matches the DMA mask of
 * @peer_adev.
 */
bool amdgpu_device_is_peer_accessible(struct amdgpu_device *adev,
				      struct amdgpu_device *peer_adev)
{
	uint64_t address_mask = peer_adev->dev->dma_mask ?
		~*peer_adev->dev->dma_mask : ~((1ULL << 32) - 1);
	resource_size_t aper_limit =
		adev->gmc.aper_base + adev->gmc.aper_size - 1;

	return pcie_p2p && (adev->gmc.visible_vram_size &&
		adev->gmc.real_vram_size == adev->gmc.visible_vram_size &&
		!(adev->gmc.aper_base & address_mask ||
		  aper_limit & address_mask));
}

int amdgpu_device_baco_enter(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct amdgpu_ras *ras = amdgpu_ras_get_context(adev);

	if (!amdgpu_device_supports_baco(adev_to_drm(adev)))
		return -ENOTSUPP;

	if (ras && adev->ras_enabled &&
	    adev->nbio.funcs->enable_doorbell_interrupt)
		adev->nbio.funcs->enable_doorbell_interrupt(adev, false);

	return amdgpu_dpm_baco_enter(adev);
}

int amdgpu_device_baco_exit(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct amdgpu_ras *ras = amdgpu_ras_get_context(adev);
	int ret = 0;

	if (!amdgpu_device_supports_baco(adev_to_drm(adev)))
		return -ENOTSUPP;

	ret = amdgpu_dpm_baco_exit(adev);
	if (ret)
		return ret;

	if (ras && adev->ras_enabled &&
	    adev->nbio.funcs->enable_doorbell_interrupt)
		adev->nbio.funcs->enable_doorbell_interrupt(adev, true);

	if (amdgpu_passthrough(adev) &&
	    adev->nbio.funcs->clear_doorbell_interrupt)
		adev->nbio.funcs->clear_doorbell_interrupt(adev);

	return 0;
}

static void amdgpu_cancel_all_tdr(struct amdgpu_device *adev)
{
	int i;

	for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
		struct amdgpu_ring *ring = adev->rings[i];

		if (!ring || !ring->sched.thread)
			continue;

		cancel_delayed_work_sync(&ring->sched.work_tdr);
	}
}

/**
 * amdgpu_pci_error_detected - Called when a PCI error is detected.
 * @pdev: PCI device struct
 * @state: PCI channel state
 *
 * Description: Called when a PCI error is detected.
 *
 * Return: PCI_ERS_RESULT_NEED_RESET or PCI_ERS_RESULT_DISCONNECT.
 */
pci_ers_result_t amdgpu_pci_error_detected(struct pci_dev *pdev, pci_channel_state_t state)
{
	struct drm_device *dev = pci_get_drvdata(pdev);
	struct amdgpu_device *adev = drm_to_adev(dev);
	int i;

	DRM_INFO("PCI error: detected callback, state(%d)!!\n", state);

	if (adev->gmc.xgmi.num_physical_nodes > 1) {
		DRM_WARN("No support for XGMI hive yet...");
		return PCI_ERS_RESULT_DISCONNECT;
	}

	adev->pci_channel_state = state;

	switch (state) {
	case pci_channel_io_normal:
		return PCI_ERS_RESULT_CAN_RECOVER;
	/* Fatal error, prepare for slot reset */
	case pci_channel_io_frozen:
		/*
		 * Cancel and wait for all TDRs in progress if failing to
		 * set  adev->in_gpu_reset in amdgpu_device_lock_adev
		 *
		 * Locking adev->reset_sem will prevent any external access
		 * to GPU during PCI error recovery
		 */
		while (!amdgpu_device_recovery_enter(adev))
			amdgpu_cancel_all_tdr(adev);

		amdgpu_device_lock_adev(adev, NULL);
		/*
		 * Block any work scheduling as we do for regular GPU reset
		 * for the duration of the recovery
		 */
		for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
			struct amdgpu_ring *ring = adev->rings[i];

			if (!ring || !ring->sched.thread)
				continue;

			drm_sched_stop(&ring->sched, NULL);
		}
		atomic_inc(&adev->gpu_reset_counter);
		return PCI_ERS_RESULT_NEED_RESET;
	case pci_channel_io_perm_failure:
		/* Permanent error, prepare for device removal */
		return PCI_ERS_RESULT_DISCONNECT;
	}

	return PCI_ERS_RESULT_NEED_RESET;
}

/**
 * amdgpu_pci_mmio_enabled - Enable MMIO and dump debug registers
 * @pdev: pointer to PCI device
 */
pci_ers_result_t amdgpu_pci_mmio_enabled(struct pci_dev *pdev)
{

	DRM_INFO("PCI error: mmio enabled callback!!\n");

	/* TODO - dump whatever for debugging purposes */

	/* This called only if amdgpu_pci_error_detected returns
	 * PCI_ERS_RESULT_CAN_RECOVER. Read/write to the device still
	 * works, no need to reset slot.
	 */

	return PCI_ERS_RESULT_RECOVERED;
}

/**
 * amdgpu_pci_slot_reset - Called when PCI slot has been reset.
 * @pdev: PCI device struct
 *
 * Description: This routine is called by the pci error recovery
 * code after the PCI slot has been reset, just before we
 * should resume normal operations.
 */
pci_ers_result_t amdgpu_pci_slot_reset(struct pci_dev *pdev)
{
	struct drm_device *dev = pci_get_drvdata(pdev);
	struct amdgpu_device *adev = drm_to_adev(dev);
	int r, i;
	struct amdgpu_reset_context reset_context;
	u32 memsize;
	struct list_head device_list;

	DRM_INFO("PCI error: slot reset callback!!\n");

	memset(&reset_context, 0, sizeof(reset_context));

	INIT_LIST_HEAD(&device_list);
	list_add_tail(&adev->reset_list, &device_list);

	/* wait for asic to come out of reset */
	msleep(500);

	/* Restore PCI confspace */
	amdgpu_device_load_pci_state(pdev);

	/* confirm  ASIC came out of reset */
	for (i = 0; i < adev->usec_timeout; i++) {
		memsize = amdgpu_asic_get_config_memsize(adev);

		if (memsize != 0xffffffff)
			break;
		udelay(1);
	}
	if (memsize == 0xffffffff) {
		r = -ETIME;
		goto out;
	}

	reset_context.method = AMD_RESET_METHOD_NONE;
	reset_context.reset_req_dev = adev;
	set_bit(AMDGPU_NEED_FULL_RESET, &reset_context.flags);
	set_bit(AMDGPU_SKIP_HW_RESET, &reset_context.flags);

	adev->no_hw_access = true;
	r = amdgpu_device_pre_asic_reset(adev, &reset_context);
	adev->no_hw_access = false;
	if (r)
		goto out;

	r = amdgpu_do_asic_reset(&device_list, &reset_context);

out:
	if (!r) {
		if (amdgpu_device_cache_pci_state(adev->pdev))
			pci_restore_state(adev->pdev);

		DRM_INFO("PCIe error recovery succeeded\n");
	} else {
		DRM_ERROR("PCIe error recovery failed, err:%d", r);
		amdgpu_device_unlock_adev(adev);
	}

	return r ? PCI_ERS_RESULT_DISCONNECT : PCI_ERS_RESULT_RECOVERED;
}

/**
 * amdgpu_pci_resume() - resume normal ops after PCI reset
 * @pdev: pointer to PCI device
 *
 * Called when the error recovery driver tells us that its
 * OK to resume normal operation.
 */
void amdgpu_pci_resume(struct pci_dev *pdev)
{
	struct drm_device *dev = pci_get_drvdata(pdev);
	struct amdgpu_device *adev = drm_to_adev(dev);
	int i;


	DRM_INFO("PCI error: resume callback!!\n");

	/* Only continue execution for the case of pci_channel_io_frozen */
	if (adev->pci_channel_state != pci_channel_io_frozen)
		return;

	for (i = 0; i < AMDGPU_MAX_RINGS; ++i) {
		struct amdgpu_ring *ring = adev->rings[i];

		if (!ring || !ring->sched.thread)
			continue;


		drm_sched_resubmit_jobs(&ring->sched);
		drm_sched_start(&ring->sched, true);
	}

	amdgpu_device_unlock_adev(adev);
}

bool amdgpu_device_cache_pci_state(struct pci_dev *pdev)
{
	struct drm_device *dev = pci_get_drvdata(pdev);
	struct amdgpu_device *adev = drm_to_adev(dev);
	int r;

	r = pci_save_state(pdev);
	if (!r) {
		kfree(adev->pci_state);

		adev->pci_state = pci_store_saved_state(pdev);

		if (!adev->pci_state) {
			DRM_ERROR("Failed to store PCI saved state");
			return false;
		}
	} else {
		DRM_WARN("Failed to save PCI state, err:%d\n", r);
		return false;
	}

	return true;
}

bool amdgpu_device_load_pci_state(struct pci_dev *pdev)
{
	struct drm_device *dev = pci_get_drvdata(pdev);
	struct amdgpu_device *adev = drm_to_adev(dev);
	int r;

	if (!adev->pci_state)
		return false;

	r = pci_load_saved_state(pdev, adev->pci_state);

	if (!r) {
		pci_restore_state(pdev);
	} else {
		DRM_WARN("Failed to load PCI state, err:%d\n", r);
		return false;
	}

	return true;
}

void amdgpu_device_flush_hdp(struct amdgpu_device *adev,
		struct amdgpu_ring *ring)
{
#ifdef CONFIG_X86_64
	if (adev->flags & AMD_IS_APU)
		return;
#endif
	if (adev->gmc.xgmi.connected_to_cpu)
		return;

	if (ring && ring->funcs->emit_hdp_flush)
		amdgpu_ring_emit_hdp_flush(ring);
	else
		amdgpu_asic_flush_hdp(adev, ring);
}

void amdgpu_device_invalidate_hdp(struct amdgpu_device *adev,
		struct amdgpu_ring *ring)
{
#ifdef CONFIG_X86_64
	if (adev->flags & AMD_IS_APU)
		return;
#endif
	if (adev->gmc.xgmi.connected_to_cpu)
		return;

	amdgpu_asic_invalidate_hdp(adev, ring);
}

/**
 * amdgpu_device_halt() - bring hardware to some kind of halt state
 *
 * @adev: amdgpu_device pointer
 *
 * Bring hardware to some kind of halt state so that no one can touch it
 * any more. It will help to maintain error context when error occurred.
 * Compare to a simple hang, the system will keep stable at least for SSH
 * access. Then it should be trivial to inspect the hardware state and
 * see what's going on. Implemented as following:
 *
 * 1. drm_dev_unplug() makes device inaccessible to user space(IOCTLs, etc),
 *    clears all CPU mappings to device, disallows remappings through page faults
 * 2. amdgpu_irq_disable_all() disables all interrupts
 * 3. amdgpu_fence_driver_hw_fini() signals all HW fences
 * 4. set adev->no_hw_access to avoid potential crashes after setp 5
 * 5. amdgpu_device_unmap_mmio() clears all MMIO mappings
 * 6. pci_disable_device() and pci_wait_for_pending_transaction()
 *    flush any in flight DMA operations
 */
void amdgpu_device_halt(struct amdgpu_device *adev)
{
	struct pci_dev *pdev = adev->pdev;
	struct drm_device *ddev = adev_to_drm(adev);

#ifdef HAVE_DRM_DEV_UNPLUG
	drm_dev_unplug(ddev);
#else
        drm_dev_unregister(ddev);
#endif

	amdgpu_irq_disable_all(adev);

	amdgpu_fence_driver_hw_fini(adev);

	adev->no_hw_access = true;

	amdgpu_device_unmap_mmio(adev);

	pci_disable_device(pdev);
	pci_wait_for_pending_transaction(pdev);
}

u32 amdgpu_device_pcie_port_rreg(struct amdgpu_device *adev,
				u32 reg)
{
	unsigned long flags, address, data;
	u32 r;

	address = adev->nbio.funcs->get_pcie_port_index_offset(adev);
	data = adev->nbio.funcs->get_pcie_port_data_offset(adev);

	spin_lock_irqsave(&adev->pcie_idx_lock, flags);
	WREG32(address, reg * 4);
	(void)RREG32(address);
	r = RREG32(data);
	spin_unlock_irqrestore(&adev->pcie_idx_lock, flags);
	return r;
}

void amdgpu_device_pcie_port_wreg(struct amdgpu_device *adev,
				u32 reg, u32 v)
{
	unsigned long flags, address, data;

	address = adev->nbio.funcs->get_pcie_port_index_offset(adev);
	data = adev->nbio.funcs->get_pcie_port_data_offset(adev);

	spin_lock_irqsave(&adev->pcie_idx_lock, flags);
	WREG32(address, reg * 4);
	(void)RREG32(address);
	WREG32(data, v);
	(void)RREG32(data);
	spin_unlock_irqrestore(&adev->pcie_idx_lock, flags);
}

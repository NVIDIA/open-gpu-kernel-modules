// SPDX-License-Identifier: GPL-2.0

/*
 * Copyright 2016-2019 HabanaLabs, Ltd.
 * All Rights Reserved.
 */

#include "habanalabs.h"
#include "../include/hw_ip/mmu/mmu_general.h"

#include <linux/pci.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#define MMU_ADDR_BUF_SIZE	40
#define MMU_ASID_BUF_SIZE	10
#define MMU_KBUF_SIZE		(MMU_ADDR_BUF_SIZE + MMU_ASID_BUF_SIZE)

static struct dentry *hl_debug_root;

static int hl_debugfs_i2c_read(struct hl_device *hdev, u8 i2c_bus, u8 i2c_addr,
				u8 i2c_reg, long *val)
{
	struct cpucp_packet pkt;
	u64 result;
	int rc;

	if (!hl_device_operational(hdev, NULL))
		return -EBUSY;

	memset(&pkt, 0, sizeof(pkt));

	pkt.ctl = cpu_to_le32(CPUCP_PACKET_I2C_RD <<
				CPUCP_PKT_CTL_OPCODE_SHIFT);
	pkt.i2c_bus = i2c_bus;
	pkt.i2c_addr = i2c_addr;
	pkt.i2c_reg = i2c_reg;

	rc = hdev->asic_funcs->send_cpu_message(hdev, (u32 *) &pkt, sizeof(pkt),
						0, &result);

	*val = (long) result;

	if (rc)
		dev_err(hdev->dev, "Failed to read from I2C, error %d\n", rc);

	return rc;
}

static int hl_debugfs_i2c_write(struct hl_device *hdev, u8 i2c_bus, u8 i2c_addr,
				u8 i2c_reg, u32 val)
{
	struct cpucp_packet pkt;
	int rc;

	if (!hl_device_operational(hdev, NULL))
		return -EBUSY;

	memset(&pkt, 0, sizeof(pkt));

	pkt.ctl = cpu_to_le32(CPUCP_PACKET_I2C_WR <<
				CPUCP_PKT_CTL_OPCODE_SHIFT);
	pkt.i2c_bus = i2c_bus;
	pkt.i2c_addr = i2c_addr;
	pkt.i2c_reg = i2c_reg;
	pkt.value = cpu_to_le64(val);

	rc = hdev->asic_funcs->send_cpu_message(hdev, (u32 *) &pkt, sizeof(pkt),
						0, NULL);

	if (rc)
		dev_err(hdev->dev, "Failed to write to I2C, error %d\n", rc);

	return rc;
}

static void hl_debugfs_led_set(struct hl_device *hdev, u8 led, u8 state)
{
	struct cpucp_packet pkt;
	int rc;

	if (!hl_device_operational(hdev, NULL))
		return;

	memset(&pkt, 0, sizeof(pkt));

	pkt.ctl = cpu_to_le32(CPUCP_PACKET_LED_SET <<
				CPUCP_PKT_CTL_OPCODE_SHIFT);
	pkt.led_index = cpu_to_le32(led);
	pkt.value = cpu_to_le64(state);

	rc = hdev->asic_funcs->send_cpu_message(hdev, (u32 *) &pkt, sizeof(pkt),
						0, NULL);

	if (rc)
		dev_err(hdev->dev, "Failed to set LED %d, error %d\n", led, rc);
}

static int command_buffers_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_cb *cb;
	bool first = true;

	spin_lock(&dev_entry->cb_spinlock);

	list_for_each_entry(cb, &dev_entry->cb_list, debugfs_list) {
		if (first) {
			first = false;
			seq_puts(s, "\n");
			seq_puts(s, " CB ID   CTX ID   CB size    CB RefCnt    mmap?   CS counter\n");
			seq_puts(s, "---------------------------------------------------------------\n");
		}
		seq_printf(s,
			"   %03llu        %d    0x%08x      %d          %d          %d\n",
			cb->id, cb->ctx->asid, cb->size,
			kref_read(&cb->refcount),
			cb->mmap, atomic_read(&cb->cs_cnt));
	}

	spin_unlock(&dev_entry->cb_spinlock);

	if (!first)
		seq_puts(s, "\n");

	return 0;
}

static int command_submission_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_cs *cs;
	bool first = true;

	spin_lock(&dev_entry->cs_spinlock);

	list_for_each_entry(cs, &dev_entry->cs_list, debugfs_list) {
		if (first) {
			first = false;
			seq_puts(s, "\n");
			seq_puts(s, " CS ID   CTX ASID   CS RefCnt   Submitted    Completed\n");
			seq_puts(s, "------------------------------------------------------\n");
		}
		seq_printf(s,
			"   %llu       %d          %d           %d            %d\n",
			cs->sequence, cs->ctx->asid,
			kref_read(&cs->refcount),
			cs->submitted, cs->completed);
	}

	spin_unlock(&dev_entry->cs_spinlock);

	if (!first)
		seq_puts(s, "\n");

	return 0;
}

static int command_submission_jobs_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_cs_job *job;
	bool first = true;

	spin_lock(&dev_entry->cs_job_spinlock);

	list_for_each_entry(job, &dev_entry->cs_job_list, debugfs_list) {
		if (first) {
			first = false;
			seq_puts(s, "\n");
			seq_puts(s, " JOB ID   CS ID    CTX ASID   JOB RefCnt   H/W Queue\n");
			seq_puts(s, "----------------------------------------------------\n");
		}
		if (job->cs)
			seq_printf(s,
				"   %02d      %llu        %d          %d           %d\n",
				job->id, job->cs->sequence, job->cs->ctx->asid,
				kref_read(&job->refcount), job->hw_queue_id);
		else
			seq_printf(s,
				"   %02d      0        %d          %d           %d\n",
				job->id, HL_KERNEL_ASID_ID,
				kref_read(&job->refcount), job->hw_queue_id);
	}

	spin_unlock(&dev_entry->cs_job_spinlock);

	if (!first)
		seq_puts(s, "\n");

	return 0;
}

static int userptr_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_userptr *userptr;
	char dma_dir[4][30] = {"DMA_BIDIRECTIONAL", "DMA_TO_DEVICE",
				"DMA_FROM_DEVICE", "DMA_NONE"};
	bool first = true;

	spin_lock(&dev_entry->userptr_spinlock);

	list_for_each_entry(userptr, &dev_entry->userptr_list, debugfs_list) {
		if (first) {
			first = false;
			seq_puts(s, "\n");
			seq_puts(s, " user virtual address     size             dma dir\n");
			seq_puts(s, "----------------------------------------------------------\n");
		}
		seq_printf(s,
			"    0x%-14llx      %-10u    %-30s\n",
			userptr->addr, userptr->size, dma_dir[userptr->dir]);
	}

	spin_unlock(&dev_entry->userptr_spinlock);

	if (!first)
		seq_puts(s, "\n");

	return 0;
}

static int vm_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_vm_hw_block_list_node *lnode;
	struct hl_ctx *ctx;
	struct hl_vm *vm;
	struct hl_vm_hash_node *hnode;
	struct hl_userptr *userptr;
	struct hl_vm_phys_pg_pack *phys_pg_pack = NULL;
	enum vm_type_t *vm_type;
	bool once = true;
	u64 j;
	int i;

	if (!dev_entry->hdev->mmu_enable)
		return 0;

	spin_lock(&dev_entry->ctx_mem_hash_spinlock);

	list_for_each_entry(ctx, &dev_entry->ctx_mem_hash_list, debugfs_list) {
		once = false;
		seq_puts(s, "\n\n----------------------------------------------------");
		seq_puts(s, "\n----------------------------------------------------\n\n");
		seq_printf(s, "ctx asid: %u\n", ctx->asid);

		seq_puts(s, "\nmappings:\n\n");
		seq_puts(s, "    virtual address        size          handle\n");
		seq_puts(s, "----------------------------------------------------\n");
		mutex_lock(&ctx->mem_hash_lock);
		hash_for_each(ctx->mem_hash, i, hnode, node) {
			vm_type = hnode->ptr;

			if (*vm_type == VM_TYPE_USERPTR) {
				userptr = hnode->ptr;
				seq_printf(s,
					"    0x%-14llx      %-10u\n",
					hnode->vaddr, userptr->size);
			} else {
				phys_pg_pack = hnode->ptr;
				seq_printf(s,
					"    0x%-14llx      %-10llu       %-4u\n",
					hnode->vaddr, phys_pg_pack->total_size,
					phys_pg_pack->handle);
			}
		}
		mutex_unlock(&ctx->mem_hash_lock);

		if (ctx->asid != HL_KERNEL_ASID_ID &&
		    !list_empty(&ctx->hw_block_mem_list)) {
			seq_puts(s, "\nhw_block mappings:\n\n");
			seq_puts(s, "    virtual address    size    HW block id\n");
			seq_puts(s, "-------------------------------------------\n");
			mutex_lock(&ctx->hw_block_list_lock);
			list_for_each_entry(lnode, &ctx->hw_block_mem_list,
					    node) {
				seq_printf(s,
					"    0x%-14lx   %-6u      %-9u\n",
					lnode->vaddr, lnode->size, lnode->id);
			}
			mutex_unlock(&ctx->hw_block_list_lock);
		}

		vm = &ctx->hdev->vm;
		spin_lock(&vm->idr_lock);

		if (!idr_is_empty(&vm->phys_pg_pack_handles))
			seq_puts(s, "\n\nallocations:\n");

		idr_for_each_entry(&vm->phys_pg_pack_handles, phys_pg_pack, i) {
			if (phys_pg_pack->asid != ctx->asid)
				continue;

			seq_printf(s, "\nhandle: %u\n", phys_pg_pack->handle);
			seq_printf(s, "page size: %u\n\n",
						phys_pg_pack->page_size);
			seq_puts(s, "   physical address\n");
			seq_puts(s, "---------------------\n");
			for (j = 0 ; j < phys_pg_pack->npages ; j++) {
				seq_printf(s, "    0x%-14llx\n",
						phys_pg_pack->pages[j]);
			}
		}
		spin_unlock(&vm->idr_lock);

	}

	spin_unlock(&dev_entry->ctx_mem_hash_spinlock);

	if (!once)
		seq_puts(s, "\n");

	return 0;
}

static int mmu_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_device *hdev = dev_entry->hdev;
	struct hl_ctx *ctx;
	struct hl_mmu_hop_info hops_info = {0};
	u64 virt_addr = dev_entry->mmu_addr, phys_addr;
	int i;

	if (!hdev->mmu_enable)
		return 0;

	if (dev_entry->mmu_asid == HL_KERNEL_ASID_ID)
		ctx = hdev->kernel_ctx;
	else
		ctx = hdev->compute_ctx;

	if (!ctx) {
		dev_err(hdev->dev, "no ctx available\n");
		return 0;
	}

	if (hl_mmu_get_tlb_info(ctx, virt_addr, &hops_info)) {
		dev_err(hdev->dev, "virt addr 0x%llx is not mapped to phys addr\n",
				virt_addr);
		return 0;
	}

	phys_addr = hops_info.hop_info[hops_info.used_hops - 1].hop_pte_val;

	if (hops_info.scrambled_vaddr &&
		(dev_entry->mmu_addr != hops_info.scrambled_vaddr))
		seq_printf(s,
			"asid: %u, virt_addr: 0x%llx, scrambled virt_addr: 0x%llx,\nphys_addr: 0x%llx, scrambled_phys_addr: 0x%llx\n",
			dev_entry->mmu_asid, dev_entry->mmu_addr,
			hops_info.scrambled_vaddr,
			hops_info.unscrambled_paddr, phys_addr);
	else
		seq_printf(s,
			"asid: %u, virt_addr: 0x%llx, phys_addr: 0x%llx\n",
			dev_entry->mmu_asid, dev_entry->mmu_addr, phys_addr);

	for (i = 0 ; i < hops_info.used_hops ; i++) {
		seq_printf(s, "hop%d_addr: 0x%llx\n",
				i, hops_info.hop_info[i].hop_addr);
		seq_printf(s, "hop%d_pte_addr: 0x%llx\n",
				i, hops_info.hop_info[i].hop_pte_addr);
		seq_printf(s, "hop%d_pte: 0x%llx\n",
				i, hops_info.hop_info[i].hop_pte_val);
	}

	return 0;
}

static ssize_t mmu_asid_va_write(struct file *file, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	struct seq_file *s = file->private_data;
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_device *hdev = dev_entry->hdev;
	char kbuf[MMU_KBUF_SIZE];
	char *c;
	ssize_t rc;

	if (!hdev->mmu_enable)
		return count;

	if (count > sizeof(kbuf) - 1)
		goto err;
	if (copy_from_user(kbuf, buf, count))
		goto err;
	kbuf[count] = 0;

	c = strchr(kbuf, ' ');
	if (!c)
		goto err;
	*c = '\0';

	rc = kstrtouint(kbuf, 10, &dev_entry->mmu_asid);
	if (rc)
		goto err;

	if (strncmp(c+1, "0x", 2))
		goto err;
	rc = kstrtoull(c+3, 16, &dev_entry->mmu_addr);
	if (rc)
		goto err;

	return count;

err:
	dev_err(hdev->dev, "usage: echo <asid> <0xaddr> > mmu\n");

	return -EINVAL;
}

static int engines_show(struct seq_file *s, void *data)
{
	struct hl_debugfs_entry *entry = s->private;
	struct hl_dbg_device_entry *dev_entry = entry->dev_entry;
	struct hl_device *hdev = dev_entry->hdev;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev,
				"Can't check device idle during reset\n");
		return 0;
	}

	hdev->asic_funcs->is_device_idle(hdev, NULL, 0, s);

	return 0;
}

static bool hl_is_device_va(struct hl_device *hdev, u64 addr)
{
	struct asic_fixed_properties *prop = &hdev->asic_prop;

	if (!hdev->mmu_enable)
		goto out;

	if (prop->dram_supports_virtual_memory &&
		(addr >= prop->dmmu.start_addr && addr < prop->dmmu.end_addr))
		return true;

	if (addr >= prop->pmmu.start_addr &&
		addr < prop->pmmu.end_addr)
		return true;

	if (addr >= prop->pmmu_huge.start_addr &&
		addr < prop->pmmu_huge.end_addr)
		return true;
out:
	return false;
}

static bool hl_is_device_internal_memory_va(struct hl_device *hdev, u64 addr,
						u32 size)
{
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	u64 dram_start_addr, dram_end_addr;

	if (!hdev->mmu_enable)
		return false;

	if (prop->dram_supports_virtual_memory) {
		dram_start_addr = prop->dmmu.start_addr;
		dram_end_addr = prop->dmmu.end_addr;
	} else {
		dram_start_addr = prop->dram_base_address;
		dram_end_addr = prop->dram_end_address;
	}

	if (hl_mem_area_inside_range(addr, size, dram_start_addr,
					dram_end_addr))
		return true;

	if (hl_mem_area_inside_range(addr, size, prop->sram_base_address,
					prop->sram_end_address))
		return true;

	return false;
}

static int device_va_to_pa(struct hl_device *hdev, u64 virt_addr, u32 size,
			u64 *phys_addr)
{
	struct hl_vm_phys_pg_pack *phys_pg_pack;
	struct hl_ctx *ctx = hdev->compute_ctx;
	struct hl_vm_hash_node *hnode;
	struct hl_userptr *userptr;
	enum vm_type_t *vm_type;
	bool valid = false;
	u64 end_address;
	u32 range_size;
	int i, rc = 0;

	if (!ctx) {
		dev_err(hdev->dev, "no ctx available\n");
		return -EINVAL;
	}

	/* Verify address is mapped */
	mutex_lock(&ctx->mem_hash_lock);
	hash_for_each(ctx->mem_hash, i, hnode, node) {
		vm_type = hnode->ptr;

		if (*vm_type == VM_TYPE_USERPTR) {
			userptr = hnode->ptr;
			range_size = userptr->size;
		} else {
			phys_pg_pack = hnode->ptr;
			range_size = phys_pg_pack->total_size;
		}

		end_address = virt_addr + size;
		if ((virt_addr >= hnode->vaddr) &&
				(end_address <= hnode->vaddr + range_size)) {
			valid = true;
			break;
		}
	}
	mutex_unlock(&ctx->mem_hash_lock);

	if (!valid) {
		dev_err(hdev->dev,
			"virt addr 0x%llx is not mapped\n",
			virt_addr);
		return -EINVAL;
	}

	rc = hl_mmu_va_to_pa(ctx, virt_addr, phys_addr);
	if (rc) {
		dev_err(hdev->dev,
			"virt addr 0x%llx is not mapped to phys addr\n",
			virt_addr);
		rc = -EINVAL;
	}

	return rc;
}

static ssize_t hl_data_read32(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u64 addr = entry->addr;
	bool user_address;
	char tmp_buf[32];
	ssize_t rc;
	u32 val;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev, "Can't read during reset\n");
		return 0;
	}

	if (*ppos)
		return 0;

	user_address = hl_is_device_va(hdev, addr);
	if (user_address) {
		rc = device_va_to_pa(hdev, addr, sizeof(val), &addr);
		if (rc)
			return rc;
	}

	rc = hdev->asic_funcs->debugfs_read32(hdev, addr, user_address, &val);
	if (rc) {
		dev_err(hdev->dev, "Failed to read from 0x%010llx\n", addr);
		return rc;
	}

	sprintf(tmp_buf, "0x%08x\n", val);
	return simple_read_from_buffer(buf, count, ppos, tmp_buf,
			strlen(tmp_buf));
}

static ssize_t hl_data_write32(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u64 addr = entry->addr;
	bool user_address;
	u32 value;
	ssize_t rc;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev, "Can't write during reset\n");
		return 0;
	}

	rc = kstrtouint_from_user(buf, count, 16, &value);
	if (rc)
		return rc;

	user_address = hl_is_device_va(hdev, addr);
	if (user_address) {
		rc = device_va_to_pa(hdev, addr, sizeof(value), &addr);
		if (rc)
			return rc;
	}

	rc = hdev->asic_funcs->debugfs_write32(hdev, addr, user_address, value);
	if (rc) {
		dev_err(hdev->dev, "Failed to write 0x%08x to 0x%010llx\n",
			value, addr);
		return rc;
	}

	return count;
}

static ssize_t hl_data_read64(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u64 addr = entry->addr;
	bool user_address;
	char tmp_buf[32];
	ssize_t rc;
	u64 val;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev, "Can't read during reset\n");
		return 0;
	}

	if (*ppos)
		return 0;

	user_address = hl_is_device_va(hdev, addr);
	if (user_address) {
		rc = device_va_to_pa(hdev, addr, sizeof(val), &addr);
		if (rc)
			return rc;
	}

	rc = hdev->asic_funcs->debugfs_read64(hdev, addr, user_address, &val);
	if (rc) {
		dev_err(hdev->dev, "Failed to read from 0x%010llx\n", addr);
		return rc;
	}

	sprintf(tmp_buf, "0x%016llx\n", val);
	return simple_read_from_buffer(buf, count, ppos, tmp_buf,
			strlen(tmp_buf));
}

static ssize_t hl_data_write64(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u64 addr = entry->addr;
	bool user_address;
	u64 value;
	ssize_t rc;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev, "Can't write during reset\n");
		return 0;
	}

	rc = kstrtoull_from_user(buf, count, 16, &value);
	if (rc)
		return rc;

	user_address = hl_is_device_va(hdev, addr);
	if (user_address) {
		rc = device_va_to_pa(hdev, addr, sizeof(value), &addr);
		if (rc)
			return rc;
	}

	rc = hdev->asic_funcs->debugfs_write64(hdev, addr, user_address, value);
	if (rc) {
		dev_err(hdev->dev, "Failed to write 0x%016llx to 0x%010llx\n",
			value, addr);
		return rc;
	}

	return count;
}

static ssize_t hl_dma_size_write(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u64 addr = entry->addr;
	ssize_t rc;
	u32 size;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev, "Can't DMA during reset\n");
		return 0;
	}
	rc = kstrtouint_from_user(buf, count, 16, &size);
	if (rc)
		return rc;

	if (!size) {
		dev_err(hdev->dev, "DMA read failed. size can't be 0\n");
		return -EINVAL;
	}

	if (size > SZ_128M) {
		dev_err(hdev->dev,
			"DMA read failed. size can't be larger than 128MB\n");
		return -EINVAL;
	}

	if (!hl_is_device_internal_memory_va(hdev, addr, size)) {
		dev_err(hdev->dev,
			"DMA read failed. Invalid 0x%010llx + 0x%08x\n",
			addr, size);
		return -EINVAL;
	}

	/* Free the previous allocation, if there was any */
	entry->blob_desc.size = 0;
	vfree(entry->blob_desc.data);

	entry->blob_desc.data = vmalloc(size);
	if (!entry->blob_desc.data)
		return -ENOMEM;

	rc = hdev->asic_funcs->debugfs_read_dma(hdev, addr, size,
						entry->blob_desc.data);
	if (rc) {
		dev_err(hdev->dev, "Failed to DMA from 0x%010llx\n", addr);
		vfree(entry->blob_desc.data);
		entry->blob_desc.data = NULL;
		return -EIO;
	}

	entry->blob_desc.size = size;

	return count;
}

static ssize_t hl_get_power_state(struct file *f, char __user *buf,
		size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	char tmp_buf[200];
	int i;

	if (*ppos)
		return 0;

	if (hdev->pdev->current_state == PCI_D0)
		i = 1;
	else if (hdev->pdev->current_state == PCI_D3hot)
		i = 2;
	else
		i = 3;

	sprintf(tmp_buf,
		"current power state: %d\n1 - D0\n2 - D3hot\n3 - Unknown\n", i);
	return simple_read_from_buffer(buf, count, ppos, tmp_buf,
			strlen(tmp_buf));
}

static ssize_t hl_set_power_state(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u32 value;
	ssize_t rc;

	rc = kstrtouint_from_user(buf, count, 10, &value);
	if (rc)
		return rc;

	if (value == 1) {
		pci_set_power_state(hdev->pdev, PCI_D0);
		pci_restore_state(hdev->pdev);
		rc = pci_enable_device(hdev->pdev);
	} else if (value == 2) {
		pci_save_state(hdev->pdev);
		pci_disable_device(hdev->pdev);
		pci_set_power_state(hdev->pdev, PCI_D3hot);
	} else {
		dev_dbg(hdev->dev, "invalid power state value %u\n", value);
		return -EINVAL;
	}

	return count;
}

static ssize_t hl_i2c_data_read(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	char tmp_buf[32];
	long val;
	ssize_t rc;

	if (*ppos)
		return 0;

	rc = hl_debugfs_i2c_read(hdev, entry->i2c_bus, entry->i2c_addr,
			entry->i2c_reg, &val);
	if (rc) {
		dev_err(hdev->dev,
			"Failed to read from I2C bus %d, addr %d, reg %d\n",
			entry->i2c_bus, entry->i2c_addr, entry->i2c_reg);
		return rc;
	}

	sprintf(tmp_buf, "0x%02lx\n", val);
	rc = simple_read_from_buffer(buf, count, ppos, tmp_buf,
			strlen(tmp_buf));

	return rc;
}

static ssize_t hl_i2c_data_write(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u32 value;
	ssize_t rc;

	rc = kstrtouint_from_user(buf, count, 16, &value);
	if (rc)
		return rc;

	rc = hl_debugfs_i2c_write(hdev, entry->i2c_bus, entry->i2c_addr,
			entry->i2c_reg, value);
	if (rc) {
		dev_err(hdev->dev,
			"Failed to write 0x%02x to I2C bus %d, addr %d, reg %d\n",
			value, entry->i2c_bus, entry->i2c_addr, entry->i2c_reg);
		return rc;
	}

	return count;
}

static ssize_t hl_led0_write(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u32 value;
	ssize_t rc;

	rc = kstrtouint_from_user(buf, count, 10, &value);
	if (rc)
		return rc;

	value = value ? 1 : 0;

	hl_debugfs_led_set(hdev, 0, value);

	return count;
}

static ssize_t hl_led1_write(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u32 value;
	ssize_t rc;

	rc = kstrtouint_from_user(buf, count, 10, &value);
	if (rc)
		return rc;

	value = value ? 1 : 0;

	hl_debugfs_led_set(hdev, 1, value);

	return count;
}

static ssize_t hl_led2_write(struct file *f, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u32 value;
	ssize_t rc;

	rc = kstrtouint_from_user(buf, count, 10, &value);
	if (rc)
		return rc;

	value = value ? 1 : 0;

	hl_debugfs_led_set(hdev, 2, value);

	return count;
}

static ssize_t hl_device_read(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	static const char *help =
		"Valid values: disable, enable, suspend, resume, cpu_timeout\n";
	return simple_read_from_buffer(buf, count, ppos, help, strlen(help));
}

static ssize_t hl_device_write(struct file *f, const char __user *buf,
				     size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	char data[30] = {0};

	/* don't allow partial writes */
	if (*ppos != 0)
		return 0;

	simple_write_to_buffer(data, 29, ppos, buf, count);

	if (strncmp("disable", data, strlen("disable")) == 0) {
		hdev->disabled = true;
	} else if (strncmp("enable", data, strlen("enable")) == 0) {
		hdev->disabled = false;
	} else if (strncmp("suspend", data, strlen("suspend")) == 0) {
		hdev->asic_funcs->suspend(hdev);
	} else if (strncmp("resume", data, strlen("resume")) == 0) {
		hdev->asic_funcs->resume(hdev);
	} else if (strncmp("cpu_timeout", data, strlen("cpu_timeout")) == 0) {
		hdev->device_cpu_disabled = true;
	} else {
		dev_err(hdev->dev,
			"Valid values: disable, enable, suspend, resume, cpu_timeout\n");
		count = -EINVAL;
	}

	return count;
}

static ssize_t hl_clk_gate_read(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	char tmp_buf[200];
	ssize_t rc;

	if (*ppos)
		return 0;

	sprintf(tmp_buf, "0x%llx\n", hdev->clock_gating_mask);
	rc = simple_read_from_buffer(buf, count, ppos, tmp_buf,
			strlen(tmp_buf) + 1);

	return rc;
}

static ssize_t hl_clk_gate_write(struct file *f, const char __user *buf,
				     size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u64 value;
	ssize_t rc;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev,
				"Can't change clock gating during reset\n");
		return 0;
	}

	rc = kstrtoull_from_user(buf, count, 16, &value);
	if (rc)
		return rc;

	hdev->clock_gating_mask = value;
	hdev->asic_funcs->set_clock_gating(hdev);

	return count;
}

static ssize_t hl_stop_on_err_read(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	char tmp_buf[200];
	ssize_t rc;

	if (*ppos)
		return 0;

	sprintf(tmp_buf, "%d\n", hdev->stop_on_err);
	rc = simple_read_from_buffer(buf, strlen(tmp_buf) + 1, ppos, tmp_buf,
			strlen(tmp_buf) + 1);

	return rc;
}

static ssize_t hl_stop_on_err_write(struct file *f, const char __user *buf,
				     size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;
	u32 value;
	ssize_t rc;

	if (atomic_read(&hdev->in_reset)) {
		dev_warn_ratelimited(hdev->dev,
				"Can't change stop on error during reset\n");
		return 0;
	}

	rc = kstrtouint_from_user(buf, count, 10, &value);
	if (rc)
		return rc;

	hdev->stop_on_err = value ? 1 : 0;

	hl_device_reset(hdev, 0);

	return count;
}

static ssize_t hl_security_violations_read(struct file *f, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct hl_dbg_device_entry *entry = file_inode(f)->i_private;
	struct hl_device *hdev = entry->hdev;

	hdev->asic_funcs->ack_protection_bits_errors(hdev);

	return 0;
}

static const struct file_operations hl_data32b_fops = {
	.owner = THIS_MODULE,
	.read = hl_data_read32,
	.write = hl_data_write32
};

static const struct file_operations hl_data64b_fops = {
	.owner = THIS_MODULE,
	.read = hl_data_read64,
	.write = hl_data_write64
};

static const struct file_operations hl_dma_size_fops = {
	.owner = THIS_MODULE,
	.write = hl_dma_size_write
};

static const struct file_operations hl_i2c_data_fops = {
	.owner = THIS_MODULE,
	.read = hl_i2c_data_read,
	.write = hl_i2c_data_write
};

static const struct file_operations hl_power_fops = {
	.owner = THIS_MODULE,
	.read = hl_get_power_state,
	.write = hl_set_power_state
};

static const struct file_operations hl_led0_fops = {
	.owner = THIS_MODULE,
	.write = hl_led0_write
};

static const struct file_operations hl_led1_fops = {
	.owner = THIS_MODULE,
	.write = hl_led1_write
};

static const struct file_operations hl_led2_fops = {
	.owner = THIS_MODULE,
	.write = hl_led2_write
};

static const struct file_operations hl_device_fops = {
	.owner = THIS_MODULE,
	.read = hl_device_read,
	.write = hl_device_write
};

static const struct file_operations hl_clk_gate_fops = {
	.owner = THIS_MODULE,
	.read = hl_clk_gate_read,
	.write = hl_clk_gate_write
};

static const struct file_operations hl_stop_on_err_fops = {
	.owner = THIS_MODULE,
	.read = hl_stop_on_err_read,
	.write = hl_stop_on_err_write
};

static const struct file_operations hl_security_violations_fops = {
	.owner = THIS_MODULE,
	.read = hl_security_violations_read
};

static const struct hl_info_list hl_debugfs_list[] = {
	{"command_buffers", command_buffers_show, NULL},
	{"command_submission", command_submission_show, NULL},
	{"command_submission_jobs", command_submission_jobs_show, NULL},
	{"userptr", userptr_show, NULL},
	{"vm", vm_show, NULL},
	{"mmu", mmu_show, mmu_asid_va_write},
	{"engines", engines_show, NULL}
};

static int hl_debugfs_open(struct inode *inode, struct file *file)
{
	struct hl_debugfs_entry *node = inode->i_private;

	return single_open(file, node->info_ent->show, node);
}

static ssize_t hl_debugfs_write(struct file *file, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	struct hl_debugfs_entry *node = file->f_inode->i_private;

	if (node->info_ent->write)
		return node->info_ent->write(file, buf, count, f_pos);
	else
		return -EINVAL;

}

static const struct file_operations hl_debugfs_fops = {
	.owner = THIS_MODULE,
	.open = hl_debugfs_open,
	.read = seq_read,
	.write = hl_debugfs_write,
	.llseek = seq_lseek,
	.release = single_release,
};

void hl_debugfs_add_device(struct hl_device *hdev)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;
	int count = ARRAY_SIZE(hl_debugfs_list);
	struct hl_debugfs_entry *entry;
	int i;

	dev_entry->hdev = hdev;
	dev_entry->entry_arr = kmalloc_array(count,
					sizeof(struct hl_debugfs_entry),
					GFP_KERNEL);
	if (!dev_entry->entry_arr)
		return;

	dev_entry->blob_desc.size = 0;
	dev_entry->blob_desc.data = NULL;

	INIT_LIST_HEAD(&dev_entry->file_list);
	INIT_LIST_HEAD(&dev_entry->cb_list);
	INIT_LIST_HEAD(&dev_entry->cs_list);
	INIT_LIST_HEAD(&dev_entry->cs_job_list);
	INIT_LIST_HEAD(&dev_entry->userptr_list);
	INIT_LIST_HEAD(&dev_entry->ctx_mem_hash_list);
	mutex_init(&dev_entry->file_mutex);
	spin_lock_init(&dev_entry->cb_spinlock);
	spin_lock_init(&dev_entry->cs_spinlock);
	spin_lock_init(&dev_entry->cs_job_spinlock);
	spin_lock_init(&dev_entry->userptr_spinlock);
	spin_lock_init(&dev_entry->ctx_mem_hash_spinlock);

	dev_entry->root = debugfs_create_dir(dev_name(hdev->dev),
						hl_debug_root);

	debugfs_create_x64("addr",
				0644,
				dev_entry->root,
				&dev_entry->addr);

	debugfs_create_file("data32",
				0644,
				dev_entry->root,
				dev_entry,
				&hl_data32b_fops);

	debugfs_create_file("data64",
				0644,
				dev_entry->root,
				dev_entry,
				&hl_data64b_fops);

	debugfs_create_file("set_power_state",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_power_fops);

	debugfs_create_u8("i2c_bus",
				0644,
				dev_entry->root,
				&dev_entry->i2c_bus);

	debugfs_create_u8("i2c_addr",
				0644,
				dev_entry->root,
				&dev_entry->i2c_addr);

	debugfs_create_u8("i2c_reg",
				0644,
				dev_entry->root,
				&dev_entry->i2c_reg);

	debugfs_create_file("i2c_data",
				0644,
				dev_entry->root,
				dev_entry,
				&hl_i2c_data_fops);

	debugfs_create_file("led0",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_led0_fops);

	debugfs_create_file("led1",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_led1_fops);

	debugfs_create_file("led2",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_led2_fops);

	debugfs_create_file("device",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_device_fops);

	debugfs_create_file("clk_gate",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_clk_gate_fops);

	debugfs_create_file("stop_on_err",
				0644,
				dev_entry->root,
				dev_entry,
				&hl_stop_on_err_fops);

	debugfs_create_file("dump_security_violations",
				0644,
				dev_entry->root,
				dev_entry,
				&hl_security_violations_fops);

	debugfs_create_file("dma_size",
				0200,
				dev_entry->root,
				dev_entry,
				&hl_dma_size_fops);

	debugfs_create_blob("data_dma",
				0400,
				dev_entry->root,
				&dev_entry->blob_desc);

	for (i = 0, entry = dev_entry->entry_arr ; i < count ; i++, entry++) {
		debugfs_create_file(hl_debugfs_list[i].name,
					0444,
					dev_entry->root,
					entry,
					&hl_debugfs_fops);
		entry->info_ent = &hl_debugfs_list[i];
		entry->dev_entry = dev_entry;
	}
}

void hl_debugfs_remove_device(struct hl_device *hdev)
{
	struct hl_dbg_device_entry *entry = &hdev->hl_debugfs;

	debugfs_remove_recursive(entry->root);

	mutex_destroy(&entry->file_mutex);

	vfree(entry->blob_desc.data);

	kfree(entry->entry_arr);
}

void hl_debugfs_add_file(struct hl_fpriv *hpriv)
{
	struct hl_dbg_device_entry *dev_entry = &hpriv->hdev->hl_debugfs;

	mutex_lock(&dev_entry->file_mutex);
	list_add(&hpriv->debugfs_list, &dev_entry->file_list);
	mutex_unlock(&dev_entry->file_mutex);
}

void hl_debugfs_remove_file(struct hl_fpriv *hpriv)
{
	struct hl_dbg_device_entry *dev_entry = &hpriv->hdev->hl_debugfs;

	mutex_lock(&dev_entry->file_mutex);
	list_del(&hpriv->debugfs_list);
	mutex_unlock(&dev_entry->file_mutex);
}

void hl_debugfs_add_cb(struct hl_cb *cb)
{
	struct hl_dbg_device_entry *dev_entry = &cb->hdev->hl_debugfs;

	spin_lock(&dev_entry->cb_spinlock);
	list_add(&cb->debugfs_list, &dev_entry->cb_list);
	spin_unlock(&dev_entry->cb_spinlock);
}

void hl_debugfs_remove_cb(struct hl_cb *cb)
{
	struct hl_dbg_device_entry *dev_entry = &cb->hdev->hl_debugfs;

	spin_lock(&dev_entry->cb_spinlock);
	list_del(&cb->debugfs_list);
	spin_unlock(&dev_entry->cb_spinlock);
}

void hl_debugfs_add_cs(struct hl_cs *cs)
{
	struct hl_dbg_device_entry *dev_entry = &cs->ctx->hdev->hl_debugfs;

	spin_lock(&dev_entry->cs_spinlock);
	list_add(&cs->debugfs_list, &dev_entry->cs_list);
	spin_unlock(&dev_entry->cs_spinlock);
}

void hl_debugfs_remove_cs(struct hl_cs *cs)
{
	struct hl_dbg_device_entry *dev_entry = &cs->ctx->hdev->hl_debugfs;

	spin_lock(&dev_entry->cs_spinlock);
	list_del(&cs->debugfs_list);
	spin_unlock(&dev_entry->cs_spinlock);
}

void hl_debugfs_add_job(struct hl_device *hdev, struct hl_cs_job *job)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;

	spin_lock(&dev_entry->cs_job_spinlock);
	list_add(&job->debugfs_list, &dev_entry->cs_job_list);
	spin_unlock(&dev_entry->cs_job_spinlock);
}

void hl_debugfs_remove_job(struct hl_device *hdev, struct hl_cs_job *job)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;

	spin_lock(&dev_entry->cs_job_spinlock);
	list_del(&job->debugfs_list);
	spin_unlock(&dev_entry->cs_job_spinlock);
}

void hl_debugfs_add_userptr(struct hl_device *hdev, struct hl_userptr *userptr)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;

	spin_lock(&dev_entry->userptr_spinlock);
	list_add(&userptr->debugfs_list, &dev_entry->userptr_list);
	spin_unlock(&dev_entry->userptr_spinlock);
}

void hl_debugfs_remove_userptr(struct hl_device *hdev,
				struct hl_userptr *userptr)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;

	spin_lock(&dev_entry->userptr_spinlock);
	list_del(&userptr->debugfs_list);
	spin_unlock(&dev_entry->userptr_spinlock);
}

void hl_debugfs_add_ctx_mem_hash(struct hl_device *hdev, struct hl_ctx *ctx)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;

	spin_lock(&dev_entry->ctx_mem_hash_spinlock);
	list_add(&ctx->debugfs_list, &dev_entry->ctx_mem_hash_list);
	spin_unlock(&dev_entry->ctx_mem_hash_spinlock);
}

void hl_debugfs_remove_ctx_mem_hash(struct hl_device *hdev, struct hl_ctx *ctx)
{
	struct hl_dbg_device_entry *dev_entry = &hdev->hl_debugfs;

	spin_lock(&dev_entry->ctx_mem_hash_spinlock);
	list_del(&ctx->debugfs_list);
	spin_unlock(&dev_entry->ctx_mem_hash_spinlock);
}

void __init hl_debugfs_init(void)
{
	hl_debug_root = debugfs_create_dir("habanalabs", NULL);
}

void hl_debugfs_fini(void)
{
	debugfs_remove_recursive(hl_debug_root);
}

// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BIOS run time interface routines.
 *
 * (C) Copyright 2020 Hewlett Packard Enterprise Development LP
 * Copyright (C) 2007-2017 Silicon Graphics, Inc. All rights reserved.
 * Copyright (c) Russ Anderson <rja@sgi.com>
 */

#include <linux/efi.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <asm/efi.h>
#include <linux/io.h>
#include <asm/pgalloc.h>
#include <asm/uv/bios.h>
#include <asm/uv/uv_hub.h>

unsigned long uv_systab_phys __ro_after_init = EFI_INVALID_TABLE_ADDR;

struct uv_systab *uv_systab;

static s64 __uv_bios_call(enum uv_bios_cmd which, u64 a1, u64 a2, u64 a3,
			u64 a4, u64 a5)
{
	struct uv_systab *tab = uv_systab;
	s64 ret;

	if (!tab || !tab->function)
		/*
		 * BIOS does not support UV systab
		 */
		return BIOS_STATUS_UNIMPLEMENTED;

	ret = efi_call_virt_pointer(tab, function, (u64)which, a1, a2, a3, a4, a5);

	return ret;
}

static s64 uv_bios_call(enum uv_bios_cmd which, u64 a1, u64 a2, u64 a3, u64 a4,
		u64 a5)
{
	s64 ret;

	if (down_interruptible(&__efi_uv_runtime_lock))
		return BIOS_STATUS_ABORT;

	ret = __uv_bios_call(which, a1, a2, a3, a4, a5);
	up(&__efi_uv_runtime_lock);

	return ret;
}

static s64 uv_bios_call_irqsave(enum uv_bios_cmd which, u64 a1, u64 a2, u64 a3,
		u64 a4, u64 a5)
{
	unsigned long bios_flags;
	s64 ret;

	if (down_interruptible(&__efi_uv_runtime_lock))
		return BIOS_STATUS_ABORT;

	local_irq_save(bios_flags);
	ret = __uv_bios_call(which, a1, a2, a3, a4, a5);
	local_irq_restore(bios_flags);

	up(&__efi_uv_runtime_lock);

	return ret;
}

long sn_partition_id;
EXPORT_SYMBOL_GPL(sn_partition_id);
long sn_coherency_id;
EXPORT_SYMBOL_GPL(sn_coherency_id);
long sn_region_size;
EXPORT_SYMBOL_GPL(sn_region_size);
long system_serial_number;
int uv_type;

s64 uv_bios_get_sn_info(int fc, int *uvtype, long *partid, long *coher,
		long *region, long *ssn)
{
	s64 ret;
	u64 v0, v1;
	union partition_info_u part;

	ret = uv_bios_call_irqsave(UV_BIOS_GET_SN_INFO, fc,
				(u64)(&v0), (u64)(&v1), 0, 0);
	if (ret != BIOS_STATUS_SUCCESS)
		return ret;

	part.val = v0;
	if (uvtype)
		*uvtype = part.hub_version;
	if (partid)
		*partid = part.partition_id;
	if (coher)
		*coher = part.coherence_id;
	if (region)
		*region = part.region_size;
	if (ssn)
		*ssn = v1;
	return ret;
}

int
uv_bios_mq_watchlist_alloc(unsigned long addr, unsigned int mq_size,
			   unsigned long *intr_mmr_offset)
{
	u64 watchlist;
	s64 ret;

	/*
	 * bios returns watchlist number or negative error number.
	 */
	ret = (int)uv_bios_call_irqsave(UV_BIOS_WATCHLIST_ALLOC, addr,
			mq_size, (u64)intr_mmr_offset,
			(u64)&watchlist, 0);
	if (ret < BIOS_STATUS_SUCCESS)
		return ret;

	return watchlist;
}
EXPORT_SYMBOL_GPL(uv_bios_mq_watchlist_alloc);

int
uv_bios_mq_watchlist_free(int blade, int watchlist_num)
{
	return (int)uv_bios_call_irqsave(UV_BIOS_WATCHLIST_FREE,
				blade, watchlist_num, 0, 0, 0);
}
EXPORT_SYMBOL_GPL(uv_bios_mq_watchlist_free);

s64
uv_bios_change_memprotect(u64 paddr, u64 len, enum uv_memprotect perms)
{
	return uv_bios_call_irqsave(UV_BIOS_MEMPROTECT, paddr, len,
					perms, 0, 0);
}
EXPORT_SYMBOL_GPL(uv_bios_change_memprotect);

s64
uv_bios_reserved_page_pa(u64 buf, u64 *cookie, u64 *addr, u64 *len)
{
	return uv_bios_call_irqsave(UV_BIOS_GET_PARTITION_ADDR, (u64)cookie,
				    (u64)addr, buf, (u64)len, 0);
}
EXPORT_SYMBOL_GPL(uv_bios_reserved_page_pa);

s64 uv_bios_freq_base(u64 clock_type, u64 *ticks_per_second)
{
	return uv_bios_call(UV_BIOS_FREQ_BASE, clock_type,
			   (u64)ticks_per_second, 0, 0, 0);
}

/*
 * uv_bios_set_legacy_vga_target - Set Legacy VGA I/O Target
 * @decode: true to enable target, false to disable target
 * @domain: PCI domain number
 * @bus: PCI bus number
 *
 * Returns:
 *    0: Success
 *    -EINVAL: Invalid domain or bus number
 *    -ENOSYS: Capability not available
 *    -EBUSY: Legacy VGA I/O cannot be retargeted at this time
 */
int uv_bios_set_legacy_vga_target(bool decode, int domain, int bus)
{
	return uv_bios_call(UV_BIOS_SET_LEGACY_VGA_TARGET,
				(u64)decode, (u64)domain, (u64)bus, 0, 0);
}

extern s64 uv_bios_get_master_nasid(u64 size, u64 *master_nasid)
{
	return uv_bios_call(UV_BIOS_EXTRA, 0, UV_BIOS_EXTRA_MASTER_NASID, 0,
				size, (u64)master_nasid);
}
EXPORT_SYMBOL_GPL(uv_bios_get_master_nasid);

extern s64 uv_bios_get_heapsize(u64 nasid, u64 size, u64 *heap_size)
{
	return uv_bios_call(UV_BIOS_EXTRA, nasid, UV_BIOS_EXTRA_GET_HEAPSIZE,
				0, size, (u64)heap_size);
}
EXPORT_SYMBOL_GPL(uv_bios_get_heapsize);

extern s64 uv_bios_install_heap(u64 nasid, u64 heap_size, u64 *bios_heap)
{
	return uv_bios_call(UV_BIOS_EXTRA, nasid, UV_BIOS_EXTRA_INSTALL_HEAP,
				0, heap_size, (u64)bios_heap);
}
EXPORT_SYMBOL_GPL(uv_bios_install_heap);

extern s64 uv_bios_obj_count(u64 nasid, u64 size, u64 *objcnt)
{
	return uv_bios_call(UV_BIOS_EXTRA, nasid, UV_BIOS_EXTRA_OBJECT_COUNT,
				0, size, (u64)objcnt);
}
EXPORT_SYMBOL_GPL(uv_bios_obj_count);

extern s64 uv_bios_enum_objs(u64 nasid, u64 size, u64 *objbuf)
{
	return uv_bios_call(UV_BIOS_EXTRA, nasid, UV_BIOS_EXTRA_ENUM_OBJECTS,
				0, size, (u64)objbuf);
}
EXPORT_SYMBOL_GPL(uv_bios_enum_objs);

extern s64 uv_bios_enum_ports(u64 nasid, u64 obj_id, u64 size, u64 *portbuf)
{
	return uv_bios_call(UV_BIOS_EXTRA, nasid, UV_BIOS_EXTRA_ENUM_PORTS,
				obj_id, size, (u64)portbuf);
}
EXPORT_SYMBOL_GPL(uv_bios_enum_ports);

extern s64 uv_bios_get_geoinfo(u64 nasid, u64 size, u64 *buf)
{
	return uv_bios_call(UV_BIOS_GET_GEOINFO, nasid, (u64)buf, size, 0, 0);
}
EXPORT_SYMBOL_GPL(uv_bios_get_geoinfo);

extern s64 uv_bios_get_pci_topology(u64 size, u64 *buf)
{
	return uv_bios_call(UV_BIOS_GET_PCI_TOPOLOGY, (u64)buf, size, 0, 0, 0);
}
EXPORT_SYMBOL_GPL(uv_bios_get_pci_topology);

unsigned long get_uv_systab_phys(bool msg)
{
	if ((uv_systab_phys == EFI_INVALID_TABLE_ADDR) ||
	    !uv_systab_phys || efi_runtime_disabled()) {
		if (msg)
			pr_crit("UV: UVsystab: missing\n");
		return 0;
	}
	return uv_systab_phys;
}

int uv_bios_init(void)
{
	unsigned long uv_systab_phys_addr;

	uv_systab = NULL;
	uv_systab_phys_addr = get_uv_systab_phys(1);
	if (!uv_systab_phys_addr)
		return -EEXIST;

	uv_systab = ioremap(uv_systab_phys_addr, sizeof(struct uv_systab));
	if (!uv_systab || strncmp(uv_systab->signature, UV_SYSTAB_SIG, 4)) {
		pr_err("UV: UVsystab: bad signature!\n");
		iounmap(uv_systab);
		return -EINVAL;
	}

	/* Starting with UV4 the UV systab size is variable */
	if (uv_systab->revision >= UV_SYSTAB_VERSION_UV4) {
		int size = uv_systab->size;

		iounmap(uv_systab);
		uv_systab = ioremap(uv_systab_phys_addr, size);
		if (!uv_systab) {
			pr_err("UV: UVsystab: ioremap(%d) failed!\n", size);
			return -EFAULT;
		}
	}
	pr_info("UV: UVsystab: Revision:%x\n", uv_systab->revision);
	return 0;
}

// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2006-2014 Intel Corporation.
 *
 * Authors: David Woodhouse <dwmw2@infradead.org>,
 *          Ashok Raj <ashok.raj@intel.com>,
 *          Shaohua Li <shaohua.li@intel.com>,
 *          Anil S Keshavamurthy <anil.s.keshavamurthy@intel.com>,
 *          Fenghua Yu <fenghua.yu@intel.com>
 *          Joerg Roedel <jroedel@suse.de>
 */

#define pr_fmt(fmt)     "DMAR: " fmt
#define dev_fmt(fmt)    pr_fmt(fmt)

#include <linux/init.h>
#include <linux/bitmap.h>
#include <linux/debugfs.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/pci.h>
#include <linux/dmar.h>
#include <linux/dma-map-ops.h>
#include <linux/mempool.h>
#include <linux/memory.h>
#include <linux/cpu.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/iova.h>
#include <linux/iommu.h>
#include <linux/dma-iommu.h>
#include <linux/intel-iommu.h>
#include <linux/syscore_ops.h>
#include <linux/tboot.h>
#include <linux/dmi.h>
#include <linux/pci-ats.h>
#include <linux/memblock.h>
#include <linux/dma-direct.h>
#include <linux/crash_dump.h>
#include <linux/numa.h>
#include <asm/irq_remapping.h>
#include <asm/cacheflush.h>
#include <asm/iommu.h>

#include "../irq_remapping.h"
#include "pasid.h"
#include "cap_audit.h"

#define ROOT_SIZE		VTD_PAGE_SIZE
#define CONTEXT_SIZE		VTD_PAGE_SIZE

#define IS_GFX_DEVICE(pdev) ((pdev->class >> 16) == PCI_BASE_CLASS_DISPLAY)
#define IS_USB_DEVICE(pdev) ((pdev->class >> 8) == PCI_CLASS_SERIAL_USB)
#define IS_ISA_DEVICE(pdev) ((pdev->class >> 8) == PCI_CLASS_BRIDGE_ISA)
#define IS_AZALIA(pdev) ((pdev)->vendor == 0x8086 && (pdev)->device == 0x3a3e)

#define IOAPIC_RANGE_START	(0xfee00000)
#define IOAPIC_RANGE_END	(0xfeefffff)
#define IOVA_START_ADDR		(0x1000)

#define DEFAULT_DOMAIN_ADDRESS_WIDTH 57

#define MAX_AGAW_WIDTH 64
#define MAX_AGAW_PFN_WIDTH	(MAX_AGAW_WIDTH - VTD_PAGE_SHIFT)

#define __DOMAIN_MAX_PFN(gaw)  ((((uint64_t)1) << ((gaw) - VTD_PAGE_SHIFT)) - 1)
#define __DOMAIN_MAX_ADDR(gaw) ((((uint64_t)1) << (gaw)) - 1)

/* We limit DOMAIN_MAX_PFN to fit in an unsigned long, and DOMAIN_MAX_ADDR
   to match. That way, we can use 'unsigned long' for PFNs with impunity. */
#define DOMAIN_MAX_PFN(gaw)	((unsigned long) min_t(uint64_t, \
				__DOMAIN_MAX_PFN(gaw), (unsigned long)-1))
#define DOMAIN_MAX_ADDR(gaw)	(((uint64_t)__DOMAIN_MAX_PFN(gaw)) << VTD_PAGE_SHIFT)

/* IO virtual address start page frame number */
#define IOVA_START_PFN		(1)

#define IOVA_PFN(addr)		((addr) >> PAGE_SHIFT)

/* page table handling */
#define LEVEL_STRIDE		(9)
#define LEVEL_MASK		(((u64)1 << LEVEL_STRIDE) - 1)

/*
 * This bitmap is used to advertise the page sizes our hardware support
 * to the IOMMU core, which will then use this information to split
 * physically contiguous memory regions it is mapping into page sizes
 * that we support.
 *
 * Traditionally the IOMMU core just handed us the mappings directly,
 * after making sure the size is an order of a 4KiB page and that the
 * mapping has natural alignment.
 *
 * To retain this behavior, we currently advertise that we support
 * all page sizes that are an order of 4KiB.
 *
 * If at some point we'd like to utilize the IOMMU core's new behavior,
 * we could change this to advertise the real page sizes we support.
 */
#define INTEL_IOMMU_PGSIZES	(~0xFFFUL)

static inline int agaw_to_level(int agaw)
{
	return agaw + 2;
}

static inline int agaw_to_width(int agaw)
{
	return min_t(int, 30 + agaw * LEVEL_STRIDE, MAX_AGAW_WIDTH);
}

static inline int width_to_agaw(int width)
{
	return DIV_ROUND_UP(width - 30, LEVEL_STRIDE);
}

static inline unsigned int level_to_offset_bits(int level)
{
	return (level - 1) * LEVEL_STRIDE;
}

static inline int pfn_level_offset(u64 pfn, int level)
{
	return (pfn >> level_to_offset_bits(level)) & LEVEL_MASK;
}

static inline u64 level_mask(int level)
{
	return -1ULL << level_to_offset_bits(level);
}

static inline u64 level_size(int level)
{
	return 1ULL << level_to_offset_bits(level);
}

static inline u64 align_to_level(u64 pfn, int level)
{
	return (pfn + level_size(level) - 1) & level_mask(level);
}

static inline unsigned long lvl_to_nr_pages(unsigned int lvl)
{
	return 1UL << min_t(int, (lvl - 1) * LEVEL_STRIDE, MAX_AGAW_PFN_WIDTH);
}

/* VT-d pages must always be _smaller_ than MM pages. Otherwise things
   are never going to work. */
static inline unsigned long dma_to_mm_pfn(unsigned long dma_pfn)
{
	return dma_pfn >> (PAGE_SHIFT - VTD_PAGE_SHIFT);
}

static inline unsigned long mm_to_dma_pfn(unsigned long mm_pfn)
{
	return mm_pfn << (PAGE_SHIFT - VTD_PAGE_SHIFT);
}
static inline unsigned long page_to_dma_pfn(struct page *pg)
{
	return mm_to_dma_pfn(page_to_pfn(pg));
}
static inline unsigned long virt_to_dma_pfn(void *p)
{
	return page_to_dma_pfn(virt_to_page(p));
}

/* global iommu list, set NULL for ignored DMAR units */
static struct intel_iommu **g_iommus;

static void __init check_tylersburg_isoch(void);
static int rwbf_quirk;

/*
 * set to 1 to panic kernel if can't successfully enable VT-d
 * (used when kernel is launched w/ TXT)
 */
static int force_on = 0;
static int intel_iommu_tboot_noforce;
static int no_platform_optin;

#define ROOT_ENTRY_NR (VTD_PAGE_SIZE/sizeof(struct root_entry))

/*
 * Take a root_entry and return the Lower Context Table Pointer (LCTP)
 * if marked present.
 */
static phys_addr_t root_entry_lctp(struct root_entry *re)
{
	if (!(re->lo & 1))
		return 0;

	return re->lo & VTD_PAGE_MASK;
}

/*
 * Take a root_entry and return the Upper Context Table Pointer (UCTP)
 * if marked present.
 */
static phys_addr_t root_entry_uctp(struct root_entry *re)
{
	if (!(re->hi & 1))
		return 0;

	return re->hi & VTD_PAGE_MASK;
}

static inline void context_clear_pasid_enable(struct context_entry *context)
{
	context->lo &= ~(1ULL << 11);
}

static inline bool context_pasid_enabled(struct context_entry *context)
{
	return !!(context->lo & (1ULL << 11));
}

static inline void context_set_copied(struct context_entry *context)
{
	context->hi |= (1ull << 3);
}

static inline bool context_copied(struct context_entry *context)
{
	return !!(context->hi & (1ULL << 3));
}

static inline bool __context_present(struct context_entry *context)
{
	return (context->lo & 1);
}

bool context_present(struct context_entry *context)
{
	return context_pasid_enabled(context) ?
	     __context_present(context) :
	     __context_present(context) && !context_copied(context);
}

static inline void context_set_present(struct context_entry *context)
{
	context->lo |= 1;
}

static inline void context_set_fault_enable(struct context_entry *context)
{
	context->lo &= (((u64)-1) << 2) | 1;
}

static inline void context_set_translation_type(struct context_entry *context,
						unsigned long value)
{
	context->lo &= (((u64)-1) << 4) | 3;
	context->lo |= (value & 3) << 2;
}

static inline void context_set_address_root(struct context_entry *context,
					    unsigned long value)
{
	context->lo &= ~VTD_PAGE_MASK;
	context->lo |= value & VTD_PAGE_MASK;
}

static inline void context_set_address_width(struct context_entry *context,
					     unsigned long value)
{
	context->hi |= value & 7;
}

static inline void context_set_domain_id(struct context_entry *context,
					 unsigned long value)
{
	context->hi |= (value & ((1 << 16) - 1)) << 8;
}

static inline int context_domain_id(struct context_entry *c)
{
	return((c->hi >> 8) & 0xffff);
}

static inline void context_clear_entry(struct context_entry *context)
{
	context->lo = 0;
	context->hi = 0;
}

/*
 * This domain is a statically identity mapping domain.
 *	1. This domain creats a static 1:1 mapping to all usable memory.
 * 	2. It maps to each iommu if successful.
 *	3. Each iommu mapps to this domain if successful.
 */
static struct dmar_domain *si_domain;
static int hw_pass_through = 1;

#define for_each_domain_iommu(idx, domain)			\
	for (idx = 0; idx < g_num_of_iommus; idx++)		\
		if (domain->iommu_refcnt[idx])

struct dmar_rmrr_unit {
	struct list_head list;		/* list of rmrr units	*/
	struct acpi_dmar_header *hdr;	/* ACPI header		*/
	u64	base_address;		/* reserved base address*/
	u64	end_address;		/* reserved end address */
	struct dmar_dev_scope *devices;	/* target devices */
	int	devices_cnt;		/* target device count */
};

struct dmar_atsr_unit {
	struct list_head list;		/* list of ATSR units */
	struct acpi_dmar_header *hdr;	/* ACPI header */
	struct dmar_dev_scope *devices;	/* target devices */
	int devices_cnt;		/* target device count */
	u8 include_all:1;		/* include all ports */
};

struct dmar_satc_unit {
	struct list_head list;		/* list of SATC units */
	struct acpi_dmar_header *hdr;	/* ACPI header */
	struct dmar_dev_scope *devices;	/* target devices */
	struct intel_iommu *iommu;	/* the corresponding iommu */
	int devices_cnt;		/* target device count */
	u8 atc_required:1;		/* ATS is required */
};

static LIST_HEAD(dmar_atsr_units);
static LIST_HEAD(dmar_rmrr_units);
static LIST_HEAD(dmar_satc_units);

#define for_each_rmrr_units(rmrr) \
	list_for_each_entry(rmrr, &dmar_rmrr_units, list)

/* bitmap for indexing intel_iommus */
static int g_num_of_iommus;

static void domain_exit(struct dmar_domain *domain);
static void domain_remove_dev_info(struct dmar_domain *domain);
static void dmar_remove_one_dev_info(struct device *dev);
static void __dmar_remove_one_dev_info(struct device_domain_info *info);
static int intel_iommu_attach_device(struct iommu_domain *domain,
				     struct device *dev);
static phys_addr_t intel_iommu_iova_to_phys(struct iommu_domain *domain,
					    dma_addr_t iova);

#ifdef CONFIG_INTEL_IOMMU_DEFAULT_ON
int dmar_disabled = 0;
#else
int dmar_disabled = 1;
#endif /* CONFIG_INTEL_IOMMU_DEFAULT_ON */

#ifdef CONFIG_INTEL_IOMMU_SCALABLE_MODE_DEFAULT_ON
int intel_iommu_sm = 1;
#else
int intel_iommu_sm;
#endif /* CONFIG_INTEL_IOMMU_SCALABLE_MODE_DEFAULT_ON */

int intel_iommu_enabled = 0;
EXPORT_SYMBOL_GPL(intel_iommu_enabled);

static int dmar_map_gfx = 1;
static int intel_iommu_strict;
static int intel_iommu_superpage = 1;
static int iommu_identity_mapping;
static int iommu_skip_te_disable;

#define IDENTMAP_GFX		2
#define IDENTMAP_AZALIA		4

int intel_iommu_gfx_mapped;
EXPORT_SYMBOL_GPL(intel_iommu_gfx_mapped);

#define DEFER_DEVICE_DOMAIN_INFO ((struct device_domain_info *)(-2))
struct device_domain_info *get_domain_info(struct device *dev)
{
	struct device_domain_info *info;

	if (!dev)
		return NULL;

	info = dev_iommu_priv_get(dev);
	if (unlikely(info == DEFER_DEVICE_DOMAIN_INFO))
		return NULL;

	return info;
}

DEFINE_SPINLOCK(device_domain_lock);
static LIST_HEAD(device_domain_list);

/*
 * Iterate over elements in device_domain_list and call the specified
 * callback @fn against each element.
 */
int for_each_device_domain(int (*fn)(struct device_domain_info *info,
				     void *data), void *data)
{
	int ret = 0;
	unsigned long flags;
	struct device_domain_info *info;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry(info, &device_domain_list, global) {
		ret = fn(info, data);
		if (ret) {
			spin_unlock_irqrestore(&device_domain_lock, flags);
			return ret;
		}
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return 0;
}

const struct iommu_ops intel_iommu_ops;

static bool translation_pre_enabled(struct intel_iommu *iommu)
{
	return (iommu->flags & VTD_FLAG_TRANS_PRE_ENABLED);
}

static void clear_translation_pre_enabled(struct intel_iommu *iommu)
{
	iommu->flags &= ~VTD_FLAG_TRANS_PRE_ENABLED;
}

static void init_translation_status(struct intel_iommu *iommu)
{
	u32 gsts;

	gsts = readl(iommu->reg + DMAR_GSTS_REG);
	if (gsts & DMA_GSTS_TES)
		iommu->flags |= VTD_FLAG_TRANS_PRE_ENABLED;
}

static int __init intel_iommu_setup(char *str)
{
	if (!str)
		return -EINVAL;
	while (*str) {
		if (!strncmp(str, "on", 2)) {
			dmar_disabled = 0;
			pr_info("IOMMU enabled\n");
		} else if (!strncmp(str, "off", 3)) {
			dmar_disabled = 1;
			no_platform_optin = 1;
			pr_info("IOMMU disabled\n");
		} else if (!strncmp(str, "igfx_off", 8)) {
			dmar_map_gfx = 0;
			pr_info("Disable GFX device mapping\n");
		} else if (!strncmp(str, "forcedac", 8)) {
			pr_warn("intel_iommu=forcedac deprecated; use iommu.forcedac instead\n");
			iommu_dma_forcedac = true;
		} else if (!strncmp(str, "strict", 6)) {
			pr_info("Disable batched IOTLB flush\n");
			intel_iommu_strict = 1;
		} else if (!strncmp(str, "sp_off", 6)) {
			pr_info("Disable supported super page\n");
			intel_iommu_superpage = 0;
		} else if (!strncmp(str, "sm_on", 5)) {
			pr_info("Intel-IOMMU: scalable mode supported\n");
			intel_iommu_sm = 1;
		} else if (!strncmp(str, "tboot_noforce", 13)) {
			pr_info("Intel-IOMMU: not forcing on after tboot. This could expose security risk for tboot\n");
			intel_iommu_tboot_noforce = 1;
		}

		str += strcspn(str, ",");
		while (*str == ',')
			str++;
	}
	return 0;
}
__setup("intel_iommu=", intel_iommu_setup);

static struct kmem_cache *iommu_domain_cache;
static struct kmem_cache *iommu_devinfo_cache;

static struct dmar_domain* get_iommu_domain(struct intel_iommu *iommu, u16 did)
{
	struct dmar_domain **domains;
	int idx = did >> 8;

	domains = iommu->domains[idx];
	if (!domains)
		return NULL;

	return domains[did & 0xff];
}

static void set_iommu_domain(struct intel_iommu *iommu, u16 did,
			     struct dmar_domain *domain)
{
	struct dmar_domain **domains;
	int idx = did >> 8;

	if (!iommu->domains[idx]) {
		size_t size = 256 * sizeof(struct dmar_domain *);
		iommu->domains[idx] = kzalloc(size, GFP_ATOMIC);
	}

	domains = iommu->domains[idx];
	if (WARN_ON(!domains))
		return;
	else
		domains[did & 0xff] = domain;
}

void *alloc_pgtable_page(int node)
{
	struct page *page;
	void *vaddr = NULL;

	page = alloc_pages_node(node, GFP_ATOMIC | __GFP_ZERO, 0);
	if (page)
		vaddr = page_address(page);
	return vaddr;
}

void free_pgtable_page(void *vaddr)
{
	free_page((unsigned long)vaddr);
}

static inline void *alloc_domain_mem(void)
{
	return kmem_cache_alloc(iommu_domain_cache, GFP_ATOMIC);
}

static void free_domain_mem(void *vaddr)
{
	kmem_cache_free(iommu_domain_cache, vaddr);
}

static inline void * alloc_devinfo_mem(void)
{
	return kmem_cache_alloc(iommu_devinfo_cache, GFP_ATOMIC);
}

static inline void free_devinfo_mem(void *vaddr)
{
	kmem_cache_free(iommu_devinfo_cache, vaddr);
}

static inline int domain_type_is_si(struct dmar_domain *domain)
{
	return domain->flags & DOMAIN_FLAG_STATIC_IDENTITY;
}

static inline bool domain_use_first_level(struct dmar_domain *domain)
{
	return domain->flags & DOMAIN_FLAG_USE_FIRST_LEVEL;
}

static inline int domain_pfn_supported(struct dmar_domain *domain,
				       unsigned long pfn)
{
	int addr_width = agaw_to_width(domain->agaw) - VTD_PAGE_SHIFT;

	return !(addr_width < BITS_PER_LONG && pfn >> addr_width);
}

static int __iommu_calculate_agaw(struct intel_iommu *iommu, int max_gaw)
{
	unsigned long sagaw;
	int agaw = -1;

	sagaw = cap_sagaw(iommu->cap);
	for (agaw = width_to_agaw(max_gaw);
	     agaw >= 0; agaw--) {
		if (test_bit(agaw, &sagaw))
			break;
	}

	return agaw;
}

/*
 * Calculate max SAGAW for each iommu.
 */
int iommu_calculate_max_sagaw(struct intel_iommu *iommu)
{
	return __iommu_calculate_agaw(iommu, MAX_AGAW_WIDTH);
}

/*
 * calculate agaw for each iommu.
 * "SAGAW" may be different across iommus, use a default agaw, and
 * get a supported less agaw for iommus that don't support the default agaw.
 */
int iommu_calculate_agaw(struct intel_iommu *iommu)
{
	return __iommu_calculate_agaw(iommu, DEFAULT_DOMAIN_ADDRESS_WIDTH);
}

/* This functionin only returns single iommu in a domain */
struct intel_iommu *domain_get_iommu(struct dmar_domain *domain)
{
	int iommu_id;

	/* si_domain and vm domain should not get here. */
	if (WARN_ON(domain->domain.type != IOMMU_DOMAIN_DMA))
		return NULL;

	for_each_domain_iommu(iommu_id, domain)
		break;

	if (iommu_id < 0 || iommu_id >= g_num_of_iommus)
		return NULL;

	return g_iommus[iommu_id];
}

static inline bool iommu_paging_structure_coherency(struct intel_iommu *iommu)
{
	return sm_supported(iommu) ?
			ecap_smpwc(iommu->ecap) : ecap_coherent(iommu->ecap);
}

static void domain_update_iommu_coherency(struct dmar_domain *domain)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	bool found = false;
	int i;

	domain->iommu_coherency = 1;

	for_each_domain_iommu(i, domain) {
		found = true;
		if (!iommu_paging_structure_coherency(g_iommus[i])) {
			domain->iommu_coherency = 0;
			break;
		}
	}
	if (found)
		return;

	/* No hardware attached; use lowest common denominator */
	rcu_read_lock();
	for_each_active_iommu(iommu, drhd) {
		if (!iommu_paging_structure_coherency(iommu)) {
			domain->iommu_coherency = 0;
			break;
		}
	}
	rcu_read_unlock();
}

static int domain_update_iommu_snooping(struct intel_iommu *skip)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	int ret = 1;

	rcu_read_lock();
	for_each_active_iommu(iommu, drhd) {
		if (iommu != skip) {
			/*
			 * If the hardware is operating in the scalable mode,
			 * the snooping control is always supported since we
			 * always set PASID-table-entry.PGSNP bit if the domain
			 * is managed outside (UNMANAGED).
			 */
			if (!sm_supported(iommu) &&
			    !ecap_sc_support(iommu->ecap)) {
				ret = 0;
				break;
			}
		}
	}
	rcu_read_unlock();

	return ret;
}

static int domain_update_iommu_superpage(struct dmar_domain *domain,
					 struct intel_iommu *skip)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	int mask = 0x3;

	if (!intel_iommu_superpage) {
		return 0;
	}

	/* set iommu_superpage to the smallest common denominator */
	rcu_read_lock();
	for_each_active_iommu(iommu, drhd) {
		if (iommu != skip) {
			if (domain && domain_use_first_level(domain)) {
				if (!cap_fl1gp_support(iommu->cap))
					mask = 0x1;
			} else {
				mask &= cap_super_page_val(iommu->cap);
			}

			if (!mask)
				break;
		}
	}
	rcu_read_unlock();

	return fls(mask);
}

static int domain_update_device_node(struct dmar_domain *domain)
{
	struct device_domain_info *info;
	int nid = NUMA_NO_NODE;

	assert_spin_locked(&device_domain_lock);

	if (list_empty(&domain->devices))
		return NUMA_NO_NODE;

	list_for_each_entry(info, &domain->devices, link) {
		if (!info->dev)
			continue;

		/*
		 * There could possibly be multiple device numa nodes as devices
		 * within the same domain may sit behind different IOMMUs. There
		 * isn't perfect answer in such situation, so we select first
		 * come first served policy.
		 */
		nid = dev_to_node(info->dev);
		if (nid != NUMA_NO_NODE)
			break;
	}

	return nid;
}

static void domain_update_iotlb(struct dmar_domain *domain);

/* Some capabilities may be different across iommus */
static void domain_update_iommu_cap(struct dmar_domain *domain)
{
	domain_update_iommu_coherency(domain);
	domain->iommu_snooping = domain_update_iommu_snooping(NULL);
	domain->iommu_superpage = domain_update_iommu_superpage(domain, NULL);

	/*
	 * If RHSA is missing, we should default to the device numa domain
	 * as fall back.
	 */
	if (domain->nid == NUMA_NO_NODE)
		domain->nid = domain_update_device_node(domain);

	/*
	 * First-level translation restricts the input-address to a
	 * canonical address (i.e., address bits 63:N have the same
	 * value as address bit [N-1], where N is 48-bits with 4-level
	 * paging and 57-bits with 5-level paging). Hence, skip bit
	 * [N-1].
	 */
	if (domain_use_first_level(domain))
		domain->domain.geometry.aperture_end = __DOMAIN_MAX_ADDR(domain->gaw - 1);
	else
		domain->domain.geometry.aperture_end = __DOMAIN_MAX_ADDR(domain->gaw);

	domain_update_iotlb(domain);
}

struct context_entry *iommu_context_addr(struct intel_iommu *iommu, u8 bus,
					 u8 devfn, int alloc)
{
	struct root_entry *root = &iommu->root_entry[bus];
	struct context_entry *context;
	u64 *entry;

	entry = &root->lo;
	if (sm_supported(iommu)) {
		if (devfn >= 0x80) {
			devfn -= 0x80;
			entry = &root->hi;
		}
		devfn *= 2;
	}
	if (*entry & 1)
		context = phys_to_virt(*entry & VTD_PAGE_MASK);
	else {
		unsigned long phy_addr;
		if (!alloc)
			return NULL;

		context = alloc_pgtable_page(iommu->node);
		if (!context)
			return NULL;

		__iommu_flush_cache(iommu, (void *)context, CONTEXT_SIZE);
		phy_addr = virt_to_phys((void *)context);
		*entry = phy_addr | 1;
		__iommu_flush_cache(iommu, entry, sizeof(*entry));
	}
	return &context[devfn];
}

static bool attach_deferred(struct device *dev)
{
	return dev_iommu_priv_get(dev) == DEFER_DEVICE_DOMAIN_INFO;
}

/**
 * is_downstream_to_pci_bridge - test if a device belongs to the PCI
 *				 sub-hierarchy of a candidate PCI-PCI bridge
 * @dev: candidate PCI device belonging to @bridge PCI sub-hierarchy
 * @bridge: the candidate PCI-PCI bridge
 *
 * Return: true if @dev belongs to @bridge PCI sub-hierarchy, else false.
 */
static bool
is_downstream_to_pci_bridge(struct device *dev, struct device *bridge)
{
	struct pci_dev *pdev, *pbridge;

	if (!dev_is_pci(dev) || !dev_is_pci(bridge))
		return false;

	pdev = to_pci_dev(dev);
	pbridge = to_pci_dev(bridge);

	if (pbridge->subordinate &&
	    pbridge->subordinate->number <= pdev->bus->number &&
	    pbridge->subordinate->busn_res.end >= pdev->bus->number)
		return true;

	return false;
}

static bool quirk_ioat_snb_local_iommu(struct pci_dev *pdev)
{
	struct dmar_drhd_unit *drhd;
	u32 vtbar;
	int rc;

	/* We know that this device on this chipset has its own IOMMU.
	 * If we find it under a different IOMMU, then the BIOS is lying
	 * to us. Hope that the IOMMU for this device is actually
	 * disabled, and it needs no translation...
	 */
	rc = pci_bus_read_config_dword(pdev->bus, PCI_DEVFN(0, 0), 0xb0, &vtbar);
	if (rc) {
		/* "can't" happen */
		dev_info(&pdev->dev, "failed to run vt-d quirk\n");
		return false;
	}
	vtbar &= 0xffff0000;

	/* we know that the this iommu should be at offset 0xa000 from vtbar */
	drhd = dmar_find_matched_drhd_unit(pdev);
	if (!drhd || drhd->reg_base_addr - vtbar != 0xa000) {
		pr_warn_once(FW_BUG "BIOS assigned incorrect VT-d unit for Intel(R) QuickData Technology device\n");
		add_taint(TAINT_FIRMWARE_WORKAROUND, LOCKDEP_STILL_OK);
		return true;
	}

	return false;
}

static bool iommu_is_dummy(struct intel_iommu *iommu, struct device *dev)
{
	if (!iommu || iommu->drhd->ignored)
		return true;

	if (dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(dev);

		if (pdev->vendor == PCI_VENDOR_ID_INTEL &&
		    pdev->device == PCI_DEVICE_ID_INTEL_IOAT_SNB &&
		    quirk_ioat_snb_local_iommu(pdev))
			return true;
	}

	return false;
}

struct intel_iommu *device_to_iommu(struct device *dev, u8 *bus, u8 *devfn)
{
	struct dmar_drhd_unit *drhd = NULL;
	struct pci_dev *pdev = NULL;
	struct intel_iommu *iommu;
	struct device *tmp;
	u16 segment = 0;
	int i;

	if (!dev)
		return NULL;

	if (dev_is_pci(dev)) {
		struct pci_dev *pf_pdev;

		pdev = pci_real_dma_dev(to_pci_dev(dev));

		/* VFs aren't listed in scope tables; we need to look up
		 * the PF instead to find the IOMMU. */
		pf_pdev = pci_physfn(pdev);
		dev = &pf_pdev->dev;
		segment = pci_domain_nr(pdev->bus);
	} else if (has_acpi_companion(dev))
		dev = &ACPI_COMPANION(dev)->dev;

	rcu_read_lock();
	for_each_iommu(iommu, drhd) {
		if (pdev && segment != drhd->segment)
			continue;

		for_each_active_dev_scope(drhd->devices,
					  drhd->devices_cnt, i, tmp) {
			if (tmp == dev) {
				/* For a VF use its original BDF# not that of the PF
				 * which we used for the IOMMU lookup. Strictly speaking
				 * we could do this for all PCI devices; we only need to
				 * get the BDF# from the scope table for ACPI matches. */
				if (pdev && pdev->is_virtfn)
					goto got_pdev;

				if (bus && devfn) {
					*bus = drhd->devices[i].bus;
					*devfn = drhd->devices[i].devfn;
				}
				goto out;
			}

			if (is_downstream_to_pci_bridge(dev, tmp))
				goto got_pdev;
		}

		if (pdev && drhd->include_all) {
		got_pdev:
			if (bus && devfn) {
				*bus = pdev->bus->number;
				*devfn = pdev->devfn;
			}
			goto out;
		}
	}
	iommu = NULL;
 out:
	if (iommu_is_dummy(iommu, dev))
		iommu = NULL;

	rcu_read_unlock();

	return iommu;
}

static void domain_flush_cache(struct dmar_domain *domain,
			       void *addr, int size)
{
	if (!domain->iommu_coherency)
		clflush_cache_range(addr, size);
}

static int device_context_mapped(struct intel_iommu *iommu, u8 bus, u8 devfn)
{
	struct context_entry *context;
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&iommu->lock, flags);
	context = iommu_context_addr(iommu, bus, devfn, 0);
	if (context)
		ret = context_present(context);
	spin_unlock_irqrestore(&iommu->lock, flags);
	return ret;
}

static void free_context_table(struct intel_iommu *iommu)
{
	int i;
	unsigned long flags;
	struct context_entry *context;

	spin_lock_irqsave(&iommu->lock, flags);
	if (!iommu->root_entry) {
		goto out;
	}
	for (i = 0; i < ROOT_ENTRY_NR; i++) {
		context = iommu_context_addr(iommu, i, 0, 0);
		if (context)
			free_pgtable_page(context);

		if (!sm_supported(iommu))
			continue;

		context = iommu_context_addr(iommu, i, 0x80, 0);
		if (context)
			free_pgtable_page(context);

	}
	free_pgtable_page(iommu->root_entry);
	iommu->root_entry = NULL;
out:
	spin_unlock_irqrestore(&iommu->lock, flags);
}

static struct dma_pte *pfn_to_dma_pte(struct dmar_domain *domain,
				      unsigned long pfn, int *target_level)
{
	struct dma_pte *parent, *pte;
	int level = agaw_to_level(domain->agaw);
	int offset;

	BUG_ON(!domain->pgd);

	if (!domain_pfn_supported(domain, pfn))
		/* Address beyond IOMMU's addressing capabilities. */
		return NULL;

	parent = domain->pgd;

	while (1) {
		void *tmp_page;

		offset = pfn_level_offset(pfn, level);
		pte = &parent[offset];
		if (!*target_level && (dma_pte_superpage(pte) || !dma_pte_present(pte)))
			break;
		if (level == *target_level)
			break;

		if (!dma_pte_present(pte)) {
			uint64_t pteval;

			tmp_page = alloc_pgtable_page(domain->nid);

			if (!tmp_page)
				return NULL;

			domain_flush_cache(domain, tmp_page, VTD_PAGE_SIZE);
			pteval = ((uint64_t)virt_to_dma_pfn(tmp_page) << VTD_PAGE_SHIFT) | DMA_PTE_READ | DMA_PTE_WRITE;
			if (domain_use_first_level(domain)) {
				pteval |= DMA_FL_PTE_XD | DMA_FL_PTE_US;
				if (domain->domain.type == IOMMU_DOMAIN_DMA)
					pteval |= DMA_FL_PTE_ACCESS;
			}
			if (cmpxchg64(&pte->val, 0ULL, pteval))
				/* Someone else set it while we were thinking; use theirs. */
				free_pgtable_page(tmp_page);
			else
				domain_flush_cache(domain, pte, sizeof(*pte));
		}
		if (level == 1)
			break;

		parent = phys_to_virt(dma_pte_addr(pte));
		level--;
	}

	if (!*target_level)
		*target_level = level;

	return pte;
}

/* return address's pte at specific level */
static struct dma_pte *dma_pfn_level_pte(struct dmar_domain *domain,
					 unsigned long pfn,
					 int level, int *large_page)
{
	struct dma_pte *parent, *pte;
	int total = agaw_to_level(domain->agaw);
	int offset;

	parent = domain->pgd;
	while (level <= total) {
		offset = pfn_level_offset(pfn, total);
		pte = &parent[offset];
		if (level == total)
			return pte;

		if (!dma_pte_present(pte)) {
			*large_page = total;
			break;
		}

		if (dma_pte_superpage(pte)) {
			*large_page = total;
			return pte;
		}

		parent = phys_to_virt(dma_pte_addr(pte));
		total--;
	}
	return NULL;
}

/* clear last level pte, a tlb flush should be followed */
static void dma_pte_clear_range(struct dmar_domain *domain,
				unsigned long start_pfn,
				unsigned long last_pfn)
{
	unsigned int large_page;
	struct dma_pte *first_pte, *pte;

	BUG_ON(!domain_pfn_supported(domain, start_pfn));
	BUG_ON(!domain_pfn_supported(domain, last_pfn));
	BUG_ON(start_pfn > last_pfn);

	/* we don't need lock here; nobody else touches the iova range */
	do {
		large_page = 1;
		first_pte = pte = dma_pfn_level_pte(domain, start_pfn, 1, &large_page);
		if (!pte) {
			start_pfn = align_to_level(start_pfn + 1, large_page + 1);
			continue;
		}
		do {
			dma_clear_pte(pte);
			start_pfn += lvl_to_nr_pages(large_page);
			pte++;
		} while (start_pfn <= last_pfn && !first_pte_in_page(pte));

		domain_flush_cache(domain, first_pte,
				   (void *)pte - (void *)first_pte);

	} while (start_pfn && start_pfn <= last_pfn);
}

static void dma_pte_free_level(struct dmar_domain *domain, int level,
			       int retain_level, struct dma_pte *pte,
			       unsigned long pfn, unsigned long start_pfn,
			       unsigned long last_pfn)
{
	pfn = max(start_pfn, pfn);
	pte = &pte[pfn_level_offset(pfn, level)];

	do {
		unsigned long level_pfn;
		struct dma_pte *level_pte;

		if (!dma_pte_present(pte) || dma_pte_superpage(pte))
			goto next;

		level_pfn = pfn & level_mask(level);
		level_pte = phys_to_virt(dma_pte_addr(pte));

		if (level > 2) {
			dma_pte_free_level(domain, level - 1, retain_level,
					   level_pte, level_pfn, start_pfn,
					   last_pfn);
		}

		/*
		 * Free the page table if we're below the level we want to
		 * retain and the range covers the entire table.
		 */
		if (level < retain_level && !(start_pfn > level_pfn ||
		      last_pfn < level_pfn + level_size(level) - 1)) {
			dma_clear_pte(pte);
			domain_flush_cache(domain, pte, sizeof(*pte));
			free_pgtable_page(level_pte);
		}
next:
		pfn += level_size(level);
	} while (!first_pte_in_page(++pte) && pfn <= last_pfn);
}

/*
 * clear last level (leaf) ptes and free page table pages below the
 * level we wish to keep intact.
 */
static void dma_pte_free_pagetable(struct dmar_domain *domain,
				   unsigned long start_pfn,
				   unsigned long last_pfn,
				   int retain_level)
{
	BUG_ON(!domain_pfn_supported(domain, start_pfn));
	BUG_ON(!domain_pfn_supported(domain, last_pfn));
	BUG_ON(start_pfn > last_pfn);

	dma_pte_clear_range(domain, start_pfn, last_pfn);

	/* We don't need lock here; nobody else touches the iova range */
	dma_pte_free_level(domain, agaw_to_level(domain->agaw), retain_level,
			   domain->pgd, 0, start_pfn, last_pfn);

	/* free pgd */
	if (start_pfn == 0 && last_pfn == DOMAIN_MAX_PFN(domain->gaw)) {
		free_pgtable_page(domain->pgd);
		domain->pgd = NULL;
	}
}

/* When a page at a given level is being unlinked from its parent, we don't
   need to *modify* it at all. All we need to do is make a list of all the
   pages which can be freed just as soon as we've flushed the IOTLB and we
   know the hardware page-walk will no longer touch them.
   The 'pte' argument is the *parent* PTE, pointing to the page that is to
   be freed. */
static struct page *dma_pte_list_pagetables(struct dmar_domain *domain,
					    int level, struct dma_pte *pte,
					    struct page *freelist)
{
	struct page *pg;

	pg = pfn_to_page(dma_pte_addr(pte) >> PAGE_SHIFT);
	pg->freelist = freelist;
	freelist = pg;

	if (level == 1)
		return freelist;

	pte = page_address(pg);
	do {
		if (dma_pte_present(pte) && !dma_pte_superpage(pte))
			freelist = dma_pte_list_pagetables(domain, level - 1,
							   pte, freelist);
		pte++;
	} while (!first_pte_in_page(pte));

	return freelist;
}

static struct page *dma_pte_clear_level(struct dmar_domain *domain, int level,
					struct dma_pte *pte, unsigned long pfn,
					unsigned long start_pfn,
					unsigned long last_pfn,
					struct page *freelist)
{
	struct dma_pte *first_pte = NULL, *last_pte = NULL;

	pfn = max(start_pfn, pfn);
	pte = &pte[pfn_level_offset(pfn, level)];

	do {
		unsigned long level_pfn;

		if (!dma_pte_present(pte))
			goto next;

		level_pfn = pfn & level_mask(level);

		/* If range covers entire pagetable, free it */
		if (start_pfn <= level_pfn &&
		    last_pfn >= level_pfn + level_size(level) - 1) {
			/* These suborbinate page tables are going away entirely. Don't
			   bother to clear them; we're just going to *free* them. */
			if (level > 1 && !dma_pte_superpage(pte))
				freelist = dma_pte_list_pagetables(domain, level - 1, pte, freelist);

			dma_clear_pte(pte);
			if (!first_pte)
				first_pte = pte;
			last_pte = pte;
		} else if (level > 1) {
			/* Recurse down into a level that isn't *entirely* obsolete */
			freelist = dma_pte_clear_level(domain, level - 1,
						       phys_to_virt(dma_pte_addr(pte)),
						       level_pfn, start_pfn, last_pfn,
						       freelist);
		}
next:
		pfn += level_size(level);
	} while (!first_pte_in_page(++pte) && pfn <= last_pfn);

	if (first_pte)
		domain_flush_cache(domain, first_pte,
				   (void *)++last_pte - (void *)first_pte);

	return freelist;
}

/* We can't just free the pages because the IOMMU may still be walking
   the page tables, and may have cached the intermediate levels. The
   pages can only be freed after the IOTLB flush has been done. */
static struct page *domain_unmap(struct dmar_domain *domain,
				 unsigned long start_pfn,
				 unsigned long last_pfn,
				 struct page *freelist)
{
	BUG_ON(!domain_pfn_supported(domain, start_pfn));
	BUG_ON(!domain_pfn_supported(domain, last_pfn));
	BUG_ON(start_pfn > last_pfn);

	/* we don't need lock here; nobody else touches the iova range */
	freelist = dma_pte_clear_level(domain, agaw_to_level(domain->agaw),
				       domain->pgd, 0, start_pfn, last_pfn,
				       freelist);

	/* free pgd */
	if (start_pfn == 0 && last_pfn == DOMAIN_MAX_PFN(domain->gaw)) {
		struct page *pgd_page = virt_to_page(domain->pgd);
		pgd_page->freelist = freelist;
		freelist = pgd_page;

		domain->pgd = NULL;
	}

	return freelist;
}

static void dma_free_pagelist(struct page *freelist)
{
	struct page *pg;

	while ((pg = freelist)) {
		freelist = pg->freelist;
		free_pgtable_page(page_address(pg));
	}
}

/* iommu handling */
static int iommu_alloc_root_entry(struct intel_iommu *iommu)
{
	struct root_entry *root;
	unsigned long flags;

	root = (struct root_entry *)alloc_pgtable_page(iommu->node);
	if (!root) {
		pr_err("Allocating root entry for %s failed\n",
			iommu->name);
		return -ENOMEM;
	}

	__iommu_flush_cache(iommu, root, ROOT_SIZE);

	spin_lock_irqsave(&iommu->lock, flags);
	iommu->root_entry = root;
	spin_unlock_irqrestore(&iommu->lock, flags);

	return 0;
}

static void iommu_set_root_entry(struct intel_iommu *iommu)
{
	u64 addr;
	u32 sts;
	unsigned long flag;

	addr = virt_to_phys(iommu->root_entry);
	if (sm_supported(iommu))
		addr |= DMA_RTADDR_SMT;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	dmar_writeq(iommu->reg + DMAR_RTADDR_REG, addr);

	writel(iommu->gcmd | DMA_GCMD_SRTP, iommu->reg + DMAR_GCMD_REG);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_GSTS_REG,
		      readl, (sts & DMA_GSTS_RTPS), sts);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);

	iommu->flush.flush_context(iommu, 0, 0, 0, DMA_CCMD_GLOBAL_INVL);
	if (sm_supported(iommu))
		qi_flush_pasid_cache(iommu, 0, QI_PC_GLOBAL, 0);
	iommu->flush.flush_iotlb(iommu, 0, 0, 0, DMA_TLB_GLOBAL_FLUSH);
}

void iommu_flush_write_buffer(struct intel_iommu *iommu)
{
	u32 val;
	unsigned long flag;

	if (!rwbf_quirk && !cap_rwbf(iommu->cap))
		return;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	writel(iommu->gcmd | DMA_GCMD_WBF, iommu->reg + DMAR_GCMD_REG);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_GSTS_REG,
		      readl, (!(val & DMA_GSTS_WBFS)), val);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
}

/* return value determine if we need a write buffer flush */
static void __iommu_flush_context(struct intel_iommu *iommu,
				  u16 did, u16 source_id, u8 function_mask,
				  u64 type)
{
	u64 val = 0;
	unsigned long flag;

	switch (type) {
	case DMA_CCMD_GLOBAL_INVL:
		val = DMA_CCMD_GLOBAL_INVL;
		break;
	case DMA_CCMD_DOMAIN_INVL:
		val = DMA_CCMD_DOMAIN_INVL|DMA_CCMD_DID(did);
		break;
	case DMA_CCMD_DEVICE_INVL:
		val = DMA_CCMD_DEVICE_INVL|DMA_CCMD_DID(did)
			| DMA_CCMD_SID(source_id) | DMA_CCMD_FM(function_mask);
		break;
	default:
		BUG();
	}
	val |= DMA_CCMD_ICC;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	dmar_writeq(iommu->reg + DMAR_CCMD_REG, val);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_CCMD_REG,
		dmar_readq, (!(val & DMA_CCMD_ICC)), val);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
}

/* return value determine if we need a write buffer flush */
static void __iommu_flush_iotlb(struct intel_iommu *iommu, u16 did,
				u64 addr, unsigned int size_order, u64 type)
{
	int tlb_offset = ecap_iotlb_offset(iommu->ecap);
	u64 val = 0, val_iva = 0;
	unsigned long flag;

	switch (type) {
	case DMA_TLB_GLOBAL_FLUSH:
		/* global flush doesn't need set IVA_REG */
		val = DMA_TLB_GLOBAL_FLUSH|DMA_TLB_IVT;
		break;
	case DMA_TLB_DSI_FLUSH:
		val = DMA_TLB_DSI_FLUSH|DMA_TLB_IVT|DMA_TLB_DID(did);
		break;
	case DMA_TLB_PSI_FLUSH:
		val = DMA_TLB_PSI_FLUSH|DMA_TLB_IVT|DMA_TLB_DID(did);
		/* IH bit is passed in as part of address */
		val_iva = size_order | addr;
		break;
	default:
		BUG();
	}
	/* Note: set drain read/write */
#if 0
	/*
	 * This is probably to be super secure.. Looks like we can
	 * ignore it without any impact.
	 */
	if (cap_read_drain(iommu->cap))
		val |= DMA_TLB_READ_DRAIN;
#endif
	if (cap_write_drain(iommu->cap))
		val |= DMA_TLB_WRITE_DRAIN;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	/* Note: Only uses first TLB reg currently */
	if (val_iva)
		dmar_writeq(iommu->reg + tlb_offset, val_iva);
	dmar_writeq(iommu->reg + tlb_offset + 8, val);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, tlb_offset + 8,
		dmar_readq, (!(val & DMA_TLB_IVT)), val);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);

	/* check IOTLB invalidation granularity */
	if (DMA_TLB_IAIG(val) == 0)
		pr_err("Flush IOTLB failed\n");
	if (DMA_TLB_IAIG(val) != DMA_TLB_IIRG(type))
		pr_debug("TLB flush request %Lx, actual %Lx\n",
			(unsigned long long)DMA_TLB_IIRG(type),
			(unsigned long long)DMA_TLB_IAIG(val));
}

static struct device_domain_info *
iommu_support_dev_iotlb (struct dmar_domain *domain, struct intel_iommu *iommu,
			 u8 bus, u8 devfn)
{
	struct device_domain_info *info;

	assert_spin_locked(&device_domain_lock);

	if (!iommu->qi)
		return NULL;

	list_for_each_entry(info, &domain->devices, link)
		if (info->iommu == iommu && info->bus == bus &&
		    info->devfn == devfn) {
			if (info->ats_supported && info->dev)
				return info;
			break;
		}

	return NULL;
}

static void domain_update_iotlb(struct dmar_domain *domain)
{
	struct device_domain_info *info;
	bool has_iotlb_device = false;

	assert_spin_locked(&device_domain_lock);

	list_for_each_entry(info, &domain->devices, link)
		if (info->ats_enabled) {
			has_iotlb_device = true;
			break;
		}

	if (!has_iotlb_device) {
		struct subdev_domain_info *sinfo;

		list_for_each_entry(sinfo, &domain->subdevices, link_domain) {
			info = get_domain_info(sinfo->pdev);
			if (info && info->ats_enabled) {
				has_iotlb_device = true;
				break;
			}
		}
	}

	domain->has_iotlb_device = has_iotlb_device;
}

static void iommu_enable_dev_iotlb(struct device_domain_info *info)
{
	struct pci_dev *pdev;

	assert_spin_locked(&device_domain_lock);

	if (!info || !dev_is_pci(info->dev))
		return;

	pdev = to_pci_dev(info->dev);
	/* For IOMMU that supports device IOTLB throttling (DIT), we assign
	 * PFSID to the invalidation desc of a VF such that IOMMU HW can gauge
	 * queue depth at PF level. If DIT is not set, PFSID will be treated as
	 * reserved, which should be set to 0.
	 */
	if (!ecap_dit(info->iommu->ecap))
		info->pfsid = 0;
	else {
		struct pci_dev *pf_pdev;

		/* pdev will be returned if device is not a vf */
		pf_pdev = pci_physfn(pdev);
		info->pfsid = pci_dev_id(pf_pdev);
	}

#ifdef CONFIG_INTEL_IOMMU_SVM
	/* The PCIe spec, in its wisdom, declares that the behaviour of
	   the device if you enable PASID support after ATS support is
	   undefined. So always enable PASID support on devices which
	   have it, even if we can't yet know if we're ever going to
	   use it. */
	if (info->pasid_supported && !pci_enable_pasid(pdev, info->pasid_supported & ~1))
		info->pasid_enabled = 1;

	if (info->pri_supported &&
	    (info->pasid_enabled ? pci_prg_resp_pasid_required(pdev) : 1)  &&
	    !pci_reset_pri(pdev) && !pci_enable_pri(pdev, 32))
		info->pri_enabled = 1;
#endif
	if (info->ats_supported && pci_ats_page_aligned(pdev) &&
	    !pci_enable_ats(pdev, VTD_PAGE_SHIFT)) {
		info->ats_enabled = 1;
		domain_update_iotlb(info->domain);
		info->ats_qdep = pci_ats_queue_depth(pdev);
	}
}

static void iommu_disable_dev_iotlb(struct device_domain_info *info)
{
	struct pci_dev *pdev;

	assert_spin_locked(&device_domain_lock);

	if (!dev_is_pci(info->dev))
		return;

	pdev = to_pci_dev(info->dev);

	if (info->ats_enabled) {
		pci_disable_ats(pdev);
		info->ats_enabled = 0;
		domain_update_iotlb(info->domain);
	}
#ifdef CONFIG_INTEL_IOMMU_SVM
	if (info->pri_enabled) {
		pci_disable_pri(pdev);
		info->pri_enabled = 0;
	}
	if (info->pasid_enabled) {
		pci_disable_pasid(pdev);
		info->pasid_enabled = 0;
	}
#endif
}

static void __iommu_flush_dev_iotlb(struct device_domain_info *info,
				    u64 addr, unsigned int mask)
{
	u16 sid, qdep;

	if (!info || !info->ats_enabled)
		return;

	sid = info->bus << 8 | info->devfn;
	qdep = info->ats_qdep;
	qi_flush_dev_iotlb(info->iommu, sid, info->pfsid,
			   qdep, addr, mask);
}

static void iommu_flush_dev_iotlb(struct dmar_domain *domain,
				  u64 addr, unsigned mask)
{
	unsigned long flags;
	struct device_domain_info *info;
	struct subdev_domain_info *sinfo;

	if (!domain->has_iotlb_device)
		return;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry(info, &domain->devices, link)
		__iommu_flush_dev_iotlb(info, addr, mask);

	list_for_each_entry(sinfo, &domain->subdevices, link_domain) {
		info = get_domain_info(sinfo->pdev);
		__iommu_flush_dev_iotlb(info, addr, mask);
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);
}

static void domain_flush_piotlb(struct intel_iommu *iommu,
				struct dmar_domain *domain,
				u64 addr, unsigned long npages, bool ih)
{
	u16 did = domain->iommu_did[iommu->seq_id];

	if (domain->default_pasid)
		qi_flush_piotlb(iommu, did, domain->default_pasid,
				addr, npages, ih);

	if (!list_empty(&domain->devices))
		qi_flush_piotlb(iommu, did, PASID_RID2PASID, addr, npages, ih);
}

static void iommu_flush_iotlb_psi(struct intel_iommu *iommu,
				  struct dmar_domain *domain,
				  unsigned long pfn, unsigned int pages,
				  int ih, int map)
{
	unsigned int mask = ilog2(__roundup_pow_of_two(pages));
	uint64_t addr = (uint64_t)pfn << VTD_PAGE_SHIFT;
	u16 did = domain->iommu_did[iommu->seq_id];

	BUG_ON(pages == 0);

	if (ih)
		ih = 1 << 6;

	if (domain_use_first_level(domain)) {
		domain_flush_piotlb(iommu, domain, addr, pages, ih);
	} else {
		/*
		 * Fallback to domain selective flush if no PSI support or
		 * the size is too big. PSI requires page size to be 2 ^ x,
		 * and the base address is naturally aligned to the size.
		 */
		if (!cap_pgsel_inv(iommu->cap) ||
		    mask > cap_max_amask_val(iommu->cap))
			iommu->flush.flush_iotlb(iommu, did, 0, 0,
							DMA_TLB_DSI_FLUSH);
		else
			iommu->flush.flush_iotlb(iommu, did, addr | ih, mask,
							DMA_TLB_PSI_FLUSH);
	}

	/*
	 * In caching mode, changes of pages from non-present to present require
	 * flush. However, device IOTLB doesn't need to be flushed in this case.
	 */
	if (!cap_caching_mode(iommu->cap) || !map)
		iommu_flush_dev_iotlb(domain, addr, mask);
}

/* Notification for newly created mappings */
static inline void __mapping_notify_one(struct intel_iommu *iommu,
					struct dmar_domain *domain,
					unsigned long pfn, unsigned int pages)
{
	/*
	 * It's a non-present to present mapping. Only flush if caching mode
	 * and second level.
	 */
	if (cap_caching_mode(iommu->cap) && !domain_use_first_level(domain))
		iommu_flush_iotlb_psi(iommu, domain, pfn, pages, 0, 1);
	else
		iommu_flush_write_buffer(iommu);
}

static void intel_flush_iotlb_all(struct iommu_domain *domain)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	int idx;

	for_each_domain_iommu(idx, dmar_domain) {
		struct intel_iommu *iommu = g_iommus[idx];
		u16 did = dmar_domain->iommu_did[iommu->seq_id];

		if (domain_use_first_level(dmar_domain))
			domain_flush_piotlb(iommu, dmar_domain, 0, -1, 0);
		else
			iommu->flush.flush_iotlb(iommu, did, 0, 0,
						 DMA_TLB_DSI_FLUSH);

		if (!cap_caching_mode(iommu->cap))
			iommu_flush_dev_iotlb(get_iommu_domain(iommu, did),
					      0, MAX_AGAW_PFN_WIDTH);
	}
}

static void iommu_disable_protect_mem_regions(struct intel_iommu *iommu)
{
	u32 pmen;
	unsigned long flags;

	if (!cap_plmr(iommu->cap) && !cap_phmr(iommu->cap))
		return;

	raw_spin_lock_irqsave(&iommu->register_lock, flags);
	pmen = readl(iommu->reg + DMAR_PMEN_REG);
	pmen &= ~DMA_PMEN_EPM;
	writel(pmen, iommu->reg + DMAR_PMEN_REG);

	/* wait for the protected region status bit to clear */
	IOMMU_WAIT_OP(iommu, DMAR_PMEN_REG,
		readl, !(pmen & DMA_PMEN_PRS), pmen);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flags);
}

static void iommu_enable_translation(struct intel_iommu *iommu)
{
	u32 sts;
	unsigned long flags;

	raw_spin_lock_irqsave(&iommu->register_lock, flags);
	iommu->gcmd |= DMA_GCMD_TE;
	writel(iommu->gcmd, iommu->reg + DMAR_GCMD_REG);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_GSTS_REG,
		      readl, (sts & DMA_GSTS_TES), sts);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flags);
}

static void iommu_disable_translation(struct intel_iommu *iommu)
{
	u32 sts;
	unsigned long flag;

	if (iommu_skip_te_disable && iommu->drhd->gfx_dedicated &&
	    (cap_read_drain(iommu->cap) || cap_write_drain(iommu->cap)))
		return;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	iommu->gcmd &= ~DMA_GCMD_TE;
	writel(iommu->gcmd, iommu->reg + DMAR_GCMD_REG);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_GSTS_REG,
		      readl, (!(sts & DMA_GSTS_TES)), sts);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
}

static int iommu_init_domains(struct intel_iommu *iommu)
{
	u32 ndomains, nlongs;
	size_t size;

	ndomains = cap_ndoms(iommu->cap);
	pr_debug("%s: Number of Domains supported <%d>\n",
		 iommu->name, ndomains);
	nlongs = BITS_TO_LONGS(ndomains);

	spin_lock_init(&iommu->lock);

	iommu->domain_ids = kcalloc(nlongs, sizeof(unsigned long), GFP_KERNEL);
	if (!iommu->domain_ids) {
		pr_err("%s: Allocating domain id array failed\n",
		       iommu->name);
		return -ENOMEM;
	}

	size = (ALIGN(ndomains, 256) >> 8) * sizeof(struct dmar_domain **);
	iommu->domains = kzalloc(size, GFP_KERNEL);

	if (iommu->domains) {
		size = 256 * sizeof(struct dmar_domain *);
		iommu->domains[0] = kzalloc(size, GFP_KERNEL);
	}

	if (!iommu->domains || !iommu->domains[0]) {
		pr_err("%s: Allocating domain array failed\n",
		       iommu->name);
		kfree(iommu->domain_ids);
		kfree(iommu->domains);
		iommu->domain_ids = NULL;
		iommu->domains    = NULL;
		return -ENOMEM;
	}

	/*
	 * If Caching mode is set, then invalid translations are tagged
	 * with domain-id 0, hence we need to pre-allocate it. We also
	 * use domain-id 0 as a marker for non-allocated domain-id, so
	 * make sure it is not used for a real domain.
	 */
	set_bit(0, iommu->domain_ids);

	/*
	 * Vt-d spec rev3.0 (section 6.2.3.1) requires that each pasid
	 * entry for first-level or pass-through translation modes should
	 * be programmed with a domain id different from those used for
	 * second-level or nested translation. We reserve a domain id for
	 * this purpose.
	 */
	if (sm_supported(iommu))
		set_bit(FLPT_DEFAULT_DID, iommu->domain_ids);

	return 0;
}

static void disable_dmar_iommu(struct intel_iommu *iommu)
{
	struct device_domain_info *info, *tmp;
	unsigned long flags;

	if (!iommu->domains || !iommu->domain_ids)
		return;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry_safe(info, tmp, &device_domain_list, global) {
		if (info->iommu != iommu)
			continue;

		if (!info->dev || !info->domain)
			continue;

		__dmar_remove_one_dev_info(info);
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);

	if (iommu->gcmd & DMA_GCMD_TE)
		iommu_disable_translation(iommu);
}

static void free_dmar_iommu(struct intel_iommu *iommu)
{
	if ((iommu->domains) && (iommu->domain_ids)) {
		int elems = ALIGN(cap_ndoms(iommu->cap), 256) >> 8;
		int i;

		for (i = 0; i < elems; i++)
			kfree(iommu->domains[i]);
		kfree(iommu->domains);
		kfree(iommu->domain_ids);
		iommu->domains = NULL;
		iommu->domain_ids = NULL;
	}

	g_iommus[iommu->seq_id] = NULL;

	/* free context mapping */
	free_context_table(iommu);

#ifdef CONFIG_INTEL_IOMMU_SVM
	if (pasid_supported(iommu)) {
		if (ecap_prs(iommu->ecap))
			intel_svm_finish_prq(iommu);
	}
	if (vccap_pasid(iommu->vccap))
		ioasid_unregister_allocator(&iommu->pasid_allocator);

#endif
}

/*
 * Check and return whether first level is used by default for
 * DMA translation.
 */
static bool first_level_by_default(void)
{
	return scalable_mode_support() && intel_cap_flts_sanity();
}

static struct dmar_domain *alloc_domain(int flags)
{
	struct dmar_domain *domain;

	domain = alloc_domain_mem();
	if (!domain)
		return NULL;

	memset(domain, 0, sizeof(*domain));
	domain->nid = NUMA_NO_NODE;
	domain->flags = flags;
	if (first_level_by_default())
		domain->flags |= DOMAIN_FLAG_USE_FIRST_LEVEL;
	domain->has_iotlb_device = false;
	INIT_LIST_HEAD(&domain->devices);
	INIT_LIST_HEAD(&domain->subdevices);

	return domain;
}

/* Must be called with iommu->lock */
static int domain_attach_iommu(struct dmar_domain *domain,
			       struct intel_iommu *iommu)
{
	unsigned long ndomains;
	int num;

	assert_spin_locked(&device_domain_lock);
	assert_spin_locked(&iommu->lock);

	domain->iommu_refcnt[iommu->seq_id] += 1;
	domain->iommu_count += 1;
	if (domain->iommu_refcnt[iommu->seq_id] == 1) {
		ndomains = cap_ndoms(iommu->cap);
		num      = find_first_zero_bit(iommu->domain_ids, ndomains);

		if (num >= ndomains) {
			pr_err("%s: No free domain ids\n", iommu->name);
			domain->iommu_refcnt[iommu->seq_id] -= 1;
			domain->iommu_count -= 1;
			return -ENOSPC;
		}

		set_bit(num, iommu->domain_ids);
		set_iommu_domain(iommu, num, domain);

		domain->iommu_did[iommu->seq_id] = num;
		domain->nid			 = iommu->node;

		domain_update_iommu_cap(domain);
	}

	return 0;
}

static int domain_detach_iommu(struct dmar_domain *domain,
			       struct intel_iommu *iommu)
{
	int num, count;

	assert_spin_locked(&device_domain_lock);
	assert_spin_locked(&iommu->lock);

	domain->iommu_refcnt[iommu->seq_id] -= 1;
	count = --domain->iommu_count;
	if (domain->iommu_refcnt[iommu->seq_id] == 0) {
		num = domain->iommu_did[iommu->seq_id];
		clear_bit(num, iommu->domain_ids);
		set_iommu_domain(iommu, num, NULL);

		domain_update_iommu_cap(domain);
		domain->iommu_did[iommu->seq_id] = 0;
	}

	return count;
}

static inline int guestwidth_to_adjustwidth(int gaw)
{
	int agaw;
	int r = (gaw - 12) % 9;

	if (r == 0)
		agaw = gaw;
	else
		agaw = gaw + 9 - r;
	if (agaw > 64)
		agaw = 64;
	return agaw;
}

static void domain_exit(struct dmar_domain *domain)
{

	/* Remove associated devices and clear attached or cached domains */
	domain_remove_dev_info(domain);

	/* destroy iovas */
	if (domain->domain.type == IOMMU_DOMAIN_DMA)
		iommu_put_dma_cookie(&domain->domain);

	if (domain->pgd) {
		struct page *freelist;

		freelist = domain_unmap(domain, 0,
					DOMAIN_MAX_PFN(domain->gaw), NULL);
		dma_free_pagelist(freelist);
	}

	free_domain_mem(domain);
}

/*
 * Get the PASID directory size for scalable mode context entry.
 * Value of X in the PDTS field of a scalable mode context entry
 * indicates PASID directory with 2^(X + 7) entries.
 */
static inline unsigned long context_get_sm_pds(struct pasid_table *table)
{
	int pds, max_pde;

	max_pde = table->max_pasid >> PASID_PDE_SHIFT;
	pds = find_first_bit((unsigned long *)&max_pde, MAX_NR_PASID_BITS);
	if (pds < 7)
		return 0;

	return pds - 7;
}

/*
 * Set the RID_PASID field of a scalable mode context entry. The
 * IOMMU hardware will use the PASID value set in this field for
 * DMA translations of DMA requests without PASID.
 */
static inline void
context_set_sm_rid2pasid(struct context_entry *context, unsigned long pasid)
{
	context->hi |= pasid & ((1 << 20) - 1);
}

/*
 * Set the DTE(Device-TLB Enable) field of a scalable mode context
 * entry.
 */
static inline void context_set_sm_dte(struct context_entry *context)
{
	context->lo |= (1 << 2);
}

/*
 * Set the PRE(Page Request Enable) field of a scalable mode context
 * entry.
 */
static inline void context_set_sm_pre(struct context_entry *context)
{
	context->lo |= (1 << 4);
}

/* Convert value to context PASID directory size field coding. */
#define context_pdts(pds)	(((pds) & 0x7) << 9)

static int domain_context_mapping_one(struct dmar_domain *domain,
				      struct intel_iommu *iommu,
				      struct pasid_table *table,
				      u8 bus, u8 devfn)
{
	u16 did = domain->iommu_did[iommu->seq_id];
	int translation = CONTEXT_TT_MULTI_LEVEL;
	struct device_domain_info *info = NULL;
	struct context_entry *context;
	unsigned long flags;
	int ret;

	WARN_ON(did == 0);

	if (hw_pass_through && domain_type_is_si(domain))
		translation = CONTEXT_TT_PASS_THROUGH;

	pr_debug("Set context mapping for %02x:%02x.%d\n",
		bus, PCI_SLOT(devfn), PCI_FUNC(devfn));

	BUG_ON(!domain->pgd);

	spin_lock_irqsave(&device_domain_lock, flags);
	spin_lock(&iommu->lock);

	ret = -ENOMEM;
	context = iommu_context_addr(iommu, bus, devfn, 1);
	if (!context)
		goto out_unlock;

	ret = 0;
	if (context_present(context))
		goto out_unlock;

	/*
	 * For kdump cases, old valid entries may be cached due to the
	 * in-flight DMA and copied pgtable, but there is no unmapping
	 * behaviour for them, thus we need an explicit cache flush for
	 * the newly-mapped device. For kdump, at this point, the device
	 * is supposed to finish reset at its driver probe stage, so no
	 * in-flight DMA will exist, and we don't need to worry anymore
	 * hereafter.
	 */
	if (context_copied(context)) {
		u16 did_old = context_domain_id(context);

		if (did_old < cap_ndoms(iommu->cap)) {
			iommu->flush.flush_context(iommu, did_old,
						   (((u16)bus) << 8) | devfn,
						   DMA_CCMD_MASK_NOBIT,
						   DMA_CCMD_DEVICE_INVL);
			iommu->flush.flush_iotlb(iommu, did_old, 0, 0,
						 DMA_TLB_DSI_FLUSH);
		}
	}

	context_clear_entry(context);

	if (sm_supported(iommu)) {
		unsigned long pds;

		WARN_ON(!table);

		/* Setup the PASID DIR pointer: */
		pds = context_get_sm_pds(table);
		context->lo = (u64)virt_to_phys(table->table) |
				context_pdts(pds);

		/* Setup the RID_PASID field: */
		context_set_sm_rid2pasid(context, PASID_RID2PASID);

		/*
		 * Setup the Device-TLB enable bit and Page request
		 * Enable bit:
		 */
		info = iommu_support_dev_iotlb(domain, iommu, bus, devfn);
		if (info && info->ats_supported)
			context_set_sm_dte(context);
		if (info && info->pri_supported)
			context_set_sm_pre(context);
	} else {
		struct dma_pte *pgd = domain->pgd;
		int agaw;

		context_set_domain_id(context, did);

		if (translation != CONTEXT_TT_PASS_THROUGH) {
			/*
			 * Skip top levels of page tables for iommu which has
			 * less agaw than default. Unnecessary for PT mode.
			 */
			for (agaw = domain->agaw; agaw > iommu->agaw; agaw--) {
				ret = -ENOMEM;
				pgd = phys_to_virt(dma_pte_addr(pgd));
				if (!dma_pte_present(pgd))
					goto out_unlock;
			}

			info = iommu_support_dev_iotlb(domain, iommu, bus, devfn);
			if (info && info->ats_supported)
				translation = CONTEXT_TT_DEV_IOTLB;
			else
				translation = CONTEXT_TT_MULTI_LEVEL;

			context_set_address_root(context, virt_to_phys(pgd));
			context_set_address_width(context, agaw);
		} else {
			/*
			 * In pass through mode, AW must be programmed to
			 * indicate the largest AGAW value supported by
			 * hardware. And ASR is ignored by hardware.
			 */
			context_set_address_width(context, iommu->msagaw);
		}

		context_set_translation_type(context, translation);
	}

	context_set_fault_enable(context);
	context_set_present(context);
	if (!ecap_coherent(iommu->ecap))
		clflush_cache_range(context, sizeof(*context));

	/*
	 * It's a non-present to present mapping. If hardware doesn't cache
	 * non-present entry we only need to flush the write-buffer. If the
	 * _does_ cache non-present entries, then it does so in the special
	 * domain #0, which we have to flush:
	 */
	if (cap_caching_mode(iommu->cap)) {
		iommu->flush.flush_context(iommu, 0,
					   (((u16)bus) << 8) | devfn,
					   DMA_CCMD_MASK_NOBIT,
					   DMA_CCMD_DEVICE_INVL);
		iommu->flush.flush_iotlb(iommu, did, 0, 0, DMA_TLB_DSI_FLUSH);
	} else {
		iommu_flush_write_buffer(iommu);
	}
	iommu_enable_dev_iotlb(info);

	ret = 0;

out_unlock:
	spin_unlock(&iommu->lock);
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return ret;
}

struct domain_context_mapping_data {
	struct dmar_domain *domain;
	struct intel_iommu *iommu;
	struct pasid_table *table;
};

static int domain_context_mapping_cb(struct pci_dev *pdev,
				     u16 alias, void *opaque)
{
	struct domain_context_mapping_data *data = opaque;

	return domain_context_mapping_one(data->domain, data->iommu,
					  data->table, PCI_BUS_NUM(alias),
					  alias & 0xff);
}

static int
domain_context_mapping(struct dmar_domain *domain, struct device *dev)
{
	struct domain_context_mapping_data data;
	struct pasid_table *table;
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	table = intel_pasid_get_table(dev);

	if (!dev_is_pci(dev))
		return domain_context_mapping_one(domain, iommu, table,
						  bus, devfn);

	data.domain = domain;
	data.iommu = iommu;
	data.table = table;

	return pci_for_each_dma_alias(to_pci_dev(dev),
				      &domain_context_mapping_cb, &data);
}

static int domain_context_mapped_cb(struct pci_dev *pdev,
				    u16 alias, void *opaque)
{
	struct intel_iommu *iommu = opaque;

	return !device_context_mapped(iommu, PCI_BUS_NUM(alias), alias & 0xff);
}

static int domain_context_mapped(struct device *dev)
{
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	if (!dev_is_pci(dev))
		return device_context_mapped(iommu, bus, devfn);

	return !pci_for_each_dma_alias(to_pci_dev(dev),
				       domain_context_mapped_cb, iommu);
}

/* Returns a number of VTD pages, but aligned to MM page size */
static inline unsigned long aligned_nrpages(unsigned long host_addr,
					    size_t size)
{
	host_addr &= ~PAGE_MASK;
	return PAGE_ALIGN(host_addr + size) >> VTD_PAGE_SHIFT;
}

/* Return largest possible superpage level for a given mapping */
static inline int hardware_largepage_caps(struct dmar_domain *domain,
					  unsigned long iov_pfn,
					  unsigned long phy_pfn,
					  unsigned long pages)
{
	int support, level = 1;
	unsigned long pfnmerge;

	support = domain->iommu_superpage;

	/* To use a large page, the virtual *and* physical addresses
	   must be aligned to 2MiB/1GiB/etc. Lower bits set in either
	   of them will mean we have to use smaller pages. So just
	   merge them and check both at once. */
	pfnmerge = iov_pfn | phy_pfn;

	while (support && !(pfnmerge & ~VTD_STRIDE_MASK)) {
		pages >>= VTD_STRIDE_SHIFT;
		if (!pages)
			break;
		pfnmerge >>= VTD_STRIDE_SHIFT;
		level++;
		support--;
	}
	return level;
}

/*
 * Ensure that old small page tables are removed to make room for superpage(s).
 * We're going to add new large pages, so make sure we don't remove their parent
 * tables. The IOTLB/devTLBs should be flushed if any PDE/PTEs are cleared.
 */
static void switch_to_super_page(struct dmar_domain *domain,
				 unsigned long start_pfn,
				 unsigned long end_pfn, int level)
{
	unsigned long lvl_pages = lvl_to_nr_pages(level);
	struct dma_pte *pte = NULL;
	int i;

	while (start_pfn <= end_pfn) {
		if (!pte)
			pte = pfn_to_dma_pte(domain, start_pfn, &level);

		if (dma_pte_present(pte)) {
			dma_pte_free_pagetable(domain, start_pfn,
					       start_pfn + lvl_pages - 1,
					       level + 1);

			for_each_domain_iommu(i, domain)
				iommu_flush_iotlb_psi(g_iommus[i], domain,
						      start_pfn, lvl_pages,
						      0, 0);
		}

		pte++;
		start_pfn += lvl_pages;
		if (first_pte_in_page(pte))
			pte = NULL;
	}
}

static int
__domain_mapping(struct dmar_domain *domain, unsigned long iov_pfn,
		 unsigned long phys_pfn, unsigned long nr_pages, int prot)
{
	unsigned int largepage_lvl = 0;
	unsigned long lvl_pages = 0;
	struct dma_pte *pte = NULL;
	phys_addr_t pteval;
	u64 attr;

	BUG_ON(!domain_pfn_supported(domain, iov_pfn + nr_pages - 1));

	if ((prot & (DMA_PTE_READ|DMA_PTE_WRITE)) == 0)
		return -EINVAL;

	attr = prot & (DMA_PTE_READ | DMA_PTE_WRITE | DMA_PTE_SNP);
	attr |= DMA_FL_PTE_PRESENT;
	if (domain_use_first_level(domain)) {
		attr |= DMA_FL_PTE_XD | DMA_FL_PTE_US;

		if (domain->domain.type == IOMMU_DOMAIN_DMA) {
			attr |= DMA_FL_PTE_ACCESS;
			if (prot & DMA_PTE_WRITE)
				attr |= DMA_FL_PTE_DIRTY;
		}
	}

	pteval = ((phys_addr_t)phys_pfn << VTD_PAGE_SHIFT) | attr;

	while (nr_pages > 0) {
		uint64_t tmp;

		if (!pte) {
			largepage_lvl = hardware_largepage_caps(domain, iov_pfn,
					phys_pfn, nr_pages);

			pte = pfn_to_dma_pte(domain, iov_pfn, &largepage_lvl);
			if (!pte)
				return -ENOMEM;
			/* It is large page*/
			if (largepage_lvl > 1) {
				unsigned long end_pfn;

				pteval |= DMA_PTE_LARGE_PAGE;
				end_pfn = ((iov_pfn + nr_pages) & level_mask(largepage_lvl)) - 1;
				switch_to_super_page(domain, iov_pfn, end_pfn, largepage_lvl);
			} else {
				pteval &= ~(uint64_t)DMA_PTE_LARGE_PAGE;
			}

		}
		/* We don't need lock here, nobody else
		 * touches the iova range
		 */
		tmp = cmpxchg64_local(&pte->val, 0ULL, pteval);
		if (tmp) {
			static int dumps = 5;
			pr_crit("ERROR: DMA PTE for vPFN 0x%lx already set (to %llx not %llx)\n",
				iov_pfn, tmp, (unsigned long long)pteval);
			if (dumps) {
				dumps--;
				debug_dma_dump_mappings(NULL);
			}
			WARN_ON(1);
		}

		lvl_pages = lvl_to_nr_pages(largepage_lvl);

		BUG_ON(nr_pages < lvl_pages);

		nr_pages -= lvl_pages;
		iov_pfn += lvl_pages;
		phys_pfn += lvl_pages;
		pteval += lvl_pages * VTD_PAGE_SIZE;

		/* If the next PTE would be the first in a new page, then we
		 * need to flush the cache on the entries we've just written.
		 * And then we'll need to recalculate 'pte', so clear it and
		 * let it get set again in the if (!pte) block above.
		 *
		 * If we're done (!nr_pages) we need to flush the cache too.
		 *
		 * Also if we've been setting superpages, we may need to
		 * recalculate 'pte' and switch back to smaller pages for the
		 * end of the mapping, if the trailing size is not enough to
		 * use another superpage (i.e. nr_pages < lvl_pages).
		 *
		 * We leave clflush for the leaf pte changes to iotlb_sync_map()
		 * callback.
		 */
		pte++;
		if (!nr_pages || first_pte_in_page(pte) ||
		    (largepage_lvl > 1 && nr_pages < lvl_pages))
			pte = NULL;
	}

	return 0;
}

static void domain_context_clear_one(struct intel_iommu *iommu, u8 bus, u8 devfn)
{
	unsigned long flags;
	struct context_entry *context;
	u16 did_old;

	if (!iommu)
		return;

	spin_lock_irqsave(&iommu->lock, flags);
	context = iommu_context_addr(iommu, bus, devfn, 0);
	if (!context) {
		spin_unlock_irqrestore(&iommu->lock, flags);
		return;
	}
	did_old = context_domain_id(context);
	context_clear_entry(context);
	__iommu_flush_cache(iommu, context, sizeof(*context));
	spin_unlock_irqrestore(&iommu->lock, flags);
	iommu->flush.flush_context(iommu,
				   did_old,
				   (((u16)bus) << 8) | devfn,
				   DMA_CCMD_MASK_NOBIT,
				   DMA_CCMD_DEVICE_INVL);

	if (sm_supported(iommu))
		qi_flush_pasid_cache(iommu, did_old, QI_PC_ALL_PASIDS, 0);

	iommu->flush.flush_iotlb(iommu,
				 did_old,
				 0,
				 0,
				 DMA_TLB_DSI_FLUSH);
}

static inline void unlink_domain_info(struct device_domain_info *info)
{
	assert_spin_locked(&device_domain_lock);
	list_del(&info->link);
	list_del(&info->global);
	if (info->dev)
		dev_iommu_priv_set(info->dev, NULL);
}

static void domain_remove_dev_info(struct dmar_domain *domain)
{
	struct device_domain_info *info, *tmp;
	unsigned long flags;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry_safe(info, tmp, &domain->devices, link)
		__dmar_remove_one_dev_info(info);
	spin_unlock_irqrestore(&device_domain_lock, flags);
}

struct dmar_domain *find_domain(struct device *dev)
{
	struct device_domain_info *info;

	if (unlikely(!dev || !dev->iommu))
		return NULL;

	if (unlikely(attach_deferred(dev)))
		return NULL;

	/* No lock here, assumes no domain exit in normal case */
	info = get_domain_info(dev);
	if (likely(info))
		return info->domain;

	return NULL;
}

static inline struct device_domain_info *
dmar_search_domain_by_dev_info(int segment, int bus, int devfn)
{
	struct device_domain_info *info;

	list_for_each_entry(info, &device_domain_list, global)
		if (info->segment == segment && info->bus == bus &&
		    info->devfn == devfn)
			return info;

	return NULL;
}

static int domain_setup_first_level(struct intel_iommu *iommu,
				    struct dmar_domain *domain,
				    struct device *dev,
				    u32 pasid)
{
	struct dma_pte *pgd = domain->pgd;
	int agaw, level;
	int flags = 0;

	/*
	 * Skip top levels of page tables for iommu which has
	 * less agaw than default. Unnecessary for PT mode.
	 */
	for (agaw = domain->agaw; agaw > iommu->agaw; agaw--) {
		pgd = phys_to_virt(dma_pte_addr(pgd));
		if (!dma_pte_present(pgd))
			return -ENOMEM;
	}

	level = agaw_to_level(agaw);
	if (level != 4 && level != 5)
		return -EINVAL;

	if (pasid != PASID_RID2PASID)
		flags |= PASID_FLAG_SUPERVISOR_MODE;
	if (level == 5)
		flags |= PASID_FLAG_FL5LP;

	if (domain->domain.type == IOMMU_DOMAIN_UNMANAGED)
		flags |= PASID_FLAG_PAGE_SNOOP;

	return intel_pasid_setup_first_level(iommu, dev, (pgd_t *)pgd, pasid,
					     domain->iommu_did[iommu->seq_id],
					     flags);
}

static bool dev_is_real_dma_subdevice(struct device *dev)
{
	return dev && dev_is_pci(dev) &&
	       pci_real_dma_dev(to_pci_dev(dev)) != to_pci_dev(dev);
}

static struct dmar_domain *dmar_insert_one_dev_info(struct intel_iommu *iommu,
						    int bus, int devfn,
						    struct device *dev,
						    struct dmar_domain *domain)
{
	struct dmar_domain *found = NULL;
	struct device_domain_info *info;
	unsigned long flags;
	int ret;

	info = alloc_devinfo_mem();
	if (!info)
		return NULL;

	if (!dev_is_real_dma_subdevice(dev)) {
		info->bus = bus;
		info->devfn = devfn;
		info->segment = iommu->segment;
	} else {
		struct pci_dev *pdev = to_pci_dev(dev);

		info->bus = pdev->bus->number;
		info->devfn = pdev->devfn;
		info->segment = pci_domain_nr(pdev->bus);
	}

	info->ats_supported = info->pasid_supported = info->pri_supported = 0;
	info->ats_enabled = info->pasid_enabled = info->pri_enabled = 0;
	info->ats_qdep = 0;
	info->dev = dev;
	info->domain = domain;
	info->iommu = iommu;
	info->pasid_table = NULL;
	info->auxd_enabled = 0;
	INIT_LIST_HEAD(&info->subdevices);

	if (dev && dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(info->dev);

		if (ecap_dev_iotlb_support(iommu->ecap) &&
		    pci_ats_supported(pdev) &&
		    dmar_find_matched_atsr_unit(pdev))
			info->ats_supported = 1;

		if (sm_supported(iommu)) {
			if (pasid_supported(iommu)) {
				int features = pci_pasid_features(pdev);
				if (features >= 0)
					info->pasid_supported = features | 1;
			}

			if (info->ats_supported && ecap_prs(iommu->ecap) &&
			    pci_pri_supported(pdev))
				info->pri_supported = 1;
		}
	}

	spin_lock_irqsave(&device_domain_lock, flags);
	if (dev)
		found = find_domain(dev);

	if (!found) {
		struct device_domain_info *info2;
		info2 = dmar_search_domain_by_dev_info(info->segment, info->bus,
						       info->devfn);
		if (info2) {
			found      = info2->domain;
			info2->dev = dev;
		}
	}

	if (found) {
		spin_unlock_irqrestore(&device_domain_lock, flags);
		free_devinfo_mem(info);
		/* Caller must free the original domain */
		return found;
	}

	spin_lock(&iommu->lock);
	ret = domain_attach_iommu(domain, iommu);
	spin_unlock(&iommu->lock);

	if (ret) {
		spin_unlock_irqrestore(&device_domain_lock, flags);
		free_devinfo_mem(info);
		return NULL;
	}

	list_add(&info->link, &domain->devices);
	list_add(&info->global, &device_domain_list);
	if (dev)
		dev_iommu_priv_set(dev, info);
	spin_unlock_irqrestore(&device_domain_lock, flags);

	/* PASID table is mandatory for a PCI device in scalable mode. */
	if (dev && dev_is_pci(dev) && sm_supported(iommu)) {
		ret = intel_pasid_alloc_table(dev);
		if (ret) {
			dev_err(dev, "PASID table allocation failed\n");
			dmar_remove_one_dev_info(dev);
			return NULL;
		}

		/* Setup the PASID entry for requests without PASID: */
		spin_lock_irqsave(&iommu->lock, flags);
		if (hw_pass_through && domain_type_is_si(domain))
			ret = intel_pasid_setup_pass_through(iommu, domain,
					dev, PASID_RID2PASID);
		else if (domain_use_first_level(domain))
			ret = domain_setup_first_level(iommu, domain, dev,
					PASID_RID2PASID);
		else
			ret = intel_pasid_setup_second_level(iommu, domain,
					dev, PASID_RID2PASID);
		spin_unlock_irqrestore(&iommu->lock, flags);
		if (ret) {
			dev_err(dev, "Setup RID2PASID failed\n");
			dmar_remove_one_dev_info(dev);
			return NULL;
		}
	}

	if (dev && domain_context_mapping(domain, dev)) {
		dev_err(dev, "Domain context map failed\n");
		dmar_remove_one_dev_info(dev);
		return NULL;
	}

	return domain;
}

static int iommu_domain_identity_map(struct dmar_domain *domain,
				     unsigned long first_vpfn,
				     unsigned long last_vpfn)
{
	/*
	 * RMRR range might have overlap with physical memory range,
	 * clear it first
	 */
	dma_pte_clear_range(domain, first_vpfn, last_vpfn);

	return __domain_mapping(domain, first_vpfn,
				first_vpfn, last_vpfn - first_vpfn + 1,
				DMA_PTE_READ|DMA_PTE_WRITE);
}

static int md_domain_init(struct dmar_domain *domain, int guest_width);

static int __init si_domain_init(int hw)
{
	struct dmar_rmrr_unit *rmrr;
	struct device *dev;
	int i, nid, ret;

	si_domain = alloc_domain(DOMAIN_FLAG_STATIC_IDENTITY);
	if (!si_domain)
		return -EFAULT;

	if (md_domain_init(si_domain, DEFAULT_DOMAIN_ADDRESS_WIDTH)) {
		domain_exit(si_domain);
		return -EFAULT;
	}

	if (hw)
		return 0;

	for_each_online_node(nid) {
		unsigned long start_pfn, end_pfn;
		int i;

		for_each_mem_pfn_range(i, nid, &start_pfn, &end_pfn, NULL) {
			ret = iommu_domain_identity_map(si_domain,
					mm_to_dma_pfn(start_pfn),
					mm_to_dma_pfn(end_pfn));
			if (ret)
				return ret;
		}
	}

	/*
	 * Identity map the RMRRs so that devices with RMRRs could also use
	 * the si_domain.
	 */
	for_each_rmrr_units(rmrr) {
		for_each_active_dev_scope(rmrr->devices, rmrr->devices_cnt,
					  i, dev) {
			unsigned long long start = rmrr->base_address;
			unsigned long long end = rmrr->end_address;

			if (WARN_ON(end < start ||
				    end >> agaw_to_width(si_domain->agaw)))
				continue;

			ret = iommu_domain_identity_map(si_domain,
					mm_to_dma_pfn(start >> PAGE_SHIFT),
					mm_to_dma_pfn(end >> PAGE_SHIFT));
			if (ret)
				return ret;
		}
	}

	return 0;
}

static int domain_add_dev_info(struct dmar_domain *domain, struct device *dev)
{
	struct dmar_domain *ndomain;
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	ndomain = dmar_insert_one_dev_info(iommu, bus, devfn, dev, domain);
	if (ndomain != domain)
		return -EBUSY;

	return 0;
}

static bool device_has_rmrr(struct device *dev)
{
	struct dmar_rmrr_unit *rmrr;
	struct device *tmp;
	int i;

	rcu_read_lock();
	for_each_rmrr_units(rmrr) {
		/*
		 * Return TRUE if this RMRR contains the device that
		 * is passed in.
		 */
		for_each_active_dev_scope(rmrr->devices,
					  rmrr->devices_cnt, i, tmp)
			if (tmp == dev ||
			    is_downstream_to_pci_bridge(dev, tmp)) {
				rcu_read_unlock();
				return true;
			}
	}
	rcu_read_unlock();
	return false;
}

/**
 * device_rmrr_is_relaxable - Test whether the RMRR of this device
 * is relaxable (ie. is allowed to be not enforced under some conditions)
 * @dev: device handle
 *
 * We assume that PCI USB devices with RMRRs have them largely
 * for historical reasons and that the RMRR space is not actively used post
 * boot.  This exclusion may change if vendors begin to abuse it.
 *
 * The same exception is made for graphics devices, with the requirement that
 * any use of the RMRR regions will be torn down before assigning the device
 * to a guest.
 *
 * Return: true if the RMRR is relaxable, false otherwise
 */
static bool device_rmrr_is_relaxable(struct device *dev)
{
	struct pci_dev *pdev;

	if (!dev_is_pci(dev))
		return false;

	pdev = to_pci_dev(dev);
	if (IS_USB_DEVICE(pdev) || IS_GFX_DEVICE(pdev))
		return true;
	else
		return false;
}

/*
 * There are a couple cases where we need to restrict the functionality of
 * devices associated with RMRRs.  The first is when evaluating a device for
 * identity mapping because problems exist when devices are moved in and out
 * of domains and their respective RMRR information is lost.  This means that
 * a device with associated RMRRs will never be in a "passthrough" domain.
 * The second is use of the device through the IOMMU API.  This interface
 * expects to have full control of the IOVA space for the device.  We cannot
 * satisfy both the requirement that RMRR access is maintained and have an
 * unencumbered IOVA space.  We also have no ability to quiesce the device's
 * use of the RMRR space or even inform the IOMMU API user of the restriction.
 * We therefore prevent devices associated with an RMRR from participating in
 * the IOMMU API, which eliminates them from device assignment.
 *
 * In both cases, devices which have relaxable RMRRs are not concerned by this
 * restriction. See device_rmrr_is_relaxable comment.
 */
static bool device_is_rmrr_locked(struct device *dev)
{
	if (!device_has_rmrr(dev))
		return false;

	if (device_rmrr_is_relaxable(dev))
		return false;

	return true;
}

/*
 * Return the required default domain type for a specific device.
 *
 * @dev: the device in query
 * @startup: true if this is during early boot
 *
 * Returns:
 *  - IOMMU_DOMAIN_DMA: device requires a dynamic mapping domain
 *  - IOMMU_DOMAIN_IDENTITY: device requires an identical mapping domain
 *  - 0: both identity and dynamic domains work for this device
 */
static int device_def_domain_type(struct device *dev)
{
	if (dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(dev);

		if ((iommu_identity_mapping & IDENTMAP_AZALIA) && IS_AZALIA(pdev))
			return IOMMU_DOMAIN_IDENTITY;

		if ((iommu_identity_mapping & IDENTMAP_GFX) && IS_GFX_DEVICE(pdev))
			return IOMMU_DOMAIN_IDENTITY;
	}

	return 0;
}

static void intel_iommu_init_qi(struct intel_iommu *iommu)
{
	/*
	 * Start from the sane iommu hardware state.
	 * If the queued invalidation is already initialized by us
	 * (for example, while enabling interrupt-remapping) then
	 * we got the things already rolling from a sane state.
	 */
	if (!iommu->qi) {
		/*
		 * Clear any previous faults.
		 */
		dmar_fault(-1, iommu);
		/*
		 * Disable queued invalidation if supported and already enabled
		 * before OS handover.
		 */
		dmar_disable_qi(iommu);
	}

	if (dmar_enable_qi(iommu)) {
		/*
		 * Queued Invalidate not enabled, use Register Based Invalidate
		 */
		iommu->flush.flush_context = __iommu_flush_context;
		iommu->flush.flush_iotlb = __iommu_flush_iotlb;
		pr_info("%s: Using Register based invalidation\n",
			iommu->name);
	} else {
		iommu->flush.flush_context = qi_flush_context;
		iommu->flush.flush_iotlb = qi_flush_iotlb;
		pr_info("%s: Using Queued invalidation\n", iommu->name);
	}
}

static int copy_context_table(struct intel_iommu *iommu,
			      struct root_entry *old_re,
			      struct context_entry **tbl,
			      int bus, bool ext)
{
	int tbl_idx, pos = 0, idx, devfn, ret = 0, did;
	struct context_entry *new_ce = NULL, ce;
	struct context_entry *old_ce = NULL;
	struct root_entry re;
	phys_addr_t old_ce_phys;

	tbl_idx = ext ? bus * 2 : bus;
	memcpy(&re, old_re, sizeof(re));

	for (devfn = 0; devfn < 256; devfn++) {
		/* First calculate the correct index */
		idx = (ext ? devfn * 2 : devfn) % 256;

		if (idx == 0) {
			/* First save what we may have and clean up */
			if (new_ce) {
				tbl[tbl_idx] = new_ce;
				__iommu_flush_cache(iommu, new_ce,
						    VTD_PAGE_SIZE);
				pos = 1;
			}

			if (old_ce)
				memunmap(old_ce);

			ret = 0;
			if (devfn < 0x80)
				old_ce_phys = root_entry_lctp(&re);
			else
				old_ce_phys = root_entry_uctp(&re);

			if (!old_ce_phys) {
				if (ext && devfn == 0) {
					/* No LCTP, try UCTP */
					devfn = 0x7f;
					continue;
				} else {
					goto out;
				}
			}

			ret = -ENOMEM;
			old_ce = memremap(old_ce_phys, PAGE_SIZE,
					MEMREMAP_WB);
			if (!old_ce)
				goto out;

			new_ce = alloc_pgtable_page(iommu->node);
			if (!new_ce)
				goto out_unmap;

			ret = 0;
		}

		/* Now copy the context entry */
		memcpy(&ce, old_ce + idx, sizeof(ce));

		if (!__context_present(&ce))
			continue;

		did = context_domain_id(&ce);
		if (did >= 0 && did < cap_ndoms(iommu->cap))
			set_bit(did, iommu->domain_ids);

		/*
		 * We need a marker for copied context entries. This
		 * marker needs to work for the old format as well as
		 * for extended context entries.
		 *
		 * Bit 67 of the context entry is used. In the old
		 * format this bit is available to software, in the
		 * extended format it is the PGE bit, but PGE is ignored
		 * by HW if PASIDs are disabled (and thus still
		 * available).
		 *
		 * So disable PASIDs first and then mark the entry
		 * copied. This means that we don't copy PASID
		 * translations from the old kernel, but this is fine as
		 * faults there are not fatal.
		 */
		context_clear_pasid_enable(&ce);
		context_set_copied(&ce);

		new_ce[idx] = ce;
	}

	tbl[tbl_idx + pos] = new_ce;

	__iommu_flush_cache(iommu, new_ce, VTD_PAGE_SIZE);

out_unmap:
	memunmap(old_ce);

out:
	return ret;
}

static int copy_translation_tables(struct intel_iommu *iommu)
{
	struct context_entry **ctxt_tbls;
	struct root_entry *old_rt;
	phys_addr_t old_rt_phys;
	int ctxt_table_entries;
	unsigned long flags;
	u64 rtaddr_reg;
	int bus, ret;
	bool new_ext, ext;

	rtaddr_reg = dmar_readq(iommu->reg + DMAR_RTADDR_REG);
	ext        = !!(rtaddr_reg & DMA_RTADDR_RTT);
	new_ext    = !!ecap_ecs(iommu->ecap);

	/*
	 * The RTT bit can only be changed when translation is disabled,
	 * but disabling translation means to open a window for data
	 * corruption. So bail out and don't copy anything if we would
	 * have to change the bit.
	 */
	if (new_ext != ext)
		return -EINVAL;

	old_rt_phys = rtaddr_reg & VTD_PAGE_MASK;
	if (!old_rt_phys)
		return -EINVAL;

	old_rt = memremap(old_rt_phys, PAGE_SIZE, MEMREMAP_WB);
	if (!old_rt)
		return -ENOMEM;

	/* This is too big for the stack - allocate it from slab */
	ctxt_table_entries = ext ? 512 : 256;
	ret = -ENOMEM;
	ctxt_tbls = kcalloc(ctxt_table_entries, sizeof(void *), GFP_KERNEL);
	if (!ctxt_tbls)
		goto out_unmap;

	for (bus = 0; bus < 256; bus++) {
		ret = copy_context_table(iommu, &old_rt[bus],
					 ctxt_tbls, bus, ext);
		if (ret) {
			pr_err("%s: Failed to copy context table for bus %d\n",
				iommu->name, bus);
			continue;
		}
	}

	spin_lock_irqsave(&iommu->lock, flags);

	/* Context tables are copied, now write them to the root_entry table */
	for (bus = 0; bus < 256; bus++) {
		int idx = ext ? bus * 2 : bus;
		u64 val;

		if (ctxt_tbls[idx]) {
			val = virt_to_phys(ctxt_tbls[idx]) | 1;
			iommu->root_entry[bus].lo = val;
		}

		if (!ext || !ctxt_tbls[idx + 1])
			continue;

		val = virt_to_phys(ctxt_tbls[idx + 1]) | 1;
		iommu->root_entry[bus].hi = val;
	}

	spin_unlock_irqrestore(&iommu->lock, flags);

	kfree(ctxt_tbls);

	__iommu_flush_cache(iommu, iommu->root_entry, PAGE_SIZE);

	ret = 0;

out_unmap:
	memunmap(old_rt);

	return ret;
}

#ifdef CONFIG_INTEL_IOMMU_SVM
static ioasid_t intel_vcmd_ioasid_alloc(ioasid_t min, ioasid_t max, void *data)
{
	struct intel_iommu *iommu = data;
	ioasid_t ioasid;

	if (!iommu)
		return INVALID_IOASID;
	/*
	 * VT-d virtual command interface always uses the full 20 bit
	 * PASID range. Host can partition guest PASID range based on
	 * policies but it is out of guest's control.
	 */
	if (min < PASID_MIN || max > intel_pasid_max_id)
		return INVALID_IOASID;

	if (vcmd_alloc_pasid(iommu, &ioasid))
		return INVALID_IOASID;

	return ioasid;
}

static void intel_vcmd_ioasid_free(ioasid_t ioasid, void *data)
{
	struct intel_iommu *iommu = data;

	if (!iommu)
		return;
	/*
	 * Sanity check the ioasid owner is done at upper layer, e.g. VFIO
	 * We can only free the PASID when all the devices are unbound.
	 */
	if (ioasid_find(NULL, ioasid, NULL)) {
		pr_alert("Cannot free active IOASID %d\n", ioasid);
		return;
	}
	vcmd_free_pasid(iommu, ioasid);
}

static void register_pasid_allocator(struct intel_iommu *iommu)
{
	/*
	 * If we are running in the host, no need for custom allocator
	 * in that PASIDs are allocated from the host system-wide.
	 */
	if (!cap_caching_mode(iommu->cap))
		return;

	if (!sm_supported(iommu)) {
		pr_warn("VT-d Scalable Mode not enabled, no PASID allocation\n");
		return;
	}

	/*
	 * Register a custom PASID allocator if we are running in a guest,
	 * guest PASID must be obtained via virtual command interface.
	 * There can be multiple vIOMMUs in each guest but only one allocator
	 * is active. All vIOMMU allocators will eventually be calling the same
	 * host allocator.
	 */
	if (!vccap_pasid(iommu->vccap))
		return;

	pr_info("Register custom PASID allocator\n");
	iommu->pasid_allocator.alloc = intel_vcmd_ioasid_alloc;
	iommu->pasid_allocator.free = intel_vcmd_ioasid_free;
	iommu->pasid_allocator.pdata = (void *)iommu;
	if (ioasid_register_allocator(&iommu->pasid_allocator)) {
		pr_warn("Custom PASID allocator failed, scalable mode disabled\n");
		/*
		 * Disable scalable mode on this IOMMU if there
		 * is no custom allocator. Mixing SM capable vIOMMU
		 * and non-SM vIOMMU are not supported.
		 */
		intel_iommu_sm = 0;
	}
}
#endif

static int __init init_dmars(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	int ret;

	/*
	 * for each drhd
	 *    allocate root
	 *    initialize and program root entry to not present
	 * endfor
	 */
	for_each_drhd_unit(drhd) {
		/*
		 * lock not needed as this is only incremented in the single
		 * threaded kernel __init code path all other access are read
		 * only
		 */
		if (g_num_of_iommus < DMAR_UNITS_SUPPORTED) {
			g_num_of_iommus++;
			continue;
		}
		pr_err_once("Exceeded %d IOMMUs\n", DMAR_UNITS_SUPPORTED);
	}

	/* Preallocate enough resources for IOMMU hot-addition */
	if (g_num_of_iommus < DMAR_UNITS_SUPPORTED)
		g_num_of_iommus = DMAR_UNITS_SUPPORTED;

	g_iommus = kcalloc(g_num_of_iommus, sizeof(struct intel_iommu *),
			GFP_KERNEL);
	if (!g_iommus) {
		pr_err("Allocating global iommu array failed\n");
		ret = -ENOMEM;
		goto error;
	}

	ret = intel_cap_audit(CAP_AUDIT_STATIC_DMAR, NULL);
	if (ret)
		goto free_iommu;

	for_each_iommu(iommu, drhd) {
		if (drhd->ignored) {
			iommu_disable_translation(iommu);
			continue;
		}

		/*
		 * Find the max pasid size of all IOMMU's in the system.
		 * We need to ensure the system pasid table is no bigger
		 * than the smallest supported.
		 */
		if (pasid_supported(iommu)) {
			u32 temp = 2 << ecap_pss(iommu->ecap);

			intel_pasid_max_id = min_t(u32, temp,
						   intel_pasid_max_id);
		}

		g_iommus[iommu->seq_id] = iommu;

		intel_iommu_init_qi(iommu);

		ret = iommu_init_domains(iommu);
		if (ret)
			goto free_iommu;

		init_translation_status(iommu);

		if (translation_pre_enabled(iommu) && !is_kdump_kernel()) {
			iommu_disable_translation(iommu);
			clear_translation_pre_enabled(iommu);
			pr_warn("Translation was enabled for %s but we are not in kdump mode\n",
				iommu->name);
		}

		/*
		 * TBD:
		 * we could share the same root & context tables
		 * among all IOMMU's. Need to Split it later.
		 */
		ret = iommu_alloc_root_entry(iommu);
		if (ret)
			goto free_iommu;

		if (translation_pre_enabled(iommu)) {
			pr_info("Translation already enabled - trying to copy translation structures\n");

			ret = copy_translation_tables(iommu);
			if (ret) {
				/*
				 * We found the IOMMU with translation
				 * enabled - but failed to copy over the
				 * old root-entry table. Try to proceed
				 * by disabling translation now and
				 * allocating a clean root-entry table.
				 * This might cause DMAR faults, but
				 * probably the dump will still succeed.
				 */
				pr_err("Failed to copy translation tables from previous kernel for %s\n",
				       iommu->name);
				iommu_disable_translation(iommu);
				clear_translation_pre_enabled(iommu);
			} else {
				pr_info("Copied translation tables from previous kernel for %s\n",
					iommu->name);
			}
		}

		if (!ecap_pass_through(iommu->ecap))
			hw_pass_through = 0;
		intel_svm_check(iommu);
	}

	/*
	 * Now that qi is enabled on all iommus, set the root entry and flush
	 * caches. This is required on some Intel X58 chipsets, otherwise the
	 * flush_context function will loop forever and the boot hangs.
	 */
	for_each_active_iommu(iommu, drhd) {
		iommu_flush_write_buffer(iommu);
#ifdef CONFIG_INTEL_IOMMU_SVM
		register_pasid_allocator(iommu);
#endif
		iommu_set_root_entry(iommu);
	}

#ifdef CONFIG_INTEL_IOMMU_BROKEN_GFX_WA
	dmar_map_gfx = 0;
#endif

	if (!dmar_map_gfx)
		iommu_identity_mapping |= IDENTMAP_GFX;

	check_tylersburg_isoch();

	ret = si_domain_init(hw_pass_through);
	if (ret)
		goto free_iommu;

	/*
	 * for each drhd
	 *   enable fault log
	 *   global invalidate context cache
	 *   global invalidate iotlb
	 *   enable translation
	 */
	for_each_iommu(iommu, drhd) {
		if (drhd->ignored) {
			/*
			 * we always have to disable PMRs or DMA may fail on
			 * this device
			 */
			if (force_on)
				iommu_disable_protect_mem_regions(iommu);
			continue;
		}

		iommu_flush_write_buffer(iommu);

#ifdef CONFIG_INTEL_IOMMU_SVM
		if (pasid_supported(iommu) && ecap_prs(iommu->ecap)) {
			/*
			 * Call dmar_alloc_hwirq() with dmar_global_lock held,
			 * could cause possible lock race condition.
			 */
			up_write(&dmar_global_lock);
			ret = intel_svm_enable_prq(iommu);
			down_write(&dmar_global_lock);
			if (ret)
				goto free_iommu;
		}
#endif
		ret = dmar_set_interrupt(iommu);
		if (ret)
			goto free_iommu;
	}

	return 0;

free_iommu:
	for_each_active_iommu(iommu, drhd) {
		disable_dmar_iommu(iommu);
		free_dmar_iommu(iommu);
	}

	kfree(g_iommus);

error:
	return ret;
}

static inline int iommu_domain_cache_init(void)
{
	int ret = 0;

	iommu_domain_cache = kmem_cache_create("iommu_domain",
					 sizeof(struct dmar_domain),
					 0,
					 SLAB_HWCACHE_ALIGN,

					 NULL);
	if (!iommu_domain_cache) {
		pr_err("Couldn't create iommu_domain cache\n");
		ret = -ENOMEM;
	}

	return ret;
}

static inline int iommu_devinfo_cache_init(void)
{
	int ret = 0;

	iommu_devinfo_cache = kmem_cache_create("iommu_devinfo",
					 sizeof(struct device_domain_info),
					 0,
					 SLAB_HWCACHE_ALIGN,
					 NULL);
	if (!iommu_devinfo_cache) {
		pr_err("Couldn't create devinfo cache\n");
		ret = -ENOMEM;
	}

	return ret;
}

static int __init iommu_init_mempool(void)
{
	int ret;
	ret = iova_cache_get();
	if (ret)
		return ret;

	ret = iommu_domain_cache_init();
	if (ret)
		goto domain_error;

	ret = iommu_devinfo_cache_init();
	if (!ret)
		return ret;

	kmem_cache_destroy(iommu_domain_cache);
domain_error:
	iova_cache_put();

	return -ENOMEM;
}

static void __init iommu_exit_mempool(void)
{
	kmem_cache_destroy(iommu_devinfo_cache);
	kmem_cache_destroy(iommu_domain_cache);
	iova_cache_put();
}

static void __init init_no_remapping_devices(void)
{
	struct dmar_drhd_unit *drhd;
	struct device *dev;
	int i;

	for_each_drhd_unit(drhd) {
		if (!drhd->include_all) {
			for_each_active_dev_scope(drhd->devices,
						  drhd->devices_cnt, i, dev)
				break;
			/* ignore DMAR unit if no devices exist */
			if (i == drhd->devices_cnt)
				drhd->ignored = 1;
		}
	}

	for_each_active_drhd_unit(drhd) {
		if (drhd->include_all)
			continue;

		for_each_active_dev_scope(drhd->devices,
					  drhd->devices_cnt, i, dev)
			if (!dev_is_pci(dev) || !IS_GFX_DEVICE(to_pci_dev(dev)))
				break;
		if (i < drhd->devices_cnt)
			continue;

		/* This IOMMU has *only* gfx devices. Either bypass it or
		   set the gfx_mapped flag, as appropriate */
		drhd->gfx_dedicated = 1;
		if (!dmar_map_gfx)
			drhd->ignored = 1;
	}
}

#ifdef CONFIG_SUSPEND
static int init_iommu_hw(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;

	for_each_active_iommu(iommu, drhd)
		if (iommu->qi)
			dmar_reenable_qi(iommu);

	for_each_iommu(iommu, drhd) {
		if (drhd->ignored) {
			/*
			 * we always have to disable PMRs or DMA may fail on
			 * this device
			 */
			if (force_on)
				iommu_disable_protect_mem_regions(iommu);
			continue;
		}

		iommu_flush_write_buffer(iommu);
		iommu_set_root_entry(iommu);
		iommu_enable_translation(iommu);
		iommu_disable_protect_mem_regions(iommu);
	}

	return 0;
}

static void iommu_flush_all(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;

	for_each_active_iommu(iommu, drhd) {
		iommu->flush.flush_context(iommu, 0, 0, 0,
					   DMA_CCMD_GLOBAL_INVL);
		iommu->flush.flush_iotlb(iommu, 0, 0, 0,
					 DMA_TLB_GLOBAL_FLUSH);
	}
}

static int iommu_suspend(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;
	unsigned long flag;

	for_each_active_iommu(iommu, drhd) {
		iommu->iommu_state = kcalloc(MAX_SR_DMAR_REGS, sizeof(u32),
					     GFP_KERNEL);
		if (!iommu->iommu_state)
			goto nomem;
	}

	iommu_flush_all();

	for_each_active_iommu(iommu, drhd) {
		iommu_disable_translation(iommu);

		raw_spin_lock_irqsave(&iommu->register_lock, flag);

		iommu->iommu_state[SR_DMAR_FECTL_REG] =
			readl(iommu->reg + DMAR_FECTL_REG);
		iommu->iommu_state[SR_DMAR_FEDATA_REG] =
			readl(iommu->reg + DMAR_FEDATA_REG);
		iommu->iommu_state[SR_DMAR_FEADDR_REG] =
			readl(iommu->reg + DMAR_FEADDR_REG);
		iommu->iommu_state[SR_DMAR_FEUADDR_REG] =
			readl(iommu->reg + DMAR_FEUADDR_REG);

		raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
	}
	return 0;

nomem:
	for_each_active_iommu(iommu, drhd)
		kfree(iommu->iommu_state);

	return -ENOMEM;
}

static void iommu_resume(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;
	unsigned long flag;

	if (init_iommu_hw()) {
		if (force_on)
			panic("tboot: IOMMU setup failed, DMAR can not resume!\n");
		else
			WARN(1, "IOMMU setup failed, DMAR can not resume!\n");
		return;
	}

	for_each_active_iommu(iommu, drhd) {

		raw_spin_lock_irqsave(&iommu->register_lock, flag);

		writel(iommu->iommu_state[SR_DMAR_FECTL_REG],
			iommu->reg + DMAR_FECTL_REG);
		writel(iommu->iommu_state[SR_DMAR_FEDATA_REG],
			iommu->reg + DMAR_FEDATA_REG);
		writel(iommu->iommu_state[SR_DMAR_FEADDR_REG],
			iommu->reg + DMAR_FEADDR_REG);
		writel(iommu->iommu_state[SR_DMAR_FEUADDR_REG],
			iommu->reg + DMAR_FEUADDR_REG);

		raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
	}

	for_each_active_iommu(iommu, drhd)
		kfree(iommu->iommu_state);
}

static struct syscore_ops iommu_syscore_ops = {
	.resume		= iommu_resume,
	.suspend	= iommu_suspend,
};

static void __init init_iommu_pm_ops(void)
{
	register_syscore_ops(&iommu_syscore_ops);
}

#else
static inline void init_iommu_pm_ops(void) {}
#endif	/* CONFIG_PM */

static int rmrr_sanity_check(struct acpi_dmar_reserved_memory *rmrr)
{
	if (!IS_ALIGNED(rmrr->base_address, PAGE_SIZE) ||
	    !IS_ALIGNED(rmrr->end_address + 1, PAGE_SIZE) ||
	    rmrr->end_address <= rmrr->base_address ||
	    arch_rmrr_sanity_check(rmrr))
		return -EINVAL;

	return 0;
}

int __init dmar_parse_one_rmrr(struct acpi_dmar_header *header, void *arg)
{
	struct acpi_dmar_reserved_memory *rmrr;
	struct dmar_rmrr_unit *rmrru;

	rmrr = (struct acpi_dmar_reserved_memory *)header;
	if (rmrr_sanity_check(rmrr)) {
		pr_warn(FW_BUG
			   "Your BIOS is broken; bad RMRR [%#018Lx-%#018Lx]\n"
			   "BIOS vendor: %s; Ver: %s; Product Version: %s\n",
			   rmrr->base_address, rmrr->end_address,
			   dmi_get_system_info(DMI_BIOS_VENDOR),
			   dmi_get_system_info(DMI_BIOS_VERSION),
			   dmi_get_system_info(DMI_PRODUCT_VERSION));
		add_taint(TAINT_FIRMWARE_WORKAROUND, LOCKDEP_STILL_OK);
	}

	rmrru = kzalloc(sizeof(*rmrru), GFP_KERNEL);
	if (!rmrru)
		goto out;

	rmrru->hdr = header;

	rmrru->base_address = rmrr->base_address;
	rmrru->end_address = rmrr->end_address;

	rmrru->devices = dmar_alloc_dev_scope((void *)(rmrr + 1),
				((void *)rmrr) + rmrr->header.length,
				&rmrru->devices_cnt);
	if (rmrru->devices_cnt && rmrru->devices == NULL)
		goto free_rmrru;

	list_add(&rmrru->list, &dmar_rmrr_units);

	return 0;
free_rmrru:
	kfree(rmrru);
out:
	return -ENOMEM;
}

static struct dmar_atsr_unit *dmar_find_atsr(struct acpi_dmar_atsr *atsr)
{
	struct dmar_atsr_unit *atsru;
	struct acpi_dmar_atsr *tmp;

	list_for_each_entry_rcu(atsru, &dmar_atsr_units, list,
				dmar_rcu_check()) {
		tmp = (struct acpi_dmar_atsr *)atsru->hdr;
		if (atsr->segment != tmp->segment)
			continue;
		if (atsr->header.length != tmp->header.length)
			continue;
		if (memcmp(atsr, tmp, atsr->header.length) == 0)
			return atsru;
	}

	return NULL;
}

int dmar_parse_one_atsr(struct acpi_dmar_header *hdr, void *arg)
{
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	if (system_state >= SYSTEM_RUNNING && !intel_iommu_enabled)
		return 0;

	atsr = container_of(hdr, struct acpi_dmar_atsr, header);
	atsru = dmar_find_atsr(atsr);
	if (atsru)
		return 0;

	atsru = kzalloc(sizeof(*atsru) + hdr->length, GFP_KERNEL);
	if (!atsru)
		return -ENOMEM;

	/*
	 * If memory is allocated from slab by ACPI _DSM method, we need to
	 * copy the memory content because the memory buffer will be freed
	 * on return.
	 */
	atsru->hdr = (void *)(atsru + 1);
	memcpy(atsru->hdr, hdr, hdr->length);
	atsru->include_all = atsr->flags & 0x1;
	if (!atsru->include_all) {
		atsru->devices = dmar_alloc_dev_scope((void *)(atsr + 1),
				(void *)atsr + atsr->header.length,
				&atsru->devices_cnt);
		if (atsru->devices_cnt && atsru->devices == NULL) {
			kfree(atsru);
			return -ENOMEM;
		}
	}

	list_add_rcu(&atsru->list, &dmar_atsr_units);

	return 0;
}

static void intel_iommu_free_atsr(struct dmar_atsr_unit *atsru)
{
	dmar_free_dev_scope(&atsru->devices, &atsru->devices_cnt);
	kfree(atsru);
}

int dmar_release_one_atsr(struct acpi_dmar_header *hdr, void *arg)
{
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	atsr = container_of(hdr, struct acpi_dmar_atsr, header);
	atsru = dmar_find_atsr(atsr);
	if (atsru) {
		list_del_rcu(&atsru->list);
		synchronize_rcu();
		intel_iommu_free_atsr(atsru);
	}

	return 0;
}

int dmar_check_one_atsr(struct acpi_dmar_header *hdr, void *arg)
{
	int i;
	struct device *dev;
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	atsr = container_of(hdr, struct acpi_dmar_atsr, header);
	atsru = dmar_find_atsr(atsr);
	if (!atsru)
		return 0;

	if (!atsru->include_all && atsru->devices && atsru->devices_cnt) {
		for_each_active_dev_scope(atsru->devices, atsru->devices_cnt,
					  i, dev)
			return -EBUSY;
	}

	return 0;
}

static struct dmar_satc_unit *dmar_find_satc(struct acpi_dmar_satc *satc)
{
	struct dmar_satc_unit *satcu;
	struct acpi_dmar_satc *tmp;

	list_for_each_entry_rcu(satcu, &dmar_satc_units, list,
				dmar_rcu_check()) {
		tmp = (struct acpi_dmar_satc *)satcu->hdr;
		if (satc->segment != tmp->segment)
			continue;
		if (satc->header.length != tmp->header.length)
			continue;
		if (memcmp(satc, tmp, satc->header.length) == 0)
			return satcu;
	}

	return NULL;
}

int dmar_parse_one_satc(struct acpi_dmar_header *hdr, void *arg)
{
	struct acpi_dmar_satc *satc;
	struct dmar_satc_unit *satcu;

	if (system_state >= SYSTEM_RUNNING && !intel_iommu_enabled)
		return 0;

	satc = container_of(hdr, struct acpi_dmar_satc, header);
	satcu = dmar_find_satc(satc);
	if (satcu)
		return 0;

	satcu = kzalloc(sizeof(*satcu) + hdr->length, GFP_KERNEL);
	if (!satcu)
		return -ENOMEM;

	satcu->hdr = (void *)(satcu + 1);
	memcpy(satcu->hdr, hdr, hdr->length);
	satcu->atc_required = satc->flags & 0x1;
	satcu->devices = dmar_alloc_dev_scope((void *)(satc + 1),
					      (void *)satc + satc->header.length,
					      &satcu->devices_cnt);
	if (satcu->devices_cnt && !satcu->devices) {
		kfree(satcu);
		return -ENOMEM;
	}
	list_add_rcu(&satcu->list, &dmar_satc_units);

	return 0;
}

static int intel_iommu_add(struct dmar_drhd_unit *dmaru)
{
	int sp, ret;
	struct intel_iommu *iommu = dmaru->iommu;

	if (g_iommus[iommu->seq_id])
		return 0;

	ret = intel_cap_audit(CAP_AUDIT_HOTPLUG_DMAR, iommu);
	if (ret)
		goto out;

	if (hw_pass_through && !ecap_pass_through(iommu->ecap)) {
		pr_warn("%s: Doesn't support hardware pass through.\n",
			iommu->name);
		return -ENXIO;
	}
	if (!ecap_sc_support(iommu->ecap) &&
	    domain_update_iommu_snooping(iommu)) {
		pr_warn("%s: Doesn't support snooping.\n",
			iommu->name);
		return -ENXIO;
	}
	sp = domain_update_iommu_superpage(NULL, iommu) - 1;
	if (sp >= 0 && !(cap_super_page_val(iommu->cap) & (1 << sp))) {
		pr_warn("%s: Doesn't support large page.\n",
			iommu->name);
		return -ENXIO;
	}

	/*
	 * Disable translation if already enabled prior to OS handover.
	 */
	if (iommu->gcmd & DMA_GCMD_TE)
		iommu_disable_translation(iommu);

	g_iommus[iommu->seq_id] = iommu;
	ret = iommu_init_domains(iommu);
	if (ret == 0)
		ret = iommu_alloc_root_entry(iommu);
	if (ret)
		goto out;

	intel_svm_check(iommu);

	if (dmaru->ignored) {
		/*
		 * we always have to disable PMRs or DMA may fail on this device
		 */
		if (force_on)
			iommu_disable_protect_mem_regions(iommu);
		return 0;
	}

	intel_iommu_init_qi(iommu);
	iommu_flush_write_buffer(iommu);

#ifdef CONFIG_INTEL_IOMMU_SVM
	if (pasid_supported(iommu) && ecap_prs(iommu->ecap)) {
		ret = intel_svm_enable_prq(iommu);
		if (ret)
			goto disable_iommu;
	}
#endif
	ret = dmar_set_interrupt(iommu);
	if (ret)
		goto disable_iommu;

	iommu_set_root_entry(iommu);
	iommu_enable_translation(iommu);

	iommu_disable_protect_mem_regions(iommu);
	return 0;

disable_iommu:
	disable_dmar_iommu(iommu);
out:
	free_dmar_iommu(iommu);
	return ret;
}

int dmar_iommu_hotplug(struct dmar_drhd_unit *dmaru, bool insert)
{
	int ret = 0;
	struct intel_iommu *iommu = dmaru->iommu;

	if (!intel_iommu_enabled)
		return 0;
	if (iommu == NULL)
		return -EINVAL;

	if (insert) {
		ret = intel_iommu_add(dmaru);
	} else {
		disable_dmar_iommu(iommu);
		free_dmar_iommu(iommu);
	}

	return ret;
}

static void intel_iommu_free_dmars(void)
{
	struct dmar_rmrr_unit *rmrru, *rmrr_n;
	struct dmar_atsr_unit *atsru, *atsr_n;
	struct dmar_satc_unit *satcu, *satc_n;

	list_for_each_entry_safe(rmrru, rmrr_n, &dmar_rmrr_units, list) {
		list_del(&rmrru->list);
		dmar_free_dev_scope(&rmrru->devices, &rmrru->devices_cnt);
		kfree(rmrru);
	}

	list_for_each_entry_safe(atsru, atsr_n, &dmar_atsr_units, list) {
		list_del(&atsru->list);
		intel_iommu_free_atsr(atsru);
	}
	list_for_each_entry_safe(satcu, satc_n, &dmar_satc_units, list) {
		list_del(&satcu->list);
		dmar_free_dev_scope(&satcu->devices, &satcu->devices_cnt);
		kfree(satcu);
	}
}

int dmar_find_matched_atsr_unit(struct pci_dev *dev)
{
	int i, ret = 1;
	struct pci_bus *bus;
	struct pci_dev *bridge = NULL;
	struct device *tmp;
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	dev = pci_physfn(dev);
	for (bus = dev->bus; bus; bus = bus->parent) {
		bridge = bus->self;
		/* If it's an integrated device, allow ATS */
		if (!bridge)
			return 1;
		/* Connected via non-PCIe: no ATS */
		if (!pci_is_pcie(bridge) ||
		    pci_pcie_type(bridge) == PCI_EXP_TYPE_PCI_BRIDGE)
			return 0;
		/* If we found the root port, look it up in the ATSR */
		if (pci_pcie_type(bridge) == PCI_EXP_TYPE_ROOT_PORT)
			break;
	}

	rcu_read_lock();
	list_for_each_entry_rcu(atsru, &dmar_atsr_units, list) {
		atsr = container_of(atsru->hdr, struct acpi_dmar_atsr, header);
		if (atsr->segment != pci_domain_nr(dev->bus))
			continue;

		for_each_dev_scope(atsru->devices, atsru->devices_cnt, i, tmp)
			if (tmp == &bridge->dev)
				goto out;

		if (atsru->include_all)
			goto out;
	}
	ret = 0;
out:
	rcu_read_unlock();

	return ret;
}

int dmar_iommu_notify_scope_dev(struct dmar_pci_notify_info *info)
{
	int ret;
	struct dmar_rmrr_unit *rmrru;
	struct dmar_atsr_unit *atsru;
	struct dmar_satc_unit *satcu;
	struct acpi_dmar_atsr *atsr;
	struct acpi_dmar_reserved_memory *rmrr;
	struct acpi_dmar_satc *satc;

	if (!intel_iommu_enabled && system_state >= SYSTEM_RUNNING)
		return 0;

	list_for_each_entry(rmrru, &dmar_rmrr_units, list) {
		rmrr = container_of(rmrru->hdr,
				    struct acpi_dmar_reserved_memory, header);
		if (info->event == BUS_NOTIFY_ADD_DEVICE) {
			ret = dmar_insert_dev_scope(info, (void *)(rmrr + 1),
				((void *)rmrr) + rmrr->header.length,
				rmrr->segment, rmrru->devices,
				rmrru->devices_cnt);
			if (ret < 0)
				return ret;
		} else if (info->event == BUS_NOTIFY_REMOVED_DEVICE) {
			dmar_remove_dev_scope(info, rmrr->segment,
				rmrru->devices, rmrru->devices_cnt);
		}
	}

	list_for_each_entry(atsru, &dmar_atsr_units, list) {
		if (atsru->include_all)
			continue;

		atsr = container_of(atsru->hdr, struct acpi_dmar_atsr, header);
		if (info->event == BUS_NOTIFY_ADD_DEVICE) {
			ret = dmar_insert_dev_scope(info, (void *)(atsr + 1),
					(void *)atsr + atsr->header.length,
					atsr->segment, atsru->devices,
					atsru->devices_cnt);
			if (ret > 0)
				break;
			else if (ret < 0)
				return ret;
		} else if (info->event == BUS_NOTIFY_REMOVED_DEVICE) {
			if (dmar_remove_dev_scope(info, atsr->segment,
					atsru->devices, atsru->devices_cnt))
				break;
		}
	}
	list_for_each_entry(satcu, &dmar_satc_units, list) {
		satc = container_of(satcu->hdr, struct acpi_dmar_satc, header);
		if (info->event == BUS_NOTIFY_ADD_DEVICE) {
			ret = dmar_insert_dev_scope(info, (void *)(satc + 1),
					(void *)satc + satc->header.length,
					satc->segment, satcu->devices,
					satcu->devices_cnt);
			if (ret > 0)
				break;
			else if (ret < 0)
				return ret;
		} else if (info->event == BUS_NOTIFY_REMOVED_DEVICE) {
			if (dmar_remove_dev_scope(info, satc->segment,
					satcu->devices, satcu->devices_cnt))
				break;
		}
	}

	return 0;
}

static int intel_iommu_memory_notifier(struct notifier_block *nb,
				       unsigned long val, void *v)
{
	struct memory_notify *mhp = v;
	unsigned long start_vpfn = mm_to_dma_pfn(mhp->start_pfn);
	unsigned long last_vpfn = mm_to_dma_pfn(mhp->start_pfn +
			mhp->nr_pages - 1);

	switch (val) {
	case MEM_GOING_ONLINE:
		if (iommu_domain_identity_map(si_domain,
					      start_vpfn, last_vpfn)) {
			pr_warn("Failed to build identity map for [%lx-%lx]\n",
				start_vpfn, last_vpfn);
			return NOTIFY_BAD;
		}
		break;

	case MEM_OFFLINE:
	case MEM_CANCEL_ONLINE:
		{
			struct dmar_drhd_unit *drhd;
			struct intel_iommu *iommu;
			struct page *freelist;

			freelist = domain_unmap(si_domain,
						start_vpfn, last_vpfn,
						NULL);

			rcu_read_lock();
			for_each_active_iommu(iommu, drhd)
				iommu_flush_iotlb_psi(iommu, si_domain,
					start_vpfn, mhp->nr_pages,
					!freelist, 0);
			rcu_read_unlock();
			dma_free_pagelist(freelist);
		}
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block intel_iommu_memory_nb = {
	.notifier_call = intel_iommu_memory_notifier,
	.priority = 0
};

static void intel_disable_iommus(void)
{
	struct intel_iommu *iommu = NULL;
	struct dmar_drhd_unit *drhd;

	for_each_iommu(iommu, drhd)
		iommu_disable_translation(iommu);
}

void intel_iommu_shutdown(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;

	if (no_iommu || dmar_disabled)
		return;

	down_write(&dmar_global_lock);

	/* Disable PMRs explicitly here. */
	for_each_iommu(iommu, drhd)
		iommu_disable_protect_mem_regions(iommu);

	/* Make sure the IOMMUs are switched off */
	intel_disable_iommus();

	up_write(&dmar_global_lock);
}

static inline struct intel_iommu *dev_to_intel_iommu(struct device *dev)
{
	struct iommu_device *iommu_dev = dev_to_iommu_device(dev);

	return container_of(iommu_dev, struct intel_iommu, iommu);
}

static ssize_t intel_iommu_show_version(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	u32 ver = readl(iommu->reg + DMAR_VER_REG);
	return sprintf(buf, "%d:%d\n",
		       DMAR_VER_MAJOR(ver), DMAR_VER_MINOR(ver));
}
static DEVICE_ATTR(version, S_IRUGO, intel_iommu_show_version, NULL);

static ssize_t intel_iommu_show_address(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	return sprintf(buf, "%llx\n", iommu->reg_phys);
}
static DEVICE_ATTR(address, S_IRUGO, intel_iommu_show_address, NULL);

static ssize_t intel_iommu_show_cap(struct device *dev,
				    struct device_attribute *attr,
				    char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	return sprintf(buf, "%llx\n", iommu->cap);
}
static DEVICE_ATTR(cap, S_IRUGO, intel_iommu_show_cap, NULL);

static ssize_t intel_iommu_show_ecap(struct device *dev,
				    struct device_attribute *attr,
				    char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	return sprintf(buf, "%llx\n", iommu->ecap);
}
static DEVICE_ATTR(ecap, S_IRUGO, intel_iommu_show_ecap, NULL);

static ssize_t intel_iommu_show_ndoms(struct device *dev,
				      struct device_attribute *attr,
				      char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	return sprintf(buf, "%ld\n", cap_ndoms(iommu->cap));
}
static DEVICE_ATTR(domains_supported, S_IRUGO, intel_iommu_show_ndoms, NULL);

static ssize_t intel_iommu_show_ndoms_used(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	return sprintf(buf, "%d\n", bitmap_weight(iommu->domain_ids,
						  cap_ndoms(iommu->cap)));
}
static DEVICE_ATTR(domains_used, S_IRUGO, intel_iommu_show_ndoms_used, NULL);

static struct attribute *intel_iommu_attrs[] = {
	&dev_attr_version.attr,
	&dev_attr_address.attr,
	&dev_attr_cap.attr,
	&dev_attr_ecap.attr,
	&dev_attr_domains_supported.attr,
	&dev_attr_domains_used.attr,
	NULL,
};

static struct attribute_group intel_iommu_group = {
	.name = "intel-iommu",
	.attrs = intel_iommu_attrs,
};

const struct attribute_group *intel_iommu_groups[] = {
	&intel_iommu_group,
	NULL,
};

static inline bool has_external_pci(void)
{
	struct pci_dev *pdev = NULL;

	for_each_pci_dev(pdev)
		if (pdev->external_facing)
			return true;

	return false;
}

static int __init platform_optin_force_iommu(void)
{
	if (!dmar_platform_optin() || no_platform_optin || !has_external_pci())
		return 0;

	if (no_iommu || dmar_disabled)
		pr_info("Intel-IOMMU force enabled due to platform opt in\n");

	/*
	 * If Intel-IOMMU is disabled by default, we will apply identity
	 * map for all devices except those marked as being untrusted.
	 */
	if (dmar_disabled)
		iommu_set_default_passthrough(false);

	dmar_disabled = 0;
	no_iommu = 0;

	return 1;
}

static int __init probe_acpi_namespace_devices(void)
{
	struct dmar_drhd_unit *drhd;
	/* To avoid a -Wunused-but-set-variable warning. */
	struct intel_iommu *iommu __maybe_unused;
	struct device *dev;
	int i, ret = 0;

	for_each_active_iommu(iommu, drhd) {
		for_each_active_dev_scope(drhd->devices,
					  drhd->devices_cnt, i, dev) {
			struct acpi_device_physical_node *pn;
			struct iommu_group *group;
			struct acpi_device *adev;

			if (dev->bus != &acpi_bus_type)
				continue;

			adev = to_acpi_device(dev);
			mutex_lock(&adev->physical_node_lock);
			list_for_each_entry(pn,
					    &adev->physical_node_list, node) {
				group = iommu_group_get(pn->dev);
				if (group) {
					iommu_group_put(group);
					continue;
				}

				pn->dev->bus->iommu_ops = &intel_iommu_ops;
				ret = iommu_probe_device(pn->dev);
				if (ret)
					break;
			}
			mutex_unlock(&adev->physical_node_lock);

			if (ret)
				return ret;
		}
	}

	return 0;
}

int __init intel_iommu_init(void)
{
	int ret = -ENODEV;
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;

	/*
	 * Intel IOMMU is required for a TXT/tboot launch or platform
	 * opt in, so enforce that.
	 */
	force_on = (!intel_iommu_tboot_noforce && tboot_force_iommu()) ||
		    platform_optin_force_iommu();

	if (iommu_init_mempool()) {
		if (force_on)
			panic("tboot: Failed to initialize iommu memory\n");
		return -ENOMEM;
	}

	down_write(&dmar_global_lock);
	if (dmar_table_init()) {
		if (force_on)
			panic("tboot: Failed to initialize DMAR table\n");
		goto out_free_dmar;
	}

	if (dmar_dev_scope_init() < 0) {
		if (force_on)
			panic("tboot: Failed to initialize DMAR device scope\n");
		goto out_free_dmar;
	}

	up_write(&dmar_global_lock);

	/*
	 * The bus notifier takes the dmar_global_lock, so lockdep will
	 * complain later when we register it under the lock.
	 */
	dmar_register_bus_notifier();

	down_write(&dmar_global_lock);

	if (!no_iommu)
		intel_iommu_debugfs_init();

	if (no_iommu || dmar_disabled) {
		/*
		 * We exit the function here to ensure IOMMU's remapping and
		 * mempool aren't setup, which means that the IOMMU's PMRs
		 * won't be disabled via the call to init_dmars(). So disable
		 * it explicitly here. The PMRs were setup by tboot prior to
		 * calling SENTER, but the kernel is expected to reset/tear
		 * down the PMRs.
		 */
		if (intel_iommu_tboot_noforce) {
			for_each_iommu(iommu, drhd)
				iommu_disable_protect_mem_regions(iommu);
		}

		/*
		 * Make sure the IOMMUs are switched off, even when we
		 * boot into a kexec kernel and the previous kernel left
		 * them enabled
		 */
		intel_disable_iommus();
		goto out_free_dmar;
	}

	if (list_empty(&dmar_rmrr_units))
		pr_info("No RMRR found\n");

	if (list_empty(&dmar_atsr_units))
		pr_info("No ATSR found\n");

	if (list_empty(&dmar_satc_units))
		pr_info("No SATC found\n");

	if (dmar_map_gfx)
		intel_iommu_gfx_mapped = 1;

	init_no_remapping_devices();

	ret = init_dmars();
	if (ret) {
		if (force_on)
			panic("tboot: Failed to initialize DMARs\n");
		pr_err("Initialization failed\n");
		goto out_free_dmar;
	}
	up_write(&dmar_global_lock);

	init_iommu_pm_ops();

	down_read(&dmar_global_lock);
	for_each_active_iommu(iommu, drhd) {
		/*
		 * The flush queue implementation does not perform
		 * page-selective invalidations that are required for efficient
		 * TLB flushes in virtual environments.  The benefit of batching
		 * is likely to be much lower than the overhead of synchronizing
		 * the virtual and physical IOMMU page-tables.
		 */
		if (!intel_iommu_strict && cap_caching_mode(iommu->cap)) {
			pr_warn("IOMMU batching is disabled due to virtualization");
			intel_iommu_strict = 1;
		}
		iommu_device_sysfs_add(&iommu->iommu, NULL,
				       intel_iommu_groups,
				       "%s", iommu->name);
		iommu_device_register(&iommu->iommu, &intel_iommu_ops, NULL);
	}
	up_read(&dmar_global_lock);

	iommu_set_dma_strict(intel_iommu_strict);
	bus_set_iommu(&pci_bus_type, &intel_iommu_ops);
	if (si_domain && !hw_pass_through)
		register_memory_notifier(&intel_iommu_memory_nb);

	down_read(&dmar_global_lock);
	if (probe_acpi_namespace_devices())
		pr_warn("ACPI name space devices didn't probe correctly\n");

	/* Finally, we enable the DMA remapping hardware. */
	for_each_iommu(iommu, drhd) {
		if (!drhd->ignored && !translation_pre_enabled(iommu))
			iommu_enable_translation(iommu);

		iommu_disable_protect_mem_regions(iommu);
	}
	up_read(&dmar_global_lock);

	pr_info("Intel(R) Virtualization Technology for Directed I/O\n");

	intel_iommu_enabled = 1;

	return 0;

out_free_dmar:
	intel_iommu_free_dmars();
	up_write(&dmar_global_lock);
	iommu_exit_mempool();
	return ret;
}

static int domain_context_clear_one_cb(struct pci_dev *pdev, u16 alias, void *opaque)
{
	struct intel_iommu *iommu = opaque;

	domain_context_clear_one(iommu, PCI_BUS_NUM(alias), alias & 0xff);
	return 0;
}

/*
 * NB - intel-iommu lacks any sort of reference counting for the users of
 * dependent devices.  If multiple endpoints have intersecting dependent
 * devices, unbinding the driver from any one of them will possibly leave
 * the others unable to operate.
 */
static void domain_context_clear(struct intel_iommu *iommu, struct device *dev)
{
	if (!iommu || !dev || !dev_is_pci(dev))
		return;

	pci_for_each_dma_alias(to_pci_dev(dev), &domain_context_clear_one_cb, iommu);
}

static void __dmar_remove_one_dev_info(struct device_domain_info *info)
{
	struct dmar_domain *domain;
	struct intel_iommu *iommu;
	unsigned long flags;

	assert_spin_locked(&device_domain_lock);

	if (WARN_ON(!info))
		return;

	iommu = info->iommu;
	domain = info->domain;

	if (info->dev) {
		if (dev_is_pci(info->dev) && sm_supported(iommu))
			intel_pasid_tear_down_entry(iommu, info->dev,
					PASID_RID2PASID, false);

		iommu_disable_dev_iotlb(info);
		if (!dev_is_real_dma_subdevice(info->dev))
			domain_context_clear(iommu, info->dev);
		intel_pasid_free_table(info->dev);
	}

	unlink_domain_info(info);

	spin_lock_irqsave(&iommu->lock, flags);
	domain_detach_iommu(domain, iommu);
	spin_unlock_irqrestore(&iommu->lock, flags);

	free_devinfo_mem(info);
}

static void dmar_remove_one_dev_info(struct device *dev)
{
	struct device_domain_info *info;
	unsigned long flags;

	spin_lock_irqsave(&device_domain_lock, flags);
	info = get_domain_info(dev);
	if (info)
		__dmar_remove_one_dev_info(info);
	spin_unlock_irqrestore(&device_domain_lock, flags);
}

static int md_domain_init(struct dmar_domain *domain, int guest_width)
{
	int adjust_width;

	/* calculate AGAW */
	domain->gaw = guest_width;
	adjust_width = guestwidth_to_adjustwidth(guest_width);
	domain->agaw = width_to_agaw(adjust_width);

	domain->iommu_coherency = 0;
	domain->iommu_snooping = 0;
	domain->iommu_superpage = 0;
	domain->max_addr = 0;

	/* always allocate the top pgd */
	domain->pgd = (struct dma_pte *)alloc_pgtable_page(domain->nid);
	if (!domain->pgd)
		return -ENOMEM;
	domain_flush_cache(domain, domain->pgd, PAGE_SIZE);
	return 0;
}

static struct iommu_domain *intel_iommu_domain_alloc(unsigned type)
{
	struct dmar_domain *dmar_domain;
	struct iommu_domain *domain;

	switch (type) {
	case IOMMU_DOMAIN_DMA:
	case IOMMU_DOMAIN_UNMANAGED:
		dmar_domain = alloc_domain(0);
		if (!dmar_domain) {
			pr_err("Can't allocate dmar_domain\n");
			return NULL;
		}
		if (md_domain_init(dmar_domain, DEFAULT_DOMAIN_ADDRESS_WIDTH)) {
			pr_err("Domain initialization failed\n");
			domain_exit(dmar_domain);
			return NULL;
		}

		if (type == IOMMU_DOMAIN_DMA &&
		    iommu_get_dma_cookie(&dmar_domain->domain))
			return NULL;

		domain = &dmar_domain->domain;
		domain->geometry.aperture_start = 0;
		domain->geometry.aperture_end   =
				__DOMAIN_MAX_ADDR(dmar_domain->gaw);
		domain->geometry.force_aperture = true;

		return domain;
	case IOMMU_DOMAIN_IDENTITY:
		return &si_domain->domain;
	default:
		return NULL;
	}

	return NULL;
}

static void intel_iommu_domain_free(struct iommu_domain *domain)
{
	if (domain != &si_domain->domain)
		domain_exit(to_dmar_domain(domain));
}

/*
 * Check whether a @domain could be attached to the @dev through the
 * aux-domain attach/detach APIs.
 */
static inline bool
is_aux_domain(struct device *dev, struct iommu_domain *domain)
{
	struct device_domain_info *info = get_domain_info(dev);

	return info && info->auxd_enabled &&
			domain->type == IOMMU_DOMAIN_UNMANAGED;
}

static inline struct subdev_domain_info *
lookup_subdev_info(struct dmar_domain *domain, struct device *dev)
{
	struct subdev_domain_info *sinfo;

	if (!list_empty(&domain->subdevices)) {
		list_for_each_entry(sinfo, &domain->subdevices, link_domain) {
			if (sinfo->pdev == dev)
				return sinfo;
		}
	}

	return NULL;
}

static int auxiliary_link_device(struct dmar_domain *domain,
				 struct device *dev)
{
	struct device_domain_info *info = get_domain_info(dev);
	struct subdev_domain_info *sinfo = lookup_subdev_info(domain, dev);

	assert_spin_locked(&device_domain_lock);
	if (WARN_ON(!info))
		return -EINVAL;

	if (!sinfo) {
		sinfo = kzalloc(sizeof(*sinfo), GFP_ATOMIC);
		if (!sinfo)
			return -ENOMEM;
		sinfo->domain = domain;
		sinfo->pdev = dev;
		list_add(&sinfo->link_phys, &info->subdevices);
		list_add(&sinfo->link_domain, &domain->subdevices);
	}

	return ++sinfo->users;
}

static int auxiliary_unlink_device(struct dmar_domain *domain,
				   struct device *dev)
{
	struct device_domain_info *info = get_domain_info(dev);
	struct subdev_domain_info *sinfo = lookup_subdev_info(domain, dev);
	int ret;

	assert_spin_locked(&device_domain_lock);
	if (WARN_ON(!info || !sinfo || sinfo->users <= 0))
		return -EINVAL;

	ret = --sinfo->users;
	if (!ret) {
		list_del(&sinfo->link_phys);
		list_del(&sinfo->link_domain);
		kfree(sinfo);
	}

	return ret;
}

static int aux_domain_add_dev(struct dmar_domain *domain,
			      struct device *dev)
{
	int ret;
	unsigned long flags;
	struct intel_iommu *iommu;

	iommu = device_to_iommu(dev, NULL, NULL);
	if (!iommu)
		return -ENODEV;

	if (domain->default_pasid <= 0) {
		u32 pasid;

		/* No private data needed for the default pasid */
		pasid = ioasid_alloc(NULL, PASID_MIN,
				     pci_max_pasids(to_pci_dev(dev)) - 1,
				     NULL);
		if (pasid == INVALID_IOASID) {
			pr_err("Can't allocate default pasid\n");
			return -ENODEV;
		}
		domain->default_pasid = pasid;
	}

	spin_lock_irqsave(&device_domain_lock, flags);
	ret = auxiliary_link_device(domain, dev);
	if (ret <= 0)
		goto link_failed;

	/*
	 * Subdevices from the same physical device can be attached to the
	 * same domain. For such cases, only the first subdevice attachment
	 * needs to go through the full steps in this function. So if ret >
	 * 1, just goto out.
	 */
	if (ret > 1)
		goto out;

	/*
	 * iommu->lock must be held to attach domain to iommu and setup the
	 * pasid entry for second level translation.
	 */
	spin_lock(&iommu->lock);
	ret = domain_attach_iommu(domain, iommu);
	if (ret)
		goto attach_failed;

	/* Setup the PASID entry for mediated devices: */
	if (domain_use_first_level(domain))
		ret = domain_setup_first_level(iommu, domain, dev,
					       domain->default_pasid);
	else
		ret = intel_pasid_setup_second_level(iommu, domain, dev,
						     domain->default_pasid);
	if (ret)
		goto table_failed;

	spin_unlock(&iommu->lock);
out:
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return 0;

table_failed:
	domain_detach_iommu(domain, iommu);
attach_failed:
	spin_unlock(&iommu->lock);
	auxiliary_unlink_device(domain, dev);
link_failed:
	spin_unlock_irqrestore(&device_domain_lock, flags);
	if (list_empty(&domain->subdevices) && domain->default_pasid > 0)
		ioasid_put(domain->default_pasid);

	return ret;
}

static void aux_domain_remove_dev(struct dmar_domain *domain,
				  struct device *dev)
{
	struct device_domain_info *info;
	struct intel_iommu *iommu;
	unsigned long flags;

	if (!is_aux_domain(dev, &domain->domain))
		return;

	spin_lock_irqsave(&device_domain_lock, flags);
	info = get_domain_info(dev);
	iommu = info->iommu;

	if (!auxiliary_unlink_device(domain, dev)) {
		spin_lock(&iommu->lock);
		intel_pasid_tear_down_entry(iommu, dev,
					    domain->default_pasid, false);
		domain_detach_iommu(domain, iommu);
		spin_unlock(&iommu->lock);
	}

	spin_unlock_irqrestore(&device_domain_lock, flags);

	if (list_empty(&domain->subdevices) && domain->default_pasid > 0)
		ioasid_put(domain->default_pasid);
}

static int prepare_domain_attach_device(struct iommu_domain *domain,
					struct device *dev)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	struct intel_iommu *iommu;
	int addr_width;

	iommu = device_to_iommu(dev, NULL, NULL);
	if (!iommu)
		return -ENODEV;

	/* check if this iommu agaw is sufficient for max mapped address */
	addr_width = agaw_to_width(iommu->agaw);
	if (addr_width > cap_mgaw(iommu->cap))
		addr_width = cap_mgaw(iommu->cap);

	if (dmar_domain->max_addr > (1LL << addr_width)) {
		dev_err(dev, "%s: iommu width (%d) is not "
		        "sufficient for the mapped address (%llx)\n",
		        __func__, addr_width, dmar_domain->max_addr);
		return -EFAULT;
	}
	dmar_domain->gaw = addr_width;

	/*
	 * Knock out extra levels of page tables if necessary
	 */
	while (iommu->agaw < dmar_domain->agaw) {
		struct dma_pte *pte;

		pte = dmar_domain->pgd;
		if (dma_pte_present(pte)) {
			dmar_domain->pgd = (struct dma_pte *)
				phys_to_virt(dma_pte_addr(pte));
			free_pgtable_page(pte);
		}
		dmar_domain->agaw--;
	}

	return 0;
}

static int intel_iommu_attach_device(struct iommu_domain *domain,
				     struct device *dev)
{
	int ret;

	if (domain->type == IOMMU_DOMAIN_UNMANAGED &&
	    device_is_rmrr_locked(dev)) {
		dev_warn(dev, "Device is ineligible for IOMMU domain attach due to platform RMRR requirement.  Contact your platform vendor.\n");
		return -EPERM;
	}

	if (is_aux_domain(dev, domain))
		return -EPERM;

	/* normally dev is not mapped */
	if (unlikely(domain_context_mapped(dev))) {
		struct dmar_domain *old_domain;

		old_domain = find_domain(dev);
		if (old_domain)
			dmar_remove_one_dev_info(dev);
	}

	ret = prepare_domain_attach_device(domain, dev);
	if (ret)
		return ret;

	return domain_add_dev_info(to_dmar_domain(domain), dev);
}

static int intel_iommu_aux_attach_device(struct iommu_domain *domain,
					 struct device *dev)
{
	int ret;

	if (!is_aux_domain(dev, domain))
		return -EPERM;

	ret = prepare_domain_attach_device(domain, dev);
	if (ret)
		return ret;

	return aux_domain_add_dev(to_dmar_domain(domain), dev);
}

static void intel_iommu_detach_device(struct iommu_domain *domain,
				      struct device *dev)
{
	dmar_remove_one_dev_info(dev);
}

static void intel_iommu_aux_detach_device(struct iommu_domain *domain,
					  struct device *dev)
{
	aux_domain_remove_dev(to_dmar_domain(domain), dev);
}

#ifdef CONFIG_INTEL_IOMMU_SVM
/*
 * 2D array for converting and sanitizing IOMMU generic TLB granularity to
 * VT-d granularity. Invalidation is typically included in the unmap operation
 * as a result of DMA or VFIO unmap. However, for assigned devices guest
 * owns the first level page tables. Invalidations of translation caches in the
 * guest are trapped and passed down to the host.
 *
 * vIOMMU in the guest will only expose first level page tables, therefore
 * we do not support IOTLB granularity for request without PASID (second level).
 *
 * For example, to find the VT-d granularity encoding for IOTLB
 * type and page selective granularity within PASID:
 * X: indexed by iommu cache type
 * Y: indexed by enum iommu_inv_granularity
 * [IOMMU_CACHE_INV_TYPE_IOTLB][IOMMU_INV_GRANU_ADDR]
 */

static const int
inv_type_granu_table[IOMMU_CACHE_INV_TYPE_NR][IOMMU_INV_GRANU_NR] = {
	/*
	 * PASID based IOTLB invalidation: PASID selective (per PASID),
	 * page selective (address granularity)
	 */
	{-EINVAL, QI_GRAN_NONG_PASID, QI_GRAN_PSI_PASID},
	/* PASID based dev TLBs */
	{-EINVAL, -EINVAL, QI_DEV_IOTLB_GRAN_PASID_SEL},
	/* PASID cache */
	{-EINVAL, -EINVAL, -EINVAL}
};

static inline int to_vtd_granularity(int type, int granu)
{
	return inv_type_granu_table[type][granu];
}

static inline u64 to_vtd_size(u64 granu_size, u64 nr_granules)
{
	u64 nr_pages = (granu_size * nr_granules) >> VTD_PAGE_SHIFT;

	/* VT-d size is encoded as 2^size of 4K pages, 0 for 4k, 9 for 2MB, etc.
	 * IOMMU cache invalidate API passes granu_size in bytes, and number of
	 * granu size in contiguous memory.
	 */
	return order_base_2(nr_pages);
}

static int
intel_iommu_sva_invalidate(struct iommu_domain *domain, struct device *dev,
			   struct iommu_cache_invalidate_info *inv_info)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	struct device_domain_info *info;
	struct intel_iommu *iommu;
	unsigned long flags;
	int cache_type;
	u8 bus, devfn;
	u16 did, sid;
	int ret = 0;
	u64 size = 0;

	if (!inv_info || !dmar_domain)
		return -EINVAL;

	if (!dev || !dev_is_pci(dev))
		return -ENODEV;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	if (!(dmar_domain->flags & DOMAIN_FLAG_NESTING_MODE))
		return -EINVAL;

	spin_lock_irqsave(&device_domain_lock, flags);
	spin_lock(&iommu->lock);
	info = get_domain_info(dev);
	if (!info) {
		ret = -EINVAL;
		goto out_unlock;
	}
	did = dmar_domain->iommu_did[iommu->seq_id];
	sid = PCI_DEVID(bus, devfn);

	/* Size is only valid in address selective invalidation */
	if (inv_info->granularity == IOMMU_INV_GRANU_ADDR)
		size = to_vtd_size(inv_info->granu.addr_info.granule_size,
				   inv_info->granu.addr_info.nb_granules);

	for_each_set_bit(cache_type,
			 (unsigned long *)&inv_info->cache,
			 IOMMU_CACHE_INV_TYPE_NR) {
		int granu = 0;
		u64 pasid = 0;
		u64 addr = 0;

		granu = to_vtd_granularity(cache_type, inv_info->granularity);
		if (granu == -EINVAL) {
			pr_err_ratelimited("Invalid cache type and granu combination %d/%d\n",
					   cache_type, inv_info->granularity);
			break;
		}

		/*
		 * PASID is stored in different locations based on the
		 * granularity.
		 */
		if (inv_info->granularity == IOMMU_INV_GRANU_PASID &&
		    (inv_info->granu.pasid_info.flags & IOMMU_INV_PASID_FLAGS_PASID))
			pasid = inv_info->granu.pasid_info.pasid;
		else if (inv_info->granularity == IOMMU_INV_GRANU_ADDR &&
			 (inv_info->granu.addr_info.flags & IOMMU_INV_ADDR_FLAGS_PASID))
			pasid = inv_info->granu.addr_info.pasid;

		switch (BIT(cache_type)) {
		case IOMMU_CACHE_INV_TYPE_IOTLB:
			/* HW will ignore LSB bits based on address mask */
			if (inv_info->granularity == IOMMU_INV_GRANU_ADDR &&
			    size &&
			    (inv_info->granu.addr_info.addr & ((BIT(VTD_PAGE_SHIFT + size)) - 1))) {
				pr_err_ratelimited("User address not aligned, 0x%llx, size order %llu\n",
						   inv_info->granu.addr_info.addr, size);
			}

			/*
			 * If granu is PASID-selective, address is ignored.
			 * We use npages = -1 to indicate that.
			 */
			qi_flush_piotlb(iommu, did, pasid,
					mm_to_dma_pfn(inv_info->granu.addr_info.addr),
					(granu == QI_GRAN_NONG_PASID) ? -1 : 1 << size,
					inv_info->granu.addr_info.flags & IOMMU_INV_ADDR_FLAGS_LEAF);

			if (!info->ats_enabled)
				break;
			/*
			 * Always flush device IOTLB if ATS is enabled. vIOMMU
			 * in the guest may assume IOTLB flush is inclusive,
			 * which is more efficient.
			 */
			fallthrough;
		case IOMMU_CACHE_INV_TYPE_DEV_IOTLB:
			/*
			 * PASID based device TLB invalidation does not support
			 * IOMMU_INV_GRANU_PASID granularity but only supports
			 * IOMMU_INV_GRANU_ADDR.
			 * The equivalent of that is we set the size to be the
			 * entire range of 64 bit. User only provides PASID info
			 * without address info. So we set addr to 0.
			 */
			if (inv_info->granularity == IOMMU_INV_GRANU_PASID) {
				size = 64 - VTD_PAGE_SHIFT;
				addr = 0;
			} else if (inv_info->granularity == IOMMU_INV_GRANU_ADDR) {
				addr = inv_info->granu.addr_info.addr;
			}

			if (info->ats_enabled)
				qi_flush_dev_iotlb_pasid(iommu, sid,
						info->pfsid, pasid,
						info->ats_qdep, addr,
						size);
			else
				pr_warn_ratelimited("Passdown device IOTLB flush w/o ATS!\n");
			break;
		default:
			dev_err_ratelimited(dev, "Unsupported IOMMU invalidation type %d\n",
					    cache_type);
			ret = -EINVAL;
		}
	}
out_unlock:
	spin_unlock(&iommu->lock);
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return ret;
}
#endif

static int intel_iommu_map(struct iommu_domain *domain,
			   unsigned long iova, phys_addr_t hpa,
			   size_t size, int iommu_prot, gfp_t gfp)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	u64 max_addr;
	int prot = 0;

	if (iommu_prot & IOMMU_READ)
		prot |= DMA_PTE_READ;
	if (iommu_prot & IOMMU_WRITE)
		prot |= DMA_PTE_WRITE;
	if ((iommu_prot & IOMMU_CACHE) && dmar_domain->iommu_snooping)
		prot |= DMA_PTE_SNP;

	max_addr = iova + size;
	if (dmar_domain->max_addr < max_addr) {
		u64 end;

		/* check if minimum agaw is sufficient for mapped address */
		end = __DOMAIN_MAX_ADDR(dmar_domain->gaw) + 1;
		if (end < max_addr) {
			pr_err("%s: iommu width (%d) is not "
			       "sufficient for the mapped address (%llx)\n",
			       __func__, dmar_domain->gaw, max_addr);
			return -EFAULT;
		}
		dmar_domain->max_addr = max_addr;
	}
	/* Round up size to next multiple of PAGE_SIZE, if it and
	   the low bits of hpa would take us onto the next page */
	size = aligned_nrpages(hpa, size);
	return __domain_mapping(dmar_domain, iova >> VTD_PAGE_SHIFT,
				hpa >> VTD_PAGE_SHIFT, size, prot);
}

static size_t intel_iommu_unmap(struct iommu_domain *domain,
				unsigned long iova, size_t size,
				struct iommu_iotlb_gather *gather)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	unsigned long start_pfn, last_pfn;
	int level = 0;

	/* Cope with horrid API which requires us to unmap more than the
	   size argument if it happens to be a large-page mapping. */
	BUG_ON(!pfn_to_dma_pte(dmar_domain, iova >> VTD_PAGE_SHIFT, &level));

	if (size < VTD_PAGE_SIZE << level_to_offset_bits(level))
		size = VTD_PAGE_SIZE << level_to_offset_bits(level);

	start_pfn = iova >> VTD_PAGE_SHIFT;
	last_pfn = (iova + size - 1) >> VTD_PAGE_SHIFT;

	gather->freelist = domain_unmap(dmar_domain, start_pfn,
					last_pfn, gather->freelist);

	if (dmar_domain->max_addr == iova + size)
		dmar_domain->max_addr = iova;

	iommu_iotlb_gather_add_page(domain, gather, iova, size);

	return size;
}

static void intel_iommu_tlb_sync(struct iommu_domain *domain,
				 struct iommu_iotlb_gather *gather)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	unsigned long iova_pfn = IOVA_PFN(gather->start);
	size_t size = gather->end - gather->start;
	unsigned long start_pfn;
	unsigned long nrpages;
	int iommu_id;

	nrpages = aligned_nrpages(gather->start, size);
	start_pfn = mm_to_dma_pfn(iova_pfn);

	for_each_domain_iommu(iommu_id, dmar_domain)
		iommu_flush_iotlb_psi(g_iommus[iommu_id], dmar_domain,
				      start_pfn, nrpages, !gather->freelist, 0);

	dma_free_pagelist(gather->freelist);
}

static phys_addr_t intel_iommu_iova_to_phys(struct iommu_domain *domain,
					    dma_addr_t iova)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	struct dma_pte *pte;
	int level = 0;
	u64 phys = 0;

	pte = pfn_to_dma_pte(dmar_domain, iova >> VTD_PAGE_SHIFT, &level);
	if (pte && dma_pte_present(pte))
		phys = dma_pte_addr(pte) +
			(iova & (BIT_MASK(level_to_offset_bits(level) +
						VTD_PAGE_SHIFT) - 1));

	return phys;
}

static bool intel_iommu_capable(enum iommu_cap cap)
{
	if (cap == IOMMU_CAP_CACHE_COHERENCY)
		return domain_update_iommu_snooping(NULL) == 1;
	if (cap == IOMMU_CAP_INTR_REMAP)
		return irq_remapping_enabled == 1;

	return false;
}

static struct iommu_device *intel_iommu_probe_device(struct device *dev)
{
	struct intel_iommu *iommu;

	iommu = device_to_iommu(dev, NULL, NULL);
	if (!iommu)
		return ERR_PTR(-ENODEV);

	if (translation_pre_enabled(iommu))
		dev_iommu_priv_set(dev, DEFER_DEVICE_DOMAIN_INFO);

	return &iommu->iommu;
}

static void intel_iommu_release_device(struct device *dev)
{
	struct intel_iommu *iommu;

	iommu = device_to_iommu(dev, NULL, NULL);
	if (!iommu)
		return;

	dmar_remove_one_dev_info(dev);

	set_dma_ops(dev, NULL);
}

static void intel_iommu_probe_finalize(struct device *dev)
{
	dma_addr_t base = IOVA_START_PFN << VTD_PAGE_SHIFT;
	struct iommu_domain *domain = iommu_get_domain_for_dev(dev);
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);

	if (domain && domain->type == IOMMU_DOMAIN_DMA)
		iommu_setup_dma_ops(dev, base,
				    __DOMAIN_MAX_ADDR(dmar_domain->gaw) - base);
	else
		set_dma_ops(dev, NULL);
}

static void intel_iommu_get_resv_regions(struct device *device,
					 struct list_head *head)
{
	int prot = DMA_PTE_READ | DMA_PTE_WRITE;
	struct iommu_resv_region *reg;
	struct dmar_rmrr_unit *rmrr;
	struct device *i_dev;
	int i;

	down_read(&dmar_global_lock);
	for_each_rmrr_units(rmrr) {
		for_each_active_dev_scope(rmrr->devices, rmrr->devices_cnt,
					  i, i_dev) {
			struct iommu_resv_region *resv;
			enum iommu_resv_type type;
			size_t length;

			if (i_dev != device &&
			    !is_downstream_to_pci_bridge(device, i_dev))
				continue;

			length = rmrr->end_address - rmrr->base_address + 1;

			type = device_rmrr_is_relaxable(device) ?
				IOMMU_RESV_DIRECT_RELAXABLE : IOMMU_RESV_DIRECT;

			resv = iommu_alloc_resv_region(rmrr->base_address,
						       length, prot, type);
			if (!resv)
				break;

			list_add_tail(&resv->list, head);
		}
	}
	up_read(&dmar_global_lock);

#ifdef CONFIG_INTEL_IOMMU_FLOPPY_WA
	if (dev_is_pci(device)) {
		struct pci_dev *pdev = to_pci_dev(device);

		if ((pdev->class >> 8) == PCI_CLASS_BRIDGE_ISA) {
			reg = iommu_alloc_resv_region(0, 1UL << 24, prot,
						   IOMMU_RESV_DIRECT_RELAXABLE);
			if (reg)
				list_add_tail(&reg->list, head);
		}
	}
#endif /* CONFIG_INTEL_IOMMU_FLOPPY_WA */

	reg = iommu_alloc_resv_region(IOAPIC_RANGE_START,
				      IOAPIC_RANGE_END - IOAPIC_RANGE_START + 1,
				      0, IOMMU_RESV_MSI);
	if (!reg)
		return;
	list_add_tail(&reg->list, head);
}

int intel_iommu_enable_pasid(struct intel_iommu *iommu, struct device *dev)
{
	struct device_domain_info *info;
	struct context_entry *context;
	struct dmar_domain *domain;
	unsigned long flags;
	u64 ctx_lo;
	int ret;

	domain = find_domain(dev);
	if (!domain)
		return -EINVAL;

	spin_lock_irqsave(&device_domain_lock, flags);
	spin_lock(&iommu->lock);

	ret = -EINVAL;
	info = get_domain_info(dev);
	if (!info || !info->pasid_supported)
		goto out;

	context = iommu_context_addr(iommu, info->bus, info->devfn, 0);
	if (WARN_ON(!context))
		goto out;

	ctx_lo = context[0].lo;

	if (!(ctx_lo & CONTEXT_PASIDE)) {
		ctx_lo |= CONTEXT_PASIDE;
		context[0].lo = ctx_lo;
		wmb();
		iommu->flush.flush_context(iommu,
					   domain->iommu_did[iommu->seq_id],
					   PCI_DEVID(info->bus, info->devfn),
					   DMA_CCMD_MASK_NOBIT,
					   DMA_CCMD_DEVICE_INVL);
	}

	/* Enable PASID support in the device, if it wasn't already */
	if (!info->pasid_enabled)
		iommu_enable_dev_iotlb(info);

	ret = 0;

 out:
	spin_unlock(&iommu->lock);
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return ret;
}

static struct iommu_group *intel_iommu_device_group(struct device *dev)
{
	if (dev_is_pci(dev))
		return pci_device_group(dev);
	return generic_device_group(dev);
}

static int intel_iommu_enable_auxd(struct device *dev)
{
	struct device_domain_info *info;
	struct intel_iommu *iommu;
	unsigned long flags;
	int ret;

	iommu = device_to_iommu(dev, NULL, NULL);
	if (!iommu || dmar_disabled)
		return -EINVAL;

	if (!sm_supported(iommu) || !pasid_supported(iommu))
		return -EINVAL;

	ret = intel_iommu_enable_pasid(iommu, dev);
	if (ret)
		return -ENODEV;

	spin_lock_irqsave(&device_domain_lock, flags);
	info = get_domain_info(dev);
	info->auxd_enabled = 1;
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return 0;
}

static int intel_iommu_disable_auxd(struct device *dev)
{
	struct device_domain_info *info;
	unsigned long flags;

	spin_lock_irqsave(&device_domain_lock, flags);
	info = get_domain_info(dev);
	if (!WARN_ON(!info))
		info->auxd_enabled = 0;
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return 0;
}

/*
 * A PCI express designated vendor specific extended capability is defined
 * in the section 3.7 of Intel scalable I/O virtualization technical spec
 * for system software and tools to detect endpoint devices supporting the
 * Intel scalable IO virtualization without host driver dependency.
 *
 * Returns the address of the matching extended capability structure within
 * the device's PCI configuration space or 0 if the device does not support
 * it.
 */
static int siov_find_pci_dvsec(struct pci_dev *pdev)
{
	int pos;
	u16 vendor, id;

	pos = pci_find_next_ext_capability(pdev, 0, 0x23);
	while (pos) {
		pci_read_config_word(pdev, pos + 4, &vendor);
		pci_read_config_word(pdev, pos + 8, &id);
		if (vendor == PCI_VENDOR_ID_INTEL && id == 5)
			return pos;

		pos = pci_find_next_ext_capability(pdev, pos, 0x23);
	}

	return 0;
}

static bool
intel_iommu_dev_has_feat(struct device *dev, enum iommu_dev_features feat)
{
	struct device_domain_info *info = get_domain_info(dev);

	if (feat == IOMMU_DEV_FEAT_AUX) {
		int ret;

		if (!dev_is_pci(dev) || dmar_disabled ||
		    !scalable_mode_support() || !pasid_mode_support())
			return false;

		ret = pci_pasid_features(to_pci_dev(dev));
		if (ret < 0)
			return false;

		return !!siov_find_pci_dvsec(to_pci_dev(dev));
	}

	if (feat == IOMMU_DEV_FEAT_IOPF)
		return info && info->pri_supported;

	if (feat == IOMMU_DEV_FEAT_SVA)
		return info && (info->iommu->flags & VTD_FLAG_SVM_CAPABLE) &&
			info->pasid_supported && info->pri_supported &&
			info->ats_supported;

	return false;
}

static int
intel_iommu_dev_enable_feat(struct device *dev, enum iommu_dev_features feat)
{
	if (feat == IOMMU_DEV_FEAT_AUX)
		return intel_iommu_enable_auxd(dev);

	if (feat == IOMMU_DEV_FEAT_IOPF)
		return intel_iommu_dev_has_feat(dev, feat) ? 0 : -ENODEV;

	if (feat == IOMMU_DEV_FEAT_SVA) {
		struct device_domain_info *info = get_domain_info(dev);

		if (!info)
			return -EINVAL;

		if (!info->pasid_enabled || !info->pri_enabled || !info->ats_enabled)
			return -EINVAL;

		if (info->iommu->flags & VTD_FLAG_SVM_CAPABLE)
			return 0;
	}

	return -ENODEV;
}

static int
intel_iommu_dev_disable_feat(struct device *dev, enum iommu_dev_features feat)
{
	if (feat == IOMMU_DEV_FEAT_AUX)
		return intel_iommu_disable_auxd(dev);

	return -ENODEV;
}

static bool
intel_iommu_dev_feat_enabled(struct device *dev, enum iommu_dev_features feat)
{
	struct device_domain_info *info = get_domain_info(dev);

	if (feat == IOMMU_DEV_FEAT_AUX)
		return scalable_mode_support() && info && info->auxd_enabled;

	return false;
}

static int
intel_iommu_aux_get_pasid(struct iommu_domain *domain, struct device *dev)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);

	return dmar_domain->default_pasid > 0 ?
			dmar_domain->default_pasid : -EINVAL;
}

static bool intel_iommu_is_attach_deferred(struct iommu_domain *domain,
					   struct device *dev)
{
	return attach_deferred(dev);
}

static int
intel_iommu_enable_nesting(struct iommu_domain *domain)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	unsigned long flags;
	int ret = -ENODEV;

	spin_lock_irqsave(&device_domain_lock, flags);
	if (nested_mode_support() && list_empty(&dmar_domain->devices)) {
		dmar_domain->flags |= DOMAIN_FLAG_NESTING_MODE;
		dmar_domain->flags &= ~DOMAIN_FLAG_USE_FIRST_LEVEL;
		ret = 0;
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return ret;
}

/*
 * Check that the device does not live on an external facing PCI port that is
 * marked as untrusted. Such devices should not be able to apply quirks and
 * thus not be able to bypass the IOMMU restrictions.
 */
static bool risky_device(struct pci_dev *pdev)
{
	if (pdev->untrusted) {
		pci_info(pdev,
			 "Skipping IOMMU quirk for dev [%04X:%04X] on untrusted PCI link\n",
			 pdev->vendor, pdev->device);
		pci_info(pdev, "Please check with your BIOS/Platform vendor about this\n");
		return true;
	}
	return false;
}

static void clflush_sync_map(struct dmar_domain *domain, unsigned long clf_pfn,
			     unsigned long clf_pages)
{
	struct dma_pte *first_pte = NULL, *pte = NULL;
	unsigned long lvl_pages = 0;
	int level = 0;

	while (clf_pages > 0) {
		if (!pte) {
			level = 0;
			pte = pfn_to_dma_pte(domain, clf_pfn, &level);
			if (WARN_ON(!pte))
				return;
			first_pte = pte;
			lvl_pages = lvl_to_nr_pages(level);
		}

		if (WARN_ON(!lvl_pages || clf_pages < lvl_pages))
			return;

		clf_pages -= lvl_pages;
		clf_pfn += lvl_pages;
		pte++;

		if (!clf_pages || first_pte_in_page(pte) ||
		    (level > 1 && clf_pages < lvl_pages)) {
			domain_flush_cache(domain, first_pte,
					   (void *)pte - (void *)first_pte);
			pte = NULL;
		}
	}
}

static void intel_iommu_iotlb_sync_map(struct iommu_domain *domain,
				       unsigned long iova, size_t size)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	unsigned long pages = aligned_nrpages(iova, size);
	unsigned long pfn = iova >> VTD_PAGE_SHIFT;
	struct intel_iommu *iommu;
	int iommu_id;

	if (!dmar_domain->iommu_coherency)
		clflush_sync_map(dmar_domain, pfn, pages);

	for_each_domain_iommu(iommu_id, dmar_domain) {
		iommu = g_iommus[iommu_id];
		__mapping_notify_one(iommu, dmar_domain, pfn, pages);
	}
}

const struct iommu_ops intel_iommu_ops = {
	.capable		= intel_iommu_capable,
	.domain_alloc		= intel_iommu_domain_alloc,
	.domain_free		= intel_iommu_domain_free,
	.enable_nesting		= intel_iommu_enable_nesting,
	.attach_dev		= intel_iommu_attach_device,
	.detach_dev		= intel_iommu_detach_device,
	.aux_attach_dev		= intel_iommu_aux_attach_device,
	.aux_detach_dev		= intel_iommu_aux_detach_device,
	.aux_get_pasid		= intel_iommu_aux_get_pasid,
	.map			= intel_iommu_map,
	.iotlb_sync_map		= intel_iommu_iotlb_sync_map,
	.unmap			= intel_iommu_unmap,
	.flush_iotlb_all        = intel_flush_iotlb_all,
	.iotlb_sync		= intel_iommu_tlb_sync,
	.iova_to_phys		= intel_iommu_iova_to_phys,
	.probe_device		= intel_iommu_probe_device,
	.probe_finalize		= intel_iommu_probe_finalize,
	.release_device		= intel_iommu_release_device,
	.get_resv_regions	= intel_iommu_get_resv_regions,
	.put_resv_regions	= generic_iommu_put_resv_regions,
	.device_group		= intel_iommu_device_group,
	.dev_has_feat		= intel_iommu_dev_has_feat,
	.dev_feat_enabled	= intel_iommu_dev_feat_enabled,
	.dev_enable_feat	= intel_iommu_dev_enable_feat,
	.dev_disable_feat	= intel_iommu_dev_disable_feat,
	.is_attach_deferred	= intel_iommu_is_attach_deferred,
	.def_domain_type	= device_def_domain_type,
	.pgsize_bitmap		= INTEL_IOMMU_PGSIZES,
#ifdef CONFIG_INTEL_IOMMU_SVM
	.cache_invalidate	= intel_iommu_sva_invalidate,
	.sva_bind_gpasid	= intel_svm_bind_gpasid,
	.sva_unbind_gpasid	= intel_svm_unbind_gpasid,
	.sva_bind		= intel_svm_bind,
	.sva_unbind		= intel_svm_unbind,
	.sva_get_pasid		= intel_svm_get_pasid,
	.page_response		= intel_svm_page_response,
#endif
};

static void quirk_iommu_igfx(struct pci_dev *dev)
{
	if (risky_device(dev))
		return;

	pci_info(dev, "Disabling IOMMU for graphics on this chipset\n");
	dmar_map_gfx = 0;
}

/* G4x/GM45 integrated gfx dmar support is totally busted. */
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2a40, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e00, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e10, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e20, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e30, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e40, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e90, quirk_iommu_igfx);

/* Broadwell igfx malfunctions with dmar */
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1606, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x160B, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x160E, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1602, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x160A, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x160D, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1616, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x161B, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x161E, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1612, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x161A, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x161D, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1626, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x162B, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x162E, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1622, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x162A, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x162D, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1636, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x163B, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x163E, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x1632, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x163A, quirk_iommu_igfx);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x163D, quirk_iommu_igfx);

static void quirk_iommu_rwbf(struct pci_dev *dev)
{
	if (risky_device(dev))
		return;

	/*
	 * Mobile 4 Series Chipset neglects to set RWBF capability,
	 * but needs it. Same seems to hold for the desktop versions.
	 */
	pci_info(dev, "Forcing write-buffer flush capability\n");
	rwbf_quirk = 1;
}

DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2a40, quirk_iommu_rwbf);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e00, quirk_iommu_rwbf);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e10, quirk_iommu_rwbf);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e20, quirk_iommu_rwbf);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e30, quirk_iommu_rwbf);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e40, quirk_iommu_rwbf);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x2e90, quirk_iommu_rwbf);

#define GGC 0x52
#define GGC_MEMORY_SIZE_MASK	(0xf << 8)
#define GGC_MEMORY_SIZE_NONE	(0x0 << 8)
#define GGC_MEMORY_SIZE_1M	(0x1 << 8)
#define GGC_MEMORY_SIZE_2M	(0x3 << 8)
#define GGC_MEMORY_VT_ENABLED	(0x8 << 8)
#define GGC_MEMORY_SIZE_2M_VT	(0x9 << 8)
#define GGC_MEMORY_SIZE_3M_VT	(0xa << 8)
#define GGC_MEMORY_SIZE_4M_VT	(0xb << 8)

static void quirk_calpella_no_shadow_gtt(struct pci_dev *dev)
{
	unsigned short ggc;

	if (risky_device(dev))
		return;

	if (pci_read_config_word(dev, GGC, &ggc))
		return;

	if (!(ggc & GGC_MEMORY_VT_ENABLED)) {
		pci_info(dev, "BIOS has allocated no shadow GTT; disabling IOMMU for graphics\n");
		dmar_map_gfx = 0;
	} else if (dmar_map_gfx) {
		/* we have to ensure the gfx device is idle before we flush */
		pci_info(dev, "Disabling batched IOTLB flush on Ironlake\n");
		intel_iommu_strict = 1;
       }
}
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x0040, quirk_calpella_no_shadow_gtt);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x0044, quirk_calpella_no_shadow_gtt);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x0062, quirk_calpella_no_shadow_gtt);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, 0x006a, quirk_calpella_no_shadow_gtt);

static void quirk_igfx_skip_te_disable(struct pci_dev *dev)
{
	unsigned short ver;

	if (!IS_GFX_DEVICE(dev))
		return;

	ver = (dev->device >> 8) & 0xff;
	if (ver != 0x45 && ver != 0x46 && ver != 0x4c &&
	    ver != 0x4e && ver != 0x8a && ver != 0x98 &&
	    ver != 0x9a)
		return;

	if (risky_device(dev))
		return;

	pci_info(dev, "Skip IOMMU disabling for graphics\n");
	iommu_skip_te_disable = 1;
}
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, PCI_ANY_ID, quirk_igfx_skip_te_disable);

/* On Tylersburg chipsets, some BIOSes have been known to enable the
   ISOCH DMAR unit for the Azalia sound device, but not give it any
   TLB entries, which causes it to deadlock. Check for that.  We do
   this in a function called from init_dmars(), instead of in a PCI
   quirk, because we don't want to print the obnoxious "BIOS broken"
   message if VT-d is actually disabled.
*/
static void __init check_tylersburg_isoch(void)
{
	struct pci_dev *pdev;
	uint32_t vtisochctrl;

	/* If there's no Azalia in the system anyway, forget it. */
	pdev = pci_get_device(PCI_VENDOR_ID_INTEL, 0x3a3e, NULL);
	if (!pdev)
		return;

	if (risky_device(pdev)) {
		pci_dev_put(pdev);
		return;
	}

	pci_dev_put(pdev);

	/* System Management Registers. Might be hidden, in which case
	   we can't do the sanity check. But that's OK, because the
	   known-broken BIOSes _don't_ actually hide it, so far. */
	pdev = pci_get_device(PCI_VENDOR_ID_INTEL, 0x342e, NULL);
	if (!pdev)
		return;

	if (risky_device(pdev)) {
		pci_dev_put(pdev);
		return;
	}

	if (pci_read_config_dword(pdev, 0x188, &vtisochctrl)) {
		pci_dev_put(pdev);
		return;
	}

	pci_dev_put(pdev);

	/* If Azalia DMA is routed to the non-isoch DMAR unit, fine. */
	if (vtisochctrl & 1)
		return;

	/* Drop all bits other than the number of TLB entries */
	vtisochctrl &= 0x1c;

	/* If we have the recommended number of TLB entries (16), fine. */
	if (vtisochctrl == 0x10)
		return;

	/* Zero TLB entries? You get to ride the short bus to school. */
	if (!vtisochctrl) {
		WARN(1, "Your BIOS is broken; DMA routed to ISOCH DMAR unit but no TLB space.\n"
		     "BIOS vendor: %s; Ver: %s; Product Version: %s\n",
		     dmi_get_system_info(DMI_BIOS_VENDOR),
		     dmi_get_system_info(DMI_BIOS_VERSION),
		     dmi_get_system_info(DMI_PRODUCT_VERSION));
		iommu_identity_mapping |= IDENTMAP_AZALIA;
		return;
	}

	pr_warn("Recommended TLB entries for ISOCH unit is 16; your BIOS set %d\n",
	       vtisochctrl);
}

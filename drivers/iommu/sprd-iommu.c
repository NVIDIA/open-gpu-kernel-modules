// SPDX-License-Identifier: GPL-2.0-only
/*
 * Unisoc IOMMU driver
 *
 * Copyright (C) 2020 Unisoc, Inc.
 * Author: Chunyan Zhang <chunyan.zhang@unisoc.com>
 */

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/dma-iommu.h>
#include <linux/dma-mapping.h>
#include <linux/errno.h>
#include <linux/iommu.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#define SPRD_IOMMU_PAGE_SHIFT	12
#define SPRD_IOMMU_PAGE_SIZE	SZ_4K

#define SPRD_EX_CFG		0x0
#define SPRD_IOMMU_VAOR_BYPASS	BIT(4)
#define SPRD_IOMMU_GATE_EN	BIT(1)
#define SPRD_IOMMU_EN		BIT(0)
#define SPRD_EX_UPDATE		0x4
#define SPRD_EX_FIRST_VPN	0x8
#define SPRD_EX_VPN_RANGE	0xc
#define SPRD_EX_FIRST_PPN	0x10
#define SPRD_EX_DEFAULT_PPN	0x14

#define SPRD_IOMMU_VERSION	0x0
#define SPRD_VERSION_MASK	GENMASK(15, 8)
#define SPRD_VERSION_SHIFT	0x8
#define SPRD_VAU_CFG		0x4
#define SPRD_VAU_UPDATE		0x8
#define SPRD_VAU_AUTH_CFG	0xc
#define SPRD_VAU_FIRST_PPN	0x10
#define SPRD_VAU_DEFAULT_PPN_RD	0x14
#define SPRD_VAU_DEFAULT_PPN_WR	0x18
#define SPRD_VAU_FIRST_VPN	0x1c
#define SPRD_VAU_VPN_RANGE	0x20

enum sprd_iommu_version {
	SPRD_IOMMU_EX,
	SPRD_IOMMU_VAU,
};

/*
 * struct sprd_iommu_device - high-level sprd IOMMU device representation,
 * including hardware information and configuration, also driver data, etc
 *
 * @ver: sprd IOMMU IP version
 * @prot_page_va: protect page base virtual address
 * @prot_page_pa: protect page base physical address, data would be
 *		  written to here while translation fault
 * @base: mapped base address for accessing registers
 * @dev: pointer to basic device structure
 * @iommu: IOMMU core representation
 * @group: IOMMU group
 * @eb: gate clock which controls IOMMU access
 */
struct sprd_iommu_device {
	enum sprd_iommu_version	ver;
	u32			*prot_page_va;
	dma_addr_t		prot_page_pa;
	void __iomem		*base;
	struct device		*dev;
	struct iommu_device	iommu;
	struct iommu_group	*group;
	struct clk		*eb;
};

struct sprd_iommu_domain {
	spinlock_t		pgtlock; /* lock for page table */
	struct iommu_domain	domain;
	u32			*pgt_va; /* page table virtual address base */
	dma_addr_t		pgt_pa; /* page table physical address base */
	struct sprd_iommu_device	*sdev;
};

static const struct iommu_ops sprd_iommu_ops;

static struct sprd_iommu_domain *to_sprd_domain(struct iommu_domain *dom)
{
	return container_of(dom, struct sprd_iommu_domain, domain);
}

static inline void
sprd_iommu_write(struct sprd_iommu_device *sdev, unsigned int reg, u32 val)
{
	writel_relaxed(val, sdev->base + reg);
}

static inline u32
sprd_iommu_read(struct sprd_iommu_device *sdev, unsigned int reg)
{
	return readl_relaxed(sdev->base + reg);
}

static inline void
sprd_iommu_update_bits(struct sprd_iommu_device *sdev, unsigned int reg,
		  u32 mask, u32 shift, u32 val)
{
	u32 t = sprd_iommu_read(sdev, reg);

	t = (t & (~(mask << shift))) | ((val & mask) << shift);
	sprd_iommu_write(sdev, reg, t);
}

static inline int
sprd_iommu_get_version(struct sprd_iommu_device *sdev)
{
	int ver = (sprd_iommu_read(sdev, SPRD_IOMMU_VERSION) &
		   SPRD_VERSION_MASK) >> SPRD_VERSION_SHIFT;

	switch (ver) {
	case SPRD_IOMMU_EX:
	case SPRD_IOMMU_VAU:
		return ver;
	default:
		return -EINVAL;
	}
}

static size_t
sprd_iommu_pgt_size(struct iommu_domain *domain)
{
	return ((domain->geometry.aperture_end -
		 domain->geometry.aperture_start + 1) >>
		SPRD_IOMMU_PAGE_SHIFT) * sizeof(u32);
}

static struct iommu_domain *sprd_iommu_domain_alloc(unsigned int domain_type)
{
	struct sprd_iommu_domain *dom;

	if (domain_type != IOMMU_DOMAIN_DMA && domain_type != IOMMU_DOMAIN_UNMANAGED)
		return NULL;

	dom = kzalloc(sizeof(*dom), GFP_KERNEL);
	if (!dom)
		return NULL;

	if (iommu_get_dma_cookie(&dom->domain)) {
		kfree(dom);
		return NULL;
	}

	spin_lock_init(&dom->pgtlock);

	dom->domain.geometry.aperture_start = 0;
	dom->domain.geometry.aperture_end = SZ_256M - 1;

	return &dom->domain;
}

static void sprd_iommu_domain_free(struct iommu_domain *domain)
{
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);

	iommu_put_dma_cookie(domain);
	kfree(dom);
}

static void sprd_iommu_first_vpn(struct sprd_iommu_domain *dom)
{
	struct sprd_iommu_device *sdev = dom->sdev;
	u32 val;
	unsigned int reg;

	if (sdev->ver == SPRD_IOMMU_EX)
		reg = SPRD_EX_FIRST_VPN;
	else
		reg = SPRD_VAU_FIRST_VPN;

	val = dom->domain.geometry.aperture_start >> SPRD_IOMMU_PAGE_SHIFT;
	sprd_iommu_write(sdev, reg, val);
}

static void sprd_iommu_vpn_range(struct sprd_iommu_domain *dom)
{
	struct sprd_iommu_device *sdev = dom->sdev;
	u32 val;
	unsigned int reg;

	if (sdev->ver == SPRD_IOMMU_EX)
		reg = SPRD_EX_VPN_RANGE;
	else
		reg = SPRD_VAU_VPN_RANGE;

	val = (dom->domain.geometry.aperture_end -
	       dom->domain.geometry.aperture_start) >> SPRD_IOMMU_PAGE_SHIFT;
	sprd_iommu_write(sdev, reg, val);
}

static void sprd_iommu_first_ppn(struct sprd_iommu_domain *dom)
{
	u32 val = dom->pgt_pa >> SPRD_IOMMU_PAGE_SHIFT;
	struct sprd_iommu_device *sdev = dom->sdev;
	unsigned int reg;

	if (sdev->ver == SPRD_IOMMU_EX)
		reg = SPRD_EX_FIRST_PPN;
	else
		reg = SPRD_VAU_FIRST_PPN;

	sprd_iommu_write(sdev, reg, val);
}

static void sprd_iommu_default_ppn(struct sprd_iommu_device *sdev)
{
	u32 val = sdev->prot_page_pa >> SPRD_IOMMU_PAGE_SHIFT;

	if (sdev->ver == SPRD_IOMMU_EX) {
		sprd_iommu_write(sdev, SPRD_EX_DEFAULT_PPN, val);
	} else if (sdev->ver == SPRD_IOMMU_VAU) {
		sprd_iommu_write(sdev, SPRD_VAU_DEFAULT_PPN_RD, val);
		sprd_iommu_write(sdev, SPRD_VAU_DEFAULT_PPN_WR, val);
	}
}

static void sprd_iommu_hw_en(struct sprd_iommu_device *sdev, bool en)
{
	unsigned int reg_cfg;
	u32 mask, val;

	if (sdev->ver == SPRD_IOMMU_EX)
		reg_cfg = SPRD_EX_CFG;
	else
		reg_cfg = SPRD_VAU_CFG;

	mask = SPRD_IOMMU_EN | SPRD_IOMMU_GATE_EN;
	val = en ? mask : 0;
	sprd_iommu_update_bits(sdev, reg_cfg, mask, 0, val);
}

static int sprd_iommu_attach_device(struct iommu_domain *domain,
				    struct device *dev)
{
	struct sprd_iommu_device *sdev = dev_iommu_priv_get(dev);
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);
	size_t pgt_size = sprd_iommu_pgt_size(domain);

	if (dom->sdev) {
		pr_err("There's already a device attached to this domain.\n");
		return -EINVAL;
	}

	dom->pgt_va = dma_alloc_coherent(sdev->dev, pgt_size, &dom->pgt_pa, GFP_KERNEL);
	if (!dom->pgt_va)
		return -ENOMEM;

	dom->sdev = sdev;

	sprd_iommu_first_ppn(dom);
	sprd_iommu_first_vpn(dom);
	sprd_iommu_vpn_range(dom);
	sprd_iommu_default_ppn(sdev);
	sprd_iommu_hw_en(sdev, true);

	return 0;
}

static void sprd_iommu_detach_device(struct iommu_domain *domain,
					     struct device *dev)
{
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);
	struct sprd_iommu_device *sdev = dom->sdev;
	size_t pgt_size = sprd_iommu_pgt_size(domain);

	if (!sdev)
		return;

	dma_free_coherent(sdev->dev, pgt_size, dom->pgt_va, dom->pgt_pa);
	sprd_iommu_hw_en(sdev, false);
	dom->sdev = NULL;
}

static int sprd_iommu_map(struct iommu_domain *domain, unsigned long iova,
			  phys_addr_t paddr, size_t size, int prot, gfp_t gfp)
{
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);
	unsigned int page_num = size >> SPRD_IOMMU_PAGE_SHIFT;
	unsigned long flags;
	unsigned int i;
	u32 *pgt_base_iova;
	u32 pabase = (u32)paddr;
	unsigned long start = domain->geometry.aperture_start;
	unsigned long end = domain->geometry.aperture_end;

	if (!dom->sdev) {
		pr_err("No sprd_iommu_device attached to the domain\n");
		return -EINVAL;
	}

	if (iova < start || (iova + size) > (end + 1)) {
		dev_err(dom->sdev->dev, "(iova(0x%lx) + size(%zx)) are not in the range!\n",
			iova, size);
		return -EINVAL;
	}

	pgt_base_iova = dom->pgt_va + ((iova - start) >> SPRD_IOMMU_PAGE_SHIFT);

	spin_lock_irqsave(&dom->pgtlock, flags);
	for (i = 0; i < page_num; i++) {
		pgt_base_iova[i] = pabase >> SPRD_IOMMU_PAGE_SHIFT;
		pabase += SPRD_IOMMU_PAGE_SIZE;
	}
	spin_unlock_irqrestore(&dom->pgtlock, flags);

	return 0;
}

static size_t sprd_iommu_unmap(struct iommu_domain *domain, unsigned long iova,
			size_t size, struct iommu_iotlb_gather *iotlb_gather)
{
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);
	unsigned long flags;
	u32 *pgt_base_iova;
	unsigned int page_num = size >> SPRD_IOMMU_PAGE_SHIFT;
	unsigned long start = domain->geometry.aperture_start;
	unsigned long end = domain->geometry.aperture_end;

	if (iova < start || (iova + size) > (end + 1))
		return -EINVAL;

	pgt_base_iova = dom->pgt_va + ((iova - start) >> SPRD_IOMMU_PAGE_SHIFT);

	spin_lock_irqsave(&dom->pgtlock, flags);
	memset(pgt_base_iova, 0, page_num * sizeof(u32));
	spin_unlock_irqrestore(&dom->pgtlock, flags);

	return 0;
}

static void sprd_iommu_sync_map(struct iommu_domain *domain,
				unsigned long iova, size_t size)
{
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);
	unsigned int reg;

	if (dom->sdev->ver == SPRD_IOMMU_EX)
		reg = SPRD_EX_UPDATE;
	else
		reg = SPRD_VAU_UPDATE;

	/* clear IOMMU TLB buffer after page table updated */
	sprd_iommu_write(dom->sdev, reg, 0xffffffff);
}

static void sprd_iommu_sync(struct iommu_domain *domain,
			    struct iommu_iotlb_gather *iotlb_gather)
{
	sprd_iommu_sync_map(domain, 0, 0);
}

static phys_addr_t sprd_iommu_iova_to_phys(struct iommu_domain *domain,
					   dma_addr_t iova)
{
	struct sprd_iommu_domain *dom = to_sprd_domain(domain);
	unsigned long flags;
	phys_addr_t pa;
	unsigned long start = domain->geometry.aperture_start;
	unsigned long end = domain->geometry.aperture_end;

	if (WARN_ON(iova < start || iova > end))
		return 0;

	spin_lock_irqsave(&dom->pgtlock, flags);
	pa = *(dom->pgt_va + ((iova - start) >> SPRD_IOMMU_PAGE_SHIFT));
	pa = (pa << SPRD_IOMMU_PAGE_SHIFT) + ((iova - start) & (SPRD_IOMMU_PAGE_SIZE - 1));
	spin_unlock_irqrestore(&dom->pgtlock, flags);

	return pa;
}

static struct iommu_device *sprd_iommu_probe_device(struct device *dev)
{
	struct iommu_fwspec *fwspec = dev_iommu_fwspec_get(dev);
	struct sprd_iommu_device *sdev;

	if (!fwspec || fwspec->ops != &sprd_iommu_ops)
		return ERR_PTR(-ENODEV);

	sdev = dev_iommu_priv_get(dev);

	return &sdev->iommu;
}

static void sprd_iommu_release_device(struct device *dev)
{
	struct iommu_fwspec *fwspec = dev_iommu_fwspec_get(dev);

	if (!fwspec || fwspec->ops != &sprd_iommu_ops)
		return;

	iommu_fwspec_free(dev);
}

static struct iommu_group *sprd_iommu_device_group(struct device *dev)
{
	struct sprd_iommu_device *sdev = dev_iommu_priv_get(dev);

	return iommu_group_ref_get(sdev->group);
}

static int sprd_iommu_of_xlate(struct device *dev, struct of_phandle_args *args)
{
	struct platform_device *pdev;

	if (!dev_iommu_priv_get(dev)) {
		pdev = of_find_device_by_node(args->np);
		dev_iommu_priv_set(dev, platform_get_drvdata(pdev));
		platform_device_put(pdev);
	}

	return 0;
}


static const struct iommu_ops sprd_iommu_ops = {
	.domain_alloc	= sprd_iommu_domain_alloc,
	.domain_free	= sprd_iommu_domain_free,
	.attach_dev	= sprd_iommu_attach_device,
	.detach_dev	= sprd_iommu_detach_device,
	.map		= sprd_iommu_map,
	.unmap		= sprd_iommu_unmap,
	.iotlb_sync_map	= sprd_iommu_sync_map,
	.iotlb_sync	= sprd_iommu_sync,
	.iova_to_phys	= sprd_iommu_iova_to_phys,
	.probe_device	= sprd_iommu_probe_device,
	.release_device	= sprd_iommu_release_device,
	.device_group	= sprd_iommu_device_group,
	.of_xlate	= sprd_iommu_of_xlate,
	.pgsize_bitmap	= ~0UL << SPRD_IOMMU_PAGE_SHIFT,
	.owner		= THIS_MODULE,
};

static const struct of_device_id sprd_iommu_of_match[] = {
	{ .compatible = "sprd,iommu-v1" },
	{ },
};
MODULE_DEVICE_TABLE(of, sprd_iommu_of_match);

/*
 * Clock is not required, access to some of IOMMUs is controlled by gate
 * clk, enabled clocks for that kind of IOMMUs before accessing.
 * Return 0 for success or no clocks found.
 */
static int sprd_iommu_clk_enable(struct sprd_iommu_device *sdev)
{
	struct clk *eb;

	eb = devm_clk_get_optional(sdev->dev, NULL);
	if (!eb)
		return 0;

	if (IS_ERR(eb))
		return PTR_ERR(eb);

	sdev->eb = eb;
	return clk_prepare_enable(eb);
}

static void sprd_iommu_clk_disable(struct sprd_iommu_device *sdev)
{
	if (sdev->eb)
		clk_disable_unprepare(sdev->eb);
}

static int sprd_iommu_probe(struct platform_device *pdev)
{
	struct sprd_iommu_device *sdev;
	struct device *dev = &pdev->dev;
	void __iomem *base;
	int ret;

	sdev = devm_kzalloc(dev, sizeof(*sdev), GFP_KERNEL);
	if (!sdev)
		return -ENOMEM;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base)) {
		dev_err(dev, "Failed to get ioremap resource.\n");
		return PTR_ERR(base);
	}
	sdev->base = base;

	sdev->prot_page_va = dma_alloc_coherent(dev, SPRD_IOMMU_PAGE_SIZE,
						&sdev->prot_page_pa, GFP_KERNEL);
	if (!sdev->prot_page_va)
		return -ENOMEM;

	platform_set_drvdata(pdev, sdev);
	sdev->dev = dev;

	/* All the client devices are in the same iommu-group */
	sdev->group = iommu_group_alloc();
	if (IS_ERR(sdev->group)) {
		ret = PTR_ERR(sdev->group);
		goto free_page;
	}

	ret = iommu_device_sysfs_add(&sdev->iommu, dev, NULL, dev_name(dev));
	if (ret)
		goto put_group;

	ret = iommu_device_register(&sdev->iommu, &sprd_iommu_ops, dev);
	if (ret)
		goto remove_sysfs;

	if (!iommu_present(&platform_bus_type))
		bus_set_iommu(&platform_bus_type, &sprd_iommu_ops);

	ret = sprd_iommu_clk_enable(sdev);
	if (ret)
		goto unregister_iommu;

	ret = sprd_iommu_get_version(sdev);
	if (ret < 0) {
		dev_err(dev, "IOMMU version(%d) is invalid.\n", ret);
		goto disable_clk;
	}
	sdev->ver = ret;

	return 0;

disable_clk:
	sprd_iommu_clk_disable(sdev);
unregister_iommu:
	iommu_device_unregister(&sdev->iommu);
remove_sysfs:
	iommu_device_sysfs_remove(&sdev->iommu);
put_group:
	iommu_group_put(sdev->group);
free_page:
	dma_free_coherent(sdev->dev, SPRD_IOMMU_PAGE_SIZE, sdev->prot_page_va, sdev->prot_page_pa);
	return ret;
}

static int sprd_iommu_remove(struct platform_device *pdev)
{
	struct sprd_iommu_device *sdev = platform_get_drvdata(pdev);

	dma_free_coherent(sdev->dev, SPRD_IOMMU_PAGE_SIZE, sdev->prot_page_va, sdev->prot_page_pa);

	iommu_group_put(sdev->group);
	sdev->group = NULL;

	bus_set_iommu(&platform_bus_type, NULL);

	platform_set_drvdata(pdev, NULL);
	iommu_device_sysfs_remove(&sdev->iommu);
	iommu_device_unregister(&sdev->iommu);

	return 0;
}

static struct platform_driver sprd_iommu_driver = {
	.driver	= {
		.name		= "sprd-iommu",
		.of_match_table	= sprd_iommu_of_match,
		.suppress_bind_attrs = true,
	},
	.probe	= sprd_iommu_probe,
	.remove	= sprd_iommu_remove,
};
module_platform_driver(sprd_iommu_driver);

MODULE_DESCRIPTION("IOMMU driver for Unisoc SoCs");
MODULE_ALIAS("platform:sprd-iommu");
MODULE_LICENSE("GPL");

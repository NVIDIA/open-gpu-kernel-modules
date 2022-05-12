// SPDX-License-Identifier: GPL-2.0
/*
 * Intel Platform Monitory Technology Telemetry driver
 *
 * Copyright (c) 2020, Intel Corporation.
 * All Rights Reserved.
 *
 * Author: "Alexander Duyck" <alexander.h.duyck@linux.intel.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/pci.h>

#include "intel_pmt_class.h"

#define PMT_XA_START		0
#define PMT_XA_MAX		INT_MAX
#define PMT_XA_LIMIT		XA_LIMIT(PMT_XA_START, PMT_XA_MAX)

/*
 * Early implementations of PMT on client platforms have some
 * differences from the server platforms (which use the Out Of Band
 * Management Services Module OOBMSM). This list tracks those
 * platforms as needed to handle those differences. Newer client
 * platforms are expected to be fully compatible with server.
 */
static const struct pci_device_id pmt_telem_early_client_pci_ids[] = {
	{ PCI_VDEVICE(INTEL, 0x467d) }, /* ADL */
	{ PCI_VDEVICE(INTEL, 0x490e) }, /* DG1 */
	{ PCI_VDEVICE(INTEL, 0x9a0d) }, /* TGL */
	{ }
};

bool intel_pmt_is_early_client_hw(struct device *dev)
{
	struct pci_dev *parent = to_pci_dev(dev->parent);

	return !!pci_match_id(pmt_telem_early_client_pci_ids, parent);
}
EXPORT_SYMBOL_GPL(intel_pmt_is_early_client_hw);

/*
 * sysfs
 */
static ssize_t
intel_pmt_read(struct file *filp, struct kobject *kobj,
	       struct bin_attribute *attr, char *buf, loff_t off,
	       size_t count)
{
	struct intel_pmt_entry *entry = container_of(attr,
						     struct intel_pmt_entry,
						     pmt_bin_attr);

	if (off < 0)
		return -EINVAL;

	if (off >= entry->size)
		return 0;

	if (count > entry->size - off)
		count = entry->size - off;

	memcpy_fromio(buf, entry->base + off, count);

	return count;
}

static int
intel_pmt_mmap(struct file *filp, struct kobject *kobj,
		struct bin_attribute *attr, struct vm_area_struct *vma)
{
	struct intel_pmt_entry *entry = container_of(attr,
						     struct intel_pmt_entry,
						     pmt_bin_attr);
	unsigned long vsize = vma->vm_end - vma->vm_start;
	struct device *dev = kobj_to_dev(kobj);
	unsigned long phys = entry->base_addr;
	unsigned long pfn = PFN_DOWN(phys);
	unsigned long psize;

	if (vma->vm_flags & (VM_WRITE | VM_MAYWRITE))
		return -EROFS;

	psize = (PFN_UP(entry->base_addr + entry->size) - pfn) * PAGE_SIZE;
	if (vsize > psize) {
		dev_err(dev, "Requested mmap size is too large\n");
		return -EINVAL;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if (io_remap_pfn_range(vma, vma->vm_start, pfn,
		vsize, vma->vm_page_prot))
		return -EAGAIN;

	return 0;
}

static ssize_t
guid_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct intel_pmt_entry *entry = dev_get_drvdata(dev);

	return sprintf(buf, "0x%x\n", entry->guid);
}
static DEVICE_ATTR_RO(guid);

static ssize_t size_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct intel_pmt_entry *entry = dev_get_drvdata(dev);

	return sprintf(buf, "%zu\n", entry->size);
}
static DEVICE_ATTR_RO(size);

static ssize_t
offset_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct intel_pmt_entry *entry = dev_get_drvdata(dev);

	return sprintf(buf, "%lu\n", offset_in_page(entry->base_addr));
}
static DEVICE_ATTR_RO(offset);

static struct attribute *intel_pmt_attrs[] = {
	&dev_attr_guid.attr,
	&dev_attr_size.attr,
	&dev_attr_offset.attr,
	NULL
};
ATTRIBUTE_GROUPS(intel_pmt);

static struct class intel_pmt_class = {
	.name = "intel_pmt",
	.owner = THIS_MODULE,
	.dev_groups = intel_pmt_groups,
};

static int intel_pmt_populate_entry(struct intel_pmt_entry *entry,
				    struct intel_pmt_header *header,
				    struct device *dev,
				    struct resource *disc_res)
{
	struct pci_dev *pci_dev = to_pci_dev(dev->parent);
	u8 bir;

	/*
	 * The base offset should always be 8 byte aligned.
	 *
	 * For non-local access types the lower 3 bits of base offset
	 * contains the index of the base address register where the
	 * telemetry can be found.
	 */
	bir = GET_BIR(header->base_offset);

	/* Local access and BARID only for now */
	switch (header->access_type) {
	case ACCESS_LOCAL:
		if (bir) {
			dev_err(dev,
				"Unsupported BAR index %d for access type %d\n",
				bir, header->access_type);
			return -EINVAL;
		}
		/*
		 * For access_type LOCAL, the base address is as follows:
		 * base address = end of discovery region + base offset
		 */
		entry->base_addr = disc_res->end + 1 + header->base_offset;

		/*
		 * Some hardware use a different calculation for the base address
		 * when access_type == ACCESS_LOCAL. On the these systems
		 * ACCCESS_LOCAL refers to an address in the same BAR as the
		 * header but at a fixed offset. But as the header address was
		 * supplied to the driver, we don't know which BAR it was in.
		 * So search for the bar whose range includes the header address.
		 */
		if (intel_pmt_is_early_client_hw(dev)) {
			int i;

			entry->base_addr = 0;
			for (i = 0; i < 6; i++)
				if (disc_res->start >= pci_resource_start(pci_dev, i) &&
				   (disc_res->start <= pci_resource_end(pci_dev, i))) {
					entry->base_addr = pci_resource_start(pci_dev, i) +
							   header->base_offset;
					break;
				}
			if (!entry->base_addr)
				return -EINVAL;
		}

		break;
	case ACCESS_BARID:
		/*
		 * If another BAR was specified then the base offset
		 * represents the offset within that BAR. SO retrieve the
		 * address from the parent PCI device and add offset.
		 */
		entry->base_addr = pci_resource_start(pci_dev, bir) +
				   GET_ADDRESS(header->base_offset);
		break;
	default:
		dev_err(dev, "Unsupported access type %d\n",
			header->access_type);
		return -EINVAL;
	}

	entry->guid = header->guid;
	entry->size = header->size;

	return 0;
}

static int intel_pmt_dev_register(struct intel_pmt_entry *entry,
				  struct intel_pmt_namespace *ns,
				  struct device *parent)
{
	struct resource res = {0};
	struct device *dev;
	int ret;

	ret = xa_alloc(ns->xa, &entry->devid, entry, PMT_XA_LIMIT, GFP_KERNEL);
	if (ret)
		return ret;

	dev = device_create(&intel_pmt_class, parent, MKDEV(0, 0), entry,
			    "%s%d", ns->name, entry->devid);

	if (IS_ERR(dev)) {
		dev_err(parent, "Could not create %s%d device node\n",
			ns->name, entry->devid);
		ret = PTR_ERR(dev);
		goto fail_dev_create;
	}

	entry->kobj = &dev->kobj;

	if (ns->attr_grp) {
		ret = sysfs_create_group(entry->kobj, ns->attr_grp);
		if (ret)
			goto fail_sysfs;
	}

	/* if size is 0 assume no data buffer, so no file needed */
	if (!entry->size)
		return 0;

	res.start = entry->base_addr;
	res.end = res.start + entry->size - 1;
	res.flags = IORESOURCE_MEM;

	entry->base = devm_ioremap_resource(dev, &res);
	if (IS_ERR(entry->base)) {
		ret = PTR_ERR(entry->base);
		goto fail_ioremap;
	}

	sysfs_bin_attr_init(&entry->pmt_bin_attr);
	entry->pmt_bin_attr.attr.name = ns->name;
	entry->pmt_bin_attr.attr.mode = 0440;
	entry->pmt_bin_attr.mmap = intel_pmt_mmap;
	entry->pmt_bin_attr.read = intel_pmt_read;
	entry->pmt_bin_attr.size = entry->size;

	ret = sysfs_create_bin_file(&dev->kobj, &entry->pmt_bin_attr);
	if (!ret)
		return 0;

fail_ioremap:
	if (ns->attr_grp)
		sysfs_remove_group(entry->kobj, ns->attr_grp);
fail_sysfs:
	device_unregister(dev);
fail_dev_create:
	xa_erase(ns->xa, entry->devid);

	return ret;
}

int intel_pmt_dev_create(struct intel_pmt_entry *entry,
			 struct intel_pmt_namespace *ns,
			 struct platform_device *pdev, int idx)
{
	struct intel_pmt_header header;
	struct resource	*disc_res;
	int ret = -ENODEV;

	disc_res = platform_get_resource(pdev, IORESOURCE_MEM, idx);
	if (!disc_res)
		return ret;

	entry->disc_table = devm_platform_ioremap_resource(pdev, idx);
	if (IS_ERR(entry->disc_table))
		return PTR_ERR(entry->disc_table);

	ret = ns->pmt_header_decode(entry, &header, &pdev->dev);
	if (ret)
		return ret;

	ret = intel_pmt_populate_entry(entry, &header, &pdev->dev, disc_res);
	if (ret)
		return ret;

	return intel_pmt_dev_register(entry, ns, &pdev->dev);

}
EXPORT_SYMBOL_GPL(intel_pmt_dev_create);

void intel_pmt_dev_destroy(struct intel_pmt_entry *entry,
			   struct intel_pmt_namespace *ns)
{
	struct device *dev = kobj_to_dev(entry->kobj);

	if (entry->size)
		sysfs_remove_bin_file(entry->kobj, &entry->pmt_bin_attr);

	if (ns->attr_grp)
		sysfs_remove_group(entry->kobj, ns->attr_grp);

	device_unregister(dev);
	xa_erase(ns->xa, entry->devid);
}
EXPORT_SYMBOL_GPL(intel_pmt_dev_destroy);

static int __init pmt_class_init(void)
{
	return class_register(&intel_pmt_class);
}

static void __exit pmt_class_exit(void)
{
	class_unregister(&intel_pmt_class);
}

module_init(pmt_class_init);
module_exit(pmt_class_exit);

MODULE_AUTHOR("Alexander Duyck <alexander.h.duyck@linux.intel.com>");
MODULE_DESCRIPTION("Intel PMT Class driver");
MODULE_LICENSE("GPL v2");

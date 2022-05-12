// SPDX-License-Identifier: GPL-2.0
/*
 * PCI VPD support
 *
 * Copyright (C) 2010 Broadcom Corporation.
 */

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/export.h>
#include <linux/sched/signal.h>
#include "pci.h"

/* VPD access through PCI 2.2+ VPD capability */

struct pci_vpd_ops {
	ssize_t (*read)(struct pci_dev *dev, loff_t pos, size_t count, void *buf);
	ssize_t (*write)(struct pci_dev *dev, loff_t pos, size_t count, const void *buf);
};

struct pci_vpd {
	const struct pci_vpd_ops *ops;
	struct mutex	lock;
	unsigned int	len;
	u16		flag;
	u8		cap;
	unsigned int	busy:1;
	unsigned int	valid:1;
};

static struct pci_dev *pci_get_func0_dev(struct pci_dev *dev)
{
	return pci_get_slot(dev->bus, PCI_DEVFN(PCI_SLOT(dev->devfn), 0));
}

/**
 * pci_read_vpd - Read one entry from Vital Product Data
 * @dev:	pci device struct
 * @pos:	offset in vpd space
 * @count:	number of bytes to read
 * @buf:	pointer to where to store result
 */
ssize_t pci_read_vpd(struct pci_dev *dev, loff_t pos, size_t count, void *buf)
{
	if (!dev->vpd || !dev->vpd->ops)
		return -ENODEV;
	return dev->vpd->ops->read(dev, pos, count, buf);
}
EXPORT_SYMBOL(pci_read_vpd);

/**
 * pci_write_vpd - Write entry to Vital Product Data
 * @dev:	pci device struct
 * @pos:	offset in vpd space
 * @count:	number of bytes to write
 * @buf:	buffer containing write data
 */
ssize_t pci_write_vpd(struct pci_dev *dev, loff_t pos, size_t count, const void *buf)
{
	if (!dev->vpd || !dev->vpd->ops)
		return -ENODEV;
	return dev->vpd->ops->write(dev, pos, count, buf);
}
EXPORT_SYMBOL(pci_write_vpd);

#define PCI_VPD_MAX_SIZE (PCI_VPD_ADDR_MASK + 1)

/**
 * pci_vpd_size - determine actual size of Vital Product Data
 * @dev:	pci device struct
 * @old_size:	current assumed size, also maximum allowed size
 */
static size_t pci_vpd_size(struct pci_dev *dev, size_t old_size)
{
	size_t off = 0;
	unsigned char header[1+2];	/* 1 byte tag, 2 bytes length */

	while (off < old_size && pci_read_vpd(dev, off, 1, header) == 1) {
		unsigned char tag;

		if (!header[0] && !off) {
			pci_info(dev, "Invalid VPD tag 00, assume missing optional VPD EPROM\n");
			return 0;
		}

		if (header[0] & PCI_VPD_LRDT) {
			/* Large Resource Data Type Tag */
			tag = pci_vpd_lrdt_tag(header);
			/* Only read length from known tag items */
			if ((tag == PCI_VPD_LTIN_ID_STRING) ||
			    (tag == PCI_VPD_LTIN_RO_DATA) ||
			    (tag == PCI_VPD_LTIN_RW_DATA)) {
				if (pci_read_vpd(dev, off+1, 2,
						 &header[1]) != 2) {
					pci_warn(dev, "invalid large VPD tag %02x size at offset %zu",
						 tag, off + 1);
					return 0;
				}
				off += PCI_VPD_LRDT_TAG_SIZE +
					pci_vpd_lrdt_size(header);
			}
		} else {
			/* Short Resource Data Type Tag */
			off += PCI_VPD_SRDT_TAG_SIZE +
				pci_vpd_srdt_size(header);
			tag = pci_vpd_srdt_tag(header);
		}

		if (tag == PCI_VPD_STIN_END)	/* End tag descriptor */
			return off;

		if ((tag != PCI_VPD_LTIN_ID_STRING) &&
		    (tag != PCI_VPD_LTIN_RO_DATA) &&
		    (tag != PCI_VPD_LTIN_RW_DATA)) {
			pci_warn(dev, "invalid %s VPD tag %02x at offset %zu",
				 (header[0] & PCI_VPD_LRDT) ? "large" : "short",
				 tag, off);
			return 0;
		}
	}
	return 0;
}

/*
 * Wait for last operation to complete.
 * This code has to spin since there is no other notification from the PCI
 * hardware. Since the VPD is often implemented by serial attachment to an
 * EEPROM, it may take many milliseconds to complete.
 *
 * Returns 0 on success, negative values indicate error.
 */
static int pci_vpd_wait(struct pci_dev *dev)
{
	struct pci_vpd *vpd = dev->vpd;
	unsigned long timeout = jiffies + msecs_to_jiffies(125);
	unsigned long max_sleep = 16;
	u16 status;
	int ret;

	if (!vpd->busy)
		return 0;

	do {
		ret = pci_user_read_config_word(dev, vpd->cap + PCI_VPD_ADDR,
						&status);
		if (ret < 0)
			return ret;

		if ((status & PCI_VPD_ADDR_F) == vpd->flag) {
			vpd->busy = 0;
			return 0;
		}

		if (fatal_signal_pending(current))
			return -EINTR;

		if (time_after(jiffies, timeout))
			break;

		usleep_range(10, max_sleep);
		if (max_sleep < 1024)
			max_sleep *= 2;
	} while (true);

	pci_warn(dev, "VPD access failed.  This is likely a firmware bug on this device.  Contact the card vendor for a firmware update\n");
	return -ETIMEDOUT;
}

static ssize_t pci_vpd_read(struct pci_dev *dev, loff_t pos, size_t count,
			    void *arg)
{
	struct pci_vpd *vpd = dev->vpd;
	int ret;
	loff_t end = pos + count;
	u8 *buf = arg;

	if (pos < 0)
		return -EINVAL;

	if (!vpd->valid) {
		vpd->valid = 1;
		vpd->len = pci_vpd_size(dev, vpd->len);
	}

	if (vpd->len == 0)
		return -EIO;

	if (pos > vpd->len)
		return 0;

	if (end > vpd->len) {
		end = vpd->len;
		count = end - pos;
	}

	if (mutex_lock_killable(&vpd->lock))
		return -EINTR;

	ret = pci_vpd_wait(dev);
	if (ret < 0)
		goto out;

	while (pos < end) {
		u32 val;
		unsigned int i, skip;

		ret = pci_user_write_config_word(dev, vpd->cap + PCI_VPD_ADDR,
						 pos & ~3);
		if (ret < 0)
			break;
		vpd->busy = 1;
		vpd->flag = PCI_VPD_ADDR_F;
		ret = pci_vpd_wait(dev);
		if (ret < 0)
			break;

		ret = pci_user_read_config_dword(dev, vpd->cap + PCI_VPD_DATA, &val);
		if (ret < 0)
			break;

		skip = pos & 3;
		for (i = 0;  i < sizeof(u32); i++) {
			if (i >= skip) {
				*buf++ = val;
				if (++pos == end)
					break;
			}
			val >>= 8;
		}
	}
out:
	mutex_unlock(&vpd->lock);
	return ret ? ret : count;
}

static ssize_t pci_vpd_write(struct pci_dev *dev, loff_t pos, size_t count,
			     const void *arg)
{
	struct pci_vpd *vpd = dev->vpd;
	const u8 *buf = arg;
	loff_t end = pos + count;
	int ret = 0;

	if (pos < 0 || (pos & 3) || (count & 3))
		return -EINVAL;

	if (!vpd->valid) {
		vpd->valid = 1;
		vpd->len = pci_vpd_size(dev, vpd->len);
	}

	if (vpd->len == 0)
		return -EIO;

	if (end > vpd->len)
		return -EINVAL;

	if (mutex_lock_killable(&vpd->lock))
		return -EINTR;

	ret = pci_vpd_wait(dev);
	if (ret < 0)
		goto out;

	while (pos < end) {
		u32 val;

		val = *buf++;
		val |= *buf++ << 8;
		val |= *buf++ << 16;
		val |= *buf++ << 24;

		ret = pci_user_write_config_dword(dev, vpd->cap + PCI_VPD_DATA, val);
		if (ret < 0)
			break;
		ret = pci_user_write_config_word(dev, vpd->cap + PCI_VPD_ADDR,
						 pos | PCI_VPD_ADDR_F);
		if (ret < 0)
			break;

		vpd->busy = 1;
		vpd->flag = 0;
		ret = pci_vpd_wait(dev);
		if (ret < 0)
			break;

		pos += sizeof(u32);
	}
out:
	mutex_unlock(&vpd->lock);
	return ret ? ret : count;
}

static const struct pci_vpd_ops pci_vpd_ops = {
	.read = pci_vpd_read,
	.write = pci_vpd_write,
};

static ssize_t pci_vpd_f0_read(struct pci_dev *dev, loff_t pos, size_t count,
			       void *arg)
{
	struct pci_dev *tdev = pci_get_func0_dev(dev);
	ssize_t ret;

	if (!tdev)
		return -ENODEV;

	ret = pci_read_vpd(tdev, pos, count, arg);
	pci_dev_put(tdev);
	return ret;
}

static ssize_t pci_vpd_f0_write(struct pci_dev *dev, loff_t pos, size_t count,
				const void *arg)
{
	struct pci_dev *tdev = pci_get_func0_dev(dev);
	ssize_t ret;

	if (!tdev)
		return -ENODEV;

	ret = pci_write_vpd(tdev, pos, count, arg);
	pci_dev_put(tdev);
	return ret;
}

static const struct pci_vpd_ops pci_vpd_f0_ops = {
	.read = pci_vpd_f0_read,
	.write = pci_vpd_f0_write,
};

void pci_vpd_init(struct pci_dev *dev)
{
	struct pci_vpd *vpd;
	u8 cap;

	cap = pci_find_capability(dev, PCI_CAP_ID_VPD);
	if (!cap)
		return;

	vpd = kzalloc(sizeof(*vpd), GFP_ATOMIC);
	if (!vpd)
		return;

	vpd->len = PCI_VPD_MAX_SIZE;
	if (dev->dev_flags & PCI_DEV_FLAGS_VPD_REF_F0)
		vpd->ops = &pci_vpd_f0_ops;
	else
		vpd->ops = &pci_vpd_ops;
	mutex_init(&vpd->lock);
	vpd->cap = cap;
	vpd->busy = 0;
	vpd->valid = 0;
	dev->vpd = vpd;
}

void pci_vpd_release(struct pci_dev *dev)
{
	kfree(dev->vpd);
}

static ssize_t vpd_read(struct file *filp, struct kobject *kobj,
			struct bin_attribute *bin_attr, char *buf, loff_t off,
			size_t count)
{
	struct pci_dev *dev = to_pci_dev(kobj_to_dev(kobj));

	return pci_read_vpd(dev, off, count, buf);
}

static ssize_t vpd_write(struct file *filp, struct kobject *kobj,
			 struct bin_attribute *bin_attr, char *buf, loff_t off,
			 size_t count)
{
	struct pci_dev *dev = to_pci_dev(kobj_to_dev(kobj));

	return pci_write_vpd(dev, off, count, buf);
}
static BIN_ATTR(vpd, 0600, vpd_read, vpd_write, 0);

static struct bin_attribute *vpd_attrs[] = {
	&bin_attr_vpd,
	NULL,
};

static umode_t vpd_attr_is_visible(struct kobject *kobj,
				   struct bin_attribute *a, int n)
{
	struct pci_dev *pdev = to_pci_dev(kobj_to_dev(kobj));

	if (!pdev->vpd)
		return 0;

	return a->attr.mode;
}

const struct attribute_group pci_dev_vpd_attr_group = {
	.bin_attrs = vpd_attrs,
	.is_bin_visible = vpd_attr_is_visible,
};

int pci_vpd_find_tag(const u8 *buf, unsigned int len, u8 rdt)
{
	int i = 0;

	/* look for LRDT tags only, end tag is the only SRDT tag */
	while (i + PCI_VPD_LRDT_TAG_SIZE <= len && buf[i] & PCI_VPD_LRDT) {
		if (buf[i] == rdt)
			return i;

		i += PCI_VPD_LRDT_TAG_SIZE + pci_vpd_lrdt_size(buf + i);
	}

	return -ENOENT;
}
EXPORT_SYMBOL_GPL(pci_vpd_find_tag);

int pci_vpd_find_info_keyword(const u8 *buf, unsigned int off,
			      unsigned int len, const char *kw)
{
	int i;

	for (i = off; i + PCI_VPD_INFO_FLD_HDR_SIZE <= off + len;) {
		if (buf[i + 0] == kw[0] &&
		    buf[i + 1] == kw[1])
			return i;

		i += PCI_VPD_INFO_FLD_HDR_SIZE +
		     pci_vpd_info_field_size(&buf[i]);
	}

	return -ENOENT;
}
EXPORT_SYMBOL_GPL(pci_vpd_find_info_keyword);

#ifdef CONFIG_PCI_QUIRKS
/*
 * Quirk non-zero PCI functions to route VPD access through function 0 for
 * devices that share VPD resources between functions.  The functions are
 * expected to be identical devices.
 */
static void quirk_f0_vpd_link(struct pci_dev *dev)
{
	struct pci_dev *f0;

	if (!PCI_FUNC(dev->devfn))
		return;

	f0 = pci_get_func0_dev(dev);
	if (!f0)
		return;

	if (f0->vpd && dev->class == f0->class &&
	    dev->vendor == f0->vendor && dev->device == f0->device)
		dev->dev_flags |= PCI_DEV_FLAGS_VPD_REF_F0;

	pci_dev_put(f0);
}
DECLARE_PCI_FIXUP_CLASS_EARLY(PCI_VENDOR_ID_INTEL, PCI_ANY_ID,
			      PCI_CLASS_NETWORK_ETHERNET, 8, quirk_f0_vpd_link);

/*
 * If a device follows the VPD format spec, the PCI core will not read or
 * write past the VPD End Tag.  But some vendors do not follow the VPD
 * format spec, so we can't tell how much data is safe to access.  Devices
 * may behave unpredictably if we access too much.  Blacklist these devices
 * so we don't touch VPD at all.
 */
static void quirk_blacklist_vpd(struct pci_dev *dev)
{
	if (dev->vpd) {
		dev->vpd->len = 0;
		pci_warn(dev, FW_BUG "disabling VPD access (can't determine size of non-standard VPD format)\n");
	}
}
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x0060, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x007c, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x0413, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x0078, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x0079, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x0073, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x0071, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x005b, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x002f, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x005d, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_LSI_LOGIC, 0x005f, quirk_blacklist_vpd);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_ATTANSIC, PCI_ANY_ID,
		quirk_blacklist_vpd);
/*
 * The Amazon Annapurna Labs 0x0031 device id is reused for other non Root Port
 * device types, so the quirk is registered for the PCI_CLASS_BRIDGE_PCI class.
 */
DECLARE_PCI_FIXUP_CLASS_FINAL(PCI_VENDOR_ID_AMAZON_ANNAPURNA_LABS, 0x0031,
			      PCI_CLASS_BRIDGE_PCI, 8, quirk_blacklist_vpd);

static void pci_vpd_set_size(struct pci_dev *dev, size_t len)
{
	struct pci_vpd *vpd = dev->vpd;

	if (!vpd || len == 0 || len > PCI_VPD_MAX_SIZE)
		return;

	vpd->valid = 1;
	vpd->len = len;
}

static void quirk_chelsio_extend_vpd(struct pci_dev *dev)
{
	int chip = (dev->device & 0xf000) >> 12;
	int func = (dev->device & 0x0f00) >>  8;
	int prod = (dev->device & 0x00ff) >>  0;

	/*
	 * If this is a T3-based adapter, there's a 1KB VPD area at offset
	 * 0xc00 which contains the preferred VPD values.  If this is a T4 or
	 * later based adapter, the special VPD is at offset 0x400 for the
	 * Physical Functions (the SR-IOV Virtual Functions have no VPD
	 * Capabilities).  The PCI VPD Access core routines will normally
	 * compute the size of the VPD by parsing the VPD Data Structure at
	 * offset 0x000.  This will result in silent failures when attempting
	 * to accesses these other VPD areas which are beyond those computed
	 * limits.
	 */
	if (chip == 0x0 && prod >= 0x20)
		pci_vpd_set_size(dev, 8192);
	else if (chip >= 0x4 && func < 0x8)
		pci_vpd_set_size(dev, 2048);
}

DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_CHELSIO, PCI_ANY_ID,
			quirk_chelsio_extend_vpd);

#endif

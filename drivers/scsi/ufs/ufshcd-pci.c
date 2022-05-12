// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Universal Flash Storage Host controller PCI glue driver
 *
 * This code is based on drivers/scsi/ufs/ufshcd-pci.c
 * Copyright (C) 2011-2013 Samsung India Software Operations
 *
 * Authors:
 *	Santosh Yaraganavi <santosh.sy@samsung.com>
 *	Vinayak Holikatti <h.vinayak@samsung.com>
 */

#include "ufshcd.h"
#include <linux/pci.h>
#include <linux/pm_runtime.h>
#include <linux/pm_qos.h>
#include <linux/debugfs.h>
#include <linux/uuid.h>
#include <linux/acpi.h>
#include <linux/gpio/consumer.h>

struct ufs_host {
	void (*late_init)(struct ufs_hba *hba);
};

enum {
	INTEL_DSM_FNS		=  0,
	INTEL_DSM_RESET		=  1,
};

struct intel_host {
	struct ufs_host ufs_host;
	u32		dsm_fns;
	u32		active_ltr;
	u32		idle_ltr;
	struct dentry	*debugfs_root;
	struct gpio_desc *reset_gpio;
};

static const guid_t intel_dsm_guid =
	GUID_INIT(0x1A4832A0, 0x7D03, 0x43CA,
		  0xB0, 0x20, 0xF6, 0xDC, 0xD1, 0x2A, 0x19, 0x50);

static int __intel_dsm(struct intel_host *intel_host, struct device *dev,
		       unsigned int fn, u32 *result)
{
	union acpi_object *obj;
	int err = 0;
	size_t len;

	obj = acpi_evaluate_dsm(ACPI_HANDLE(dev), &intel_dsm_guid, 0, fn, NULL);
	if (!obj)
		return -EOPNOTSUPP;

	if (obj->type != ACPI_TYPE_BUFFER || obj->buffer.length < 1) {
		err = -EINVAL;
		goto out;
	}

	len = min_t(size_t, obj->buffer.length, 4);

	*result = 0;
	memcpy(result, obj->buffer.pointer, len);
out:
	ACPI_FREE(obj);

	return err;
}

static int intel_dsm(struct intel_host *intel_host, struct device *dev,
		     unsigned int fn, u32 *result)
{
	if (fn > 31 || !(intel_host->dsm_fns & (1 << fn)))
		return -EOPNOTSUPP;

	return __intel_dsm(intel_host, dev, fn, result);
}

static void intel_dsm_init(struct intel_host *intel_host, struct device *dev)
{
	int err;

	err = __intel_dsm(intel_host, dev, INTEL_DSM_FNS, &intel_host->dsm_fns);
	dev_dbg(dev, "DSM fns %#x, error %d\n", intel_host->dsm_fns, err);
}

static int ufs_intel_hce_enable_notify(struct ufs_hba *hba,
				       enum ufs_notify_change_status status)
{
	/* Cannot enable ICE until after HC enable */
	if (status == POST_CHANGE && hba->caps & UFSHCD_CAP_CRYPTO) {
		u32 hce = ufshcd_readl(hba, REG_CONTROLLER_ENABLE);

		hce |= CRYPTO_GENERAL_ENABLE;
		ufshcd_writel(hba, hce, REG_CONTROLLER_ENABLE);
	}

	return 0;
}

static int ufs_intel_disable_lcc(struct ufs_hba *hba)
{
	u32 attr = UIC_ARG_MIB(PA_LOCAL_TX_LCC_ENABLE);
	u32 lcc_enable = 0;

	ufshcd_dme_get(hba, attr, &lcc_enable);
	if (lcc_enable)
		ufshcd_disable_host_tx_lcc(hba);

	return 0;
}

static int ufs_intel_link_startup_notify(struct ufs_hba *hba,
					 enum ufs_notify_change_status status)
{
	int err = 0;

	switch (status) {
	case PRE_CHANGE:
		err = ufs_intel_disable_lcc(hba);
		break;
	case POST_CHANGE:
		break;
	default:
		break;
	}

	return err;
}

#define INTEL_ACTIVELTR		0x804
#define INTEL_IDLELTR		0x808

#define INTEL_LTR_REQ		BIT(15)
#define INTEL_LTR_SCALE_MASK	GENMASK(11, 10)
#define INTEL_LTR_SCALE_1US	(2 << 10)
#define INTEL_LTR_SCALE_32US	(3 << 10)
#define INTEL_LTR_VALUE_MASK	GENMASK(9, 0)

static void intel_cache_ltr(struct ufs_hba *hba)
{
	struct intel_host *host = ufshcd_get_variant(hba);

	host->active_ltr = readl(hba->mmio_base + INTEL_ACTIVELTR);
	host->idle_ltr = readl(hba->mmio_base + INTEL_IDLELTR);
}

static void intel_ltr_set(struct device *dev, s32 val)
{
	struct ufs_hba *hba = dev_get_drvdata(dev);
	struct intel_host *host = ufshcd_get_variant(hba);
	u32 ltr;

	pm_runtime_get_sync(dev);

	/*
	 * Program latency tolerance (LTR) accordingly what has been asked
	 * by the PM QoS layer or disable it in case we were passed
	 * negative value or PM_QOS_LATENCY_ANY.
	 */
	ltr = readl(hba->mmio_base + INTEL_ACTIVELTR);

	if (val == PM_QOS_LATENCY_ANY || val < 0) {
		ltr &= ~INTEL_LTR_REQ;
	} else {
		ltr |= INTEL_LTR_REQ;
		ltr &= ~INTEL_LTR_SCALE_MASK;
		ltr &= ~INTEL_LTR_VALUE_MASK;

		if (val > INTEL_LTR_VALUE_MASK) {
			val >>= 5;
			if (val > INTEL_LTR_VALUE_MASK)
				val = INTEL_LTR_VALUE_MASK;
			ltr |= INTEL_LTR_SCALE_32US | val;
		} else {
			ltr |= INTEL_LTR_SCALE_1US | val;
		}
	}

	if (ltr == host->active_ltr)
		goto out;

	writel(ltr, hba->mmio_base + INTEL_ACTIVELTR);
	writel(ltr, hba->mmio_base + INTEL_IDLELTR);

	/* Cache the values into intel_host structure */
	intel_cache_ltr(hba);
out:
	pm_runtime_put(dev);
}

static void intel_ltr_expose(struct device *dev)
{
	dev->power.set_latency_tolerance = intel_ltr_set;
	dev_pm_qos_expose_latency_tolerance(dev);
}

static void intel_ltr_hide(struct device *dev)
{
	dev_pm_qos_hide_latency_tolerance(dev);
	dev->power.set_latency_tolerance = NULL;
}

static void intel_add_debugfs(struct ufs_hba *hba)
{
	struct dentry *dir = debugfs_create_dir(dev_name(hba->dev), NULL);
	struct intel_host *host = ufshcd_get_variant(hba);

	intel_cache_ltr(hba);

	host->debugfs_root = dir;
	debugfs_create_x32("active_ltr", 0444, dir, &host->active_ltr);
	debugfs_create_x32("idle_ltr", 0444, dir, &host->idle_ltr);
}

static void intel_remove_debugfs(struct ufs_hba *hba)
{
	struct intel_host *host = ufshcd_get_variant(hba);

	debugfs_remove_recursive(host->debugfs_root);
}

static int ufs_intel_device_reset(struct ufs_hba *hba)
{
	struct intel_host *host = ufshcd_get_variant(hba);

	if (host->dsm_fns & INTEL_DSM_RESET) {
		u32 result = 0;
		int err;

		err = intel_dsm(host, hba->dev, INTEL_DSM_RESET, &result);
		if (!err && !result)
			err = -EIO;
		if (err)
			dev_err(hba->dev, "%s: DSM error %d result %u\n",
				__func__, err, result);
		return err;
	}

	if (!host->reset_gpio)
		return -EOPNOTSUPP;

	gpiod_set_value_cansleep(host->reset_gpio, 1);
	usleep_range(10, 15);

	gpiod_set_value_cansleep(host->reset_gpio, 0);
	usleep_range(10, 15);

	return 0;
}

static struct gpio_desc *ufs_intel_get_reset_gpio(struct device *dev)
{
	/* GPIO in _DSD has active low setting */
	return devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
}

static int ufs_intel_common_init(struct ufs_hba *hba)
{
	struct intel_host *host;

	hba->caps |= UFSHCD_CAP_RPM_AUTOSUSPEND;

	host = devm_kzalloc(hba->dev, sizeof(*host), GFP_KERNEL);
	if (!host)
		return -ENOMEM;
	ufshcd_set_variant(hba, host);
	intel_dsm_init(host, hba->dev);
	if (host->dsm_fns & INTEL_DSM_RESET) {
		if (hba->vops->device_reset)
			hba->caps |= UFSHCD_CAP_DEEPSLEEP;
	} else {
		if (hba->vops->device_reset)
			host->reset_gpio = ufs_intel_get_reset_gpio(hba->dev);
		if (IS_ERR(host->reset_gpio)) {
			dev_err(hba->dev, "%s: failed to get reset GPIO, error %ld\n",
				__func__, PTR_ERR(host->reset_gpio));
			host->reset_gpio = NULL;
		}
		if (host->reset_gpio) {
			gpiod_set_value_cansleep(host->reset_gpio, 0);
			hba->caps |= UFSHCD_CAP_DEEPSLEEP;
		}
	}
	intel_ltr_expose(hba->dev);
	intel_add_debugfs(hba);
	return 0;
}

static void ufs_intel_common_exit(struct ufs_hba *hba)
{
	intel_remove_debugfs(hba);
	intel_ltr_hide(hba->dev);
}

static int ufs_intel_resume(struct ufs_hba *hba, enum ufs_pm_op op)
{
	/*
	 * To support S4 (suspend-to-disk) with spm_lvl other than 5, the base
	 * address registers must be restored because the restore kernel can
	 * have used different addresses.
	 */
	ufshcd_writel(hba, lower_32_bits(hba->utrdl_dma_addr),
		      REG_UTP_TRANSFER_REQ_LIST_BASE_L);
	ufshcd_writel(hba, upper_32_bits(hba->utrdl_dma_addr),
		      REG_UTP_TRANSFER_REQ_LIST_BASE_H);
	ufshcd_writel(hba, lower_32_bits(hba->utmrdl_dma_addr),
		      REG_UTP_TASK_REQ_LIST_BASE_L);
	ufshcd_writel(hba, upper_32_bits(hba->utmrdl_dma_addr),
		      REG_UTP_TASK_REQ_LIST_BASE_H);

	if (ufshcd_is_link_hibern8(hba)) {
		int ret = ufshcd_uic_hibern8_exit(hba);

		if (!ret) {
			ufshcd_set_link_active(hba);
		} else {
			dev_err(hba->dev, "%s: hibern8 exit failed %d\n",
				__func__, ret);
			/*
			 * Force reset and restore. Any other actions can lead
			 * to an unrecoverable state.
			 */
			ufshcd_set_link_off(hba);
		}
	}

	return 0;
}

static int ufs_intel_ehl_init(struct ufs_hba *hba)
{
	hba->quirks |= UFSHCD_QUIRK_BROKEN_AUTO_HIBERN8;
	return ufs_intel_common_init(hba);
}

static void ufs_intel_lkf_late_init(struct ufs_hba *hba)
{
	/* LKF always needs a full reset, so set PM accordingly */
	if (hba->caps & UFSHCD_CAP_DEEPSLEEP) {
		hba->spm_lvl = UFS_PM_LVL_6;
		hba->rpm_lvl = UFS_PM_LVL_6;
	} else {
		hba->spm_lvl = UFS_PM_LVL_5;
		hba->rpm_lvl = UFS_PM_LVL_5;
	}
}

static int ufs_intel_lkf_init(struct ufs_hba *hba)
{
	struct ufs_host *ufs_host;
	int err;

	hba->quirks |= UFSHCD_QUIRK_BROKEN_AUTO_HIBERN8;
	hba->caps |= UFSHCD_CAP_CRYPTO;
	err = ufs_intel_common_init(hba);
	ufs_host = ufshcd_get_variant(hba);
	ufs_host->late_init = ufs_intel_lkf_late_init;
	return err;
}

static struct ufs_hba_variant_ops ufs_intel_cnl_hba_vops = {
	.name                   = "intel-pci",
	.init			= ufs_intel_common_init,
	.exit			= ufs_intel_common_exit,
	.link_startup_notify	= ufs_intel_link_startup_notify,
	.resume			= ufs_intel_resume,
};

static struct ufs_hba_variant_ops ufs_intel_ehl_hba_vops = {
	.name                   = "intel-pci",
	.init			= ufs_intel_ehl_init,
	.exit			= ufs_intel_common_exit,
	.link_startup_notify	= ufs_intel_link_startup_notify,
	.resume			= ufs_intel_resume,
};

static struct ufs_hba_variant_ops ufs_intel_lkf_hba_vops = {
	.name                   = "intel-pci",
	.init			= ufs_intel_lkf_init,
	.exit			= ufs_intel_common_exit,
	.hce_enable_notify	= ufs_intel_hce_enable_notify,
	.link_startup_notify	= ufs_intel_link_startup_notify,
	.resume			= ufs_intel_resume,
	.device_reset		= ufs_intel_device_reset,
};

#ifdef CONFIG_PM_SLEEP
/**
 * ufshcd_pci_suspend - suspend power management function
 * @dev: pointer to PCI device handle
 *
 * Returns 0 if successful
 * Returns non-zero otherwise
 */
static int ufshcd_pci_suspend(struct device *dev)
{
	return ufshcd_system_suspend(dev_get_drvdata(dev));
}

/**
 * ufshcd_pci_resume - resume power management function
 * @dev: pointer to PCI device handle
 *
 * Returns 0 if successful
 * Returns non-zero otherwise
 */
static int ufshcd_pci_resume(struct device *dev)
{
	return ufshcd_system_resume(dev_get_drvdata(dev));
}

/**
 * ufshcd_pci_poweroff - suspend-to-disk poweroff function
 * @dev: pointer to PCI device handle
 *
 * Returns 0 if successful
 * Returns non-zero otherwise
 */
static int ufshcd_pci_poweroff(struct device *dev)
{
	struct ufs_hba *hba = dev_get_drvdata(dev);
	int spm_lvl = hba->spm_lvl;
	int ret;

	/*
	 * For poweroff we need to set the UFS device to PowerDown mode.
	 * Force spm_lvl to ensure that.
	 */
	hba->spm_lvl = 5;
	ret = ufshcd_system_suspend(hba);
	hba->spm_lvl = spm_lvl;
	return ret;
}

#endif /* !CONFIG_PM_SLEEP */

#ifdef CONFIG_PM
static int ufshcd_pci_runtime_suspend(struct device *dev)
{
	return ufshcd_runtime_suspend(dev_get_drvdata(dev));
}
static int ufshcd_pci_runtime_resume(struct device *dev)
{
	return ufshcd_runtime_resume(dev_get_drvdata(dev));
}
static int ufshcd_pci_runtime_idle(struct device *dev)
{
	return ufshcd_runtime_idle(dev_get_drvdata(dev));
}
#endif /* !CONFIG_PM */

/**
 * ufshcd_pci_shutdown - main function to put the controller in reset state
 * @pdev: pointer to PCI device handle
 */
static void ufshcd_pci_shutdown(struct pci_dev *pdev)
{
	ufshcd_shutdown((struct ufs_hba *)pci_get_drvdata(pdev));
}

/**
 * ufshcd_pci_remove - de-allocate PCI/SCSI host and host memory space
 *		data structure memory
 * @pdev: pointer to PCI handle
 */
static void ufshcd_pci_remove(struct pci_dev *pdev)
{
	struct ufs_hba *hba = pci_get_drvdata(pdev);

	pm_runtime_forbid(&pdev->dev);
	pm_runtime_get_noresume(&pdev->dev);
	ufshcd_remove(hba);
	ufshcd_dealloc_host(hba);
}

/**
 * ufshcd_pci_probe - probe routine of the driver
 * @pdev: pointer to PCI device handle
 * @id: PCI device id
 *
 * Returns 0 on success, non-zero value on failure
 */
static int
ufshcd_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct ufs_host *ufs_host;
	struct ufs_hba *hba;
	void __iomem *mmio_base;
	int err;

	err = pcim_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev, "pcim_enable_device failed\n");
		return err;
	}

	pci_set_master(pdev);

	err = pcim_iomap_regions(pdev, 1 << 0, UFSHCD);
	if (err < 0) {
		dev_err(&pdev->dev, "request and iomap failed\n");
		return err;
	}

	mmio_base = pcim_iomap_table(pdev)[0];

	err = ufshcd_alloc_host(&pdev->dev, &hba);
	if (err) {
		dev_err(&pdev->dev, "Allocation failed\n");
		return err;
	}

	pci_set_drvdata(pdev, hba);

	hba->vops = (struct ufs_hba_variant_ops *)id->driver_data;

	err = ufshcd_init(hba, mmio_base, pdev->irq);
	if (err) {
		dev_err(&pdev->dev, "Initialization failed\n");
		ufshcd_dealloc_host(hba);
		return err;
	}

	ufs_host = ufshcd_get_variant(hba);
	if (ufs_host && ufs_host->late_init)
		ufs_host->late_init(hba);

	pm_runtime_put_noidle(&pdev->dev);
	pm_runtime_allow(&pdev->dev);

	return 0;
}

static const struct dev_pm_ops ufshcd_pci_pm_ops = {
#ifdef CONFIG_PM_SLEEP
	.suspend	= ufshcd_pci_suspend,
	.resume		= ufshcd_pci_resume,
	.freeze		= ufshcd_pci_suspend,
	.thaw		= ufshcd_pci_resume,
	.poweroff	= ufshcd_pci_poweroff,
	.restore	= ufshcd_pci_resume,
#endif
	SET_RUNTIME_PM_OPS(ufshcd_pci_runtime_suspend,
			   ufshcd_pci_runtime_resume,
			   ufshcd_pci_runtime_idle)
};

static const struct pci_device_id ufshcd_pci_tbl[] = {
	{ PCI_VENDOR_ID_SAMSUNG, 0xC00C, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ PCI_VDEVICE(INTEL, 0x9DFA), (kernel_ulong_t)&ufs_intel_cnl_hba_vops },
	{ PCI_VDEVICE(INTEL, 0x4B41), (kernel_ulong_t)&ufs_intel_ehl_hba_vops },
	{ PCI_VDEVICE(INTEL, 0x4B43), (kernel_ulong_t)&ufs_intel_ehl_hba_vops },
	{ PCI_VDEVICE(INTEL, 0x98FA), (kernel_ulong_t)&ufs_intel_lkf_hba_vops },
	{ }	/* terminate list */
};

MODULE_DEVICE_TABLE(pci, ufshcd_pci_tbl);

static struct pci_driver ufshcd_pci_driver = {
	.name = UFSHCD,
	.id_table = ufshcd_pci_tbl,
	.probe = ufshcd_pci_probe,
	.remove = ufshcd_pci_remove,
	.shutdown = ufshcd_pci_shutdown,
	.driver = {
		.pm = &ufshcd_pci_pm_ops
	},
};

module_pci_driver(ufshcd_pci_driver);

MODULE_AUTHOR("Santosh Yaragnavi <santosh.sy@samsung.com>");
MODULE_AUTHOR("Vinayak Holikatti <h.vinayak@samsung.com>");
MODULE_DESCRIPTION("UFS host controller PCI glue driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(UFSHCD_DRIVER_VERSION);

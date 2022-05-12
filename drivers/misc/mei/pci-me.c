// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2003-2020, Intel Corporation. All rights reserved.
 * Intel Management Engine Interface (Intel MEI) Linux driver
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>

#include <linux/mei.h>

#include "mei_dev.h"
#include "client.h"
#include "hw-me-regs.h"
#include "hw-me.h"

/* mei_pci_tbl - PCI Device ID Table */
static const struct pci_device_id mei_me_pci_tbl[] = {
	{MEI_PCI_DEVICE(MEI_DEV_ID_82946GZ, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_82G35, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_82Q965, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_82G965, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_82GM965, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_82GME965, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_82Q35, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_82G33, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_82Q33, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_82X38, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_3200, MEI_ME_ICH_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_6, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_7, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_8, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_9, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9_10, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9M_1, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9M_2, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9M_3, MEI_ME_ICH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH9M_4, MEI_ME_ICH_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH10_1, MEI_ME_ICH10_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH10_2, MEI_ME_ICH10_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH10_3, MEI_ME_ICH10_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ICH10_4, MEI_ME_ICH10_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_IBXPK_1, MEI_ME_PCH6_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_IBXPK_2, MEI_ME_PCH6_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CPT_1, MEI_ME_PCH_CPT_PBG_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_PBG_1, MEI_ME_PCH_CPT_PBG_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_PPT_1, MEI_ME_PCH7_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_PPT_2, MEI_ME_PCH7_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_PPT_3, MEI_ME_PCH7_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_LPT_H, MEI_ME_PCH8_SPS_4_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_LPT_W, MEI_ME_PCH8_SPS_4_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_LPT_LP, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_LPT_HR, MEI_ME_PCH8_SPS_4_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_WPT_LP, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_WPT_LP_2, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_SPT, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_SPT_2, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_SPT_3, MEI_ME_PCH8_ITOUCH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_SPT_H, MEI_ME_PCH8_SPS_4_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_SPT_H_2, MEI_ME_PCH8_SPS_4_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_LBG, MEI_ME_PCH12_SPS_4_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_BXT_M, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_APL_I, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_DNV_IE, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_GLK, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_KBP, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_KBP_2, MEI_ME_PCH8_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_KBP_3, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_CNP_LP, MEI_ME_PCH12_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CNP_LP_3, MEI_ME_PCH8_ITOUCH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CNP_H, MEI_ME_PCH12_SPS_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CNP_H_3, MEI_ME_PCH12_SPS_ITOUCH_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_CMP_LP, MEI_ME_PCH12_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CMP_LP_3, MEI_ME_PCH8_ITOUCH_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CMP_V, MEI_ME_PCH12_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CMP_H, MEI_ME_PCH12_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_CMP_H_3, MEI_ME_PCH8_ITOUCH_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_ICP_LP, MEI_ME_PCH12_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_TGP_LP, MEI_ME_PCH15_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_TGP_H, MEI_ME_PCH15_SPS_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_JSP_N, MEI_ME_PCH15_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_MCC, MEI_ME_PCH15_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_MCC_4, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_CDF, MEI_ME_PCH8_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_EBG, MEI_ME_PCH15_SPS_CFG)},

	{MEI_PCI_DEVICE(MEI_DEV_ID_ADP_S, MEI_ME_PCH15_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ADP_LP, MEI_ME_PCH15_CFG)},
	{MEI_PCI_DEVICE(MEI_DEV_ID_ADP_P, MEI_ME_PCH15_CFG)},

	/* required last entry */
	{0, }
};

MODULE_DEVICE_TABLE(pci, mei_me_pci_tbl);

#ifdef CONFIG_PM
static inline void mei_me_set_pm_domain(struct mei_device *dev);
static inline void mei_me_unset_pm_domain(struct mei_device *dev);
#else
static inline void mei_me_set_pm_domain(struct mei_device *dev) {}
static inline void mei_me_unset_pm_domain(struct mei_device *dev) {}
#endif /* CONFIG_PM */

static int mei_me_read_fws(const struct mei_device *dev, int where, u32 *val)
{
	struct pci_dev *pdev = to_pci_dev(dev->dev);

	return pci_read_config_dword(pdev, where, val);
}

/**
 * mei_me_quirk_probe - probe for devices that doesn't valid ME interface
 *
 * @pdev: PCI device structure
 * @cfg: per generation config
 *
 * Return: true if ME Interface is valid, false otherwise
 */
static bool mei_me_quirk_probe(struct pci_dev *pdev,
				const struct mei_cfg *cfg)
{
	if (cfg->quirk_probe && cfg->quirk_probe(pdev)) {
		dev_info(&pdev->dev, "Device doesn't have valid ME Interface\n");
		return false;
	}

	return true;
}

/**
 * mei_me_probe - Device Initialization Routine
 *
 * @pdev: PCI device structure
 * @ent: entry in kcs_pci_tbl
 *
 * Return: 0 on success, <0 on failure.
 */
static int mei_me_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	const struct mei_cfg *cfg;
	struct mei_device *dev;
	struct mei_me_hw *hw;
	unsigned int irqflags;
	int err;

	cfg = mei_me_get_cfg(ent->driver_data);
	if (!cfg)
		return -ENODEV;

	if (!mei_me_quirk_probe(pdev, cfg))
		return -ENODEV;

	/* enable pci dev */
	err = pcim_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev, "failed to enable pci device.\n");
		goto end;
	}
	/* set PCI host mastering  */
	pci_set_master(pdev);
	/* pci request regions and mapping IO device memory for mei driver */
	err = pcim_iomap_regions(pdev, BIT(0), KBUILD_MODNAME);
	if (err) {
		dev_err(&pdev->dev, "failed to get pci regions.\n");
		goto end;
	}

	if (dma_set_mask(&pdev->dev, DMA_BIT_MASK(64)) ||
	    dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64))) {

		err = dma_set_mask(&pdev->dev, DMA_BIT_MASK(32));
		if (err)
			err = dma_set_coherent_mask(&pdev->dev,
						    DMA_BIT_MASK(32));
	}
	if (err) {
		dev_err(&pdev->dev, "No usable DMA configuration, aborting\n");
		goto end;
	}

	/* allocates and initializes the mei dev structure */
	dev = mei_me_dev_init(&pdev->dev, cfg);
	if (!dev) {
		err = -ENOMEM;
		goto end;
	}
	hw = to_me_hw(dev);
	hw->mem_addr = pcim_iomap_table(pdev)[0];
	hw->read_fws = mei_me_read_fws;

	pci_enable_msi(pdev);

	hw->irq = pdev->irq;

	 /* request and enable interrupt */
	irqflags = pci_dev_msi_enabled(pdev) ? IRQF_ONESHOT : IRQF_SHARED;

	err = request_threaded_irq(pdev->irq,
			mei_me_irq_quick_handler,
			mei_me_irq_thread_handler,
			irqflags, KBUILD_MODNAME, dev);
	if (err) {
		dev_err(&pdev->dev, "request_threaded_irq failure. irq = %d\n",
		       pdev->irq);
		goto end;
	}

	if (mei_start(dev)) {
		dev_err(&pdev->dev, "init hw failure.\n");
		err = -ENODEV;
		goto release_irq;
	}

	pm_runtime_set_autosuspend_delay(&pdev->dev, MEI_ME_RPM_TIMEOUT);
	pm_runtime_use_autosuspend(&pdev->dev);

	err = mei_register(dev, &pdev->dev);
	if (err)
		goto stop;

	pci_set_drvdata(pdev, dev);

	/*
	 * MEI requires to resume from runtime suspend mode
	 * in order to perform link reset flow upon system suspend.
	 */
	dev_pm_set_driver_flags(&pdev->dev, DPM_FLAG_NO_DIRECT_COMPLETE);

	/*
	 * ME maps runtime suspend/resume to D0i states,
	 * hence we need to go around native PCI runtime service which
	 * eventually brings the device into D3cold/hot state,
	 * but the mei device cannot wake up from D3 unlike from D0i3.
	 * To get around the PCI device native runtime pm,
	 * ME uses runtime pm domain handlers which take precedence
	 * over the driver's pm handlers.
	 */
	mei_me_set_pm_domain(dev);

	if (mei_pg_is_enabled(dev)) {
		pm_runtime_put_noidle(&pdev->dev);
		if (hw->d0i3_supported)
			pm_runtime_allow(&pdev->dev);
	}

	dev_dbg(&pdev->dev, "initialization successful.\n");

	return 0;

stop:
	mei_stop(dev);
release_irq:
	mei_cancel_work(dev);
	mei_disable_interrupts(dev);
	free_irq(pdev->irq, dev);
end:
	dev_err(&pdev->dev, "initialization failed.\n");
	return err;
}

/**
 * mei_me_shutdown - Device Removal Routine
 *
 * @pdev: PCI device structure
 *
 * mei_me_shutdown is called from the reboot notifier
 * it's a simplified version of remove so we go down
 * faster.
 */
static void mei_me_shutdown(struct pci_dev *pdev)
{
	struct mei_device *dev;

	dev = pci_get_drvdata(pdev);
	if (!dev)
		return;

	dev_dbg(&pdev->dev, "shutdown\n");
	mei_stop(dev);

	mei_me_unset_pm_domain(dev);

	mei_disable_interrupts(dev);
	free_irq(pdev->irq, dev);
}

/**
 * mei_me_remove - Device Removal Routine
 *
 * @pdev: PCI device structure
 *
 * mei_me_remove is called by the PCI subsystem to alert the driver
 * that it should release a PCI device.
 */
static void mei_me_remove(struct pci_dev *pdev)
{
	struct mei_device *dev;

	dev = pci_get_drvdata(pdev);
	if (!dev)
		return;

	if (mei_pg_is_enabled(dev))
		pm_runtime_get_noresume(&pdev->dev);

	dev_dbg(&pdev->dev, "stop\n");
	mei_stop(dev);

	mei_me_unset_pm_domain(dev);

	mei_disable_interrupts(dev);

	free_irq(pdev->irq, dev);

	mei_deregister(dev);
}

#ifdef CONFIG_PM_SLEEP
static int mei_me_pci_suspend(struct device *device)
{
	struct pci_dev *pdev = to_pci_dev(device);
	struct mei_device *dev = pci_get_drvdata(pdev);

	if (!dev)
		return -ENODEV;

	dev_dbg(&pdev->dev, "suspend\n");

	mei_stop(dev);

	mei_disable_interrupts(dev);

	free_irq(pdev->irq, dev);
	pci_disable_msi(pdev);

	return 0;
}

static int mei_me_pci_resume(struct device *device)
{
	struct pci_dev *pdev = to_pci_dev(device);
	struct mei_device *dev;
	unsigned int irqflags;
	int err;

	dev = pci_get_drvdata(pdev);
	if (!dev)
		return -ENODEV;

	pci_enable_msi(pdev);

	irqflags = pci_dev_msi_enabled(pdev) ? IRQF_ONESHOT : IRQF_SHARED;

	/* request and enable interrupt */
	err = request_threaded_irq(pdev->irq,
			mei_me_irq_quick_handler,
			mei_me_irq_thread_handler,
			irqflags, KBUILD_MODNAME, dev);

	if (err) {
		dev_err(&pdev->dev, "request_threaded_irq failed: irq = %d.\n",
				pdev->irq);
		return err;
	}

	err = mei_restart(dev);
	if (err)
		return err;

	/* Start timer if stopped in suspend */
	schedule_delayed_work(&dev->timer_work, HZ);

	return 0;
}
#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_PM
static int mei_me_pm_runtime_idle(struct device *device)
{
	struct mei_device *dev;

	dev_dbg(device, "rpm: me: runtime_idle\n");

	dev = dev_get_drvdata(device);
	if (!dev)
		return -ENODEV;
	if (mei_write_is_idle(dev))
		pm_runtime_autosuspend(device);

	return -EBUSY;
}

static int mei_me_pm_runtime_suspend(struct device *device)
{
	struct mei_device *dev;
	int ret;

	dev_dbg(device, "rpm: me: runtime suspend\n");

	dev = dev_get_drvdata(device);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->device_lock);

	if (mei_write_is_idle(dev))
		ret = mei_me_pg_enter_sync(dev);
	else
		ret = -EAGAIN;

	mutex_unlock(&dev->device_lock);

	dev_dbg(device, "rpm: me: runtime suspend ret=%d\n", ret);

	if (ret && ret != -EAGAIN)
		schedule_work(&dev->reset_work);

	return ret;
}

static int mei_me_pm_runtime_resume(struct device *device)
{
	struct mei_device *dev;
	int ret;

	dev_dbg(device, "rpm: me: runtime resume\n");

	dev = dev_get_drvdata(device);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->device_lock);

	ret = mei_me_pg_exit_sync(dev);

	mutex_unlock(&dev->device_lock);

	dev_dbg(device, "rpm: me: runtime resume ret = %d\n", ret);

	if (ret)
		schedule_work(&dev->reset_work);

	return ret;
}

/**
 * mei_me_set_pm_domain - fill and set pm domain structure for device
 *
 * @dev: mei_device
 */
static inline void mei_me_set_pm_domain(struct mei_device *dev)
{
	struct pci_dev *pdev  = to_pci_dev(dev->dev);

	if (pdev->dev.bus && pdev->dev.bus->pm) {
		dev->pg_domain.ops = *pdev->dev.bus->pm;

		dev->pg_domain.ops.runtime_suspend = mei_me_pm_runtime_suspend;
		dev->pg_domain.ops.runtime_resume = mei_me_pm_runtime_resume;
		dev->pg_domain.ops.runtime_idle = mei_me_pm_runtime_idle;

		dev_pm_domain_set(&pdev->dev, &dev->pg_domain);
	}
}

/**
 * mei_me_unset_pm_domain - clean pm domain structure for device
 *
 * @dev: mei_device
 */
static inline void mei_me_unset_pm_domain(struct mei_device *dev)
{
	/* stop using pm callbacks if any */
	dev_pm_domain_set(dev->dev, NULL);
}

static const struct dev_pm_ops mei_me_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(mei_me_pci_suspend,
				mei_me_pci_resume)
	SET_RUNTIME_PM_OPS(
		mei_me_pm_runtime_suspend,
		mei_me_pm_runtime_resume,
		mei_me_pm_runtime_idle)
};

#define MEI_ME_PM_OPS	(&mei_me_pm_ops)
#else
#define MEI_ME_PM_OPS	NULL
#endif /* CONFIG_PM */
/*
 *  PCI driver structure
 */
static struct pci_driver mei_me_driver = {
	.name = KBUILD_MODNAME,
	.id_table = mei_me_pci_tbl,
	.probe = mei_me_probe,
	.remove = mei_me_remove,
	.shutdown = mei_me_shutdown,
	.driver.pm = MEI_ME_PM_OPS,
	.driver.probe_type = PROBE_PREFER_ASYNCHRONOUS,
};

module_pci_driver(mei_me_driver);

MODULE_AUTHOR("Intel Corporation");
MODULE_DESCRIPTION("Intel(R) Management Engine Interface");
MODULE_LICENSE("GPL v2");

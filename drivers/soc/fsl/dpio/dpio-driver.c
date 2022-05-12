// SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause)
/*
 * Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright NXP 2016
 *
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/msi.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/sys_soc.h>

#include <linux/fsl/mc.h>
#include <soc/fsl/dpaa2-io.h>

#include "qbman-portal.h"
#include "dpio.h"
#include "dpio-cmd.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Freescale Semiconductor, Inc");
MODULE_DESCRIPTION("DPIO Driver");

struct dpio_priv {
	struct dpaa2_io *io;
};

static cpumask_var_t cpus_unused_mask;

static const struct soc_device_attribute ls1088a_soc[] = {
	{.family = "QorIQ LS1088A"},
	{ /* sentinel */ }
};

static const struct soc_device_attribute ls2080a_soc[] = {
	{.family = "QorIQ LS2080A"},
	{ /* sentinel */ }
};

static const struct soc_device_attribute ls2088a_soc[] = {
	{.family = "QorIQ LS2088A"},
	{ /* sentinel */ }
};

static const struct soc_device_attribute lx2160a_soc[] = {
	{.family = "QorIQ LX2160A"},
	{ /* sentinel */ }
};

static int dpaa2_dpio_get_cluster_sdest(struct fsl_mc_device *dpio_dev, int cpu)
{
	int cluster_base, cluster_size;

	if (soc_device_match(ls1088a_soc)) {
		cluster_base = 2;
		cluster_size = 4;
	} else if (soc_device_match(ls2080a_soc) ||
		   soc_device_match(ls2088a_soc) ||
		   soc_device_match(lx2160a_soc)) {
		cluster_base = 0;
		cluster_size = 2;
	} else {
		dev_err(&dpio_dev->dev, "unknown SoC version\n");
		return -1;
	}

	return cluster_base + cpu / cluster_size;
}

static irqreturn_t dpio_irq_handler(int irq_num, void *arg)
{
	struct device *dev = (struct device *)arg;
	struct dpio_priv *priv = dev_get_drvdata(dev);

	return dpaa2_io_irq(priv->io);
}

static void unregister_dpio_irq_handlers(struct fsl_mc_device *dpio_dev)
{
	struct fsl_mc_device_irq *irq;

	irq = dpio_dev->irqs[0];

	/* clear the affinity hint */
	irq_set_affinity_hint(irq->msi_desc->irq, NULL);
}

static int register_dpio_irq_handlers(struct fsl_mc_device *dpio_dev, int cpu)
{
	int error;
	struct fsl_mc_device_irq *irq;

	irq = dpio_dev->irqs[0];
	error = devm_request_irq(&dpio_dev->dev,
				 irq->msi_desc->irq,
				 dpio_irq_handler,
				 0,
				 dev_name(&dpio_dev->dev),
				 &dpio_dev->dev);
	if (error < 0) {
		dev_err(&dpio_dev->dev,
			"devm_request_irq() failed: %d\n",
			error);
		return error;
	}

	/* set the affinity hint */
	if (irq_set_affinity_hint(irq->msi_desc->irq, cpumask_of(cpu)))
		dev_err(&dpio_dev->dev,
			"irq_set_affinity failed irq %d cpu %d\n",
			irq->msi_desc->irq, cpu);

	return 0;
}

static int dpaa2_dpio_probe(struct fsl_mc_device *dpio_dev)
{
	struct dpio_attr dpio_attrs;
	struct dpaa2_io_desc desc;
	struct dpio_priv *priv;
	int err = -ENOMEM;
	struct device *dev = &dpio_dev->dev;
	int possible_next_cpu;
	int sdest;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		goto err_priv_alloc;

	dev_set_drvdata(dev, priv);

	err = fsl_mc_portal_allocate(dpio_dev, 0, &dpio_dev->mc_io);
	if (err) {
		dev_dbg(dev, "MC portal allocation failed\n");
		err = -EPROBE_DEFER;
		goto err_priv_alloc;
	}

	err = dpio_open(dpio_dev->mc_io, 0, dpio_dev->obj_desc.id,
			&dpio_dev->mc_handle);
	if (err) {
		dev_err(dev, "dpio_open() failed\n");
		goto err_open;
	}

	err = dpio_reset(dpio_dev->mc_io, 0, dpio_dev->mc_handle);
	if (err) {
		dev_err(dev, "dpio_reset() failed\n");
		goto err_reset;
	}

	err = dpio_get_attributes(dpio_dev->mc_io, 0, dpio_dev->mc_handle,
				  &dpio_attrs);
	if (err) {
		dev_err(dev, "dpio_get_attributes() failed %d\n", err);
		goto err_get_attr;
	}
	desc.qman_version = dpio_attrs.qbman_version;

	err = dpio_enable(dpio_dev->mc_io, 0, dpio_dev->mc_handle);
	if (err) {
		dev_err(dev, "dpio_enable() failed %d\n", err);
		goto err_get_attr;
	}

	/* initialize DPIO descriptor */
	desc.receives_notifications = dpio_attrs.num_priorities ? 1 : 0;
	desc.has_8prio = dpio_attrs.num_priorities == 8 ? 1 : 0;
	desc.dpio_id = dpio_dev->obj_desc.id;

	/* get the cpu to use for the affinity hint */
	possible_next_cpu = cpumask_first(cpus_unused_mask);
	if (possible_next_cpu >= nr_cpu_ids) {
		dev_err(dev, "probe failed. Number of DPIOs exceeds NR_CPUS.\n");
		err = -ERANGE;
		goto err_allocate_irqs;
	}
	desc.cpu = possible_next_cpu;
	cpumask_clear_cpu(possible_next_cpu, cpus_unused_mask);

	sdest = dpaa2_dpio_get_cluster_sdest(dpio_dev, desc.cpu);
	if (sdest >= 0) {
		err = dpio_set_stashing_destination(dpio_dev->mc_io, 0,
						    dpio_dev->mc_handle,
						    sdest);
		if (err)
			dev_err(dev, "dpio_set_stashing_destination failed for cpu%d\n",
				desc.cpu);
	}

	if (dpio_dev->obj_desc.region_count < 3) {
		/* No support for DDR backed portals, use classic mapping */
		/*
		 * Set the CENA regs to be the cache inhibited area of the
		 * portal to avoid coherency issues if a user migrates to
		 * another core.
		 */
		desc.regs_cena = devm_memremap(dev, dpio_dev->regions[1].start,
					resource_size(&dpio_dev->regions[1]),
					MEMREMAP_WC);
	} else {
		desc.regs_cena = devm_memremap(dev, dpio_dev->regions[2].start,
					resource_size(&dpio_dev->regions[2]),
					MEMREMAP_WB);
	}

	if (IS_ERR(desc.regs_cena)) {
		dev_err(dev, "devm_memremap failed\n");
		err = PTR_ERR(desc.regs_cena);
		goto err_allocate_irqs;
	}

	desc.regs_cinh = devm_ioremap(dev, dpio_dev->regions[1].start,
				      resource_size(&dpio_dev->regions[1]));
	if (!desc.regs_cinh) {
		err = -ENOMEM;
		dev_err(dev, "devm_ioremap failed\n");
		goto err_allocate_irqs;
	}

	err = fsl_mc_allocate_irqs(dpio_dev);
	if (err) {
		dev_err(dev, "fsl_mc_allocate_irqs failed. err=%d\n", err);
		goto err_allocate_irqs;
	}

	priv->io = dpaa2_io_create(&desc, dev);
	if (!priv->io) {
		dev_err(dev, "dpaa2_io_create failed\n");
		err = -ENOMEM;
		goto err_dpaa2_io_create;
	}

	err = register_dpio_irq_handlers(dpio_dev, desc.cpu);
	if (err)
		goto err_register_dpio_irq;

	dev_info(dev, "probed\n");
	dev_dbg(dev, "   receives_notifications = %d\n",
		desc.receives_notifications);
	dpio_close(dpio_dev->mc_io, 0, dpio_dev->mc_handle);

	return 0;

err_dpaa2_io_create:
	unregister_dpio_irq_handlers(dpio_dev);
err_register_dpio_irq:
	fsl_mc_free_irqs(dpio_dev);
err_allocate_irqs:
	dpio_disable(dpio_dev->mc_io, 0, dpio_dev->mc_handle);
err_get_attr:
err_reset:
	dpio_close(dpio_dev->mc_io, 0, dpio_dev->mc_handle);
err_open:
	fsl_mc_portal_free(dpio_dev->mc_io);
err_priv_alloc:
	return err;
}

/* Tear down interrupts for a given DPIO object */
static void dpio_teardown_irqs(struct fsl_mc_device *dpio_dev)
{
	unregister_dpio_irq_handlers(dpio_dev);
	fsl_mc_free_irqs(dpio_dev);
}

static int dpaa2_dpio_remove(struct fsl_mc_device *dpio_dev)
{
	struct device *dev;
	struct dpio_priv *priv;
	int err = 0, cpu;

	dev = &dpio_dev->dev;
	priv = dev_get_drvdata(dev);
	cpu = dpaa2_io_get_cpu(priv->io);

	dpaa2_io_down(priv->io);

	dpio_teardown_irqs(dpio_dev);

	cpumask_set_cpu(cpu, cpus_unused_mask);

	err = dpio_open(dpio_dev->mc_io, 0, dpio_dev->obj_desc.id,
			&dpio_dev->mc_handle);
	if (err) {
		dev_err(dev, "dpio_open() failed\n");
		goto err_open;
	}

	dpio_disable(dpio_dev->mc_io, 0, dpio_dev->mc_handle);

	dpio_close(dpio_dev->mc_io, 0, dpio_dev->mc_handle);

	fsl_mc_portal_free(dpio_dev->mc_io);

	return 0;

err_open:
	fsl_mc_portal_free(dpio_dev->mc_io);

	return err;
}

static const struct fsl_mc_device_id dpaa2_dpio_match_id_table[] = {
	{
		.vendor = FSL_MC_VENDOR_FREESCALE,
		.obj_type = "dpio",
	},
	{ .vendor = 0x0 }
};

static struct fsl_mc_driver dpaa2_dpio_driver = {
	.driver = {
		.name		= KBUILD_MODNAME,
		.owner		= THIS_MODULE,
	},
	.probe		= dpaa2_dpio_probe,
	.remove		= dpaa2_dpio_remove,
	.match_id_table = dpaa2_dpio_match_id_table
};

static int dpio_driver_init(void)
{
	if (!zalloc_cpumask_var(&cpus_unused_mask, GFP_KERNEL))
		return -ENOMEM;
	cpumask_copy(cpus_unused_mask, cpu_online_mask);

	return fsl_mc_driver_register(&dpaa2_dpio_driver);
}

static void dpio_driver_exit(void)
{
	free_cpumask_var(cpus_unused_mask);
	fsl_mc_driver_unregister(&dpaa2_dpio_driver);
}
module_init(dpio_driver_init);
module_exit(dpio_driver_exit);

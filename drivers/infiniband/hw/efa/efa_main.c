// SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause
/*
 * Copyright 2018-2021 Amazon.com, Inc. or its affiliates. All rights reserved.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/utsname.h>
#include <linux/version.h>

#include <rdma/ib_user_verbs.h>

#include "efa.h"

#define PCI_DEV_ID_EFA0_VF 0xefa0
#define PCI_DEV_ID_EFA1_VF 0xefa1

static const struct pci_device_id efa_pci_tbl[] = {
	{ PCI_VDEVICE(AMAZON, PCI_DEV_ID_EFA0_VF) },
	{ PCI_VDEVICE(AMAZON, PCI_DEV_ID_EFA1_VF) },
	{ }
};

MODULE_AUTHOR("Amazon.com, Inc. or its affiliates");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION(DEVICE_NAME);
MODULE_DEVICE_TABLE(pci, efa_pci_tbl);

#define EFA_REG_BAR 0
#define EFA_MEM_BAR 2
#define EFA_BASE_BAR_MASK (BIT(EFA_REG_BAR) | BIT(EFA_MEM_BAR))

#define EFA_AENQ_ENABLED_GROUPS \
	(BIT(EFA_ADMIN_FATAL_ERROR) | BIT(EFA_ADMIN_WARNING) | \
	 BIT(EFA_ADMIN_NOTIFICATION) | BIT(EFA_ADMIN_KEEP_ALIVE))

/* This handler will called for unknown event group or unimplemented handlers */
static void unimplemented_aenq_handler(void *data,
				       struct efa_admin_aenq_entry *aenq_e)
{
	struct efa_dev *dev = (struct efa_dev *)data;

	ibdev_err(&dev->ibdev,
		  "Unknown event was received or event with unimplemented handler\n");
}

static void efa_keep_alive(void *data, struct efa_admin_aenq_entry *aenq_e)
{
	struct efa_dev *dev = (struct efa_dev *)data;

	atomic64_inc(&dev->stats.keep_alive_rcvd);
}

static struct efa_aenq_handlers aenq_handlers = {
	.handlers = {
		[EFA_ADMIN_KEEP_ALIVE] = efa_keep_alive,
	},
	.unimplemented_handler = unimplemented_aenq_handler
};

static void efa_release_bars(struct efa_dev *dev, int bars_mask)
{
	struct pci_dev *pdev = dev->pdev;
	int release_bars;

	release_bars = pci_select_bars(pdev, IORESOURCE_MEM) & bars_mask;
	pci_release_selected_regions(pdev, release_bars);
}

static irqreturn_t efa_intr_msix_mgmnt(int irq, void *data)
{
	struct efa_dev *dev = data;

	efa_com_admin_q_comp_intr_handler(&dev->edev);
	efa_com_aenq_intr_handler(&dev->edev, data);

	return IRQ_HANDLED;
}

static int efa_request_mgmnt_irq(struct efa_dev *dev)
{
	struct efa_irq *irq;
	int err;

	irq = &dev->admin_irq;
	err = request_irq(irq->vector, irq->handler, 0, irq->name,
			  irq->data);
	if (err) {
		dev_err(&dev->pdev->dev, "Failed to request admin irq (%d)\n",
			err);
		return err;
	}

	dev_dbg(&dev->pdev->dev, "Set affinity hint of mgmnt irq to %*pbl (irq vector: %d)\n",
		nr_cpumask_bits, &irq->affinity_hint_mask, irq->vector);
	irq_set_affinity_hint(irq->vector, &irq->affinity_hint_mask);

	return 0;
}

static void efa_setup_mgmnt_irq(struct efa_dev *dev)
{
	u32 cpu;

	snprintf(dev->admin_irq.name, EFA_IRQNAME_SIZE,
		 "efa-mgmnt@pci:%s", pci_name(dev->pdev));
	dev->admin_irq.handler = efa_intr_msix_mgmnt;
	dev->admin_irq.data = dev;
	dev->admin_irq.vector =
		pci_irq_vector(dev->pdev, dev->admin_msix_vector_idx);
	cpu = cpumask_first(cpu_online_mask);
	dev->admin_irq.cpu = cpu;
	cpumask_set_cpu(cpu,
			&dev->admin_irq.affinity_hint_mask);
	dev_info(&dev->pdev->dev, "Setup irq:0x%p vector:%d name:%s\n",
		 &dev->admin_irq,
		 dev->admin_irq.vector,
		 dev->admin_irq.name);
}

static void efa_free_mgmnt_irq(struct efa_dev *dev)
{
	struct efa_irq *irq;

	irq = &dev->admin_irq;
	irq_set_affinity_hint(irq->vector, NULL);
	free_irq(irq->vector, irq->data);
}

static int efa_set_mgmnt_irq(struct efa_dev *dev)
{
	efa_setup_mgmnt_irq(dev);

	return efa_request_mgmnt_irq(dev);
}

static int efa_request_doorbell_bar(struct efa_dev *dev)
{
	u8 db_bar_idx = dev->dev_attr.db_bar;
	struct pci_dev *pdev = dev->pdev;
	int bars;
	int err;

	if (!(BIT(db_bar_idx) & EFA_BASE_BAR_MASK)) {
		bars = pci_select_bars(pdev, IORESOURCE_MEM) & BIT(db_bar_idx);

		err = pci_request_selected_regions(pdev, bars, DRV_MODULE_NAME);
		if (err) {
			dev_err(&dev->pdev->dev,
				"pci_request_selected_regions for bar %d failed %d\n",
				db_bar_idx, err);
			return err;
		}
	}

	dev->db_bar_addr = pci_resource_start(dev->pdev, db_bar_idx);
	dev->db_bar_len = pci_resource_len(dev->pdev, db_bar_idx);

	return 0;
}

static void efa_release_doorbell_bar(struct efa_dev *dev)
{
	if (!(BIT(dev->dev_attr.db_bar) & EFA_BASE_BAR_MASK))
		efa_release_bars(dev, BIT(dev->dev_attr.db_bar));
}

static void efa_update_hw_hints(struct efa_dev *dev,
				struct efa_com_get_hw_hints_result *hw_hints)
{
	struct efa_com_dev *edev = &dev->edev;

	if (hw_hints->mmio_read_timeout)
		edev->mmio_read.mmio_read_timeout =
			hw_hints->mmio_read_timeout * 1000;

	if (hw_hints->poll_interval)
		edev->aq.poll_interval = hw_hints->poll_interval;

	if (hw_hints->admin_completion_timeout)
		edev->aq.completion_timeout =
			hw_hints->admin_completion_timeout;
}

static void efa_stats_init(struct efa_dev *dev)
{
	atomic64_t *s = (atomic64_t *)&dev->stats;
	int i;

	for (i = 0; i < sizeof(dev->stats) / sizeof(*s); i++, s++)
		atomic64_set(s, 0);
}

static void efa_set_host_info(struct efa_dev *dev)
{
	struct efa_admin_set_feature_resp resp = {};
	struct efa_admin_set_feature_cmd cmd = {};
	struct efa_admin_host_info *hinf;
	u32 bufsz = sizeof(*hinf);
	dma_addr_t hinf_dma;

	if (!efa_com_check_supported_feature_id(&dev->edev,
						EFA_ADMIN_HOST_INFO))
		return;

	/* Failures in host info set shall not disturb probe */
	hinf = dma_alloc_coherent(&dev->pdev->dev, bufsz, &hinf_dma,
				  GFP_KERNEL);
	if (!hinf)
		return;

	strscpy(hinf->os_dist_str, utsname()->release,
		sizeof(hinf->os_dist_str));
	hinf->os_type = EFA_ADMIN_OS_LINUX;
	strscpy(hinf->kernel_ver_str, utsname()->version,
		sizeof(hinf->kernel_ver_str));
	hinf->kernel_ver = LINUX_VERSION_CODE;
	EFA_SET(&hinf->driver_ver, EFA_ADMIN_HOST_INFO_DRIVER_MAJOR, 0);
	EFA_SET(&hinf->driver_ver, EFA_ADMIN_HOST_INFO_DRIVER_MINOR, 0);
	EFA_SET(&hinf->driver_ver, EFA_ADMIN_HOST_INFO_DRIVER_SUB_MINOR, 0);
	EFA_SET(&hinf->driver_ver, EFA_ADMIN_HOST_INFO_DRIVER_MODULE_TYPE, 0);
	EFA_SET(&hinf->bdf, EFA_ADMIN_HOST_INFO_BUS, dev->pdev->bus->number);
	EFA_SET(&hinf->bdf, EFA_ADMIN_HOST_INFO_DEVICE,
		PCI_SLOT(dev->pdev->devfn));
	EFA_SET(&hinf->bdf, EFA_ADMIN_HOST_INFO_FUNCTION,
		PCI_FUNC(dev->pdev->devfn));
	EFA_SET(&hinf->spec_ver, EFA_ADMIN_HOST_INFO_SPEC_MAJOR,
		EFA_COMMON_SPEC_VERSION_MAJOR);
	EFA_SET(&hinf->spec_ver, EFA_ADMIN_HOST_INFO_SPEC_MINOR,
		EFA_COMMON_SPEC_VERSION_MINOR);
	EFA_SET(&hinf->flags, EFA_ADMIN_HOST_INFO_INTREE, 1);
	EFA_SET(&hinf->flags, EFA_ADMIN_HOST_INFO_GDR, 0);

	efa_com_set_feature_ex(&dev->edev, &resp, &cmd, EFA_ADMIN_HOST_INFO,
			       hinf_dma, bufsz);

	dma_free_coherent(&dev->pdev->dev, bufsz, hinf, hinf_dma);
}

static const struct ib_device_ops efa_dev_ops = {
	.owner = THIS_MODULE,
	.driver_id = RDMA_DRIVER_EFA,
	.uverbs_abi_ver = EFA_UVERBS_ABI_VERSION,

	.alloc_hw_stats = efa_alloc_hw_stats,
	.alloc_pd = efa_alloc_pd,
	.alloc_ucontext = efa_alloc_ucontext,
	.create_cq = efa_create_cq,
	.create_qp = efa_create_qp,
	.create_user_ah = efa_create_ah,
	.dealloc_pd = efa_dealloc_pd,
	.dealloc_ucontext = efa_dealloc_ucontext,
	.dereg_mr = efa_dereg_mr,
	.destroy_ah = efa_destroy_ah,
	.destroy_cq = efa_destroy_cq,
	.destroy_qp = efa_destroy_qp,
	.get_hw_stats = efa_get_hw_stats,
	.get_link_layer = efa_port_link_layer,
	.get_port_immutable = efa_get_port_immutable,
	.mmap = efa_mmap,
	.mmap_free = efa_mmap_free,
	.modify_qp = efa_modify_qp,
	.query_device = efa_query_device,
	.query_gid = efa_query_gid,
	.query_pkey = efa_query_pkey,
	.query_port = efa_query_port,
	.query_qp = efa_query_qp,
	.reg_user_mr = efa_reg_mr,

	INIT_RDMA_OBJ_SIZE(ib_ah, efa_ah, ibah),
	INIT_RDMA_OBJ_SIZE(ib_cq, efa_cq, ibcq),
	INIT_RDMA_OBJ_SIZE(ib_pd, efa_pd, ibpd),
	INIT_RDMA_OBJ_SIZE(ib_ucontext, efa_ucontext, ibucontext),
};

static int efa_ib_device_add(struct efa_dev *dev)
{
	struct efa_com_get_hw_hints_result hw_hints;
	struct pci_dev *pdev = dev->pdev;
	int err;

	efa_stats_init(dev);

	err = efa_com_get_device_attr(&dev->edev, &dev->dev_attr);
	if (err)
		return err;

	dev_dbg(&dev->pdev->dev, "Doorbells bar (%d)\n", dev->dev_attr.db_bar);
	err = efa_request_doorbell_bar(dev);
	if (err)
		return err;

	err = efa_com_get_hw_hints(&dev->edev, &hw_hints);
	if (err)
		goto err_release_doorbell_bar;

	efa_update_hw_hints(dev, &hw_hints);

	/* Try to enable all the available aenq groups */
	err = efa_com_set_aenq_config(&dev->edev, EFA_AENQ_ENABLED_GROUPS);
	if (err)
		goto err_release_doorbell_bar;

	efa_set_host_info(dev);

	dev->ibdev.node_type = RDMA_NODE_UNSPECIFIED;
	dev->ibdev.phys_port_cnt = 1;
	dev->ibdev.num_comp_vectors = 1;
	dev->ibdev.dev.parent = &pdev->dev;

	ib_set_device_ops(&dev->ibdev, &efa_dev_ops);

	err = ib_register_device(&dev->ibdev, "efa_%d", &pdev->dev);
	if (err)
		goto err_release_doorbell_bar;

	ibdev_info(&dev->ibdev, "IB device registered\n");

	return 0;

err_release_doorbell_bar:
	efa_release_doorbell_bar(dev);
	return err;
}

static void efa_ib_device_remove(struct efa_dev *dev)
{
	efa_com_dev_reset(&dev->edev, EFA_REGS_RESET_NORMAL);
	ibdev_info(&dev->ibdev, "Unregister ib device\n");
	ib_unregister_device(&dev->ibdev);
	efa_release_doorbell_bar(dev);
}

static void efa_disable_msix(struct efa_dev *dev)
{
	pci_free_irq_vectors(dev->pdev);
}

static int efa_enable_msix(struct efa_dev *dev)
{
	int msix_vecs, irq_num;

	/* Reserve the max msix vectors we might need */
	msix_vecs = EFA_NUM_MSIX_VEC;
	dev_dbg(&dev->pdev->dev, "Trying to enable MSI-X, vectors %d\n",
		msix_vecs);

	dev->admin_msix_vector_idx = EFA_MGMNT_MSIX_VEC_IDX;
	irq_num = pci_alloc_irq_vectors(dev->pdev, msix_vecs,
					msix_vecs, PCI_IRQ_MSIX);

	if (irq_num < 0) {
		dev_err(&dev->pdev->dev, "Failed to enable MSI-X. irq_num %d\n",
			irq_num);
		return -ENOSPC;
	}

	if (irq_num != msix_vecs) {
		dev_err(&dev->pdev->dev,
			"Allocated %d MSI-X (out of %d requested)\n",
			irq_num, msix_vecs);
		return -ENOSPC;
	}

	return 0;
}

static int efa_device_init(struct efa_com_dev *edev, struct pci_dev *pdev)
{
	int dma_width;
	int err;

	err = efa_com_dev_reset(edev, EFA_REGS_RESET_NORMAL);
	if (err)
		return err;

	err = efa_com_validate_version(edev);
	if (err)
		return err;

	dma_width = efa_com_get_dma_width(edev);
	if (dma_width < 0) {
		err = dma_width;
		return err;
	}

	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(dma_width));
	if (err) {
		dev_err(&pdev->dev, "dma_set_mask_and_coherent failed %d\n", err);
		return err;
	}

	dma_set_max_seg_size(&pdev->dev, UINT_MAX);
	return 0;
}

static struct efa_dev *efa_probe_device(struct pci_dev *pdev)
{
	struct efa_com_dev *edev;
	struct efa_dev *dev;
	int bars;
	int err;

	err = pci_enable_device_mem(pdev);
	if (err) {
		dev_err(&pdev->dev, "pci_enable_device_mem() failed!\n");
		return ERR_PTR(err);
	}

	pci_set_master(pdev);

	dev = ib_alloc_device(efa_dev, ibdev);
	if (!dev) {
		dev_err(&pdev->dev, "Device alloc failed\n");
		err = -ENOMEM;
		goto err_disable_device;
	}

	pci_set_drvdata(pdev, dev);
	edev = &dev->edev;
	edev->efa_dev = dev;
	edev->dmadev = &pdev->dev;
	dev->pdev = pdev;

	bars = pci_select_bars(pdev, IORESOURCE_MEM) & EFA_BASE_BAR_MASK;
	err = pci_request_selected_regions(pdev, bars, DRV_MODULE_NAME);
	if (err) {
		dev_err(&pdev->dev, "pci_request_selected_regions failed %d\n",
			err);
		goto err_ibdev_destroy;
	}

	dev->reg_bar_addr = pci_resource_start(pdev, EFA_REG_BAR);
	dev->reg_bar_len = pci_resource_len(pdev, EFA_REG_BAR);
	dev->mem_bar_addr = pci_resource_start(pdev, EFA_MEM_BAR);
	dev->mem_bar_len = pci_resource_len(pdev, EFA_MEM_BAR);

	edev->reg_bar = devm_ioremap(&pdev->dev,
				     dev->reg_bar_addr,
				     dev->reg_bar_len);
	if (!edev->reg_bar) {
		dev_err(&pdev->dev, "Failed to remap register bar\n");
		err = -EFAULT;
		goto err_release_bars;
	}

	err = efa_com_mmio_reg_read_init(edev);
	if (err) {
		dev_err(&pdev->dev, "Failed to init readless MMIO\n");
		goto err_iounmap;
	}

	err = efa_device_init(edev, pdev);
	if (err) {
		dev_err(&pdev->dev, "EFA device init failed\n");
		if (err == -ETIME)
			err = -EPROBE_DEFER;
		goto err_reg_read_destroy;
	}

	err = efa_enable_msix(dev);
	if (err)
		goto err_reg_read_destroy;

	edev->aq.msix_vector_idx = dev->admin_msix_vector_idx;
	edev->aenq.msix_vector_idx = dev->admin_msix_vector_idx;

	err = efa_set_mgmnt_irq(dev);
	if (err)
		goto err_disable_msix;

	err = efa_com_admin_init(edev, &aenq_handlers);
	if (err)
		goto err_free_mgmnt_irq;

	return dev;

err_free_mgmnt_irq:
	efa_free_mgmnt_irq(dev);
err_disable_msix:
	efa_disable_msix(dev);
err_reg_read_destroy:
	efa_com_mmio_reg_read_destroy(edev);
err_iounmap:
	devm_iounmap(&pdev->dev, edev->reg_bar);
err_release_bars:
	efa_release_bars(dev, EFA_BASE_BAR_MASK);
err_ibdev_destroy:
	ib_dealloc_device(&dev->ibdev);
err_disable_device:
	pci_disable_device(pdev);
	return ERR_PTR(err);
}

static void efa_remove_device(struct pci_dev *pdev)
{
	struct efa_dev *dev = pci_get_drvdata(pdev);
	struct efa_com_dev *edev;

	edev = &dev->edev;
	efa_com_admin_destroy(edev);
	efa_free_mgmnt_irq(dev);
	efa_disable_msix(dev);
	efa_com_mmio_reg_read_destroy(edev);
	devm_iounmap(&pdev->dev, edev->reg_bar);
	efa_release_bars(dev, EFA_BASE_BAR_MASK);
	ib_dealloc_device(&dev->ibdev);
	pci_disable_device(pdev);
}

static int efa_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct efa_dev *dev;
	int err;

	dev = efa_probe_device(pdev);
	if (IS_ERR(dev))
		return PTR_ERR(dev);

	err = efa_ib_device_add(dev);
	if (err)
		goto err_remove_device;

	return 0;

err_remove_device:
	efa_remove_device(pdev);
	return err;
}

static void efa_remove(struct pci_dev *pdev)
{
	struct efa_dev *dev = pci_get_drvdata(pdev);

	efa_ib_device_remove(dev);
	efa_remove_device(pdev);
}

static struct pci_driver efa_pci_driver = {
	.name           = DRV_MODULE_NAME,
	.id_table       = efa_pci_tbl,
	.probe          = efa_probe,
	.remove         = efa_remove,
};

module_pci_driver(efa_pci_driver);

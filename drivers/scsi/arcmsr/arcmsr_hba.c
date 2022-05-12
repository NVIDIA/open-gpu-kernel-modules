/*
*******************************************************************************
**        O.S   : Linux
**   FILE NAME  : arcmsr_hba.c
**        BY    : Nick Cheng, C.L. Huang
**   Description: SCSI RAID Device Driver for Areca RAID Controller
*******************************************************************************
** Copyright (C) 2002 - 2014, Areca Technology Corporation All rights reserved
**
**     Web site: www.areca.com.tw
**       E-mail: support@areca.com.tw
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License version 2 as
** published by the Free Software Foundation.
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*******************************************************************************
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING,BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION)HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE)ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
** For history of changes, see Documentation/scsi/ChangeLog.arcmsr
**     Firmware Specification, see Documentation/scsi/arcmsr_spec.rst
*******************************************************************************
*/
#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/spinlock.h>
#include <linux/pci_ids.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/aer.h>
#include <linux/circ_buf.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_tcq.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsicam.h>
#include "arcmsr.h"
MODULE_AUTHOR("Nick Cheng, C.L. Huang <support@areca.com.tw>");
MODULE_DESCRIPTION("Areca ARC11xx/12xx/16xx/188x SAS/SATA RAID Controller Driver");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(ARCMSR_DRIVER_VERSION);

static int msix_enable = 1;
module_param(msix_enable, int, S_IRUGO);
MODULE_PARM_DESC(msix_enable, "Enable MSI-X interrupt(0 ~ 1), msix_enable=1(enable), =0(disable)");

static int msi_enable = 1;
module_param(msi_enable, int, S_IRUGO);
MODULE_PARM_DESC(msi_enable, "Enable MSI interrupt(0 ~ 1), msi_enable=1(enable), =0(disable)");

static int host_can_queue = ARCMSR_DEFAULT_OUTSTANDING_CMD;
module_param(host_can_queue, int, S_IRUGO);
MODULE_PARM_DESC(host_can_queue, " adapter queue depth(32 ~ 1024), default is 128");

static int cmd_per_lun = ARCMSR_DEFAULT_CMD_PERLUN;
module_param(cmd_per_lun, int, S_IRUGO);
MODULE_PARM_DESC(cmd_per_lun, " device queue depth(1 ~ 128), default is 32");

static int dma_mask_64 = 0;
module_param(dma_mask_64, int, S_IRUGO);
MODULE_PARM_DESC(dma_mask_64, " set DMA mask to 64 bits(0 ~ 1), dma_mask_64=1(64 bits), =0(32 bits)");

static int set_date_time = 0;
module_param(set_date_time, int, S_IRUGO);
MODULE_PARM_DESC(set_date_time, " send date, time to iop(0 ~ 1), set_date_time=1(enable), default(=0) is disable");

static int cmd_timeout = ARCMSR_DEFAULT_TIMEOUT;
module_param(cmd_timeout, int, S_IRUGO);
MODULE_PARM_DESC(cmd_timeout, " scsi cmd timeout(0 ~ 120 sec.), default is 90");

#define	ARCMSR_SLEEPTIME	10
#define	ARCMSR_RETRYCOUNT	12

static wait_queue_head_t wait_q;
static int arcmsr_iop_message_xfer(struct AdapterControlBlock *acb,
					struct scsi_cmnd *cmd);
static int arcmsr_iop_confirm(struct AdapterControlBlock *acb);
static int arcmsr_abort(struct scsi_cmnd *);
static int arcmsr_bus_reset(struct scsi_cmnd *);
static int arcmsr_bios_param(struct scsi_device *sdev,
		struct block_device *bdev, sector_t capacity, int *info);
static int arcmsr_queue_command(struct Scsi_Host *h, struct scsi_cmnd *cmd);
static int arcmsr_probe(struct pci_dev *pdev,
				const struct pci_device_id *id);
static int __maybe_unused arcmsr_suspend(struct device *dev);
static int __maybe_unused arcmsr_resume(struct device *dev);
static void arcmsr_remove(struct pci_dev *pdev);
static void arcmsr_shutdown(struct pci_dev *pdev);
static void arcmsr_iop_init(struct AdapterControlBlock *acb);
static void arcmsr_free_ccb_pool(struct AdapterControlBlock *acb);
static u32 arcmsr_disable_outbound_ints(struct AdapterControlBlock *acb);
static void arcmsr_enable_outbound_ints(struct AdapterControlBlock *acb,
	u32 intmask_org);
static void arcmsr_stop_adapter_bgrb(struct AdapterControlBlock *acb);
static void arcmsr_hbaA_flush_cache(struct AdapterControlBlock *acb);
static void arcmsr_hbaB_flush_cache(struct AdapterControlBlock *acb);
static void arcmsr_request_device_map(struct timer_list *t);
static void arcmsr_message_isr_bh_fn(struct work_struct *work);
static bool arcmsr_get_firmware_spec(struct AdapterControlBlock *acb);
static void arcmsr_start_adapter_bgrb(struct AdapterControlBlock *acb);
static void arcmsr_hbaC_message_isr(struct AdapterControlBlock *pACB);
static void arcmsr_hbaD_message_isr(struct AdapterControlBlock *acb);
static void arcmsr_hbaE_message_isr(struct AdapterControlBlock *acb);
static void arcmsr_hbaE_postqueue_isr(struct AdapterControlBlock *acb);
static void arcmsr_hbaF_postqueue_isr(struct AdapterControlBlock *acb);
static void arcmsr_hardware_reset(struct AdapterControlBlock *acb);
static const char *arcmsr_info(struct Scsi_Host *);
static irqreturn_t arcmsr_interrupt(struct AdapterControlBlock *acb);
static void arcmsr_free_irq(struct pci_dev *, struct AdapterControlBlock *);
static void arcmsr_wait_firmware_ready(struct AdapterControlBlock *acb);
static void arcmsr_set_iop_datetime(struct timer_list *);
static int arcmsr_slave_config(struct scsi_device *sdev);
static int arcmsr_adjust_disk_queue_depth(struct scsi_device *sdev, int queue_depth)
{
	if (queue_depth > ARCMSR_MAX_CMD_PERLUN)
		queue_depth = ARCMSR_MAX_CMD_PERLUN;
	return scsi_change_queue_depth(sdev, queue_depth);
}

static struct scsi_host_template arcmsr_scsi_host_template = {
	.module			= THIS_MODULE,
	.name			= "Areca SAS/SATA RAID driver",
	.info			= arcmsr_info,
	.queuecommand		= arcmsr_queue_command,
	.eh_abort_handler	= arcmsr_abort,
	.eh_bus_reset_handler	= arcmsr_bus_reset,
	.bios_param		= arcmsr_bios_param,
	.slave_configure	= arcmsr_slave_config,
	.change_queue_depth	= arcmsr_adjust_disk_queue_depth,
	.can_queue		= ARCMSR_DEFAULT_OUTSTANDING_CMD,
	.this_id		= ARCMSR_SCSI_INITIATOR_ID,
	.sg_tablesize	        = ARCMSR_DEFAULT_SG_ENTRIES,
	.max_sectors		= ARCMSR_MAX_XFER_SECTORS_C,
	.cmd_per_lun		= ARCMSR_DEFAULT_CMD_PERLUN,
	.shost_attrs		= arcmsr_host_attrs,
	.no_write_same		= 1,
};

static struct pci_device_id arcmsr_device_id_table[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1110),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1120),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1130),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1160),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1170),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1200),
		.driver_data = ACB_ADAPTER_TYPE_B},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1201),
		.driver_data = ACB_ADAPTER_TYPE_B},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1202),
		.driver_data = ACB_ADAPTER_TYPE_B},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1203),
		.driver_data = ACB_ADAPTER_TYPE_B},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1210),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1214),
		.driver_data = ACB_ADAPTER_TYPE_D},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1220),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1230),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1260),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1270),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1280),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1380),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1381),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1680),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1681),
		.driver_data = ACB_ADAPTER_TYPE_A},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1880),
		.driver_data = ACB_ADAPTER_TYPE_C},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1884),
		.driver_data = ACB_ADAPTER_TYPE_E},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1886),
		.driver_data = ACB_ADAPTER_TYPE_F},
	{0, 0}, /* Terminating entry */
};
MODULE_DEVICE_TABLE(pci, arcmsr_device_id_table);

static SIMPLE_DEV_PM_OPS(arcmsr_pm_ops, arcmsr_suspend, arcmsr_resume);

static struct pci_driver arcmsr_pci_driver = {
	.name			= "arcmsr",
	.id_table		= arcmsr_device_id_table,
	.probe			= arcmsr_probe,
	.remove			= arcmsr_remove,
	.driver.pm		= &arcmsr_pm_ops,
	.shutdown		= arcmsr_shutdown,
};
/*
****************************************************************************
****************************************************************************
*/

static void arcmsr_free_io_queue(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B:
	case ACB_ADAPTER_TYPE_D:
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		dma_free_coherent(&acb->pdev->dev, acb->ioqueue_size,
			acb->dma_coherent2, acb->dma_coherent_handle2);
		break;
	}
}

static bool arcmsr_remap_pciregion(struct AdapterControlBlock *acb)
{
	struct pci_dev *pdev = acb->pdev;
	switch (acb->adapter_type){
	case ACB_ADAPTER_TYPE_A:{
		acb->pmuA = ioremap(pci_resource_start(pdev,0), pci_resource_len(pdev,0));
		if (!acb->pmuA) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		break;
	}
	case ACB_ADAPTER_TYPE_B:{
		void __iomem *mem_base0, *mem_base1;
		mem_base0 = ioremap(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));
		if (!mem_base0) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		mem_base1 = ioremap(pci_resource_start(pdev, 2), pci_resource_len(pdev, 2));
		if (!mem_base1) {
			iounmap(mem_base0);
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		acb->mem_base0 = mem_base0;
		acb->mem_base1 = mem_base1;
		break;
	}
	case ACB_ADAPTER_TYPE_C:{
		acb->pmuC = ioremap(pci_resource_start(pdev, 1), pci_resource_len(pdev, 1));
		if (!acb->pmuC) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		if (readl(&acb->pmuC->outbound_doorbell) & ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR, &acb->pmuC->outbound_doorbell_clear);/*clear interrupt*/
			return true;
		}
		break;
	}
	case ACB_ADAPTER_TYPE_D: {
		void __iomem *mem_base0;
		unsigned long addr, range;

		addr = (unsigned long)pci_resource_start(pdev, 0);
		range = pci_resource_len(pdev, 0);
		mem_base0 = ioremap(addr, range);
		if (!mem_base0) {
			pr_notice("arcmsr%d: memory mapping region fail\n",
				acb->host->host_no);
			return false;
		}
		acb->mem_base0 = mem_base0;
		break;
		}
	case ACB_ADAPTER_TYPE_E: {
		acb->pmuE = ioremap(pci_resource_start(pdev, 1),
			pci_resource_len(pdev, 1));
		if (!acb->pmuE) {
			pr_notice("arcmsr%d: memory mapping region fail \n",
				acb->host->host_no);
			return false;
		}
		writel(0, &acb->pmuE->host_int_status); /*clear interrupt*/
		writel(ARCMSR_HBEMU_DOORBELL_SYNC, &acb->pmuE->iobound_doorbell);	/* synchronize doorbell to 0 */
		acb->in_doorbell = 0;
		acb->out_doorbell = 0;
		break;
		}
	case ACB_ADAPTER_TYPE_F: {
		acb->pmuF = ioremap(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));
		if (!acb->pmuF) {
			pr_notice("arcmsr%d: memory mapping region fail\n",
				acb->host->host_no);
			return false;
		}
		writel(0, &acb->pmuF->host_int_status); /* clear interrupt */
		writel(ARCMSR_HBFMU_DOORBELL_SYNC, &acb->pmuF->iobound_doorbell);
		acb->in_doorbell = 0;
		acb->out_doorbell = 0;
		break;
		}
	}
	return true;
}

static void arcmsr_unmap_pciregion(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		iounmap(acb->pmuA);
		break;
	case ACB_ADAPTER_TYPE_B:
		iounmap(acb->mem_base0);
		iounmap(acb->mem_base1);
		break;
	case ACB_ADAPTER_TYPE_C:
		iounmap(acb->pmuC);
		break;
	case ACB_ADAPTER_TYPE_D:
		iounmap(acb->mem_base0);
		break;
	case ACB_ADAPTER_TYPE_E:
		iounmap(acb->pmuE);
		break;
	case ACB_ADAPTER_TYPE_F:
		iounmap(acb->pmuF);
		break;
	}
}

static irqreturn_t arcmsr_do_interrupt(int irq, void *dev_id)
{
	irqreturn_t handle_state;
	struct AdapterControlBlock *acb = dev_id;

	handle_state = arcmsr_interrupt(acb);
	return handle_state;
}

static int arcmsr_bios_param(struct scsi_device *sdev,
		struct block_device *bdev, sector_t capacity, int *geom)
{
	int heads, sectors, cylinders, total_capacity;

	if (scsi_partsize(bdev, capacity, geom))
		return 0;

	total_capacity = capacity;
	heads = 64;
	sectors = 32;
	cylinders = total_capacity / (heads * sectors);
	if (cylinders > 1024) {
		heads = 255;
		sectors = 63;
		cylinders = total_capacity / (heads * sectors);
	}
	geom[0] = heads;
	geom[1] = sectors;
	geom[2] = cylinders;
	return 0;
}

static uint8_t arcmsr_hbaA_wait_msgint_ready(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(&reg->outbound_intstatus) &
				ARCMSR_MU_OUTBOUND_MESSAGE0_INT) {
			writel(ARCMSR_MU_OUTBOUND_MESSAGE0_INT,
				&reg->outbound_intstatus);
			return true;
		}
		msleep(10);
	} /* max 20 seconds */

	return false;
}

static uint8_t arcmsr_hbaB_wait_msgint_ready(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(reg->iop2drv_doorbell)
			& ARCMSR_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN,
					reg->iop2drv_doorbell);
			writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT,
					reg->drv2iop_doorbell);
			return true;
		}
		msleep(10);
	} /* max 20 seconds */

	return false;
}

static uint8_t arcmsr_hbaC_wait_msgint_ready(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *phbcmu = pACB->pmuC;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(&phbcmu->outbound_doorbell)
				& ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR,
				&phbcmu->outbound_doorbell_clear); /*clear interrupt*/
			return true;
		}
		msleep(10);
	} /* max 20 seconds */

	return false;
}

static bool arcmsr_hbaD_wait_msgint_ready(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *reg = pACB->pmuD;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(reg->outbound_doorbell)
			& ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE,
				reg->outbound_doorbell);
			return true;
		}
		msleep(10);
	} /* max 20 seconds */
	return false;
}

static bool arcmsr_hbaE_wait_msgint_ready(struct AdapterControlBlock *pACB)
{
	int i;
	uint32_t read_doorbell;
	struct MessageUnit_E __iomem *phbcmu = pACB->pmuE;

	for (i = 0; i < 2000; i++) {
		read_doorbell = readl(&phbcmu->iobound_doorbell);
		if ((read_doorbell ^ pACB->in_doorbell) & ARCMSR_HBEMU_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(0, &phbcmu->host_int_status); /*clear interrupt*/
			pACB->in_doorbell = read_doorbell;
			return true;
		}
		msleep(10);
	} /* max 20 seconds */
	return false;
}

static void arcmsr_hbaA_flush_cache(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	int retry_count = 30;
	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, &reg->inbound_msgaddr0);
	do {
		if (arcmsr_hbaA_wait_msgint_ready(acb))
			break;
		else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout, retry count down = %d \n", acb->host->host_no, retry_count);
		}
	} while (retry_count != 0);
}

static void arcmsr_hbaB_flush_cache(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	int retry_count = 30;
	writel(ARCMSR_MESSAGE_FLUSH_CACHE, reg->drv2iop_doorbell);
	do {
		if (arcmsr_hbaB_wait_msgint_ready(acb))
			break;
		else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout,retry count down = %d \n", acb->host->host_no, retry_count);
		}
	} while (retry_count != 0);
}

static void arcmsr_hbaC_flush_cache(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	int retry_count = 30;/* enlarge wait flush adapter cache time: 10 minute */
	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	do {
		if (arcmsr_hbaC_wait_msgint_ready(pACB)) {
			break;
		} else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout,retry count down = %d \n", pACB->host->host_no, retry_count);
		}
	} while (retry_count != 0);
	return;
}

static void arcmsr_hbaD_flush_cache(struct AdapterControlBlock *pACB)
{
	int retry_count = 15;
	struct MessageUnit_D *reg = pACB->pmuD;

	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, reg->inbound_msgaddr0);
	do {
		if (arcmsr_hbaD_wait_msgint_ready(pACB))
			break;

		retry_count--;
		pr_notice("arcmsr%d: wait 'flush adapter "
			"cache' timeout, retry count down = %d\n",
			pACB->host->host_no, retry_count);
	} while (retry_count != 0);
}

static void arcmsr_hbaE_flush_cache(struct AdapterControlBlock *pACB)
{
	int retry_count = 30;
	struct MessageUnit_E __iomem *reg = pACB->pmuE;

	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, &reg->inbound_msgaddr0);
	pACB->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
	writel(pACB->out_doorbell, &reg->iobound_doorbell);
	do {
		if (arcmsr_hbaE_wait_msgint_ready(pACB))
			break;
		retry_count--;
		pr_notice("arcmsr%d: wait 'flush adapter "
			"cache' timeout, retry count down = %d\n",
			pACB->host->host_no, retry_count);
	} while (retry_count != 0);
}

static void arcmsr_flush_adapter_cache(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A:
		arcmsr_hbaA_flush_cache(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		arcmsr_hbaB_flush_cache(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		arcmsr_hbaC_flush_cache(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		arcmsr_hbaD_flush_cache(acb);
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		arcmsr_hbaE_flush_cache(acb);
		break;
	}
}

static void arcmsr_hbaB_assign_regAddr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;

	if (acb->pdev->device == PCI_DEVICE_ID_ARECA_1203) {
		reg->drv2iop_doorbell = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL_1203);
		reg->drv2iop_doorbell_mask = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL_MASK_1203);
		reg->iop2drv_doorbell = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL_1203);
		reg->iop2drv_doorbell_mask = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL_MASK_1203);
	} else {
		reg->drv2iop_doorbell= MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL);
		reg->drv2iop_doorbell_mask = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL_MASK);
		reg->iop2drv_doorbell = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL);
		reg->iop2drv_doorbell_mask = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL_MASK);
	}
	reg->message_wbuffer = MEM_BASE1(ARCMSR_MESSAGE_WBUFFER);
	reg->message_rbuffer =  MEM_BASE1(ARCMSR_MESSAGE_RBUFFER);
	reg->message_rwbuffer = MEM_BASE1(ARCMSR_MESSAGE_RWBUFFER);
}

static void arcmsr_hbaD_assign_regAddr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_D *reg = acb->pmuD;

	reg->chip_id = MEM_BASE0(ARCMSR_ARC1214_CHIP_ID);
	reg->cpu_mem_config = MEM_BASE0(ARCMSR_ARC1214_CPU_MEMORY_CONFIGURATION);
	reg->i2o_host_interrupt_mask = MEM_BASE0(ARCMSR_ARC1214_I2_HOST_INTERRUPT_MASK);
	reg->sample_at_reset = MEM_BASE0(ARCMSR_ARC1214_SAMPLE_RESET);
	reg->reset_request = MEM_BASE0(ARCMSR_ARC1214_RESET_REQUEST);
	reg->host_int_status = MEM_BASE0(ARCMSR_ARC1214_MAIN_INTERRUPT_STATUS);
	reg->pcief0_int_enable = MEM_BASE0(ARCMSR_ARC1214_PCIE_F0_INTERRUPT_ENABLE);
	reg->inbound_msgaddr0 = MEM_BASE0(ARCMSR_ARC1214_INBOUND_MESSAGE0);
	reg->inbound_msgaddr1 = MEM_BASE0(ARCMSR_ARC1214_INBOUND_MESSAGE1);
	reg->outbound_msgaddr0 = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_MESSAGE0);
	reg->outbound_msgaddr1 = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_MESSAGE1);
	reg->inbound_doorbell = MEM_BASE0(ARCMSR_ARC1214_INBOUND_DOORBELL);
	reg->outbound_doorbell = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_DOORBELL);
	reg->outbound_doorbell_enable = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_DOORBELL_ENABLE);
	reg->inboundlist_base_low = MEM_BASE0(ARCMSR_ARC1214_INBOUND_LIST_BASE_LOW);
	reg->inboundlist_base_high = MEM_BASE0(ARCMSR_ARC1214_INBOUND_LIST_BASE_HIGH);
	reg->inboundlist_write_pointer = MEM_BASE0(ARCMSR_ARC1214_INBOUND_LIST_WRITE_POINTER);
	reg->outboundlist_base_low = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_BASE_LOW);
	reg->outboundlist_base_high = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_BASE_HIGH);
	reg->outboundlist_copy_pointer = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_COPY_POINTER);
	reg->outboundlist_read_pointer = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_READ_POINTER);
	reg->outboundlist_interrupt_cause = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_INTERRUPT_CAUSE);
	reg->outboundlist_interrupt_enable = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_INTERRUPT_ENABLE);
	reg->message_wbuffer = MEM_BASE0(ARCMSR_ARC1214_MESSAGE_WBUFFER);
	reg->message_rbuffer = MEM_BASE0(ARCMSR_ARC1214_MESSAGE_RBUFFER);
	reg->msgcode_rwbuffer = MEM_BASE0(ARCMSR_ARC1214_MESSAGE_RWBUFFER);
}

static void arcmsr_hbaF_assign_regAddr(struct AdapterControlBlock *acb)
{
	dma_addr_t host_buffer_dma;
	struct MessageUnit_F __iomem *pmuF;

	memset(acb->dma_coherent2, 0xff, acb->completeQ_size);
	acb->message_wbuffer = (uint32_t *)round_up((unsigned long)acb->dma_coherent2 +
		acb->completeQ_size, 4);
	acb->message_rbuffer = ((void *)acb->message_wbuffer) + 0x100;
	acb->msgcode_rwbuffer = ((void *)acb->message_wbuffer) + 0x200;
	memset((void *)acb->message_wbuffer, 0, MESG_RW_BUFFER_SIZE);
	host_buffer_dma = round_up(acb->dma_coherent_handle2 + acb->completeQ_size, 4);
	pmuF = acb->pmuF;
	/* host buffer low address, bit0:1 all buffer active */
	writel(lower_32_bits(host_buffer_dma | 1), &pmuF->inbound_msgaddr0);
	/* host buffer high address */
	writel(upper_32_bits(host_buffer_dma), &pmuF->inbound_msgaddr1);
	/* set host buffer physical address */
	writel(ARCMSR_HBFMU_DOORBELL_SYNC1, &pmuF->iobound_doorbell);
}

static bool arcmsr_alloc_io_queue(struct AdapterControlBlock *acb)
{
	bool rtn = true;
	void *dma_coherent;
	dma_addr_t dma_coherent_handle;
	struct pci_dev *pdev = acb->pdev;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B: {
		acb->ioqueue_size = roundup(sizeof(struct MessageUnit_B), 32);
		dma_coherent = dma_alloc_coherent(&pdev->dev, acb->ioqueue_size,
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent) {
			pr_notice("arcmsr%d: DMA allocation failed\n", acb->host->host_no);
			return false;
		}
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = dma_coherent;
		acb->pmuB = (struct MessageUnit_B *)dma_coherent;
		arcmsr_hbaB_assign_regAddr(acb);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		acb->ioqueue_size = roundup(sizeof(struct MessageUnit_D), 32);
		dma_coherent = dma_alloc_coherent(&pdev->dev, acb->ioqueue_size,
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent) {
			pr_notice("arcmsr%d: DMA allocation failed\n", acb->host->host_no);
			return false;
		}
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = dma_coherent;
		acb->pmuD = (struct MessageUnit_D *)dma_coherent;
		arcmsr_hbaD_assign_regAddr(acb);
		}
		break;
	case ACB_ADAPTER_TYPE_E: {
		uint32_t completeQ_size;
		completeQ_size = sizeof(struct deliver_completeQ) * ARCMSR_MAX_HBE_DONEQUEUE + 128;
		acb->ioqueue_size = roundup(completeQ_size, 32);
		dma_coherent = dma_alloc_coherent(&pdev->dev, acb->ioqueue_size,
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent){
			pr_notice("arcmsr%d: DMA allocation failed\n", acb->host->host_no);
			return false;
		}
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = dma_coherent;
		acb->pCompletionQ = dma_coherent;
		acb->completionQ_entry = acb->ioqueue_size / sizeof(struct deliver_completeQ);
		acb->doneq_index = 0;
		}
		break;
	case ACB_ADAPTER_TYPE_F: {
		uint32_t QueueDepth;
		uint32_t depthTbl[] = {256, 512, 1024, 128, 64, 32};

		arcmsr_wait_firmware_ready(acb);
		QueueDepth = depthTbl[readl(&acb->pmuF->outbound_msgaddr1) & 7];
		acb->completeQ_size = sizeof(struct deliver_completeQ) * QueueDepth + 128;
		acb->ioqueue_size = roundup(acb->completeQ_size + MESG_RW_BUFFER_SIZE, 32);
		dma_coherent = dma_alloc_coherent(&pdev->dev, acb->ioqueue_size,
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent) {
			pr_notice("arcmsr%d: DMA allocation failed\n", acb->host->host_no);
			return false;
		}
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = dma_coherent;
		acb->pCompletionQ = dma_coherent;
		acb->completionQ_entry = acb->completeQ_size / sizeof(struct deliver_completeQ);
		acb->doneq_index = 0;
		arcmsr_hbaF_assign_regAddr(acb);
		}
		break;
	default:
		break;
	}
	return rtn;
}

static int arcmsr_alloc_ccb_pool(struct AdapterControlBlock *acb)
{
	struct pci_dev *pdev = acb->pdev;
	void *dma_coherent;
	dma_addr_t dma_coherent_handle;
	struct CommandControlBlock *ccb_tmp;
	int i = 0, j = 0;
	unsigned long cdb_phyaddr, next_ccb_phy;
	unsigned long roundup_ccbsize;
	unsigned long max_xfer_len;
	unsigned long max_sg_entrys;
	uint32_t  firm_config_version, curr_phy_upper32;

	for (i = 0; i < ARCMSR_MAX_TARGETID; i++)
		for (j = 0; j < ARCMSR_MAX_TARGETLUN; j++)
			acb->devstate[i][j] = ARECA_RAID_GONE;

	max_xfer_len = ARCMSR_MAX_XFER_LEN;
	max_sg_entrys = ARCMSR_DEFAULT_SG_ENTRIES;
	firm_config_version = acb->firm_cfg_version;
	if((firm_config_version & 0xFF) >= 3){
		max_xfer_len = (ARCMSR_CDB_SG_PAGE_LENGTH << ((firm_config_version >> 8) & 0xFF)) * 1024;/* max 4M byte */
		max_sg_entrys = (max_xfer_len/4096);
	}
	acb->host->max_sectors = max_xfer_len/512;
	acb->host->sg_tablesize = max_sg_entrys;
	roundup_ccbsize = roundup(sizeof(struct CommandControlBlock) + (max_sg_entrys - 1) * sizeof(struct SG64ENTRY), 32);
	acb->uncache_size = roundup_ccbsize * acb->maxFreeCCB;
	if (acb->adapter_type != ACB_ADAPTER_TYPE_F)
		acb->uncache_size += acb->ioqueue_size;
	dma_coherent = dma_alloc_coherent(&pdev->dev, acb->uncache_size, &dma_coherent_handle, GFP_KERNEL);
	if(!dma_coherent){
		printk(KERN_NOTICE "arcmsr%d: dma_alloc_coherent got error\n", acb->host->host_no);
		return -ENOMEM;
	}
	acb->dma_coherent = dma_coherent;
	acb->dma_coherent_handle = dma_coherent_handle;
	memset(dma_coherent, 0, acb->uncache_size);
	acb->ccbsize = roundup_ccbsize;
	ccb_tmp = dma_coherent;
	curr_phy_upper32 = upper_32_bits(dma_coherent_handle);
	acb->vir2phy_offset = (unsigned long)dma_coherent - (unsigned long)dma_coherent_handle;
	for(i = 0; i < acb->maxFreeCCB; i++){
		cdb_phyaddr = (unsigned long)dma_coherent_handle + offsetof(struct CommandControlBlock, arcmsr_cdb);
		switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A:
		case ACB_ADAPTER_TYPE_B:
			ccb_tmp->cdb_phyaddr = cdb_phyaddr >> 5;
			break;
		case ACB_ADAPTER_TYPE_C:
		case ACB_ADAPTER_TYPE_D:
		case ACB_ADAPTER_TYPE_E:
		case ACB_ADAPTER_TYPE_F:
			ccb_tmp->cdb_phyaddr = cdb_phyaddr;
			break;
		}
		acb->pccb_pool[i] = ccb_tmp;
		ccb_tmp->acb = acb;
		ccb_tmp->smid = (u32)i << 16;
		INIT_LIST_HEAD(&ccb_tmp->list);
		next_ccb_phy = dma_coherent_handle + roundup_ccbsize;
		if (upper_32_bits(next_ccb_phy) != curr_phy_upper32) {
			acb->maxFreeCCB = i;
			acb->host->can_queue = i;
			break;
		}
		else
			list_add_tail(&ccb_tmp->list, &acb->ccb_free_list);
		ccb_tmp = (struct CommandControlBlock *)((unsigned long)ccb_tmp + roundup_ccbsize);
		dma_coherent_handle = next_ccb_phy;
	}
	if (acb->adapter_type != ACB_ADAPTER_TYPE_F) {
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = ccb_tmp;
	}
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B:
		acb->pmuB = (struct MessageUnit_B *)acb->dma_coherent2;
		arcmsr_hbaB_assign_regAddr(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		acb->pmuD = (struct MessageUnit_D *)acb->dma_coherent2;
		arcmsr_hbaD_assign_regAddr(acb);
		break;
	case ACB_ADAPTER_TYPE_E:
		acb->pCompletionQ = acb->dma_coherent2;
		acb->completionQ_entry = acb->ioqueue_size / sizeof(struct deliver_completeQ);
		acb->doneq_index = 0;
		break;
	}	
	return 0;
}

static void arcmsr_message_isr_bh_fn(struct work_struct *work) 
{
	struct AdapterControlBlock *acb = container_of(work,
		struct AdapterControlBlock, arcmsr_do_message_isr_bh);
	char *acb_dev_map = (char *)acb->device_map;
	uint32_t __iomem *signature = NULL;
	char __iomem *devicemap = NULL;
	int target, lun;
	struct scsi_device *psdev;
	char diff, temp;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg  = acb->pmuA;

		signature = (uint32_t __iomem *)(&reg->message_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->message_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg  = acb->pmuB;

		signature = (uint32_t __iomem *)(&reg->message_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->message_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg  = acb->pmuC;

		signature = (uint32_t __iomem *)(&reg->msgcode_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->msgcode_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg  = acb->pmuD;

		signature = (uint32_t __iomem *)(&reg->msgcode_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->msgcode_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_E: {
		struct MessageUnit_E __iomem *reg  = acb->pmuE;

		signature = (uint32_t __iomem *)(&reg->msgcode_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->msgcode_rwbuffer[21]);
		break;
		}
	case ACB_ADAPTER_TYPE_F: {
		signature = (uint32_t __iomem *)(&acb->msgcode_rwbuffer[0]);
		devicemap = (char __iomem *)(&acb->msgcode_rwbuffer[21]);
		break;
		}
	}
	if (readl(signature) != ARCMSR_SIGNATURE_GET_CONFIG)
		return;
	for (target = 0; target < ARCMSR_MAX_TARGETID - 1;
		target++) {
		temp = readb(devicemap);
		diff = (*acb_dev_map) ^ temp;
		if (diff != 0) {
			*acb_dev_map = temp;
			for (lun = 0; lun < ARCMSR_MAX_TARGETLUN;
				lun++) {
				if ((diff & 0x01) == 1 &&
					(temp & 0x01) == 1) {
					scsi_add_device(acb->host,
						0, target, lun);
				} else if ((diff & 0x01) == 1
					&& (temp & 0x01) == 0) {
					psdev = scsi_device_lookup(acb->host,
						0, target, lun);
					if (psdev != NULL) {
						scsi_remove_device(psdev);
						scsi_device_put(psdev);
					}
				}
				temp >>= 1;
				diff >>= 1;
			}
		}
		devicemap++;
		acb_dev_map++;
	}
	acb->acb_flags &= ~ACB_F_MSG_GET_CONFIG;
}

static int
arcmsr_request_irq(struct pci_dev *pdev, struct AdapterControlBlock *acb)
{
	unsigned long flags;
	int nvec, i;

	if (msix_enable == 0)
		goto msi_int0;
	nvec = pci_alloc_irq_vectors(pdev, 1, ARCMST_NUM_MSIX_VECTORS,
			PCI_IRQ_MSIX);
	if (nvec > 0) {
		pr_info("arcmsr%d: msi-x enabled\n", acb->host->host_no);
		flags = 0;
	} else {
msi_int0:
		if (msi_enable == 1) {
			nvec = pci_alloc_irq_vectors(pdev, 1, 1, PCI_IRQ_MSI);
			if (nvec == 1) {
				dev_info(&pdev->dev, "msi enabled\n");
				goto msi_int1;
			}
		}
		nvec = pci_alloc_irq_vectors(pdev, 1, 1, PCI_IRQ_LEGACY);
		if (nvec < 1)
			return FAILED;
msi_int1:
		flags = IRQF_SHARED;
	}

	acb->vector_count = nvec;
	for (i = 0; i < nvec; i++) {
		if (request_irq(pci_irq_vector(pdev, i), arcmsr_do_interrupt,
				flags, "arcmsr", acb)) {
			pr_warn("arcmsr%d: request_irq =%d failed!\n",
				acb->host->host_no, pci_irq_vector(pdev, i));
			goto out_free_irq;
		}
	}

	return SUCCESS;
out_free_irq:
	while (--i >= 0)
		free_irq(pci_irq_vector(pdev, i), acb);
	pci_free_irq_vectors(pdev);
	return FAILED;
}

static void arcmsr_init_get_devmap_timer(struct AdapterControlBlock *pacb)
{
	INIT_WORK(&pacb->arcmsr_do_message_isr_bh, arcmsr_message_isr_bh_fn);
	pacb->fw_flag = FW_NORMAL;
	timer_setup(&pacb->eternal_timer, arcmsr_request_device_map, 0);
	pacb->eternal_timer.expires = jiffies + msecs_to_jiffies(6 * HZ);
	add_timer(&pacb->eternal_timer);
}

static void arcmsr_init_set_datetime_timer(struct AdapterControlBlock *pacb)
{
	timer_setup(&pacb->refresh_timer, arcmsr_set_iop_datetime, 0);
	pacb->refresh_timer.expires = jiffies + msecs_to_jiffies(60 * 1000);
	add_timer(&pacb->refresh_timer);
}

static int arcmsr_set_dma_mask(struct AdapterControlBlock *acb)
{
	struct pci_dev *pcidev = acb->pdev;

	if (IS_DMA64) {
		if (((acb->adapter_type == ACB_ADAPTER_TYPE_A) && !dma_mask_64) ||
		    dma_set_mask(&pcidev->dev, DMA_BIT_MASK(64)))
			goto	dma32;
		if (dma_set_coherent_mask(&pcidev->dev, DMA_BIT_MASK(64)) ||
		    dma_set_mask_and_coherent(&pcidev->dev, DMA_BIT_MASK(64))) {
			printk("arcmsr: set DMA 64 mask failed\n");
			return -ENXIO;
		}
	} else {
dma32:
		if (dma_set_mask(&pcidev->dev, DMA_BIT_MASK(32)) ||
		    dma_set_coherent_mask(&pcidev->dev, DMA_BIT_MASK(32)) ||
		    dma_set_mask_and_coherent(&pcidev->dev, DMA_BIT_MASK(32))) {
			printk("arcmsr: set DMA 32-bit mask failed\n");
			return -ENXIO;
		}
	}
	return 0;
}

static int arcmsr_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct Scsi_Host *host;
	struct AdapterControlBlock *acb;
	uint8_t bus,dev_fun;
	int error;
	error = pci_enable_device(pdev);
	if(error){
		return -ENODEV;
	}
	host = scsi_host_alloc(&arcmsr_scsi_host_template, sizeof(struct AdapterControlBlock));
	if(!host){
    		goto pci_disable_dev;
	}
	init_waitqueue_head(&wait_q);
	bus = pdev->bus->number;
	dev_fun = pdev->devfn;
	acb = (struct AdapterControlBlock *) host->hostdata;
	memset(acb,0,sizeof(struct AdapterControlBlock));
	acb->pdev = pdev;
	acb->adapter_type = id->driver_data;
	if (arcmsr_set_dma_mask(acb))
		goto scsi_host_release;
	acb->host = host;
	host->max_lun = ARCMSR_MAX_TARGETLUN;
	host->max_id = ARCMSR_MAX_TARGETID;		/*16:8*/
	host->max_cmd_len = 16;	 			/*this is issue of 64bit LBA ,over 2T byte*/
	if ((host_can_queue < ARCMSR_MIN_OUTSTANDING_CMD) || (host_can_queue > ARCMSR_MAX_OUTSTANDING_CMD))
		host_can_queue = ARCMSR_DEFAULT_OUTSTANDING_CMD;
	host->can_queue = host_can_queue;	/* max simultaneous cmds */
	if ((cmd_per_lun < ARCMSR_MIN_CMD_PERLUN) || (cmd_per_lun > ARCMSR_MAX_CMD_PERLUN))
		cmd_per_lun = ARCMSR_DEFAULT_CMD_PERLUN;
	host->cmd_per_lun = cmd_per_lun;
	host->this_id = ARCMSR_SCSI_INITIATOR_ID;
	host->unique_id = (bus << 8) | dev_fun;
	pci_set_drvdata(pdev, host);
	pci_set_master(pdev);
	error = pci_request_regions(pdev, "arcmsr");
	if(error){
		goto scsi_host_release;
	}
	spin_lock_init(&acb->eh_lock);
	spin_lock_init(&acb->ccblist_lock);
	spin_lock_init(&acb->postq_lock);
	spin_lock_init(&acb->doneq_lock);
	spin_lock_init(&acb->rqbuffer_lock);
	spin_lock_init(&acb->wqbuffer_lock);
	acb->acb_flags |= (ACB_F_MESSAGE_WQBUFFER_CLEARED |
			ACB_F_MESSAGE_RQBUFFER_CLEARED |
			ACB_F_MESSAGE_WQBUFFER_READED);
	acb->acb_flags &= ~ACB_F_SCSISTOPADAPTER;
	INIT_LIST_HEAD(&acb->ccb_free_list);
	error = arcmsr_remap_pciregion(acb);
	if(!error){
		goto pci_release_regs;
	}
	error = arcmsr_alloc_io_queue(acb);
	if (!error)
		goto unmap_pci_region;
	error = arcmsr_get_firmware_spec(acb);
	if(!error){
		goto free_hbb_mu;
	}
	if (acb->adapter_type != ACB_ADAPTER_TYPE_F)
		arcmsr_free_io_queue(acb);
	error = arcmsr_alloc_ccb_pool(acb);
	if(error){
		goto unmap_pci_region;
	}
	error = scsi_add_host(host, &pdev->dev);
	if(error){
		goto free_ccb_pool;
	}
	if (arcmsr_request_irq(pdev, acb) == FAILED)
		goto scsi_host_remove;
	arcmsr_iop_init(acb);
	arcmsr_init_get_devmap_timer(acb);
	if (set_date_time)
		arcmsr_init_set_datetime_timer(acb);
	if(arcmsr_alloc_sysfs_attr(acb))
		goto out_free_sysfs;
	scsi_scan_host(host);
	return 0;
out_free_sysfs:
	if (set_date_time)
		del_timer_sync(&acb->refresh_timer);
	del_timer_sync(&acb->eternal_timer);
	flush_work(&acb->arcmsr_do_message_isr_bh);
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
	arcmsr_free_irq(pdev, acb);
scsi_host_remove:
	scsi_remove_host(host);
free_ccb_pool:
	arcmsr_free_ccb_pool(acb);
	goto unmap_pci_region;
free_hbb_mu:
	arcmsr_free_io_queue(acb);
unmap_pci_region:
	arcmsr_unmap_pciregion(acb);
pci_release_regs:
	pci_release_regions(pdev);
scsi_host_release:
	scsi_host_put(host);
pci_disable_dev:
	pci_disable_device(pdev);
	return -ENODEV;
}

static void arcmsr_free_irq(struct pci_dev *pdev,
		struct AdapterControlBlock *acb)
{
	int i;

	for (i = 0; i < acb->vector_count; i++)
		free_irq(pci_irq_vector(pdev, i), acb);
	pci_free_irq_vectors(pdev);
}

static int __maybe_unused arcmsr_suspend(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)host->hostdata;

	arcmsr_disable_outbound_ints(acb);
	arcmsr_free_irq(pdev, acb);
	del_timer_sync(&acb->eternal_timer);
	if (set_date_time)
		del_timer_sync(&acb->refresh_timer);
	flush_work(&acb->arcmsr_do_message_isr_bh);
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
	return 0;
}

static int __maybe_unused arcmsr_resume(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)host->hostdata;

	if (arcmsr_set_dma_mask(acb))
		goto controller_unregister;
	if (arcmsr_request_irq(pdev, acb) == FAILED)
		goto controller_stop;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		uint32_t i;
		for (i = 0; i < ARCMSR_MAX_HBB_POSTQUEUE; i++) {
			reg->post_qbuffer[i] = 0;
			reg->done_qbuffer[i] = 0;
		}
		reg->postq_index = 0;
		reg->doneq_index = 0;
		break;
		}
	case ACB_ADAPTER_TYPE_E:
		writel(0, &acb->pmuE->host_int_status);
		writel(ARCMSR_HBEMU_DOORBELL_SYNC, &acb->pmuE->iobound_doorbell);
		acb->in_doorbell = 0;
		acb->out_doorbell = 0;
		acb->doneq_index = 0;
		break;
	case ACB_ADAPTER_TYPE_F:
		writel(0, &acb->pmuF->host_int_status);
		writel(ARCMSR_HBFMU_DOORBELL_SYNC, &acb->pmuF->iobound_doorbell);
		acb->in_doorbell = 0;
		acb->out_doorbell = 0;
		acb->doneq_index = 0;
		arcmsr_hbaF_assign_regAddr(acb);
		break;
	}
	arcmsr_iop_init(acb);
	arcmsr_init_get_devmap_timer(acb);
	if (set_date_time)
		arcmsr_init_set_datetime_timer(acb);
	return 0;
controller_stop:
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
controller_unregister:
	scsi_remove_host(host);
	arcmsr_free_ccb_pool(acb);
	if (acb->adapter_type == ACB_ADAPTER_TYPE_F)
		arcmsr_free_io_queue(acb);
	arcmsr_unmap_pciregion(acb);
	scsi_host_put(host);
	return -ENODEV;
}

static uint8_t arcmsr_hbaA_abort_allcmd(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, &reg->inbound_msgaddr0);
	if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'abort all outstanding command' timeout\n"
			, acb->host->host_no);
		return false;
	}
	return true;
}

static uint8_t arcmsr_hbaB_abort_allcmd(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;

	writel(ARCMSR_MESSAGE_ABORT_CMD, reg->drv2iop_doorbell);
	if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'abort all outstanding command' timeout\n"
			, acb->host->host_no);
		return false;
	}
	return true;
}
static uint8_t arcmsr_hbaC_abort_allcmd(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	if (!arcmsr_hbaC_wait_msgint_ready(pACB)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'abort all outstanding command' timeout\n"
			, pACB->host->host_no);
		return false;
	}
	return true;
}

static uint8_t arcmsr_hbaD_abort_allcmd(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *reg = pACB->pmuD;

	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, reg->inbound_msgaddr0);
	if (!arcmsr_hbaD_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait 'abort all outstanding "
			"command' timeout\n", pACB->host->host_no);
		return false;
	}
	return true;
}

static uint8_t arcmsr_hbaE_abort_allcmd(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_E __iomem *reg = pACB->pmuE;

	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, &reg->inbound_msgaddr0);
	pACB->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
	writel(pACB->out_doorbell, &reg->iobound_doorbell);
	if (!arcmsr_hbaE_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait 'abort all outstanding "
			"command' timeout\n", pACB->host->host_no);
		return false;
	}
	return true;
}

static uint8_t arcmsr_abort_allcmd(struct AdapterControlBlock *acb)
{
	uint8_t rtnval = 0;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		rtnval = arcmsr_hbaA_abort_allcmd(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		rtnval = arcmsr_hbaB_abort_allcmd(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		rtnval = arcmsr_hbaC_abort_allcmd(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		rtnval = arcmsr_hbaD_abort_allcmd(acb);
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		rtnval = arcmsr_hbaE_abort_allcmd(acb);
		break;
	}
	return rtnval;
}

static void arcmsr_pci_unmap_dma(struct CommandControlBlock *ccb)
{
	struct scsi_cmnd *pcmd = ccb->pcmd;

	scsi_dma_unmap(pcmd);
}

static void arcmsr_ccb_complete(struct CommandControlBlock *ccb)
{
	struct AdapterControlBlock *acb = ccb->acb;
	struct scsi_cmnd *pcmd = ccb->pcmd;
	unsigned long flags;
	atomic_dec(&acb->ccboutstandingcount);
	arcmsr_pci_unmap_dma(ccb);
	ccb->startdone = ARCMSR_CCB_DONE;
	spin_lock_irqsave(&acb->ccblist_lock, flags);
	list_add_tail(&ccb->list, &acb->ccb_free_list);
	spin_unlock_irqrestore(&acb->ccblist_lock, flags);
	pcmd->scsi_done(pcmd);
}

static void arcmsr_report_sense_info(struct CommandControlBlock *ccb)
{

	struct scsi_cmnd *pcmd = ccb->pcmd;
	struct SENSE_DATA *sensebuffer = (struct SENSE_DATA *)pcmd->sense_buffer;
	pcmd->result = (DID_OK << 16) | (CHECK_CONDITION << 1);
	if (sensebuffer) {
		int sense_data_length =
			sizeof(struct SENSE_DATA) < SCSI_SENSE_BUFFERSIZE
			? sizeof(struct SENSE_DATA) : SCSI_SENSE_BUFFERSIZE;
		memset(sensebuffer, 0, SCSI_SENSE_BUFFERSIZE);
		memcpy(sensebuffer, ccb->arcmsr_cdb.SenseData, sense_data_length);
		sensebuffer->ErrorCode = SCSI_SENSE_CURRENT_ERRORS;
		sensebuffer->Valid = 1;
		pcmd->result |= (DRIVER_SENSE << 24);
	}
}

static u32 arcmsr_disable_outbound_ints(struct AdapterControlBlock *acb)
{
	u32 orig_mask = 0;
	switch (acb->adapter_type) {	
	case ACB_ADAPTER_TYPE_A : {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		orig_mask = readl(&reg->outbound_intmask);
		writel(orig_mask|ARCMSR_MU_OUTBOUND_ALL_INTMASKENABLE, \
						&reg->outbound_intmask);
		}
		break;
	case ACB_ADAPTER_TYPE_B : {
		struct MessageUnit_B *reg = acb->pmuB;
		orig_mask = readl(reg->iop2drv_doorbell_mask);
		writel(0, reg->iop2drv_doorbell_mask);
		}
		break;
	case ACB_ADAPTER_TYPE_C:{
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		/* disable all outbound interrupt */
		orig_mask = readl(&reg->host_int_mask); /* disable outbound message0 int */
		writel(orig_mask|ARCMSR_HBCMU_ALL_INTMASKENABLE, &reg->host_int_mask);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		/* disable all outbound interrupt */
		writel(ARCMSR_ARC1214_ALL_INT_DISABLE, reg->pcief0_int_enable);
		}
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		orig_mask = readl(&reg->host_int_mask);
		writel(orig_mask | ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR | ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR, &reg->host_int_mask);
		readl(&reg->host_int_mask); /* Dummy readl to force pci flush */
		}
		break;
	}
	return orig_mask;
}

static void arcmsr_report_ccb_state(struct AdapterControlBlock *acb, 
			struct CommandControlBlock *ccb, bool error)
{
	uint8_t id, lun;
	id = ccb->pcmd->device->id;
	lun = ccb->pcmd->device->lun;
	if (!error) {
		if (acb->devstate[id][lun] == ARECA_RAID_GONE)
			acb->devstate[id][lun] = ARECA_RAID_GOOD;
		ccb->pcmd->result = DID_OK << 16;
		arcmsr_ccb_complete(ccb);
	}else{
		switch (ccb->arcmsr_cdb.DeviceStatus) {
		case ARCMSR_DEV_SELECT_TIMEOUT: {
			acb->devstate[id][lun] = ARECA_RAID_GONE;
			ccb->pcmd->result = DID_NO_CONNECT << 16;
			arcmsr_ccb_complete(ccb);
			}
			break;

		case ARCMSR_DEV_ABORTED:

		case ARCMSR_DEV_INIT_FAIL: {
			acb->devstate[id][lun] = ARECA_RAID_GONE;
			ccb->pcmd->result = DID_BAD_TARGET << 16;
			arcmsr_ccb_complete(ccb);
			}
			break;

		case ARCMSR_DEV_CHECK_CONDITION: {
			acb->devstate[id][lun] = ARECA_RAID_GOOD;
			arcmsr_report_sense_info(ccb);
			arcmsr_ccb_complete(ccb);
			}
			break;

		default:
			printk(KERN_NOTICE
				"arcmsr%d: scsi id = %d lun = %d isr get command error done, \
				but got unknown DeviceStatus = 0x%x \n"
				, acb->host->host_no
				, id
				, lun
				, ccb->arcmsr_cdb.DeviceStatus);
				acb->devstate[id][lun] = ARECA_RAID_GONE;
				ccb->pcmd->result = DID_NO_CONNECT << 16;
				arcmsr_ccb_complete(ccb);
			break;
		}
	}
}

static void arcmsr_drain_donequeue(struct AdapterControlBlock *acb, struct CommandControlBlock *pCCB, bool error)
{
	if ((pCCB->acb != acb) || (pCCB->startdone != ARCMSR_CCB_START)) {
		if (pCCB->startdone == ARCMSR_CCB_ABORTED) {
			struct scsi_cmnd *abortcmd = pCCB->pcmd;
			if (abortcmd) {
				abortcmd->result |= DID_ABORT << 16;
				arcmsr_ccb_complete(pCCB);
				printk(KERN_NOTICE "arcmsr%d: pCCB ='0x%p' isr got aborted command \n",
				acb->host->host_no, pCCB);
			}
			return;
		}
		printk(KERN_NOTICE "arcmsr%d: isr get an illegal ccb command \
				done acb = '0x%p'"
				"ccb = '0x%p' ccbacb = '0x%p' startdone = 0x%x"
				" ccboutstandingcount = %d \n"
				, acb->host->host_no
				, acb
				, pCCB
				, pCCB->acb
				, pCCB->startdone
				, atomic_read(&acb->ccboutstandingcount));
		return;
	}
	arcmsr_report_ccb_state(acb, pCCB, error);
}

static void arcmsr_done4abort_postqueue(struct AdapterControlBlock *acb)
{
	int i = 0;
	uint32_t flag_ccb;
	struct ARCMSR_CDB *pARCMSR_CDB;
	bool error;
	struct CommandControlBlock *pCCB;
	unsigned long ccb_cdb_phy;

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		uint32_t outbound_intstatus;
		outbound_intstatus = readl(&reg->outbound_intstatus) &
					acb->outbound_int_enable;
		/*clear and abort all outbound posted Q*/
		writel(outbound_intstatus, &reg->outbound_intstatus);/*clear interrupt*/
		while(((flag_ccb = readl(&reg->outbound_queueport)) != 0xFFFFFFFF)
				&& (i++ < acb->maxOutstanding)) {
			ccb_cdb_phy = (flag_ccb << 5) & 0xffffffff;
			if (acb->cdb_phyadd_hipart)
				ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
			pARCMSR_CDB = (struct ARCMSR_CDB *)(acb->vir2phy_offset + ccb_cdb_phy);
			pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
			error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
			arcmsr_drain_donequeue(acb, pCCB, error);
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*clear all outbound posted Q*/
		writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell); /* clear doorbell interrupt */
		for (i = 0; i < ARCMSR_MAX_HBB_POSTQUEUE; i++) {
			flag_ccb = reg->done_qbuffer[i];
			if (flag_ccb != 0) {
				reg->done_qbuffer[i] = 0;
				ccb_cdb_phy = (flag_ccb << 5) & 0xffffffff;
				if (acb->cdb_phyadd_hipart)
					ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
				pARCMSR_CDB = (struct ARCMSR_CDB *)(acb->vir2phy_offset + ccb_cdb_phy);
				pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
				error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
				arcmsr_drain_donequeue(acb, pCCB, error);
			}
			reg->post_qbuffer[i] = 0;
		}
		reg->doneq_index = 0;
		reg->postq_index = 0;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		while ((readl(&reg->host_int_status) & ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR) && (i++ < acb->maxOutstanding)) {
			/*need to do*/
			flag_ccb = readl(&reg->outbound_queueport_low);
			ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
			if (acb->cdb_phyadd_hipart)
				ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
			pARCMSR_CDB = (struct  ARCMSR_CDB *)(acb->vir2phy_offset + ccb_cdb_phy);
			pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
			error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ? true : false;
			arcmsr_drain_donequeue(acb, pCCB, error);
		}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D  *pmu = acb->pmuD;
		uint32_t outbound_write_pointer;
		uint32_t doneq_index, index_stripped, addressLow, residual, toggle;
		unsigned long flags;

		residual = atomic_read(&acb->ccboutstandingcount);
		for (i = 0; i < residual; i++) {
			spin_lock_irqsave(&acb->doneq_lock, flags);
			outbound_write_pointer =
				pmu->done_qbuffer[0].addressLow + 1;
			doneq_index = pmu->doneq_index;
			if ((doneq_index & 0xFFF) !=
				(outbound_write_pointer & 0xFFF)) {
				toggle = doneq_index & 0x4000;
				index_stripped = (doneq_index & 0xFFF) + 1;
				index_stripped %= ARCMSR_MAX_ARC1214_DONEQUEUE;
				pmu->doneq_index = index_stripped ? (index_stripped | toggle) :
					((toggle ^ 0x4000) + 1);
				doneq_index = pmu->doneq_index;
				spin_unlock_irqrestore(&acb->doneq_lock, flags);
				addressLow = pmu->done_qbuffer[doneq_index &
					0xFFF].addressLow;
				ccb_cdb_phy = (addressLow & 0xFFFFFFF0);
				if (acb->cdb_phyadd_hipart)
					ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
				pARCMSR_CDB = (struct  ARCMSR_CDB *)
					(acb->vir2phy_offset + ccb_cdb_phy);
				pCCB = container_of(pARCMSR_CDB,
					struct CommandControlBlock, arcmsr_cdb);
				error = (addressLow &
					ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ?
					true : false;
				arcmsr_drain_donequeue(acb, pCCB, error);
				writel(doneq_index,
					pmu->outboundlist_read_pointer);
			} else {
				spin_unlock_irqrestore(&acb->doneq_lock, flags);
				mdelay(10);
			}
		}
		pmu->postq_index = 0;
		pmu->doneq_index = 0x40FF;
		}
		break;
	case ACB_ADAPTER_TYPE_E:
		arcmsr_hbaE_postqueue_isr(acb);
		break;
	case ACB_ADAPTER_TYPE_F:
		arcmsr_hbaF_postqueue_isr(acb);
		break;
	}
}

static void arcmsr_remove_scsi_devices(struct AdapterControlBlock *acb)
{
	char *acb_dev_map = (char *)acb->device_map;
	int target, lun, i;
	struct scsi_device *psdev;
	struct CommandControlBlock *ccb;
	char temp;

	for (i = 0; i < acb->maxFreeCCB; i++) {
		ccb = acb->pccb_pool[i];
		if (ccb->startdone == ARCMSR_CCB_START) {
			ccb->pcmd->result = DID_NO_CONNECT << 16;
			arcmsr_pci_unmap_dma(ccb);
			ccb->pcmd->scsi_done(ccb->pcmd);
		}
	}
	for (target = 0; target < ARCMSR_MAX_TARGETID; target++) {
		temp = *acb_dev_map;
		if (temp) {
			for (lun = 0; lun < ARCMSR_MAX_TARGETLUN; lun++) {
				if (temp & 1) {
					psdev = scsi_device_lookup(acb->host,
						0, target, lun);
					if (psdev != NULL) {
						scsi_remove_device(psdev);
						scsi_device_put(psdev);
					}
				}
				temp >>= 1;
			}
			*acb_dev_map = 0;
		}
		acb_dev_map++;
	}
}

static void arcmsr_free_pcidev(struct AdapterControlBlock *acb)
{
	struct pci_dev *pdev;
	struct Scsi_Host *host;

	host = acb->host;
	arcmsr_free_sysfs_attr(acb);
	scsi_remove_host(host);
	flush_work(&acb->arcmsr_do_message_isr_bh);
	del_timer_sync(&acb->eternal_timer);
	if (set_date_time)
		del_timer_sync(&acb->refresh_timer);
	pdev = acb->pdev;
	arcmsr_free_irq(pdev, acb);
	arcmsr_free_ccb_pool(acb);
	if (acb->adapter_type == ACB_ADAPTER_TYPE_F)
		arcmsr_free_io_queue(acb);
	arcmsr_unmap_pciregion(acb);
	pci_release_regions(pdev);
	scsi_host_put(host);
	pci_disable_device(pdev);
}

static void arcmsr_remove(struct pci_dev *pdev)
{
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *) host->hostdata;
	int poll_count = 0;
	uint16_t dev_id;

	pci_read_config_word(pdev, PCI_DEVICE_ID, &dev_id);
	if (dev_id == 0xffff) {
		acb->acb_flags &= ~ACB_F_IOP_INITED;
		acb->acb_flags |= ACB_F_ADAPTER_REMOVED;
		arcmsr_remove_scsi_devices(acb);
		arcmsr_free_pcidev(acb);
		return;
	}
	arcmsr_free_sysfs_attr(acb);
	scsi_remove_host(host);
	flush_work(&acb->arcmsr_do_message_isr_bh);
	del_timer_sync(&acb->eternal_timer);
	if (set_date_time)
		del_timer_sync(&acb->refresh_timer);
	arcmsr_disable_outbound_ints(acb);
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);	
	acb->acb_flags |= ACB_F_SCSISTOPADAPTER;
	acb->acb_flags &= ~ACB_F_IOP_INITED;

	for (poll_count = 0; poll_count < acb->maxOutstanding; poll_count++){
		if (!atomic_read(&acb->ccboutstandingcount))
			break;
		arcmsr_interrupt(acb);/* FIXME: need spinlock */
		msleep(25);
	}

	if (atomic_read(&acb->ccboutstandingcount)) {
		int i;

		arcmsr_abort_allcmd(acb);
		arcmsr_done4abort_postqueue(acb);
		for (i = 0; i < acb->maxFreeCCB; i++) {
			struct CommandControlBlock *ccb = acb->pccb_pool[i];
			if (ccb->startdone == ARCMSR_CCB_START) {
				ccb->startdone = ARCMSR_CCB_ABORTED;
				ccb->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(ccb);
			}
		}
	}
	arcmsr_free_irq(pdev, acb);
	arcmsr_free_ccb_pool(acb);
	if (acb->adapter_type == ACB_ADAPTER_TYPE_F)
		arcmsr_free_io_queue(acb);
	arcmsr_unmap_pciregion(acb);
	pci_release_regions(pdev);
	scsi_host_put(host);
	pci_disable_device(pdev);
}

static void arcmsr_shutdown(struct pci_dev *pdev)
{
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)host->hostdata;
	if (acb->acb_flags & ACB_F_ADAPTER_REMOVED)
		return;
	del_timer_sync(&acb->eternal_timer);
	if (set_date_time)
		del_timer_sync(&acb->refresh_timer);
	arcmsr_disable_outbound_ints(acb);
	arcmsr_free_irq(pdev, acb);
	flush_work(&acb->arcmsr_do_message_isr_bh);
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
}

static int arcmsr_module_init(void)
{
	int error = 0;
	error = pci_register_driver(&arcmsr_pci_driver);
	return error;
}

static void arcmsr_module_exit(void)
{
	pci_unregister_driver(&arcmsr_pci_driver);
}
module_init(arcmsr_module_init);
module_exit(arcmsr_module_exit);

static void arcmsr_enable_outbound_ints(struct AdapterControlBlock *acb,
						u32 intmask_org)
{
	u32 mask;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		mask = intmask_org & ~(ARCMSR_MU_OUTBOUND_POSTQUEUE_INTMASKENABLE |
			     ARCMSR_MU_OUTBOUND_DOORBELL_INTMASKENABLE|
			     ARCMSR_MU_OUTBOUND_MESSAGE0_INTMASKENABLE);
		writel(mask, &reg->outbound_intmask);
		acb->outbound_int_enable = ~(intmask_org & mask) & 0x000000ff;
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		mask = intmask_org | (ARCMSR_IOP2DRV_DATA_WRITE_OK |
			ARCMSR_IOP2DRV_DATA_READ_OK |
			ARCMSR_IOP2DRV_CDB_DONE |
			ARCMSR_IOP2DRV_MESSAGE_CMD_DONE);
		writel(mask, reg->iop2drv_doorbell_mask);
		acb->outbound_int_enable = (intmask_org | mask) & 0x0000000f;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		mask = ~(ARCMSR_HBCMU_UTILITY_A_ISR_MASK | ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR_MASK|ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR_MASK);
		writel(intmask_org & mask, &reg->host_int_mask);
		acb->outbound_int_enable = ~(intmask_org & mask) & 0x0000000f;
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;

		mask = ARCMSR_ARC1214_ALL_INT_ENABLE;
		writel(intmask_org | mask, reg->pcief0_int_enable);
		break;
		}
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;

		mask = ~(ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR | ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR);
		writel(intmask_org & mask, &reg->host_int_mask);
		break;
		}
	}
}

static int arcmsr_build_ccb(struct AdapterControlBlock *acb,
	struct CommandControlBlock *ccb, struct scsi_cmnd *pcmd)
{
	struct ARCMSR_CDB *arcmsr_cdb = (struct ARCMSR_CDB *)&ccb->arcmsr_cdb;
	int8_t *psge = (int8_t *)&arcmsr_cdb->u;
	__le32 address_lo, address_hi;
	int arccdbsize = 0x30;
	__le32 length = 0;
	int i;
	struct scatterlist *sg;
	int nseg;
	ccb->pcmd = pcmd;
	memset(arcmsr_cdb, 0, sizeof(struct ARCMSR_CDB));
	arcmsr_cdb->TargetID = pcmd->device->id;
	arcmsr_cdb->LUN = pcmd->device->lun;
	arcmsr_cdb->Function = 1;
	arcmsr_cdb->msgContext = 0;
	memcpy(arcmsr_cdb->Cdb, pcmd->cmnd, pcmd->cmd_len);

	nseg = scsi_dma_map(pcmd);
	if (unlikely(nseg > acb->host->sg_tablesize || nseg < 0))
		return FAILED;
	scsi_for_each_sg(pcmd, sg, nseg, i) {
		/* Get the physical address of the current data pointer */
		length = cpu_to_le32(sg_dma_len(sg));
		address_lo = cpu_to_le32(dma_addr_lo32(sg_dma_address(sg)));
		address_hi = cpu_to_le32(dma_addr_hi32(sg_dma_address(sg)));
		if (address_hi == 0) {
			struct SG32ENTRY *pdma_sg = (struct SG32ENTRY *)psge;

			pdma_sg->address = address_lo;
			pdma_sg->length = length;
			psge += sizeof (struct SG32ENTRY);
			arccdbsize += sizeof (struct SG32ENTRY);
		} else {
			struct SG64ENTRY *pdma_sg = (struct SG64ENTRY *)psge;

			pdma_sg->addresshigh = address_hi;
			pdma_sg->address = address_lo;
			pdma_sg->length = length|cpu_to_le32(IS_SG64_ADDR);
			psge += sizeof (struct SG64ENTRY);
			arccdbsize += sizeof (struct SG64ENTRY);
		}
	}
	arcmsr_cdb->sgcount = (uint8_t)nseg;
	arcmsr_cdb->DataLength = scsi_bufflen(pcmd);
	arcmsr_cdb->msgPages = arccdbsize/0x100 + (arccdbsize % 0x100 ? 1 : 0);
	if ( arccdbsize > 256)
		arcmsr_cdb->Flags |= ARCMSR_CDB_FLAG_SGL_BSIZE;
	if (pcmd->sc_data_direction == DMA_TO_DEVICE)
		arcmsr_cdb->Flags |= ARCMSR_CDB_FLAG_WRITE;
	ccb->arc_cdb_size = arccdbsize;
	return SUCCESS;
}

static void arcmsr_post_ccb(struct AdapterControlBlock *acb, struct CommandControlBlock *ccb)
{
	uint32_t cdb_phyaddr = ccb->cdb_phyaddr;
	struct ARCMSR_CDB *arcmsr_cdb = (struct ARCMSR_CDB *)&ccb->arcmsr_cdb;
	atomic_inc(&acb->ccboutstandingcount);
	ccb->startdone = ARCMSR_CCB_START;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;

		if (arcmsr_cdb->Flags & ARCMSR_CDB_FLAG_SGL_BSIZE)
			writel(cdb_phyaddr | ARCMSR_CCBPOST_FLAG_SGL_BSIZE,
			&reg->inbound_queueport);
		else
			writel(cdb_phyaddr, &reg->inbound_queueport);
		break;
	}

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		uint32_t ending_index, index = reg->postq_index;

		ending_index = ((index + 1) % ARCMSR_MAX_HBB_POSTQUEUE);
		reg->post_qbuffer[ending_index] = 0;
		if (arcmsr_cdb->Flags & ARCMSR_CDB_FLAG_SGL_BSIZE) {
			reg->post_qbuffer[index] =
				cdb_phyaddr | ARCMSR_CCBPOST_FLAG_SGL_BSIZE;
		} else {
			reg->post_qbuffer[index] = cdb_phyaddr;
		}
		index++;
		index %= ARCMSR_MAX_HBB_POSTQUEUE;/*if last index number set it to 0 */
		reg->postq_index = index;
		writel(ARCMSR_DRV2IOP_CDB_POSTED, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *phbcmu = acb->pmuC;
		uint32_t ccb_post_stamp, arc_cdb_size;

		arc_cdb_size = (ccb->arc_cdb_size > 0x300) ? 0x300 : ccb->arc_cdb_size;
		ccb_post_stamp = (cdb_phyaddr | ((arc_cdb_size - 1) >> 6) | 1);
		writel(upper_32_bits(ccb->cdb_phyaddr), &phbcmu->inbound_queueport_high);
		writel(ccb_post_stamp, &phbcmu->inbound_queueport_low);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D  *pmu = acb->pmuD;
		u16 index_stripped;
		u16 postq_index, toggle;
		unsigned long flags;
		struct InBound_SRB *pinbound_srb;

		spin_lock_irqsave(&acb->postq_lock, flags);
		postq_index = pmu->postq_index;
		pinbound_srb = (struct InBound_SRB *)&(pmu->post_qbuffer[postq_index & 0xFF]);
		pinbound_srb->addressHigh = upper_32_bits(ccb->cdb_phyaddr);
		pinbound_srb->addressLow = cdb_phyaddr;
		pinbound_srb->length = ccb->arc_cdb_size >> 2;
		arcmsr_cdb->msgContext = dma_addr_lo32(cdb_phyaddr);
		toggle = postq_index & 0x4000;
		index_stripped = postq_index + 1;
		index_stripped &= (ARCMSR_MAX_ARC1214_POSTQUEUE - 1);
		pmu->postq_index = index_stripped ? (index_stripped | toggle) :
			(toggle ^ 0x4000);
		writel(postq_index, pmu->inboundlist_write_pointer);
		spin_unlock_irqrestore(&acb->postq_lock, flags);
		break;
		}
	case ACB_ADAPTER_TYPE_E: {
		struct MessageUnit_E __iomem *pmu = acb->pmuE;
		u32 ccb_post_stamp, arc_cdb_size;

		arc_cdb_size = (ccb->arc_cdb_size > 0x300) ? 0x300 : ccb->arc_cdb_size;
		ccb_post_stamp = (ccb->smid | ((arc_cdb_size - 1) >> 6));
		writel(0, &pmu->inbound_queueport_high);
		writel(ccb_post_stamp, &pmu->inbound_queueport_low);
		break;
		}
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_F __iomem *pmu = acb->pmuF;
		u32 ccb_post_stamp, arc_cdb_size;

		if (ccb->arc_cdb_size <= 0x300)
			arc_cdb_size = (ccb->arc_cdb_size - 1) >> 6 | 1;
		else
			arc_cdb_size = (((ccb->arc_cdb_size + 0xff) >> 8) + 2) << 1 | 1;
		ccb_post_stamp = (ccb->smid | arc_cdb_size);
		writel(0, &pmu->inbound_queueport_high);
		writel(ccb_post_stamp, &pmu->inbound_queueport_low);
		break;
		}
	}
}

static void arcmsr_hbaA_stop_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, &reg->inbound_msgaddr0);
	if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'stop adapter background rebuild' timeout\n"
			, acb->host->host_no);
	}
}

static void arcmsr_hbaB_stop_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_MESSAGE_STOP_BGRB, reg->drv2iop_doorbell);

	if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'stop adapter background rebuild' timeout\n"
			, acb->host->host_no);
	}
}

static void arcmsr_hbaC_stop_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	pACB->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	if (!arcmsr_hbaC_wait_msgint_ready(pACB)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'stop adapter background rebuild' timeout\n"
			, pACB->host->host_no);
	}
	return;
}

static void arcmsr_hbaD_stop_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *reg = pACB->pmuD;

	pACB->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, reg->inbound_msgaddr0);
	if (!arcmsr_hbaD_wait_msgint_ready(pACB))
		pr_notice("arcmsr%d: wait 'stop adapter background rebuild' "
			"timeout\n", pACB->host->host_no);
}

static void arcmsr_hbaE_stop_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_E __iomem *reg = pACB->pmuE;

	pACB->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, &reg->inbound_msgaddr0);
	pACB->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
	writel(pACB->out_doorbell, &reg->iobound_doorbell);
	if (!arcmsr_hbaE_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait 'stop adapter background rebuild' "
			"timeout\n", pACB->host->host_no);
	}
}

static void arcmsr_stop_adapter_bgrb(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		arcmsr_hbaA_stop_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		arcmsr_hbaB_stop_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		arcmsr_hbaC_stop_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		arcmsr_hbaD_stop_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		arcmsr_hbaE_stop_bgrb(acb);
		break;
	}
}

static void arcmsr_free_ccb_pool(struct AdapterControlBlock *acb)
{
	dma_free_coherent(&acb->pdev->dev, acb->uncache_size, acb->dma_coherent, acb->dma_coherent_handle);
}

static void arcmsr_iop_message_read(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		writel(ARCMSR_DRV2IOP_DATA_READ_OK, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;

		writel(ARCMSR_HBCMU_DRV2IOP_DATA_READ_OK, &reg->inbound_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		writel(ARCMSR_ARC1214_DRV2IOP_DATA_OUT_READ,
			reg->inbound_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_DATA_READ_OK;
		writel(acb->out_doorbell, &reg->iobound_doorbell);
		}
		break;
	}
}

static void arcmsr_iop_message_wrote(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_INBOUND_DRIVER_DATA_WRITE_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_DRV2IOP_DATA_WRITE_OK, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_HBCMU_DRV2IOP_DATA_WRITE_OK, &reg->inbound_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		writel(ARCMSR_ARC1214_DRV2IOP_DATA_IN_READY,
			reg->inbound_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_DATA_WRITE_OK;
		writel(acb->out_doorbell, &reg->iobound_doorbell);
		}
		break;
	}
}

struct QBUFFER __iomem *arcmsr_get_iop_rqbuffer(struct AdapterControlBlock *acb)
{
	struct QBUFFER __iomem *qbuffer = NULL;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		qbuffer = (struct QBUFFER __iomem *)&reg->message_rbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		qbuffer = (struct QBUFFER __iomem *)reg->message_rbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *phbcmu = acb->pmuC;
		qbuffer = (struct QBUFFER __iomem *)&phbcmu->message_rbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		qbuffer = (struct QBUFFER __iomem *)reg->message_rbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_E: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		qbuffer = (struct QBUFFER __iomem *)&reg->message_rbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_F: {
		qbuffer = (struct QBUFFER __iomem *)acb->message_rbuffer;
		}
		break;
	}
	return qbuffer;
}

static struct QBUFFER __iomem *arcmsr_get_iop_wqbuffer(struct AdapterControlBlock *acb)
{
	struct QBUFFER __iomem *pqbuffer = NULL;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		pqbuffer = (struct QBUFFER __iomem *) &reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B  *reg = acb->pmuB;
		pqbuffer = (struct QBUFFER __iomem *)reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		pqbuffer = (struct QBUFFER __iomem *)&reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		pqbuffer = (struct QBUFFER __iomem *)reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_E: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		pqbuffer = (struct QBUFFER __iomem *)&reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_F:
		pqbuffer = (struct QBUFFER __iomem *)acb->message_wbuffer;
		break;
	}
	return pqbuffer;
}

static uint32_t
arcmsr_Read_iop_rqbuffer_in_DWORD(struct AdapterControlBlock *acb,
		struct QBUFFER __iomem *prbuffer)
{
	uint8_t *pQbuffer;
	uint8_t *buf1 = NULL;
	uint32_t __iomem *iop_data;
	uint32_t iop_len, data_len, *buf2 = NULL;

	iop_data = (uint32_t __iomem *)prbuffer->data;
	iop_len = readl(&prbuffer->data_len);
	if (iop_len > 0) {
		buf1 = kmalloc(128, GFP_ATOMIC);
		buf2 = (uint32_t *)buf1;
		if (buf1 == NULL)
			return 0;
		data_len = iop_len;
		while (data_len >= 4) {
			*buf2++ = readl(iop_data);
			iop_data++;
			data_len -= 4;
		}
		if (data_len)
			*buf2 = readl(iop_data);
		buf2 = (uint32_t *)buf1;
	}
	while (iop_len > 0) {
		pQbuffer = &acb->rqbuffer[acb->rqbuf_putIndex];
		*pQbuffer = *buf1;
		acb->rqbuf_putIndex++;
		/* if last, index number set it to 0 */
		acb->rqbuf_putIndex %= ARCMSR_MAX_QBUFFER;
		buf1++;
		iop_len--;
	}
	kfree(buf2);
	/* let IOP know data has been read */
	arcmsr_iop_message_read(acb);
	return 1;
}

uint32_t
arcmsr_Read_iop_rqbuffer_data(struct AdapterControlBlock *acb,
	struct QBUFFER __iomem *prbuffer) {

	uint8_t *pQbuffer;
	uint8_t __iomem *iop_data;
	uint32_t iop_len;

	if (acb->adapter_type > ACB_ADAPTER_TYPE_B)
		return arcmsr_Read_iop_rqbuffer_in_DWORD(acb, prbuffer);
	iop_data = (uint8_t __iomem *)prbuffer->data;
	iop_len = readl(&prbuffer->data_len);
	while (iop_len > 0) {
		pQbuffer = &acb->rqbuffer[acb->rqbuf_putIndex];
		*pQbuffer = readb(iop_data);
		acb->rqbuf_putIndex++;
		acb->rqbuf_putIndex %= ARCMSR_MAX_QBUFFER;
		iop_data++;
		iop_len--;
	}
	arcmsr_iop_message_read(acb);
	return 1;
}

static void arcmsr_iop2drv_data_wrote_handle(struct AdapterControlBlock *acb)
{
	unsigned long flags;
	struct QBUFFER __iomem  *prbuffer;
	int32_t buf_empty_len;

	spin_lock_irqsave(&acb->rqbuffer_lock, flags);
	prbuffer = arcmsr_get_iop_rqbuffer(acb);
	buf_empty_len = (acb->rqbuf_putIndex - acb->rqbuf_getIndex - 1) &
		(ARCMSR_MAX_QBUFFER - 1);
	if (buf_empty_len >= readl(&prbuffer->data_len)) {
		if (arcmsr_Read_iop_rqbuffer_data(acb, prbuffer) == 0)
			acb->acb_flags |= ACB_F_IOPDATA_OVERFLOW;
	} else
		acb->acb_flags |= ACB_F_IOPDATA_OVERFLOW;
	spin_unlock_irqrestore(&acb->rqbuffer_lock, flags);
}

static void arcmsr_write_ioctldata2iop_in_DWORD(struct AdapterControlBlock *acb)
{
	uint8_t *pQbuffer;
	struct QBUFFER __iomem *pwbuffer;
	uint8_t *buf1 = NULL;
	uint32_t __iomem *iop_data;
	uint32_t allxfer_len = 0, data_len, *buf2 = NULL, data;

	if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_READED) {
		buf1 = kmalloc(128, GFP_ATOMIC);
		buf2 = (uint32_t *)buf1;
		if (buf1 == NULL)
			return;

		acb->acb_flags &= (~ACB_F_MESSAGE_WQBUFFER_READED);
		pwbuffer = arcmsr_get_iop_wqbuffer(acb);
		iop_data = (uint32_t __iomem *)pwbuffer->data;
		while ((acb->wqbuf_getIndex != acb->wqbuf_putIndex)
			&& (allxfer_len < 124)) {
			pQbuffer = &acb->wqbuffer[acb->wqbuf_getIndex];
			*buf1 = *pQbuffer;
			acb->wqbuf_getIndex++;
			acb->wqbuf_getIndex %= ARCMSR_MAX_QBUFFER;
			buf1++;
			allxfer_len++;
		}
		data_len = allxfer_len;
		buf1 = (uint8_t *)buf2;
		while (data_len >= 4) {
			data = *buf2++;
			writel(data, iop_data);
			iop_data++;
			data_len -= 4;
		}
		if (data_len) {
			data = *buf2;
			writel(data, iop_data);
		}
		writel(allxfer_len, &pwbuffer->data_len);
		kfree(buf1);
		arcmsr_iop_message_wrote(acb);
	}
}

void
arcmsr_write_ioctldata2iop(struct AdapterControlBlock *acb)
{
	uint8_t *pQbuffer;
	struct QBUFFER __iomem *pwbuffer;
	uint8_t __iomem *iop_data;
	int32_t allxfer_len = 0;

	if (acb->adapter_type > ACB_ADAPTER_TYPE_B) {
		arcmsr_write_ioctldata2iop_in_DWORD(acb);
		return;
	}
	if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_READED) {
		acb->acb_flags &= (~ACB_F_MESSAGE_WQBUFFER_READED);
		pwbuffer = arcmsr_get_iop_wqbuffer(acb);
		iop_data = (uint8_t __iomem *)pwbuffer->data;
		while ((acb->wqbuf_getIndex != acb->wqbuf_putIndex)
			&& (allxfer_len < 124)) {
			pQbuffer = &acb->wqbuffer[acb->wqbuf_getIndex];
			writeb(*pQbuffer, iop_data);
			acb->wqbuf_getIndex++;
			acb->wqbuf_getIndex %= ARCMSR_MAX_QBUFFER;
			iop_data++;
			allxfer_len++;
		}
		writel(allxfer_len, &pwbuffer->data_len);
		arcmsr_iop_message_wrote(acb);
	}
}

static void arcmsr_iop2drv_data_read_handle(struct AdapterControlBlock *acb)
{
	unsigned long flags;

	spin_lock_irqsave(&acb->wqbuffer_lock, flags);
	acb->acb_flags |= ACB_F_MESSAGE_WQBUFFER_READED;
	if (acb->wqbuf_getIndex != acb->wqbuf_putIndex)
		arcmsr_write_ioctldata2iop(acb);
	if (acb->wqbuf_getIndex == acb->wqbuf_putIndex)
		acb->acb_flags |= ACB_F_MESSAGE_WQBUFFER_CLEARED;
	spin_unlock_irqrestore(&acb->wqbuffer_lock, flags);
}

static void arcmsr_hbaA_doorbell_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	outbound_doorbell = readl(&reg->outbound_doorbell);
	do {
		writel(outbound_doorbell, &reg->outbound_doorbell);
		if (outbound_doorbell & ARCMSR_OUTBOUND_IOP331_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(acb);
		if (outbound_doorbell & ARCMSR_OUTBOUND_IOP331_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(acb);
		outbound_doorbell = readl(&reg->outbound_doorbell);
	} while (outbound_doorbell & (ARCMSR_OUTBOUND_IOP331_DATA_WRITE_OK
		| ARCMSR_OUTBOUND_IOP331_DATA_READ_OK));
}
static void arcmsr_hbaC_doorbell_isr(struct AdapterControlBlock *pACB)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	/*
	*******************************************************************
	**  Maybe here we need to check wrqbuffer_lock is lock or not
	**  DOORBELL: din! don!
	**  check if there are any mail need to pack from firmware
	*******************************************************************
	*/
	outbound_doorbell = readl(&reg->outbound_doorbell);
	do {
		writel(outbound_doorbell, &reg->outbound_doorbell_clear);
		readl(&reg->outbound_doorbell_clear);
		if (outbound_doorbell & ARCMSR_HBCMU_IOP2DRV_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(pACB);
		if (outbound_doorbell & ARCMSR_HBCMU_IOP2DRV_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(pACB);
		if (outbound_doorbell & ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE)
			arcmsr_hbaC_message_isr(pACB);
		outbound_doorbell = readl(&reg->outbound_doorbell);
	} while (outbound_doorbell & (ARCMSR_HBCMU_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_HBCMU_IOP2DRV_DATA_READ_OK
		| ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE));
}

static void arcmsr_hbaD_doorbell_isr(struct AdapterControlBlock *pACB)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_D  *pmu = pACB->pmuD;

	outbound_doorbell = readl(pmu->outbound_doorbell);
	do {
		writel(outbound_doorbell, pmu->outbound_doorbell);
		if (outbound_doorbell & ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE)
			arcmsr_hbaD_message_isr(pACB);
		if (outbound_doorbell & ARCMSR_ARC1214_IOP2DRV_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(pACB);
		if (outbound_doorbell & ARCMSR_ARC1214_IOP2DRV_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(pACB);
		outbound_doorbell = readl(pmu->outbound_doorbell);
	} while (outbound_doorbell & (ARCMSR_ARC1214_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_ARC1214_IOP2DRV_DATA_READ_OK
		| ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE));
}

static void arcmsr_hbaE_doorbell_isr(struct AdapterControlBlock *pACB)
{
	uint32_t outbound_doorbell, in_doorbell, tmp;
	struct MessageUnit_E __iomem *reg = pACB->pmuE;

	in_doorbell = readl(&reg->iobound_doorbell);
	outbound_doorbell = in_doorbell ^ pACB->in_doorbell;
	do {
		writel(0, &reg->host_int_status); /* clear interrupt */
		if (outbound_doorbell & ARCMSR_HBEMU_IOP2DRV_DATA_WRITE_OK) {
			arcmsr_iop2drv_data_wrote_handle(pACB);
		}
		if (outbound_doorbell & ARCMSR_HBEMU_IOP2DRV_DATA_READ_OK) {
			arcmsr_iop2drv_data_read_handle(pACB);
		}
		if (outbound_doorbell & ARCMSR_HBEMU_IOP2DRV_MESSAGE_CMD_DONE) {
			arcmsr_hbaE_message_isr(pACB);
		}
		tmp = in_doorbell;
		in_doorbell = readl(&reg->iobound_doorbell);
		outbound_doorbell = tmp ^ in_doorbell;
	} while (outbound_doorbell & (ARCMSR_HBEMU_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_HBEMU_IOP2DRV_DATA_READ_OK
		| ARCMSR_HBEMU_IOP2DRV_MESSAGE_CMD_DONE));
	pACB->in_doorbell = in_doorbell;
}

static void arcmsr_hbaA_postqueue_isr(struct AdapterControlBlock *acb)
{
	uint32_t flag_ccb;
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	struct ARCMSR_CDB *pARCMSR_CDB;
	struct CommandControlBlock *pCCB;
	bool error;
	unsigned long cdb_phy_addr;

	while ((flag_ccb = readl(&reg->outbound_queueport)) != 0xFFFFFFFF) {
		cdb_phy_addr = (flag_ccb << 5) & 0xffffffff;
		if (acb->cdb_phyadd_hipart)
			cdb_phy_addr = cdb_phy_addr | acb->cdb_phyadd_hipart;
		pARCMSR_CDB = (struct ARCMSR_CDB *)(acb->vir2phy_offset + cdb_phy_addr);
		pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
		arcmsr_drain_donequeue(acb, pCCB, error);
	}
}
static void arcmsr_hbaB_postqueue_isr(struct AdapterControlBlock *acb)
{
	uint32_t index;
	uint32_t flag_ccb;
	struct MessageUnit_B *reg = acb->pmuB;
	struct ARCMSR_CDB *pARCMSR_CDB;
	struct CommandControlBlock *pCCB;
	bool error;
	unsigned long cdb_phy_addr;

	index = reg->doneq_index;
	while ((flag_ccb = reg->done_qbuffer[index]) != 0) {
		cdb_phy_addr = (flag_ccb << 5) & 0xffffffff;
		if (acb->cdb_phyadd_hipart)
			cdb_phy_addr = cdb_phy_addr | acb->cdb_phyadd_hipart;
		pARCMSR_CDB = (struct ARCMSR_CDB *)(acb->vir2phy_offset + cdb_phy_addr);
		pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
		arcmsr_drain_donequeue(acb, pCCB, error);
		reg->done_qbuffer[index] = 0;
		index++;
		index %= ARCMSR_MAX_HBB_POSTQUEUE;
		reg->doneq_index = index;
	}
}

static void arcmsr_hbaC_postqueue_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_C __iomem *phbcmu;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *ccb;
	uint32_t flag_ccb, throttling = 0;
	unsigned long ccb_cdb_phy;
	int error;

	phbcmu = acb->pmuC;
	/* areca cdb command done */
	/* Use correct offset and size for syncing */

	while ((flag_ccb = readl(&phbcmu->outbound_queueport_low)) !=
			0xFFFFFFFF) {
		ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
		if (acb->cdb_phyadd_hipart)
			ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset
			+ ccb_cdb_phy);
		ccb = container_of(arcmsr_cdb, struct CommandControlBlock,
			arcmsr_cdb);
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1)
			? true : false;
		/* check if command done with no error */
		arcmsr_drain_donequeue(acb, ccb, error);
		throttling++;
		if (throttling == ARCMSR_HBC_ISR_THROTTLING_LEVEL) {
			writel(ARCMSR_HBCMU_DRV2IOP_POSTQUEUE_THROTTLING,
				&phbcmu->inbound_doorbell);
			throttling = 0;
		}
	}
}

static void arcmsr_hbaD_postqueue_isr(struct AdapterControlBlock *acb)
{
	u32 outbound_write_pointer, doneq_index, index_stripped, toggle;
	uint32_t addressLow;
	int error;
	struct MessageUnit_D  *pmu;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *ccb;
	unsigned long flags, ccb_cdb_phy;

	spin_lock_irqsave(&acb->doneq_lock, flags);
	pmu = acb->pmuD;
	outbound_write_pointer = pmu->done_qbuffer[0].addressLow + 1;
	doneq_index = pmu->doneq_index;
	if ((doneq_index & 0xFFF) != (outbound_write_pointer & 0xFFF)) {
		do {
			toggle = doneq_index & 0x4000;
			index_stripped = (doneq_index & 0xFFF) + 1;
			index_stripped %= ARCMSR_MAX_ARC1214_DONEQUEUE;
			pmu->doneq_index = index_stripped ? (index_stripped | toggle) :
				((toggle ^ 0x4000) + 1);
			doneq_index = pmu->doneq_index;
			addressLow = pmu->done_qbuffer[doneq_index &
				0xFFF].addressLow;
			ccb_cdb_phy = (addressLow & 0xFFFFFFF0);
			if (acb->cdb_phyadd_hipart)
				ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
			arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset
				+ ccb_cdb_phy);
			ccb = container_of(arcmsr_cdb,
				struct CommandControlBlock, arcmsr_cdb);
			error = (addressLow & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1)
				? true : false;
			arcmsr_drain_donequeue(acb, ccb, error);
			writel(doneq_index, pmu->outboundlist_read_pointer);
		} while ((doneq_index & 0xFFF) !=
			(outbound_write_pointer & 0xFFF));
	}
	writel(ARCMSR_ARC1214_OUTBOUND_LIST_INTERRUPT_CLEAR,
		pmu->outboundlist_interrupt_cause);
	readl(pmu->outboundlist_interrupt_cause);
	spin_unlock_irqrestore(&acb->doneq_lock, flags);
}

static void arcmsr_hbaE_postqueue_isr(struct AdapterControlBlock *acb)
{
	uint32_t doneq_index;
	uint16_t cmdSMID;
	int error;
	struct MessageUnit_E __iomem *pmu;
	struct CommandControlBlock *ccb;
	unsigned long flags;

	spin_lock_irqsave(&acb->doneq_lock, flags);
	doneq_index = acb->doneq_index;
	pmu = acb->pmuE;
	while ((readl(&pmu->reply_post_producer_index) & 0xFFFF) != doneq_index) {
		cmdSMID = acb->pCompletionQ[doneq_index].cmdSMID;
		ccb = acb->pccb_pool[cmdSMID];
		error = (acb->pCompletionQ[doneq_index].cmdFlag
			& ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ? true : false;
		arcmsr_drain_donequeue(acb, ccb, error);
		doneq_index++;
		if (doneq_index >= acb->completionQ_entry)
			doneq_index = 0;
	}
	acb->doneq_index = doneq_index;
	writel(doneq_index, &pmu->reply_post_consumer_index);
	spin_unlock_irqrestore(&acb->doneq_lock, flags);
}

static void arcmsr_hbaF_postqueue_isr(struct AdapterControlBlock *acb)
{
	uint32_t doneq_index;
	uint16_t cmdSMID;
	int error;
	struct MessageUnit_F __iomem *phbcmu;
	struct CommandControlBlock *ccb;
	unsigned long flags;

	spin_lock_irqsave(&acb->doneq_lock, flags);
	doneq_index = acb->doneq_index;
	phbcmu = acb->pmuF;
	while (1) {
		cmdSMID = acb->pCompletionQ[doneq_index].cmdSMID;
		if (cmdSMID == 0xffff)
			break;
		ccb = acb->pccb_pool[cmdSMID];
		error = (acb->pCompletionQ[doneq_index].cmdFlag &
			ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ? true : false;
		arcmsr_drain_donequeue(acb, ccb, error);
		acb->pCompletionQ[doneq_index].cmdSMID = 0xffff;
		doneq_index++;
		if (doneq_index >= acb->completionQ_entry)
			doneq_index = 0;
	}
	acb->doneq_index = doneq_index;
	writel(doneq_index, &phbcmu->reply_post_consumer_index);
	spin_unlock_irqrestore(&acb->doneq_lock, flags);
}

/*
**********************************************************************************
** Handle a message interrupt
**
** The only message interrupt we expect is in response to a query for the current adapter config.  
** We want this in order to compare the drivemap so that we can detect newly-attached drives.
**********************************************************************************
*/
static void arcmsr_hbaA_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg  = acb->pmuA;
	/*clear interrupt and message state*/
	writel(ARCMSR_MU_OUTBOUND_MESSAGE0_INT, &reg->outbound_intstatus);
	if (acb->acb_flags & ACB_F_MSG_GET_CONFIG)
		schedule_work(&acb->arcmsr_do_message_isr_bh);
}
static void arcmsr_hbaB_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg  = acb->pmuB;

	/*clear interrupt and message state*/
	writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
	if (acb->acb_flags & ACB_F_MSG_GET_CONFIG)
		schedule_work(&acb->arcmsr_do_message_isr_bh);
}
/*
**********************************************************************************
** Handle a message interrupt
**
** The only message interrupt we expect is in response to a query for the
** current adapter config.
** We want this in order to compare the drivemap so that we can detect newly-attached drives.
**********************************************************************************
*/
static void arcmsr_hbaC_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_C __iomem *reg  = acb->pmuC;
	/*clear interrupt and message state*/
	writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR, &reg->outbound_doorbell_clear);
	if (acb->acb_flags & ACB_F_MSG_GET_CONFIG)
		schedule_work(&acb->arcmsr_do_message_isr_bh);
}

static void arcmsr_hbaD_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_D *reg  = acb->pmuD;

	writel(ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE, reg->outbound_doorbell);
	readl(reg->outbound_doorbell);
	if (acb->acb_flags & ACB_F_MSG_GET_CONFIG)
		schedule_work(&acb->arcmsr_do_message_isr_bh);
}

static void arcmsr_hbaE_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_E __iomem *reg  = acb->pmuE;

	writel(0, &reg->host_int_status);
	if (acb->acb_flags & ACB_F_MSG_GET_CONFIG)
		schedule_work(&acb->arcmsr_do_message_isr_bh);
}

static int arcmsr_hbaA_handle_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_intstatus;
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	outbound_intstatus = readl(&reg->outbound_intstatus) &
		acb->outbound_int_enable;
	if (!(outbound_intstatus & ARCMSR_MU_OUTBOUND_HANDLE_INT))
		return IRQ_NONE;
	do {
		writel(outbound_intstatus, &reg->outbound_intstatus);
		if (outbound_intstatus & ARCMSR_MU_OUTBOUND_DOORBELL_INT)
			arcmsr_hbaA_doorbell_isr(acb);
		if (outbound_intstatus & ARCMSR_MU_OUTBOUND_POSTQUEUE_INT)
			arcmsr_hbaA_postqueue_isr(acb);
		if (outbound_intstatus & ARCMSR_MU_OUTBOUND_MESSAGE0_INT)
			arcmsr_hbaA_message_isr(acb);
		outbound_intstatus = readl(&reg->outbound_intstatus) &
			acb->outbound_int_enable;
	} while (outbound_intstatus & (ARCMSR_MU_OUTBOUND_DOORBELL_INT
		| ARCMSR_MU_OUTBOUND_POSTQUEUE_INT
		| ARCMSR_MU_OUTBOUND_MESSAGE0_INT));
	return IRQ_HANDLED;
}

static int arcmsr_hbaB_handle_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_B *reg = acb->pmuB;
	outbound_doorbell = readl(reg->iop2drv_doorbell) &
				acb->outbound_int_enable;
	if (!outbound_doorbell)
		return IRQ_NONE;
	do {
		writel(~outbound_doorbell, reg->iop2drv_doorbell);
		writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell);
		if (outbound_doorbell & ARCMSR_IOP2DRV_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(acb);
		if (outbound_doorbell & ARCMSR_IOP2DRV_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(acb);
		if (outbound_doorbell & ARCMSR_IOP2DRV_CDB_DONE)
			arcmsr_hbaB_postqueue_isr(acb);
		if (outbound_doorbell & ARCMSR_IOP2DRV_MESSAGE_CMD_DONE)
			arcmsr_hbaB_message_isr(acb);
		outbound_doorbell = readl(reg->iop2drv_doorbell) &
			acb->outbound_int_enable;
	} while (outbound_doorbell & (ARCMSR_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_IOP2DRV_DATA_READ_OK
		| ARCMSR_IOP2DRV_CDB_DONE
		| ARCMSR_IOP2DRV_MESSAGE_CMD_DONE));
	return IRQ_HANDLED;
}

static int arcmsr_hbaC_handle_isr(struct AdapterControlBlock *pACB)
{
	uint32_t host_interrupt_status;
	struct MessageUnit_C __iomem *phbcmu = pACB->pmuC;
	/*
	*********************************************
	**   check outbound intstatus
	*********************************************
	*/
	host_interrupt_status = readl(&phbcmu->host_int_status) &
		(ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR);
	if (!host_interrupt_status)
		return IRQ_NONE;
	do {
		if (host_interrupt_status & ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR)
			arcmsr_hbaC_doorbell_isr(pACB);
		/* MU post queue interrupts*/
		if (host_interrupt_status & ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR)
			arcmsr_hbaC_postqueue_isr(pACB);
		host_interrupt_status = readl(&phbcmu->host_int_status);
	} while (host_interrupt_status & (ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR));
	return IRQ_HANDLED;
}

static irqreturn_t arcmsr_hbaD_handle_isr(struct AdapterControlBlock *pACB)
{
	u32 host_interrupt_status;
	struct MessageUnit_D  *pmu = pACB->pmuD;

	host_interrupt_status = readl(pmu->host_int_status) &
		(ARCMSR_ARC1214_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_ARC1214_OUTBOUND_DOORBELL_ISR);
	if (!host_interrupt_status)
		return IRQ_NONE;
	do {
		/* MU post queue interrupts*/
		if (host_interrupt_status &
			ARCMSR_ARC1214_OUTBOUND_POSTQUEUE_ISR)
			arcmsr_hbaD_postqueue_isr(pACB);
		if (host_interrupt_status &
			ARCMSR_ARC1214_OUTBOUND_DOORBELL_ISR)
			arcmsr_hbaD_doorbell_isr(pACB);
		host_interrupt_status = readl(pmu->host_int_status);
	} while (host_interrupt_status &
		(ARCMSR_ARC1214_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_ARC1214_OUTBOUND_DOORBELL_ISR));
	return IRQ_HANDLED;
}

static irqreturn_t arcmsr_hbaE_handle_isr(struct AdapterControlBlock *pACB)
{
	uint32_t host_interrupt_status;
	struct MessageUnit_E __iomem *pmu = pACB->pmuE;

	host_interrupt_status = readl(&pmu->host_int_status) &
		(ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR);
	if (!host_interrupt_status)
		return IRQ_NONE;
	do {
		/* MU ioctl transfer doorbell interrupts*/
		if (host_interrupt_status & ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR) {
			arcmsr_hbaE_doorbell_isr(pACB);
		}
		/* MU post queue interrupts*/
		if (host_interrupt_status & ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR) {
			arcmsr_hbaE_postqueue_isr(pACB);
		}
		host_interrupt_status = readl(&pmu->host_int_status);
	} while (host_interrupt_status & (ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR));
	return IRQ_HANDLED;
}

static irqreturn_t arcmsr_hbaF_handle_isr(struct AdapterControlBlock *pACB)
{
	uint32_t host_interrupt_status;
	struct MessageUnit_F __iomem *phbcmu = pACB->pmuF;

	host_interrupt_status = readl(&phbcmu->host_int_status) &
		(ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR);
	if (!host_interrupt_status)
		return IRQ_NONE;
	do {
		/* MU post queue interrupts*/
		if (host_interrupt_status & ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR)
			arcmsr_hbaF_postqueue_isr(pACB);

		/* MU ioctl transfer doorbell interrupts*/
		if (host_interrupt_status & ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR)
			arcmsr_hbaE_doorbell_isr(pACB);

		host_interrupt_status = readl(&phbcmu->host_int_status);
	} while (host_interrupt_status & (ARCMSR_HBEMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBEMU_OUTBOUND_DOORBELL_ISR));
	return IRQ_HANDLED;
}

static irqreturn_t arcmsr_interrupt(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		return arcmsr_hbaA_handle_isr(acb);
	case ACB_ADAPTER_TYPE_B:
		return arcmsr_hbaB_handle_isr(acb);
	case ACB_ADAPTER_TYPE_C:
		return arcmsr_hbaC_handle_isr(acb);
	case ACB_ADAPTER_TYPE_D:
		return arcmsr_hbaD_handle_isr(acb);
	case ACB_ADAPTER_TYPE_E:
		return arcmsr_hbaE_handle_isr(acb);
	case ACB_ADAPTER_TYPE_F:
		return arcmsr_hbaF_handle_isr(acb);
	default:
		return IRQ_NONE;
	}
}

static void arcmsr_iop_parking(struct AdapterControlBlock *acb)
{
	if (acb) {
		/* stop adapter background rebuild */
		if (acb->acb_flags & ACB_F_MSG_START_BGRB) {
			uint32_t intmask_org;
			acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
			intmask_org = arcmsr_disable_outbound_ints(acb);
			arcmsr_stop_adapter_bgrb(acb);
			arcmsr_flush_adapter_cache(acb);
			arcmsr_enable_outbound_ints(acb, intmask_org);
		}
	}
}


void arcmsr_clear_iop2drv_rqueue_buffer(struct AdapterControlBlock *acb)
{
	uint32_t	i;

	if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
		for (i = 0; i < 15; i++) {
			if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
				acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
				acb->rqbuf_getIndex = 0;
				acb->rqbuf_putIndex = 0;
				arcmsr_iop_message_read(acb);
				mdelay(30);
			} else if (acb->rqbuf_getIndex !=
				   acb->rqbuf_putIndex) {
				acb->rqbuf_getIndex = 0;
				acb->rqbuf_putIndex = 0;
				mdelay(30);
			} else
				break;
		}
	}
}

static int arcmsr_iop_message_xfer(struct AdapterControlBlock *acb,
		struct scsi_cmnd *cmd)
{
	char *buffer;
	unsigned short use_sg;
	int retvalue = 0, transfer_len = 0;
	unsigned long flags;
	struct CMD_MESSAGE_FIELD *pcmdmessagefld;
	uint32_t controlcode = (uint32_t)cmd->cmnd[5] << 24 |
		(uint32_t)cmd->cmnd[6] << 16 |
		(uint32_t)cmd->cmnd[7] << 8 |
		(uint32_t)cmd->cmnd[8];
	struct scatterlist *sg;

	use_sg = scsi_sg_count(cmd);
	sg = scsi_sglist(cmd);
	buffer = kmap_atomic(sg_page(sg)) + sg->offset;
	if (use_sg > 1) {
		retvalue = ARCMSR_MESSAGE_FAIL;
		goto message_out;
	}
	transfer_len += sg->length;
	if (transfer_len > sizeof(struct CMD_MESSAGE_FIELD)) {
		retvalue = ARCMSR_MESSAGE_FAIL;
		pr_info("%s: ARCMSR_MESSAGE_FAIL!\n", __func__);
		goto message_out;
	}
	pcmdmessagefld = (struct CMD_MESSAGE_FIELD *)buffer;
	switch (controlcode) {
	case ARCMSR_MESSAGE_READ_RQBUFFER: {
		unsigned char *ver_addr;
		uint8_t *ptmpQbuffer;
		uint32_t allxfer_len = 0;
		ver_addr = kmalloc(ARCMSR_API_DATA_BUFLEN, GFP_ATOMIC);
		if (!ver_addr) {
			retvalue = ARCMSR_MESSAGE_FAIL;
			pr_info("%s: memory not enough!\n", __func__);
			goto message_out;
		}
		ptmpQbuffer = ver_addr;
		spin_lock_irqsave(&acb->rqbuffer_lock, flags);
		if (acb->rqbuf_getIndex != acb->rqbuf_putIndex) {
			unsigned int tail = acb->rqbuf_getIndex;
			unsigned int head = acb->rqbuf_putIndex;
			unsigned int cnt_to_end = CIRC_CNT_TO_END(head, tail, ARCMSR_MAX_QBUFFER);

			allxfer_len = CIRC_CNT(head, tail, ARCMSR_MAX_QBUFFER);
			if (allxfer_len > ARCMSR_API_DATA_BUFLEN)
				allxfer_len = ARCMSR_API_DATA_BUFLEN;

			if (allxfer_len <= cnt_to_end)
				memcpy(ptmpQbuffer, acb->rqbuffer + tail, allxfer_len);
			else {
				memcpy(ptmpQbuffer, acb->rqbuffer + tail, cnt_to_end);
				memcpy(ptmpQbuffer + cnt_to_end, acb->rqbuffer, allxfer_len - cnt_to_end);
			}
			acb->rqbuf_getIndex = (acb->rqbuf_getIndex + allxfer_len) % ARCMSR_MAX_QBUFFER;
		}
		memcpy(pcmdmessagefld->messagedatabuffer, ver_addr,
			allxfer_len);
		if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
			struct QBUFFER __iomem *prbuffer;
			acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
			prbuffer = arcmsr_get_iop_rqbuffer(acb);
			if (arcmsr_Read_iop_rqbuffer_data(acb, prbuffer) == 0)
				acb->acb_flags |= ACB_F_IOPDATA_OVERFLOW;
		}
		spin_unlock_irqrestore(&acb->rqbuffer_lock, flags);
		kfree(ver_addr);
		pcmdmessagefld->cmdmessage.Length = allxfer_len;
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		break;
	}
	case ARCMSR_MESSAGE_WRITE_WQBUFFER: {
		unsigned char *ver_addr;
		uint32_t user_len;
		int32_t cnt2end;
		uint8_t *pQbuffer, *ptmpuserbuffer;

		user_len = pcmdmessagefld->cmdmessage.Length;
		if (user_len > ARCMSR_API_DATA_BUFLEN) {
			retvalue = ARCMSR_MESSAGE_FAIL;
			goto message_out;
		}

		ver_addr = kmalloc(ARCMSR_API_DATA_BUFLEN, GFP_ATOMIC);
		if (!ver_addr) {
			retvalue = ARCMSR_MESSAGE_FAIL;
			goto message_out;
		}
		ptmpuserbuffer = ver_addr;

		memcpy(ptmpuserbuffer,
			pcmdmessagefld->messagedatabuffer, user_len);
		spin_lock_irqsave(&acb->wqbuffer_lock, flags);
		if (acb->wqbuf_putIndex != acb->wqbuf_getIndex) {
			struct SENSE_DATA *sensebuffer =
				(struct SENSE_DATA *)cmd->sense_buffer;
			arcmsr_write_ioctldata2iop(acb);
			/* has error report sensedata */
			sensebuffer->ErrorCode = SCSI_SENSE_CURRENT_ERRORS;
			sensebuffer->SenseKey = ILLEGAL_REQUEST;
			sensebuffer->AdditionalSenseLength = 0x0A;
			sensebuffer->AdditionalSenseCode = 0x20;
			sensebuffer->Valid = 1;
			retvalue = ARCMSR_MESSAGE_FAIL;
		} else {
			pQbuffer = &acb->wqbuffer[acb->wqbuf_putIndex];
			cnt2end = ARCMSR_MAX_QBUFFER - acb->wqbuf_putIndex;
			if (user_len > cnt2end) {
				memcpy(pQbuffer, ptmpuserbuffer, cnt2end);
				ptmpuserbuffer += cnt2end;
				user_len -= cnt2end;
				acb->wqbuf_putIndex = 0;
				pQbuffer = acb->wqbuffer;
			}
			memcpy(pQbuffer, ptmpuserbuffer, user_len);
			acb->wqbuf_putIndex += user_len;
			acb->wqbuf_putIndex %= ARCMSR_MAX_QBUFFER;
			if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_CLEARED) {
				acb->acb_flags &=
						~ACB_F_MESSAGE_WQBUFFER_CLEARED;
				arcmsr_write_ioctldata2iop(acb);
			}
		}
		spin_unlock_irqrestore(&acb->wqbuffer_lock, flags);
		kfree(ver_addr);
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		break;
	}
	case ARCMSR_MESSAGE_CLEAR_RQBUFFER: {
		uint8_t *pQbuffer = acb->rqbuffer;

		arcmsr_clear_iop2drv_rqueue_buffer(acb);
		spin_lock_irqsave(&acb->rqbuffer_lock, flags);
		acb->acb_flags |= ACB_F_MESSAGE_RQBUFFER_CLEARED;
		acb->rqbuf_getIndex = 0;
		acb->rqbuf_putIndex = 0;
		memset(pQbuffer, 0, ARCMSR_MAX_QBUFFER);
		spin_unlock_irqrestore(&acb->rqbuffer_lock, flags);
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		break;
	}
	case ARCMSR_MESSAGE_CLEAR_WQBUFFER: {
		uint8_t *pQbuffer = acb->wqbuffer;
		spin_lock_irqsave(&acb->wqbuffer_lock, flags);
		acb->acb_flags |= (ACB_F_MESSAGE_WQBUFFER_CLEARED |
			ACB_F_MESSAGE_WQBUFFER_READED);
		acb->wqbuf_getIndex = 0;
		acb->wqbuf_putIndex = 0;
		memset(pQbuffer, 0, ARCMSR_MAX_QBUFFER);
		spin_unlock_irqrestore(&acb->wqbuffer_lock, flags);
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		break;
	}
	case ARCMSR_MESSAGE_CLEAR_ALLQBUFFER: {
		uint8_t *pQbuffer;
		arcmsr_clear_iop2drv_rqueue_buffer(acb);
		spin_lock_irqsave(&acb->rqbuffer_lock, flags);
		acb->acb_flags |= ACB_F_MESSAGE_RQBUFFER_CLEARED;
		acb->rqbuf_getIndex = 0;
		acb->rqbuf_putIndex = 0;
		pQbuffer = acb->rqbuffer;
		memset(pQbuffer, 0, sizeof(struct QBUFFER));
		spin_unlock_irqrestore(&acb->rqbuffer_lock, flags);
		spin_lock_irqsave(&acb->wqbuffer_lock, flags);
		acb->acb_flags |= (ACB_F_MESSAGE_WQBUFFER_CLEARED |
			ACB_F_MESSAGE_WQBUFFER_READED);
		acb->wqbuf_getIndex = 0;
		acb->wqbuf_putIndex = 0;
		pQbuffer = acb->wqbuffer;
		memset(pQbuffer, 0, sizeof(struct QBUFFER));
		spin_unlock_irqrestore(&acb->wqbuffer_lock, flags);
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		break;
	}
	case ARCMSR_MESSAGE_RETURN_CODE_3F: {
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_3F;
		break;
	}
	case ARCMSR_MESSAGE_SAY_HELLO: {
		int8_t *hello_string = "Hello! I am ARCMSR";
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		memcpy(pcmdmessagefld->messagedatabuffer,
			hello_string, (int16_t)strlen(hello_string));
		break;
	}
	case ARCMSR_MESSAGE_SAY_GOODBYE: {
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		arcmsr_iop_parking(acb);
		break;
	}
	case ARCMSR_MESSAGE_FLUSH_ADAPTER_CACHE: {
		if (acb->fw_flag == FW_DEADLOCK)
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
		else
			pcmdmessagefld->cmdmessage.ReturnCode =
				ARCMSR_MESSAGE_RETURNCODE_OK;
		arcmsr_flush_adapter_cache(acb);
		break;
	}
	default:
		retvalue = ARCMSR_MESSAGE_FAIL;
		pr_info("%s: unknown controlcode!\n", __func__);
	}
message_out:
	if (use_sg) {
		struct scatterlist *sg = scsi_sglist(cmd);
		kunmap_atomic(buffer - sg->offset);
	}
	return retvalue;
}

static struct CommandControlBlock *arcmsr_get_freeccb(struct AdapterControlBlock *acb)
{
	struct list_head *head;
	struct CommandControlBlock *ccb = NULL;
	unsigned long flags;

	spin_lock_irqsave(&acb->ccblist_lock, flags);
	head = &acb->ccb_free_list;
	if (!list_empty(head)) {
		ccb = list_entry(head->next, struct CommandControlBlock, list);
		list_del_init(&ccb->list);
	}else{
		spin_unlock_irqrestore(&acb->ccblist_lock, flags);
		return NULL;
	}
	spin_unlock_irqrestore(&acb->ccblist_lock, flags);
	return ccb;
}

static void arcmsr_handle_virtual_command(struct AdapterControlBlock *acb,
		struct scsi_cmnd *cmd)
{
	switch (cmd->cmnd[0]) {
	case INQUIRY: {
		unsigned char inqdata[36];
		char *buffer;
		struct scatterlist *sg;

		if (cmd->device->lun) {
			cmd->result = (DID_TIME_OUT << 16);
			cmd->scsi_done(cmd);
			return;
		}
		inqdata[0] = TYPE_PROCESSOR;
		/* Periph Qualifier & Periph Dev Type */
		inqdata[1] = 0;
		/* rem media bit & Dev Type Modifier */
		inqdata[2] = 0;
		/* ISO, ECMA, & ANSI versions */
		inqdata[4] = 31;
		/* length of additional data */
		memcpy(&inqdata[8], "Areca   ", 8);
		/* Vendor Identification */
		memcpy(&inqdata[16], "RAID controller ", 16);
		/* Product Identification */
		memcpy(&inqdata[32], "R001", 4); /* Product Revision */

		sg = scsi_sglist(cmd);
		buffer = kmap_atomic(sg_page(sg)) + sg->offset;

		memcpy(buffer, inqdata, sizeof(inqdata));
		sg = scsi_sglist(cmd);
		kunmap_atomic(buffer - sg->offset);

		cmd->scsi_done(cmd);
	}
	break;
	case WRITE_BUFFER:
	case READ_BUFFER: {
		if (arcmsr_iop_message_xfer(acb, cmd))
			cmd->result = (DID_ERROR << 16);
		cmd->scsi_done(cmd);
	}
	break;
	default:
		cmd->scsi_done(cmd);
	}
}

static int arcmsr_queue_command_lck(struct scsi_cmnd *cmd,
	void (* done)(struct scsi_cmnd *))
{
	struct Scsi_Host *host = cmd->device->host;
	struct AdapterControlBlock *acb = (struct AdapterControlBlock *) host->hostdata;
	struct CommandControlBlock *ccb;
	int target = cmd->device->id;

	if (acb->acb_flags & ACB_F_ADAPTER_REMOVED) {
		cmd->result = (DID_NO_CONNECT << 16);
		cmd->scsi_done(cmd);
		return 0;
	}
	cmd->scsi_done = done;
	cmd->host_scribble = NULL;
	cmd->result = 0;
	if (target == 16) {
		/* virtual device for iop message transfer */
		arcmsr_handle_virtual_command(acb, cmd);
		return 0;
	}
	ccb = arcmsr_get_freeccb(acb);
	if (!ccb)
		return SCSI_MLQUEUE_HOST_BUSY;
	if (arcmsr_build_ccb( acb, ccb, cmd ) == FAILED) {
		cmd->result = (DID_ERROR << 16) | (RESERVATION_CONFLICT << 1);
		cmd->scsi_done(cmd);
		return 0;
	}
	arcmsr_post_ccb(acb, ccb);
	return 0;
}

static DEF_SCSI_QCMD(arcmsr_queue_command)

static int arcmsr_slave_config(struct scsi_device *sdev)
{
	unsigned int	dev_timeout;

	dev_timeout = sdev->request_queue->rq_timeout;
	if ((cmd_timeout > 0) && ((cmd_timeout * HZ) > dev_timeout))
		blk_queue_rq_timeout(sdev->request_queue, cmd_timeout * HZ);
	return 0;
}

static void arcmsr_get_adapter_config(struct AdapterControlBlock *pACB, uint32_t *rwbuffer)
{
	int count;
	uint32_t *acb_firm_model = (uint32_t *)pACB->firm_model;
	uint32_t *acb_firm_version = (uint32_t *)pACB->firm_version;
	uint32_t *acb_device_map = (uint32_t *)pACB->device_map;
	uint32_t *firm_model = &rwbuffer[15];
	uint32_t *firm_version = &rwbuffer[17];
	uint32_t *device_map = &rwbuffer[21];

	count = 2;
	while (count) {
		*acb_firm_model = readl(firm_model);
		acb_firm_model++;
		firm_model++;
		count--;
	}
	count = 4;
	while (count) {
		*acb_firm_version = readl(firm_version);
		acb_firm_version++;
		firm_version++;
		count--;
	}
	count = 4;
	while (count) {
		*acb_device_map = readl(device_map);
		acb_device_map++;
		device_map++;
		count--;
	}
	pACB->signature = readl(&rwbuffer[0]);
	pACB->firm_request_len = readl(&rwbuffer[1]);
	pACB->firm_numbers_queue = readl(&rwbuffer[2]);
	pACB->firm_sdram_size = readl(&rwbuffer[3]);
	pACB->firm_hd_channels = readl(&rwbuffer[4]);
	pACB->firm_cfg_version = readl(&rwbuffer[25]);
	pr_notice("Areca RAID Controller%d: Model %s, F/W %s\n",
		pACB->host->host_no,
		pACB->firm_model,
		pACB->firm_version);
}

static bool arcmsr_hbaA_get_config(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;

	arcmsr_wait_firmware_ready(acb);
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
	if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", acb->host->host_no);
		return false;
	}
	arcmsr_get_adapter_config(acb, reg->message_rwbuffer);
	return true;
}
static bool arcmsr_hbaB_get_config(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;

	arcmsr_wait_firmware_ready(acb);
	writel(ARCMSR_MESSAGE_START_DRIVER_MODE, reg->drv2iop_doorbell);
	if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
		printk(KERN_ERR "arcmsr%d: can't set driver mode.\n", acb->host->host_no);
		return false;
	}
	writel(ARCMSR_MESSAGE_GET_CONFIG, reg->drv2iop_doorbell);
	if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", acb->host->host_no);
		return false;
	}
	arcmsr_get_adapter_config(acb, reg->message_rwbuffer);
	return true;
}

static bool arcmsr_hbaC_get_config(struct AdapterControlBlock *pACB)
{
	uint32_t intmask_org;
	struct MessageUnit_C __iomem *reg = pACB->pmuC;

	/* disable all outbound interrupt */
	intmask_org = readl(&reg->host_int_mask); /* disable outbound message0 int */
	writel(intmask_org|ARCMSR_HBCMU_ALL_INTMASKENABLE, &reg->host_int_mask);
	/* wait firmware ready */
	arcmsr_wait_firmware_ready(pACB);
	/* post "get config" instruction */
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	/* wait message ready */
	if (!arcmsr_hbaC_wait_msgint_ready(pACB)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", pACB->host->host_no);
		return false;
	}
	arcmsr_get_adapter_config(pACB, reg->msgcode_rwbuffer);
	return true;
}

static bool arcmsr_hbaD_get_config(struct AdapterControlBlock *acb)
{
	struct MessageUnit_D *reg = acb->pmuD;

	if (readl(acb->pmuD->outbound_doorbell) &
		ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE) {
		writel(ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE,
			acb->pmuD->outbound_doorbell);/*clear interrupt*/
	}
	arcmsr_wait_firmware_ready(acb);
	/* post "get config" instruction */
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, reg->inbound_msgaddr0);
	/* wait message ready */
	if (!arcmsr_hbaD_wait_msgint_ready(acb)) {
		pr_notice("arcmsr%d: wait get adapter firmware "
			"miscellaneous data timeout\n", acb->host->host_no);
		return false;
	}
	arcmsr_get_adapter_config(acb, reg->msgcode_rwbuffer);
	return true;
}

static bool arcmsr_hbaE_get_config(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_E __iomem *reg = pACB->pmuE;
	uint32_t intmask_org;

	/* disable all outbound interrupt */
	intmask_org = readl(&reg->host_int_mask); /* disable outbound message0 int */
	writel(intmask_org | ARCMSR_HBEMU_ALL_INTMASKENABLE, &reg->host_int_mask);
	/* wait firmware ready */
	arcmsr_wait_firmware_ready(pACB);
	mdelay(20);
	/* post "get config" instruction */
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);

	pACB->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
	writel(pACB->out_doorbell, &reg->iobound_doorbell);
	/* wait message ready */
	if (!arcmsr_hbaE_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait get adapter firmware "
			"miscellaneous data timeout\n", pACB->host->host_no);
		return false;
	}
	arcmsr_get_adapter_config(pACB, reg->msgcode_rwbuffer);
	return true;
}

static bool arcmsr_hbaF_get_config(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_F __iomem *reg = pACB->pmuF;
	uint32_t intmask_org;

	/* disable all outbound interrupt */
	intmask_org = readl(&reg->host_int_mask); /* disable outbound message0 int */
	writel(intmask_org | ARCMSR_HBEMU_ALL_INTMASKENABLE, &reg->host_int_mask);
	/* wait firmware ready */
	arcmsr_wait_firmware_ready(pACB);
	/* post "get config" instruction */
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);

	pACB->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
	writel(pACB->out_doorbell, &reg->iobound_doorbell);
	/* wait message ready */
	if (!arcmsr_hbaE_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait get adapter firmware miscellaneous data timeout\n",
			  pACB->host->host_no);
		return false;
	}
	arcmsr_get_adapter_config(pACB, pACB->msgcode_rwbuffer);
	return true;
}

static bool arcmsr_get_firmware_spec(struct AdapterControlBlock *acb)
{
	bool rtn = false;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		rtn = arcmsr_hbaA_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		rtn = arcmsr_hbaB_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		rtn = arcmsr_hbaC_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		rtn = arcmsr_hbaD_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_E:
		rtn = arcmsr_hbaE_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_F:
		rtn = arcmsr_hbaF_get_config(acb);
		break;
	default:
		break;
	}
	acb->maxOutstanding = acb->firm_numbers_queue - 1;
	if (acb->host->can_queue >= acb->firm_numbers_queue)
		acb->host->can_queue = acb->maxOutstanding;
	else
		acb->maxOutstanding = acb->host->can_queue;
	acb->maxFreeCCB = acb->host->can_queue;
	if (acb->maxFreeCCB < ARCMSR_MAX_FREECCB_NUM)
		acb->maxFreeCCB += 64;
	return rtn;
}

static int arcmsr_hbaA_polling_ccbdone(struct AdapterControlBlock *acb,
	struct CommandControlBlock *poll_ccb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	struct CommandControlBlock *ccb;
	struct ARCMSR_CDB *arcmsr_cdb;
	uint32_t flag_ccb, outbound_intstatus, poll_ccb_done = 0, poll_count = 0;
	int rtn;
	bool error;
	unsigned long ccb_cdb_phy;

polling_hba_ccb_retry:
	poll_count++;
	outbound_intstatus = readl(&reg->outbound_intstatus) & acb->outbound_int_enable;
	writel(outbound_intstatus, &reg->outbound_intstatus);/*clear interrupt*/
	while (1) {
		if ((flag_ccb = readl(&reg->outbound_queueport)) == 0xFFFFFFFF) {
			if (poll_ccb_done){
				rtn = SUCCESS;
				break;
			}else {
				msleep(25);
				if (poll_count > 100){
					rtn = FAILED;
					break;
				}
				goto polling_hba_ccb_retry;
			}
		}
		ccb_cdb_phy = (flag_ccb << 5) & 0xffffffff;
		if (acb->cdb_phyadd_hipart)
			ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset + ccb_cdb_phy);
		ccb = container_of(arcmsr_cdb, struct CommandControlBlock, arcmsr_cdb);
		poll_ccb_done |= (ccb == poll_ccb) ? 1 : 0;
		if ((ccb->acb != acb) || (ccb->startdone != ARCMSR_CCB_START)) {
			if ((ccb->startdone == ARCMSR_CCB_ABORTED) || (ccb == poll_ccb)) {
				printk(KERN_NOTICE "arcmsr%d: scsi id = %d lun = %d ccb = '0x%p'"
					" poll command abort successfully \n"
					, acb->host->host_no
					, ccb->pcmd->device->id
					, (u32)ccb->pcmd->device->lun
					, ccb);
				ccb->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(ccb);
				continue;
			}
			printk(KERN_NOTICE "arcmsr%d: polling get an illegal ccb"
				" command done ccb = '0x%p'"
				"ccboutstandingcount = %d \n"
				, acb->host->host_no
				, ccb
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		}
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
		arcmsr_report_ccb_state(acb, ccb, error);
	}
	return rtn;
}

static int arcmsr_hbaB_polling_ccbdone(struct AdapterControlBlock *acb,
					struct CommandControlBlock *poll_ccb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *ccb;
	uint32_t flag_ccb, poll_ccb_done = 0, poll_count = 0;
	int index, rtn;
	bool error;
	unsigned long ccb_cdb_phy;

polling_hbb_ccb_retry:
	poll_count++;
	/* clear doorbell interrupt */
	writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
	while(1){
		index = reg->doneq_index;
		flag_ccb = reg->done_qbuffer[index];
		if (flag_ccb == 0) {
			if (poll_ccb_done){
				rtn = SUCCESS;
				break;
			}else {
				msleep(25);
				if (poll_count > 100){
					rtn = FAILED;
					break;
				}
				goto polling_hbb_ccb_retry;
			}
		}
		reg->done_qbuffer[index] = 0;
		index++;
		/*if last index number set it to 0 */
		index %= ARCMSR_MAX_HBB_POSTQUEUE;
		reg->doneq_index = index;
		/* check if command done with no error*/
		ccb_cdb_phy = (flag_ccb << 5) & 0xffffffff;
		if (acb->cdb_phyadd_hipart)
			ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset + ccb_cdb_phy);
		ccb = container_of(arcmsr_cdb, struct CommandControlBlock, arcmsr_cdb);
		poll_ccb_done |= (ccb == poll_ccb) ? 1 : 0;
		if ((ccb->acb != acb) || (ccb->startdone != ARCMSR_CCB_START)) {
			if ((ccb->startdone == ARCMSR_CCB_ABORTED) || (ccb == poll_ccb)) {
				printk(KERN_NOTICE "arcmsr%d: scsi id = %d lun = %d ccb = '0x%p'"
					" poll command abort successfully \n"
					,acb->host->host_no
					,ccb->pcmd->device->id
					,(u32)ccb->pcmd->device->lun
					,ccb);
				ccb->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(ccb);
				continue;
			}
			printk(KERN_NOTICE "arcmsr%d: polling get an illegal ccb"
				" command done ccb = '0x%p'"
				"ccboutstandingcount = %d \n"
				, acb->host->host_no
				, ccb
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		} 
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
		arcmsr_report_ccb_state(acb, ccb, error);
	}
	return rtn;
}

static int arcmsr_hbaC_polling_ccbdone(struct AdapterControlBlock *acb,
		struct CommandControlBlock *poll_ccb)
{
	struct MessageUnit_C __iomem *reg = acb->pmuC;
	uint32_t flag_ccb;
	struct ARCMSR_CDB *arcmsr_cdb;
	bool error;
	struct CommandControlBlock *pCCB;
	uint32_t poll_ccb_done = 0, poll_count = 0;
	int rtn;
	unsigned long ccb_cdb_phy;

polling_hbc_ccb_retry:
	poll_count++;
	while (1) {
		if ((readl(&reg->host_int_status) & ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR) == 0) {
			if (poll_ccb_done) {
				rtn = SUCCESS;
				break;
			} else {
				msleep(25);
				if (poll_count > 100) {
					rtn = FAILED;
					break;
				}
				goto polling_hbc_ccb_retry;
			}
		}
		flag_ccb = readl(&reg->outbound_queueport_low);
		ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
		if (acb->cdb_phyadd_hipart)
			ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset + ccb_cdb_phy);
		pCCB = container_of(arcmsr_cdb, struct CommandControlBlock, arcmsr_cdb);
		poll_ccb_done |= (pCCB == poll_ccb) ? 1 : 0;
		/* check ifcommand done with no error*/
		if ((pCCB->acb != acb) || (pCCB->startdone != ARCMSR_CCB_START)) {
			if (pCCB->startdone == ARCMSR_CCB_ABORTED) {
				printk(KERN_NOTICE "arcmsr%d: scsi id = %d lun = %d ccb = '0x%p'"
					" poll command abort successfully \n"
					, acb->host->host_no
					, pCCB->pcmd->device->id
					, (u32)pCCB->pcmd->device->lun
					, pCCB);
				pCCB->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(pCCB);
				continue;
			}
			printk(KERN_NOTICE "arcmsr%d: polling get an illegal ccb"
				" command done ccb = '0x%p'"
				"ccboutstandingcount = %d \n"
				, acb->host->host_no
				, pCCB
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		}
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ? true : false;
		arcmsr_report_ccb_state(acb, pCCB, error);
	}
	return rtn;
}

static int arcmsr_hbaD_polling_ccbdone(struct AdapterControlBlock *acb,
				struct CommandControlBlock *poll_ccb)
{
	bool error;
	uint32_t poll_ccb_done = 0, poll_count = 0, flag_ccb;
	int rtn, doneq_index, index_stripped, outbound_write_pointer, toggle;
	unsigned long flags, ccb_cdb_phy;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *pCCB;
	struct MessageUnit_D *pmu = acb->pmuD;

polling_hbaD_ccb_retry:
	poll_count++;
	while (1) {
		spin_lock_irqsave(&acb->doneq_lock, flags);
		outbound_write_pointer = pmu->done_qbuffer[0].addressLow + 1;
		doneq_index = pmu->doneq_index;
		if ((outbound_write_pointer & 0xFFF) == (doneq_index & 0xFFF)) {
			spin_unlock_irqrestore(&acb->doneq_lock, flags);
			if (poll_ccb_done) {
				rtn = SUCCESS;
				break;
			} else {
				msleep(25);
				if (poll_count > 40) {
					rtn = FAILED;
					break;
				}
				goto polling_hbaD_ccb_retry;
			}
		}
		toggle = doneq_index & 0x4000;
		index_stripped = (doneq_index & 0xFFF) + 1;
		index_stripped %= ARCMSR_MAX_ARC1214_DONEQUEUE;
		pmu->doneq_index = index_stripped ? (index_stripped | toggle) :
				((toggle ^ 0x4000) + 1);
		doneq_index = pmu->doneq_index;
		spin_unlock_irqrestore(&acb->doneq_lock, flags);
		flag_ccb = pmu->done_qbuffer[doneq_index & 0xFFF].addressLow;
		ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
		if (acb->cdb_phyadd_hipart)
			ccb_cdb_phy = ccb_cdb_phy | acb->cdb_phyadd_hipart;
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset +
			ccb_cdb_phy);
		pCCB = container_of(arcmsr_cdb, struct CommandControlBlock,
			arcmsr_cdb);
		poll_ccb_done |= (pCCB == poll_ccb) ? 1 : 0;
		if ((pCCB->acb != acb) ||
			(pCCB->startdone != ARCMSR_CCB_START)) {
			if (pCCB->startdone == ARCMSR_CCB_ABORTED) {
				pr_notice("arcmsr%d: scsi id = %d "
					"lun = %d ccb = '0x%p' poll command "
					"abort successfully\n"
					, acb->host->host_no
					, pCCB->pcmd->device->id
					, (u32)pCCB->pcmd->device->lun
					, pCCB);
				pCCB->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(pCCB);
				continue;
			}
			pr_notice("arcmsr%d: polling an illegal "
				"ccb command done ccb = '0x%p' "
				"ccboutstandingcount = %d\n"
				, acb->host->host_no
				, pCCB
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		}
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1)
			? true : false;
		arcmsr_report_ccb_state(acb, pCCB, error);
	}
	return rtn;
}

static int arcmsr_hbaE_polling_ccbdone(struct AdapterControlBlock *acb,
				struct CommandControlBlock *poll_ccb)
{
	bool error;
	uint32_t poll_ccb_done = 0, poll_count = 0, doneq_index;
	uint16_t cmdSMID;
	unsigned long flags;
	int rtn;
	struct CommandControlBlock *pCCB;
	struct MessageUnit_E __iomem *reg = acb->pmuE;

	polling_hbaC_ccb_retry:
	poll_count++;
	while (1) {
		spin_lock_irqsave(&acb->doneq_lock, flags);
		doneq_index = acb->doneq_index;
		if ((readl(&reg->reply_post_producer_index) & 0xFFFF) ==
				doneq_index) {
			spin_unlock_irqrestore(&acb->doneq_lock, flags);
			if (poll_ccb_done) {
				rtn = SUCCESS;
				break;
			} else {
				msleep(25);
				if (poll_count > 40) {
					rtn = FAILED;
					break;
				}
				goto polling_hbaC_ccb_retry;
			}
		}
		cmdSMID = acb->pCompletionQ[doneq_index].cmdSMID;
		doneq_index++;
		if (doneq_index >= acb->completionQ_entry)
			doneq_index = 0;
		acb->doneq_index = doneq_index;
		spin_unlock_irqrestore(&acb->doneq_lock, flags);
		pCCB = acb->pccb_pool[cmdSMID];
		poll_ccb_done |= (pCCB == poll_ccb) ? 1 : 0;
		/* check if command done with no error*/
		if ((pCCB->acb != acb) || (pCCB->startdone != ARCMSR_CCB_START)) {
			if (pCCB->startdone == ARCMSR_CCB_ABORTED) {
				pr_notice("arcmsr%d: scsi id = %d "
					"lun = %d ccb = '0x%p' poll command "
					"abort successfully\n"
					, acb->host->host_no
					, pCCB->pcmd->device->id
					, (u32)pCCB->pcmd->device->lun
					, pCCB);
				pCCB->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(pCCB);
				continue;
			}
			pr_notice("arcmsr%d: polling an illegal "
				"ccb command done ccb = '0x%p' "
				"ccboutstandingcount = %d\n"
				, acb->host->host_no
				, pCCB
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		}
		error = (acb->pCompletionQ[doneq_index].cmdFlag &
			ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ? true : false;
		arcmsr_report_ccb_state(acb, pCCB, error);
	}
	writel(doneq_index, &reg->reply_post_consumer_index);
	return rtn;
}

static int arcmsr_polling_ccbdone(struct AdapterControlBlock *acb,
					struct CommandControlBlock *poll_ccb)
{
	int rtn = 0;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A:
		rtn = arcmsr_hbaA_polling_ccbdone(acb, poll_ccb);
		break;
	case ACB_ADAPTER_TYPE_B:
		rtn = arcmsr_hbaB_polling_ccbdone(acb, poll_ccb);
		break;
	case ACB_ADAPTER_TYPE_C:
		rtn = arcmsr_hbaC_polling_ccbdone(acb, poll_ccb);
		break;
	case ACB_ADAPTER_TYPE_D:
		rtn = arcmsr_hbaD_polling_ccbdone(acb, poll_ccb);
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		rtn = arcmsr_hbaE_polling_ccbdone(acb, poll_ccb);
		break;
	}
	return rtn;
}

static void arcmsr_set_iop_datetime(struct timer_list *t)
{
	struct AdapterControlBlock *pacb = from_timer(pacb, t, refresh_timer);
	unsigned int next_time;
	struct tm tm;

	union {
		struct	{
		uint16_t	signature;
		uint8_t		year;
		uint8_t		month;
		uint8_t		date;
		uint8_t		hour;
		uint8_t		minute;
		uint8_t		second;
		} a;
		struct	{
		uint32_t	msg_time[2];
		} b;
	} datetime;

	time64_to_tm(ktime_get_real_seconds(), -sys_tz.tz_minuteswest * 60, &tm);

	datetime.a.signature = 0x55AA;
	datetime.a.year = tm.tm_year - 100; /* base 2000 instead of 1900 */
	datetime.a.month = tm.tm_mon;
	datetime.a.date = tm.tm_mday;
	datetime.a.hour = tm.tm_hour;
	datetime.a.minute = tm.tm_min;
	datetime.a.second = tm.tm_sec;

	switch (pacb->adapter_type) {
		case ACB_ADAPTER_TYPE_A: {
			struct MessageUnit_A __iomem *reg = pacb->pmuA;
			writel(datetime.b.msg_time[0], &reg->message_rwbuffer[0]);
			writel(datetime.b.msg_time[1], &reg->message_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SYNC_TIMER, &reg->inbound_msgaddr0);
			break;
		}
		case ACB_ADAPTER_TYPE_B: {
			uint32_t __iomem *rwbuffer;
			struct MessageUnit_B *reg = pacb->pmuB;
			rwbuffer = reg->message_rwbuffer;
			writel(datetime.b.msg_time[0], rwbuffer++);
			writel(datetime.b.msg_time[1], rwbuffer++);
			writel(ARCMSR_MESSAGE_SYNC_TIMER, reg->drv2iop_doorbell);
			break;
		}
		case ACB_ADAPTER_TYPE_C: {
			struct MessageUnit_C __iomem *reg = pacb->pmuC;
			writel(datetime.b.msg_time[0], &reg->msgcode_rwbuffer[0]);
			writel(datetime.b.msg_time[1], &reg->msgcode_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SYNC_TIMER, &reg->inbound_msgaddr0);
			writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
			break;
		}
		case ACB_ADAPTER_TYPE_D: {
			uint32_t __iomem *rwbuffer;
			struct MessageUnit_D *reg = pacb->pmuD;
			rwbuffer = reg->msgcode_rwbuffer;
			writel(datetime.b.msg_time[0], rwbuffer++);
			writel(datetime.b.msg_time[1], rwbuffer++);
			writel(ARCMSR_INBOUND_MESG0_SYNC_TIMER, reg->inbound_msgaddr0);
			break;
		}
		case ACB_ADAPTER_TYPE_E: {
			struct MessageUnit_E __iomem *reg = pacb->pmuE;
			writel(datetime.b.msg_time[0], &reg->msgcode_rwbuffer[0]);
			writel(datetime.b.msg_time[1], &reg->msgcode_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SYNC_TIMER, &reg->inbound_msgaddr0);
			pacb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
			writel(pacb->out_doorbell, &reg->iobound_doorbell);
			break;
		}
		case ACB_ADAPTER_TYPE_F: {
			struct MessageUnit_F __iomem *reg = pacb->pmuF;

			pacb->msgcode_rwbuffer[0] = datetime.b.msg_time[0];
			pacb->msgcode_rwbuffer[1] = datetime.b.msg_time[1];
			writel(ARCMSR_INBOUND_MESG0_SYNC_TIMER, &reg->inbound_msgaddr0);
			pacb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
			writel(pacb->out_doorbell, &reg->iobound_doorbell);
			break;
		}
	}
	if (sys_tz.tz_minuteswest)
		next_time = ARCMSR_HOURS;
	else
		next_time = ARCMSR_MINUTES;
	mod_timer(&pacb->refresh_timer, jiffies + msecs_to_jiffies(next_time));
}

static int arcmsr_iop_confirm(struct AdapterControlBlock *acb)
{
	uint32_t cdb_phyaddr, cdb_phyaddr_hi32;
	dma_addr_t dma_coherent_handle;

	/*
	********************************************************************
	** here we need to tell iop 331 our freeccb.HighPart
	** if freeccb.HighPart is not zero
	********************************************************************
	*/
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B:
	case ACB_ADAPTER_TYPE_D:
		dma_coherent_handle = acb->dma_coherent_handle2;
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		dma_coherent_handle = acb->dma_coherent_handle +
			offsetof(struct CommandControlBlock, arcmsr_cdb);
		break;
	default:
		dma_coherent_handle = acb->dma_coherent_handle;
		break;
	}
	cdb_phyaddr = lower_32_bits(dma_coherent_handle);
	cdb_phyaddr_hi32 = upper_32_bits(dma_coherent_handle);
	acb->cdb_phyaddr_hi32 = cdb_phyaddr_hi32;
	acb->cdb_phyadd_hipart = ((uint64_t)cdb_phyaddr_hi32) << 32;
	/*
	***********************************************************************
	**    if adapter type B, set window of "post command Q"
	***********************************************************************
	*/
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		if (cdb_phyaddr_hi32 != 0) {
			struct MessageUnit_A __iomem *reg = acb->pmuA;
			writel(ARCMSR_SIGNATURE_SET_CONFIG, \
						&reg->message_rwbuffer[0]);
			writel(cdb_phyaddr_hi32, &reg->message_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, \
							&reg->inbound_msgaddr0);
			if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "arcmsr%d: ""set ccb high \
				part physical address timeout\n",
				acb->host->host_no);
				return 1;
			}
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		uint32_t __iomem *rwbuffer;

		struct MessageUnit_B *reg = acb->pmuB;
		reg->postq_index = 0;
		reg->doneq_index = 0;
		writel(ARCMSR_MESSAGE_SET_POST_WINDOW, reg->drv2iop_doorbell);
		if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d: cannot set driver mode\n", \
				acb->host->host_no);
			return 1;
		}
		rwbuffer = reg->message_rwbuffer;
		/* driver "set config" signature */
		writel(ARCMSR_SIGNATURE_SET_CONFIG, rwbuffer++);
		/* normal should be zero */
		writel(cdb_phyaddr_hi32, rwbuffer++);
		/* postQ size (256 + 8)*4	 */
		writel(cdb_phyaddr, rwbuffer++);
		/* doneQ size (256 + 8)*4	 */
		writel(cdb_phyaddr + 1056, rwbuffer++);
		/* ccb maxQ size must be --> [(256 + 8)*4]*/
		writel(1056, rwbuffer);

		writel(ARCMSR_MESSAGE_SET_CONFIG, reg->drv2iop_doorbell);
		if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d: 'set command Q window' \
			timeout \n",acb->host->host_no);
			return 1;
		}
		writel(ARCMSR_MESSAGE_START_DRIVER_MODE, reg->drv2iop_doorbell);
		if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
			pr_err("arcmsr%d: can't set driver mode.\n",
				acb->host->host_no);
			return 1;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
			struct MessageUnit_C __iomem *reg = acb->pmuC;

			printk(KERN_NOTICE "arcmsr%d: cdb_phyaddr_hi32=0x%x\n",
					acb->adapter_index, cdb_phyaddr_hi32);
			writel(ARCMSR_SIGNATURE_SET_CONFIG, &reg->msgcode_rwbuffer[0]);
			writel(cdb_phyaddr_hi32, &reg->msgcode_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, &reg->inbound_msgaddr0);
			writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
			if (!arcmsr_hbaC_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "arcmsr%d: 'set command Q window' \
				timeout \n", acb->host->host_no);
				return 1;
			}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		uint32_t __iomem *rwbuffer;
		struct MessageUnit_D *reg = acb->pmuD;
		reg->postq_index = 0;
		reg->doneq_index = 0;
		rwbuffer = reg->msgcode_rwbuffer;
		writel(ARCMSR_SIGNATURE_SET_CONFIG, rwbuffer++);
		writel(cdb_phyaddr_hi32, rwbuffer++);
		writel(cdb_phyaddr, rwbuffer++);
		writel(cdb_phyaddr + (ARCMSR_MAX_ARC1214_POSTQUEUE *
			sizeof(struct InBound_SRB)), rwbuffer++);
		writel(0x100, rwbuffer);
		writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, reg->inbound_msgaddr0);
		if (!arcmsr_hbaD_wait_msgint_ready(acb)) {
			pr_notice("arcmsr%d: 'set command Q window' timeout\n",
				acb->host->host_no);
			return 1;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_E: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		writel(ARCMSR_SIGNATURE_SET_CONFIG, &reg->msgcode_rwbuffer[0]);
		writel(ARCMSR_SIGNATURE_1884, &reg->msgcode_rwbuffer[1]);
		writel(cdb_phyaddr, &reg->msgcode_rwbuffer[2]);
		writel(cdb_phyaddr_hi32, &reg->msgcode_rwbuffer[3]);
		writel(acb->ccbsize, &reg->msgcode_rwbuffer[4]);
		writel(lower_32_bits(acb->dma_coherent_handle2), &reg->msgcode_rwbuffer[5]);
		writel(upper_32_bits(acb->dma_coherent_handle2), &reg->msgcode_rwbuffer[6]);
		writel(acb->ioqueue_size, &reg->msgcode_rwbuffer[7]);
		writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, &reg->inbound_msgaddr0);
		acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
		writel(acb->out_doorbell, &reg->iobound_doorbell);
		if (!arcmsr_hbaE_wait_msgint_ready(acb)) {
			pr_notice("arcmsr%d: 'set command Q window' timeout \n",
				acb->host->host_no);
			return 1;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_F __iomem *reg = acb->pmuF;

		acb->msgcode_rwbuffer[0] = ARCMSR_SIGNATURE_SET_CONFIG;
		acb->msgcode_rwbuffer[1] = ARCMSR_SIGNATURE_1886;
		acb->msgcode_rwbuffer[2] = cdb_phyaddr;
		acb->msgcode_rwbuffer[3] = cdb_phyaddr_hi32;
		acb->msgcode_rwbuffer[4] = acb->ccbsize;
		acb->msgcode_rwbuffer[5] = lower_32_bits(acb->dma_coherent_handle2);
		acb->msgcode_rwbuffer[6] = upper_32_bits(acb->dma_coherent_handle2);
		acb->msgcode_rwbuffer[7] = acb->completeQ_size;
		writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, &reg->inbound_msgaddr0);
		acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
		writel(acb->out_doorbell, &reg->iobound_doorbell);
		if (!arcmsr_hbaE_wait_msgint_ready(acb)) {
			pr_notice("arcmsr%d: 'set command Q window' timeout\n",
				acb->host->host_no);
			return 1;
		}
		}
		break;
	}
	return 0;
}

static void arcmsr_wait_firmware_ready(struct AdapterControlBlock *acb)
{
	uint32_t firmware_state = 0;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		do {
			if (!(acb->acb_flags & ACB_F_IOP_INITED))
				msleep(20);
			firmware_state = readl(&reg->outbound_msgaddr1);
		} while ((firmware_state & ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		do {
			if (!(acb->acb_flags & ACB_F_IOP_INITED))
				msleep(20);
			firmware_state = readl(reg->iop2drv_doorbell);
		} while ((firmware_state & ARCMSR_MESSAGE_FIRMWARE_OK) == 0);
		writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		do {
			if (!(acb->acb_flags & ACB_F_IOP_INITED))
				msleep(20);
			firmware_state = readl(&reg->outbound_msgaddr1);
		} while ((firmware_state & ARCMSR_HBCMU_MESSAGE_FIRMWARE_OK) == 0);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		do {
			if (!(acb->acb_flags & ACB_F_IOP_INITED))
				msleep(20);
			firmware_state = readl(reg->outbound_msgaddr1);
		} while ((firmware_state &
			ARCMSR_ARC1214_MESSAGE_FIRMWARE_OK) == 0);
		}
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		do {
			if (!(acb->acb_flags & ACB_F_IOP_INITED))
				msleep(20);
			firmware_state = readl(&reg->outbound_msgaddr1);
		} while ((firmware_state & ARCMSR_HBEMU_MESSAGE_FIRMWARE_OK) == 0);
		}
		break;
	}
}

static void arcmsr_request_device_map(struct timer_list *t)
{
	struct AdapterControlBlock *acb = from_timer(acb, t, eternal_timer);
	if (acb->acb_flags & (ACB_F_MSG_GET_CONFIG | ACB_F_BUS_RESET | ACB_F_ABORT)) {
		mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
	} else {
		acb->fw_flag = FW_NORMAL;
		switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A: {
			struct MessageUnit_A __iomem *reg = acb->pmuA;
			writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
			break;
			}
		case ACB_ADAPTER_TYPE_B: {
			struct MessageUnit_B *reg = acb->pmuB;
			writel(ARCMSR_MESSAGE_GET_CONFIG, reg->drv2iop_doorbell);
			break;
			}
		case ACB_ADAPTER_TYPE_C: {
			struct MessageUnit_C __iomem *reg = acb->pmuC;
			writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
			writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
			break;
			}
		case ACB_ADAPTER_TYPE_D: {
			struct MessageUnit_D *reg = acb->pmuD;
			writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, reg->inbound_msgaddr0);
			break;
			}
		case ACB_ADAPTER_TYPE_E: {
			struct MessageUnit_E __iomem *reg = acb->pmuE;
			writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
			acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
			writel(acb->out_doorbell, &reg->iobound_doorbell);
			break;
			}
		case ACB_ADAPTER_TYPE_F: {
			struct MessageUnit_F __iomem *reg = acb->pmuF;
			uint32_t outMsg1 = readl(&reg->outbound_msgaddr1);

			if (!(outMsg1 & ARCMSR_HBFMU_MESSAGE_FIRMWARE_OK) ||
				(outMsg1 & ARCMSR_HBFMU_MESSAGE_NO_VOLUME_CHANGE))
				goto nxt6s;
			writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
			acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
			writel(acb->out_doorbell, &reg->iobound_doorbell);
			break;
			}
		default:
			return;
		}
		acb->acb_flags |= ACB_F_MSG_GET_CONFIG;
nxt6s:
		mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
	}
}

static void arcmsr_hbaA_start_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	acb->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_START_BGRB, &reg->inbound_msgaddr0);
	if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'start adapter background \
				rebuild' timeout \n", acb->host->host_no);
	}
}

static void arcmsr_hbaB_start_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	acb->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_MESSAGE_START_BGRB, reg->drv2iop_doorbell);
	if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'start adapter background \
				rebuild' timeout \n",acb->host->host_no);
	}
}

static void arcmsr_hbaC_start_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *phbcmu = pACB->pmuC;
	pACB->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_START_BGRB, &phbcmu->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &phbcmu->inbound_doorbell);
	if (!arcmsr_hbaC_wait_msgint_ready(pACB)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'start adapter background \
				rebuild' timeout \n", pACB->host->host_no);
	}
	return;
}

static void arcmsr_hbaD_start_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *pmu = pACB->pmuD;

	pACB->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_START_BGRB, pmu->inbound_msgaddr0);
	if (!arcmsr_hbaD_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait 'start adapter "
			"background rebuild' timeout\n", pACB->host->host_no);
	}
}

static void arcmsr_hbaE_start_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_E __iomem *pmu = pACB->pmuE;

	pACB->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_START_BGRB, &pmu->inbound_msgaddr0);
	pACB->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_MESSAGE_CMD_DONE;
	writel(pACB->out_doorbell, &pmu->iobound_doorbell);
	if (!arcmsr_hbaE_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait 'start adapter "
			"background rebuild' timeout \n", pACB->host->host_no);
	}
}

static void arcmsr_start_adapter_bgrb(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		arcmsr_hbaA_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		arcmsr_hbaB_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		arcmsr_hbaC_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		arcmsr_hbaD_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:
		arcmsr_hbaE_start_bgrb(acb);
		break;
	}
}

static void arcmsr_clear_doorbell_queue_buffer(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		uint32_t outbound_doorbell;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(&reg->outbound_doorbell);
		/*clear doorbell interrupt */
		writel(outbound_doorbell, &reg->outbound_doorbell);
		writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		uint32_t outbound_doorbell, i;
		writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
		writel(ARCMSR_DRV2IOP_DATA_READ_OK, reg->drv2iop_doorbell);
		/* let IOP know data has been read */
		for(i=0; i < 200; i++) {
			msleep(20);
			outbound_doorbell = readl(reg->iop2drv_doorbell);
			if( outbound_doorbell & ARCMSR_IOP2DRV_DATA_WRITE_OK) {
				writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
				writel(ARCMSR_DRV2IOP_DATA_READ_OK, reg->drv2iop_doorbell);
			} else
				break;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		uint32_t outbound_doorbell, i;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(&reg->outbound_doorbell);
		writel(outbound_doorbell, &reg->outbound_doorbell_clear);
		writel(ARCMSR_HBCMU_DRV2IOP_DATA_READ_OK, &reg->inbound_doorbell);
		for (i = 0; i < 200; i++) {
			msleep(20);
			outbound_doorbell = readl(&reg->outbound_doorbell);
			if (outbound_doorbell &
				ARCMSR_HBCMU_IOP2DRV_DATA_WRITE_OK) {
				writel(outbound_doorbell,
					&reg->outbound_doorbell_clear);
				writel(ARCMSR_HBCMU_DRV2IOP_DATA_READ_OK,
					&reg->inbound_doorbell);
			} else
				break;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		uint32_t outbound_doorbell, i;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(reg->outbound_doorbell);
		writel(outbound_doorbell, reg->outbound_doorbell);
		writel(ARCMSR_ARC1214_DRV2IOP_DATA_OUT_READ,
			reg->inbound_doorbell);
		for (i = 0; i < 200; i++) {
			msleep(20);
			outbound_doorbell = readl(reg->outbound_doorbell);
			if (outbound_doorbell &
				ARCMSR_ARC1214_IOP2DRV_DATA_WRITE_OK) {
				writel(outbound_doorbell,
					reg->outbound_doorbell);
				writel(ARCMSR_ARC1214_DRV2IOP_DATA_OUT_READ,
					reg->inbound_doorbell);
			} else
				break;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F: {
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		uint32_t i, tmp;

		acb->in_doorbell = readl(&reg->iobound_doorbell);
		writel(0, &reg->host_int_status); /*clear interrupt*/
		acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_DATA_READ_OK;
		writel(acb->out_doorbell, &reg->iobound_doorbell);
		for(i=0; i < 200; i++) {
			msleep(20);
			tmp = acb->in_doorbell;
			acb->in_doorbell = readl(&reg->iobound_doorbell);
			if((tmp ^ acb->in_doorbell) & ARCMSR_HBEMU_IOP2DRV_DATA_WRITE_OK) {
				writel(0, &reg->host_int_status); /*clear interrupt*/
				acb->out_doorbell ^= ARCMSR_HBEMU_DRV2IOP_DATA_READ_OK;
				writel(acb->out_doorbell, &reg->iobound_doorbell);
			} else
				break;
		}
		}
		break;
	}
}

static void arcmsr_enable_eoi_mode(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		return;
	case ACB_ADAPTER_TYPE_B:
		{
			struct MessageUnit_B *reg = acb->pmuB;
			writel(ARCMSR_MESSAGE_ACTIVE_EOI_MODE, reg->drv2iop_doorbell);
			if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "ARCMSR IOP enables EOI_MODE TIMEOUT");
				return;
			}
		}
		break;
	case ACB_ADAPTER_TYPE_C:
		return;
	}
	return;
}

static void arcmsr_hardware_reset(struct AdapterControlBlock *acb)
{
	uint8_t value[64];
	int i, count = 0;
	struct MessageUnit_A __iomem *pmuA = acb->pmuA;
	struct MessageUnit_C __iomem *pmuC = acb->pmuC;
	struct MessageUnit_D *pmuD = acb->pmuD;

	/* backup pci config data */
	printk(KERN_NOTICE "arcmsr%d: executing hw bus reset .....\n", acb->host->host_no);
	for (i = 0; i < 64; i++) {
		pci_read_config_byte(acb->pdev, i, &value[i]);
	}
	/* hardware reset signal */
	if (acb->dev_id == 0x1680) {
		writel(ARCMSR_ARC1680_BUS_RESET, &pmuA->reserved1[0]);
	} else if (acb->dev_id == 0x1880) {
		do {
			count++;
			writel(0xF, &pmuC->write_sequence);
			writel(0x4, &pmuC->write_sequence);
			writel(0xB, &pmuC->write_sequence);
			writel(0x2, &pmuC->write_sequence);
			writel(0x7, &pmuC->write_sequence);
			writel(0xD, &pmuC->write_sequence);
		} while (((readl(&pmuC->host_diagnostic) & ARCMSR_ARC1880_DiagWrite_ENABLE) == 0) && (count < 5));
		writel(ARCMSR_ARC1880_RESET_ADAPTER, &pmuC->host_diagnostic);
	} else if (acb->dev_id == 0x1884) {
		struct MessageUnit_E __iomem *pmuE = acb->pmuE;
		do {
			count++;
			writel(0x4, &pmuE->write_sequence_3xxx);
			writel(0xB, &pmuE->write_sequence_3xxx);
			writel(0x2, &pmuE->write_sequence_3xxx);
			writel(0x7, &pmuE->write_sequence_3xxx);
			writel(0xD, &pmuE->write_sequence_3xxx);
			mdelay(10);
		} while (((readl(&pmuE->host_diagnostic_3xxx) &
			ARCMSR_ARC1884_DiagWrite_ENABLE) == 0) && (count < 5));
		writel(ARCMSR_ARC188X_RESET_ADAPTER, &pmuE->host_diagnostic_3xxx);
	} else if (acb->dev_id == 0x1214) {
		writel(0x20, pmuD->reset_request);
	} else {
		pci_write_config_byte(acb->pdev, 0x84, 0x20);
	}
	msleep(2000);
	/* write back pci config data */
	for (i = 0; i < 64; i++) {
		pci_write_config_byte(acb->pdev, i, value[i]);
	}
	msleep(1000);
	return;
}

static bool arcmsr_reset_in_progress(struct AdapterControlBlock *acb)
{
	bool rtn = true;

	switch(acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:{
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		rtn = ((readl(&reg->outbound_msgaddr1) &
			ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0) ? true : false;
		}
		break;
	case ACB_ADAPTER_TYPE_B:{
		struct MessageUnit_B *reg = acb->pmuB;
		rtn = ((readl(reg->iop2drv_doorbell) &
			ARCMSR_MESSAGE_FIRMWARE_OK) == 0) ? true : false;
		}
		break;
	case ACB_ADAPTER_TYPE_C:{
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		rtn = (readl(&reg->host_diagnostic) & 0x04) ? true : false;
		}
		break;
	case ACB_ADAPTER_TYPE_D:{
		struct MessageUnit_D *reg = acb->pmuD;
		rtn = ((readl(reg->sample_at_reset) & 0x80) == 0) ?
			true : false;
		}
		break;
	case ACB_ADAPTER_TYPE_E:
	case ACB_ADAPTER_TYPE_F:{
		struct MessageUnit_E __iomem *reg = acb->pmuE;
		rtn = (readl(&reg->host_diagnostic_3xxx) &
			ARCMSR_ARC188X_RESET_ADAPTER) ? true : false;
		}
		break;
	}
	return rtn;
}

static void arcmsr_iop_init(struct AdapterControlBlock *acb)
{
	uint32_t intmask_org;
	/* disable all outbound interrupt */
	intmask_org = arcmsr_disable_outbound_ints(acb);
	arcmsr_wait_firmware_ready(acb);
	arcmsr_iop_confirm(acb);
	/*start background rebuild*/
	arcmsr_start_adapter_bgrb(acb);
	/* empty doorbell Qbuffer if door bell ringed */
	arcmsr_clear_doorbell_queue_buffer(acb);
	arcmsr_enable_eoi_mode(acb);
	/* enable outbound Post Queue,outbound doorbell Interrupt */
	arcmsr_enable_outbound_ints(acb, intmask_org);
	acb->acb_flags |= ACB_F_IOP_INITED;
}

static uint8_t arcmsr_iop_reset(struct AdapterControlBlock *acb)
{
	struct CommandControlBlock *ccb;
	uint32_t intmask_org;
	uint8_t rtnval = 0x00;
	int i = 0;
	unsigned long flags;

	if (atomic_read(&acb->ccboutstandingcount) != 0) {
		/* disable all outbound interrupt */
		intmask_org = arcmsr_disable_outbound_ints(acb);
		/* talk to iop 331 outstanding command aborted */
		rtnval = arcmsr_abort_allcmd(acb);
		/* clear all outbound posted Q */
		arcmsr_done4abort_postqueue(acb);
		for (i = 0; i < acb->maxFreeCCB; i++) {
			ccb = acb->pccb_pool[i];
			if (ccb->startdone == ARCMSR_CCB_START) {
				scsi_dma_unmap(ccb->pcmd);
				ccb->startdone = ARCMSR_CCB_DONE;
				ccb->ccb_flags = 0;
				spin_lock_irqsave(&acb->ccblist_lock, flags);
				list_add_tail(&ccb->list, &acb->ccb_free_list);
				spin_unlock_irqrestore(&acb->ccblist_lock, flags);
			}
		}
		atomic_set(&acb->ccboutstandingcount, 0);
		/* enable all outbound interrupt */
		arcmsr_enable_outbound_ints(acb, intmask_org);
		return rtnval;
	}
	return rtnval;
}

static int arcmsr_bus_reset(struct scsi_cmnd *cmd)
{
	struct AdapterControlBlock *acb;
	int retry_count = 0;
	int rtn = FAILED;
	acb = (struct AdapterControlBlock *) cmd->device->host->hostdata;
	if (acb->acb_flags & ACB_F_ADAPTER_REMOVED)
		return SUCCESS;
	pr_notice("arcmsr: executing bus reset eh.....num_resets = %d,"
		" num_aborts = %d \n", acb->num_resets, acb->num_aborts);
	acb->num_resets++;

	if (acb->acb_flags & ACB_F_BUS_RESET) {
		long timeout;
		pr_notice("arcmsr: there is a bus reset eh proceeding...\n");
		timeout = wait_event_timeout(wait_q, (acb->acb_flags
			& ACB_F_BUS_RESET) == 0, 220 * HZ);
		if (timeout)
			return SUCCESS;
	}
	acb->acb_flags |= ACB_F_BUS_RESET;
	if (!arcmsr_iop_reset(acb)) {
		arcmsr_hardware_reset(acb);
		acb->acb_flags &= ~ACB_F_IOP_INITED;
wait_reset_done:
		ssleep(ARCMSR_SLEEPTIME);
		if (arcmsr_reset_in_progress(acb)) {
			if (retry_count > ARCMSR_RETRYCOUNT) {
				acb->fw_flag = FW_DEADLOCK;
				pr_notice("arcmsr%d: waiting for hw bus reset"
					" return, RETRY TERMINATED!!\n",
					acb->host->host_no);
				return FAILED;
			}
			retry_count++;
			goto wait_reset_done;
		}
		arcmsr_iop_init(acb);
		acb->fw_flag = FW_NORMAL;
		mod_timer(&acb->eternal_timer, jiffies +
			msecs_to_jiffies(6 * HZ));
		acb->acb_flags &= ~ACB_F_BUS_RESET;
		rtn = SUCCESS;
		pr_notice("arcmsr: scsi bus reset eh returns with success\n");
	} else {
		acb->acb_flags &= ~ACB_F_BUS_RESET;
		acb->fw_flag = FW_NORMAL;
		mod_timer(&acb->eternal_timer, jiffies +
			msecs_to_jiffies(6 * HZ));
		rtn = SUCCESS;
	}
	return rtn;
}

static int arcmsr_abort_one_cmd(struct AdapterControlBlock *acb,
		struct CommandControlBlock *ccb)
{
	int rtn;
	rtn = arcmsr_polling_ccbdone(acb, ccb);
	return rtn;
}

static int arcmsr_abort(struct scsi_cmnd *cmd)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)cmd->device->host->hostdata;
	int i = 0;
	int rtn = FAILED;
	uint32_t intmask_org;

	if (acb->acb_flags & ACB_F_ADAPTER_REMOVED)
		return SUCCESS;
	printk(KERN_NOTICE
		"arcmsr%d: abort device command of scsi id = %d lun = %d\n",
		acb->host->host_no, cmd->device->id, (u32)cmd->device->lun);
	acb->acb_flags |= ACB_F_ABORT;
	acb->num_aborts++;
	/*
	************************************************
	** the all interrupt service routine is locked
	** we need to handle it as soon as possible and exit
	************************************************
	*/
	if (!atomic_read(&acb->ccboutstandingcount)) {
		acb->acb_flags &= ~ACB_F_ABORT;
		return rtn;
	}

	intmask_org = arcmsr_disable_outbound_ints(acb);
	for (i = 0; i < acb->maxFreeCCB; i++) {
		struct CommandControlBlock *ccb = acb->pccb_pool[i];
		if (ccb->startdone == ARCMSR_CCB_START && ccb->pcmd == cmd) {
			ccb->startdone = ARCMSR_CCB_ABORTED;
			rtn = arcmsr_abort_one_cmd(acb, ccb);
			break;
		}
	}
	acb->acb_flags &= ~ACB_F_ABORT;
	arcmsr_enable_outbound_ints(acb, intmask_org);
	return rtn;
}

static const char *arcmsr_info(struct Scsi_Host *host)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *) host->hostdata;
	static char buf[256];
	char *type;
	int raid6 = 1;
	switch (acb->pdev->device) {
	case PCI_DEVICE_ID_ARECA_1110:
	case PCI_DEVICE_ID_ARECA_1200:
	case PCI_DEVICE_ID_ARECA_1202:
	case PCI_DEVICE_ID_ARECA_1210:
		raid6 = 0;
		fallthrough;
	case PCI_DEVICE_ID_ARECA_1120:
	case PCI_DEVICE_ID_ARECA_1130:
	case PCI_DEVICE_ID_ARECA_1160:
	case PCI_DEVICE_ID_ARECA_1170:
	case PCI_DEVICE_ID_ARECA_1201:
	case PCI_DEVICE_ID_ARECA_1203:
	case PCI_DEVICE_ID_ARECA_1220:
	case PCI_DEVICE_ID_ARECA_1230:
	case PCI_DEVICE_ID_ARECA_1260:
	case PCI_DEVICE_ID_ARECA_1270:
	case PCI_DEVICE_ID_ARECA_1280:
		type = "SATA";
		break;
	case PCI_DEVICE_ID_ARECA_1214:
	case PCI_DEVICE_ID_ARECA_1380:
	case PCI_DEVICE_ID_ARECA_1381:
	case PCI_DEVICE_ID_ARECA_1680:
	case PCI_DEVICE_ID_ARECA_1681:
	case PCI_DEVICE_ID_ARECA_1880:
	case PCI_DEVICE_ID_ARECA_1884:
		type = "SAS/SATA";
		break;
	case PCI_DEVICE_ID_ARECA_1886:
		type = "NVMe/SAS/SATA";
		break;
	default:
		type = "unknown";
		raid6 =	0;
		break;
	}
	sprintf(buf, "Areca %s RAID Controller %s\narcmsr version %s\n",
		type, raid6 ? "(RAID6 capable)" : "", ARCMSR_DRIVER_VERSION);
	return buf;
}

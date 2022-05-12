/*
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2008 - 2011 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * The full GNU General Public License is included in this distribution
 * in the file called LICENSE.GPL.
 *
 * BSD LICENSE
 *
 * Copyright(c) 2008 - 2011 Intel Corporation. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/efi.h>
#include <asm/string.h>
#include <scsi/scsi_host.h>
#include "host.h"
#include "isci.h"
#include "task.h"
#include "probe_roms.h"

#define MAJ 1
#define MIN 2
#define BUILD 0
#define DRV_VERSION __stringify(MAJ) "." __stringify(MIN) "." \
	__stringify(BUILD)

MODULE_VERSION(DRV_VERSION);

static struct scsi_transport_template *isci_transport_template;

static const struct pci_device_id isci_id_table[] = {
	{ PCI_VDEVICE(INTEL, 0x1D61),},
	{ PCI_VDEVICE(INTEL, 0x1D63),},
	{ PCI_VDEVICE(INTEL, 0x1D65),},
	{ PCI_VDEVICE(INTEL, 0x1D67),},
	{ PCI_VDEVICE(INTEL, 0x1D69),},
	{ PCI_VDEVICE(INTEL, 0x1D6B),},
	{ PCI_VDEVICE(INTEL, 0x1D60),},
	{ PCI_VDEVICE(INTEL, 0x1D62),},
	{ PCI_VDEVICE(INTEL, 0x1D64),},
	{ PCI_VDEVICE(INTEL, 0x1D66),},
	{ PCI_VDEVICE(INTEL, 0x1D68),},
	{ PCI_VDEVICE(INTEL, 0x1D6A),},
	{}
};

MODULE_DEVICE_TABLE(pci, isci_id_table);

/* linux isci specific settings */

unsigned char no_outbound_task_to = 2;
module_param(no_outbound_task_to, byte, 0);
MODULE_PARM_DESC(no_outbound_task_to, "No Outbound Task Timeout (1us incr)");

u16 ssp_max_occ_to = 20;
module_param(ssp_max_occ_to, ushort, 0);
MODULE_PARM_DESC(ssp_max_occ_to, "SSP Max occupancy timeout (100us incr)");

u16 stp_max_occ_to = 5;
module_param(stp_max_occ_to, ushort, 0);
MODULE_PARM_DESC(stp_max_occ_to, "STP Max occupancy timeout (100us incr)");

u16 ssp_inactive_to = 5;
module_param(ssp_inactive_to, ushort, 0);
MODULE_PARM_DESC(ssp_inactive_to, "SSP inactivity timeout (100us incr)");

u16 stp_inactive_to = 5;
module_param(stp_inactive_to, ushort, 0);
MODULE_PARM_DESC(stp_inactive_to, "STP inactivity timeout (100us incr)");

unsigned char phy_gen = SCIC_SDS_PARM_GEN2_SPEED;
module_param(phy_gen, byte, 0);
MODULE_PARM_DESC(phy_gen, "PHY generation (1: 1.5Gbps 2: 3.0Gbps 3: 6.0Gbps)");

unsigned char max_concurr_spinup;
module_param(max_concurr_spinup, byte, 0);
MODULE_PARM_DESC(max_concurr_spinup, "Max concurrent device spinup");

uint cable_selection_override = CABLE_OVERRIDE_DISABLED;
module_param(cable_selection_override, uint, 0);

MODULE_PARM_DESC(cable_selection_override,
		 "This field indicates length of the SAS/SATA cable between "
		 "host and device. If any bits > 15 are set (default) "
		 "indicates \"use platform defaults\"");

static ssize_t isci_show_id(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = container_of(dev, typeof(*shost), shost_dev);
	struct sas_ha_struct *sas_ha = SHOST_TO_SAS_HA(shost);
	struct isci_host *ihost = container_of(sas_ha, typeof(*ihost), sas_ha);

	return snprintf(buf, PAGE_SIZE, "%d\n", ihost->id);
}

static DEVICE_ATTR(isci_id, S_IRUGO, isci_show_id, NULL);

static struct device_attribute *isci_host_attrs[] = {
	&dev_attr_isci_id,
	NULL
};

static struct scsi_host_template isci_sht = {

	.module				= THIS_MODULE,
	.name				= DRV_NAME,
	.proc_name			= DRV_NAME,
	.queuecommand			= sas_queuecommand,
	.dma_need_drain			= ata_scsi_dma_need_drain,
	.target_alloc			= sas_target_alloc,
	.slave_configure		= sas_slave_configure,
	.scan_finished			= isci_host_scan_finished,
	.scan_start			= isci_host_start,
	.change_queue_depth		= sas_change_queue_depth,
	.bios_param			= sas_bios_param,
	.can_queue			= ISCI_CAN_QUEUE_VAL,
	.this_id			= -1,
	.sg_tablesize			= SG_ALL,
	.max_sectors			= SCSI_DEFAULT_MAX_SECTORS,
	.eh_abort_handler		= sas_eh_abort_handler,
	.eh_device_reset_handler        = sas_eh_device_reset_handler,
	.eh_target_reset_handler        = sas_eh_target_reset_handler,
	.target_destroy			= sas_target_destroy,
	.ioctl				= sas_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl			= sas_ioctl,
#endif
	.shost_attrs			= isci_host_attrs,
	.track_queue_depth		= 1,
};

static struct sas_domain_function_template isci_transport_ops  = {

	/* The class calls these to notify the LLDD of an event. */
	.lldd_port_formed	= isci_port_formed,
	.lldd_port_deformed	= isci_port_deformed,

	/* The class calls these when a device is found or gone. */
	.lldd_dev_found		= isci_remote_device_found,
	.lldd_dev_gone		= isci_remote_device_gone,

	.lldd_execute_task	= isci_task_execute_task,
	/* Task Management Functions. Must be called from process context. */
	.lldd_abort_task	= isci_task_abort_task,
	.lldd_abort_task_set	= isci_task_abort_task_set,
	.lldd_clear_aca		= isci_task_clear_aca,
	.lldd_clear_task_set	= isci_task_clear_task_set,
	.lldd_I_T_nexus_reset	= isci_task_I_T_nexus_reset,
	.lldd_lu_reset		= isci_task_lu_reset,
	.lldd_query_task	= isci_task_query_task,

	/* ata recovery called from ata-eh */
	.lldd_ata_check_ready	= isci_ata_check_ready,

	/* Port and Adapter management */
	.lldd_clear_nexus_port	= isci_task_clear_nexus_port,
	.lldd_clear_nexus_ha	= isci_task_clear_nexus_ha,

	/* Phy management */
	.lldd_control_phy	= isci_phy_control,

	/* GPIO support */
	.lldd_write_gpio	= isci_gpio_write,
};


/******************************************************************************
* P R O T E C T E D  M E T H O D S
******************************************************************************/



/**
 * isci_register_sas_ha() - This method initializes various lldd
 *    specific members of the sas_ha struct and calls the libsas
 *    sas_register_ha() function.
 * @isci_host: This parameter specifies the lldd specific wrapper for the
 *    libsas sas_ha struct.
 *
 * This method returns an error code indicating success or failure. The user
 * should check for possible memory allocation error return otherwise, a zero
 * indicates success.
 */
static int isci_register_sas_ha(struct isci_host *isci_host)
{
	int i;
	struct sas_ha_struct *sas_ha = &(isci_host->sas_ha);
	struct asd_sas_phy **sas_phys;
	struct asd_sas_port **sas_ports;

	sas_phys = devm_kcalloc(&isci_host->pdev->dev,
				SCI_MAX_PHYS, sizeof(void *),
				GFP_KERNEL);
	if (!sas_phys)
		return -ENOMEM;

	sas_ports = devm_kcalloc(&isci_host->pdev->dev,
				 SCI_MAX_PORTS, sizeof(void *),
				 GFP_KERNEL);
	if (!sas_ports)
		return -ENOMEM;

	sas_ha->sas_ha_name = DRV_NAME;
	sas_ha->lldd_module = THIS_MODULE;
	sas_ha->sas_addr    = &isci_host->phys[0].sas_addr[0];

	for (i = 0; i < SCI_MAX_PHYS; i++) {
		sas_phys[i] = &isci_host->phys[i].sas_phy;
		sas_ports[i] = &isci_host->sas_ports[i];
	}

	sas_ha->sas_phy  = sas_phys;
	sas_ha->sas_port = sas_ports;
	sas_ha->num_phys = SCI_MAX_PHYS;

	sas_ha->strict_wide_ports = 1;

	sas_register_ha(sas_ha);

	return 0;
}

static void isci_unregister(struct isci_host *isci_host)
{
	struct Scsi_Host *shost;

	if (!isci_host)
		return;

	shost = to_shost(isci_host);
	sas_unregister_ha(&isci_host->sas_ha);

	sas_remove_host(shost);
	scsi_host_put(shost);
}

static int isci_pci_init(struct pci_dev *pdev)
{
	int err, bar_num, bar_mask = 0;
	void __iomem * const *iomap;

	err = pcim_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev,
			"failed enable PCI device %s!\n",
			pci_name(pdev));
		return err;
	}

	for (bar_num = 0; bar_num < SCI_PCI_BAR_COUNT; bar_num++)
		bar_mask |= 1 << (bar_num * 2);

	err = pcim_iomap_regions(pdev, bar_mask, DRV_NAME);
	if (err)
		return err;

	iomap = pcim_iomap_table(pdev);
	if (!iomap)
		return -ENOMEM;

	pci_set_master(pdev);

	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	if (err)
		err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
	return err;
}

static int num_controllers(struct pci_dev *pdev)
{
	/* bar size alone can tell us if we are running with a dual controller
	 * part, no need to trust revision ids that might be under broken firmware
	 * control
	 */
	resource_size_t scu_bar_size = pci_resource_len(pdev, SCI_SCU_BAR*2);
	resource_size_t smu_bar_size = pci_resource_len(pdev, SCI_SMU_BAR*2);

	if (scu_bar_size >= SCI_SCU_BAR_SIZE*SCI_MAX_CONTROLLERS &&
	    smu_bar_size >= SCI_SMU_BAR_SIZE*SCI_MAX_CONTROLLERS)
		return SCI_MAX_CONTROLLERS;
	else
		return 1;
}

static int isci_setup_interrupts(struct pci_dev *pdev)
{
	int err, i, num_msix;
	struct isci_host *ihost;
	struct isci_pci_info *pci_info = to_pci_info(pdev);

	/*
	 *  Determine the number of vectors associated with this
	 *  PCI function.
	 */
	num_msix = num_controllers(pdev) * SCI_NUM_MSI_X_INT;

	err = pci_alloc_irq_vectors(pdev, num_msix, num_msix, PCI_IRQ_MSIX);
	if (err < 0)
		goto intx;

	for (i = 0; i < num_msix; i++) {
		int id = i / SCI_NUM_MSI_X_INT;
		irq_handler_t isr;

		ihost = pci_info->hosts[id];
		/* odd numbered vectors are error interrupts */
		if (i & 1)
			isr = isci_error_isr;
		else
			isr = isci_msix_isr;

		err = devm_request_irq(&pdev->dev, pci_irq_vector(pdev, i),
				isr, 0, DRV_NAME"-msix", ihost);
		if (!err)
			continue;

		dev_info(&pdev->dev, "msix setup failed falling back to intx\n");
		while (i--) {
			id = i / SCI_NUM_MSI_X_INT;
			ihost = pci_info->hosts[id];
			devm_free_irq(&pdev->dev, pci_irq_vector(pdev, i),
					ihost);
		}
		pci_free_irq_vectors(pdev);
		goto intx;
	}
	return 0;

 intx:
	for_each_isci_host(i, ihost, pdev) {
		err = devm_request_irq(&pdev->dev, pci_irq_vector(pdev, 0),
				isci_intx_isr, IRQF_SHARED, DRV_NAME"-intx",
				ihost);
		if (err)
			break;
	}
	return err;
}

static void isci_user_parameters_get(struct sci_user_parameters *u)
{
	int i;

	for (i = 0; i < SCI_MAX_PHYS; i++) {
		struct sci_phy_user_params *u_phy = &u->phys[i];

		u_phy->max_speed_generation = phy_gen;

		/* we are not exporting these for now */
		u_phy->align_insertion_frequency = 0x7f;
		u_phy->in_connection_align_insertion_frequency = 0xff;
		u_phy->notify_enable_spin_up_insertion_frequency = 0x33;
	}

	u->stp_inactivity_timeout = stp_inactive_to;
	u->ssp_inactivity_timeout = ssp_inactive_to;
	u->stp_max_occupancy_timeout = stp_max_occ_to;
	u->ssp_max_occupancy_timeout = ssp_max_occ_to;
	u->no_outbound_task_timeout = no_outbound_task_to;
	u->max_concurr_spinup = max_concurr_spinup;
}

static enum sci_status sci_user_parameters_set(struct isci_host *ihost,
					       struct sci_user_parameters *sci_parms)
{
	u16 index;

	/*
	 * Validate the user parameters.  If they are not legal, then
	 * return a failure.
	 */
	for (index = 0; index < SCI_MAX_PHYS; index++) {
		struct sci_phy_user_params *u;

		u = &sci_parms->phys[index];

		if (!((u->max_speed_generation <= SCIC_SDS_PARM_MAX_SPEED) &&
		      (u->max_speed_generation > SCIC_SDS_PARM_NO_SPEED)))
			return SCI_FAILURE_INVALID_PARAMETER_VALUE;

		if ((u->in_connection_align_insertion_frequency < 3) ||
		    (u->align_insertion_frequency == 0) ||
		    (u->notify_enable_spin_up_insertion_frequency == 0))
			return SCI_FAILURE_INVALID_PARAMETER_VALUE;
	}

	if ((sci_parms->stp_inactivity_timeout == 0) ||
	    (sci_parms->ssp_inactivity_timeout == 0) ||
	    (sci_parms->stp_max_occupancy_timeout == 0) ||
	    (sci_parms->ssp_max_occupancy_timeout == 0) ||
	    (sci_parms->no_outbound_task_timeout == 0))
		return SCI_FAILURE_INVALID_PARAMETER_VALUE;

	memcpy(&ihost->user_parameters, sci_parms, sizeof(*sci_parms));

	return SCI_SUCCESS;
}

static void sci_oem_defaults(struct isci_host *ihost)
{
	/* these defaults are overridden by the platform / firmware */
	struct sci_user_parameters *user = &ihost->user_parameters;
	struct sci_oem_params *oem = &ihost->oem_parameters;
	int i;

	/* Default to APC mode. */
	oem->controller.mode_type = SCIC_PORT_AUTOMATIC_CONFIGURATION_MODE;

	/* Default to APC mode. */
	oem->controller.max_concurr_spin_up = 1;

	/* Default to no SSC operation. */
	oem->controller.do_enable_ssc = false;

	/* Default to short cables on all phys. */
	oem->controller.cable_selection_mask = 0;

	/* Initialize all of the port parameter information to narrow ports. */
	for (i = 0; i < SCI_MAX_PORTS; i++)
		oem->ports[i].phy_mask = 0;

	/* Initialize all of the phy parameter information. */
	for (i = 0; i < SCI_MAX_PHYS; i++) {
		/* Default to 3G (i.e. Gen 2). */
		user->phys[i].max_speed_generation = SCIC_SDS_PARM_GEN2_SPEED;

		/* the frequencies cannot be 0 */
		user->phys[i].align_insertion_frequency = 0x7f;
		user->phys[i].in_connection_align_insertion_frequency = 0xff;
		user->phys[i].notify_enable_spin_up_insertion_frequency = 0x33;

		/* Previous Vitesse based expanders had a arbitration issue that
		 * is worked around by having the upper 32-bits of SAS address
		 * with a value greater then the Vitesse company identifier.
		 * Hence, usage of 0x5FCFFFFF.
		 */
		oem->phys[i].sas_address.low = 0x1 + ihost->id;
		oem->phys[i].sas_address.high = 0x5FCFFFFF;
	}

	user->stp_inactivity_timeout = 5;
	user->ssp_inactivity_timeout = 5;
	user->stp_max_occupancy_timeout = 5;
	user->ssp_max_occupancy_timeout = 20;
	user->no_outbound_task_timeout = 2;
}

static struct isci_host *isci_host_alloc(struct pci_dev *pdev, int id)
{
	struct isci_orom *orom = to_pci_info(pdev)->orom;
	struct sci_user_parameters sci_user_params;
	u8 oem_version = ISCI_ROM_VER_1_0;
	struct isci_host *ihost;
	struct Scsi_Host *shost;
	int err, i;

	ihost = devm_kzalloc(&pdev->dev, sizeof(*ihost), GFP_KERNEL);
	if (!ihost)
		return NULL;

	ihost->pdev = pdev;
	ihost->id = id;
	spin_lock_init(&ihost->scic_lock);
	init_waitqueue_head(&ihost->eventq);
	ihost->sas_ha.dev = &ihost->pdev->dev;
	ihost->sas_ha.lldd_ha = ihost;
	tasklet_init(&ihost->completion_tasklet,
		     isci_host_completion_routine, (unsigned long)ihost);

	/* validate module parameters */
	/* TODO: kill struct sci_user_parameters and reference directly */
	sci_oem_defaults(ihost);
	isci_user_parameters_get(&sci_user_params);
	if (sci_user_parameters_set(ihost, &sci_user_params)) {
		dev_warn(&pdev->dev,
			 "%s: sci_user_parameters_set failed\n", __func__);
		return NULL;
	}

	/* sanity check platform (or 'firmware') oem parameters */
	if (orom) {
		if (id < 0 || id >= SCI_MAX_CONTROLLERS || id > orom->hdr.num_elements) {
			dev_warn(&pdev->dev, "parsing firmware oem parameters failed\n");
			return NULL;
		}
		ihost->oem_parameters = orom->ctrl[id];
		oem_version = orom->hdr.version;
	}

	/* validate oem parameters (platform, firmware, or built-in defaults) */
	if (sci_oem_parameters_validate(&ihost->oem_parameters, oem_version)) {
		dev_warn(&pdev->dev, "oem parameter validation failed\n");
		return NULL;
	}

	for (i = 0; i < SCI_MAX_PORTS; i++) {
		struct isci_port *iport = &ihost->ports[i];

		INIT_LIST_HEAD(&iport->remote_dev_list);
		iport->isci_host = ihost;
	}

	for (i = 0; i < SCI_MAX_PHYS; i++)
		isci_phy_init(&ihost->phys[i], ihost, i);

	for (i = 0; i < SCI_MAX_REMOTE_DEVICES; i++) {
		struct isci_remote_device *idev = &ihost->devices[i];

		INIT_LIST_HEAD(&idev->node);
	}

	shost = scsi_host_alloc(&isci_sht, sizeof(void *));
	if (!shost)
		return NULL;

	dev_info(&pdev->dev, "%sSCU controller %d: phy 3-0 cables: "
		 "{%s, %s, %s, %s}\n",
		 (is_cable_select_overridden() ? "* " : ""), ihost->id,
		 lookup_cable_names(decode_cable_selection(ihost, 3)),
		 lookup_cable_names(decode_cable_selection(ihost, 2)),
		 lookup_cable_names(decode_cable_selection(ihost, 1)),
		 lookup_cable_names(decode_cable_selection(ihost, 0)));

	err = isci_host_init(ihost);
	if (err)
		goto err_shost;

	SHOST_TO_SAS_HA(shost) = &ihost->sas_ha;
	ihost->sas_ha.core.shost = shost;
	shost->transportt = isci_transport_template;

	shost->max_id = ~0;
	shost->max_lun = ~0;
	shost->max_cmd_len = MAX_COMMAND_SIZE;

	/* turn on DIF support */
	scsi_host_set_prot(shost,
			   SHOST_DIF_TYPE1_PROTECTION |
			   SHOST_DIF_TYPE2_PROTECTION |
			   SHOST_DIF_TYPE3_PROTECTION);
	scsi_host_set_guard(shost, SHOST_DIX_GUARD_CRC);

	err = scsi_add_host(shost, &pdev->dev);
	if (err)
		goto err_shost;

	err = isci_register_sas_ha(ihost);
	if (err)
		goto err_shost_remove;

	return ihost;

 err_shost_remove:
	scsi_remove_host(shost);
 err_shost:
	scsi_host_put(shost);

	return NULL;
}

static int isci_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct isci_pci_info *pci_info;
	int err, i;
	struct isci_host *isci_host;
	const struct firmware *fw = NULL;
	struct isci_orom *orom = NULL;
	char *source = "(platform)";

	dev_info(&pdev->dev, "driver configured for rev: %d silicon\n",
		 pdev->revision);

	pci_info = devm_kzalloc(&pdev->dev, sizeof(*pci_info), GFP_KERNEL);
	if (!pci_info)
		return -ENOMEM;
	pci_set_drvdata(pdev, pci_info);

	if (efi_rt_services_supported(EFI_RT_SUPPORTED_GET_VARIABLE))
		orom = isci_get_efi_var(pdev);

	if (!orom)
		orom = isci_request_oprom(pdev);

	for (i = 0; orom && i < num_controllers(pdev); i++) {
		if (sci_oem_parameters_validate(&orom->ctrl[i],
						orom->hdr.version)) {
			dev_warn(&pdev->dev,
				 "[%d]: invalid oem parameters detected, falling back to firmware\n", i);
			orom = NULL;
			break;
		}
	}

	if (!orom) {
		source = "(firmware)";
		orom = isci_request_firmware(pdev, fw);
		if (!orom) {
			/* TODO convert this to WARN_TAINT_ONCE once the
			 * orom/efi parameter support is widely available
			 */
			dev_warn(&pdev->dev,
				 "Loading user firmware failed, using default "
				 "values\n");
			dev_warn(&pdev->dev,
				 "Default OEM configuration being used: 4 "
				 "narrow ports, and default SAS Addresses\n");
		}
	}

	if (orom)
		dev_info(&pdev->dev,
			 "OEM SAS parameters (version: %u.%u) loaded %s\n",
			 (orom->hdr.version & 0xf0) >> 4,
			 (orom->hdr.version & 0xf), source);

	pci_info->orom = orom;

	err = isci_pci_init(pdev);
	if (err)
		return err;

	for (i = 0; i < num_controllers(pdev); i++) {
		struct isci_host *h = isci_host_alloc(pdev, i);

		if (!h) {
			err = -ENOMEM;
			goto err_host_alloc;
		}
		pci_info->hosts[i] = h;
	}

	err = isci_setup_interrupts(pdev);
	if (err)
		goto err_host_alloc;

	for_each_isci_host(i, isci_host, pdev)
		scsi_scan_host(to_shost(isci_host));

	return 0;

 err_host_alloc:
	for_each_isci_host(i, isci_host, pdev)
		isci_unregister(isci_host);
	return err;
}

static void isci_pci_remove(struct pci_dev *pdev)
{
	struct isci_host *ihost;
	int i;

	for_each_isci_host(i, ihost, pdev) {
		wait_for_start(ihost);
		isci_unregister(ihost);
		isci_host_deinit(ihost);
	}
}

#ifdef CONFIG_PM_SLEEP
static int isci_suspend(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct isci_host *ihost;
	int i;

	for_each_isci_host(i, ihost, pdev) {
		sas_suspend_ha(&ihost->sas_ha);
		isci_host_deinit(ihost);
	}

	return 0;
}

static int isci_resume(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct isci_host *ihost;
	int i;

	for_each_isci_host(i, ihost, pdev) {
		sas_prep_resume_ha(&ihost->sas_ha);

		isci_host_init(ihost);
		isci_host_start(ihost->sas_ha.core.shost);
		wait_for_start(ihost);

		sas_resume_ha(&ihost->sas_ha);
	}

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(isci_pm_ops, isci_suspend, isci_resume);

static struct pci_driver isci_pci_driver = {
	.name		= DRV_NAME,
	.id_table	= isci_id_table,
	.probe		= isci_pci_probe,
	.remove		= isci_pci_remove,
	.driver.pm      = &isci_pm_ops,
};

static __init int isci_init(void)
{
	int err;

	pr_info("%s: Intel(R) C600 SAS Controller Driver - version %s\n",
		DRV_NAME, DRV_VERSION);

	isci_transport_template = sas_domain_attach_transport(&isci_transport_ops);
	if (!isci_transport_template)
		return -ENOMEM;

	err = pci_register_driver(&isci_pci_driver);
	if (err)
		sas_release_transport(isci_transport_template);

	return err;
}

static __exit void isci_exit(void)
{
	pci_unregister_driver(&isci_pci_driver);
	sas_release_transport(isci_transport_template);
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_FIRMWARE(ISCI_FW_NAME);
module_init(isci_init);
module_exit(isci_exit);

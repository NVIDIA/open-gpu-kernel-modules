/*
 * PMC-Sierra 8001/8081/8088/8089 SAS/SATA based host adapters driver
 *
 * Copyright (c) 2008-2009 USI Co., Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */
#include <linux/firmware.h>
#include <linux/slab.h>
#include "pm8001_sas.h"
#include "pm8001_ctl.h"
#include "pm8001_chips.h"

/* scsi host attributes */

/**
 * pm8001_ctl_mpi_interface_rev_show - MPI interface revision number
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_mpi_interface_rev_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id == chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%d\n",
			pm8001_ha->main_cfg_tbl.pm8001_tbl.interface_rev);
	} else {
		return snprintf(buf, PAGE_SIZE, "%d\n",
			pm8001_ha->main_cfg_tbl.pm80xx_tbl.interface_rev);
	}
}
static
DEVICE_ATTR(interface_rev, S_IRUGO, pm8001_ctl_mpi_interface_rev_show, NULL);

/**
 * controller_fatal_error_show - check controller is under fatal err
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read only' shost attribute.
 */
static ssize_t controller_fatal_error_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	return snprintf(buf, PAGE_SIZE, "%d\n",
			pm8001_ha->controller_fatal_error);
}
static DEVICE_ATTR_RO(controller_fatal_error);

/**
 * pm8001_ctl_fw_version_show - firmware version
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_fw_version_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id == chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%02x.%02x.%02x.%02x\n",
		(u8)(pm8001_ha->main_cfg_tbl.pm8001_tbl.firmware_rev >> 24),
		(u8)(pm8001_ha->main_cfg_tbl.pm8001_tbl.firmware_rev >> 16),
		(u8)(pm8001_ha->main_cfg_tbl.pm8001_tbl.firmware_rev >> 8),
		(u8)(pm8001_ha->main_cfg_tbl.pm8001_tbl.firmware_rev));
	} else {
		return snprintf(buf, PAGE_SIZE, "%02x.%02x.%02x.%02x\n",
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.firmware_rev >> 24),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.firmware_rev >> 16),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.firmware_rev >> 8),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.firmware_rev));
	}
}
static DEVICE_ATTR(fw_version, S_IRUGO, pm8001_ctl_fw_version_show, NULL);

/**
 * pm8001_ctl_ila_version_show - ila version
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_ila_version_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id != chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%02x.%02x.%02x.%02x\n",
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.ila_version >> 24),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.ila_version >> 16),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.ila_version >> 8),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.ila_version));
	}
	return 0;
}
static DEVICE_ATTR(ila_version, 0444, pm8001_ctl_ila_version_show, NULL);

/**
 * pm8001_ctl_inactive_fw_version_show - Inacative firmware version number
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_inactive_fw_version_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id != chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%02x.%02x.%02x.%02x\n",
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.inc_fw_version >> 24),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.inc_fw_version >> 16),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.inc_fw_version >> 8),
		(u8)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.inc_fw_version));
	}
	return 0;
}
static
DEVICE_ATTR(inc_fw_ver, 0444, pm8001_ctl_inactive_fw_version_show, NULL);

/**
 * pm8001_ctl_max_out_io_show - max outstanding io supported
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_max_out_io_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id == chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%d\n",
			pm8001_ha->main_cfg_tbl.pm8001_tbl.max_out_io);
	} else {
		return snprintf(buf, PAGE_SIZE, "%d\n",
			pm8001_ha->main_cfg_tbl.pm80xx_tbl.max_out_io);
	}
}
static DEVICE_ATTR(max_out_io, S_IRUGO, pm8001_ctl_max_out_io_show, NULL);
/**
 * pm8001_ctl_max_devices_show - max devices support
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_max_devices_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id == chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%04d\n",
			(u16)(pm8001_ha->main_cfg_tbl.pm8001_tbl.max_sgl >> 16)
			);
	} else {
		return snprintf(buf, PAGE_SIZE, "%04d\n",
			(u16)(pm8001_ha->main_cfg_tbl.pm80xx_tbl.max_sgl >> 16)
			);
	}
}
static DEVICE_ATTR(max_devices, S_IRUGO, pm8001_ctl_max_devices_show, NULL);
/**
 * pm8001_ctl_max_sg_list_show - max sg list supported iff not 0.0 for no
 * hardware limitation
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_max_sg_list_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	if (pm8001_ha->chip_id == chip_8001) {
		return snprintf(buf, PAGE_SIZE, "%04d\n",
			pm8001_ha->main_cfg_tbl.pm8001_tbl.max_sgl & 0x0000FFFF
			);
	} else {
		return snprintf(buf, PAGE_SIZE, "%04d\n",
			pm8001_ha->main_cfg_tbl.pm80xx_tbl.max_sgl & 0x0000FFFF
			);
	}
}
static DEVICE_ATTR(max_sg_list, S_IRUGO, pm8001_ctl_max_sg_list_show, NULL);

#define SAS_1_0 0x1
#define SAS_1_1 0x2
#define SAS_2_0 0x4

static ssize_t
show_sas_spec_support_status(unsigned int mode, char *buf)
{
	ssize_t len = 0;

	if (mode & SAS_1_1)
		len = sprintf(buf, "%s", "SAS1.1");
	if (mode & SAS_2_0)
		len += sprintf(buf + len, "%s%s", len ? ", " : "", "SAS2.0");
	len += sprintf(buf + len, "\n");

	return len;
}

/**
 * pm8001_ctl_sas_spec_support_show - sas spec supported
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_sas_spec_support_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	unsigned int mode;
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	/* fe000000 means supports SAS2.1 */
	if (pm8001_ha->chip_id == chip_8001)
		mode = (pm8001_ha->main_cfg_tbl.pm8001_tbl.ctrl_cap_flag &
							0xfe000000)>>25;
	else
		/* fe000000 means supports SAS2.1 */
		mode = (pm8001_ha->main_cfg_tbl.pm80xx_tbl.ctrl_cap_flag &
							0xfe000000)>>25;
	return show_sas_spec_support_status(mode, buf);
}
static DEVICE_ATTR(sas_spec_support, S_IRUGO,
		   pm8001_ctl_sas_spec_support_show, NULL);

/**
 * pm8001_ctl_host_sas_address_show - sas address
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * This is the controller sas address
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_host_sas_address_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	return snprintf(buf, PAGE_SIZE, "0x%016llx\n",
			be64_to_cpu(*(__be64 *)pm8001_ha->sas_addr));
}
static DEVICE_ATTR(host_sas_address, S_IRUGO,
		   pm8001_ctl_host_sas_address_show, NULL);

/**
 * pm8001_ctl_logging_level_show - logging level
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read/write' shost attribute.
 */
static ssize_t pm8001_ctl_logging_level_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	return snprintf(buf, PAGE_SIZE, "%08xh\n", pm8001_ha->logging_level);
}

static ssize_t pm8001_ctl_logging_level_store(struct device *cdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	int val = 0;

	if (sscanf(buf, "%x", &val) != 1)
		return -EINVAL;

	pm8001_ha->logging_level = val;
	return strlen(buf);
}

static DEVICE_ATTR(logging_level, S_IRUGO | S_IWUSR,
	pm8001_ctl_logging_level_show, pm8001_ctl_logging_level_store);
/**
 * pm8001_ctl_aap_log_show - aap1 event log
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_aap_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	u8 *ptr = (u8 *)pm8001_ha->memoryMap.region[AAP1].virt_ptr;
	int i;

	char *str = buf;
	int max = 2;
	for (i = 0; i < max; i++) {
		str += sprintf(str, "0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x"
			       "0x%08x 0x%08x\n",
			       pm8001_ctl_aap1_memmap(ptr, i, 0),
			       pm8001_ctl_aap1_memmap(ptr, i, 4),
			       pm8001_ctl_aap1_memmap(ptr, i, 8),
			       pm8001_ctl_aap1_memmap(ptr, i, 12),
			       pm8001_ctl_aap1_memmap(ptr, i, 16),
			       pm8001_ctl_aap1_memmap(ptr, i, 20),
			       pm8001_ctl_aap1_memmap(ptr, i, 24),
			       pm8001_ctl_aap1_memmap(ptr, i, 28));
	}

	return str - buf;
}
static DEVICE_ATTR(aap_log, S_IRUGO, pm8001_ctl_aap_log_show, NULL);
/**
 * pm8001_ctl_ib_queue_log_show - Out bound Queue log
 * @cdev:pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_ib_queue_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	int offset;
	char *str = buf;
	int start = 0;
	u32 ib_offset = pm8001_ha->ib_offset;
#define IB_MEMMAP(c)	\
		(*(u32 *)((u8 *)pm8001_ha->	\
		memoryMap.region[ib_offset].virt_ptr +	\
		pm8001_ha->evtlog_ib_offset + (c)))

	for (offset = 0; offset < IB_OB_READ_TIMES; offset++) {
		str += sprintf(str, "0x%08x\n", IB_MEMMAP(start));
		start = start + 4;
	}
	pm8001_ha->evtlog_ib_offset += SYSFS_OFFSET;
	if (((pm8001_ha->evtlog_ib_offset) % (PM80XX_IB_OB_QUEUE_SIZE)) == 0)
		pm8001_ha->evtlog_ib_offset = 0;

	return str - buf;
}

static DEVICE_ATTR(ib_log, S_IRUGO, pm8001_ctl_ib_queue_log_show, NULL);
/**
 * pm8001_ctl_ob_queue_log_show - Out bound Queue log
 * @cdev:pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 * A sysfs 'read-only' shost attribute.
 */

static ssize_t pm8001_ctl_ob_queue_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	int offset;
	char *str = buf;
	int start = 0;
	u32 ob_offset = pm8001_ha->ob_offset;
#define OB_MEMMAP(c)	\
		(*(u32 *)((u8 *)pm8001_ha->	\
		memoryMap.region[ob_offset].virt_ptr +	\
		pm8001_ha->evtlog_ob_offset + (c)))

	for (offset = 0; offset < IB_OB_READ_TIMES; offset++) {
		str += sprintf(str, "0x%08x\n", OB_MEMMAP(start));
		start = start + 4;
	}
	pm8001_ha->evtlog_ob_offset += SYSFS_OFFSET;
	if (((pm8001_ha->evtlog_ob_offset) % (PM80XX_IB_OB_QUEUE_SIZE)) == 0)
		pm8001_ha->evtlog_ob_offset = 0;

	return str - buf;
}
static DEVICE_ATTR(ob_log, S_IRUGO, pm8001_ctl_ob_queue_log_show, NULL);
/**
 * pm8001_ctl_bios_version_show - Bios version Display
 * @cdev:pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf:the buffer returned
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_bios_version_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	char *str = buf;
	int bios_index;
	DECLARE_COMPLETION_ONSTACK(completion);
	struct pm8001_ioctl_payload payload;

	pm8001_ha->nvmd_completion = &completion;
	payload.minor_function = 7;
	payload.offset = 0;
	payload.rd_length = 4096;
	payload.func_specific = kzalloc(4096, GFP_KERNEL);
	if (!payload.func_specific)
		return -ENOMEM;
	if (PM8001_CHIP_DISP->get_nvmd_req(pm8001_ha, &payload)) {
		kfree(payload.func_specific);
		return -ENOMEM;
	}
	wait_for_completion(&completion);
	for (bios_index = BIOSOFFSET; bios_index < BIOS_OFFSET_LIMIT;
		bios_index++)
		str += sprintf(str, "%c",
			*(payload.func_specific+bios_index));
	kfree(payload.func_specific);
	return str - buf;
}
static DEVICE_ATTR(bios_version, S_IRUGO, pm8001_ctl_bios_version_show, NULL);
/**
 * event_log_size_show - event log size
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs read  shost attribute.
 */
static ssize_t event_log_size_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	return snprintf(buf, PAGE_SIZE, "%d\n",
		pm8001_ha->main_cfg_tbl.pm80xx_tbl.event_log_size);
}
static DEVICE_ATTR_RO(event_log_size);
/**
 * pm8001_ctl_iop_log_show - IOP event log
 * @cdev: pointer to embedded class device
 * @attr: device attribute (unused)
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */
static ssize_t pm8001_ctl_iop_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	char *str = buf;
	u32 read_size =
		pm8001_ha->main_cfg_tbl.pm80xx_tbl.event_log_size / 1024;
	static u32 start, end, count;
	u32 max_read_times = 32;
	u32 max_count = (read_size * 1024) / (max_read_times * 4);
	u32 *temp = (u32 *)pm8001_ha->memoryMap.region[IOP].virt_ptr;

	if ((count % max_count) == 0) {
		start = 0;
		end = max_read_times;
		count = 0;
	} else {
		start = end;
		end = end + max_read_times;
	}

	for (; start < end; start++)
		str += sprintf(str, "%08x ", *(temp+start));
	count++;
	return str - buf;
}
static DEVICE_ATTR(iop_log, S_IRUGO, pm8001_ctl_iop_log_show, NULL);

/**
 ** pm8001_ctl_fatal_log_show - fatal error logging
 ** @cdev:pointer to embedded class device
 ** @attr: device attribute
 ** @buf: the buffer returned
 **
 ** A sysfs 'read-only' shost attribute.
 **/

static ssize_t pm8001_ctl_fatal_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	ssize_t count;

	count = pm80xx_get_fatal_dump(cdev, attr, buf);
	return count;
}

static DEVICE_ATTR(fatal_log, S_IRUGO, pm8001_ctl_fatal_log_show, NULL);

/**
 ** non_fatal_log_show - non fatal error logging
 ** @cdev:pointer to embedded class device
 ** @attr: device attribute
 ** @buf: the buffer returned
 **
 ** A sysfs 'read-only' shost attribute.
 **/
static ssize_t non_fatal_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	u32 count;

	count = pm80xx_get_non_fatal_dump(cdev, attr, buf);
	return count;
}
static DEVICE_ATTR_RO(non_fatal_log);

static ssize_t non_fatal_count_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	return snprintf(buf, PAGE_SIZE, "%08x",
			pm8001_ha->non_fatal_count);
}

static ssize_t non_fatal_count_store(struct device *cdev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	int val = 0;

	if (kstrtoint(buf, 16, &val) != 0)
		return -EINVAL;

	pm8001_ha->non_fatal_count = val;
	return strlen(buf);
}
static DEVICE_ATTR_RW(non_fatal_count);

/**
 ** pm8001_ctl_gsm_log_show - gsm dump collection
 ** @cdev:pointer to embedded class device
 ** @attr: device attribute (unused)
 ** @buf: the buffer returned
 ** A sysfs 'read-only' shost attribute.
 **/
static ssize_t pm8001_ctl_gsm_log_show(struct device *cdev,
	struct device_attribute *attr, char *buf)
{
	ssize_t count;

	count = pm8001_get_gsm_dump(cdev, SYSFS_OFFSET, buf);
	return count;
}

static DEVICE_ATTR(gsm_log, S_IRUGO, pm8001_ctl_gsm_log_show, NULL);

#define FLASH_CMD_NONE      0x00
#define FLASH_CMD_UPDATE    0x01
#define FLASH_CMD_SET_NVMD    0x02

struct flash_command {
     u8      command[8];
     int     code;
};

static const struct flash_command flash_command_table[] = {
     {"set_nvmd",    FLASH_CMD_SET_NVMD},
     {"update",      FLASH_CMD_UPDATE},
     {"",            FLASH_CMD_NONE} /* Last entry should be NULL. */
};

struct error_fw {
     char    *reason;
     int     err_code;
};

static const struct error_fw flash_error_table[] = {
     {"Failed to open fw image file",	FAIL_OPEN_BIOS_FILE},
     {"image header mismatch",		FLASH_UPDATE_HDR_ERR},
     {"image offset mismatch",		FLASH_UPDATE_OFFSET_ERR},
     {"image CRC Error",		FLASH_UPDATE_CRC_ERR},
     {"image length Error.",		FLASH_UPDATE_LENGTH_ERR},
     {"Failed to program flash chip",	FLASH_UPDATE_HW_ERR},
     {"Flash chip not supported.",	FLASH_UPDATE_DNLD_NOT_SUPPORTED},
     {"Flash update disabled.",		FLASH_UPDATE_DISABLED},
     {"Flash in progress",		FLASH_IN_PROGRESS},
     {"Image file size Error",		FAIL_FILE_SIZE},
     {"Input parameter error",		FAIL_PARAMETERS},
     {"Out of memory",			FAIL_OUT_MEMORY},
     {"OK", 0}	/* Last entry err_code = 0. */
};

static int pm8001_set_nvmd(struct pm8001_hba_info *pm8001_ha)
{
	struct pm8001_ioctl_payload	*payload;
	DECLARE_COMPLETION_ONSTACK(completion);
	u8		*ioctlbuffer;
	u32		ret;
	u32		length = 1024 * 5 + sizeof(*payload) - 1;

	if (pm8001_ha->fw_image->size > 4096) {
		pm8001_ha->fw_status = FAIL_FILE_SIZE;
		return -EFAULT;
	}

	ioctlbuffer = kzalloc(length, GFP_KERNEL);
	if (!ioctlbuffer) {
		pm8001_ha->fw_status = FAIL_OUT_MEMORY;
		return -ENOMEM;
	}
	payload = (struct pm8001_ioctl_payload *)ioctlbuffer;
	memcpy((u8 *)&payload->func_specific, (u8 *)pm8001_ha->fw_image->data,
				pm8001_ha->fw_image->size);
	payload->wr_length = pm8001_ha->fw_image->size;
	payload->id = 0;
	payload->minor_function = 0x1;
	pm8001_ha->nvmd_completion = &completion;
	ret = PM8001_CHIP_DISP->set_nvmd_req(pm8001_ha, payload);
	if (ret) {
		pm8001_ha->fw_status = FAIL_OUT_MEMORY;
		goto out;
	}
	wait_for_completion(&completion);
out:
	kfree(ioctlbuffer);
	return ret;
}

static int pm8001_update_flash(struct pm8001_hba_info *pm8001_ha)
{
	struct pm8001_ioctl_payload	*payload;
	DECLARE_COMPLETION_ONSTACK(completion);
	u8		*ioctlbuffer;
	struct fw_control_info	*fwControl;
	u32		partitionSize, partitionSizeTmp;
	u32		loopNumber, loopcount;
	struct pm8001_fw_image_header *image_hdr;
	u32		sizeRead = 0;
	u32		ret = 0;
	u32		length = 1024 * 16 + sizeof(*payload) - 1;

	if (pm8001_ha->fw_image->size < 28) {
		pm8001_ha->fw_status = FAIL_FILE_SIZE;
		return -EFAULT;
	}
	ioctlbuffer = kzalloc(length, GFP_KERNEL);
	if (!ioctlbuffer) {
		pm8001_ha->fw_status = FAIL_OUT_MEMORY;
		return -ENOMEM;
	}
	image_hdr = (struct pm8001_fw_image_header *)pm8001_ha->fw_image->data;
	while (sizeRead < pm8001_ha->fw_image->size) {
		partitionSizeTmp =
			*(u32 *)((u8 *)&image_hdr->image_length + sizeRead);
		partitionSize = be32_to_cpu(partitionSizeTmp);
		loopcount = DIV_ROUND_UP(partitionSize + HEADER_LEN,
					IOCTL_BUF_SIZE);
		for (loopNumber = 0; loopNumber < loopcount; loopNumber++) {
			payload = (struct pm8001_ioctl_payload *)ioctlbuffer;
			payload->wr_length = 1024*16;
			payload->id = 0;
			fwControl =
			      (struct fw_control_info *)&payload->func_specific;
			fwControl->len = IOCTL_BUF_SIZE;   /* IN */
			fwControl->size = partitionSize + HEADER_LEN;/* IN */
			fwControl->retcode = 0;/* OUT */
			fwControl->offset = loopNumber * IOCTL_BUF_SIZE;/*OUT */

		/* for the last chunk of data in case file size is not even with
		4k, load only the rest*/
		if (((loopcount-loopNumber) == 1) &&
			((partitionSize + HEADER_LEN) % IOCTL_BUF_SIZE)) {
			fwControl->len =
				(partitionSize + HEADER_LEN) % IOCTL_BUF_SIZE;
			memcpy((u8 *)fwControl->buffer,
				(u8 *)pm8001_ha->fw_image->data + sizeRead,
				(partitionSize + HEADER_LEN) % IOCTL_BUF_SIZE);
			sizeRead +=
				(partitionSize + HEADER_LEN) % IOCTL_BUF_SIZE;
		} else {
			memcpy((u8 *)fwControl->buffer,
				(u8 *)pm8001_ha->fw_image->data + sizeRead,
				IOCTL_BUF_SIZE);
			sizeRead += IOCTL_BUF_SIZE;
		}

		pm8001_ha->nvmd_completion = &completion;
		ret = PM8001_CHIP_DISP->fw_flash_update_req(pm8001_ha, payload);
		if (ret) {
			pm8001_ha->fw_status = FAIL_OUT_MEMORY;
			goto out;
		}
		wait_for_completion(&completion);
		if (fwControl->retcode > FLASH_UPDATE_IN_PROGRESS) {
			pm8001_ha->fw_status = fwControl->retcode;
			ret = -EFAULT;
			goto out;
		}
		}
	}
out:
	kfree(ioctlbuffer);
	return ret;
}
static ssize_t pm8001_store_update_fw(struct device *cdev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	char *cmd_ptr, *filename_ptr;
	int res, i;
	int flash_command = FLASH_CMD_NONE;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	/* this test protects us from running two flash processes at once,
	 * so we should start with this test */
	if (pm8001_ha->fw_status == FLASH_IN_PROGRESS)
		return -EINPROGRESS;
	pm8001_ha->fw_status = FLASH_IN_PROGRESS;

	cmd_ptr = kcalloc(count, 2, GFP_KERNEL);
	if (!cmd_ptr) {
		pm8001_ha->fw_status = FAIL_OUT_MEMORY;
		return -ENOMEM;
	}

	filename_ptr = cmd_ptr + count;
	res = sscanf(buf, "%s %s", cmd_ptr, filename_ptr);
	if (res != 2) {
		pm8001_ha->fw_status = FAIL_PARAMETERS;
		ret = -EINVAL;
		goto out;
	}

	for (i = 0; flash_command_table[i].code != FLASH_CMD_NONE; i++) {
		if (!memcmp(flash_command_table[i].command,
				 cmd_ptr, strlen(cmd_ptr))) {
			flash_command = flash_command_table[i].code;
			break;
		}
	}
	if (flash_command == FLASH_CMD_NONE) {
		pm8001_ha->fw_status = FAIL_PARAMETERS;
		ret = -EINVAL;
		goto out;
	}

	ret = request_firmware(&pm8001_ha->fw_image,
			       filename_ptr,
			       pm8001_ha->dev);

	if (ret) {
		pm8001_dbg(pm8001_ha, FAIL,
			   "Failed to load firmware image file %s, error %d\n",
			   filename_ptr, ret);
		pm8001_ha->fw_status = FAIL_OPEN_BIOS_FILE;
		goto out;
	}

	if (FLASH_CMD_UPDATE == flash_command)
		ret = pm8001_update_flash(pm8001_ha);
	else
		ret = pm8001_set_nvmd(pm8001_ha);

	release_firmware(pm8001_ha->fw_image);
out:
	kfree(cmd_ptr);

	if (ret)
		return ret;

	pm8001_ha->fw_status = FLASH_OK;
	return count;
}

static ssize_t pm8001_show_update_fw(struct device *cdev,
				     struct device_attribute *attr, char *buf)
{
	int i;
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;

	for (i = 0; flash_error_table[i].err_code != 0; i++) {
		if (flash_error_table[i].err_code == pm8001_ha->fw_status)
			break;
	}
	if (pm8001_ha->fw_status != FLASH_IN_PROGRESS)
		pm8001_ha->fw_status = FLASH_OK;

	return snprintf(buf, PAGE_SIZE, "status=%x %s\n",
			flash_error_table[i].err_code,
			flash_error_table[i].reason);
}
static DEVICE_ATTR(update_fw, S_IRUGO|S_IWUSR|S_IWGRP,
	pm8001_show_update_fw, pm8001_store_update_fw);

/**
 * ctl_mpi_state_show - controller MPI state check
 * @cdev: pointer to embedded class device
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */

static const char *const mpiStateText[] = {
	"MPI is not initialized",
	"MPI is successfully initialized",
	"MPI termination is in progress",
	"MPI initialization failed with error in [31:16]"
};

static ssize_t ctl_mpi_state_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	unsigned int mpidw0;

	mpidw0 = pm8001_mr32(pm8001_ha->general_stat_tbl_addr, 0);
	return sysfs_emit(buf, "%s\n", mpiStateText[mpidw0 & 0x0003]);
}
static DEVICE_ATTR_RO(ctl_mpi_state);

/**
 * ctl_hmi_error_show - controller MPI initialization fails
 * @cdev: pointer to embedded class device
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */

static ssize_t ctl_hmi_error_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	unsigned int mpidw0;

	mpidw0 = pm8001_mr32(pm8001_ha->general_stat_tbl_addr, 0);
	return sysfs_emit(buf, "0x%08x\n", (mpidw0 >> 16));
}
static DEVICE_ATTR_RO(ctl_hmi_error);

/**
 * ctl_raae_count_show - controller raae count check
 * @cdev: pointer to embedded class device
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */

static ssize_t ctl_raae_count_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	unsigned int raaecnt;

	raaecnt = pm8001_mr32(pm8001_ha->general_stat_tbl_addr, 12);
	return sysfs_emit(buf, "0x%08x\n", raaecnt);
}
static DEVICE_ATTR_RO(ctl_raae_count);

/**
 * ctl_iop0_count_show - controller iop0 count check
 * @cdev: pointer to embedded class device
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */

static ssize_t ctl_iop0_count_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	unsigned int iop0cnt;

	iop0cnt = pm8001_mr32(pm8001_ha->general_stat_tbl_addr, 16);
	return sysfs_emit(buf, "0x%08x\n", iop0cnt);
}
static DEVICE_ATTR_RO(ctl_iop0_count);

/**
 * ctl_iop1_count_show - controller iop1 count check
 * @cdev: pointer to embedded class device
 * @buf: the buffer returned
 *
 * A sysfs 'read-only' shost attribute.
 */

static ssize_t ctl_iop1_count_show(struct device *cdev,
		struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(cdev);
	struct sas_ha_struct *sha = SHOST_TO_SAS_HA(shost);
	struct pm8001_hba_info *pm8001_ha = sha->lldd_ha;
	unsigned int iop1cnt;

	iop1cnt = pm8001_mr32(pm8001_ha->general_stat_tbl_addr, 20);
	return sysfs_emit(buf, "0x%08x\n", iop1cnt);

}
static DEVICE_ATTR_RO(ctl_iop1_count);

struct device_attribute *pm8001_host_attrs[] = {
	&dev_attr_interface_rev,
	&dev_attr_controller_fatal_error,
	&dev_attr_fw_version,
	&dev_attr_update_fw,
	&dev_attr_aap_log,
	&dev_attr_iop_log,
	&dev_attr_fatal_log,
	&dev_attr_non_fatal_log,
	&dev_attr_non_fatal_count,
	&dev_attr_gsm_log,
	&dev_attr_max_out_io,
	&dev_attr_max_devices,
	&dev_attr_max_sg_list,
	&dev_attr_sas_spec_support,
	&dev_attr_logging_level,
	&dev_attr_event_log_size,
	&dev_attr_host_sas_address,
	&dev_attr_bios_version,
	&dev_attr_ib_log,
	&dev_attr_ob_log,
	&dev_attr_ila_version,
	&dev_attr_inc_fw_ver,
	&dev_attr_ctl_mpi_state,
	&dev_attr_ctl_hmi_error,
	&dev_attr_ctl_raae_count,
	&dev_attr_ctl_iop0_count,
	&dev_attr_ctl_iop1_count,
	NULL,
};


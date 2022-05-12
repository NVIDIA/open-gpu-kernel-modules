// SPDX-License-Identifier: GPL-2.0-only
/*
 * QLogic qlcnic NIC Driver
 * Copyright (c) 2009-2013 QLogic Corporation
 */

#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/swab.h>
#include <linux/dma-mapping.h>
#include <net/ip.h>
#include <linux/ipv6.h>
#include <linux/inetdevice.h>
#include <linux/sysfs.h>
#include <linux/aer.h>
#include <linux/log2.h>
#ifdef CONFIG_QLCNIC_HWMON
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#endif

#include "qlcnic.h"
#include "qlcnic_hw.h"

int qlcnicvf_config_bridged_mode(struct qlcnic_adapter *adapter, u32 enable)
{
	return -EOPNOTSUPP;
}

int qlcnicvf_config_led(struct qlcnic_adapter *adapter, u32 state, u32 rate)
{
	return -EOPNOTSUPP;
}

static ssize_t qlcnic_store_bridged_mode(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t len)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	unsigned long new;
	int ret = -EINVAL;

	if (!(adapter->ahw->capabilities & QLCNIC_FW_CAPABILITY_BDG))
		goto err_out;

	if (!test_bit(__QLCNIC_DEV_UP, &adapter->state))
		goto err_out;

	if (kstrtoul(buf, 2, &new))
		goto err_out;

	if (!qlcnic_config_bridged_mode(adapter, !!new))
		ret = len;

err_out:
	return ret;
}

static ssize_t qlcnic_show_bridged_mode(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	int bridged_mode = 0;

	if (adapter->ahw->capabilities & QLCNIC_FW_CAPABILITY_BDG)
		bridged_mode = !!(adapter->flags & QLCNIC_BRIDGE_ENABLED);

	return sprintf(buf, "%d\n", bridged_mode);
}

static ssize_t qlcnic_store_diag_mode(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t len)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	unsigned long new;

	if (kstrtoul(buf, 2, &new))
		return -EINVAL;

	if (!!new != !!(adapter->flags & QLCNIC_DIAG_ENABLED))
		adapter->flags ^= QLCNIC_DIAG_ENABLED;

	return len;
}

static ssize_t qlcnic_show_diag_mode(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", !!(adapter->flags & QLCNIC_DIAG_ENABLED));
}

static int qlcnic_validate_beacon(struct qlcnic_adapter *adapter, u16 beacon,
				  u8 *state, u8 *rate)
{
	*rate = LSB(beacon);
	*state = MSB(beacon);

	QLCDB(adapter, DRV, "rate %x state %x\n", *rate, *state);

	if (!*state) {
		*rate = __QLCNIC_MAX_LED_RATE;
		return 0;
	} else if (*state > __QLCNIC_MAX_LED_STATE) {
		return -EINVAL;
	}

	if ((!*rate) || (*rate > __QLCNIC_MAX_LED_RATE))
		return -EINVAL;

	return 0;
}

static int qlcnic_83xx_store_beacon(struct qlcnic_adapter *adapter,
				    const char *buf, size_t len)
{
	struct qlcnic_hardware_context *ahw = adapter->ahw;
	unsigned long h_beacon;
	int err;

	if (test_bit(__QLCNIC_RESETTING, &adapter->state))
		return -EIO;

	if (kstrtoul(buf, 2, &h_beacon))
		return -EINVAL;

	qlcnic_get_beacon_state(adapter);

	if (ahw->beacon_state == h_beacon)
		return len;

	rtnl_lock();
	if (!ahw->beacon_state) {
		if (test_and_set_bit(__QLCNIC_LED_ENABLE, &adapter->state)) {
			rtnl_unlock();
			return -EBUSY;
		}
	}

	if (h_beacon)
		err = qlcnic_83xx_config_led(adapter, 1, h_beacon);
	else
		err = qlcnic_83xx_config_led(adapter, 0, !h_beacon);
	if (!err)
		ahw->beacon_state = h_beacon;

	if (!ahw->beacon_state)
		clear_bit(__QLCNIC_LED_ENABLE, &adapter->state);

	rtnl_unlock();
	return len;
}

static int qlcnic_82xx_store_beacon(struct qlcnic_adapter *adapter,
				    const char *buf, size_t len)
{
	struct qlcnic_hardware_context *ahw = adapter->ahw;
	int err, drv_sds_rings = adapter->drv_sds_rings;
	u16 beacon;
	u8 b_state, b_rate;

	if (len != sizeof(u16))
		return -EINVAL;

	memcpy(&beacon, buf, sizeof(u16));
	err = qlcnic_validate_beacon(adapter, beacon, &b_state, &b_rate);
	if (err)
		return err;

	qlcnic_get_beacon_state(adapter);

	if (ahw->beacon_state == b_state)
		return len;

	rtnl_lock();
	if (!ahw->beacon_state) {
		if (test_and_set_bit(__QLCNIC_LED_ENABLE, &adapter->state)) {
			rtnl_unlock();
			return -EBUSY;
		}
	}

	if (test_bit(__QLCNIC_RESETTING, &adapter->state)) {
		err = -EIO;
		goto out;
	}

	if (!test_bit(__QLCNIC_DEV_UP, &adapter->state)) {
		err = qlcnic_diag_alloc_res(adapter->netdev, QLCNIC_LED_TEST);
		if (err)
			goto out;
		set_bit(__QLCNIC_DIAG_RES_ALLOC, &adapter->state);
	}

	err = qlcnic_config_led(adapter, b_state, b_rate);
	if (!err) {
		err = len;
		ahw->beacon_state = b_state;
	}

	if (test_and_clear_bit(__QLCNIC_DIAG_RES_ALLOC, &adapter->state))
		qlcnic_diag_free_res(adapter->netdev, drv_sds_rings);

out:
	if (!ahw->beacon_state)
		clear_bit(__QLCNIC_LED_ENABLE, &adapter->state);
	rtnl_unlock();

	return err;
}

static ssize_t qlcnic_store_beacon(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t len)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	int err = 0;

	if (adapter->ahw->op_mode == QLCNIC_NON_PRIV_FUNC) {
		dev_warn(dev,
			 "LED test not supported in non privileged mode\n");
		return -EOPNOTSUPP;
	}

	if (qlcnic_82xx_check(adapter))
		err = qlcnic_82xx_store_beacon(adapter, buf, len);
	else if (qlcnic_83xx_check(adapter))
		err = qlcnic_83xx_store_beacon(adapter, buf, len);
	else
		return -EIO;

	return err;
}

static ssize_t qlcnic_show_beacon(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", adapter->ahw->beacon_state);
}

static int qlcnic_sysfs_validate_crb(struct qlcnic_adapter *adapter,
				     loff_t offset, size_t size)
{
	size_t crb_size = 4;

	if (!(adapter->flags & QLCNIC_DIAG_ENABLED))
		return -EIO;

	if (offset < QLCNIC_PCI_CRBSPACE) {
		if (ADDR_IN_RANGE(offset, QLCNIC_PCI_CAMQM,
				  QLCNIC_PCI_CAMQM_END))
			crb_size = 8;
		else
			return -EINVAL;
	}

	if ((size != crb_size) || (offset & (crb_size-1)))
		return  -EINVAL;

	return 0;
}

static ssize_t qlcnic_sysfs_read_crb(struct file *filp, struct kobject *kobj,
				     struct bin_attribute *attr, char *buf,
				     loff_t offset, size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	int ret;

	ret = qlcnic_sysfs_validate_crb(adapter, offset, size);
	if (ret != 0)
		return ret;
	qlcnic_read_crb(adapter, buf, offset, size);
	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));

	return size;
}

static ssize_t qlcnic_sysfs_write_crb(struct file *filp, struct kobject *kobj,
				      struct bin_attribute *attr, char *buf,
				      loff_t offset, size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	int ret;

	ret = qlcnic_sysfs_validate_crb(adapter, offset, size);
	if (ret != 0)
		return ret;

	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	qlcnic_write_crb(adapter, buf, offset, size);
	return size;
}

static int qlcnic_sysfs_validate_mem(struct qlcnic_adapter *adapter,
				     loff_t offset, size_t size)
{
	if (!(adapter->flags & QLCNIC_DIAG_ENABLED))
		return -EIO;

	if ((size != 8) || (offset & 0x7))
		return  -EIO;

	return 0;
}

static ssize_t qlcnic_sysfs_read_mem(struct file *filp, struct kobject *kobj,
				     struct bin_attribute *attr, char *buf,
				     loff_t offset, size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	u64 data;
	int ret;

	ret = qlcnic_sysfs_validate_mem(adapter, offset, size);
	if (ret != 0)
		return ret;

	if (qlcnic_pci_mem_read_2M(adapter, offset, &data))
		return -EIO;

	memcpy(buf, &data, size);
	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));

	return size;
}

static ssize_t qlcnic_sysfs_write_mem(struct file *filp, struct kobject *kobj,
				      struct bin_attribute *attr, char *buf,
				      loff_t offset, size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	u64 data;
	int ret;

	ret = qlcnic_sysfs_validate_mem(adapter, offset, size);
	if (ret != 0)
		return ret;

	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	memcpy(&data, buf, size);

	if (qlcnic_pci_mem_write_2M(adapter, offset, data))
		return -EIO;

	return size;
}

int qlcnic_is_valid_nic_func(struct qlcnic_adapter *adapter, u8 pci_func)
{
	int i;

	for (i = 0; i < adapter->ahw->total_nic_func; i++) {
		if (adapter->npars[i].pci_func == pci_func)
			return i;
	}

	dev_err(&adapter->pdev->dev, "%s: Invalid nic function\n", __func__);
	return -EINVAL;
}

static int validate_pm_config(struct qlcnic_adapter *adapter,
			      struct qlcnic_pm_func_cfg *pm_cfg, int count)
{
	u8 src_pci_func, s_esw_id, d_esw_id;
	u8 dest_pci_func;
	int i, src_index, dest_index;

	for (i = 0; i < count; i++) {
		src_pci_func = pm_cfg[i].pci_func;
		dest_pci_func = pm_cfg[i].dest_npar;
		src_index = qlcnic_is_valid_nic_func(adapter, src_pci_func);
		if (src_index < 0)
			return -EINVAL;

		dest_index = qlcnic_is_valid_nic_func(adapter, dest_pci_func);
		if (dest_index < 0)
			return -EINVAL;

		s_esw_id = adapter->npars[src_index].phy_port;
		d_esw_id = adapter->npars[dest_index].phy_port;

		if (s_esw_id != d_esw_id)
			return -EINVAL;
	}

	return 0;
}

static ssize_t qlcnic_sysfs_write_pm_config(struct file *filp,
					    struct kobject *kobj,
					    struct bin_attribute *attr,
					    char *buf, loff_t offset,
					    size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_pm_func_cfg *pm_cfg;
	u32 id, action, pci_func;
	int count, rem, i, ret, index;

	count	= size / sizeof(struct qlcnic_pm_func_cfg);
	rem	= size % sizeof(struct qlcnic_pm_func_cfg);
	if (rem)
		return -EINVAL;

	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	pm_cfg = (struct qlcnic_pm_func_cfg *)buf;
	ret = validate_pm_config(adapter, pm_cfg, count);

	if (ret)
		return ret;
	for (i = 0; i < count; i++) {
		pci_func = pm_cfg[i].pci_func;
		action = !!pm_cfg[i].action;
		index = qlcnic_is_valid_nic_func(adapter, pci_func);
		if (index < 0)
			return -EINVAL;

		id = adapter->npars[index].phy_port;
		ret = qlcnic_config_port_mirroring(adapter, id,
						   action, pci_func);
		if (ret)
			return ret;
	}

	for (i = 0; i < count; i++) {
		pci_func = pm_cfg[i].pci_func;
		index = qlcnic_is_valid_nic_func(adapter, pci_func);
		if (index < 0)
			return -EINVAL;
		id = adapter->npars[index].phy_port;
		adapter->npars[index].enable_pm = !!pm_cfg[i].action;
		adapter->npars[index].dest_npar = id;
	}

	return size;
}

static ssize_t qlcnic_sysfs_read_pm_config(struct file *filp,
					   struct kobject *kobj,
					   struct bin_attribute *attr,
					   char *buf, loff_t offset,
					   size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_pm_func_cfg *pm_cfg;
	u8 pci_func;
	u32 count;
	int i;

	memset(buf, 0, size);
	pm_cfg = (struct qlcnic_pm_func_cfg *)buf;
	count = size / sizeof(struct qlcnic_pm_func_cfg);
	for (i = 0; i < adapter->ahw->total_nic_func; i++) {
		pci_func = adapter->npars[i].pci_func;
		if (pci_func >= count) {
			dev_dbg(dev, "%s: Total nic functions[%d], App sent function count[%d]\n",
				__func__, adapter->ahw->total_nic_func, count);
			continue;
		}
		if (!adapter->npars[i].eswitch_status)
			continue;

		pm_cfg[pci_func].action = adapter->npars[i].enable_pm;
		pm_cfg[pci_func].dest_npar = 0;
		pm_cfg[pci_func].pci_func = i;
	}
	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	return size;
}

static int validate_esw_config(struct qlcnic_adapter *adapter,
			       struct qlcnic_esw_func_cfg *esw_cfg, int count)
{
	struct qlcnic_hardware_context *ahw = adapter->ahw;
	int i, ret;
	u32 op_mode;
	u8 pci_func;

	if (qlcnic_82xx_check(adapter))
		op_mode = readl(ahw->pci_base0 + QLCNIC_DRV_OP_MODE);
	else
		op_mode = QLCRDX(ahw, QLC_83XX_DRV_OP_MODE);

	for (i = 0; i < count; i++) {
		pci_func = esw_cfg[i].pci_func;
		if (pci_func >= ahw->max_vnic_func)
			return -EINVAL;

		if (adapter->ahw->op_mode == QLCNIC_MGMT_FUNC)
			if (qlcnic_is_valid_nic_func(adapter, pci_func) < 0)
				return -EINVAL;

		switch (esw_cfg[i].op_mode) {
		case QLCNIC_PORT_DEFAULTS:
			if (qlcnic_82xx_check(adapter)) {
				ret = QLC_DEV_GET_DRV(op_mode, pci_func);
			} else {
				ret = QLC_83XX_GET_FUNC_PRIVILEGE(op_mode,
								  pci_func);
				esw_cfg[i].offload_flags = 0;
			}

			if (ret != QLCNIC_NON_PRIV_FUNC) {
				if (esw_cfg[i].mac_anti_spoof != 0)
					return -EINVAL;
				if (esw_cfg[i].mac_override != 1)
					return -EINVAL;
				if (esw_cfg[i].promisc_mode != 1)
					return -EINVAL;
			}
			break;
		case QLCNIC_ADD_VLAN:
			if (!IS_VALID_VLAN(esw_cfg[i].vlan_id))
				return -EINVAL;
			if (!esw_cfg[i].op_type)
				return -EINVAL;
			break;
		case QLCNIC_DEL_VLAN:
			if (!esw_cfg[i].op_type)
				return -EINVAL;
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

static ssize_t qlcnic_sysfs_write_esw_config(struct file *file,
					     struct kobject *kobj,
					     struct bin_attribute *attr,
					     char *buf, loff_t offset,
					     size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_esw_func_cfg *esw_cfg;
	struct qlcnic_npar_info *npar;
	int count, rem, i, ret;
	int index;
	u8 op_mode = 0, pci_func;

	count	= size / sizeof(struct qlcnic_esw_func_cfg);
	rem	= size % sizeof(struct qlcnic_esw_func_cfg);
	if (rem)
		return -EINVAL;

	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	esw_cfg = (struct qlcnic_esw_func_cfg *)buf;
	ret = validate_esw_config(adapter, esw_cfg, count);
	if (ret)
		return ret;

	for (i = 0; i < count; i++) {
		if (adapter->ahw->op_mode == QLCNIC_MGMT_FUNC)
			if (qlcnic_config_switch_port(adapter, &esw_cfg[i]))
				return -EINVAL;

		if (adapter->ahw->pci_func != esw_cfg[i].pci_func)
			continue;

		op_mode = esw_cfg[i].op_mode;
		qlcnic_get_eswitch_port_config(adapter, &esw_cfg[i]);
		esw_cfg[i].op_mode = op_mode;
		esw_cfg[i].pci_func = adapter->ahw->pci_func;

		switch (esw_cfg[i].op_mode) {
		case QLCNIC_PORT_DEFAULTS:
			qlcnic_set_eswitch_port_features(adapter, &esw_cfg[i]);
			rtnl_lock();
			qlcnic_set_netdev_features(adapter, &esw_cfg[i]);
			rtnl_unlock();
			break;
		case QLCNIC_ADD_VLAN:
			qlcnic_set_vlan_config(adapter, &esw_cfg[i]);
			break;
		case QLCNIC_DEL_VLAN:
			esw_cfg[i].vlan_id = 0;
			qlcnic_set_vlan_config(adapter, &esw_cfg[i]);
			break;
		}
	}

	if (adapter->ahw->op_mode != QLCNIC_MGMT_FUNC)
		goto out;

	for (i = 0; i < count; i++) {
		pci_func = esw_cfg[i].pci_func;
		index = qlcnic_is_valid_nic_func(adapter, pci_func);
		if (index < 0)
			return -EINVAL;
		npar = &adapter->npars[index];
		switch (esw_cfg[i].op_mode) {
		case QLCNIC_PORT_DEFAULTS:
			npar->promisc_mode = esw_cfg[i].promisc_mode;
			npar->mac_override = esw_cfg[i].mac_override;
			npar->offload_flags = esw_cfg[i].offload_flags;
			npar->mac_anti_spoof = esw_cfg[i].mac_anti_spoof;
			npar->discard_tagged = esw_cfg[i].discard_tagged;
			break;
		case QLCNIC_ADD_VLAN:
			npar->pvid = esw_cfg[i].vlan_id;
			break;
		case QLCNIC_DEL_VLAN:
			npar->pvid = 0;
			break;
		}
	}
out:
	return size;
}

static ssize_t qlcnic_sysfs_read_esw_config(struct file *file,
					    struct kobject *kobj,
					    struct bin_attribute *attr,
					    char *buf, loff_t offset,
					    size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_esw_func_cfg *esw_cfg;
	u8 pci_func;
	u32 count;
	int i;

	memset(buf, 0, size);
	esw_cfg = (struct qlcnic_esw_func_cfg *)buf;
	count = size / sizeof(struct qlcnic_esw_func_cfg);
	for (i = 0; i < adapter->ahw->total_nic_func; i++) {
		pci_func = adapter->npars[i].pci_func;
		if (pci_func >= count) {
			dev_dbg(dev, "%s: Total nic functions[%d], App sent function count[%d]\n",
				__func__, adapter->ahw->total_nic_func, count);
			continue;
		}
		if (!adapter->npars[i].eswitch_status)
			continue;

		esw_cfg[pci_func].pci_func = pci_func;
		if (qlcnic_get_eswitch_port_config(adapter, &esw_cfg[pci_func]))
			return -EINVAL;
	}
	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	return size;
}

static int validate_npar_config(struct qlcnic_adapter *adapter,
				struct qlcnic_npar_func_cfg *np_cfg,
				int count)
{
	u8 pci_func, i;

	for (i = 0; i < count; i++) {
		pci_func = np_cfg[i].pci_func;
		if (qlcnic_is_valid_nic_func(adapter, pci_func) < 0)
			return -EINVAL;

		if (!IS_VALID_BW(np_cfg[i].min_bw) ||
		    !IS_VALID_BW(np_cfg[i].max_bw))
			return -EINVAL;
	}
	return 0;
}

static ssize_t qlcnic_sysfs_write_npar_config(struct file *file,
					      struct kobject *kobj,
					      struct bin_attribute *attr,
					      char *buf, loff_t offset,
					      size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_info nic_info;
	struct qlcnic_npar_func_cfg *np_cfg;
	int i, count, rem, ret, index;
	u8 pci_func;

	count	= size / sizeof(struct qlcnic_npar_func_cfg);
	rem	= size % sizeof(struct qlcnic_npar_func_cfg);
	if (rem)
		return -EINVAL;

	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	np_cfg = (struct qlcnic_npar_func_cfg *)buf;
	ret = validate_npar_config(adapter, np_cfg, count);
	if (ret)
		return ret;

	for (i = 0; i < count; i++) {
		pci_func = np_cfg[i].pci_func;

		memset(&nic_info, 0, sizeof(struct qlcnic_info));
		ret = qlcnic_get_nic_info(adapter, &nic_info, pci_func);
		if (ret)
			return ret;
		nic_info.pci_func = pci_func;
		nic_info.min_tx_bw = np_cfg[i].min_bw;
		nic_info.max_tx_bw = np_cfg[i].max_bw;
		ret = qlcnic_set_nic_info(adapter, &nic_info);
		if (ret)
			return ret;
		index = qlcnic_is_valid_nic_func(adapter, pci_func);
		if (index < 0)
			return -EINVAL;
		adapter->npars[index].min_bw = nic_info.min_tx_bw;
		adapter->npars[index].max_bw = nic_info.max_tx_bw;
	}

	return size;
}

static ssize_t qlcnic_sysfs_read_npar_config(struct file *file,
					     struct kobject *kobj,
					     struct bin_attribute *attr,
					     char *buf, loff_t offset,
					     size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_npar_func_cfg *np_cfg;
	struct qlcnic_info nic_info;
	u8 pci_func;
	int i, ret;
	u32 count;

	memset(&nic_info, 0, sizeof(struct qlcnic_info));
	memset(buf, 0, size);
	np_cfg = (struct qlcnic_npar_func_cfg *)buf;

	count = size / sizeof(struct qlcnic_npar_func_cfg);
	for (i = 0; i < adapter->ahw->total_nic_func; i++) {
		if (adapter->npars[i].pci_func >= count) {
			dev_dbg(dev, "%s: Total nic functions[%d], App sent function count[%d]\n",
				__func__, adapter->ahw->total_nic_func, count);
			continue;
		}
		if (!adapter->npars[i].eswitch_status)
			continue;
		pci_func = adapter->npars[i].pci_func;
		if (qlcnic_is_valid_nic_func(adapter, pci_func) < 0)
			continue;
		ret = qlcnic_get_nic_info(adapter, &nic_info, pci_func);
		if (ret)
			return ret;

		np_cfg[pci_func].pci_func = pci_func;
		np_cfg[pci_func].op_mode = (u8)nic_info.op_mode;
		np_cfg[pci_func].port_num = nic_info.phys_port;
		np_cfg[pci_func].fw_capab = nic_info.capabilities;
		np_cfg[pci_func].min_bw = nic_info.min_tx_bw;
		np_cfg[pci_func].max_bw = nic_info.max_tx_bw;
		np_cfg[pci_func].max_tx_queues = nic_info.max_tx_ques;
		np_cfg[pci_func].max_rx_queues = nic_info.max_rx_ques;
	}
	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	return size;
}

static ssize_t qlcnic_sysfs_get_port_stats(struct file *file,
					   struct kobject *kobj,
					   struct bin_attribute *attr,
					   char *buf, loff_t offset,
					   size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_esw_statistics port_stats;
	int ret;

	if (qlcnic_83xx_check(adapter))
		return -EOPNOTSUPP;

	if (size != sizeof(struct qlcnic_esw_statistics))
		return -EINVAL;

	if (offset >= adapter->ahw->max_vnic_func)
		return -EINVAL;

	memset(&port_stats, 0, size);
	ret = qlcnic_get_port_stats(adapter, offset, QLCNIC_QUERY_RX_COUNTER,
				    &port_stats.rx);
	if (ret)
		return ret;

	ret = qlcnic_get_port_stats(adapter, offset, QLCNIC_QUERY_TX_COUNTER,
				    &port_stats.tx);
	if (ret)
		return ret;

	memcpy(buf, &port_stats, size);
	return size;
}

static ssize_t qlcnic_sysfs_get_esw_stats(struct file *file,
					  struct kobject *kobj,
					  struct bin_attribute *attr,
					  char *buf, loff_t offset,
					  size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_esw_statistics esw_stats;
	int ret;

	if (qlcnic_83xx_check(adapter))
		return -EOPNOTSUPP;

	if (size != sizeof(struct qlcnic_esw_statistics))
		return -EINVAL;

	if (offset >= QLCNIC_NIU_MAX_XG_PORTS)
		return -EINVAL;

	memset(&esw_stats, 0, size);
	ret = qlcnic_get_eswitch_stats(adapter, offset, QLCNIC_QUERY_RX_COUNTER,
				       &esw_stats.rx);
	if (ret)
		return ret;

	ret = qlcnic_get_eswitch_stats(adapter, offset, QLCNIC_QUERY_TX_COUNTER,
				       &esw_stats.tx);
	if (ret)
		return ret;

	memcpy(buf, &esw_stats, size);
	return size;
}

static ssize_t qlcnic_sysfs_clear_esw_stats(struct file *file,
					    struct kobject *kobj,
					    struct bin_attribute *attr,
					    char *buf, loff_t offset,
					    size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	int ret;

	if (qlcnic_83xx_check(adapter))
		return -EOPNOTSUPP;

	if (offset >= QLCNIC_NIU_MAX_XG_PORTS)
		return -EINVAL;

	ret = qlcnic_clear_esw_stats(adapter, QLCNIC_STATS_ESWITCH, offset,
				     QLCNIC_QUERY_RX_COUNTER);
	if (ret)
		return ret;

	ret = qlcnic_clear_esw_stats(adapter, QLCNIC_STATS_ESWITCH, offset,
				     QLCNIC_QUERY_TX_COUNTER);
	if (ret)
		return ret;

	return size;
}

static ssize_t qlcnic_sysfs_clear_port_stats(struct file *file,
					     struct kobject *kobj,
					     struct bin_attribute *attr,
					     char *buf, loff_t offset,
					     size_t size)
{

	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	int ret;

	if (qlcnic_83xx_check(adapter))
		return -EOPNOTSUPP;

	if (offset >= adapter->ahw->max_vnic_func)
		return -EINVAL;

	ret = qlcnic_clear_esw_stats(adapter, QLCNIC_STATS_PORT, offset,
				     QLCNIC_QUERY_RX_COUNTER);
	if (ret)
		return ret;

	ret = qlcnic_clear_esw_stats(adapter, QLCNIC_STATS_PORT, offset,
				     QLCNIC_QUERY_TX_COUNTER);
	if (ret)
		return ret;

	return size;
}

static ssize_t qlcnic_sysfs_read_pci_config(struct file *file,
					    struct kobject *kobj,
					    struct bin_attribute *attr,
					    char *buf, loff_t offset,
					    size_t size)
{
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	struct qlcnic_pci_func_cfg *pci_cfg;
	struct qlcnic_pci_info *pci_info;
	int i, ret;
	u32 count;

	pci_info = kcalloc(size, sizeof(*pci_info), GFP_KERNEL);
	if (!pci_info)
		return -ENOMEM;

	ret = qlcnic_get_pci_info(adapter, pci_info);
	if (ret) {
		kfree(pci_info);
		return ret;
	}

	pci_cfg = (struct qlcnic_pci_func_cfg *)buf;
	count = size / sizeof(struct qlcnic_pci_func_cfg);
	qlcnic_swap32_buffer((u32 *)pci_info, size / sizeof(u32));
	for (i = 0; i < count; i++) {
		pci_cfg[i].pci_func = pci_info[i].id;
		pci_cfg[i].func_type = pci_info[i].type;
		pci_cfg[i].func_state = 0;
		pci_cfg[i].port_num = pci_info[i].default_port;
		pci_cfg[i].min_bw = pci_info[i].tx_min_bw;
		pci_cfg[i].max_bw = pci_info[i].tx_max_bw;
		memcpy(&pci_cfg[i].def_mac_addr, &pci_info[i].mac, ETH_ALEN);
	}

	kfree(pci_info);
	return size;
}

static ssize_t qlcnic_83xx_sysfs_flash_read_handler(struct file *filp,
						    struct kobject *kobj,
						    struct bin_attribute *attr,
						    char *buf, loff_t offset,
						    size_t size)
{
	unsigned char *p_read_buf;
	int  ret, count;
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);

	if (!size)
		return -EINVAL;

	count = size / sizeof(u32);

	if (size % sizeof(u32))
		count++;

	p_read_buf = kcalloc(size, sizeof(unsigned char), GFP_KERNEL);
	if (!p_read_buf)
		return -ENOMEM;
	if (qlcnic_83xx_lock_flash(adapter) != 0) {
		kfree(p_read_buf);
		return -EIO;
	}

	ret = qlcnic_83xx_lockless_flash_read32(adapter, offset, p_read_buf,
						count);

	if (ret) {
		qlcnic_83xx_unlock_flash(adapter);
		kfree(p_read_buf);
		return ret;
	}

	qlcnic_83xx_unlock_flash(adapter);
	qlcnic_swap32_buffer((u32 *)p_read_buf, count);
	memcpy(buf, p_read_buf, size);
	kfree(p_read_buf);

	return size;
}

static int qlcnic_83xx_sysfs_flash_bulk_write(struct qlcnic_adapter *adapter,
					      char *buf, loff_t offset,
					      size_t size)
{
	int  i, ret, count;
	unsigned char *p_cache, *p_src;

	p_cache = kcalloc(size, sizeof(unsigned char), GFP_KERNEL);
	if (!p_cache)
		return -ENOMEM;

	count = size / sizeof(u32);
	qlcnic_swap32_buffer((u32 *)buf, count);
	memcpy(p_cache, buf, size);
	p_src = p_cache;

	if (qlcnic_83xx_lock_flash(adapter) != 0) {
		kfree(p_cache);
		return -EIO;
	}

	if (adapter->ahw->fdt.mfg_id == adapter->flash_mfg_id) {
		ret = qlcnic_83xx_enable_flash_write(adapter);
		if (ret) {
			kfree(p_cache);
			qlcnic_83xx_unlock_flash(adapter);
			return -EIO;
		}
	}

	for (i = 0; i < count / QLC_83XX_FLASH_WRITE_MAX; i++) {
		ret = qlcnic_83xx_flash_bulk_write(adapter, offset,
						   (u32 *)p_src,
						   QLC_83XX_FLASH_WRITE_MAX);

		if (ret) {
			if (adapter->ahw->fdt.mfg_id == adapter->flash_mfg_id) {
				ret = qlcnic_83xx_disable_flash_write(adapter);
				if (ret) {
					kfree(p_cache);
					qlcnic_83xx_unlock_flash(adapter);
					return -EIO;
				}
			}

			kfree(p_cache);
			qlcnic_83xx_unlock_flash(adapter);
			return -EIO;
		}

		p_src = p_src + sizeof(u32)*QLC_83XX_FLASH_WRITE_MAX;
		offset = offset + sizeof(u32)*QLC_83XX_FLASH_WRITE_MAX;
	}

	if (adapter->ahw->fdt.mfg_id == adapter->flash_mfg_id) {
		ret = qlcnic_83xx_disable_flash_write(adapter);
		if (ret) {
			kfree(p_cache);
			qlcnic_83xx_unlock_flash(adapter);
			return -EIO;
		}
	}

	kfree(p_cache);
	qlcnic_83xx_unlock_flash(adapter);

	return 0;
}

static int qlcnic_83xx_sysfs_flash_write(struct qlcnic_adapter *adapter,
					 char *buf, loff_t offset, size_t size)
{
	int  i, ret, count;
	unsigned char *p_cache, *p_src;

	p_cache = kcalloc(size, sizeof(unsigned char), GFP_KERNEL);
	if (!p_cache)
		return -ENOMEM;

	qlcnic_swap32_buffer((u32 *)buf, size / sizeof(u32));
	memcpy(p_cache, buf, size);
	p_src = p_cache;
	count = size / sizeof(u32);

	if (qlcnic_83xx_lock_flash(adapter) != 0) {
		kfree(p_cache);
		return -EIO;
	}

	if (adapter->ahw->fdt.mfg_id == adapter->flash_mfg_id) {
		ret = qlcnic_83xx_enable_flash_write(adapter);
		if (ret) {
			kfree(p_cache);
			qlcnic_83xx_unlock_flash(adapter);
			return -EIO;
		}
	}

	for (i = 0; i < count; i++) {
		ret = qlcnic_83xx_flash_write32(adapter, offset, (u32 *)p_src);
		if (ret) {
			if (adapter->ahw->fdt.mfg_id == adapter->flash_mfg_id) {
				ret = qlcnic_83xx_disable_flash_write(adapter);
				if (ret) {
					kfree(p_cache);
					qlcnic_83xx_unlock_flash(adapter);
					return -EIO;
				}
			}
			kfree(p_cache);
			qlcnic_83xx_unlock_flash(adapter);
			return -EIO;
		}

		p_src = p_src + sizeof(u32);
		offset = offset + sizeof(u32);
	}

	if (adapter->ahw->fdt.mfg_id == adapter->flash_mfg_id) {
		ret = qlcnic_83xx_disable_flash_write(adapter);
		if (ret) {
			kfree(p_cache);
			qlcnic_83xx_unlock_flash(adapter);
			return -EIO;
		}
	}

	kfree(p_cache);
	qlcnic_83xx_unlock_flash(adapter);

	return 0;
}

static ssize_t qlcnic_83xx_sysfs_flash_write_handler(struct file *filp,
						     struct kobject *kobj,
						     struct bin_attribute *attr,
						     char *buf, loff_t offset,
						     size_t size)
{
	int  ret;
	static int flash_mode;
	unsigned long data;
	struct device *dev = kobj_to_dev(kobj);
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);

	ret = kstrtoul(buf, 16, &data);
	if (ret)
		return ret;

	switch (data) {
	case QLC_83XX_FLASH_SECTOR_ERASE_CMD:
		flash_mode = QLC_83XX_ERASE_MODE;
		ret = qlcnic_83xx_erase_flash_sector(adapter, offset);
		if (ret) {
			dev_err(&adapter->pdev->dev,
				"%s failed at %d\n", __func__, __LINE__);
			return -EIO;
		}
		break;

	case QLC_83XX_FLASH_BULK_WRITE_CMD:
		flash_mode = QLC_83XX_BULK_WRITE_MODE;
		break;

	case QLC_83XX_FLASH_WRITE_CMD:
		flash_mode = QLC_83XX_WRITE_MODE;
		break;
	default:
		if (flash_mode == QLC_83XX_BULK_WRITE_MODE) {
			ret = qlcnic_83xx_sysfs_flash_bulk_write(adapter, buf,
								 offset, size);
			if (ret) {
				dev_err(&adapter->pdev->dev,
					"%s failed at %d\n",
					__func__, __LINE__);
				return -EIO;
			}
		}

		if (flash_mode == QLC_83XX_WRITE_MODE) {
			ret = qlcnic_83xx_sysfs_flash_write(adapter, buf,
							    offset, size);
			if (ret) {
				dev_err(&adapter->pdev->dev,
					"%s failed at %d\n", __func__,
					__LINE__);
				return -EIO;
			}
		}
	}

	return size;
}

static const struct device_attribute dev_attr_bridged_mode = {
	.attr = { .name = "bridged_mode", .mode = 0644 },
	.show = qlcnic_show_bridged_mode,
	.store = qlcnic_store_bridged_mode,
};

static const struct device_attribute dev_attr_diag_mode = {
	.attr = { .name = "diag_mode", .mode = 0644 },
	.show = qlcnic_show_diag_mode,
	.store = qlcnic_store_diag_mode,
};

static const struct device_attribute dev_attr_beacon = {
	.attr = { .name = "beacon", .mode = 0644 },
	.show = qlcnic_show_beacon,
	.store = qlcnic_store_beacon,
};

static const struct bin_attribute bin_attr_crb = {
	.attr = { .name = "crb", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_read_crb,
	.write = qlcnic_sysfs_write_crb,
};

static const struct bin_attribute bin_attr_mem = {
	.attr = { .name = "mem", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_read_mem,
	.write = qlcnic_sysfs_write_mem,
};

static const struct bin_attribute bin_attr_npar_config = {
	.attr = { .name = "npar_config", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_read_npar_config,
	.write = qlcnic_sysfs_write_npar_config,
};

static const struct bin_attribute bin_attr_pci_config = {
	.attr = { .name = "pci_config", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_read_pci_config,
	.write = NULL,
};

static const struct bin_attribute bin_attr_port_stats = {
	.attr = { .name = "port_stats", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_get_port_stats,
	.write = qlcnic_sysfs_clear_port_stats,
};

static const struct bin_attribute bin_attr_esw_stats = {
	.attr = { .name = "esw_stats", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_get_esw_stats,
	.write = qlcnic_sysfs_clear_esw_stats,
};

static const struct bin_attribute bin_attr_esw_config = {
	.attr = { .name = "esw_config", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_read_esw_config,
	.write = qlcnic_sysfs_write_esw_config,
};

static const struct bin_attribute bin_attr_pm_config = {
	.attr = { .name = "pm_config", .mode = 0644 },
	.size = 0,
	.read = qlcnic_sysfs_read_pm_config,
	.write = qlcnic_sysfs_write_pm_config,
};

static const struct bin_attribute bin_attr_flash = {
	.attr = { .name = "flash", .mode = 0644 },
	.size = 0,
	.read = qlcnic_83xx_sysfs_flash_read_handler,
	.write = qlcnic_83xx_sysfs_flash_write_handler,
};

#ifdef CONFIG_QLCNIC_HWMON

static ssize_t qlcnic_hwmon_show_temp(struct device *dev,
				      struct device_attribute *dev_attr,
				      char *buf)
{
	struct qlcnic_adapter *adapter = dev_get_drvdata(dev);
	unsigned int temperature = 0, value = 0;

	if (qlcnic_83xx_check(adapter))
		value = QLCRDX(adapter->ahw, QLC_83XX_ASIC_TEMP);
	else if (qlcnic_82xx_check(adapter))
		value = QLC_SHARED_REG_RD32(adapter, QLCNIC_ASIC_TEMP);

	temperature = qlcnic_get_temp_val(value);
	/* display millidegree celcius */
	temperature *= 1000;
	return sprintf(buf, "%u\n", temperature);
}

/* hwmon-sysfs attributes */
static SENSOR_DEVICE_ATTR(temp1_input, 0444,
			  qlcnic_hwmon_show_temp, NULL, 1);

static struct attribute *qlcnic_hwmon_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	NULL
};

ATTRIBUTE_GROUPS(qlcnic_hwmon);

void qlcnic_register_hwmon_dev(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;
	struct device *hwmon_dev;

	/* Skip hwmon registration for a VF device */
	if (qlcnic_sriov_vf_check(adapter)) {
		adapter->ahw->hwmon_dev = NULL;
		return;
	}
	hwmon_dev = hwmon_device_register_with_groups(dev, qlcnic_driver_name,
						      adapter,
						      qlcnic_hwmon_groups);
	if (IS_ERR(hwmon_dev)) {
		dev_err(dev, "Cannot register with hwmon, err=%ld\n",
			PTR_ERR(hwmon_dev));
		hwmon_dev = NULL;
	}
	adapter->ahw->hwmon_dev = hwmon_dev;
}

void qlcnic_unregister_hwmon_dev(struct qlcnic_adapter *adapter)
{
	struct device *hwmon_dev = adapter->ahw->hwmon_dev;
	if (hwmon_dev) {
		hwmon_device_unregister(hwmon_dev);
		adapter->ahw->hwmon_dev = NULL;
	}
}
#endif

void qlcnic_create_sysfs_entries(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;

	if (adapter->ahw->capabilities & QLCNIC_FW_CAPABILITY_BDG)
		if (device_create_file(dev, &dev_attr_bridged_mode))
			dev_warn(dev,
				 "failed to create bridged_mode sysfs entry\n");
}

void qlcnic_remove_sysfs_entries(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;

	if (adapter->ahw->capabilities & QLCNIC_FW_CAPABILITY_BDG)
		device_remove_file(dev, &dev_attr_bridged_mode);
}

static void qlcnic_create_diag_entries(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;

	if (device_create_bin_file(dev, &bin_attr_port_stats))
		dev_info(dev, "failed to create port stats sysfs entry");

	if (adapter->ahw->op_mode == QLCNIC_NON_PRIV_FUNC)
		return;
	if (device_create_file(dev, &dev_attr_diag_mode))
		dev_info(dev, "failed to create diag_mode sysfs entry\n");
	if (device_create_bin_file(dev, &bin_attr_crb))
		dev_info(dev, "failed to create crb sysfs entry\n");
	if (device_create_bin_file(dev, &bin_attr_mem))
		dev_info(dev, "failed to create mem sysfs entry\n");

	if (test_bit(__QLCNIC_MAINTENANCE_MODE, &adapter->state))
		return;

	if (device_create_bin_file(dev, &bin_attr_pci_config))
		dev_info(dev, "failed to create pci config sysfs entry");

	if (device_create_file(dev, &dev_attr_beacon))
		dev_info(dev, "failed to create beacon sysfs entry");

	if (!(adapter->flags & QLCNIC_ESWITCH_ENABLED))
		return;
	if (device_create_bin_file(dev, &bin_attr_esw_config))
		dev_info(dev, "failed to create esw config sysfs entry");
	if (adapter->ahw->op_mode != QLCNIC_MGMT_FUNC)
		return;
	if (device_create_bin_file(dev, &bin_attr_npar_config))
		dev_info(dev, "failed to create npar config sysfs entry");
	if (device_create_bin_file(dev, &bin_attr_pm_config))
		dev_info(dev, "failed to create pm config sysfs entry");
	if (device_create_bin_file(dev, &bin_attr_esw_stats))
		dev_info(dev, "failed to create eswitch stats sysfs entry");
}

static void qlcnic_remove_diag_entries(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;

	device_remove_bin_file(dev, &bin_attr_port_stats);

	if (adapter->ahw->op_mode == QLCNIC_NON_PRIV_FUNC)
		return;
	device_remove_file(dev, &dev_attr_diag_mode);
	device_remove_bin_file(dev, &bin_attr_crb);
	device_remove_bin_file(dev, &bin_attr_mem);

	if (test_bit(__QLCNIC_MAINTENANCE_MODE, &adapter->state))
		return;

	device_remove_bin_file(dev, &bin_attr_pci_config);
	device_remove_file(dev, &dev_attr_beacon);
	if (!(adapter->flags & QLCNIC_ESWITCH_ENABLED))
		return;
	device_remove_bin_file(dev, &bin_attr_esw_config);
	if (adapter->ahw->op_mode != QLCNIC_MGMT_FUNC)
		return;
	device_remove_bin_file(dev, &bin_attr_npar_config);
	device_remove_bin_file(dev, &bin_attr_pm_config);
	device_remove_bin_file(dev, &bin_attr_esw_stats);
}

void qlcnic_82xx_add_sysfs(struct qlcnic_adapter *adapter)
{
	qlcnic_create_diag_entries(adapter);
}

void qlcnic_82xx_remove_sysfs(struct qlcnic_adapter *adapter)
{
	qlcnic_remove_diag_entries(adapter);
}

void qlcnic_83xx_add_sysfs(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;

	qlcnic_create_diag_entries(adapter);

	if (sysfs_create_bin_file(&dev->kobj, &bin_attr_flash))
		dev_info(dev, "failed to create flash sysfs entry\n");
}

void qlcnic_83xx_remove_sysfs(struct qlcnic_adapter *adapter)
{
	struct device *dev = &adapter->pdev->dev;

	qlcnic_remove_diag_entries(adapter);
	sysfs_remove_bin_file(&dev->kobj, &bin_attr_flash);
}

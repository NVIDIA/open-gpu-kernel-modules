/*
 * Copyright(c) 2015-2017 Intel Corporation.
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
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
 * BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 *
 */
#include <linux/ctype.h>

#include "hfi.h"
#include "mad.h"
#include "trace.h"

/*
 * Start of per-port congestion control structures and support code
 */

/*
 * Congestion control table size followed by table entries
 */
static ssize_t read_cc_table_bin(struct file *filp, struct kobject *kobj,
				 struct bin_attribute *bin_attr,
				 char *buf, loff_t pos, size_t count)
{
	int ret;
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, pport_cc_kobj);
	struct cc_state *cc_state;

	ret = ppd->total_cct_entry * sizeof(struct ib_cc_table_entry_shadow)
		 + sizeof(__be16);

	if (pos > ret)
		return -EINVAL;

	if (count > ret - pos)
		count = ret - pos;

	if (!count)
		return count;

	rcu_read_lock();
	cc_state = get_cc_state(ppd);
	if (!cc_state) {
		rcu_read_unlock();
		return -EINVAL;
	}
	memcpy(buf, (void *)&cc_state->cct + pos, count);
	rcu_read_unlock();

	return count;
}

static void port_release(struct kobject *kobj)
{
	/* nothing to do since memory is freed by hfi1_free_devdata() */
}

static const struct bin_attribute cc_table_bin_attr = {
	.attr = {.name = "cc_table_bin", .mode = 0444},
	.read = read_cc_table_bin,
	.size = PAGE_SIZE,
};

/*
 * Congestion settings: port control, control map and an array of 16
 * entries for the congestion entries - increase, timer, event log
 * trigger threshold and the minimum injection rate delay.
 */
static ssize_t read_cc_setting_bin(struct file *filp, struct kobject *kobj,
				   struct bin_attribute *bin_attr,
				   char *buf, loff_t pos, size_t count)
{
	int ret;
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, pport_cc_kobj);
	struct cc_state *cc_state;

	ret = sizeof(struct opa_congestion_setting_attr_shadow);

	if (pos > ret)
		return -EINVAL;
	if (count > ret - pos)
		count = ret - pos;

	if (!count)
		return count;

	rcu_read_lock();
	cc_state = get_cc_state(ppd);
	if (!cc_state) {
		rcu_read_unlock();
		return -EINVAL;
	}
	memcpy(buf, (void *)&cc_state->cong_setting + pos, count);
	rcu_read_unlock();

	return count;
}

static const struct bin_attribute cc_setting_bin_attr = {
	.attr = {.name = "cc_settings_bin", .mode = 0444},
	.read = read_cc_setting_bin,
	.size = PAGE_SIZE,
};

struct hfi1_port_attr {
	struct attribute attr;
	ssize_t	(*show)(struct hfi1_pportdata *, char *);
	ssize_t	(*store)(struct hfi1_pportdata *, const char *, size_t);
};

static ssize_t cc_prescan_show(struct hfi1_pportdata *ppd, char *buf)
{
	return sysfs_emit(buf, "%s\n", ppd->cc_prescan ? "on" : "off");
}

static ssize_t cc_prescan_store(struct hfi1_pportdata *ppd, const char *buf,
				size_t count)
{
	if (!memcmp(buf, "on", 2))
		ppd->cc_prescan = true;
	else if (!memcmp(buf, "off", 3))
		ppd->cc_prescan = false;

	return count;
}

static struct hfi1_port_attr cc_prescan_attr =
		__ATTR(cc_prescan, 0600, cc_prescan_show, cc_prescan_store);

static ssize_t cc_attr_show(struct kobject *kobj, struct attribute *attr,
			    char *buf)
{
	struct hfi1_port_attr *port_attr =
		container_of(attr, struct hfi1_port_attr, attr);
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, pport_cc_kobj);

	return port_attr->show(ppd, buf);
}

static ssize_t cc_attr_store(struct kobject *kobj, struct attribute *attr,
			     const char *buf, size_t count)
{
	struct hfi1_port_attr *port_attr =
		container_of(attr, struct hfi1_port_attr, attr);
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, pport_cc_kobj);

	return port_attr->store(ppd, buf, count);
}

static const struct sysfs_ops port_cc_sysfs_ops = {
	.show = cc_attr_show,
	.store = cc_attr_store
};

static struct attribute *port_cc_default_attributes[] = {
	&cc_prescan_attr.attr,
	NULL
};

static struct kobj_type port_cc_ktype = {
	.release = port_release,
	.sysfs_ops = &port_cc_sysfs_ops,
	.default_attrs = port_cc_default_attributes
};

/* Start sc2vl */
#define HFI1_SC2VL_ATTR(N)				    \
	static struct hfi1_sc2vl_attr hfi1_sc2vl_attr_##N = { \
		.attr = { .name = __stringify(N), .mode = 0444 }, \
		.sc = N \
	}

struct hfi1_sc2vl_attr {
	struct attribute attr;
	int sc;
};

HFI1_SC2VL_ATTR(0);
HFI1_SC2VL_ATTR(1);
HFI1_SC2VL_ATTR(2);
HFI1_SC2VL_ATTR(3);
HFI1_SC2VL_ATTR(4);
HFI1_SC2VL_ATTR(5);
HFI1_SC2VL_ATTR(6);
HFI1_SC2VL_ATTR(7);
HFI1_SC2VL_ATTR(8);
HFI1_SC2VL_ATTR(9);
HFI1_SC2VL_ATTR(10);
HFI1_SC2VL_ATTR(11);
HFI1_SC2VL_ATTR(12);
HFI1_SC2VL_ATTR(13);
HFI1_SC2VL_ATTR(14);
HFI1_SC2VL_ATTR(15);
HFI1_SC2VL_ATTR(16);
HFI1_SC2VL_ATTR(17);
HFI1_SC2VL_ATTR(18);
HFI1_SC2VL_ATTR(19);
HFI1_SC2VL_ATTR(20);
HFI1_SC2VL_ATTR(21);
HFI1_SC2VL_ATTR(22);
HFI1_SC2VL_ATTR(23);
HFI1_SC2VL_ATTR(24);
HFI1_SC2VL_ATTR(25);
HFI1_SC2VL_ATTR(26);
HFI1_SC2VL_ATTR(27);
HFI1_SC2VL_ATTR(28);
HFI1_SC2VL_ATTR(29);
HFI1_SC2VL_ATTR(30);
HFI1_SC2VL_ATTR(31);

static struct attribute *sc2vl_default_attributes[] = {
	&hfi1_sc2vl_attr_0.attr,
	&hfi1_sc2vl_attr_1.attr,
	&hfi1_sc2vl_attr_2.attr,
	&hfi1_sc2vl_attr_3.attr,
	&hfi1_sc2vl_attr_4.attr,
	&hfi1_sc2vl_attr_5.attr,
	&hfi1_sc2vl_attr_6.attr,
	&hfi1_sc2vl_attr_7.attr,
	&hfi1_sc2vl_attr_8.attr,
	&hfi1_sc2vl_attr_9.attr,
	&hfi1_sc2vl_attr_10.attr,
	&hfi1_sc2vl_attr_11.attr,
	&hfi1_sc2vl_attr_12.attr,
	&hfi1_sc2vl_attr_13.attr,
	&hfi1_sc2vl_attr_14.attr,
	&hfi1_sc2vl_attr_15.attr,
	&hfi1_sc2vl_attr_16.attr,
	&hfi1_sc2vl_attr_17.attr,
	&hfi1_sc2vl_attr_18.attr,
	&hfi1_sc2vl_attr_19.attr,
	&hfi1_sc2vl_attr_20.attr,
	&hfi1_sc2vl_attr_21.attr,
	&hfi1_sc2vl_attr_22.attr,
	&hfi1_sc2vl_attr_23.attr,
	&hfi1_sc2vl_attr_24.attr,
	&hfi1_sc2vl_attr_25.attr,
	&hfi1_sc2vl_attr_26.attr,
	&hfi1_sc2vl_attr_27.attr,
	&hfi1_sc2vl_attr_28.attr,
	&hfi1_sc2vl_attr_29.attr,
	&hfi1_sc2vl_attr_30.attr,
	&hfi1_sc2vl_attr_31.attr,
	NULL
};

static ssize_t sc2vl_attr_show(struct kobject *kobj, struct attribute *attr,
			       char *buf)
{
	struct hfi1_sc2vl_attr *sattr =
		container_of(attr, struct hfi1_sc2vl_attr, attr);
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, sc2vl_kobj);
	struct hfi1_devdata *dd = ppd->dd;

	return sysfs_emit(buf, "%u\n", *((u8 *)dd->sc2vl + sattr->sc));
}

static const struct sysfs_ops hfi1_sc2vl_ops = {
	.show = sc2vl_attr_show,
};

static struct kobj_type hfi1_sc2vl_ktype = {
	.release = port_release,
	.sysfs_ops = &hfi1_sc2vl_ops,
	.default_attrs = sc2vl_default_attributes
};

/* End sc2vl */

/* Start sl2sc */
#define HFI1_SL2SC_ATTR(N)				    \
	static struct hfi1_sl2sc_attr hfi1_sl2sc_attr_##N = {	  \
		.attr = { .name = __stringify(N), .mode = 0444 }, \
		.sl = N						  \
	}

struct hfi1_sl2sc_attr {
	struct attribute attr;
	int sl;
};

HFI1_SL2SC_ATTR(0);
HFI1_SL2SC_ATTR(1);
HFI1_SL2SC_ATTR(2);
HFI1_SL2SC_ATTR(3);
HFI1_SL2SC_ATTR(4);
HFI1_SL2SC_ATTR(5);
HFI1_SL2SC_ATTR(6);
HFI1_SL2SC_ATTR(7);
HFI1_SL2SC_ATTR(8);
HFI1_SL2SC_ATTR(9);
HFI1_SL2SC_ATTR(10);
HFI1_SL2SC_ATTR(11);
HFI1_SL2SC_ATTR(12);
HFI1_SL2SC_ATTR(13);
HFI1_SL2SC_ATTR(14);
HFI1_SL2SC_ATTR(15);
HFI1_SL2SC_ATTR(16);
HFI1_SL2SC_ATTR(17);
HFI1_SL2SC_ATTR(18);
HFI1_SL2SC_ATTR(19);
HFI1_SL2SC_ATTR(20);
HFI1_SL2SC_ATTR(21);
HFI1_SL2SC_ATTR(22);
HFI1_SL2SC_ATTR(23);
HFI1_SL2SC_ATTR(24);
HFI1_SL2SC_ATTR(25);
HFI1_SL2SC_ATTR(26);
HFI1_SL2SC_ATTR(27);
HFI1_SL2SC_ATTR(28);
HFI1_SL2SC_ATTR(29);
HFI1_SL2SC_ATTR(30);
HFI1_SL2SC_ATTR(31);

static struct attribute *sl2sc_default_attributes[] = {
	&hfi1_sl2sc_attr_0.attr,
	&hfi1_sl2sc_attr_1.attr,
	&hfi1_sl2sc_attr_2.attr,
	&hfi1_sl2sc_attr_3.attr,
	&hfi1_sl2sc_attr_4.attr,
	&hfi1_sl2sc_attr_5.attr,
	&hfi1_sl2sc_attr_6.attr,
	&hfi1_sl2sc_attr_7.attr,
	&hfi1_sl2sc_attr_8.attr,
	&hfi1_sl2sc_attr_9.attr,
	&hfi1_sl2sc_attr_10.attr,
	&hfi1_sl2sc_attr_11.attr,
	&hfi1_sl2sc_attr_12.attr,
	&hfi1_sl2sc_attr_13.attr,
	&hfi1_sl2sc_attr_14.attr,
	&hfi1_sl2sc_attr_15.attr,
	&hfi1_sl2sc_attr_16.attr,
	&hfi1_sl2sc_attr_17.attr,
	&hfi1_sl2sc_attr_18.attr,
	&hfi1_sl2sc_attr_19.attr,
	&hfi1_sl2sc_attr_20.attr,
	&hfi1_sl2sc_attr_21.attr,
	&hfi1_sl2sc_attr_22.attr,
	&hfi1_sl2sc_attr_23.attr,
	&hfi1_sl2sc_attr_24.attr,
	&hfi1_sl2sc_attr_25.attr,
	&hfi1_sl2sc_attr_26.attr,
	&hfi1_sl2sc_attr_27.attr,
	&hfi1_sl2sc_attr_28.attr,
	&hfi1_sl2sc_attr_29.attr,
	&hfi1_sl2sc_attr_30.attr,
	&hfi1_sl2sc_attr_31.attr,
	NULL
};

static ssize_t sl2sc_attr_show(struct kobject *kobj, struct attribute *attr,
			       char *buf)
{
	struct hfi1_sl2sc_attr *sattr =
		container_of(attr, struct hfi1_sl2sc_attr, attr);
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, sl2sc_kobj);
	struct hfi1_ibport *ibp = &ppd->ibport_data;

	return sysfs_emit(buf, "%u\n", ibp->sl_to_sc[sattr->sl]);
}

static const struct sysfs_ops hfi1_sl2sc_ops = {
	.show = sl2sc_attr_show,
};

static struct kobj_type hfi1_sl2sc_ktype = {
	.release = port_release,
	.sysfs_ops = &hfi1_sl2sc_ops,
	.default_attrs = sl2sc_default_attributes
};

/* End sl2sc */

/* Start vl2mtu */

#define HFI1_VL2MTU_ATTR(N) \
	static struct hfi1_vl2mtu_attr hfi1_vl2mtu_attr_##N = { \
		.attr = { .name = __stringify(N), .mode = 0444 }, \
		.vl = N						  \
	}

struct hfi1_vl2mtu_attr {
	struct attribute attr;
	int vl;
};

HFI1_VL2MTU_ATTR(0);
HFI1_VL2MTU_ATTR(1);
HFI1_VL2MTU_ATTR(2);
HFI1_VL2MTU_ATTR(3);
HFI1_VL2MTU_ATTR(4);
HFI1_VL2MTU_ATTR(5);
HFI1_VL2MTU_ATTR(6);
HFI1_VL2MTU_ATTR(7);
HFI1_VL2MTU_ATTR(8);
HFI1_VL2MTU_ATTR(9);
HFI1_VL2MTU_ATTR(10);
HFI1_VL2MTU_ATTR(11);
HFI1_VL2MTU_ATTR(12);
HFI1_VL2MTU_ATTR(13);
HFI1_VL2MTU_ATTR(14);
HFI1_VL2MTU_ATTR(15);

static struct attribute *vl2mtu_default_attributes[] = {
	&hfi1_vl2mtu_attr_0.attr,
	&hfi1_vl2mtu_attr_1.attr,
	&hfi1_vl2mtu_attr_2.attr,
	&hfi1_vl2mtu_attr_3.attr,
	&hfi1_vl2mtu_attr_4.attr,
	&hfi1_vl2mtu_attr_5.attr,
	&hfi1_vl2mtu_attr_6.attr,
	&hfi1_vl2mtu_attr_7.attr,
	&hfi1_vl2mtu_attr_8.attr,
	&hfi1_vl2mtu_attr_9.attr,
	&hfi1_vl2mtu_attr_10.attr,
	&hfi1_vl2mtu_attr_11.attr,
	&hfi1_vl2mtu_attr_12.attr,
	&hfi1_vl2mtu_attr_13.attr,
	&hfi1_vl2mtu_attr_14.attr,
	&hfi1_vl2mtu_attr_15.attr,
	NULL
};

static ssize_t vl2mtu_attr_show(struct kobject *kobj, struct attribute *attr,
				char *buf)
{
	struct hfi1_vl2mtu_attr *vlattr =
		container_of(attr, struct hfi1_vl2mtu_attr, attr);
	struct hfi1_pportdata *ppd =
		container_of(kobj, struct hfi1_pportdata, vl2mtu_kobj);
	struct hfi1_devdata *dd = ppd->dd;

	return sysfs_emit(buf, "%u\n", dd->vld[vlattr->vl].mtu);
}

static const struct sysfs_ops hfi1_vl2mtu_ops = {
	.show = vl2mtu_attr_show,
};

static struct kobj_type hfi1_vl2mtu_ktype = {
	.release = port_release,
	.sysfs_ops = &hfi1_vl2mtu_ops,
	.default_attrs = vl2mtu_default_attributes
};

/* end of per-port file structures and support code */

/*
 * Start of per-unit (or driver, in some cases, but replicated
 * per unit) functions (these get a device *)
 */
static ssize_t hw_rev_show(struct device *device, struct device_attribute *attr,
			   char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);

	return sysfs_emit(buf, "%x\n", dd_from_dev(dev)->minrev);
}
static DEVICE_ATTR_RO(hw_rev);

static ssize_t board_id_show(struct device *device,
			     struct device_attribute *attr, char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);

	if (!dd->boardname)
		return -EINVAL;

	return sysfs_emit(buf, "%s\n", dd->boardname);
}
static DEVICE_ATTR_RO(board_id);

static ssize_t boardversion_show(struct device *device,
				 struct device_attribute *attr, char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);

	/* The string printed here is already newline-terminated. */
	return sysfs_emit(buf, "%s", dd->boardversion);
}
static DEVICE_ATTR_RO(boardversion);

static ssize_t nctxts_show(struct device *device,
			   struct device_attribute *attr, char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);

	/*
	 * Return the smaller of send and receive contexts.
	 * Normally, user level applications would require both a send
	 * and a receive context, so returning the smaller of the two counts
	 * give a more accurate picture of total contexts available.
	 */
	return sysfs_emit(buf, "%u\n",
			  min(dd->num_user_contexts,
			      (u32)dd->sc_sizes[SC_USER].count));
}
static DEVICE_ATTR_RO(nctxts);

static ssize_t nfreectxts_show(struct device *device,
			       struct device_attribute *attr, char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);

	/* Return the number of free user ports (contexts) available. */
	return sysfs_emit(buf, "%u\n", dd->freectxts);
}
static DEVICE_ATTR_RO(nfreectxts);

static ssize_t serial_show(struct device *device,
			   struct device_attribute *attr, char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);

	/* dd->serial is already newline terminated in chip.c */
	return sysfs_emit(buf, "%s", dd->serial);
}
static DEVICE_ATTR_RO(serial);

static ssize_t chip_reset_store(struct device *device,
				struct device_attribute *attr, const char *buf,
				size_t count)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);
	int ret;

	if (count < 5 || memcmp(buf, "reset", 5) || !dd->diag_client) {
		ret = -EINVAL;
		goto bail;
	}

	ret = hfi1_reset_device(dd->unit);
bail:
	return ret < 0 ? ret : count;
}
static DEVICE_ATTR_WO(chip_reset);

/*
 * Convert the reported temperature from an integer (reported in
 * units of 0.25C) to a floating point number.
 */
#define temp_d(t) ((t) >> 2)
#define temp_f(t) (((t)&0x3) * 25u)

/*
 * Dump tempsense values, in decimal, to ease shell-scripts.
 */
static ssize_t tempsense_show(struct device *device,
			      struct device_attribute *attr, char *buf)
{
	struct hfi1_ibdev *dev =
		rdma_device_to_drv_device(device, struct hfi1_ibdev, rdi.ibdev);
	struct hfi1_devdata *dd = dd_from_dev(dev);
	struct hfi1_temp temp;
	int ret;

	ret = hfi1_tempsense_rd(dd, &temp);
	if (ret)
		return ret;

	return sysfs_emit(buf, "%u.%02u %u.%02u %u.%02u %u.%02u %u %u %u\n",
			  temp_d(temp.curr), temp_f(temp.curr),
			  temp_d(temp.lo_lim), temp_f(temp.lo_lim),
			  temp_d(temp.hi_lim), temp_f(temp.hi_lim),
			  temp_d(temp.crit_lim), temp_f(temp.crit_lim),
			  temp.triggers & 0x1,
			  temp.triggers & 0x2,
			  temp.triggers & 0x4);
}
static DEVICE_ATTR_RO(tempsense);

/*
 * end of per-unit (or driver, in some cases, but replicated
 * per unit) functions
 */

/* start of per-unit file structures and support code */
static struct attribute *hfi1_attributes[] = {
	&dev_attr_hw_rev.attr,
	&dev_attr_board_id.attr,
	&dev_attr_nctxts.attr,
	&dev_attr_nfreectxts.attr,
	&dev_attr_serial.attr,
	&dev_attr_boardversion.attr,
	&dev_attr_tempsense.attr,
	&dev_attr_chip_reset.attr,
	NULL,
};

const struct attribute_group ib_hfi1_attr_group = {
	.attrs = hfi1_attributes,
};

int hfi1_create_port_files(struct ib_device *ibdev, u32 port_num,
			   struct kobject *kobj)
{
	struct hfi1_pportdata *ppd;
	struct hfi1_devdata *dd = dd_from_ibdev(ibdev);
	int ret;

	if (!port_num || port_num > dd->num_pports) {
		dd_dev_err(dd,
			   "Skipping infiniband class with invalid port %u\n",
			   port_num);
		return -ENODEV;
	}
	ppd = &dd->pport[port_num - 1];

	ret = kobject_init_and_add(&ppd->sc2vl_kobj, &hfi1_sc2vl_ktype, kobj,
				   "sc2vl");
	if (ret) {
		dd_dev_err(dd,
			   "Skipping sc2vl sysfs info, (err %d) port %u\n",
			   ret, port_num);
		/*
		 * Based on the documentation for kobject_init_and_add(), the
		 * caller should call kobject_put even if this call fails.
		 */
		goto bail_sc2vl;
	}
	kobject_uevent(&ppd->sc2vl_kobj, KOBJ_ADD);

	ret = kobject_init_and_add(&ppd->sl2sc_kobj, &hfi1_sl2sc_ktype, kobj,
				   "sl2sc");
	if (ret) {
		dd_dev_err(dd,
			   "Skipping sl2sc sysfs info, (err %d) port %u\n",
			   ret, port_num);
		goto bail_sl2sc;
	}
	kobject_uevent(&ppd->sl2sc_kobj, KOBJ_ADD);

	ret = kobject_init_and_add(&ppd->vl2mtu_kobj, &hfi1_vl2mtu_ktype, kobj,
				   "vl2mtu");
	if (ret) {
		dd_dev_err(dd,
			   "Skipping vl2mtu sysfs info, (err %d) port %u\n",
			   ret, port_num);
		goto bail_vl2mtu;
	}
	kobject_uevent(&ppd->vl2mtu_kobj, KOBJ_ADD);

	ret = kobject_init_and_add(&ppd->pport_cc_kobj, &port_cc_ktype,
				   kobj, "CCMgtA");
	if (ret) {
		dd_dev_err(dd,
			   "Skipping Congestion Control sysfs info, (err %d) port %u\n",
			   ret, port_num);
		goto bail_cc;
	}

	kobject_uevent(&ppd->pport_cc_kobj, KOBJ_ADD);

	ret = sysfs_create_bin_file(&ppd->pport_cc_kobj, &cc_setting_bin_attr);
	if (ret) {
		dd_dev_err(dd,
			   "Skipping Congestion Control setting sysfs info, (err %d) port %u\n",
			   ret, port_num);
		goto bail_cc;
	}

	ret = sysfs_create_bin_file(&ppd->pport_cc_kobj, &cc_table_bin_attr);
	if (ret) {
		dd_dev_err(dd,
			   "Skipping Congestion Control table sysfs info, (err %d) port %u\n",
			   ret, port_num);
		goto bail_cc_entry_bin;
	}

	dd_dev_info(dd,
		    "Congestion Control Agent enabled for port %d\n",
		    port_num);

	return 0;

bail_cc_entry_bin:
	sysfs_remove_bin_file(&ppd->pport_cc_kobj,
			      &cc_setting_bin_attr);
bail_cc:
	kobject_put(&ppd->pport_cc_kobj);
bail_vl2mtu:
	kobject_put(&ppd->vl2mtu_kobj);
bail_sl2sc:
	kobject_put(&ppd->sl2sc_kobj);
bail_sc2vl:
	kobject_put(&ppd->sc2vl_kobj);
	return ret;
}

struct sde_attribute {
	struct attribute attr;
	ssize_t (*show)(struct sdma_engine *sde, char *buf);
	ssize_t (*store)(struct sdma_engine *sde, const char *buf, size_t cnt);
};

static ssize_t sde_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct sde_attribute *sde_attr =
		container_of(attr, struct sde_attribute, attr);
	struct sdma_engine *sde =
		container_of(kobj, struct sdma_engine, kobj);

	if (!sde_attr->show)
		return -EINVAL;

	return sde_attr->show(sde, buf);
}

static ssize_t sde_store(struct kobject *kobj, struct attribute *attr,
			 const char *buf, size_t count)
{
	struct sde_attribute *sde_attr =
		container_of(attr, struct sde_attribute, attr);
	struct sdma_engine *sde =
		container_of(kobj, struct sdma_engine, kobj);

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (!sde_attr->store)
		return -EINVAL;

	return sde_attr->store(sde, buf, count);
}

static const struct sysfs_ops sde_sysfs_ops = {
	.show = sde_show,
	.store = sde_store,
};

static struct kobj_type sde_ktype = {
	.sysfs_ops = &sde_sysfs_ops,
};

#define SDE_ATTR(_name, _mode, _show, _store) \
	struct sde_attribute sde_attr_##_name = \
		__ATTR(_name, _mode, _show, _store)

static ssize_t sde_show_cpu_to_sde_map(struct sdma_engine *sde, char *buf)
{
	return sdma_get_cpu_to_sde_map(sde, buf);
}

static ssize_t sde_store_cpu_to_sde_map(struct sdma_engine *sde,
					const char *buf, size_t count)
{
	return sdma_set_cpu_to_sde_map(sde, buf, count);
}

static ssize_t sde_show_vl(struct sdma_engine *sde, char *buf)
{
	int vl;

	vl = sdma_engine_get_vl(sde);
	if (vl < 0)
		return vl;

	return sysfs_emit(buf, "%d\n", vl);
}

static SDE_ATTR(cpu_list, S_IWUSR | S_IRUGO,
		sde_show_cpu_to_sde_map,
		sde_store_cpu_to_sde_map);
static SDE_ATTR(vl, S_IRUGO, sde_show_vl, NULL);

static struct sde_attribute *sde_attribs[] = {
	&sde_attr_cpu_list,
	&sde_attr_vl
};

/*
 * Register and create our files in /sys/class/infiniband.
 */
int hfi1_verbs_register_sysfs(struct hfi1_devdata *dd)
{
	struct ib_device *dev = &dd->verbs_dev.rdi.ibdev;
	struct device *class_dev = &dev->dev;
	int i, j, ret;

	for (i = 0; i < dd->num_sdma; i++) {
		ret = kobject_init_and_add(&dd->per_sdma[i].kobj,
					   &sde_ktype, &class_dev->kobj,
					   "sdma%d", i);
		if (ret)
			goto bail;

		for (j = 0; j < ARRAY_SIZE(sde_attribs); j++) {
			ret = sysfs_create_file(&dd->per_sdma[i].kobj,
						&sde_attribs[j]->attr);
			if (ret)
				goto bail;
		}
	}

	return 0;
bail:
	/*
	 * The function kobject_put() will call kobject_del() if the kobject
	 * has been added successfully. The sysfs files created under the
	 * kobject directory will also be removed during the process.
	 */
	for (; i >= 0; i--)
		kobject_put(&dd->per_sdma[i].kobj);

	return ret;
}

/*
 * Unregister and remove our files in /sys/class/infiniband.
 */
void hfi1_verbs_unregister_sysfs(struct hfi1_devdata *dd)
{
	struct hfi1_pportdata *ppd;
	int i;

	/* Unwind operations in hfi1_verbs_register_sysfs() */
	for (i = 0; i < dd->num_sdma; i++)
		kobject_put(&dd->per_sdma[i].kobj);

	for (i = 0; i < dd->num_pports; i++) {
		ppd = &dd->pport[i];

		sysfs_remove_bin_file(&ppd->pport_cc_kobj,
				      &cc_setting_bin_attr);
		sysfs_remove_bin_file(&ppd->pport_cc_kobj,
				      &cc_table_bin_attr);
		kobject_put(&ppd->pport_cc_kobj);
		kobject_put(&ppd->vl2mtu_kobj);
		kobject_put(&ppd->sl2sc_kobj);
		kobject_put(&ppd->sc2vl_kobj);
	}
}

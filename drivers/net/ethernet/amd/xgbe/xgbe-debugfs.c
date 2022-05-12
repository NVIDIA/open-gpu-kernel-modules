/*
 * AMD 10Gb Ethernet driver
 *
 * This file is available to you under your choice of the following two
 * licenses:
 *
 * License 1: GPLv2
 *
 * Copyright (c) 2014 Advanced Micro Devices, Inc.
 *
 * This file is free software; you may copy, redistribute and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or (at
 * your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *     The Synopsys DWC ETHER XGMAC Software Driver and documentation
 *     (hereinafter "Software") is an unsupported proprietary work of Synopsys,
 *     Inc. unless otherwise expressly agreed to in writing between Synopsys
 *     and you.
 *
 *     The Software IS NOT an item of Licensed Software or Licensed Product
 *     under any End User Software License Agreement or Agreement for Licensed
 *     Product with Synopsys or any supplement thereto.  Permission is hereby
 *     granted, free of charge, to any person obtaining a copy of this software
 *     annotated with this license and the Software, to deal in the Software
 *     without restriction, including without limitation the rights to use,
 *     copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *     of the Software, and to permit persons to whom the Software is furnished
 *     to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 *     in all copies or substantial portions of the Software.
 *
 *     THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS"
 *     BASIS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *     TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *     PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS
 *     BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *     THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * License 2: Modified BSD
 *
 * Copyright (c) 2014 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *     The Synopsys DWC ETHER XGMAC Software Driver and documentation
 *     (hereinafter "Software") is an unsupported proprietary work of Synopsys,
 *     Inc. unless otherwise expressly agreed to in writing between Synopsys
 *     and you.
 *
 *     The Software IS NOT an item of Licensed Software or Licensed Product
 *     under any End User Software License Agreement or Agreement for Licensed
 *     Product with Synopsys or any supplement thereto.  Permission is hereby
 *     granted, free of charge, to any person obtaining a copy of this software
 *     annotated with this license and the Software, to deal in the Software
 *     without restriction, including without limitation the rights to use,
 *     copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *     of the Software, and to permit persons to whom the Software is furnished
 *     to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 *     in all copies or substantial portions of the Software.
 *
 *     THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS"
 *     BASIS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *     TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *     PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS
 *     BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *     THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "xgbe.h"
#include "xgbe-common.h"

static ssize_t xgbe_common_read(char __user *buffer, size_t count,
				loff_t *ppos, unsigned int value)
{
	char *buf;
	ssize_t len;

	if (*ppos != 0)
		return 0;

	buf = kasprintf(GFP_KERNEL, "0x%08x\n", value);
	if (!buf)
		return -ENOMEM;

	if (count < strlen(buf)) {
		kfree(buf);
		return -ENOSPC;
	}

	len = simple_read_from_buffer(buffer, count, ppos, buf, strlen(buf));
	kfree(buf);

	return len;
}

static ssize_t xgbe_common_write(const char __user *buffer, size_t count,
				 loff_t *ppos, unsigned int *value)
{
	char workarea[32];
	ssize_t len;
	int ret;

	if (*ppos != 0)
		return -EINVAL;

	if (count >= sizeof(workarea))
		return -ENOSPC;

	len = simple_write_to_buffer(workarea, sizeof(workarea) - 1, ppos,
				     buffer, count);
	if (len < 0)
		return len;

	workarea[len] = '\0';
	ret = kstrtouint(workarea, 16, value);
	if (ret)
		return -EIO;

	return len;
}

static ssize_t xgmac_reg_addr_read(struct file *filp, char __user *buffer,
				   size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_read(buffer, count, ppos, pdata->debugfs_xgmac_reg);
}

static ssize_t xgmac_reg_addr_write(struct file *filp,
				    const char __user *buffer,
				    size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_write(buffer, count, ppos,
				 &pdata->debugfs_xgmac_reg);
}

static ssize_t xgmac_reg_value_read(struct file *filp, char __user *buffer,
				    size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;

	value = XGMAC_IOREAD(pdata, pdata->debugfs_xgmac_reg);

	return xgbe_common_read(buffer, count, ppos, value);
}

static ssize_t xgmac_reg_value_write(struct file *filp,
				     const char __user *buffer,
				     size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;
	ssize_t len;

	len = xgbe_common_write(buffer, count, ppos, &value);
	if (len < 0)
		return len;

	XGMAC_IOWRITE(pdata, pdata->debugfs_xgmac_reg, value);

	return len;
}

static const struct file_operations xgmac_reg_addr_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xgmac_reg_addr_read,
	.write = xgmac_reg_addr_write,
};

static const struct file_operations xgmac_reg_value_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xgmac_reg_value_read,
	.write = xgmac_reg_value_write,
};

static ssize_t xpcs_mmd_read(struct file *filp, char __user *buffer,
			     size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_read(buffer, count, ppos, pdata->debugfs_xpcs_mmd);
}

static ssize_t xpcs_mmd_write(struct file *filp, const char __user *buffer,
			      size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_write(buffer, count, ppos,
				 &pdata->debugfs_xpcs_mmd);
}

static ssize_t xpcs_reg_addr_read(struct file *filp, char __user *buffer,
				  size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_read(buffer, count, ppos, pdata->debugfs_xpcs_reg);
}

static ssize_t xpcs_reg_addr_write(struct file *filp, const char __user *buffer,
				   size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_write(buffer, count, ppos,
				 &pdata->debugfs_xpcs_reg);
}

static ssize_t xpcs_reg_value_read(struct file *filp, char __user *buffer,
				   size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;

	value = XMDIO_READ(pdata, pdata->debugfs_xpcs_mmd,
			   pdata->debugfs_xpcs_reg);

	return xgbe_common_read(buffer, count, ppos, value);
}

static ssize_t xpcs_reg_value_write(struct file *filp,
				    const char __user *buffer,
				    size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;
	ssize_t len;

	len = xgbe_common_write(buffer, count, ppos, &value);
	if (len < 0)
		return len;

	XMDIO_WRITE(pdata, pdata->debugfs_xpcs_mmd, pdata->debugfs_xpcs_reg,
		    value);

	return len;
}

static const struct file_operations xpcs_mmd_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xpcs_mmd_read,
	.write = xpcs_mmd_write,
};

static const struct file_operations xpcs_reg_addr_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xpcs_reg_addr_read,
	.write = xpcs_reg_addr_write,
};

static const struct file_operations xpcs_reg_value_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xpcs_reg_value_read,
	.write = xpcs_reg_value_write,
};

static ssize_t xprop_reg_addr_read(struct file *filp, char __user *buffer,
				   size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_read(buffer, count, ppos, pdata->debugfs_xprop_reg);
}

static ssize_t xprop_reg_addr_write(struct file *filp,
				    const char __user *buffer,
				    size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_write(buffer, count, ppos,
				 &pdata->debugfs_xprop_reg);
}

static ssize_t xprop_reg_value_read(struct file *filp, char __user *buffer,
				    size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;

	value = XP_IOREAD(pdata, pdata->debugfs_xprop_reg);

	return xgbe_common_read(buffer, count, ppos, value);
}

static ssize_t xprop_reg_value_write(struct file *filp,
				     const char __user *buffer,
				     size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;
	ssize_t len;

	len = xgbe_common_write(buffer, count, ppos, &value);
	if (len < 0)
		return len;

	XP_IOWRITE(pdata, pdata->debugfs_xprop_reg, value);

	return len;
}

static const struct file_operations xprop_reg_addr_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xprop_reg_addr_read,
	.write = xprop_reg_addr_write,
};

static const struct file_operations xprop_reg_value_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xprop_reg_value_read,
	.write = xprop_reg_value_write,
};

static ssize_t xi2c_reg_addr_read(struct file *filp, char __user *buffer,
				  size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_read(buffer, count, ppos, pdata->debugfs_xi2c_reg);
}

static ssize_t xi2c_reg_addr_write(struct file *filp,
				   const char __user *buffer,
				   size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;

	return xgbe_common_write(buffer, count, ppos,
				 &pdata->debugfs_xi2c_reg);
}

static ssize_t xi2c_reg_value_read(struct file *filp, char __user *buffer,
				   size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;

	value = XI2C_IOREAD(pdata, pdata->debugfs_xi2c_reg);

	return xgbe_common_read(buffer, count, ppos, value);
}

static ssize_t xi2c_reg_value_write(struct file *filp,
				    const char __user *buffer,
				    size_t count, loff_t *ppos)
{
	struct xgbe_prv_data *pdata = filp->private_data;
	unsigned int value;
	ssize_t len;

	len = xgbe_common_write(buffer, count, ppos, &value);
	if (len < 0)
		return len;

	XI2C_IOWRITE(pdata, pdata->debugfs_xi2c_reg, value);

	return len;
}

static const struct file_operations xi2c_reg_addr_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xi2c_reg_addr_read,
	.write = xi2c_reg_addr_write,
};

static const struct file_operations xi2c_reg_value_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read =  xi2c_reg_value_read,
	.write = xi2c_reg_value_write,
};

void xgbe_debugfs_init(struct xgbe_prv_data *pdata)
{
	char *buf;

	/* Set defaults */
	pdata->debugfs_xgmac_reg = 0;
	pdata->debugfs_xpcs_mmd = 1;
	pdata->debugfs_xpcs_reg = 0;

	buf = kasprintf(GFP_KERNEL, "amd-xgbe-%s", pdata->netdev->name);
	if (!buf)
		return;

	pdata->xgbe_debugfs = debugfs_create_dir(buf, NULL);

	debugfs_create_file("xgmac_register", 0600, pdata->xgbe_debugfs, pdata,
			    &xgmac_reg_addr_fops);

	debugfs_create_file("xgmac_register_value", 0600, pdata->xgbe_debugfs,
			    pdata, &xgmac_reg_value_fops);

	debugfs_create_file("xpcs_mmd", 0600, pdata->xgbe_debugfs, pdata,
			    &xpcs_mmd_fops);

	debugfs_create_file("xpcs_register", 0600, pdata->xgbe_debugfs, pdata,
			    &xpcs_reg_addr_fops);

	debugfs_create_file("xpcs_register_value", 0600, pdata->xgbe_debugfs,
			    pdata, &xpcs_reg_value_fops);

	if (pdata->xprop_regs) {
		debugfs_create_file("xprop_register", 0600, pdata->xgbe_debugfs,
				    pdata, &xprop_reg_addr_fops);

		debugfs_create_file("xprop_register_value", 0600,
				    pdata->xgbe_debugfs, pdata,
				    &xprop_reg_value_fops);
	}

	if (pdata->xi2c_regs) {
		debugfs_create_file("xi2c_register", 0600, pdata->xgbe_debugfs,
				    pdata, &xi2c_reg_addr_fops);

		debugfs_create_file("xi2c_register_value", 0600,
				    pdata->xgbe_debugfs, pdata,
				    &xi2c_reg_value_fops);
	}

	if (pdata->vdata->an_cdr_workaround) {
		debugfs_create_bool("an_cdr_workaround", 0600,
				    pdata->xgbe_debugfs,
				    &pdata->debugfs_an_cdr_workaround);

		debugfs_create_bool("an_cdr_track_early", 0600,
				    pdata->xgbe_debugfs,
				    &pdata->debugfs_an_cdr_track_early);
	}

	kfree(buf);
}

void xgbe_debugfs_exit(struct xgbe_prv_data *pdata)
{
	debugfs_remove_recursive(pdata->xgbe_debugfs);
	pdata->xgbe_debugfs = NULL;
}

void xgbe_debugfs_rename(struct xgbe_prv_data *pdata)
{
	char *buf;

	if (!pdata->xgbe_debugfs)
		return;

	buf = kasprintf(GFP_KERNEL, "amd-xgbe-%s", pdata->netdev->name);
	if (!buf)
		return;

	if (!strcmp(pdata->xgbe_debugfs->d_name.name, buf))
		goto out;

	debugfs_rename(pdata->xgbe_debugfs->d_parent, pdata->xgbe_debugfs,
		       pdata->xgbe_debugfs->d_parent, buf);

out:
	kfree(buf);
}

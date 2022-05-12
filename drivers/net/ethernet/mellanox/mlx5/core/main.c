/*
 * Copyright (c) 2013-2015, Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/highmem.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/io-mapping.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/mlx5/driver.h>
#include <linux/mlx5/cq.h>
#include <linux/mlx5/qp.h>
#include <linux/debugfs.h>
#include <linux/kmod.h>
#include <linux/mlx5/mlx5_ifc.h>
#include <linux/mlx5/vport.h>
#ifdef CONFIG_RFS_ACCEL
#include <linux/cpu_rmap.h>
#endif
#include <linux/version.h>
#include <net/devlink.h>
#include "mlx5_core.h"
#include "lib/eq.h"
#include "fs_core.h"
#include "lib/mpfs.h"
#include "eswitch.h"
#include "devlink.h"
#include "fw_reset.h"
#include "lib/mlx5.h"
#include "fpga/core.h"
#include "fpga/ipsec.h"
#include "accel/ipsec.h"
#include "accel/tls.h"
#include "lib/clock.h"
#include "lib/vxlan.h"
#include "lib/geneve.h"
#include "lib/devcom.h"
#include "lib/pci_vsc.h"
#include "diag/fw_tracer.h"
#include "ecpf.h"
#include "lib/hv_vhca.h"
#include "diag/rsc_dump.h"
#include "sf/vhca_event.h"
#include "sf/dev/dev.h"
#include "sf/sf.h"

MODULE_AUTHOR("Eli Cohen <eli@mellanox.com>");
MODULE_DESCRIPTION("Mellanox 5th generation network adapters (ConnectX series) core driver");
MODULE_LICENSE("Dual BSD/GPL");

unsigned int mlx5_core_debug_mask;
module_param_named(debug_mask, mlx5_core_debug_mask, uint, 0644);
MODULE_PARM_DESC(debug_mask, "debug mask: 1 = dump cmd data, 2 = dump cmd exec time, 3 = both. Default=0");

static unsigned int prof_sel = MLX5_DEFAULT_PROF;
module_param_named(prof_sel, prof_sel, uint, 0444);
MODULE_PARM_DESC(prof_sel, "profile selector. Valid range 0 - 2");

static u32 sw_owner_id[4];

enum {
	MLX5_ATOMIC_REQ_MODE_BE = 0x0,
	MLX5_ATOMIC_REQ_MODE_HOST_ENDIANNESS = 0x1,
};

static struct mlx5_profile profile[] = {
	[0] = {
		.mask           = 0,
	},
	[1] = {
		.mask		= MLX5_PROF_MASK_QP_SIZE,
		.log_max_qp	= 12,
	},
	[2] = {
		.mask		= MLX5_PROF_MASK_QP_SIZE |
				  MLX5_PROF_MASK_MR_CACHE,
		.log_max_qp	= 18,
		.mr_cache[0]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[1]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[2]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[3]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[4]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[5]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[6]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[7]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[8]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[9]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[10]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[11]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[12]	= {
			.size	= 64,
			.limit	= 32
		},
		.mr_cache[13]	= {
			.size	= 32,
			.limit	= 16
		},
		.mr_cache[14]	= {
			.size	= 16,
			.limit	= 8
		},
		.mr_cache[15]	= {
			.size	= 8,
			.limit	= 4
		},
	},
};

#define FW_INIT_TIMEOUT_MILI		2000
#define FW_INIT_WAIT_MS			2
#define FW_PRE_INIT_TIMEOUT_MILI	120000
#define FW_INIT_WARN_MESSAGE_INTERVAL	20000

static int fw_initializing(struct mlx5_core_dev *dev)
{
	return ioread32be(&dev->iseg->initializing) >> 31;
}

static int wait_fw_init(struct mlx5_core_dev *dev, u32 max_wait_mili,
			u32 warn_time_mili)
{
	unsigned long warn = jiffies + msecs_to_jiffies(warn_time_mili);
	unsigned long end = jiffies + msecs_to_jiffies(max_wait_mili);
	int err = 0;

	BUILD_BUG_ON(FW_PRE_INIT_TIMEOUT_MILI < FW_INIT_WARN_MESSAGE_INTERVAL);

	while (fw_initializing(dev)) {
		if (time_after(jiffies, end)) {
			err = -EBUSY;
			break;
		}
		if (warn_time_mili && time_after(jiffies, warn)) {
			mlx5_core_warn(dev, "Waiting for FW initialization, timeout abort in %ds\n",
				       jiffies_to_msecs(end - warn) / 1000);
			warn = jiffies + msecs_to_jiffies(warn_time_mili);
		}
		msleep(FW_INIT_WAIT_MS);
	}

	return err;
}

static void mlx5_set_driver_version(struct mlx5_core_dev *dev)
{
	int driver_ver_sz = MLX5_FLD_SZ_BYTES(set_driver_version_in,
					      driver_version);
	u8 in[MLX5_ST_SZ_BYTES(set_driver_version_in)] = {};
	int remaining_size = driver_ver_sz;
	char *string;

	if (!MLX5_CAP_GEN(dev, driver_version))
		return;

	string = MLX5_ADDR_OF(set_driver_version_in, in, driver_version);

	strncpy(string, "Linux", remaining_size);

	remaining_size = max_t(int, 0, driver_ver_sz - strlen(string));
	strncat(string, ",", remaining_size);

	remaining_size = max_t(int, 0, driver_ver_sz - strlen(string));
	strncat(string, KBUILD_MODNAME, remaining_size);

	remaining_size = max_t(int, 0, driver_ver_sz - strlen(string));
	strncat(string, ",", remaining_size);

	remaining_size = max_t(int, 0, driver_ver_sz - strlen(string));

	snprintf(string + strlen(string), remaining_size, "%u.%u.%u",
		LINUX_VERSION_MAJOR, LINUX_VERSION_PATCHLEVEL,
		LINUX_VERSION_SUBLEVEL);

	/*Send the command*/
	MLX5_SET(set_driver_version_in, in, opcode,
		 MLX5_CMD_OP_SET_DRIVER_VERSION);

	mlx5_cmd_exec_in(dev, set_driver_version, in);
}

static int set_dma_caps(struct pci_dev *pdev)
{
	int err;

	err = pci_set_dma_mask(pdev, DMA_BIT_MASK(64));
	if (err) {
		dev_warn(&pdev->dev, "Warning: couldn't set 64-bit PCI DMA mask\n");
		err = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev, "Can't set PCI DMA mask, aborting\n");
			return err;
		}
	}

	err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(64));
	if (err) {
		dev_warn(&pdev->dev,
			 "Warning: couldn't set 64-bit consistent PCI DMA mask\n");
		err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev,
				"Can't set consistent PCI DMA mask, aborting\n");
			return err;
		}
	}

	dma_set_max_seg_size(&pdev->dev, 2u * 1024 * 1024 * 1024);
	return err;
}

static int mlx5_pci_enable_device(struct mlx5_core_dev *dev)
{
	struct pci_dev *pdev = dev->pdev;
	int err = 0;

	mutex_lock(&dev->pci_status_mutex);
	if (dev->pci_status == MLX5_PCI_STATUS_DISABLED) {
		err = pci_enable_device(pdev);
		if (!err)
			dev->pci_status = MLX5_PCI_STATUS_ENABLED;
	}
	mutex_unlock(&dev->pci_status_mutex);

	return err;
}

static void mlx5_pci_disable_device(struct mlx5_core_dev *dev)
{
	struct pci_dev *pdev = dev->pdev;

	mutex_lock(&dev->pci_status_mutex);
	if (dev->pci_status == MLX5_PCI_STATUS_ENABLED) {
		pci_disable_device(pdev);
		dev->pci_status = MLX5_PCI_STATUS_DISABLED;
	}
	mutex_unlock(&dev->pci_status_mutex);
}

static int request_bar(struct pci_dev *pdev)
{
	int err = 0;

	if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
		dev_err(&pdev->dev, "Missing registers BAR, aborting\n");
		return -ENODEV;
	}

	err = pci_request_regions(pdev, KBUILD_MODNAME);
	if (err)
		dev_err(&pdev->dev, "Couldn't get PCI resources, aborting\n");

	return err;
}

static void release_bar(struct pci_dev *pdev)
{
	pci_release_regions(pdev);
}

struct mlx5_reg_host_endianness {
	u8	he;
	u8      rsvd[15];
};

#define CAP_MASK(pos, size) ((u64)((1 << (size)) - 1) << (pos))

enum {
	MLX5_CAP_BITS_RW_MASK = CAP_MASK(MLX5_CAP_OFF_CMDIF_CSUM, 2) |
				MLX5_DEV_CAP_FLAG_DCT,
};

static u16 to_fw_pkey_sz(struct mlx5_core_dev *dev, u32 size)
{
	switch (size) {
	case 128:
		return 0;
	case 256:
		return 1;
	case 512:
		return 2;
	case 1024:
		return 3;
	case 2048:
		return 4;
	case 4096:
		return 5;
	default:
		mlx5_core_warn(dev, "invalid pkey table size %d\n", size);
		return 0;
	}
}

static int mlx5_core_get_caps_mode(struct mlx5_core_dev *dev,
				   enum mlx5_cap_type cap_type,
				   enum mlx5_cap_mode cap_mode)
{
	u8 in[MLX5_ST_SZ_BYTES(query_hca_cap_in)];
	int out_sz = MLX5_ST_SZ_BYTES(query_hca_cap_out);
	void *out, *hca_caps;
	u16 opmod = (cap_type << 1) | (cap_mode & 0x01);
	int err;

	memset(in, 0, sizeof(in));
	out = kzalloc(out_sz, GFP_KERNEL);
	if (!out)
		return -ENOMEM;

	MLX5_SET(query_hca_cap_in, in, opcode, MLX5_CMD_OP_QUERY_HCA_CAP);
	MLX5_SET(query_hca_cap_in, in, op_mod, opmod);
	err = mlx5_cmd_exec_inout(dev, query_hca_cap, in, out);
	if (err) {
		mlx5_core_warn(dev,
			       "QUERY_HCA_CAP : type(%x) opmode(%x) Failed(%d)\n",
			       cap_type, cap_mode, err);
		goto query_ex;
	}

	hca_caps =  MLX5_ADDR_OF(query_hca_cap_out, out, capability);

	switch (cap_mode) {
	case HCA_CAP_OPMOD_GET_MAX:
		memcpy(dev->caps.hca_max[cap_type], hca_caps,
		       MLX5_UN_SZ_BYTES(hca_cap_union));
		break;
	case HCA_CAP_OPMOD_GET_CUR:
		memcpy(dev->caps.hca_cur[cap_type], hca_caps,
		       MLX5_UN_SZ_BYTES(hca_cap_union));
		break;
	default:
		mlx5_core_warn(dev,
			       "Tried to query dev cap type(%x) with wrong opmode(%x)\n",
			       cap_type, cap_mode);
		err = -EINVAL;
		break;
	}
query_ex:
	kfree(out);
	return err;
}

int mlx5_core_get_caps(struct mlx5_core_dev *dev, enum mlx5_cap_type cap_type)
{
	int ret;

	ret = mlx5_core_get_caps_mode(dev, cap_type, HCA_CAP_OPMOD_GET_CUR);
	if (ret)
		return ret;
	return mlx5_core_get_caps_mode(dev, cap_type, HCA_CAP_OPMOD_GET_MAX);
}

static int set_caps(struct mlx5_core_dev *dev, void *in, int opmod)
{
	MLX5_SET(set_hca_cap_in, in, opcode, MLX5_CMD_OP_SET_HCA_CAP);
	MLX5_SET(set_hca_cap_in, in, op_mod, opmod << 1);
	return mlx5_cmd_exec_in(dev, set_hca_cap, in);
}

static int handle_hca_cap_atomic(struct mlx5_core_dev *dev, void *set_ctx)
{
	void *set_hca_cap;
	int req_endianness;
	int err;

	if (!MLX5_CAP_GEN(dev, atomic))
		return 0;

	err = mlx5_core_get_caps(dev, MLX5_CAP_ATOMIC);
	if (err)
		return err;

	req_endianness =
		MLX5_CAP_ATOMIC(dev,
				supported_atomic_req_8B_endianness_mode_1);

	if (req_endianness != MLX5_ATOMIC_REQ_MODE_HOST_ENDIANNESS)
		return 0;

	set_hca_cap = MLX5_ADDR_OF(set_hca_cap_in, set_ctx, capability);

	/* Set requestor to host endianness */
	MLX5_SET(atomic_caps, set_hca_cap, atomic_req_8B_endianness_mode,
		 MLX5_ATOMIC_REQ_MODE_HOST_ENDIANNESS);

	return set_caps(dev, set_ctx, MLX5_SET_HCA_CAP_OP_MOD_ATOMIC);
}

static int handle_hca_cap_odp(struct mlx5_core_dev *dev, void *set_ctx)
{
	void *set_hca_cap;
	bool do_set = false;
	int err;

	if (!IS_ENABLED(CONFIG_INFINIBAND_ON_DEMAND_PAGING) ||
	    !MLX5_CAP_GEN(dev, pg))
		return 0;

	err = mlx5_core_get_caps(dev, MLX5_CAP_ODP);
	if (err)
		return err;

	set_hca_cap = MLX5_ADDR_OF(set_hca_cap_in, set_ctx, capability);
	memcpy(set_hca_cap, dev->caps.hca_cur[MLX5_CAP_ODP],
	       MLX5_ST_SZ_BYTES(odp_cap));

#define ODP_CAP_SET_MAX(dev, field)                                            \
	do {                                                                   \
		u32 _res = MLX5_CAP_ODP_MAX(dev, field);                       \
		if (_res) {                                                    \
			do_set = true;                                         \
			MLX5_SET(odp_cap, set_hca_cap, field, _res);           \
		}                                                              \
	} while (0)

	ODP_CAP_SET_MAX(dev, ud_odp_caps.srq_receive);
	ODP_CAP_SET_MAX(dev, rc_odp_caps.srq_receive);
	ODP_CAP_SET_MAX(dev, xrc_odp_caps.srq_receive);
	ODP_CAP_SET_MAX(dev, xrc_odp_caps.send);
	ODP_CAP_SET_MAX(dev, xrc_odp_caps.receive);
	ODP_CAP_SET_MAX(dev, xrc_odp_caps.write);
	ODP_CAP_SET_MAX(dev, xrc_odp_caps.read);
	ODP_CAP_SET_MAX(dev, xrc_odp_caps.atomic);
	ODP_CAP_SET_MAX(dev, dc_odp_caps.srq_receive);
	ODP_CAP_SET_MAX(dev, dc_odp_caps.send);
	ODP_CAP_SET_MAX(dev, dc_odp_caps.receive);
	ODP_CAP_SET_MAX(dev, dc_odp_caps.write);
	ODP_CAP_SET_MAX(dev, dc_odp_caps.read);
	ODP_CAP_SET_MAX(dev, dc_odp_caps.atomic);

	if (!do_set)
		return 0;

	return set_caps(dev, set_ctx, MLX5_SET_HCA_CAP_OP_MOD_ODP);
}

static int handle_hca_cap(struct mlx5_core_dev *dev, void *set_ctx)
{
	struct mlx5_profile *prof = &dev->profile;
	void *set_hca_cap;
	int err;

	err = mlx5_core_get_caps(dev, MLX5_CAP_GENERAL);
	if (err)
		return err;

	set_hca_cap = MLX5_ADDR_OF(set_hca_cap_in, set_ctx,
				   capability);
	memcpy(set_hca_cap, dev->caps.hca_cur[MLX5_CAP_GENERAL],
	       MLX5_ST_SZ_BYTES(cmd_hca_cap));

	mlx5_core_dbg(dev, "Current Pkey table size %d Setting new size %d\n",
		      mlx5_to_sw_pkey_sz(MLX5_CAP_GEN(dev, pkey_table_size)),
		      128);
	/* we limit the size of the pkey table to 128 entries for now */
	MLX5_SET(cmd_hca_cap, set_hca_cap, pkey_table_size,
		 to_fw_pkey_sz(dev, 128));

	/* Check log_max_qp from HCA caps to set in current profile */
	if (MLX5_CAP_GEN_MAX(dev, log_max_qp) < prof->log_max_qp) {
		mlx5_core_warn(dev, "log_max_qp value in current profile is %d, changing it to HCA capability limit (%d)\n",
			       prof->log_max_qp,
			       MLX5_CAP_GEN_MAX(dev, log_max_qp));
		prof->log_max_qp = MLX5_CAP_GEN_MAX(dev, log_max_qp);
	}
	if (prof->mask & MLX5_PROF_MASK_QP_SIZE)
		MLX5_SET(cmd_hca_cap, set_hca_cap, log_max_qp,
			 prof->log_max_qp);

	/* disable cmdif checksum */
	MLX5_SET(cmd_hca_cap, set_hca_cap, cmdif_checksum, 0);

	/* Enable 4K UAR only when HCA supports it and page size is bigger
	 * than 4K.
	 */
	if (MLX5_CAP_GEN_MAX(dev, uar_4k) && PAGE_SIZE > 4096)
		MLX5_SET(cmd_hca_cap, set_hca_cap, uar_4k, 1);

	MLX5_SET(cmd_hca_cap, set_hca_cap, log_uar_page_sz, PAGE_SHIFT - 12);

	if (MLX5_CAP_GEN_MAX(dev, cache_line_128byte))
		MLX5_SET(cmd_hca_cap,
			 set_hca_cap,
			 cache_line_128byte,
			 cache_line_size() >= 128 ? 1 : 0);

	if (MLX5_CAP_GEN_MAX(dev, dct))
		MLX5_SET(cmd_hca_cap, set_hca_cap, dct, 1);

	if (MLX5_CAP_GEN_MAX(dev, pci_sync_for_fw_update_event))
		MLX5_SET(cmd_hca_cap, set_hca_cap, pci_sync_for_fw_update_event, 1);

	if (MLX5_CAP_GEN_MAX(dev, num_vhca_ports))
		MLX5_SET(cmd_hca_cap,
			 set_hca_cap,
			 num_vhca_ports,
			 MLX5_CAP_GEN_MAX(dev, num_vhca_ports));

	if (MLX5_CAP_GEN_MAX(dev, release_all_pages))
		MLX5_SET(cmd_hca_cap, set_hca_cap, release_all_pages, 1);

	if (MLX5_CAP_GEN_MAX(dev, mkey_by_name))
		MLX5_SET(cmd_hca_cap, set_hca_cap, mkey_by_name, 1);

	mlx5_vhca_state_cap_handle(dev, set_hca_cap);

	if (MLX5_CAP_GEN_MAX(dev, num_total_dynamic_vf_msix))
		MLX5_SET(cmd_hca_cap, set_hca_cap, num_total_dynamic_vf_msix,
			 MLX5_CAP_GEN_MAX(dev, num_total_dynamic_vf_msix));

	return set_caps(dev, set_ctx, MLX5_SET_HCA_CAP_OP_MOD_GENERAL_DEVICE);
}

static int handle_hca_cap_roce(struct mlx5_core_dev *dev, void *set_ctx)
{
	void *set_hca_cap;
	int err;

	if (!MLX5_CAP_GEN(dev, roce))
		return 0;

	err = mlx5_core_get_caps(dev, MLX5_CAP_ROCE);
	if (err)
		return err;

	if (MLX5_CAP_ROCE(dev, sw_r_roce_src_udp_port) ||
	    !MLX5_CAP_ROCE_MAX(dev, sw_r_roce_src_udp_port))
		return 0;

	set_hca_cap = MLX5_ADDR_OF(set_hca_cap_in, set_ctx, capability);
	memcpy(set_hca_cap, dev->caps.hca_cur[MLX5_CAP_ROCE],
	       MLX5_ST_SZ_BYTES(roce_cap));
	MLX5_SET(roce_cap, set_hca_cap, sw_r_roce_src_udp_port, 1);

	err = set_caps(dev, set_ctx, MLX5_SET_HCA_CAP_OP_MOD_ROCE);
	return err;
}

static int set_hca_cap(struct mlx5_core_dev *dev)
{
	int set_sz = MLX5_ST_SZ_BYTES(set_hca_cap_in);
	void *set_ctx;
	int err;

	set_ctx = kzalloc(set_sz, GFP_KERNEL);
	if (!set_ctx)
		return -ENOMEM;

	err = handle_hca_cap(dev, set_ctx);
	if (err) {
		mlx5_core_err(dev, "handle_hca_cap failed\n");
		goto out;
	}

	memset(set_ctx, 0, set_sz);
	err = handle_hca_cap_atomic(dev, set_ctx);
	if (err) {
		mlx5_core_err(dev, "handle_hca_cap_atomic failed\n");
		goto out;
	}

	memset(set_ctx, 0, set_sz);
	err = handle_hca_cap_odp(dev, set_ctx);
	if (err) {
		mlx5_core_err(dev, "handle_hca_cap_odp failed\n");
		goto out;
	}

	memset(set_ctx, 0, set_sz);
	err = handle_hca_cap_roce(dev, set_ctx);
	if (err) {
		mlx5_core_err(dev, "handle_hca_cap_roce failed\n");
		goto out;
	}

out:
	kfree(set_ctx);
	return err;
}

static int set_hca_ctrl(struct mlx5_core_dev *dev)
{
	struct mlx5_reg_host_endianness he_in;
	struct mlx5_reg_host_endianness he_out;
	int err;

	if (!mlx5_core_is_pf(dev))
		return 0;

	memset(&he_in, 0, sizeof(he_in));
	he_in.he = MLX5_SET_HOST_ENDIANNESS;
	err = mlx5_core_access_reg(dev, &he_in,  sizeof(he_in),
					&he_out, sizeof(he_out),
					MLX5_REG_HOST_ENDIANNESS, 0, 1);
	return err;
}

static int mlx5_core_set_hca_defaults(struct mlx5_core_dev *dev)
{
	int ret = 0;

	/* Disable local_lb by default */
	if (MLX5_CAP_GEN(dev, port_type) == MLX5_CAP_PORT_TYPE_ETH)
		ret = mlx5_nic_vport_update_local_lb(dev, false);

	return ret;
}

int mlx5_core_enable_hca(struct mlx5_core_dev *dev, u16 func_id)
{
	u32 in[MLX5_ST_SZ_DW(enable_hca_in)] = {};

	MLX5_SET(enable_hca_in, in, opcode, MLX5_CMD_OP_ENABLE_HCA);
	MLX5_SET(enable_hca_in, in, function_id, func_id);
	MLX5_SET(enable_hca_in, in, embedded_cpu_function,
		 dev->caps.embedded_cpu);
	return mlx5_cmd_exec_in(dev, enable_hca, in);
}

int mlx5_core_disable_hca(struct mlx5_core_dev *dev, u16 func_id)
{
	u32 in[MLX5_ST_SZ_DW(disable_hca_in)] = {};

	MLX5_SET(disable_hca_in, in, opcode, MLX5_CMD_OP_DISABLE_HCA);
	MLX5_SET(disable_hca_in, in, function_id, func_id);
	MLX5_SET(enable_hca_in, in, embedded_cpu_function,
		 dev->caps.embedded_cpu);
	return mlx5_cmd_exec_in(dev, disable_hca, in);
}

static int mlx5_core_set_issi(struct mlx5_core_dev *dev)
{
	u32 query_out[MLX5_ST_SZ_DW(query_issi_out)] = {};
	u32 query_in[MLX5_ST_SZ_DW(query_issi_in)] = {};
	u32 sup_issi;
	int err;

	MLX5_SET(query_issi_in, query_in, opcode, MLX5_CMD_OP_QUERY_ISSI);
	err = mlx5_cmd_exec_inout(dev, query_issi, query_in, query_out);
	if (err) {
		u32 syndrome;
		u8 status;

		mlx5_cmd_mbox_status(query_out, &status, &syndrome);
		if (!status || syndrome == MLX5_DRIVER_SYND) {
			mlx5_core_err(dev, "Failed to query ISSI err(%d) status(%d) synd(%d)\n",
				      err, status, syndrome);
			return err;
		}

		mlx5_core_warn(dev, "Query ISSI is not supported by FW, ISSI is 0\n");
		dev->issi = 0;
		return 0;
	}

	sup_issi = MLX5_GET(query_issi_out, query_out, supported_issi_dw0);

	if (sup_issi & (1 << 1)) {
		u32 set_in[MLX5_ST_SZ_DW(set_issi_in)] = {};

		MLX5_SET(set_issi_in, set_in, opcode, MLX5_CMD_OP_SET_ISSI);
		MLX5_SET(set_issi_in, set_in, current_issi, 1);
		err = mlx5_cmd_exec_in(dev, set_issi, set_in);
		if (err) {
			mlx5_core_err(dev, "Failed to set ISSI to 1 err(%d)\n",
				      err);
			return err;
		}

		dev->issi = 1;

		return 0;
	} else if (sup_issi & (1 << 0) || !sup_issi) {
		return 0;
	}

	return -EOPNOTSUPP;
}

static int mlx5_pci_init(struct mlx5_core_dev *dev, struct pci_dev *pdev,
			 const struct pci_device_id *id)
{
	struct mlx5_priv *priv = &dev->priv;
	int err = 0;

	mutex_init(&dev->pci_status_mutex);
	pci_set_drvdata(dev->pdev, dev);

	dev->bar_addr = pci_resource_start(pdev, 0);
	priv->numa_node = dev_to_node(mlx5_core_dma_dev(dev));

	err = mlx5_pci_enable_device(dev);
	if (err) {
		mlx5_core_err(dev, "Cannot enable PCI device, aborting\n");
		return err;
	}

	err = request_bar(pdev);
	if (err) {
		mlx5_core_err(dev, "error requesting BARs, aborting\n");
		goto err_disable;
	}

	pci_set_master(pdev);

	err = set_dma_caps(pdev);
	if (err) {
		mlx5_core_err(dev, "Failed setting DMA capabilities mask, aborting\n");
		goto err_clr_master;
	}

	if (pci_enable_atomic_ops_to_root(pdev, PCI_EXP_DEVCAP2_ATOMIC_COMP32) &&
	    pci_enable_atomic_ops_to_root(pdev, PCI_EXP_DEVCAP2_ATOMIC_COMP64) &&
	    pci_enable_atomic_ops_to_root(pdev, PCI_EXP_DEVCAP2_ATOMIC_COMP128))
		mlx5_core_dbg(dev, "Enabling pci atomics failed\n");

	dev->iseg_base = dev->bar_addr;
	dev->iseg = ioremap(dev->iseg_base, sizeof(*dev->iseg));
	if (!dev->iseg) {
		err = -ENOMEM;
		mlx5_core_err(dev, "Failed mapping initialization segment, aborting\n");
		goto err_clr_master;
	}

	mlx5_pci_vsc_init(dev);
	dev->caps.embedded_cpu = mlx5_read_embedded_cpu(dev);
	return 0;

err_clr_master:
	pci_clear_master(dev->pdev);
	release_bar(dev->pdev);
err_disable:
	mlx5_pci_disable_device(dev);
	return err;
}

static void mlx5_pci_close(struct mlx5_core_dev *dev)
{
	/* health work might still be active, and it needs pci bar in
	 * order to know the NIC state. Therefore, drain the health WQ
	 * before removing the pci bars
	 */
	mlx5_drain_health_wq(dev);
	iounmap(dev->iseg);
	pci_clear_master(dev->pdev);
	release_bar(dev->pdev);
	mlx5_pci_disable_device(dev);
}

static int mlx5_init_once(struct mlx5_core_dev *dev)
{
	int err;

	dev->priv.devcom = mlx5_devcom_register_device(dev);
	if (IS_ERR(dev->priv.devcom))
		mlx5_core_err(dev, "failed to register with devcom (0x%p)\n",
			      dev->priv.devcom);

	err = mlx5_query_board_id(dev);
	if (err) {
		mlx5_core_err(dev, "query board id failed\n");
		goto err_devcom;
	}

	err = mlx5_irq_table_init(dev);
	if (err) {
		mlx5_core_err(dev, "failed to initialize irq table\n");
		goto err_devcom;
	}

	err = mlx5_eq_table_init(dev);
	if (err) {
		mlx5_core_err(dev, "failed to initialize eq\n");
		goto err_irq_cleanup;
	}

	err = mlx5_events_init(dev);
	if (err) {
		mlx5_core_err(dev, "failed to initialize events\n");
		goto err_eq_cleanup;
	}

	err = mlx5_fw_reset_init(dev);
	if (err) {
		mlx5_core_err(dev, "failed to initialize fw reset events\n");
		goto err_events_cleanup;
	}

	mlx5_cq_debugfs_init(dev);

	mlx5_init_reserved_gids(dev);

	mlx5_init_clock(dev);

	dev->vxlan = mlx5_vxlan_create(dev);
	dev->geneve = mlx5_geneve_create(dev);

	err = mlx5_init_rl_table(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init rate limiting\n");
		goto err_tables_cleanup;
	}

	err = mlx5_mpfs_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init l2 table %d\n", err);
		goto err_rl_cleanup;
	}

	err = mlx5_sriov_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init sriov %d\n", err);
		goto err_mpfs_cleanup;
	}

	err = mlx5_eswitch_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init eswitch %d\n", err);
		goto err_sriov_cleanup;
	}

	err = mlx5_fpga_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init fpga device %d\n", err);
		goto err_eswitch_cleanup;
	}

	err = mlx5_vhca_event_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init vhca event notifier %d\n", err);
		goto err_fpga_cleanup;
	}

	err = mlx5_sf_hw_table_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init SF HW table %d\n", err);
		goto err_sf_hw_table_cleanup;
	}

	err = mlx5_sf_table_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init SF table %d\n", err);
		goto err_sf_table_cleanup;
	}

	dev->dm = mlx5_dm_create(dev);
	if (IS_ERR(dev->dm))
		mlx5_core_warn(dev, "Failed to init device memory%d\n", err);

	dev->tracer = mlx5_fw_tracer_create(dev);
	dev->hv_vhca = mlx5_hv_vhca_create(dev);
	dev->rsc_dump = mlx5_rsc_dump_create(dev);

	return 0;

err_sf_table_cleanup:
	mlx5_sf_hw_table_cleanup(dev);
err_sf_hw_table_cleanup:
	mlx5_vhca_event_cleanup(dev);
err_fpga_cleanup:
	mlx5_fpga_cleanup(dev);
err_eswitch_cleanup:
	mlx5_eswitch_cleanup(dev->priv.eswitch);
err_sriov_cleanup:
	mlx5_sriov_cleanup(dev);
err_mpfs_cleanup:
	mlx5_mpfs_cleanup(dev);
err_rl_cleanup:
	mlx5_cleanup_rl_table(dev);
err_tables_cleanup:
	mlx5_geneve_destroy(dev->geneve);
	mlx5_vxlan_destroy(dev->vxlan);
	mlx5_cq_debugfs_cleanup(dev);
	mlx5_fw_reset_cleanup(dev);
err_events_cleanup:
	mlx5_events_cleanup(dev);
err_eq_cleanup:
	mlx5_eq_table_cleanup(dev);
err_irq_cleanup:
	mlx5_irq_table_cleanup(dev);
err_devcom:
	mlx5_devcom_unregister_device(dev->priv.devcom);

	return err;
}

static void mlx5_cleanup_once(struct mlx5_core_dev *dev)
{
	mlx5_rsc_dump_destroy(dev);
	mlx5_hv_vhca_destroy(dev->hv_vhca);
	mlx5_fw_tracer_destroy(dev->tracer);
	mlx5_dm_cleanup(dev);
	mlx5_sf_table_cleanup(dev);
	mlx5_sf_hw_table_cleanup(dev);
	mlx5_vhca_event_cleanup(dev);
	mlx5_fpga_cleanup(dev);
	mlx5_eswitch_cleanup(dev->priv.eswitch);
	mlx5_sriov_cleanup(dev);
	mlx5_mpfs_cleanup(dev);
	mlx5_cleanup_rl_table(dev);
	mlx5_geneve_destroy(dev->geneve);
	mlx5_vxlan_destroy(dev->vxlan);
	mlx5_cleanup_clock(dev);
	mlx5_cleanup_reserved_gids(dev);
	mlx5_cq_debugfs_cleanup(dev);
	mlx5_fw_reset_cleanup(dev);
	mlx5_events_cleanup(dev);
	mlx5_eq_table_cleanup(dev);
	mlx5_irq_table_cleanup(dev);
	mlx5_devcom_unregister_device(dev->priv.devcom);
}

static int mlx5_function_setup(struct mlx5_core_dev *dev, bool boot)
{
	int err;

	mlx5_core_info(dev, "firmware version: %d.%d.%d\n", fw_rev_maj(dev),
		       fw_rev_min(dev), fw_rev_sub(dev));

	/* Only PFs hold the relevant PCIe information for this query */
	if (mlx5_core_is_pf(dev))
		pcie_print_link_status(dev->pdev);

	/* wait for firmware to accept initialization segments configurations
	 */
	err = wait_fw_init(dev, FW_PRE_INIT_TIMEOUT_MILI, FW_INIT_WARN_MESSAGE_INTERVAL);
	if (err) {
		mlx5_core_err(dev, "Firmware over %d MS in pre-initializing state, aborting\n",
			      FW_PRE_INIT_TIMEOUT_MILI);
		return err;
	}

	err = mlx5_cmd_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed initializing command interface, aborting\n");
		return err;
	}

	err = wait_fw_init(dev, FW_INIT_TIMEOUT_MILI, 0);
	if (err) {
		mlx5_core_err(dev, "Firmware over %d MS in initializing state, aborting\n",
			      FW_INIT_TIMEOUT_MILI);
		goto err_cmd_cleanup;
	}

	mlx5_cmd_set_state(dev, MLX5_CMDIF_STATE_UP);

	err = mlx5_core_enable_hca(dev, 0);
	if (err) {
		mlx5_core_err(dev, "enable hca failed\n");
		goto err_cmd_cleanup;
	}

	err = mlx5_core_set_issi(dev);
	if (err) {
		mlx5_core_err(dev, "failed to set issi\n");
		goto err_disable_hca;
	}

	err = mlx5_satisfy_startup_pages(dev, 1);
	if (err) {
		mlx5_core_err(dev, "failed to allocate boot pages\n");
		goto err_disable_hca;
	}

	err = set_hca_ctrl(dev);
	if (err) {
		mlx5_core_err(dev, "set_hca_ctrl failed\n");
		goto reclaim_boot_pages;
	}

	err = set_hca_cap(dev);
	if (err) {
		mlx5_core_err(dev, "set_hca_cap failed\n");
		goto reclaim_boot_pages;
	}

	err = mlx5_satisfy_startup_pages(dev, 0);
	if (err) {
		mlx5_core_err(dev, "failed to allocate init pages\n");
		goto reclaim_boot_pages;
	}

	err = mlx5_cmd_init_hca(dev, sw_owner_id);
	if (err) {
		mlx5_core_err(dev, "init hca failed\n");
		goto reclaim_boot_pages;
	}

	mlx5_set_driver_version(dev);

	mlx5_start_health_poll(dev);

	err = mlx5_query_hca_caps(dev);
	if (err) {
		mlx5_core_err(dev, "query hca failed\n");
		goto stop_health;
	}

	return 0;

stop_health:
	mlx5_stop_health_poll(dev, boot);
reclaim_boot_pages:
	mlx5_reclaim_startup_pages(dev);
err_disable_hca:
	mlx5_core_disable_hca(dev, 0);
err_cmd_cleanup:
	mlx5_cmd_set_state(dev, MLX5_CMDIF_STATE_DOWN);
	mlx5_cmd_cleanup(dev);

	return err;
}

static int mlx5_function_teardown(struct mlx5_core_dev *dev, bool boot)
{
	int err;

	mlx5_stop_health_poll(dev, boot);
	err = mlx5_cmd_teardown_hca(dev);
	if (err) {
		mlx5_core_err(dev, "tear_down_hca failed, skip cleanup\n");
		return err;
	}
	mlx5_reclaim_startup_pages(dev);
	mlx5_core_disable_hca(dev, 0);
	mlx5_cmd_set_state(dev, MLX5_CMDIF_STATE_DOWN);
	mlx5_cmd_cleanup(dev);

	return 0;
}

static int mlx5_load(struct mlx5_core_dev *dev)
{
	int err;

	dev->priv.uar = mlx5_get_uars_page(dev);
	if (IS_ERR(dev->priv.uar)) {
		mlx5_core_err(dev, "Failed allocating uar, aborting\n");
		err = PTR_ERR(dev->priv.uar);
		return err;
	}

	mlx5_events_start(dev);
	mlx5_pagealloc_start(dev);

	err = mlx5_irq_table_create(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to alloc IRQs\n");
		goto err_irq_table;
	}

	err = mlx5_eq_table_create(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to create EQs\n");
		goto err_eq_table;
	}

	err = mlx5_fw_tracer_init(dev->tracer);
	if (err) {
		mlx5_core_err(dev, "Failed to init FW tracer\n");
		goto err_fw_tracer;
	}

	mlx5_fw_reset_events_start(dev);
	mlx5_hv_vhca_init(dev->hv_vhca);

	err = mlx5_rsc_dump_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init Resource dump\n");
		goto err_rsc_dump;
	}

	err = mlx5_fpga_device_start(dev);
	if (err) {
		mlx5_core_err(dev, "fpga device start failed %d\n", err);
		goto err_fpga_start;
	}

	mlx5_accel_ipsec_init(dev);

	err = mlx5_accel_tls_init(dev);
	if (err) {
		mlx5_core_err(dev, "TLS device start failed %d\n", err);
		goto err_tls_start;
	}

	err = mlx5_init_fs(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init flow steering\n");
		goto err_fs;
	}

	err = mlx5_core_set_hca_defaults(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to set hca defaults\n");
		goto err_set_hca;
	}

	mlx5_vhca_event_start(dev);

	err = mlx5_sf_hw_table_create(dev);
	if (err) {
		mlx5_core_err(dev, "sf table create failed %d\n", err);
		goto err_vhca;
	}

	err = mlx5_ec_init(dev);
	if (err) {
		mlx5_core_err(dev, "Failed to init embedded CPU\n");
		goto err_ec;
	}

	err = mlx5_sriov_attach(dev);
	if (err) {
		mlx5_core_err(dev, "sriov init failed %d\n", err);
		goto err_sriov;
	}

	mlx5_sf_dev_table_create(dev);

	return 0;

err_sriov:
	mlx5_ec_cleanup(dev);
err_ec:
	mlx5_sf_hw_table_destroy(dev);
err_vhca:
	mlx5_vhca_event_stop(dev);
err_set_hca:
	mlx5_cleanup_fs(dev);
err_fs:
	mlx5_accel_tls_cleanup(dev);
err_tls_start:
	mlx5_accel_ipsec_cleanup(dev);
	mlx5_fpga_device_stop(dev);
err_fpga_start:
	mlx5_rsc_dump_cleanup(dev);
err_rsc_dump:
	mlx5_hv_vhca_cleanup(dev->hv_vhca);
	mlx5_fw_reset_events_stop(dev);
	mlx5_fw_tracer_cleanup(dev->tracer);
err_fw_tracer:
	mlx5_eq_table_destroy(dev);
err_eq_table:
	mlx5_irq_table_destroy(dev);
err_irq_table:
	mlx5_pagealloc_stop(dev);
	mlx5_events_stop(dev);
	mlx5_put_uars_page(dev, dev->priv.uar);
	return err;
}

static void mlx5_unload(struct mlx5_core_dev *dev)
{
	mlx5_sf_dev_table_destroy(dev);
	mlx5_sriov_detach(dev);
	mlx5_ec_cleanup(dev);
	mlx5_sf_hw_table_destroy(dev);
	mlx5_vhca_event_stop(dev);
	mlx5_cleanup_fs(dev);
	mlx5_accel_ipsec_cleanup(dev);
	mlx5_accel_tls_cleanup(dev);
	mlx5_fpga_device_stop(dev);
	mlx5_rsc_dump_cleanup(dev);
	mlx5_hv_vhca_cleanup(dev->hv_vhca);
	mlx5_fw_reset_events_stop(dev);
	mlx5_fw_tracer_cleanup(dev->tracer);
	mlx5_eq_table_destroy(dev);
	mlx5_irq_table_destroy(dev);
	mlx5_pagealloc_stop(dev);
	mlx5_events_stop(dev);
	mlx5_put_uars_page(dev, dev->priv.uar);
}

int mlx5_init_one(struct mlx5_core_dev *dev)
{
	int err = 0;

	mutex_lock(&dev->intf_state_mutex);
	if (test_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state)) {
		mlx5_core_warn(dev, "interface is up, NOP\n");
		goto out;
	}
	/* remove any previous indication of internal error */
	dev->state = MLX5_DEVICE_STATE_UP;

	err = mlx5_function_setup(dev, true);
	if (err)
		goto err_function;

	err = mlx5_init_once(dev);
	if (err) {
		mlx5_core_err(dev, "sw objs init failed\n");
		goto function_teardown;
	}

	err = mlx5_load(dev);
	if (err)
		goto err_load;

	set_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);

	err = mlx5_devlink_register(priv_to_devlink(dev), dev->device);
	if (err)
		goto err_devlink_reg;

	err = mlx5_register_device(dev);
	if (err)
		goto err_register;

	mutex_unlock(&dev->intf_state_mutex);
	return 0;

err_register:
	mlx5_devlink_unregister(priv_to_devlink(dev));
err_devlink_reg:
	clear_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);
	mlx5_unload(dev);
err_load:
	mlx5_cleanup_once(dev);
function_teardown:
	mlx5_function_teardown(dev, true);
err_function:
	dev->state = MLX5_DEVICE_STATE_INTERNAL_ERROR;
out:
	mutex_unlock(&dev->intf_state_mutex);
	return err;
}

void mlx5_uninit_one(struct mlx5_core_dev *dev)
{
	mutex_lock(&dev->intf_state_mutex);

	mlx5_unregister_device(dev);
	mlx5_devlink_unregister(priv_to_devlink(dev));

	if (!test_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state)) {
		mlx5_core_warn(dev, "%s: interface is down, NOP\n",
			       __func__);
		mlx5_cleanup_once(dev);
		goto out;
	}

	clear_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);
	mlx5_unload(dev);
	mlx5_cleanup_once(dev);
	mlx5_function_teardown(dev, true);
out:
	mutex_unlock(&dev->intf_state_mutex);
}

int mlx5_load_one(struct mlx5_core_dev *dev)
{
	int err = 0;

	mutex_lock(&dev->intf_state_mutex);
	if (test_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state)) {
		mlx5_core_warn(dev, "interface is up, NOP\n");
		goto out;
	}
	/* remove any previous indication of internal error */
	dev->state = MLX5_DEVICE_STATE_UP;

	err = mlx5_function_setup(dev, false);
	if (err)
		goto err_function;

	err = mlx5_load(dev);
	if (err)
		goto err_load;

	set_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);

	err = mlx5_attach_device(dev);
	if (err)
		goto err_attach;

	mutex_unlock(&dev->intf_state_mutex);
	return 0;

err_attach:
	clear_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);
	mlx5_unload(dev);
err_load:
	mlx5_function_teardown(dev, false);
err_function:
	dev->state = MLX5_DEVICE_STATE_INTERNAL_ERROR;
out:
	mutex_unlock(&dev->intf_state_mutex);
	return err;
}

void mlx5_unload_one(struct mlx5_core_dev *dev)
{
	mutex_lock(&dev->intf_state_mutex);

	mlx5_detach_device(dev);

	if (!test_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state)) {
		mlx5_core_warn(dev, "%s: interface is down, NOP\n",
			       __func__);
		goto out;
	}

	clear_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);
	mlx5_unload(dev);
	mlx5_function_teardown(dev, false);
out:
	mutex_unlock(&dev->intf_state_mutex);
}

int mlx5_mdev_init(struct mlx5_core_dev *dev, int profile_idx)
{
	struct mlx5_priv *priv = &dev->priv;
	int err;

	memcpy(&dev->profile, &profile[profile_idx], sizeof(dev->profile));
	INIT_LIST_HEAD(&priv->ctx_list);
	spin_lock_init(&priv->ctx_lock);
	mutex_init(&dev->intf_state_mutex);

	mutex_init(&priv->bfregs.reg_head.lock);
	mutex_init(&priv->bfregs.wc_head.lock);
	INIT_LIST_HEAD(&priv->bfregs.reg_head.list);
	INIT_LIST_HEAD(&priv->bfregs.wc_head.list);

	mutex_init(&priv->alloc_mutex);
	mutex_init(&priv->pgdir_mutex);
	INIT_LIST_HEAD(&priv->pgdir_list);

	priv->dbg_root = debugfs_create_dir(dev_name(dev->device),
					    mlx5_debugfs_root);
	INIT_LIST_HEAD(&priv->traps);

	err = mlx5_health_init(dev);
	if (err)
		goto err_health_init;

	err = mlx5_pagealloc_init(dev);
	if (err)
		goto err_pagealloc_init;

	err = mlx5_adev_init(dev);
	if (err)
		goto err_adev_init;

	return 0;

err_adev_init:
	mlx5_pagealloc_cleanup(dev);
err_pagealloc_init:
	mlx5_health_cleanup(dev);
err_health_init:
	debugfs_remove(dev->priv.dbg_root);
	mutex_destroy(&priv->pgdir_mutex);
	mutex_destroy(&priv->alloc_mutex);
	mutex_destroy(&priv->bfregs.wc_head.lock);
	mutex_destroy(&priv->bfregs.reg_head.lock);
	mutex_destroy(&dev->intf_state_mutex);
	return err;
}

void mlx5_mdev_uninit(struct mlx5_core_dev *dev)
{
	struct mlx5_priv *priv = &dev->priv;

	mlx5_adev_cleanup(dev);
	mlx5_pagealloc_cleanup(dev);
	mlx5_health_cleanup(dev);
	debugfs_remove_recursive(dev->priv.dbg_root);
	mutex_destroy(&priv->pgdir_mutex);
	mutex_destroy(&priv->alloc_mutex);
	mutex_destroy(&priv->bfregs.wc_head.lock);
	mutex_destroy(&priv->bfregs.reg_head.lock);
	mutex_destroy(&dev->intf_state_mutex);
}

static int probe_one(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct mlx5_core_dev *dev;
	struct devlink *devlink;
	int err;

	devlink = mlx5_devlink_alloc();
	if (!devlink) {
		dev_err(&pdev->dev, "devlink alloc failed\n");
		return -ENOMEM;
	}

	dev = devlink_priv(devlink);
	dev->device = &pdev->dev;
	dev->pdev = pdev;

	dev->coredev_type = id->driver_data & MLX5_PCI_DEV_IS_VF ?
			 MLX5_COREDEV_VF : MLX5_COREDEV_PF;

	dev->priv.adev_idx = mlx5_adev_idx_alloc();
	if (dev->priv.adev_idx < 0) {
		err = dev->priv.adev_idx;
		goto adev_init_err;
	}

	err = mlx5_mdev_init(dev, prof_sel);
	if (err)
		goto mdev_init_err;

	err = mlx5_pci_init(dev, pdev, id);
	if (err) {
		mlx5_core_err(dev, "mlx5_pci_init failed with error code %d\n",
			      err);
		goto pci_init_err;
	}

	err = mlx5_init_one(dev);
	if (err) {
		mlx5_core_err(dev, "mlx5_init_one failed with error code %d\n",
			      err);
		goto err_init_one;
	}

	err = mlx5_crdump_enable(dev);
	if (err)
		dev_err(&pdev->dev, "mlx5_crdump_enable failed with error code %d\n", err);

	pci_save_state(pdev);
	if (!mlx5_core_is_mp_slave(dev))
		devlink_reload_enable(devlink);
	return 0;

err_init_one:
	mlx5_pci_close(dev);
pci_init_err:
	mlx5_mdev_uninit(dev);
mdev_init_err:
	mlx5_adev_idx_free(dev->priv.adev_idx);
adev_init_err:
	mlx5_devlink_free(devlink);

	return err;
}

static void remove_one(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev  = pci_get_drvdata(pdev);
	struct devlink *devlink = priv_to_devlink(dev);

	devlink_reload_disable(devlink);
	mlx5_crdump_disable(dev);
	mlx5_drain_health_wq(dev);
	mlx5_uninit_one(dev);
	mlx5_pci_close(dev);
	mlx5_mdev_uninit(dev);
	mlx5_adev_idx_free(dev->priv.adev_idx);
	mlx5_devlink_free(devlink);
}

static pci_ers_result_t mlx5_pci_err_detected(struct pci_dev *pdev,
					      pci_channel_state_t state)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);

	mlx5_core_info(dev, "%s was called\n", __func__);

	mlx5_enter_error_state(dev, false);
	mlx5_error_sw_reset(dev);
	mlx5_unload_one(dev);
	mlx5_drain_health_wq(dev);
	mlx5_pci_disable_device(dev);

	return state == pci_channel_io_perm_failure ?
		PCI_ERS_RESULT_DISCONNECT : PCI_ERS_RESULT_NEED_RESET;
}

/* wait for the device to show vital signs by waiting
 * for the health counter to start counting.
 */
static int wait_vital(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	struct mlx5_core_health *health = &dev->priv.health;
	const int niter = 100;
	u32 last_count = 0;
	u32 count;
	int i;

	for (i = 0; i < niter; i++) {
		count = ioread32be(health->health_counter);
		if (count && count != 0xffffffff) {
			if (last_count && last_count != count) {
				mlx5_core_info(dev,
					       "wait vital counter value 0x%x after %d iterations\n",
					       count, i);
				return 0;
			}
			last_count = count;
		}
		msleep(50);
	}

	return -ETIMEDOUT;
}

static pci_ers_result_t mlx5_pci_slot_reset(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	int err;

	mlx5_core_info(dev, "%s was called\n", __func__);

	err = mlx5_pci_enable_device(dev);
	if (err) {
		mlx5_core_err(dev, "%s: mlx5_pci_enable_device failed with error code: %d\n",
			      __func__, err);
		return PCI_ERS_RESULT_DISCONNECT;
	}

	pci_set_master(pdev);
	pci_restore_state(pdev);
	pci_save_state(pdev);

	if (wait_vital(pdev)) {
		mlx5_core_err(dev, "%s: wait_vital timed out\n", __func__);
		return PCI_ERS_RESULT_DISCONNECT;
	}

	return PCI_ERS_RESULT_RECOVERED;
}

static void mlx5_pci_resume(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	int err;

	mlx5_core_info(dev, "%s was called\n", __func__);

	err = mlx5_load_one(dev);
	if (err)
		mlx5_core_err(dev, "%s: mlx5_load_one failed with error code: %d\n",
			      __func__, err);
	else
		mlx5_core_info(dev, "%s: device recovered\n", __func__);
}

static const struct pci_error_handlers mlx5_err_handler = {
	.error_detected = mlx5_pci_err_detected,
	.slot_reset	= mlx5_pci_slot_reset,
	.resume		= mlx5_pci_resume
};

static int mlx5_try_fast_unload(struct mlx5_core_dev *dev)
{
	bool fast_teardown = false, force_teardown = false;
	int ret = 1;

	fast_teardown = MLX5_CAP_GEN(dev, fast_teardown);
	force_teardown = MLX5_CAP_GEN(dev, force_teardown);

	mlx5_core_dbg(dev, "force teardown firmware support=%d\n", force_teardown);
	mlx5_core_dbg(dev, "fast teardown firmware support=%d\n", fast_teardown);

	if (!fast_teardown && !force_teardown)
		return -EOPNOTSUPP;

	if (dev->state == MLX5_DEVICE_STATE_INTERNAL_ERROR) {
		mlx5_core_dbg(dev, "Device in internal error state, giving up\n");
		return -EAGAIN;
	}

	/* Panic tear down fw command will stop the PCI bus communication
	 * with the HCA, so the health polll is no longer needed.
	 */
	mlx5_drain_health_wq(dev);
	mlx5_stop_health_poll(dev, false);

	ret = mlx5_cmd_fast_teardown_hca(dev);
	if (!ret)
		goto succeed;

	ret = mlx5_cmd_force_teardown_hca(dev);
	if (!ret)
		goto succeed;

	mlx5_core_dbg(dev, "Firmware couldn't do fast unload error: %d\n", ret);
	mlx5_start_health_poll(dev);
	return ret;

succeed:
	mlx5_enter_error_state(dev, true);

	/* Some platforms requiring freeing the IRQ's in the shutdown
	 * flow. If they aren't freed they can't be allocated after
	 * kexec. There is no need to cleanup the mlx5_core software
	 * contexts.
	 */
	mlx5_core_eq_free_irqs(dev);

	return 0;
}

static void shutdown(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev  = pci_get_drvdata(pdev);
	int err;

	mlx5_core_info(dev, "Shutdown was called\n");
	err = mlx5_try_fast_unload(dev);
	if (err)
		mlx5_unload_one(dev);
	mlx5_pci_disable_device(dev);
}

static int mlx5_suspend(struct pci_dev *pdev, pm_message_t state)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);

	mlx5_unload_one(dev);

	return 0;
}

static int mlx5_resume(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);

	return mlx5_load_one(dev);
}

static const struct pci_device_id mlx5_core_pci_table[] = {
	{ PCI_VDEVICE(MELLANOX, PCI_DEVICE_ID_MELLANOX_CONNECTIB) },
	{ PCI_VDEVICE(MELLANOX, 0x1012), MLX5_PCI_DEV_IS_VF},	/* Connect-IB VF */
	{ PCI_VDEVICE(MELLANOX, PCI_DEVICE_ID_MELLANOX_CONNECTX4) },
	{ PCI_VDEVICE(MELLANOX, 0x1014), MLX5_PCI_DEV_IS_VF},	/* ConnectX-4 VF */
	{ PCI_VDEVICE(MELLANOX, PCI_DEVICE_ID_MELLANOX_CONNECTX4_LX) },
	{ PCI_VDEVICE(MELLANOX, 0x1016), MLX5_PCI_DEV_IS_VF},	/* ConnectX-4LX VF */
	{ PCI_VDEVICE(MELLANOX, 0x1017) },			/* ConnectX-5, PCIe 3.0 */
	{ PCI_VDEVICE(MELLANOX, 0x1018), MLX5_PCI_DEV_IS_VF},	/* ConnectX-5 VF */
	{ PCI_VDEVICE(MELLANOX, 0x1019) },			/* ConnectX-5 Ex */
	{ PCI_VDEVICE(MELLANOX, 0x101a), MLX5_PCI_DEV_IS_VF},	/* ConnectX-5 Ex VF */
	{ PCI_VDEVICE(MELLANOX, 0x101b) },			/* ConnectX-6 */
	{ PCI_VDEVICE(MELLANOX, 0x101c), MLX5_PCI_DEV_IS_VF},	/* ConnectX-6 VF */
	{ PCI_VDEVICE(MELLANOX, 0x101d) },			/* ConnectX-6 Dx */
	{ PCI_VDEVICE(MELLANOX, 0x101e), MLX5_PCI_DEV_IS_VF},	/* ConnectX Family mlx5Gen Virtual Function */
	{ PCI_VDEVICE(MELLANOX, 0x101f) },			/* ConnectX-6 LX */
	{ PCI_VDEVICE(MELLANOX, 0x1021) },			/* ConnectX-7 */
	{ PCI_VDEVICE(MELLANOX, 0xa2d2) },			/* BlueField integrated ConnectX-5 network controller */
	{ PCI_VDEVICE(MELLANOX, 0xa2d3), MLX5_PCI_DEV_IS_VF},	/* BlueField integrated ConnectX-5 network controller VF */
	{ PCI_VDEVICE(MELLANOX, 0xa2d6) },			/* BlueField-2 integrated ConnectX-6 Dx network controller */
	{ PCI_VDEVICE(MELLANOX, 0xa2dc) },			/* BlueField-3 integrated ConnectX-7 network controller */
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, mlx5_core_pci_table);

void mlx5_disable_device(struct mlx5_core_dev *dev)
{
	mlx5_error_sw_reset(dev);
	mlx5_unload_one(dev);
}

int mlx5_recover_device(struct mlx5_core_dev *dev)
{
	int ret = -EIO;

	mlx5_pci_disable_device(dev);
	if (mlx5_pci_slot_reset(dev->pdev) == PCI_ERS_RESULT_RECOVERED)
		ret = mlx5_load_one(dev);
	return ret;
}

static struct pci_driver mlx5_core_driver = {
	.name           = KBUILD_MODNAME,
	.id_table       = mlx5_core_pci_table,
	.probe          = probe_one,
	.remove         = remove_one,
	.suspend        = mlx5_suspend,
	.resume         = mlx5_resume,
	.shutdown	= shutdown,
	.err_handler	= &mlx5_err_handler,
	.sriov_configure   = mlx5_core_sriov_configure,
	.sriov_get_vf_total_msix = mlx5_sriov_get_vf_total_msix,
	.sriov_set_msix_vec_count = mlx5_core_sriov_set_msix_vec_count,
};

static void mlx5_core_verify_params(void)
{
	if (prof_sel >= ARRAY_SIZE(profile)) {
		pr_warn("mlx5_core: WARNING: Invalid module parameter prof_sel %d, valid range 0-%zu, changing back to default(%d)\n",
			prof_sel,
			ARRAY_SIZE(profile) - 1,
			MLX5_DEFAULT_PROF);
		prof_sel = MLX5_DEFAULT_PROF;
	}
}

static int __init init(void)
{
	int err;

	WARN_ONCE(strcmp(MLX5_ADEV_NAME, KBUILD_MODNAME),
		  "mlx5_core name not in sync with kernel module name");

	get_random_bytes(&sw_owner_id, sizeof(sw_owner_id));

	mlx5_core_verify_params();
	mlx5_fpga_ipsec_build_fs_cmds();
	mlx5_register_debugfs();

	err = pci_register_driver(&mlx5_core_driver);
	if (err)
		goto err_debug;

	err = mlx5_sf_driver_register();
	if (err)
		goto err_sf;

#ifdef CONFIG_MLX5_CORE_EN
	err = mlx5e_init();
	if (err) {
		pci_unregister_driver(&mlx5_core_driver);
		goto err_debug;
	}
#endif

	return 0;

err_sf:
	pci_unregister_driver(&mlx5_core_driver);
err_debug:
	mlx5_unregister_debugfs();
	return err;
}

static void __exit cleanup(void)
{
#ifdef CONFIG_MLX5_CORE_EN
	mlx5e_cleanup();
#endif
	mlx5_sf_driver_unregister();
	pci_unregister_driver(&mlx5_core_driver);
	mlx5_unregister_debugfs();
}

module_init(init);
module_exit(cleanup);

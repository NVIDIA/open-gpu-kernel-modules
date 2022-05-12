/*
 * Copyright (c) 2006, 2007 Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2007, 2008 Mellanox Technologies. All rights reserved.
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

#include <linux/slab.h>
#include <linux/export.h>
#include <linux/errno.h>
#include <net/devlink.h>

#include "mlx4.h"

struct mlx4_device_context {
	struct list_head	list;
	struct list_head	bond_list;
	struct mlx4_interface  *intf;
	void		       *context;
};

static LIST_HEAD(intf_list);
static LIST_HEAD(dev_list);
static DEFINE_MUTEX(intf_mutex);

static void mlx4_add_device(struct mlx4_interface *intf, struct mlx4_priv *priv)
{
	struct mlx4_device_context *dev_ctx;

	dev_ctx = kmalloc(sizeof(*dev_ctx), GFP_KERNEL);
	if (!dev_ctx)
		return;

	dev_ctx->intf    = intf;
	dev_ctx->context = intf->add(&priv->dev);

	if (dev_ctx->context) {
		spin_lock_irq(&priv->ctx_lock);
		list_add_tail(&dev_ctx->list, &priv->ctx_list);
		spin_unlock_irq(&priv->ctx_lock);
		if (intf->activate)
			intf->activate(&priv->dev, dev_ctx->context);
	} else
		kfree(dev_ctx);

}

static void mlx4_remove_device(struct mlx4_interface *intf, struct mlx4_priv *priv)
{
	struct mlx4_device_context *dev_ctx;

	list_for_each_entry(dev_ctx, &priv->ctx_list, list)
		if (dev_ctx->intf == intf) {
			spin_lock_irq(&priv->ctx_lock);
			list_del(&dev_ctx->list);
			spin_unlock_irq(&priv->ctx_lock);

			intf->remove(&priv->dev, dev_ctx->context);
			kfree(dev_ctx);
			return;
		}
}

int mlx4_register_interface(struct mlx4_interface *intf)
{
	struct mlx4_priv *priv;

	if (!intf->add || !intf->remove)
		return -EINVAL;

	mutex_lock(&intf_mutex);

	list_add_tail(&intf->list, &intf_list);
	list_for_each_entry(priv, &dev_list, dev_list) {
		if (mlx4_is_mfunc(&priv->dev) && (intf->flags & MLX4_INTFF_BONDING)) {
			mlx4_dbg(&priv->dev,
				 "SRIOV, disabling HA mode for intf proto %d\n", intf->protocol);
			intf->flags &= ~MLX4_INTFF_BONDING;
		}
		mlx4_add_device(intf, priv);
	}

	mutex_unlock(&intf_mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(mlx4_register_interface);

void mlx4_unregister_interface(struct mlx4_interface *intf)
{
	struct mlx4_priv *priv;

	mutex_lock(&intf_mutex);

	list_for_each_entry(priv, &dev_list, dev_list)
		mlx4_remove_device(intf, priv);

	list_del(&intf->list);

	mutex_unlock(&intf_mutex);
}
EXPORT_SYMBOL_GPL(mlx4_unregister_interface);

int mlx4_do_bond(struct mlx4_dev *dev, bool enable)
{
	struct mlx4_priv *priv = mlx4_priv(dev);
	struct mlx4_device_context *dev_ctx = NULL, *temp_dev_ctx;
	unsigned long flags;
	int ret;
	LIST_HEAD(bond_list);

	if (!(dev->caps.flags2 & MLX4_DEV_CAP_FLAG2_PORT_REMAP))
		return -EOPNOTSUPP;

	ret = mlx4_disable_rx_port_check(dev, enable);
	if (ret) {
		mlx4_err(dev, "Fail to %s rx port check\n",
			 enable ? "enable" : "disable");
		return ret;
	}
	if (enable) {
		dev->flags |= MLX4_FLAG_BONDED;
	} else {
		ret = mlx4_virt2phy_port_map(dev, 1, 2);
		if (ret) {
			mlx4_err(dev, "Fail to reset port map\n");
			return ret;
		}
		dev->flags &= ~MLX4_FLAG_BONDED;
	}

	spin_lock_irqsave(&priv->ctx_lock, flags);
	list_for_each_entry_safe(dev_ctx, temp_dev_ctx, &priv->ctx_list, list) {
		if (dev_ctx->intf->flags & MLX4_INTFF_BONDING) {
			list_add_tail(&dev_ctx->bond_list, &bond_list);
			list_del(&dev_ctx->list);
		}
	}
	spin_unlock_irqrestore(&priv->ctx_lock, flags);

	list_for_each_entry(dev_ctx, &bond_list, bond_list) {
		dev_ctx->intf->remove(dev, dev_ctx->context);
		dev_ctx->context =  dev_ctx->intf->add(dev);

		spin_lock_irqsave(&priv->ctx_lock, flags);
		list_add_tail(&dev_ctx->list, &priv->ctx_list);
		spin_unlock_irqrestore(&priv->ctx_lock, flags);

		mlx4_dbg(dev, "Interface for protocol %d restarted with bonded mode %s\n",
			 dev_ctx->intf->protocol, enable ?
			 "enabled" : "disabled");
	}
	return 0;
}

void mlx4_dispatch_event(struct mlx4_dev *dev, enum mlx4_dev_event type,
			 unsigned long param)
{
	struct mlx4_priv *priv = mlx4_priv(dev);
	struct mlx4_device_context *dev_ctx;
	unsigned long flags;

	spin_lock_irqsave(&priv->ctx_lock, flags);

	list_for_each_entry(dev_ctx, &priv->ctx_list, list)
		if (dev_ctx->intf->event)
			dev_ctx->intf->event(dev, dev_ctx->context, type, param);

	spin_unlock_irqrestore(&priv->ctx_lock, flags);
}

int mlx4_register_device(struct mlx4_dev *dev)
{
	struct mlx4_priv *priv = mlx4_priv(dev);
	struct mlx4_interface *intf;

	mutex_lock(&intf_mutex);

	dev->persist->interface_state |= MLX4_INTERFACE_STATE_UP;
	list_add_tail(&priv->dev_list, &dev_list);
	list_for_each_entry(intf, &intf_list, list)
		mlx4_add_device(intf, priv);

	mutex_unlock(&intf_mutex);
	mlx4_start_catas_poll(dev);

	return 0;
}

void mlx4_unregister_device(struct mlx4_dev *dev)
{
	struct mlx4_priv *priv = mlx4_priv(dev);
	struct mlx4_interface *intf;

	if (!(dev->persist->interface_state & MLX4_INTERFACE_STATE_UP))
		return;

	mlx4_stop_catas_poll(dev);
	if (dev->persist->interface_state & MLX4_INTERFACE_STATE_DELETION &&
	    mlx4_is_slave(dev)) {
		/* In mlx4_remove_one on a VF */
		u32 slave_read =
			swab32(readl(&mlx4_priv(dev)->mfunc.comm->slave_read));

		if (mlx4_comm_internal_err(slave_read)) {
			mlx4_dbg(dev, "%s: comm channel is down, entering error state.\n",
				 __func__);
			mlx4_enter_error_state(dev->persist);
		}
	}
	mutex_lock(&intf_mutex);

	list_for_each_entry(intf, &intf_list, list)
		mlx4_remove_device(intf, priv);

	list_del(&priv->dev_list);
	dev->persist->interface_state &= ~MLX4_INTERFACE_STATE_UP;

	mutex_unlock(&intf_mutex);
}

void *mlx4_get_protocol_dev(struct mlx4_dev *dev, enum mlx4_protocol proto, int port)
{
	struct mlx4_priv *priv = mlx4_priv(dev);
	struct mlx4_device_context *dev_ctx;
	unsigned long flags;
	void *result = NULL;

	spin_lock_irqsave(&priv->ctx_lock, flags);

	list_for_each_entry(dev_ctx, &priv->ctx_list, list)
		if (dev_ctx->intf->protocol == proto && dev_ctx->intf->get_dev) {
			result = dev_ctx->intf->get_dev(dev, dev_ctx->context, port);
			break;
		}

	spin_unlock_irqrestore(&priv->ctx_lock, flags);

	return result;
}
EXPORT_SYMBOL_GPL(mlx4_get_protocol_dev);

struct devlink_port *mlx4_get_devlink_port(struct mlx4_dev *dev, int port)
{
	struct mlx4_port_info *info = &mlx4_priv(dev)->port[port];

	return &info->devlink_port;
}
EXPORT_SYMBOL_GPL(mlx4_get_devlink_port);

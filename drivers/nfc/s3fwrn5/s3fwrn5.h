/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * NCI based driver for Samsung S3FWRN5 NFC chip
 *
 * Copyright (C) 2015 Samsung Electrnoics
 * Robert Baldyga <r.baldyga@samsung.com>
 */

#ifndef __LOCAL_S3FWRN5_H_
#define __LOCAL_S3FWRN5_H_

#include <linux/nfc.h>

#include <net/nfc/nci_core.h>

#include "firmware.h"

enum s3fwrn5_mode {
	S3FWRN5_MODE_COLD,
	S3FWRN5_MODE_NCI,
	S3FWRN5_MODE_FW,
};

struct s3fwrn5_phy_ops {
	void (*set_wake)(void *id, bool sleep);
	void (*set_mode)(void *id, enum s3fwrn5_mode);
	enum s3fwrn5_mode (*get_mode)(void *id);
	int (*write)(void *id, struct sk_buff *skb);
};

struct s3fwrn5_info {
	struct nci_dev *ndev;
	void *phy_id;
	struct device *pdev;

	const struct s3fwrn5_phy_ops *phy_ops;

	struct s3fwrn5_fw_info fw_info;

	struct mutex mutex;
};

static inline int s3fwrn5_set_mode(struct s3fwrn5_info *info,
	enum s3fwrn5_mode mode)
{
	if (!info->phy_ops->set_mode)
		return -EOPNOTSUPP;

	info->phy_ops->set_mode(info->phy_id, mode);

	return 0;
}

static inline enum s3fwrn5_mode s3fwrn5_get_mode(struct s3fwrn5_info *info)
{
	if (!info->phy_ops->get_mode)
		return -EOPNOTSUPP;

	return info->phy_ops->get_mode(info->phy_id);
}

static inline int s3fwrn5_set_wake(struct s3fwrn5_info *info, bool wake)
{
	if (!info->phy_ops->set_wake)
		return -EOPNOTSUPP;

	info->phy_ops->set_wake(info->phy_id, wake);

	return 0;
}

static inline int s3fwrn5_write(struct s3fwrn5_info *info, struct sk_buff *skb)
{
	if (!info->phy_ops->write)
		return -EOPNOTSUPP;

	return info->phy_ops->write(info->phy_id, skb);
}

int s3fwrn5_probe(struct nci_dev **ndev, void *phy_id, struct device *pdev,
	const struct s3fwrn5_phy_ops *phy_ops);
void s3fwrn5_remove(struct nci_dev *ndev);

int s3fwrn5_recv_frame(struct nci_dev *ndev, struct sk_buff *skb,
	enum s3fwrn5_mode mode);

#endif /* __LOCAL_S3FWRN5_H_ */

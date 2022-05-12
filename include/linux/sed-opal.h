/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright © 2016 Intel Corporation
 *
 * Authors:
 *    Rafael Antognolli <rafael.antognolli@intel.com>
 *    Scott  Bauer      <scott.bauer@intel.com>
 */

#ifndef LINUX_OPAL_H
#define LINUX_OPAL_H

#include <uapi/linux/sed-opal.h>
#include <linux/kernel.h>

struct opal_dev;

typedef int (sec_send_recv)(void *data, u16 spsp, u8 secp, void *buffer,
		size_t len, bool send);

#ifdef CONFIG_BLK_SED_OPAL
void free_opal_dev(struct opal_dev *dev);
bool opal_unlock_from_suspend(struct opal_dev *dev);
struct opal_dev *init_opal_dev(void *data, sec_send_recv *send_recv);
int sed_ioctl(struct opal_dev *dev, unsigned int cmd, void __user *ioctl_ptr);

static inline bool is_sed_ioctl(unsigned int cmd)
{
	switch (cmd) {
	case IOC_OPAL_SAVE:
	case IOC_OPAL_LOCK_UNLOCK:
	case IOC_OPAL_TAKE_OWNERSHIP:
	case IOC_OPAL_ACTIVATE_LSP:
	case IOC_OPAL_SET_PW:
	case IOC_OPAL_ACTIVATE_USR:
	case IOC_OPAL_REVERT_TPR:
	case IOC_OPAL_LR_SETUP:
	case IOC_OPAL_ADD_USR_TO_LR:
	case IOC_OPAL_ENABLE_DISABLE_MBR:
	case IOC_OPAL_ERASE_LR:
	case IOC_OPAL_SECURE_ERASE_LR:
	case IOC_OPAL_PSID_REVERT_TPR:
	case IOC_OPAL_MBR_DONE:
	case IOC_OPAL_WRITE_SHADOW_MBR:
	case IOC_OPAL_GENERIC_TABLE_RW:
		return true;
	}
	return false;
}
#else
static inline void free_opal_dev(struct opal_dev *dev)
{
}

static inline bool is_sed_ioctl(unsigned int cmd)
{
	return false;
}

static inline int sed_ioctl(struct opal_dev *dev, unsigned int cmd,
			    void __user *ioctl_ptr)
{
	return 0;
}
static inline bool opal_unlock_from_suspend(struct opal_dev *dev)
{
	return false;
}
#define init_opal_dev(data, send_recv)		NULL
#endif /* CONFIG_BLK_SED_OPAL */
#endif /* LINUX_OPAL_H */

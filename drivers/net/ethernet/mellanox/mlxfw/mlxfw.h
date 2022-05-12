/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/* Copyright (c) 2017-2019 Mellanox Technologies. All rights reserved */

#ifndef _MLXFW_H
#define _MLXFW_H

#include <linux/firmware.h>
#include <linux/netlink.h>
#include <linux/device.h>
#include <net/devlink.h>

struct mlxfw_dev {
	const struct mlxfw_dev_ops *ops;
	const char *psid;
	u16 psid_size;
	struct devlink *devlink;
};

static inline
struct device *mlxfw_dev_dev(struct mlxfw_dev *mlxfw_dev)
{
	return mlxfw_dev->devlink->dev;
}

#define MLXFW_PRFX "mlxfw: "

#define mlxfw_info(mlxfw_dev, fmt, ...) \
	dev_info(mlxfw_dev_dev(mlxfw_dev), MLXFW_PRFX fmt, ## __VA_ARGS__)
#define mlxfw_err(mlxfw_dev, fmt, ...) \
	dev_err(mlxfw_dev_dev(mlxfw_dev), MLXFW_PRFX fmt, ## __VA_ARGS__)
#define mlxfw_dbg(mlxfw_dev, fmt, ...) \
	dev_dbg(mlxfw_dev_dev(mlxfw_dev), MLXFW_PRFX fmt, ## __VA_ARGS__)

enum mlxfw_fsm_state {
	MLXFW_FSM_STATE_IDLE,
	MLXFW_FSM_STATE_LOCKED,
	MLXFW_FSM_STATE_INITIALIZE,
	MLXFW_FSM_STATE_DOWNLOAD,
	MLXFW_FSM_STATE_VERIFY,
	MLXFW_FSM_STATE_APPLY,
	MLXFW_FSM_STATE_ACTIVATE,
};

enum mlxfw_fsm_state_err {
	MLXFW_FSM_STATE_ERR_OK,
	MLXFW_FSM_STATE_ERR_ERROR,
	MLXFW_FSM_STATE_ERR_REJECTED_DIGEST_ERR,
	MLXFW_FSM_STATE_ERR_REJECTED_NOT_APPLICABLE,
	MLXFW_FSM_STATE_ERR_REJECTED_UNKNOWN_KEY,
	MLXFW_FSM_STATE_ERR_REJECTED_AUTH_FAILED,
	MLXFW_FSM_STATE_ERR_REJECTED_UNSIGNED,
	MLXFW_FSM_STATE_ERR_REJECTED_KEY_NOT_APPLICABLE,
	MLXFW_FSM_STATE_ERR_REJECTED_BAD_FORMAT,
	MLXFW_FSM_STATE_ERR_BLOCKED_PENDING_RESET,
	MLXFW_FSM_STATE_ERR_MAX,
};

enum mlxfw_fsm_reactivate_status {
	MLXFW_FSM_REACTIVATE_STATUS_OK,
	MLXFW_FSM_REACTIVATE_STATUS_BUSY,
	MLXFW_FSM_REACTIVATE_STATUS_PROHIBITED_FW_VER_ERR,
	MLXFW_FSM_REACTIVATE_STATUS_FIRST_PAGE_COPY_FAILED,
	MLXFW_FSM_REACTIVATE_STATUS_FIRST_PAGE_ERASE_FAILED,
	MLXFW_FSM_REACTIVATE_STATUS_FIRST_PAGE_RESTORE_FAILED,
	MLXFW_FSM_REACTIVATE_STATUS_CANDIDATE_FW_DEACTIVATION_FAILED,
	MLXFW_FSM_REACTIVATE_STATUS_FW_ALREADY_ACTIVATED,
	MLXFW_FSM_REACTIVATE_STATUS_ERR_DEVICE_RESET_REQUIRED,
	MLXFW_FSM_REACTIVATE_STATUS_ERR_FW_PROGRAMMING_NEEDED,
	MLXFW_FSM_REACTIVATE_STATUS_MAX,
};

struct mlxfw_dev_ops {
	int (*component_query)(struct mlxfw_dev *mlxfw_dev, u16 component_index,
			       u32 *p_max_size, u8 *p_align_bits,
			       u16 *p_max_write_size);

	int (*fsm_lock)(struct mlxfw_dev *mlxfw_dev, u32 *fwhandle);

	int (*fsm_component_update)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle,
				    u16 component_index, u32 component_size);

	int (*fsm_block_download)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle,
				  u8 *data, u16 size, u32 offset);

	int (*fsm_component_verify)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle,
				    u16 component_index);

	int (*fsm_activate)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle);

	int (*fsm_reactivate)(struct mlxfw_dev *mlxfw_dev, u8 *status);

	int (*fsm_query_state)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle,
			       enum mlxfw_fsm_state *fsm_state,
			       enum mlxfw_fsm_state_err *fsm_state_err);

	void (*fsm_cancel)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle);

	void (*fsm_release)(struct mlxfw_dev *mlxfw_dev, u32 fwhandle);
};

#if IS_REACHABLE(CONFIG_MLXFW)
int mlxfw_firmware_flash(struct mlxfw_dev *mlxfw_dev,
			 const struct firmware *firmware,
			 struct netlink_ext_ack *extack);
#else
static inline
int mlxfw_firmware_flash(struct mlxfw_dev *mlxfw_dev,
			 const struct firmware *firmware,
			 struct netlink_ext_ack *extack)
{
	return -EOPNOTSUPP;
}
#endif

#endif

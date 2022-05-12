/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * linux/drivers/media/platform/s5p-mfc/s5p_mfc_ctrl.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 */

#ifndef S5P_MFC_CTRL_H
#define S5P_MFC_CTRL_H

#include "s5p_mfc_common.h"

int s5p_mfc_release_firmware(struct s5p_mfc_dev *dev);
int s5p_mfc_alloc_firmware(struct s5p_mfc_dev *dev);
int s5p_mfc_load_firmware(struct s5p_mfc_dev *dev);

int s5p_mfc_init_hw(struct s5p_mfc_dev *dev);
void s5p_mfc_deinit_hw(struct s5p_mfc_dev *dev);

int s5p_mfc_sleep(struct s5p_mfc_dev *dev);
int s5p_mfc_wakeup(struct s5p_mfc_dev *dev);

int s5p_mfc_reset(struct s5p_mfc_dev *dev);

int s5p_mfc_open_mfc_inst(struct s5p_mfc_dev *dev, struct s5p_mfc_ctx *ctx);
void s5p_mfc_close_mfc_inst(struct s5p_mfc_dev *dev, struct s5p_mfc_ctx *ctx);

#endif /* S5P_MFC_CTRL_H */

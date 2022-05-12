/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * linux/drivers/media/platform/s5p-mfc/s5p_mfc_dec.h
 *
 * Copyright (C) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 */

#ifndef S5P_MFC_DEC_H_
#define S5P_MFC_DEC_H_

const struct s5p_mfc_codec_ops *get_dec_codec_ops(void);
struct vb2_ops *get_dec_queue_ops(void);
const struct v4l2_ioctl_ops *get_dec_v4l2_ioctl_ops(void);
struct s5p_mfc_fmt *get_dec_def_fmt(bool src);
int s5p_mfc_dec_ctrls_setup(struct s5p_mfc_ctx *ctx);
void s5p_mfc_dec_ctrls_delete(struct s5p_mfc_ctx *ctx);
void s5p_mfc_dec_init(struct s5p_mfc_ctx *ctx);

#endif /* S5P_MFC_DEC_H_ */

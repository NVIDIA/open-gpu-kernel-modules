/* SPDX-License-Identifier: GPL-2.0+ */
/* Copyright (C) 2016-2017 Dmitry Osipenko <digetx@gmail.com> */
#ifndef _UAPI_TEGRA_VDE_H_
#define _UAPI_TEGRA_VDE_H_

#include <linux/types.h>
#include <asm/ioctl.h>

#define FLAG_B_FRAME		0x1
#define FLAG_REFERENCE		0x2

struct tegra_vde_h264_frame {
	__s32 y_fd;
	__s32 cb_fd;
	__s32 cr_fd;
	__s32 aux_fd;
	__u32 y_offset;
	__u32 cb_offset;
	__u32 cr_offset;
	__u32 aux_offset;
	__u32 frame_num;
	__u32 flags;

	// Must be zero'ed
	__u32 reserved[6];
};

struct tegra_vde_h264_decoder_ctx {
	__s32 bitstream_data_fd;
	__u32 bitstream_data_offset;

	__u64 dpb_frames_ptr;
	__u32 dpb_frames_nb;
	__u32 dpb_ref_frames_with_earlier_poc_nb;

	// SPS
	__u32 baseline_profile;
	__u32 level_idc;
	__u32 log2_max_pic_order_cnt_lsb;
	__u32 log2_max_frame_num;
	__u32 pic_order_cnt_type;
	__u32 direct_8x8_inference_flag;
	__u32 pic_width_in_mbs;
	__u32 pic_height_in_mbs;

	// PPS
	__u32 pic_init_qp;
	__u32 deblocking_filter_control_present_flag;
	__u32 constrained_intra_pred_flag;
	__u32 chroma_qp_index_offset;
	__u32 pic_order_present_flag;

	// Slice header
	__u32 num_ref_idx_l0_active_minus1;
	__u32 num_ref_idx_l1_active_minus1;

	// Must be zero'ed
	__u32 reserved[11];
};

#define VDE_IOCTL_BASE			('v' + 0x20)

#define VDE_IO(nr)			_IO(VDE_IOCTL_BASE, nr)
#define VDE_IOR(nr, type)		_IOR(VDE_IOCTL_BASE, nr, type)
#define VDE_IOW(nr, type)		_IOW(VDE_IOCTL_BASE, nr, type)
#define VDE_IOWR(nr, type)		_IOWR(VDE_IOCTL_BASE, nr, type)

#define TEGRA_VDE_DECODE_H264		0x00

#define TEGRA_VDE_IOCTL_DECODE_H264	\
	VDE_IOW(TEGRA_VDE_DECODE_H264, struct tegra_vde_h264_decoder_ctx)

#endif // _UAPI_TEGRA_VDE_H_

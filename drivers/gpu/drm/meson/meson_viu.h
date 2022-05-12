/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2016 BayLibre, SAS
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 */

/* Video Input Unit */

#ifndef __MESON_VIU_H
#define __MESON_VIU_H

/* OSDx_BLKx_CFG */
#define OSD_MALI_SRC_EN		BIT(30)

#define OSD_CANVAS_SEL		16

#define OSD_ENDIANNESS_LE	BIT(15)
#define OSD_ENDIANNESS_BE	(0)

#define OSD_BLK_MODE_422	(0x03 << 8)
#define OSD_BLK_MODE_16		(0x04 << 8)
#define OSD_BLK_MODE_32		(0x05 << 8)
#define OSD_BLK_MODE_24		(0x07 << 8)

#define OSD_OUTPUT_COLOR_RGB	BIT(7)
#define OSD_OUTPUT_COLOR_YUV	(0)

#define OSD_COLOR_MATRIX_32_RGBA	(0x00 << 2)
#define OSD_COLOR_MATRIX_32_ARGB	(0x01 << 2)
#define OSD_COLOR_MATRIX_32_ABGR	(0x02 << 2)
#define OSD_COLOR_MATRIX_32_BGRA	(0x03 << 2)

#define OSD_COLOR_MATRIX_24_RGB		(0x00 << 2)

#define OSD_COLOR_MATRIX_16_RGB655	(0x00 << 2)
#define OSD_COLOR_MATRIX_16_RGB565	(0x04 << 2)

#define OSD_MALI_COLOR_MODE_R8		(0 << 8)
#define OSD_MALI_COLOR_MODE_YUV422	(1 << 8)
#define OSD_MALI_COLOR_MODE_RGB565	(2 << 8)
#define OSD_MALI_COLOR_MODE_RGBA5551	(3 << 8)
#define OSD_MALI_COLOR_MODE_RGBA4444	(4 << 8)
#define OSD_MALI_COLOR_MODE_RGBA8888	(5 << 8)
#define OSD_MALI_COLOR_MODE_RGB888	(7 << 8)
#define OSD_MALI_COLOR_MODE_YUV422_10B	(8 << 8)
#define OSD_MALI_COLOR_MODE_RGBA1010102	(9 << 8)

#define OSD_INTERLACE_ENABLED	BIT(1)
#define OSD_INTERLACE_ODD	BIT(0)
#define OSD_INTERLACE_EVEN	(0)

/* OSDx_CTRL_STAT */
#define OSD_ENABLE		BIT(21)
#define OSD_MEM_LINEAR_ADDR	BIT(2)
#define OSD_BLK0_ENABLE		BIT(0)

#define OSD_GLOBAL_ALPHA_SHIFT	12

/* OSDx_CTRL_STAT2 */
#define OSD_DPATH_MALI_AFBCD	BIT(15)
#define OSD_REPLACE_EN		BIT(14)
#define OSD_REPLACE_SHIFT	6
#define OSD_PENDING_STAT_CLEAN	BIT(1)

void meson_viu_osd1_reset(struct meson_drm *priv);
void meson_viu_g12a_enable_osd1_afbc(struct meson_drm *priv);
void meson_viu_g12a_disable_osd1_afbc(struct meson_drm *priv);
void meson_viu_gxm_enable_osd1_afbc(struct meson_drm *priv);
void meson_viu_gxm_disable_osd1_afbc(struct meson_drm *priv);
void meson_viu_init(struct meson_drm *priv);

#endif /* __MESON_VIU_H */

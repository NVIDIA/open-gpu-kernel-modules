/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * drivers/video/geode/geodefb.h
 *   -- Geode framebuffer driver
 *
 * Copyright (C) 2005 Arcom Control Systems Ltd.
 */
#ifndef __GEODEFB_H__
#define __GEODEFB_H__

struct geodefb_info;

struct geode_dc_ops {
	void (*set_mode)(struct fb_info *);
	void (*set_palette_reg)(struct fb_info *, unsigned, unsigned, unsigned, unsigned);
};

struct geode_vid_ops {
	void (*set_dclk)(struct fb_info *);
	void (*configure_display)(struct fb_info *);
	int  (*blank_display)(struct fb_info *, int blank_mode);
};

struct geodefb_par {
	int enable_crt;
	int panel_x; /* dimensions of an attached flat panel, non-zero => enable panel */
	int panel_y;
	void __iomem *dc_regs;
	void __iomem *vid_regs;
	const struct geode_dc_ops  *dc_ops;
	const struct geode_vid_ops *vid_ops;
};

#endif /* !__GEODEFB_H__ */

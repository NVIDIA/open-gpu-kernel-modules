// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 1998-2008 VIA Technologies, Inc. All Rights Reserved.
 * Copyright 2001-2008 S3 Graphics, Inc. All Rights Reserved.

 */
#include "global.h"
int viafb_platform_epia_dvi = STATE_OFF;
int viafb_device_lcd_dualedge = STATE_OFF;
int viafb_bus_width = 12;
int viafb_display_hardware_layout = HW_LAYOUT_LCD_DVI;
int viafb_DeviceStatus = CRT_Device;
int viafb_hotplug;
int viafb_refresh = 60;
int viafb_refresh1 = 60;
int viafb_lcd_dsp_method = LCD_EXPANDSION;
int viafb_lcd_mode = LCD_OPENLDI;
int viafb_CRT_ON = 1;
int viafb_DVI_ON;
int viafb_LCD_ON ;
int viafb_LCD2_ON;
int viafb_SAMM_ON;
int viafb_dual_fb;
unsigned int viafb_second_xres = 640;
unsigned int viafb_second_yres = 480;
int viafb_hotplug_Xres = 640;
int viafb_hotplug_Yres = 480;
int viafb_hotplug_bpp = 32;
int viafb_hotplug_refresh = 60;
int viafb_primary_dev = None_Device;
int viafb_lcd_panel_id = LCD_PANEL_ID_MAXIMUM + 1;
struct fb_info *viafbinfo;
struct fb_info *viafbinfo1;
struct viafb_par *viaparinfo;
struct viafb_par *viaparinfo1;


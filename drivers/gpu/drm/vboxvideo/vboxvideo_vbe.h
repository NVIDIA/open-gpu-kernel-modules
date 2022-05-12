/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2006-2016 Oracle Corporation */

#ifndef __VBOXVIDEO_VBE_H__
#define __VBOXVIDEO_VBE_H__

/* GUEST <-> HOST Communication API */

#define VBE_DISPI_BANK_ADDRESS          0xA0000
#define VBE_DISPI_BANK_SIZE_KB          64

#define VBE_DISPI_MAX_XRES              16384
#define VBE_DISPI_MAX_YRES              16384
#define VBE_DISPI_MAX_BPP               32

#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF

#define VBE_DISPI_IOPORT_DAC_WRITE_INDEX  0x03C8
#define VBE_DISPI_IOPORT_DAC_DATA         0x03C9

#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_INDEX_BANK            0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
#define VBE_DISPI_INDEX_X_OFFSET        0x8
#define VBE_DISPI_INDEX_Y_OFFSET        0x9
#define VBE_DISPI_INDEX_VBOX_VIDEO      0xa
#define VBE_DISPI_INDEX_FB_BASE_HI      0xb

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2
#define VBE_DISPI_ID3                   0xB0C3
#define VBE_DISPI_ID4                   0xB0C4

#define VBE_DISPI_ID_VBOX_VIDEO         0xBE00
/* The VBOX interface id. Indicates support for VBVA shared memory interface. */
#define VBE_DISPI_ID_HGSMI              0xBE01
#define VBE_DISPI_ID_ANYX               0xBE02

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_GETCAPS               0x02
#define VBE_DISPI_8BIT_DAC              0x20

#define VGA_PORT_HGSMI_HOST             0x3b0
#define VGA_PORT_HGSMI_GUEST            0x3d0

#endif

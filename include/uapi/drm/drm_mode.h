/*
 * Copyright (c) 2007 Dave Airlie <airlied@linux.ie>
 * Copyright (c) 2007 Jakob Bornecrantz <wallbraker@gmail.com>
 * Copyright (c) 2008 Red Hat Inc.
 * Copyright (c) 2007-2008 Tungsten Graphics, Inc., Cedar Park, TX., USA
 * Copyright (c) 2007-2008 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _DRM_MODE_H
#define _DRM_MODE_H

#include "drm.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * DOC: overview
 *
 * DRM exposes many UAPI and structure definition to have a consistent
 * and standardized interface with user.
 * Userspace can refer to these structure definitions and UAPI formats
 * to communicate to driver
 */

#define DRM_CONNECTOR_NAME_LEN	32
#define DRM_DISPLAY_MODE_LEN	32
#define DRM_PROP_NAME_LEN	32

#define DRM_MODE_TYPE_BUILTIN	(1<<0) /* deprecated */
#define DRM_MODE_TYPE_CLOCK_C	((1<<1) | DRM_MODE_TYPE_BUILTIN) /* deprecated */
#define DRM_MODE_TYPE_CRTC_C	((1<<2) | DRM_MODE_TYPE_BUILTIN) /* deprecated */
#define DRM_MODE_TYPE_PREFERRED	(1<<3)
#define DRM_MODE_TYPE_DEFAULT	(1<<4) /* deprecated */
#define DRM_MODE_TYPE_USERDEF	(1<<5)
#define DRM_MODE_TYPE_DRIVER	(1<<6)

#define DRM_MODE_TYPE_ALL	(DRM_MODE_TYPE_PREFERRED |	\
				 DRM_MODE_TYPE_USERDEF |	\
				 DRM_MODE_TYPE_DRIVER)

/* Video mode flags */
/* bit compatible with the xrandr RR_ definitions (bits 0-13)
 *
 * ABI warning: Existing userspace really expects
 * the mode flags to match the xrandr definitions. Any
 * changes that don't match the xrandr definitions will
 * likely need a new client cap or some other mechanism
 * to avoid breaking existing userspace. This includes
 * allocating new flags in the previously unused bits!
 */
#define DRM_MODE_FLAG_PHSYNC			(1<<0)
#define DRM_MODE_FLAG_NHSYNC			(1<<1)
#define DRM_MODE_FLAG_PVSYNC			(1<<2)
#define DRM_MODE_FLAG_NVSYNC			(1<<3)
#define DRM_MODE_FLAG_INTERLACE			(1<<4)
#define DRM_MODE_FLAG_DBLSCAN			(1<<5)
#define DRM_MODE_FLAG_CSYNC			(1<<6)
#define DRM_MODE_FLAG_PCSYNC			(1<<7)
#define DRM_MODE_FLAG_NCSYNC			(1<<8)
#define DRM_MODE_FLAG_HSKEW			(1<<9) /* hskew provided */
#define DRM_MODE_FLAG_BCAST			(1<<10) /* deprecated */
#define DRM_MODE_FLAG_PIXMUX			(1<<11) /* deprecated */
#define DRM_MODE_FLAG_DBLCLK			(1<<12)
#define DRM_MODE_FLAG_CLKDIV2			(1<<13)
 /*
  * When adding a new stereo mode don't forget to adjust DRM_MODE_FLAGS_3D_MAX
  * (define not exposed to user space).
  */
#define DRM_MODE_FLAG_3D_MASK			(0x1f<<14)
#define  DRM_MODE_FLAG_3D_NONE		(0<<14)
#define  DRM_MODE_FLAG_3D_FRAME_PACKING		(1<<14)
#define  DRM_MODE_FLAG_3D_FIELD_ALTERNATIVE	(2<<14)
#define  DRM_MODE_FLAG_3D_LINE_ALTERNATIVE	(3<<14)
#define  DRM_MODE_FLAG_3D_SIDE_BY_SIDE_FULL	(4<<14)
#define  DRM_MODE_FLAG_3D_L_DEPTH		(5<<14)
#define  DRM_MODE_FLAG_3D_L_DEPTH_GFX_GFX_DEPTH	(6<<14)
#define  DRM_MODE_FLAG_3D_TOP_AND_BOTTOM	(7<<14)
#define  DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF	(8<<14)

/* Picture aspect ratio options */
#define DRM_MODE_PICTURE_ASPECT_NONE		0
#define DRM_MODE_PICTURE_ASPECT_4_3		1
#define DRM_MODE_PICTURE_ASPECT_16_9		2
#define DRM_MODE_PICTURE_ASPECT_64_27		3
#define DRM_MODE_PICTURE_ASPECT_256_135		4

/* Content type options */
#define DRM_MODE_CONTENT_TYPE_NO_DATA		0
#define DRM_MODE_CONTENT_TYPE_GRAPHICS		1
#define DRM_MODE_CONTENT_TYPE_PHOTO		2
#define DRM_MODE_CONTENT_TYPE_CINEMA		3
#define DRM_MODE_CONTENT_TYPE_GAME		4

/* Aspect ratio flag bitmask (4 bits 22:19) */
#define DRM_MODE_FLAG_PIC_AR_MASK		(0x0F<<19)
#define  DRM_MODE_FLAG_PIC_AR_NONE \
			(DRM_MODE_PICTURE_ASPECT_NONE<<19)
#define  DRM_MODE_FLAG_PIC_AR_4_3 \
			(DRM_MODE_PICTURE_ASPECT_4_3<<19)
#define  DRM_MODE_FLAG_PIC_AR_16_9 \
			(DRM_MODE_PICTURE_ASPECT_16_9<<19)
#define  DRM_MODE_FLAG_PIC_AR_64_27 \
			(DRM_MODE_PICTURE_ASPECT_64_27<<19)
#define  DRM_MODE_FLAG_PIC_AR_256_135 \
			(DRM_MODE_PICTURE_ASPECT_256_135<<19)

#define  DRM_MODE_FLAG_ALL	(DRM_MODE_FLAG_PHSYNC |		\
				 DRM_MODE_FLAG_NHSYNC |		\
				 DRM_MODE_FLAG_PVSYNC |		\
				 DRM_MODE_FLAG_NVSYNC |		\
				 DRM_MODE_FLAG_INTERLACE |	\
				 DRM_MODE_FLAG_DBLSCAN |	\
				 DRM_MODE_FLAG_CSYNC |		\
				 DRM_MODE_FLAG_PCSYNC |		\
				 DRM_MODE_FLAG_NCSYNC |		\
				 DRM_MODE_FLAG_HSKEW |		\
				 DRM_MODE_FLAG_DBLCLK |		\
				 DRM_MODE_FLAG_CLKDIV2 |	\
				 DRM_MODE_FLAG_3D_MASK)

/* DPMS flags */
/* bit compatible with the xorg definitions. */
#define DRM_MODE_DPMS_ON	0
#define DRM_MODE_DPMS_STANDBY	1
#define DRM_MODE_DPMS_SUSPEND	2
#define DRM_MODE_DPMS_OFF	3

/* Scaling mode options */
#define DRM_MODE_SCALE_NONE		0 /* Unmodified timing (display or
					     software can still scale) */
#define DRM_MODE_SCALE_FULLSCREEN	1 /* Full screen, ignore aspect */
#define DRM_MODE_SCALE_CENTER		2 /* Centered, no scaling */
#define DRM_MODE_SCALE_ASPECT		3 /* Full screen, preserve aspect */

/* Dithering mode options */
#define DRM_MODE_DITHERING_OFF	0
#define DRM_MODE_DITHERING_ON	1
#define DRM_MODE_DITHERING_AUTO 2

/* Dirty info options */
#define DRM_MODE_DIRTY_OFF      0
#define DRM_MODE_DIRTY_ON       1
#define DRM_MODE_DIRTY_ANNOTATE 2

/* Link Status options */
#define DRM_MODE_LINK_STATUS_GOOD	0
#define DRM_MODE_LINK_STATUS_BAD	1

/*
 * DRM_MODE_ROTATE_<degrees>
 *
 * Signals that a drm plane is been rotated <degrees> degrees in counter
 * clockwise direction.
 *
 * This define is provided as a convenience, looking up the property id
 * using the name->prop id lookup is the preferred method.
 */
#define DRM_MODE_ROTATE_0       (1<<0)
#define DRM_MODE_ROTATE_90      (1<<1)
#define DRM_MODE_ROTATE_180     (1<<2)
#define DRM_MODE_ROTATE_270     (1<<3)

/*
 * DRM_MODE_ROTATE_MASK
 *
 * Bitmask used to look for drm plane rotations.
 */
#define DRM_MODE_ROTATE_MASK (\
		DRM_MODE_ROTATE_0  | \
		DRM_MODE_ROTATE_90  | \
		DRM_MODE_ROTATE_180 | \
		DRM_MODE_ROTATE_270)

/*
 * DRM_MODE_REFLECT_<axis>
 *
 * Signals that the contents of a drm plane is reflected along the <axis> axis,
 * in the same way as mirroring.
 * See kerneldoc chapter "Plane Composition Properties" for more details.
 *
 * This define is provided as a convenience, looking up the property id
 * using the name->prop id lookup is the preferred method.
 */
#define DRM_MODE_REFLECT_X      (1<<4)
#define DRM_MODE_REFLECT_Y      (1<<5)

/*
 * DRM_MODE_REFLECT_MASK
 *
 * Bitmask used to look for drm plane reflections.
 */
#define DRM_MODE_REFLECT_MASK (\
		DRM_MODE_REFLECT_X | \
		DRM_MODE_REFLECT_Y)

/* Content Protection Flags */
#define DRM_MODE_CONTENT_PROTECTION_UNDESIRED	0
#define DRM_MODE_CONTENT_PROTECTION_DESIRED     1
#define DRM_MODE_CONTENT_PROTECTION_ENABLED     2

/**
 * struct drm_mode_modeinfo - Display mode information.
 * @clock: pixel clock in kHz
 * @hdisplay: horizontal display size
 * @hsync_start: horizontal sync start
 * @hsync_end: horizontal sync end
 * @htotal: horizontal total size
 * @hskew: horizontal skew
 * @vdisplay: vertical display size
 * @vsync_start: vertical sync start
 * @vsync_end: vertical sync end
 * @vtotal: vertical total size
 * @vscan: vertical scan
 * @vrefresh: approximate vertical refresh rate in Hz
 * @flags: bitmask of misc. flags, see DRM_MODE_FLAG_* defines
 * @type: bitmask of type flags, see DRM_MODE_TYPE_* defines
 * @name: string describing the mode resolution
 *
 * This is the user-space API display mode information structure. For the
 * kernel version see struct drm_display_mode.
 */
struct drm_mode_modeinfo {
	__u32 clock;
	__u16 hdisplay;
	__u16 hsync_start;
	__u16 hsync_end;
	__u16 htotal;
	__u16 hskew;
	__u16 vdisplay;
	__u16 vsync_start;
	__u16 vsync_end;
	__u16 vtotal;
	__u16 vscan;

	__u32 vrefresh;

	__u32 flags;
	__u32 type;
	char name[DRM_DISPLAY_MODE_LEN];
};

struct drm_mode_card_res {
	__u64 fb_id_ptr;
	__u64 crtc_id_ptr;
	__u64 connector_id_ptr;
	__u64 encoder_id_ptr;
	__u32 count_fbs;
	__u32 count_crtcs;
	__u32 count_connectors;
	__u32 count_encoders;
	__u32 min_width;
	__u32 max_width;
	__u32 min_height;
	__u32 max_height;
};

struct drm_mode_crtc {
	__u64 set_connectors_ptr;
	__u32 count_connectors;

	__u32 crtc_id; /**< Id */
	__u32 fb_id; /**< Id of framebuffer */

	__u32 x; /**< x Position on the framebuffer */
	__u32 y; /**< y Position on the framebuffer */

	__u32 gamma_size;
	__u32 mode_valid;
	struct drm_mode_modeinfo mode;
};

#define DRM_MODE_PRESENT_TOP_FIELD	(1<<0)
#define DRM_MODE_PRESENT_BOTTOM_FIELD	(1<<1)

/* Planes blend with or override other bits on the CRTC */
struct drm_mode_set_plane {
	__u32 plane_id;
	__u32 crtc_id;
	__u32 fb_id; /* fb object contains surface format type */
	__u32 flags; /* see above flags */

	/* Signed dest location allows it to be partially off screen */
	__s32 crtc_x;
	__s32 crtc_y;
	__u32 crtc_w;
	__u32 crtc_h;

	/* Source values are 16.16 fixed point */
	__u32 src_x;
	__u32 src_y;
	__u32 src_h;
	__u32 src_w;
};

struct drm_mode_get_plane {
	__u32 plane_id;

	__u32 crtc_id;
	__u32 fb_id;

	__u32 possible_crtcs;
	__u32 gamma_size;

	__u32 count_format_types;
	__u64 format_type_ptr;
};

struct drm_mode_get_plane_res {
	__u64 plane_id_ptr;
	__u32 count_planes;
};

#define DRM_MODE_ENCODER_NONE	0
#define DRM_MODE_ENCODER_DAC	1
#define DRM_MODE_ENCODER_TMDS	2
#define DRM_MODE_ENCODER_LVDS	3
#define DRM_MODE_ENCODER_TVDAC	4
#define DRM_MODE_ENCODER_VIRTUAL 5
#define DRM_MODE_ENCODER_DSI	6
#define DRM_MODE_ENCODER_DPMST	7
#define DRM_MODE_ENCODER_DPI	8

struct drm_mode_get_encoder {
	__u32 encoder_id;
	__u32 encoder_type;

	__u32 crtc_id; /**< Id of crtc */

	__u32 possible_crtcs;
	__u32 possible_clones;
};

/* This is for connectors with multiple signal types. */
/* Try to match DRM_MODE_CONNECTOR_X as closely as possible. */
enum drm_mode_subconnector {
	DRM_MODE_SUBCONNECTOR_Automatic   = 0,  /* DVI-I, TV     */
	DRM_MODE_SUBCONNECTOR_Unknown     = 0,  /* DVI-I, TV, DP */
	DRM_MODE_SUBCONNECTOR_VGA	  = 1,  /*            DP */
	DRM_MODE_SUBCONNECTOR_DVID	  = 3,  /* DVI-I      DP */
	DRM_MODE_SUBCONNECTOR_DVIA	  = 4,  /* DVI-I         */
	DRM_MODE_SUBCONNECTOR_Composite   = 5,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_SVIDEO	  = 6,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_Component   = 8,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_SCART	  = 9,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_DisplayPort = 10, /*            DP */
	DRM_MODE_SUBCONNECTOR_HDMIA       = 11, /*            DP */
	DRM_MODE_SUBCONNECTOR_Native      = 15, /*            DP */
	DRM_MODE_SUBCONNECTOR_Wireless    = 18, /*            DP */
};

#define DRM_MODE_CONNECTOR_Unknown	0
#define DRM_MODE_CONNECTOR_VGA		1
#define DRM_MODE_CONNECTOR_DVII		2
#define DRM_MODE_CONNECTOR_DVID		3
#define DRM_MODE_CONNECTOR_DVIA		4
#define DRM_MODE_CONNECTOR_Composite	5
#define DRM_MODE_CONNECTOR_SVIDEO	6
#define DRM_MODE_CONNECTOR_LVDS		7
#define DRM_MODE_CONNECTOR_Component	8
#define DRM_MODE_CONNECTOR_9PinDIN	9
#define DRM_MODE_CONNECTOR_DisplayPort	10
#define DRM_MODE_CONNECTOR_HDMIA	11
#define DRM_MODE_CONNECTOR_HDMIB	12
#define DRM_MODE_CONNECTOR_TV		13
#define DRM_MODE_CONNECTOR_eDP		14
#define DRM_MODE_CONNECTOR_VIRTUAL      15
#define DRM_MODE_CONNECTOR_DSI		16
#define DRM_MODE_CONNECTOR_DPI		17
#define DRM_MODE_CONNECTOR_WRITEBACK	18
#define DRM_MODE_CONNECTOR_SPI		19
#define DRM_MODE_CONNECTOR_USB		20

/**
 * struct drm_mode_get_connector - Get connector metadata.
 *
 * User-space can perform a GETCONNECTOR ioctl to retrieve information about a
 * connector. User-space is expected to retrieve encoders, modes and properties
 * by performing this ioctl at least twice: the first time to retrieve the
 * number of elements, the second time to retrieve the elements themselves.
 *
 * To retrieve the number of elements, set @count_props and @count_encoders to
 * zero, set @count_modes to 1, and set @modes_ptr to a temporary struct
 * drm_mode_modeinfo element.
 *
 * To retrieve the elements, allocate arrays for @encoders_ptr, @modes_ptr,
 * @props_ptr and @prop_values_ptr, then set @count_modes, @count_props and
 * @count_encoders to their capacity.
 *
 * Performing the ioctl only twice may be racy: the number of elements may have
 * changed with a hotplug event in-between the two ioctls. User-space is
 * expected to retry the last ioctl until the number of elements stabilizes.
 * The kernel won't fill any array which doesn't have the expected length.
 *
 * **Force-probing a connector**
 *
 * If the @count_modes field is set to zero and the DRM client is the current
 * DRM master, the kernel will perform a forced probe on the connector to
 * refresh the connector status, modes and EDID. A forced-probe can be slow,
 * might cause flickering and the ioctl will block.
 *
 * User-space needs to force-probe connectors to ensure their metadata is
 * up-to-date at startup and after receiving a hot-plug event. User-space
 * may perform a forced-probe when the user explicitly requests it. User-space
 * shouldn't perform a forced-probe in other situations.
 */
struct drm_mode_get_connector {
	/** @encoders_ptr: Pointer to ``__u32`` array of object IDs. */
	__u64 encoders_ptr;
	/** @modes_ptr: Pointer to struct drm_mode_modeinfo array. */
	__u64 modes_ptr;
	/** @props_ptr: Pointer to ``__u32`` array of property IDs. */
	__u64 props_ptr;
	/** @prop_values_ptr: Pointer to ``__u64`` array of property values. */
	__u64 prop_values_ptr;

	/** @count_modes: Number of modes. */
	__u32 count_modes;
	/** @count_props: Number of properties. */
	__u32 count_props;
	/** @count_encoders: Number of encoders. */
	__u32 count_encoders;

	/** @encoder_id: Object ID of the current encoder. */
	__u32 encoder_id;
	/** @connector_id: Object ID of the connector. */
	__u32 connector_id;
	/**
	 * @connector_type: Type of the connector.
	 *
	 * See DRM_MODE_CONNECTOR_* defines.
	 */
	__u32 connector_type;
	/**
	 * @connector_type_id: Type-specific connector number.
	 *
	 * This is not an object ID. This is a per-type connector number. Each
	 * (type, type_id) combination is unique across all connectors of a DRM
	 * device.
	 */
	__u32 connector_type_id;

	/**
	 * @connection: Status of the connector.
	 *
	 * See enum drm_connector_status.
	 */
	__u32 connection;
	/** @mm_width: Width of the connected sink in millimeters. */
	__u32 mm_width;
	/** @mm_height: Height of the connected sink in millimeters. */
	__u32 mm_height;
	/**
	 * @subpixel: Subpixel order of the connected sink.
	 *
	 * See enum subpixel_order.
	 */
	__u32 subpixel;

	/** @pad: Padding, must be zero. */
	__u32 pad;
};

#define DRM_MODE_PROP_PENDING	(1<<0) /* deprecated, do not use */
#define DRM_MODE_PROP_RANGE	(1<<1)
#define DRM_MODE_PROP_IMMUTABLE	(1<<2)
#define DRM_MODE_PROP_ENUM	(1<<3) /* enumerated type with text strings */
#define DRM_MODE_PROP_BLOB	(1<<4)
#define DRM_MODE_PROP_BITMASK	(1<<5) /* bitmask of enumerated types */

/* non-extended types: legacy bitmask, one bit per type: */
#define DRM_MODE_PROP_LEGACY_TYPE  ( \
		DRM_MODE_PROP_RANGE | \
		DRM_MODE_PROP_ENUM | \
		DRM_MODE_PROP_BLOB | \
		DRM_MODE_PROP_BITMASK)

/* extended-types: rather than continue to consume a bit per type,
 * grab a chunk of the bits to use as integer type id.
 */
#define DRM_MODE_PROP_EXTENDED_TYPE	0x0000ffc0
#define DRM_MODE_PROP_TYPE(n)		((n) << 6)
#define DRM_MODE_PROP_OBJECT		DRM_MODE_PROP_TYPE(1)
#define DRM_MODE_PROP_SIGNED_RANGE	DRM_MODE_PROP_TYPE(2)

/* the PROP_ATOMIC flag is used to hide properties from userspace that
 * is not aware of atomic properties.  This is mostly to work around
 * older userspace (DDX drivers) that read/write each prop they find,
 * witout being aware that this could be triggering a lengthy modeset.
 */
#define DRM_MODE_PROP_ATOMIC        0x80000000

struct drm_mode_property_enum {
	__u64 value;
	char name[DRM_PROP_NAME_LEN];
};

struct drm_mode_get_property {
	__u64 values_ptr; /* values and blob lengths */
	__u64 enum_blob_ptr; /* enum and blob id ptrs */

	__u32 prop_id;
	__u32 flags;
	char name[DRM_PROP_NAME_LEN];

	__u32 count_values;
	/* This is only used to count enum values, not blobs. The _blobs is
	 * simply because of a historical reason, i.e. backwards compat. */
	__u32 count_enum_blobs;
};

struct drm_mode_connector_set_property {
	__u64 value;
	__u32 prop_id;
	__u32 connector_id;
};

#define DRM_MODE_OBJECT_CRTC 0xcccccccc
#define DRM_MODE_OBJECT_CONNECTOR 0xc0c0c0c0
#define DRM_MODE_OBJECT_ENCODER 0xe0e0e0e0
#define DRM_MODE_OBJECT_MODE 0xdededede
#define DRM_MODE_OBJECT_PROPERTY 0xb0b0b0b0
#define DRM_MODE_OBJECT_FB 0xfbfbfbfb
#define DRM_MODE_OBJECT_BLOB 0xbbbbbbbb
#define DRM_MODE_OBJECT_PLANE 0xeeeeeeee
#define DRM_MODE_OBJECT_ANY 0

struct drm_mode_obj_get_properties {
	__u64 props_ptr;
	__u64 prop_values_ptr;
	__u32 count_props;
	__u32 obj_id;
	__u32 obj_type;
};

struct drm_mode_obj_set_property {
	__u64 value;
	__u32 prop_id;
	__u32 obj_id;
	__u32 obj_type;
};

struct drm_mode_get_blob {
	__u32 blob_id;
	__u32 length;
	__u64 data;
};

struct drm_mode_fb_cmd {
	__u32 fb_id;
	__u32 width;
	__u32 height;
	__u32 pitch;
	__u32 bpp;
	__u32 depth;
	/* driver specific handle */
	__u32 handle;
};

#define DRM_MODE_FB_INTERLACED	(1<<0) /* for interlaced framebuffers */
#define DRM_MODE_FB_MODIFIERS	(1<<1) /* enables ->modifer[] */

struct drm_mode_fb_cmd2 {
	__u32 fb_id;
	__u32 width;
	__u32 height;
	__u32 pixel_format; /* fourcc code from drm_fourcc.h */
	__u32 flags; /* see above flags */

	/*
	 * In case of planar formats, this ioctl allows up to 4
	 * buffer objects with offsets and pitches per plane.
	 * The pitch and offset order is dictated by the fourcc,
	 * e.g. NV12 (https://fourcc.org/yuv.php#NV12) is described as:
	 *
	 *   YUV 4:2:0 image with a plane of 8 bit Y samples
	 *   followed by an interleaved U/V plane containing
	 *   8 bit 2x2 subsampled colour difference samples.
	 *
	 * So it would consist of Y as offsets[0] and UV as
	 * offsets[1].  Note that offsets[0] will generally
	 * be 0 (but this is not required).
	 *
	 * To accommodate tiled, compressed, etc formats, a
	 * modifier can be specified.  The default value of zero
	 * indicates "native" format as specified by the fourcc.
	 * Vendor specific modifier token.  Note that even though
	 * it looks like we have a modifier per-plane, we in fact
	 * do not. The modifier for each plane must be identical.
	 * Thus all combinations of different data layouts for
	 * multi plane formats must be enumerated as separate
	 * modifiers.
	 */
	__u32 handles[4];
	__u32 pitches[4]; /* pitch for each plane */
	__u32 offsets[4]; /* offset of each plane */
	__u64 modifier[4]; /* ie, tiling, compress */
};

#define DRM_MODE_FB_DIRTY_ANNOTATE_COPY 0x01
#define DRM_MODE_FB_DIRTY_ANNOTATE_FILL 0x02
#define DRM_MODE_FB_DIRTY_FLAGS         0x03

#define DRM_MODE_FB_DIRTY_MAX_CLIPS     256

/*
 * Mark a region of a framebuffer as dirty.
 *
 * Some hardware does not automatically update display contents
 * as a hardware or software draw to a framebuffer. This ioctl
 * allows userspace to tell the kernel and the hardware what
 * regions of the framebuffer have changed.
 *
 * The kernel or hardware is free to update more then just the
 * region specified by the clip rects. The kernel or hardware
 * may also delay and/or coalesce several calls to dirty into a
 * single update.
 *
 * Userspace may annotate the updates, the annotates are a
 * promise made by the caller that the change is either a copy
 * of pixels or a fill of a single color in the region specified.
 *
 * If the DRM_MODE_FB_DIRTY_ANNOTATE_COPY flag is given then
 * the number of updated regions are half of num_clips given,
 * where the clip rects are paired in src and dst. The width and
 * height of each one of the pairs must match.
 *
 * If the DRM_MODE_FB_DIRTY_ANNOTATE_FILL flag is given the caller
 * promises that the region specified of the clip rects is filled
 * completely with a single color as given in the color argument.
 */

struct drm_mode_fb_dirty_cmd {
	__u32 fb_id;
	__u32 flags;
	__u32 color;
	__u32 num_clips;
	__u64 clips_ptr;
};

struct drm_mode_mode_cmd {
	__u32 connector_id;
	struct drm_mode_modeinfo mode;
};

#define DRM_MODE_CURSOR_BO	0x01
#define DRM_MODE_CURSOR_MOVE	0x02
#define DRM_MODE_CURSOR_FLAGS	0x03

/*
 * depending on the value in flags different members are used.
 *
 * CURSOR_BO uses
 *    crtc_id
 *    width
 *    height
 *    handle - if 0 turns the cursor off
 *
 * CURSOR_MOVE uses
 *    crtc_id
 *    x
 *    y
 */
struct drm_mode_cursor {
	__u32 flags;
	__u32 crtc_id;
	__s32 x;
	__s32 y;
	__u32 width;
	__u32 height;
	/* driver specific handle */
	__u32 handle;
};

struct drm_mode_cursor2 {
	__u32 flags;
	__u32 crtc_id;
	__s32 x;
	__s32 y;
	__u32 width;
	__u32 height;
	/* driver specific handle */
	__u32 handle;
	__s32 hot_x;
	__s32 hot_y;
};

struct drm_mode_crtc_lut {
	__u32 crtc_id;
	__u32 gamma_size;

	/* pointers to arrays */
	__u64 red;
	__u64 green;
	__u64 blue;
};

struct drm_color_ctm {
	/*
	 * Conversion matrix in S31.32 sign-magnitude
	 * (not two's complement!) format.
	 */
	__u64 matrix[9];
};

struct drm_color_lut {
	/*
	 * Values are mapped linearly to 0.0 - 1.0 range, with 0x0 == 0.0 and
	 * 0xffff == 1.0.
	 */
	__u16 red;
	__u16 green;
	__u16 blue;
	__u16 reserved;
};

/**
 * struct hdr_metadata_infoframe - HDR Metadata Infoframe Data.
 *
 * HDR Metadata Infoframe as per CTA 861.G spec. This is expected
 * to match exactly with the spec.
 *
 * Userspace is expected to pass the metadata information as per
 * the format described in this structure.
 */
struct hdr_metadata_infoframe {
	/**
	 * @eotf: Electro-Optical Transfer Function (EOTF)
	 * used in the stream.
	 */
	__u8 eotf;
	/**
	 * @metadata_type: Static_Metadata_Descriptor_ID.
	 */
	__u8 metadata_type;
	/**
	 * @display_primaries: Color Primaries of the Data.
	 * These are coded as unsigned 16-bit values in units of
	 * 0.00002, where 0x0000 represents zero and 0xC350
	 * represents 1.0000.
	 * @display_primaries.x: X cordinate of color primary.
	 * @display_primaries.y: Y cordinate of color primary.
	 */
	struct {
		__u16 x, y;
		} display_primaries[3];
	/**
	 * @white_point: White Point of Colorspace Data.
	 * These are coded as unsigned 16-bit values in units of
	 * 0.00002, where 0x0000 represents zero and 0xC350
	 * represents 1.0000.
	 * @white_point.x: X cordinate of whitepoint of color primary.
	 * @white_point.y: Y cordinate of whitepoint of color primary.
	 */
	struct {
		__u16 x, y;
		} white_point;
	/**
	 * @max_display_mastering_luminance: Max Mastering Display Luminance.
	 * This value is coded as an unsigned 16-bit value in units of 1 cd/m2,
	 * where 0x0001 represents 1 cd/m2 and 0xFFFF represents 65535 cd/m2.
	 */
	__u16 max_display_mastering_luminance;
	/**
	 * @min_display_mastering_luminance: Min Mastering Display Luminance.
	 * This value is coded as an unsigned 16-bit value in units of
	 * 0.0001 cd/m2, where 0x0001 represents 0.0001 cd/m2 and 0xFFFF
	 * represents 6.5535 cd/m2.
	 */
	__u16 min_display_mastering_luminance;
	/**
	 * @max_cll: Max Content Light Level.
	 * This value is coded as an unsigned 16-bit value in units of 1 cd/m2,
	 * where 0x0001 represents 1 cd/m2 and 0xFFFF represents 65535 cd/m2.
	 */
	__u16 max_cll;
	/**
	 * @max_fall: Max Frame Average Light Level.
	 * This value is coded as an unsigned 16-bit value in units of 1 cd/m2,
	 * where 0x0001 represents 1 cd/m2 and 0xFFFF represents 65535 cd/m2.
	 */
	__u16 max_fall;
};

/**
 * struct hdr_output_metadata - HDR output metadata
 *
 * Metadata Information to be passed from userspace
 */
struct hdr_output_metadata {
	/**
	 * @metadata_type: Static_Metadata_Descriptor_ID.
	 */
	__u32 metadata_type;
	/**
	 * @hdmi_metadata_type1: HDR Metadata Infoframe.
	 */
	union {
		struct hdr_metadata_infoframe hdmi_metadata_type1;
	};
};

#define DRM_MODE_PAGE_FLIP_EVENT 0x01
#define DRM_MODE_PAGE_FLIP_ASYNC 0x02
#define DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE 0x4
#define DRM_MODE_PAGE_FLIP_TARGET_RELATIVE 0x8
#define DRM_MODE_PAGE_FLIP_TARGET (DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE | \
				   DRM_MODE_PAGE_FLIP_TARGET_RELATIVE)
#define DRM_MODE_PAGE_FLIP_FLAGS (DRM_MODE_PAGE_FLIP_EVENT | \
				  DRM_MODE_PAGE_FLIP_ASYNC | \
				  DRM_MODE_PAGE_FLIP_TARGET)

/*
 * Request a page flip on the specified crtc.
 *
 * This ioctl will ask KMS to schedule a page flip for the specified
 * crtc.  Once any pending rendering targeting the specified fb (as of
 * ioctl time) has completed, the crtc will be reprogrammed to display
 * that fb after the next vertical refresh.  The ioctl returns
 * immediately, but subsequent rendering to the current fb will block
 * in the execbuffer ioctl until the page flip happens.  If a page
 * flip is already pending as the ioctl is called, EBUSY will be
 * returned.
 *
 * Flag DRM_MODE_PAGE_FLIP_EVENT requests that drm sends back a vblank
 * event (see drm.h: struct drm_event_vblank) when the page flip is
 * done.  The user_data field passed in with this ioctl will be
 * returned as the user_data field in the vblank event struct.
 *
 * Flag DRM_MODE_PAGE_FLIP_ASYNC requests that the flip happen
 * 'as soon as possible', meaning that it not delay waiting for vblank.
 * This may cause tearing on the screen.
 *
 * The reserved field must be zero.
 */

struct drm_mode_crtc_page_flip {
	__u32 crtc_id;
	__u32 fb_id;
	__u32 flags;
	__u32 reserved;
	__u64 user_data;
};

/*
 * Request a page flip on the specified crtc.
 *
 * Same as struct drm_mode_crtc_page_flip, but supports new flags and
 * re-purposes the reserved field:
 *
 * The sequence field must be zero unless either of the
 * DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE/RELATIVE flags is specified. When
 * the ABSOLUTE flag is specified, the sequence field denotes the absolute
 * vblank sequence when the flip should take effect. When the RELATIVE
 * flag is specified, the sequence field denotes the relative (to the
 * current one when the ioctl is called) vblank sequence when the flip
 * should take effect. NOTE: DRM_IOCTL_WAIT_VBLANK must still be used to
 * make sure the vblank sequence before the target one has passed before
 * calling this ioctl. The purpose of the
 * DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE/RELATIVE flags is merely to clarify
 * the target for when code dealing with a page flip runs during a
 * vertical blank period.
 */

struct drm_mode_crtc_page_flip_target {
	__u32 crtc_id;
	__u32 fb_id;
	__u32 flags;
	__u32 sequence;
	__u64 user_data;
};

/* create a dumb scanout buffer */
struct drm_mode_create_dumb {
	__u32 height;
	__u32 width;
	__u32 bpp;
	__u32 flags;
	/* handle, pitch, size will be returned */
	__u32 handle;
	__u32 pitch;
	__u64 size;
};

/* set up for mmap of a dumb scanout buffer */
struct drm_mode_map_dumb {
	/** Handle for the object being mapped. */
	__u32 handle;
	__u32 pad;
	/**
	 * Fake offset to use for subsequent mmap call
	 *
	 * This is a fixed-size type for 32/64 compatibility.
	 */
	__u64 offset;
};

struct drm_mode_destroy_dumb {
	__u32 handle;
};

/* page-flip flags are valid, plus: */
#define DRM_MODE_ATOMIC_TEST_ONLY 0x0100
#define DRM_MODE_ATOMIC_NONBLOCK  0x0200
#define DRM_MODE_ATOMIC_ALLOW_MODESET 0x0400

#define DRM_MODE_ATOMIC_FLAGS (\
		DRM_MODE_PAGE_FLIP_EVENT |\
		DRM_MODE_PAGE_FLIP_ASYNC |\
		DRM_MODE_ATOMIC_TEST_ONLY |\
		DRM_MODE_ATOMIC_NONBLOCK |\
		DRM_MODE_ATOMIC_ALLOW_MODESET)

struct drm_mode_atomic {
	__u32 flags;
	__u32 count_objs;
	__u64 objs_ptr;
	__u64 count_props_ptr;
	__u64 props_ptr;
	__u64 prop_values_ptr;
	__u64 reserved;
	__u64 user_data;
};

struct drm_format_modifier_blob {
#define FORMAT_BLOB_CURRENT 1
	/* Version of this blob format */
	__u32 version;

	/* Flags */
	__u32 flags;

	/* Number of fourcc formats supported */
	__u32 count_formats;

	/* Where in this blob the formats exist (in bytes) */
	__u32 formats_offset;

	/* Number of drm_format_modifiers */
	__u32 count_modifiers;

	/* Where in this blob the modifiers exist (in bytes) */
	__u32 modifiers_offset;

	/* __u32 formats[] */
	/* struct drm_format_modifier modifiers[] */
};

struct drm_format_modifier {
	/* Bitmask of formats in get_plane format list this info applies to. The
	 * offset allows a sliding window of which 64 formats (bits).
	 *
	 * Some examples:
	 * In today's world with < 65 formats, and formats 0, and 2 are
	 * supported
	 * 0x0000000000000005
	 *		  ^-offset = 0, formats = 5
	 *
	 * If the number formats grew to 128, and formats 98-102 are
	 * supported with the modifier:
	 *
	 * 0x0000007c00000000 0000000000000000
	 *		  ^
	 *		  |__offset = 64, formats = 0x7c00000000
	 *
	 */
	__u64 formats;
	__u32 offset;
	__u32 pad;

	/* The modifier that applies to the >get_plane format list bitmask. */
	__u64 modifier;
};

/**
 * struct drm_mode_create_blob - Create New blob property
 *
 * Create a new 'blob' data property, copying length bytes from data pointer,
 * and returning new blob ID.
 */
struct drm_mode_create_blob {
	/** @data: Pointer to data to copy. */
	__u64 data;
	/** @length: Length of data to copy. */
	__u32 length;
	/** @blob_id: Return: new property ID. */
	__u32 blob_id;
};

/**
 * struct drm_mode_destroy_blob - Destroy user blob
 * @blob_id: blob_id to destroy
 *
 * Destroy a user-created blob property.
 *
 * User-space can release blobs as soon as they do not need to refer to them by
 * their blob object ID.  For instance, if you are using a MODE_ID blob in an
 * atomic commit and you will not make another commit re-using the same ID, you
 * can destroy the blob as soon as the commit has been issued, without waiting
 * for it to complete.
 */
struct drm_mode_destroy_blob {
	__u32 blob_id;
};

/**
 * struct drm_mode_create_lease - Create lease
 *
 * Lease mode resources, creating another drm_master.
 */
struct drm_mode_create_lease {
	/** @object_ids: Pointer to array of object ids (__u32) */
	__u64 object_ids;
	/** @object_count: Number of object ids */
	__u32 object_count;
	/** @flags: flags for new FD (O_CLOEXEC, etc) */
	__u32 flags;

	/** @lessee_id: Return: unique identifier for lessee. */
	__u32 lessee_id;
	/** @fd: Return: file descriptor to new drm_master file */
	__u32 fd;
};

/**
 * struct drm_mode_list_lessees - List lessees
 *
 * List lesses from a drm_master.
 */
struct drm_mode_list_lessees {
	/**
	 * @count_lessees: Number of lessees.
	 *
	 * On input, provides length of the array.
	 * On output, provides total number. No
	 * more than the input number will be written
	 * back, so two calls can be used to get
	 * the size and then the data.
	 */
	__u32 count_lessees;
	/** @pad: Padding. */
	__u32 pad;

	/**
	 * @lessees_ptr: Pointer to lessees.
	 *
	 * Pointer to __u64 array of lessee ids
	 */
	__u64 lessees_ptr;
};

/**
 * struct drm_mode_get_lease - Get Lease
 *
 * Get leased objects.
 */
struct drm_mode_get_lease {
	/**
	 * @count_objects: Number of leased objects.
	 *
	 * On input, provides length of the array.
	 * On output, provides total number. No
	 * more than the input number will be written
	 * back, so two calls can be used to get
	 * the size and then the data.
	 */
	__u32 count_objects;
	/** @pad: Padding. */
	__u32 pad;

	/**
	 * @objects_ptr: Pointer to objects.
	 *
	 * Pointer to __u32 array of object ids.
	 */
	__u64 objects_ptr;
};

/**
 * struct drm_mode_revoke_lease - Revoke lease
 */
struct drm_mode_revoke_lease {
	/** @lessee_id: Unique ID of lessee */
	__u32 lessee_id;
};

/**
 * struct drm_mode_rect - Two dimensional rectangle.
 * @x1: Horizontal starting coordinate (inclusive).
 * @y1: Vertical starting coordinate (inclusive).
 * @x2: Horizontal ending coordinate (exclusive).
 * @y2: Vertical ending coordinate (exclusive).
 *
 * With drm subsystem using struct drm_rect to manage rectangular area this
 * export it to user-space.
 *
 * Currently used by drm_mode_atomic blob property FB_DAMAGE_CLIPS.
 */
struct drm_mode_rect {
	__s32 x1;
	__s32 y1;
	__s32 x2;
	__s32 y2;
};

#if defined(__cplusplus)
}
#endif

#endif

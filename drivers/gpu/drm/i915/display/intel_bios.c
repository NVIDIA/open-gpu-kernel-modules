/*
 * Copyright © 2006 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

#include <drm/drm_dp_helper.h>

#include "display/intel_display.h"
#include "display/intel_display_types.h"
#include "display/intel_gmbus.h"

#include "i915_drv.h"

#define _INTEL_BIOS_PRIVATE
#include "intel_vbt_defs.h"

/**
 * DOC: Video BIOS Table (VBT)
 *
 * The Video BIOS Table, or VBT, provides platform and board specific
 * configuration information to the driver that is not discoverable or available
 * through other means. The configuration is mostly related to display
 * hardware. The VBT is available via the ACPI OpRegion or, on older systems, in
 * the PCI ROM.
 *
 * The VBT consists of a VBT Header (defined as &struct vbt_header), a BDB
 * Header (&struct bdb_header), and a number of BIOS Data Blocks (BDB) that
 * contain the actual configuration information. The VBT Header, and thus the
 * VBT, begins with "$VBT" signature. The VBT Header contains the offset of the
 * BDB Header. The data blocks are concatenated after the BDB Header. The data
 * blocks have a 1-byte Block ID, 2-byte Block Size, and Block Size bytes of
 * data. (Block 53, the MIPI Sequence Block is an exception.)
 *
 * The driver parses the VBT during load. The relevant information is stored in
 * driver private data for ease of use, and the actual VBT is not read after
 * that.
 */

/* Wrapper for VBT child device config */
struct intel_bios_encoder_data {
	struct drm_i915_private *i915;

	struct child_device_config child;
	struct dsc_compression_parameters_entry *dsc;
	struct list_head node;
};

#define	SLAVE_ADDR1	0x70
#define	SLAVE_ADDR2	0x72

/* Get BDB block size given a pointer to Block ID. */
static u32 _get_blocksize(const u8 *block_base)
{
	/* The MIPI Sequence Block v3+ has a separate size field. */
	if (*block_base == BDB_MIPI_SEQUENCE && *(block_base + 3) >= 3)
		return *((const u32 *)(block_base + 4));
	else
		return *((const u16 *)(block_base + 1));
}

/* Get BDB block size give a pointer to data after Block ID and Block Size. */
static u32 get_blocksize(const void *block_data)
{
	return _get_blocksize(block_data - 3);
}

static const void *
find_section(const void *_bdb, enum bdb_block_id section_id)
{
	const struct bdb_header *bdb = _bdb;
	const u8 *base = _bdb;
	int index = 0;
	u32 total, current_size;
	enum bdb_block_id current_id;

	/* skip to first section */
	index += bdb->header_size;
	total = bdb->bdb_size;

	/* walk the sections looking for section_id */
	while (index + 3 < total) {
		current_id = *(base + index);
		current_size = _get_blocksize(base + index);
		index += 3;

		if (index + current_size > total)
			return NULL;

		if (current_id == section_id)
			return base + index;

		index += current_size;
	}

	return NULL;
}

static void
fill_detail_timing_data(struct drm_display_mode *panel_fixed_mode,
			const struct lvds_dvo_timing *dvo_timing)
{
	panel_fixed_mode->hdisplay = (dvo_timing->hactive_hi << 8) |
		dvo_timing->hactive_lo;
	panel_fixed_mode->hsync_start = panel_fixed_mode->hdisplay +
		((dvo_timing->hsync_off_hi << 8) | dvo_timing->hsync_off_lo);
	panel_fixed_mode->hsync_end = panel_fixed_mode->hsync_start +
		((dvo_timing->hsync_pulse_width_hi << 8) |
			dvo_timing->hsync_pulse_width_lo);
	panel_fixed_mode->htotal = panel_fixed_mode->hdisplay +
		((dvo_timing->hblank_hi << 8) | dvo_timing->hblank_lo);

	panel_fixed_mode->vdisplay = (dvo_timing->vactive_hi << 8) |
		dvo_timing->vactive_lo;
	panel_fixed_mode->vsync_start = panel_fixed_mode->vdisplay +
		((dvo_timing->vsync_off_hi << 4) | dvo_timing->vsync_off_lo);
	panel_fixed_mode->vsync_end = panel_fixed_mode->vsync_start +
		((dvo_timing->vsync_pulse_width_hi << 4) |
			dvo_timing->vsync_pulse_width_lo);
	panel_fixed_mode->vtotal = panel_fixed_mode->vdisplay +
		((dvo_timing->vblank_hi << 8) | dvo_timing->vblank_lo);
	panel_fixed_mode->clock = dvo_timing->clock * 10;
	panel_fixed_mode->type = DRM_MODE_TYPE_PREFERRED;

	if (dvo_timing->hsync_positive)
		panel_fixed_mode->flags |= DRM_MODE_FLAG_PHSYNC;
	else
		panel_fixed_mode->flags |= DRM_MODE_FLAG_NHSYNC;

	if (dvo_timing->vsync_positive)
		panel_fixed_mode->flags |= DRM_MODE_FLAG_PVSYNC;
	else
		panel_fixed_mode->flags |= DRM_MODE_FLAG_NVSYNC;

	panel_fixed_mode->width_mm = (dvo_timing->himage_hi << 8) |
		dvo_timing->himage_lo;
	panel_fixed_mode->height_mm = (dvo_timing->vimage_hi << 8) |
		dvo_timing->vimage_lo;

	/* Some VBTs have bogus h/vtotal values */
	if (panel_fixed_mode->hsync_end > panel_fixed_mode->htotal)
		panel_fixed_mode->htotal = panel_fixed_mode->hsync_end + 1;
	if (panel_fixed_mode->vsync_end > panel_fixed_mode->vtotal)
		panel_fixed_mode->vtotal = panel_fixed_mode->vsync_end + 1;

	drm_mode_set_name(panel_fixed_mode);
}

static const struct lvds_dvo_timing *
get_lvds_dvo_timing(const struct bdb_lvds_lfp_data *lvds_lfp_data,
		    const struct bdb_lvds_lfp_data_ptrs *lvds_lfp_data_ptrs,
		    int index)
{
	/*
	 * the size of fp_timing varies on the different platform.
	 * So calculate the DVO timing relative offset in LVDS data
	 * entry to get the DVO timing entry
	 */

	int lfp_data_size =
		lvds_lfp_data_ptrs->ptr[1].dvo_timing_offset -
		lvds_lfp_data_ptrs->ptr[0].dvo_timing_offset;
	int dvo_timing_offset =
		lvds_lfp_data_ptrs->ptr[0].dvo_timing_offset -
		lvds_lfp_data_ptrs->ptr[0].fp_timing_offset;
	char *entry = (char *)lvds_lfp_data->data + lfp_data_size * index;

	return (struct lvds_dvo_timing *)(entry + dvo_timing_offset);
}

/* get lvds_fp_timing entry
 * this function may return NULL if the corresponding entry is invalid
 */
static const struct lvds_fp_timing *
get_lvds_fp_timing(const struct bdb_header *bdb,
		   const struct bdb_lvds_lfp_data *data,
		   const struct bdb_lvds_lfp_data_ptrs *ptrs,
		   int index)
{
	size_t data_ofs = (const u8 *)data - (const u8 *)bdb;
	u16 data_size = ((const u16 *)data)[-1]; /* stored in header */
	size_t ofs;

	if (index >= ARRAY_SIZE(ptrs->ptr))
		return NULL;
	ofs = ptrs->ptr[index].fp_timing_offset;
	if (ofs < data_ofs ||
	    ofs + sizeof(struct lvds_fp_timing) > data_ofs + data_size)
		return NULL;
	return (const struct lvds_fp_timing *)((const u8 *)bdb + ofs);
}

/* Parse general panel options */
static void
parse_panel_options(struct drm_i915_private *i915,
		    const struct bdb_header *bdb)
{
	const struct bdb_lvds_options *lvds_options;
	int panel_type;
	int drrs_mode;
	int ret;

	lvds_options = find_section(bdb, BDB_LVDS_OPTIONS);
	if (!lvds_options)
		return;

	i915->vbt.lvds_dither = lvds_options->pixel_dither;

	ret = intel_opregion_get_panel_type(i915);
	if (ret >= 0) {
		drm_WARN_ON(&i915->drm, ret > 0xf);
		panel_type = ret;
		drm_dbg_kms(&i915->drm, "Panel type: %d (OpRegion)\n",
			    panel_type);
	} else {
		if (lvds_options->panel_type > 0xf) {
			drm_dbg_kms(&i915->drm,
				    "Invalid VBT panel type 0x%x\n",
				    lvds_options->panel_type);
			return;
		}
		panel_type = lvds_options->panel_type;
		drm_dbg_kms(&i915->drm, "Panel type: %d (VBT)\n",
			    panel_type);
	}

	i915->vbt.panel_type = panel_type;

	drrs_mode = (lvds_options->dps_panel_type_bits
				>> (panel_type * 2)) & MODE_MASK;
	/*
	 * VBT has static DRRS = 0 and seamless DRRS = 2.
	 * The below piece of code is required to adjust vbt.drrs_type
	 * to match the enum drrs_support_type.
	 */
	switch (drrs_mode) {
	case 0:
		i915->vbt.drrs_type = STATIC_DRRS_SUPPORT;
		drm_dbg_kms(&i915->drm, "DRRS supported mode is static\n");
		break;
	case 2:
		i915->vbt.drrs_type = SEAMLESS_DRRS_SUPPORT;
		drm_dbg_kms(&i915->drm,
			    "DRRS supported mode is seamless\n");
		break;
	default:
		i915->vbt.drrs_type = DRRS_NOT_SUPPORTED;
		drm_dbg_kms(&i915->drm,
			    "DRRS not supported (VBT input)\n");
		break;
	}
}

/* Try to find integrated panel timing data */
static void
parse_lfp_panel_dtd(struct drm_i915_private *i915,
		    const struct bdb_header *bdb)
{
	const struct bdb_lvds_lfp_data *lvds_lfp_data;
	const struct bdb_lvds_lfp_data_ptrs *lvds_lfp_data_ptrs;
	const struct lvds_dvo_timing *panel_dvo_timing;
	const struct lvds_fp_timing *fp_timing;
	struct drm_display_mode *panel_fixed_mode;
	int panel_type = i915->vbt.panel_type;

	lvds_lfp_data = find_section(bdb, BDB_LVDS_LFP_DATA);
	if (!lvds_lfp_data)
		return;

	lvds_lfp_data_ptrs = find_section(bdb, BDB_LVDS_LFP_DATA_PTRS);
	if (!lvds_lfp_data_ptrs)
		return;

	panel_dvo_timing = get_lvds_dvo_timing(lvds_lfp_data,
					       lvds_lfp_data_ptrs,
					       panel_type);

	panel_fixed_mode = kzalloc(sizeof(*panel_fixed_mode), GFP_KERNEL);
	if (!panel_fixed_mode)
		return;

	fill_detail_timing_data(panel_fixed_mode, panel_dvo_timing);

	i915->vbt.lfp_lvds_vbt_mode = panel_fixed_mode;

	drm_dbg_kms(&i915->drm,
		    "Found panel mode in BIOS VBT legacy lfp table:\n");
	drm_mode_debug_printmodeline(panel_fixed_mode);

	fp_timing = get_lvds_fp_timing(bdb, lvds_lfp_data,
				       lvds_lfp_data_ptrs,
				       panel_type);
	if (fp_timing) {
		/* check the resolution, just to be sure */
		if (fp_timing->x_res == panel_fixed_mode->hdisplay &&
		    fp_timing->y_res == panel_fixed_mode->vdisplay) {
			i915->vbt.bios_lvds_val = fp_timing->lvds_reg_val;
			drm_dbg_kms(&i915->drm,
				    "VBT initial LVDS value %x\n",
				    i915->vbt.bios_lvds_val);
		}
	}
}

static void
parse_generic_dtd(struct drm_i915_private *i915,
		  const struct bdb_header *bdb)
{
	const struct bdb_generic_dtd *generic_dtd;
	const struct generic_dtd_entry *dtd;
	struct drm_display_mode *panel_fixed_mode;
	int num_dtd;

	generic_dtd = find_section(bdb, BDB_GENERIC_DTD);
	if (!generic_dtd)
		return;

	if (generic_dtd->gdtd_size < sizeof(struct generic_dtd_entry)) {
		drm_err(&i915->drm, "GDTD size %u is too small.\n",
			generic_dtd->gdtd_size);
		return;
	} else if (generic_dtd->gdtd_size !=
		   sizeof(struct generic_dtd_entry)) {
		drm_err(&i915->drm, "Unexpected GDTD size %u\n",
			generic_dtd->gdtd_size);
		/* DTD has unknown fields, but keep going */
	}

	num_dtd = (get_blocksize(generic_dtd) -
		   sizeof(struct bdb_generic_dtd)) / generic_dtd->gdtd_size;
	if (i915->vbt.panel_type >= num_dtd) {
		drm_err(&i915->drm,
			"Panel type %d not found in table of %d DTD's\n",
			i915->vbt.panel_type, num_dtd);
		return;
	}

	dtd = &generic_dtd->dtd[i915->vbt.panel_type];

	panel_fixed_mode = kzalloc(sizeof(*panel_fixed_mode), GFP_KERNEL);
	if (!panel_fixed_mode)
		return;

	panel_fixed_mode->hdisplay = dtd->hactive;
	panel_fixed_mode->hsync_start =
		panel_fixed_mode->hdisplay + dtd->hfront_porch;
	panel_fixed_mode->hsync_end =
		panel_fixed_mode->hsync_start + dtd->hsync;
	panel_fixed_mode->htotal =
		panel_fixed_mode->hdisplay + dtd->hblank;

	panel_fixed_mode->vdisplay = dtd->vactive;
	panel_fixed_mode->vsync_start =
		panel_fixed_mode->vdisplay + dtd->vfront_porch;
	panel_fixed_mode->vsync_end =
		panel_fixed_mode->vsync_start + dtd->vsync;
	panel_fixed_mode->vtotal =
		panel_fixed_mode->vdisplay + dtd->vblank;

	panel_fixed_mode->clock = dtd->pixel_clock;
	panel_fixed_mode->width_mm = dtd->width_mm;
	panel_fixed_mode->height_mm = dtd->height_mm;

	panel_fixed_mode->type = DRM_MODE_TYPE_PREFERRED;
	drm_mode_set_name(panel_fixed_mode);

	if (dtd->hsync_positive_polarity)
		panel_fixed_mode->flags |= DRM_MODE_FLAG_PHSYNC;
	else
		panel_fixed_mode->flags |= DRM_MODE_FLAG_NHSYNC;

	if (dtd->vsync_positive_polarity)
		panel_fixed_mode->flags |= DRM_MODE_FLAG_PVSYNC;
	else
		panel_fixed_mode->flags |= DRM_MODE_FLAG_NVSYNC;

	drm_dbg_kms(&i915->drm,
		    "Found panel mode in BIOS VBT generic dtd table:\n");
	drm_mode_debug_printmodeline(panel_fixed_mode);

	i915->vbt.lfp_lvds_vbt_mode = panel_fixed_mode;
}

static void
parse_panel_dtd(struct drm_i915_private *i915,
		const struct bdb_header *bdb)
{
	/*
	 * Older VBTs provided provided DTD information for internal displays
	 * through the "LFP panel DTD" block (42).  As of VBT revision 229,
	 * that block is now deprecated and DTD information should be provided
	 * via a newer "generic DTD" block (58).  Just to be safe, we'll
	 * try the new generic DTD block first on VBT >= 229, but still fall
	 * back to trying the old LFP block if that fails.
	 */
	if (bdb->version >= 229)
		parse_generic_dtd(i915, bdb);
	if (!i915->vbt.lfp_lvds_vbt_mode)
		parse_lfp_panel_dtd(i915, bdb);
}

static void
parse_lfp_backlight(struct drm_i915_private *i915,
		    const struct bdb_header *bdb)
{
	const struct bdb_lfp_backlight_data *backlight_data;
	const struct lfp_backlight_data_entry *entry;
	int panel_type = i915->vbt.panel_type;
	u16 level;

	backlight_data = find_section(bdb, BDB_LVDS_BACKLIGHT);
	if (!backlight_data)
		return;

	if (backlight_data->entry_size != sizeof(backlight_data->data[0])) {
		drm_dbg_kms(&i915->drm,
			    "Unsupported backlight data entry size %u\n",
			    backlight_data->entry_size);
		return;
	}

	entry = &backlight_data->data[panel_type];

	i915->vbt.backlight.present = entry->type == BDB_BACKLIGHT_TYPE_PWM;
	if (!i915->vbt.backlight.present) {
		drm_dbg_kms(&i915->drm,
			    "PWM backlight not present in VBT (type %u)\n",
			    entry->type);
		return;
	}

	i915->vbt.backlight.type = INTEL_BACKLIGHT_DISPLAY_DDI;
	if (bdb->version >= 191 &&
	    get_blocksize(backlight_data) >= sizeof(*backlight_data)) {
		const struct lfp_backlight_control_method *method;

		method = &backlight_data->backlight_control[panel_type];
		i915->vbt.backlight.type = method->type;
		i915->vbt.backlight.controller = method->controller;
	}

	i915->vbt.backlight.pwm_freq_hz = entry->pwm_freq_hz;
	i915->vbt.backlight.active_low_pwm = entry->active_low_pwm;

	if (bdb->version >= 234) {
		u16 min_level;
		bool scale;

		level = backlight_data->brightness_level[panel_type].level;
		min_level = backlight_data->brightness_min_level[panel_type].level;

		if (bdb->version >= 236)
			scale = backlight_data->brightness_precision_bits[panel_type] == 16;
		else
			scale = level > 255;

		if (scale)
			min_level = min_level / 255;

		if (min_level > 255) {
			drm_warn(&i915->drm, "Brightness min level > 255\n");
			level = 255;
		}
		i915->vbt.backlight.min_brightness = min_level;
	} else {
		level = backlight_data->level[panel_type];
		i915->vbt.backlight.min_brightness = entry->min_brightness;
	}

	drm_dbg_kms(&i915->drm,
		    "VBT backlight PWM modulation frequency %u Hz, "
		    "active %s, min brightness %u, level %u, controller %u\n",
		    i915->vbt.backlight.pwm_freq_hz,
		    i915->vbt.backlight.active_low_pwm ? "low" : "high",
		    i915->vbt.backlight.min_brightness,
		    level,
		    i915->vbt.backlight.controller);
}

/* Try to find sdvo panel data */
static void
parse_sdvo_panel_data(struct drm_i915_private *i915,
		      const struct bdb_header *bdb)
{
	const struct bdb_sdvo_panel_dtds *dtds;
	struct drm_display_mode *panel_fixed_mode;
	int index;

	index = i915->params.vbt_sdvo_panel_type;
	if (index == -2) {
		drm_dbg_kms(&i915->drm,
			    "Ignore SDVO panel mode from BIOS VBT tables.\n");
		return;
	}

	if (index == -1) {
		const struct bdb_sdvo_lvds_options *sdvo_lvds_options;

		sdvo_lvds_options = find_section(bdb, BDB_SDVO_LVDS_OPTIONS);
		if (!sdvo_lvds_options)
			return;

		index = sdvo_lvds_options->panel_type;
	}

	dtds = find_section(bdb, BDB_SDVO_PANEL_DTDS);
	if (!dtds)
		return;

	panel_fixed_mode = kzalloc(sizeof(*panel_fixed_mode), GFP_KERNEL);
	if (!panel_fixed_mode)
		return;

	fill_detail_timing_data(panel_fixed_mode, &dtds->dtds[index]);

	i915->vbt.sdvo_lvds_vbt_mode = panel_fixed_mode;

	drm_dbg_kms(&i915->drm,
		    "Found SDVO panel mode in BIOS VBT tables:\n");
	drm_mode_debug_printmodeline(panel_fixed_mode);
}

static int intel_bios_ssc_frequency(struct drm_i915_private *i915,
				    bool alternate)
{
	switch (DISPLAY_VER(i915)) {
	case 2:
		return alternate ? 66667 : 48000;
	case 3:
	case 4:
		return alternate ? 100000 : 96000;
	default:
		return alternate ? 100000 : 120000;
	}
}

static void
parse_general_features(struct drm_i915_private *i915,
		       const struct bdb_header *bdb)
{
	const struct bdb_general_features *general;

	general = find_section(bdb, BDB_GENERAL_FEATURES);
	if (!general)
		return;

	i915->vbt.int_tv_support = general->int_tv_support;
	/* int_crt_support can't be trusted on earlier platforms */
	if (bdb->version >= 155 &&
	    (HAS_DDI(i915) || IS_VALLEYVIEW(i915)))
		i915->vbt.int_crt_support = general->int_crt_support;
	i915->vbt.lvds_use_ssc = general->enable_ssc;
	i915->vbt.lvds_ssc_freq =
		intel_bios_ssc_frequency(i915, general->ssc_freq);
	i915->vbt.display_clock_mode = general->display_clock_mode;
	i915->vbt.fdi_rx_polarity_inverted = general->fdi_rx_polarity_inverted;
	if (bdb->version >= 181) {
		i915->vbt.orientation = general->rotate_180 ?
			DRM_MODE_PANEL_ORIENTATION_BOTTOM_UP :
			DRM_MODE_PANEL_ORIENTATION_NORMAL;
	} else {
		i915->vbt.orientation = DRM_MODE_PANEL_ORIENTATION_UNKNOWN;
	}
	drm_dbg_kms(&i915->drm,
		    "BDB_GENERAL_FEATURES int_tv_support %d int_crt_support %d lvds_use_ssc %d lvds_ssc_freq %d display_clock_mode %d fdi_rx_polarity_inverted %d\n",
		    i915->vbt.int_tv_support,
		    i915->vbt.int_crt_support,
		    i915->vbt.lvds_use_ssc,
		    i915->vbt.lvds_ssc_freq,
		    i915->vbt.display_clock_mode,
		    i915->vbt.fdi_rx_polarity_inverted);
}

static const struct child_device_config *
child_device_ptr(const struct bdb_general_definitions *defs, int i)
{
	return (const void *) &defs->devices[i * defs->child_dev_size];
}

static void
parse_sdvo_device_mapping(struct drm_i915_private *i915)
{
	struct sdvo_device_mapping *mapping;
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;
	int count = 0;

	/*
	 * Only parse SDVO mappings on gens that could have SDVO. This isn't
	 * accurate and doesn't have to be, as long as it's not too strict.
	 */
	if (!IS_DISPLAY_VER(i915, 3, 7)) {
		drm_dbg_kms(&i915->drm, "Skipping SDVO device mapping\n");
		return;
	}

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		if (child->slave_addr != SLAVE_ADDR1 &&
		    child->slave_addr != SLAVE_ADDR2) {
			/*
			 * If the slave address is neither 0x70 nor 0x72,
			 * it is not a SDVO device. Skip it.
			 */
			continue;
		}
		if (child->dvo_port != DEVICE_PORT_DVOB &&
		    child->dvo_port != DEVICE_PORT_DVOC) {
			/* skip the incorrect SDVO port */
			drm_dbg_kms(&i915->drm,
				    "Incorrect SDVO port. Skip it\n");
			continue;
		}
		drm_dbg_kms(&i915->drm,
			    "the SDVO device with slave addr %2x is found on"
			    " %s port\n",
			    child->slave_addr,
			    (child->dvo_port == DEVICE_PORT_DVOB) ?
			    "SDVOB" : "SDVOC");
		mapping = &i915->vbt.sdvo_mappings[child->dvo_port - 1];
		if (!mapping->initialized) {
			mapping->dvo_port = child->dvo_port;
			mapping->slave_addr = child->slave_addr;
			mapping->dvo_wiring = child->dvo_wiring;
			mapping->ddc_pin = child->ddc_pin;
			mapping->i2c_pin = child->i2c_pin;
			mapping->initialized = 1;
			drm_dbg_kms(&i915->drm,
				    "SDVO device: dvo=%x, addr=%x, wiring=%d, ddc_pin=%d, i2c_pin=%d\n",
				    mapping->dvo_port, mapping->slave_addr,
				    mapping->dvo_wiring, mapping->ddc_pin,
				    mapping->i2c_pin);
		} else {
			drm_dbg_kms(&i915->drm,
				    "Maybe one SDVO port is shared by "
				    "two SDVO device.\n");
		}
		if (child->slave2_addr) {
			/* Maybe this is a SDVO device with multiple inputs */
			/* And the mapping info is not added */
			drm_dbg_kms(&i915->drm,
				    "there exists the slave2_addr. Maybe this"
				    " is a SDVO device with multiple inputs.\n");
		}
		count++;
	}

	if (!count) {
		/* No SDVO device info is found */
		drm_dbg_kms(&i915->drm,
			    "No SDVO device info is found in VBT\n");
	}
}

static void
parse_driver_features(struct drm_i915_private *i915,
		      const struct bdb_header *bdb)
{
	const struct bdb_driver_features *driver;

	driver = find_section(bdb, BDB_DRIVER_FEATURES);
	if (!driver)
		return;

	if (DISPLAY_VER(i915) >= 5) {
		/*
		 * Note that we consider BDB_DRIVER_FEATURE_INT_SDVO_LVDS
		 * to mean "eDP". The VBT spec doesn't agree with that
		 * interpretation, but real world VBTs seem to.
		 */
		if (driver->lvds_config != BDB_DRIVER_FEATURE_INT_LVDS)
			i915->vbt.int_lvds_support = 0;
	} else {
		/*
		 * FIXME it's not clear which BDB version has the LVDS config
		 * bits defined. Revision history in the VBT spec says:
		 * "0.92 | Add two definitions for VBT value of LVDS Active
		 *  Config (00b and 11b values defined) | 06/13/2005"
		 * but does not the specify the BDB version.
		 *
		 * So far version 134 (on i945gm) is the oldest VBT observed
		 * in the wild with the bits correctly populated. Version
		 * 108 (on i85x) does not have the bits correctly populated.
		 */
		if (bdb->version >= 134 &&
		    driver->lvds_config != BDB_DRIVER_FEATURE_INT_LVDS &&
		    driver->lvds_config != BDB_DRIVER_FEATURE_INT_SDVO_LVDS)
			i915->vbt.int_lvds_support = 0;
	}

	if (bdb->version < 228) {
		drm_dbg_kms(&i915->drm, "DRRS State Enabled:%d\n",
			    driver->drrs_enabled);
		/*
		 * If DRRS is not supported, drrs_type has to be set to 0.
		 * This is because, VBT is configured in such a way that
		 * static DRRS is 0 and DRRS not supported is represented by
		 * driver->drrs_enabled=false
		 */
		if (!driver->drrs_enabled)
			i915->vbt.drrs_type = DRRS_NOT_SUPPORTED;

		i915->vbt.psr.enable = driver->psr_enabled;
	}
}

static void
parse_power_conservation_features(struct drm_i915_private *i915,
				  const struct bdb_header *bdb)
{
	const struct bdb_lfp_power *power;
	u8 panel_type = i915->vbt.panel_type;

	if (bdb->version < 228)
		return;

	power = find_section(bdb, BDB_LFP_POWER);
	if (!power)
		return;

	i915->vbt.psr.enable = power->psr & BIT(panel_type);

	/*
	 * If DRRS is not supported, drrs_type has to be set to 0.
	 * This is because, VBT is configured in such a way that
	 * static DRRS is 0 and DRRS not supported is represented by
	 * power->drrs & BIT(panel_type)=false
	 */
	if (!(power->drrs & BIT(panel_type)))
		i915->vbt.drrs_type = DRRS_NOT_SUPPORTED;

	if (bdb->version >= 232)
		i915->vbt.edp.hobl = power->hobl & BIT(panel_type);
}

static void
parse_edp(struct drm_i915_private *i915, const struct bdb_header *bdb)
{
	const struct bdb_edp *edp;
	const struct edp_power_seq *edp_pps;
	const struct edp_fast_link_params *edp_link_params;
	int panel_type = i915->vbt.panel_type;

	edp = find_section(bdb, BDB_EDP);
	if (!edp)
		return;

	switch ((edp->color_depth >> (panel_type * 2)) & 3) {
	case EDP_18BPP:
		i915->vbt.edp.bpp = 18;
		break;
	case EDP_24BPP:
		i915->vbt.edp.bpp = 24;
		break;
	case EDP_30BPP:
		i915->vbt.edp.bpp = 30;
		break;
	}

	/* Get the eDP sequencing and link info */
	edp_pps = &edp->power_seqs[panel_type];
	edp_link_params = &edp->fast_link_params[panel_type];

	i915->vbt.edp.pps = *edp_pps;

	switch (edp_link_params->rate) {
	case EDP_RATE_1_62:
		i915->vbt.edp.rate = DP_LINK_BW_1_62;
		break;
	case EDP_RATE_2_7:
		i915->vbt.edp.rate = DP_LINK_BW_2_7;
		break;
	default:
		drm_dbg_kms(&i915->drm,
			    "VBT has unknown eDP link rate value %u\n",
			     edp_link_params->rate);
		break;
	}

	switch (edp_link_params->lanes) {
	case EDP_LANE_1:
		i915->vbt.edp.lanes = 1;
		break;
	case EDP_LANE_2:
		i915->vbt.edp.lanes = 2;
		break;
	case EDP_LANE_4:
		i915->vbt.edp.lanes = 4;
		break;
	default:
		drm_dbg_kms(&i915->drm,
			    "VBT has unknown eDP lane count value %u\n",
			    edp_link_params->lanes);
		break;
	}

	switch (edp_link_params->preemphasis) {
	case EDP_PREEMPHASIS_NONE:
		i915->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_0;
		break;
	case EDP_PREEMPHASIS_3_5dB:
		i915->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_1;
		break;
	case EDP_PREEMPHASIS_6dB:
		i915->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_2;
		break;
	case EDP_PREEMPHASIS_9_5dB:
		i915->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_3;
		break;
	default:
		drm_dbg_kms(&i915->drm,
			    "VBT has unknown eDP pre-emphasis value %u\n",
			    edp_link_params->preemphasis);
		break;
	}

	switch (edp_link_params->vswing) {
	case EDP_VSWING_0_4V:
		i915->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_0;
		break;
	case EDP_VSWING_0_6V:
		i915->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_1;
		break;
	case EDP_VSWING_0_8V:
		i915->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_2;
		break;
	case EDP_VSWING_1_2V:
		i915->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_3;
		break;
	default:
		drm_dbg_kms(&i915->drm,
			    "VBT has unknown eDP voltage swing value %u\n",
			    edp_link_params->vswing);
		break;
	}

	if (bdb->version >= 173) {
		u8 vswing;

		/* Don't read from VBT if module parameter has valid value*/
		if (i915->params.edp_vswing) {
			i915->vbt.edp.low_vswing =
				i915->params.edp_vswing == 1;
		} else {
			vswing = (edp->edp_vswing_preemph >> (panel_type * 4)) & 0xF;
			i915->vbt.edp.low_vswing = vswing == 0;
		}
	}
}

static void
parse_psr(struct drm_i915_private *i915, const struct bdb_header *bdb)
{
	const struct bdb_psr *psr;
	const struct psr_table *psr_table;
	int panel_type = i915->vbt.panel_type;

	psr = find_section(bdb, BDB_PSR);
	if (!psr) {
		drm_dbg_kms(&i915->drm, "No PSR BDB found.\n");
		return;
	}

	psr_table = &psr->psr_table[panel_type];

	i915->vbt.psr.full_link = psr_table->full_link;
	i915->vbt.psr.require_aux_wakeup = psr_table->require_aux_to_wakeup;

	/* Allowed VBT values goes from 0 to 15 */
	i915->vbt.psr.idle_frames = psr_table->idle_frames < 0 ? 0 :
		psr_table->idle_frames > 15 ? 15 : psr_table->idle_frames;

	switch (psr_table->lines_to_wait) {
	case 0:
		i915->vbt.psr.lines_to_wait = PSR_0_LINES_TO_WAIT;
		break;
	case 1:
		i915->vbt.psr.lines_to_wait = PSR_1_LINE_TO_WAIT;
		break;
	case 2:
		i915->vbt.psr.lines_to_wait = PSR_4_LINES_TO_WAIT;
		break;
	case 3:
		i915->vbt.psr.lines_to_wait = PSR_8_LINES_TO_WAIT;
		break;
	default:
		drm_dbg_kms(&i915->drm,
			    "VBT has unknown PSR lines to wait %u\n",
			    psr_table->lines_to_wait);
		break;
	}

	/*
	 * New psr options 0=500us, 1=100us, 2=2500us, 3=0us
	 * Old decimal value is wake up time in multiples of 100 us.
	 */
	if (bdb->version >= 205 &&
	    (DISPLAY_VER(i915) >= 9 && !IS_BROXTON(i915))) {
		switch (psr_table->tp1_wakeup_time) {
		case 0:
			i915->vbt.psr.tp1_wakeup_time_us = 500;
			break;
		case 1:
			i915->vbt.psr.tp1_wakeup_time_us = 100;
			break;
		case 3:
			i915->vbt.psr.tp1_wakeup_time_us = 0;
			break;
		default:
			drm_dbg_kms(&i915->drm,
				    "VBT tp1 wakeup time value %d is outside range[0-3], defaulting to max value 2500us\n",
				    psr_table->tp1_wakeup_time);
			fallthrough;
		case 2:
			i915->vbt.psr.tp1_wakeup_time_us = 2500;
			break;
		}

		switch (psr_table->tp2_tp3_wakeup_time) {
		case 0:
			i915->vbt.psr.tp2_tp3_wakeup_time_us = 500;
			break;
		case 1:
			i915->vbt.psr.tp2_tp3_wakeup_time_us = 100;
			break;
		case 3:
			i915->vbt.psr.tp2_tp3_wakeup_time_us = 0;
			break;
		default:
			drm_dbg_kms(&i915->drm,
				    "VBT tp2_tp3 wakeup time value %d is outside range[0-3], defaulting to max value 2500us\n",
				    psr_table->tp2_tp3_wakeup_time);
			fallthrough;
		case 2:
			i915->vbt.psr.tp2_tp3_wakeup_time_us = 2500;
		break;
		}
	} else {
		i915->vbt.psr.tp1_wakeup_time_us = psr_table->tp1_wakeup_time * 100;
		i915->vbt.psr.tp2_tp3_wakeup_time_us = psr_table->tp2_tp3_wakeup_time * 100;
	}

	if (bdb->version >= 226) {
		u32 wakeup_time = psr->psr2_tp2_tp3_wakeup_time;

		wakeup_time = (wakeup_time >> (2 * panel_type)) & 0x3;
		switch (wakeup_time) {
		case 0:
			wakeup_time = 500;
			break;
		case 1:
			wakeup_time = 100;
			break;
		case 3:
			wakeup_time = 50;
			break;
		default:
		case 2:
			wakeup_time = 2500;
			break;
		}
		i915->vbt.psr.psr2_tp2_tp3_wakeup_time_us = wakeup_time;
	} else {
		/* Reusing PSR1 wakeup time for PSR2 in older VBTs */
		i915->vbt.psr.psr2_tp2_tp3_wakeup_time_us = i915->vbt.psr.tp2_tp3_wakeup_time_us;
	}
}

static void parse_dsi_backlight_ports(struct drm_i915_private *i915,
				      u16 version, enum port port)
{
	if (!i915->vbt.dsi.config->dual_link || version < 197) {
		i915->vbt.dsi.bl_ports = BIT(port);
		if (i915->vbt.dsi.config->cabc_supported)
			i915->vbt.dsi.cabc_ports = BIT(port);

		return;
	}

	switch (i915->vbt.dsi.config->dl_dcs_backlight_ports) {
	case DL_DCS_PORT_A:
		i915->vbt.dsi.bl_ports = BIT(PORT_A);
		break;
	case DL_DCS_PORT_C:
		i915->vbt.dsi.bl_ports = BIT(PORT_C);
		break;
	default:
	case DL_DCS_PORT_A_AND_C:
		i915->vbt.dsi.bl_ports = BIT(PORT_A) | BIT(PORT_C);
		break;
	}

	if (!i915->vbt.dsi.config->cabc_supported)
		return;

	switch (i915->vbt.dsi.config->dl_dcs_cabc_ports) {
	case DL_DCS_PORT_A:
		i915->vbt.dsi.cabc_ports = BIT(PORT_A);
		break;
	case DL_DCS_PORT_C:
		i915->vbt.dsi.cabc_ports = BIT(PORT_C);
		break;
	default:
	case DL_DCS_PORT_A_AND_C:
		i915->vbt.dsi.cabc_ports =
					BIT(PORT_A) | BIT(PORT_C);
		break;
	}
}

static void
parse_mipi_config(struct drm_i915_private *i915,
		  const struct bdb_header *bdb)
{
	const struct bdb_mipi_config *start;
	const struct mipi_config *config;
	const struct mipi_pps_data *pps;
	int panel_type = i915->vbt.panel_type;
	enum port port;

	/* parse MIPI blocks only if LFP type is MIPI */
	if (!intel_bios_is_dsi_present(i915, &port))
		return;

	/* Initialize this to undefined indicating no generic MIPI support */
	i915->vbt.dsi.panel_id = MIPI_DSI_UNDEFINED_PANEL_ID;

	/* Block #40 is already parsed and panel_fixed_mode is
	 * stored in i915->lfp_lvds_vbt_mode
	 * resuse this when needed
	 */

	/* Parse #52 for panel index used from panel_type already
	 * parsed
	 */
	start = find_section(bdb, BDB_MIPI_CONFIG);
	if (!start) {
		drm_dbg_kms(&i915->drm, "No MIPI config BDB found");
		return;
	}

	drm_dbg(&i915->drm, "Found MIPI Config block, panel index = %d\n",
		panel_type);

	/*
	 * get hold of the correct configuration block and pps data as per
	 * the panel_type as index
	 */
	config = &start->config[panel_type];
	pps = &start->pps[panel_type];

	/* store as of now full data. Trim when we realise all is not needed */
	i915->vbt.dsi.config = kmemdup(config, sizeof(struct mipi_config), GFP_KERNEL);
	if (!i915->vbt.dsi.config)
		return;

	i915->vbt.dsi.pps = kmemdup(pps, sizeof(struct mipi_pps_data), GFP_KERNEL);
	if (!i915->vbt.dsi.pps) {
		kfree(i915->vbt.dsi.config);
		return;
	}

	parse_dsi_backlight_ports(i915, bdb->version, port);

	/* FIXME is the 90 vs. 270 correct? */
	switch (config->rotation) {
	case ENABLE_ROTATION_0:
		/*
		 * Most (all?) VBTs claim 0 degrees despite having
		 * an upside down panel, thus we do not trust this.
		 */
		i915->vbt.dsi.orientation =
			DRM_MODE_PANEL_ORIENTATION_UNKNOWN;
		break;
	case ENABLE_ROTATION_90:
		i915->vbt.dsi.orientation =
			DRM_MODE_PANEL_ORIENTATION_RIGHT_UP;
		break;
	case ENABLE_ROTATION_180:
		i915->vbt.dsi.orientation =
			DRM_MODE_PANEL_ORIENTATION_BOTTOM_UP;
		break;
	case ENABLE_ROTATION_270:
		i915->vbt.dsi.orientation =
			DRM_MODE_PANEL_ORIENTATION_LEFT_UP;
		break;
	}

	/* We have mandatory mipi config blocks. Initialize as generic panel */
	i915->vbt.dsi.panel_id = MIPI_DSI_GENERIC_PANEL_ID;
}

/* Find the sequence block and size for the given panel. */
static const u8 *
find_panel_sequence_block(const struct bdb_mipi_sequence *sequence,
			  u16 panel_id, u32 *seq_size)
{
	u32 total = get_blocksize(sequence);
	const u8 *data = &sequence->data[0];
	u8 current_id;
	u32 current_size;
	int header_size = sequence->version >= 3 ? 5 : 3;
	int index = 0;
	int i;

	/* skip new block size */
	if (sequence->version >= 3)
		data += 4;

	for (i = 0; i < MAX_MIPI_CONFIGURATIONS && index < total; i++) {
		if (index + header_size > total) {
			DRM_ERROR("Invalid sequence block (header)\n");
			return NULL;
		}

		current_id = *(data + index);
		if (sequence->version >= 3)
			current_size = *((const u32 *)(data + index + 1));
		else
			current_size = *((const u16 *)(data + index + 1));

		index += header_size;

		if (index + current_size > total) {
			DRM_ERROR("Invalid sequence block\n");
			return NULL;
		}

		if (current_id == panel_id) {
			*seq_size = current_size;
			return data + index;
		}

		index += current_size;
	}

	DRM_ERROR("Sequence block detected but no valid configuration\n");

	return NULL;
}

static int goto_next_sequence(const u8 *data, int index, int total)
{
	u16 len;

	/* Skip Sequence Byte. */
	for (index = index + 1; index < total; index += len) {
		u8 operation_byte = *(data + index);
		index++;

		switch (operation_byte) {
		case MIPI_SEQ_ELEM_END:
			return index;
		case MIPI_SEQ_ELEM_SEND_PKT:
			if (index + 4 > total)
				return 0;

			len = *((const u16 *)(data + index + 2)) + 4;
			break;
		case MIPI_SEQ_ELEM_DELAY:
			len = 4;
			break;
		case MIPI_SEQ_ELEM_GPIO:
			len = 2;
			break;
		case MIPI_SEQ_ELEM_I2C:
			if (index + 7 > total)
				return 0;
			len = *(data + index + 6) + 7;
			break;
		default:
			DRM_ERROR("Unknown operation byte\n");
			return 0;
		}
	}

	return 0;
}

static int goto_next_sequence_v3(const u8 *data, int index, int total)
{
	int seq_end;
	u16 len;
	u32 size_of_sequence;

	/*
	 * Could skip sequence based on Size of Sequence alone, but also do some
	 * checking on the structure.
	 */
	if (total < 5) {
		DRM_ERROR("Too small sequence size\n");
		return 0;
	}

	/* Skip Sequence Byte. */
	index++;

	/*
	 * Size of Sequence. Excludes the Sequence Byte and the size itself,
	 * includes MIPI_SEQ_ELEM_END byte, excludes the final MIPI_SEQ_END
	 * byte.
	 */
	size_of_sequence = *((const u32 *)(data + index));
	index += 4;

	seq_end = index + size_of_sequence;
	if (seq_end > total) {
		DRM_ERROR("Invalid sequence size\n");
		return 0;
	}

	for (; index < total; index += len) {
		u8 operation_byte = *(data + index);
		index++;

		if (operation_byte == MIPI_SEQ_ELEM_END) {
			if (index != seq_end) {
				DRM_ERROR("Invalid element structure\n");
				return 0;
			}
			return index;
		}

		len = *(data + index);
		index++;

		/*
		 * FIXME: Would be nice to check elements like for v1/v2 in
		 * goto_next_sequence() above.
		 */
		switch (operation_byte) {
		case MIPI_SEQ_ELEM_SEND_PKT:
		case MIPI_SEQ_ELEM_DELAY:
		case MIPI_SEQ_ELEM_GPIO:
		case MIPI_SEQ_ELEM_I2C:
		case MIPI_SEQ_ELEM_SPI:
		case MIPI_SEQ_ELEM_PMIC:
			break;
		default:
			DRM_ERROR("Unknown operation byte %u\n",
				  operation_byte);
			break;
		}
	}

	return 0;
}

/*
 * Get len of pre-fixed deassert fragment from a v1 init OTP sequence,
 * skip all delay + gpio operands and stop at the first DSI packet op.
 */
static int get_init_otp_deassert_fragment_len(struct drm_i915_private *i915)
{
	const u8 *data = i915->vbt.dsi.sequence[MIPI_SEQ_INIT_OTP];
	int index, len;

	if (drm_WARN_ON(&i915->drm,
			!data || i915->vbt.dsi.seq_version != 1))
		return 0;

	/* index = 1 to skip sequence byte */
	for (index = 1; data[index] != MIPI_SEQ_ELEM_END; index += len) {
		switch (data[index]) {
		case MIPI_SEQ_ELEM_SEND_PKT:
			return index == 1 ? 0 : index;
		case MIPI_SEQ_ELEM_DELAY:
			len = 5; /* 1 byte for operand + uint32 */
			break;
		case MIPI_SEQ_ELEM_GPIO:
			len = 3; /* 1 byte for op, 1 for gpio_nr, 1 for value */
			break;
		default:
			return 0;
		}
	}

	return 0;
}

/*
 * Some v1 VBT MIPI sequences do the deassert in the init OTP sequence.
 * The deassert must be done before calling intel_dsi_device_ready, so for
 * these devices we split the init OTP sequence into a deassert sequence and
 * the actual init OTP part.
 */
static void fixup_mipi_sequences(struct drm_i915_private *i915)
{
	u8 *init_otp;
	int len;

	/* Limit this to VLV for now. */
	if (!IS_VALLEYVIEW(i915))
		return;

	/* Limit this to v1 vid-mode sequences */
	if (i915->vbt.dsi.config->is_cmd_mode ||
	    i915->vbt.dsi.seq_version != 1)
		return;

	/* Only do this if there are otp and assert seqs and no deassert seq */
	if (!i915->vbt.dsi.sequence[MIPI_SEQ_INIT_OTP] ||
	    !i915->vbt.dsi.sequence[MIPI_SEQ_ASSERT_RESET] ||
	    i915->vbt.dsi.sequence[MIPI_SEQ_DEASSERT_RESET])
		return;

	/* The deassert-sequence ends at the first DSI packet */
	len = get_init_otp_deassert_fragment_len(i915);
	if (!len)
		return;

	drm_dbg_kms(&i915->drm,
		    "Using init OTP fragment to deassert reset\n");

	/* Copy the fragment, update seq byte and terminate it */
	init_otp = (u8 *)i915->vbt.dsi.sequence[MIPI_SEQ_INIT_OTP];
	i915->vbt.dsi.deassert_seq = kmemdup(init_otp, len + 1, GFP_KERNEL);
	if (!i915->vbt.dsi.deassert_seq)
		return;
	i915->vbt.dsi.deassert_seq[0] = MIPI_SEQ_DEASSERT_RESET;
	i915->vbt.dsi.deassert_seq[len] = MIPI_SEQ_ELEM_END;
	/* Use the copy for deassert */
	i915->vbt.dsi.sequence[MIPI_SEQ_DEASSERT_RESET] =
		i915->vbt.dsi.deassert_seq;
	/* Replace the last byte of the fragment with init OTP seq byte */
	init_otp[len - 1] = MIPI_SEQ_INIT_OTP;
	/* And make MIPI_MIPI_SEQ_INIT_OTP point to it */
	i915->vbt.dsi.sequence[MIPI_SEQ_INIT_OTP] = init_otp + len - 1;
}

static void
parse_mipi_sequence(struct drm_i915_private *i915,
		    const struct bdb_header *bdb)
{
	int panel_type = i915->vbt.panel_type;
	const struct bdb_mipi_sequence *sequence;
	const u8 *seq_data;
	u32 seq_size;
	u8 *data;
	int index = 0;

	/* Only our generic panel driver uses the sequence block. */
	if (i915->vbt.dsi.panel_id != MIPI_DSI_GENERIC_PANEL_ID)
		return;

	sequence = find_section(bdb, BDB_MIPI_SEQUENCE);
	if (!sequence) {
		drm_dbg_kms(&i915->drm,
			    "No MIPI Sequence found, parsing complete\n");
		return;
	}

	/* Fail gracefully for forward incompatible sequence block. */
	if (sequence->version >= 4) {
		drm_err(&i915->drm,
			"Unable to parse MIPI Sequence Block v%u\n",
			sequence->version);
		return;
	}

	drm_dbg(&i915->drm, "Found MIPI sequence block v%u\n",
		sequence->version);

	seq_data = find_panel_sequence_block(sequence, panel_type, &seq_size);
	if (!seq_data)
		return;

	data = kmemdup(seq_data, seq_size, GFP_KERNEL);
	if (!data)
		return;

	/* Parse the sequences, store pointers to each sequence. */
	for (;;) {
		u8 seq_id = *(data + index);
		if (seq_id == MIPI_SEQ_END)
			break;

		if (seq_id >= MIPI_SEQ_MAX) {
			drm_err(&i915->drm, "Unknown sequence %u\n",
				seq_id);
			goto err;
		}

		/* Log about presence of sequences we won't run. */
		if (seq_id == MIPI_SEQ_TEAR_ON || seq_id == MIPI_SEQ_TEAR_OFF)
			drm_dbg_kms(&i915->drm,
				    "Unsupported sequence %u\n", seq_id);

		i915->vbt.dsi.sequence[seq_id] = data + index;

		if (sequence->version >= 3)
			index = goto_next_sequence_v3(data, index, seq_size);
		else
			index = goto_next_sequence(data, index, seq_size);
		if (!index) {
			drm_err(&i915->drm, "Invalid sequence %u\n",
				seq_id);
			goto err;
		}
	}

	i915->vbt.dsi.data = data;
	i915->vbt.dsi.size = seq_size;
	i915->vbt.dsi.seq_version = sequence->version;

	fixup_mipi_sequences(i915);

	drm_dbg(&i915->drm, "MIPI related VBT parsing complete\n");
	return;

err:
	kfree(data);
	memset(i915->vbt.dsi.sequence, 0, sizeof(i915->vbt.dsi.sequence));
}

static void
parse_compression_parameters(struct drm_i915_private *i915,
			     const struct bdb_header *bdb)
{
	const struct bdb_compression_parameters *params;
	struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;
	u16 block_size;
	int index;

	if (bdb->version < 198)
		return;

	params = find_section(bdb, BDB_COMPRESSION_PARAMETERS);
	if (params) {
		/* Sanity checks */
		if (params->entry_size != sizeof(params->data[0])) {
			drm_dbg_kms(&i915->drm,
				    "VBT: unsupported compression param entry size\n");
			return;
		}

		block_size = get_blocksize(params);
		if (block_size < sizeof(*params)) {
			drm_dbg_kms(&i915->drm,
				    "VBT: expected 16 compression param entries\n");
			return;
		}
	}

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		if (!child->compression_enable)
			continue;

		if (!params) {
			drm_dbg_kms(&i915->drm,
				    "VBT: compression params not available\n");
			continue;
		}

		if (child->compression_method_cps) {
			drm_dbg_kms(&i915->drm,
				    "VBT: CPS compression not supported\n");
			continue;
		}

		index = child->compression_structure_index;

		devdata->dsc = kmemdup(&params->data[index],
				       sizeof(*devdata->dsc), GFP_KERNEL);
	}
}

static u8 translate_iboost(u8 val)
{
	static const u8 mapping[] = { 1, 3, 7 }; /* See VBT spec */

	if (val >= ARRAY_SIZE(mapping)) {
		DRM_DEBUG_KMS("Unsupported I_boost value found in VBT (%d), display may not work properly\n", val);
		return 0;
	}
	return mapping[val];
}

static enum port get_port_by_ddc_pin(struct drm_i915_private *i915, u8 ddc_pin)
{
	const struct ddi_vbt_port_info *info;
	enum port port;

	if (!ddc_pin)
		return PORT_NONE;

	for_each_port(port) {
		info = &i915->vbt.ddi_port_info[port];

		if (info->devdata && ddc_pin == info->alternate_ddc_pin)
			return port;
	}

	return PORT_NONE;
}

static void sanitize_ddc_pin(struct drm_i915_private *i915,
			     enum port port)
{
	struct ddi_vbt_port_info *info = &i915->vbt.ddi_port_info[port];
	struct child_device_config *child;
	enum port p;

	p = get_port_by_ddc_pin(i915, info->alternate_ddc_pin);
	if (p == PORT_NONE)
		return;

	drm_dbg_kms(&i915->drm,
		    "port %c trying to use the same DDC pin (0x%x) as port %c, "
		    "disabling port %c DVI/HDMI support\n",
		    port_name(port), info->alternate_ddc_pin,
		    port_name(p), port_name(p));

	/*
	 * If we have multiple ports supposedly sharing the pin, then dvi/hdmi
	 * couldn't exist on the shared port. Otherwise they share the same ddc
	 * pin and system couldn't communicate with them separately.
	 *
	 * Give inverse child device order the priority, last one wins. Yes,
	 * there are real machines (eg. Asrock B250M-HDV) where VBT has both
	 * port A and port E with the same AUX ch and we must pick port E :(
	 */
	info = &i915->vbt.ddi_port_info[p];
	child = &info->devdata->child;

	child->device_type &= ~DEVICE_TYPE_TMDS_DVI_SIGNALING;
	child->device_type |= DEVICE_TYPE_NOT_HDMI_OUTPUT;

	info->alternate_ddc_pin = 0;
}

static enum port get_port_by_aux_ch(struct drm_i915_private *i915, u8 aux_ch)
{
	const struct ddi_vbt_port_info *info;
	enum port port;

	if (!aux_ch)
		return PORT_NONE;

	for_each_port(port) {
		info = &i915->vbt.ddi_port_info[port];

		if (info->devdata && aux_ch == info->alternate_aux_channel)
			return port;
	}

	return PORT_NONE;
}

static void sanitize_aux_ch(struct drm_i915_private *i915,
			    enum port port)
{
	struct ddi_vbt_port_info *info = &i915->vbt.ddi_port_info[port];
	struct child_device_config *child;
	enum port p;

	p = get_port_by_aux_ch(i915, info->alternate_aux_channel);
	if (p == PORT_NONE)
		return;

	drm_dbg_kms(&i915->drm,
		    "port %c trying to use the same AUX CH (0x%x) as port %c, "
		    "disabling port %c DP support\n",
		    port_name(port), info->alternate_aux_channel,
		    port_name(p), port_name(p));

	/*
	 * If we have multiple ports supposedly sharing the aux channel, then DP
	 * couldn't exist on the shared port. Otherwise they share the same aux
	 * channel and system couldn't communicate with them separately.
	 *
	 * Give inverse child device order the priority, last one wins. Yes,
	 * there are real machines (eg. Asrock B250M-HDV) where VBT has both
	 * port A and port E with the same AUX ch and we must pick port E :(
	 */
	info = &i915->vbt.ddi_port_info[p];
	child = &info->devdata->child;

	child->device_type &= ~DEVICE_TYPE_DISPLAYPORT_OUTPUT;
	info->alternate_aux_channel = 0;
}

static const u8 cnp_ddc_pin_map[] = {
	[0] = 0, /* N/A */
	[DDC_BUS_DDI_B] = GMBUS_PIN_1_BXT,
	[DDC_BUS_DDI_C] = GMBUS_PIN_2_BXT,
	[DDC_BUS_DDI_D] = GMBUS_PIN_4_CNP, /* sic */
	[DDC_BUS_DDI_F] = GMBUS_PIN_3_BXT, /* sic */
};

static const u8 icp_ddc_pin_map[] = {
	[ICL_DDC_BUS_DDI_A] = GMBUS_PIN_1_BXT,
	[ICL_DDC_BUS_DDI_B] = GMBUS_PIN_2_BXT,
	[TGL_DDC_BUS_DDI_C] = GMBUS_PIN_3_BXT,
	[ICL_DDC_BUS_PORT_1] = GMBUS_PIN_9_TC1_ICP,
	[ICL_DDC_BUS_PORT_2] = GMBUS_PIN_10_TC2_ICP,
	[ICL_DDC_BUS_PORT_3] = GMBUS_PIN_11_TC3_ICP,
	[ICL_DDC_BUS_PORT_4] = GMBUS_PIN_12_TC4_ICP,
	[TGL_DDC_BUS_PORT_5] = GMBUS_PIN_13_TC5_TGP,
	[TGL_DDC_BUS_PORT_6] = GMBUS_PIN_14_TC6_TGP,
};

static const u8 rkl_pch_tgp_ddc_pin_map[] = {
	[ICL_DDC_BUS_DDI_A] = GMBUS_PIN_1_BXT,
	[ICL_DDC_BUS_DDI_B] = GMBUS_PIN_2_BXT,
	[RKL_DDC_BUS_DDI_D] = GMBUS_PIN_9_TC1_ICP,
	[RKL_DDC_BUS_DDI_E] = GMBUS_PIN_10_TC2_ICP,
};

static const u8 adls_ddc_pin_map[] = {
	[ICL_DDC_BUS_DDI_A] = GMBUS_PIN_1_BXT,
	[ADLS_DDC_BUS_PORT_TC1] = GMBUS_PIN_9_TC1_ICP,
	[ADLS_DDC_BUS_PORT_TC2] = GMBUS_PIN_10_TC2_ICP,
	[ADLS_DDC_BUS_PORT_TC3] = GMBUS_PIN_11_TC3_ICP,
	[ADLS_DDC_BUS_PORT_TC4] = GMBUS_PIN_12_TC4_ICP,
};

static const u8 gen9bc_tgp_ddc_pin_map[] = {
	[DDC_BUS_DDI_B] = GMBUS_PIN_2_BXT,
	[DDC_BUS_DDI_C] = GMBUS_PIN_9_TC1_ICP,
	[DDC_BUS_DDI_D] = GMBUS_PIN_10_TC2_ICP,
};

static u8 map_ddc_pin(struct drm_i915_private *i915, u8 vbt_pin)
{
	const u8 *ddc_pin_map;
	int n_entries;

	if (IS_ALDERLAKE_S(i915)) {
		ddc_pin_map = adls_ddc_pin_map;
		n_entries = ARRAY_SIZE(adls_ddc_pin_map);
	} else if (INTEL_PCH_TYPE(i915) >= PCH_DG1) {
		return vbt_pin;
	} else if (IS_ROCKETLAKE(i915) && INTEL_PCH_TYPE(i915) == PCH_TGP) {
		ddc_pin_map = rkl_pch_tgp_ddc_pin_map;
		n_entries = ARRAY_SIZE(rkl_pch_tgp_ddc_pin_map);
	} else if (HAS_PCH_TGP(i915) && DISPLAY_VER(i915) == 9) {
		ddc_pin_map = gen9bc_tgp_ddc_pin_map;
		n_entries = ARRAY_SIZE(gen9bc_tgp_ddc_pin_map);
	} else if (INTEL_PCH_TYPE(i915) >= PCH_ICP) {
		ddc_pin_map = icp_ddc_pin_map;
		n_entries = ARRAY_SIZE(icp_ddc_pin_map);
	} else if (HAS_PCH_CNP(i915)) {
		ddc_pin_map = cnp_ddc_pin_map;
		n_entries = ARRAY_SIZE(cnp_ddc_pin_map);
	} else {
		/* Assuming direct map */
		return vbt_pin;
	}

	if (vbt_pin < n_entries && ddc_pin_map[vbt_pin] != 0)
		return ddc_pin_map[vbt_pin];

	drm_dbg_kms(&i915->drm,
		    "Ignoring alternate pin: VBT claims DDC pin %d, which is not valid for this platform\n",
		    vbt_pin);
	return 0;
}

static enum port __dvo_port_to_port(int n_ports, int n_dvo,
				    const int port_mapping[][3], u8 dvo_port)
{
	enum port port;
	int i;

	for (port = PORT_A; port < n_ports; port++) {
		for (i = 0; i < n_dvo; i++) {
			if (port_mapping[port][i] == -1)
				break;

			if (dvo_port == port_mapping[port][i])
				return port;
		}
	}

	return PORT_NONE;
}

static enum port dvo_port_to_port(struct drm_i915_private *i915,
				  u8 dvo_port)
{
	/*
	 * Each DDI port can have more than one value on the "DVO Port" field,
	 * so look for all the possible values for each port.
	 */
	static const int port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_D] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_E] = { DVO_PORT_HDMIE, DVO_PORT_DPE, DVO_PORT_CRT },
		[PORT_F] = { DVO_PORT_HDMIF, DVO_PORT_DPF, -1 },
		[PORT_G] = { DVO_PORT_HDMIG, DVO_PORT_DPG, -1 },
		[PORT_H] = { DVO_PORT_HDMIH, DVO_PORT_DPH, -1 },
		[PORT_I] = { DVO_PORT_HDMII, DVO_PORT_DPI, -1 },
	};
	/*
	 * RKL VBT uses PHY based mapping. Combo PHYs A,B,C,D
	 * map to DDI A,B,TC1,TC2 respectively.
	 */
	static const int rkl_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { -1 },
		[PORT_TC1] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_TC2] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
	};
	/*
	 * Alderlake S ports used in the driver are PORT_A, PORT_D, PORT_E,
	 * PORT_F and PORT_G, we need to map that to correct VBT sections.
	 */
	static const int adls_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { -1 },
		[PORT_C] = { -1 },
		[PORT_TC1] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_TC2] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_TC3] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_TC4] = { DVO_PORT_HDMIE, DVO_PORT_DPE, -1 },
	};
	static const int xelpd_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_D_XELPD] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_E_XELPD] = { DVO_PORT_HDMIE, DVO_PORT_DPE, -1 },
		[PORT_TC1] = { DVO_PORT_HDMIF, DVO_PORT_DPF, -1 },
		[PORT_TC2] = { DVO_PORT_HDMIG, DVO_PORT_DPG, -1 },
		[PORT_TC3] = { DVO_PORT_HDMIH, DVO_PORT_DPH, -1 },
		[PORT_TC4] = { DVO_PORT_HDMII, DVO_PORT_DPI, -1 },
	};

	if (DISPLAY_VER(i915) == 13)
		return __dvo_port_to_port(ARRAY_SIZE(xelpd_port_mapping),
					  ARRAY_SIZE(xelpd_port_mapping[0]),
					  xelpd_port_mapping,
					  dvo_port);
	else if (IS_ALDERLAKE_S(i915))
		return __dvo_port_to_port(ARRAY_SIZE(adls_port_mapping),
					  ARRAY_SIZE(adls_port_mapping[0]),
					  adls_port_mapping,
					  dvo_port);
	else if (IS_DG1(i915) || IS_ROCKETLAKE(i915))
		return __dvo_port_to_port(ARRAY_SIZE(rkl_port_mapping),
					  ARRAY_SIZE(rkl_port_mapping[0]),
					  rkl_port_mapping,
					  dvo_port);
	else
		return __dvo_port_to_port(ARRAY_SIZE(port_mapping),
					  ARRAY_SIZE(port_mapping[0]),
					  port_mapping,
					  dvo_port);
}

static int parse_bdb_230_dp_max_link_rate(const int vbt_max_link_rate)
{
	switch (vbt_max_link_rate) {
	default:
	case BDB_230_VBT_DP_MAX_LINK_RATE_DEF:
		return 0;
	case BDB_230_VBT_DP_MAX_LINK_RATE_UHBR20:
		return 2000000;
	case BDB_230_VBT_DP_MAX_LINK_RATE_UHBR13P5:
		return 1350000;
	case BDB_230_VBT_DP_MAX_LINK_RATE_UHBR10:
		return 1000000;
	case BDB_230_VBT_DP_MAX_LINK_RATE_HBR3:
		return 810000;
	case BDB_230_VBT_DP_MAX_LINK_RATE_HBR2:
		return 540000;
	case BDB_230_VBT_DP_MAX_LINK_RATE_HBR:
		return 270000;
	case BDB_230_VBT_DP_MAX_LINK_RATE_LBR:
		return 162000;
	}
}

static int parse_bdb_216_dp_max_link_rate(const int vbt_max_link_rate)
{
	switch (vbt_max_link_rate) {
	default:
	case BDB_216_VBT_DP_MAX_LINK_RATE_HBR3:
		return 810000;
	case BDB_216_VBT_DP_MAX_LINK_RATE_HBR2:
		return 540000;
	case BDB_216_VBT_DP_MAX_LINK_RATE_HBR:
		return 270000;
	case BDB_216_VBT_DP_MAX_LINK_RATE_LBR:
		return 162000;
	}
}

static void sanitize_device_type(struct intel_bios_encoder_data *devdata,
				 enum port port)
{
	struct drm_i915_private *i915 = devdata->i915;
	bool is_hdmi;

	if (port != PORT_A || DISPLAY_VER(i915) >= 12)
		return;

	if (!(devdata->child.device_type & DEVICE_TYPE_TMDS_DVI_SIGNALING))
		return;

	is_hdmi = !(devdata->child.device_type & DEVICE_TYPE_NOT_HDMI_OUTPUT);

	drm_dbg_kms(&i915->drm, "VBT claims port A supports DVI%s, ignoring\n",
		    is_hdmi ? "/HDMI" : "");

	devdata->child.device_type &= ~DEVICE_TYPE_TMDS_DVI_SIGNALING;
	devdata->child.device_type |= DEVICE_TYPE_NOT_HDMI_OUTPUT;
}

static bool
intel_bios_encoder_supports_crt(const struct intel_bios_encoder_data *devdata)
{
	return devdata->child.device_type & DEVICE_TYPE_ANALOG_OUTPUT;
}

bool
intel_bios_encoder_supports_dvi(const struct intel_bios_encoder_data *devdata)
{
	return devdata->child.device_type & DEVICE_TYPE_TMDS_DVI_SIGNALING;
}

bool
intel_bios_encoder_supports_hdmi(const struct intel_bios_encoder_data *devdata)
{
	return intel_bios_encoder_supports_dvi(devdata) &&
		(devdata->child.device_type & DEVICE_TYPE_NOT_HDMI_OUTPUT) == 0;
}

bool
intel_bios_encoder_supports_dp(const struct intel_bios_encoder_data *devdata)
{
	return devdata->child.device_type & DEVICE_TYPE_DISPLAYPORT_OUTPUT;
}

static bool
intel_bios_encoder_supports_edp(const struct intel_bios_encoder_data *devdata)
{
	return intel_bios_encoder_supports_dp(devdata) &&
		devdata->child.device_type & DEVICE_TYPE_INTERNAL_CONNECTOR;
}

static bool is_port_valid(struct drm_i915_private *i915, enum port port)
{
	/*
	 * On some ICL/CNL SKUs port F is not present, but broken VBTs mark
	 * the port as present. Only try to initialize port F for the
	 * SKUs that may actually have it.
	 */
	if (port == PORT_F && (IS_ICELAKE(i915) || IS_CANNONLAKE(i915)))
		return IS_ICL_WITH_PORT_F(i915) || IS_CNL_WITH_PORT_F(i915);

	return true;
}

static void parse_ddi_port(struct drm_i915_private *i915,
			   struct intel_bios_encoder_data *devdata)
{
	const struct child_device_config *child = &devdata->child;
	struct ddi_vbt_port_info *info;
	bool is_dvi, is_hdmi, is_dp, is_edp, is_crt, supports_typec_usb, supports_tbt;
	int dp_boost_level, hdmi_boost_level;
	enum port port;

	port = dvo_port_to_port(i915, child->dvo_port);
	if (port == PORT_NONE)
		return;

	if (!is_port_valid(i915, port)) {
		drm_dbg_kms(&i915->drm,
			    "VBT reports port %c as supported, but that can't be true: skipping\n",
			    port_name(port));
		return;
	}

	info = &i915->vbt.ddi_port_info[port];

	if (info->devdata) {
		drm_dbg_kms(&i915->drm,
			    "More than one child device for port %c in VBT, using the first.\n",
			    port_name(port));
		return;
	}

	sanitize_device_type(devdata, port);

	is_dvi = intel_bios_encoder_supports_dvi(devdata);
	is_dp = intel_bios_encoder_supports_dp(devdata);
	is_crt = intel_bios_encoder_supports_crt(devdata);
	is_hdmi = intel_bios_encoder_supports_hdmi(devdata);
	is_edp = intel_bios_encoder_supports_edp(devdata);

	supports_typec_usb = intel_bios_encoder_supports_typec_usb(devdata);
	supports_tbt = intel_bios_encoder_supports_tbt(devdata);

	drm_dbg_kms(&i915->drm,
		    "Port %c VBT info: CRT:%d DVI:%d HDMI:%d DP:%d eDP:%d LSPCON:%d USB-Type-C:%d TBT:%d DSC:%d\n",
		    port_name(port), is_crt, is_dvi, is_hdmi, is_dp, is_edp,
		    HAS_LSPCON(i915) && child->lspcon,
		    supports_typec_usb, supports_tbt,
		    devdata->dsc != NULL);

	if (is_dvi) {
		u8 ddc_pin;

		ddc_pin = map_ddc_pin(i915, child->ddc_pin);
		if (intel_gmbus_is_valid_pin(i915, ddc_pin)) {
			info->alternate_ddc_pin = ddc_pin;
			sanitize_ddc_pin(i915, port);
		} else {
			drm_dbg_kms(&i915->drm,
				    "Port %c has invalid DDC pin %d, "
				    "sticking to defaults\n",
				    port_name(port), ddc_pin);
		}
	}

	if (is_dp) {
		info->alternate_aux_channel = child->aux_channel;

		sanitize_aux_ch(i915, port);
	}

	if (i915->vbt.version >= 158) {
		/* The VBT HDMI level shift values match the table we have. */
		u8 hdmi_level_shift = child->hdmi_level_shifter_value;
		drm_dbg_kms(&i915->drm,
			    "Port %c VBT HDMI level shift: %d\n",
			    port_name(port),
			    hdmi_level_shift);
		info->hdmi_level_shift = hdmi_level_shift;
		info->hdmi_level_shift_set = true;
	}

	if (i915->vbt.version >= 204) {
		int max_tmds_clock;

		switch (child->hdmi_max_data_rate) {
		default:
			MISSING_CASE(child->hdmi_max_data_rate);
			fallthrough;
		case HDMI_MAX_DATA_RATE_PLATFORM:
			max_tmds_clock = 0;
			break;
		case HDMI_MAX_DATA_RATE_297:
			max_tmds_clock = 297000;
			break;
		case HDMI_MAX_DATA_RATE_165:
			max_tmds_clock = 165000;
			break;
		}

		if (max_tmds_clock)
			drm_dbg_kms(&i915->drm,
				    "Port %c VBT HDMI max TMDS clock: %d kHz\n",
				    port_name(port), max_tmds_clock);
		info->max_tmds_clock = max_tmds_clock;
	}

	/* I_boost config for SKL and above */
	dp_boost_level = intel_bios_encoder_dp_boost_level(devdata);
	if (dp_boost_level)
		drm_dbg_kms(&i915->drm,
			    "Port %c VBT (e)DP boost level: %d\n",
			    port_name(port), dp_boost_level);

	hdmi_boost_level = intel_bios_encoder_hdmi_boost_level(devdata);
	if (hdmi_boost_level)
		drm_dbg_kms(&i915->drm,
			    "Port %c VBT HDMI boost level: %d\n",
			    port_name(port), hdmi_boost_level);

	/* DP max link rate for CNL+ */
	if (i915->vbt.version >= 216) {
		if (i915->vbt.version >= 230)
			info->dp_max_link_rate = parse_bdb_230_dp_max_link_rate(child->dp_max_link_rate);
		else
			info->dp_max_link_rate = parse_bdb_216_dp_max_link_rate(child->dp_max_link_rate);

		drm_dbg_kms(&i915->drm,
			    "Port %c VBT DP max link rate: %d\n",
			    port_name(port), info->dp_max_link_rate);
	}

	info->devdata = devdata;
}

static void parse_ddi_ports(struct drm_i915_private *i915)
{
	struct intel_bios_encoder_data *devdata;

	if (!HAS_DDI(i915) && !IS_CHERRYVIEW(i915))
		return;

	if (i915->vbt.version < 155)
		return;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node)
		parse_ddi_port(i915, devdata);
}

static void
parse_general_definitions(struct drm_i915_private *i915,
			  const struct bdb_header *bdb)
{
	const struct bdb_general_definitions *defs;
	struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;
	int i, child_device_num;
	u8 expected_size;
	u16 block_size;
	int bus_pin;

	defs = find_section(bdb, BDB_GENERAL_DEFINITIONS);
	if (!defs) {
		drm_dbg_kms(&i915->drm,
			    "No general definition block is found, no devices defined.\n");
		return;
	}

	block_size = get_blocksize(defs);
	if (block_size < sizeof(*defs)) {
		drm_dbg_kms(&i915->drm,
			    "General definitions block too small (%u)\n",
			    block_size);
		return;
	}

	bus_pin = defs->crt_ddc_gmbus_pin;
	drm_dbg_kms(&i915->drm, "crt_ddc_bus_pin: %d\n", bus_pin);
	if (intel_gmbus_is_valid_pin(i915, bus_pin))
		i915->vbt.crt_ddc_pin = bus_pin;

	if (bdb->version < 106) {
		expected_size = 22;
	} else if (bdb->version < 111) {
		expected_size = 27;
	} else if (bdb->version < 195) {
		expected_size = LEGACY_CHILD_DEVICE_CONFIG_SIZE;
	} else if (bdb->version == 195) {
		expected_size = 37;
	} else if (bdb->version <= 215) {
		expected_size = 38;
	} else if (bdb->version <= 237) {
		expected_size = 39;
	} else {
		expected_size = sizeof(*child);
		BUILD_BUG_ON(sizeof(*child) < 39);
		drm_dbg(&i915->drm,
			"Expected child device config size for VBT version %u not known; assuming %u\n",
			bdb->version, expected_size);
	}

	/* Flag an error for unexpected size, but continue anyway. */
	if (defs->child_dev_size != expected_size)
		drm_err(&i915->drm,
			"Unexpected child device config size %u (expected %u for VBT version %u)\n",
			defs->child_dev_size, expected_size, bdb->version);

	/* The legacy sized child device config is the minimum we need. */
	if (defs->child_dev_size < LEGACY_CHILD_DEVICE_CONFIG_SIZE) {
		drm_dbg_kms(&i915->drm,
			    "Child device config size %u is too small.\n",
			    defs->child_dev_size);
		return;
	}

	/* get the number of child device */
	child_device_num = (block_size - sizeof(*defs)) / defs->child_dev_size;

	for (i = 0; i < child_device_num; i++) {
		child = child_device_ptr(defs, i);
		if (!child->device_type)
			continue;

		drm_dbg_kms(&i915->drm,
			    "Found VBT child device with type 0x%x\n",
			    child->device_type);

		devdata = kzalloc(sizeof(*devdata), GFP_KERNEL);
		if (!devdata)
			break;

		devdata->i915 = i915;

		/*
		 * Copy as much as we know (sizeof) and is available
		 * (child_dev_size) of the child device config. Accessing the
		 * data must depend on VBT version.
		 */
		memcpy(&devdata->child, child,
		       min_t(size_t, defs->child_dev_size, sizeof(*child)));

		list_add_tail(&devdata->node, &i915->vbt.display_devices);
	}

	if (list_empty(&i915->vbt.display_devices))
		drm_dbg_kms(&i915->drm,
			    "no child dev is parsed from VBT\n");
}

/* Common defaults which may be overridden by VBT. */
static void
init_vbt_defaults(struct drm_i915_private *i915)
{
	i915->vbt.crt_ddc_pin = GMBUS_PIN_VGADDC;

	/* Default to having backlight */
	i915->vbt.backlight.present = true;

	/* LFP panel data */
	i915->vbt.lvds_dither = 1;

	/* SDVO panel data */
	i915->vbt.sdvo_lvds_vbt_mode = NULL;

	/* general features */
	i915->vbt.int_tv_support = 1;
	i915->vbt.int_crt_support = 1;

	/* driver features */
	i915->vbt.int_lvds_support = 1;

	/* Default to using SSC */
	i915->vbt.lvds_use_ssc = 1;
	/*
	 * Core/SandyBridge/IvyBridge use alternative (120MHz) reference
	 * clock for LVDS.
	 */
	i915->vbt.lvds_ssc_freq = intel_bios_ssc_frequency(i915,
							   !HAS_PCH_SPLIT(i915));
	drm_dbg_kms(&i915->drm, "Set default to SSC at %d kHz\n",
		    i915->vbt.lvds_ssc_freq);
}

/* Defaults to initialize only if there is no VBT. */
static void
init_vbt_missing_defaults(struct drm_i915_private *i915)
{
	enum port port;
	int ports = PORT_A | PORT_B | PORT_C | PORT_D | PORT_E | PORT_F;

	if (!HAS_DDI(i915) && !IS_CHERRYVIEW(i915))
		return;

	for_each_port_masked(port, ports) {
		struct intel_bios_encoder_data *devdata;
		struct child_device_config *child;
		enum phy phy = intel_port_to_phy(i915, port);

		/*
		 * VBT has the TypeC mode (native,TBT/USB) and we don't want
		 * to detect it.
		 */
		if (intel_phy_is_tc(i915, phy))
			continue;

		/* Create fake child device config */
		devdata = kzalloc(sizeof(*devdata), GFP_KERNEL);
		if (!devdata)
			break;

		devdata->i915 = i915;
		child = &devdata->child;

		if (port == PORT_F)
			child->dvo_port = DVO_PORT_HDMIF;
		else if (port == PORT_E)
			child->dvo_port = DVO_PORT_HDMIE;
		else
			child->dvo_port = DVO_PORT_HDMIA + port;

		if (port != PORT_A && port != PORT_E)
			child->device_type |= DEVICE_TYPE_TMDS_DVI_SIGNALING;

		if (port != PORT_E)
			child->device_type |= DEVICE_TYPE_DISPLAYPORT_OUTPUT;

		if (port == PORT_A)
			child->device_type |= DEVICE_TYPE_INTERNAL_CONNECTOR;

		list_add_tail(&devdata->node, &i915->vbt.display_devices);

		drm_dbg_kms(&i915->drm,
			    "Generating default VBT child device with type 0x04%x on port %c\n",
			    child->device_type, port_name(port));
	}

	/* Bypass some minimum baseline VBT version checks */
	i915->vbt.version = 155;
}

static const struct bdb_header *get_bdb_header(const struct vbt_header *vbt)
{
	const void *_vbt = vbt;

	return _vbt + vbt->bdb_offset;
}

/**
 * intel_bios_is_valid_vbt - does the given buffer contain a valid VBT
 * @buf:	pointer to a buffer to validate
 * @size:	size of the buffer
 *
 * Returns true on valid VBT.
 */
bool intel_bios_is_valid_vbt(const void *buf, size_t size)
{
	const struct vbt_header *vbt = buf;
	const struct bdb_header *bdb;

	if (!vbt)
		return false;

	if (sizeof(struct vbt_header) > size) {
		DRM_DEBUG_DRIVER("VBT header incomplete\n");
		return false;
	}

	if (memcmp(vbt->signature, "$VBT", 4)) {
		DRM_DEBUG_DRIVER("VBT invalid signature\n");
		return false;
	}

	if (vbt->vbt_size > size) {
		DRM_DEBUG_DRIVER("VBT incomplete (vbt_size overflows)\n");
		return false;
	}

	size = vbt->vbt_size;

	if (range_overflows_t(size_t,
			      vbt->bdb_offset,
			      sizeof(struct bdb_header),
			      size)) {
		DRM_DEBUG_DRIVER("BDB header incomplete\n");
		return false;
	}

	bdb = get_bdb_header(vbt);
	if (range_overflows_t(size_t, vbt->bdb_offset, bdb->bdb_size, size)) {
		DRM_DEBUG_DRIVER("BDB incomplete\n");
		return false;
	}

	return vbt;
}

static struct vbt_header *oprom_get_vbt(struct drm_i915_private *i915)
{
	struct pci_dev *pdev = to_pci_dev(i915->drm.dev);
	void __iomem *p = NULL, *oprom;
	struct vbt_header *vbt;
	u16 vbt_size;
	size_t i, size;

	oprom = pci_map_rom(pdev, &size);
	if (!oprom)
		return NULL;

	/* Scour memory looking for the VBT signature. */
	for (i = 0; i + 4 < size; i += 4) {
		if (ioread32(oprom + i) != *((const u32 *)"$VBT"))
			continue;

		p = oprom + i;
		size -= i;
		break;
	}

	if (!p)
		goto err_unmap_oprom;

	if (sizeof(struct vbt_header) > size) {
		drm_dbg(&i915->drm, "VBT header incomplete\n");
		goto err_unmap_oprom;
	}

	vbt_size = ioread16(p + offsetof(struct vbt_header, vbt_size));
	if (vbt_size > size) {
		drm_dbg(&i915->drm,
			"VBT incomplete (vbt_size overflows)\n");
		goto err_unmap_oprom;
	}

	/* The rest will be validated by intel_bios_is_valid_vbt() */
	vbt = kmalloc(vbt_size, GFP_KERNEL);
	if (!vbt)
		goto err_unmap_oprom;

	memcpy_fromio(vbt, p, vbt_size);

	if (!intel_bios_is_valid_vbt(vbt, vbt_size))
		goto err_free_vbt;

	pci_unmap_rom(pdev, oprom);

	return vbt;

err_free_vbt:
	kfree(vbt);
err_unmap_oprom:
	pci_unmap_rom(pdev, oprom);

	return NULL;
}

/**
 * intel_bios_init - find VBT and initialize settings from the BIOS
 * @i915: i915 device instance
 *
 * Parse and initialize settings from the Video BIOS Tables (VBT). If the VBT
 * was not found in ACPI OpRegion, try to find it in PCI ROM first. Also
 * initialize some defaults if the VBT is not present at all.
 */
void intel_bios_init(struct drm_i915_private *i915)
{
	const struct vbt_header *vbt = i915->opregion.vbt;
	struct vbt_header *oprom_vbt = NULL;
	const struct bdb_header *bdb;

	INIT_LIST_HEAD(&i915->vbt.display_devices);

	if (!HAS_DISPLAY(i915)) {
		drm_dbg_kms(&i915->drm,
			    "Skipping VBT init due to disabled display.\n");
		return;
	}

	init_vbt_defaults(i915);

	/* If the OpRegion does not have VBT, look in PCI ROM. */
	if (!vbt) {
		oprom_vbt = oprom_get_vbt(i915);
		if (!oprom_vbt)
			goto out;

		vbt = oprom_vbt;

		drm_dbg_kms(&i915->drm, "Found valid VBT in PCI ROM\n");
	}

	bdb = get_bdb_header(vbt);
	i915->vbt.version = bdb->version;

	drm_dbg_kms(&i915->drm,
		    "VBT signature \"%.*s\", BDB version %d\n",
		    (int)sizeof(vbt->signature), vbt->signature, bdb->version);

	/* Grab useful general definitions */
	parse_general_features(i915, bdb);
	parse_general_definitions(i915, bdb);
	parse_panel_options(i915, bdb);
	parse_panel_dtd(i915, bdb);
	parse_lfp_backlight(i915, bdb);
	parse_sdvo_panel_data(i915, bdb);
	parse_driver_features(i915, bdb);
	parse_power_conservation_features(i915, bdb);
	parse_edp(i915, bdb);
	parse_psr(i915, bdb);
	parse_mipi_config(i915, bdb);
	parse_mipi_sequence(i915, bdb);

	/* Depends on child device list */
	parse_compression_parameters(i915, bdb);

out:
	if (!vbt) {
		drm_info(&i915->drm,
			 "Failed to find VBIOS tables (VBT)\n");
		init_vbt_missing_defaults(i915);
	}

	/* Further processing on pre-parsed or generated child device data */
	parse_sdvo_device_mapping(i915);
	parse_ddi_ports(i915);

	kfree(oprom_vbt);
}

/**
 * intel_bios_driver_remove - Free any resources allocated by intel_bios_init()
 * @i915: i915 device instance
 */
void intel_bios_driver_remove(struct drm_i915_private *i915)
{
	struct intel_bios_encoder_data *devdata, *n;

	list_for_each_entry_safe(devdata, n, &i915->vbt.display_devices, node) {
		list_del(&devdata->node);
		kfree(devdata->dsc);
		kfree(devdata);
	}

	kfree(i915->vbt.sdvo_lvds_vbt_mode);
	i915->vbt.sdvo_lvds_vbt_mode = NULL;
	kfree(i915->vbt.lfp_lvds_vbt_mode);
	i915->vbt.lfp_lvds_vbt_mode = NULL;
	kfree(i915->vbt.dsi.data);
	i915->vbt.dsi.data = NULL;
	kfree(i915->vbt.dsi.pps);
	i915->vbt.dsi.pps = NULL;
	kfree(i915->vbt.dsi.config);
	i915->vbt.dsi.config = NULL;
	kfree(i915->vbt.dsi.deassert_seq);
	i915->vbt.dsi.deassert_seq = NULL;
}

/**
 * intel_bios_is_tv_present - is integrated TV present in VBT
 * @i915: i915 device instance
 *
 * Return true if TV is present. If no child devices were parsed from VBT,
 * assume TV is present.
 */
bool intel_bios_is_tv_present(struct drm_i915_private *i915)
{
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;

	if (!i915->vbt.int_tv_support)
		return false;

	if (list_empty(&i915->vbt.display_devices))
		return true;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		/*
		 * If the device type is not TV, continue.
		 */
		switch (child->device_type) {
		case DEVICE_TYPE_INT_TV:
		case DEVICE_TYPE_TV:
		case DEVICE_TYPE_TV_SVIDEO_COMPOSITE:
			break;
		default:
			continue;
		}
		/* Only when the addin_offset is non-zero, it is regarded
		 * as present.
		 */
		if (child->addin_offset)
			return true;
	}

	return false;
}

/**
 * intel_bios_is_lvds_present - is LVDS present in VBT
 * @i915:	i915 device instance
 * @i2c_pin:	i2c pin for LVDS if present
 *
 * Return true if LVDS is present. If no child devices were parsed from VBT,
 * assume LVDS is present.
 */
bool intel_bios_is_lvds_present(struct drm_i915_private *i915, u8 *i2c_pin)
{
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;

	if (list_empty(&i915->vbt.display_devices))
		return true;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		/* If the device type is not LFP, continue.
		 * We have to check both the new identifiers as well as the
		 * old for compatibility with some BIOSes.
		 */
		if (child->device_type != DEVICE_TYPE_INT_LFP &&
		    child->device_type != DEVICE_TYPE_LFP)
			continue;

		if (intel_gmbus_is_valid_pin(i915, child->i2c_pin))
			*i2c_pin = child->i2c_pin;

		/* However, we cannot trust the BIOS writers to populate
		 * the VBT correctly.  Since LVDS requires additional
		 * information from AIM blocks, a non-zero addin offset is
		 * a good indicator that the LVDS is actually present.
		 */
		if (child->addin_offset)
			return true;

		/* But even then some BIOS writers perform some black magic
		 * and instantiate the device without reference to any
		 * additional data.  Trust that if the VBT was written into
		 * the OpRegion then they have validated the LVDS's existence.
		 */
		if (i915->opregion.vbt)
			return true;
	}

	return false;
}

/**
 * intel_bios_is_port_present - is the specified digital port present
 * @i915:	i915 device instance
 * @port:	port to check
 *
 * Return true if the device in %port is present.
 */
bool intel_bios_is_port_present(struct drm_i915_private *i915, enum port port)
{
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;
	static const struct {
		u16 dp, hdmi;
	} port_mapping[] = {
		[PORT_B] = { DVO_PORT_DPB, DVO_PORT_HDMIB, },
		[PORT_C] = { DVO_PORT_DPC, DVO_PORT_HDMIC, },
		[PORT_D] = { DVO_PORT_DPD, DVO_PORT_HDMID, },
		[PORT_E] = { DVO_PORT_DPE, DVO_PORT_HDMIE, },
		[PORT_F] = { DVO_PORT_DPF, DVO_PORT_HDMIF, },
	};

	if (HAS_DDI(i915)) {
		const struct ddi_vbt_port_info *port_info =
			&i915->vbt.ddi_port_info[port];

		return port_info->devdata;
	}

	/* FIXME maybe deal with port A as well? */
	if (drm_WARN_ON(&i915->drm,
			port == PORT_A) || port >= ARRAY_SIZE(port_mapping))
		return false;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		if ((child->dvo_port == port_mapping[port].dp ||
		     child->dvo_port == port_mapping[port].hdmi) &&
		    (child->device_type & (DEVICE_TYPE_TMDS_DVI_SIGNALING |
					   DEVICE_TYPE_DISPLAYPORT_OUTPUT)))
			return true;
	}

	return false;
}

/**
 * intel_bios_is_port_edp - is the device in given port eDP
 * @i915:	i915 device instance
 * @port:	port to check
 *
 * Return true if the device in %port is eDP.
 */
bool intel_bios_is_port_edp(struct drm_i915_private *i915, enum port port)
{
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;
	static const short port_mapping[] = {
		[PORT_B] = DVO_PORT_DPB,
		[PORT_C] = DVO_PORT_DPC,
		[PORT_D] = DVO_PORT_DPD,
		[PORT_E] = DVO_PORT_DPE,
		[PORT_F] = DVO_PORT_DPF,
	};

	if (HAS_DDI(i915)) {
		const struct intel_bios_encoder_data *devdata;

		devdata = intel_bios_encoder_data_lookup(i915, port);

		return devdata && intel_bios_encoder_supports_edp(devdata);
	}

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		if (child->dvo_port == port_mapping[port] &&
		    (child->device_type & DEVICE_TYPE_eDP_BITS) ==
		    (DEVICE_TYPE_eDP & DEVICE_TYPE_eDP_BITS))
			return true;
	}

	return false;
}

static bool child_dev_is_dp_dual_mode(const struct child_device_config *child,
				      enum port port)
{
	static const struct {
		u16 dp, hdmi;
	} port_mapping[] = {
		/*
		 * Buggy VBTs may declare DP ports as having
		 * HDMI type dvo_port :( So let's check both.
		 */
		[PORT_B] = { DVO_PORT_DPB, DVO_PORT_HDMIB, },
		[PORT_C] = { DVO_PORT_DPC, DVO_PORT_HDMIC, },
		[PORT_D] = { DVO_PORT_DPD, DVO_PORT_HDMID, },
		[PORT_E] = { DVO_PORT_DPE, DVO_PORT_HDMIE, },
		[PORT_F] = { DVO_PORT_DPF, DVO_PORT_HDMIF, },
	};

	if (port == PORT_A || port >= ARRAY_SIZE(port_mapping))
		return false;

	if ((child->device_type & DEVICE_TYPE_DP_DUAL_MODE_BITS) !=
	    (DEVICE_TYPE_DP_DUAL_MODE & DEVICE_TYPE_DP_DUAL_MODE_BITS))
		return false;

	if (child->dvo_port == port_mapping[port].dp)
		return true;

	/* Only accept a HDMI dvo_port as DP++ if it has an AUX channel */
	if (child->dvo_port == port_mapping[port].hdmi &&
	    child->aux_channel != 0)
		return true;

	return false;
}

bool intel_bios_is_port_dp_dual_mode(struct drm_i915_private *i915,
				     enum port port)
{
	const struct intel_bios_encoder_data *devdata;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		if (child_dev_is_dp_dual_mode(&devdata->child, port))
			return true;
	}

	return false;
}

/**
 * intel_bios_is_dsi_present - is DSI present in VBT
 * @i915:	i915 device instance
 * @port:	port for DSI if present
 *
 * Return true if DSI is present, and return the port in %port.
 */
bool intel_bios_is_dsi_present(struct drm_i915_private *i915,
			       enum port *port)
{
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;
	u8 dvo_port;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		if (!(child->device_type & DEVICE_TYPE_MIPI_OUTPUT))
			continue;

		dvo_port = child->dvo_port;

		if (dvo_port == DVO_PORT_MIPIA ||
		    (dvo_port == DVO_PORT_MIPIB && DISPLAY_VER(i915) >= 11) ||
		    (dvo_port == DVO_PORT_MIPIC && DISPLAY_VER(i915) < 11)) {
			if (port)
				*port = dvo_port - DVO_PORT_MIPIA;
			return true;
		} else if (dvo_port == DVO_PORT_MIPIB ||
			   dvo_port == DVO_PORT_MIPIC ||
			   dvo_port == DVO_PORT_MIPID) {
			drm_dbg_kms(&i915->drm,
				    "VBT has unsupported DSI port %c\n",
				    port_name(dvo_port - DVO_PORT_MIPIA));
		}
	}

	return false;
}

static void fill_dsc(struct intel_crtc_state *crtc_state,
		     struct dsc_compression_parameters_entry *dsc,
		     int dsc_max_bpc)
{
	struct drm_dsc_config *vdsc_cfg = &crtc_state->dsc.config;
	int bpc = 8;

	vdsc_cfg->dsc_version_major = dsc->version_major;
	vdsc_cfg->dsc_version_minor = dsc->version_minor;

	if (dsc->support_12bpc && dsc_max_bpc >= 12)
		bpc = 12;
	else if (dsc->support_10bpc && dsc_max_bpc >= 10)
		bpc = 10;
	else if (dsc->support_8bpc && dsc_max_bpc >= 8)
		bpc = 8;
	else
		DRM_DEBUG_KMS("VBT: Unsupported BPC %d for DCS\n",
			      dsc_max_bpc);

	crtc_state->pipe_bpp = bpc * 3;

	crtc_state->dsc.compressed_bpp = min(crtc_state->pipe_bpp,
					     VBT_DSC_MAX_BPP(dsc->max_bpp));

	/*
	 * FIXME: This is ugly, and slice count should take DSC engine
	 * throughput etc. into account.
	 *
	 * Also, per spec DSI supports 1, 2, 3 or 4 horizontal slices.
	 */
	if (dsc->slices_per_line & BIT(2)) {
		crtc_state->dsc.slice_count = 4;
	} else if (dsc->slices_per_line & BIT(1)) {
		crtc_state->dsc.slice_count = 2;
	} else {
		/* FIXME */
		if (!(dsc->slices_per_line & BIT(0)))
			DRM_DEBUG_KMS("VBT: Unsupported DSC slice count for DSI\n");

		crtc_state->dsc.slice_count = 1;
	}

	if (crtc_state->hw.adjusted_mode.crtc_hdisplay %
	    crtc_state->dsc.slice_count != 0)
		DRM_DEBUG_KMS("VBT: DSC hdisplay %d not divisible by slice count %d\n",
			      crtc_state->hw.adjusted_mode.crtc_hdisplay,
			      crtc_state->dsc.slice_count);

	/*
	 * The VBT rc_buffer_block_size and rc_buffer_size definitions
	 * correspond to DP 1.4 DPCD offsets 0x62 and 0x63.
	 */
	vdsc_cfg->rc_model_size = drm_dsc_dp_rc_buffer_size(dsc->rc_buffer_block_size,
							    dsc->rc_buffer_size);

	/* FIXME: DSI spec says bpc + 1 for this one */
	vdsc_cfg->line_buf_depth = VBT_DSC_LINE_BUFFER_DEPTH(dsc->line_buffer_depth);

	vdsc_cfg->block_pred_enable = dsc->block_prediction_enable;

	vdsc_cfg->slice_height = dsc->slice_height;
}

/* FIXME: initially DSI specific */
bool intel_bios_get_dsc_params(struct intel_encoder *encoder,
			       struct intel_crtc_state *crtc_state,
			       int dsc_max_bpc)
{
	struct drm_i915_private *i915 = to_i915(encoder->base.dev);
	const struct intel_bios_encoder_data *devdata;
	const struct child_device_config *child;

	list_for_each_entry(devdata, &i915->vbt.display_devices, node) {
		child = &devdata->child;

		if (!(child->device_type & DEVICE_TYPE_MIPI_OUTPUT))
			continue;

		if (child->dvo_port - DVO_PORT_MIPIA == encoder->port) {
			if (!devdata->dsc)
				return false;

			if (crtc_state)
				fill_dsc(crtc_state, devdata->dsc, dsc_max_bpc);

			return true;
		}
	}

	return false;
}

/**
 * intel_bios_is_port_hpd_inverted - is HPD inverted for %port
 * @i915:	i915 device instance
 * @port:	port to check
 *
 * Return true if HPD should be inverted for %port.
 */
bool
intel_bios_is_port_hpd_inverted(const struct drm_i915_private *i915,
				enum port port)
{
	const struct intel_bios_encoder_data *devdata =
		i915->vbt.ddi_port_info[port].devdata;

	if (drm_WARN_ON_ONCE(&i915->drm,
			     !IS_GEMINILAKE(i915) && !IS_BROXTON(i915)))
		return false;

	return devdata && devdata->child.hpd_invert;
}

/**
 * intel_bios_is_lspcon_present - if LSPCON is attached on %port
 * @i915:	i915 device instance
 * @port:	port to check
 *
 * Return true if LSPCON is present on this port
 */
bool
intel_bios_is_lspcon_present(const struct drm_i915_private *i915,
			     enum port port)
{
	const struct intel_bios_encoder_data *devdata =
		i915->vbt.ddi_port_info[port].devdata;

	return HAS_LSPCON(i915) && devdata && devdata->child.lspcon;
}

/**
 * intel_bios_is_lane_reversal_needed - if lane reversal needed on port
 * @i915:       i915 device instance
 * @port:       port to check
 *
 * Return true if port requires lane reversal
 */
bool
intel_bios_is_lane_reversal_needed(const struct drm_i915_private *i915,
				   enum port port)
{
	const struct intel_bios_encoder_data *devdata =
		i915->vbt.ddi_port_info[port].devdata;

	return devdata && devdata->child.lane_reversal;
}

enum aux_ch intel_bios_port_aux_ch(struct drm_i915_private *i915,
				   enum port port)
{
	const struct ddi_vbt_port_info *info =
		&i915->vbt.ddi_port_info[port];
	enum aux_ch aux_ch;

	if (!info->alternate_aux_channel) {
		aux_ch = (enum aux_ch)port;

		drm_dbg_kms(&i915->drm,
			    "using AUX %c for port %c (platform default)\n",
			    aux_ch_name(aux_ch), port_name(port));
		return aux_ch;
	}

	/*
	 * RKL/DG1 VBT uses PHY based mapping. Combo PHYs A,B,C,D
	 * map to DDI A,B,TC1,TC2 respectively.
	 *
	 * ADL-S VBT uses PHY based mapping. Combo PHYs A,B,C,D,E
	 * map to DDI A,TC1,TC2,TC3,TC4 respectively.
	 */
	switch (info->alternate_aux_channel) {
	case DP_AUX_A:
		aux_ch = AUX_CH_A;
		break;
	case DP_AUX_B:
		if (IS_ALDERLAKE_S(i915))
			aux_ch = AUX_CH_USBC1;
		else
			aux_ch = AUX_CH_B;
		break;
	case DP_AUX_C:
		if (IS_ALDERLAKE_S(i915))
			aux_ch = AUX_CH_USBC2;
		else if (IS_DG1(i915) || IS_ROCKETLAKE(i915))
			aux_ch = AUX_CH_USBC1;
		else
			aux_ch = AUX_CH_C;
		break;
	case DP_AUX_D:
		if (DISPLAY_VER(i915) == 13)
			aux_ch = AUX_CH_D_XELPD;
		else if (IS_ALDERLAKE_S(i915))
			aux_ch = AUX_CH_USBC3;
		else if (IS_DG1(i915) || IS_ROCKETLAKE(i915))
			aux_ch = AUX_CH_USBC2;
		else
			aux_ch = AUX_CH_D;
		break;
	case DP_AUX_E:
		if (DISPLAY_VER(i915) == 13)
			aux_ch = AUX_CH_E_XELPD;
		else if (IS_ALDERLAKE_S(i915))
			aux_ch = AUX_CH_USBC4;
		else
			aux_ch = AUX_CH_E;
		break;
	case DP_AUX_F:
		if (DISPLAY_VER(i915) == 13)
			aux_ch = AUX_CH_USBC1;
		else
			aux_ch = AUX_CH_F;
		break;
	case DP_AUX_G:
		if (DISPLAY_VER(i915) == 13)
			aux_ch = AUX_CH_USBC2;
		else
			aux_ch = AUX_CH_G;
		break;
	case DP_AUX_H:
		if (DISPLAY_VER(i915) == 13)
			aux_ch = AUX_CH_USBC3;
		else
			aux_ch = AUX_CH_H;
		break;
	case DP_AUX_I:
		if (DISPLAY_VER(i915) == 13)
			aux_ch = AUX_CH_USBC4;
		else
			aux_ch = AUX_CH_I;
		break;
	default:
		MISSING_CASE(info->alternate_aux_channel);
		aux_ch = AUX_CH_A;
		break;
	}

	drm_dbg_kms(&i915->drm, "using AUX %c for port %c (VBT)\n",
		    aux_ch_name(aux_ch), port_name(port));

	return aux_ch;
}

int intel_bios_max_tmds_clock(struct intel_encoder *encoder)
{
	struct drm_i915_private *i915 = to_i915(encoder->base.dev);

	return i915->vbt.ddi_port_info[encoder->port].max_tmds_clock;
}

int intel_bios_hdmi_level_shift(struct intel_encoder *encoder)
{
	struct drm_i915_private *i915 = to_i915(encoder->base.dev);
	const struct ddi_vbt_port_info *info =
		&i915->vbt.ddi_port_info[encoder->port];

	return info->hdmi_level_shift_set ? info->hdmi_level_shift : -1;
}

int intel_bios_encoder_dp_boost_level(const struct intel_bios_encoder_data *devdata)
{
	if (!devdata || devdata->i915->vbt.version < 196 || !devdata->child.iboost)
		return 0;

	return translate_iboost(devdata->child.dp_iboost_level);
}

int intel_bios_encoder_hdmi_boost_level(const struct intel_bios_encoder_data *devdata)
{
	if (!devdata || devdata->i915->vbt.version < 196 || !devdata->child.iboost)
		return 0;

	return translate_iboost(devdata->child.hdmi_iboost_level);
}

int intel_bios_dp_max_link_rate(struct intel_encoder *encoder)
{
	struct drm_i915_private *i915 = to_i915(encoder->base.dev);

	return i915->vbt.ddi_port_info[encoder->port].dp_max_link_rate;
}

int intel_bios_alternate_ddc_pin(struct intel_encoder *encoder)
{
	struct drm_i915_private *i915 = to_i915(encoder->base.dev);

	return i915->vbt.ddi_port_info[encoder->port].alternate_ddc_pin;
}

bool intel_bios_encoder_supports_typec_usb(const struct intel_bios_encoder_data *devdata)
{
	return devdata->i915->vbt.version >= 195 && devdata->child.dp_usb_type_c;
}

bool intel_bios_encoder_supports_tbt(const struct intel_bios_encoder_data *devdata)
{
	return devdata->i915->vbt.version >= 209 && devdata->child.tbt;
}

const struct intel_bios_encoder_data *
intel_bios_encoder_data_lookup(struct drm_i915_private *i915, enum port port)
{
	return i915->vbt.ddi_port_info[port].devdata;
}

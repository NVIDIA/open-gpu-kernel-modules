/*
 * Copyright (c) 2006 Dave Airlie <airlied@linux.ie>
 * Copyright (c) 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __INTEL_DISPLAY_TYPES_H__
#define __INTEL_DISPLAY_TYPES_H__

#include <linux/async.h>
#include <linux/i2c.h>
#include <linux/pwm.h>
#include <linux/sched/clock.h>

#include <drm/drm_atomic.h>
#include <drm/drm_crtc.h>
#include <drm/drm_dp_dual_mode_helper.h>
#include <drm/drm_dp_mst_helper.h>
#include <drm/drm_encoder.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_rect.h>
#include <drm/drm_vblank.h>
#include <drm/i915_mei_hdcp_interface.h>
#include <media/cec-notifier.h>

#include "i915_drv.h"

struct drm_printer;
struct __intel_global_objs_state;

/*
 * Display related stuff
 */

/* these are outputs from the chip - integrated only
   external chips are via DVO or SDVO output */
enum intel_output_type {
	INTEL_OUTPUT_UNUSED = 0,
	INTEL_OUTPUT_ANALOG = 1,
	INTEL_OUTPUT_DVO = 2,
	INTEL_OUTPUT_SDVO = 3,
	INTEL_OUTPUT_LVDS = 4,
	INTEL_OUTPUT_TVOUT = 5,
	INTEL_OUTPUT_HDMI = 6,
	INTEL_OUTPUT_DP = 7,
	INTEL_OUTPUT_EDP = 8,
	INTEL_OUTPUT_DSI = 9,
	INTEL_OUTPUT_DDI = 10,
	INTEL_OUTPUT_DP_MST = 11,
};

enum hdmi_force_audio {
	HDMI_AUDIO_OFF_DVI = -2,	/* no aux data for HDMI-DVI converter */
	HDMI_AUDIO_OFF,			/* force turn off HDMI audio */
	HDMI_AUDIO_AUTO,		/* trust EDID */
	HDMI_AUDIO_ON,			/* force turn on HDMI audio */
};

/* "Broadcast RGB" property */
enum intel_broadcast_rgb {
	INTEL_BROADCAST_RGB_AUTO,
	INTEL_BROADCAST_RGB_FULL,
	INTEL_BROADCAST_RGB_LIMITED,
};

struct intel_fb_view {
	/*
	 * The remap information used in the remapped and rotated views to
	 * create the DMA scatter-gather list for each FB color plane. This sg
	 * list is created along with the view type (gtt.type) specific
	 * i915_vma object and contains the list of FB object pages (reordered
	 * in the rotated view) that are visible in the view.
	 * In the normal view the FB object's backing store sg list is used
	 * directly and hence the remap information here is not used.
	 */
	struct i915_ggtt_view gtt;

	/*
	 * The GTT view (gtt.type) specific information for each FB color
	 * plane. In the normal GTT view all formats (up to 4 color planes),
	 * in the rotated and remapped GTT view all no-CCS formats (up to 2
	 * color planes) are supported.
	 *
	 * TODO: add support for CCS formats in the remapped GTT view.
	 *
	 * The view information shared by all FB color planes in the FB,
	 * like dst x/y and src/dst width, is stored separately in
	 * intel_plane_state.
	 */
	struct i915_color_plane_view {
		u32 offset;
		unsigned int x, y;
		/*
		 * Plane stride in:
		 *   bytes for 0/180 degree rotation
		 *   pixels for 90/270 degree rotation
		 */
		unsigned int stride;
	} color_plane[4];
};

struct intel_framebuffer {
	struct drm_framebuffer base;
	struct intel_frontbuffer *frontbuffer;

	/* Params to remap the FB pages and program the plane registers in each view. */
	struct intel_fb_view normal_view;
	union {
		struct intel_fb_view rotated_view;
		struct intel_fb_view remapped_view;
	};

	struct i915_address_space *dpt_vm;
};

struct intel_fbdev {
	struct drm_fb_helper helper;
	struct intel_framebuffer *fb;
	struct i915_vma *vma;
	unsigned long vma_flags;
	async_cookie_t cookie;
	int preferred_bpp;

	/* Whether or not fbdev hpd processing is temporarily suspended */
	bool hpd_suspended : 1;
	/* Set when a hotplug was received while HPD processing was
	 * suspended
	 */
	bool hpd_waiting : 1;

	/* Protects hpd_suspended */
	struct mutex hpd_lock;
};

enum intel_hotplug_state {
	INTEL_HOTPLUG_UNCHANGED,
	INTEL_HOTPLUG_CHANGED,
	INTEL_HOTPLUG_RETRY,
};

struct intel_encoder {
	struct drm_encoder base;

	enum intel_output_type type;
	enum port port;
	u16 cloneable;
	u8 pipe_mask;
	enum intel_hotplug_state (*hotplug)(struct intel_encoder *encoder,
					    struct intel_connector *connector);
	enum intel_output_type (*compute_output_type)(struct intel_encoder *,
						      struct intel_crtc_state *,
						      struct drm_connector_state *);
	int (*compute_config)(struct intel_encoder *,
			      struct intel_crtc_state *,
			      struct drm_connector_state *);
	int (*compute_config_late)(struct intel_encoder *,
				   struct intel_crtc_state *,
				   struct drm_connector_state *);
	void (*update_prepare)(struct intel_atomic_state *,
			       struct intel_encoder *,
			       struct intel_crtc *);
	void (*pre_pll_enable)(struct intel_atomic_state *,
			       struct intel_encoder *,
			       const struct intel_crtc_state *,
			       const struct drm_connector_state *);
	void (*pre_enable)(struct intel_atomic_state *,
			   struct intel_encoder *,
			   const struct intel_crtc_state *,
			   const struct drm_connector_state *);
	void (*enable)(struct intel_atomic_state *,
		       struct intel_encoder *,
		       const struct intel_crtc_state *,
		       const struct drm_connector_state *);
	void (*update_complete)(struct intel_atomic_state *,
				struct intel_encoder *,
				struct intel_crtc *);
	void (*disable)(struct intel_atomic_state *,
			struct intel_encoder *,
			const struct intel_crtc_state *,
			const struct drm_connector_state *);
	void (*post_disable)(struct intel_atomic_state *,
			     struct intel_encoder *,
			     const struct intel_crtc_state *,
			     const struct drm_connector_state *);
	void (*post_pll_disable)(struct intel_atomic_state *,
				 struct intel_encoder *,
				 const struct intel_crtc_state *,
				 const struct drm_connector_state *);
	void (*update_pipe)(struct intel_atomic_state *,
			    struct intel_encoder *,
			    const struct intel_crtc_state *,
			    const struct drm_connector_state *);
	/* Read out the current hw state of this connector, returning true if
	 * the encoder is active. If the encoder is enabled it also set the pipe
	 * it is connected to in the pipe parameter. */
	bool (*get_hw_state)(struct intel_encoder *, enum pipe *pipe);
	/* Reconstructs the equivalent mode flags for the current hardware
	 * state. This must be called _after_ display->get_pipe_config has
	 * pre-filled the pipe config. Note that intel_encoder->base.crtc must
	 * be set correctly before calling this function. */
	void (*get_config)(struct intel_encoder *,
			   struct intel_crtc_state *pipe_config);

	/*
	 * Optional hook called during init/resume to sync any state
	 * stored in the encoder (eg. DP link parameters) wrt. the HW state.
	 */
	void (*sync_state)(struct intel_encoder *encoder,
			   const struct intel_crtc_state *crtc_state);

	/*
	 * Optional hook, returning true if this encoder allows a fastset
	 * during the initial commit, false otherwise.
	 */
	bool (*initial_fastset_check)(struct intel_encoder *encoder,
				      struct intel_crtc_state *crtc_state);

	/*
	 * Acquires the power domains needed for an active encoder during
	 * hardware state readout.
	 */
	void (*get_power_domains)(struct intel_encoder *encoder,
				  struct intel_crtc_state *crtc_state);
	/*
	 * Called during system suspend after all pending requests for the
	 * encoder are flushed (for example for DP AUX transactions) and
	 * device interrupts are disabled.
	 */
	void (*suspend)(struct intel_encoder *);
	/*
	 * Called during system reboot/shutdown after all the
	 * encoders have been disabled and suspended.
	 */
	void (*shutdown)(struct intel_encoder *encoder);
	/*
	 * Enable/disable the clock to the port.
	 */
	void (*enable_clock)(struct intel_encoder *encoder,
			     const struct intel_crtc_state *crtc_state);
	void (*disable_clock)(struct intel_encoder *encoder);
	/*
	 * Returns whether the port clock is enabled or not.
	 */
	bool (*is_clock_enabled)(struct intel_encoder *encoder);
	enum hpd_pin hpd_pin;
	enum intel_display_power_domain power_domain;
	/* for communication with audio component; protected by av_mutex */
	const struct drm_connector *audio_connector;

	/* VBT information for this encoder (may be NULL for older platforms) */
	const struct intel_bios_encoder_data *devdata;
};

struct intel_panel_bl_funcs {
	/* Connector and platform specific backlight functions */
	int (*setup)(struct intel_connector *connector, enum pipe pipe);
	u32 (*get)(struct intel_connector *connector, enum pipe pipe);
	void (*set)(const struct drm_connector_state *conn_state, u32 level);
	void (*disable)(const struct drm_connector_state *conn_state, u32 level);
	void (*enable)(const struct intel_crtc_state *crtc_state,
		       const struct drm_connector_state *conn_state, u32 level);
	u32 (*hz_to_pwm)(struct intel_connector *connector, u32 hz);
};

struct intel_panel {
	struct drm_display_mode *fixed_mode;
	struct drm_display_mode *downclock_mode;

	/* backlight */
	struct {
		bool present;
		u32 level;
		u32 min;
		u32 max;
		bool enabled;
		bool combination_mode;	/* gen 2/4 only */
		bool active_low_pwm;
		bool alternate_pwm_increment;	/* lpt+ */

		/* PWM chip */
		u32 pwm_level_min;
		u32 pwm_level_max;
		bool pwm_enabled;
		bool util_pin_active_low;	/* bxt+ */
		u8 controller;		/* bxt+ only */
		struct pwm_device *pwm;
		struct pwm_state pwm_state;

		/* DPCD backlight */
		union {
			struct {
				u8 pwmgen_bit_count;
			} vesa;
			struct {
				bool sdr_uses_aux;
			} intel;
		} edp;

		struct backlight_device *device;

		const struct intel_panel_bl_funcs *funcs;
		const struct intel_panel_bl_funcs *pwm_funcs;
		void (*power)(struct intel_connector *, bool enable);
	} backlight;
};

struct intel_digital_port;

enum check_link_response {
	HDCP_LINK_PROTECTED	= 0,
	HDCP_TOPOLOGY_CHANGE,
	HDCP_LINK_INTEGRITY_FAILURE,
	HDCP_REAUTH_REQUEST
};

/*
 * This structure serves as a translation layer between the generic HDCP code
 * and the bus-specific code. What that means is that HDCP over HDMI differs
 * from HDCP over DP, so to account for these differences, we need to
 * communicate with the receiver through this shim.
 *
 * For completeness, the 2 buses differ in the following ways:
 *	- DP AUX vs. DDC
 *		HDCP registers on the receiver are set via DP AUX for DP, and
 *		they are set via DDC for HDMI.
 *	- Receiver register offsets
 *		The offsets of the registers are different for DP vs. HDMI
 *	- Receiver register masks/offsets
 *		For instance, the ready bit for the KSV fifo is in a different
 *		place on DP vs HDMI
 *	- Receiver register names
 *		Seriously. In the DP spec, the 16-bit register containing
 *		downstream information is called BINFO, on HDMI it's called
 *		BSTATUS. To confuse matters further, DP has a BSTATUS register
 *		with a completely different definition.
 *	- KSV FIFO
 *		On HDMI, the ksv fifo is read all at once, whereas on DP it must
 *		be read 3 keys at a time
 *	- Aksv output
 *		Since Aksv is hidden in hardware, there's different procedures
 *		to send it over DP AUX vs DDC
 */
struct intel_hdcp_shim {
	/* Outputs the transmitter's An and Aksv values to the receiver. */
	int (*write_an_aksv)(struct intel_digital_port *dig_port, u8 *an);

	/* Reads the receiver's key selection vector */
	int (*read_bksv)(struct intel_digital_port *dig_port, u8 *bksv);

	/*
	 * Reads BINFO from DP receivers and BSTATUS from HDMI receivers. The
	 * definitions are the same in the respective specs, but the names are
	 * different. Call it BSTATUS since that's the name the HDMI spec
	 * uses and it was there first.
	 */
	int (*read_bstatus)(struct intel_digital_port *dig_port,
			    u8 *bstatus);

	/* Determines whether a repeater is present downstream */
	int (*repeater_present)(struct intel_digital_port *dig_port,
				bool *repeater_present);

	/* Reads the receiver's Ri' value */
	int (*read_ri_prime)(struct intel_digital_port *dig_port, u8 *ri);

	/* Determines if the receiver's KSV FIFO is ready for consumption */
	int (*read_ksv_ready)(struct intel_digital_port *dig_port,
			      bool *ksv_ready);

	/* Reads the ksv fifo for num_downstream devices */
	int (*read_ksv_fifo)(struct intel_digital_port *dig_port,
			     int num_downstream, u8 *ksv_fifo);

	/* Reads a 32-bit part of V' from the receiver */
	int (*read_v_prime_part)(struct intel_digital_port *dig_port,
				 int i, u32 *part);

	/* Enables HDCP signalling on the port */
	int (*toggle_signalling)(struct intel_digital_port *dig_port,
				 enum transcoder cpu_transcoder,
				 bool enable);

	/* Enable/Disable stream encryption on DP MST Transport Link */
	int (*stream_encryption)(struct intel_connector *connector,
				 bool enable);

	/* Ensures the link is still protected */
	bool (*check_link)(struct intel_digital_port *dig_port,
			   struct intel_connector *connector);

	/* Detects panel's hdcp capability. This is optional for HDMI. */
	int (*hdcp_capable)(struct intel_digital_port *dig_port,
			    bool *hdcp_capable);

	/* HDCP adaptation(DP/HDMI) required on the port */
	enum hdcp_wired_protocol protocol;

	/* Detects whether sink is HDCP2.2 capable */
	int (*hdcp_2_2_capable)(struct intel_digital_port *dig_port,
				bool *capable);

	/* Detects whether a HDCP 1.4 sink connected in MST topology */
	int (*streams_type1_capable)(struct intel_connector *connector,
				     bool *capable);

	/* Write HDCP2.2 messages */
	int (*write_2_2_msg)(struct intel_digital_port *dig_port,
			     void *buf, size_t size);

	/* Read HDCP2.2 messages */
	int (*read_2_2_msg)(struct intel_digital_port *dig_port,
			    u8 msg_id, void *buf, size_t size);

	/*
	 * Implementation of DP HDCP2.2 Errata for the communication of stream
	 * type to Receivers. In DP HDCP2.2 Stream type is one of the input to
	 * the HDCP2.2 Cipher for En/De-Cryption. Not applicable for HDMI.
	 */
	int (*config_stream_type)(struct intel_digital_port *dig_port,
				  bool is_repeater, u8 type);

	/* Enable/Disable HDCP 2.2 stream encryption on DP MST Transport Link */
	int (*stream_2_2_encryption)(struct intel_connector *connector,
				     bool enable);

	/* HDCP2.2 Link Integrity Check */
	int (*check_2_2_link)(struct intel_digital_port *dig_port,
			      struct intel_connector *connector);
};

struct intel_hdcp {
	const struct intel_hdcp_shim *shim;
	/* Mutex for hdcp state of the connector */
	struct mutex mutex;
	u64 value;
	struct delayed_work check_work;
	struct work_struct prop_work;

	/* HDCP1.4 Encryption status */
	bool hdcp_encrypted;

	/* HDCP2.2 related definitions */
	/* Flag indicates whether this connector supports HDCP2.2 or not. */
	bool hdcp2_supported;

	/* HDCP2.2 Encryption status */
	bool hdcp2_encrypted;

	/*
	 * Content Stream Type defined by content owner. TYPE0(0x0) content can
	 * flow in the link protected by HDCP2.2 or HDCP1.4, where as TYPE1(0x1)
	 * content can flow only through a link protected by HDCP2.2.
	 */
	u8 content_type;

	bool is_paired;
	bool is_repeater;

	/*
	 * Count of ReceiverID_List received. Initialized to 0 at AKE_INIT.
	 * Incremented after processing the RepeaterAuth_Send_ReceiverID_List.
	 * When it rolls over re-auth has to be triggered.
	 */
	u32 seq_num_v;

	/*
	 * Count of RepeaterAuth_Stream_Manage msg propagated.
	 * Initialized to 0 on AKE_INIT. Incremented after every successful
	 * transmission of RepeaterAuth_Stream_Manage message. When it rolls
	 * over re-Auth has to be triggered.
	 */
	u32 seq_num_m;

	/*
	 * Work queue to signal the CP_IRQ. Used for the waiters to read the
	 * available information from HDCP DP sink.
	 */
	wait_queue_head_t cp_irq_queue;
	atomic_t cp_irq_count;
	int cp_irq_count_cached;

	/*
	 * HDCP register access for gen12+ need the transcoder associated.
	 * Transcoder attached to the connector could be changed at modeset.
	 * Hence caching the transcoder here.
	 */
	enum transcoder cpu_transcoder;
	/* Only used for DP MST stream encryption */
	enum transcoder stream_transcoder;
};

struct intel_connector {
	struct drm_connector base;
	/*
	 * The fixed encoder this connector is connected to.
	 */
	struct intel_encoder *encoder;

	/* ACPI device id for ACPI and driver cooperation */
	u32 acpi_device_id;

	/* Reads out the current hw, returning true if the connector is enabled
	 * and active (i.e. dpms ON state). */
	bool (*get_hw_state)(struct intel_connector *);

	/* Panel info for eDP and LVDS */
	struct intel_panel panel;

	/* Cached EDID for eDP and LVDS. May hold ERR_PTR for invalid EDID. */
	struct edid *edid;
	struct edid *detect_edid;

	/* Number of times hotplug detection was tried after an HPD interrupt */
	int hotplug_retries;

	/* since POLL and HPD connectors may use the same HPD line keep the native
	   state of connector->polled in case hotplug storm detection changes it */
	u8 polled;

	struct drm_dp_mst_port *port;

	struct intel_dp *mst_port;

	/* Work struct to schedule a uevent on link train failure */
	struct work_struct modeset_retry_work;

	struct intel_hdcp hdcp;
};

struct intel_digital_connector_state {
	struct drm_connector_state base;

	enum hdmi_force_audio force_audio;
	int broadcast_rgb;
};

#define to_intel_digital_connector_state(x) container_of(x, struct intel_digital_connector_state, base)

struct dpll {
	/* given values */
	int n;
	int m1, m2;
	int p1, p2;
	/* derived values */
	int	dot;
	int	vco;
	int	m;
	int	p;
};

struct intel_atomic_state {
	struct drm_atomic_state base;

	intel_wakeref_t wakeref;

	struct __intel_global_objs_state *global_objs;
	int num_global_objs;

	bool dpll_set, modeset;

	struct intel_shared_dpll_state shared_dpll[I915_NUM_PLLS];

	/*
	 * Current watermarks can't be trusted during hardware readout, so
	 * don't bother calculating intermediate watermarks.
	 */
	bool skip_intermediate_wm;

	bool rps_interactive;

	struct i915_sw_fence commit_ready;

	struct llist_node freed;
};

struct intel_plane_state {
	struct drm_plane_state uapi;

	/*
	 * actual hardware state, the state we program to the hardware.
	 * The following members are used to verify the hardware state:
	 * During initial hw readout, they need to be copied from uapi.
	 */
	struct {
		struct drm_crtc *crtc;
		struct drm_framebuffer *fb;

		u16 alpha;
		u16 pixel_blend_mode;
		unsigned int rotation;
		enum drm_color_encoding color_encoding;
		enum drm_color_range color_range;
		enum drm_scaling_filter scaling_filter;
	} hw;

	struct i915_vma *ggtt_vma;
	struct i915_vma *dpt_vma;
	unsigned long flags;
#define PLANE_HAS_FENCE BIT(0)

	struct intel_fb_view view;

	/* plane control register */
	u32 ctl;

	/* plane color control register */
	u32 color_ctl;

	/* chroma upsampler control register */
	u32 cus_ctl;

	/*
	 * scaler_id
	 *    = -1 : not using a scaler
	 *    >=  0 : using a scalers
	 *
	 * plane requiring a scaler:
	 *   - During check_plane, its bit is set in
	 *     crtc_state->scaler_state.scaler_users by calling helper function
	 *     update_scaler_plane.
	 *   - scaler_id indicates the scaler it got assigned.
	 *
	 * plane doesn't require a scaler:
	 *   - this can happen when scaling is no more required or plane simply
	 *     got disabled.
	 *   - During check_plane, corresponding bit is reset in
	 *     crtc_state->scaler_state.scaler_users by calling helper function
	 *     update_scaler_plane.
	 */
	int scaler_id;

	/*
	 * planar_linked_plane:
	 *
	 * ICL planar formats require 2 planes that are updated as pairs.
	 * This member is used to make sure the other plane is also updated
	 * when required, and for update_slave() to find the correct
	 * plane_state to pass as argument.
	 */
	struct intel_plane *planar_linked_plane;

	/*
	 * planar_slave:
	 * If set don't update use the linked plane's state for updating
	 * this plane during atomic commit with the update_slave() callback.
	 *
	 * It's also used by the watermark code to ignore wm calculations on
	 * this plane. They're calculated by the linked plane's wm code.
	 */
	u32 planar_slave;

	struct drm_intel_sprite_colorkey ckey;

	struct drm_rect psr2_sel_fetch_area;

	/* Clear Color Value */
	u64 ccval;
};

struct intel_initial_plane_config {
	struct intel_framebuffer *fb;
	struct i915_vma *vma;
	unsigned int tiling;
	int size;
	u32 base;
	u8 rotation;
};

struct intel_scaler {
	int in_use;
	u32 mode;
};

struct intel_crtc_scaler_state {
#define SKL_NUM_SCALERS 2
	struct intel_scaler scalers[SKL_NUM_SCALERS];

	/*
	 * scaler_users: keeps track of users requesting scalers on this crtc.
	 *
	 *     If a bit is set, a user is using a scaler.
	 *     Here user can be a plane or crtc as defined below:
	 *       bits 0-30 - plane (bit position is index from drm_plane_index)
	 *       bit 31    - crtc
	 *
	 * Instead of creating a new index to cover planes and crtc, using
	 * existing drm_plane_index for planes which is well less than 31
	 * planes and bit 31 for crtc. This should be fine to cover all
	 * our platforms.
	 *
	 * intel_atomic_setup_scalers will setup available scalers to users
	 * requesting scalers. It will gracefully fail if request exceeds
	 * avilability.
	 */
#define SKL_CRTC_INDEX 31
	unsigned scaler_users;

	/* scaler used by crtc for panel fitting purpose */
	int scaler_id;
};

/* {crtc,crtc_state}->mode_flags */
/* Flag to get scanline using frame time stamps */
#define I915_MODE_FLAG_GET_SCANLINE_FROM_TIMESTAMP (1<<1)
/* Flag to use the scanline counter instead of the pixel counter */
#define I915_MODE_FLAG_USE_SCANLINE_COUNTER (1<<2)
/*
 * TE0 or TE1 flag is set if the crtc has a DSI encoder which
 * is operating in command mode.
 * Flag to use TE from DSI0 instead of VBI in command mode
 */
#define I915_MODE_FLAG_DSI_USE_TE0 (1<<3)
/* Flag to use TE from DSI1 instead of VBI in command mode */
#define I915_MODE_FLAG_DSI_USE_TE1 (1<<4)
/* Flag to indicate mipi dsi periodic command mode where we do not get TE */
#define I915_MODE_FLAG_DSI_PERIODIC_CMD_MODE (1<<5)
/* Do tricks to make vblank timestamps sane with VRR? */
#define I915_MODE_FLAG_VRR (1<<6)

struct intel_wm_level {
	bool enable;
	u32 pri_val;
	u32 spr_val;
	u32 cur_val;
	u32 fbc_val;
};

struct intel_pipe_wm {
	struct intel_wm_level wm[5];
	bool fbc_wm_enabled;
	bool pipe_enabled;
	bool sprites_enabled;
	bool sprites_scaled;
};

struct skl_wm_level {
	u16 min_ddb_alloc;
	u16 blocks;
	u8 lines;
	bool enable;
	bool ignore_lines;
	bool can_sagv;
};

struct skl_plane_wm {
	struct skl_wm_level wm[8];
	struct skl_wm_level uv_wm[8];
	struct skl_wm_level trans_wm;
	struct {
		struct skl_wm_level wm0;
		struct skl_wm_level trans_wm;
	} sagv;
	bool is_planar;
};

struct skl_pipe_wm {
	struct skl_plane_wm planes[I915_MAX_PLANES];
	bool use_sagv_wm;
};

enum vlv_wm_level {
	VLV_WM_LEVEL_PM2,
	VLV_WM_LEVEL_PM5,
	VLV_WM_LEVEL_DDR_DVFS,
	NUM_VLV_WM_LEVELS,
};

struct vlv_wm_state {
	struct g4x_pipe_wm wm[NUM_VLV_WM_LEVELS];
	struct g4x_sr_wm sr[NUM_VLV_WM_LEVELS];
	u8 num_levels;
	bool cxsr;
};

struct vlv_fifo_state {
	u16 plane[I915_MAX_PLANES];
};

enum g4x_wm_level {
	G4X_WM_LEVEL_NORMAL,
	G4X_WM_LEVEL_SR,
	G4X_WM_LEVEL_HPLL,
	NUM_G4X_WM_LEVELS,
};

struct g4x_wm_state {
	struct g4x_pipe_wm wm;
	struct g4x_sr_wm sr;
	struct g4x_sr_wm hpll;
	bool cxsr;
	bool hpll_en;
	bool fbc_en;
};

struct intel_crtc_wm_state {
	union {
		/*
		 * raw:
		 * The "raw" watermark values produced by the formula
		 * given the plane's current state. They do not consider
		 * how much FIFO is actually allocated for each plane.
		 *
		 * optimal:
		 * The "optimal" watermark values given the current
		 * state of the planes and the amount of FIFO
		 * allocated to each, ignoring any previous state
		 * of the planes.
		 *
		 * intermediate:
		 * The "intermediate" watermark values when transitioning
		 * between the old and new "optimal" values. Used when
		 * the watermark registers are single buffered and hence
		 * their state changes asynchronously with regards to the
		 * actual plane registers. These are essentially the
		 * worst case combination of the old and new "optimal"
		 * watermarks, which are therefore safe to use when the
		 * plane is in either its old or new state.
		 */
		struct {
			struct intel_pipe_wm intermediate;
			struct intel_pipe_wm optimal;
		} ilk;

		struct {
			struct skl_pipe_wm raw;
			/* gen9+ only needs 1-step wm programming */
			struct skl_pipe_wm optimal;
			struct skl_ddb_entry ddb;
			struct skl_ddb_entry plane_ddb_y[I915_MAX_PLANES];
			struct skl_ddb_entry plane_ddb_uv[I915_MAX_PLANES];
		} skl;

		struct {
			struct g4x_pipe_wm raw[NUM_VLV_WM_LEVELS]; /* not inverted */
			struct vlv_wm_state intermediate; /* inverted */
			struct vlv_wm_state optimal; /* inverted */
			struct vlv_fifo_state fifo_state;
		} vlv;

		struct {
			struct g4x_pipe_wm raw[NUM_G4X_WM_LEVELS];
			struct g4x_wm_state intermediate;
			struct g4x_wm_state optimal;
		} g4x;
	};

	/*
	 * Platforms with two-step watermark programming will need to
	 * update watermark programming post-vblank to switch from the
	 * safe intermediate watermarks to the optimal final
	 * watermarks.
	 */
	bool need_postvbl_update;
};

enum intel_output_format {
	INTEL_OUTPUT_FORMAT_RGB,
	INTEL_OUTPUT_FORMAT_YCBCR420,
	INTEL_OUTPUT_FORMAT_YCBCR444,
};

struct intel_crtc_state {
	/*
	 * uapi (drm) state. This is the software state shown to userspace.
	 * In particular, the following members are used for bookkeeping:
	 * - crtc
	 * - state
	 * - *_changed
	 * - event
	 * - commit
	 * - mode_blob
	 */
	struct drm_crtc_state uapi;

	/*
	 * actual hardware state, the state we program to the hardware.
	 * The following members are used to verify the hardware state:
	 * - enable
	 * - active
	 * - mode / pipe_mode / adjusted_mode
	 * - color property blobs.
	 *
	 * During initial hw readout, they need to be copied to uapi.
	 *
	 * Bigjoiner will allow a transcoder mode that spans 2 pipes;
	 * Use the pipe_mode for calculations like watermarks, pipe
	 * scaler, and bandwidth.
	 *
	 * Use adjusted_mode for things that need to know the full
	 * mode on the transcoder, which spans all pipes.
	 */
	struct {
		bool active, enable;
		struct drm_property_blob *degamma_lut, *gamma_lut, *ctm;
		struct drm_display_mode mode, pipe_mode, adjusted_mode;
		enum drm_scaling_filter scaling_filter;
	} hw;

	/**
	 * quirks - bitfield with hw state readout quirks
	 *
	 * For various reasons the hw state readout code might not be able to
	 * completely faithfully read out the current state. These cases are
	 * tracked with quirk flags so that fastboot and state checker can act
	 * accordingly.
	 */
#define PIPE_CONFIG_QUIRK_MODE_SYNC_FLAGS	(1<<0) /* unreliable sync mode.flags */
#define PIPE_CONFIG_QUIRK_BIGJOINER_SLAVE      (1<<1) /* bigjoiner slave, partial readout */
	unsigned long quirks;

	unsigned fb_bits; /* framebuffers to flip */
	bool update_pipe; /* can a fast modeset be performed? */
	bool disable_cxsr;
	bool update_wm_pre, update_wm_post; /* watermarks are updated */
	bool fifo_changed; /* FIFO split is changed */
	bool preload_luts;
	bool inherited; /* state inherited from BIOS? */

	/* Pipe source size (ie. panel fitter input size)
	 * All planes will be positioned inside this space,
	 * and get clipped at the edges. */
	int pipe_src_w, pipe_src_h;

	/*
	 * Pipe pixel rate, adjusted for
	 * panel fitter/pipe scaler downscaling.
	 */
	unsigned int pixel_rate;

	/* Whether to set up the PCH/FDI. Note that we never allow sharing
	 * between pch encoders and cpu encoders. */
	bool has_pch_encoder;

	/* Are we sending infoframes on the attached port */
	bool has_infoframe;

	/* CPU Transcoder for the pipe. Currently this can only differ from the
	 * pipe on Haswell and later (where we have a special eDP transcoder)
	 * and Broxton (where we have special DSI transcoders). */
	enum transcoder cpu_transcoder;

	/*
	 * Use reduced/limited/broadcast rbg range, compressing from the full
	 * range fed into the crtcs.
	 */
	bool limited_color_range;

	/* Bitmask of encoder types (enum intel_output_type)
	 * driven by the pipe.
	 */
	unsigned int output_types;

	/* Whether we should send NULL infoframes. Required for audio. */
	bool has_hdmi_sink;

	/* Audio enabled on this pipe. Only valid if either has_hdmi_sink or
	 * has_dp_encoder is set. */
	bool has_audio;

	/*
	 * Enable dithering, used when the selected pipe bpp doesn't match the
	 * plane bpp.
	 */
	bool dither;

	/*
	 * Dither gets enabled for 18bpp which causes CRC mismatch errors for
	 * compliance video pattern tests.
	 * Disable dither only if it is a compliance test request for
	 * 18bpp.
	 */
	bool dither_force_disable;

	/* Controls for the clock computation, to override various stages. */
	bool clock_set;

	/* SDVO TV has a bunch of special case. To make multifunction encoders
	 * work correctly, we need to track this at runtime.*/
	bool sdvo_tv_clock;

	/*
	 * crtc bandwidth limit, don't increase pipe bpp or clock if not really
	 * required. This is set in the 2nd loop of calling encoder's
	 * ->compute_config if the first pick doesn't work out.
	 */
	bool bw_constrained;

	/* Settings for the intel dpll used on pretty much everything but
	 * haswell. */
	struct dpll dpll;

	/* Selected dpll when shared or NULL. */
	struct intel_shared_dpll *shared_dpll;

	/* Actual register state of the dpll, for shared dpll cross-checking. */
	struct intel_dpll_hw_state dpll_hw_state;

	/*
	 * ICL reserved DPLLs for the CRTC/port. The active PLL is selected by
	 * setting shared_dpll and dpll_hw_state to one of these reserved ones.
	 */
	struct icl_port_dpll {
		struct intel_shared_dpll *pll;
		struct intel_dpll_hw_state hw_state;
	} icl_port_dplls[ICL_PORT_DPLL_COUNT];

	/* DSI PLL registers */
	struct {
		u32 ctrl, div;
	} dsi_pll;

	int pipe_bpp;
	struct intel_link_m_n dp_m_n;

	/* m2_n2 for eDP downclock */
	struct intel_link_m_n dp_m2_n2;
	bool has_drrs;

	bool has_psr;
	bool has_psr2;
	bool enable_psr2_sel_fetch;
	u32 dc3co_exitline;

	/*
	 * Frequence the dpll for the port should run at. Differs from the
	 * adjusted dotclock e.g. for DP or 10/12bpc hdmi mode. This is also
	 * already multiplied by pixel_multiplier.
	 */
	int port_clock;

	/* Used by SDVO (and if we ever fix it, HDMI). */
	unsigned pixel_multiplier;

	/* I915_MODE_FLAG_* */
	u8 mode_flags;

	u8 lane_count;

	/*
	 * Used by platforms having DP/HDMI PHY with programmable lane
	 * latency optimization.
	 */
	u8 lane_lat_optim_mask;

	/* minimum acceptable voltage level */
	u8 min_voltage_level;

	/* Panel fitter controls for gen2-gen4 + VLV */
	struct {
		u32 control;
		u32 pgm_ratios;
		u32 lvds_border_bits;
	} gmch_pfit;

	/* Panel fitter placement and size for Ironlake+ */
	struct {
		struct drm_rect dst;
		bool enabled;
		bool force_thru;
	} pch_pfit;

	/* FDI configuration, only valid if has_pch_encoder is set. */
	int fdi_lanes;
	struct intel_link_m_n fdi_m_n;

	bool ips_enabled;

	bool crc_enabled;

	bool enable_fbc;

	bool double_wide;

	int pbn;

	struct intel_crtc_scaler_state scaler_state;

	/* w/a for waiting 2 vblanks during crtc enable */
	enum pipe hsw_workaround_pipe;

	/* IVB sprite scaling w/a (WaCxSRDisabledForSpriteScaling:ivb) */
	bool disable_lp_wm;

	struct intel_crtc_wm_state wm;

	int min_cdclk[I915_MAX_PLANES];

	u32 data_rate[I915_MAX_PLANES];

	/* FIXME unify with data_rate[] */
	u64 plane_data_rate[I915_MAX_PLANES];
	u64 uv_plane_data_rate[I915_MAX_PLANES];

	/* Gamma mode programmed on the pipe */
	u32 gamma_mode;

	union {
		/* CSC mode programmed on the pipe */
		u32 csc_mode;

		/* CHV CGM mode */
		u32 cgm_mode;
	};

	/* bitmask of logically enabled planes (enum plane_id) */
	u8 enabled_planes;

	/* bitmask of actually visible planes (enum plane_id) */
	u8 active_planes;
	u8 nv12_planes;
	u8 c8_planes;

	/* bitmask of planes that will be updated during the commit */
	u8 update_planes;

	struct {
		u32 enable;
		u32 gcp;
		union hdmi_infoframe avi;
		union hdmi_infoframe spd;
		union hdmi_infoframe hdmi;
		union hdmi_infoframe drm;
		struct drm_dp_vsc_sdp vsc;
	} infoframes;

	/* HDMI scrambling status */
	bool hdmi_scrambling;

	/* HDMI High TMDS char rate ratio */
	bool hdmi_high_tmds_clock_ratio;

	/* Output format RGB/YCBCR etc */
	enum intel_output_format output_format;

	/* enable pipe gamma? */
	bool gamma_enable;

	/* enable pipe csc? */
	bool csc_enable;

	/* enable pipe big joiner? */
	bool bigjoiner;

	/* big joiner slave crtc? */
	bool bigjoiner_slave;

	/* linked crtc for bigjoiner, either slave or master */
	struct intel_crtc *bigjoiner_linked_crtc;

	/* Display Stream compression state */
	struct {
		bool compression_enable;
		bool dsc_split;
		u16 compressed_bpp;
		u8 slice_count;
		struct drm_dsc_config config;
	} dsc;

	/* HSW+ linetime watermarks */
	u16 linetime;
	u16 ips_linetime;

	/* Forward Error correction State */
	bool fec_enable;

	/* Pointer to master transcoder in case of tiled displays */
	enum transcoder master_transcoder;

	/* Bitmask to indicate slaves attached */
	u8 sync_mode_slaves_mask;

	/* Only valid on TGL+ */
	enum transcoder mst_master_transcoder;

	/* For DSB related info */
	struct intel_dsb *dsb;

	u32 psr2_man_track_ctl;

	/* Variable Refresh Rate state */
	struct {
		bool enable;
		u8 pipeline_full;
		u16 flipline, vmin, vmax, guardband;
	} vrr;

	/* Stream Splitter for eDP MSO */
	struct {
		bool enable;
		u8 link_count;
		u8 pixel_overlap;
	} splitter;
};

enum intel_pipe_crc_source {
	INTEL_PIPE_CRC_SOURCE_NONE,
	INTEL_PIPE_CRC_SOURCE_PLANE1,
	INTEL_PIPE_CRC_SOURCE_PLANE2,
	INTEL_PIPE_CRC_SOURCE_PLANE3,
	INTEL_PIPE_CRC_SOURCE_PLANE4,
	INTEL_PIPE_CRC_SOURCE_PLANE5,
	INTEL_PIPE_CRC_SOURCE_PLANE6,
	INTEL_PIPE_CRC_SOURCE_PLANE7,
	INTEL_PIPE_CRC_SOURCE_PIPE,
	/* TV/DP on pre-gen5/vlv can't use the pipe source. */
	INTEL_PIPE_CRC_SOURCE_TV,
	INTEL_PIPE_CRC_SOURCE_DP_B,
	INTEL_PIPE_CRC_SOURCE_DP_C,
	INTEL_PIPE_CRC_SOURCE_DP_D,
	INTEL_PIPE_CRC_SOURCE_AUTO,
	INTEL_PIPE_CRC_SOURCE_MAX,
};

#define INTEL_PIPE_CRC_ENTRIES_NR	128
struct intel_pipe_crc {
	spinlock_t lock;
	int skipped;
	enum intel_pipe_crc_source source;
};

struct intel_crtc {
	struct drm_crtc base;
	enum pipe pipe;
	/*
	 * Whether the crtc and the connected output pipeline is active. Implies
	 * that crtc->enabled is set, i.e. the current mode configuration has
	 * some outputs connected to this crtc.
	 */
	bool active;
	u8 plane_ids_mask;

	/* I915_MODE_FLAG_* */
	u8 mode_flags;

	u16 vmax_vblank_start;

	struct intel_display_power_domain_set enabled_power_domains;
	struct intel_overlay *overlay;

	struct intel_crtc_state *config;

	/* Access to these should be protected by dev_priv->irq_lock. */
	bool cpu_fifo_underrun_disabled;
	bool pch_fifo_underrun_disabled;

	/* per-pipe watermark state */
	struct {
		/* watermarks currently being used  */
		union {
			struct intel_pipe_wm ilk;
			struct vlv_wm_state vlv;
			struct g4x_wm_state g4x;
		} active;
	} wm;

	int scanline_offset;

	struct {
		unsigned start_vbl_count;
		ktime_t start_vbl_time;
		int min_vbl, max_vbl;
		int scanline_start;
#ifdef CONFIG_DRM_I915_DEBUG_VBLANK_EVADE
		struct {
			u64 min;
			u64 max;
			u64 sum;
			unsigned int over;
			unsigned int times[17]; /* [1us, 16ms] */
		} vbl;
#endif
	} debug;

	/* scalers available on this crtc */
	int num_scalers;

#ifdef CONFIG_DEBUG_FS
	struct intel_pipe_crc pipe_crc;
#endif
};

struct intel_plane {
	struct drm_plane base;
	enum i9xx_plane_id i9xx_plane;
	enum plane_id id;
	enum pipe pipe;
	bool has_fbc;
	bool has_ccs;
	bool need_async_flip_disable_wa;
	u32 frontbuffer_bit;

	struct {
		u32 base, cntl, size;
	} cursor;

	/*
	 * NOTE: Do not place new plane state fields here (e.g., when adding
	 * new plane properties).  New runtime state should now be placed in
	 * the intel_plane_state structure and accessed via plane_state.
	 */

	int (*min_width)(const struct drm_framebuffer *fb,
			 int color_plane,
			 unsigned int rotation);
	int (*max_width)(const struct drm_framebuffer *fb,
			 int color_plane,
			 unsigned int rotation);
	int (*max_height)(const struct drm_framebuffer *fb,
			  int color_plane,
			  unsigned int rotation);
	unsigned int (*max_stride)(struct intel_plane *plane,
				   u32 pixel_format, u64 modifier,
				   unsigned int rotation);
	void (*update_plane)(struct intel_plane *plane,
			     const struct intel_crtc_state *crtc_state,
			     const struct intel_plane_state *plane_state);
	void (*disable_plane)(struct intel_plane *plane,
			      const struct intel_crtc_state *crtc_state);
	bool (*get_hw_state)(struct intel_plane *plane, enum pipe *pipe);
	int (*check_plane)(struct intel_crtc_state *crtc_state,
			   struct intel_plane_state *plane_state);
	int (*min_cdclk)(const struct intel_crtc_state *crtc_state,
			 const struct intel_plane_state *plane_state);
	void (*async_flip)(struct intel_plane *plane,
			   const struct intel_crtc_state *crtc_state,
			   const struct intel_plane_state *plane_state,
			   bool async_flip);
	void (*enable_flip_done)(struct intel_plane *plane);
	void (*disable_flip_done)(struct intel_plane *plane);
};

struct intel_watermark_params {
	u16 fifo_size;
	u16 max_wm;
	u8 default_wm;
	u8 guard_size;
	u8 cacheline_size;
};

struct cxsr_latency {
	bool is_desktop : 1;
	bool is_ddr3 : 1;
	u16 fsb_freq;
	u16 mem_freq;
	u16 display_sr;
	u16 display_hpll_disable;
	u16 cursor_sr;
	u16 cursor_hpll_disable;
};

#define to_intel_atomic_state(x) container_of(x, struct intel_atomic_state, base)
#define to_intel_crtc(x) container_of(x, struct intel_crtc, base)
#define to_intel_crtc_state(x) container_of(x, struct intel_crtc_state, uapi)
#define to_intel_connector(x) container_of(x, struct intel_connector, base)
#define to_intel_encoder(x) container_of(x, struct intel_encoder, base)
#define to_intel_framebuffer(x) container_of(x, struct intel_framebuffer, base)
#define to_intel_plane(x) container_of(x, struct intel_plane, base)
#define to_intel_plane_state(x) container_of(x, struct intel_plane_state, uapi)
#define intel_fb_obj(x) ((x) ? to_intel_bo((x)->obj[0]) : NULL)

struct intel_hdmi {
	i915_reg_t hdmi_reg;
	int ddc_bus;
	struct {
		enum drm_dp_dual_mode_type type;
		int max_tmds_clock;
	} dp_dual_mode;
	bool has_hdmi_sink;
	bool has_audio;
	struct intel_connector *attached_connector;
	struct cec_notifier *cec_notifier;
};

struct intel_dp_mst_encoder;
/*
 * enum link_m_n_set:
 *	When platform provides two set of M_N registers for dp, we can
 *	program them and switch between them incase of DRRS.
 *	But When only one such register is provided, we have to program the
 *	required divider value on that registers itself based on the DRRS state.
 *
 * M1_N1	: Program dp_m_n on M1_N1 registers
 *			  dp_m2_n2 on M2_N2 registers (If supported)
 *
 * M2_N2	: Program dp_m2_n2 on M1_N1 registers
 *			  M2_N2 registers are not supported
 */

enum link_m_n_set {
	/* Sets the m1_n1 and m2_n2 */
	M1_N1 = 0,
	M2_N2
};

struct intel_dp_compliance_data {
	unsigned long edid;
	u8 video_pattern;
	u16 hdisplay, vdisplay;
	u8 bpc;
	struct drm_dp_phy_test_params phytest;
};

struct intel_dp_compliance {
	unsigned long test_type;
	struct intel_dp_compliance_data test_data;
	bool test_active;
	int test_link_rate;
	u8 test_lane_count;
};

struct intel_dp_pcon_frl {
	bool is_trained;
	int trained_rate_gbps;
};

struct intel_pps {
	int panel_power_up_delay;
	int panel_power_down_delay;
	int panel_power_cycle_delay;
	int backlight_on_delay;
	int backlight_off_delay;
	struct delayed_work panel_vdd_work;
	bool want_panel_vdd;
	unsigned long last_power_on;
	unsigned long last_backlight_off;
	ktime_t panel_power_off_time;
	intel_wakeref_t vdd_wakeref;

	/*
	 * Pipe whose power sequencer is currently locked into
	 * this port. Only relevant on VLV/CHV.
	 */
	enum pipe pps_pipe;
	/*
	 * Pipe currently driving the port. Used for preventing
	 * the use of the PPS for any pipe currentrly driving
	 * external DP as that will mess things up on VLV.
	 */
	enum pipe active_pipe;
	/*
	 * Set if the sequencer may be reset due to a power transition,
	 * requiring a reinitialization. Only relevant on BXT.
	 */
	bool pps_reset;
	struct edp_power_seq pps_delays;
};

struct intel_psr {
	/* Mutex for PSR state of the transcoder */
	struct mutex lock;

#define I915_PSR_DEBUG_MODE_MASK	0x0f
#define I915_PSR_DEBUG_DEFAULT		0x00
#define I915_PSR_DEBUG_DISABLE		0x01
#define I915_PSR_DEBUG_ENABLE		0x02
#define I915_PSR_DEBUG_FORCE_PSR1	0x03
#define I915_PSR_DEBUG_ENABLE_SEL_FETCH	0x4
#define I915_PSR_DEBUG_IRQ		0x10

	u32 debug;
	bool sink_support;
	bool source_support;
	bool enabled;
	bool paused;
	enum pipe pipe;
	enum transcoder transcoder;
	bool active;
	struct work_struct work;
	unsigned int busy_frontbuffer_bits;
	bool sink_psr2_support;
	bool link_standby;
	bool colorimetry_support;
	bool psr2_enabled;
	bool psr2_sel_fetch_enabled;
	u8 sink_sync_latency;
	ktime_t last_entry_attempt;
	ktime_t last_exit;
	bool sink_not_reliable;
	bool irq_aux_error;
	u16 su_x_granularity;
	u32 dc3co_exitline;
	u32 dc3co_exit_delay;
	struct delayed_work dc3co_work;
	struct drm_dp_vsc_sdp vsc;
};

struct intel_dp {
	i915_reg_t output_reg;
	u32 DP;
	int link_rate;
	u8 lane_count;
	u8 sink_count;
	bool link_trained;
	bool has_hdmi_sink;
	bool has_audio;
	bool reset_link_params;
	bool use_max_params;
	u8 dpcd[DP_RECEIVER_CAP_SIZE];
	u8 psr_dpcd[EDP_PSR_RECEIVER_CAP_SIZE];
	u8 downstream_ports[DP_MAX_DOWNSTREAM_PORTS];
	u8 edp_dpcd[EDP_DISPLAY_CTL_CAP_SIZE];
	u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE];
	u8 lttpr_common_caps[DP_LTTPR_COMMON_CAP_SIZE];
	u8 lttpr_phy_caps[DP_MAX_LTTPR_COUNT][DP_LTTPR_PHY_CAP_SIZE];
	u8 fec_capable;
	u8 pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE];
	/* source rates */
	int num_source_rates;
	const int *source_rates;
	/* sink rates as reported by DP_MAX_LINK_RATE/DP_SUPPORTED_LINK_RATES */
	int num_sink_rates;
	int sink_rates[DP_MAX_SUPPORTED_RATES];
	bool use_rate_select;
	/* intersection of source and sink rates */
	int num_common_rates;
	int common_rates[DP_MAX_SUPPORTED_RATES];
	/* Max lane count for the current link */
	int max_link_lane_count;
	/* Max rate for the current link */
	int max_link_rate;
	int mso_link_count;
	int mso_pixel_overlap;
	/* sink or branch descriptor */
	struct drm_dp_desc desc;
	struct drm_dp_aux aux;
	u32 aux_busy_last_status;
	u8 train_set[4];

	struct intel_pps pps;

	bool can_mst; /* this port supports mst */
	bool is_mst;
	int active_mst_links;

	/* connector directly attached - won't be use for modeset in mst world */
	struct intel_connector *attached_connector;

	/* mst connector list */
	struct intel_dp_mst_encoder *mst_encoders[I915_MAX_PIPES];
	struct drm_dp_mst_topology_mgr mst_mgr;

	u32 (*get_aux_clock_divider)(struct intel_dp *dp, int index);
	/*
	 * This function returns the value we have to program the AUX_CTL
	 * register with to kick off an AUX transaction.
	 */
	u32 (*get_aux_send_ctl)(struct intel_dp *dp, int send_bytes,
				u32 aux_clock_divider);

	i915_reg_t (*aux_ch_ctl_reg)(struct intel_dp *dp);
	i915_reg_t (*aux_ch_data_reg)(struct intel_dp *dp, int index);

	/* This is called before a link training is starterd */
	void (*prepare_link_retrain)(struct intel_dp *intel_dp,
				     const struct intel_crtc_state *crtc_state);
	void (*set_link_train)(struct intel_dp *intel_dp,
			       const struct intel_crtc_state *crtc_state,
			       u8 dp_train_pat);
	void (*set_idle_link_train)(struct intel_dp *intel_dp,
				    const struct intel_crtc_state *crtc_state);
	void (*set_signal_levels)(struct intel_dp *intel_dp,
				  const struct intel_crtc_state *crtc_state);

	u8 (*preemph_max)(struct intel_dp *intel_dp);
	u8 (*voltage_max)(struct intel_dp *intel_dp,
			  const struct intel_crtc_state *crtc_state);

	/* Displayport compliance testing */
	struct intel_dp_compliance compliance;

	/* Downstream facing port caps */
	struct {
		int min_tmds_clock, max_tmds_clock;
		int max_dotclock;
		int pcon_max_frl_bw;
		u8 max_bpc;
		bool ycbcr_444_to_420;
		bool rgb_to_ycbcr;
	} dfp;

	/* To control wakeup latency, e.g. for irq-driven dp aux transfers. */
	struct pm_qos_request pm_qos;

	/* Display stream compression testing */
	bool force_dsc_en;

	bool hobl_failed;
	bool hobl_active;

	struct intel_dp_pcon_frl frl;

	struct intel_psr psr;
};

enum lspcon_vendor {
	LSPCON_VENDOR_MCA,
	LSPCON_VENDOR_PARADE
};

struct intel_lspcon {
	bool active;
	bool hdr_supported;
	enum drm_lspcon_mode mode;
	enum lspcon_vendor vendor;
};

struct intel_digital_port {
	struct intel_encoder base;
	u32 saved_port_bits;
	struct intel_dp dp;
	struct intel_hdmi hdmi;
	struct intel_lspcon lspcon;
	enum irqreturn (*hpd_pulse)(struct intel_digital_port *, bool);
	bool release_cl2_override;
	u8 max_lanes;
	/* Used for DP and ICL+ TypeC/DP and TypeC/HDMI ports. */
	enum aux_ch aux_ch;
	enum intel_display_power_domain ddi_io_power_domain;
	intel_wakeref_t ddi_io_wakeref;
	intel_wakeref_t aux_wakeref;
	struct mutex tc_lock;	/* protects the TypeC port mode */
	intel_wakeref_t tc_lock_wakeref;
	int tc_link_refcount;
	bool tc_legacy_port:1;
	char tc_port_name[8];
	enum tc_port_mode tc_mode;
	enum phy_fia tc_phy_fia;
	u8 tc_phy_fia_idx;

	/* protects num_hdcp_streams reference count, hdcp_port_data and hdcp_auth_status */
	struct mutex hdcp_mutex;
	/* the number of pipes using HDCP signalling out of this port */
	unsigned int num_hdcp_streams;
	/* port HDCP auth status */
	bool hdcp_auth_status;
	/* HDCP port data need to pass to security f/w */
	struct hdcp_port_data hdcp_port_data;

	void (*write_infoframe)(struct intel_encoder *encoder,
				const struct intel_crtc_state *crtc_state,
				unsigned int type,
				const void *frame, ssize_t len);
	void (*read_infoframe)(struct intel_encoder *encoder,
			       const struct intel_crtc_state *crtc_state,
			       unsigned int type,
			       void *frame, ssize_t len);
	void (*set_infoframes)(struct intel_encoder *encoder,
			       bool enable,
			       const struct intel_crtc_state *crtc_state,
			       const struct drm_connector_state *conn_state);
	u32 (*infoframes_enabled)(struct intel_encoder *encoder,
				  const struct intel_crtc_state *pipe_config);
	bool (*connected)(struct intel_encoder *encoder);
};

struct intel_dp_mst_encoder {
	struct intel_encoder base;
	enum pipe pipe;
	struct intel_digital_port *primary;
	struct intel_connector *connector;
};

static inline enum dpio_channel
vlv_dig_port_to_channel(struct intel_digital_port *dig_port)
{
	switch (dig_port->base.port) {
	case PORT_B:
	case PORT_D:
		return DPIO_CH0;
	case PORT_C:
		return DPIO_CH1;
	default:
		BUG();
	}
}

static inline enum dpio_phy
vlv_dig_port_to_phy(struct intel_digital_port *dig_port)
{
	switch (dig_port->base.port) {
	case PORT_B:
	case PORT_C:
		return DPIO_PHY0;
	case PORT_D:
		return DPIO_PHY1;
	default:
		BUG();
	}
}

static inline enum dpio_channel
vlv_pipe_to_channel(enum pipe pipe)
{
	switch (pipe) {
	case PIPE_A:
	case PIPE_C:
		return DPIO_CH0;
	case PIPE_B:
		return DPIO_CH1;
	default:
		BUG();
	}
}

static inline bool intel_pipe_valid(struct drm_i915_private *i915, enum pipe pipe)
{
	return (pipe >= 0 &&
		pipe < ARRAY_SIZE(i915->pipe_to_crtc_mapping) &&
		INTEL_INFO(i915)->pipe_mask & BIT(pipe) &&
		i915->pipe_to_crtc_mapping[pipe]);
}

static inline struct intel_crtc *
intel_get_first_crtc(struct drm_i915_private *dev_priv)
{
	return to_intel_crtc(drm_crtc_from_index(&dev_priv->drm, 0));
}

static inline struct intel_crtc *
intel_get_crtc_for_pipe(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	/* pipe_to_crtc_mapping may have hole on any of 3 display pipe system */
	drm_WARN_ON(&dev_priv->drm,
		    !(INTEL_INFO(dev_priv)->pipe_mask & BIT(pipe)));
	return dev_priv->pipe_to_crtc_mapping[pipe];
}

static inline struct intel_crtc *
intel_get_crtc_for_plane(struct drm_i915_private *dev_priv, enum i9xx_plane_id plane)
{
	return dev_priv->plane_to_crtc_mapping[plane];
}

struct intel_load_detect_pipe {
	struct drm_atomic_state *restore_state;
};

static inline struct intel_encoder *
intel_attached_encoder(struct intel_connector *connector)
{
	return connector->encoder;
}

static inline bool intel_encoder_is_dig_port(struct intel_encoder *encoder)
{
	switch (encoder->type) {
	case INTEL_OUTPUT_DDI:
	case INTEL_OUTPUT_DP:
	case INTEL_OUTPUT_EDP:
	case INTEL_OUTPUT_HDMI:
		return true;
	default:
		return false;
	}
}

static inline bool intel_encoder_is_mst(struct intel_encoder *encoder)
{
	return encoder->type == INTEL_OUTPUT_DP_MST;
}

static inline struct intel_dp_mst_encoder *
enc_to_mst(struct intel_encoder *encoder)
{
	return container_of(&encoder->base, struct intel_dp_mst_encoder,
			    base.base);
}

static inline struct intel_digital_port *
enc_to_dig_port(struct intel_encoder *encoder)
{
	struct intel_encoder *intel_encoder = encoder;

	if (intel_encoder_is_dig_port(intel_encoder))
		return container_of(&encoder->base, struct intel_digital_port,
				    base.base);
	else if (intel_encoder_is_mst(intel_encoder))
		return enc_to_mst(encoder)->primary;
	else
		return NULL;
}

static inline struct intel_digital_port *
intel_attached_dig_port(struct intel_connector *connector)
{
	return enc_to_dig_port(intel_attached_encoder(connector));
}

static inline struct intel_hdmi *
enc_to_intel_hdmi(struct intel_encoder *encoder)
{
	return &enc_to_dig_port(encoder)->hdmi;
}

static inline struct intel_hdmi *
intel_attached_hdmi(struct intel_connector *connector)
{
	return enc_to_intel_hdmi(intel_attached_encoder(connector));
}

static inline struct intel_dp *enc_to_intel_dp(struct intel_encoder *encoder)
{
	return &enc_to_dig_port(encoder)->dp;
}

static inline struct intel_dp *intel_attached_dp(struct intel_connector *connector)
{
	return enc_to_intel_dp(intel_attached_encoder(connector));
}

static inline bool intel_encoder_is_dp(struct intel_encoder *encoder)
{
	switch (encoder->type) {
	case INTEL_OUTPUT_DP:
	case INTEL_OUTPUT_EDP:
		return true;
	case INTEL_OUTPUT_DDI:
		/* Skip pure HDMI/DVI DDI encoders */
		return i915_mmio_reg_valid(enc_to_intel_dp(encoder)->output_reg);
	default:
		return false;
	}
}

static inline struct intel_lspcon *
enc_to_intel_lspcon(struct intel_encoder *encoder)
{
	return &enc_to_dig_port(encoder)->lspcon;
}

static inline struct intel_digital_port *
dp_to_dig_port(struct intel_dp *intel_dp)
{
	return container_of(intel_dp, struct intel_digital_port, dp);
}

static inline struct intel_lspcon *
dp_to_lspcon(struct intel_dp *intel_dp)
{
	return &dp_to_dig_port(intel_dp)->lspcon;
}

static inline struct drm_i915_private *
dp_to_i915(struct intel_dp *intel_dp)
{
	return to_i915(dp_to_dig_port(intel_dp)->base.base.dev);
}

#define CAN_PSR(intel_dp) ((intel_dp)->psr.sink_support && \
			   (intel_dp)->psr.source_support)

static inline bool intel_encoder_can_psr(struct intel_encoder *encoder)
{
	if (!intel_encoder_is_dp(encoder))
		return false;

	return CAN_PSR(enc_to_intel_dp(encoder));
}

static inline struct intel_digital_port *
hdmi_to_dig_port(struct intel_hdmi *intel_hdmi)
{
	return container_of(intel_hdmi, struct intel_digital_port, hdmi);
}

static inline struct intel_plane_state *
intel_atomic_get_plane_state(struct intel_atomic_state *state,
				 struct intel_plane *plane)
{
	struct drm_plane_state *ret =
		drm_atomic_get_plane_state(&state->base, &plane->base);

	if (IS_ERR(ret))
		return ERR_CAST(ret);

	return to_intel_plane_state(ret);
}

static inline struct intel_plane_state *
intel_atomic_get_old_plane_state(struct intel_atomic_state *state,
				 struct intel_plane *plane)
{
	return to_intel_plane_state(drm_atomic_get_old_plane_state(&state->base,
								   &plane->base));
}

static inline struct intel_plane_state *
intel_atomic_get_new_plane_state(struct intel_atomic_state *state,
				 struct intel_plane *plane)
{
	return to_intel_plane_state(drm_atomic_get_new_plane_state(&state->base,
								   &plane->base));
}

static inline struct intel_crtc_state *
intel_atomic_get_old_crtc_state(struct intel_atomic_state *state,
				struct intel_crtc *crtc)
{
	return to_intel_crtc_state(drm_atomic_get_old_crtc_state(&state->base,
								 &crtc->base));
}

static inline struct intel_crtc_state *
intel_atomic_get_new_crtc_state(struct intel_atomic_state *state,
				struct intel_crtc *crtc)
{
	return to_intel_crtc_state(drm_atomic_get_new_crtc_state(&state->base,
								 &crtc->base));
}

static inline struct intel_digital_connector_state *
intel_atomic_get_new_connector_state(struct intel_atomic_state *state,
				     struct intel_connector *connector)
{
	return to_intel_digital_connector_state(
			drm_atomic_get_new_connector_state(&state->base,
			&connector->base));
}

static inline struct intel_digital_connector_state *
intel_atomic_get_old_connector_state(struct intel_atomic_state *state,
				     struct intel_connector *connector)
{
	return to_intel_digital_connector_state(
			drm_atomic_get_old_connector_state(&state->base,
			&connector->base));
}

/* intel_display.c */
static inline bool
intel_crtc_has_type(const struct intel_crtc_state *crtc_state,
		    enum intel_output_type type)
{
	return crtc_state->output_types & (1 << type);
}
static inline bool
intel_crtc_has_dp_encoder(const struct intel_crtc_state *crtc_state)
{
	return crtc_state->output_types &
		((1 << INTEL_OUTPUT_DP) |
		 (1 << INTEL_OUTPUT_DP_MST) |
		 (1 << INTEL_OUTPUT_EDP));
}

static inline bool
intel_crtc_needs_modeset(const struct intel_crtc_state *crtc_state)
{
	return drm_atomic_crtc_needs_modeset(&crtc_state->uapi);
}

static inline void
intel_wait_for_vblank(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	struct intel_crtc *crtc = intel_get_crtc_for_pipe(dev_priv, pipe);

	drm_crtc_wait_one_vblank(&crtc->base);
}

static inline void
intel_wait_for_vblank_if_active(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	const struct intel_crtc *crtc = intel_get_crtc_for_pipe(dev_priv, pipe);

	if (crtc->active)
		intel_wait_for_vblank(dev_priv, pipe);
}

static inline bool intel_modifier_uses_dpt(struct drm_i915_private *i915, u64 modifier)
{
	return DISPLAY_VER(i915) >= 13 && modifier != DRM_FORMAT_MOD_LINEAR;
}

static inline bool intel_fb_uses_dpt(const struct drm_framebuffer *fb)
{
	return fb && intel_modifier_uses_dpt(to_i915(fb->dev), fb->modifier);
}

static inline u32 intel_plane_ggtt_offset(const struct intel_plane_state *plane_state)
{
	return i915_ggtt_offset(plane_state->ggtt_vma);
}

static inline struct intel_frontbuffer *
to_intel_frontbuffer(struct drm_framebuffer *fb)
{
	return fb ? to_intel_framebuffer(fb)->frontbuffer : NULL;
}

static inline bool intel_panel_use_ssc(struct drm_i915_private *dev_priv)
{
	if (dev_priv->params.panel_use_ssc >= 0)
		return dev_priv->params.panel_use_ssc != 0;
	return dev_priv->vbt.lvds_use_ssc
		&& !(dev_priv->quirks & QUIRK_LVDS_SSC_DISABLE);
}

static inline u32 i9xx_dpll_compute_fp(struct dpll *dpll)
{
	return dpll->n << 16 | dpll->m1 << 8 | dpll->m2;
}

static inline u32 intel_fdi_link_freq(struct drm_i915_private *dev_priv,
				      const struct intel_crtc_state *pipe_config)
{
	if (HAS_DDI(dev_priv))
		return pipe_config->port_clock; /* SPLL */
	else
		return dev_priv->fdi_pll_freq;
}

static inline bool is_ccs_modifier(u64 modifier)
{
	return modifier == I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS ||
	       modifier == I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC ||
	       modifier == I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS ||
	       modifier == I915_FORMAT_MOD_Y_TILED_CCS ||
	       modifier == I915_FORMAT_MOD_Yf_TILED_CCS;
}

static inline bool is_gen12_ccs_modifier(u64 modifier)
{
	return modifier == I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS ||
	       modifier == I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC ||
	       modifier == I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS;
}

#endif /*  __INTEL_DISPLAY_TYPES_H__ */

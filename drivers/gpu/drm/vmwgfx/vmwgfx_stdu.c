// SPDX-License-Identifier: GPL-2.0 OR MIT
/******************************************************************************
 *
 * COPYRIGHT (C) 2014-2015 VMware, Inc., Palo Alto, CA., USA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************/

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_damage_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_vblank.h>

#include "vmwgfx_kms.h"
#include "device_include/svga3d_surfacedefs.h"

#define vmw_crtc_to_stdu(x) \
	container_of(x, struct vmw_screen_target_display_unit, base.crtc)
#define vmw_encoder_to_stdu(x) \
	container_of(x, struct vmw_screen_target_display_unit, base.encoder)
#define vmw_connector_to_stdu(x) \
	container_of(x, struct vmw_screen_target_display_unit, base.connector)



enum stdu_content_type {
	SAME_AS_DISPLAY = 0,
	SEPARATE_SURFACE,
	SEPARATE_BO
};

/**
 * struct vmw_stdu_dirty - closure structure for the update functions
 *
 * @base: The base type we derive from. Used by vmw_kms_helper_dirty().
 * @transfer: Transfer direction for DMA command.
 * @left: Left side of bounding box.
 * @right: Right side of bounding box.
 * @top: Top side of bounding box.
 * @bottom: Bottom side of bounding box.
 * @fb_left: Left side of the framebuffer/content bounding box
 * @fb_top: Top of the framebuffer/content bounding box
 * @pitch: framebuffer pitch (stride)
 * @buf: buffer object when DMA-ing between buffer and screen targets.
 * @sid: Surface ID when copying between surface and screen targets.
 */
struct vmw_stdu_dirty {
	struct vmw_kms_dirty base;
	SVGA3dTransferType  transfer;
	s32 left, right, top, bottom;
	s32 fb_left, fb_top;
	u32 pitch;
	union {
		struct vmw_buffer_object *buf;
		u32 sid;
	};
};

/*
 * SVGA commands that are used by this code. Please see the device headers
 * for explanation.
 */
struct vmw_stdu_update {
	SVGA3dCmdHeader header;
	SVGA3dCmdUpdateGBScreenTarget body;
};

struct vmw_stdu_dma {
	SVGA3dCmdHeader     header;
	SVGA3dCmdSurfaceDMA body;
};

struct vmw_stdu_surface_copy {
	SVGA3dCmdHeader      header;
	SVGA3dCmdSurfaceCopy body;
};

struct vmw_stdu_update_gb_image {
	SVGA3dCmdHeader header;
	SVGA3dCmdUpdateGBImage body;
};

/**
 * struct vmw_screen_target_display_unit
 *
 * @base: VMW specific DU structure
 * @display_srf: surface to be displayed.  The dimension of this will always
 *               match the display mode.  If the display mode matches
 *               content_vfbs dimensions, then this is a pointer into the
 *               corresponding field in content_vfbs.  If not, then this
 *               is a separate buffer to which content_vfbs will blit to.
 * @content_fb_type: content_fb type
 * @display_width:  display width
 * @display_height: display height
 * @defined:     true if the current display unit has been initialized
 * @cpp:         Bytes per pixel
 */
struct vmw_screen_target_display_unit {
	struct vmw_display_unit base;
	struct vmw_surface *display_srf;
	enum stdu_content_type content_fb_type;
	s32 display_width, display_height;

	bool defined;

	/* For CPU Blit */
	unsigned int cpp;
};



static void vmw_stdu_destroy(struct vmw_screen_target_display_unit *stdu);



/******************************************************************************
 * Screen Target Display Unit CRTC Functions
 *****************************************************************************/


/**
 * vmw_stdu_crtc_destroy - cleans up the STDU
 *
 * @crtc: used to get a reference to the containing STDU
 */
static void vmw_stdu_crtc_destroy(struct drm_crtc *crtc)
{
	vmw_stdu_destroy(vmw_crtc_to_stdu(crtc));
}

/**
 * vmw_stdu_define_st - Defines a Screen Target
 *
 * @dev_priv:  VMW DRM device
 * @stdu: display unit to create a Screen Target for
 * @mode: The mode to set.
 * @crtc_x: X coordinate of screen target relative to framebuffer origin.
 * @crtc_y: Y coordinate of screen target relative to framebuffer origin.
 *
 * Creates a STDU that we can used later.  This function is called whenever the
 * framebuffer size changes.
 *
 * RETURNs:
 * 0 on success, error code on failure
 */
static int vmw_stdu_define_st(struct vmw_private *dev_priv,
			      struct vmw_screen_target_display_unit *stdu,
			      struct drm_display_mode *mode,
			      int crtc_x, int crtc_y)
{
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdDefineGBScreenTarget body;
	} *cmd;

	cmd = VMW_CMD_RESERVE(dev_priv, sizeof(*cmd));
	if (unlikely(cmd == NULL))
		return -ENOMEM;

	cmd->header.id   = SVGA_3D_CMD_DEFINE_GB_SCREENTARGET;
	cmd->header.size = sizeof(cmd->body);

	cmd->body.stid   = stdu->base.unit;
	cmd->body.width  = mode->hdisplay;
	cmd->body.height = mode->vdisplay;
	cmd->body.flags  = (0 == cmd->body.stid) ? SVGA_STFLAG_PRIMARY : 0;
	cmd->body.dpi    = 0;
	cmd->body.xRoot  = crtc_x;
	cmd->body.yRoot  = crtc_y;

	stdu->base.set_gui_x = cmd->body.xRoot;
	stdu->base.set_gui_y = cmd->body.yRoot;

	vmw_cmd_commit(dev_priv, sizeof(*cmd));

	stdu->defined = true;
	stdu->display_width  = mode->hdisplay;
	stdu->display_height = mode->vdisplay;

	return 0;
}



/**
 * vmw_stdu_bind_st - Binds a surface to a Screen Target
 *
 * @dev_priv: VMW DRM device
 * @stdu: display unit affected
 * @res: Buffer to bind to the screen target.  Set to NULL to blank screen.
 *
 * Binding a surface to a Screen Target the same as flipping
 */
static int vmw_stdu_bind_st(struct vmw_private *dev_priv,
			    struct vmw_screen_target_display_unit *stdu,
			    const struct vmw_resource *res)
{
	SVGA3dSurfaceImageId image;

	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdBindGBScreenTarget body;
	} *cmd;


	if (!stdu->defined) {
		DRM_ERROR("No screen target defined\n");
		return -EINVAL;
	}

	/* Set up image using information in vfb */
	memset(&image, 0, sizeof(image));
	image.sid = res ? res->id : SVGA3D_INVALID_ID;

	cmd = VMW_CMD_RESERVE(dev_priv, sizeof(*cmd));
	if (unlikely(cmd == NULL))
		return -ENOMEM;

	cmd->header.id   = SVGA_3D_CMD_BIND_GB_SCREENTARGET;
	cmd->header.size = sizeof(cmd->body);

	cmd->body.stid   = stdu->base.unit;
	cmd->body.image  = image;

	vmw_cmd_commit(dev_priv, sizeof(*cmd));

	return 0;
}

/**
 * vmw_stdu_populate_update - populate an UPDATE_GB_SCREENTARGET command with a
 * bounding box.
 *
 * @cmd: Pointer to command stream.
 * @unit: Screen target unit.
 * @left: Left side of bounding box.
 * @right: Right side of bounding box.
 * @top: Top side of bounding box.
 * @bottom: Bottom side of bounding box.
 */
static void vmw_stdu_populate_update(void *cmd, int unit,
				     s32 left, s32 right, s32 top, s32 bottom)
{
	struct vmw_stdu_update *update = cmd;

	update->header.id   = SVGA_3D_CMD_UPDATE_GB_SCREENTARGET;
	update->header.size = sizeof(update->body);

	update->body.stid   = unit;
	update->body.rect.x = left;
	update->body.rect.y = top;
	update->body.rect.w = right - left;
	update->body.rect.h = bottom - top;
}

/**
 * vmw_stdu_update_st - Full update of a Screen Target
 *
 * @dev_priv: VMW DRM device
 * @stdu: display unit affected
 *
 * This function needs to be called whenever the content of a screen
 * target has changed completely. Typically as a result of a backing
 * surface change.
 *
 * RETURNS:
 * 0 on success, error code on failure
 */
static int vmw_stdu_update_st(struct vmw_private *dev_priv,
			      struct vmw_screen_target_display_unit *stdu)
{
	struct vmw_stdu_update *cmd;

	if (!stdu->defined) {
		DRM_ERROR("No screen target defined");
		return -EINVAL;
	}

	cmd = VMW_CMD_RESERVE(dev_priv, sizeof(*cmd));
	if (unlikely(cmd == NULL))
		return -ENOMEM;

	vmw_stdu_populate_update(cmd, stdu->base.unit,
				 0, stdu->display_width,
				 0, stdu->display_height);

	vmw_cmd_commit(dev_priv, sizeof(*cmd));

	return 0;
}



/**
 * vmw_stdu_destroy_st - Destroy a Screen Target
 *
 * @dev_priv:  VMW DRM device
 * @stdu: display unit to destroy
 */
static int vmw_stdu_destroy_st(struct vmw_private *dev_priv,
			       struct vmw_screen_target_display_unit *stdu)
{
	int    ret;

	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdDestroyGBScreenTarget body;
	} *cmd;


	/* Nothing to do if not successfully defined */
	if (unlikely(!stdu->defined))
		return 0;

	cmd = VMW_CMD_RESERVE(dev_priv, sizeof(*cmd));
	if (unlikely(cmd == NULL))
		return -ENOMEM;

	cmd->header.id   = SVGA_3D_CMD_DESTROY_GB_SCREENTARGET;
	cmd->header.size = sizeof(cmd->body);

	cmd->body.stid   = stdu->base.unit;

	vmw_cmd_commit(dev_priv, sizeof(*cmd));

	/* Force sync */
	ret = vmw_fallback_wait(dev_priv, false, true, 0, false, 3*HZ);
	if (unlikely(ret != 0))
		DRM_ERROR("Failed to sync with HW");

	stdu->defined = false;
	stdu->display_width  = 0;
	stdu->display_height = 0;

	return ret;
}


/**
 * vmw_stdu_crtc_mode_set_nofb - Updates screen target size
 *
 * @crtc: CRTC associated with the screen target
 *
 * This function defines/destroys a screen target
 *
 */
static void vmw_stdu_crtc_mode_set_nofb(struct drm_crtc *crtc)
{
	struct vmw_private *dev_priv;
	struct vmw_screen_target_display_unit *stdu;
	struct drm_connector_state *conn_state;
	struct vmw_connector_state *vmw_conn_state;
	int x, y, ret;

	stdu = vmw_crtc_to_stdu(crtc);
	dev_priv = vmw_priv(crtc->dev);
	conn_state = stdu->base.connector.state;
	vmw_conn_state = vmw_connector_state_to_vcs(conn_state);

	if (stdu->defined) {
		ret = vmw_stdu_bind_st(dev_priv, stdu, NULL);
		if (ret)
			DRM_ERROR("Failed to blank CRTC\n");

		(void) vmw_stdu_update_st(dev_priv, stdu);

		ret = vmw_stdu_destroy_st(dev_priv, stdu);
		if (ret)
			DRM_ERROR("Failed to destroy Screen Target\n");

		stdu->content_fb_type = SAME_AS_DISPLAY;
	}

	if (!crtc->state->enable)
		return;

	x = vmw_conn_state->gui_x;
	y = vmw_conn_state->gui_y;

	vmw_svga_enable(dev_priv);
	ret = vmw_stdu_define_st(dev_priv, stdu, &crtc->mode, x, y);

	if (ret)
		DRM_ERROR("Failed to define Screen Target of size %dx%d\n",
			  crtc->x, crtc->y);
}


static void vmw_stdu_crtc_helper_prepare(struct drm_crtc *crtc)
{
}

static void vmw_stdu_crtc_atomic_enable(struct drm_crtc *crtc,
					struct drm_atomic_state *state)
{
}

static void vmw_stdu_crtc_atomic_disable(struct drm_crtc *crtc,
					 struct drm_atomic_state *state)
{
	struct vmw_private *dev_priv;
	struct vmw_screen_target_display_unit *stdu;
	int ret;


	if (!crtc) {
		DRM_ERROR("CRTC is NULL\n");
		return;
	}

	stdu     = vmw_crtc_to_stdu(crtc);
	dev_priv = vmw_priv(crtc->dev);

	if (stdu->defined) {
		ret = vmw_stdu_bind_st(dev_priv, stdu, NULL);
		if (ret)
			DRM_ERROR("Failed to blank CRTC\n");

		(void) vmw_stdu_update_st(dev_priv, stdu);

		ret = vmw_stdu_destroy_st(dev_priv, stdu);
		if (ret)
			DRM_ERROR("Failed to destroy Screen Target\n");

		stdu->content_fb_type = SAME_AS_DISPLAY;
	}
}

/**
 * vmw_stdu_bo_clip - Callback to encode a suface DMA command cliprect
 *
 * @dirty: The closure structure.
 *
 * Encodes a surface DMA command cliprect and updates the bounding box
 * for the DMA.
 */
static void vmw_stdu_bo_clip(struct vmw_kms_dirty *dirty)
{
	struct vmw_stdu_dirty *ddirty =
		container_of(dirty, struct vmw_stdu_dirty, base);
	struct vmw_stdu_dma *cmd = dirty->cmd;
	struct SVGA3dCopyBox *blit = (struct SVGA3dCopyBox *) &cmd[1];

	blit += dirty->num_hits;
	blit->srcx = dirty->fb_x;
	blit->srcy = dirty->fb_y;
	blit->x = dirty->unit_x1;
	blit->y = dirty->unit_y1;
	blit->d = 1;
	blit->w = dirty->unit_x2 - dirty->unit_x1;
	blit->h = dirty->unit_y2 - dirty->unit_y1;
	dirty->num_hits++;

	if (ddirty->transfer != SVGA3D_WRITE_HOST_VRAM)
		return;

	/* Destination bounding box */
	ddirty->left = min_t(s32, ddirty->left, dirty->unit_x1);
	ddirty->top = min_t(s32, ddirty->top, dirty->unit_y1);
	ddirty->right = max_t(s32, ddirty->right, dirty->unit_x2);
	ddirty->bottom = max_t(s32, ddirty->bottom, dirty->unit_y2);
}

/**
 * vmw_stdu_bo_fifo_commit - Callback to fill in and submit a DMA command.
 *
 * @dirty: The closure structure.
 *
 * Fills in the missing fields in a DMA command, and optionally encodes
 * a screen target update command, depending on transfer direction.
 */
static void vmw_stdu_bo_fifo_commit(struct vmw_kms_dirty *dirty)
{
	struct vmw_stdu_dirty *ddirty =
		container_of(dirty, struct vmw_stdu_dirty, base);
	struct vmw_screen_target_display_unit *stdu =
		container_of(dirty->unit, typeof(*stdu), base);
	struct vmw_stdu_dma *cmd = dirty->cmd;
	struct SVGA3dCopyBox *blit = (struct SVGA3dCopyBox *) &cmd[1];
	SVGA3dCmdSurfaceDMASuffix *suffix =
		(SVGA3dCmdSurfaceDMASuffix *) &blit[dirty->num_hits];
	size_t blit_size = sizeof(*blit) * dirty->num_hits + sizeof(*suffix);

	if (!dirty->num_hits) {
		vmw_cmd_commit(dirty->dev_priv, 0);
		return;
	}

	cmd->header.id = SVGA_3D_CMD_SURFACE_DMA;
	cmd->header.size = sizeof(cmd->body) + blit_size;
	vmw_bo_get_guest_ptr(&ddirty->buf->base, &cmd->body.guest.ptr);
	cmd->body.guest.pitch = ddirty->pitch;
	cmd->body.host.sid = stdu->display_srf->res.id;
	cmd->body.host.face = 0;
	cmd->body.host.mipmap = 0;
	cmd->body.transfer = ddirty->transfer;
	suffix->suffixSize = sizeof(*suffix);
	suffix->maximumOffset = ddirty->buf->base.base.size;

	if (ddirty->transfer == SVGA3D_WRITE_HOST_VRAM) {
		blit_size += sizeof(struct vmw_stdu_update);

		vmw_stdu_populate_update(&suffix[1], stdu->base.unit,
					 ddirty->left, ddirty->right,
					 ddirty->top, ddirty->bottom);
	}

	vmw_cmd_commit(dirty->dev_priv, sizeof(*cmd) + blit_size);

	stdu->display_srf->res.res_dirty = true;
	ddirty->left = ddirty->top = S32_MAX;
	ddirty->right = ddirty->bottom = S32_MIN;
}


/**
 * vmw_stdu_bo_cpu_clip - Callback to encode a CPU blit
 *
 * @dirty: The closure structure.
 *
 * This function calculates the bounding box for all the incoming clips.
 */
static void vmw_stdu_bo_cpu_clip(struct vmw_kms_dirty *dirty)
{
	struct vmw_stdu_dirty *ddirty =
		container_of(dirty, struct vmw_stdu_dirty, base);

	dirty->num_hits = 1;

	/* Calculate destination bounding box */
	ddirty->left = min_t(s32, ddirty->left, dirty->unit_x1);
	ddirty->top = min_t(s32, ddirty->top, dirty->unit_y1);
	ddirty->right = max_t(s32, ddirty->right, dirty->unit_x2);
	ddirty->bottom = max_t(s32, ddirty->bottom, dirty->unit_y2);

	/*
	 * Calculate content bounding box.  We only need the top-left
	 * coordinate because width and height will be the same as the
	 * destination bounding box above
	 */
	ddirty->fb_left = min_t(s32, ddirty->fb_left, dirty->fb_x);
	ddirty->fb_top  = min_t(s32, ddirty->fb_top, dirty->fb_y);
}


/**
 * vmw_stdu_bo_cpu_commit - Callback to do a CPU blit from buffer object
 *
 * @dirty: The closure structure.
 *
 * For the special case when we cannot create a proxy surface in a
 * 2D VM, we have to do a CPU blit ourselves.
 */
static void vmw_stdu_bo_cpu_commit(struct vmw_kms_dirty *dirty)
{
	struct vmw_stdu_dirty *ddirty =
		container_of(dirty, struct vmw_stdu_dirty, base);
	struct vmw_screen_target_display_unit *stdu =
		container_of(dirty->unit, typeof(*stdu), base);
	s32 width, height;
	s32 src_pitch, dst_pitch;
	struct ttm_buffer_object *src_bo, *dst_bo;
	u32 src_offset, dst_offset;
	struct vmw_diff_cpy diff = VMW_CPU_BLIT_DIFF_INITIALIZER(stdu->cpp);

	if (!dirty->num_hits)
		return;

	width = ddirty->right - ddirty->left;
	height = ddirty->bottom - ddirty->top;

	if (width == 0 || height == 0)
		return;

	/* Assume we are blitting from Guest (bo) to Host (display_srf) */
	dst_pitch = stdu->display_srf->metadata.base_size.width * stdu->cpp;
	dst_bo = &stdu->display_srf->res.backup->base;
	dst_offset = ddirty->top * dst_pitch + ddirty->left * stdu->cpp;

	src_pitch = ddirty->pitch;
	src_bo = &ddirty->buf->base;
	src_offset = ddirty->fb_top * src_pitch + ddirty->fb_left * stdu->cpp;

	/* Swap src and dst if the assumption was wrong. */
	if (ddirty->transfer != SVGA3D_WRITE_HOST_VRAM) {
		swap(dst_pitch, src_pitch);
		swap(dst_bo, src_bo);
		swap(src_offset, dst_offset);
	}

	(void) vmw_bo_cpu_blit(dst_bo, dst_offset, dst_pitch,
			       src_bo, src_offset, src_pitch,
			       width * stdu->cpp, height, &diff);

	if (ddirty->transfer == SVGA3D_WRITE_HOST_VRAM &&
	    drm_rect_visible(&diff.rect)) {
		struct vmw_private *dev_priv;
		struct vmw_stdu_update *cmd;
		struct drm_clip_rect region;
		int ret;

		/* We are updating the actual surface, not a proxy */
		region.x1 = diff.rect.x1;
		region.x2 = diff.rect.x2;
		region.y1 = diff.rect.y1;
		region.y2 = diff.rect.y2;
		ret = vmw_kms_update_proxy(&stdu->display_srf->res, &region,
					   1, 1);
		if (ret)
			goto out_cleanup;


		dev_priv = vmw_priv(stdu->base.crtc.dev);
		cmd = VMW_CMD_RESERVE(dev_priv, sizeof(*cmd));
		if (!cmd)
			goto out_cleanup;

		vmw_stdu_populate_update(cmd, stdu->base.unit,
					 region.x1, region.x2,
					 region.y1, region.y2);

		vmw_cmd_commit(dev_priv, sizeof(*cmd));
	}

out_cleanup:
	ddirty->left = ddirty->top = ddirty->fb_left = ddirty->fb_top = S32_MAX;
	ddirty->right = ddirty->bottom = S32_MIN;
}

/**
 * vmw_kms_stdu_dma - Perform a DMA transfer between a buffer-object backed
 * framebuffer and the screen target system.
 *
 * @dev_priv: Pointer to the device private structure.
 * @file_priv: Pointer to a struct drm-file identifying the caller. May be
 * set to NULL, but then @user_fence_rep must also be set to NULL.
 * @vfb: Pointer to the buffer-object backed framebuffer.
 * @user_fence_rep: User-space provided structure for fence information.
 * @clips: Array of clip rects. Either @clips or @vclips must be NULL.
 * @vclips: Alternate array of clip rects. Either @clips or @vclips must
 * be NULL.
 * @num_clips: Number of clip rects in @clips or @vclips.
 * @increment: Increment to use when looping over @clips or @vclips.
 * @to_surface: Whether to DMA to the screen target system as opposed to
 * from the screen target system.
 * @interruptible: Whether to perform waits interruptible if possible.
 * @crtc: If crtc is passed, perform stdu dma on that crtc only.
 *
 * If DMA-ing till the screen target system, the function will also notify
 * the screen target system that a bounding box of the cliprects has been
 * updated.
 * Returns 0 on success, negative error code on failure. -ERESTARTSYS if
 * interrupted.
 */
int vmw_kms_stdu_dma(struct vmw_private *dev_priv,
		     struct drm_file *file_priv,
		     struct vmw_framebuffer *vfb,
		     struct drm_vmw_fence_rep __user *user_fence_rep,
		     struct drm_clip_rect *clips,
		     struct drm_vmw_rect *vclips,
		     uint32_t num_clips,
		     int increment,
		     bool to_surface,
		     bool interruptible,
		     struct drm_crtc *crtc)
{
	struct vmw_buffer_object *buf =
		container_of(vfb, struct vmw_framebuffer_bo, base)->buffer;
	struct vmw_stdu_dirty ddirty;
	int ret;
	bool cpu_blit = !(dev_priv->capabilities & SVGA_CAP_3D);
	DECLARE_VAL_CONTEXT(val_ctx, NULL, 0);

	/*
	 * VMs without 3D support don't have the surface DMA command and
	 * we'll be using a CPU blit, and the framebuffer should be moved out
	 * of VRAM.
	 */
	ret = vmw_validation_add_bo(&val_ctx, buf, false, cpu_blit);
	if (ret)
		return ret;

	ret = vmw_validation_prepare(&val_ctx, NULL, interruptible);
	if (ret)
		goto out_unref;

	ddirty.transfer = (to_surface) ? SVGA3D_WRITE_HOST_VRAM :
		SVGA3D_READ_HOST_VRAM;
	ddirty.left = ddirty.top = S32_MAX;
	ddirty.right = ddirty.bottom = S32_MIN;
	ddirty.fb_left = ddirty.fb_top = S32_MAX;
	ddirty.pitch = vfb->base.pitches[0];
	ddirty.buf = buf;
	ddirty.base.fifo_commit = vmw_stdu_bo_fifo_commit;
	ddirty.base.clip = vmw_stdu_bo_clip;
	ddirty.base.fifo_reserve_size = sizeof(struct vmw_stdu_dma) +
		num_clips * sizeof(SVGA3dCopyBox) +
		sizeof(SVGA3dCmdSurfaceDMASuffix);
	if (to_surface)
		ddirty.base.fifo_reserve_size += sizeof(struct vmw_stdu_update);


	if (cpu_blit) {
		ddirty.base.fifo_commit = vmw_stdu_bo_cpu_commit;
		ddirty.base.clip = vmw_stdu_bo_cpu_clip;
		ddirty.base.fifo_reserve_size = 0;
	}

	ddirty.base.crtc = crtc;

	ret = vmw_kms_helper_dirty(dev_priv, vfb, clips, vclips,
				   0, 0, num_clips, increment, &ddirty.base);

	vmw_kms_helper_validation_finish(dev_priv, file_priv, &val_ctx, NULL,
					 user_fence_rep);
	return ret;

out_unref:
	vmw_validation_unref_lists(&val_ctx);
	return ret;
}

/**
 * vmw_kms_stdu_surface_clip - Callback to encode a surface copy command cliprect
 *
 * @dirty: The closure structure.
 *
 * Encodes a surface copy command cliprect and updates the bounding box
 * for the copy.
 */
static void vmw_kms_stdu_surface_clip(struct vmw_kms_dirty *dirty)
{
	struct vmw_stdu_dirty *sdirty =
		container_of(dirty, struct vmw_stdu_dirty, base);
	struct vmw_stdu_surface_copy *cmd = dirty->cmd;
	struct vmw_screen_target_display_unit *stdu =
		container_of(dirty->unit, typeof(*stdu), base);

	if (sdirty->sid != stdu->display_srf->res.id) {
		struct SVGA3dCopyBox *blit = (struct SVGA3dCopyBox *) &cmd[1];

		blit += dirty->num_hits;
		blit->srcx = dirty->fb_x;
		blit->srcy = dirty->fb_y;
		blit->x = dirty->unit_x1;
		blit->y = dirty->unit_y1;
		blit->d = 1;
		blit->w = dirty->unit_x2 - dirty->unit_x1;
		blit->h = dirty->unit_y2 - dirty->unit_y1;
	}

	dirty->num_hits++;

	/* Destination bounding box */
	sdirty->left = min_t(s32, sdirty->left, dirty->unit_x1);
	sdirty->top = min_t(s32, sdirty->top, dirty->unit_y1);
	sdirty->right = max_t(s32, sdirty->right, dirty->unit_x2);
	sdirty->bottom = max_t(s32, sdirty->bottom, dirty->unit_y2);
}

/**
 * vmw_kms_stdu_surface_fifo_commit - Callback to fill in and submit a surface
 * copy command.
 *
 * @dirty: The closure structure.
 *
 * Fills in the missing fields in a surface copy command, and encodes a screen
 * target update command.
 */
static void vmw_kms_stdu_surface_fifo_commit(struct vmw_kms_dirty *dirty)
{
	struct vmw_stdu_dirty *sdirty =
		container_of(dirty, struct vmw_stdu_dirty, base);
	struct vmw_screen_target_display_unit *stdu =
		container_of(dirty->unit, typeof(*stdu), base);
	struct vmw_stdu_surface_copy *cmd = dirty->cmd;
	struct vmw_stdu_update *update;
	size_t blit_size = sizeof(SVGA3dCopyBox) * dirty->num_hits;
	size_t commit_size;

	if (!dirty->num_hits) {
		vmw_cmd_commit(dirty->dev_priv, 0);
		return;
	}

	if (sdirty->sid != stdu->display_srf->res.id) {
		struct SVGA3dCopyBox *blit = (struct SVGA3dCopyBox *) &cmd[1];

		cmd->header.id = SVGA_3D_CMD_SURFACE_COPY;
		cmd->header.size = sizeof(cmd->body) + blit_size;
		cmd->body.src.sid = sdirty->sid;
		cmd->body.dest.sid = stdu->display_srf->res.id;
		update = (struct vmw_stdu_update *) &blit[dirty->num_hits];
		commit_size = sizeof(*cmd) + blit_size + sizeof(*update);
		stdu->display_srf->res.res_dirty = true;
	} else {
		update = dirty->cmd;
		commit_size = sizeof(*update);
	}

	vmw_stdu_populate_update(update, stdu->base.unit, sdirty->left,
				 sdirty->right, sdirty->top, sdirty->bottom);

	vmw_cmd_commit(dirty->dev_priv, commit_size);

	sdirty->left = sdirty->top = S32_MAX;
	sdirty->right = sdirty->bottom = S32_MIN;
}

/**
 * vmw_kms_stdu_surface_dirty - Dirty part of a surface backed framebuffer
 *
 * @dev_priv: Pointer to the device private structure.
 * @framebuffer: Pointer to the surface-buffer backed framebuffer.
 * @clips: Array of clip rects. Either @clips or @vclips must be NULL.
 * @vclips: Alternate array of clip rects. Either @clips or @vclips must
 * be NULL.
 * @srf: Pointer to surface to blit from. If NULL, the surface attached
 * to @framebuffer will be used.
 * @dest_x: X coordinate offset to align @srf with framebuffer coordinates.
 * @dest_y: Y coordinate offset to align @srf with framebuffer coordinates.
 * @num_clips: Number of clip rects in @clips.
 * @inc: Increment to use when looping over @clips.
 * @out_fence: If non-NULL, will return a ref-counted pointer to a
 * struct vmw_fence_obj. The returned fence pointer may be NULL in which
 * case the device has already synchronized.
 * @crtc: If crtc is passed, perform surface dirty on that crtc only.
 *
 * Returns 0 on success, negative error code on failure. -ERESTARTSYS if
 * interrupted.
 */
int vmw_kms_stdu_surface_dirty(struct vmw_private *dev_priv,
			       struct vmw_framebuffer *framebuffer,
			       struct drm_clip_rect *clips,
			       struct drm_vmw_rect *vclips,
			       struct vmw_resource *srf,
			       s32 dest_x,
			       s32 dest_y,
			       unsigned num_clips, int inc,
			       struct vmw_fence_obj **out_fence,
			       struct drm_crtc *crtc)
{
	struct vmw_framebuffer_surface *vfbs =
		container_of(framebuffer, typeof(*vfbs), base);
	struct vmw_stdu_dirty sdirty;
	DECLARE_VAL_CONTEXT(val_ctx, NULL, 0);
	int ret;

	if (!srf)
		srf = &vfbs->surface->res;

	ret = vmw_validation_add_resource(&val_ctx, srf, 0, VMW_RES_DIRTY_NONE,
					  NULL, NULL);
	if (ret)
		return ret;

	ret = vmw_validation_prepare(&val_ctx, &dev_priv->cmdbuf_mutex, true);
	if (ret)
		goto out_unref;

	if (vfbs->is_bo_proxy) {
		ret = vmw_kms_update_proxy(srf, clips, num_clips, inc);
		if (ret)
			goto out_finish;
	}

	sdirty.base.fifo_commit = vmw_kms_stdu_surface_fifo_commit;
	sdirty.base.clip = vmw_kms_stdu_surface_clip;
	sdirty.base.fifo_reserve_size = sizeof(struct vmw_stdu_surface_copy) +
		sizeof(SVGA3dCopyBox) * num_clips +
		sizeof(struct vmw_stdu_update);
	sdirty.base.crtc = crtc;
	sdirty.sid = srf->id;
	sdirty.left = sdirty.top = S32_MAX;
	sdirty.right = sdirty.bottom = S32_MIN;

	ret = vmw_kms_helper_dirty(dev_priv, framebuffer, clips, vclips,
				   dest_x, dest_y, num_clips, inc,
				   &sdirty.base);
out_finish:
	vmw_kms_helper_validation_finish(dev_priv, NULL, &val_ctx, out_fence,
					 NULL);

	return ret;

out_unref:
	vmw_validation_unref_lists(&val_ctx);
	return ret;
}


/*
 *  Screen Target CRTC dispatch table
 */
static const struct drm_crtc_funcs vmw_stdu_crtc_funcs = {
	.gamma_set = vmw_du_crtc_gamma_set,
	.destroy = vmw_stdu_crtc_destroy,
	.reset = vmw_du_crtc_reset,
	.atomic_duplicate_state = vmw_du_crtc_duplicate_state,
	.atomic_destroy_state = vmw_du_crtc_destroy_state,
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.get_vblank_counter = vmw_get_vblank_counter,
	.enable_vblank = vmw_enable_vblank,
	.disable_vblank = vmw_disable_vblank,
};



/******************************************************************************
 * Screen Target Display Unit Encoder Functions
 *****************************************************************************/

/**
 * vmw_stdu_encoder_destroy - cleans up the STDU
 *
 * @encoder: used the get the containing STDU
 *
 * vmwgfx cleans up crtc/encoder/connector all at the same time so technically
 * this can be a no-op.  Nevertheless, it doesn't hurt of have this in case
 * the common KMS code changes and somehow vmw_stdu_crtc_destroy() doesn't
 * get called.
 */
static void vmw_stdu_encoder_destroy(struct drm_encoder *encoder)
{
	vmw_stdu_destroy(vmw_encoder_to_stdu(encoder));
}

static const struct drm_encoder_funcs vmw_stdu_encoder_funcs = {
	.destroy = vmw_stdu_encoder_destroy,
};



/******************************************************************************
 * Screen Target Display Unit Connector Functions
 *****************************************************************************/

/**
 * vmw_stdu_connector_destroy - cleans up the STDU
 *
 * @connector: used to get the containing STDU
 *
 * vmwgfx cleans up crtc/encoder/connector all at the same time so technically
 * this can be a no-op.  Nevertheless, it doesn't hurt of have this in case
 * the common KMS code changes and somehow vmw_stdu_crtc_destroy() doesn't
 * get called.
 */
static void vmw_stdu_connector_destroy(struct drm_connector *connector)
{
	vmw_stdu_destroy(vmw_connector_to_stdu(connector));
}



static const struct drm_connector_funcs vmw_stdu_connector_funcs = {
	.dpms = vmw_du_connector_dpms,
	.detect = vmw_du_connector_detect,
	.fill_modes = vmw_du_connector_fill_modes,
	.destroy = vmw_stdu_connector_destroy,
	.reset = vmw_du_connector_reset,
	.atomic_duplicate_state = vmw_du_connector_duplicate_state,
	.atomic_destroy_state = vmw_du_connector_destroy_state,
};


static const struct
drm_connector_helper_funcs vmw_stdu_connector_helper_funcs = {
};



/******************************************************************************
 * Screen Target Display Plane Functions
 *****************************************************************************/



/**
 * vmw_stdu_primary_plane_cleanup_fb - Unpins the display surface
 *
 * @plane:  display plane
 * @old_state: Contains the FB to clean up
 *
 * Unpins the display surface
 *
 * Returns 0 on success
 */
static void
vmw_stdu_primary_plane_cleanup_fb(struct drm_plane *plane,
				  struct drm_plane_state *old_state)
{
	struct vmw_plane_state *vps = vmw_plane_state_to_vps(old_state);

	if (vps->surf)
		WARN_ON(!vps->pinned);

	vmw_du_plane_cleanup_fb(plane, old_state);

	vps->content_fb_type = SAME_AS_DISPLAY;
	vps->cpp = 0;
}



/**
 * vmw_stdu_primary_plane_prepare_fb - Readies the display surface
 *
 * @plane:  display plane
 * @new_state: info on the new plane state, including the FB
 *
 * This function allocates a new display surface if the content is
 * backed by a buffer object.  The display surface is pinned here, and it'll
 * be unpinned in .cleanup_fb()
 *
 * Returns 0 on success
 */
static int
vmw_stdu_primary_plane_prepare_fb(struct drm_plane *plane,
				  struct drm_plane_state *new_state)
{
	struct vmw_private *dev_priv = vmw_priv(plane->dev);
	struct drm_framebuffer *new_fb = new_state->fb;
	struct vmw_framebuffer *vfb;
	struct vmw_plane_state *vps = vmw_plane_state_to_vps(new_state);
	enum stdu_content_type new_content_type;
	struct vmw_framebuffer_surface *new_vfbs;
	uint32_t hdisplay = new_state->crtc_w, vdisplay = new_state->crtc_h;
	int ret;

	/* No FB to prepare */
	if (!new_fb) {
		if (vps->surf) {
			WARN_ON(vps->pinned != 0);
			vmw_surface_unreference(&vps->surf);
		}

		return 0;
	}

	vfb = vmw_framebuffer_to_vfb(new_fb);
	new_vfbs = (vfb->bo) ? NULL : vmw_framebuffer_to_vfbs(new_fb);

	if (new_vfbs &&
	    new_vfbs->surface->metadata.base_size.width == hdisplay &&
	    new_vfbs->surface->metadata.base_size.height == vdisplay)
		new_content_type = SAME_AS_DISPLAY;
	else if (vfb->bo)
		new_content_type = SEPARATE_BO;
	else
		new_content_type = SEPARATE_SURFACE;

	if (new_content_type != SAME_AS_DISPLAY) {
		struct vmw_surface_metadata metadata = {0};

		/*
		 * If content buffer is a buffer object, then we have to
		 * construct surface info
		 */
		if (new_content_type == SEPARATE_BO) {

			switch (new_fb->format->cpp[0]*8) {
			case 32:
				metadata.format = SVGA3D_X8R8G8B8;
				break;

			case 16:
				metadata.format = SVGA3D_R5G6B5;
				break;

			case 8:
				metadata.format = SVGA3D_P8;
				break;

			default:
				DRM_ERROR("Invalid format\n");
				return -EINVAL;
			}

			metadata.mip_levels[0] = 1;
			metadata.num_sizes = 1;
			metadata.scanout = true;
		} else {
			metadata = new_vfbs->surface->metadata;
		}

		metadata.base_size.width = hdisplay;
		metadata.base_size.height = vdisplay;
		metadata.base_size.depth = 1;

		if (vps->surf) {
			struct drm_vmw_size cur_base_size =
				vps->surf->metadata.base_size;

			if (cur_base_size.width != metadata.base_size.width ||
			    cur_base_size.height != metadata.base_size.height ||
			    vps->surf->metadata.format != metadata.format) {
				WARN_ON(vps->pinned != 0);
				vmw_surface_unreference(&vps->surf);
			}

		}

		if (!vps->surf) {
			ret = vmw_gb_surface_define(dev_priv, 0, &metadata,
						    &vps->surf);
			if (ret != 0) {
				DRM_ERROR("Couldn't allocate STDU surface.\n");
				return ret;
			}
		}
	} else {
		/*
		 * prepare_fb and clean_fb should only take care of pinning
		 * and unpinning.  References are tracked by state objects.
		 * The only time we add a reference in prepare_fb is if the
		 * state object doesn't have a reference to begin with
		 */
		if (vps->surf) {
			WARN_ON(vps->pinned != 0);
			vmw_surface_unreference(&vps->surf);
		}

		vps->surf = vmw_surface_reference(new_vfbs->surface);
	}

	if (vps->surf) {

		/* Pin new surface before flipping */
		ret = vmw_resource_pin(&vps->surf->res, false);
		if (ret)
			goto out_srf_unref;

		vps->pinned++;
	}

	vps->content_fb_type = new_content_type;

	/*
	 * This should only happen if the buffer object is too large to create a
	 * proxy surface for.
	 * If we are a 2D VM with a buffer object then we have to use CPU blit
	 * so cache these mappings
	 */
	if (vps->content_fb_type == SEPARATE_BO &&
	    !(dev_priv->capabilities & SVGA_CAP_3D))
		vps->cpp = new_fb->pitches[0] / new_fb->width;

	return 0;

out_srf_unref:
	vmw_surface_unreference(&vps->surf);
	return ret;
}

static uint32_t vmw_stdu_bo_fifo_size(struct vmw_du_update_plane *update,
				      uint32_t num_hits)
{
	return sizeof(struct vmw_stdu_dma) + sizeof(SVGA3dCopyBox) * num_hits +
		sizeof(SVGA3dCmdSurfaceDMASuffix) +
		sizeof(struct vmw_stdu_update);
}

static uint32_t vmw_stdu_bo_fifo_size_cpu(struct vmw_du_update_plane *update,
					  uint32_t num_hits)
{
	return sizeof(struct vmw_stdu_update_gb_image) +
		sizeof(struct vmw_stdu_update);
}

static uint32_t vmw_stdu_bo_populate_dma(struct vmw_du_update_plane  *update,
					 void *cmd, uint32_t num_hits)
{
	struct vmw_screen_target_display_unit *stdu;
	struct vmw_framebuffer_bo *vfbbo;
	struct vmw_stdu_dma *cmd_dma = cmd;

	stdu = container_of(update->du, typeof(*stdu), base);
	vfbbo = container_of(update->vfb, typeof(*vfbbo), base);

	cmd_dma->header.id = SVGA_3D_CMD_SURFACE_DMA;
	cmd_dma->header.size = sizeof(cmd_dma->body) +
		sizeof(struct SVGA3dCopyBox) * num_hits +
		sizeof(SVGA3dCmdSurfaceDMASuffix);
	vmw_bo_get_guest_ptr(&vfbbo->buffer->base, &cmd_dma->body.guest.ptr);
	cmd_dma->body.guest.pitch = update->vfb->base.pitches[0];
	cmd_dma->body.host.sid = stdu->display_srf->res.id;
	cmd_dma->body.host.face = 0;
	cmd_dma->body.host.mipmap = 0;
	cmd_dma->body.transfer = SVGA3D_WRITE_HOST_VRAM;

	return sizeof(*cmd_dma);
}

static uint32_t vmw_stdu_bo_populate_clip(struct vmw_du_update_plane  *update,
					  void *cmd, struct drm_rect *clip,
					  uint32_t fb_x, uint32_t fb_y)
{
	struct SVGA3dCopyBox *box = cmd;

	box->srcx = fb_x;
	box->srcy = fb_y;
	box->srcz = 0;
	box->x = clip->x1;
	box->y = clip->y1;
	box->z = 0;
	box->w = drm_rect_width(clip);
	box->h = drm_rect_height(clip);
	box->d = 1;

	return sizeof(*box);
}

static uint32_t vmw_stdu_bo_populate_update(struct vmw_du_update_plane  *update,
					    void *cmd, struct drm_rect *bb)
{
	struct vmw_screen_target_display_unit *stdu;
	struct vmw_framebuffer_bo *vfbbo;
	SVGA3dCmdSurfaceDMASuffix *suffix = cmd;

	stdu = container_of(update->du, typeof(*stdu), base);
	vfbbo = container_of(update->vfb, typeof(*vfbbo), base);

	suffix->suffixSize = sizeof(*suffix);
	suffix->maximumOffset = vfbbo->buffer->base.base.size;

	vmw_stdu_populate_update(&suffix[1], stdu->base.unit, bb->x1, bb->x2,
				 bb->y1, bb->y2);

	return sizeof(*suffix) + sizeof(struct vmw_stdu_update);
}

static uint32_t vmw_stdu_bo_pre_clip_cpu(struct vmw_du_update_plane  *update,
					 void *cmd, uint32_t num_hits)
{
	struct vmw_du_update_plane_buffer *bo_update =
		container_of(update, typeof(*bo_update), base);

	bo_update->fb_left = INT_MAX;
	bo_update->fb_top = INT_MAX;

	return 0;
}

static uint32_t vmw_stdu_bo_clip_cpu(struct vmw_du_update_plane  *update,
				     void *cmd, struct drm_rect *clip,
				     uint32_t fb_x, uint32_t fb_y)
{
	struct vmw_du_update_plane_buffer *bo_update =
		container_of(update, typeof(*bo_update), base);

	bo_update->fb_left = min_t(int, bo_update->fb_left, fb_x);
	bo_update->fb_top = min_t(int, bo_update->fb_top, fb_y);

	return 0;
}

static uint32_t
vmw_stdu_bo_populate_update_cpu(struct vmw_du_update_plane  *update, void *cmd,
				struct drm_rect *bb)
{
	struct vmw_du_update_plane_buffer *bo_update;
	struct vmw_screen_target_display_unit *stdu;
	struct vmw_framebuffer_bo *vfbbo;
	struct vmw_diff_cpy diff = VMW_CPU_BLIT_DIFF_INITIALIZER(0);
	struct vmw_stdu_update_gb_image *cmd_img = cmd;
	struct vmw_stdu_update *cmd_update;
	struct ttm_buffer_object *src_bo, *dst_bo;
	u32 src_offset, dst_offset;
	s32 src_pitch, dst_pitch;
	s32 width, height;

	bo_update = container_of(update, typeof(*bo_update), base);
	stdu = container_of(update->du, typeof(*stdu), base);
	vfbbo = container_of(update->vfb, typeof(*vfbbo), base);

	width = bb->x2 - bb->x1;
	height = bb->y2 - bb->y1;

	diff.cpp = stdu->cpp;

	dst_bo = &stdu->display_srf->res.backup->base;
	dst_pitch = stdu->display_srf->metadata.base_size.width * stdu->cpp;
	dst_offset = bb->y1 * dst_pitch + bb->x1 * stdu->cpp;

	src_bo = &vfbbo->buffer->base;
	src_pitch = update->vfb->base.pitches[0];
	src_offset = bo_update->fb_top * src_pitch + bo_update->fb_left *
		stdu->cpp;

	(void) vmw_bo_cpu_blit(dst_bo, dst_offset, dst_pitch, src_bo,
			       src_offset, src_pitch, width * stdu->cpp, height,
			       &diff);

	if (drm_rect_visible(&diff.rect)) {
		SVGA3dBox *box = &cmd_img->body.box;

		cmd_img->header.id = SVGA_3D_CMD_UPDATE_GB_IMAGE;
		cmd_img->header.size = sizeof(cmd_img->body);
		cmd_img->body.image.sid = stdu->display_srf->res.id;
		cmd_img->body.image.face = 0;
		cmd_img->body.image.mipmap = 0;

		box->x = diff.rect.x1;
		box->y = diff.rect.y1;
		box->z = 0;
		box->w = drm_rect_width(&diff.rect);
		box->h = drm_rect_height(&diff.rect);
		box->d = 1;

		cmd_update = (struct vmw_stdu_update *)&cmd_img[1];
		vmw_stdu_populate_update(cmd_update, stdu->base.unit,
					 diff.rect.x1, diff.rect.x2,
					 diff.rect.y1, diff.rect.y2);

		return sizeof(*cmd_img) + sizeof(*cmd_update);
	}

	return 0;
}

/**
 * vmw_stdu_plane_update_bo - Update display unit for bo backed fb.
 * @dev_priv: device private.
 * @plane: plane state.
 * @old_state: old plane state.
 * @vfb: framebuffer which is blitted to display unit.
 * @out_fence: If non-NULL, will return a ref-counted pointer to vmw_fence_obj.
 *             The returned fence pointer may be NULL in which case the device
 *             has already synchronized.
 *
 * Return: 0 on success or a negative error code on failure.
 */
static int vmw_stdu_plane_update_bo(struct vmw_private *dev_priv,
				    struct drm_plane *plane,
				    struct drm_plane_state *old_state,
				    struct vmw_framebuffer *vfb,
				    struct vmw_fence_obj **out_fence)
{
	struct vmw_du_update_plane_buffer bo_update;

	memset(&bo_update, 0, sizeof(struct vmw_du_update_plane_buffer));
	bo_update.base.plane = plane;
	bo_update.base.old_state = old_state;
	bo_update.base.dev_priv = dev_priv;
	bo_update.base.du = vmw_crtc_to_du(plane->state->crtc);
	bo_update.base.vfb = vfb;
	bo_update.base.out_fence = out_fence;
	bo_update.base.mutex = NULL;
	bo_update.base.cpu_blit = !(dev_priv->capabilities & SVGA_CAP_3D);
	bo_update.base.intr = false;

	/*
	 * VM without 3D support don't have surface DMA command and framebuffer
	 * should be moved out of VRAM.
	 */
	if (bo_update.base.cpu_blit) {
		bo_update.base.calc_fifo_size = vmw_stdu_bo_fifo_size_cpu;
		bo_update.base.pre_clip = vmw_stdu_bo_pre_clip_cpu;
		bo_update.base.clip = vmw_stdu_bo_clip_cpu;
		bo_update.base.post_clip = vmw_stdu_bo_populate_update_cpu;
	} else {
		bo_update.base.calc_fifo_size = vmw_stdu_bo_fifo_size;
		bo_update.base.pre_clip = vmw_stdu_bo_populate_dma;
		bo_update.base.clip = vmw_stdu_bo_populate_clip;
		bo_update.base.post_clip = vmw_stdu_bo_populate_update;
	}

	return vmw_du_helper_plane_update(&bo_update.base);
}

static uint32_t
vmw_stdu_surface_fifo_size_same_display(struct vmw_du_update_plane *update,
					uint32_t num_hits)
{
	struct vmw_framebuffer_surface *vfbs;
	uint32_t size = 0;

	vfbs = container_of(update->vfb, typeof(*vfbs), base);

	if (vfbs->is_bo_proxy)
		size += sizeof(struct vmw_stdu_update_gb_image) * num_hits;

	size += sizeof(struct vmw_stdu_update);

	return size;
}

static uint32_t vmw_stdu_surface_fifo_size(struct vmw_du_update_plane *update,
					   uint32_t num_hits)
{
	struct vmw_framebuffer_surface *vfbs;
	uint32_t size = 0;

	vfbs = container_of(update->vfb, typeof(*vfbs), base);

	if (vfbs->is_bo_proxy)
		size += sizeof(struct vmw_stdu_update_gb_image) * num_hits;

	size += sizeof(struct vmw_stdu_surface_copy) + sizeof(SVGA3dCopyBox) *
		num_hits + sizeof(struct vmw_stdu_update);

	return size;
}

static uint32_t
vmw_stdu_surface_update_proxy(struct vmw_du_update_plane *update, void *cmd)
{
	struct vmw_framebuffer_surface *vfbs;
	struct drm_plane_state *state = update->plane->state;
	struct drm_plane_state *old_state = update->old_state;
	struct vmw_stdu_update_gb_image *cmd_update = cmd;
	struct drm_atomic_helper_damage_iter iter;
	struct drm_rect clip;
	uint32_t copy_size = 0;

	vfbs = container_of(update->vfb, typeof(*vfbs), base);

	/*
	 * proxy surface is special where a buffer object type fb is wrapped
	 * in a surface and need an update gb image command to sync with device.
	 */
	drm_atomic_helper_damage_iter_init(&iter, old_state, state);
	drm_atomic_for_each_plane_damage(&iter, &clip) {
		SVGA3dBox *box = &cmd_update->body.box;

		cmd_update->header.id = SVGA_3D_CMD_UPDATE_GB_IMAGE;
		cmd_update->header.size = sizeof(cmd_update->body);
		cmd_update->body.image.sid = vfbs->surface->res.id;
		cmd_update->body.image.face = 0;
		cmd_update->body.image.mipmap = 0;

		box->x = clip.x1;
		box->y = clip.y1;
		box->z = 0;
		box->w = drm_rect_width(&clip);
		box->h = drm_rect_height(&clip);
		box->d = 1;

		copy_size += sizeof(*cmd_update);
		cmd_update++;
	}

	return copy_size;
}

static uint32_t
vmw_stdu_surface_populate_copy(struct vmw_du_update_plane  *update, void *cmd,
			       uint32_t num_hits)
{
	struct vmw_screen_target_display_unit *stdu;
	struct vmw_framebuffer_surface *vfbs;
	struct vmw_stdu_surface_copy *cmd_copy = cmd;

	stdu = container_of(update->du, typeof(*stdu), base);
	vfbs = container_of(update->vfb, typeof(*vfbs), base);

	cmd_copy->header.id = SVGA_3D_CMD_SURFACE_COPY;
	cmd_copy->header.size = sizeof(cmd_copy->body) + sizeof(SVGA3dCopyBox) *
		num_hits;
	cmd_copy->body.src.sid = vfbs->surface->res.id;
	cmd_copy->body.dest.sid = stdu->display_srf->res.id;

	return sizeof(*cmd_copy);
}

static uint32_t
vmw_stdu_surface_populate_clip(struct vmw_du_update_plane  *update, void *cmd,
			       struct drm_rect *clip, uint32_t fb_x,
			       uint32_t fb_y)
{
	struct SVGA3dCopyBox *box = cmd;

	box->srcx = fb_x;
	box->srcy = fb_y;
	box->srcz = 0;
	box->x = clip->x1;
	box->y = clip->y1;
	box->z = 0;
	box->w = drm_rect_width(clip);
	box->h = drm_rect_height(clip);
	box->d = 1;

	return sizeof(*box);
}

static uint32_t
vmw_stdu_surface_populate_update(struct vmw_du_update_plane  *update, void *cmd,
				 struct drm_rect *bb)
{
	vmw_stdu_populate_update(cmd, update->du->unit, bb->x1, bb->x2, bb->y1,
				 bb->y2);

	return sizeof(struct vmw_stdu_update);
}

/**
 * vmw_stdu_plane_update_surface - Update display unit for surface backed fb
 * @dev_priv: Device private
 * @plane: Plane state
 * @old_state: Old plane state
 * @vfb: Framebuffer which is blitted to display unit
 * @out_fence: If non-NULL, will return a ref-counted pointer to vmw_fence_obj.
 *             The returned fence pointer may be NULL in which case the device
 *             has already synchronized.
 *
 * Return: 0 on success or a negative error code on failure.
 */
static int vmw_stdu_plane_update_surface(struct vmw_private *dev_priv,
					 struct drm_plane *plane,
					 struct drm_plane_state *old_state,
					 struct vmw_framebuffer *vfb,
					 struct vmw_fence_obj **out_fence)
{
	struct vmw_du_update_plane srf_update;
	struct vmw_screen_target_display_unit *stdu;
	struct vmw_framebuffer_surface *vfbs;

	stdu = vmw_crtc_to_stdu(plane->state->crtc);
	vfbs = container_of(vfb, typeof(*vfbs), base);

	memset(&srf_update, 0, sizeof(struct vmw_du_update_plane));
	srf_update.plane = plane;
	srf_update.old_state = old_state;
	srf_update.dev_priv = dev_priv;
	srf_update.du = vmw_crtc_to_du(plane->state->crtc);
	srf_update.vfb = vfb;
	srf_update.out_fence = out_fence;
	srf_update.mutex = &dev_priv->cmdbuf_mutex;
	srf_update.cpu_blit = false;
	srf_update.intr = true;

	if (vfbs->is_bo_proxy)
		srf_update.post_prepare = vmw_stdu_surface_update_proxy;

	if (vfbs->surface->res.id != stdu->display_srf->res.id) {
		srf_update.calc_fifo_size = vmw_stdu_surface_fifo_size;
		srf_update.pre_clip = vmw_stdu_surface_populate_copy;
		srf_update.clip = vmw_stdu_surface_populate_clip;
	} else {
		srf_update.calc_fifo_size =
			vmw_stdu_surface_fifo_size_same_display;
	}

	srf_update.post_clip = vmw_stdu_surface_populate_update;

	return vmw_du_helper_plane_update(&srf_update);
}

/**
 * vmw_stdu_primary_plane_atomic_update - formally switches STDU to new plane
 * @plane: display plane
 * @state: Only used to get crtc info
 *
 * Formally update stdu->display_srf to the new plane, and bind the new
 * plane STDU.  This function is called during the commit phase when
 * all the preparation have been done and all the configurations have
 * been checked.
 */
static void
vmw_stdu_primary_plane_atomic_update(struct drm_plane *plane,
				     struct drm_atomic_state *state)
{
	struct drm_plane_state *old_state = drm_atomic_get_old_plane_state(state, plane);
	struct drm_plane_state *new_state = drm_atomic_get_new_plane_state(state, plane);
	struct vmw_plane_state *vps = vmw_plane_state_to_vps(new_state);
	struct drm_crtc *crtc = new_state->crtc;
	struct vmw_screen_target_display_unit *stdu;
	struct drm_pending_vblank_event *event;
	struct vmw_fence_obj *fence = NULL;
	struct vmw_private *dev_priv;
	int ret;

	/* If case of device error, maintain consistent atomic state */
	if (crtc && new_state->fb) {
		struct vmw_framebuffer *vfb =
			vmw_framebuffer_to_vfb(new_state->fb);
		stdu = vmw_crtc_to_stdu(crtc);
		dev_priv = vmw_priv(crtc->dev);

		stdu->display_srf = vps->surf;
		stdu->content_fb_type = vps->content_fb_type;
		stdu->cpp = vps->cpp;

		ret = vmw_stdu_bind_st(dev_priv, stdu, &stdu->display_srf->res);
		if (ret)
			DRM_ERROR("Failed to bind surface to STDU.\n");

		if (vfb->bo)
			ret = vmw_stdu_plane_update_bo(dev_priv, plane,
						       old_state, vfb, &fence);
		else
			ret = vmw_stdu_plane_update_surface(dev_priv, plane,
							    old_state, vfb,
							    &fence);
		if (ret)
			DRM_ERROR("Failed to update STDU.\n");
	} else {
		crtc = old_state->crtc;
		stdu = vmw_crtc_to_stdu(crtc);
		dev_priv = vmw_priv(crtc->dev);

		/* Blank STDU when fb and crtc are NULL */
		if (!stdu->defined)
			return;

		ret = vmw_stdu_bind_st(dev_priv, stdu, NULL);
		if (ret)
			DRM_ERROR("Failed to blank STDU\n");

		ret = vmw_stdu_update_st(dev_priv, stdu);
		if (ret)
			DRM_ERROR("Failed to update STDU.\n");

		return;
	}

	/* In case of error, vblank event is send in vmw_du_crtc_atomic_flush */
	event = crtc->state->event;
	if (event && fence) {
		struct drm_file *file_priv = event->base.file_priv;

		ret = vmw_event_fence_action_queue(file_priv,
						   fence,
						   &event->base,
						   &event->event.vbl.tv_sec,
						   &event->event.vbl.tv_usec,
						   true);
		if (ret)
			DRM_ERROR("Failed to queue event on fence.\n");
		else
			crtc->state->event = NULL;
	}

	if (fence)
		vmw_fence_obj_unreference(&fence);
}


static const struct drm_plane_funcs vmw_stdu_plane_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane = drm_atomic_helper_disable_plane,
	.destroy = vmw_du_primary_plane_destroy,
	.reset = vmw_du_plane_reset,
	.atomic_duplicate_state = vmw_du_plane_duplicate_state,
	.atomic_destroy_state = vmw_du_plane_destroy_state,
};

static const struct drm_plane_funcs vmw_stdu_cursor_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane = drm_atomic_helper_disable_plane,
	.destroy = vmw_du_cursor_plane_destroy,
	.reset = vmw_du_plane_reset,
	.atomic_duplicate_state = vmw_du_plane_duplicate_state,
	.atomic_destroy_state = vmw_du_plane_destroy_state,
};


/*
 * Atomic Helpers
 */
static const struct
drm_plane_helper_funcs vmw_stdu_cursor_plane_helper_funcs = {
	.atomic_check = vmw_du_cursor_plane_atomic_check,
	.atomic_update = vmw_du_cursor_plane_atomic_update,
	.prepare_fb = vmw_du_cursor_plane_prepare_fb,
	.cleanup_fb = vmw_du_plane_cleanup_fb,
};

static const struct
drm_plane_helper_funcs vmw_stdu_primary_plane_helper_funcs = {
	.atomic_check = vmw_du_primary_plane_atomic_check,
	.atomic_update = vmw_stdu_primary_plane_atomic_update,
	.prepare_fb = vmw_stdu_primary_plane_prepare_fb,
	.cleanup_fb = vmw_stdu_primary_plane_cleanup_fb,
};

static const struct drm_crtc_helper_funcs vmw_stdu_crtc_helper_funcs = {
	.prepare = vmw_stdu_crtc_helper_prepare,
	.mode_set_nofb = vmw_stdu_crtc_mode_set_nofb,
	.atomic_check = vmw_du_crtc_atomic_check,
	.atomic_begin = vmw_du_crtc_atomic_begin,
	.atomic_flush = vmw_du_crtc_atomic_flush,
	.atomic_enable = vmw_stdu_crtc_atomic_enable,
	.atomic_disable = vmw_stdu_crtc_atomic_disable,
};


/**
 * vmw_stdu_init - Sets up a Screen Target Display Unit
 *
 * @dev_priv: VMW DRM device
 * @unit: unit number range from 0 to VMWGFX_NUM_DISPLAY_UNITS
 *
 * This function is called once per CRTC, and allocates one Screen Target
 * display unit to represent that CRTC.  Since the SVGA device does not separate
 * out encoder and connector, they are represented as part of the STDU as well.
 */
static int vmw_stdu_init(struct vmw_private *dev_priv, unsigned unit)
{
	struct vmw_screen_target_display_unit *stdu;
	struct drm_device *dev = &dev_priv->drm;
	struct drm_connector *connector;
	struct drm_encoder *encoder;
	struct drm_plane *primary, *cursor;
	struct drm_crtc *crtc;
	int    ret;


	stdu = kzalloc(sizeof(*stdu), GFP_KERNEL);
	if (!stdu)
		return -ENOMEM;

	stdu->base.unit = unit;
	crtc = &stdu->base.crtc;
	encoder = &stdu->base.encoder;
	connector = &stdu->base.connector;
	primary = &stdu->base.primary;
	cursor = &stdu->base.cursor;

	stdu->base.pref_active = (unit == 0);
	stdu->base.pref_width  = dev_priv->initial_width;
	stdu->base.pref_height = dev_priv->initial_height;
	stdu->base.is_implicit = false;

	/* Initialize primary plane */
	ret = drm_universal_plane_init(dev, primary,
				       0, &vmw_stdu_plane_funcs,
				       vmw_primary_plane_formats,
				       ARRAY_SIZE(vmw_primary_plane_formats),
				       NULL, DRM_PLANE_TYPE_PRIMARY, NULL);
	if (ret) {
		DRM_ERROR("Failed to initialize primary plane");
		goto err_free;
	}

	drm_plane_helper_add(primary, &vmw_stdu_primary_plane_helper_funcs);
	drm_plane_enable_fb_damage_clips(primary);

	/* Initialize cursor plane */
	ret = drm_universal_plane_init(dev, cursor,
			0, &vmw_stdu_cursor_funcs,
			vmw_cursor_plane_formats,
			ARRAY_SIZE(vmw_cursor_plane_formats),
			NULL, DRM_PLANE_TYPE_CURSOR, NULL);
	if (ret) {
		DRM_ERROR("Failed to initialize cursor plane");
		drm_plane_cleanup(&stdu->base.primary);
		goto err_free;
	}

	drm_plane_helper_add(cursor, &vmw_stdu_cursor_plane_helper_funcs);

	ret = drm_connector_init(dev, connector, &vmw_stdu_connector_funcs,
				 DRM_MODE_CONNECTOR_VIRTUAL);
	if (ret) {
		DRM_ERROR("Failed to initialize connector\n");
		goto err_free;
	}

	drm_connector_helper_add(connector, &vmw_stdu_connector_helper_funcs);
	connector->status = vmw_du_connector_detect(connector, false);

	ret = drm_encoder_init(dev, encoder, &vmw_stdu_encoder_funcs,
			       DRM_MODE_ENCODER_VIRTUAL, NULL);
	if (ret) {
		DRM_ERROR("Failed to initialize encoder\n");
		goto err_free_connector;
	}

	(void) drm_connector_attach_encoder(connector, encoder);
	encoder->possible_crtcs = (1 << unit);
	encoder->possible_clones = 0;

	ret = drm_connector_register(connector);
	if (ret) {
		DRM_ERROR("Failed to register connector\n");
		goto err_free_encoder;
	}

	ret = drm_crtc_init_with_planes(dev, crtc, &stdu->base.primary,
					&stdu->base.cursor,
					&vmw_stdu_crtc_funcs, NULL);
	if (ret) {
		DRM_ERROR("Failed to initialize CRTC\n");
		goto err_free_unregister;
	}

	drm_crtc_helper_add(crtc, &vmw_stdu_crtc_helper_funcs);

	drm_mode_crtc_set_gamma_size(crtc, 256);

	drm_object_attach_property(&connector->base,
				   dev_priv->hotplug_mode_update_property, 1);
	drm_object_attach_property(&connector->base,
				   dev->mode_config.suggested_x_property, 0);
	drm_object_attach_property(&connector->base,
				   dev->mode_config.suggested_y_property, 0);
	return 0;

err_free_unregister:
	drm_connector_unregister(connector);
err_free_encoder:
	drm_encoder_cleanup(encoder);
err_free_connector:
	drm_connector_cleanup(connector);
err_free:
	kfree(stdu);
	return ret;
}



/**
 *  vmw_stdu_destroy - Cleans up a vmw_screen_target_display_unit
 *
 *  @stdu:  Screen Target Display Unit to be destroyed
 *
 *  Clean up after vmw_stdu_init
 */
static void vmw_stdu_destroy(struct vmw_screen_target_display_unit *stdu)
{
	vmw_du_cleanup(&stdu->base);
	kfree(stdu);
}



/******************************************************************************
 * Screen Target Display KMS Functions
 *
 * These functions are called by the common KMS code in vmwgfx_kms.c
 *****************************************************************************/

/**
 * vmw_kms_stdu_init_display - Initializes a Screen Target based display
 *
 * @dev_priv: VMW DRM device
 *
 * This function initialize a Screen Target based display device.  It checks
 * the capability bits to make sure the underlying hardware can support
 * screen targets, and then creates the maximum number of CRTCs, a.k.a Display
 * Units, as supported by the display hardware.
 *
 * RETURNS:
 * 0 on success, error code otherwise
 */
int vmw_kms_stdu_init_display(struct vmw_private *dev_priv)
{
	struct drm_device *dev = &dev_priv->drm;
	int i, ret;


	/* Do nothing if Screen Target support is turned off */
	if (!VMWGFX_ENABLE_SCREEN_TARGET_OTABLE || !dev_priv->has_mob)
		return -ENOSYS;

	if (!(dev_priv->capabilities & SVGA_CAP_GBOBJECTS))
		return -ENOSYS;

	ret = drm_vblank_init(dev, VMWGFX_NUM_DISPLAY_UNITS);
	if (unlikely(ret != 0))
		return ret;

	dev_priv->active_display_unit = vmw_du_screen_target;

	for (i = 0; i < VMWGFX_NUM_DISPLAY_UNITS; ++i) {
		ret = vmw_stdu_init(dev_priv, i);

		if (unlikely(ret != 0)) {
			DRM_ERROR("Failed to initialize STDU %d", i);
			return ret;
		}
	}

	drm_mode_config_reset(dev);

	DRM_INFO("Screen Target Display device initialized\n");

	return 0;
}

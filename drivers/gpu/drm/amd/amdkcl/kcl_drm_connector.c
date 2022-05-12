/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */
#include <kcl/kcl_drm_connector.h>

#ifndef HAVE_DRM_CONNECTOR_INIT_WITH_DDC
int _kcl_drm_connector_init_with_ddc(struct drm_device *dev,
				struct drm_connector *connector,
				const struct drm_connector_funcs *funcs,
				int connector_type,
				struct i2c_adapter *ddc)
{
	return drm_connector_init(dev, connector, funcs, connector_type);
}
EXPORT_SYMBOL(_kcl_drm_connector_init_with_ddc);
#endif

#ifndef HAVE_DRM_MODE_CONFIG_DP_SUBCONNECTOR_PROPERTY
amdkcl_dummy_symbol(drm_connector_attach_dp_subconnector_property, void, return,
				  struct drm_connector *connector)
amdkcl_dummy_symbol(drm_dp_set_subconnector_property, void, return,
				  struct drm_connector *connector, enum drm_connector_status status,
				  const u8 *dpcd, const u8 prot_cap[4])
#endif

#ifndef HAVE_DRM_CONNECTOR_ATOMIC_HDR_METADATA_EQUAL

bool drm_connector_atomic_hdr_metadata_equal(struct drm_connector_state *old_state,
                                             struct drm_connector_state *new_state)
{
#ifdef HAVE_DRM_CONNECTOR_STATE_HDR_OUTPUT_METADATA
        struct drm_property_blob *old_blob = old_state->hdr_output_metadata;
        struct drm_property_blob *new_blob = new_state->hdr_output_metadata;

        if (!old_blob || !new_blob)
                return old_blob == new_blob;

        if (old_blob->length != new_blob->length)
                return false;

        return !memcmp(old_blob->data, new_blob->data, old_blob->length);
#else
	return false;
#endif
}
EXPORT_SYMBOL(drm_connector_atomic_hdr_metadata_equal);
#endif

#if !defined(HAVE_DRM_CONNECTOR_ATTACH_HDR_OUTPUT_METADATA_PROPERTY)
int drm_connector_attach_hdr_output_metadata_property(struct drm_connector *connector)
{
#ifdef HAVE_DRM_CONNECTOR_ATTACH_HDR_OUTPUT_METADATA_PROPERTY
        struct drm_device *dev = connector->dev;
        struct drm_property *prop = dev->mode_config.hdr_output_metadata_property;

        drm_object_attach_property(&connector->base, prop, 0);
#endif

        return 0;
}
EXPORT_SYMBOL(drm_connector_attach_hdr_output_metadata_property);
#endif

#if !defined(HAVE_DRM_CONNECTOR_SET_PANEL_ORIENTATION_WITH_QUIRK)
int _kcl_drm_connector_set_panel_orientation_with_quirk(
        struct drm_connector *connector,
        enum drm_panel_orientation panel_orientation,
        int width, int height)
{
#ifdef HAVE_DRM_PANEL_ORIENTATION_ENUM
	return drm_connector_init_panel_orientation_property(connector, width, height);
#else
	pr_warn_once("this function is not implemented in this legacy kernel");
	return 0;
#endif
}
EXPORT_SYMBOL(_kcl_drm_connector_set_panel_orientation_with_quirk);
#endif

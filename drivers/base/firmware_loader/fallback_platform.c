// SPDX-License-Identifier: GPL-2.0

#include <linux/efi_embedded_fw.h>
#include <linux/property.h>
#include <linux/security.h>
#include <linux/vmalloc.h>

#include "fallback.h"
#include "firmware.h"

int firmware_fallback_platform(struct fw_priv *fw_priv)
{
	const u8 *data;
	size_t size;
	int rc;

	if (!(fw_priv->opt_flags & FW_OPT_FALLBACK_PLATFORM))
		return -ENOENT;

	rc = security_kernel_load_data(LOADING_FIRMWARE, true);
	if (rc)
		return rc;

	rc = efi_get_embedded_fw(fw_priv->fw_name, &data, &size);
	if (rc)
		return rc; /* rc == -ENOENT when the fw was not found */

	if (fw_priv->data && size > fw_priv->allocated_size)
		return -ENOMEM;

	rc = security_kernel_post_load_data((u8 *)data, size, LOADING_FIRMWARE,
						"platform");
	if (rc)
		return rc;

	if (!fw_priv->data)
		fw_priv->data = vmalloc(size);
	if (!fw_priv->data)
		return -ENOMEM;

	memcpy(fw_priv->data, data, size);
	fw_priv->size = size;
	fw_state_done(fw_priv);
	return 0;
}

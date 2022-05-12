// SPDX-License-Identifier: MIT
/*
 * Copyright © 2020 Intel Corporation
 */

#include <drm/drm_print.h>

#include "gt/debugfs_gt.h"
#include "intel_huc.h"
#include "intel_huc_debugfs.h"

static int huc_info_show(struct seq_file *m, void *data)
{
	struct intel_huc *huc = m->private;
	struct drm_printer p = drm_seq_file_printer(m);

	if (!intel_huc_is_supported(huc))
		return -ENODEV;

	intel_huc_load_status(huc, &p);

	return 0;
}
DEFINE_GT_DEBUGFS_ATTRIBUTE(huc_info);

void intel_huc_debugfs_register(struct intel_huc *huc, struct dentry *root)
{
	static const struct debugfs_gt_file files[] = {
		{ "huc_info", &huc_info_fops, NULL },
	};

	if (!intel_huc_is_supported(huc))
		return;

	intel_gt_debugfs_register_files(root, files, ARRAY_SIZE(files), huc);
}

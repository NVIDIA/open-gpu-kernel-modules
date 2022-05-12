/*
 * Copyright (C) 2009 Red Hat <bskeggs@redhat.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * Authors:
 *  Ben Skeggs <bskeggs@redhat.com>
 */

#include <linux/debugfs.h>
#include <nvif/class.h>
#include <nvif/if0001.h>
#include "nouveau_debugfs.h"
#include "nouveau_drv.h"

static int
nouveau_debugfs_vbios_image(struct seq_file *m, void *data)
{
	struct drm_info_node *node = (struct drm_info_node *) m->private;
	struct nouveau_drm *drm = nouveau_drm(node->minor->dev);
	int i;

	for (i = 0; i < drm->vbios.length; i++)
		seq_printf(m, "%c", drm->vbios.data[i]);
	return 0;
}

static int
nouveau_debugfs_strap_peek(struct seq_file *m, void *data)
{
	struct drm_info_node *node = m->private;
	struct nouveau_drm *drm = nouveau_drm(node->minor->dev);
	int ret;

	ret = pm_runtime_get_sync(drm->dev->dev);
	if (ret < 0 && ret != -EACCES) {
		pm_runtime_put_autosuspend(drm->dev->dev);
		return ret;
	}

	seq_printf(m, "0x%08x\n",
		   nvif_rd32(&drm->client.device.object, 0x101000));

	pm_runtime_mark_last_busy(drm->dev->dev);
	pm_runtime_put_autosuspend(drm->dev->dev);

	return 0;
}

static int
nouveau_debugfs_pstate_get(struct seq_file *m, void *data)
{
	struct drm_device *drm = m->private;
	struct nouveau_debugfs *debugfs = nouveau_debugfs(drm);
	struct nvif_object *ctrl = &debugfs->ctrl;
	struct nvif_control_pstate_info_v0 info = {};
	int ret, i;

	if (!debugfs)
		return -ENODEV;

	ret = nvif_mthd(ctrl, NVIF_CONTROL_PSTATE_INFO, &info, sizeof(info));
	if (ret)
		return ret;

	for (i = 0; i < info.count + 1; i++) {
		const s32 state = i < info.count ? i :
			NVIF_CONTROL_PSTATE_ATTR_V0_STATE_CURRENT;
		struct nvif_control_pstate_attr_v0 attr = {
			.state = state,
			.index = 0,
		};

		ret = nvif_mthd(ctrl, NVIF_CONTROL_PSTATE_ATTR,
				&attr, sizeof(attr));
		if (ret)
			return ret;

		if (i < info.count)
			seq_printf(m, "%02x:", attr.state);
		else
			seq_printf(m, "%s:", info.pwrsrc == 0 ? "DC" :
					     info.pwrsrc == 1 ? "AC" : "--");

		attr.index = 0;
		do {
			attr.state = state;
			ret = nvif_mthd(ctrl, NVIF_CONTROL_PSTATE_ATTR,
					&attr, sizeof(attr));
			if (ret)
				return ret;

			seq_printf(m, " %s %d", attr.name, attr.min);
			if (attr.min != attr.max)
				seq_printf(m, "-%d", attr.max);
			seq_printf(m, " %s", attr.unit);
		} while (attr.index);

		if (state >= 0) {
			if (info.ustate_ac == state)
				seq_printf(m, " AC");
			if (info.ustate_dc == state)
				seq_printf(m, " DC");
			if (info.pstate == state)
				seq_printf(m, " *");
		} else {
			if (info.ustate_ac < -1)
				seq_printf(m, " AC");
			if (info.ustate_dc < -1)
				seq_printf(m, " DC");
		}

		seq_printf(m, "\n");
	}

	return 0;
}

static ssize_t
nouveau_debugfs_pstate_set(struct file *file, const char __user *ubuf,
			   size_t len, loff_t *offp)
{
	struct seq_file *m = file->private_data;
	struct drm_device *drm = m->private;
	struct nouveau_debugfs *debugfs = nouveau_debugfs(drm);
	struct nvif_object *ctrl = &debugfs->ctrl;
	struct nvif_control_pstate_user_v0 args = { .pwrsrc = -EINVAL };
	char buf[32] = {}, *tmp, *cur = buf;
	long value, ret;

	if (!debugfs)
		return -ENODEV;

	if (len >= sizeof(buf))
		return -EINVAL;

	if (copy_from_user(buf, ubuf, len))
		return -EFAULT;

	if ((tmp = strchr(buf, '\n')))
		*tmp = '\0';

	if (!strncasecmp(cur, "dc:", 3)) {
		args.pwrsrc = 0;
		cur += 3;
	} else
	if (!strncasecmp(cur, "ac:", 3)) {
		args.pwrsrc = 1;
		cur += 3;
	}

	if (!strcasecmp(cur, "none"))
		args.ustate = NVIF_CONTROL_PSTATE_USER_V0_STATE_UNKNOWN;
	else
	if (!strcasecmp(cur, "auto"))
		args.ustate = NVIF_CONTROL_PSTATE_USER_V0_STATE_PERFMON;
	else {
		ret = kstrtol(cur, 16, &value);
		if (ret)
			return ret;
		args.ustate = value;
	}

	ret = pm_runtime_get_sync(drm->dev);
	if (ret < 0 && ret != -EACCES) {
		pm_runtime_put_autosuspend(drm->dev);
		return ret;
	}

	ret = nvif_mthd(ctrl, NVIF_CONTROL_PSTATE_USER, &args, sizeof(args));
	pm_runtime_put_autosuspend(drm->dev);
	if (ret < 0)
		return ret;

	return len;
}

static int
nouveau_debugfs_pstate_open(struct inode *inode, struct file *file)
{
	return single_open(file, nouveau_debugfs_pstate_get, inode->i_private);
}

static const struct file_operations nouveau_pstate_fops = {
	.owner = THIS_MODULE,
	.open = nouveau_debugfs_pstate_open,
	.read = seq_read,
	.write = nouveau_debugfs_pstate_set,
};

static struct drm_info_list nouveau_debugfs_list[] = {
	{ "vbios.rom",  nouveau_debugfs_vbios_image, 0, NULL },
	{ "strap_peek", nouveau_debugfs_strap_peek, 0, NULL },
};
#define NOUVEAU_DEBUGFS_ENTRIES ARRAY_SIZE(nouveau_debugfs_list)

static const struct nouveau_debugfs_files {
	const char *name;
	const struct file_operations *fops;
} nouveau_debugfs_files[] = {
	{"pstate", &nouveau_pstate_fops},
};

void
nouveau_drm_debugfs_init(struct drm_minor *minor)
{
	struct nouveau_drm *drm = nouveau_drm(minor->dev);
	struct dentry *dentry;
	int i;

	for (i = 0; i < ARRAY_SIZE(nouveau_debugfs_files); i++) {
		debugfs_create_file(nouveau_debugfs_files[i].name,
				    S_IRUGO | S_IWUSR,
				    minor->debugfs_root, minor->dev,
				    nouveau_debugfs_files[i].fops);
	}

	drm_debugfs_create_files(nouveau_debugfs_list,
				 NOUVEAU_DEBUGFS_ENTRIES,
				 minor->debugfs_root, minor);

	/* Set the size of the vbios since we know it, and it's confusing to
	 * userspace if it wants to seek() but the file has a length of 0
	 */
	dentry = debugfs_lookup("vbios.rom", minor->debugfs_root);
	if (!dentry)
		return;

	d_inode(dentry)->i_size = drm->vbios.length;
	dput(dentry);
}

int
nouveau_debugfs_init(struct nouveau_drm *drm)
{
	int ret;

	drm->debugfs = kzalloc(sizeof(*drm->debugfs), GFP_KERNEL);
	if (!drm->debugfs)
		return -ENOMEM;

	ret = nvif_object_ctor(&drm->client.device.object, "debugfsCtrl", 0,
			       NVIF_CLASS_CONTROL, NULL, 0,
			       &drm->debugfs->ctrl);
	if (ret)
		return ret;

	return 0;
}

void
nouveau_debugfs_fini(struct nouveau_drm *drm)
{
	if (drm->debugfs && drm->debugfs->ctrl.priv)
		nvif_object_dtor(&drm->debugfs->ctrl);

	kfree(drm->debugfs);
	drm->debugfs = NULL;
}

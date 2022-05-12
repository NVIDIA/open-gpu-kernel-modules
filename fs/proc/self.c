// SPDX-License-Identifier: GPL-2.0
#include <linux/cache.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/pid_namespace.h>
#include "internal.h"

/*
 * /proc/self:
 */
static const char *proc_self_get_link(struct dentry *dentry,
				      struct inode *inode,
				      struct delayed_call *done)
{
	struct pid_namespace *ns = proc_pid_ns(inode->i_sb);
	pid_t tgid = task_tgid_nr_ns(current, ns);
	char *name;

	if (!tgid)
		return ERR_PTR(-ENOENT);
	/* max length of unsigned int in decimal + NULL term */
	name = kmalloc(10 + 1, dentry ? GFP_KERNEL : GFP_ATOMIC);
	if (unlikely(!name))
		return dentry ? ERR_PTR(-ENOMEM) : ERR_PTR(-ECHILD);
	sprintf(name, "%u", tgid);
	set_delayed_call(done, kfree_link, name);
	return name;
}

static const struct inode_operations proc_self_inode_operations = {
	.get_link	= proc_self_get_link,
};

static unsigned self_inum __ro_after_init;

int proc_setup_self(struct super_block *s)
{
	struct inode *root_inode = d_inode(s->s_root);
	struct proc_fs_info *fs_info = proc_sb_info(s);
	struct dentry *self;
	int ret = -ENOMEM;

	inode_lock(root_inode);
	self = d_alloc_name(s->s_root, "self");
	if (self) {
		struct inode *inode = new_inode(s);
		if (inode) {
			inode->i_ino = self_inum;
			inode->i_mtime = inode->i_atime = inode->i_ctime = current_time(inode);
			inode->i_mode = S_IFLNK | S_IRWXUGO;
			inode->i_uid = GLOBAL_ROOT_UID;
			inode->i_gid = GLOBAL_ROOT_GID;
			inode->i_op = &proc_self_inode_operations;
			d_add(self, inode);
			ret = 0;
		} else {
			dput(self);
		}
	}
	inode_unlock(root_inode);

	if (ret)
		pr_err("proc_fill_super: can't allocate /proc/self\n");
	else
		fs_info->proc_self = self;

	return ret;
}

void __init proc_self_init(void)
{
	proc_alloc_inum(&self_inum);
}

// SPDX-License-Identifier: GPL-2.0
/*
 * Intel Speed Select Interface: Common functions
 * Copyright (c) 2019, Intel Corporation.
 * All rights reserved.
 *
 * Author: Srinivas Pandruvada <srinivas.pandruvada@linux.intel.com>
 */

#include <linux/cpufeature.h>
#include <linux/cpuhotplug.h>
#include <linux/fs.h>
#include <linux/hashtable.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <uapi/linux/isst_if.h>

#include "isst_if_common.h"

#define MSR_THREAD_ID_INFO	0x53
#define MSR_CPU_BUS_NUMBER	0x128

static struct isst_if_cmd_cb punit_callbacks[ISST_IF_DEV_MAX];

static int punit_msr_white_list[] = {
	MSR_TURBO_RATIO_LIMIT,
	MSR_CONFIG_TDP_CONTROL,
	MSR_TURBO_RATIO_LIMIT1,
	MSR_TURBO_RATIO_LIMIT2,
};

struct isst_valid_cmd_ranges {
	u16 cmd;
	u16 sub_cmd_beg;
	u16 sub_cmd_end;
};

struct isst_cmd_set_req_type {
	u16 cmd;
	u16 sub_cmd;
	u16 param;
};

static const struct isst_valid_cmd_ranges isst_valid_cmds[] = {
	{0xD0, 0x00, 0x03},
	{0x7F, 0x00, 0x0B},
	{0x7F, 0x10, 0x12},
	{0x7F, 0x20, 0x23},
	{0x94, 0x03, 0x03},
	{0x95, 0x03, 0x03},
};

static const struct isst_cmd_set_req_type isst_cmd_set_reqs[] = {
	{0xD0, 0x00, 0x08},
	{0xD0, 0x01, 0x08},
	{0xD0, 0x02, 0x08},
	{0xD0, 0x03, 0x08},
	{0x7F, 0x02, 0x00},
	{0x7F, 0x08, 0x00},
	{0x95, 0x03, 0x03},
};

struct isst_cmd {
	struct hlist_node hnode;
	u64 data;
	u32 cmd;
	int cpu;
	int mbox_cmd_type;
	u32 param;
};

static DECLARE_HASHTABLE(isst_hash, 8);
static DEFINE_MUTEX(isst_hash_lock);

static int isst_store_new_cmd(int cmd, u32 cpu, int mbox_cmd_type, u32 param,
			      u32 data)
{
	struct isst_cmd *sst_cmd;

	sst_cmd = kmalloc(sizeof(*sst_cmd), GFP_KERNEL);
	if (!sst_cmd)
		return -ENOMEM;

	sst_cmd->cpu = cpu;
	sst_cmd->cmd = cmd;
	sst_cmd->mbox_cmd_type = mbox_cmd_type;
	sst_cmd->param = param;
	sst_cmd->data = data;

	hash_add(isst_hash, &sst_cmd->hnode, sst_cmd->cmd);

	return 0;
}

static void isst_delete_hash(void)
{
	struct isst_cmd *sst_cmd;
	struct hlist_node *tmp;
	int i;

	hash_for_each_safe(isst_hash, i, tmp, sst_cmd, hnode) {
		hash_del(&sst_cmd->hnode);
		kfree(sst_cmd);
	}
}

/**
 * isst_store_cmd() - Store command to a hash table
 * @cmd: Mailbox command.
 * @sub_cmd: Mailbox sub-command or MSR id.
 * @mbox_cmd_type: Mailbox or MSR command.
 * @param: Mailbox parameter.
 * @data: Mailbox request data or MSR data.
 *
 * Stores the command to a hash table if there is no such command already
 * stored. If already stored update the latest parameter and data for the
 * command.
 *
 * Return: Return result of store to hash table, 0 for success, others for
 * failure.
 */
int isst_store_cmd(int cmd, int sub_cmd, u32 cpu, int mbox_cmd_type,
		   u32 param, u64 data)
{
	struct isst_cmd *sst_cmd;
	int full_cmd, ret;

	full_cmd = (cmd & GENMASK_ULL(15, 0)) << 16;
	full_cmd |= (sub_cmd & GENMASK_ULL(15, 0));
	mutex_lock(&isst_hash_lock);
	hash_for_each_possible(isst_hash, sst_cmd, hnode, full_cmd) {
		if (sst_cmd->cmd == full_cmd && sst_cmd->cpu == cpu &&
		    sst_cmd->mbox_cmd_type == mbox_cmd_type) {
			sst_cmd->param = param;
			sst_cmd->data = data;
			mutex_unlock(&isst_hash_lock);
			return 0;
		}
	}

	ret = isst_store_new_cmd(full_cmd, cpu, mbox_cmd_type, param, data);
	mutex_unlock(&isst_hash_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(isst_store_cmd);

static void isst_mbox_resume_command(struct isst_if_cmd_cb *cb,
				     struct isst_cmd *sst_cmd)
{
	struct isst_if_mbox_cmd mbox_cmd;
	int wr_only;

	mbox_cmd.command = (sst_cmd->cmd & GENMASK_ULL(31, 16)) >> 16;
	mbox_cmd.sub_command = sst_cmd->cmd & GENMASK_ULL(15, 0);
	mbox_cmd.parameter = sst_cmd->param;
	mbox_cmd.req_data = sst_cmd->data;
	mbox_cmd.logical_cpu = sst_cmd->cpu;
	(cb->cmd_callback)((u8 *)&mbox_cmd, &wr_only, 1);
}

/**
 * isst_resume_common() - Process Resume request
 *
 * On resume replay all mailbox commands and MSRs.
 *
 * Return: None.
 */
void isst_resume_common(void)
{
	struct isst_cmd *sst_cmd;
	int i;

	hash_for_each(isst_hash, i, sst_cmd, hnode) {
		struct isst_if_cmd_cb *cb;

		if (sst_cmd->mbox_cmd_type) {
			cb = &punit_callbacks[ISST_IF_DEV_MBOX];
			if (cb->registered)
				isst_mbox_resume_command(cb, sst_cmd);
		} else {
			wrmsrl_safe_on_cpu(sst_cmd->cpu, sst_cmd->cmd,
					   sst_cmd->data);
		}
	}
}
EXPORT_SYMBOL_GPL(isst_resume_common);

static void isst_restore_msr_local(int cpu)
{
	struct isst_cmd *sst_cmd;
	int i;

	mutex_lock(&isst_hash_lock);
	for (i = 0; i < ARRAY_SIZE(punit_msr_white_list); ++i) {
		if (!punit_msr_white_list[i])
			break;

		hash_for_each_possible(isst_hash, sst_cmd, hnode,
				       punit_msr_white_list[i]) {
			if (!sst_cmd->mbox_cmd_type && sst_cmd->cpu == cpu)
				wrmsrl_safe(sst_cmd->cmd, sst_cmd->data);
		}
	}
	mutex_unlock(&isst_hash_lock);
}

/**
 * isst_if_mbox_cmd_invalid() - Check invalid mailbox commands
 * @cmd: Pointer to the command structure to verify.
 *
 * Invalid command to PUNIT to may result in instability of the platform.
 * This function has a whitelist of commands, which are allowed.
 *
 * Return: Return true if the command is invalid, else false.
 */
bool isst_if_mbox_cmd_invalid(struct isst_if_mbox_cmd *cmd)
{
	int i;

	if (cmd->logical_cpu >= nr_cpu_ids)
		return true;

	for (i = 0; i < ARRAY_SIZE(isst_valid_cmds); ++i) {
		if (cmd->command == isst_valid_cmds[i].cmd &&
		    (cmd->sub_command >= isst_valid_cmds[i].sub_cmd_beg &&
		     cmd->sub_command <= isst_valid_cmds[i].sub_cmd_end)) {
			return false;
		}
	}

	return true;
}
EXPORT_SYMBOL_GPL(isst_if_mbox_cmd_invalid);

/**
 * isst_if_mbox_cmd_set_req() - Check mailbox command is a set request
 * @cmd: Pointer to the command structure to verify.
 *
 * Check if the given mail box level is set request and not a get request.
 *
 * Return: Return true if the command is set_req, else false.
 */
bool isst_if_mbox_cmd_set_req(struct isst_if_mbox_cmd *cmd)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(isst_cmd_set_reqs); ++i) {
		if (cmd->command == isst_cmd_set_reqs[i].cmd &&
		    cmd->sub_command == isst_cmd_set_reqs[i].sub_cmd &&
		    cmd->parameter == isst_cmd_set_reqs[i].param) {
			return true;
		}
	}

	return false;
}
EXPORT_SYMBOL_GPL(isst_if_mbox_cmd_set_req);

static int isst_if_get_platform_info(void __user *argp)
{
	struct isst_if_platform_info info;

	info.api_version = ISST_IF_API_VERSION,
	info.driver_version = ISST_IF_DRIVER_VERSION,
	info.max_cmds_per_ioctl = ISST_IF_CMD_LIMIT,
	info.mbox_supported = punit_callbacks[ISST_IF_DEV_MBOX].registered;
	info.mmio_supported = punit_callbacks[ISST_IF_DEV_MMIO].registered;

	if (copy_to_user(argp, &info, sizeof(info)))
		return -EFAULT;

	return 0;
}


struct isst_if_cpu_info {
	/* For BUS 0 and BUS 1 only, which we need for PUNIT interface */
	int bus_info[2];
	int punit_cpu_id;
};

static struct isst_if_cpu_info *isst_cpu_info;

/**
 * isst_if_get_pci_dev() - Get the PCI device instance for a CPU
 * @cpu: Logical CPU number.
 * @bus_number: The bus number assigned by the hardware.
 * @dev: The device number assigned by the hardware.
 * @fn: The function number assigned by the hardware.
 *
 * Using cached bus information, find out the PCI device for a bus number,
 * device and function.
 *
 * Return: Return pci_dev pointer or NULL.
 */
struct pci_dev *isst_if_get_pci_dev(int cpu, int bus_no, int dev, int fn)
{
	int bus_number;

	if (bus_no < 0 || bus_no > 1 || cpu < 0 || cpu >= nr_cpu_ids ||
	    cpu >= num_possible_cpus())
		return NULL;

	bus_number = isst_cpu_info[cpu].bus_info[bus_no];
	if (bus_number < 0)
		return NULL;

	return pci_get_domain_bus_and_slot(0, bus_number, PCI_DEVFN(dev, fn));
}
EXPORT_SYMBOL_GPL(isst_if_get_pci_dev);

static int isst_if_cpu_online(unsigned int cpu)
{
	u64 data;
	int ret;

	ret = rdmsrl_safe(MSR_CPU_BUS_NUMBER, &data);
	if (ret) {
		/* This is not a fatal error on MSR mailbox only I/F */
		isst_cpu_info[cpu].bus_info[0] = -1;
		isst_cpu_info[cpu].bus_info[1] = -1;
	} else {
		isst_cpu_info[cpu].bus_info[0] = data & 0xff;
		isst_cpu_info[cpu].bus_info[1] = (data >> 8) & 0xff;
	}

	ret = rdmsrl_safe(MSR_THREAD_ID_INFO, &data);
	if (ret) {
		isst_cpu_info[cpu].punit_cpu_id = -1;
		return ret;
	}
	isst_cpu_info[cpu].punit_cpu_id = data;

	isst_restore_msr_local(cpu);

	return 0;
}

static int isst_if_online_id;

static int isst_if_cpu_info_init(void)
{
	int ret;

	isst_cpu_info = kcalloc(num_possible_cpus(),
				sizeof(*isst_cpu_info),
				GFP_KERNEL);
	if (!isst_cpu_info)
		return -ENOMEM;

	ret = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN,
				"platform/x86/isst-if:online",
				isst_if_cpu_online, NULL);
	if (ret < 0) {
		kfree(isst_cpu_info);
		return ret;
	}

	isst_if_online_id = ret;

	return 0;
}

static void isst_if_cpu_info_exit(void)
{
	cpuhp_remove_state(isst_if_online_id);
	kfree(isst_cpu_info);
};

static long isst_if_proc_phyid_req(u8 *cmd_ptr, int *write_only, int resume)
{
	struct isst_if_cpu_map *cpu_map;

	cpu_map = (struct isst_if_cpu_map *)cmd_ptr;
	if (cpu_map->logical_cpu >= nr_cpu_ids ||
	    cpu_map->logical_cpu >= num_possible_cpus())
		return -EINVAL;

	*write_only = 0;
	cpu_map->physical_cpu = isst_cpu_info[cpu_map->logical_cpu].punit_cpu_id;

	return 0;
}

static bool match_punit_msr_white_list(int msr)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(punit_msr_white_list); ++i) {
		if (punit_msr_white_list[i] == msr)
			return true;
	}

	return false;
}

static long isst_if_msr_cmd_req(u8 *cmd_ptr, int *write_only, int resume)
{
	struct isst_if_msr_cmd *msr_cmd;
	int ret;

	msr_cmd = (struct isst_if_msr_cmd *)cmd_ptr;

	if (!match_punit_msr_white_list(msr_cmd->msr))
		return -EINVAL;

	if (msr_cmd->logical_cpu >= nr_cpu_ids)
		return -EINVAL;

	if (msr_cmd->read_write) {
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		ret = wrmsrl_safe_on_cpu(msr_cmd->logical_cpu,
					 msr_cmd->msr,
					 msr_cmd->data);
		*write_only = 1;
		if (!ret && !resume)
			ret = isst_store_cmd(0, msr_cmd->msr,
					     msr_cmd->logical_cpu,
					     0, 0, msr_cmd->data);
	} else {
		u64 data;

		ret = rdmsrl_safe_on_cpu(msr_cmd->logical_cpu,
					 msr_cmd->msr, &data);
		if (!ret) {
			msr_cmd->data = data;
			*write_only = 0;
		}
	}


	return ret;
}

static long isst_if_exec_multi_cmd(void __user *argp, struct isst_if_cmd_cb *cb)
{
	unsigned char __user *ptr;
	u32 cmd_count;
	u8 *cmd_ptr;
	long ret;
	int i;

	/* Each multi command has u32 command count as the first field */
	if (copy_from_user(&cmd_count, argp, sizeof(cmd_count)))
		return -EFAULT;

	if (!cmd_count || cmd_count > ISST_IF_CMD_LIMIT)
		return -EINVAL;

	cmd_ptr = kmalloc(cb->cmd_size, GFP_KERNEL);
	if (!cmd_ptr)
		return -ENOMEM;

	/* cb->offset points to start of the command after the command count */
	ptr = argp + cb->offset;

	for (i = 0; i < cmd_count; ++i) {
		int wr_only;

		if (signal_pending(current)) {
			ret = -EINTR;
			break;
		}

		if (copy_from_user(cmd_ptr, ptr, cb->cmd_size)) {
			ret = -EFAULT;
			break;
		}

		ret = cb->cmd_callback(cmd_ptr, &wr_only, 0);
		if (ret)
			break;

		if (!wr_only && copy_to_user(ptr, cmd_ptr, cb->cmd_size)) {
			ret = -EFAULT;
			break;
		}

		ptr += cb->cmd_size;
	}

	kfree(cmd_ptr);

	return i ? i : ret;
}

static long isst_if_def_ioctl(struct file *file, unsigned int cmd,
			      unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct isst_if_cmd_cb cmd_cb;
	struct isst_if_cmd_cb *cb;
	long ret = -ENOTTY;

	switch (cmd) {
	case ISST_IF_GET_PLATFORM_INFO:
		ret = isst_if_get_platform_info(argp);
		break;
	case ISST_IF_GET_PHY_ID:
		cmd_cb.cmd_size = sizeof(struct isst_if_cpu_map);
		cmd_cb.offset = offsetof(struct isst_if_cpu_maps, cpu_map);
		cmd_cb.cmd_callback = isst_if_proc_phyid_req;
		ret = isst_if_exec_multi_cmd(argp, &cmd_cb);
		break;
	case ISST_IF_IO_CMD:
		cb = &punit_callbacks[ISST_IF_DEV_MMIO];
		if (cb->registered)
			ret = isst_if_exec_multi_cmd(argp, cb);
		break;
	case ISST_IF_MBOX_COMMAND:
		cb = &punit_callbacks[ISST_IF_DEV_MBOX];
		if (cb->registered)
			ret = isst_if_exec_multi_cmd(argp, cb);
		break;
	case ISST_IF_MSR_COMMAND:
		cmd_cb.cmd_size = sizeof(struct isst_if_msr_cmd);
		cmd_cb.offset = offsetof(struct isst_if_msr_cmds, msr_cmd);
		cmd_cb.cmd_callback = isst_if_msr_cmd_req;
		ret = isst_if_exec_multi_cmd(argp, &cmd_cb);
		break;
	default:
		break;
	}

	return ret;
}

static DEFINE_MUTEX(punit_misc_dev_lock);
static int misc_usage_count;
static int misc_device_ret;
static int misc_device_open;

static int isst_if_open(struct inode *inode, struct file *file)
{
	int i, ret = 0;

	/* Fail open, if a module is going away */
	mutex_lock(&punit_misc_dev_lock);
	for (i = 0; i < ISST_IF_DEV_MAX; ++i) {
		struct isst_if_cmd_cb *cb = &punit_callbacks[i];

		if (cb->registered && !try_module_get(cb->owner)) {
			ret = -ENODEV;
			break;
		}
	}
	if (ret) {
		int j;

		for (j = 0; j < i; ++j) {
			struct isst_if_cmd_cb *cb;

			cb = &punit_callbacks[j];
			if (cb->registered)
				module_put(cb->owner);
		}
	} else {
		misc_device_open++;
	}
	mutex_unlock(&punit_misc_dev_lock);

	return ret;
}

static int isst_if_relase(struct inode *inode, struct file *f)
{
	int i;

	mutex_lock(&punit_misc_dev_lock);
	misc_device_open--;
	for (i = 0; i < ISST_IF_DEV_MAX; ++i) {
		struct isst_if_cmd_cb *cb = &punit_callbacks[i];

		if (cb->registered)
			module_put(cb->owner);
	}
	mutex_unlock(&punit_misc_dev_lock);

	return 0;
}

static const struct file_operations isst_if_char_driver_ops = {
	.open = isst_if_open,
	.unlocked_ioctl = isst_if_def_ioctl,
	.release = isst_if_relase,
};

static struct miscdevice isst_if_char_driver = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "isst_interface",
	.fops		= &isst_if_char_driver_ops,
};

/**
 * isst_if_cdev_register() - Register callback for IOCTL
 * @device_type: The device type this callback handling.
 * @cb:	Callback structure.
 *
 * This function registers a callback to device type. On very first call
 * it will register a misc device, which is used for user kernel interface.
 * Other calls simply increment ref count. Registry will fail, if the user
 * already opened misc device for operation. Also if the misc device
 * creation failed, then it will not try again and all callers will get
 * failure code.
 *
 * Return: Return the return value from the misc creation device or -EINVAL
 * for unsupported device type.
 */
int isst_if_cdev_register(int device_type, struct isst_if_cmd_cb *cb)
{
	if (misc_device_ret)
		return misc_device_ret;

	if (device_type >= ISST_IF_DEV_MAX)
		return -EINVAL;

	mutex_lock(&punit_misc_dev_lock);
	if (misc_device_open) {
		mutex_unlock(&punit_misc_dev_lock);
		return -EAGAIN;
	}
	if (!misc_usage_count) {
		int ret;

		misc_device_ret = misc_register(&isst_if_char_driver);
		if (misc_device_ret)
			goto unlock_exit;

		ret = isst_if_cpu_info_init();
		if (ret) {
			misc_deregister(&isst_if_char_driver);
			misc_device_ret = ret;
			goto unlock_exit;
		}
	}
	memcpy(&punit_callbacks[device_type], cb, sizeof(*cb));
	punit_callbacks[device_type].registered = 1;
	misc_usage_count++;
unlock_exit:
	mutex_unlock(&punit_misc_dev_lock);

	return misc_device_ret;
}
EXPORT_SYMBOL_GPL(isst_if_cdev_register);

/**
 * isst_if_cdev_unregister() - Unregister callback for IOCTL
 * @device_type: The device type to unregister.
 *
 * This function unregisters the previously registered callback. If this
 * is the last callback unregistering, then misc device is removed.
 *
 * Return: None.
 */
void isst_if_cdev_unregister(int device_type)
{
	mutex_lock(&punit_misc_dev_lock);
	misc_usage_count--;
	punit_callbacks[device_type].registered = 0;
	if (device_type == ISST_IF_DEV_MBOX)
		isst_delete_hash();
	if (!misc_usage_count && !misc_device_ret) {
		misc_deregister(&isst_if_char_driver);
		isst_if_cpu_info_exit();
	}
	mutex_unlock(&punit_misc_dev_lock);
}
EXPORT_SYMBOL_GPL(isst_if_cdev_unregister);

MODULE_LICENSE("GPL v2");

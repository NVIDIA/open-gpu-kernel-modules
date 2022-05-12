// SPDX-License-Identifier: GPL-2.0-or-later
/*
	Copyright (C) 2004 - 2009 Ivo van Doorn <IvDoorn@gmail.com>
	<http://rt2x00.serialmonkey.com>

 */

/*
	Module: rt2x00lib
	Abstract: rt2x00 debugfs specific routines.
 */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "rt2x00.h"
#include "rt2x00lib.h"
#include "rt2x00dump.h"

#define MAX_LINE_LENGTH 64

struct rt2x00debug_crypto {
	unsigned long success;
	unsigned long icv_error;
	unsigned long mic_error;
	unsigned long key_error;
};

struct rt2x00debug_intf {
	/*
	 * Pointer to driver structure where
	 * this debugfs entry belongs to.
	 */
	struct rt2x00_dev *rt2x00dev;

	/*
	 * Reference to the rt2x00debug structure
	 * which can be used to communicate with
	 * the registers.
	 */
	const struct rt2x00debug *debug;

	/*
	 * Debugfs entries for:
	 * - driver folder
	 *   - driver file
	 *   - chipset file
	 *   - device state flags file
	 *   - device capability flags file
	 *   - hardware restart file
	 *   - register folder
	 *     - csr offset/value files
	 *     - eeprom offset/value files
	 *     - bbp offset/value files
	 *     - rf offset/value files
	 *     - rfcsr offset/value files
	 *   - queue folder
	 *     - frame dump file
	 *     - queue stats file
	 *     - crypto stats file
	 */
	struct dentry *driver_folder;

	/*
	 * The frame dump file only allows a single reader,
	 * so we need to store the current state here.
	 */
	unsigned long frame_dump_flags;
#define FRAME_DUMP_FILE_OPEN	1

	/*
	 * We queue each frame before dumping it to the user,
	 * per read command we will pass a single skb structure
	 * so we should be prepared to queue multiple sk buffers
	 * before sending it to userspace.
	 */
	struct sk_buff_head frame_dump_skbqueue;
	wait_queue_head_t frame_dump_waitqueue;

	/*
	 * HW crypto statistics.
	 * All statistics are stored separately per cipher type.
	 */
	struct rt2x00debug_crypto crypto_stats[CIPHER_MAX];

	/*
	 * Driver and chipset files will use a data buffer
	 * that has been created in advance. This will simplify
	 * the code since we can use the debugfs functions.
	 */
	struct debugfs_blob_wrapper driver_blob;
	struct debugfs_blob_wrapper chipset_blob;

	/*
	 * Requested offset for each register type.
	 */
	unsigned int offset_csr;
	unsigned int offset_eeprom;
	unsigned int offset_bbp;
	unsigned int offset_rf;
	unsigned int offset_rfcsr;
};

void rt2x00debug_update_crypto(struct rt2x00_dev *rt2x00dev,
			       struct rxdone_entry_desc *rxdesc)
{
	struct rt2x00debug_intf *intf = rt2x00dev->debugfs_intf;
	enum cipher cipher = rxdesc->cipher;
	enum rx_crypto status = rxdesc->cipher_status;

	if (cipher == CIPHER_TKIP_NO_MIC)
		cipher = CIPHER_TKIP;
	if (cipher == CIPHER_NONE || cipher >= CIPHER_MAX)
		return;

	/* Remove CIPHER_NONE index */
	cipher--;

	intf->crypto_stats[cipher].success += (status == RX_CRYPTO_SUCCESS);
	intf->crypto_stats[cipher].icv_error += (status == RX_CRYPTO_FAIL_ICV);
	intf->crypto_stats[cipher].mic_error += (status == RX_CRYPTO_FAIL_MIC);
	intf->crypto_stats[cipher].key_error += (status == RX_CRYPTO_FAIL_KEY);
}

void rt2x00debug_dump_frame(struct rt2x00_dev *rt2x00dev,
			    enum rt2x00_dump_type type, struct queue_entry *entry)
{
	struct rt2x00debug_intf *intf = rt2x00dev->debugfs_intf;
	struct sk_buff *skb = entry->skb;
	struct skb_frame_desc *skbdesc = get_skb_frame_desc(skb);
	struct sk_buff *skbcopy;
	struct rt2x00dump_hdr *dump_hdr;
	struct timespec64 timestamp;
	u32 data_len;

	if (likely(!test_bit(FRAME_DUMP_FILE_OPEN, &intf->frame_dump_flags)))
		return;

	ktime_get_ts64(&timestamp);

	if (skb_queue_len(&intf->frame_dump_skbqueue) > 20) {
		rt2x00_dbg(rt2x00dev, "txrx dump queue length exceeded\n");
		return;
	}

	data_len = skb->len;
	if (skbdesc->flags & SKBDESC_DESC_IN_SKB)
		data_len -= skbdesc->desc_len;

	skbcopy = alloc_skb(sizeof(*dump_hdr) + skbdesc->desc_len + data_len,
			    GFP_ATOMIC);
	if (!skbcopy) {
		rt2x00_dbg(rt2x00dev, "Failed to copy skb for dump\n");
		return;
	}

	dump_hdr = skb_put(skbcopy, sizeof(*dump_hdr));
	dump_hdr->version = cpu_to_le32(DUMP_HEADER_VERSION);
	dump_hdr->header_length = cpu_to_le32(sizeof(*dump_hdr));
	dump_hdr->desc_length = cpu_to_le32(skbdesc->desc_len);
	dump_hdr->data_length = cpu_to_le32(data_len);
	dump_hdr->chip_rt = cpu_to_le16(rt2x00dev->chip.rt);
	dump_hdr->chip_rf = cpu_to_le16(rt2x00dev->chip.rf);
	dump_hdr->chip_rev = cpu_to_le16(rt2x00dev->chip.rev);
	dump_hdr->type = cpu_to_le16(type);
	dump_hdr->queue_index = entry->queue->qid;
	dump_hdr->entry_index = entry->entry_idx;
	dump_hdr->timestamp_sec = cpu_to_le32(timestamp.tv_sec);
	dump_hdr->timestamp_usec = cpu_to_le32(timestamp.tv_nsec /
					       NSEC_PER_USEC);

	if (!(skbdesc->flags & SKBDESC_DESC_IN_SKB))
		skb_put_data(skbcopy, skbdesc->desc, skbdesc->desc_len);
	skb_put_data(skbcopy, skb->data, skb->len);

	skb_queue_tail(&intf->frame_dump_skbqueue, skbcopy);
	wake_up_interruptible(&intf->frame_dump_waitqueue);

	/*
	 * Verify that the file has not been closed while we were working.
	 */
	if (!test_bit(FRAME_DUMP_FILE_OPEN, &intf->frame_dump_flags))
		skb_queue_purge(&intf->frame_dump_skbqueue);
}
EXPORT_SYMBOL_GPL(rt2x00debug_dump_frame);

static int rt2x00debug_file_open(struct inode *inode, struct file *file)
{
	struct rt2x00debug_intf *intf = inode->i_private;

	file->private_data = inode->i_private;

	if (!try_module_get(intf->debug->owner))
		return -EBUSY;

	return 0;
}

static int rt2x00debug_file_release(struct inode *inode, struct file *file)
{
	struct rt2x00debug_intf *intf = file->private_data;

	module_put(intf->debug->owner);

	return 0;
}

static int rt2x00debug_open_queue_dump(struct inode *inode, struct file *file)
{
	struct rt2x00debug_intf *intf = inode->i_private;
	int retval;

	retval = rt2x00debug_file_open(inode, file);
	if (retval)
		return retval;

	if (test_and_set_bit(FRAME_DUMP_FILE_OPEN, &intf->frame_dump_flags)) {
		rt2x00debug_file_release(inode, file);
		return -EBUSY;
	}

	return 0;
}

static int rt2x00debug_release_queue_dump(struct inode *inode, struct file *file)
{
	struct rt2x00debug_intf *intf = inode->i_private;

	skb_queue_purge(&intf->frame_dump_skbqueue);

	clear_bit(FRAME_DUMP_FILE_OPEN, &intf->frame_dump_flags);

	return rt2x00debug_file_release(inode, file);
}

static ssize_t rt2x00debug_read_queue_dump(struct file *file,
					   char __user *buf,
					   size_t length,
					   loff_t *offset)
{
	struct rt2x00debug_intf *intf = file->private_data;
	struct sk_buff *skb;
	size_t status;
	int retval;

	if (file->f_flags & O_NONBLOCK)
		return -EAGAIN;

	retval =
	    wait_event_interruptible(intf->frame_dump_waitqueue,
				     (skb =
				     skb_dequeue(&intf->frame_dump_skbqueue)));
	if (retval)
		return retval;

	status = min_t(size_t, skb->len, length);
	if (copy_to_user(buf, skb->data, status)) {
		status = -EFAULT;
		goto exit;
	}

	*offset += status;

exit:
	kfree_skb(skb);

	return status;
}

static __poll_t rt2x00debug_poll_queue_dump(struct file *file,
						poll_table *wait)
{
	struct rt2x00debug_intf *intf = file->private_data;

	poll_wait(file, &intf->frame_dump_waitqueue, wait);

	if (!skb_queue_empty(&intf->frame_dump_skbqueue))
		return EPOLLOUT | EPOLLWRNORM;

	return 0;
}

static const struct file_operations rt2x00debug_fop_queue_dump = {
	.owner		= THIS_MODULE,
	.read		= rt2x00debug_read_queue_dump,
	.poll		= rt2x00debug_poll_queue_dump,
	.open		= rt2x00debug_open_queue_dump,
	.release	= rt2x00debug_release_queue_dump,
	.llseek		= default_llseek,
};

static ssize_t rt2x00debug_read_queue_stats(struct file *file,
					    char __user *buf,
					    size_t length,
					    loff_t *offset)
{
	struct rt2x00debug_intf *intf = file->private_data;
	struct data_queue *queue;
	unsigned long irqflags;
	unsigned int lines = 1 + intf->rt2x00dev->data_queues;
	size_t size;
	char *data;
	char *temp;

	if (*offset)
		return 0;

	data = kcalloc(lines, MAX_LINE_LENGTH, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	temp = data +
	    sprintf(data, "qid\tflags\t\tcount\tlimit\tlength\tindex\tdma done\tdone\n");

	queue_for_each(intf->rt2x00dev, queue) {
		spin_lock_irqsave(&queue->index_lock, irqflags);

		temp += sprintf(temp, "%d\t0x%.8x\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
				queue->qid, (unsigned int)queue->flags,
				queue->count, queue->limit, queue->length,
				queue->index[Q_INDEX],
				queue->index[Q_INDEX_DMA_DONE],
				queue->index[Q_INDEX_DONE]);

		spin_unlock_irqrestore(&queue->index_lock, irqflags);
	}

	size = strlen(data);
	size = min(size, length);

	if (copy_to_user(buf, data, size)) {
		kfree(data);
		return -EFAULT;
	}

	kfree(data);

	*offset += size;
	return size;
}

static const struct file_operations rt2x00debug_fop_queue_stats = {
	.owner		= THIS_MODULE,
	.read		= rt2x00debug_read_queue_stats,
	.open		= rt2x00debug_file_open,
	.release	= rt2x00debug_file_release,
	.llseek		= default_llseek,
};

#ifdef CONFIG_RT2X00_LIB_CRYPTO
static ssize_t rt2x00debug_read_crypto_stats(struct file *file,
					     char __user *buf,
					     size_t length,
					     loff_t *offset)
{
	struct rt2x00debug_intf *intf = file->private_data;
	static const char * const name[] = { "WEP64", "WEP128", "TKIP", "AES" };
	char *data;
	char *temp;
	size_t size;
	unsigned int i;

	if (*offset)
		return 0;

	data = kcalloc(1 + CIPHER_MAX, MAX_LINE_LENGTH, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	temp = data;
	temp += sprintf(data, "cipher\tsuccess\ticv err\tmic err\tkey err\n");

	for (i = 0; i < CIPHER_MAX; i++) {
		temp += sprintf(temp, "%s\t%lu\t%lu\t%lu\t%lu\n", name[i],
				intf->crypto_stats[i].success,
				intf->crypto_stats[i].icv_error,
				intf->crypto_stats[i].mic_error,
				intf->crypto_stats[i].key_error);
	}

	size = strlen(data);
	size = min(size, length);

	if (copy_to_user(buf, data, size)) {
		kfree(data);
		return -EFAULT;
	}

	kfree(data);

	*offset += size;
	return size;
}

static const struct file_operations rt2x00debug_fop_crypto_stats = {
	.owner		= THIS_MODULE,
	.read		= rt2x00debug_read_crypto_stats,
	.open		= rt2x00debug_file_open,
	.release	= rt2x00debug_file_release,
	.llseek		= default_llseek,
};
#endif

#define RT2X00DEBUGFS_OPS_READ(__name, __format, __type)	\
static ssize_t rt2x00debug_read_##__name(struct file *file,	\
					 char __user *buf,	\
					 size_t length,		\
					 loff_t *offset)	\
{								\
	struct rt2x00debug_intf *intf = file->private_data;	\
	const struct rt2x00debug *debug = intf->debug;		\
	char line[16];						\
	size_t size;						\
	unsigned int index = intf->offset_##__name;		\
	__type value;						\
								\
	if (*offset)						\
		return 0;					\
								\
	if (index >= debug->__name.word_count)			\
		return -EINVAL;					\
								\
	index += (debug->__name.word_base /			\
		  debug->__name.word_size);			\
								\
	if (debug->__name.flags & RT2X00DEBUGFS_OFFSET)		\
		index *= debug->__name.word_size;		\
								\
	value = debug->__name.read(intf->rt2x00dev, index);	\
								\
	size = sprintf(line, __format, value);			\
								\
	return simple_read_from_buffer(buf, length, offset, line, size); \
}

#define RT2X00DEBUGFS_OPS_WRITE(__name, __type)			\
static ssize_t rt2x00debug_write_##__name(struct file *file,	\
					  const char __user *buf,\
					  size_t length,	\
					  loff_t *offset)	\
{								\
	struct rt2x00debug_intf *intf = file->private_data;	\
	const struct rt2x00debug *debug = intf->debug;		\
	char line[17];						\
	size_t size;						\
	unsigned int index = intf->offset_##__name;		\
	__type value;						\
								\
	if (*offset)						\
		return 0;					\
								\
	if (index >= debug->__name.word_count)			\
		return -EINVAL;					\
								\
	if (length > sizeof(line))				\
		return -EINVAL;					\
								\
	if (copy_from_user(line, buf, length))			\
		return -EFAULT;					\
	line[16] = 0;						\
						\
	size = strlen(line);					\
	value = simple_strtoul(line, NULL, 0);			\
								\
	index += (debug->__name.word_base /			\
		  debug->__name.word_size);			\
								\
	if (debug->__name.flags & RT2X00DEBUGFS_OFFSET)		\
		index *= debug->__name.word_size;		\
								\
	debug->__name.write(intf->rt2x00dev, index, value);	\
								\
	*offset += size;					\
	return size;						\
}

#define RT2X00DEBUGFS_OPS(__name, __format, __type)		\
RT2X00DEBUGFS_OPS_READ(__name, __format, __type);		\
RT2X00DEBUGFS_OPS_WRITE(__name, __type);			\
								\
static const struct file_operations rt2x00debug_fop_##__name = {\
	.owner		= THIS_MODULE,				\
	.read		= rt2x00debug_read_##__name,		\
	.write		= rt2x00debug_write_##__name,		\
	.open		= rt2x00debug_file_open,		\
	.release	= rt2x00debug_file_release,		\
	.llseek		= generic_file_llseek,			\
};

RT2X00DEBUGFS_OPS(csr, "0x%.8x\n", u32);
RT2X00DEBUGFS_OPS(eeprom, "0x%.4x\n", u16);
RT2X00DEBUGFS_OPS(bbp, "0x%.2x\n", u8);
RT2X00DEBUGFS_OPS(rf, "0x%.8x\n", u32);
RT2X00DEBUGFS_OPS(rfcsr, "0x%.2x\n", u8);

static ssize_t rt2x00debug_read_dev_flags(struct file *file,
					  char __user *buf,
					  size_t length,
					  loff_t *offset)
{
	struct rt2x00debug_intf *intf =	file->private_data;
	char line[16];
	size_t size;

	if (*offset)
		return 0;

	size = sprintf(line, "0x%.8x\n", (unsigned int)intf->rt2x00dev->flags);

	return simple_read_from_buffer(buf, length, offset, line, size);
}

static const struct file_operations rt2x00debug_fop_dev_flags = {
	.owner		= THIS_MODULE,
	.read		= rt2x00debug_read_dev_flags,
	.open		= rt2x00debug_file_open,
	.release	= rt2x00debug_file_release,
	.llseek		= default_llseek,
};

static ssize_t rt2x00debug_read_cap_flags(struct file *file,
					  char __user *buf,
					  size_t length,
					  loff_t *offset)
{
	struct rt2x00debug_intf *intf =	file->private_data;
	char line[16];
	size_t size;

	if (*offset)
		return 0;

	size = sprintf(line, "0x%.8x\n", (unsigned int)intf->rt2x00dev->cap_flags);

	return simple_read_from_buffer(buf, length, offset, line, size);
}

static const struct file_operations rt2x00debug_fop_cap_flags = {
	.owner		= THIS_MODULE,
	.read		= rt2x00debug_read_cap_flags,
	.open		= rt2x00debug_file_open,
	.release	= rt2x00debug_file_release,
	.llseek		= default_llseek,
};

static ssize_t rt2x00debug_write_restart_hw(struct file *file,
					    const char __user *buf,
					    size_t length,
					    loff_t *offset)
{
	struct rt2x00debug_intf *intf =	file->private_data;
	struct rt2x00_dev *rt2x00dev = intf->rt2x00dev;
	static unsigned long last_reset = INITIAL_JIFFIES;

	if (!rt2x00_has_cap_restart_hw(rt2x00dev))
		return -EOPNOTSUPP;

	if (time_before(jiffies, last_reset + msecs_to_jiffies(2000)))
		return -EBUSY;

	last_reset = jiffies;

	ieee80211_restart_hw(rt2x00dev->hw);
	return length;
}

static const struct file_operations rt2x00debug_restart_hw = {
	.owner = THIS_MODULE,
	.write = rt2x00debug_write_restart_hw,
	.open = simple_open,
	.llseek = generic_file_llseek,
};

static void rt2x00debug_create_file_driver(const char *name,
					   struct rt2x00debug_intf *intf,
					   struct debugfs_blob_wrapper *blob)
{
	char *data;

	data = kzalloc(3 * MAX_LINE_LENGTH, GFP_KERNEL);
	if (!data)
		return;

	blob->data = data;
	data += sprintf(data, "driver:\t%s\n", intf->rt2x00dev->ops->name);
	data += sprintf(data, "version:\t%s\n", DRV_VERSION);
	blob->size = strlen(blob->data);

	debugfs_create_blob(name, 0400, intf->driver_folder, blob);
}

static void rt2x00debug_create_file_chipset(const char *name,
					    struct rt2x00debug_intf *intf,
					    struct debugfs_blob_wrapper *blob)
{
	const struct rt2x00debug *debug = intf->debug;
	char *data;

	data = kzalloc(9 * MAX_LINE_LENGTH, GFP_KERNEL);
	if (!data)
		return;

	blob->data = data;
	data += sprintf(data, "rt chip:\t%04x\n", intf->rt2x00dev->chip.rt);
	data += sprintf(data, "rf chip:\t%04x\n", intf->rt2x00dev->chip.rf);
	data += sprintf(data, "revision:\t%04x\n", intf->rt2x00dev->chip.rev);
	data += sprintf(data, "\n");
	data += sprintf(data, "register\tbase\twords\twordsize\n");
#define RT2X00DEBUGFS_SPRINTF_REGISTER(__name)			\
{								\
	if (debug->__name.read)					\
		data += sprintf(data, __stringify(__name)	\
				"\t%d\t%d\t%d\n",		\
				debug->__name.word_base,	\
				debug->__name.word_count,	\
				debug->__name.word_size);	\
}
	RT2X00DEBUGFS_SPRINTF_REGISTER(csr);
	RT2X00DEBUGFS_SPRINTF_REGISTER(eeprom);
	RT2X00DEBUGFS_SPRINTF_REGISTER(bbp);
	RT2X00DEBUGFS_SPRINTF_REGISTER(rf);
	RT2X00DEBUGFS_SPRINTF_REGISTER(rfcsr);
#undef RT2X00DEBUGFS_SPRINTF_REGISTER

	blob->size = strlen(blob->data);

	debugfs_create_blob(name, 0400, intf->driver_folder, blob);
}

void rt2x00debug_register(struct rt2x00_dev *rt2x00dev)
{
	const struct rt2x00debug *debug = rt2x00dev->ops->debugfs;
	struct rt2x00debug_intf *intf;
	struct dentry *queue_folder;
	struct dentry *register_folder;

	intf = kzalloc(sizeof(struct rt2x00debug_intf), GFP_KERNEL);
	if (!intf) {
		rt2x00_err(rt2x00dev, "Failed to allocate debug handler\n");
		return;
	}

	intf->debug = debug;
	intf->rt2x00dev = rt2x00dev;
	rt2x00dev->debugfs_intf = intf;

	intf->driver_folder =
	    debugfs_create_dir(intf->rt2x00dev->ops->name,
			       rt2x00dev->hw->wiphy->debugfsdir);

	rt2x00debug_create_file_driver("driver", intf, &intf->driver_blob);
	rt2x00debug_create_file_chipset("chipset", intf, &intf->chipset_blob);
	debugfs_create_file("dev_flags", 0400, intf->driver_folder, intf,
			    &rt2x00debug_fop_dev_flags);
	debugfs_create_file("cap_flags", 0400, intf->driver_folder, intf,
			    &rt2x00debug_fop_cap_flags);
	debugfs_create_file("restart_hw", 0200, intf->driver_folder, intf,
			    &rt2x00debug_restart_hw);

	register_folder = debugfs_create_dir("register", intf->driver_folder);

#define RT2X00DEBUGFS_CREATE_REGISTER_ENTRY(__intf, __name)		\
({									\
	if (debug->__name.read) {					\
		debugfs_create_u32(__stringify(__name) "_offset", 0600,	\
				   register_folder,			\
				   &(__intf)->offset_##__name);		\
									\
		debugfs_create_file(__stringify(__name) "_value", 0600,	\
				    register_folder, (__intf),		\
				    &rt2x00debug_fop_##__name);		\
	}								\
})

	RT2X00DEBUGFS_CREATE_REGISTER_ENTRY(intf, csr);
	RT2X00DEBUGFS_CREATE_REGISTER_ENTRY(intf, eeprom);
	RT2X00DEBUGFS_CREATE_REGISTER_ENTRY(intf, bbp);
	RT2X00DEBUGFS_CREATE_REGISTER_ENTRY(intf, rf);
	RT2X00DEBUGFS_CREATE_REGISTER_ENTRY(intf, rfcsr);

#undef RT2X00DEBUGFS_CREATE_REGISTER_ENTRY

	queue_folder = debugfs_create_dir("queue", intf->driver_folder);

	debugfs_create_file("dump", 0400, queue_folder, intf,
			    &rt2x00debug_fop_queue_dump);

	skb_queue_head_init(&intf->frame_dump_skbqueue);
	init_waitqueue_head(&intf->frame_dump_waitqueue);

	debugfs_create_file("queue", 0400, queue_folder, intf,
			    &rt2x00debug_fop_queue_stats);

#ifdef CONFIG_RT2X00_LIB_CRYPTO
	if (rt2x00_has_cap_hw_crypto(rt2x00dev))
		debugfs_create_file("crypto", 0444, queue_folder, intf,
				    &rt2x00debug_fop_crypto_stats);
#endif

	return;
}

void rt2x00debug_deregister(struct rt2x00_dev *rt2x00dev)
{
	struct rt2x00debug_intf *intf = rt2x00dev->debugfs_intf;

	if (unlikely(!intf))
		return;

	skb_queue_purge(&intf->frame_dump_skbqueue);

	debugfs_remove_recursive(intf->driver_folder);
	kfree(intf->chipset_blob.data);
	kfree(intf->driver_blob.data);
	kfree(intf);

	rt2x00dev->debugfs_intf = NULL;
}

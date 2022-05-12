// SPDX-License-Identifier: GPL-2.0-only
/*
 * dell_rbu.c
 * Bios Update driver for Dell systems
 * Author: Dell Inc
 *         Abhay Salunke <abhay_salunke@dell.com>
 *
 * Copyright (C) 2005 Dell Inc.
 *
 * Remote BIOS Update (rbu) driver is used for updating DELL BIOS by
 * creating entries in the /sys file systems on Linux 2.6 and higher
 * kernels. The driver supports two mechanism to update the BIOS namely
 * contiguous and packetized. Both these methods still require having some
 * application to set the CMOS bit indicating the BIOS to update itself
 * after a reboot.
 *
 * Contiguous method:
 * This driver writes the incoming data in a monolithic image by allocating
 * contiguous physical pages large enough to accommodate the incoming BIOS
 * image size.
 *
 * Packetized method:
 * The driver writes the incoming packet image by allocating a new packet
 * on every time the packet data is written. This driver requires an
 * application to break the BIOS image in to fixed sized packet chunks.
 *
 * See Documentation/admin-guide/dell_rbu.rst for more info.
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/blkdev.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/moduleparam.h>
#include <linux/firmware.h>
#include <linux/dma-mapping.h>
#include <asm/set_memory.h>

MODULE_AUTHOR("Abhay Salunke <abhay_salunke@dell.com>");
MODULE_DESCRIPTION("Driver for updating BIOS image on DELL systems");
MODULE_LICENSE("GPL");
MODULE_VERSION("3.2");

#define BIOS_SCAN_LIMIT 0xffffffff
#define MAX_IMAGE_LENGTH 16
static struct _rbu_data {
	void *image_update_buffer;
	unsigned long image_update_buffer_size;
	unsigned long bios_image_size;
	int image_update_ordernum;
	spinlock_t lock;
	unsigned long packet_read_count;
	unsigned long num_packets;
	unsigned long packetsize;
	unsigned long imagesize;
	int entry_created;
} rbu_data;

static char image_type[MAX_IMAGE_LENGTH + 1] = "mono";
module_param_string(image_type, image_type, sizeof (image_type), 0);
MODULE_PARM_DESC(image_type, "BIOS image type. choose- mono or packet or init");

static unsigned long allocation_floor = 0x100000;
module_param(allocation_floor, ulong, 0644);
MODULE_PARM_DESC(allocation_floor, "Minimum address for allocations when using Packet mode");

struct packet_data {
	struct list_head list;
	size_t length;
	void *data;
	int ordernum;
};

static struct packet_data packet_data_head;

static struct platform_device *rbu_device;
static int context;

static void init_packet_head(void)
{
	INIT_LIST_HEAD(&packet_data_head.list);
	rbu_data.packet_read_count = 0;
	rbu_data.num_packets = 0;
	rbu_data.packetsize = 0;
	rbu_data.imagesize = 0;
}

static int create_packet(void *data, size_t length)
{
	struct packet_data *newpacket;
	int ordernum = 0;
	int retval = 0;
	unsigned int packet_array_size = 0;
	void **invalid_addr_packet_array = NULL;
	void *packet_data_temp_buf = NULL;
	unsigned int idx = 0;

	pr_debug("entry\n");

	if (!rbu_data.packetsize) {
		pr_debug("packetsize not specified\n");
		retval = -EINVAL;
		goto out_noalloc;
	}

	spin_unlock(&rbu_data.lock);

	newpacket = kzalloc(sizeof (struct packet_data), GFP_KERNEL);

	if (!newpacket) {
		pr_warn("failed to allocate new packet\n");
		retval = -ENOMEM;
		spin_lock(&rbu_data.lock);
		goto out_noalloc;
	}

	ordernum = get_order(length);

	/*
	 * BIOS errata mean we cannot allocate packets below 1MB or they will
	 * be overwritten by BIOS.
	 *
	 * array to temporarily hold packets
	 * that are below the allocation floor
	 *
	 * NOTE: very simplistic because we only need the floor to be at 1MB
	 *       due to BIOS errata. This shouldn't be used for higher floors
	 *       or you will run out of mem trying to allocate the array.
	 */
	packet_array_size = max_t(unsigned int, allocation_floor / rbu_data.packetsize, 1);
	invalid_addr_packet_array = kcalloc(packet_array_size, sizeof(void *),
						GFP_KERNEL);

	if (!invalid_addr_packet_array) {
		pr_warn("failed to allocate invalid_addr_packet_array\n");
		retval = -ENOMEM;
		spin_lock(&rbu_data.lock);
		goto out_alloc_packet;
	}

	while (!packet_data_temp_buf) {
		packet_data_temp_buf = (unsigned char *)
			__get_free_pages(GFP_KERNEL, ordernum);
		if (!packet_data_temp_buf) {
			pr_warn("failed to allocate new packet\n");
			retval = -ENOMEM;
			spin_lock(&rbu_data.lock);
			goto out_alloc_packet_array;
		}

		if ((unsigned long)virt_to_phys(packet_data_temp_buf)
				< allocation_floor) {
			pr_debug("packet 0x%lx below floor at 0x%lx\n",
					(unsigned long)virt_to_phys(
						packet_data_temp_buf),
					allocation_floor);
			invalid_addr_packet_array[idx++] = packet_data_temp_buf;
			packet_data_temp_buf = NULL;
		}
	}
	/*
	 * set to uncachable or it may never get written back before reboot
	 */
	set_memory_uc((unsigned long)packet_data_temp_buf, 1 << ordernum);

	spin_lock(&rbu_data.lock);

	newpacket->data = packet_data_temp_buf;

	pr_debug("newpacket at physical addr %lx\n",
		(unsigned long)virt_to_phys(newpacket->data));

	/* packets may not have fixed size */
	newpacket->length = length;
	newpacket->ordernum = ordernum;
	++rbu_data.num_packets;

	/* initialize the newly created packet headers */
	INIT_LIST_HEAD(&newpacket->list);
	list_add_tail(&newpacket->list, &packet_data_head.list);

	memcpy(newpacket->data, data, length);

	pr_debug("exit\n");

out_alloc_packet_array:
	/* always free packet array */
	while (idx--) {
		pr_debug("freeing unused packet below floor 0x%lx\n",
			(unsigned long)virt_to_phys(invalid_addr_packet_array[idx]));
		free_pages((unsigned long)invalid_addr_packet_array[idx], ordernum);
	}
	kfree(invalid_addr_packet_array);

out_alloc_packet:
	/* if error, free data */
	if (retval)
		kfree(newpacket);

out_noalloc:
	return retval;
}

static int packetize_data(const u8 *data, size_t length)
{
	int rc = 0;
	int done = 0;
	int packet_length;
	u8 *temp;
	u8 *end = (u8 *) data + length;
	pr_debug("data length %zd\n", length);
	if (!rbu_data.packetsize) {
		pr_warn("packetsize not specified\n");
		return -EIO;
	}

	temp = (u8 *) data;

	/* packetize the hunk */
	while (!done) {
		if ((temp + rbu_data.packetsize) < end)
			packet_length = rbu_data.packetsize;
		else {
			/* this is the last packet */
			packet_length = end - temp;
			done = 1;
		}

		if ((rc = create_packet(temp, packet_length)))
			return rc;

		pr_debug("%p:%td\n", temp, (end - temp));
		temp += packet_length;
	}

	rbu_data.imagesize = length;

	return rc;
}

static int do_packet_read(char *data, struct packet_data *newpacket,
	int length, int bytes_read, int *list_read_count)
{
	void *ptemp_buf;
	int bytes_copied = 0;
	int j = 0;

	*list_read_count += newpacket->length;

	if (*list_read_count > bytes_read) {
		/* point to the start of unread data */
		j = newpacket->length - (*list_read_count - bytes_read);
		/* point to the offset in the packet buffer */
		ptemp_buf = (u8 *) newpacket->data + j;
		/*
		 * check if there is enough room in
		 * * the incoming buffer
		 */
		if (length > (*list_read_count - bytes_read))
			/*
			 * copy what ever is there in this
			 * packet and move on
			 */
			bytes_copied = (*list_read_count - bytes_read);
		else
			/* copy the remaining */
			bytes_copied = length;
		memcpy(data, ptemp_buf, bytes_copied);
	}
	return bytes_copied;
}

static int packet_read_list(char *data, size_t * pread_length)
{
	struct packet_data *newpacket;
	int temp_count = 0;
	int bytes_copied = 0;
	int bytes_read = 0;
	int remaining_bytes = 0;
	char *pdest = data;

	/* check if we have any packets */
	if (0 == rbu_data.num_packets)
		return -ENOMEM;

	remaining_bytes = *pread_length;
	bytes_read = rbu_data.packet_read_count;

	list_for_each_entry(newpacket, (&packet_data_head.list)->next, list) {
		bytes_copied = do_packet_read(pdest, newpacket,
			remaining_bytes, bytes_read, &temp_count);
		remaining_bytes -= bytes_copied;
		bytes_read += bytes_copied;
		pdest += bytes_copied;
		/*
		 * check if we reached end of buffer before reaching the
		 * last packet
		 */
		if (remaining_bytes == 0)
			break;
	}
	/*finally set the bytes read */
	*pread_length = bytes_read - rbu_data.packet_read_count;
	rbu_data.packet_read_count = bytes_read;
	return 0;
}

static void packet_empty_list(void)
{
	struct packet_data *newpacket, *tmp;

	list_for_each_entry_safe(newpacket, tmp, (&packet_data_head.list)->next, list) {
		list_del(&newpacket->list);

		/*
		 * zero out the RBU packet memory before freeing
		 * to make sure there are no stale RBU packets left in memory
		 */
		memset(newpacket->data, 0, rbu_data.packetsize);
		set_memory_wb((unsigned long)newpacket->data,
			1 << newpacket->ordernum);
		free_pages((unsigned long) newpacket->data,
			newpacket->ordernum);
		kfree(newpacket);
	}
	rbu_data.packet_read_count = 0;
	rbu_data.num_packets = 0;
	rbu_data.imagesize = 0;
}

/*
 * img_update_free: Frees the buffer allocated for storing BIOS image
 * Always called with lock held and returned with lock held
 */
static void img_update_free(void)
{
	if (!rbu_data.image_update_buffer)
		return;
	/*
	 * zero out this buffer before freeing it to get rid of any stale
	 * BIOS image copied in memory.
	 */
	memset(rbu_data.image_update_buffer, 0,
		rbu_data.image_update_buffer_size);
	free_pages((unsigned long) rbu_data.image_update_buffer,
		rbu_data.image_update_ordernum);

	/*
	 * Re-initialize the rbu_data variables after a free
	 */
	rbu_data.image_update_ordernum = -1;
	rbu_data.image_update_buffer = NULL;
	rbu_data.image_update_buffer_size = 0;
	rbu_data.bios_image_size = 0;
}

/*
 * img_update_realloc: This function allocates the contiguous pages to
 * accommodate the requested size of data. The memory address and size
 * values are stored globally and on every call to this function the new
 * size is checked to see if more data is required than the existing size.
 * If true the previous memory is freed and new allocation is done to
 * accommodate the new size. If the incoming size is less then than the
 * already allocated size, then that memory is reused. This function is
 * called with lock held and returns with lock held.
 */
static int img_update_realloc(unsigned long size)
{
	unsigned char *image_update_buffer = NULL;
	unsigned long img_buf_phys_addr;
	int ordernum;

	/*
	 * check if the buffer of sufficient size has been
	 * already allocated
	 */
	if (rbu_data.image_update_buffer_size >= size) {
		/*
		 * check for corruption
		 */
		if ((size != 0) && (rbu_data.image_update_buffer == NULL)) {
			pr_err("corruption check failed\n");
			return -EINVAL;
		}
		/*
		 * we have a valid pre-allocated buffer with
		 * sufficient size
		 */
		return 0;
	}

	/*
	 * free any previously allocated buffer
	 */
	img_update_free();

	spin_unlock(&rbu_data.lock);

	ordernum = get_order(size);
	image_update_buffer =
		(unsigned char *)__get_free_pages(GFP_DMA32, ordernum);
	spin_lock(&rbu_data.lock);
	if (!image_update_buffer) {
		pr_debug("Not enough memory for image update: size = %ld\n", size);
		return -ENOMEM;
	}

	img_buf_phys_addr = (unsigned long)virt_to_phys(image_update_buffer);
	if (WARN_ON_ONCE(img_buf_phys_addr > BIOS_SCAN_LIMIT))
		return -EINVAL; /* can't happen per definition */

	rbu_data.image_update_buffer = image_update_buffer;
	rbu_data.image_update_buffer_size = size;
	rbu_data.bios_image_size = rbu_data.image_update_buffer_size;
	rbu_data.image_update_ordernum = ordernum;
	return 0;
}

static ssize_t read_packet_data(char *buffer, loff_t pos, size_t count)
{
	int retval;
	size_t bytes_left;
	size_t data_length;
	char *ptempBuf = buffer;

	/* check to see if we have something to return */
	if (rbu_data.num_packets == 0) {
		pr_debug("no packets written\n");
		retval = -ENOMEM;
		goto read_rbu_data_exit;
	}

	if (pos > rbu_data.imagesize) {
		retval = 0;
		pr_warn("data underrun\n");
		goto read_rbu_data_exit;
	}

	bytes_left = rbu_data.imagesize - pos;
	data_length = min(bytes_left, count);

	if ((retval = packet_read_list(ptempBuf, &data_length)) < 0)
		goto read_rbu_data_exit;

	if ((pos + count) > rbu_data.imagesize) {
		rbu_data.packet_read_count = 0;
		/* this was the last copy */
		retval = bytes_left;
	} else
		retval = count;

      read_rbu_data_exit:
	return retval;
}

static ssize_t read_rbu_mono_data(char *buffer, loff_t pos, size_t count)
{
	/* check to see if we have something to return */
	if ((rbu_data.image_update_buffer == NULL) ||
		(rbu_data.bios_image_size == 0)) {
		pr_debug("image_update_buffer %p, bios_image_size %lu\n",
			rbu_data.image_update_buffer,
			rbu_data.bios_image_size);
		return -ENOMEM;
	}

	return memory_read_from_buffer(buffer, count, &pos,
			rbu_data.image_update_buffer, rbu_data.bios_image_size);
}

static ssize_t data_read(struct file *filp, struct kobject *kobj,
			 struct bin_attribute *bin_attr,
			 char *buffer, loff_t pos, size_t count)
{
	ssize_t ret_count = 0;

	spin_lock(&rbu_data.lock);

	if (!strcmp(image_type, "mono"))
		ret_count = read_rbu_mono_data(buffer, pos, count);
	else if (!strcmp(image_type, "packet"))
		ret_count = read_packet_data(buffer, pos, count);
	else
		pr_debug("invalid image type specified\n");

	spin_unlock(&rbu_data.lock);
	return ret_count;
}
static BIN_ATTR_RO(data, 0);

static void callbackfn_rbu(const struct firmware *fw, void *context)
{
	rbu_data.entry_created = 0;

	if (!fw)
		return;

	if (!fw->size)
		goto out;

	spin_lock(&rbu_data.lock);
	if (!strcmp(image_type, "mono")) {
		if (!img_update_realloc(fw->size))
			memcpy(rbu_data.image_update_buffer,
				fw->data, fw->size);
	} else if (!strcmp(image_type, "packet")) {
		/*
		 * we need to free previous packets if a
		 * new hunk of packets needs to be downloaded
		 */
		packet_empty_list();
		if (packetize_data(fw->data, fw->size))
			/* Incase something goes wrong when we are
			 * in middle of packetizing the data, we
			 * need to free up whatever packets might
			 * have been created before we quit.
			 */
			packet_empty_list();
	} else
		pr_debug("invalid image type specified\n");
	spin_unlock(&rbu_data.lock);
 out:
	release_firmware(fw);
}

static ssize_t image_type_read(struct file *filp, struct kobject *kobj,
			       struct bin_attribute *bin_attr,
			       char *buffer, loff_t pos, size_t count)
{
	int size = 0;
	if (!pos)
		size = scnprintf(buffer, count, "%s\n", image_type);
	return size;
}

static ssize_t image_type_write(struct file *filp, struct kobject *kobj,
				struct bin_attribute *bin_attr,
				char *buffer, loff_t pos, size_t count)
{
	int rc = count;
	int req_firm_rc = 0;
	int i;
	spin_lock(&rbu_data.lock);
	/*
	 * Find the first newline or space
	 */
	for (i = 0; i < count; ++i)
		if (buffer[i] == '\n' || buffer[i] == ' ') {
			buffer[i] = '\0';
			break;
		}
	if (i == count)
		buffer[count] = '\0';

	if (strstr(buffer, "mono"))
		strcpy(image_type, "mono");
	else if (strstr(buffer, "packet"))
		strcpy(image_type, "packet");
	else if (strstr(buffer, "init")) {
		/*
		 * If due to the user error the driver gets in a bad
		 * state where even though it is loaded , the
		 * /sys/class/firmware/dell_rbu entries are missing.
		 * to cover this situation the user can recreate entries
		 * by writing init to image_type.
		 */
		if (!rbu_data.entry_created) {
			spin_unlock(&rbu_data.lock);
			req_firm_rc = request_firmware_nowait(THIS_MODULE,
				FW_ACTION_NOHOTPLUG, "dell_rbu",
				&rbu_device->dev, GFP_KERNEL, &context,
				callbackfn_rbu);
			if (req_firm_rc) {
				pr_err("request_firmware_nowait failed %d\n", rc);
				rc = -EIO;
			} else
				rbu_data.entry_created = 1;

			spin_lock(&rbu_data.lock);
		}
	} else {
		pr_warn("image_type is invalid\n");
		spin_unlock(&rbu_data.lock);
		return -EINVAL;
	}

	/* we must free all previous allocations */
	packet_empty_list();
	img_update_free();
	spin_unlock(&rbu_data.lock);

	return rc;
}
static BIN_ATTR_RW(image_type, 0);

static ssize_t packet_size_read(struct file *filp, struct kobject *kobj,
				struct bin_attribute *bin_attr,
				char *buffer, loff_t pos, size_t count)
{
	int size = 0;
	if (!pos) {
		spin_lock(&rbu_data.lock);
		size = scnprintf(buffer, count, "%lu\n", rbu_data.packetsize);
		spin_unlock(&rbu_data.lock);
	}
	return size;
}

static ssize_t packet_size_write(struct file *filp, struct kobject *kobj,
				 struct bin_attribute *bin_attr,
				 char *buffer, loff_t pos, size_t count)
{
	unsigned long temp;
	spin_lock(&rbu_data.lock);
	packet_empty_list();
	sscanf(buffer, "%lu", &temp);
	if (temp < 0xffffffff)
		rbu_data.packetsize = temp;

	spin_unlock(&rbu_data.lock);
	return count;
}
static BIN_ATTR_RW(packet_size, 0);

static struct bin_attribute *rbu_bin_attrs[] = {
	&bin_attr_data,
	&bin_attr_image_type,
	&bin_attr_packet_size,
	NULL
};

static const struct attribute_group rbu_group = {
	.bin_attrs = rbu_bin_attrs,
};

static int __init dcdrbu_init(void)
{
	int rc;
	spin_lock_init(&rbu_data.lock);

	init_packet_head();
	rbu_device = platform_device_register_simple("dell_rbu", -1, NULL, 0);
	if (IS_ERR(rbu_device)) {
		pr_err("platform_device_register_simple failed\n");
		return PTR_ERR(rbu_device);
	}

	rc = sysfs_create_group(&rbu_device->dev.kobj, &rbu_group);
	if (rc)
		goto out_devreg;

	rbu_data.entry_created = 0;
	return 0;

out_devreg:
	platform_device_unregister(rbu_device);
	return rc;
}

static __exit void dcdrbu_exit(void)
{
	spin_lock(&rbu_data.lock);
	packet_empty_list();
	img_update_free();
	spin_unlock(&rbu_data.lock);
	sysfs_remove_group(&rbu_device->dev.kobj, &rbu_group);
	platform_device_unregister(rbu_device);
}

module_exit(dcdrbu_exit);
module_init(dcdrbu_init);

// SPDX-License-Identifier: GPL-2.0-only
/*
 * DDR PHY Front End (DPFE) driver for Broadcom set top box SoCs
 *
 * Copyright (c) 2017 Broadcom
 */

/*
 * This driver provides access to the DPFE interface of Broadcom STB SoCs.
 * The firmware running on the DCPU inside the DDR PHY can provide current
 * information about the system's RAM, for instance the DRAM refresh rate.
 * This can be used as an indirect indicator for the DRAM's temperature.
 * Slower refresh rate means cooler RAM, higher refresh rate means hotter
 * RAM.
 *
 * Throughout the driver, we use readl_relaxed() and writel_relaxed(), which
 * already contain the appropriate le32_to_cpu()/cpu_to_le32() calls.
 *
 * Note regarding the loading of the firmware image: we use be32_to_cpu()
 * and le_32_to_cpu(), so we can support the following four cases:
 *     - LE kernel + LE firmware image (the most common case)
 *     - LE kernel + BE firmware image
 *     - BE kernel + LE firmware image
 *     - BE kernel + BE firmware image
 *
 * The DPCU always runs in big endian mode. The firmware image, however, can
 * be in either format. Also, communication between host CPU and DCPU is
 * always in little endian.
 */

#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

#define DRVNAME			"brcmstb-dpfe"

/* DCPU register offsets */
#define REG_DCPU_RESET		0x0
#define REG_TO_DCPU_MBOX	0x10
#define REG_TO_HOST_MBOX	0x14

/* Macros to process offsets returned by the DCPU */
#define DRAM_MSG_ADDR_OFFSET	0x0
#define DRAM_MSG_TYPE_OFFSET	0x1c
#define DRAM_MSG_ADDR_MASK	((1UL << DRAM_MSG_TYPE_OFFSET) - 1)
#define DRAM_MSG_TYPE_MASK	((1UL << \
				 (BITS_PER_LONG - DRAM_MSG_TYPE_OFFSET)) - 1)

/* Message RAM */
#define DCPU_MSG_RAM_START	0x100
#define DCPU_MSG_RAM(x)		(DCPU_MSG_RAM_START + (x) * sizeof(u32))

/* DRAM Info Offsets & Masks */
#define DRAM_INFO_INTERVAL	0x0
#define DRAM_INFO_MR4		0x4
#define DRAM_INFO_ERROR		0x8
#define DRAM_INFO_MR4_MASK	0xff
#define DRAM_INFO_MR4_SHIFT	24	/* We need to look at byte 3 */

/* DRAM MR4 Offsets & Masks */
#define DRAM_MR4_REFRESH	0x0	/* Refresh rate */
#define DRAM_MR4_SR_ABORT	0x3	/* Self Refresh Abort */
#define DRAM_MR4_PPRE		0x4	/* Post-package repair entry/exit */
#define DRAM_MR4_TH_OFFS	0x5	/* Thermal Offset; vendor specific */
#define DRAM_MR4_TUF		0x7	/* Temperature Update Flag */

#define DRAM_MR4_REFRESH_MASK	0x7
#define DRAM_MR4_SR_ABORT_MASK	0x1
#define DRAM_MR4_PPRE_MASK	0x1
#define DRAM_MR4_TH_OFFS_MASK	0x3
#define DRAM_MR4_TUF_MASK	0x1

/* DRAM Vendor Offsets & Masks (API v2) */
#define DRAM_VENDOR_MR5		0x0
#define DRAM_VENDOR_MR6		0x4
#define DRAM_VENDOR_MR7		0x8
#define DRAM_VENDOR_MR8		0xc
#define DRAM_VENDOR_ERROR	0x10
#define DRAM_VENDOR_MASK	0xff
#define DRAM_VENDOR_SHIFT	24	/* We need to look at byte 3 */

/* DRAM Information Offsets & Masks (API v3) */
#define DRAM_DDR_INFO_MR4	0x0
#define DRAM_DDR_INFO_MR5	0x4
#define DRAM_DDR_INFO_MR6	0x8
#define DRAM_DDR_INFO_MR7	0xc
#define DRAM_DDR_INFO_MR8	0x10
#define DRAM_DDR_INFO_ERROR	0x14
#define DRAM_DDR_INFO_MASK	0xff

/* Reset register bits & masks */
#define DCPU_RESET_SHIFT	0x0
#define DCPU_RESET_MASK		0x1
#define DCPU_CLK_DISABLE_SHIFT	0x2

/* DCPU return codes */
#define DCPU_RET_ERROR_BIT	BIT(31)
#define DCPU_RET_SUCCESS	0x1
#define DCPU_RET_ERR_HEADER	(DCPU_RET_ERROR_BIT | BIT(0))
#define DCPU_RET_ERR_INVAL	(DCPU_RET_ERROR_BIT | BIT(1))
#define DCPU_RET_ERR_CHKSUM	(DCPU_RET_ERROR_BIT | BIT(2))
#define DCPU_RET_ERR_COMMAND	(DCPU_RET_ERROR_BIT | BIT(3))
/* This error code is not firmware defined and only used in the driver. */
#define DCPU_RET_ERR_TIMEDOUT	(DCPU_RET_ERROR_BIT | BIT(4))

/* Firmware magic */
#define DPFE_BE_MAGIC		0xfe1010fe
#define DPFE_LE_MAGIC		0xfe0101fe

/* Error codes */
#define ERR_INVALID_MAGIC	-1
#define ERR_INVALID_SIZE	-2
#define ERR_INVALID_CHKSUM	-3

/* Message types */
#define DPFE_MSG_TYPE_COMMAND	1
#define DPFE_MSG_TYPE_RESPONSE	2

#define DELAY_LOOP_MAX		1000

enum dpfe_msg_fields {
	MSG_HEADER,
	MSG_COMMAND,
	MSG_ARG_COUNT,
	MSG_ARG0,
	MSG_FIELD_MAX	= 16 /* Max number of arguments */
};

enum dpfe_commands {
	DPFE_CMD_GET_INFO,
	DPFE_CMD_GET_REFRESH,
	DPFE_CMD_GET_VENDOR,
	DPFE_CMD_MAX /* Last entry */
};

/*
 * Format of the binary firmware file:
 *
 *   entry
 *      0    header
 *              value:  0xfe0101fe  <== little endian
 *                      0xfe1010fe  <== big endian
 *      1    sequence:
 *              [31:16] total segments on this build
 *              [15:0]  this segment sequence.
 *      2    FW version
 *      3    IMEM byte size
 *      4    DMEM byte size
 *           IMEM
 *           DMEM
 *      last checksum ==> sum of everything
 */
struct dpfe_firmware_header {
	u32 magic;
	u32 sequence;
	u32 version;
	u32 imem_size;
	u32 dmem_size;
};

/* Things we only need during initialization. */
struct init_data {
	unsigned int dmem_len;
	unsigned int imem_len;
	unsigned int chksum;
	bool is_big_endian;
};

/* API version and corresponding commands */
struct dpfe_api {
	int version;
	const char *fw_name;
	const struct attribute_group **sysfs_attrs;
	u32 command[DPFE_CMD_MAX][MSG_FIELD_MAX];
};

/* Things we need for as long as we are active. */
struct brcmstb_dpfe_priv {
	void __iomem *regs;
	void __iomem *dmem;
	void __iomem *imem;
	struct device *dev;
	const struct dpfe_api *dpfe_api;
	struct mutex lock;
};

/*
 * Forward declaration of our sysfs attribute functions, so we can declare the
 * attribute data structures early.
 */
static ssize_t show_info(struct device *, struct device_attribute *, char *);
static ssize_t show_refresh(struct device *, struct device_attribute *, char *);
static ssize_t store_refresh(struct device *, struct device_attribute *,
			  const char *, size_t);
static ssize_t show_vendor(struct device *, struct device_attribute *, char *);
static ssize_t show_dram(struct device *, struct device_attribute *, char *);

/*
 * Declare our attributes early, so they can be referenced in the API data
 * structure. We need to do this, because the attributes depend on the API
 * version.
 */
static DEVICE_ATTR(dpfe_info, 0444, show_info, NULL);
static DEVICE_ATTR(dpfe_refresh, 0644, show_refresh, store_refresh);
static DEVICE_ATTR(dpfe_vendor, 0444, show_vendor, NULL);
static DEVICE_ATTR(dpfe_dram, 0444, show_dram, NULL);

/* API v2 sysfs attributes */
static struct attribute *dpfe_v2_attrs[] = {
	&dev_attr_dpfe_info.attr,
	&dev_attr_dpfe_refresh.attr,
	&dev_attr_dpfe_vendor.attr,
	NULL
};
ATTRIBUTE_GROUPS(dpfe_v2);

/* API v3 sysfs attributes */
static struct attribute *dpfe_v3_attrs[] = {
	&dev_attr_dpfe_info.attr,
	&dev_attr_dpfe_dram.attr,
	NULL
};
ATTRIBUTE_GROUPS(dpfe_v3);

/*
 * Old API v2 firmware commands, as defined in the rev 0.61 specification, we
 * use a version set to 1 to denote that it is not compatible with the new API
 * v2 and onwards.
 */
static const struct dpfe_api dpfe_api_old_v2 = {
	.version = 1,
	.fw_name = "dpfe.bin",
	.sysfs_attrs = dpfe_v2_groups,
	.command = {
		[DPFE_CMD_GET_INFO] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 1,
			[MSG_ARG_COUNT] = 1,
			[MSG_ARG0] = 1,
		},
		[DPFE_CMD_GET_REFRESH] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 2,
			[MSG_ARG_COUNT] = 1,
			[MSG_ARG0] = 1,
		},
		[DPFE_CMD_GET_VENDOR] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 2,
			[MSG_ARG_COUNT] = 1,
			[MSG_ARG0] = 2,
		},
	}
};

/*
 * API v2 firmware commands, as defined in the rev 0.8 specification, named new
 * v2 here
 */
static const struct dpfe_api dpfe_api_new_v2 = {
	.version = 2,
	.fw_name = NULL, /* We expect the firmware to have been downloaded! */
	.sysfs_attrs = dpfe_v2_groups,
	.command = {
		[DPFE_CMD_GET_INFO] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 0x101,
		},
		[DPFE_CMD_GET_REFRESH] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 0x201,
		},
		[DPFE_CMD_GET_VENDOR] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 0x202,
		},
	}
};

/* API v3 firmware commands */
static const struct dpfe_api dpfe_api_v3 = {
	.version = 3,
	.fw_name = NULL, /* We expect the firmware to have been downloaded! */
	.sysfs_attrs = dpfe_v3_groups,
	.command = {
		[DPFE_CMD_GET_INFO] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 0x0101,
			[MSG_ARG_COUNT] = 1,
			[MSG_ARG0] = 1,
		},
		[DPFE_CMD_GET_REFRESH] = {
			[MSG_HEADER] = DPFE_MSG_TYPE_COMMAND,
			[MSG_COMMAND] = 0x0202,
			[MSG_ARG_COUNT] = 0,
		},
		/* There's no GET_VENDOR command in API v3. */
	},
};

static const char *get_error_text(unsigned int i)
{
	static const char * const error_text[] = {
		"Success", "Header code incorrect",
		"Unknown command or argument", "Incorrect checksum",
		"Malformed command", "Timed out", "Unknown error",
	};

	if (unlikely(i >= ARRAY_SIZE(error_text)))
		i = ARRAY_SIZE(error_text) - 1;

	return error_text[i];
}

static bool is_dcpu_enabled(struct brcmstb_dpfe_priv *priv)
{
	u32 val;

	mutex_lock(&priv->lock);
	val = readl_relaxed(priv->regs + REG_DCPU_RESET);
	mutex_unlock(&priv->lock);

	return !(val & DCPU_RESET_MASK);
}

static void __disable_dcpu(struct brcmstb_dpfe_priv *priv)
{
	u32 val;

	if (!is_dcpu_enabled(priv))
		return;

	mutex_lock(&priv->lock);

	/* Put DCPU in reset if it's running. */
	val = readl_relaxed(priv->regs + REG_DCPU_RESET);
	val |= (1 << DCPU_RESET_SHIFT);
	writel_relaxed(val, priv->regs + REG_DCPU_RESET);

	mutex_unlock(&priv->lock);
}

static void __enable_dcpu(struct brcmstb_dpfe_priv *priv)
{
	void __iomem *regs = priv->regs;
	u32 val;

	mutex_lock(&priv->lock);

	/* Clear mailbox registers. */
	writel_relaxed(0, regs + REG_TO_DCPU_MBOX);
	writel_relaxed(0, regs + REG_TO_HOST_MBOX);

	/* Disable DCPU clock gating */
	val = readl_relaxed(regs + REG_DCPU_RESET);
	val &= ~(1 << DCPU_CLK_DISABLE_SHIFT);
	writel_relaxed(val, regs + REG_DCPU_RESET);

	/* Take DCPU out of reset */
	val = readl_relaxed(regs + REG_DCPU_RESET);
	val &= ~(1 << DCPU_RESET_SHIFT);
	writel_relaxed(val, regs + REG_DCPU_RESET);

	mutex_unlock(&priv->lock);
}

static unsigned int get_msg_chksum(const u32 msg[], unsigned int max)
{
	unsigned int sum = 0;
	unsigned int i;

	/* Don't include the last field in the checksum. */
	for (i = 0; i < max; i++)
		sum += msg[i];

	return sum;
}

static void __iomem *get_msg_ptr(struct brcmstb_dpfe_priv *priv, u32 response,
				 char *buf, ssize_t *size)
{
	unsigned int msg_type;
	unsigned int offset;
	void __iomem *ptr = NULL;

	/* There is no need to use this function for API v3 or later. */
	if (unlikely(priv->dpfe_api->version >= 3))
		return NULL;

	msg_type = (response >> DRAM_MSG_TYPE_OFFSET) & DRAM_MSG_TYPE_MASK;
	offset = (response >> DRAM_MSG_ADDR_OFFSET) & DRAM_MSG_ADDR_MASK;

	/*
	 * msg_type == 1: the offset is relative to the message RAM
	 * msg_type == 0: the offset is relative to the data RAM (this is the
	 *                previous way of passing data)
	 * msg_type is anything else: there's critical hardware problem
	 */
	switch (msg_type) {
	case 1:
		ptr = priv->regs + DCPU_MSG_RAM_START + offset;
		break;
	case 0:
		ptr = priv->dmem + offset;
		break;
	default:
		dev_emerg(priv->dev, "invalid message reply from DCPU: %#x\n",
			response);
		if (buf && size)
			*size = sprintf(buf,
				"FATAL: communication error with DCPU\n");
	}

	return ptr;
}

static void __finalize_command(struct brcmstb_dpfe_priv *priv)
{
	unsigned int release_mbox;

	/*
	 * It depends on the API version which MBOX register we have to write to
	 * to signal we are done.
	 */
	release_mbox = (priv->dpfe_api->version < 2)
			? REG_TO_HOST_MBOX : REG_TO_DCPU_MBOX;
	writel_relaxed(0, priv->regs + release_mbox);
}

static int __send_command(struct brcmstb_dpfe_priv *priv, unsigned int cmd,
			  u32 result[])
{
	const u32 *msg = priv->dpfe_api->command[cmd];
	void __iomem *regs = priv->regs;
	unsigned int i, chksum, chksum_idx;
	int ret = 0;
	u32 resp;

	if (cmd >= DPFE_CMD_MAX)
		return -1;

	mutex_lock(&priv->lock);

	/* Wait for DCPU to become ready */
	for (i = 0; i < DELAY_LOOP_MAX; i++) {
		resp = readl_relaxed(regs + REG_TO_HOST_MBOX);
		if (resp == 0)
			break;
		msleep(1);
	}
	if (resp != 0) {
		mutex_unlock(&priv->lock);
		return -ffs(DCPU_RET_ERR_TIMEDOUT);
	}

	/* Compute checksum over the message */
	chksum_idx = msg[MSG_ARG_COUNT] + MSG_ARG_COUNT + 1;
	chksum = get_msg_chksum(msg, chksum_idx);

	/* Write command and arguments to message area */
	for (i = 0; i < MSG_FIELD_MAX; i++) {
		if (i == chksum_idx)
			writel_relaxed(chksum, regs + DCPU_MSG_RAM(i));
		else
			writel_relaxed(msg[i], regs + DCPU_MSG_RAM(i));
	}

	/* Tell DCPU there is a command waiting */
	writel_relaxed(1, regs + REG_TO_DCPU_MBOX);

	/* Wait for DCPU to process the command */
	for (i = 0; i < DELAY_LOOP_MAX; i++) {
		/* Read response code */
		resp = readl_relaxed(regs + REG_TO_HOST_MBOX);
		if (resp > 0)
			break;
		msleep(1);
	}

	if (i == DELAY_LOOP_MAX) {
		resp = (DCPU_RET_ERR_TIMEDOUT & ~DCPU_RET_ERROR_BIT);
		ret = -ffs(resp);
	} else {
		/* Read response data */
		for (i = 0; i < MSG_FIELD_MAX; i++)
			result[i] = readl_relaxed(regs + DCPU_MSG_RAM(i));
		chksum_idx = result[MSG_ARG_COUNT] + MSG_ARG_COUNT + 1;
	}

	/* Tell DCPU we are done */
	__finalize_command(priv);

	mutex_unlock(&priv->lock);

	if (ret)
		return ret;

	/* Verify response */
	chksum = get_msg_chksum(result, chksum_idx);
	if (chksum != result[chksum_idx])
		resp = DCPU_RET_ERR_CHKSUM;

	if (resp != DCPU_RET_SUCCESS) {
		resp &= ~DCPU_RET_ERROR_BIT;
		ret = -ffs(resp);
	}

	return ret;
}

/* Ensure that the firmware file loaded meets all the requirements. */
static int __verify_firmware(struct init_data *init,
			     const struct firmware *fw)
{
	const struct dpfe_firmware_header *header = (void *)fw->data;
	unsigned int dmem_size, imem_size, total_size;
	bool is_big_endian = false;
	const u32 *chksum_ptr;

	if (header->magic == DPFE_BE_MAGIC)
		is_big_endian = true;
	else if (header->magic != DPFE_LE_MAGIC)
		return ERR_INVALID_MAGIC;

	if (is_big_endian) {
		dmem_size = be32_to_cpu(header->dmem_size);
		imem_size = be32_to_cpu(header->imem_size);
	} else {
		dmem_size = le32_to_cpu(header->dmem_size);
		imem_size = le32_to_cpu(header->imem_size);
	}

	/* Data and instruction sections are 32 bit words. */
	if ((dmem_size % sizeof(u32)) != 0 || (imem_size % sizeof(u32)) != 0)
		return ERR_INVALID_SIZE;

	/*
	 * The header + the data section + the instruction section + the
	 * checksum must be equal to the total firmware size.
	 */
	total_size = dmem_size + imem_size + sizeof(*header) +
		sizeof(*chksum_ptr);
	if (total_size != fw->size)
		return ERR_INVALID_SIZE;

	/* The checksum comes at the very end. */
	chksum_ptr = (void *)fw->data + sizeof(*header) + dmem_size + imem_size;

	init->is_big_endian = is_big_endian;
	init->dmem_len = dmem_size;
	init->imem_len = imem_size;
	init->chksum = (is_big_endian)
		? be32_to_cpu(*chksum_ptr) : le32_to_cpu(*chksum_ptr);

	return 0;
}

/* Verify checksum by reading back the firmware from co-processor RAM. */
static int __verify_fw_checksum(struct init_data *init,
				struct brcmstb_dpfe_priv *priv,
				const struct dpfe_firmware_header *header,
				u32 checksum)
{
	u32 magic, sequence, version, sum;
	u32 __iomem *dmem = priv->dmem;
	u32 __iomem *imem = priv->imem;
	unsigned int i;

	if (init->is_big_endian) {
		magic = be32_to_cpu(header->magic);
		sequence = be32_to_cpu(header->sequence);
		version = be32_to_cpu(header->version);
	} else {
		magic = le32_to_cpu(header->magic);
		sequence = le32_to_cpu(header->sequence);
		version = le32_to_cpu(header->version);
	}

	sum = magic + sequence + version + init->dmem_len + init->imem_len;

	for (i = 0; i < init->dmem_len / sizeof(u32); i++)
		sum += readl_relaxed(dmem + i);

	for (i = 0; i < init->imem_len / sizeof(u32); i++)
		sum += readl_relaxed(imem + i);

	return (sum == checksum) ? 0 : -1;
}

static int __write_firmware(u32 __iomem *mem, const u32 *fw,
			    unsigned int size, bool is_big_endian)
{
	unsigned int i;

	/* Convert size to 32-bit words. */
	size /= sizeof(u32);

	/* It is recommended to clear the firmware area first. */
	for (i = 0; i < size; i++)
		writel_relaxed(0, mem + i);

	/* Now copy it. */
	if (is_big_endian) {
		for (i = 0; i < size; i++)
			writel_relaxed(be32_to_cpu(fw[i]), mem + i);
	} else {
		for (i = 0; i < size; i++)
			writel_relaxed(le32_to_cpu(fw[i]), mem + i);
	}

	return 0;
}

static int brcmstb_dpfe_download_firmware(struct brcmstb_dpfe_priv *priv)
{
	const struct dpfe_firmware_header *header;
	unsigned int dmem_size, imem_size;
	struct device *dev = priv->dev;
	bool is_big_endian = false;
	const struct firmware *fw;
	const u32 *dmem, *imem;
	struct init_data init;
	const void *fw_blob;
	int ret;

	/*
	 * Skip downloading the firmware if the DCPU is already running and
	 * responding to commands.
	 */
	if (is_dcpu_enabled(priv)) {
		u32 response[MSG_FIELD_MAX];

		ret = __send_command(priv, DPFE_CMD_GET_INFO, response);
		if (!ret)
			return 0;
	}

	/*
	 * If the firmware filename is NULL it means the boot firmware has to
	 * download the DCPU firmware for us. If that didn't work, we have to
	 * bail, since downloading it ourselves wouldn't work either.
	 */
	if (!priv->dpfe_api->fw_name)
		return -ENODEV;

	ret = firmware_request_nowarn(&fw, priv->dpfe_api->fw_name, dev);
	/*
	 * Defer the firmware download if the firmware file couldn't be found.
	 * The root file system may not be available yet.
	 */
	if (ret)
		return (ret == -ENOENT) ? -EPROBE_DEFER : ret;

	ret = __verify_firmware(&init, fw);
	if (ret) {
		ret = -EFAULT;
		goto release_fw;
	}

	__disable_dcpu(priv);

	is_big_endian = init.is_big_endian;
	dmem_size = init.dmem_len;
	imem_size = init.imem_len;

	/* At the beginning of the firmware blob is a header. */
	header = (struct dpfe_firmware_header *)fw->data;
	/* Void pointer to the beginning of the actual firmware. */
	fw_blob = fw->data + sizeof(*header);
	/* IMEM comes right after the header. */
	imem = fw_blob;
	/* DMEM follows after IMEM. */
	dmem = fw_blob + imem_size;

	ret = __write_firmware(priv->dmem, dmem, dmem_size, is_big_endian);
	if (ret)
		goto release_fw;
	ret = __write_firmware(priv->imem, imem, imem_size, is_big_endian);
	if (ret)
		goto release_fw;

	ret = __verify_fw_checksum(&init, priv, header, init.chksum);
	if (ret)
		goto release_fw;

	__enable_dcpu(priv);

release_fw:
	release_firmware(fw);
	return ret;
}

static ssize_t generic_show(unsigned int command, u32 response[],
			    struct brcmstb_dpfe_priv *priv, char *buf)
{
	int ret;

	if (!priv)
		return sprintf(buf, "ERROR: driver private data not set\n");

	ret = __send_command(priv, command, response);
	if (ret < 0)
		return sprintf(buf, "ERROR: %s\n", get_error_text(-ret));

	return 0;
}

static ssize_t show_info(struct device *dev, struct device_attribute *devattr,
			 char *buf)
{
	u32 response[MSG_FIELD_MAX];
	struct brcmstb_dpfe_priv *priv;
	unsigned int info;
	ssize_t ret;

	priv = dev_get_drvdata(dev);
	ret = generic_show(DPFE_CMD_GET_INFO, response, priv, buf);
	if (ret)
		return ret;

	info = response[MSG_ARG0];

	return sprintf(buf, "%u.%u.%u.%u\n",
		       (info >> 24) & 0xff,
		       (info >> 16) & 0xff,
		       (info >> 8) & 0xff,
		       info & 0xff);
}

static ssize_t show_refresh(struct device *dev,
			    struct device_attribute *devattr, char *buf)
{
	u32 response[MSG_FIELD_MAX];
	void __iomem *info;
	struct brcmstb_dpfe_priv *priv;
	u8 refresh, sr_abort, ppre, thermal_offs, tuf;
	u32 mr4;
	ssize_t ret;

	priv = dev_get_drvdata(dev);
	ret = generic_show(DPFE_CMD_GET_REFRESH, response, priv, buf);
	if (ret)
		return ret;

	info = get_msg_ptr(priv, response[MSG_ARG0], buf, &ret);
	if (!info)
		return ret;

	mr4 = (readl_relaxed(info + DRAM_INFO_MR4) >> DRAM_INFO_MR4_SHIFT) &
	       DRAM_INFO_MR4_MASK;

	refresh = (mr4 >> DRAM_MR4_REFRESH) & DRAM_MR4_REFRESH_MASK;
	sr_abort = (mr4 >> DRAM_MR4_SR_ABORT) & DRAM_MR4_SR_ABORT_MASK;
	ppre = (mr4 >> DRAM_MR4_PPRE) & DRAM_MR4_PPRE_MASK;
	thermal_offs = (mr4 >> DRAM_MR4_TH_OFFS) & DRAM_MR4_TH_OFFS_MASK;
	tuf = (mr4 >> DRAM_MR4_TUF) & DRAM_MR4_TUF_MASK;

	return sprintf(buf, "%#x %#x %#x %#x %#x %#x %#x\n",
		       readl_relaxed(info + DRAM_INFO_INTERVAL),
		       refresh, sr_abort, ppre, thermal_offs, tuf,
		       readl_relaxed(info + DRAM_INFO_ERROR));
}

static ssize_t store_refresh(struct device *dev, struct device_attribute *attr,
			  const char *buf, size_t count)
{
	u32 response[MSG_FIELD_MAX];
	struct brcmstb_dpfe_priv *priv;
	void __iomem *info;
	unsigned long val;
	int ret;

	if (kstrtoul(buf, 0, &val) < 0)
		return -EINVAL;

	priv = dev_get_drvdata(dev);
	ret = __send_command(priv, DPFE_CMD_GET_REFRESH, response);
	if (ret)
		return ret;

	info = get_msg_ptr(priv, response[MSG_ARG0], NULL, NULL);
	if (!info)
		return -EIO;

	writel_relaxed(val, info + DRAM_INFO_INTERVAL);

	return count;
}

static ssize_t show_vendor(struct device *dev, struct device_attribute *devattr,
			   char *buf)
{
	u32 response[MSG_FIELD_MAX];
	struct brcmstb_dpfe_priv *priv;
	void __iomem *info;
	ssize_t ret;
	u32 mr5, mr6, mr7, mr8, err;

	priv = dev_get_drvdata(dev);
	ret = generic_show(DPFE_CMD_GET_VENDOR, response, priv, buf);
	if (ret)
		return ret;

	info = get_msg_ptr(priv, response[MSG_ARG0], buf, &ret);
	if (!info)
		return ret;

	mr5 = (readl_relaxed(info + DRAM_VENDOR_MR5) >> DRAM_VENDOR_SHIFT) &
		DRAM_VENDOR_MASK;
	mr6 = (readl_relaxed(info + DRAM_VENDOR_MR6) >> DRAM_VENDOR_SHIFT) &
		DRAM_VENDOR_MASK;
	mr7 = (readl_relaxed(info + DRAM_VENDOR_MR7) >> DRAM_VENDOR_SHIFT) &
		DRAM_VENDOR_MASK;
	mr8 = (readl_relaxed(info + DRAM_VENDOR_MR8) >> DRAM_VENDOR_SHIFT) &
		DRAM_VENDOR_MASK;
	err = readl_relaxed(info + DRAM_VENDOR_ERROR) & DRAM_VENDOR_MASK;

	return sprintf(buf, "%#x %#x %#x %#x %#x\n", mr5, mr6, mr7, mr8, err);
}

static ssize_t show_dram(struct device *dev, struct device_attribute *devattr,
			 char *buf)
{
	u32 response[MSG_FIELD_MAX];
	struct brcmstb_dpfe_priv *priv;
	ssize_t ret;
	u32 mr4, mr5, mr6, mr7, mr8, err;

	priv = dev_get_drvdata(dev);
	ret = generic_show(DPFE_CMD_GET_REFRESH, response, priv, buf);
	if (ret)
		return ret;

	mr4 = response[MSG_ARG0 + 0] & DRAM_INFO_MR4_MASK;
	mr5 = response[MSG_ARG0 + 1] & DRAM_DDR_INFO_MASK;
	mr6 = response[MSG_ARG0 + 2] & DRAM_DDR_INFO_MASK;
	mr7 = response[MSG_ARG0 + 3] & DRAM_DDR_INFO_MASK;
	mr8 = response[MSG_ARG0 + 4] & DRAM_DDR_INFO_MASK;
	err = response[MSG_ARG0 + 5] & DRAM_DDR_INFO_MASK;

	return sprintf(buf, "%#x %#x %#x %#x %#x %#x\n", mr4, mr5, mr6, mr7,
			mr8, err);
}

static int brcmstb_dpfe_resume(struct platform_device *pdev)
{
	struct brcmstb_dpfe_priv *priv = platform_get_drvdata(pdev);

	return brcmstb_dpfe_download_firmware(priv);
}

static int brcmstb_dpfe_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct brcmstb_dpfe_priv *priv;
	struct resource *res;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;

	mutex_init(&priv->lock);
	platform_set_drvdata(pdev, priv);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dpfe-cpu");
	priv->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->regs)) {
		dev_err(dev, "couldn't map DCPU registers\n");
		return -ENODEV;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dpfe-dmem");
	priv->dmem = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->dmem)) {
		dev_err(dev, "Couldn't map DCPU data memory\n");
		return -ENOENT;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dpfe-imem");
	priv->imem = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->imem)) {
		dev_err(dev, "Couldn't map DCPU instruction memory\n");
		return -ENOENT;
	}

	priv->dpfe_api = of_device_get_match_data(dev);
	if (unlikely(!priv->dpfe_api)) {
		/*
		 * It should be impossible to end up here, but to be safe we
		 * check anyway.
		 */
		dev_err(dev, "Couldn't determine API\n");
		return -ENOENT;
	}

	ret = brcmstb_dpfe_download_firmware(priv);
	if (ret)
		return dev_err_probe(dev, ret, "Couldn't download firmware\n");

	ret = sysfs_create_groups(&pdev->dev.kobj, priv->dpfe_api->sysfs_attrs);
	if (!ret)
		dev_info(dev, "registered with API v%d.\n",
			 priv->dpfe_api->version);

	return ret;
}

static int brcmstb_dpfe_remove(struct platform_device *pdev)
{
	struct brcmstb_dpfe_priv *priv = dev_get_drvdata(&pdev->dev);

	sysfs_remove_groups(&pdev->dev.kobj, priv->dpfe_api->sysfs_attrs);

	return 0;
}

static const struct of_device_id brcmstb_dpfe_of_match[] = {
	/* Use legacy API v2 for a select number of chips */
	{ .compatible = "brcm,bcm7268-dpfe-cpu", .data = &dpfe_api_old_v2 },
	{ .compatible = "brcm,bcm7271-dpfe-cpu", .data = &dpfe_api_old_v2 },
	{ .compatible = "brcm,bcm7278-dpfe-cpu", .data = &dpfe_api_old_v2 },
	{ .compatible = "brcm,bcm7211-dpfe-cpu", .data = &dpfe_api_new_v2 },
	/* API v3 is the default going forward */
	{ .compatible = "brcm,dpfe-cpu", .data = &dpfe_api_v3 },
	{}
};
MODULE_DEVICE_TABLE(of, brcmstb_dpfe_of_match);

static struct platform_driver brcmstb_dpfe_driver = {
	.driver	= {
		.name = DRVNAME,
		.of_match_table = brcmstb_dpfe_of_match,
	},
	.probe = brcmstb_dpfe_probe,
	.remove	= brcmstb_dpfe_remove,
	.resume = brcmstb_dpfe_resume,
};

module_platform_driver(brcmstb_dpfe_driver);

MODULE_AUTHOR("Markus Mayer <mmayer@broadcom.com>");
MODULE_DESCRIPTION("BRCMSTB DDR PHY Front End Driver");
MODULE_LICENSE("GPL");

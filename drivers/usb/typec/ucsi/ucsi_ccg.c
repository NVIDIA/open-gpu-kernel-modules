// SPDX-License-Identifier: GPL-2.0
/*
 * UCSI driver for Cypress CCGx Type-C controller
 *
 * Copyright (C) 2017-2018 NVIDIA Corporation. All rights reserved.
 * Author: Ajay Gupta <ajayg@nvidia.com>
 *
 * Some code borrowed from drivers/usb/typec/ucsi/ucsi_acpi.c
 */
#include <linux/acpi.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/usb/typec_dp.h>

#include <asm/unaligned.h>
#include "ucsi.h"

enum enum_fw_mode {
	BOOT,   /* bootloader */
	FW1,    /* FW partition-1 (contains secondary fw) */
	FW2,    /* FW partition-2 (contains primary fw) */
	FW_INVALID,
};

#define CCGX_RAB_DEVICE_MODE			0x0000
#define CCGX_RAB_INTR_REG			0x0006
#define  DEV_INT				BIT(0)
#define  PORT0_INT				BIT(1)
#define  PORT1_INT				BIT(2)
#define  UCSI_READ_INT				BIT(7)
#define CCGX_RAB_JUMP_TO_BOOT			0x0007
#define  TO_BOOT				'J'
#define  TO_ALT_FW				'A'
#define CCGX_RAB_RESET_REQ			0x0008
#define  RESET_SIG				'R'
#define  CMD_RESET_I2C				0x0
#define  CMD_RESET_DEV				0x1
#define CCGX_RAB_ENTER_FLASHING			0x000A
#define  FLASH_ENTER_SIG			'P'
#define CCGX_RAB_VALIDATE_FW			0x000B
#define CCGX_RAB_FLASH_ROW_RW			0x000C
#define  FLASH_SIG				'F'
#define  FLASH_RD_CMD				0x0
#define  FLASH_WR_CMD				0x1
#define  FLASH_FWCT1_WR_CMD			0x2
#define  FLASH_FWCT2_WR_CMD			0x3
#define  FLASH_FWCT_SIG_WR_CMD			0x4
#define CCGX_RAB_READ_ALL_VER			0x0010
#define CCGX_RAB_READ_FW2_VER			0x0020
#define CCGX_RAB_UCSI_CONTROL			0x0039
#define CCGX_RAB_UCSI_CONTROL_START		BIT(0)
#define CCGX_RAB_UCSI_CONTROL_STOP		BIT(1)
#define CCGX_RAB_UCSI_DATA_BLOCK(offset)	(0xf000 | ((offset) & 0xff))
#define REG_FLASH_RW_MEM        0x0200
#define DEV_REG_IDX				CCGX_RAB_DEVICE_MODE
#define CCGX_RAB_PDPORT_ENABLE			0x002C
#define  PDPORT_1		BIT(0)
#define  PDPORT_2		BIT(1)
#define CCGX_RAB_RESPONSE			0x007E
#define  ASYNC_EVENT				BIT(7)

/* CCGx events & async msg codes */
#define RESET_COMPLETE		0x80
#define EVENT_INDEX		RESET_COMPLETE
#define PORT_CONNECT_DET	0x84
#define PORT_DISCONNECT_DET	0x85
#define ROLE_SWAP_COMPELETE	0x87

/* ccg firmware */
#define CYACD_LINE_SIZE         527
#define CCG4_ROW_SIZE           256
#define FW1_METADATA_ROW        0x1FF
#define FW2_METADATA_ROW        0x1FE
#define FW_CFG_TABLE_SIG_SIZE	256

static int secondary_fw_min_ver = 41;

enum enum_flash_mode {
	SECONDARY_BL,	/* update secondary using bootloader */
	PRIMARY,	/* update primary using secondary */
	SECONDARY,	/* update secondary using primary */
	FLASH_NOT_NEEDED,	/* update not required */
	FLASH_INVALID,
};

static const char * const ccg_fw_names[] = {
	"ccg_boot.cyacd",
	"ccg_primary.cyacd",
	"ccg_secondary.cyacd"
};

struct ccg_dev_info {
#define CCG_DEVINFO_FWMODE_SHIFT (0)
#define CCG_DEVINFO_FWMODE_MASK (0x3 << CCG_DEVINFO_FWMODE_SHIFT)
#define CCG_DEVINFO_PDPORTS_SHIFT (2)
#define CCG_DEVINFO_PDPORTS_MASK (0x3 << CCG_DEVINFO_PDPORTS_SHIFT)
	u8 mode;
	u8 bl_mode;
	__le16 silicon_id;
	__le16 bl_last_row;
} __packed;

struct version_format {
	__le16 build;
	u8 patch;
	u8 ver;
#define CCG_VERSION_PATCH(x) ((x) << 16)
#define CCG_VERSION(x)	((x) << 24)
#define CCG_VERSION_MIN_SHIFT (0)
#define CCG_VERSION_MIN_MASK (0xf << CCG_VERSION_MIN_SHIFT)
#define CCG_VERSION_MAJ_SHIFT (4)
#define CCG_VERSION_MAJ_MASK (0xf << CCG_VERSION_MAJ_SHIFT)
} __packed;

/*
 * Firmware version 3.1.10 or earlier, built for NVIDIA has known issue
 * of missing interrupt when a device is connected for runtime resume
 */
#define CCG_FW_BUILD_NVIDIA	(('n' << 8) | 'v')
#define CCG_OLD_FW_VERSION	(CCG_VERSION(0x31) | CCG_VERSION_PATCH(10))

/* Altmode offset for NVIDIA Function Test Board (FTB) */
#define NVIDIA_FTB_DP_OFFSET	(2)
#define NVIDIA_FTB_DBG_OFFSET	(3)

struct version_info {
	struct version_format base;
	struct version_format app;
};

struct fw_config_table {
	u32 identity;
	u16 table_size;
	u8 fwct_version;
	u8 is_key_change;
	u8 guid[16];
	struct version_format base;
	struct version_format app;
	u8 primary_fw_digest[32];
	u32 key_exp_length;
	u8 key_modulus[256];
	u8 key_exp[4];
};

/* CCGx response codes */
enum ccg_resp_code {
	CMD_NO_RESP             = 0x00,
	CMD_SUCCESS             = 0x02,
	FLASH_DATA_AVAILABLE    = 0x03,
	CMD_INVALID             = 0x05,
	FLASH_UPDATE_FAIL       = 0x07,
	INVALID_FW              = 0x08,
	INVALID_ARG             = 0x09,
	CMD_NOT_SUPPORT         = 0x0A,
	TRANSACTION_FAIL        = 0x0C,
	PD_CMD_FAIL             = 0x0D,
	UNDEF_ERROR             = 0x0F,
	INVALID_RESP		= 0x10,
};

#define CCG_EVENT_MAX	(EVENT_INDEX + 43)

struct ccg_cmd {
	u16 reg;
	u32 data;
	int len;
	u32 delay; /* ms delay for cmd timeout  */
};

struct ccg_resp {
	u8 code;
	u8 length;
};

struct ucsi_ccg_altmode {
	u16 svid;
	u32 mid;
	u8 linked_idx;
	u8 active_idx;
#define UCSI_MULTI_DP_INDEX	(0xff)
	bool checked;
} __packed;

struct ucsi_ccg {
	struct device *dev;
	struct ucsi *ucsi;
	struct i2c_client *client;

	struct ccg_dev_info info;
	/* version info for boot, primary and secondary */
	struct version_info version[FW2 + 1];
	u32 fw_version;
	/* CCG HPI communication flags */
	unsigned long flags;
#define RESET_PENDING	0
#define DEV_CMD_PENDING	1
	struct ccg_resp dev_resp;
	u8 cmd_resp;
	int port_num;
	int irq;
	struct work_struct work;
	struct mutex lock; /* to sync between user and driver thread */

	/* fw build with vendor information */
	u16 fw_build;
	struct work_struct pm_work;

	struct completion complete;

	u64 last_cmd_sent;
	bool has_multiple_dp;
	struct ucsi_ccg_altmode orig[UCSI_MAX_ALTMODES];
	struct ucsi_ccg_altmode updated[UCSI_MAX_ALTMODES];
};

static int ccg_read(struct ucsi_ccg *uc, u16 rab, u8 *data, u32 len)
{
	struct i2c_client *client = uc->client;
	const struct i2c_adapter_quirks *quirks = client->adapter->quirks;
	unsigned char buf[2];
	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags  = 0x0,
			.len	= sizeof(buf),
			.buf	= buf,
		},
		{
			.addr	= client->addr,
			.flags  = I2C_M_RD,
			.buf	= data,
		},
	};
	u32 rlen, rem_len = len, max_read_len = len;
	int status;

	/* check any max_read_len limitation on i2c adapter */
	if (quirks && quirks->max_read_len)
		max_read_len = quirks->max_read_len;

	pm_runtime_get_sync(uc->dev);
	while (rem_len > 0) {
		msgs[1].buf = &data[len - rem_len];
		rlen = min_t(u16, rem_len, max_read_len);
		msgs[1].len = rlen;
		put_unaligned_le16(rab, buf);
		status = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
		if (status < 0) {
			dev_err(uc->dev, "i2c_transfer failed %d\n", status);
			pm_runtime_put_sync(uc->dev);
			return status;
		}
		rab += rlen;
		rem_len -= rlen;
	}

	pm_runtime_put_sync(uc->dev);
	return 0;
}

static int ccg_write(struct ucsi_ccg *uc, u16 rab, const u8 *data, u32 len)
{
	struct i2c_client *client = uc->client;
	unsigned char *buf;
	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags  = 0x0,
		}
	};
	int status;

	buf = kzalloc(len + sizeof(rab), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	put_unaligned_le16(rab, buf);
	memcpy(buf + sizeof(rab), data, len);

	msgs[0].len = len + sizeof(rab);
	msgs[0].buf = buf;

	pm_runtime_get_sync(uc->dev);
	status = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
	if (status < 0) {
		dev_err(uc->dev, "i2c_transfer failed %d\n", status);
		pm_runtime_put_sync(uc->dev);
		kfree(buf);
		return status;
	}

	pm_runtime_put_sync(uc->dev);
	kfree(buf);
	return 0;
}

static int ucsi_ccg_init(struct ucsi_ccg *uc)
{
	unsigned int count = 10;
	u8 data;
	int status;

	data = CCGX_RAB_UCSI_CONTROL_STOP;
	status = ccg_write(uc, CCGX_RAB_UCSI_CONTROL, &data, sizeof(data));
	if (status < 0)
		return status;

	data = CCGX_RAB_UCSI_CONTROL_START;
	status = ccg_write(uc, CCGX_RAB_UCSI_CONTROL, &data, sizeof(data));
	if (status < 0)
		return status;

	/*
	 * Flush CCGx RESPONSE queue by acking interrupts. Above ucsi control
	 * register write will push response which must be cleared.
	 */
	do {
		status = ccg_read(uc, CCGX_RAB_INTR_REG, &data, sizeof(data));
		if (status < 0)
			return status;

		if (!data)
			return 0;

		status = ccg_write(uc, CCGX_RAB_INTR_REG, &data, sizeof(data));
		if (status < 0)
			return status;

		usleep_range(10000, 11000);
	} while (--count);

	return -ETIMEDOUT;
}

static void ucsi_ccg_update_get_current_cam_cmd(struct ucsi_ccg *uc, u8 *data)
{
	u8 cam, new_cam;

	cam = data[0];
	new_cam = uc->orig[cam].linked_idx;
	uc->updated[new_cam].active_idx = cam;
	data[0] = new_cam;
}

static bool ucsi_ccg_update_altmodes(struct ucsi *ucsi,
				     struct ucsi_altmode *orig,
				     struct ucsi_altmode *updated)
{
	struct ucsi_ccg *uc = ucsi_get_drvdata(ucsi);
	struct ucsi_ccg_altmode *alt, *new_alt;
	int i, j, k = 0;
	bool found = false;

	alt = uc->orig;
	new_alt = uc->updated;
	memset(uc->updated, 0, sizeof(uc->updated));

	/*
	 * Copy original connector altmodes to new structure.
	 * We need this before second loop since second loop
	 * checks for duplicate altmodes.
	 */
	for (i = 0; i < UCSI_MAX_ALTMODES; i++) {
		alt[i].svid = orig[i].svid;
		alt[i].mid = orig[i].mid;
		if (!alt[i].svid)
			break;
	}

	for (i = 0; i < UCSI_MAX_ALTMODES; i++) {
		if (!alt[i].svid)
			break;

		/* already checked and considered */
		if (alt[i].checked)
			continue;

		if (!DP_CONF_GET_PIN_ASSIGN(alt[i].mid)) {
			/* Found Non DP altmode */
			new_alt[k].svid = alt[i].svid;
			new_alt[k].mid |= alt[i].mid;
			new_alt[k].linked_idx = i;
			alt[i].linked_idx = k;
			updated[k].svid = new_alt[k].svid;
			updated[k].mid = new_alt[k].mid;
			k++;
			continue;
		}

		for (j = i + 1; j < UCSI_MAX_ALTMODES; j++) {
			if (alt[i].svid != alt[j].svid ||
			    !DP_CONF_GET_PIN_ASSIGN(alt[j].mid)) {
				continue;
			} else {
				/* Found duplicate DP mode */
				new_alt[k].svid = alt[i].svid;
				new_alt[k].mid |= alt[i].mid | alt[j].mid;
				new_alt[k].linked_idx = UCSI_MULTI_DP_INDEX;
				alt[i].linked_idx = k;
				alt[j].linked_idx = k;
				alt[j].checked = true;
				found = true;
			}
		}
		if (found) {
			uc->has_multiple_dp = true;
		} else {
			/* Didn't find any duplicate DP altmode */
			new_alt[k].svid = alt[i].svid;
			new_alt[k].mid |= alt[i].mid;
			new_alt[k].linked_idx = i;
			alt[i].linked_idx = k;
		}
		updated[k].svid = new_alt[k].svid;
		updated[k].mid = new_alt[k].mid;
		k++;
	}
	return found;
}

static void ucsi_ccg_update_set_new_cam_cmd(struct ucsi_ccg *uc,
					    struct ucsi_connector *con,
					    u64 *cmd)
{
	struct ucsi_ccg_altmode *new_port, *port;
	struct typec_altmode *alt = NULL;
	u8 new_cam, cam, pin;
	bool enter_new_mode;
	int i, j, k = 0xff;

	port = uc->orig;
	new_cam = UCSI_SET_NEW_CAM_GET_AM(*cmd);
	new_port = &uc->updated[new_cam];
	cam = new_port->linked_idx;
	enter_new_mode = UCSI_SET_NEW_CAM_ENTER(*cmd);

	/*
	 * If CAM is UCSI_MULTI_DP_INDEX then this is DP altmode
	 * with multiple DP mode. Find out CAM for best pin assignment
	 * among all DP mode. Priorite pin E->D->C after making sure
	 * the partner supports that pin.
	 */
	if (cam == UCSI_MULTI_DP_INDEX) {
		if (enter_new_mode) {
			for (i = 0; con->partner_altmode[i]; i++) {
				alt = con->partner_altmode[i];
				if (alt->svid == new_port->svid)
					break;
			}
			/*
			 * alt will always be non NULL since this is
			 * UCSI_SET_NEW_CAM command and so there will be
			 * at least one con->partner_altmode[i] with svid
			 * matching with new_port->svid.
			 */
			for (j = 0; port[j].svid; j++) {
				pin = DP_CONF_GET_PIN_ASSIGN(port[j].mid);
				if (alt && port[j].svid == alt->svid &&
				    (pin & DP_CONF_GET_PIN_ASSIGN(alt->vdo))) {
					/* prioritize pin E->D->C */
					if (k == 0xff || (k != 0xff && pin >
					    DP_CONF_GET_PIN_ASSIGN(port[k].mid))
					    ) {
						k = j;
					}
				}
			}
			cam = k;
			new_port->active_idx = cam;
		} else {
			cam = new_port->active_idx;
		}
	}
	*cmd &= ~UCSI_SET_NEW_CAM_AM_MASK;
	*cmd |= UCSI_SET_NEW_CAM_SET_AM(cam);
}

/*
 * Change the order of vdo values of NVIDIA test device FTB
 * (Function Test Board) which reports altmode list with vdo=0x3
 * first and then vdo=0x. Current logic to assign mode value is
 * based on order in altmode list and it causes a mismatch of CON
 * and SOP altmodes since NVIDIA GPU connector has order of vdo=0x1
 * first and then vdo=0x3
 */
static void ucsi_ccg_nvidia_altmode(struct ucsi_ccg *uc,
				    struct ucsi_altmode *alt)
{
	switch (UCSI_ALTMODE_OFFSET(uc->last_cmd_sent)) {
	case NVIDIA_FTB_DP_OFFSET:
		if (alt[0].mid == USB_TYPEC_NVIDIA_VLINK_DBG_VDO)
			alt[0].mid = USB_TYPEC_NVIDIA_VLINK_DP_VDO |
				DP_CAP_DP_SIGNALING | DP_CAP_USB |
				DP_CONF_SET_PIN_ASSIGN(BIT(DP_PIN_ASSIGN_E));
		break;
	case NVIDIA_FTB_DBG_OFFSET:
		if (alt[0].mid == USB_TYPEC_NVIDIA_VLINK_DP_VDO)
			alt[0].mid = USB_TYPEC_NVIDIA_VLINK_DBG_VDO;
		break;
	default:
		break;
	}
}

static int ucsi_ccg_read(struct ucsi *ucsi, unsigned int offset,
			 void *val, size_t val_len)
{
	struct ucsi_ccg *uc = ucsi_get_drvdata(ucsi);
	u16 reg = CCGX_RAB_UCSI_DATA_BLOCK(offset);
	struct ucsi_altmode *alt;
	int ret;

	ret = ccg_read(uc, reg, val, val_len);
	if (ret)
		return ret;

	if (offset != UCSI_MESSAGE_IN)
		return ret;

	switch (UCSI_COMMAND(uc->last_cmd_sent)) {
	case UCSI_GET_CURRENT_CAM:
		if (uc->has_multiple_dp)
			ucsi_ccg_update_get_current_cam_cmd(uc, (u8 *)val);
		break;
	case UCSI_GET_ALTERNATE_MODES:
		if (UCSI_ALTMODE_RECIPIENT(uc->last_cmd_sent) ==
		    UCSI_RECIPIENT_SOP) {
			alt = val;
			if (alt[0].svid == USB_TYPEC_NVIDIA_VLINK_SID)
				ucsi_ccg_nvidia_altmode(uc, alt);
		}
		break;
	default:
		break;
	}
	uc->last_cmd_sent = 0;

	return ret;
}

static int ucsi_ccg_async_write(struct ucsi *ucsi, unsigned int offset,
				const void *val, size_t val_len)
{
	u16 reg = CCGX_RAB_UCSI_DATA_BLOCK(offset);

	return ccg_write(ucsi_get_drvdata(ucsi), reg, val, val_len);
}

static int ucsi_ccg_sync_write(struct ucsi *ucsi, unsigned int offset,
			       const void *val, size_t val_len)
{
	struct ucsi_ccg *uc = ucsi_get_drvdata(ucsi);
	struct ucsi_connector *con;
	int con_index;
	int ret;

	mutex_lock(&uc->lock);
	pm_runtime_get_sync(uc->dev);
	set_bit(DEV_CMD_PENDING, &uc->flags);

	if (offset == UCSI_CONTROL && val_len == sizeof(uc->last_cmd_sent)) {
		uc->last_cmd_sent = *(u64 *)val;

		if (UCSI_COMMAND(uc->last_cmd_sent) == UCSI_SET_NEW_CAM &&
		    uc->has_multiple_dp) {
			con_index = (uc->last_cmd_sent >> 16) &
				    UCSI_CMD_CONNECTOR_MASK;
			con = &uc->ucsi->connector[con_index - 1];
			ucsi_ccg_update_set_new_cam_cmd(uc, con, (u64 *)val);
		}
	}

	ret = ucsi_ccg_async_write(ucsi, offset, val, val_len);
	if (ret)
		goto err_clear_bit;

	if (!wait_for_completion_timeout(&uc->complete, msecs_to_jiffies(5000)))
		ret = -ETIMEDOUT;

err_clear_bit:
	clear_bit(DEV_CMD_PENDING, &uc->flags);
	pm_runtime_put_sync(uc->dev);
	mutex_unlock(&uc->lock);

	return ret;
}

static const struct ucsi_operations ucsi_ccg_ops = {
	.read = ucsi_ccg_read,
	.sync_write = ucsi_ccg_sync_write,
	.async_write = ucsi_ccg_async_write,
	.update_altmodes = ucsi_ccg_update_altmodes
};

static irqreturn_t ccg_irq_handler(int irq, void *data)
{
	u16 reg = CCGX_RAB_UCSI_DATA_BLOCK(UCSI_CCI);
	struct ucsi_ccg *uc = data;
	u8 intr_reg;
	u32 cci;
	int ret;

	ret = ccg_read(uc, CCGX_RAB_INTR_REG, &intr_reg, sizeof(intr_reg));
	if (ret)
		return ret;

	ret = ccg_read(uc, reg, (void *)&cci, sizeof(cci));
	if (ret)
		goto err_clear_irq;

	if (UCSI_CCI_CONNECTOR(cci))
		ucsi_connector_change(uc->ucsi, UCSI_CCI_CONNECTOR(cci));

	if (test_bit(DEV_CMD_PENDING, &uc->flags) &&
	    cci & (UCSI_CCI_ACK_COMPLETE | UCSI_CCI_COMMAND_COMPLETE))
		complete(&uc->complete);

err_clear_irq:
	ccg_write(uc, CCGX_RAB_INTR_REG, &intr_reg, sizeof(intr_reg));

	return IRQ_HANDLED;
}

static void ccg_pm_workaround_work(struct work_struct *pm_work)
{
	ccg_irq_handler(0, container_of(pm_work, struct ucsi_ccg, pm_work));
}

static int get_fw_info(struct ucsi_ccg *uc)
{
	int err;

	err = ccg_read(uc, CCGX_RAB_READ_ALL_VER, (u8 *)(&uc->version),
		       sizeof(uc->version));
	if (err < 0)
		return err;

	uc->fw_version = CCG_VERSION(uc->version[FW2].app.ver) |
			CCG_VERSION_PATCH(uc->version[FW2].app.patch);

	err = ccg_read(uc, CCGX_RAB_DEVICE_MODE, (u8 *)(&uc->info),
		       sizeof(uc->info));
	if (err < 0)
		return err;

	return 0;
}

static inline bool invalid_async_evt(int code)
{
	return (code >= CCG_EVENT_MAX) || (code < EVENT_INDEX);
}

static void ccg_process_response(struct ucsi_ccg *uc)
{
	struct device *dev = uc->dev;

	if (uc->dev_resp.code & ASYNC_EVENT) {
		if (uc->dev_resp.code == RESET_COMPLETE) {
			if (test_bit(RESET_PENDING, &uc->flags))
				uc->cmd_resp = uc->dev_resp.code;
			get_fw_info(uc);
		}
		if (invalid_async_evt(uc->dev_resp.code))
			dev_err(dev, "invalid async evt %d\n",
				uc->dev_resp.code);
	} else {
		if (test_bit(DEV_CMD_PENDING, &uc->flags)) {
			uc->cmd_resp = uc->dev_resp.code;
			clear_bit(DEV_CMD_PENDING, &uc->flags);
		} else {
			dev_err(dev, "dev resp 0x%04x but no cmd pending\n",
				uc->dev_resp.code);
		}
	}
}

static int ccg_read_response(struct ucsi_ccg *uc)
{
	unsigned long target = jiffies + msecs_to_jiffies(1000);
	struct device *dev = uc->dev;
	u8 intval;
	int status;

	/* wait for interrupt status to get updated */
	do {
		status = ccg_read(uc, CCGX_RAB_INTR_REG, &intval,
				  sizeof(intval));
		if (status < 0)
			return status;

		if (intval & DEV_INT)
			break;
		usleep_range(500, 600);
	} while (time_is_after_jiffies(target));

	if (time_is_before_jiffies(target)) {
		dev_err(dev, "response timeout error\n");
		return -ETIME;
	}

	status = ccg_read(uc, CCGX_RAB_RESPONSE, (u8 *)&uc->dev_resp,
			  sizeof(uc->dev_resp));
	if (status < 0)
		return status;

	status = ccg_write(uc, CCGX_RAB_INTR_REG, &intval, sizeof(intval));
	if (status < 0)
		return status;

	return 0;
}

/* Caller must hold uc->lock */
static int ccg_send_command(struct ucsi_ccg *uc, struct ccg_cmd *cmd)
{
	struct device *dev = uc->dev;
	int ret;

	switch (cmd->reg & 0xF000) {
	case DEV_REG_IDX:
		set_bit(DEV_CMD_PENDING, &uc->flags);
		break;
	default:
		dev_err(dev, "invalid cmd register\n");
		break;
	}

	ret = ccg_write(uc, cmd->reg, (u8 *)&cmd->data, cmd->len);
	if (ret < 0)
		return ret;

	msleep(cmd->delay);

	ret = ccg_read_response(uc);
	if (ret < 0) {
		dev_err(dev, "response read error\n");
		switch (cmd->reg & 0xF000) {
		case DEV_REG_IDX:
			clear_bit(DEV_CMD_PENDING, &uc->flags);
			break;
		default:
			dev_err(dev, "invalid cmd register\n");
			break;
		}
		return -EIO;
	}
	ccg_process_response(uc);

	return uc->cmd_resp;
}

static int ccg_cmd_enter_flashing(struct ucsi_ccg *uc)
{
	struct ccg_cmd cmd;
	int ret;

	cmd.reg = CCGX_RAB_ENTER_FLASHING;
	cmd.data = FLASH_ENTER_SIG;
	cmd.len = 1;
	cmd.delay = 50;

	mutex_lock(&uc->lock);

	ret = ccg_send_command(uc, &cmd);

	mutex_unlock(&uc->lock);

	if (ret != CMD_SUCCESS) {
		dev_err(uc->dev, "enter flashing failed ret=%d\n", ret);
		return ret;
	}

	return 0;
}

static int ccg_cmd_reset(struct ucsi_ccg *uc)
{
	struct ccg_cmd cmd;
	u8 *p;
	int ret;

	p = (u8 *)&cmd.data;
	cmd.reg = CCGX_RAB_RESET_REQ;
	p[0] = RESET_SIG;
	p[1] = CMD_RESET_DEV;
	cmd.len = 2;
	cmd.delay = 5000;

	mutex_lock(&uc->lock);

	set_bit(RESET_PENDING, &uc->flags);

	ret = ccg_send_command(uc, &cmd);
	if (ret != RESET_COMPLETE)
		goto err_clear_flag;

	ret = 0;

err_clear_flag:
	clear_bit(RESET_PENDING, &uc->flags);

	mutex_unlock(&uc->lock);

	return ret;
}

static int ccg_cmd_port_control(struct ucsi_ccg *uc, bool enable)
{
	struct ccg_cmd cmd;
	int ret;

	cmd.reg = CCGX_RAB_PDPORT_ENABLE;
	if (enable)
		cmd.data = (uc->port_num == 1) ?
			    PDPORT_1 : (PDPORT_1 | PDPORT_2);
	else
		cmd.data = 0x0;
	cmd.len = 1;
	cmd.delay = 10;

	mutex_lock(&uc->lock);

	ret = ccg_send_command(uc, &cmd);

	mutex_unlock(&uc->lock);

	if (ret != CMD_SUCCESS) {
		dev_err(uc->dev, "port control failed ret=%d\n", ret);
		return ret;
	}
	return 0;
}

static int ccg_cmd_jump_boot_mode(struct ucsi_ccg *uc, int bl_mode)
{
	struct ccg_cmd cmd;
	int ret;

	cmd.reg = CCGX_RAB_JUMP_TO_BOOT;

	if (bl_mode)
		cmd.data = TO_BOOT;
	else
		cmd.data = TO_ALT_FW;

	cmd.len = 1;
	cmd.delay = 100;

	mutex_lock(&uc->lock);

	set_bit(RESET_PENDING, &uc->flags);

	ret = ccg_send_command(uc, &cmd);
	if (ret != RESET_COMPLETE)
		goto err_clear_flag;

	ret = 0;

err_clear_flag:
	clear_bit(RESET_PENDING, &uc->flags);

	mutex_unlock(&uc->lock);

	return ret;
}

static int
ccg_cmd_write_flash_row(struct ucsi_ccg *uc, u16 row,
			const void *data, u8 fcmd)
{
	struct i2c_client *client = uc->client;
	struct ccg_cmd cmd;
	u8 buf[CCG4_ROW_SIZE + 2];
	u8 *p;
	int ret;

	/* Copy the data into the flash read/write memory. */
	put_unaligned_le16(REG_FLASH_RW_MEM, buf);

	memcpy(buf + 2, data, CCG4_ROW_SIZE);

	mutex_lock(&uc->lock);

	ret = i2c_master_send(client, buf, CCG4_ROW_SIZE + 2);
	if (ret != CCG4_ROW_SIZE + 2) {
		dev_err(uc->dev, "REG_FLASH_RW_MEM write fail %d\n", ret);
		mutex_unlock(&uc->lock);
		return ret < 0 ? ret : -EIO;
	}

	/* Use the FLASH_ROW_READ_WRITE register to trigger */
	/* writing of data to the desired flash row */
	p = (u8 *)&cmd.data;
	cmd.reg = CCGX_RAB_FLASH_ROW_RW;
	p[0] = FLASH_SIG;
	p[1] = fcmd;
	put_unaligned_le16(row, &p[2]);
	cmd.len = 4;
	cmd.delay = 50;
	if (fcmd == FLASH_FWCT_SIG_WR_CMD)
		cmd.delay += 400;
	if (row == 510)
		cmd.delay += 220;
	ret = ccg_send_command(uc, &cmd);

	mutex_unlock(&uc->lock);

	if (ret != CMD_SUCCESS) {
		dev_err(uc->dev, "write flash row failed ret=%d\n", ret);
		return ret;
	}

	return 0;
}

static int ccg_cmd_validate_fw(struct ucsi_ccg *uc, unsigned int fwid)
{
	struct ccg_cmd cmd;
	int ret;

	cmd.reg = CCGX_RAB_VALIDATE_FW;
	cmd.data = fwid;
	cmd.len = 1;
	cmd.delay = 500;

	mutex_lock(&uc->lock);

	ret = ccg_send_command(uc, &cmd);

	mutex_unlock(&uc->lock);

	if (ret != CMD_SUCCESS)
		return ret;

	return 0;
}

static bool ccg_check_vendor_version(struct ucsi_ccg *uc,
				     struct version_format *app,
				     struct fw_config_table *fw_cfg)
{
	struct device *dev = uc->dev;

	/* Check if the fw build is for supported vendors */
	if (le16_to_cpu(app->build) != uc->fw_build) {
		dev_info(dev, "current fw is not from supported vendor\n");
		return false;
	}

	/* Check if the new fw build is for supported vendors */
	if (le16_to_cpu(fw_cfg->app.build) != uc->fw_build) {
		dev_info(dev, "new fw is not from supported vendor\n");
		return false;
	}
	return true;
}

static bool ccg_check_fw_version(struct ucsi_ccg *uc, const char *fw_name,
				 struct version_format *app)
{
	const struct firmware *fw = NULL;
	struct device *dev = uc->dev;
	struct fw_config_table fw_cfg;
	u32 cur_version, new_version;
	bool is_later = false;

	if (request_firmware(&fw, fw_name, dev) != 0) {
		dev_err(dev, "error: Failed to open cyacd file %s\n", fw_name);
		return false;
	}

	/*
	 * check if signed fw
	 * last part of fw image is fw cfg table and signature
	 */
	if (fw->size < sizeof(fw_cfg) + FW_CFG_TABLE_SIG_SIZE)
		goto out_release_firmware;

	memcpy((uint8_t *)&fw_cfg, fw->data + fw->size -
	       sizeof(fw_cfg) - FW_CFG_TABLE_SIG_SIZE, sizeof(fw_cfg));

	if (fw_cfg.identity != ('F' | 'W' << 8 | 'C' << 16 | 'T' << 24)) {
		dev_info(dev, "not a signed image\n");
		goto out_release_firmware;
	}

	/* compare input version with FWCT version */
	cur_version = le16_to_cpu(app->build) | CCG_VERSION_PATCH(app->patch) |
			CCG_VERSION(app->ver);

	new_version = le16_to_cpu(fw_cfg.app.build) |
			CCG_VERSION_PATCH(fw_cfg.app.patch) |
			CCG_VERSION(fw_cfg.app.ver);

	if (!ccg_check_vendor_version(uc, app, &fw_cfg))
		goto out_release_firmware;

	if (new_version > cur_version)
		is_later = true;

out_release_firmware:
	release_firmware(fw);
	return is_later;
}

static int ccg_fw_update_needed(struct ucsi_ccg *uc,
				enum enum_flash_mode *mode)
{
	struct device *dev = uc->dev;
	int err;
	struct version_info version[3];

	err = ccg_read(uc, CCGX_RAB_DEVICE_MODE, (u8 *)(&uc->info),
		       sizeof(uc->info));
	if (err) {
		dev_err(dev, "read device mode failed\n");
		return err;
	}

	err = ccg_read(uc, CCGX_RAB_READ_ALL_VER, (u8 *)version,
		       sizeof(version));
	if (err) {
		dev_err(dev, "read device mode failed\n");
		return err;
	}

	if (memcmp(&version[FW1], "\0\0\0\0\0\0\0\0",
		   sizeof(struct version_info)) == 0) {
		dev_info(dev, "secondary fw is not flashed\n");
		*mode = SECONDARY_BL;
	} else if (le16_to_cpu(version[FW1].base.build) <
		secondary_fw_min_ver) {
		dev_info(dev, "secondary fw version is too low (< %d)\n",
			 secondary_fw_min_ver);
		*mode = SECONDARY;
	} else if (memcmp(&version[FW2], "\0\0\0\0\0\0\0\0",
		   sizeof(struct version_info)) == 0) {
		dev_info(dev, "primary fw is not flashed\n");
		*mode = PRIMARY;
	} else if (ccg_check_fw_version(uc, ccg_fw_names[PRIMARY],
		   &version[FW2].app)) {
		dev_info(dev, "found primary fw with later version\n");
		*mode = PRIMARY;
	} else {
		dev_info(dev, "secondary and primary fw are the latest\n");
		*mode = FLASH_NOT_NEEDED;
	}
	return 0;
}

static int do_flash(struct ucsi_ccg *uc, enum enum_flash_mode mode)
{
	struct device *dev = uc->dev;
	const struct firmware *fw = NULL;
	const char *p, *s;
	const char *eof;
	int err, row, len, line_sz, line_cnt = 0;
	unsigned long start_time = jiffies;
	struct fw_config_table  fw_cfg;
	u8 fw_cfg_sig[FW_CFG_TABLE_SIG_SIZE];
	u8 *wr_buf;

	err = request_firmware(&fw, ccg_fw_names[mode], dev);
	if (err) {
		dev_err(dev, "request %s failed err=%d\n",
			ccg_fw_names[mode], err);
		return err;
	}

	if (((uc->info.mode & CCG_DEVINFO_FWMODE_MASK) >>
			CCG_DEVINFO_FWMODE_SHIFT) == FW2) {
		err = ccg_cmd_port_control(uc, false);
		if (err < 0)
			goto release_fw;
		err = ccg_cmd_jump_boot_mode(uc, 0);
		if (err < 0)
			goto release_fw;
	}

	eof = fw->data + fw->size;

	/*
	 * check if signed fw
	 * last part of fw image is fw cfg table and signature
	 */
	if (fw->size < sizeof(fw_cfg) + sizeof(fw_cfg_sig))
		goto not_signed_fw;

	memcpy((uint8_t *)&fw_cfg, fw->data + fw->size -
	       sizeof(fw_cfg) - sizeof(fw_cfg_sig), sizeof(fw_cfg));

	if (fw_cfg.identity != ('F' | ('W' << 8) | ('C' << 16) | ('T' << 24))) {
		dev_info(dev, "not a signed image\n");
		goto not_signed_fw;
	}
	eof = fw->data + fw->size - sizeof(fw_cfg) - sizeof(fw_cfg_sig);

	memcpy((uint8_t *)&fw_cfg_sig,
	       fw->data + fw->size - sizeof(fw_cfg_sig), sizeof(fw_cfg_sig));

	/* flash fw config table and signature first */
	err = ccg_cmd_write_flash_row(uc, 0, (u8 *)&fw_cfg,
				      FLASH_FWCT1_WR_CMD);
	if (err)
		goto release_fw;

	err = ccg_cmd_write_flash_row(uc, 0, (u8 *)&fw_cfg + CCG4_ROW_SIZE,
				      FLASH_FWCT2_WR_CMD);
	if (err)
		goto release_fw;

	err = ccg_cmd_write_flash_row(uc, 0, &fw_cfg_sig,
				      FLASH_FWCT_SIG_WR_CMD);
	if (err)
		goto release_fw;

not_signed_fw:
	wr_buf = kzalloc(CCG4_ROW_SIZE + 4, GFP_KERNEL);
	if (!wr_buf) {
		err = -ENOMEM;
		goto release_fw;
	}

	err = ccg_cmd_enter_flashing(uc);
	if (err)
		goto release_mem;

	/*****************************************************************
	 * CCG firmware image (.cyacd) file line format
	 *
	 * :00rrrrllll[dd....]cc/r/n
	 *
	 * :00   header
	 * rrrr is row number to flash				(4 char)
	 * llll is data len to flash				(4 char)
	 * dd   is a data field represents one byte of data	(512 char)
	 * cc   is checksum					(2 char)
	 * \r\n newline
	 *
	 * Total length: 3 + 4 + 4 + 512 + 2 + 2 = 527
	 *
	 *****************************************************************/

	p = strnchr(fw->data, fw->size, ':');
	while (p < eof) {
		s = strnchr(p + 1, eof - p - 1, ':');

		if (!s)
			s = eof;

		line_sz = s - p;

		if (line_sz != CYACD_LINE_SIZE) {
			dev_err(dev, "Bad FW format line_sz=%d\n", line_sz);
			err =  -EINVAL;
			goto release_mem;
		}

		if (hex2bin(wr_buf, p + 3, CCG4_ROW_SIZE + 4)) {
			err =  -EINVAL;
			goto release_mem;
		}

		row = get_unaligned_be16(wr_buf);
		len = get_unaligned_be16(&wr_buf[2]);

		if (len != CCG4_ROW_SIZE) {
			err =  -EINVAL;
			goto release_mem;
		}

		err = ccg_cmd_write_flash_row(uc, row, wr_buf + 4,
					      FLASH_WR_CMD);
		if (err)
			goto release_mem;

		line_cnt++;
		p = s;
	}

	dev_info(dev, "total %d row flashed. time: %dms\n",
		 line_cnt, jiffies_to_msecs(jiffies - start_time));

	err = ccg_cmd_validate_fw(uc, (mode == PRIMARY) ? FW2 :  FW1);
	if (err)
		dev_err(dev, "%s validation failed err=%d\n",
			(mode == PRIMARY) ? "FW2" :  "FW1", err);
	else
		dev_info(dev, "%s validated\n",
			 (mode == PRIMARY) ? "FW2" :  "FW1");

	err = ccg_cmd_port_control(uc, false);
	if (err < 0)
		goto release_mem;

	err = ccg_cmd_reset(uc);
	if (err < 0)
		goto release_mem;

	err = ccg_cmd_port_control(uc, true);
	if (err < 0)
		goto release_mem;

release_mem:
	kfree(wr_buf);

release_fw:
	release_firmware(fw);
	return err;
}

/*******************************************************************************
 * CCG4 has two copies of the firmware in addition to the bootloader.
 * If the device is running FW1, FW2 can be updated with the new version.
 * Dual firmware mode allows the CCG device to stay in a PD contract and support
 * USB PD and Type-C functionality while a firmware update is in progress.
 ******************************************************************************/
static int ccg_fw_update(struct ucsi_ccg *uc, enum enum_flash_mode flash_mode)
{
	int err = 0;

	while (flash_mode != FLASH_NOT_NEEDED) {
		err = do_flash(uc, flash_mode);
		if (err < 0)
			return err;
		err = ccg_fw_update_needed(uc, &flash_mode);
		if (err < 0)
			return err;
	}
	dev_info(uc->dev, "CCG FW update successful\n");

	return err;
}

static int ccg_restart(struct ucsi_ccg *uc)
{
	struct device *dev = uc->dev;
	int status;

	status = ucsi_ccg_init(uc);
	if (status < 0) {
		dev_err(dev, "ucsi_ccg_start fail, err=%d\n", status);
		return status;
	}

	status = request_threaded_irq(uc->irq, NULL, ccg_irq_handler,
				      IRQF_ONESHOT | IRQF_TRIGGER_HIGH,
				      dev_name(dev), uc);
	if (status < 0) {
		dev_err(dev, "request_threaded_irq failed - %d\n", status);
		return status;
	}

	status = ucsi_register(uc->ucsi);
	if (status) {
		dev_err(uc->dev, "failed to register the interface\n");
		return status;
	}

	pm_runtime_enable(uc->dev);
	return 0;
}

static void ccg_update_firmware(struct work_struct *work)
{
	struct ucsi_ccg *uc = container_of(work, struct ucsi_ccg, work);
	enum enum_flash_mode flash_mode;
	int status;

	status = ccg_fw_update_needed(uc, &flash_mode);
	if (status < 0)
		return;

	if (flash_mode != FLASH_NOT_NEEDED) {
		ucsi_unregister(uc->ucsi);
		pm_runtime_disable(uc->dev);
		free_irq(uc->irq, uc);

		ccg_fw_update(uc, flash_mode);
		ccg_restart(uc);
	}
}

static ssize_t do_flash_store(struct device *dev,
			      struct device_attribute *attr,
			      const char *buf, size_t n)
{
	struct ucsi_ccg *uc = i2c_get_clientdata(to_i2c_client(dev));
	bool flash;

	if (kstrtobool(buf, &flash))
		return -EINVAL;

	if (!flash)
		return n;

	if (uc->fw_build == 0x0) {
		dev_err(dev, "fail to flash FW due to missing FW build info\n");
		return -EINVAL;
	}

	schedule_work(&uc->work);
	return n;
}

static DEVICE_ATTR_WO(do_flash);

static struct attribute *ucsi_ccg_attrs[] = {
	&dev_attr_do_flash.attr,
	NULL,
};
ATTRIBUTE_GROUPS(ucsi_ccg);

static int ucsi_ccg_probe(struct i2c_client *client,
			  const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct ucsi_ccg *uc;
	int status;

	uc = devm_kzalloc(dev, sizeof(*uc), GFP_KERNEL);
	if (!uc)
		return -ENOMEM;

	uc->dev = dev;
	uc->client = client;
	mutex_init(&uc->lock);
	init_completion(&uc->complete);
	INIT_WORK(&uc->work, ccg_update_firmware);
	INIT_WORK(&uc->pm_work, ccg_pm_workaround_work);

	/* Only fail FW flashing when FW build information is not provided */
	status = device_property_read_u16(dev, "ccgx,firmware-build",
					  &uc->fw_build);
	if (status)
		dev_err(uc->dev, "failed to get FW build information\n");

	/* reset ccg device and initialize ucsi */
	status = ucsi_ccg_init(uc);
	if (status < 0) {
		dev_err(uc->dev, "ucsi_ccg_init failed - %d\n", status);
		return status;
	}

	status = get_fw_info(uc);
	if (status < 0) {
		dev_err(uc->dev, "get_fw_info failed - %d\n", status);
		return status;
	}

	uc->port_num = 1;

	if (uc->info.mode & CCG_DEVINFO_PDPORTS_MASK)
		uc->port_num++;

	uc->ucsi = ucsi_create(dev, &ucsi_ccg_ops);
	if (IS_ERR(uc->ucsi))
		return PTR_ERR(uc->ucsi);

	ucsi_set_drvdata(uc->ucsi, uc);

	status = request_threaded_irq(client->irq, NULL, ccg_irq_handler,
				      IRQF_ONESHOT | IRQF_TRIGGER_HIGH,
				      dev_name(dev), uc);
	if (status < 0) {
		dev_err(uc->dev, "request_threaded_irq failed - %d\n", status);
		goto out_ucsi_destroy;
	}

	uc->irq = client->irq;

	status = ucsi_register(uc->ucsi);
	if (status)
		goto out_free_irq;

	i2c_set_clientdata(client, uc);

	pm_runtime_set_active(uc->dev);
	pm_runtime_enable(uc->dev);
	pm_runtime_use_autosuspend(uc->dev);
	pm_runtime_set_autosuspend_delay(uc->dev, 5000);
	pm_runtime_idle(uc->dev);

	return 0;

out_free_irq:
	free_irq(uc->irq, uc);
out_ucsi_destroy:
	ucsi_destroy(uc->ucsi);

	return status;
}

static int ucsi_ccg_remove(struct i2c_client *client)
{
	struct ucsi_ccg *uc = i2c_get_clientdata(client);

	cancel_work_sync(&uc->pm_work);
	cancel_work_sync(&uc->work);
	pm_runtime_disable(uc->dev);
	ucsi_unregister(uc->ucsi);
	ucsi_destroy(uc->ucsi);
	free_irq(uc->irq, uc);

	return 0;
}

static const struct i2c_device_id ucsi_ccg_device_id[] = {
	{"ccgx-ucsi", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, ucsi_ccg_device_id);

static int ucsi_ccg_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct ucsi_ccg *uc = i2c_get_clientdata(client);

	return ucsi_resume(uc->ucsi);
}

static int ucsi_ccg_runtime_suspend(struct device *dev)
{
	return 0;
}

static int ucsi_ccg_runtime_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct ucsi_ccg *uc = i2c_get_clientdata(client);

	/*
	 * Firmware version 3.1.10 or earlier, built for NVIDIA has known issue
	 * of missing interrupt when a device is connected for runtime resume.
	 * Schedule a work to call ISR as a workaround.
	 */
	if (uc->fw_build == CCG_FW_BUILD_NVIDIA &&
	    uc->fw_version <= CCG_OLD_FW_VERSION)
		schedule_work(&uc->pm_work);

	return 0;
}

static const struct dev_pm_ops ucsi_ccg_pm = {
	.resume = ucsi_ccg_resume,
	.runtime_suspend = ucsi_ccg_runtime_suspend,
	.runtime_resume = ucsi_ccg_runtime_resume,
};

static struct i2c_driver ucsi_ccg_driver = {
	.driver = {
		.name = "ucsi_ccg",
		.pm = &ucsi_ccg_pm,
		.dev_groups = ucsi_ccg_groups,
	},
	.probe = ucsi_ccg_probe,
	.remove = ucsi_ccg_remove,
	.id_table = ucsi_ccg_device_id,
};

module_i2c_driver(ucsi_ccg_driver);

MODULE_AUTHOR("Ajay Gupta <ajayg@nvidia.com>");
MODULE_DESCRIPTION("UCSI driver for Cypress CCGx Type-C controller");
MODULE_LICENSE("GPL v2");

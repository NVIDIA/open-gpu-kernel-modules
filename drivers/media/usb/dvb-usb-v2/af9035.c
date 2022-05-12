// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Afatech AF9035 DVB USB driver
 *
 * Copyright (C) 2009 Antti Palosaari <crope@iki.fi>
 * Copyright (C) 2012 Antti Palosaari <crope@iki.fi>
 */

#include "af9035.h"

/* Max transfer size done by I2C transfer functions */
#define MAX_XFER_SIZE  64

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

static u16 af9035_checksum(const u8 *buf, size_t len)
{
	size_t i;
	u16 checksum = 0;

	for (i = 1; i < len; i++) {
		if (i % 2)
			checksum += buf[i] << 8;
		else
			checksum += buf[i];
	}
	checksum = ~checksum;

	return checksum;
}

static int af9035_ctrl_msg(struct dvb_usb_device *d, struct usb_req *req)
{
#define REQ_HDR_LEN 4 /* send header size */
#define ACK_HDR_LEN 3 /* rece header size */
#define CHECKSUM_LEN 2
#define USB_TIMEOUT 2000
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret, wlen, rlen;
	u16 checksum, tmp_checksum;

	mutex_lock(&d->usb_mutex);

	/* buffer overflow check */
	if (req->wlen > (BUF_LEN - REQ_HDR_LEN - CHECKSUM_LEN) ||
			req->rlen > (BUF_LEN - ACK_HDR_LEN - CHECKSUM_LEN)) {
		dev_err(&intf->dev, "too much data wlen=%d rlen=%d\n",
			req->wlen, req->rlen);
		ret = -EINVAL;
		goto exit;
	}

	state->buf[0] = REQ_HDR_LEN + req->wlen + CHECKSUM_LEN - 1;
	state->buf[1] = req->mbox;
	state->buf[2] = req->cmd;
	state->buf[3] = state->seq++;
	memcpy(&state->buf[REQ_HDR_LEN], req->wbuf, req->wlen);

	wlen = REQ_HDR_LEN + req->wlen + CHECKSUM_LEN;
	rlen = ACK_HDR_LEN + req->rlen + CHECKSUM_LEN;

	/* calc and add checksum */
	checksum = af9035_checksum(state->buf, state->buf[0] - 1);
	state->buf[state->buf[0] - 1] = (checksum >> 8);
	state->buf[state->buf[0] - 0] = (checksum & 0xff);

	/* no ack for these packets */
	if (req->cmd == CMD_FW_DL)
		rlen = 0;

	ret = dvb_usbv2_generic_rw_locked(d,
			state->buf, wlen, state->buf, rlen);
	if (ret)
		goto exit;

	/* no ack for those packets */
	if (req->cmd == CMD_FW_DL)
		goto exit;

	/* verify checksum */
	checksum = af9035_checksum(state->buf, rlen - 2);
	tmp_checksum = (state->buf[rlen - 2] << 8) | state->buf[rlen - 1];
	if (tmp_checksum != checksum) {
		dev_err(&intf->dev, "command=%02x checksum mismatch (%04x != %04x)\n",
			req->cmd, tmp_checksum, checksum);
		ret = -EIO;
		goto exit;
	}

	/* check status */
	if (state->buf[2]) {
		/* fw returns status 1 when IR code was not received */
		if (req->cmd == CMD_IR_GET || state->buf[2] == 1) {
			ret = 1;
			goto exit;
		}

		dev_dbg(&intf->dev, "command=%02x failed fw error=%d\n",
			req->cmd, state->buf[2]);
		ret = -EIO;
		goto exit;
	}

	/* read request, copy returned data to return buf */
	if (req->rlen)
		memcpy(req->rbuf, &state->buf[ACK_HDR_LEN], req->rlen);
exit:
	mutex_unlock(&d->usb_mutex);
	return ret;
}

/* write multiple registers */
static int af9035_wr_regs(struct dvb_usb_device *d, u32 reg, u8 *val, int len)
{
	struct usb_interface *intf = d->intf;
	u8 wbuf[MAX_XFER_SIZE];
	u8 mbox = (reg >> 16) & 0xff;
	struct usb_req req = { CMD_MEM_WR, mbox, 6 + len, wbuf, 0, NULL };

	if (6 + len > sizeof(wbuf)) {
		dev_warn(&intf->dev, "i2c wr: len=%d is too big!\n", len);
		return -EOPNOTSUPP;
	}

	wbuf[0] = len;
	wbuf[1] = 2;
	wbuf[2] = 0;
	wbuf[3] = 0;
	wbuf[4] = (reg >> 8) & 0xff;
	wbuf[5] = (reg >> 0) & 0xff;
	memcpy(&wbuf[6], val, len);

	return af9035_ctrl_msg(d, &req);
}

/* read multiple registers */
static int af9035_rd_regs(struct dvb_usb_device *d, u32 reg, u8 *val, int len)
{
	u8 wbuf[] = { len, 2, 0, 0, (reg >> 8) & 0xff, reg & 0xff };
	u8 mbox = (reg >> 16) & 0xff;
	struct usb_req req = { CMD_MEM_RD, mbox, sizeof(wbuf), wbuf, len, val };

	return af9035_ctrl_msg(d, &req);
}

/* write single register */
static int af9035_wr_reg(struct dvb_usb_device *d, u32 reg, u8 val)
{
	return af9035_wr_regs(d, reg, &val, 1);
}

/* read single register */
static int af9035_rd_reg(struct dvb_usb_device *d, u32 reg, u8 *val)
{
	return af9035_rd_regs(d, reg, val, 1);
}

/* write single register with mask */
static int af9035_wr_reg_mask(struct dvb_usb_device *d, u32 reg, u8 val,
		u8 mask)
{
	int ret;
	u8 tmp;

	/* no need for read if whole reg is written */
	if (mask != 0xff) {
		ret = af9035_rd_regs(d, reg, &tmp, 1);
		if (ret)
			return ret;

		val &= mask;
		tmp &= ~mask;
		val |= tmp;
	}

	return af9035_wr_regs(d, reg, &val, 1);
}

static int af9035_add_i2c_dev(struct dvb_usb_device *d, const char *type,
		u8 addr, void *platform_data, struct i2c_adapter *adapter)
{
	int ret, num;
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	struct i2c_client *client;
	struct i2c_board_info board_info = {
		.addr = addr,
		.platform_data = platform_data,
	};

	strscpy(board_info.type, type, I2C_NAME_SIZE);

	/* find first free client */
	for (num = 0; num < AF9035_I2C_CLIENT_MAX; num++) {
		if (state->i2c_client[num] == NULL)
			break;
	}

	dev_dbg(&intf->dev, "num=%d\n", num);

	if (num == AF9035_I2C_CLIENT_MAX) {
		dev_err(&intf->dev, "I2C client out of index\n");
		ret = -ENODEV;
		goto err;
	}

	request_module("%s", board_info.type);

	/* register I2C device */
	client = i2c_new_client_device(adapter, &board_info);
	if (!i2c_client_has_driver(client)) {
		ret = -ENODEV;
		goto err;
	}

	/* increase I2C driver usage count */
	if (!try_module_get(client->dev.driver->owner)) {
		i2c_unregister_device(client);
		ret = -ENODEV;
		goto err;
	}

	state->i2c_client[num] = client;
	return 0;
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

static void af9035_del_i2c_dev(struct dvb_usb_device *d)
{
	int num;
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	struct i2c_client *client;

	/* find last used client */
	num = AF9035_I2C_CLIENT_MAX;
	while (num--) {
		if (state->i2c_client[num] != NULL)
			break;
	}

	dev_dbg(&intf->dev, "num=%d\n", num);

	if (num == -1) {
		dev_err(&intf->dev, "I2C client out of index\n");
		goto err;
	}

	client = state->i2c_client[num];

	/* decrease I2C driver usage count */
	module_put(client->dev.driver->owner);

	/* unregister I2C device */
	i2c_unregister_device(client);

	state->i2c_client[num] = NULL;
	return;
err:
	dev_dbg(&intf->dev, "failed\n");
}

static int af9035_i2c_master_xfer(struct i2c_adapter *adap,
		struct i2c_msg msg[], int num)
{
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	struct state *state = d_to_priv(d);
	int ret;

	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	/*
	 * AF9035 I2C sub header is 5 bytes long. Meaning of those bytes are:
	 * 0: data len
	 * 1: I2C addr << 1
	 * 2: reg addr len
	 *    byte 3 and 4 can be used as reg addr
	 * 3: reg addr MSB
	 *    used when reg addr len is set to 2
	 * 4: reg addr LSB
	 *    used when reg addr len is set to 1 or 2
	 *
	 * For the simplify we do not use register addr at all.
	 * NOTE: As a firmware knows tuner type there is very small possibility
	 * there could be some tuner I2C hacks done by firmware and this may
	 * lead problems if firmware expects those bytes are used.
	 *
	 * TODO: Here is few hacks. AF9035 chip integrates AF9033 demodulator.
	 * IT9135 chip integrates AF9033 demodulator and RF tuner. For dual
	 * tuner devices, there is also external AF9033 demodulator connected
	 * via external I2C bus. All AF9033 demod I2C traffic, both single and
	 * dual tuner configuration, is covered by firmware - actual USB IO
	 * looks just like a memory access.
	 * In case of IT913x chip, there is own tuner driver. It is implemented
	 * currently as a I2C driver, even tuner IP block is likely build
	 * directly into the demodulator memory space and there is no own I2C
	 * bus. I2C subsystem does not allow register multiple devices to same
	 * bus, having same slave address. Due to that we reuse demod address,
	 * shifted by one bit, on that case.
	 *
	 * For IT930x we use a different command and the sub header is
	 * different as well:
	 * 0: data len
	 * 1: I2C bus (0x03 seems to be only value used)
	 * 2: I2C addr << 1
	 */
#define AF9035_IS_I2C_XFER_WRITE_READ(_msg, _num) \
	(_num == 2 && !(_msg[0].flags & I2C_M_RD) && (_msg[1].flags & I2C_M_RD))
#define AF9035_IS_I2C_XFER_WRITE(_msg, _num) \
	(_num == 1 && !(_msg[0].flags & I2C_M_RD))
#define AF9035_IS_I2C_XFER_READ(_msg, _num) \
	(_num == 1 && (_msg[0].flags & I2C_M_RD))

	if (AF9035_IS_I2C_XFER_WRITE_READ(msg, num)) {
		if (msg[0].len > 40 || msg[1].len > 40) {
			/* TODO: correct limits > 40 */
			ret = -EOPNOTSUPP;
		} else if ((msg[0].addr == state->af9033_i2c_addr[0]) ||
			   (msg[0].addr == state->af9033_i2c_addr[1])) {
			/* demod access via firmware interface */
			u32 reg = msg[0].buf[0] << 16 | msg[0].buf[1] << 8 |
					msg[0].buf[2];

			if (msg[0].addr == state->af9033_i2c_addr[1])
				reg |= 0x100000;

			ret = af9035_rd_regs(d, reg, &msg[1].buf[0],
					msg[1].len);
		} else if (state->no_read) {
			memset(msg[1].buf, 0, msg[1].len);
			ret = 0;
		} else {
			/* I2C write + read */
			u8 buf[MAX_XFER_SIZE];
			struct usb_req req = { CMD_I2C_RD, 0, 5 + msg[0].len,
					buf, msg[1].len, msg[1].buf };

			if (state->chip_type == 0x9306) {
				req.cmd = CMD_GENERIC_I2C_RD;
				req.wlen = 3 + msg[0].len;
			}
			req.mbox |= ((msg[0].addr & 0x80)  >>  3);

			buf[0] = msg[1].len;
			if (state->chip_type == 0x9306) {
				buf[1] = 0x03; /* I2C bus */
				buf[2] = msg[0].addr << 1;
				memcpy(&buf[3], msg[0].buf, msg[0].len);
			} else {
				buf[1] = msg[0].addr << 1;
				buf[3] = 0x00; /* reg addr MSB */
				buf[4] = 0x00; /* reg addr LSB */

				/* Keep prev behavior for write req len > 2*/
				if (msg[0].len > 2) {
					buf[2] = 0x00; /* reg addr len */
					memcpy(&buf[5], msg[0].buf, msg[0].len);

				/* Use reg addr fields if write req len <= 2 */
				} else {
					req.wlen = 5;
					buf[2] = msg[0].len;
					if (msg[0].len == 2) {
						buf[3] = msg[0].buf[0];
						buf[4] = msg[0].buf[1];
					} else if (msg[0].len == 1) {
						buf[4] = msg[0].buf[0];
					}
				}
			}
			ret = af9035_ctrl_msg(d, &req);
		}
	} else if (AF9035_IS_I2C_XFER_WRITE(msg, num)) {
		if (msg[0].len > 40) {
			/* TODO: correct limits > 40 */
			ret = -EOPNOTSUPP;
		} else if ((msg[0].addr == state->af9033_i2c_addr[0]) ||
			   (msg[0].addr == state->af9033_i2c_addr[1])) {
			/* demod access via firmware interface */
			u32 reg = msg[0].buf[0] << 16 | msg[0].buf[1] << 8 |
					msg[0].buf[2];

			if (msg[0].addr == state->af9033_i2c_addr[1])
				reg |= 0x100000;

			ret = (msg[0].len >= 3) ? af9035_wr_regs(d, reg,
							         &msg[0].buf[3],
							         msg[0].len - 3)
					        : -EOPNOTSUPP;
		} else {
			/* I2C write */
			u8 buf[MAX_XFER_SIZE];
			struct usb_req req = { CMD_I2C_WR, 0, 5 + msg[0].len,
					buf, 0, NULL };

			if (state->chip_type == 0x9306) {
				req.cmd = CMD_GENERIC_I2C_WR;
				req.wlen = 3 + msg[0].len;
			}

			req.mbox |= ((msg[0].addr & 0x80)  >>  3);
			buf[0] = msg[0].len;
			if (state->chip_type == 0x9306) {
				buf[1] = 0x03; /* I2C bus */
				buf[2] = msg[0].addr << 1;
				memcpy(&buf[3], msg[0].buf, msg[0].len);
			} else {
				buf[1] = msg[0].addr << 1;
				buf[2] = 0x00; /* reg addr len */
				buf[3] = 0x00; /* reg addr MSB */
				buf[4] = 0x00; /* reg addr LSB */
				memcpy(&buf[5], msg[0].buf, msg[0].len);
			}
			ret = af9035_ctrl_msg(d, &req);
		}
	} else if (AF9035_IS_I2C_XFER_READ(msg, num)) {
		if (msg[0].len > 40) {
			/* TODO: correct limits > 40 */
			ret = -EOPNOTSUPP;
		} else if (state->no_read) {
			memset(msg[0].buf, 0, msg[0].len);
			ret = 0;
		} else {
			/* I2C read */
			u8 buf[5];
			struct usb_req req = { CMD_I2C_RD, 0, sizeof(buf),
						buf, msg[0].len, msg[0].buf };

			if (state->chip_type == 0x9306) {
				req.cmd = CMD_GENERIC_I2C_RD;
				req.wlen = 3;
			}
			req.mbox |= ((msg[0].addr & 0x80)  >>  3);
			buf[0] = msg[0].len;
			if (state->chip_type == 0x9306) {
				buf[1] = 0x03; /* I2C bus */
				buf[2] = msg[0].addr << 1;
			} else {
				buf[1] = msg[0].addr << 1;
				buf[2] = 0x00; /* reg addr len */
				buf[3] = 0x00; /* reg addr MSB */
				buf[4] = 0x00; /* reg addr LSB */
			}
			ret = af9035_ctrl_msg(d, &req);
		}
	} else {
		/*
		 * We support only three kind of I2C transactions:
		 * 1) 1 x write + 1 x read (repeated start)
		 * 2) 1 x write
		 * 3) 1 x read
		 */
		ret = -EOPNOTSUPP;
	}

	mutex_unlock(&d->i2c_mutex);

	if (ret < 0)
		return ret;
	else
		return num;
}

static u32 af9035_i2c_functionality(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C;
}

static struct i2c_algorithm af9035_i2c_algo = {
	.master_xfer = af9035_i2c_master_xfer,
	.functionality = af9035_i2c_functionality,
};

static int af9035_identify_state(struct dvb_usb_device *d, const char **name)
{
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret, i, ts_mode_invalid;
	unsigned int utmp, eeprom_addr;
	u8 tmp;
	u8 wbuf[1] = { 1 };
	u8 rbuf[4];
	struct usb_req req = { CMD_FW_QUERYINFO, 0, sizeof(wbuf), wbuf,
			sizeof(rbuf), rbuf };

	ret = af9035_rd_regs(d, 0x1222, rbuf, 3);
	if (ret < 0)
		goto err;

	state->chip_version = rbuf[0];
	state->chip_type = rbuf[2] << 8 | rbuf[1] << 0;

	ret = af9035_rd_reg(d, 0x384f, &state->prechip_version);
	if (ret < 0)
		goto err;

	dev_info(&intf->dev, "prechip_version=%02x chip_version=%02x chip_type=%04x\n",
		 state->prechip_version, state->chip_version, state->chip_type);

	if (state->chip_type == 0x9135) {
		if (state->chip_version == 0x02) {
			*name = AF9035_FIRMWARE_IT9135_V2;
			utmp = 0x00461d;
		} else {
			*name = AF9035_FIRMWARE_IT9135_V1;
			utmp = 0x00461b;
		}

		/* Check if eeprom exists */
		ret = af9035_rd_reg(d, utmp, &tmp);
		if (ret < 0)
			goto err;

		if (tmp == 0x00) {
			dev_dbg(&intf->dev, "no eeprom\n");
			state->no_eeprom = true;
			goto check_firmware_status;
		}

		eeprom_addr = EEPROM_BASE_IT9135;
	} else if (state->chip_type == 0x9306) {
		*name = AF9035_FIRMWARE_IT9303;
		state->no_eeprom = true;
		goto check_firmware_status;
	} else {
		*name = AF9035_FIRMWARE_AF9035;
		eeprom_addr = EEPROM_BASE_AF9035;
	}

	/* Read and store eeprom */
	for (i = 0; i < 256; i += 32) {
		ret = af9035_rd_regs(d, eeprom_addr + i, &state->eeprom[i], 32);
		if (ret < 0)
			goto err;
	}

	dev_dbg(&intf->dev, "eeprom dump:\n");
	for (i = 0; i < 256; i += 16)
		dev_dbg(&intf->dev, "%*ph\n", 16, &state->eeprom[i]);

	/* check for dual tuner mode */
	tmp = state->eeprom[EEPROM_TS_MODE];
	ts_mode_invalid = 0;
	switch (tmp) {
	case 0:
		break;
	case 1:
	case 3:
		state->dual_mode = true;
		break;
	case 5:
		if (state->chip_type != 0x9135 && state->chip_type != 0x9306)
			state->dual_mode = true;	/* AF9035 */
		else
			ts_mode_invalid = 1;
		break;
	default:
		ts_mode_invalid = 1;
	}

	dev_dbg(&intf->dev, "ts mode=%d dual mode=%d\n", tmp, state->dual_mode);

	if (ts_mode_invalid)
		dev_info(&intf->dev, "ts mode=%d not supported, defaulting to single tuner mode!", tmp);

check_firmware_status:
	ret = af9035_ctrl_msg(d, &req);
	if (ret < 0)
		goto err;

	dev_dbg(&intf->dev, "reply=%*ph\n", 4, rbuf);
	if (rbuf[0] || rbuf[1] || rbuf[2] || rbuf[3])
		ret = WARM;
	else
		ret = COLD;

	return ret;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_download_firmware_old(struct dvb_usb_device *d,
		const struct firmware *fw)
{
	struct usb_interface *intf = d->intf;
	int ret, i, j, len;
	u8 wbuf[1];
	struct usb_req req = { 0, 0, 0, NULL, 0, NULL };
	struct usb_req req_fw_dl = { CMD_FW_DL, 0, 0, wbuf, 0, NULL };
	u8 hdr_core;
	u16 hdr_addr, hdr_data_len, hdr_checksum;
	#define MAX_DATA 58
	#define HDR_SIZE 7

	/*
	 * Thanks to Daniel Glöckner <daniel-gl@gmx.net> about that info!
	 *
	 * byte 0: MCS 51 core
	 *  There are two inside the AF9035 (1=Link and 2=OFDM) with separate
	 *  address spaces
	 * byte 1-2: Big endian destination address
	 * byte 3-4: Big endian number of data bytes following the header
	 * byte 5-6: Big endian header checksum, apparently ignored by the chip
	 *  Calculated as ~(h[0]*256+h[1]+h[2]*256+h[3]+h[4]*256)
	 */

	for (i = fw->size; i > HDR_SIZE;) {
		hdr_core = fw->data[fw->size - i + 0];
		hdr_addr = fw->data[fw->size - i + 1] << 8;
		hdr_addr |= fw->data[fw->size - i + 2] << 0;
		hdr_data_len = fw->data[fw->size - i + 3] << 8;
		hdr_data_len |= fw->data[fw->size - i + 4] << 0;
		hdr_checksum = fw->data[fw->size - i + 5] << 8;
		hdr_checksum |= fw->data[fw->size - i + 6] << 0;

		dev_dbg(&intf->dev, "core=%d addr=%04x data_len=%d checksum=%04x\n",
			hdr_core, hdr_addr, hdr_data_len, hdr_checksum);

		if (((hdr_core != 1) && (hdr_core != 2)) ||
				(hdr_data_len > i)) {
			dev_dbg(&intf->dev, "bad firmware\n");
			break;
		}

		/* download begin packet */
		req.cmd = CMD_FW_DL_BEGIN;
		ret = af9035_ctrl_msg(d, &req);
		if (ret < 0)
			goto err;

		/* download firmware packet(s) */
		for (j = HDR_SIZE + hdr_data_len; j > 0; j -= MAX_DATA) {
			len = j;
			if (len > MAX_DATA)
				len = MAX_DATA;
			req_fw_dl.wlen = len;
			req_fw_dl.wbuf = (u8 *) &fw->data[fw->size - i +
					HDR_SIZE + hdr_data_len - j];
			ret = af9035_ctrl_msg(d, &req_fw_dl);
			if (ret < 0)
				goto err;
		}

		/* download end packet */
		req.cmd = CMD_FW_DL_END;
		ret = af9035_ctrl_msg(d, &req);
		if (ret < 0)
			goto err;

		i -= hdr_data_len + HDR_SIZE;

		dev_dbg(&intf->dev, "data uploaded=%zu\n", fw->size - i);
	}

	/* print warn if firmware is bad, continue and see what happens */
	if (i)
		dev_warn(&intf->dev, "bad firmware\n");

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_download_firmware_new(struct dvb_usb_device *d,
		const struct firmware *fw)
{
	struct usb_interface *intf = d->intf;
	int ret, i, i_prev;
	struct usb_req req_fw_dl = { CMD_FW_SCATTER_WR, 0, 0, NULL, 0, NULL };
	#define HDR_SIZE 7

	/*
	 * There seems to be following firmware header. Meaning of bytes 0-3
	 * is unknown.
	 *
	 * 0: 3
	 * 1: 0, 1
	 * 2: 0
	 * 3: 1, 2, 3
	 * 4: addr MSB
	 * 5: addr LSB
	 * 6: count of data bytes ?
	 */
	for (i = HDR_SIZE, i_prev = 0; i <= fw->size; i++) {
		if (i == fw->size ||
				(fw->data[i + 0] == 0x03 &&
				(fw->data[i + 1] == 0x00 ||
				fw->data[i + 1] == 0x01) &&
				fw->data[i + 2] == 0x00)) {
			req_fw_dl.wlen = i - i_prev;
			req_fw_dl.wbuf = (u8 *) &fw->data[i_prev];
			i_prev = i;
			ret = af9035_ctrl_msg(d, &req_fw_dl);
			if (ret < 0)
				goto err;

			dev_dbg(&intf->dev, "data uploaded=%d\n", i);
		}
	}

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_download_firmware(struct dvb_usb_device *d,
		const struct firmware *fw)
{
	struct usb_interface *intf = d->intf;
	struct state *state = d_to_priv(d);
	int ret;
	u8 wbuf[1];
	u8 rbuf[4];
	u8 tmp;
	struct usb_req req = { 0, 0, 0, NULL, 0, NULL };
	struct usb_req req_fw_ver = { CMD_FW_QUERYINFO, 0, 1, wbuf, 4, rbuf };

	dev_dbg(&intf->dev, "\n");

	/*
	 * In case of dual tuner configuration we need to do some extra
	 * initialization in order to download firmware to slave demod too,
	 * which is done by master demod.
	 * Master feeds also clock and controls power via GPIO.
	 */
	if (state->dual_mode) {
		/* configure gpioh1, reset & power slave demod */
		ret = af9035_wr_reg_mask(d, 0x00d8b0, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0x00d8b1, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0x00d8af, 0x00, 0x01);
		if (ret < 0)
			goto err;

		usleep_range(10000, 50000);

		ret = af9035_wr_reg_mask(d, 0x00d8af, 0x01, 0x01);
		if (ret < 0)
			goto err;

		/* tell the slave I2C address */
		tmp = state->eeprom[EEPROM_2ND_DEMOD_ADDR];

		/* Use default I2C address if eeprom has no address set */
		if (!tmp)
			tmp = 0x1d << 1; /* 8-bit format used by chip */

		if ((state->chip_type == 0x9135) ||
				(state->chip_type == 0x9306)) {
			ret = af9035_wr_reg(d, 0x004bfb, tmp);
			if (ret < 0)
				goto err;
		} else {
			ret = af9035_wr_reg(d, 0x00417f, tmp);
			if (ret < 0)
				goto err;

			/* enable clock out */
			ret = af9035_wr_reg_mask(d, 0x00d81a, 0x01, 0x01);
			if (ret < 0)
				goto err;
		}
	}

	if (fw->data[0] == 0x01)
		ret = af9035_download_firmware_old(d, fw);
	else
		ret = af9035_download_firmware_new(d, fw);
	if (ret < 0)
		goto err;

	/* firmware loaded, request boot */
	req.cmd = CMD_FW_BOOT;
	ret = af9035_ctrl_msg(d, &req);
	if (ret < 0)
		goto err;

	/* ensure firmware starts */
	wbuf[0] = 1;
	ret = af9035_ctrl_msg(d, &req_fw_ver);
	if (ret < 0)
		goto err;

	if (!(rbuf[0] || rbuf[1] || rbuf[2] || rbuf[3])) {
		dev_err(&intf->dev, "firmware did not run\n");
		ret = -ENODEV;
		goto err;
	}

	dev_info(&intf->dev, "firmware version=%d.%d.%d.%d",
		 rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_read_config(struct dvb_usb_device *d)
{
	struct usb_interface *intf = d->intf;
	struct state *state = d_to_priv(d);
	int ret, i;
	u8 tmp;
	u16 tmp16;

	/* Demod I2C address */
	state->af9033_i2c_addr[0] = 0x1c;
	state->af9033_i2c_addr[1] = 0x1d;
	state->af9033_config[0].adc_multiplier = AF9033_ADC_MULTIPLIER_2X;
	state->af9033_config[1].adc_multiplier = AF9033_ADC_MULTIPLIER_2X;
	state->af9033_config[0].ts_mode = AF9033_TS_MODE_USB;
	state->af9033_config[1].ts_mode = AF9033_TS_MODE_SERIAL;
	state->it930x_addresses = 0;

	if (state->chip_type == 0x9135) {
		/* feed clock for integrated RF tuner */
		state->af9033_config[0].dyn0_clk = true;
		state->af9033_config[1].dyn0_clk = true;

		if (state->chip_version == 0x02) {
			state->af9033_config[0].tuner = AF9033_TUNER_IT9135_60;
			state->af9033_config[1].tuner = AF9033_TUNER_IT9135_60;
		} else {
			state->af9033_config[0].tuner = AF9033_TUNER_IT9135_38;
			state->af9033_config[1].tuner = AF9033_TUNER_IT9135_38;
		}

		if (state->no_eeprom) {
			/* Remote controller to NEC polling by default */
			state->ir_mode = 0x05;
			state->ir_type = 0x00;

			goto skip_eeprom;
		}
	} else if (state->chip_type == 0x9306) {
		/*
		 * IT930x is an USB bridge, only single demod-single tuner
		 * configurations seen so far.
		 */
		if ((le16_to_cpu(d->udev->descriptor.idVendor) == USB_VID_AVERMEDIA) &&
		    (le16_to_cpu(d->udev->descriptor.idProduct) == USB_PID_AVERMEDIA_TD310)) {
			state->it930x_addresses = 1;
		}
		return 0;
	}

	/* Remote controller */
	state->ir_mode = state->eeprom[EEPROM_IR_MODE];
	state->ir_type = state->eeprom[EEPROM_IR_TYPE];

	if (state->dual_mode) {
		/* Read 2nd demodulator I2C address. 8-bit format on eeprom */
		tmp = state->eeprom[EEPROM_2ND_DEMOD_ADDR];
		if (tmp)
			state->af9033_i2c_addr[1] = tmp >> 1;

		dev_dbg(&intf->dev, "2nd demod I2C addr=%02x\n",
			state->af9033_i2c_addr[1]);
	}

	for (i = 0; i < state->dual_mode + 1; i++) {
		unsigned int eeprom_offset = 0;

		/* tuner */
		tmp = state->eeprom[EEPROM_1_TUNER_ID + eeprom_offset];
		dev_dbg(&intf->dev, "[%d]tuner=%02x\n", i, tmp);

		/* tuner sanity check */
		if (state->chip_type == 0x9135) {
			if (state->chip_version == 0x02) {
				/* IT9135 BX (v2) */
				switch (tmp) {
				case AF9033_TUNER_IT9135_60:
				case AF9033_TUNER_IT9135_61:
				case AF9033_TUNER_IT9135_62:
					state->af9033_config[i].tuner = tmp;
					break;
				}
			} else {
				/* IT9135 AX (v1) */
				switch (tmp) {
				case AF9033_TUNER_IT9135_38:
				case AF9033_TUNER_IT9135_51:
				case AF9033_TUNER_IT9135_52:
					state->af9033_config[i].tuner = tmp;
					break;
				}
			}
		} else {
			/* AF9035 */
			state->af9033_config[i].tuner = tmp;
		}

		if (state->af9033_config[i].tuner != tmp) {
			dev_info(&intf->dev, "[%d] overriding tuner from %02x to %02x\n",
				 i, tmp, state->af9033_config[i].tuner);
		}

		switch (state->af9033_config[i].tuner) {
		case AF9033_TUNER_TUA9001:
		case AF9033_TUNER_FC0011:
		case AF9033_TUNER_MXL5007T:
		case AF9033_TUNER_TDA18218:
		case AF9033_TUNER_FC2580:
		case AF9033_TUNER_FC0012:
			state->af9033_config[i].spec_inv = 1;
			break;
		case AF9033_TUNER_IT9135_38:
		case AF9033_TUNER_IT9135_51:
		case AF9033_TUNER_IT9135_52:
		case AF9033_TUNER_IT9135_60:
		case AF9033_TUNER_IT9135_61:
		case AF9033_TUNER_IT9135_62:
			break;
		default:
			dev_warn(&intf->dev, "tuner id=%02x not supported, please report!",
				 tmp);
		}

		/* disable dual mode if driver does not support it */
		if (i == 1)
			switch (state->af9033_config[i].tuner) {
			case AF9033_TUNER_FC0012:
			case AF9033_TUNER_IT9135_38:
			case AF9033_TUNER_IT9135_51:
			case AF9033_TUNER_IT9135_52:
			case AF9033_TUNER_IT9135_60:
			case AF9033_TUNER_IT9135_61:
			case AF9033_TUNER_IT9135_62:
			case AF9033_TUNER_MXL5007T:
				break;
			default:
				state->dual_mode = false;
				dev_info(&intf->dev, "driver does not support 2nd tuner and will disable it");
		}

		/* tuner IF frequency */
		tmp = state->eeprom[EEPROM_1_IF_L + eeprom_offset];
		tmp16 = tmp << 0;
		tmp = state->eeprom[EEPROM_1_IF_H + eeprom_offset];
		tmp16 |= tmp << 8;
		dev_dbg(&intf->dev, "[%d]IF=%d\n", i, tmp16);

		eeprom_offset += 0x10; /* shift for the 2nd tuner params */
	}

skip_eeprom:
	/* get demod clock */
	ret = af9035_rd_reg(d, 0x00d800, &tmp);
	if (ret < 0)
		goto err;

	tmp = (tmp >> 0) & 0x0f;

	for (i = 0; i < ARRAY_SIZE(state->af9033_config); i++) {
		if (state->chip_type == 0x9135)
			state->af9033_config[i].clock = clock_lut_it9135[tmp];
		else
			state->af9033_config[i].clock = clock_lut_af9035[tmp];
	}

	state->no_read = false;
	/* Some MXL5007T devices cannot properly handle tuner I2C read ops. */
	if (state->af9033_config[0].tuner == AF9033_TUNER_MXL5007T &&
		le16_to_cpu(d->udev->descriptor.idVendor) == USB_VID_AVERMEDIA)

		switch (le16_to_cpu(d->udev->descriptor.idProduct)) {
		case USB_PID_AVERMEDIA_A867:
		case USB_PID_AVERMEDIA_TWINSTAR:
			dev_info(&intf->dev,
				 "Device may have issues with I2C read operations. Enabling fix.\n");
			state->no_read = true;
			break;
		}

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_tua9001_tuner_callback(struct dvb_usb_device *d,
		int cmd, int arg)
{
	struct usb_interface *intf = d->intf;
	int ret;
	u8 val;

	dev_dbg(&intf->dev, "cmd=%d arg=%d\n", cmd, arg);

	/*
	 * CEN     always enabled by hardware wiring
	 * RESETN  GPIOT3
	 * RXEN    GPIOT2
	 */

	switch (cmd) {
	case TUA9001_CMD_RESETN:
		if (arg)
			val = 0x00;
		else
			val = 0x01;

		ret = af9035_wr_reg_mask(d, 0x00d8e7, val, 0x01);
		if (ret < 0)
			goto err;
		break;
	case TUA9001_CMD_RXEN:
		if (arg)
			val = 0x01;
		else
			val = 0x00;

		ret = af9035_wr_reg_mask(d, 0x00d8eb, val, 0x01);
		if (ret < 0)
			goto err;
		break;
	}

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}


static int af9035_fc0011_tuner_callback(struct dvb_usb_device *d,
		int cmd, int arg)
{
	struct usb_interface *intf = d->intf;
	int ret;

	switch (cmd) {
	case FC0011_FE_CALLBACK_POWER:
		/* Tuner enable */
		ret = af9035_wr_reg_mask(d, 0xd8eb, 1, 1);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0xd8ec, 1, 1);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0xd8ed, 1, 1);
		if (ret < 0)
			goto err;

		/* LED */
		ret = af9035_wr_reg_mask(d, 0xd8d0, 1, 1);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0xd8d1, 1, 1);
		if (ret < 0)
			goto err;

		usleep_range(10000, 50000);
		break;
	case FC0011_FE_CALLBACK_RESET:
		ret = af9035_wr_reg(d, 0xd8e9, 1);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg(d, 0xd8e8, 1);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg(d, 0xd8e7, 1);
		if (ret < 0)
			goto err;

		usleep_range(10000, 20000);

		ret = af9035_wr_reg(d, 0xd8e7, 0);
		if (ret < 0)
			goto err;

		usleep_range(10000, 20000);
		break;
	default:
		ret = -EINVAL;
		goto err;
	}

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_tuner_callback(struct dvb_usb_device *d, int cmd, int arg)
{
	struct state *state = d_to_priv(d);

	switch (state->af9033_config[0].tuner) {
	case AF9033_TUNER_FC0011:
		return af9035_fc0011_tuner_callback(d, cmd, arg);
	case AF9033_TUNER_TUA9001:
		return af9035_tua9001_tuner_callback(d, cmd, arg);
	default:
		break;
	}

	return 0;
}

static int af9035_frontend_callback(void *adapter_priv, int component,
				    int cmd, int arg)
{
	struct i2c_adapter *adap = adapter_priv;
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "component=%d cmd=%d arg=%d\n",
		component, cmd, arg);

	switch (component) {
	case DVB_FRONTEND_COMPONENT_TUNER:
		return af9035_tuner_callback(d, cmd, arg);
	default:
		break;
	}

	return 0;
}

static int af9035_get_adapter_count(struct dvb_usb_device *d)
{
	struct state *state = d_to_priv(d);

	return state->dual_mode + 1;
}

static int af9035_frontend_attach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;
	int ret;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	if (!state->af9033_config[adap->id].tuner) {
		/* unsupported tuner */
		ret = -ENODEV;
		goto err;
	}

	state->af9033_config[adap->id].fe = &adap->fe[0];
	state->af9033_config[adap->id].ops = &state->ops;
	ret = af9035_add_i2c_dev(d, "af9033", state->af9033_i2c_addr[adap->id],
			&state->af9033_config[adap->id], &d->i2c_adap);
	if (ret)
		goto err;

	if (adap->fe[0] == NULL) {
		ret = -ENODEV;
		goto err;
	}

	/* disable I2C-gate */
	adap->fe[0]->ops.i2c_gate_ctrl = NULL;
	adap->fe[0]->callback = af9035_frontend_callback;

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

/*
 * The I2C speed register is calculated with:
 *	I2C speed register = (1000000000 / (24.4 * 16 * I2C_speed))
 *
 * The default speed register for it930x is 7, with means a
 * speed of ~366 kbps
 */
#define I2C_SPEED_366K 7

static int it930x_frontend_attach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;
	int ret;
	struct si2168_config si2168_config;
	struct i2c_adapter *adapter;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	/* I2C master bus 2 clock speed 366k */
	ret = af9035_wr_reg(d, 0x00f6a7, I2C_SPEED_366K);
	if (ret < 0)
		goto err;

	/* I2C master bus 1,3 clock speed 366k */
	ret = af9035_wr_reg(d, 0x00f103, I2C_SPEED_366K);
	if (ret < 0)
		goto err;

	/* set gpio11 low */
	ret = af9035_wr_reg_mask(d, 0xd8d4, 0x01, 0x01);
	if (ret < 0)
		goto err;

	ret = af9035_wr_reg_mask(d, 0xd8d5, 0x01, 0x01);
	if (ret < 0)
		goto err;

	ret = af9035_wr_reg_mask(d, 0xd8d3, 0x01, 0x01);
	if (ret < 0)
		goto err;

	/* Tuner enable using gpiot2_en, gpiot2_on and gpiot2_o (reset) */
	ret = af9035_wr_reg_mask(d, 0xd8b8, 0x01, 0x01);
	if (ret < 0)
		goto err;

	ret = af9035_wr_reg_mask(d, 0xd8b9, 0x01, 0x01);
	if (ret < 0)
		goto err;

	ret = af9035_wr_reg_mask(d, 0xd8b7, 0x00, 0x01);
	if (ret < 0)
		goto err;

	msleep(200);

	ret = af9035_wr_reg_mask(d, 0xd8b7, 0x01, 0x01);
	if (ret < 0)
		goto err;

	memset(&si2168_config, 0, sizeof(si2168_config));
	si2168_config.i2c_adapter = &adapter;
	si2168_config.fe = &adap->fe[0];
	si2168_config.ts_mode = SI2168_TS_SERIAL;

	state->af9033_config[adap->id].fe = &adap->fe[0];
	state->af9033_config[adap->id].ops = &state->ops;
	ret = af9035_add_i2c_dev(d, "si2168",
				 it930x_addresses_table[state->it930x_addresses].frontend_i2c_addr,
				 &si2168_config, &d->i2c_adap);
	if (ret)
		goto err;

	if (adap->fe[0] == NULL) {
		ret = -ENODEV;
		goto err;
	}
	state->i2c_adapter_demod = adapter;

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_frontend_detach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	if (adap->id == 1) {
		if (state->i2c_client[1])
			af9035_del_i2c_dev(d);
	} else if (adap->id == 0) {
		if (state->i2c_client[0])
			af9035_del_i2c_dev(d);
	}

	return 0;
}

static const struct fc0011_config af9035_fc0011_config = {
	.i2c_address = 0x60,
};

static struct mxl5007t_config af9035_mxl5007t_config[] = {
	{
		.xtal_freq_hz = MxL_XTAL_24_MHZ,
		.if_freq_hz = MxL_IF_4_57_MHZ,
		.invert_if = 0,
		.loop_thru_enable = 0,
		.clk_out_enable = 0,
		.clk_out_amp = MxL_CLKOUT_AMP_0_94V,
	}, {
		.xtal_freq_hz = MxL_XTAL_24_MHZ,
		.if_freq_hz = MxL_IF_4_57_MHZ,
		.invert_if = 0,
		.loop_thru_enable = 1,
		.clk_out_enable = 1,
		.clk_out_amp = MxL_CLKOUT_AMP_0_94V,
	}
};

static struct tda18218_config af9035_tda18218_config = {
	.i2c_address = 0x60,
	.i2c_wr_max = 21,
};

static const struct fc0012_config af9035_fc0012_config[] = {
	{
		.i2c_address = 0x63,
		.xtal_freq = FC_XTAL_36_MHZ,
		.dual_master = true,
		.loop_through = true,
		.clock_out = true,
	}, {
		.i2c_address = 0x63 | 0x80, /* I2C bus select hack */
		.xtal_freq = FC_XTAL_36_MHZ,
		.dual_master = true,
	}
};

static int af9035_tuner_attach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;
	int ret;
	struct dvb_frontend *fe;
	struct i2c_msg msg[1];
	u8 tuner_addr;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	/*
	 * XXX: Hack used in that function: we abuse unused I2C address bit [7]
	 * to carry info about used I2C bus for dual tuner configuration.
	 */

	switch (state->af9033_config[adap->id].tuner) {
	case AF9033_TUNER_TUA9001: {
		struct tua9001_platform_data tua9001_pdata = {
			.dvb_frontend = adap->fe[0],
		};

		/*
		 * AF9035 gpiot3 = TUA9001 RESETN
		 * AF9035 gpiot2 = TUA9001 RXEN
		 */

		/* configure gpiot2 and gpiot2 as output */
		ret = af9035_wr_reg_mask(d, 0x00d8ec, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0x00d8ed, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0x00d8e8, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0x00d8e9, 0x01, 0x01);
		if (ret < 0)
			goto err;

		/* attach tuner */
		ret = af9035_add_i2c_dev(d, "tua9001", 0x60, &tua9001_pdata,
					 &d->i2c_adap);
		if (ret)
			goto err;

		fe = adap->fe[0];
		break;
	}
	case AF9033_TUNER_FC0011:
		fe = dvb_attach(fc0011_attach, adap->fe[0],
				&d->i2c_adap, &af9035_fc0011_config);
		break;
	case AF9033_TUNER_MXL5007T:
		if (adap->id == 0) {
			ret = af9035_wr_reg(d, 0x00d8e0, 1);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8e1, 1);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8df, 0);
			if (ret < 0)
				goto err;

			msleep(30);

			ret = af9035_wr_reg(d, 0x00d8df, 1);
			if (ret < 0)
				goto err;

			msleep(300);

			ret = af9035_wr_reg(d, 0x00d8c0, 1);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8c1, 1);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8bf, 0);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8b4, 1);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8b5, 1);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg(d, 0x00d8b3, 1);
			if (ret < 0)
				goto err;

			tuner_addr = 0x60;
		} else {
			tuner_addr = 0x60 | 0x80; /* I2C bus hack */
		}

		/* attach tuner */
		fe = dvb_attach(mxl5007t_attach, adap->fe[0], &d->i2c_adap,
				tuner_addr, &af9035_mxl5007t_config[adap->id]);
		break;
	case AF9033_TUNER_TDA18218:
		/* attach tuner */
		fe = dvb_attach(tda18218_attach, adap->fe[0],
				&d->i2c_adap, &af9035_tda18218_config);
		break;
	case AF9033_TUNER_FC2580: {
		struct fc2580_platform_data fc2580_pdata = {
			.dvb_frontend = adap->fe[0],
		};

		/* Tuner enable using gpiot2_o, gpiot2_en and gpiot2_on  */
		ret = af9035_wr_reg_mask(d, 0xd8eb, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0xd8ec, 0x01, 0x01);
		if (ret < 0)
			goto err;

		ret = af9035_wr_reg_mask(d, 0xd8ed, 0x01, 0x01);
		if (ret < 0)
			goto err;

		usleep_range(10000, 50000);
		/* attach tuner */
		ret = af9035_add_i2c_dev(d, "fc2580", 0x56, &fc2580_pdata,
					 &d->i2c_adap);
		if (ret)
			goto err;

		fe = adap->fe[0];
		break;
	}
	case AF9033_TUNER_FC0012:
		/*
		 * AF9035 gpiot2 = FC0012 enable
		 * XXX: there seems to be something on gpioh8 too, but on my
		 * my test I didn't find any difference.
		 */

		if (adap->id == 0) {
			/* configure gpiot2 as output and high */
			ret = af9035_wr_reg_mask(d, 0xd8eb, 0x01, 0x01);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg_mask(d, 0xd8ec, 0x01, 0x01);
			if (ret < 0)
				goto err;

			ret = af9035_wr_reg_mask(d, 0xd8ed, 0x01, 0x01);
			if (ret < 0)
				goto err;
		} else {
			/*
			 * FIXME: That belongs for the FC0012 driver.
			 * Write 02 to FC0012 master tuner register 0d directly
			 * in order to make slave tuner working.
			 */
			msg[0].addr = 0x63;
			msg[0].flags = 0;
			msg[0].len = 2;
			msg[0].buf = "\x0d\x02";
			ret = i2c_transfer(&d->i2c_adap, msg, 1);
			if (ret < 0)
				goto err;
		}

		usleep_range(10000, 50000);

		fe = dvb_attach(fc0012_attach, adap->fe[0], &d->i2c_adap,
				&af9035_fc0012_config[adap->id]);
		break;
	case AF9033_TUNER_IT9135_38:
	case AF9033_TUNER_IT9135_51:
	case AF9033_TUNER_IT9135_52:
	case AF9033_TUNER_IT9135_60:
	case AF9033_TUNER_IT9135_61:
	case AF9033_TUNER_IT9135_62:
	{
		struct platform_device *pdev;
		const char *name;
		struct it913x_platform_data it913x_pdata = {
			.regmap = state->af9033_config[adap->id].regmap,
			.fe = adap->fe[0],
		};

		switch (state->af9033_config[adap->id].tuner) {
		case AF9033_TUNER_IT9135_38:
		case AF9033_TUNER_IT9135_51:
		case AF9033_TUNER_IT9135_52:
			name = "it9133ax-tuner";
			break;
		case AF9033_TUNER_IT9135_60:
		case AF9033_TUNER_IT9135_61:
		case AF9033_TUNER_IT9135_62:
			name = "it9133bx-tuner";
			break;
		default:
			ret = -ENODEV;
			goto err;
		}

		if (state->dual_mode) {
			if (adap->id == 0)
				it913x_pdata.role = IT913X_ROLE_DUAL_MASTER;
			else
				it913x_pdata.role = IT913X_ROLE_DUAL_SLAVE;
		} else {
			it913x_pdata.role = IT913X_ROLE_SINGLE;
		}

		request_module("%s", "it913x");
		pdev = platform_device_register_data(&d->intf->dev, name,
						     PLATFORM_DEVID_AUTO,
						     &it913x_pdata,
						     sizeof(it913x_pdata));
		if (IS_ERR(pdev) || !pdev->dev.driver) {
			ret = -ENODEV;
			goto err;
		}
		if (!try_module_get(pdev->dev.driver->owner)) {
			platform_device_unregister(pdev);
			ret = -ENODEV;
			goto err;
		}

		state->platform_device_tuner[adap->id] = pdev;
		fe = adap->fe[0];
		break;
	}
	default:
		fe = NULL;
	}

	if (fe == NULL) {
		ret = -ENODEV;
		goto err;
	}

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int it930x_tuner_attach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;
	int ret;
	struct si2157_config si2157_config;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	memset(&si2157_config, 0, sizeof(si2157_config));
	si2157_config.fe = adap->fe[0];

	/*
	 * HACK: The Logilink VG0022A and TerraTec TC2 Stick have
	 * a bug: when the si2157 firmware that came with the device
	 * is replaced by a new one, the I2C transfers to the tuner
	 * will return just 0xff.
	 *
	 * Probably, the vendor firmware has some patch specifically
	 * designed for this device. So, we can't replace by the
	 * generic firmware. The right solution would be to extract
	 * the si2157 firmware from the original driver and ask the
	 * driver to load the specifically designed firmware, but,
	 * while we don't have that, the next best solution is to just
	 * keep the original firmware at the device.
	 */
	if ((le16_to_cpu(d->udev->descriptor.idVendor) == USB_VID_DEXATEK &&
	     le16_to_cpu(d->udev->descriptor.idProduct) == 0x0100) ||
	    (le16_to_cpu(d->udev->descriptor.idVendor) == USB_VID_TERRATEC &&
	     le16_to_cpu(d->udev->descriptor.idProduct) == USB_PID_TERRATEC_CINERGY_TC2_STICK))
		si2157_config.dont_load_firmware = true;

	si2157_config.if_port = it930x_addresses_table[state->it930x_addresses].tuner_if_port;
	ret = af9035_add_i2c_dev(d, "si2157",
				 it930x_addresses_table[state->it930x_addresses].tuner_i2c_addr,
				 &si2157_config, state->i2c_adapter_demod);
	if (ret)
		goto err;

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}


static int it930x_tuner_detach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	if (adap->id == 1) {
		if (state->i2c_client[3])
			af9035_del_i2c_dev(d);
	} else if (adap->id == 0) {
		if (state->i2c_client[1])
			af9035_del_i2c_dev(d);
	}

	return 0;
}


static int af9035_tuner_detach(struct dvb_usb_adapter *adap)
{
	struct state *state = adap_to_priv(adap);
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "adap->id=%d\n", adap->id);

	switch (state->af9033_config[adap->id].tuner) {
	case AF9033_TUNER_TUA9001:
	case AF9033_TUNER_FC2580:
		if (adap->id == 1) {
			if (state->i2c_client[3])
				af9035_del_i2c_dev(d);
		} else if (adap->id == 0) {
			if (state->i2c_client[1])
				af9035_del_i2c_dev(d);
		}
		break;
	case AF9033_TUNER_IT9135_38:
	case AF9033_TUNER_IT9135_51:
	case AF9033_TUNER_IT9135_52:
	case AF9033_TUNER_IT9135_60:
	case AF9033_TUNER_IT9135_61:
	case AF9033_TUNER_IT9135_62:
	{
		struct platform_device *pdev;

		pdev = state->platform_device_tuner[adap->id];
		if (pdev) {
			module_put(pdev->dev.driver->owner);
			platform_device_unregister(pdev);
		}
		break;
	}
	}

	return 0;
}

static int af9035_init(struct dvb_usb_device *d)
{
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret, i;
	u16 frame_size = (d->udev->speed == USB_SPEED_FULL ? 5 : 87) * 188 / 4;
	u8 packet_size = (d->udev->speed == USB_SPEED_FULL ? 64 : 512) / 4;
	struct reg_val_mask tab[] = {
		{ 0x80f99d, 0x01, 0x01 },
		{ 0x80f9a4, 0x01, 0x01 },
		{ 0x00dd11, 0x00, 0x20 },
		{ 0x00dd11, 0x00, 0x40 },
		{ 0x00dd13, 0x00, 0x20 },
		{ 0x00dd13, 0x00, 0x40 },
		{ 0x00dd11, 0x20, 0x20 },
		{ 0x00dd88, (frame_size >> 0) & 0xff, 0xff},
		{ 0x00dd89, (frame_size >> 8) & 0xff, 0xff},
		{ 0x00dd0c, packet_size, 0xff},
		{ 0x00dd11, state->dual_mode << 6, 0x40 },
		{ 0x00dd8a, (frame_size >> 0) & 0xff, 0xff},
		{ 0x00dd8b, (frame_size >> 8) & 0xff, 0xff},
		{ 0x00dd0d, packet_size, 0xff },
		{ 0x80f9a3, state->dual_mode, 0x01 },
		{ 0x80f9cd, state->dual_mode, 0x01 },
		{ 0x80f99d, 0x00, 0x01 },
		{ 0x80f9a4, 0x00, 0x01 },
	};

	dev_dbg(&intf->dev, "USB speed=%d frame_size=%04x packet_size=%02x\n",
		d->udev->speed, frame_size, packet_size);

	/* init endpoints */
	for (i = 0; i < ARRAY_SIZE(tab); i++) {
		ret = af9035_wr_reg_mask(d, tab[i].reg, tab[i].val,
				tab[i].mask);
		if (ret < 0)
			goto err;
	}

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int it930x_init(struct dvb_usb_device *d)
{
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret, i;
	u16 frame_size = (d->udev->speed == USB_SPEED_FULL ? 5 : 816) * 188 / 4;
	u8 packet_size = (d->udev->speed == USB_SPEED_FULL ? 64 : 512) / 4;
	struct reg_val_mask tab[] = {
		{ 0x00da1a, 0x00, 0x01 }, /* ignore_sync_byte */
		{ 0x00f41f, 0x04, 0x04 }, /* dvbt_inten */
		{ 0x00da10, 0x00, 0x01 }, /* mpeg_full_speed */
		{ 0x00f41a, 0x01, 0x01 }, /* dvbt_en */
		{ 0x00da1d, 0x01, 0x01 }, /* mp2_sw_rst, reset EP4 */
		{ 0x00dd11, 0x00, 0x20 }, /* ep4_tx_en, disable EP4 */
		{ 0x00dd13, 0x00, 0x20 }, /* ep4_tx_nak, disable EP4 NAK */
		{ 0x00dd11, 0x20, 0x20 }, /* ep4_tx_en, enable EP4 */
		{ 0x00dd11, 0x00, 0x40 }, /* ep5_tx_en, disable EP5 */
		{ 0x00dd13, 0x00, 0x40 }, /* ep5_tx_nak, disable EP5 NAK */
		{ 0x00dd11, state->dual_mode << 6, 0x40 }, /* enable EP5 */
		{ 0x00dd88, (frame_size >> 0) & 0xff, 0xff},
		{ 0x00dd89, (frame_size >> 8) & 0xff, 0xff},
		{ 0x00dd0c, packet_size, 0xff},
		{ 0x00dd8a, (frame_size >> 0) & 0xff, 0xff},
		{ 0x00dd8b, (frame_size >> 8) & 0xff, 0xff},
		{ 0x00dd0d, packet_size, 0xff },
		{ 0x00da1d, 0x00, 0x01 }, /* mp2_sw_rst, disable */
		{ 0x00d833, 0x01, 0xff }, /* slew rate ctrl: slew rate boosts */
		{ 0x00d830, 0x00, 0xff }, /* Bit 0 of output driving control */
		{ 0x00d831, 0x01, 0xff }, /* Bit 1 of output driving control */
		{ 0x00d832, 0x00, 0xff }, /* Bit 2 of output driving control */

		/* suspend gpio1 for TS-C */
		{ 0x00d8b0, 0x01, 0xff }, /* gpio1 */
		{ 0x00d8b1, 0x01, 0xff }, /* gpio1 */
		{ 0x00d8af, 0x00, 0xff }, /* gpio1 */

		/* suspend gpio7 for TS-D */
		{ 0x00d8c4, 0x01, 0xff }, /* gpio7 */
		{ 0x00d8c5, 0x01, 0xff }, /* gpio7 */
		{ 0x00d8c3, 0x00, 0xff }, /* gpio7 */

		/* suspend gpio13 for TS-B */
		{ 0x00d8dc, 0x01, 0xff }, /* gpio13 */
		{ 0x00d8dd, 0x01, 0xff }, /* gpio13 */
		{ 0x00d8db, 0x00, 0xff }, /* gpio13 */

		/* suspend gpio14 for TS-E */
		{ 0x00d8e4, 0x01, 0xff }, /* gpio14 */
		{ 0x00d8e5, 0x01, 0xff }, /* gpio14 */
		{ 0x00d8e3, 0x00, 0xff }, /* gpio14 */

		/* suspend gpio15 for TS-A */
		{ 0x00d8e8, 0x01, 0xff }, /* gpio15 */
		{ 0x00d8e9, 0x01, 0xff }, /* gpio15 */
		{ 0x00d8e7, 0x00, 0xff }, /* gpio15 */

		{ 0x00da58, 0x00, 0x01 }, /* ts_in_src, serial */
		{ 0x00da73, 0x01, 0xff }, /* ts0_aggre_mode */
		{ 0x00da78, 0x47, 0xff }, /* ts0_sync_byte */
		{ 0x00da4c, 0x01, 0xff }, /* ts0_en */
		{ 0x00da5a, 0x1f, 0xff }, /* ts_fail_ignore */
	};

	dev_dbg(&intf->dev, "USB speed=%d frame_size=%04x packet_size=%02x\n",
		d->udev->speed, frame_size, packet_size);

	/* init endpoints */
	for (i = 0; i < ARRAY_SIZE(tab); i++) {
		ret = af9035_wr_reg_mask(d, tab[i].reg,
				tab[i].val, tab[i].mask);

		if (ret < 0)
			goto err;
	}

	return 0;
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}


#if IS_ENABLED(CONFIG_RC_CORE)
static int af9035_rc_query(struct dvb_usb_device *d)
{
	struct usb_interface *intf = d->intf;
	int ret;
	enum rc_proto proto;
	u32 key;
	u8 buf[4];
	struct usb_req req = { CMD_IR_GET, 0, 0, NULL, 4, buf };

	ret = af9035_ctrl_msg(d, &req);
	if (ret == 1)
		return 0;
	else if (ret < 0)
		goto err;

	if ((buf[2] + buf[3]) == 0xff) {
		if ((buf[0] + buf[1]) == 0xff) {
			/* NEC standard 16bit */
			key = RC_SCANCODE_NEC(buf[0], buf[2]);
			proto = RC_PROTO_NEC;
		} else {
			/* NEC extended 24bit */
			key = RC_SCANCODE_NECX(buf[0] << 8 | buf[1], buf[2]);
			proto = RC_PROTO_NECX;
		}
	} else {
		/* NEC full code 32bit */
		key = RC_SCANCODE_NEC32(buf[0] << 24 | buf[1] << 16 |
					buf[2] << 8  | buf[3]);
		proto = RC_PROTO_NEC32;
	}

	dev_dbg(&intf->dev, "%*ph\n", 4, buf);

	rc_keydown(d->rc_dev, proto, key, 0);

	return 0;

err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);

	return ret;
}

static int af9035_get_rc_config(struct dvb_usb_device *d, struct dvb_usb_rc *rc)
{
	struct state *state = d_to_priv(d);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "ir_mode=%02x ir_type=%02x\n",
		state->ir_mode, state->ir_type);

	/* don't activate rc if in HID mode or if not available */
	if (state->ir_mode == 0x05) {
		switch (state->ir_type) {
		case 0: /* NEC */
		default:
			rc->allowed_protos = RC_PROTO_BIT_NEC |
					RC_PROTO_BIT_NECX | RC_PROTO_BIT_NEC32;
			break;
		case 1: /* RC6 */
			rc->allowed_protos = RC_PROTO_BIT_RC6_MCE;
			break;
		}

		rc->query = af9035_rc_query;
		rc->interval = 500;

		/* load empty to enable rc */
		if (!rc->map_name)
			rc->map_name = RC_MAP_EMPTY;
	}

	return 0;
}
#else
	#define af9035_get_rc_config NULL
#endif

static int af9035_get_stream_config(struct dvb_frontend *fe, u8 *ts_type,
		struct usb_data_stream_properties *stream)
{
	struct dvb_usb_device *d = fe_to_d(fe);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "adap=%d\n", fe_to_adap(fe)->id);

	if (d->udev->speed == USB_SPEED_FULL)
		stream->u.bulk.buffersize = 5 * 188;

	return 0;
}

static int af9035_pid_filter_ctrl(struct dvb_usb_adapter *adap, int onoff)
{
	struct state *state = adap_to_priv(adap);

	return state->ops.pid_filter_ctrl(adap->fe[0], onoff);
}

static int af9035_pid_filter(struct dvb_usb_adapter *adap, int index, u16 pid,
		int onoff)
{
	struct state *state = adap_to_priv(adap);

	return state->ops.pid_filter(adap->fe[0], index, pid, onoff);
}

static int af9035_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	char manufacturer[sizeof("Afatech")];

	memset(manufacturer, 0, sizeof(manufacturer));
	usb_string(udev, udev->descriptor.iManufacturer,
			manufacturer, sizeof(manufacturer));
	/*
	 * There is two devices having same ID but different chipset. One uses
	 * AF9015 and the other IT9135 chipset. Only difference seen on lsusb
	 * is iManufacturer string.
	 *
	 * idVendor           0x0ccd TerraTec Electronic GmbH
	 * idProduct          0x0099
	 * bcdDevice            2.00
	 * iManufacturer           1 Afatech
	 * iProduct                2 DVB-T 2
	 *
	 * idVendor           0x0ccd TerraTec Electronic GmbH
	 * idProduct          0x0099
	 * bcdDevice            2.00
	 * iManufacturer           1 ITE Technologies, Inc.
	 * iProduct                2 DVB-T TV Stick
	 */
	if ((le16_to_cpu(udev->descriptor.idVendor) == USB_VID_TERRATEC) &&
			(le16_to_cpu(udev->descriptor.idProduct) == 0x0099)) {
		if (!strcmp("Afatech", manufacturer)) {
			dev_dbg(&udev->dev, "rejecting device\n");
			return -ENODEV;
		}
	}

	return dvb_usbv2_probe(intf, id);
}

/* interface 0 is used by DVB-T receiver and
   interface 1 is for remote controller (HID) */
static const struct dvb_usb_device_properties af9035_props = {
	.driver_name = KBUILD_MODNAME,
	.owner = THIS_MODULE,
	.adapter_nr = adapter_nr,
	.size_of_priv = sizeof(struct state),

	.generic_bulk_ctrl_endpoint = 0x02,
	.generic_bulk_ctrl_endpoint_response = 0x81,

	.identify_state = af9035_identify_state,
	.download_firmware = af9035_download_firmware,

	.i2c_algo = &af9035_i2c_algo,
	.read_config = af9035_read_config,
	.frontend_attach = af9035_frontend_attach,
	.frontend_detach = af9035_frontend_detach,
	.tuner_attach = af9035_tuner_attach,
	.tuner_detach = af9035_tuner_detach,
	.init = af9035_init,
	.get_rc_config = af9035_get_rc_config,
	.get_stream_config = af9035_get_stream_config,

	.get_adapter_count = af9035_get_adapter_count,
	.adapter = {
		{
			.caps = DVB_USB_ADAP_HAS_PID_FILTER |
				DVB_USB_ADAP_PID_FILTER_CAN_BE_TURNED_OFF,

			.pid_filter_count = 32,
			.pid_filter_ctrl = af9035_pid_filter_ctrl,
			.pid_filter = af9035_pid_filter,

			.stream = DVB_USB_STREAM_BULK(0x84, 6, 87 * 188),
		}, {
			.caps = DVB_USB_ADAP_HAS_PID_FILTER |
				DVB_USB_ADAP_PID_FILTER_CAN_BE_TURNED_OFF,

			.pid_filter_count = 32,
			.pid_filter_ctrl = af9035_pid_filter_ctrl,
			.pid_filter = af9035_pid_filter,

			.stream = DVB_USB_STREAM_BULK(0x85, 6, 87 * 188),
		},
	},
};

static const struct dvb_usb_device_properties it930x_props = {
	.driver_name = KBUILD_MODNAME,
	.owner = THIS_MODULE,
	.adapter_nr = adapter_nr,
	.size_of_priv = sizeof(struct state),

	.generic_bulk_ctrl_endpoint = 0x02,
	.generic_bulk_ctrl_endpoint_response = 0x81,

	.identify_state = af9035_identify_state,
	.download_firmware = af9035_download_firmware,

	.i2c_algo = &af9035_i2c_algo,
	.read_config = af9035_read_config,
	.frontend_attach = it930x_frontend_attach,
	.frontend_detach = af9035_frontend_detach,
	.tuner_attach = it930x_tuner_attach,
	.tuner_detach = it930x_tuner_detach,
	.init = it930x_init,
	.get_stream_config = af9035_get_stream_config,

	.get_adapter_count = af9035_get_adapter_count,
	.adapter = {
		{
			.stream = DVB_USB_STREAM_BULK(0x84, 4, 816 * 188),
		}, {
			.stream = DVB_USB_STREAM_BULK(0x85, 4, 816 * 188),
		},
	},
};

static const struct usb_device_id af9035_id_table[] = {
	/* AF9035 devices */
	{ DVB_USB_DEVICE(USB_VID_AFATECH, USB_PID_AFATECH_AF9035_9035,
		&af9035_props, "Afatech AF9035 reference design", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AFATECH, USB_PID_AFATECH_AF9035_1000,
		&af9035_props, "Afatech AF9035 reference design", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AFATECH, USB_PID_AFATECH_AF9035_1001,
		&af9035_props, "Afatech AF9035 reference design", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AFATECH, USB_PID_AFATECH_AF9035_1002,
		&af9035_props, "Afatech AF9035 reference design", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AFATECH, USB_PID_AFATECH_AF9035_1003,
		&af9035_props, "Afatech AF9035 reference design", NULL) },
	{ DVB_USB_DEVICE(USB_VID_TERRATEC, USB_PID_TERRATEC_CINERGY_T_STICK,
		&af9035_props, "TerraTec Cinergy T Stick", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A835,
		&af9035_props, "AVerMedia AVerTV Volar HD/PRO (A835)", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_B835,
		&af9035_props, "AVerMedia AVerTV Volar HD/PRO (A835)", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_1867,
		&af9035_props, "AVerMedia HD Volar (A867)", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A867,
		&af9035_props, "AVerMedia HD Volar (A867)", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_TWINSTAR,
		&af9035_props, "AVerMedia Twinstar (A825)", NULL) },
	{ DVB_USB_DEVICE(USB_VID_ASUS, USB_PID_ASUS_U3100MINI_PLUS,
		&af9035_props, "Asus U3100Mini Plus", NULL) },
	{ DVB_USB_DEVICE(USB_VID_TERRATEC, 0x00aa,
		&af9035_props, "TerraTec Cinergy T Stick (rev. 2)", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, 0x0337,
		&af9035_props, "AVerMedia HD Volar (A867)", NULL) },
       { DVB_USB_DEVICE(USB_VID_GTEK, USB_PID_EVOLVEO_XTRATV_STICK,
	       &af9035_props, "EVOLVEO XtraTV stick", NULL) },

	/* IT9135 devices */
	{ DVB_USB_DEVICE(USB_VID_ITETECH, USB_PID_ITETECH_IT9135,
		&af9035_props, "ITE 9135 Generic", RC_MAP_IT913X_V1) },
	{ DVB_USB_DEVICE(USB_VID_ITETECH, USB_PID_ITETECH_IT9135_9005,
		&af9035_props, "ITE 9135(9005) Generic", RC_MAP_IT913X_V2) },
	{ DVB_USB_DEVICE(USB_VID_ITETECH, USB_PID_ITETECH_IT9135_9006,
		&af9035_props, "ITE 9135(9006) Generic", RC_MAP_IT913X_V1) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A835B_1835,
		&af9035_props, "Avermedia A835B(1835)", RC_MAP_IT913X_V2) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A835B_2835,
		&af9035_props, "Avermedia A835B(2835)", RC_MAP_IT913X_V2) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A835B_3835,
		&af9035_props, "Avermedia A835B(3835)", RC_MAP_IT913X_V2) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A835B_4835,
		&af9035_props, "Avermedia A835B(4835)",	RC_MAP_IT913X_V2) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_TD110,
		&af9035_props, "Avermedia AverTV Volar HD 2 (TD110)", RC_MAP_AVERMEDIA_RM_KS) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_H335,
		&af9035_props, "Avermedia H335", RC_MAP_IT913X_V2) },
	{ DVB_USB_DEVICE(USB_VID_KWORLD_2, USB_PID_KWORLD_UB499_2T_T09,
		&af9035_props, "Kworld UB499-2T T09", RC_MAP_IT913X_V1) },
	{ DVB_USB_DEVICE(USB_VID_KWORLD_2, USB_PID_SVEON_STV22_IT9137,
		&af9035_props, "Sveon STV22 Dual DVB-T HDTV",
							RC_MAP_IT913X_V1) },
	{ DVB_USB_DEVICE(USB_VID_KWORLD_2, USB_PID_CTVDIGDUAL_V2,
		&af9035_props, "Digital Dual TV Receiver CTVDIGDUAL_V2",
							RC_MAP_IT913X_V1) },
	{ DVB_USB_DEVICE(USB_VID_TERRATEC, USB_PID_TERRATEC_T1,
		&af9035_props, "TerraTec T1", RC_MAP_IT913X_V1) },
	/* XXX: that same ID [0ccd:0099] is used by af9015 driver too */
	{ DVB_USB_DEVICE(USB_VID_TERRATEC, 0x0099,
		&af9035_props, "TerraTec Cinergy T Stick Dual RC (rev. 2)",
		NULL) },
	{ DVB_USB_DEVICE(USB_VID_LEADTEK, 0x6a05,
		&af9035_props, "Leadtek WinFast DTV Dongle Dual", NULL) },
	{ DVB_USB_DEVICE(USB_VID_HAUPPAUGE, 0xf900,
		&af9035_props, "Hauppauge WinTV-MiniStick 2", NULL) },
	{ DVB_USB_DEVICE(USB_VID_PCTV, USB_PID_PCTV_78E,
		&af9035_props, "PCTV AndroiDTV (78e)", RC_MAP_IT913X_V1) },
	{ DVB_USB_DEVICE(USB_VID_PCTV, USB_PID_PCTV_79E,
		&af9035_props, "PCTV microStick (79e)", RC_MAP_IT913X_V2) },

	/* IT930x devices */
	{ DVB_USB_DEVICE(USB_VID_ITETECH, USB_PID_ITETECH_IT9303,
		&it930x_props, "ITE 9303 Generic", NULL) },
	{ DVB_USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_TD310,
		&it930x_props, "AVerMedia TD310 DVB-T2", NULL) },
	{ DVB_USB_DEVICE(USB_VID_DEXATEK, 0x0100,
		&it930x_props, "Logilink VG0022A", NULL) },
	{ DVB_USB_DEVICE(USB_VID_TERRATEC, USB_PID_TERRATEC_CINERGY_TC2_STICK,
		&it930x_props, "TerraTec Cinergy TC2 Stick", NULL) },
	{ }
};
MODULE_DEVICE_TABLE(usb, af9035_id_table);

static struct usb_driver af9035_usb_driver = {
	.name = KBUILD_MODNAME,
	.id_table = af9035_id_table,
	.probe = af9035_probe,
	.disconnect = dvb_usbv2_disconnect,
	.suspend = dvb_usbv2_suspend,
	.resume = dvb_usbv2_resume,
	.reset_resume = dvb_usbv2_reset_resume,
	.no_dynamic_id = 1,
	.soft_unbind = 1,
};

module_usb_driver(af9035_usb_driver);

MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
MODULE_DESCRIPTION("Afatech AF9035 driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(AF9035_FIRMWARE_AF9035);
MODULE_FIRMWARE(AF9035_FIRMWARE_IT9135_V1);
MODULE_FIRMWARE(AF9035_FIRMWARE_IT9135_V2);
MODULE_FIRMWARE(AF9035_FIRMWARE_IT9303);

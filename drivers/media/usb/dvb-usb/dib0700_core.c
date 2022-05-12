// SPDX-License-Identifier: GPL-2.0-only
/* Linux driver for devices based on the DiBcom DiB0700 USB bridge
 *
 *  Copyright (C) 2005-6 DiBcom, SA
 */
#include "dib0700.h"

/* debug */
int dvb_usb_dib0700_debug;
module_param_named(debug,dvb_usb_dib0700_debug, int, 0644);
MODULE_PARM_DESC(debug, "set debugging level (1=info,2=fw,4=fwdata,8=data (or-able))." DVB_USB_DEBUG_STATUS);

static int nb_packet_buffer_size = 21;
module_param(nb_packet_buffer_size, int, 0644);
MODULE_PARM_DESC(nb_packet_buffer_size,
	"Set the dib0700 driver data buffer size. This parameter corresponds to the number of TS packets. The actual size of the data buffer corresponds to this parameter multiplied by 188 (default: 21)");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);


int dib0700_get_version(struct dvb_usb_device *d, u32 *hwversion,
			u32 *romversion, u32 *ramversion, u32 *fwtype)
{
	struct dib0700_state *st = d->priv;
	int ret;

	if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
		err("could not acquire lock");
		return -EINTR;
	}

	ret = usb_control_msg(d->udev, usb_rcvctrlpipe(d->udev, 0),
				  REQUEST_GET_VERSION,
				  USB_TYPE_VENDOR | USB_DIR_IN, 0, 0,
				  st->buf, 16, USB_CTRL_GET_TIMEOUT);
	if (hwversion != NULL)
		*hwversion  = (st->buf[0] << 24)  | (st->buf[1] << 16)  |
			(st->buf[2] << 8)  | st->buf[3];
	if (romversion != NULL)
		*romversion = (st->buf[4] << 24)  | (st->buf[5] << 16)  |
			(st->buf[6] << 8)  | st->buf[7];
	if (ramversion != NULL)
		*ramversion = (st->buf[8] << 24)  | (st->buf[9] << 16)  |
			(st->buf[10] << 8) | st->buf[11];
	if (fwtype != NULL)
		*fwtype     = (st->buf[12] << 24) | (st->buf[13] << 16) |
			(st->buf[14] << 8) | st->buf[15];
	mutex_unlock(&d->usb_mutex);
	return ret;
}

/* expecting rx buffer: request data[0] data[1] ... data[2] */
static int dib0700_ctrl_wr(struct dvb_usb_device *d, u8 *tx, u8 txlen)
{
	int status;

	deb_data(">>> ");
	debug_dump(tx, txlen, deb_data);

	status = usb_control_msg(d->udev, usb_sndctrlpipe(d->udev,0),
		tx[0], USB_TYPE_VENDOR | USB_DIR_OUT, 0, 0, tx, txlen,
		USB_CTRL_GET_TIMEOUT);

	if (status != txlen)
		deb_data("ep 0 write error (status = %d, len: %d)\n",status,txlen);

	return status < 0 ? status : 0;
}

/* expecting tx buffer: request data[0] ... data[n] (n <= 4) */
int dib0700_ctrl_rd(struct dvb_usb_device *d, u8 *tx, u8 txlen, u8 *rx, u8 rxlen)
{
	u16 index, value;
	int status;

	if (txlen < 2) {
		err("tx buffer length is smaller than 2. Makes no sense.");
		return -EINVAL;
	}
	if (txlen > 4) {
		err("tx buffer length is larger than 4. Not supported.");
		return -EINVAL;
	}

	deb_data(">>> ");
	debug_dump(tx,txlen,deb_data);

	value = ((txlen - 2) << 8) | tx[1];
	index = 0;
	if (txlen > 2)
		index |= (tx[2] << 8);
	if (txlen > 3)
		index |= tx[3];

	status = usb_control_msg(d->udev, usb_rcvctrlpipe(d->udev,0), tx[0],
			USB_TYPE_VENDOR | USB_DIR_IN, value, index, rx, rxlen,
			USB_CTRL_GET_TIMEOUT);

	if (status < 0)
		deb_info("ep 0 read error (status = %d)\n",status);

	deb_data("<<< ");
	debug_dump(rx, rxlen, deb_data);

	return status; /* length in case of success */
}

int dib0700_set_gpio(struct dvb_usb_device *d, enum dib07x0_gpios gpio, u8 gpio_dir, u8 gpio_val)
{
	struct dib0700_state *st = d->priv;
	int ret;

	if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
		err("could not acquire lock");
		return -EINTR;
	}

	st->buf[0] = REQUEST_SET_GPIO;
	st->buf[1] = gpio;
	st->buf[2] = ((gpio_dir & 0x01) << 7) | ((gpio_val & 0x01) << 6);

	ret = dib0700_ctrl_wr(d, st->buf, 3);

	mutex_unlock(&d->usb_mutex);
	return ret;
}

static int dib0700_set_usb_xfer_len(struct dvb_usb_device *d, u16 nb_ts_packets)
{
	struct dib0700_state *st = d->priv;
	int ret;

	if (st->fw_version >= 0x10201) {
		if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
			err("could not acquire lock");
			return -EINTR;
		}

		st->buf[0] = REQUEST_SET_USB_XFER_LEN;
		st->buf[1] = (nb_ts_packets >> 8) & 0xff;
		st->buf[2] = nb_ts_packets & 0xff;

		deb_info("set the USB xfer len to %i Ts packet\n", nb_ts_packets);

		ret = dib0700_ctrl_wr(d, st->buf, 3);
		mutex_unlock(&d->usb_mutex);
	} else {
		deb_info("this firmware does not allow to change the USB xfer len\n");
		ret = -EIO;
	}

	return ret;
}

/*
 * I2C master xfer function (supported in 1.20 firmware)
 */
static int dib0700_i2c_xfer_new(struct i2c_adapter *adap, struct i2c_msg *msg,
				int num)
{
	/* The new i2c firmware messages are more reliable and in particular
	   properly support i2c read calls not preceded by a write */

	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	struct dib0700_state *st = d->priv;
	uint8_t bus_mode = 1;  /* 0=eeprom bus, 1=frontend bus */
	uint8_t gen_mode = 0; /* 0=master i2c, 1=gpio i2c */
	uint8_t en_start = 0;
	uint8_t en_stop = 0;
	int result, i;

	/* Ensure nobody else hits the i2c bus while we're sending our
	   sequence of messages, (such as the remote control thread) */
	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EINTR;

	for (i = 0; i < num; i++) {
		if (i == 0) {
			/* First message in the transaction */
			en_start = 1;
		} else if (!(msg[i].flags & I2C_M_NOSTART)) {
			/* Device supports repeated-start */
			en_start = 1;
		} else {
			/* Not the first packet and device doesn't support
			   repeated start */
			en_start = 0;
		}
		if (i == (num - 1)) {
			/* Last message in the transaction */
			en_stop = 1;
		}

		if (msg[i].flags & I2C_M_RD) {
			/* Read request */
			u16 index, value;
			uint8_t i2c_dest;

			i2c_dest = (msg[i].addr << 1);
			value = ((en_start << 7) | (en_stop << 6) |
				 (msg[i].len & 0x3F)) << 8 | i2c_dest;
			/* I2C ctrl + FE bus; */
			index = ((gen_mode << 6) & 0xC0) |
				((bus_mode << 4) & 0x30);

			result = usb_control_msg(d->udev,
						 usb_rcvctrlpipe(d->udev, 0),
						 REQUEST_NEW_I2C_READ,
						 USB_TYPE_VENDOR | USB_DIR_IN,
						 value, index, st->buf,
						 msg[i].len,
						 USB_CTRL_GET_TIMEOUT);
			if (result < 0) {
				deb_info("i2c read error (status = %d)\n", result);
				goto unlock;
			}

			if (msg[i].len > sizeof(st->buf)) {
				deb_info("buffer too small to fit %d bytes\n",
					 msg[i].len);
				result = -EIO;
				goto unlock;
			}

			memcpy(msg[i].buf, st->buf, msg[i].len);

			deb_data("<<< ");
			debug_dump(msg[i].buf, msg[i].len, deb_data);

		} else {
			/* Write request */
			if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
				err("could not acquire lock");
				result = -EINTR;
				goto unlock;
			}
			st->buf[0] = REQUEST_NEW_I2C_WRITE;
			st->buf[1] = msg[i].addr << 1;
			st->buf[2] = (en_start << 7) | (en_stop << 6) |
				(msg[i].len & 0x3F);
			/* I2C ctrl + FE bus; */
			st->buf[3] = ((gen_mode << 6) & 0xC0) |
				 ((bus_mode << 4) & 0x30);

			if (msg[i].len > sizeof(st->buf) - 4) {
				deb_info("i2c message to big: %d\n",
					 msg[i].len);
				mutex_unlock(&d->usb_mutex);
				result = -EIO;
				goto unlock;
			}

			/* The Actual i2c payload */
			memcpy(&st->buf[4], msg[i].buf, msg[i].len);

			deb_data(">>> ");
			debug_dump(st->buf, msg[i].len + 4, deb_data);

			result = usb_control_msg(d->udev,
						 usb_sndctrlpipe(d->udev, 0),
						 REQUEST_NEW_I2C_WRITE,
						 USB_TYPE_VENDOR | USB_DIR_OUT,
						 0, 0, st->buf, msg[i].len + 4,
						 USB_CTRL_GET_TIMEOUT);
			mutex_unlock(&d->usb_mutex);
			if (result < 0) {
				deb_info("i2c write error (status = %d)\n", result);
				break;
			}
		}
	}
	result = i;

unlock:
	mutex_unlock(&d->i2c_mutex);
	return result;
}

/*
 * I2C master xfer function (pre-1.20 firmware)
 */
static int dib0700_i2c_xfer_legacy(struct i2c_adapter *adap,
				   struct i2c_msg *msg, int num)
{
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	struct dib0700_state *st = d->priv;
	int i, len, result;

	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EINTR;
	if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
		err("could not acquire lock");
		mutex_unlock(&d->i2c_mutex);
		return -EINTR;
	}

	for (i = 0; i < num; i++) {
		/* fill in the address */
		st->buf[1] = msg[i].addr << 1;
		/* fill the buffer */
		if (msg[i].len > sizeof(st->buf) - 2) {
			deb_info("i2c xfer to big: %d\n",
				msg[i].len);
			result = -EIO;
			goto unlock;
		}
		memcpy(&st->buf[2], msg[i].buf, msg[i].len);

		/* write/read request */
		if (i+1 < num && (msg[i+1].flags & I2C_M_RD)) {
			st->buf[0] = REQUEST_I2C_READ;
			st->buf[1] |= 1;

			/* special thing in the current firmware: when length is zero the read-failed */
			len = dib0700_ctrl_rd(d, st->buf, msg[i].len + 2,
					      st->buf, msg[i + 1].len);
			if (len <= 0) {
				deb_info("I2C read failed on address 0x%02x\n",
						msg[i].addr);
				result = -EIO;
				goto unlock;
			}

			if (msg[i + 1].len > sizeof(st->buf)) {
				deb_info("i2c xfer buffer to small for %d\n",
					msg[i].len);
				result = -EIO;
				goto unlock;
			}
			memcpy(msg[i + 1].buf, st->buf, msg[i + 1].len);

			msg[i+1].len = len;

			i++;
		} else {
			st->buf[0] = REQUEST_I2C_WRITE;
			result = dib0700_ctrl_wr(d, st->buf, msg[i].len + 2);
			if (result < 0)
				goto unlock;
		}
	}
	result = i;
unlock:
	mutex_unlock(&d->usb_mutex);
	mutex_unlock(&d->i2c_mutex);

	return result;
}

static int dib0700_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msg,
			    int num)
{
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	struct dib0700_state *st = d->priv;

	if (st->fw_use_new_i2c_api == 1) {
		/* User running at least fw 1.20 */
		return dib0700_i2c_xfer_new(adap, msg, num);
	} else {
		/* Use legacy calls */
		return dib0700_i2c_xfer_legacy(adap, msg, num);
	}
}

static u32 dib0700_i2c_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C;
}

struct i2c_algorithm dib0700_i2c_algo = {
	.master_xfer   = dib0700_i2c_xfer,
	.functionality = dib0700_i2c_func,
};

int dib0700_identify_state(struct usb_device *udev,
			   const struct dvb_usb_device_properties *props,
			   const struct dvb_usb_device_description **desc,
			   int *cold)
{
	s16 ret;
	u8 *b;

	b = kmalloc(16, GFP_KERNEL);
	if (!b)
		return	-ENOMEM;


	ret = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
		REQUEST_GET_VERSION, USB_TYPE_VENDOR | USB_DIR_IN, 0, 0, b, 16, USB_CTRL_GET_TIMEOUT);

	deb_info("FW GET_VERSION length: %d\n",ret);

	*cold = ret <= 0;
	deb_info("cold: %d\n", *cold);

	kfree(b);
	return 0;
}

static int dib0700_set_clock(struct dvb_usb_device *d, u8 en_pll,
	u8 pll_src, u8 pll_range, u8 clock_gpio3, u16 pll_prediv,
	u16 pll_loopdiv, u16 free_div, u16 dsuScaler)
{
	struct dib0700_state *st = d->priv;
	int ret;

	if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
		err("could not acquire lock");
		return -EINTR;
	}

	st->buf[0] = REQUEST_SET_CLOCK;
	st->buf[1] = (en_pll << 7) | (pll_src << 6) |
		(pll_range << 5) | (clock_gpio3 << 4);
	st->buf[2] = (pll_prediv >> 8)  & 0xff; /* MSB */
	st->buf[3] =  pll_prediv        & 0xff; /* LSB */
	st->buf[4] = (pll_loopdiv >> 8) & 0xff; /* MSB */
	st->buf[5] =  pll_loopdiv       & 0xff; /* LSB */
	st->buf[6] = (free_div >> 8)    & 0xff; /* MSB */
	st->buf[7] =  free_div          & 0xff; /* LSB */
	st->buf[8] = (dsuScaler >> 8)   & 0xff; /* MSB */
	st->buf[9] =  dsuScaler         & 0xff; /* LSB */

	ret = dib0700_ctrl_wr(d, st->buf, 10);
	mutex_unlock(&d->usb_mutex);

	return ret;
}

int dib0700_set_i2c_speed(struct dvb_usb_device *d, u16 scl_kHz)
{
	struct dib0700_state *st = d->priv;
	u16 divider;
	int ret;

	if (scl_kHz == 0)
		return -EINVAL;

	if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
		err("could not acquire lock");
		return -EINTR;
	}

	st->buf[0] = REQUEST_SET_I2C_PARAM;
	divider = (u16) (30000 / scl_kHz);
	st->buf[1] = 0;
	st->buf[2] = (u8) (divider >> 8);
	st->buf[3] = (u8) (divider & 0xff);
	divider = (u16) (72000 / scl_kHz);
	st->buf[4] = (u8) (divider >> 8);
	st->buf[5] = (u8) (divider & 0xff);
	divider = (u16) (72000 / scl_kHz); /* clock: 72MHz */
	st->buf[6] = (u8) (divider >> 8);
	st->buf[7] = (u8) (divider & 0xff);

	deb_info("setting I2C speed: %04x %04x %04x (%d kHz).",
		(st->buf[2] << 8) | (st->buf[3]), (st->buf[4] << 8) |
		st->buf[5], (st->buf[6] << 8) | st->buf[7], scl_kHz);

	ret = dib0700_ctrl_wr(d, st->buf, 8);
	mutex_unlock(&d->usb_mutex);

	return ret;
}


int dib0700_ctrl_clock(struct dvb_usb_device *d, u32 clk_MHz, u8 clock_out_gp3)
{
	switch (clk_MHz) {
		case 72: dib0700_set_clock(d, 1, 0, 1, clock_out_gp3, 2, 24, 0, 0x4c); break;
		default: return -EINVAL;
	}
	return 0;
}

static int dib0700_jumpram(struct usb_device *udev, u32 address)
{
	int ret = 0, actlen;
	u8 *buf;

	buf = kmalloc(8, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	buf[0] = REQUEST_JUMPRAM;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = (address >> 24) & 0xff;
	buf[5] = (address >> 16) & 0xff;
	buf[6] = (address >> 8)  & 0xff;
	buf[7] =  address        & 0xff;

	if ((ret = usb_bulk_msg(udev, usb_sndbulkpipe(udev, 0x01),buf,8,&actlen,1000)) < 0) {
		deb_fw("jumpram to 0x%x failed\n",address);
		goto out;
	}
	if (actlen != 8) {
		deb_fw("jumpram to 0x%x failed\n",address);
		ret = -EIO;
		goto out;
	}
out:
	kfree(buf);
	return ret;
}

int dib0700_download_firmware(struct usb_device *udev, const struct firmware *fw)
{
	struct hexline hx;
	int pos = 0, ret, act_len, i, adap_num;
	u8 *buf;
	u32 fw_version;

	buf = kmalloc(260, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	while ((ret = dvb_usb_get_hexline(fw, &hx, &pos)) > 0) {
		deb_fwdata("writing to address 0x%08x (buffer: 0x%02x %02x)\n",
				hx.addr, hx.len, hx.chk);

		buf[0] = hx.len;
		buf[1] = (hx.addr >> 8) & 0xff;
		buf[2] =  hx.addr       & 0xff;
		buf[3] = hx.type;
		memcpy(&buf[4],hx.data,hx.len);
		buf[4+hx.len] = hx.chk;

		ret = usb_bulk_msg(udev,
			usb_sndbulkpipe(udev, 0x01),
			buf,
			hx.len + 5,
			&act_len,
			1000);

		if (ret < 0) {
			err("firmware download failed at %d with %d",pos,ret);
			goto out;
		}
	}

	if (ret == 0) {
		/* start the firmware */
		if ((ret = dib0700_jumpram(udev, 0x70000000)) == 0) {
			info("firmware started successfully.");
			msleep(500);
		}
	} else
		ret = -EIO;

	/* the number of ts packet has to be at least 1 */
	if (nb_packet_buffer_size < 1)
		nb_packet_buffer_size = 1;

	/* get the firmware version */
	usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
				  REQUEST_GET_VERSION,
				  USB_TYPE_VENDOR | USB_DIR_IN, 0, 0,
				  buf, 16, USB_CTRL_GET_TIMEOUT);
	fw_version = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];

	/* set the buffer size - DVB-USB is allocating URB buffers
	 * only after the firwmare download was successful */
	for (i = 0; i < dib0700_device_count; i++) {
		for (adap_num = 0; adap_num < dib0700_devices[i].num_adapters;
				adap_num++) {
			if (fw_version >= 0x10201) {
				dib0700_devices[i].adapter[adap_num].fe[0].stream.u.bulk.buffersize = 188*nb_packet_buffer_size;
			} else {
				/* for fw version older than 1.20.1,
				 * the buffersize has to be n times 512 */
				dib0700_devices[i].adapter[adap_num].fe[0].stream.u.bulk.buffersize = ((188*nb_packet_buffer_size+188/2)/512)*512;
				if (dib0700_devices[i].adapter[adap_num].fe[0].stream.u.bulk.buffersize < 512)
					dib0700_devices[i].adapter[adap_num].fe[0].stream.u.bulk.buffersize = 512;
			}
		}
	}
out:
	kfree(buf);
	return ret;
}

int dib0700_streaming_ctrl(struct dvb_usb_adapter *adap, int onoff)
{
	struct dib0700_state *st = adap->dev->priv;
	int ret;

	if ((onoff != 0) && (st->fw_version >= 0x10201)) {
		/* for firmware later than 1.20.1,
		 * the USB xfer length can be set  */
		ret = dib0700_set_usb_xfer_len(adap->dev,
			st->nb_packet_buffer_size);
		if (ret < 0) {
			deb_info("can not set the USB xfer len\n");
			return ret;
		}
	}

	mutex_lock(&adap->dev->usb_mutex);

	st->buf[0] = REQUEST_ENABLE_VIDEO;
	/* this bit gives a kind of command,
	 * rather than enabling something or not */
	st->buf[1] = (onoff << 4) | 0x00;

	if (st->disable_streaming_master_mode == 1)
		st->buf[2] = 0x00;
	else
		st->buf[2] = 0x01 << 4; /* Master mode */

	st->buf[3] = 0x00;

	deb_info("modifying (%d) streaming state for %d\n", onoff, adap->id);

	st->channel_state &= ~0x3;
	if ((adap->fe_adap[0].stream.props.endpoint != 2)
			&& (adap->fe_adap[0].stream.props.endpoint != 3)) {
		deb_info("the endpoint number (%i) is not correct, use the adapter id instead", adap->fe_adap[0].stream.props.endpoint);
		if (onoff)
			st->channel_state |=	1 << (adap->id);
		else
			st->channel_state |=	1 << ~(adap->id);
	} else {
		if (onoff)
			st->channel_state |=	1 << (adap->fe_adap[0].stream.props.endpoint-2);
		else
			st->channel_state |=	1 << (3-adap->fe_adap[0].stream.props.endpoint);
	}

	st->buf[2] |= st->channel_state;

	deb_info("data for streaming: %x %x\n", st->buf[1], st->buf[2]);

	ret = dib0700_ctrl_wr(adap->dev, st->buf, 4);
	mutex_unlock(&adap->dev->usb_mutex);

	return ret;
}

int dib0700_change_protocol(struct rc_dev *rc, u64 *rc_proto)
{
	struct dvb_usb_device *d = rc->priv;
	struct dib0700_state *st = d->priv;
	int new_proto, ret;

	if (mutex_lock_interruptible(&d->usb_mutex) < 0) {
		err("could not acquire lock");
		return -EINTR;
	}

	st->buf[0] = REQUEST_SET_RC;
	st->buf[1] = 0;
	st->buf[2] = 0;

	/* Set the IR mode */
	if (*rc_proto & RC_PROTO_BIT_RC5) {
		new_proto = 1;
		*rc_proto = RC_PROTO_BIT_RC5;
	} else if (*rc_proto & RC_PROTO_BIT_NEC) {
		new_proto = 0;
		*rc_proto = RC_PROTO_BIT_NEC;
	} else if (*rc_proto & RC_PROTO_BIT_RC6_MCE) {
		if (st->fw_version < 0x10200) {
			ret = -EINVAL;
			goto out;
		}
		new_proto = 2;
		*rc_proto = RC_PROTO_BIT_RC6_MCE;
	} else {
		ret = -EINVAL;
		goto out;
	}

	st->buf[1] = new_proto;

	ret = dib0700_ctrl_wr(d, st->buf, 3);
	if (ret < 0) {
		err("ir protocol setup failed");
		goto out;
	}

	d->props.rc.core.protocol = *rc_proto;

out:
	mutex_unlock(&d->usb_mutex);
	return ret;
}

/* This is the structure of the RC response packet starting in firmware 1.20 */
struct dib0700_rc_response {
	u8 report_id;
	u8 data_state;
	union {
		struct {
			u8 system;
			u8 not_system;
			u8 data;
			u8 not_data;
		} nec;
		struct {
			u8 not_used;
			u8 system;
			u8 data;
			u8 not_data;
		} rc5;
	};
};
#define RC_MSG_SIZE_V1_20 6

static void dib0700_rc_urb_completion(struct urb *purb)
{
	struct dvb_usb_device *d = purb->context;
	struct dib0700_rc_response *poll_reply;
	enum rc_proto protocol;
	u32 keycode;
	u8 toggle;

	deb_info("%s()\n", __func__);
	if (d->rc_dev == NULL) {
		/* This will occur if disable_rc_polling=1 */
		kfree(purb->transfer_buffer);
		usb_free_urb(purb);
		return;
	}

	poll_reply = purb->transfer_buffer;

	if (purb->status < 0) {
		deb_info("discontinuing polling\n");
		kfree(purb->transfer_buffer);
		usb_free_urb(purb);
		return;
	}

	if (purb->actual_length != RC_MSG_SIZE_V1_20) {
		deb_info("malformed rc msg size=%d\n", purb->actual_length);
		goto resubmit;
	}

	deb_data("IR ID = %02X state = %02X System = %02X %02X Cmd = %02X %02X (len %d)\n",
		 poll_reply->report_id, poll_reply->data_state,
		 poll_reply->nec.system, poll_reply->nec.not_system,
		 poll_reply->nec.data, poll_reply->nec.not_data,
		 purb->actual_length);

	switch (d->props.rc.core.protocol) {
	case RC_PROTO_BIT_NEC:
		toggle = 0;

		/* NEC protocol sends repeat code as 0 0 0 FF */
		if (poll_reply->nec.system     == 0x00 &&
		    poll_reply->nec.not_system == 0x00 &&
		    poll_reply->nec.data       == 0x00 &&
		    poll_reply->nec.not_data   == 0xff) {
			poll_reply->data_state = 2;
			rc_repeat(d->rc_dev);
			goto resubmit;
		}

		if ((poll_reply->nec.data ^ poll_reply->nec.not_data) != 0xff) {
			deb_data("NEC32 protocol\n");
			keycode = RC_SCANCODE_NEC32(poll_reply->nec.system     << 24 |
						     poll_reply->nec.not_system << 16 |
						     poll_reply->nec.data       << 8  |
						     poll_reply->nec.not_data);
			protocol = RC_PROTO_NEC32;
		} else if ((poll_reply->nec.system ^ poll_reply->nec.not_system) != 0xff) {
			deb_data("NEC extended protocol\n");
			keycode = RC_SCANCODE_NECX(poll_reply->nec.system << 8 |
						    poll_reply->nec.not_system,
						    poll_reply->nec.data);

			protocol = RC_PROTO_NECX;
		} else {
			deb_data("NEC normal protocol\n");
			keycode = RC_SCANCODE_NEC(poll_reply->nec.system,
						   poll_reply->nec.data);
			protocol = RC_PROTO_NEC;
		}

		break;
	default:
		deb_data("RC5 protocol\n");
		protocol = RC_PROTO_RC5;
		toggle = poll_reply->report_id;
		keycode = RC_SCANCODE_RC5(poll_reply->rc5.system, poll_reply->rc5.data);

		if ((poll_reply->rc5.data ^ poll_reply->rc5.not_data) != 0xff) {
			/* Key failed integrity check */
			err("key failed integrity check: %02x %02x %02x %02x",
			    poll_reply->rc5.not_used, poll_reply->rc5.system,
			    poll_reply->rc5.data, poll_reply->rc5.not_data);
			goto resubmit;
		}

		break;
	}

	rc_keydown(d->rc_dev, protocol, keycode, toggle);

resubmit:
	/* Clean the buffer before we requeue */
	memset(purb->transfer_buffer, 0, RC_MSG_SIZE_V1_20);

	/* Requeue URB */
	usb_submit_urb(purb, GFP_ATOMIC);
}

int dib0700_rc_setup(struct dvb_usb_device *d, struct usb_interface *intf)
{
	struct dib0700_state *st = d->priv;
	struct urb *purb;
	const struct usb_endpoint_descriptor *e;
	int ret, rc_ep = 1;
	unsigned int pipe = 0;

	/* Poll-based. Don't initialize bulk mode */
	if (st->fw_version < 0x10200 || !intf)
		return 0;

	/* Starting in firmware 1.20, the RC info is provided on a bulk pipe */

	if (intf->cur_altsetting->desc.bNumEndpoints < rc_ep + 1)
		return -ENODEV;

	purb = usb_alloc_urb(0, GFP_KERNEL);
	if (purb == NULL)
		return -ENOMEM;

	purb->transfer_buffer = kzalloc(RC_MSG_SIZE_V1_20, GFP_KERNEL);
	if (purb->transfer_buffer == NULL) {
		err("rc kzalloc failed");
		usb_free_urb(purb);
		return -ENOMEM;
	}

	purb->status = -EINPROGRESS;

	/*
	 * Some devices like the Hauppauge NovaTD model 52009 use an interrupt
	 * endpoint, while others use a bulk one.
	 */
	e = &intf->cur_altsetting->endpoint[rc_ep].desc;
	if (usb_endpoint_dir_in(e)) {
		if (usb_endpoint_xfer_bulk(e)) {
			pipe = usb_rcvbulkpipe(d->udev, rc_ep);
			usb_fill_bulk_urb(purb, d->udev, pipe,
					  purb->transfer_buffer,
					  RC_MSG_SIZE_V1_20,
					  dib0700_rc_urb_completion, d);

		} else if (usb_endpoint_xfer_int(e)) {
			pipe = usb_rcvintpipe(d->udev, rc_ep);
			usb_fill_int_urb(purb, d->udev, pipe,
					  purb->transfer_buffer,
					  RC_MSG_SIZE_V1_20,
					  dib0700_rc_urb_completion, d, 1);
		}
	}

	if (!pipe) {
		err("There's no endpoint for remote controller");
		kfree(purb->transfer_buffer);
		usb_free_urb(purb);
		return 0;
	}

	ret = usb_submit_urb(purb, GFP_ATOMIC);
	if (ret) {
		err("rc submit urb failed");
		kfree(purb->transfer_buffer);
		usb_free_urb(purb);
	}

	return ret;
}

static int dib0700_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	int i;
	struct dvb_usb_device *dev;

	for (i = 0; i < dib0700_device_count; i++)
		if (dvb_usb_device_init(intf, &dib0700_devices[i], THIS_MODULE,
		    &dev, adapter_nr) == 0) {
			struct dib0700_state *st = dev->priv;
			u32 hwversion, romversion, fw_version, fwtype;

			dib0700_get_version(dev, &hwversion, &romversion,
				&fw_version, &fwtype);

			deb_info("Firmware version: %x, %d, 0x%x, %d\n",
				hwversion, romversion, fw_version, fwtype);

			st->fw_version = fw_version;
			st->nb_packet_buffer_size = (u32)nb_packet_buffer_size;

			/* Disable polling mode on newer firmwares */
			if (st->fw_version >= 0x10200)
				dev->props.rc.core.bulk_mode = true;
			else
				dev->props.rc.core.bulk_mode = false;

			dib0700_rc_setup(dev, intf);

			return 0;
		}

	return -ENODEV;
}

static void dib0700_disconnect(struct usb_interface *intf)
{
	struct dvb_usb_device *d = usb_get_intfdata(intf);
	struct dib0700_state *st = d->priv;
	struct i2c_client *client;

	/* remove I2C client for tuner */
	client = st->i2c_client_tuner;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for demodulator */
	client = st->i2c_client_demod;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	dvb_usb_device_exit(intf);
}


static struct usb_driver dib0700_driver = {
	.name       = "dvb_usb_dib0700",
	.probe      = dib0700_probe,
	.disconnect = dib0700_disconnect,
	.id_table   = dib0700_usb_id_table,
};

module_usb_driver(dib0700_driver);

MODULE_FIRMWARE("dvb-usb-dib0700-1.20.fw");
MODULE_AUTHOR("Patrick Boettcher <patrick.boettcher@posteo.de>");
MODULE_DESCRIPTION("Driver for devices based on DiBcom DiB0700 - USB bridge");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");

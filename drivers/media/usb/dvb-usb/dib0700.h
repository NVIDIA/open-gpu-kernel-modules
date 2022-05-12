/* SPDX-License-Identifier: GPL-2.0-only */
/* Linux driver for devices based on the DiBcom DiB0700 USB bridge
 *
 *  Copyright (C) 2005-6 DiBcom, SA
 */
#ifndef _DIB0700_H_
#define _DIB0700_H_

#define DVB_USB_LOG_PREFIX "dib0700"
#include "dvb-usb.h"

#include "dib07x0.h"

extern int dvb_usb_dib0700_debug;
#define deb_info(args...)   dprintk(dvb_usb_dib0700_debug,0x01,args)
#define deb_fw(args...)     dprintk(dvb_usb_dib0700_debug,0x02,args)
#define deb_fwdata(args...) dprintk(dvb_usb_dib0700_debug,0x04,args)
#define deb_data(args...)   dprintk(dvb_usb_dib0700_debug,0x08,args)

#define REQUEST_SET_USB_XFER_LEN    0x0 /* valid only for firmware version */
					/* higher than 1.21 */
#define REQUEST_I2C_READ            0x2
#define REQUEST_I2C_WRITE           0x3
#define REQUEST_POLL_RC             0x4 /* deprecated in firmware v1.20 */
#define REQUEST_JUMPRAM             0x8
#define REQUEST_SET_CLOCK           0xB
#define REQUEST_SET_GPIO            0xC
#define REQUEST_ENABLE_VIDEO        0xF
	// 1 Byte: 4MSB(1 = enable streaming, 0 = disable streaming) 4LSB(Video Mode: 0 = MPEG2 188Bytes, 1 = Analog)
	// 2 Byte: MPEG2 mode:  4MSB(1 = Master Mode, 0 = Slave Mode) 4LSB(Channel 1 = bit0, Channel 2 = bit1)
	// 2 Byte: Analog mode: 4MSB(0 = 625 lines, 1 = 525 lines)    4LSB(     "                "           )
#define REQUEST_SET_I2C_PARAM       0x10
#define REQUEST_SET_RC              0x11
#define REQUEST_NEW_I2C_READ        0x12
#define REQUEST_NEW_I2C_WRITE       0x13
#define REQUEST_GET_VERSION         0x15

struct dib0700_state {
	u8 channel_state;
	u16 mt2060_if1[2];
	u8 rc_toggle;
	u8 rc_counter;
	u8 is_dib7000pc;
	u8 fw_use_new_i2c_api;
	u8 disable_streaming_master_mode;
	u32 fw_version;
	u32 nb_packet_buffer_size;
	int (*read_status)(struct dvb_frontend *, enum fe_status *);
	int (*sleep)(struct dvb_frontend* fe);
	u8 buf[255];
	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
};

int dib0700_get_version(struct dvb_usb_device *d, u32 *hwversion,
			u32 *romversion, u32 *ramversion, u32 *fwtype);
int dib0700_set_gpio(struct dvb_usb_device *d, enum dib07x0_gpios gpio,
		     u8 gpio_dir, u8 gpio_val);
int dib0700_ctrl_clock(struct dvb_usb_device *d, u32 clk_MHz, u8 clock_out_gp3);
int dib0700_ctrl_rd(struct dvb_usb_device *d, u8 *tx, u8 txlen, u8 *rx,
		    u8 rxlen);
int dib0700_download_firmware(struct usb_device *d,
			      const struct firmware *fw);
int dib0700_rc_setup(struct dvb_usb_device *d, struct usb_interface *intf);
int dib0700_streaming_ctrl(struct dvb_usb_adapter *adap, int onoff);
int dib0700_identify_state(struct usb_device *d,
			   const struct dvb_usb_device_properties *props,
			   const struct dvb_usb_device_description **desc,
			   int *cold);
int dib0700_change_protocol(struct rc_dev *dev, u64 *rc_proto);
int dib0700_set_i2c_speed(struct dvb_usb_device *d, u16 scl_kHz);

extern struct i2c_algorithm dib0700_i2c_algo;
extern int dib0700_device_count;
extern int dvb_usb_dib0700_ir_proto;
extern struct dvb_usb_device_properties dib0700_devices[];
extern struct usb_device_id dib0700_usb_id_table[];

#endif

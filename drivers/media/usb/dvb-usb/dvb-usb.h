/* SPDX-License-Identifier: GPL-2.0 */
/* dvb-usb.h is part of the DVB USB library.
 *
 * Copyright (C) 2004-6 Patrick Boettcher (patrick.boettcher@posteo.de)
 * see dvb-usb-init.c for copyright information.
 *
 * the headerfile, all dvb-usb-drivers have to include.
 *
 * TODO: clean-up the structures for unused fields and update the comments
 */
#ifndef __DVB_USB_H__
#define __DVB_USB_H__

#include <linux/input.h>
#include <linux/usb.h>
#include <linux/firmware.h>
#include <linux/mutex.h>
#include <media/rc-core.h>

#include <media/dvb_frontend.h>
#include <media/dvb_demux.h>
#include <media/dvb_net.h>
#include <media/dmxdev.h>

#include "dvb-pll.h"

#include <media/dvb-usb-ids.h>

/* debug */
#ifdef CONFIG_DVB_USB_DEBUG
#define dprintk(var, level, args...) \
	    do { if (((var) & (level))) { printk(args); } } while (0)

#define debug_dump(b, l, func) {\
	int loop_; \
	for (loop_ = 0; loop_ < (l); loop_++) \
		func("%02x ", b[loop_]); \
	func("\n");\
}
#define DVB_USB_DEBUG_STATUS
#else
#define dprintk(var, level, args...) no_printk(args)
#define debug_dump(b, l, func) do { } while (0)

#define DVB_USB_DEBUG_STATUS " (debugging is not enabled)"

#endif

/* generic log methods - taken from usb.h */
#ifndef DVB_USB_LOG_PREFIX
 #define DVB_USB_LOG_PREFIX "dvb-usb (please define a log prefix)"
#endif

#undef err
#define err(format, arg...)  printk(KERN_ERR     DVB_USB_LOG_PREFIX ": " format "\n" , ## arg)
#undef info
#define info(format, arg...) printk(KERN_INFO    DVB_USB_LOG_PREFIX ": " format "\n" , ## arg)
#undef warn
#define warn(format, arg...) printk(KERN_WARNING DVB_USB_LOG_PREFIX ": " format "\n" , ## arg)

/**
 * struct dvb_usb_device_description - name and its according USB IDs
 * @name: real name of the box, regardless which DVB USB device class is in use
 * @cold_ids: array of struct usb_device_id which describe the device in
 *  pre-firmware state
 * @warm_ids: array of struct usb_device_id which describe the device in
 *  post-firmware state
 *
 * Each DVB USB device class can have one or more actual devices, this struct
 * assigns a name to it.
 */
struct dvb_usb_device_description {
	const char *name;

#define DVB_USB_ID_MAX_NUM 15
	struct usb_device_id *cold_ids[DVB_USB_ID_MAX_NUM];
	struct usb_device_id *warm_ids[DVB_USB_ID_MAX_NUM];
};

static inline u8 rc5_custom(struct rc_map_table *key)
{
	return (key->scancode >> 8) & 0xff;
}

static inline u8 rc5_data(struct rc_map_table *key)
{
	return key->scancode & 0xff;
}

static inline u16 rc5_scan(struct rc_map_table *key)
{
	return key->scancode & 0xffff;
}

struct dvb_usb_device;
struct dvb_usb_adapter;
struct usb_data_stream;

/*
 * Properties of USB streaming - TODO this structure should be somewhere else
 * describes the kind of USB transfer used for data-streaming.
 *  (BULK or ISOC)
 */
struct usb_data_stream_properties {
#define USB_BULK  1
#define USB_ISOC  2
	int type;
	int count;
	int endpoint;

	union {
		struct {
			int buffersize; /* per URB */
		} bulk;
		struct {
			int framesperurb;
			int framesize;
			int interval;
		} isoc;
	} u;
};

/**
 * struct dvb_usb_adapter_fe_properties - properties of a dvb-usb-adapter.
 *    A DVB-USB-Adapter is basically a dvb_adapter which is present on a USB-device.
 * @caps: capabilities of the DVB USB device.
 * @pid_filter_count: number of PID filter position in the optional hardware
 *  PID-filter.
 * @num_frontends: number of frontends of the DVB USB adapter.
 * @frontend_ctrl: called to power on/off active frontend.
 * @streaming_ctrl: called to start and stop the MPEG2-TS streaming of the
 *  device (not URB submitting/killing).
 *  This callback will be called without data URBs being active - data URBs
 *  will be submitted only after streaming_ctrl(1) returns successfully and
 *  they will be killed before streaming_ctrl(0) gets called.
 * @pid_filter_ctrl: called to en/disable the PID filter, if any.
 * @pid_filter: called to set/unset a PID for filtering.
 * @frontend_attach: called to attach the possible frontends (fill fe-field
 *  of struct dvb_usb_device).
 * @tuner_attach: called to attach the correct tuner and to fill pll_addr,
 *  pll_desc and pll_init_buf of struct dvb_usb_device).
 * @stream: configuration of the USB streaming
 * @size_of_priv: size of the priv memory in struct dvb_usb_adapter
 */
struct dvb_usb_adapter_fe_properties {
#define DVB_USB_ADAP_HAS_PID_FILTER               0x01
#define DVB_USB_ADAP_PID_FILTER_CAN_BE_TURNED_OFF 0x02
#define DVB_USB_ADAP_NEED_PID_FILTERING           0x04
#define DVB_USB_ADAP_RECEIVES_204_BYTE_TS         0x08
#define DVB_USB_ADAP_RECEIVES_RAW_PAYLOAD         0x10
	int caps;
	int pid_filter_count;

	int (*streaming_ctrl)  (struct dvb_usb_adapter *, int);
	int (*pid_filter_ctrl) (struct dvb_usb_adapter *, int);
	int (*pid_filter)      (struct dvb_usb_adapter *, int, u16, int);

	int (*frontend_attach) (struct dvb_usb_adapter *);
	int (*tuner_attach)    (struct dvb_usb_adapter *);

	struct usb_data_stream_properties stream;

	int size_of_priv;
};

#define MAX_NO_OF_FE_PER_ADAP 3
struct dvb_usb_adapter_properties {
	int size_of_priv;

	int (*frontend_ctrl)   (struct dvb_frontend *, int);

	int num_frontends;
	struct dvb_usb_adapter_fe_properties fe[MAX_NO_OF_FE_PER_ADAP];
};

/**
 * struct dvb_rc_legacy - old properties of remote controller
 * @rc_map_table: a hard-wired array of struct rc_map_table (NULL to disable
 *  remote control handling).
 * @rc_map_size: number of items in @rc_map_table.
 * @rc_query: called to query an event event.
 * @rc_interval: time in ms between two queries.
 */
struct dvb_rc_legacy {
/* remote control properties */
#define REMOTE_NO_KEY_PRESSED      0x00
#define REMOTE_KEY_PRESSED         0x01
#define REMOTE_KEY_REPEAT          0x02
	struct rc_map_table  *rc_map_table;
	int rc_map_size;
	int (*rc_query) (struct dvb_usb_device *, u32 *, int *);
	int rc_interval;
};

/**
 * struct dvb_rc - properties of remote controller, using rc-core
 * @rc_codes: name of rc codes table
 * @protocol: type of protocol(s) currently used by the driver
 * @allowed_protos: protocol(s) supported by the driver
 * @driver_type: Used to point if a device supports raw mode
 * @change_protocol: callback to change protocol
 * @module_name: module name
 * @rc_query: called to query an event event.
 * @rc_interval: time in ms between two queries.
 * @bulk_mode: device supports bulk mode for RC (disable polling mode)
 * @scancode_mask: scancode mask
 */
struct dvb_rc {
	char *rc_codes;
	u64 protocol;
	u64 allowed_protos;
	enum rc_driver_type driver_type;
	int (*change_protocol)(struct rc_dev *dev, u64 *rc_proto);
	char *module_name;
	int (*rc_query) (struct dvb_usb_device *d);
	int rc_interval;
	bool bulk_mode;				/* uses bulk mode */
	u32 scancode_mask;
};

/**
 * enum dvb_usb_mode - Specifies if it is using a legacy driver or a new one
 *		       based on rc-core
 * This is initialized/used only inside dvb-usb-remote.c.
 * It shouldn't be set by the drivers.
 *
 * @DVB_RC_LEGACY: legacy driver
 * @DVB_RC_CORE: rc-core driver
 */
enum dvb_usb_mode {
	DVB_RC_LEGACY,
	DVB_RC_CORE,
};

/**
 * struct dvb_usb_device_properties - properties of a dvb-usb-device
 * @caps: capabilities
 * @usb_ctrl: which USB device-side controller is in use. Needed for firmware
 *  download.
 * @firmware: name of the firmware file.
 * @download_firmware: called to download the firmware when the usb_ctrl is
 *  DEVICE_SPECIFIC.
 * @no_reconnect: device doesn't do a reconnect after downloading the firmware,
 *  so do the warm initialization right after it
 *
 * @size_of_priv: how many bytes shall be allocated for the private field
 *  of struct dvb_usb_device.
 * @priv_init: optional callback to initialize the variable that private field
 * of struct dvb_usb_device has pointer to just after it had been allocated and
 * zeroed.
 * @priv_destroy: just like priv_init, only called before deallocating
 * the memory pointed by private field of struct dvb_usb_device.
 *
 * @num_adapters: the number of adapters in @adapters
 * @adapter: the adapters
 * @power_ctrl: called to enable/disable power of the device.
 * @read_mac_address: called to read the MAC address of the device.
 * @identify_state: called to determine the state (cold or warm), when it
 *  is not distinguishable by the USB IDs.
 *
 * @rc: remote controller properties
 *
 * @i2c_algo: i2c_algorithm if the device has I2CoverUSB.
 *
 * @generic_bulk_ctrl_endpoint: most of the DVB USB devices have a generic
 *  endpoint which received control messages with bulk transfers. When this
 *  is non-zero, one can use dvb_usb_generic_rw and dvb_usb_generic_write-
 *  helper functions.
 *
 * @generic_bulk_ctrl_endpoint_response: some DVB USB devices use a separate
 *  endpoint for responses to control messages sent with bulk transfers via
 *  the generic_bulk_ctrl_endpoint. When this is non-zero, this will be used
 *  instead of the generic_bulk_ctrl_endpoint when reading usb responses in
 *  the dvb_usb_generic_rw helper function.
 *
 * @num_device_descs: number of struct dvb_usb_device_description in @devices
 * @devices: array of struct dvb_usb_device_description compatibles with these
 *  properties.
 */
struct dvb_usb_device_properties {
#define MAX_NO_OF_ADAPTER_PER_DEVICE 2
#define DVB_USB_IS_AN_I2C_ADAPTER            0x01
	int caps;

#define DEVICE_SPECIFIC 0
#define CYPRESS_AN2135  1
#define CYPRESS_AN2235  2
#define CYPRESS_FX2     3
	int        usb_ctrl;
	int        (*download_firmware) (struct usb_device *, const struct firmware *);
	const char *firmware;
	int        no_reconnect;

	int size_of_priv;
	int (*priv_init)(struct dvb_usb_device *);
	void (*priv_destroy)(struct dvb_usb_device *);

	int num_adapters;
	struct dvb_usb_adapter_properties adapter[MAX_NO_OF_ADAPTER_PER_DEVICE];

	int (*power_ctrl)       (struct dvb_usb_device *, int);
	int (*read_mac_address) (struct dvb_usb_device *, u8 []);
	int (*identify_state)(struct usb_device *udev,
			      const struct dvb_usb_device_properties *props,
			      const struct dvb_usb_device_description **desc,
			      int *cold);

	struct {
		enum dvb_usb_mode mode;	/* Drivers shouldn't touch on it */
		struct dvb_rc_legacy legacy;
		struct dvb_rc core;
	} rc;

	struct i2c_algorithm *i2c_algo;

	int generic_bulk_ctrl_endpoint;
	int generic_bulk_ctrl_endpoint_response;

	int num_device_descs;
	struct dvb_usb_device_description devices[12];
};

/**
 * struct usb_data_stream - generic object of an USB stream
 * @udev: the USB device
 * @props: data stream properties
 * @state: state of the stream
 * @complete: complete callback
 * @urb_list: list of URBs
 * @buf_num: number of buffer allocated.
 * @buf_size: size of each buffer in buf_list.
 * @buf_list: array containing all allocate buffers for streaming.
 * @dma_addr: list of dma_addr_t for each buffer in buf_list.
 *
 * @urbs_initialized: number of URBs initialized.
 * @urbs_submitted: number of URBs submitted.
 * @user_priv: for private use.
 */
struct usb_data_stream {
#define MAX_NO_URBS_FOR_DATA_STREAM 10
	struct usb_device                 *udev;
	struct usb_data_stream_properties  props;

#define USB_STATE_INIT    0x00
#define USB_STATE_URB_BUF 0x01
	int state;

	void (*complete) (struct usb_data_stream *, u8 *, size_t);

	struct urb    *urb_list[MAX_NO_URBS_FOR_DATA_STREAM];
	int            buf_num;
	unsigned long  buf_size;
	u8            *buf_list[MAX_NO_URBS_FOR_DATA_STREAM];
	dma_addr_t     dma_addr[MAX_NO_URBS_FOR_DATA_STREAM];

	int urbs_initialized;
	int urbs_submitted;

	void *user_priv;
};

/**
 * struct dvb_usb_fe_adapter - a DVB adapter on a USB device
 * @fe: frontend
 * @fe_init:  rerouted frontend-init (wakeup) function.
 * @fe_sleep: rerouted frontend-sleep function.
 * @stream: the usb data stream.
 * @pid_filtering: is hardware pid_filtering used or not.
 * @max_feed_count: how many feeds can be handled simultaneously by this
 *  device
 * @priv: private pointer
 */
struct dvb_usb_fe_adapter {
	struct dvb_frontend *fe;

	int (*fe_init)  (struct dvb_frontend *);
	int (*fe_sleep) (struct dvb_frontend *);

	struct usb_data_stream stream;

	int pid_filtering;
	int max_feed_count;

	void *priv;
};

/**
 * struct dvb_usb_adapter - a DVB adapter on a USB device
 * @dev: DVB USB device pointer
 * @props: properties
 * @state: status
 * @id: index of this adapter (starting with 0).
 *
 * @feedcount: number of requested feeds (used for streaming-activation)
 *
 * @dvb_adap: device's dvb_adapter.
 * @dmxdev: device's dmxdev.
 * @demux: device's software demuxer.
 * @dvb_net: device's dvb_net interfaces.
 *
 * @fe_adap: frontend adapters
 * @active_fe: active frontend
 * @num_frontends_initialized: number of initialized frontends
 * @priv: private pointer
 */
struct dvb_usb_adapter {
	struct dvb_usb_device *dev;
	struct dvb_usb_adapter_properties props;

#define DVB_USB_ADAP_STATE_INIT 0x000
#define DVB_USB_ADAP_STATE_DVB  0x001
	int state;

	u8  id;

	int feedcount;

	/* dvb */
	struct dvb_adapter   dvb_adap;
	struct dmxdev        dmxdev;
	struct dvb_demux     demux;
	struct dvb_net       dvb_net;

	struct dvb_usb_fe_adapter fe_adap[MAX_NO_OF_FE_PER_ADAP];
	int active_fe;
	int num_frontends_initialized;

	void *priv;
};

/**
 * struct dvb_usb_device - object of a DVB USB device
 * @props: copy of the struct dvb_usb_properties this device belongs to.
 * @desc: pointer to the device's struct dvb_usb_device_description.
 * @state: initialization and runtime state of the device.
 *
 * @powered: indicated whether the device is power or not.
 *  Powered is in/decremented for each call to modify the state.
 * @udev: pointer to the device's struct usb_device.
 *
 * @data_mutex: mutex to protect the data structure used to store URB data
 * @usb_mutex: mutex of USB control messages (reading needs two messages).
 *	Please notice that this mutex is used internally at the generic
 *	URB control functions. So, drivers using dvb_usb_generic_rw() and
 *	derivated functions should not lock it internally.
 * @i2c_mutex: mutex for i2c-transfers
 *
 * @i2c_adap: device's i2c_adapter if it uses I2CoverUSB
 *
 * @num_adapters_initialized: number of initialized adapters
 * @adapter: adapters
 *
 * @rc_dev: rc device for the remote control (rc-core mode)
 * @input_dev: input device for the remote control (legacy mode)
 * @rc_phys: rc device path
 * @rc_query_work: struct work_struct frequent rc queries
 * @last_event: last triggered event
 * @last_state: last state (no, pressed, repeat)
 * @owner: owner of the dvb_adapter
 * @priv: private data of the actual driver (allocate by dvb-usb, size defined
 *  in size_of_priv of dvb_usb_properties).
 */
struct dvb_usb_device {
	struct dvb_usb_device_properties props;
	const struct dvb_usb_device_description *desc;

	struct usb_device *udev;

#define DVB_USB_STATE_INIT        0x000
#define DVB_USB_STATE_I2C         0x001
#define DVB_USB_STATE_DVB         0x002
#define DVB_USB_STATE_REMOTE      0x004
	int state;

	int powered;

	/* locking */
	struct mutex data_mutex;
	struct mutex usb_mutex;

	/* i2c */
	struct mutex i2c_mutex;
	struct i2c_adapter i2c_adap;

	int                    num_adapters_initialized;
	struct dvb_usb_adapter adapter[MAX_NO_OF_ADAPTER_PER_DEVICE];

	/* remote control */
	struct rc_dev *rc_dev;
	struct input_dev *input_dev;
	char rc_phys[64];
	struct delayed_work rc_query_work;
	u32 last_event;
	int last_state;

	struct module *owner;

	void *priv;
};

extern int dvb_usb_device_init(struct usb_interface *,
			       const struct dvb_usb_device_properties *,
			       struct module *, struct dvb_usb_device **,
			       short *adapter_nums);
extern void dvb_usb_device_exit(struct usb_interface *);

/* the generic read/write method for device control */
extern int __must_check
dvb_usb_generic_rw(struct dvb_usb_device *, u8 *, u16, u8 *, u16, int);
extern int __must_check
dvb_usb_generic_write(struct dvb_usb_device *, u8 *, u16);

/* commonly used remote control parsing */
int dvb_usb_nec_rc_key_to_event(struct dvb_usb_device *d, u8 keybuf[5],
				u32 *event, int *state);

/* commonly used firmware download types and function */
struct hexline {
	u8 len;
	u32 addr;
	u8 type;
	u8 data[255];
	u8 chk;
};
extern int usb_cypress_load_firmware(struct usb_device *udev, const struct firmware *fw, int type);
extern int dvb_usb_get_hexline(const struct firmware *fw, struct hexline *hx, int *pos);


#endif

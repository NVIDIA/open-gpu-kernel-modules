/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 */
#ifndef __PVRUSB2_HDW_H
#define __PVRUSB2_HDW_H

#include <linux/usb.h>
#include <linux/videodev2.h>
#include <media/v4l2-dev.h>
#include "pvrusb2-io.h"
#include "pvrusb2-ctrl.h"


/* Private internal control ids, look these up with
   pvr2_hdw_get_ctrl_by_id() - these are NOT visible in V4L */
#define PVR2_CID_STDCUR 2
#define PVR2_CID_STDAVAIL 3
#define PVR2_CID_INPUT 4
#define PVR2_CID_AUDIOMODE 5
#define PVR2_CID_FREQUENCY 6
#define PVR2_CID_HRES 7
#define PVR2_CID_VRES 8
#define PVR2_CID_CROPL 9
#define PVR2_CID_CROPT 10
#define PVR2_CID_CROPW 11
#define PVR2_CID_CROPH 12
#define PVR2_CID_CROPCAPPAN 13
#define PVR2_CID_CROPCAPPAD 14
#define PVR2_CID_CROPCAPBL 15
#define PVR2_CID_CROPCAPBT 16
#define PVR2_CID_CROPCAPBW 17
#define PVR2_CID_CROPCAPBH 18
#define PVR2_CID_STDDETECT 19

/* Legal values for the INPUT state variable */
#define PVR2_CVAL_INPUT_TV 0
#define PVR2_CVAL_INPUT_DTV 1
#define PVR2_CVAL_INPUT_COMPOSITE 2
#define PVR2_CVAL_INPUT_SVIDEO 3
#define PVR2_CVAL_INPUT_RADIO 4
#define PVR2_CVAL_INPUT_MAX PVR2_CVAL_INPUT_RADIO

enum pvr2_config {
	pvr2_config_empty,    /* No configuration */
	pvr2_config_mpeg,     /* Encoded / compressed video */
	pvr2_config_vbi,      /* Standard vbi info */
	pvr2_config_pcm,      /* Audio raw pcm stream */
	pvr2_config_rawvideo, /* Video raw frames */
};

enum pvr2_v4l_type {
	pvr2_v4l_type_video,
	pvr2_v4l_type_vbi,
	pvr2_v4l_type_radio,
};

/* Major states that we can be in:
 *
 *  DEAD - Device is in an unusable state and cannot be recovered.  This
 *  can happen if we completely lose the ability to communicate with it
 *  (but it might still on the bus).  In this state there's nothing we can
 *  do; it must be replugged in order to recover.
 *
 *  COLD - Device is in an unusable state, needs microcontroller firmware.
 *
 *  WARM - We can communicate with the device and the proper
 *  microcontroller firmware is running, but other device initialization is
 *  still needed (e.g. encoder firmware).
 *
 *  ERROR - A problem prevents capture operation (e.g. encoder firmware
 *  missing).
 *
 *  READY - Device is operational, but not streaming.
 *
 *  RUN - Device is streaming.
 *
 */
#define PVR2_STATE_NONE 0
#define PVR2_STATE_DEAD 1
#define PVR2_STATE_COLD 2
#define PVR2_STATE_WARM 3
#define PVR2_STATE_ERROR 4
#define PVR2_STATE_READY 5
#define PVR2_STATE_RUN 6

/* Translate configuration enum to a string label */
const char *pvr2_config_get_name(enum pvr2_config);

struct pvr2_hdw;

/* Create and return a structure for interacting with the underlying
   hardware */
struct pvr2_hdw *pvr2_hdw_create(struct usb_interface *intf,
				 const struct usb_device_id *devid);

/* Perform second stage initialization, passing in a notification callback
   for when the master state changes. */
int pvr2_hdw_initialize(struct pvr2_hdw *,
			void (*callback_func)(void *),
			void *callback_data);

/* Destroy hardware interaction structure */
void pvr2_hdw_destroy(struct pvr2_hdw *);

/* Return true if in the ready (normal) state */
int pvr2_hdw_dev_ok(struct pvr2_hdw *);

/* Return small integer number [1..N] for logical instance number of this
   device.  This is useful for indexing array-valued module parameters. */
int pvr2_hdw_get_unit_number(struct pvr2_hdw *);

/* Get pointer to underlying USB device */
struct usb_device *pvr2_hdw_get_dev(struct pvr2_hdw *);

/* Retrieve serial number of device */
unsigned long pvr2_hdw_get_sn(struct pvr2_hdw *);

/* Retrieve bus location info of device */
const char *pvr2_hdw_get_bus_info(struct pvr2_hdw *);

/* Retrieve per-instance string identifier for this specific device */
const char *pvr2_hdw_get_device_identifier(struct pvr2_hdw *);

/* Called when hardware has been unplugged */
void pvr2_hdw_disconnect(struct pvr2_hdw *);

/* Sets v4l2_dev of a video_device struct */
void pvr2_hdw_set_v4l2_dev(struct pvr2_hdw *, struct video_device *);

/* Get the number of defined controls */
unsigned int pvr2_hdw_get_ctrl_count(struct pvr2_hdw *);

/* Retrieve a control handle given its index (0..count-1) */
struct pvr2_ctrl *pvr2_hdw_get_ctrl_by_index(struct pvr2_hdw *,unsigned int);

/* Retrieve a control handle given its internal ID (if any) */
struct pvr2_ctrl *pvr2_hdw_get_ctrl_by_id(struct pvr2_hdw *,unsigned int);

/* Retrieve a control handle given its V4L ID (if any) */
struct pvr2_ctrl *pvr2_hdw_get_ctrl_v4l(struct pvr2_hdw *,unsigned int ctl_id);

/* Retrieve a control handle given its immediate predecessor V4L ID (if any) */
struct pvr2_ctrl *pvr2_hdw_get_ctrl_nextv4l(struct pvr2_hdw *,
					    unsigned int ctl_id);

/* Commit all control changes made up to this point */
int pvr2_hdw_commit_ctl(struct pvr2_hdw *);

/* Return a bit mask of valid input selections for this device.  Mask bits
 * will be according to PVR_CVAL_INPUT_xxxx definitions. */
unsigned int pvr2_hdw_get_input_available(struct pvr2_hdw *);

/* Return a bit mask of allowed input selections for this device.  Mask bits
 * will be according to PVR_CVAL_INPUT_xxxx definitions. */
unsigned int pvr2_hdw_get_input_allowed(struct pvr2_hdw *);

/* Change the set of allowed input selections for this device.  Both
   change_mask and change_valu are mask bits according to
   PVR_CVAL_INPUT_xxxx definitions.  The change_mask parameter indicate
   which settings are being changed and the change_val parameter indicates
   whether corresponding settings are being set or cleared. */
int pvr2_hdw_set_input_allowed(struct pvr2_hdw *,
			       unsigned int change_mask,
			       unsigned int change_val);

/* Return name for this driver instance */
const char *pvr2_hdw_get_driver_name(struct pvr2_hdw *);

/* Mark tuner status stale so that it will be re-fetched */
void pvr2_hdw_execute_tuner_poll(struct pvr2_hdw *);

/* Return information about the tuner */
int pvr2_hdw_get_tuner_status(struct pvr2_hdw *,struct v4l2_tuner *);

/* Return information about cropping capabilities */
int pvr2_hdw_get_cropcap(struct pvr2_hdw *, struct v4l2_cropcap *);

/* Query device and see if it thinks it is on a high-speed USB link */
int pvr2_hdw_is_hsm(struct pvr2_hdw *);

/* Return a string token representative of the hardware type */
const char *pvr2_hdw_get_type(struct pvr2_hdw *);

/* Return a single line description of the hardware type */
const char *pvr2_hdw_get_desc(struct pvr2_hdw *);

/* Turn streaming on/off */
int pvr2_hdw_set_streaming(struct pvr2_hdw *,int);

/* Find out if streaming is on */
int pvr2_hdw_get_streaming(struct pvr2_hdw *);

/* Retrieve driver overall state */
int pvr2_hdw_get_state(struct pvr2_hdw *);

/* Configure the type of stream to generate */
int pvr2_hdw_set_stream_type(struct pvr2_hdw *, enum pvr2_config);

/* Get handle to video output stream */
struct pvr2_stream *pvr2_hdw_get_video_stream(struct pvr2_hdw *);

/* Enable / disable retrieval of CPU firmware or prom contents.  This must
   be enabled before pvr2_hdw_cpufw_get() will function.  Note that doing
   this may prevent the device from running (and leaving this mode may
   imply a device reset). */
void pvr2_hdw_cpufw_set_enabled(struct pvr2_hdw *,
				int mode, /* 0=8KB FX2, 1=16KB FX2, 2=PROM */
				int enable_flag);

/* Return true if we're in a mode for retrieval CPU firmware */
int pvr2_hdw_cpufw_get_enabled(struct pvr2_hdw *);

/* Retrieve a piece of the CPU's firmware at the given offset.  Return
   value is the number of bytes retrieved or zero if we're past the end or
   an error otherwise (e.g. if firmware retrieval is not enabled). */
int pvr2_hdw_cpufw_get(struct pvr2_hdw *,unsigned int offs,
		       char *buf,unsigned int cnt);

/* Retrieve a previously stored v4l minor device number */
int pvr2_hdw_v4l_get_minor_number(struct pvr2_hdw *,enum pvr2_v4l_type index);

/* Store a v4l minor device number */
void pvr2_hdw_v4l_store_minor_number(struct pvr2_hdw *,
				     enum pvr2_v4l_type index,int);

/* The following entry points are all lower level things you normally don't
   want to worry about. */

/* Issue a command and get a response from the device.  LOTS of higher
   level stuff is built on this. */
int pvr2_send_request(struct pvr2_hdw *,
		      void *write_ptr,unsigned int write_len,
		      void *read_ptr,unsigned int read_len);

/* Slightly higher level device communication functions. */
int pvr2_write_register(struct pvr2_hdw *, u16, u32);

/* Call if for any reason we can't talk to the hardware anymore - this will
   cause the driver to stop flailing on the device. */
void pvr2_hdw_render_useless(struct pvr2_hdw *);

/* Set / clear 8051's reset bit */
void pvr2_hdw_cpureset_assert(struct pvr2_hdw *,int);

/* Execute a USB-commanded device reset */
void pvr2_hdw_device_reset(struct pvr2_hdw *);

/* Reset worker's error trapping circuit breaker */
int pvr2_hdw_untrip(struct pvr2_hdw *);

/* Execute hard reset command (after this point it's likely that the
   encoder will have to be reconfigured).  This also clears the "useless"
   state. */
int pvr2_hdw_cmd_deep_reset(struct pvr2_hdw *);

/* Execute simple reset command */
int pvr2_hdw_cmd_powerup(struct pvr2_hdw *);

/* Order decoder to reset */
int pvr2_hdw_cmd_decoder_reset(struct pvr2_hdw *);

/* Direct manipulation of GPIO bits */
int pvr2_hdw_gpio_get_dir(struct pvr2_hdw *hdw,u32 *);
int pvr2_hdw_gpio_get_out(struct pvr2_hdw *hdw,u32 *);
int pvr2_hdw_gpio_get_in(struct pvr2_hdw *hdw,u32 *);
int pvr2_hdw_gpio_chg_dir(struct pvr2_hdw *hdw,u32 msk,u32 val);
int pvr2_hdw_gpio_chg_out(struct pvr2_hdw *hdw,u32 msk,u32 val);

/* This data structure is specifically for the next function... */
struct pvr2_hdw_debug_info {
	int big_lock_held;
	int ctl_lock_held;
	int flag_disconnected;
	int flag_init_ok;
	int flag_ok;
	int fw1_state;
	int flag_decoder_missed;
	int flag_tripped;
	int state_encoder_ok;
	int state_encoder_run;
	int state_decoder_run;
	int state_decoder_ready;
	int state_usbstream_run;
	int state_decoder_quiescent;
	int state_pipeline_config;
	int state_pipeline_req;
	int state_pipeline_pause;
	int state_pipeline_idle;
	int cmd_debug_state;
	int cmd_debug_write_len;
	int cmd_debug_read_len;
	int cmd_debug_write_pend;
	int cmd_debug_read_pend;
	int cmd_debug_timeout;
	int cmd_debug_rstatus;
	int cmd_debug_wstatus;
	unsigned char cmd_code;
};

/* Non-intrusively retrieve internal state info - this is useful for
   diagnosing lockups.  Note that this operation is completed without any
   kind of locking and so it is not atomic and may yield inconsistent
   results.  This is *purely* a debugging aid. */
void pvr2_hdw_get_debug_info_unlocked(const struct pvr2_hdw *hdw,
				      struct pvr2_hdw_debug_info *);

/* Intrusively retrieve internal state info - this is useful for
   diagnosing overall driver state.  This operation synchronizes against
   the overall driver mutex - so if there are locking problems this will
   likely hang!  This is *purely* a debugging aid. */
void pvr2_hdw_get_debug_info_locked(struct pvr2_hdw *hdw,
				    struct pvr2_hdw_debug_info *);

/* Report out several lines of text that describes driver internal state.
   Results are written into the passed-in buffer. */
unsigned int pvr2_hdw_state_report(struct pvr2_hdw *hdw,
				   char *buf_ptr,unsigned int buf_size);

/* Cause modules to log their state once */
void pvr2_hdw_trigger_module_log(struct pvr2_hdw *hdw);

/* Cause encoder firmware to be uploaded into the device.  This is normally
   done autonomously, but the interface is exported here because it is also
   a debugging aid. */
int pvr2_upload_firmware2(struct pvr2_hdw *hdw);

#endif /* __PVRUSB2_HDW_H */

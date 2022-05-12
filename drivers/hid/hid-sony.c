// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  HID driver for Sony / PS2 / PS3 / PS4 BD devices.
 *
 *  Copyright (c) 1999 Andreas Gal
 *  Copyright (c) 2000-2005 Vojtech Pavlik <vojtech@suse.cz>
 *  Copyright (c) 2005 Michael Haboustak <mike-@cinci.rr.com> for Concept2, Inc
 *  Copyright (c) 2008 Jiri Slaby
 *  Copyright (c) 2012 David Dillow <dave@thedillows.org>
 *  Copyright (c) 2006-2013 Jiri Kosina
 *  Copyright (c) 2013 Colin Leitner <colin.leitner@gmail.com>
 *  Copyright (c) 2014-2016 Frank Praznik <frank.praznik@gmail.com>
 *  Copyright (c) 2018 Todd Kelner
 *  Copyright (c) 2020 Pascal Giard <pascal.giard@etsmtl.ca>
 *  Copyright (c) 2020 Sanjay Govind <sanjay.govind9@gmail.com>
 */

/*
 */

/*
 * NOTE: in order for the Sony PS3 BD Remote Control to be found by
 * a Bluetooth host, the key combination Start+Enter has to be kept pressed
 * for about 7 seconds with the Bluetooth Host Controller in discovering mode.
 *
 * There will be no PIN request from the device.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/power_supply.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/idr.h>
#include <linux/input/mt.h>
#include <linux/crc32.h>
#include <linux/usb.h>
#include <linux/timer.h>
#include <asm/unaligned.h>

#include "hid-ids.h"

#define VAIO_RDESC_CONSTANT       BIT(0)
#define SIXAXIS_CONTROLLER_USB    BIT(1)
#define SIXAXIS_CONTROLLER_BT     BIT(2)
#define BUZZ_CONTROLLER           BIT(3)
#define PS3REMOTE                 BIT(4)
#define DUALSHOCK4_CONTROLLER_USB BIT(5)
#define DUALSHOCK4_CONTROLLER_BT  BIT(6)
#define DUALSHOCK4_DONGLE         BIT(7)
#define MOTION_CONTROLLER_USB     BIT(8)
#define MOTION_CONTROLLER_BT      BIT(9)
#define NAVIGATION_CONTROLLER_USB BIT(10)
#define NAVIGATION_CONTROLLER_BT  BIT(11)
#define SINO_LITE_CONTROLLER      BIT(12)
#define FUTUREMAX_DANCE_MAT       BIT(13)
#define NSG_MR5U_REMOTE_BT        BIT(14)
#define NSG_MR7U_REMOTE_BT        BIT(15)
#define SHANWAN_GAMEPAD           BIT(16)
#define GH_GUITAR_CONTROLLER      BIT(17)
#define GHL_GUITAR_PS3WIIU        BIT(18)

#define SIXAXIS_CONTROLLER (SIXAXIS_CONTROLLER_USB | SIXAXIS_CONTROLLER_BT)
#define MOTION_CONTROLLER (MOTION_CONTROLLER_USB | MOTION_CONTROLLER_BT)
#define NAVIGATION_CONTROLLER (NAVIGATION_CONTROLLER_USB |\
				NAVIGATION_CONTROLLER_BT)
#define DUALSHOCK4_CONTROLLER (DUALSHOCK4_CONTROLLER_USB |\
				DUALSHOCK4_CONTROLLER_BT | \
				DUALSHOCK4_DONGLE)
#define SONY_LED_SUPPORT (SIXAXIS_CONTROLLER | BUZZ_CONTROLLER |\
				DUALSHOCK4_CONTROLLER | MOTION_CONTROLLER |\
				NAVIGATION_CONTROLLER)
#define SONY_BATTERY_SUPPORT (SIXAXIS_CONTROLLER | DUALSHOCK4_CONTROLLER |\
				MOTION_CONTROLLER_BT | NAVIGATION_CONTROLLER)
#define SONY_FF_SUPPORT (SIXAXIS_CONTROLLER | DUALSHOCK4_CONTROLLER |\
				MOTION_CONTROLLER)
#define SONY_BT_DEVICE (SIXAXIS_CONTROLLER_BT | DUALSHOCK4_CONTROLLER_BT |\
			MOTION_CONTROLLER_BT | NAVIGATION_CONTROLLER_BT)
#define NSG_MRXU_REMOTE (NSG_MR5U_REMOTE_BT | NSG_MR7U_REMOTE_BT)

#define MAX_LEDS 4
#define NSG_MRXU_MAX_X 1667
#define NSG_MRXU_MAX_Y 1868

#define GHL_GUITAR_POKE_INTERVAL 10 /* In seconds */
#define GUITAR_TILT_USAGE 44

/* Magic value and data taken from GHLtarUtility:
 * https://github.com/ghlre/GHLtarUtility/blob/master/PS3Guitar.cs
 * Note: The Wii U and PS3 dongles happen to share the same!
 */
static const u16 ghl_ps3wiiu_magic_value = 0x201;
static const char ghl_ps3wiiu_magic_data[] = {
	0x02, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* PS/3 Motion controller */
static u8 motion_rdesc[] = {
	0x05, 0x01,         /*  Usage Page (Desktop),               */
	0x09, 0x04,         /*  Usage (Joystick),                   */
	0xA1, 0x01,         /*  Collection (Application),           */
	0xA1, 0x02,         /*      Collection (Logical),           */
	0x85, 0x01,         /*          Report ID (1),              */
	0x75, 0x01,         /*          Report Size (1),            */
	0x95, 0x15,         /*          Report Count (21),          */
	0x15, 0x00,         /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x35, 0x00,         /*          Physical Minimum (0),       */
	0x45, 0x01,         /*          Physical Maximum (1),       */
	0x05, 0x09,         /*          Usage Page (Button),        */
	0x19, 0x01,         /*          Usage Minimum (01h),        */
	0x29, 0x15,         /*          Usage Maximum (15h),        */
	0x81, 0x02,         /*          Input (Variable),           * Buttons */
	0x95, 0x0B,         /*          Report Count (11),          */
	0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),         */
	0x81, 0x03,         /*          Input (Constant, Variable), * Padding */
	0x15, 0x00,         /*          Logical Minimum (0),        */
	0x26, 0xFF, 0x00,   /*          Logical Maximum (255),      */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0xA1, 0x00,         /*          Collection (Physical),      */
	0x75, 0x08,         /*              Report Size (8),        */
	0x95, 0x01,         /*              Report Count (1),       */
	0x35, 0x00,         /*              Physical Minimum (0),   */
	0x46, 0xFF, 0x00,   /*              Physical Maximum (255), */
	0x09, 0x30,         /*              Usage (X),              */
	0x81, 0x02,         /*              Input (Variable),       * Trigger */
	0xC0,               /*          End Collection,             */
	0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),         */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x07,         /*          Report Count (7),           * skip 7 bytes */
	0x81, 0x02,         /*          Input (Variable),           */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x75, 0x10,         /*          Report Size (16),           */
	0x46, 0xFF, 0xFF,   /*          Physical Maximum (65535),   */
	0x27, 0xFF, 0xFF, 0x00, 0x00, /*      Logical Maximum (65535),    */
	0x95, 0x03,         /*          Report Count (3),           * 3x Accels */
	0x09, 0x33,         /*              Usage (rX),             */
	0x09, 0x34,         /*              Usage (rY),             */
	0x09, 0x35,         /*              Usage (rZ),             */
	0x81, 0x02,         /*          Input (Variable),           */
	0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),         */
	0x95, 0x03,         /*          Report Count (3),           * Skip Accels 2nd frame */
	0x81, 0x02,         /*          Input (Variable),           */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x09, 0x01,         /*          Usage (Pointer),            */
	0x95, 0x03,         /*          Report Count (3),           * 3x Gyros */
	0x81, 0x02,         /*          Input (Variable),           */
	0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),         */
	0x95, 0x03,         /*          Report Count (3),           * Skip Gyros 2nd frame */
	0x81, 0x02,         /*          Input (Variable),           */
	0x75, 0x0C,         /*          Report Size (12),           */
	0x46, 0xFF, 0x0F,   /*          Physical Maximum (4095),    */
	0x26, 0xFF, 0x0F,   /*          Logical Maximum (4095),     */
	0x95, 0x04,         /*          Report Count (4),           * Skip Temp and Magnetometers */
	0x81, 0x02,         /*          Input (Variable),           */
	0x75, 0x08,         /*          Report Size (8),            */
	0x46, 0xFF, 0x00,   /*          Physical Maximum (255),     */
	0x26, 0xFF, 0x00,   /*          Logical Maximum (255),      */
	0x95, 0x06,         /*          Report Count (6),           * Skip Timestamp and Extension Bytes */
	0x81, 0x02,         /*          Input (Variable),           */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x30,         /*          Report Count (48),          */
	0x09, 0x01,         /*          Usage (Pointer),            */
	0x91, 0x02,         /*          Output (Variable),          */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x30,         /*          Report Count (48),          */
	0x09, 0x01,         /*          Usage (Pointer),            */
	0xB1, 0x02,         /*          Feature (Variable),         */
	0xC0,               /*      End Collection,                 */
	0xA1, 0x02,         /*      Collection (Logical),           */
	0x85, 0x02,         /*          Report ID (2),              */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x30,         /*          Report Count (48),          */
	0x09, 0x01,         /*          Usage (Pointer),            */
	0xB1, 0x02,         /*          Feature (Variable),         */
	0xC0,               /*      End Collection,                 */
	0xA1, 0x02,         /*      Collection (Logical),           */
	0x85, 0xEE,         /*          Report ID (238),            */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x30,         /*          Report Count (48),          */
	0x09, 0x01,         /*          Usage (Pointer),            */
	0xB1, 0x02,         /*          Feature (Variable),         */
	0xC0,               /*      End Collection,                 */
	0xA1, 0x02,         /*      Collection (Logical),           */
	0x85, 0xEF,         /*          Report ID (239),            */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x30,         /*          Report Count (48),          */
	0x09, 0x01,         /*          Usage (Pointer),            */
	0xB1, 0x02,         /*          Feature (Variable),         */
	0xC0,               /*      End Collection,                 */
	0xC0                /*  End Collection                      */
};

static u8 ps3remote_rdesc[] = {
	0x05, 0x01,          /* GUsagePage Generic Desktop */
	0x09, 0x05,          /* LUsage 0x05 [Game Pad] */
	0xA1, 0x01,          /* MCollection Application (mouse, keyboard) */

	 /* Use collection 1 for joypad buttons */
	 0xA1, 0x02,         /* MCollection Logical (interrelated data) */

	  /*
	   * Ignore the 1st byte, maybe it is used for a controller
	   * number but it's not needed for correct operation
	   */
	  0x75, 0x08,        /* GReportSize 0x08 [8] */
	  0x95, 0x01,        /* GReportCount 0x01 [1] */
	  0x81, 0x01,        /* MInput 0x01 (Const[0] Arr[1] Abs[2]) */

	  /*
	   * Bytes from 2nd to 4th are a bitmap for joypad buttons, for these
	   * buttons multiple keypresses are allowed
	   */
	  0x05, 0x09,        /* GUsagePage Button */
	  0x19, 0x01,        /* LUsageMinimum 0x01 [Button 1 (primary/trigger)] */
	  0x29, 0x18,        /* LUsageMaximum 0x18 [Button 24] */
	  0x14,              /* GLogicalMinimum [0] */
	  0x25, 0x01,        /* GLogicalMaximum 0x01 [1] */
	  0x75, 0x01,        /* GReportSize 0x01 [1] */
	  0x95, 0x18,        /* GReportCount 0x18 [24] */
	  0x81, 0x02,        /* MInput 0x02 (Data[0] Var[1] Abs[2]) */

	  0xC0,              /* MEndCollection */

	 /* Use collection 2 for remote control buttons */
	 0xA1, 0x02,         /* MCollection Logical (interrelated data) */

	  /* 5th byte is used for remote control buttons */
	  0x05, 0x09,        /* GUsagePage Button */
	  0x18,              /* LUsageMinimum [No button pressed] */
	  0x29, 0xFE,        /* LUsageMaximum 0xFE [Button 254] */
	  0x14,              /* GLogicalMinimum [0] */
	  0x26, 0xFE, 0x00,  /* GLogicalMaximum 0x00FE [254] */
	  0x75, 0x08,        /* GReportSize 0x08 [8] */
	  0x95, 0x01,        /* GReportCount 0x01 [1] */
	  0x80,              /* MInput  */

	  /*
	   * Ignore bytes from 6th to 11th, 6th to 10th are always constant at
	   * 0xff and 11th is for press indication
	   */
	  0x75, 0x08,        /* GReportSize 0x08 [8] */
	  0x95, 0x06,        /* GReportCount 0x06 [6] */
	  0x81, 0x01,        /* MInput 0x01 (Const[0] Arr[1] Abs[2]) */

	  /* 12th byte is for battery strength */
	  0x05, 0x06,        /* GUsagePage Generic Device Controls */
	  0x09, 0x20,        /* LUsage 0x20 [Battery Strength] */
	  0x14,              /* GLogicalMinimum [0] */
	  0x25, 0x05,        /* GLogicalMaximum 0x05 [5] */
	  0x75, 0x08,        /* GReportSize 0x08 [8] */
	  0x95, 0x01,        /* GReportCount 0x01 [1] */
	  0x81, 0x02,        /* MInput 0x02 (Data[0] Var[1] Abs[2]) */

	  0xC0,              /* MEndCollection */

	 0xC0                /* MEndCollection [Game Pad] */
};

static const unsigned int ps3remote_keymap_joypad_buttons[] = {
	[0x01] = KEY_SELECT,
	[0x02] = BTN_THUMBL,		/* L3 */
	[0x03] = BTN_THUMBR,		/* R3 */
	[0x04] = BTN_START,
	[0x05] = KEY_UP,
	[0x06] = KEY_RIGHT,
	[0x07] = KEY_DOWN,
	[0x08] = KEY_LEFT,
	[0x09] = BTN_TL2,		/* L2 */
	[0x0a] = BTN_TR2,		/* R2 */
	[0x0b] = BTN_TL,		/* L1 */
	[0x0c] = BTN_TR,		/* R1 */
	[0x0d] = KEY_OPTION,		/* options/triangle */
	[0x0e] = KEY_BACK,		/* back/circle */
	[0x0f] = BTN_0,			/* cross */
	[0x10] = KEY_SCREEN,		/* view/square */
	[0x11] = KEY_HOMEPAGE,		/* PS button */
	[0x14] = KEY_ENTER,
};
static const unsigned int ps3remote_keymap_remote_buttons[] = {
	[0x00] = KEY_1,
	[0x01] = KEY_2,
	[0x02] = KEY_3,
	[0x03] = KEY_4,
	[0x04] = KEY_5,
	[0x05] = KEY_6,
	[0x06] = KEY_7,
	[0x07] = KEY_8,
	[0x08] = KEY_9,
	[0x09] = KEY_0,
	[0x0e] = KEY_ESC,		/* return */
	[0x0f] = KEY_CLEAR,
	[0x16] = KEY_EJECTCD,
	[0x1a] = KEY_MENU,		/* top menu */
	[0x28] = KEY_TIME,
	[0x30] = KEY_PREVIOUS,
	[0x31] = KEY_NEXT,
	[0x32] = KEY_PLAY,
	[0x33] = KEY_REWIND,		/* scan back */
	[0x34] = KEY_FORWARD,		/* scan forward */
	[0x38] = KEY_STOP,
	[0x39] = KEY_PAUSE,
	[0x40] = KEY_CONTEXT_MENU,	/* pop up/menu */
	[0x60] = KEY_FRAMEBACK,		/* slow/step back */
	[0x61] = KEY_FRAMEFORWARD,	/* slow/step forward */
	[0x63] = KEY_SUBTITLE,
	[0x64] = KEY_AUDIO,
	[0x65] = KEY_ANGLE,
	[0x70] = KEY_INFO,		/* display */
	[0x80] = KEY_BLUE,
	[0x81] = KEY_RED,
	[0x82] = KEY_GREEN,
	[0x83] = KEY_YELLOW,
};

static const unsigned int buzz_keymap[] = {
	/*
	 * The controller has 4 remote buzzers, each with one LED and 5
	 * buttons.
	 *
	 * We use the mapping chosen by the controller, which is:
	 *
	 * Key          Offset
	 * -------------------
	 * Buzz              1
	 * Blue              5
	 * Orange            4
	 * Green             3
	 * Yellow            2
	 *
	 * So, for example, the orange button on the third buzzer is mapped to
	 * BTN_TRIGGER_HAPPY14
	 */
	 [1] = BTN_TRIGGER_HAPPY1,
	 [2] = BTN_TRIGGER_HAPPY2,
	 [3] = BTN_TRIGGER_HAPPY3,
	 [4] = BTN_TRIGGER_HAPPY4,
	 [5] = BTN_TRIGGER_HAPPY5,
	 [6] = BTN_TRIGGER_HAPPY6,
	 [7] = BTN_TRIGGER_HAPPY7,
	 [8] = BTN_TRIGGER_HAPPY8,
	 [9] = BTN_TRIGGER_HAPPY9,
	[10] = BTN_TRIGGER_HAPPY10,
	[11] = BTN_TRIGGER_HAPPY11,
	[12] = BTN_TRIGGER_HAPPY12,
	[13] = BTN_TRIGGER_HAPPY13,
	[14] = BTN_TRIGGER_HAPPY14,
	[15] = BTN_TRIGGER_HAPPY15,
	[16] = BTN_TRIGGER_HAPPY16,
	[17] = BTN_TRIGGER_HAPPY17,
	[18] = BTN_TRIGGER_HAPPY18,
	[19] = BTN_TRIGGER_HAPPY19,
	[20] = BTN_TRIGGER_HAPPY20,
};

/* The Navigation controller is a partial DS3 and uses the same HID report
 * and hence the same keymap indices, however not not all axes/buttons
 * are physically present. We use the same axis and button mapping as
 * the DS3, which uses the Linux gamepad spec.
 */
static const unsigned int navigation_absmap[] = {
	[0x30] = ABS_X,
	[0x31] = ABS_Y,
	[0x33] = ABS_Z, /* L2 */
};

/* Buttons not physically available on the device, but still available
 * in the reports are explicitly set to 0 for documentation purposes.
 */
static const unsigned int navigation_keymap[] = {
	[0x01] = 0, /* Select */
	[0x02] = BTN_THUMBL, /* L3 */
	[0x03] = 0, /* R3 */
	[0x04] = 0, /* Start */
	[0x05] = BTN_DPAD_UP, /* Up */
	[0x06] = BTN_DPAD_RIGHT, /* Right */
	[0x07] = BTN_DPAD_DOWN, /* Down */
	[0x08] = BTN_DPAD_LEFT, /* Left */
	[0x09] = BTN_TL2, /* L2 */
	[0x0a] = 0, /* R2 */
	[0x0b] = BTN_TL, /* L1 */
	[0x0c] = 0, /* R1 */
	[0x0d] = BTN_NORTH, /* Triangle */
	[0x0e] = BTN_EAST, /* Circle */
	[0x0f] = BTN_SOUTH, /* Cross */
	[0x10] = BTN_WEST, /* Square */
	[0x11] = BTN_MODE, /* PS */
};

static const unsigned int sixaxis_absmap[] = {
	[0x30] = ABS_X,
	[0x31] = ABS_Y,
	[0x32] = ABS_RX, /* right stick X */
	[0x35] = ABS_RY, /* right stick Y */
};

static const unsigned int sixaxis_keymap[] = {
	[0x01] = BTN_SELECT, /* Select */
	[0x02] = BTN_THUMBL, /* L3 */
	[0x03] = BTN_THUMBR, /* R3 */
	[0x04] = BTN_START, /* Start */
	[0x05] = BTN_DPAD_UP, /* Up */
	[0x06] = BTN_DPAD_RIGHT, /* Right */
	[0x07] = BTN_DPAD_DOWN, /* Down */
	[0x08] = BTN_DPAD_LEFT, /* Left */
	[0x09] = BTN_TL2, /* L2 */
	[0x0a] = BTN_TR2, /* R2 */
	[0x0b] = BTN_TL, /* L1 */
	[0x0c] = BTN_TR, /* R1 */
	[0x0d] = BTN_NORTH, /* Triangle */
	[0x0e] = BTN_EAST, /* Circle */
	[0x0f] = BTN_SOUTH, /* Cross */
	[0x10] = BTN_WEST, /* Square */
	[0x11] = BTN_MODE, /* PS */
};

static const unsigned int ds4_absmap[] = {
	[0x30] = ABS_X,
	[0x31] = ABS_Y,
	[0x32] = ABS_RX, /* right stick X */
	[0x33] = ABS_Z, /* L2 */
	[0x34] = ABS_RZ, /* R2 */
	[0x35] = ABS_RY, /* right stick Y */
};

static const unsigned int ds4_keymap[] = {
	[0x1] = BTN_WEST, /* Square */
	[0x2] = BTN_SOUTH, /* Cross */
	[0x3] = BTN_EAST, /* Circle */
	[0x4] = BTN_NORTH, /* Triangle */
	[0x5] = BTN_TL, /* L1 */
	[0x6] = BTN_TR, /* R1 */
	[0x7] = BTN_TL2, /* L2 */
	[0x8] = BTN_TR2, /* R2 */
	[0x9] = BTN_SELECT, /* Share */
	[0xa] = BTN_START, /* Options */
	[0xb] = BTN_THUMBL, /* L3 */
	[0xc] = BTN_THUMBR, /* R3 */
	[0xd] = BTN_MODE, /* PS */
};

static const struct {int x; int y; } ds4_hat_mapping[] = {
	{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
	{0, 0}
};

static enum power_supply_property sony_battery_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_SCOPE,
	POWER_SUPPLY_PROP_STATUS,
};

struct sixaxis_led {
	u8 time_enabled; /* the total time the led is active (0xff means forever) */
	u8 duty_length;  /* how long a cycle is in deciseconds (0 means "really fast") */
	u8 enabled;
	u8 duty_off; /* % of duty_length the led is off (0xff means 100%) */
	u8 duty_on;  /* % of duty_length the led is on (0xff mean 100%) */
} __packed;

struct sixaxis_rumble {
	u8 padding;
	u8 right_duration; /* Right motor duration (0xff means forever) */
	u8 right_motor_on; /* Right (small) motor on/off, only supports values of 0 or 1 (off/on) */
	u8 left_duration;    /* Left motor duration (0xff means forever) */
	u8 left_motor_force; /* left (large) motor, supports force values from 0 to 255 */
} __packed;

struct sixaxis_output_report {
	u8 report_id;
	struct sixaxis_rumble rumble;
	u8 padding[4];
	u8 leds_bitmap; /* bitmap of enabled LEDs: LED_1 = 0x02, LED_2 = 0x04, ... */
	struct sixaxis_led led[4];    /* LEDx at (4 - x) */
	struct sixaxis_led _reserved; /* LED5, not actually soldered */
} __packed;

union sixaxis_output_report_01 {
	struct sixaxis_output_report data;
	u8 buf[36];
};

struct motion_output_report_02 {
	u8 type, zero;
	u8 r, g, b;
	u8 zero2;
	u8 rumble;
};

#define DS4_FEATURE_REPORT_0x02_SIZE 37
#define DS4_FEATURE_REPORT_0x05_SIZE 41
#define DS4_FEATURE_REPORT_0x81_SIZE 7
#define DS4_FEATURE_REPORT_0xA3_SIZE 49
#define DS4_INPUT_REPORT_0x11_SIZE 78
#define DS4_OUTPUT_REPORT_0x05_SIZE 32
#define DS4_OUTPUT_REPORT_0x11_SIZE 78
#define SIXAXIS_REPORT_0xF2_SIZE 17
#define SIXAXIS_REPORT_0xF5_SIZE 8
#define MOTION_REPORT_0x02_SIZE 49

/* Offsets relative to USB input report (0x1). Bluetooth (0x11) requires an
 * additional +2.
 */
#define DS4_INPUT_REPORT_AXIS_OFFSET      1
#define DS4_INPUT_REPORT_BUTTON_OFFSET    5
#define DS4_INPUT_REPORT_TIMESTAMP_OFFSET 10
#define DS4_INPUT_REPORT_GYRO_X_OFFSET   13
#define DS4_INPUT_REPORT_BATTERY_OFFSET  30
#define DS4_INPUT_REPORT_TOUCHPAD_OFFSET 33

#define SENSOR_SUFFIX " Motion Sensors"
#define DS4_TOUCHPAD_SUFFIX " Touchpad"

/* Default to 4ms poll interval, which is same as USB (not adjustable). */
#define DS4_BT_DEFAULT_POLL_INTERVAL_MS 4
#define DS4_BT_MAX_POLL_INTERVAL_MS 62
#define DS4_GYRO_RES_PER_DEG_S 1024
#define DS4_ACC_RES_PER_G      8192

#define SIXAXIS_INPUT_REPORT_ACC_X_OFFSET 41
#define SIXAXIS_ACC_RES_PER_G 113

static DEFINE_SPINLOCK(sony_dev_list_lock);
static LIST_HEAD(sony_device_list);
static DEFINE_IDA(sony_device_id_allocator);

/* Used for calibration of DS4 accelerometer and gyro. */
struct ds4_calibration_data {
	int abs_code;
	short bias;
	/* Calibration requires scaling against a sensitivity value, which is a
	 * float. Store sensitivity as a fraction to limit floating point
	 * calculations until final calibration.
	 */
	int sens_numer;
	int sens_denom;
};

enum ds4_dongle_state {
	DONGLE_DISCONNECTED,
	DONGLE_CALIBRATING,
	DONGLE_CONNECTED,
	DONGLE_DISABLED
};

enum sony_worker {
	SONY_WORKER_STATE,
	SONY_WORKER_HOTPLUG
};

struct sony_sc {
	spinlock_t lock;
	struct list_head list_node;
	struct hid_device *hdev;
	struct input_dev *touchpad;
	struct input_dev *sensor_dev;
	struct led_classdev *leds[MAX_LEDS];
	unsigned long quirks;
	struct work_struct hotplug_worker;
	struct work_struct state_worker;
	void (*send_output_report)(struct sony_sc *);
	struct power_supply *battery;
	struct power_supply_desc battery_desc;
	int device_id;
	unsigned fw_version;
	bool fw_version_created;
	unsigned hw_version;
	bool hw_version_created;
	u8 *output_report_dmabuf;

#ifdef CONFIG_SONY_FF
	u8 left;
	u8 right;
#endif

	u8 mac_address[6];
	u8 hotplug_worker_initialized;
	u8 state_worker_initialized;
	u8 defer_initialization;
	u8 battery_capacity;
	int battery_status;
	u8 led_state[MAX_LEDS];
	u8 led_delay_on[MAX_LEDS];
	u8 led_delay_off[MAX_LEDS];
	u8 led_count;

	bool timestamp_initialized;
	u16 prev_timestamp;
	unsigned int timestamp_us;

	u8 ds4_bt_poll_interval;
	enum ds4_dongle_state ds4_dongle_state;
	/* DS4 calibration data */
	struct ds4_calibration_data ds4_calib_data[6];
	/* GH Live */
	struct timer_list ghl_poke_timer;
	struct usb_ctrlrequest *ghl_cr;
	u8 *ghl_databuf;
};

static void sony_set_leds(struct sony_sc *sc);

static inline void sony_schedule_work(struct sony_sc *sc,
				      enum sony_worker which)
{
	unsigned long flags;

	switch (which) {
	case SONY_WORKER_STATE:
		spin_lock_irqsave(&sc->lock, flags);
		if (!sc->defer_initialization && sc->state_worker_initialized)
			schedule_work(&sc->state_worker);
		spin_unlock_irqrestore(&sc->lock, flags);
		break;
	case SONY_WORKER_HOTPLUG:
		if (sc->hotplug_worker_initialized)
			schedule_work(&sc->hotplug_worker);
		break;
	}
}

static void ghl_magic_poke_cb(struct urb *urb)
{
	if (urb) {
		/* Free sc->ghl_cr and sc->ghl_databuf allocated in
		 * ghl_magic_poke()
		 */
		kfree(urb->setup_packet);
		kfree(urb->transfer_buffer);
	}
}

static void ghl_magic_poke(struct timer_list *t)
{
	struct sony_sc *sc = from_timer(sc, t, ghl_poke_timer);

	int ret;
	unsigned int pipe;
	struct urb *urb;
	struct usb_device *usbdev = to_usb_device(sc->hdev->dev.parent->parent);
	const u16 poke_size =
		ARRAY_SIZE(ghl_ps3wiiu_magic_data);

	pipe = usb_sndctrlpipe(usbdev, 0);

	if (!sc->ghl_cr) {
		sc->ghl_cr = kzalloc(sizeof(*sc->ghl_cr), GFP_ATOMIC);
		if (!sc->ghl_cr)
			goto resched;
	}

	if (!sc->ghl_databuf) {
		sc->ghl_databuf = kzalloc(poke_size, GFP_ATOMIC);
		if (!sc->ghl_databuf)
			goto resched;
	}

	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb)
		goto resched;

	sc->ghl_cr->bRequestType =
		USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_OUT;
	sc->ghl_cr->bRequest = USB_REQ_SET_CONFIGURATION;
	sc->ghl_cr->wValue = cpu_to_le16(ghl_ps3wiiu_magic_value);
	sc->ghl_cr->wIndex = 0;
	sc->ghl_cr->wLength = cpu_to_le16(poke_size);
	memcpy(sc->ghl_databuf, ghl_ps3wiiu_magic_data, poke_size);

	usb_fill_control_urb(
		urb, usbdev, pipe,
		(unsigned char *) sc->ghl_cr, sc->ghl_databuf,
		poke_size, ghl_magic_poke_cb, NULL);
	ret = usb_submit_urb(urb, GFP_ATOMIC);
	if (ret < 0) {
		kfree(sc->ghl_databuf);
		kfree(sc->ghl_cr);
	}
	usb_free_urb(urb);

resched:
	/* Reschedule for next time */
	mod_timer(&sc->ghl_poke_timer, jiffies + GHL_GUITAR_POKE_INTERVAL*HZ);
}

static int guitar_mapping(struct hid_device *hdev, struct hid_input *hi,
			  struct hid_field *field, struct hid_usage *usage,
			  unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_MSVENDOR) {
		unsigned int abs = usage->hid & HID_USAGE;

		if (abs == GUITAR_TILT_USAGE) {
			hid_map_usage_clear(hi, usage, bit, max, EV_ABS, ABS_RY);
			return 1;
		}
	}
	return 0;
}

static ssize_t ds4_show_poll_interval(struct device *dev,
				struct device_attribute
				*attr, char *buf)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);

	return snprintf(buf, PAGE_SIZE, "%i\n", sc->ds4_bt_poll_interval);
}

static ssize_t ds4_store_poll_interval(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);
	unsigned long flags;
	u8 interval;

	if (kstrtou8(buf, 0, &interval))
		return -EINVAL;

	if (interval > DS4_BT_MAX_POLL_INTERVAL_MS)
		return -EINVAL;

	spin_lock_irqsave(&sc->lock, flags);
	sc->ds4_bt_poll_interval = interval;
	spin_unlock_irqrestore(&sc->lock, flags);

	sony_schedule_work(sc, SONY_WORKER_STATE);

	return count;
}

static DEVICE_ATTR(bt_poll_interval, 0644, ds4_show_poll_interval,
		ds4_store_poll_interval);

static ssize_t sony_show_firmware_version(struct device *dev,
				struct device_attribute
				*attr, char *buf)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);

	return snprintf(buf, PAGE_SIZE, "0x%04x\n", sc->fw_version);
}

static DEVICE_ATTR(firmware_version, 0444, sony_show_firmware_version, NULL);

static ssize_t sony_show_hardware_version(struct device *dev,
				struct device_attribute
				*attr, char *buf)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);

	return snprintf(buf, PAGE_SIZE, "0x%04x\n", sc->hw_version);
}

static DEVICE_ATTR(hardware_version, 0444, sony_show_hardware_version, NULL);

static u8 *motion_fixup(struct hid_device *hdev, u8 *rdesc,
			     unsigned int *rsize)
{
	*rsize = sizeof(motion_rdesc);
	return motion_rdesc;
}

static u8 *ps3remote_fixup(struct hid_device *hdev, u8 *rdesc,
			     unsigned int *rsize)
{
	*rsize = sizeof(ps3remote_rdesc);
	return ps3remote_rdesc;
}

static int ps3remote_mapping(struct hid_device *hdev, struct hid_input *hi,
			     struct hid_field *field, struct hid_usage *usage,
			     unsigned long **bit, int *max)
{
	unsigned int key = usage->hid & HID_USAGE;

	if ((usage->hid & HID_USAGE_PAGE) != HID_UP_BUTTON)
		return -1;

	switch (usage->collection_index) {
	case 1:
		if (key >= ARRAY_SIZE(ps3remote_keymap_joypad_buttons))
			return -1;

		key = ps3remote_keymap_joypad_buttons[key];
		if (!key)
			return -1;
		break;
	case 2:
		if (key >= ARRAY_SIZE(ps3remote_keymap_remote_buttons))
			return -1;

		key = ps3remote_keymap_remote_buttons[key];
		if (!key)
			return -1;
		break;
	default:
		return -1;
	}

	hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
	return 1;
}

static int navigation_mapping(struct hid_device *hdev, struct hid_input *hi,
			  struct hid_field *field, struct hid_usage *usage,
			  unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_BUTTON) {
		unsigned int key = usage->hid & HID_USAGE;

		if (key >= ARRAY_SIZE(sixaxis_keymap))
			return -1;

		key = navigation_keymap[key];
		if (!key)
			return -1;

		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	} else if (usage->hid == HID_GD_POINTER) {
		/* See comment in sixaxis_mapping, basically the L2 (and R2)
		 * triggers are reported through GD Pointer.
		 * In addition we ignore any analog button 'axes' and only
		 * support digital buttons.
		 */
		switch (usage->usage_index) {
		case 8: /* L2 */
			usage->hid = HID_GD_Z;
			break;
		default:
			return -1;
		}

		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, usage->hid & 0xf);
		return 1;
	} else if ((usage->hid & HID_USAGE_PAGE) == HID_UP_GENDESK) {
		unsigned int abs = usage->hid & HID_USAGE;

		if (abs >= ARRAY_SIZE(navigation_absmap))
			return -1;

		abs = navigation_absmap[abs];

		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, abs);
		return 1;
	}

	return -1;
}


static int sixaxis_mapping(struct hid_device *hdev, struct hid_input *hi,
			  struct hid_field *field, struct hid_usage *usage,
			  unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_BUTTON) {
		unsigned int key = usage->hid & HID_USAGE;

		if (key >= ARRAY_SIZE(sixaxis_keymap))
			return -1;

		key = sixaxis_keymap[key];
		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	} else if (usage->hid == HID_GD_POINTER) {
		/* The DS3 provides analog values for most buttons and even
		 * for HAT axes through GD Pointer. L2 and R2 are reported
		 * among these as well instead of as GD Z / RZ. Remap L2
		 * and R2 and ignore other analog 'button axes' as there is
		 * no good way for reporting them.
		 */
		switch (usage->usage_index) {
		case 8: /* L2 */
			usage->hid = HID_GD_Z;
			break;
		case 9: /* R2 */
			usage->hid = HID_GD_RZ;
			break;
		default:
			return -1;
		}

		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, usage->hid & 0xf);
		return 1;
	} else if ((usage->hid & HID_USAGE_PAGE) == HID_UP_GENDESK) {
		unsigned int abs = usage->hid & HID_USAGE;

		if (abs >= ARRAY_SIZE(sixaxis_absmap))
			return -1;

		abs = sixaxis_absmap[abs];

		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, abs);
		return 1;
	}

	return -1;
}

static int ds4_mapping(struct hid_device *hdev, struct hid_input *hi,
		       struct hid_field *field, struct hid_usage *usage,
		       unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_BUTTON) {
		unsigned int key = usage->hid & HID_USAGE;

		if (key >= ARRAY_SIZE(ds4_keymap))
			return -1;

		key = ds4_keymap[key];
		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	} else if ((usage->hid & HID_USAGE_PAGE) == HID_UP_GENDESK) {
		unsigned int abs = usage->hid & HID_USAGE;

		/* Let the HID parser deal with the HAT. */
		if (usage->hid == HID_GD_HATSWITCH)
			return 0;

		if (abs >= ARRAY_SIZE(ds4_absmap))
			return -1;

		abs = ds4_absmap[abs];
		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, abs);
		return 1;
	}

	return 0;
}

static u8 *sony_report_fixup(struct hid_device *hdev, u8 *rdesc,
		unsigned int *rsize)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	if (sc->quirks & (SINO_LITE_CONTROLLER | FUTUREMAX_DANCE_MAT))
		return rdesc;

	/*
	 * Some Sony RF receivers wrongly declare the mouse pointer as a
	 * a constant non-data variable.
	 */
	if ((sc->quirks & VAIO_RDESC_CONSTANT) && *rsize >= 56 &&
	    /* usage page: generic desktop controls */
	    /* rdesc[0] == 0x05 && rdesc[1] == 0x01 && */
	    /* usage: mouse */
	    rdesc[2] == 0x09 && rdesc[3] == 0x02 &&
	    /* input (usage page for x,y axes): constant, variable, relative */
	    rdesc[54] == 0x81 && rdesc[55] == 0x07) {
		hid_info(hdev, "Fixing up Sony RF Receiver report descriptor\n");
		/* input: data, variable, relative */
		rdesc[55] = 0x06;
	}

	if (sc->quirks & MOTION_CONTROLLER)
		return motion_fixup(hdev, rdesc, rsize);

	if (sc->quirks & PS3REMOTE)
		return ps3remote_fixup(hdev, rdesc, rsize);

	/*
	 * Some knock-off USB dongles incorrectly report their button count
	 * as 13 instead of 16 causing three non-functional buttons.
	 */
	if ((sc->quirks & SIXAXIS_CONTROLLER_USB) && *rsize >= 45 &&
		/* Report Count (13) */
		rdesc[23] == 0x95 && rdesc[24] == 0x0D &&
		/* Usage Maximum (13) */
		rdesc[37] == 0x29 && rdesc[38] == 0x0D &&
		/* Report Count (3) */
		rdesc[43] == 0x95 && rdesc[44] == 0x03) {
		hid_info(hdev, "Fixing up USB dongle report descriptor\n");
		rdesc[24] = 0x10;
		rdesc[38] = 0x10;
		rdesc[44] = 0x00;
	}

	return rdesc;
}

static void sixaxis_parse_report(struct sony_sc *sc, u8 *rd, int size)
{
	static const u8 sixaxis_battery_capacity[] = { 0, 1, 25, 50, 75, 100 };
	unsigned long flags;
	int offset;
	u8 battery_capacity;
	int battery_status;

	/*
	 * The sixaxis is charging if the battery value is 0xee
	 * and it is fully charged if the value is 0xef.
	 * It does not report the actual level while charging so it
	 * is set to 100% while charging is in progress.
	 */
	offset = (sc->quirks & MOTION_CONTROLLER) ? 12 : 30;

	if (rd[offset] >= 0xee) {
		battery_capacity = 100;
		battery_status = (rd[offset] & 0x01) ? POWER_SUPPLY_STATUS_FULL : POWER_SUPPLY_STATUS_CHARGING;
	} else {
		u8 index = rd[offset] <= 5 ? rd[offset] : 5;
		battery_capacity = sixaxis_battery_capacity[index];
		battery_status = POWER_SUPPLY_STATUS_DISCHARGING;
	}

	spin_lock_irqsave(&sc->lock, flags);
	sc->battery_capacity = battery_capacity;
	sc->battery_status = battery_status;
	spin_unlock_irqrestore(&sc->lock, flags);

	if (sc->quirks & SIXAXIS_CONTROLLER) {
		int val;

		offset = SIXAXIS_INPUT_REPORT_ACC_X_OFFSET;
		val = ((rd[offset+1] << 8) | rd[offset]) - 511;
		input_report_abs(sc->sensor_dev, ABS_X, val);

		/* Y and Z are swapped and inversed */
		val = 511 - ((rd[offset+5] << 8) | rd[offset+4]);
		input_report_abs(sc->sensor_dev, ABS_Y, val);

		val = 511 - ((rd[offset+3] << 8) | rd[offset+2]);
		input_report_abs(sc->sensor_dev, ABS_Z, val);

		input_sync(sc->sensor_dev);
	}
}

static void dualshock4_parse_report(struct sony_sc *sc, u8 *rd, int size)
{
	struct hid_input *hidinput = list_entry(sc->hdev->inputs.next,
						struct hid_input, list);
	struct input_dev *input_dev = hidinput->input;
	unsigned long flags;
	int n, m, offset, num_touch_data, max_touch_data;
	u8 cable_state, battery_capacity;
	int battery_status;
	u16 timestamp;

	/* When using Bluetooth the header is 2 bytes longer, so skip these. */
	int data_offset = (sc->quirks & DUALSHOCK4_CONTROLLER_BT) ? 2 : 0;

	/* Second bit of third button byte is for the touchpad button. */
	offset = data_offset + DS4_INPUT_REPORT_BUTTON_OFFSET;
	input_report_key(sc->touchpad, BTN_LEFT, rd[offset+2] & 0x2);

	/*
	 * The default behavior of the Dualshock 4 is to send reports using
	 * report type 1 when running over Bluetooth. However, when feature
	 * report 2 is requested during the controller initialization it starts
	 * sending input reports in report 17. Since report 17 is undefined
	 * in the default HID descriptor, the HID layer won't generate events.
	 * While it is possible (and this was done before) to fixup the HID
	 * descriptor to add this mapping, it was better to do this manually.
	 * The reason is there were various pieces software both open and closed
	 * source, relying on the descriptors to be the same across various
	 * operating systems. If the descriptors wouldn't match some
	 * applications e.g. games on Wine would not be able to function due
	 * to different descriptors, which such applications are not parsing.
	 */
	if (rd[0] == 17) {
		int value;

		offset = data_offset + DS4_INPUT_REPORT_AXIS_OFFSET;
		input_report_abs(input_dev, ABS_X, rd[offset]);
		input_report_abs(input_dev, ABS_Y, rd[offset+1]);
		input_report_abs(input_dev, ABS_RX, rd[offset+2]);
		input_report_abs(input_dev, ABS_RY, rd[offset+3]);

		value = rd[offset+4] & 0xf;
		if (value > 7)
			value = 8; /* Center 0, 0 */
		input_report_abs(input_dev, ABS_HAT0X, ds4_hat_mapping[value].x);
		input_report_abs(input_dev, ABS_HAT0Y, ds4_hat_mapping[value].y);

		input_report_key(input_dev, BTN_WEST, rd[offset+4] & 0x10);
		input_report_key(input_dev, BTN_SOUTH, rd[offset+4] & 0x20);
		input_report_key(input_dev, BTN_EAST, rd[offset+4] & 0x40);
		input_report_key(input_dev, BTN_NORTH, rd[offset+4] & 0x80);

		input_report_key(input_dev, BTN_TL, rd[offset+5] & 0x1);
		input_report_key(input_dev, BTN_TR, rd[offset+5] & 0x2);
		input_report_key(input_dev, BTN_TL2, rd[offset+5] & 0x4);
		input_report_key(input_dev, BTN_TR2, rd[offset+5] & 0x8);
		input_report_key(input_dev, BTN_SELECT, rd[offset+5] & 0x10);
		input_report_key(input_dev, BTN_START, rd[offset+5] & 0x20);
		input_report_key(input_dev, BTN_THUMBL, rd[offset+5] & 0x40);
		input_report_key(input_dev, BTN_THUMBR, rd[offset+5] & 0x80);

		input_report_key(input_dev, BTN_MODE, rd[offset+6] & 0x1);

		input_report_abs(input_dev, ABS_Z, rd[offset+7]);
		input_report_abs(input_dev, ABS_RZ, rd[offset+8]);

		input_sync(input_dev);
	}

	/* Convert timestamp (in 5.33us unit) to timestamp_us */
	offset = data_offset + DS4_INPUT_REPORT_TIMESTAMP_OFFSET;
	timestamp = get_unaligned_le16(&rd[offset]);
	if (!sc->timestamp_initialized) {
		sc->timestamp_us = ((unsigned int)timestamp * 16) / 3;
		sc->timestamp_initialized = true;
	} else {
		u16 delta;

		if (sc->prev_timestamp > timestamp)
			delta = (U16_MAX - sc->prev_timestamp + timestamp + 1);
		else
			delta = timestamp - sc->prev_timestamp;
		sc->timestamp_us += (delta * 16) / 3;
	}
	sc->prev_timestamp = timestamp;
	input_event(sc->sensor_dev, EV_MSC, MSC_TIMESTAMP, sc->timestamp_us);

	offset = data_offset + DS4_INPUT_REPORT_GYRO_X_OFFSET;
	for (n = 0; n < 6; n++) {
		/* Store data in int for more precision during mult_frac. */
		int raw_data = (short)((rd[offset+1] << 8) | rd[offset]);
		struct ds4_calibration_data *calib = &sc->ds4_calib_data[n];

		/* High precision is needed during calibration, but the
		 * calibrated values are within 32-bit.
		 * Note: we swap numerator 'x' and 'numer' in mult_frac for
		 *       precision reasons so we don't need 64-bit.
		 */
		int calib_data = mult_frac(calib->sens_numer,
					   raw_data - calib->bias,
					   calib->sens_denom);

		input_report_abs(sc->sensor_dev, calib->abs_code, calib_data);
		offset += 2;
	}
	input_sync(sc->sensor_dev);

	/*
	 * The lower 4 bits of byte 30 (or 32 for BT) contain the battery level
	 * and the 5th bit contains the USB cable state.
	 */
	offset = data_offset + DS4_INPUT_REPORT_BATTERY_OFFSET;
	cable_state = (rd[offset] >> 4) & 0x01;

	/*
	 * Interpretation of the battery_capacity data depends on the cable state.
	 * When no cable is connected (bit4 is 0):
	 * - 0:10: percentage in units of 10%.
	 * When a cable is plugged in:
	 * - 0-10: percentage in units of 10%.
	 * - 11: battery is full
	 * - 14: not charging due to Voltage or temperature error
	 * - 15: charge error
	 */
	if (cable_state) {
		u8 battery_data = rd[offset] & 0xf;

		if (battery_data < 10) {
			/* Take the mid-point for each battery capacity value,
			 * because on the hardware side 0 = 0-9%, 1=10-19%, etc.
			 * This matches official platform behavior, which does
			 * the same.
			 */
			battery_capacity = battery_data * 10 + 5;
			battery_status = POWER_SUPPLY_STATUS_CHARGING;
		} else if (battery_data == 10) {
			battery_capacity = 100;
			battery_status = POWER_SUPPLY_STATUS_CHARGING;
		} else if (battery_data == 11) {
			battery_capacity = 100;
			battery_status = POWER_SUPPLY_STATUS_FULL;
		} else { /* 14, 15 and undefined values */
			battery_capacity = 0;
			battery_status = POWER_SUPPLY_STATUS_UNKNOWN;
		}
	} else {
		u8 battery_data = rd[offset] & 0xf;

		if (battery_data < 10)
			battery_capacity = battery_data * 10 + 5;
		else /* 10 */
			battery_capacity = 100;

		battery_status = POWER_SUPPLY_STATUS_DISCHARGING;
	}

	spin_lock_irqsave(&sc->lock, flags);
	sc->battery_capacity = battery_capacity;
	sc->battery_status = battery_status;
	spin_unlock_irqrestore(&sc->lock, flags);

	/*
	 * The Dualshock 4 multi-touch trackpad data starts at offset 33 on USB
	 * and 35 on Bluetooth.
	 * The first byte indicates the number of touch data in the report.
	 * Trackpad data starts 2 bytes later (e.g. 35 for USB).
	 */
	offset = data_offset + DS4_INPUT_REPORT_TOUCHPAD_OFFSET;
	max_touch_data = (sc->quirks & DUALSHOCK4_CONTROLLER_BT) ? 4 : 3;
	if (rd[offset] > 0 && rd[offset] <= max_touch_data)
		num_touch_data = rd[offset];
	else
		num_touch_data = 1;
	offset += 1;

	for (m = 0; m < num_touch_data; m++) {
		/* Skip past timestamp */
		offset += 1;

		/*
		 * The first 7 bits of the first byte is a counter and bit 8 is
		 * a touch indicator that is 0 when pressed and 1 when not
		 * pressed.
		 * The next 3 bytes are two 12 bit touch coordinates, X and Y.
		 * The data for the second touch is in the same format and
		 * immediately follows the data for the first.
		 */
		for (n = 0; n < 2; n++) {
			u16 x, y;
			bool active;

			x = rd[offset+1] | ((rd[offset+2] & 0xF) << 8);
			y = ((rd[offset+2] & 0xF0) >> 4) | (rd[offset+3] << 4);

			active = !(rd[offset] >> 7);
			input_mt_slot(sc->touchpad, n);
			input_mt_report_slot_state(sc->touchpad, MT_TOOL_FINGER, active);

			if (active) {
				input_report_abs(sc->touchpad, ABS_MT_POSITION_X, x);
				input_report_abs(sc->touchpad, ABS_MT_POSITION_Y, y);
			}

			offset += 4;
		}
		input_mt_sync_frame(sc->touchpad);
		input_sync(sc->touchpad);
	}
}

static void nsg_mrxu_parse_report(struct sony_sc *sc, u8 *rd, int size)
{
	int n, offset, relx, rely;
	u8 active;

	/*
	 * The NSG-MRxU multi-touch trackpad data starts at offset 1 and
	 *   the touch-related data starts at offset 2.
	 * For the first byte, bit 0 is set when touchpad button is pressed.
	 * Bit 2 is set when a touch is active and the drag (Fn) key is pressed.
	 * This drag key is mapped to BTN_LEFT.  It is operational only when a 
	 *   touch point is active.
	 * Bit 4 is set when only the first touch point is active.
	 * Bit 6 is set when only the second touch point is active.
	 * Bits 5 and 7 are set when both touch points are active.
	 * The next 3 bytes are two 12 bit X/Y coordinates for the first touch.
	 * The following byte, offset 5, has the touch width and length.
	 *   Bits 0-4=X (width), bits 5-7=Y (length).
	 * A signed relative X coordinate is at offset 6.
	 * The bytes at offset 7-9 are the second touch X/Y coordinates.
	 * Offset 10 has the second touch width and length.
	 * Offset 11 has the relative Y coordinate.
	 */
	offset = 1;

	input_report_key(sc->touchpad, BTN_LEFT, rd[offset] & 0x0F);
	active = (rd[offset] >> 4);
	relx = (s8) rd[offset+5];
	rely = ((s8) rd[offset+10]) * -1;

	offset++;

	for (n = 0; n < 2; n++) {
		u16 x, y;
		u8 contactx, contacty;

		x = rd[offset] | ((rd[offset+1] & 0x0F) << 8);
		y = ((rd[offset+1] & 0xF0) >> 4) | (rd[offset+2] << 4);

		input_mt_slot(sc->touchpad, n);
		input_mt_report_slot_state(sc->touchpad, MT_TOOL_FINGER, active & 0x03);

		if (active & 0x03) {
			contactx = rd[offset+3] & 0x0F;
			contacty = rd[offset+3] >> 4;
			input_report_abs(sc->touchpad, ABS_MT_TOUCH_MAJOR,
				max(contactx, contacty));
			input_report_abs(sc->touchpad, ABS_MT_TOUCH_MINOR,
				min(contactx, contacty));
			input_report_abs(sc->touchpad, ABS_MT_ORIENTATION,
				(bool) (contactx > contacty));
			input_report_abs(sc->touchpad, ABS_MT_POSITION_X, x);
			input_report_abs(sc->touchpad, ABS_MT_POSITION_Y,
				NSG_MRXU_MAX_Y - y);
			/*
			 * The relative coordinates belong to the first touch
			 * point, when present, or to the second touch point
			 * when the first is not active.
			 */
			if ((n == 0) || ((n == 1) && (active & 0x01))) {
				input_report_rel(sc->touchpad, REL_X, relx);
				input_report_rel(sc->touchpad, REL_Y, rely);
			}
		}

		offset += 5;
		active >>= 2;
	}

	input_mt_sync_frame(sc->touchpad);

	input_sync(sc->touchpad);
}

static int sony_raw_event(struct hid_device *hdev, struct hid_report *report,
		u8 *rd, int size)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	/*
	 * Sixaxis HID report has acclerometers/gyro with MSByte first, this
	 * has to be BYTE_SWAPPED before passing up to joystick interface
	 */
	if ((sc->quirks & SIXAXIS_CONTROLLER) && rd[0] == 0x01 && size == 49) {
		/*
		 * When connected via Bluetooth the Sixaxis occasionally sends
		 * a report with the second byte 0xff and the rest zeroed.
		 *
		 * This report does not reflect the actual state of the
		 * controller must be ignored to avoid generating false input
		 * events.
		 */
		if (rd[1] == 0xff)
			return -EINVAL;

		swap(rd[41], rd[42]);
		swap(rd[43], rd[44]);
		swap(rd[45], rd[46]);
		swap(rd[47], rd[48]);

		sixaxis_parse_report(sc, rd, size);
	} else if ((sc->quirks & MOTION_CONTROLLER_BT) && rd[0] == 0x01 && size == 49) {
		sixaxis_parse_report(sc, rd, size);
	} else if ((sc->quirks & NAVIGATION_CONTROLLER) && rd[0] == 0x01 &&
			size == 49) {
		sixaxis_parse_report(sc, rd, size);
	} else if ((sc->quirks & DUALSHOCK4_CONTROLLER_USB) && rd[0] == 0x01 &&
			size == 64) {
		dualshock4_parse_report(sc, rd, size);
	} else if (((sc->quirks & DUALSHOCK4_CONTROLLER_BT) && rd[0] == 0x11 &&
			size == 78)) {
		/* CRC check */
		u8 bthdr = 0xA1;
		u32 crc;
		u32 report_crc;

		crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
		crc = ~crc32_le(crc, rd, DS4_INPUT_REPORT_0x11_SIZE-4);
		report_crc = get_unaligned_le32(&rd[DS4_INPUT_REPORT_0x11_SIZE-4]);
		if (crc != report_crc) {
			hid_dbg(sc->hdev, "DualShock 4 input report's CRC check failed, received crc 0x%0x != 0x%0x\n",
				report_crc, crc);
			return -EILSEQ;
		}

		dualshock4_parse_report(sc, rd, size);
	} else if ((sc->quirks & DUALSHOCK4_DONGLE) && rd[0] == 0x01 &&
			size == 64) {
		unsigned long flags;
		enum ds4_dongle_state dongle_state;

		/*
		 * In the case of a DS4 USB dongle, bit[2] of byte 31 indicates
		 * if a DS4 is actually connected (indicated by '0').
		 * For non-dongle, this bit is always 0 (connected).
		 */
		bool connected = (rd[31] & 0x04) ? false : true;

		spin_lock_irqsave(&sc->lock, flags);
		dongle_state = sc->ds4_dongle_state;
		spin_unlock_irqrestore(&sc->lock, flags);

		/*
		 * The dongle always sends input reports even when no
		 * DS4 is attached. When a DS4 is connected, we need to
		 * obtain calibration data before we can use it.
		 * The code below tracks dongle state and kicks of
		 * calibration when needed and only allows us to process
		 * input if a DS4 is actually connected.
		 */
		if (dongle_state == DONGLE_DISCONNECTED && connected) {
			hid_info(sc->hdev, "DualShock 4 USB dongle: controller connected\n");
			sony_set_leds(sc);

			spin_lock_irqsave(&sc->lock, flags);
			sc->ds4_dongle_state = DONGLE_CALIBRATING;
			spin_unlock_irqrestore(&sc->lock, flags);

			sony_schedule_work(sc, SONY_WORKER_HOTPLUG);

			/* Don't process the report since we don't have
			 * calibration data, but let hidraw have it anyway.
			 */
			return 0;
		} else if ((dongle_state == DONGLE_CONNECTED ||
			    dongle_state == DONGLE_DISABLED) && !connected) {
			hid_info(sc->hdev, "DualShock 4 USB dongle: controller disconnected\n");

			spin_lock_irqsave(&sc->lock, flags);
			sc->ds4_dongle_state = DONGLE_DISCONNECTED;
			spin_unlock_irqrestore(&sc->lock, flags);

			/* Return 0, so hidraw can get the report. */
			return 0;
		} else if (dongle_state == DONGLE_CALIBRATING ||
			   dongle_state == DONGLE_DISABLED ||
			   dongle_state == DONGLE_DISCONNECTED) {
			/* Return 0, so hidraw can get the report. */
			return 0;
		}

		dualshock4_parse_report(sc, rd, size);

	} else if ((sc->quirks & NSG_MRXU_REMOTE) && rd[0] == 0x02) {
		nsg_mrxu_parse_report(sc, rd, size);
		return 1;
	}

	if (sc->defer_initialization) {
		sc->defer_initialization = 0;
		sony_schedule_work(sc, SONY_WORKER_STATE);
	}

	return 0;
}

static int sony_mapping(struct hid_device *hdev, struct hid_input *hi,
			struct hid_field *field, struct hid_usage *usage,
			unsigned long **bit, int *max)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	if (sc->quirks & BUZZ_CONTROLLER) {
		unsigned int key = usage->hid & HID_USAGE;

		if ((usage->hid & HID_USAGE_PAGE) != HID_UP_BUTTON)
			return -1;

		switch (usage->collection_index) {
		case 1:
			if (key >= ARRAY_SIZE(buzz_keymap))
				return -1;

			key = buzz_keymap[key];
			if (!key)
				return -1;
			break;
		default:
			return -1;
		}

		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	}

	if (sc->quirks & PS3REMOTE)
		return ps3remote_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & NAVIGATION_CONTROLLER)
		return navigation_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & SIXAXIS_CONTROLLER)
		return sixaxis_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & DUALSHOCK4_CONTROLLER)
		return ds4_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & GH_GUITAR_CONTROLLER)
		return guitar_mapping(hdev, hi, field, usage, bit, max);

	/* Let hid-core decide for the others */
	return 0;
}

static int sony_register_touchpad(struct sony_sc *sc, int touch_count,
		int w, int h, int touch_major, int touch_minor, int orientation)
{
	size_t name_sz;
	char *name;
	int ret;

	sc->touchpad = devm_input_allocate_device(&sc->hdev->dev);
	if (!sc->touchpad)
		return -ENOMEM;

	input_set_drvdata(sc->touchpad, sc);
	sc->touchpad->dev.parent = &sc->hdev->dev;
	sc->touchpad->phys = sc->hdev->phys;
	sc->touchpad->uniq = sc->hdev->uniq;
	sc->touchpad->id.bustype = sc->hdev->bus;
	sc->touchpad->id.vendor = sc->hdev->vendor;
	sc->touchpad->id.product = sc->hdev->product;
	sc->touchpad->id.version = sc->hdev->version;

	/* Append a suffix to the controller name as there are various
	 * DS4 compatible non-Sony devices with different names.
	 */
	name_sz = strlen(sc->hdev->name) + sizeof(DS4_TOUCHPAD_SUFFIX);
	name = devm_kzalloc(&sc->hdev->dev, name_sz, GFP_KERNEL);
	if (!name)
		return -ENOMEM;
	snprintf(name, name_sz, "%s" DS4_TOUCHPAD_SUFFIX, sc->hdev->name);
	sc->touchpad->name = name;

	/* We map the button underneath the touchpad to BTN_LEFT. */
	__set_bit(EV_KEY, sc->touchpad->evbit);
	__set_bit(BTN_LEFT, sc->touchpad->keybit);
	__set_bit(INPUT_PROP_BUTTONPAD, sc->touchpad->propbit);

	input_set_abs_params(sc->touchpad, ABS_MT_POSITION_X, 0, w, 0, 0);
	input_set_abs_params(sc->touchpad, ABS_MT_POSITION_Y, 0, h, 0, 0);

	if (touch_major > 0) {
		input_set_abs_params(sc->touchpad, ABS_MT_TOUCH_MAJOR, 
			0, touch_major, 0, 0);
		if (touch_minor > 0)
			input_set_abs_params(sc->touchpad, ABS_MT_TOUCH_MINOR, 
				0, touch_minor, 0, 0);
		if (orientation > 0)
			input_set_abs_params(sc->touchpad, ABS_MT_ORIENTATION, 
				0, orientation, 0, 0);
	}

	if (sc->quirks & NSG_MRXU_REMOTE) {
		__set_bit(EV_REL, sc->touchpad->evbit);
	}

	ret = input_mt_init_slots(sc->touchpad, touch_count, INPUT_MT_POINTER);
	if (ret < 0)
		return ret;

	ret = input_register_device(sc->touchpad);
	if (ret < 0)
		return ret;

	return 0;
}

static int sony_register_sensors(struct sony_sc *sc)
{
	size_t name_sz;
	char *name;
	int ret;
	int range;

	sc->sensor_dev = devm_input_allocate_device(&sc->hdev->dev);
	if (!sc->sensor_dev)
		return -ENOMEM;

	input_set_drvdata(sc->sensor_dev, sc);
	sc->sensor_dev->dev.parent = &sc->hdev->dev;
	sc->sensor_dev->phys = sc->hdev->phys;
	sc->sensor_dev->uniq = sc->hdev->uniq;
	sc->sensor_dev->id.bustype = sc->hdev->bus;
	sc->sensor_dev->id.vendor = sc->hdev->vendor;
	sc->sensor_dev->id.product = sc->hdev->product;
	sc->sensor_dev->id.version = sc->hdev->version;

	/* Append a suffix to the controller name as there are various
	 * DS4 compatible non-Sony devices with different names.
	 */
	name_sz = strlen(sc->hdev->name) + sizeof(SENSOR_SUFFIX);
	name = devm_kzalloc(&sc->hdev->dev, name_sz, GFP_KERNEL);
	if (!name)
		return -ENOMEM;
	snprintf(name, name_sz, "%s" SENSOR_SUFFIX, sc->hdev->name);
	sc->sensor_dev->name = name;

	if (sc->quirks & SIXAXIS_CONTROLLER) {
		/* For the DS3 we only support the accelerometer, which works
		 * quite well even without calibration. The device also has
		 * a 1-axis gyro, but it is very difficult to manage from within
		 * the driver even to get data, the sensor is inaccurate and
		 * the behavior is very different between hardware revisions.
		 */
		input_set_abs_params(sc->sensor_dev, ABS_X, -512, 511, 4, 0);
		input_set_abs_params(sc->sensor_dev, ABS_Y, -512, 511, 4, 0);
		input_set_abs_params(sc->sensor_dev, ABS_Z, -512, 511, 4, 0);
		input_abs_set_res(sc->sensor_dev, ABS_X, SIXAXIS_ACC_RES_PER_G);
		input_abs_set_res(sc->sensor_dev, ABS_Y, SIXAXIS_ACC_RES_PER_G);
		input_abs_set_res(sc->sensor_dev, ABS_Z, SIXAXIS_ACC_RES_PER_G);
	} else if (sc->quirks & DUALSHOCK4_CONTROLLER) {
		range = DS4_ACC_RES_PER_G*4;
		input_set_abs_params(sc->sensor_dev, ABS_X, -range, range, 16, 0);
		input_set_abs_params(sc->sensor_dev, ABS_Y, -range, range, 16, 0);
		input_set_abs_params(sc->sensor_dev, ABS_Z, -range, range, 16, 0);
		input_abs_set_res(sc->sensor_dev, ABS_X, DS4_ACC_RES_PER_G);
		input_abs_set_res(sc->sensor_dev, ABS_Y, DS4_ACC_RES_PER_G);
		input_abs_set_res(sc->sensor_dev, ABS_Z, DS4_ACC_RES_PER_G);

		range = DS4_GYRO_RES_PER_DEG_S*2048;
		input_set_abs_params(sc->sensor_dev, ABS_RX, -range, range, 16, 0);
		input_set_abs_params(sc->sensor_dev, ABS_RY, -range, range, 16, 0);
		input_set_abs_params(sc->sensor_dev, ABS_RZ, -range, range, 16, 0);
		input_abs_set_res(sc->sensor_dev, ABS_RX, DS4_GYRO_RES_PER_DEG_S);
		input_abs_set_res(sc->sensor_dev, ABS_RY, DS4_GYRO_RES_PER_DEG_S);
		input_abs_set_res(sc->sensor_dev, ABS_RZ, DS4_GYRO_RES_PER_DEG_S);

		__set_bit(EV_MSC, sc->sensor_dev->evbit);
		__set_bit(MSC_TIMESTAMP, sc->sensor_dev->mscbit);
	}

	__set_bit(INPUT_PROP_ACCELEROMETER, sc->sensor_dev->propbit);

	ret = input_register_device(sc->sensor_dev);
	if (ret < 0)
		return ret;

	return 0;
}

/*
 * Sending HID_REQ_GET_REPORT changes the operation mode of the ps3 controller
 * to "operational".  Without this, the ps3 controller will not report any
 * events.
 */
static int sixaxis_set_operational_usb(struct hid_device *hdev)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);
	const int buf_size =
		max(SIXAXIS_REPORT_0xF2_SIZE, SIXAXIS_REPORT_0xF5_SIZE);
	u8 *buf;
	int ret;

	buf = kmalloc(buf_size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(hdev, 0xf2, buf, SIXAXIS_REPORT_0xF2_SIZE,
				 HID_FEATURE_REPORT, HID_REQ_GET_REPORT);
	if (ret < 0) {
		hid_err(hdev, "can't set operational mode: step 1\n");
		goto out;
	}

	/*
	 * Some compatible controllers like the Speedlink Strike FX and
	 * Gasia need another query plus an USB interrupt to get operational.
	 */
	ret = hid_hw_raw_request(hdev, 0xf5, buf, SIXAXIS_REPORT_0xF5_SIZE,
				 HID_FEATURE_REPORT, HID_REQ_GET_REPORT);
	if (ret < 0) {
		hid_err(hdev, "can't set operational mode: step 2\n");
		goto out;
	}

	/*
	 * But the USB interrupt would cause SHANWAN controllers to
	 * start rumbling non-stop, so skip step 3 for these controllers.
	 */
	if (sc->quirks & SHANWAN_GAMEPAD)
		goto out;

	ret = hid_hw_output_report(hdev, buf, 1);
	if (ret < 0) {
		hid_info(hdev, "can't set operational mode: step 3, ignoring\n");
		ret = 0;
	}

out:
	kfree(buf);

	return ret;
}

static int sixaxis_set_operational_bt(struct hid_device *hdev)
{
	static const u8 report[] = { 0xf4, 0x42, 0x03, 0x00, 0x00 };
	u8 *buf;
	int ret;

	buf = kmemdup(report, sizeof(report), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(hdev, buf[0], buf, sizeof(report),
				  HID_FEATURE_REPORT, HID_REQ_SET_REPORT);

	kfree(buf);

	return ret;
}

/*
 * Request DS4 calibration data for the motion sensors.
 * For Bluetooth this also affects the operating mode (see below).
 */
static int dualshock4_get_calibration_data(struct sony_sc *sc)
{
	u8 *buf;
	int ret;
	short gyro_pitch_bias, gyro_pitch_plus, gyro_pitch_minus;
	short gyro_yaw_bias, gyro_yaw_plus, gyro_yaw_minus;
	short gyro_roll_bias, gyro_roll_plus, gyro_roll_minus;
	short gyro_speed_plus, gyro_speed_minus;
	short acc_x_plus, acc_x_minus;
	short acc_y_plus, acc_y_minus;
	short acc_z_plus, acc_z_minus;
	int speed_2x;
	int range_2g;

	/* For Bluetooth we use a different request, which supports CRC.
	 * Note: in Bluetooth mode feature report 0x02 also changes the state
	 * of the controller, so that it sends input reports of type 0x11.
	 */
	if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE)) {
		int retries;

		buf = kmalloc(DS4_FEATURE_REPORT_0x02_SIZE, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		/* We should normally receive the feature report data we asked
		 * for, but hidraw applications such as Steam can issue feature
		 * reports as well. In particular for Dongle reconnects, Steam
		 * and this function are competing resulting in often receiving
		 * data for a different HID report, so retry a few times.
		 */
		for (retries = 0; retries < 3; retries++) {
			ret = hid_hw_raw_request(sc->hdev, 0x02, buf,
						 DS4_FEATURE_REPORT_0x02_SIZE,
						 HID_FEATURE_REPORT,
						 HID_REQ_GET_REPORT);
			if (ret < 0)
				goto err_stop;

			if (buf[0] != 0x02) {
				if (retries < 2) {
					hid_warn(sc->hdev, "Retrying DualShock 4 get calibration report (0x02) request\n");
					continue;
				} else {
					ret = -EILSEQ;
					goto err_stop;
				}
			} else {
				break;
			}
		}
	} else {
		u8 bthdr = 0xA3;
		u32 crc;
		u32 report_crc;
		int retries;

		buf = kmalloc(DS4_FEATURE_REPORT_0x05_SIZE, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		for (retries = 0; retries < 3; retries++) {
			ret = hid_hw_raw_request(sc->hdev, 0x05, buf,
						 DS4_FEATURE_REPORT_0x05_SIZE,
						 HID_FEATURE_REPORT,
						 HID_REQ_GET_REPORT);
			if (ret < 0)
				goto err_stop;

			/* CRC check */
			crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
			crc = ~crc32_le(crc, buf, DS4_FEATURE_REPORT_0x05_SIZE-4);
			report_crc = get_unaligned_le32(&buf[DS4_FEATURE_REPORT_0x05_SIZE-4]);
			if (crc != report_crc) {
				hid_warn(sc->hdev, "DualShock 4 calibration report's CRC check failed, received crc 0x%0x != 0x%0x\n",
					report_crc, crc);
				if (retries < 2) {
					hid_warn(sc->hdev, "Retrying DualShock 4 get calibration report request\n");
					continue;
				} else {
					ret = -EILSEQ;
					goto err_stop;
				}
			} else {
				break;
			}
		}
	}

	gyro_pitch_bias  = get_unaligned_le16(&buf[1]);
	gyro_yaw_bias    = get_unaligned_le16(&buf[3]);
	gyro_roll_bias   = get_unaligned_le16(&buf[5]);
	if (sc->quirks & DUALSHOCK4_CONTROLLER_USB) {
		gyro_pitch_plus  = get_unaligned_le16(&buf[7]);
		gyro_pitch_minus = get_unaligned_le16(&buf[9]);
		gyro_yaw_plus    = get_unaligned_le16(&buf[11]);
		gyro_yaw_minus   = get_unaligned_le16(&buf[13]);
		gyro_roll_plus   = get_unaligned_le16(&buf[15]);
		gyro_roll_minus  = get_unaligned_le16(&buf[17]);
	} else {
		/* BT + Dongle */
		gyro_pitch_plus  = get_unaligned_le16(&buf[7]);
		gyro_yaw_plus    = get_unaligned_le16(&buf[9]);
		gyro_roll_plus   = get_unaligned_le16(&buf[11]);
		gyro_pitch_minus = get_unaligned_le16(&buf[13]);
		gyro_yaw_minus   = get_unaligned_le16(&buf[15]);
		gyro_roll_minus  = get_unaligned_le16(&buf[17]);
	}
	gyro_speed_plus  = get_unaligned_le16(&buf[19]);
	gyro_speed_minus = get_unaligned_le16(&buf[21]);
	acc_x_plus       = get_unaligned_le16(&buf[23]);
	acc_x_minus      = get_unaligned_le16(&buf[25]);
	acc_y_plus       = get_unaligned_le16(&buf[27]);
	acc_y_minus      = get_unaligned_le16(&buf[29]);
	acc_z_plus       = get_unaligned_le16(&buf[31]);
	acc_z_minus      = get_unaligned_le16(&buf[33]);

	/* Set gyroscope calibration and normalization parameters.
	 * Data values will be normalized to 1/DS4_GYRO_RES_PER_DEG_S degree/s.
	 */
	speed_2x = (gyro_speed_plus + gyro_speed_minus);
	sc->ds4_calib_data[0].abs_code = ABS_RX;
	sc->ds4_calib_data[0].bias = gyro_pitch_bias;
	sc->ds4_calib_data[0].sens_numer = speed_2x*DS4_GYRO_RES_PER_DEG_S;
	sc->ds4_calib_data[0].sens_denom = gyro_pitch_plus - gyro_pitch_minus;

	sc->ds4_calib_data[1].abs_code = ABS_RY;
	sc->ds4_calib_data[1].bias = gyro_yaw_bias;
	sc->ds4_calib_data[1].sens_numer = speed_2x*DS4_GYRO_RES_PER_DEG_S;
	sc->ds4_calib_data[1].sens_denom = gyro_yaw_plus - gyro_yaw_minus;

	sc->ds4_calib_data[2].abs_code = ABS_RZ;
	sc->ds4_calib_data[2].bias = gyro_roll_bias;
	sc->ds4_calib_data[2].sens_numer = speed_2x*DS4_GYRO_RES_PER_DEG_S;
	sc->ds4_calib_data[2].sens_denom = gyro_roll_plus - gyro_roll_minus;

	/* Set accelerometer calibration and normalization parameters.
	 * Data values will be normalized to 1/DS4_ACC_RES_PER_G G.
	 */
	range_2g = acc_x_plus - acc_x_minus;
	sc->ds4_calib_data[3].abs_code = ABS_X;
	sc->ds4_calib_data[3].bias = acc_x_plus - range_2g / 2;
	sc->ds4_calib_data[3].sens_numer = 2*DS4_ACC_RES_PER_G;
	sc->ds4_calib_data[3].sens_denom = range_2g;

	range_2g = acc_y_plus - acc_y_minus;
	sc->ds4_calib_data[4].abs_code = ABS_Y;
	sc->ds4_calib_data[4].bias = acc_y_plus - range_2g / 2;
	sc->ds4_calib_data[4].sens_numer = 2*DS4_ACC_RES_PER_G;
	sc->ds4_calib_data[4].sens_denom = range_2g;

	range_2g = acc_z_plus - acc_z_minus;
	sc->ds4_calib_data[5].abs_code = ABS_Z;
	sc->ds4_calib_data[5].bias = acc_z_plus - range_2g / 2;
	sc->ds4_calib_data[5].sens_numer = 2*DS4_ACC_RES_PER_G;
	sc->ds4_calib_data[5].sens_denom = range_2g;

err_stop:
	kfree(buf);
	return ret;
}

static void dualshock4_calibration_work(struct work_struct *work)
{
	struct sony_sc *sc = container_of(work, struct sony_sc, hotplug_worker);
	unsigned long flags;
	enum ds4_dongle_state dongle_state;
	int ret;

	ret = dualshock4_get_calibration_data(sc);
	if (ret < 0) {
		/* This call is very unlikely to fail for the dongle. When it
		 * fails we are probably in a very bad state, so mark the
		 * dongle as disabled. We will re-enable the dongle if a new
		 * DS4 hotplug is detect from sony_raw_event as any issues
		 * are likely resolved then (the dongle is quite stupid).
		 */
		hid_err(sc->hdev, "DualShock 4 USB dongle: calibration failed, disabling device\n");
		dongle_state = DONGLE_DISABLED;
	} else {
		hid_info(sc->hdev, "DualShock 4 USB dongle: calibration completed\n");
		dongle_state = DONGLE_CONNECTED;
	}

	spin_lock_irqsave(&sc->lock, flags);
	sc->ds4_dongle_state = dongle_state;
	spin_unlock_irqrestore(&sc->lock, flags);
}

static int dualshock4_get_version_info(struct sony_sc *sc)
{
	u8 *buf;
	int ret;

	buf = kmalloc(DS4_FEATURE_REPORT_0xA3_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(sc->hdev, 0xA3, buf,
				 DS4_FEATURE_REPORT_0xA3_SIZE,
				 HID_FEATURE_REPORT,
				 HID_REQ_GET_REPORT);
	if (ret < 0) {
		kfree(buf);
		return ret;
	}

	sc->hw_version = get_unaligned_le16(&buf[35]);
	sc->fw_version = get_unaligned_le16(&buf[41]);

	kfree(buf);
	return 0;
}

static void sixaxis_set_leds_from_id(struct sony_sc *sc)
{
	static const u8 sixaxis_leds[10][4] = {
				{ 0x01, 0x00, 0x00, 0x00 },
				{ 0x00, 0x01, 0x00, 0x00 },
				{ 0x00, 0x00, 0x01, 0x00 },
				{ 0x00, 0x00, 0x00, 0x01 },
				{ 0x01, 0x00, 0x00, 0x01 },
				{ 0x00, 0x01, 0x00, 0x01 },
				{ 0x00, 0x00, 0x01, 0x01 },
				{ 0x01, 0x00, 0x01, 0x01 },
				{ 0x00, 0x01, 0x01, 0x01 },
				{ 0x01, 0x01, 0x01, 0x01 }
	};

	int id = sc->device_id;

	BUILD_BUG_ON(MAX_LEDS < ARRAY_SIZE(sixaxis_leds[0]));

	if (id < 0)
		return;

	id %= 10;
	memcpy(sc->led_state, sixaxis_leds[id], sizeof(sixaxis_leds[id]));
}

static void dualshock4_set_leds_from_id(struct sony_sc *sc)
{
	/* The first 4 color/index entries match what the PS4 assigns */
	static const u8 color_code[7][3] = {
			/* Blue   */	{ 0x00, 0x00, 0x40 },
			/* Red	  */	{ 0x40, 0x00, 0x00 },
			/* Green  */	{ 0x00, 0x40, 0x00 },
			/* Pink   */	{ 0x20, 0x00, 0x20 },
			/* Orange */	{ 0x02, 0x01, 0x00 },
			/* Teal   */	{ 0x00, 0x01, 0x01 },
			/* White  */	{ 0x01, 0x01, 0x01 }
	};

	int id = sc->device_id;

	BUILD_BUG_ON(MAX_LEDS < ARRAY_SIZE(color_code[0]));

	if (id < 0)
		return;

	id %= 7;
	memcpy(sc->led_state, color_code[id], sizeof(color_code[id]));
}

static void buzz_set_leds(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	struct list_head *report_list =
		&hdev->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report = list_entry(report_list->next,
		struct hid_report, list);
	s32 *value = report->field[0]->value;

	BUILD_BUG_ON(MAX_LEDS < 4);

	value[0] = 0x00;
	value[1] = sc->led_state[0] ? 0xff : 0x00;
	value[2] = sc->led_state[1] ? 0xff : 0x00;
	value[3] = sc->led_state[2] ? 0xff : 0x00;
	value[4] = sc->led_state[3] ? 0xff : 0x00;
	value[5] = 0x00;
	value[6] = 0x00;
	hid_hw_request(hdev, report, HID_REQ_SET_REPORT);
}

static void sony_set_leds(struct sony_sc *sc)
{
	if (!(sc->quirks & BUZZ_CONTROLLER))
		sony_schedule_work(sc, SONY_WORKER_STATE);
	else
		buzz_set_leds(sc);
}

static void sony_led_set_brightness(struct led_classdev *led,
				    enum led_brightness value)
{
	struct device *dev = led->dev->parent;
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *drv_data;

	int n;
	int force_update;

	drv_data = hid_get_drvdata(hdev);
	if (!drv_data) {
		hid_err(hdev, "No device data\n");
		return;
	}

	/*
	 * The Sixaxis on USB will override any LED settings sent to it
	 * and keep flashing all of the LEDs until the PS button is pressed.
	 * Updates, even if redundant, must be always be sent to the
	 * controller to avoid having to toggle the state of an LED just to
	 * stop the flashing later on.
	 */
	force_update = !!(drv_data->quirks & SIXAXIS_CONTROLLER_USB);

	for (n = 0; n < drv_data->led_count; n++) {
		if (led == drv_data->leds[n] && (force_update ||
			(value != drv_data->led_state[n] ||
			drv_data->led_delay_on[n] ||
			drv_data->led_delay_off[n]))) {

			drv_data->led_state[n] = value;

			/* Setting the brightness stops the blinking */
			drv_data->led_delay_on[n] = 0;
			drv_data->led_delay_off[n] = 0;

			sony_set_leds(drv_data);
			break;
		}
	}
}

static enum led_brightness sony_led_get_brightness(struct led_classdev *led)
{
	struct device *dev = led->dev->parent;
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *drv_data;

	int n;

	drv_data = hid_get_drvdata(hdev);
	if (!drv_data) {
		hid_err(hdev, "No device data\n");
		return LED_OFF;
	}

	for (n = 0; n < drv_data->led_count; n++) {
		if (led == drv_data->leds[n])
			return drv_data->led_state[n];
	}

	return LED_OFF;
}

static int sony_led_blink_set(struct led_classdev *led, unsigned long *delay_on,
				unsigned long *delay_off)
{
	struct device *dev = led->dev->parent;
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *drv_data = hid_get_drvdata(hdev);
	int n;
	u8 new_on, new_off;

	if (!drv_data) {
		hid_err(hdev, "No device data\n");
		return -EINVAL;
	}

	/* Max delay is 255 deciseconds or 2550 milliseconds */
	if (*delay_on > 2550)
		*delay_on = 2550;
	if (*delay_off > 2550)
		*delay_off = 2550;

	/* Blink at 1 Hz if both values are zero */
	if (!*delay_on && !*delay_off)
		*delay_on = *delay_off = 500;

	new_on = *delay_on / 10;
	new_off = *delay_off / 10;

	for (n = 0; n < drv_data->led_count; n++) {
		if (led == drv_data->leds[n])
			break;
	}

	/* This LED is not registered on this device */
	if (n >= drv_data->led_count)
		return -EINVAL;

	/* Don't schedule work if the values didn't change */
	if (new_on != drv_data->led_delay_on[n] ||
		new_off != drv_data->led_delay_off[n]) {
		drv_data->led_delay_on[n] = new_on;
		drv_data->led_delay_off[n] = new_off;
		sony_schedule_work(drv_data, SONY_WORKER_STATE);
	}

	return 0;
}

static int sony_leds_init(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	int n, ret = 0;
	int use_ds4_names;
	struct led_classdev *led;
	size_t name_sz;
	char *name;
	size_t name_len;
	const char *name_fmt;
	static const char * const ds4_name_str[] = { "red", "green", "blue",
						  "global" };
	u8 max_brightness[MAX_LEDS] = { [0 ... (MAX_LEDS - 1)] = 1 };
	u8 use_hw_blink[MAX_LEDS] = { 0 };

	BUG_ON(!(sc->quirks & SONY_LED_SUPPORT));

	if (sc->quirks & BUZZ_CONTROLLER) {
		sc->led_count = 4;
		use_ds4_names = 0;
		name_len = strlen("::buzz#");
		name_fmt = "%s::buzz%d";
		/* Validate expected report characteristics. */
		if (!hid_validate_values(hdev, HID_OUTPUT_REPORT, 0, 0, 7))
			return -ENODEV;
	} else if (sc->quirks & DUALSHOCK4_CONTROLLER) {
		dualshock4_set_leds_from_id(sc);
		sc->led_state[3] = 1;
		sc->led_count = 4;
		memset(max_brightness, 255, 3);
		use_hw_blink[3] = 1;
		use_ds4_names = 1;
		name_len = 0;
		name_fmt = "%s:%s";
	} else if (sc->quirks & MOTION_CONTROLLER) {
		sc->led_count = 3;
		memset(max_brightness, 255, 3);
		use_ds4_names = 1;
		name_len = 0;
		name_fmt = "%s:%s";
	} else if (sc->quirks & NAVIGATION_CONTROLLER) {
		static const u8 navigation_leds[4] = {0x01, 0x00, 0x00, 0x00};

		memcpy(sc->led_state, navigation_leds, sizeof(navigation_leds));
		sc->led_count = 1;
		memset(use_hw_blink, 1, 4);
		use_ds4_names = 0;
		name_len = strlen("::sony#");
		name_fmt = "%s::sony%d";
	} else {
		sixaxis_set_leds_from_id(sc);
		sc->led_count = 4;
		memset(use_hw_blink, 1, 4);
		use_ds4_names = 0;
		name_len = strlen("::sony#");
		name_fmt = "%s::sony%d";
	}

	/*
	 * Clear LEDs as we have no way of reading their initial state. This is
	 * only relevant if the driver is loaded after somebody actively set the
	 * LEDs to on
	 */
	sony_set_leds(sc);

	name_sz = strlen(dev_name(&hdev->dev)) + name_len + 1;

	for (n = 0; n < sc->led_count; n++) {

		if (use_ds4_names)
			name_sz = strlen(dev_name(&hdev->dev)) + strlen(ds4_name_str[n]) + 2;

		led = devm_kzalloc(&hdev->dev, sizeof(struct led_classdev) + name_sz, GFP_KERNEL);
		if (!led) {
			hid_err(hdev, "Couldn't allocate memory for LED %d\n", n);
			return -ENOMEM;
		}

		name = (void *)(&led[1]);
		if (use_ds4_names)
			snprintf(name, name_sz, name_fmt, dev_name(&hdev->dev),
			ds4_name_str[n]);
		else
			snprintf(name, name_sz, name_fmt, dev_name(&hdev->dev), n + 1);
		led->name = name;
		led->brightness = sc->led_state[n];
		led->max_brightness = max_brightness[n];
		led->flags = LED_CORE_SUSPENDRESUME;
		led->brightness_get = sony_led_get_brightness;
		led->brightness_set = sony_led_set_brightness;

		if (use_hw_blink[n])
			led->blink_set = sony_led_blink_set;

		sc->leds[n] = led;

		ret = devm_led_classdev_register(&hdev->dev, led);
		if (ret) {
			hid_err(hdev, "Failed to register LED %d\n", n);
			return ret;
		}
	}

	return 0;
}

static void sixaxis_send_output_report(struct sony_sc *sc)
{
	static const union sixaxis_output_report_01 default_report = {
		.buf = {
			0x01,
			0x01, 0xff, 0x00, 0xff, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00,
			0xff, 0x27, 0x10, 0x00, 0x32,
			0xff, 0x27, 0x10, 0x00, 0x32,
			0xff, 0x27, 0x10, 0x00, 0x32,
			0xff, 0x27, 0x10, 0x00, 0x32,
			0x00, 0x00, 0x00, 0x00, 0x00
		}
	};
	struct sixaxis_output_report *report =
		(struct sixaxis_output_report *)sc->output_report_dmabuf;
	int n;

	/* Initialize the report with default values */
	memcpy(report, &default_report, sizeof(struct sixaxis_output_report));

#ifdef CONFIG_SONY_FF
	report->rumble.right_motor_on = sc->right ? 1 : 0;
	report->rumble.left_motor_force = sc->left;
#endif

	report->leds_bitmap |= sc->led_state[0] << 1;
	report->leds_bitmap |= sc->led_state[1] << 2;
	report->leds_bitmap |= sc->led_state[2] << 3;
	report->leds_bitmap |= sc->led_state[3] << 4;

	/* Set flag for all leds off, required for 3rd party INTEC controller */
	if ((report->leds_bitmap & 0x1E) == 0)
		report->leds_bitmap |= 0x20;

	/*
	 * The LEDs in the report are indexed in reverse order to their
	 * corresponding light on the controller.
	 * Index 0 = LED 4, index 1 = LED 3, etc...
	 *
	 * In the case of both delay values being zero (blinking disabled) the
	 * default report values should be used or the controller LED will be
	 * always off.
	 */
	for (n = 0; n < 4; n++) {
		if (sc->led_delay_on[n] || sc->led_delay_off[n]) {
			report->led[3 - n].duty_off = sc->led_delay_off[n];
			report->led[3 - n].duty_on = sc->led_delay_on[n];
		}
	}

	/* SHANWAN controllers require output reports via intr channel */
	if (sc->quirks & SHANWAN_GAMEPAD)
		hid_hw_output_report(sc->hdev, (u8 *)report,
				sizeof(struct sixaxis_output_report));
	else
		hid_hw_raw_request(sc->hdev, report->report_id, (u8 *)report,
				sizeof(struct sixaxis_output_report),
				HID_OUTPUT_REPORT, HID_REQ_SET_REPORT);
}

static void dualshock4_send_output_report(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	u8 *buf = sc->output_report_dmabuf;
	int offset;

	/*
	 * NOTE: The lower 6 bits of buf[1] field of the Bluetooth report
	 * control the interval at which Dualshock 4 reports data:
	 * 0x00 - 1ms
	 * 0x01 - 1ms
	 * 0x02 - 2ms
	 * 0x3E - 62ms
	 * 0x3F - disabled
	 */
	if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE)) {
		memset(buf, 0, DS4_OUTPUT_REPORT_0x05_SIZE);
		buf[0] = 0x05;
		buf[1] = 0x07; /* blink + LEDs + motor */
		offset = 4;
	} else {
		memset(buf, 0, DS4_OUTPUT_REPORT_0x11_SIZE);
		buf[0] = 0x11;
		buf[1] = 0xC0 /* HID + CRC */ | sc->ds4_bt_poll_interval;
		buf[3] = 0x07; /* blink + LEDs + motor */
		offset = 6;
	}

#ifdef CONFIG_SONY_FF
	buf[offset++] = sc->right;
	buf[offset++] = sc->left;
#else
	offset += 2;
#endif

	/* LED 3 is the global control */
	if (sc->led_state[3]) {
		buf[offset++] = sc->led_state[0];
		buf[offset++] = sc->led_state[1];
		buf[offset++] = sc->led_state[2];
	} else {
		offset += 3;
	}

	/* If both delay values are zero the DualShock 4 disables blinking. */
	buf[offset++] = sc->led_delay_on[3];
	buf[offset++] = sc->led_delay_off[3];

	if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE))
		hid_hw_output_report(hdev, buf, DS4_OUTPUT_REPORT_0x05_SIZE);
	else {
		/* CRC generation */
		u8 bthdr = 0xA2;
		u32 crc;

		crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
		crc = ~crc32_le(crc, buf, DS4_OUTPUT_REPORT_0x11_SIZE-4);
		put_unaligned_le32(crc, &buf[74]);
		hid_hw_output_report(hdev, buf, DS4_OUTPUT_REPORT_0x11_SIZE);
	}
}

static void motion_send_output_report(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	struct motion_output_report_02 *report =
		(struct motion_output_report_02 *)sc->output_report_dmabuf;

	memset(report, 0, MOTION_REPORT_0x02_SIZE);

	report->type = 0x02; /* set leds */
	report->r = sc->led_state[0];
	report->g = sc->led_state[1];
	report->b = sc->led_state[2];

#ifdef CONFIG_SONY_FF
	report->rumble = max(sc->right, sc->left);
#endif

	hid_hw_output_report(hdev, (u8 *)report, MOTION_REPORT_0x02_SIZE);
}

static inline void sony_send_output_report(struct sony_sc *sc)
{
	if (sc->send_output_report)
		sc->send_output_report(sc);
}

static void sony_state_worker(struct work_struct *work)
{
	struct sony_sc *sc = container_of(work, struct sony_sc, state_worker);

	sc->send_output_report(sc);
}

static int sony_allocate_output_report(struct sony_sc *sc)
{
	if ((sc->quirks & SIXAXIS_CONTROLLER) ||
			(sc->quirks & NAVIGATION_CONTROLLER))
		sc->output_report_dmabuf =
			devm_kmalloc(&sc->hdev->dev,
				sizeof(union sixaxis_output_report_01),
				GFP_KERNEL);
	else if (sc->quirks & DUALSHOCK4_CONTROLLER_BT)
		sc->output_report_dmabuf = devm_kmalloc(&sc->hdev->dev,
						DS4_OUTPUT_REPORT_0x11_SIZE,
						GFP_KERNEL);
	else if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE))
		sc->output_report_dmabuf = devm_kmalloc(&sc->hdev->dev,
						DS4_OUTPUT_REPORT_0x05_SIZE,
						GFP_KERNEL);
	else if (sc->quirks & MOTION_CONTROLLER)
		sc->output_report_dmabuf = devm_kmalloc(&sc->hdev->dev,
						MOTION_REPORT_0x02_SIZE,
						GFP_KERNEL);
	else
		return 0;

	if (!sc->output_report_dmabuf)
		return -ENOMEM;

	return 0;
}

#ifdef CONFIG_SONY_FF
static int sony_play_effect(struct input_dev *dev, void *data,
			    struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct sony_sc *sc = hid_get_drvdata(hid);

	if (effect->type != FF_RUMBLE)
		return 0;

	sc->left = effect->u.rumble.strong_magnitude / 256;
	sc->right = effect->u.rumble.weak_magnitude / 256;

	sony_schedule_work(sc, SONY_WORKER_STATE);
	return 0;
}

static int sony_init_ff(struct sony_sc *sc)
{
	struct hid_input *hidinput;
	struct input_dev *input_dev;

	if (list_empty(&sc->hdev->inputs)) {
		hid_err(sc->hdev, "no inputs found\n");
		return -ENODEV;
	}
	hidinput = list_entry(sc->hdev->inputs.next, struct hid_input, list);
	input_dev = hidinput->input;

	input_set_capability(input_dev, EV_FF, FF_RUMBLE);
	return input_ff_create_memless(input_dev, NULL, sony_play_effect);
}

#else
static int sony_init_ff(struct sony_sc *sc)
{
	return 0;
}

#endif

static int sony_battery_get_property(struct power_supply *psy,
				     enum power_supply_property psp,
				     union power_supply_propval *val)
{
	struct sony_sc *sc = power_supply_get_drvdata(psy);
	unsigned long flags;
	int ret = 0;
	u8 battery_capacity;
	int battery_status;

	spin_lock_irqsave(&sc->lock, flags);
	battery_capacity = sc->battery_capacity;
	battery_status = sc->battery_status;
	spin_unlock_irqrestore(&sc->lock, flags);

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = 1;
		break;
	case POWER_SUPPLY_PROP_SCOPE:
		val->intval = POWER_SUPPLY_SCOPE_DEVICE;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = battery_capacity;
		break;
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = battery_status;
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int sony_battery_probe(struct sony_sc *sc, int append_dev_id)
{
	const char *battery_str_fmt = append_dev_id ?
		"sony_controller_battery_%pMR_%i" :
		"sony_controller_battery_%pMR";
	struct power_supply_config psy_cfg = { .drv_data = sc, };
	struct hid_device *hdev = sc->hdev;
	int ret;

	/*
	 * Set the default battery level to 100% to avoid low battery warnings
	 * if the battery is polled before the first device report is received.
	 */
	sc->battery_capacity = 100;

	sc->battery_desc.properties = sony_battery_props;
	sc->battery_desc.num_properties = ARRAY_SIZE(sony_battery_props);
	sc->battery_desc.get_property = sony_battery_get_property;
	sc->battery_desc.type = POWER_SUPPLY_TYPE_BATTERY;
	sc->battery_desc.use_for_apm = 0;
	sc->battery_desc.name = devm_kasprintf(&hdev->dev, GFP_KERNEL,
					  battery_str_fmt, sc->mac_address, sc->device_id);
	if (!sc->battery_desc.name)
		return -ENOMEM;

	sc->battery = devm_power_supply_register(&hdev->dev, &sc->battery_desc,
					    &psy_cfg);
	if (IS_ERR(sc->battery)) {
		ret = PTR_ERR(sc->battery);
		hid_err(hdev, "Unable to register battery device\n");
		return ret;
	}

	power_supply_powers(sc->battery, &hdev->dev);
	return 0;
}

/*
 * If a controller is plugged in via USB while already connected via Bluetooth
 * it will show up as two devices. A global list of connected controllers and
 * their MAC addresses is maintained to ensure that a device is only connected
 * once.
 *
 * Some USB-only devices masquerade as Sixaxis controllers and all have the
 * same dummy Bluetooth address, so a comparison of the connection type is
 * required.  Devices are only rejected in the case where two devices have
 * matching Bluetooth addresses on different bus types.
 */
static inline int sony_compare_connection_type(struct sony_sc *sc0,
						struct sony_sc *sc1)
{
	const int sc0_not_bt = !(sc0->quirks & SONY_BT_DEVICE);
	const int sc1_not_bt = !(sc1->quirks & SONY_BT_DEVICE);

	return sc0_not_bt == sc1_not_bt;
}

static int sony_check_add_dev_list(struct sony_sc *sc)
{
	struct sony_sc *entry;
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sony_dev_list_lock, flags);

	list_for_each_entry(entry, &sony_device_list, list_node) {
		ret = memcmp(sc->mac_address, entry->mac_address,
				sizeof(sc->mac_address));
		if (!ret) {
			if (sony_compare_connection_type(sc, entry)) {
				ret = 1;
			} else {
				ret = -EEXIST;
				hid_info(sc->hdev,
				"controller with MAC address %pMR already connected\n",
				sc->mac_address);
			}
			goto unlock;
		}
	}

	ret = 0;
	list_add(&(sc->list_node), &sony_device_list);

unlock:
	spin_unlock_irqrestore(&sony_dev_list_lock, flags);
	return ret;
}

static void sony_remove_dev_list(struct sony_sc *sc)
{
	unsigned long flags;

	if (sc->list_node.next) {
		spin_lock_irqsave(&sony_dev_list_lock, flags);
		list_del(&(sc->list_node));
		spin_unlock_irqrestore(&sony_dev_list_lock, flags);
	}
}

static int sony_get_bt_devaddr(struct sony_sc *sc)
{
	int ret;

	/* HIDP stores the device MAC address as a string in the uniq field. */
	ret = strlen(sc->hdev->uniq);
	if (ret != 17)
		return -EINVAL;

	ret = sscanf(sc->hdev->uniq,
		"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		&sc->mac_address[5], &sc->mac_address[4], &sc->mac_address[3],
		&sc->mac_address[2], &sc->mac_address[1], &sc->mac_address[0]);

	if (ret != 6)
		return -EINVAL;

	return 0;
}

static int sony_check_add(struct sony_sc *sc)
{
	u8 *buf = NULL;
	int n, ret;

	if ((sc->quirks & DUALSHOCK4_CONTROLLER_BT) ||
	    (sc->quirks & MOTION_CONTROLLER_BT) ||
	    (sc->quirks & NAVIGATION_CONTROLLER_BT) ||
	    (sc->quirks & SIXAXIS_CONTROLLER_BT)) {
		/*
		 * sony_get_bt_devaddr() attempts to parse the Bluetooth MAC
		 * address from the uniq string where HIDP stores it.
		 * As uniq cannot be guaranteed to be a MAC address in all cases
		 * a failure of this function should not prevent the connection.
		 */
		if (sony_get_bt_devaddr(sc) < 0) {
			hid_warn(sc->hdev, "UNIQ does not contain a MAC address; duplicate check skipped\n");
			return 0;
		}
	} else if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE)) {
		buf = kmalloc(DS4_FEATURE_REPORT_0x81_SIZE, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		/*
		 * The MAC address of a DS4 controller connected via USB can be
		 * retrieved with feature report 0x81. The address begins at
		 * offset 1.
		 */
		ret = hid_hw_raw_request(sc->hdev, 0x81, buf,
				DS4_FEATURE_REPORT_0x81_SIZE, HID_FEATURE_REPORT,
				HID_REQ_GET_REPORT);

		if (ret != DS4_FEATURE_REPORT_0x81_SIZE) {
			hid_err(sc->hdev, "failed to retrieve feature report 0x81 with the DualShock 4 MAC address\n");
			ret = ret < 0 ? ret : -EINVAL;
			goto out_free;
		}

		memcpy(sc->mac_address, &buf[1], sizeof(sc->mac_address));

		snprintf(sc->hdev->uniq, sizeof(sc->hdev->uniq),
			 "%pMR", sc->mac_address);
	} else if ((sc->quirks & SIXAXIS_CONTROLLER_USB) ||
			(sc->quirks & NAVIGATION_CONTROLLER_USB)) {
		buf = kmalloc(SIXAXIS_REPORT_0xF2_SIZE, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		/*
		 * The MAC address of a Sixaxis controller connected via USB can
		 * be retrieved with feature report 0xf2. The address begins at
		 * offset 4.
		 */
		ret = hid_hw_raw_request(sc->hdev, 0xf2, buf,
				SIXAXIS_REPORT_0xF2_SIZE, HID_FEATURE_REPORT,
				HID_REQ_GET_REPORT);

		if (ret != SIXAXIS_REPORT_0xF2_SIZE) {
			hid_err(sc->hdev, "failed to retrieve feature report 0xf2 with the Sixaxis MAC address\n");
			ret = ret < 0 ? ret : -EINVAL;
			goto out_free;
		}

		/*
		 * The Sixaxis device MAC in the report is big-endian and must
		 * be byte-swapped.
		 */
		for (n = 0; n < 6; n++)
			sc->mac_address[5-n] = buf[4+n];

		snprintf(sc->hdev->uniq, sizeof(sc->hdev->uniq),
			 "%pMR", sc->mac_address);
	} else {
		return 0;
	}

	ret = sony_check_add_dev_list(sc);

out_free:

	kfree(buf);

	return ret;
}

static int sony_set_device_id(struct sony_sc *sc)
{
	int ret;

	/*
	 * Only DualShock 4 or Sixaxis controllers get an id.
	 * All others are set to -1.
	 */
	if ((sc->quirks & SIXAXIS_CONTROLLER) ||
	    (sc->quirks & DUALSHOCK4_CONTROLLER)) {
		ret = ida_simple_get(&sony_device_id_allocator, 0, 0,
					GFP_KERNEL);
		if (ret < 0) {
			sc->device_id = -1;
			return ret;
		}
		sc->device_id = ret;
	} else {
		sc->device_id = -1;
	}

	return 0;
}

static void sony_release_device_id(struct sony_sc *sc)
{
	if (sc->device_id >= 0) {
		ida_simple_remove(&sony_device_id_allocator, sc->device_id);
		sc->device_id = -1;
	}
}

static inline void sony_init_output_report(struct sony_sc *sc,
				void (*send_output_report)(struct sony_sc *))
{
	sc->send_output_report = send_output_report;

	if (!sc->state_worker_initialized)
		INIT_WORK(&sc->state_worker, sony_state_worker);

	sc->state_worker_initialized = 1;
}

static inline void sony_cancel_work_sync(struct sony_sc *sc)
{
	unsigned long flags;

	if (sc->hotplug_worker_initialized)
		cancel_work_sync(&sc->hotplug_worker);
	if (sc->state_worker_initialized) {
		spin_lock_irqsave(&sc->lock, flags);
		sc->state_worker_initialized = 0;
		spin_unlock_irqrestore(&sc->lock, flags);
		cancel_work_sync(&sc->state_worker);
	}
}

static int sony_input_configured(struct hid_device *hdev,
					struct hid_input *hidinput)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);
	int append_dev_id;
	int ret;

	ret = sony_set_device_id(sc);
	if (ret < 0) {
		hid_err(hdev, "failed to allocate the device id\n");
		goto err_stop;
	}

	ret = append_dev_id = sony_check_add(sc);
	if (ret < 0)
		goto err_stop;

	ret = sony_allocate_output_report(sc);
	if (ret < 0) {
		hid_err(hdev, "failed to allocate the output report buffer\n");
		goto err_stop;
	}

	if (sc->quirks & NAVIGATION_CONTROLLER_USB) {
		/*
		 * The Sony Sixaxis does not handle HID Output Reports on the
		 * Interrupt EP like it could, so we need to force HID Output
		 * Reports to use HID_REQ_SET_REPORT on the Control EP.
		 *
		 * There is also another issue about HID Output Reports via USB,
		 * the Sixaxis does not want the report_id as part of the data
		 * packet, so we have to discard buf[0] when sending the actual
		 * control message, even for numbered reports, humpf!
		 *
		 * Additionally, the Sixaxis on USB isn't properly initialized
		 * until the PS logo button is pressed and as such won't retain
		 * any state set by an output report, so the initial
		 * configuration report is deferred until the first input
		 * report arrives.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;
		hdev->quirks |= HID_QUIRK_SKIP_OUTPUT_REPORT_ID;
		sc->defer_initialization = 1;

		ret = sixaxis_set_operational_usb(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & NAVIGATION_CONTROLLER_BT) {
		/*
		 * The Navigation controller wants output reports sent on the ctrl
		 * endpoint when connected via Bluetooth.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;

		ret = sixaxis_set_operational_bt(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & SIXAXIS_CONTROLLER_USB) {
		/*
		 * The Sony Sixaxis does not handle HID Output Reports on the
		 * Interrupt EP and the device only becomes active when the
		 * PS button is pressed. See comment for Navigation controller
		 * above for more details.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;
		hdev->quirks |= HID_QUIRK_SKIP_OUTPUT_REPORT_ID;
		sc->defer_initialization = 1;

		ret = sixaxis_set_operational_usb(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		ret = sony_register_sensors(sc);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize motion sensors: %d\n", ret);
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & SIXAXIS_CONTROLLER_BT) {
		/*
		 * The Sixaxis wants output reports sent on the ctrl endpoint
		 * when connected via Bluetooth.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;

		ret = sixaxis_set_operational_bt(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		ret = sony_register_sensors(sc);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize motion sensors: %d\n", ret);
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & DUALSHOCK4_CONTROLLER) {
		ret = dualshock4_get_calibration_data(sc);
		if (ret < 0) {
			hid_err(hdev, "Failed to get calibration data from Dualshock 4\n");
			goto err_stop;
		}

		ret = dualshock4_get_version_info(sc);
		if (ret < 0) {
			hid_err(sc->hdev, "Failed to get version data from Dualshock 4\n");
			goto err_stop;
		}

		ret = device_create_file(&sc->hdev->dev, &dev_attr_firmware_version);
		if (ret) {
			hid_err(sc->hdev, "can't create sysfs firmware_version attribute err: %d\n", ret);
			goto err_stop;
		}
		sc->fw_version_created = true;

		ret = device_create_file(&sc->hdev->dev, &dev_attr_hardware_version);
		if (ret) {
			hid_err(sc->hdev, "can't create sysfs hardware_version attribute err: %d\n", ret);
			goto err_stop;
		}
		sc->hw_version_created = true;

		/*
		 * The Dualshock 4 touchpad supports 2 touches and has a
		 * resolution of 1920x942 (44.86 dots/mm).
		 */
		ret = sony_register_touchpad(sc, 2, 1920, 942, 0, 0, 0);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize multi-touch slots: %d\n",
			ret);
			goto err_stop;
		}

		ret = sony_register_sensors(sc);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize motion sensors: %d\n", ret);
			goto err_stop;
		}

		if (sc->quirks & DUALSHOCK4_CONTROLLER_BT) {
			sc->ds4_bt_poll_interval = DS4_BT_DEFAULT_POLL_INTERVAL_MS;
			ret = device_create_file(&sc->hdev->dev, &dev_attr_bt_poll_interval);
			if (ret)
				hid_warn(sc->hdev,
				 "can't create sysfs bt_poll_interval attribute err: %d\n",
				 ret);
		}

		if (sc->quirks & DUALSHOCK4_DONGLE) {
			INIT_WORK(&sc->hotplug_worker, dualshock4_calibration_work);
			sc->hotplug_worker_initialized = 1;
			sc->ds4_dongle_state = DONGLE_DISCONNECTED;
		}

		sony_init_output_report(sc, dualshock4_send_output_report);
	} else if (sc->quirks & NSG_MRXU_REMOTE) {
		/*
		 * The NSG-MRxU touchpad supports 2 touches and has a
		 * resolution of 1667x1868
		 */
		ret = sony_register_touchpad(sc, 2,
			NSG_MRXU_MAX_X, NSG_MRXU_MAX_Y, 15, 15, 1);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize multi-touch slots: %d\n",
			ret);
			goto err_stop;
		}

	} else if (sc->quirks & MOTION_CONTROLLER) {
		sony_init_output_report(sc, motion_send_output_report);
	} else {
		ret = 0;
	}

	if (sc->quirks & SONY_LED_SUPPORT) {
		ret = sony_leds_init(sc);
		if (ret < 0)
			goto err_stop;
	}

	if (sc->quirks & SONY_BATTERY_SUPPORT) {
		ret = sony_battery_probe(sc, append_dev_id);
		if (ret < 0)
			goto err_stop;

		/* Open the device to receive reports with battery info */
		ret = hid_hw_open(hdev);
		if (ret < 0) {
			hid_err(hdev, "hw open failed\n");
			goto err_stop;
		}
	}

	if (sc->quirks & SONY_FF_SUPPORT) {
		ret = sony_init_ff(sc);
		if (ret < 0)
			goto err_close;
	}

	return 0;
err_close:
	hid_hw_close(hdev);
err_stop:
	/* Piggy back on the default ds4_bt_ poll_interval to determine
	 * if we need to remove the file as we don't know for sure if we
	 * executed that logic.
	 */
	if (sc->ds4_bt_poll_interval)
		device_remove_file(&sc->hdev->dev, &dev_attr_bt_poll_interval);
	if (sc->fw_version_created)
		device_remove_file(&sc->hdev->dev, &dev_attr_firmware_version);
	if (sc->hw_version_created)
		device_remove_file(&sc->hdev->dev, &dev_attr_hardware_version);
	sony_cancel_work_sync(sc);
	sony_remove_dev_list(sc);
	sony_release_device_id(sc);
	return ret;
}

static int sony_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;
	unsigned long quirks = id->driver_data;
	struct sony_sc *sc;
	unsigned int connect_mask = HID_CONNECT_DEFAULT;

	if (!strcmp(hdev->name, "FutureMax Dance Mat"))
		quirks |= FUTUREMAX_DANCE_MAT;

	if (!strcmp(hdev->name, "SHANWAN PS3 GamePad"))
		quirks |= SHANWAN_GAMEPAD;

	sc = devm_kzalloc(&hdev->dev, sizeof(*sc), GFP_KERNEL);
	if (sc == NULL) {
		hid_err(hdev, "can't alloc sony descriptor\n");
		return -ENOMEM;
	}

	spin_lock_init(&sc->lock);

	sc->quirks = quirks;
	hid_set_drvdata(hdev, sc);
	sc->hdev = hdev;

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		return ret;
	}

	if (sc->quirks & VAIO_RDESC_CONSTANT)
		connect_mask |= HID_CONNECT_HIDDEV_FORCE;
	else if (sc->quirks & SIXAXIS_CONTROLLER)
		connect_mask |= HID_CONNECT_HIDDEV_FORCE;

	/* Patch the hw version on DS3/4 compatible devices, so applications can
	 * distinguish between the default HID mappings and the mappings defined
	 * by the Linux game controller spec. This is important for the SDL2
	 * library, which has a game controller database, which uses device ids
	 * in combination with version as a key.
	 */
	if (sc->quirks & (SIXAXIS_CONTROLLER | DUALSHOCK4_CONTROLLER))
		hdev->version |= 0x8000;

	ret = hid_hw_start(hdev, connect_mask);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		return ret;
	}

	/* sony_input_configured can fail, but this doesn't result
	 * in hid_hw_start failures (intended). Check whether
	 * the HID layer claimed the device else fail.
	 * We don't know the actual reason for the failure, most
	 * likely it is due to EEXIST in case of double connection
	 * of USB and Bluetooth, but could have been due to ENOMEM
	 * or other reasons as well.
	 */
	if (!(hdev->claimed & HID_CLAIMED_INPUT)) {
		hid_err(hdev, "failed to claim input\n");
		hid_hw_stop(hdev);
		return -ENODEV;
	}

	if (sc->quirks & GHL_GUITAR_PS3WIIU) {
		timer_setup(&sc->ghl_poke_timer, ghl_magic_poke, 0);
		mod_timer(&sc->ghl_poke_timer,
			  jiffies + GHL_GUITAR_POKE_INTERVAL*HZ);
	}

	return ret;
}

static void sony_remove(struct hid_device *hdev)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	if (sc->quirks & GHL_GUITAR_PS3WIIU)
		del_timer_sync(&sc->ghl_poke_timer);

	hid_hw_close(hdev);

	if (sc->quirks & DUALSHOCK4_CONTROLLER_BT)
		device_remove_file(&sc->hdev->dev, &dev_attr_bt_poll_interval);

	if (sc->fw_version_created)
		device_remove_file(&sc->hdev->dev, &dev_attr_firmware_version);

	if (sc->hw_version_created)
		device_remove_file(&sc->hdev->dev, &dev_attr_hardware_version);

	sony_cancel_work_sync(sc);

	sony_remove_dev_list(sc);

	sony_release_device_id(sc);

	hid_hw_stop(hdev);
}

#ifdef CONFIG_PM

static int sony_suspend(struct hid_device *hdev, pm_message_t message)
{
#ifdef CONFIG_SONY_FF

	/* On suspend stop any running force-feedback events */
	if (SONY_FF_SUPPORT) {
		struct sony_sc *sc = hid_get_drvdata(hdev);

		sc->left = sc->right = 0;
		sony_send_output_report(sc);
	}

#endif
	return 0;
}

static int sony_resume(struct hid_device *hdev)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	/*
	 * The Sixaxis and navigation controllers on USB need to be
	 * reinitialized on resume or they won't behave properly.
	 */
	if ((sc->quirks & SIXAXIS_CONTROLLER_USB) ||
		(sc->quirks & NAVIGATION_CONTROLLER_USB)) {
		sixaxis_set_operational_usb(sc->hdev);
		sc->defer_initialization = 1;
	}

	return 0;
}

#endif

static const struct hid_device_id sony_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS3_CONTROLLER),
		.driver_data = SIXAXIS_CONTROLLER_USB },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_NAVIGATION_CONTROLLER),
		.driver_data = NAVIGATION_CONTROLLER_USB },
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_NAVIGATION_CONTROLLER),
		.driver_data = NAVIGATION_CONTROLLER_BT },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_MOTION_CONTROLLER),
		.driver_data = MOTION_CONTROLLER_USB },
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_MOTION_CONTROLLER),
		.driver_data = MOTION_CONTROLLER_BT },
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS3_CONTROLLER),
		.driver_data = SIXAXIS_CONTROLLER_BT },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_VAIO_VGX_MOUSE),
		.driver_data = VAIO_RDESC_CONSTANT },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_VAIO_VGP_MOUSE),
		.driver_data = VAIO_RDESC_CONSTANT },
	/*
	 * Wired Buzz Controller. Reported as Sony Hub from its USB ID and as
	 * Logitech joystick from the device descriptor.
	 */
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_BUZZ_CONTROLLER),
		.driver_data = BUZZ_CONTROLLER },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_WIRELESS_BUZZ_CONTROLLER),
		.driver_data = BUZZ_CONTROLLER },
	/* PS3 BD Remote Control */
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS3_BDREMOTE),
		.driver_data = PS3REMOTE },
	/* Logitech Harmony Adapter for PS3 */
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_LOGITECH, USB_DEVICE_ID_LOGITECH_HARMONY_PS3),
		.driver_data = PS3REMOTE },
	/* SMK-Link PS3 BD Remote Control */
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SMK, USB_DEVICE_ID_SMK_PS3_BDREMOTE),
		.driver_data = PS3REMOTE },
	/* Sony Dualshock 4 controllers for PS4 */
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS4_CONTROLLER),
		.driver_data = DUALSHOCK4_CONTROLLER_USB },
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS4_CONTROLLER),
		.driver_data = DUALSHOCK4_CONTROLLER_BT },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS4_CONTROLLER_2),
		.driver_data = DUALSHOCK4_CONTROLLER_USB },
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS4_CONTROLLER_2),
		.driver_data = DUALSHOCK4_CONTROLLER_BT },
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_PS4_CONTROLLER_DONGLE),
		.driver_data = DUALSHOCK4_DONGLE },
	/* Nyko Core Controller for PS3 */
	{ HID_USB_DEVICE(USB_VENDOR_ID_SINO_LITE, USB_DEVICE_ID_SINO_LITE_CONTROLLER),
		.driver_data = SIXAXIS_CONTROLLER_USB | SINO_LITE_CONTROLLER },
	/* SMK-Link NSG-MR5U Remote Control */
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SMK, USB_DEVICE_ID_SMK_NSG_MR5U_REMOTE),
		.driver_data = NSG_MR5U_REMOTE_BT },
	/* SMK-Link NSG-MR7U Remote Control */
	{ HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_SMK, USB_DEVICE_ID_SMK_NSG_MR7U_REMOTE),
		.driver_data = NSG_MR7U_REMOTE_BT },
	/* Guitar Hero Live PS3 and Wii U guitar dongles */
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY_RHYTHM, USB_DEVICE_ID_SONY_PS3WIIU_GHLIVE_DONGLE),
		.driver_data = GHL_GUITAR_PS3WIIU | GH_GUITAR_CONTROLLER },
	/* Guitar Hero PC Guitar Dongle */
	{ HID_USB_DEVICE(USB_VENDOR_ID_ACTIVISION, USB_DEVICE_ID_ACTIVISION_GUITAR_DONGLE),
		.driver_data = GH_GUITAR_CONTROLLER },
	/* Guitar Hero PS3 World Tour Guitar Dongle */
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY_RHYTHM, USB_DEVICE_ID_SONY_PS3_GUITAR_DONGLE),
		.driver_data = GH_GUITAR_CONTROLLER },
	{ }
};
MODULE_DEVICE_TABLE(hid, sony_devices);

static struct hid_driver sony_driver = {
	.name             = "sony",
	.id_table         = sony_devices,
	.input_mapping    = sony_mapping,
	.input_configured = sony_input_configured,
	.probe            = sony_probe,
	.remove           = sony_remove,
	.report_fixup     = sony_report_fixup,
	.raw_event        = sony_raw_event,

#ifdef CONFIG_PM
	.suspend          = sony_suspend,
	.resume	          = sony_resume,
	.reset_resume     = sony_resume,
#endif
};

static int __init sony_init(void)
{
	dbg_hid("Sony:%s\n", __func__);

	return hid_register_driver(&sony_driver);
}

static void __exit sony_exit(void)
{
	dbg_hid("Sony:%s\n", __func__);

	hid_unregister_driver(&sony_driver);
	ida_destroy(&sony_device_id_allocator);
}
module_init(sony_init);
module_exit(sony_exit);

MODULE_LICENSE("GPL");

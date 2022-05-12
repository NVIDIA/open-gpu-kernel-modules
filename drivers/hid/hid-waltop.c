// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  HID driver for Waltop devices not fully compliant with HID standard
 *
 *  Copyright (c) 2010 Nikolai Kondrashov
 */

/*
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>

#include "hid-ids.h"

/*
 * There exists an official driver on the manufacturer's website, which
 * wasn't submitted to the kernel, for some reason. The official driver
 * doesn't seem to support extra features of some tablets, like wheels.
 *
 * It shows that the feature report ID 2 could be used to control any waltop
 * tablet input mode, switching it between "default", "tablet" and "ink".
 *
 * This driver only uses "default" mode for all the supported tablets. This
 * mode tries to be HID-compatible (not very successfully), but cripples the
 * resolution of some tablets.
 *
 * The "tablet" mode uses some proprietary, yet decipherable protocol, which
 * represents the correct resolution, but is possibly HID-incompatible (i.e.
 * indescribable by a report descriptor).
 *
 * The purpose of the "ink" mode is unknown.
 *
 * The feature reports needed for switching to each mode are these:
 *
 * 02 16 00     default
 * 02 16 01     tablet
 * 02 16 02     ink
 */

/* Size of the original report descriptor of Slim Tablet 5.8 inch */
#define SLIM_TABLET_5_8_INCH_RDESC_ORIG_SIZE	222

/* Fixed Slim Tablet 5.8 inch descriptor */
static __u8 slim_tablet_5_8_inch_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x04,         /*          Report Size (4),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x80,               /*          Input,                      */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x75, 0x01,         /*          Report Size (1),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x34,               /*          Physical Minimum (0),       */
	0x09, 0x30,         /*          Usage (X),                  */
	0x46, 0x88, 0x13,   /*          Physical Maximum (5000),    */
	0x26, 0x10, 0x27,   /*          Logical Maximum (10000),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x46, 0xB8, 0x0B,   /*          Physical Maximum (3000),    */
	0x26, 0x70, 0x17,   /*          Logical Maximum (6000),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xC0,               /*      End Collection,                 */
	0xC0                /*  End Collection                      */
};

/* Size of the original report descriptor of Slim Tablet 12.1 inch */
#define SLIM_TABLET_12_1_INCH_RDESC_ORIG_SIZE	269

/* Fixed Slim Tablet 12.1 inch descriptor */
static __u8 slim_tablet_12_1_inch_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x04,         /*          Report Size (4),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x80,               /*          Input,                      */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x75, 0x01,         /*          Report Size (1),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x34,               /*          Physical Minimum (0),       */
	0x09, 0x30,         /*          Usage (X),                  */
	0x46, 0x10, 0x27,   /*          Physical Maximum (10000),   */
	0x26, 0x20, 0x4E,   /*          Logical Maximum (20000),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x46, 0x6A, 0x18,   /*          Physical Maximum (6250),    */
	0x26, 0xD4, 0x30,   /*          Logical Maximum (12500),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xC0,               /*      End Collection,                 */
	0xC0                /*  End Collection                      */
};

/* Size of the original report descriptor of Q Pad */
#define Q_PAD_RDESC_ORIG_SIZE	241

/* Fixed Q Pad descriptor */
static __u8 q_pad_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x04,         /*          Report Size (4),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x80,               /*          Input,                      */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x75, 0x01,         /*          Report Size (1),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x34,               /*          Physical Minimum (0),       */
	0x09, 0x30,         /*          Usage (X),                  */
	0x46, 0x70, 0x17,   /*          Physical Maximum (6000),    */
	0x26, 0x00, 0x30,   /*          Logical Maximum (12288),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x46, 0x94, 0x11,   /*          Physical Maximum (4500),    */
	0x26, 0x00, 0x24,   /*          Logical Maximum (9216),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xC0,               /*      End Collection,                 */
	0xC0                /*  End Collection                      */
};

/* Size of the original report descriptor of tablet with PID 0038 */
#define PID_0038_RDESC_ORIG_SIZE	241

/*
 * Fixed report descriptor for tablet with PID 0038.
 */
static __u8 pid_0038_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x04,         /*          Report Size (4),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x80,               /*          Input,                      */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x75, 0x01,         /*          Report Size (1),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x34,               /*          Physical Minimum (0),       */
	0x09, 0x30,         /*          Usage (X),                  */
	0x46, 0x2E, 0x22,   /*          Physical Maximum (8750),    */
	0x26, 0x00, 0x46,   /*          Logical Maximum (17920),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x46, 0x82, 0x14,   /*          Physical Maximum (5250),    */
	0x26, 0x00, 0x2A,   /*          Logical Maximum (10752),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xC0,               /*      End Collection,                 */
	0xC0                /*  End Collection                      */
};

/* Size of the original report descriptor of Media Tablet 10.6 inch */
#define MEDIA_TABLET_10_6_INCH_RDESC_ORIG_SIZE	300

/* Fixed Media Tablet 10.6 inch descriptor */
static __u8 media_tablet_10_6_inch_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x04,         /*          Report Size (4),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x80,               /*          Input,                      */
	0x75, 0x01,         /*          Report Size (1),            */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x34,               /*          Physical Minimum (0),       */
	0x09, 0x30,         /*          Usage (X),                  */
	0x46, 0x28, 0x23,   /*          Physical Maximum (9000),    */
	0x26, 0x50, 0x46,   /*          Logical Maximum (18000),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x46, 0x7C, 0x15,   /*          Physical Maximum (5500),    */
	0x26, 0xF8, 0x2A,   /*          Logical Maximum (11000),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xC0,               /*      End Collection,                 */
	0xC0,               /*  End Collection,                     */
	0x05, 0x01,         /*  Usage Page (Desktop),               */
	0x09, 0x02,         /*  Usage (Mouse),                      */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x01,         /*      Report ID (1),                  */
	0x09, 0x01,         /*      Usage (Pointer),                */
	0xA0,               /*      Collection (Physical),          */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x95, 0x02,         /*          Report Count (2),           */
	0x15, 0xFF,         /*          Logical Minimum (-1),       */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x09, 0x38,         /*          Usage (Wheel),              */
	0x0B, 0x38, 0x02,   /*          Usage (Consumer AC Pan),    */
		0x0C, 0x00,
	0x81, 0x06,         /*          Input (Variable, Relative), */
	0x95, 0x02,         /*          Report Count (2),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0xC0,               /*      End Collection,                 */
	0xC0,               /*  End Collection,                     */
	0x05, 0x0C,         /*  Usage Page (Consumer),              */
	0x09, 0x01,         /*  Usage (Consumer Control),           */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x0D,         /*      Report ID (13),                 */
	0x95, 0x01,         /*      Report Count (1),               */
	0x75, 0x10,         /*      Report Size (16),               */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0x0A, 0x2F, 0x02,   /*      Usage (AC Zoom),                */
	0x0A, 0x2E, 0x02,   /*      Usage (AC Zoom Out),            */
	0x0A, 0x2D, 0x02,   /*      Usage (AC Zoom In),             */
	0x09, 0xB6,         /*      Usage (Scan Previous Track),    */
	0x09, 0xB5,         /*      Usage (Scan Next Track),        */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x0A, 0x2E, 0x02,   /*      Usage (AC Zoom Out),            */
	0x0A, 0x2D, 0x02,   /*      Usage (AC Zoom In),             */
	0x15, 0x0C,         /*      Logical Minimum (12),           */
	0x25, 0x17,         /*      Logical Maximum (23),           */
	0x75, 0x05,         /*      Report Size (5),                */
	0x80,               /*      Input,                          */
	0x75, 0x03,         /*      Report Size (3),                */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0x75, 0x20,         /*      Report Size (32),               */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0xC0,               /*  End Collection,                     */
	0x09, 0x01,         /*  Usage (Consumer Control),           */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x0C,         /*      Report ID (12),                 */
	0x75, 0x01,         /*      Report Size (1),                */
	0x09, 0xE9,         /*      Usage (Volume Inc),             */
	0x09, 0xEA,         /*      Usage (Volume Dec),             */
	0x09, 0xE2,         /*      Usage (Mute),                   */
	0x14,               /*      Logical Minimum (0),            */
	0x25, 0x01,         /*      Logical Maximum (1),            */
	0x95, 0x03,         /*      Report Count (3),               */
	0x81, 0x06,         /*      Input (Variable, Relative),     */
	0x95, 0x35,         /*      Report Count (53),              */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0xC0                /*  End Collection                      */
};

/* Size of the original report descriptor of Media Tablet 14.1 inch */
#define MEDIA_TABLET_14_1_INCH_RDESC_ORIG_SIZE	309

/* Fixed Media Tablet 14.1 inch descriptor */
static __u8 media_tablet_14_1_inch_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x04,         /*          Report Size (4),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x80,               /*          Input,                      */
	0x75, 0x01,         /*          Report Size (1),            */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x34,               /*          Physical Minimum (0),       */
	0x09, 0x30,         /*          Usage (X),                  */
	0x46, 0xE0, 0x2E,   /*          Physical Maximum (12000),   */
	0x26, 0xFF, 0x3F,   /*          Logical Maximum (16383),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x46, 0x52, 0x1C,   /*          Physical Maximum (7250),    */
	0x26, 0xFF, 0x3F,   /*          Logical Maximum (16383),    */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x81, 0x02,         /*          Input (Variable),           */
	0xC0,               /*      End Collection,                 */
	0xC0,               /*  End Collection,                     */
	0x05, 0x01,         /*  Usage Page (Desktop),               */
	0x09, 0x02,         /*  Usage (Mouse),                      */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x01,         /*      Report ID (1),                  */
	0x09, 0x01,         /*      Usage (Pointer),                */
	0xA0,               /*      Collection (Physical),          */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x95, 0x02,         /*          Report Count (2),           */
	0x15, 0xFF,         /*          Logical Minimum (-1),       */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x09, 0x38,         /*          Usage (Wheel),              */
	0x0B, 0x38, 0x02,   /*          Usage (Consumer AC Pan),    */
		0x0C, 0x00,
	0x81, 0x06,         /*          Input (Variable, Relative), */
	0xC0,               /*      End Collection,                 */
	0xC0,               /*  End Collection,                     */
	0x05, 0x0C,         /*  Usage Page (Consumer),              */
	0x09, 0x01,         /*  Usage (Consumer Control),           */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x0D,         /*      Report ID (13),                 */
	0x95, 0x01,         /*      Report Count (1),               */
	0x75, 0x10,         /*      Report Size (16),               */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0x0A, 0x2F, 0x02,   /*      Usage (AC Zoom),                */
	0x0A, 0x2E, 0x02,   /*      Usage (AC Zoom Out),            */
	0x0A, 0x2D, 0x02,   /*      Usage (AC Zoom In),             */
	0x09, 0xB6,         /*      Usage (Scan Previous Track),    */
	0x09, 0xB5,         /*      Usage (Scan Next Track),        */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x08,               /*      Usage (00h),                    */
	0x0A, 0x2E, 0x02,   /*      Usage (AC Zoom Out),            */
	0x0A, 0x2D, 0x02,   /*      Usage (AC Zoom In),             */
	0x15, 0x0C,         /*      Logical Minimum (12),           */
	0x25, 0x17,         /*      Logical Maximum (23),           */
	0x75, 0x05,         /*      Report Size (5),                */
	0x80,               /*      Input,                          */
	0x75, 0x03,         /*      Report Size (3),                */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0x75, 0x20,         /*      Report Size (32),               */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0xC0,               /*  End Collection,                     */
	0x09, 0x01,         /*  Usage (Consumer Control),           */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x0C,         /*      Report ID (12),                 */
	0x75, 0x01,         /*      Report Size (1),                */
	0x09, 0xE9,         /*      Usage (Volume Inc),             */
	0x09, 0xEA,         /*      Usage (Volume Dec),             */
	0x09, 0xE2,         /*      Usage (Mute),                   */
	0x14,               /*      Logical Minimum (0),            */
	0x25, 0x01,         /*      Logical Maximum (1),            */
	0x95, 0x03,         /*      Report Count (3),               */
	0x81, 0x06,         /*      Input (Variable, Relative),     */
	0x75, 0x05,         /*      Report Size (5),                */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0xC0                /*  End Collection                      */
};

/* Size of the original report descriptor of Sirius Battery Free Tablet */
#define SIRIUS_BATTERY_FREE_TABLET_RDESC_ORIG_SIZE	335

/* Fixed Sirius Battery Free Tablet descriptor */
static __u8 sirius_battery_free_tablet_rdesc_fixed[] = {
	0x05, 0x0D,         /*  Usage Page (Digitizer),             */
	0x09, 0x02,         /*  Usage (Pen),                        */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x10,         /*      Report ID (16),                 */
	0x09, 0x20,         /*      Usage (Stylus),                 */
	0xA0,               /*      Collection (Physical),          */
	0x95, 0x01,         /*          Report Count (1),           */
	0x15, 0x01,         /*          Logical Minimum (1),        */
	0x25, 0x03,         /*          Logical Maximum (3),        */
	0x75, 0x02,         /*          Report Size (2),            */
	0x09, 0x42,         /*          Usage (Tip Switch),         */
	0x09, 0x44,         /*          Usage (Barrel Switch),      */
	0x09, 0x46,         /*          Usage (Tablet Pick),        */
	0x80,               /*          Input,                      */
	0x14,               /*          Logical Minimum (0),        */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x75, 0x01,         /*          Report Size (1),            */
	0x09, 0x3C,         /*          Usage (Invert),             */
	0x81, 0x02,         /*          Input (Variable),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x09, 0x32,         /*          Usage (In Range),           */
	0x81, 0x02,         /*          Input (Variable),           */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0xA4,               /*          Push,                       */
	0x05, 0x01,         /*          Usage Page (Desktop),       */
	0x55, 0xFD,         /*          Unit Exponent (-3),         */
	0x65, 0x13,         /*          Unit (Inch),                */
	0x34,               /*          Physical Minimum (0),       */
	0x14,               /*          Logical Minimum (0),        */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x46, 0x10, 0x27,   /*          Physical Maximum (10000),   */
	0x26, 0x20, 0x4E,   /*          Logical Maximum (20000),    */
	0x09, 0x30,         /*          Usage (X),                  */
	0x81, 0x02,         /*          Input (Variable),           */
	0x46, 0x70, 0x17,   /*          Physical Maximum (6000),    */
	0x26, 0xE0, 0x2E,   /*          Logical Maximum (12000),    */
	0x09, 0x31,         /*          Usage (Y),                  */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0x75, 0x10,         /*          Report Size (16),           */
	0x95, 0x01,         /*          Report Count (1),           */
	0x14,               /*          Logical Minimum (0),        */
	0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
	0x09, 0x30,         /*          Usage (Tip Pressure),       */
	0x81, 0x02,         /*          Input (Variable),           */
	0xA4,               /*          Push,                       */
	0x55, 0xFE,         /*          Unit Exponent (-2),         */
	0x65, 0x12,         /*          Unit (Radians),             */
	0x35, 0x97,         /*          Physical Minimum (-105),    */
	0x45, 0x69,         /*          Physical Maximum (105),     */
	0x15, 0x97,         /*          Logical Minimum (-105),     */
	0x25, 0x69,         /*          Logical Maximum (105),      */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x02,         /*          Report Count (2),           */
	0x09, 0x3D,         /*          Usage (X Tilt),             */
	0x09, 0x3E,         /*          Usage (Y Tilt),             */
	0x81, 0x02,         /*          Input (Variable),           */
	0xB4,               /*          Pop,                        */
	0xC0,               /*      End Collection,                 */
	0xC0,               /*  End Collection,                     */
	0x05, 0x01,         /*  Usage Page (Desktop),               */
	0x09, 0x02,         /*  Usage (Mouse),                      */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x01,         /*      Report ID (1),                  */
	0x09, 0x01,         /*      Usage (Pointer),                */
	0xA0,               /*      Collection (Physical),          */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0x09, 0x38,         /*          Usage (Wheel),              */
	0x15, 0xFF,         /*          Logical Minimum (-1),       */
	0x25, 0x01,         /*          Logical Maximum (1),        */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x01,         /*          Report Count (1),           */
	0x81, 0x06,         /*          Input (Variable, Relative), */
	0x75, 0x08,         /*          Report Size (8),            */
	0x95, 0x03,         /*          Report Count (3),           */
	0x81, 0x03,         /*          Input (Constant, Variable), */
	0xC0,               /*      End Collection,                 */
	0xC0,               /*  End Collection,                     */
	0x05, 0x01,         /*  Usage Page (Desktop),               */
	0x09, 0x06,         /*  Usage (Keyboard),                   */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x0D,         /*      Report ID (13),                 */
	0x05, 0x07,         /*      Usage Page (Keyboard),          */
	0x19, 0xE0,         /*      Usage Minimum (KB Leftcontrol), */
	0x29, 0xE7,         /*      Usage Maximum (KB Right GUI),   */
	0x14,               /*      Logical Minimum (0),            */
	0x25, 0x01,         /*      Logical Maximum (1),            */
	0x75, 0x01,         /*      Report Size (1),                */
	0x95, 0x08,         /*      Report Count (8),               */
	0x81, 0x02,         /*      Input (Variable),               */
	0x75, 0x08,         /*      Report Size (8),                */
	0x95, 0x01,         /*      Report Count (1),               */
	0x81, 0x01,         /*      Input (Constant),               */
	0x18,               /*      Usage Minimum (None),           */
	0x29, 0x65,         /*      Usage Maximum (KB Application), */
	0x14,               /*      Logical Minimum (0),            */
	0x25, 0x65,         /*      Logical Maximum (101),          */
	0x75, 0x08,         /*      Report Size (8),                */
	0x95, 0x05,         /*      Report Count (5),               */
	0x80,               /*      Input,                          */
	0xC0,               /*  End Collection,                     */
	0x05, 0x0C,         /*  Usage Page (Consumer),              */
	0x09, 0x01,         /*  Usage (Consumer Control),           */
	0xA1, 0x01,         /*  Collection (Application),           */
	0x85, 0x0C,         /*      Report ID (12),                 */
	0x09, 0xE9,         /*      Usage (Volume Inc),             */
	0x09, 0xEA,         /*      Usage (Volume Dec),             */
	0x14,               /*      Logical Minimum (0),            */
	0x25, 0x01,         /*      Logical Maximum (1),            */
	0x75, 0x01,         /*      Report Size (1),                */
	0x95, 0x02,         /*      Report Count (2),               */
	0x81, 0x02,         /*      Input (Variable),               */
	0x75, 0x06,         /*      Report Size (6),                */
	0x95, 0x01,         /*      Report Count (1),               */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0x75, 0x10,         /*      Report Size (16),               */
	0x95, 0x03,         /*      Report Count (3),               */
	0x81, 0x03,         /*      Input (Constant, Variable),     */
	0xC0                /*  End Collection                      */
};

static __u8 *waltop_report_fixup(struct hid_device *hdev, __u8 *rdesc,
		unsigned int *rsize)
{
	switch (hdev->product) {
	case USB_DEVICE_ID_WALTOP_SLIM_TABLET_5_8_INCH:
		if (*rsize == SLIM_TABLET_5_8_INCH_RDESC_ORIG_SIZE) {
			rdesc = slim_tablet_5_8_inch_rdesc_fixed;
			*rsize = sizeof(slim_tablet_5_8_inch_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_WALTOP_SLIM_TABLET_12_1_INCH:
		if (*rsize == SLIM_TABLET_12_1_INCH_RDESC_ORIG_SIZE) {
			rdesc = slim_tablet_12_1_inch_rdesc_fixed;
			*rsize = sizeof(slim_tablet_12_1_inch_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_WALTOP_Q_PAD:
		if (*rsize == Q_PAD_RDESC_ORIG_SIZE) {
			rdesc = q_pad_rdesc_fixed;
			*rsize = sizeof(q_pad_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_WALTOP_PID_0038:
		if (*rsize == PID_0038_RDESC_ORIG_SIZE) {
			rdesc = pid_0038_rdesc_fixed;
			*rsize = sizeof(pid_0038_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_WALTOP_MEDIA_TABLET_10_6_INCH:
		if (*rsize == MEDIA_TABLET_10_6_INCH_RDESC_ORIG_SIZE) {
			rdesc = media_tablet_10_6_inch_rdesc_fixed;
			*rsize = sizeof(media_tablet_10_6_inch_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_WALTOP_MEDIA_TABLET_14_1_INCH:
		if (*rsize == MEDIA_TABLET_14_1_INCH_RDESC_ORIG_SIZE) {
			rdesc = media_tablet_14_1_inch_rdesc_fixed;
			*rsize = sizeof(media_tablet_14_1_inch_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_WALTOP_SIRIUS_BATTERY_FREE_TABLET:
		if (*rsize == SIRIUS_BATTERY_FREE_TABLET_RDESC_ORIG_SIZE) {
			rdesc = sirius_battery_free_tablet_rdesc_fixed;
			*rsize = sizeof(sirius_battery_free_tablet_rdesc_fixed);
		}
		break;
	}
	return rdesc;
}

static int waltop_raw_event(struct hid_device *hdev, struct hid_report *report,
		     u8 *data, int size)
{
	/* If this is a pen input report */
	if (report->type == HID_INPUT_REPORT && report->id == 16 && size >= 8) {
		/*
		 * Ignore reported pressure when a barrel button is pressed,
		 * because it is rarely correct.
		 */

		/* If a barrel button is pressed */
		if ((data[1] & 0xF) > 1) {
			/* Report zero pressure */
			data[6] = 0;
			data[7] = 0;
		}
	}

	/* If this is a pen input report of Sirius Battery Free Tablet */
	if (hdev->product == USB_DEVICE_ID_WALTOP_SIRIUS_BATTERY_FREE_TABLET &&
	    report->type == HID_INPUT_REPORT &&
	    report->id == 16 &&
	    size == 10) {
		/*
		 * The tablet reports tilt as roughly sin(a)*21 (18 means 60
		 * degrees).
		 *
		 * This array stores angles as radians * 100, corresponding to
		 * reported values up to 60 degrees, as expected by userspace.
		 */
		static const s8 tilt_to_radians[] = {
			0, 5, 10, 14, 19, 24, 29, 34, 40, 45,
			50, 56, 62, 68, 74, 81, 88, 96, 105
		};

		s8 tilt_x = (s8)data[8];
		s8 tilt_y = (s8)data[9];
		s8 sign_x = tilt_x >= 0 ? 1 : -1;
		s8 sign_y = tilt_y >= 0 ? 1 : -1;

		tilt_x *= sign_x;
		tilt_y *= sign_y;

		/*
		 * Reverse the Y Tilt direction to match the HID standard and
		 * userspace expectations. See HID Usage Tables v1.12 16.3.2
		 * Tilt Orientation.
		 */
		sign_y *= -1;

		/*
		 * This effectively clamps reported tilt to 60 degrees - the
		 * range expected by userspace
		 */
		if (tilt_x > ARRAY_SIZE(tilt_to_radians) - 1)
			tilt_x = ARRAY_SIZE(tilt_to_radians) - 1;
		if (tilt_y > ARRAY_SIZE(tilt_to_radians) - 1)
			tilt_y = ARRAY_SIZE(tilt_to_radians) - 1;

		data[8] = tilt_to_radians[tilt_x] * sign_x;
		data[9] = tilt_to_radians[tilt_y] * sign_y;
	}

	return 0;
}

static const struct hid_device_id waltop_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
				USB_DEVICE_ID_WALTOP_SLIM_TABLET_5_8_INCH) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
				USB_DEVICE_ID_WALTOP_SLIM_TABLET_12_1_INCH) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
				USB_DEVICE_ID_WALTOP_Q_PAD) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
				USB_DEVICE_ID_WALTOP_PID_0038) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
				USB_DEVICE_ID_WALTOP_MEDIA_TABLET_10_6_INCH) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
				USB_DEVICE_ID_WALTOP_MEDIA_TABLET_14_1_INCH) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_WALTOP,
			 USB_DEVICE_ID_WALTOP_SIRIUS_BATTERY_FREE_TABLET) },
	{ }
};
MODULE_DEVICE_TABLE(hid, waltop_devices);

static struct hid_driver waltop_driver = {
	.name = "waltop",
	.id_table = waltop_devices,
	.report_fixup = waltop_report_fixup,
	.raw_event = waltop_raw_event,
};
module_hid_driver(waltop_driver);

MODULE_LICENSE("GPL");

/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __HID_DEBUG_H
#define __HID_DEBUG_H

/*
 *  Copyright (c) 2007-2009	Jiri Kosina
 */

/*
 */

#ifdef CONFIG_DEBUG_FS

#include <linux/kfifo.h>

#define HID_DEBUG_BUFSIZE 512
#define HID_DEBUG_FIFOSIZE 512

void hid_dump_input(struct hid_device *, struct hid_usage *, __s32);
void hid_dump_report(struct hid_device *, int , u8 *, int);
void hid_dump_device(struct hid_device *, struct seq_file *);
void hid_dump_field(struct hid_field *, int, struct seq_file *);
char *hid_resolv_usage(unsigned, struct seq_file *);
void hid_debug_register(struct hid_device *, const char *);
void hid_debug_unregister(struct hid_device *);
void hid_debug_init(void);
void hid_debug_exit(void);
void hid_debug_event(struct hid_device *, char *);

struct hid_debug_list {
	DECLARE_KFIFO_PTR(hid_debug_fifo, char);
	struct fasync_struct *fasync;
	struct hid_device *hdev;
	struct list_head node;
	struct mutex read_mutex;
};

#else

#define hid_dump_input(a,b,c)		do { } while (0)
#define hid_dump_report(a,b,c,d)	do { } while (0)
#define hid_dump_device(a,b)		do { } while (0)
#define hid_dump_field(a,b,c)		do { } while (0)
#define hid_resolv_usage(a,b)		do { } while (0)
#define hid_debug_register(a, b)	do { } while (0)
#define hid_debug_unregister(a)		do { } while (0)
#define hid_debug_init()		do { } while (0)
#define hid_debug_exit()		do { } while (0)
#define hid_debug_event(a,b)		do { } while (0)

#endif

#endif

/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _INPUT_MT_H
#define _INPUT_MT_H

/*
 * Input Multitouch Library
 *
 * Copyright (c) 2010 Henrik Rydberg
 */

#include <linux/input.h>

#define TRKID_MAX	0xffff

#define INPUT_MT_POINTER	0x0001	/* pointer device, e.g. trackpad */
#define INPUT_MT_DIRECT		0x0002	/* direct device, e.g. touchscreen */
#define INPUT_MT_DROP_UNUSED	0x0004	/* drop contacts not seen in frame */
#define INPUT_MT_TRACK		0x0008	/* use in-kernel tracking */
#define INPUT_MT_SEMI_MT	0x0010	/* semi-mt device, finger count handled manually */

/**
 * struct input_mt_slot - represents the state of an input MT slot
 * @abs: holds current values of ABS_MT axes for this slot
 * @frame: last frame at which input_mt_report_slot_state() was called
 * @key: optional driver designation of this slot
 */
struct input_mt_slot {
	int abs[ABS_MT_LAST - ABS_MT_FIRST + 1];
	unsigned int frame;
	unsigned int key;
};

/**
 * struct input_mt - state of tracked contacts
 * @trkid: stores MT tracking ID for the next contact
 * @num_slots: number of MT slots the device uses
 * @slot: MT slot currently being transmitted
 * @flags: input_mt operation flags
 * @frame: increases every time input_mt_sync_frame() is called
 * @red: reduced cost matrix for in-kernel tracking
 * @slots: array of slots holding current values of tracked contacts
 */
struct input_mt {
	int trkid;
	int num_slots;
	int slot;
	unsigned int flags;
	unsigned int frame;
	int *red;
	struct input_mt_slot slots[];
};

static inline void input_mt_set_value(struct input_mt_slot *slot,
				      unsigned code, int value)
{
	slot->abs[code - ABS_MT_FIRST] = value;
}

static inline int input_mt_get_value(const struct input_mt_slot *slot,
				     unsigned code)
{
	return slot->abs[code - ABS_MT_FIRST];
}

static inline bool input_mt_is_active(const struct input_mt_slot *slot)
{
	return input_mt_get_value(slot, ABS_MT_TRACKING_ID) >= 0;
}

static inline bool input_mt_is_used(const struct input_mt *mt,
				    const struct input_mt_slot *slot)
{
	return slot->frame == mt->frame;
}

int input_mt_init_slots(struct input_dev *dev, unsigned int num_slots,
			unsigned int flags);
void input_mt_destroy_slots(struct input_dev *dev);

static inline int input_mt_new_trkid(struct input_mt *mt)
{
	return mt->trkid++ & TRKID_MAX;
}

static inline void input_mt_slot(struct input_dev *dev, int slot)
{
	input_event(dev, EV_ABS, ABS_MT_SLOT, slot);
}

static inline bool input_is_mt_value(int axis)
{
	return axis >= ABS_MT_FIRST && axis <= ABS_MT_LAST;
}

static inline bool input_is_mt_axis(int axis)
{
	return axis == ABS_MT_SLOT || input_is_mt_value(axis);
}

bool input_mt_report_slot_state(struct input_dev *dev,
				unsigned int tool_type, bool active);

static inline void input_mt_report_slot_inactive(struct input_dev *dev)
{
	input_mt_report_slot_state(dev, 0, false);
}

void input_mt_report_finger_count(struct input_dev *dev, int count);
void input_mt_report_pointer_emulation(struct input_dev *dev, bool use_count);
void input_mt_drop_unused(struct input_dev *dev);

void input_mt_sync_frame(struct input_dev *dev);

/**
 * struct input_mt_pos - contact position
 * @x: horizontal coordinate
 * @y: vertical coordinate
 */
struct input_mt_pos {
	s16 x, y;
};

int input_mt_assign_slots(struct input_dev *dev, int *slots,
			  const struct input_mt_pos *pos, int num_pos,
			  int dmax);

int input_mt_get_slot_by_key(struct input_dev *dev, int key);

#endif

// SPDX-License-Identifier: GPL-2.0-only
/*
 * LED Triggers Core
 *
 * Copyright 2005-2007 Openedhand Ltd.
 *
 * Author: Richard Purdie <rpurdie@openedhand.com>
 */

#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/rwsem.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include "leds.h"

/*
 * Nests outside led_cdev->trigger_lock
 */
static DECLARE_RWSEM(triggers_list_lock);
LIST_HEAD(trigger_list);

 /* Used by LED Class */

static inline bool
trigger_relevant(struct led_classdev *led_cdev, struct led_trigger *trig)
{
	return !trig->trigger_type || trig->trigger_type == led_cdev->trigger_type;
}

ssize_t led_trigger_write(struct file *filp, struct kobject *kobj,
			  struct bin_attribute *bin_attr, char *buf,
			  loff_t pos, size_t count)
{
	struct device *dev = kobj_to_dev(kobj);
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct led_trigger *trig;
	int ret = count;

	mutex_lock(&led_cdev->led_access);

	if (led_sysfs_is_disabled(led_cdev)) {
		ret = -EBUSY;
		goto unlock;
	}

	if (sysfs_streq(buf, "none")) {
		led_trigger_remove(led_cdev);
		goto unlock;
	}

	down_read(&triggers_list_lock);
	list_for_each_entry(trig, &trigger_list, next_trig) {
		if (sysfs_streq(buf, trig->name) && trigger_relevant(led_cdev, trig)) {
			down_write(&led_cdev->trigger_lock);
			led_trigger_set(led_cdev, trig);
			up_write(&led_cdev->trigger_lock);

			up_read(&triggers_list_lock);
			goto unlock;
		}
	}
	/* we come here only if buf matches no trigger */
	ret = -EINVAL;
	up_read(&triggers_list_lock);

unlock:
	mutex_unlock(&led_cdev->led_access);
	return ret;
}
EXPORT_SYMBOL_GPL(led_trigger_write);

__printf(3, 4)
static int led_trigger_snprintf(char *buf, ssize_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	if (size <= 0)
		i = vsnprintf(NULL, 0, fmt, args);
	else
		i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

static int led_trigger_format(char *buf, size_t size,
			      struct led_classdev *led_cdev)
{
	struct led_trigger *trig;
	int len = led_trigger_snprintf(buf, size, "%s",
				       led_cdev->trigger ? "none" : "[none]");

	list_for_each_entry(trig, &trigger_list, next_trig) {
		bool hit;

		if (!trigger_relevant(led_cdev, trig))
			continue;

		hit = led_cdev->trigger && !strcmp(led_cdev->trigger->name, trig->name);

		len += led_trigger_snprintf(buf + len, size - len,
					    " %s%s%s", hit ? "[" : "",
					    trig->name, hit ? "]" : "");
	}

	len += led_trigger_snprintf(buf + len, size - len, "\n");

	return len;
}

/*
 * It was stupid to create 10000 cpu triggers, but we are stuck with it now.
 * Don't make that mistake again. We work around it here by creating binary
 * attribute, which is not limited by length. This is _not_ good design, do not
 * copy it.
 */
ssize_t led_trigger_read(struct file *filp, struct kobject *kobj,
			struct bin_attribute *attr, char *buf,
			loff_t pos, size_t count)
{
	struct device *dev = kobj_to_dev(kobj);
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	void *data;
	int len;

	down_read(&triggers_list_lock);
	down_read(&led_cdev->trigger_lock);

	len = led_trigger_format(NULL, 0, led_cdev);
	data = kvmalloc(len + 1, GFP_KERNEL);
	if (!data) {
		up_read(&led_cdev->trigger_lock);
		up_read(&triggers_list_lock);
		return -ENOMEM;
	}
	len = led_trigger_format(data, len + 1, led_cdev);

	up_read(&led_cdev->trigger_lock);
	up_read(&triggers_list_lock);

	len = memory_read_from_buffer(buf, count, &pos, data, len);

	kvfree(data);

	return len;
}
EXPORT_SYMBOL_GPL(led_trigger_read);

/* Caller must ensure led_cdev->trigger_lock held */
int led_trigger_set(struct led_classdev *led_cdev, struct led_trigger *trig)
{
	unsigned long flags;
	char *event = NULL;
	char *envp[2];
	const char *name;
	int ret;

	if (!led_cdev->trigger && !trig)
		return 0;

	name = trig ? trig->name : "none";
	event = kasprintf(GFP_KERNEL, "TRIGGER=%s", name);

	/* Remove any existing trigger */
	if (led_cdev->trigger) {
		write_lock_irqsave(&led_cdev->trigger->leddev_list_lock, flags);
		list_del(&led_cdev->trig_list);
		write_unlock_irqrestore(&led_cdev->trigger->leddev_list_lock,
			flags);
		cancel_work_sync(&led_cdev->set_brightness_work);
		led_stop_software_blink(led_cdev);
		if (led_cdev->trigger->deactivate)
			led_cdev->trigger->deactivate(led_cdev);
		device_remove_groups(led_cdev->dev, led_cdev->trigger->groups);
		led_cdev->trigger = NULL;
		led_cdev->trigger_data = NULL;
		led_cdev->activated = false;
		led_set_brightness(led_cdev, LED_OFF);
	}
	if (trig) {
		write_lock_irqsave(&trig->leddev_list_lock, flags);
		list_add_tail(&led_cdev->trig_list, &trig->led_cdevs);
		write_unlock_irqrestore(&trig->leddev_list_lock, flags);
		led_cdev->trigger = trig;

		if (trig->activate)
			ret = trig->activate(led_cdev);
		else
			ret = 0;

		if (ret)
			goto err_activate;

		ret = device_add_groups(led_cdev->dev, trig->groups);
		if (ret) {
			dev_err(led_cdev->dev, "Failed to add trigger attributes\n");
			goto err_add_groups;
		}
	}

	if (event) {
		envp[0] = event;
		envp[1] = NULL;
		if (kobject_uevent_env(&led_cdev->dev->kobj, KOBJ_CHANGE, envp))
			dev_err(led_cdev->dev,
				"%s: Error sending uevent\n", __func__);
		kfree(event);
	}

	return 0;

err_add_groups:

	if (trig->deactivate)
		trig->deactivate(led_cdev);
err_activate:

	write_lock_irqsave(&led_cdev->trigger->leddev_list_lock, flags);
	list_del(&led_cdev->trig_list);
	write_unlock_irqrestore(&led_cdev->trigger->leddev_list_lock, flags);
	led_cdev->trigger = NULL;
	led_cdev->trigger_data = NULL;
	led_set_brightness(led_cdev, LED_OFF);
	kfree(event);

	return ret;
}
EXPORT_SYMBOL_GPL(led_trigger_set);

void led_trigger_remove(struct led_classdev *led_cdev)
{
	down_write(&led_cdev->trigger_lock);
	led_trigger_set(led_cdev, NULL);
	up_write(&led_cdev->trigger_lock);
}
EXPORT_SYMBOL_GPL(led_trigger_remove);

void led_trigger_set_default(struct led_classdev *led_cdev)
{
	struct led_trigger *trig;

	if (!led_cdev->default_trigger)
		return;

	down_read(&triggers_list_lock);
	down_write(&led_cdev->trigger_lock);
	list_for_each_entry(trig, &trigger_list, next_trig) {
		if (!strcmp(led_cdev->default_trigger, trig->name) &&
		    trigger_relevant(led_cdev, trig)) {
			led_cdev->flags |= LED_INIT_DEFAULT_TRIGGER;
			led_trigger_set(led_cdev, trig);
			break;
		}
	}
	up_write(&led_cdev->trigger_lock);
	up_read(&triggers_list_lock);
}
EXPORT_SYMBOL_GPL(led_trigger_set_default);

void led_trigger_rename_static(const char *name, struct led_trigger *trig)
{
	/* new name must be on a temporary string to prevent races */
	BUG_ON(name == trig->name);

	down_write(&triggers_list_lock);
	/* this assumes that trig->name was originaly allocated to
	 * non constant storage */
	strcpy((char *)trig->name, name);
	up_write(&triggers_list_lock);
}
EXPORT_SYMBOL_GPL(led_trigger_rename_static);

/* LED Trigger Interface */

int led_trigger_register(struct led_trigger *trig)
{
	struct led_classdev *led_cdev;
	struct led_trigger *_trig;

	rwlock_init(&trig->leddev_list_lock);
	INIT_LIST_HEAD(&trig->led_cdevs);

	down_write(&triggers_list_lock);
	/* Make sure the trigger's name isn't already in use */
	list_for_each_entry(_trig, &trigger_list, next_trig) {
		if (!strcmp(_trig->name, trig->name) &&
		    (trig->trigger_type == _trig->trigger_type ||
		     !trig->trigger_type || !_trig->trigger_type)) {
			up_write(&triggers_list_lock);
			return -EEXIST;
		}
	}
	/* Add to the list of led triggers */
	list_add_tail(&trig->next_trig, &trigger_list);
	up_write(&triggers_list_lock);

	/* Register with any LEDs that have this as a default trigger */
	down_read(&leds_list_lock);
	list_for_each_entry(led_cdev, &leds_list, node) {
		down_write(&led_cdev->trigger_lock);
		if (!led_cdev->trigger && led_cdev->default_trigger &&
		    !strcmp(led_cdev->default_trigger, trig->name) &&
		    trigger_relevant(led_cdev, trig)) {
			led_cdev->flags |= LED_INIT_DEFAULT_TRIGGER;
			led_trigger_set(led_cdev, trig);
		}
		up_write(&led_cdev->trigger_lock);
	}
	up_read(&leds_list_lock);

	return 0;
}
EXPORT_SYMBOL_GPL(led_trigger_register);

void led_trigger_unregister(struct led_trigger *trig)
{
	struct led_classdev *led_cdev;

	if (list_empty_careful(&trig->next_trig))
		return;

	/* Remove from the list of led triggers */
	down_write(&triggers_list_lock);
	list_del_init(&trig->next_trig);
	up_write(&triggers_list_lock);

	/* Remove anyone actively using this trigger */
	down_read(&leds_list_lock);
	list_for_each_entry(led_cdev, &leds_list, node) {
		down_write(&led_cdev->trigger_lock);
		if (led_cdev->trigger == trig)
			led_trigger_set(led_cdev, NULL);
		up_write(&led_cdev->trigger_lock);
	}
	up_read(&leds_list_lock);
}
EXPORT_SYMBOL_GPL(led_trigger_unregister);

static void devm_led_trigger_release(struct device *dev, void *res)
{
	led_trigger_unregister(*(struct led_trigger **)res);
}

int devm_led_trigger_register(struct device *dev,
			      struct led_trigger *trig)
{
	struct led_trigger **dr;
	int rc;

	dr = devres_alloc(devm_led_trigger_release, sizeof(*dr),
			  GFP_KERNEL);
	if (!dr)
		return -ENOMEM;

	*dr = trig;

	rc = led_trigger_register(trig);
	if (rc)
		devres_free(dr);
	else
		devres_add(dev, dr);

	return rc;
}
EXPORT_SYMBOL_GPL(devm_led_trigger_register);

/* Simple LED Trigger Interface */

void led_trigger_event(struct led_trigger *trig,
			enum led_brightness brightness)
{
	struct led_classdev *led_cdev;
	unsigned long flags;

	if (!trig)
		return;

	read_lock_irqsave(&trig->leddev_list_lock, flags);
	list_for_each_entry(led_cdev, &trig->led_cdevs, trig_list)
		led_set_brightness(led_cdev, brightness);
	read_unlock_irqrestore(&trig->leddev_list_lock, flags);
}
EXPORT_SYMBOL_GPL(led_trigger_event);

static void led_trigger_blink_setup(struct led_trigger *trig,
			     unsigned long *delay_on,
			     unsigned long *delay_off,
			     int oneshot,
			     int invert)
{
	struct led_classdev *led_cdev;
	unsigned long flags;

	if (!trig)
		return;

	read_lock_irqsave(&trig->leddev_list_lock, flags);
	list_for_each_entry(led_cdev, &trig->led_cdevs, trig_list) {
		if (oneshot)
			led_blink_set_oneshot(led_cdev, delay_on, delay_off,
					      invert);
		else
			led_blink_set(led_cdev, delay_on, delay_off);
	}
	read_unlock_irqrestore(&trig->leddev_list_lock, flags);
}

void led_trigger_blink(struct led_trigger *trig,
		       unsigned long *delay_on,
		       unsigned long *delay_off)
{
	led_trigger_blink_setup(trig, delay_on, delay_off, 0, 0);
}
EXPORT_SYMBOL_GPL(led_trigger_blink);

void led_trigger_blink_oneshot(struct led_trigger *trig,
			       unsigned long *delay_on,
			       unsigned long *delay_off,
			       int invert)
{
	led_trigger_blink_setup(trig, delay_on, delay_off, 1, invert);
}
EXPORT_SYMBOL_GPL(led_trigger_blink_oneshot);

void led_trigger_register_simple(const char *name, struct led_trigger **tp)
{
	struct led_trigger *trig;
	int err;

	trig = kzalloc(sizeof(struct led_trigger), GFP_KERNEL);

	if (trig) {
		trig->name = name;
		err = led_trigger_register(trig);
		if (err < 0) {
			kfree(trig);
			trig = NULL;
			pr_warn("LED trigger %s failed to register (%d)\n",
				name, err);
		}
	} else {
		pr_warn("LED trigger %s failed to register (no memory)\n",
			name);
	}
	*tp = trig;
}
EXPORT_SYMBOL_GPL(led_trigger_register_simple);

void led_trigger_unregister_simple(struct led_trigger *trig)
{
	if (trig)
		led_trigger_unregister(trig);
	kfree(trig);
}
EXPORT_SYMBOL_GPL(led_trigger_unregister_simple);

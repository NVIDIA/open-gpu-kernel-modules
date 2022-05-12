// SPDX-License-Identifier: GPL-2.0
/*
 * kernel/power/wakelock.c
 *
 * User space wakeup sources support.
 *
 * Copyright (C) 2012 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This code is based on the analogous interface allowing user space to
 * manipulate wakelocks on Android.
 */

#include <linux/capability.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include "power.h"

static DEFINE_MUTEX(wakelocks_lock);

struct wakelock {
	char			*name;
	struct rb_node		node;
	struct wakeup_source	*ws;
#ifdef CONFIG_PM_WAKELOCKS_GC
	struct list_head	lru;
#endif
};

static struct rb_root wakelocks_tree = RB_ROOT;

ssize_t pm_show_wakelocks(char *buf, bool show_active)
{
	struct rb_node *node;
	struct wakelock *wl;
	char *str = buf;
	char *end = buf + PAGE_SIZE;

	mutex_lock(&wakelocks_lock);

	for (node = rb_first(&wakelocks_tree); node; node = rb_next(node)) {
		wl = rb_entry(node, struct wakelock, node);
		if (wl->ws->active == show_active)
			str += scnprintf(str, end - str, "%s ", wl->name);
	}
	if (str > buf)
		str--;

	str += scnprintf(str, end - str, "\n");

	mutex_unlock(&wakelocks_lock);
	return (str - buf);
}

#if CONFIG_PM_WAKELOCKS_LIMIT > 0
static unsigned int number_of_wakelocks;

static inline bool wakelocks_limit_exceeded(void)
{
	return number_of_wakelocks > CONFIG_PM_WAKELOCKS_LIMIT;
}

static inline void increment_wakelocks_number(void)
{
	number_of_wakelocks++;
}

static inline void decrement_wakelocks_number(void)
{
	number_of_wakelocks--;
}
#else /* CONFIG_PM_WAKELOCKS_LIMIT = 0 */
static inline bool wakelocks_limit_exceeded(void) { return false; }
static inline void increment_wakelocks_number(void) {}
static inline void decrement_wakelocks_number(void) {}
#endif /* CONFIG_PM_WAKELOCKS_LIMIT */

#ifdef CONFIG_PM_WAKELOCKS_GC
#define WL_GC_COUNT_MAX	100
#define WL_GC_TIME_SEC	300

static void __wakelocks_gc(struct work_struct *work);
static LIST_HEAD(wakelocks_lru_list);
static DECLARE_WORK(wakelock_work, __wakelocks_gc);
static unsigned int wakelocks_gc_count;

static inline void wakelocks_lru_add(struct wakelock *wl)
{
	list_add(&wl->lru, &wakelocks_lru_list);
}

static inline void wakelocks_lru_most_recent(struct wakelock *wl)
{
	list_move(&wl->lru, &wakelocks_lru_list);
}

static void __wakelocks_gc(struct work_struct *work)
{
	struct wakelock *wl, *aux;
	ktime_t now;

	mutex_lock(&wakelocks_lock);

	now = ktime_get();
	list_for_each_entry_safe_reverse(wl, aux, &wakelocks_lru_list, lru) {
		u64 idle_time_ns;
		bool active;

		spin_lock_irq(&wl->ws->lock);
		idle_time_ns = ktime_to_ns(ktime_sub(now, wl->ws->last_time));
		active = wl->ws->active;
		spin_unlock_irq(&wl->ws->lock);

		if (idle_time_ns < ((u64)WL_GC_TIME_SEC * NSEC_PER_SEC))
			break;

		if (!active) {
			wakeup_source_unregister(wl->ws);
			rb_erase(&wl->node, &wakelocks_tree);
			list_del(&wl->lru);
			kfree(wl->name);
			kfree(wl);
			decrement_wakelocks_number();
		}
	}
	wakelocks_gc_count = 0;

	mutex_unlock(&wakelocks_lock);
}

static void wakelocks_gc(void)
{
	if (++wakelocks_gc_count <= WL_GC_COUNT_MAX)
		return;

	schedule_work(&wakelock_work);
}
#else /* !CONFIG_PM_WAKELOCKS_GC */
static inline void wakelocks_lru_add(struct wakelock *wl) {}
static inline void wakelocks_lru_most_recent(struct wakelock *wl) {}
static inline void wakelocks_gc(void) {}
#endif /* !CONFIG_PM_WAKELOCKS_GC */

static struct wakelock *wakelock_lookup_add(const char *name, size_t len,
					    bool add_if_not_found)
{
	struct rb_node **node = &wakelocks_tree.rb_node;
	struct rb_node *parent = *node;
	struct wakelock *wl;

	while (*node) {
		int diff;

		parent = *node;
		wl = rb_entry(*node, struct wakelock, node);
		diff = strncmp(name, wl->name, len);
		if (diff == 0) {
			if (wl->name[len])
				diff = -1;
			else
				return wl;
		}
		if (diff < 0)
			node = &(*node)->rb_left;
		else
			node = &(*node)->rb_right;
	}
	if (!add_if_not_found)
		return ERR_PTR(-EINVAL);

	if (wakelocks_limit_exceeded())
		return ERR_PTR(-ENOSPC);

	/* Not found, we have to add a new one. */
	wl = kzalloc(sizeof(*wl), GFP_KERNEL);
	if (!wl)
		return ERR_PTR(-ENOMEM);

	wl->name = kstrndup(name, len, GFP_KERNEL);
	if (!wl->name) {
		kfree(wl);
		return ERR_PTR(-ENOMEM);
	}

	wl->ws = wakeup_source_register(NULL, wl->name);
	if (!wl->ws) {
		kfree(wl->name);
		kfree(wl);
		return ERR_PTR(-ENOMEM);
	}
	wl->ws->last_time = ktime_get();

	rb_link_node(&wl->node, parent, node);
	rb_insert_color(&wl->node, &wakelocks_tree);
	wakelocks_lru_add(wl);
	increment_wakelocks_number();
	return wl;
}

int pm_wake_lock(const char *buf)
{
	const char *str = buf;
	struct wakelock *wl;
	u64 timeout_ns = 0;
	size_t len;
	int ret = 0;

	if (!capable(CAP_BLOCK_SUSPEND))
		return -EPERM;

	while (*str && !isspace(*str))
		str++;

	len = str - buf;
	if (!len)
		return -EINVAL;

	if (*str && *str != '\n') {
		/* Find out if there's a valid timeout string appended. */
		ret = kstrtou64(skip_spaces(str), 10, &timeout_ns);
		if (ret)
			return -EINVAL;
	}

	mutex_lock(&wakelocks_lock);

	wl = wakelock_lookup_add(buf, len, true);
	if (IS_ERR(wl)) {
		ret = PTR_ERR(wl);
		goto out;
	}
	if (timeout_ns) {
		u64 timeout_ms = timeout_ns + NSEC_PER_MSEC - 1;

		do_div(timeout_ms, NSEC_PER_MSEC);
		__pm_wakeup_event(wl->ws, timeout_ms);
	} else {
		__pm_stay_awake(wl->ws);
	}

	wakelocks_lru_most_recent(wl);

 out:
	mutex_unlock(&wakelocks_lock);
	return ret;
}

int pm_wake_unlock(const char *buf)
{
	struct wakelock *wl;
	size_t len;
	int ret = 0;

	if (!capable(CAP_BLOCK_SUSPEND))
		return -EPERM;

	len = strlen(buf);
	if (!len)
		return -EINVAL;

	if (buf[len-1] == '\n')
		len--;

	if (!len)
		return -EINVAL;

	mutex_lock(&wakelocks_lock);

	wl = wakelock_lookup_add(buf, len, false);
	if (IS_ERR(wl)) {
		ret = PTR_ERR(wl);
		goto out;
	}
	__pm_relax(wl->ws);

	wakelocks_lru_most_recent(wl);
	wakelocks_gc();

 out:
	mutex_unlock(&wakelocks_lock);
	return ret;
}

// SPDX-License-Identifier: GPL-2.0
/**
 * A generic FSM based on fsm used in isdn4linux
 *
 */

#include "fsm.h"
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/timer.h>

MODULE_AUTHOR("(C) 2000 IBM Corp. by Fritz Elfert (felfert@millenux.com)");
MODULE_DESCRIPTION("Finite state machine helper functions");
MODULE_LICENSE("GPL");

fsm_instance *
init_fsm(char *name, const char **state_names, const char **event_names, int nr_states,
		int nr_events, const fsm_node *tmpl, int tmpl_len, gfp_t order)
{
	int i;
	fsm_instance *this;
	fsm_function_t *m;
	fsm *f;

	this = kzalloc(sizeof(fsm_instance), order);
	if (this == NULL) {
		printk(KERN_WARNING
			"fsm(%s): init_fsm: Couldn't alloc instance\n", name);
		return NULL;
	}
	strlcpy(this->name, name, sizeof(this->name));
	init_waitqueue_head(&this->wait_q);

	f = kzalloc(sizeof(fsm), order);
	if (f == NULL) {
		printk(KERN_WARNING
			"fsm(%s): init_fsm: Couldn't alloc fsm\n", name);
		kfree_fsm(this);
		return NULL;
	}
	f->nr_events = nr_events;
	f->nr_states = nr_states;
	f->event_names = event_names;
	f->state_names = state_names;
	this->f = f;

	m = kcalloc(nr_states*nr_events, sizeof(fsm_function_t), order);
	if (m == NULL) {
		printk(KERN_WARNING
			"fsm(%s): init_fsm: Couldn't alloc jumptable\n", name);
		kfree_fsm(this);
		return NULL;
	}
	f->jumpmatrix = m;

	for (i = 0; i < tmpl_len; i++) {
		if ((tmpl[i].cond_state >= nr_states) ||
		    (tmpl[i].cond_event >= nr_events)   ) {
			printk(KERN_ERR
				"fsm(%s): init_fsm: Bad template l=%d st(%ld/%ld) ev(%ld/%ld)\n",
				name, i, (long)tmpl[i].cond_state, (long)f->nr_states,
				(long)tmpl[i].cond_event, (long)f->nr_events);
			kfree_fsm(this);
			return NULL;
		} else
			m[nr_states * tmpl[i].cond_event + tmpl[i].cond_state] =
				tmpl[i].function;
	}
	return this;
}

void
kfree_fsm(fsm_instance *this)
{
	if (this) {
		if (this->f) {
			kfree(this->f->jumpmatrix);
			kfree(this->f);
		}
		kfree(this);
	} else
		printk(KERN_WARNING
			"fsm: kfree_fsm called with NULL argument\n");
}

#if FSM_DEBUG_HISTORY
void
fsm_print_history(fsm_instance *fi)
{
	int idx = 0;
	int i;

	if (fi->history_size >= FSM_HISTORY_SIZE)
		idx = fi->history_index;

	printk(KERN_DEBUG "fsm(%s): History:\n", fi->name);
	for (i = 0; i < fi->history_size; i++) {
		int e = fi->history[idx].event;
		int s = fi->history[idx++].state;
		idx %= FSM_HISTORY_SIZE;
		if (e == -1)
			printk(KERN_DEBUG "  S=%s\n",
			       fi->f->state_names[s]);
		else
			printk(KERN_DEBUG "  S=%s E=%s\n",
			       fi->f->state_names[s],
			       fi->f->event_names[e]);
	}
	fi->history_size = fi->history_index = 0;
}

void
fsm_record_history(fsm_instance *fi, int state, int event)
{
	fi->history[fi->history_index].state = state;
	fi->history[fi->history_index++].event = event;
	fi->history_index %= FSM_HISTORY_SIZE;
	if (fi->history_size < FSM_HISTORY_SIZE)
		fi->history_size++;
}
#endif

const char *
fsm_getstate_str(fsm_instance *fi)
{
	int st = atomic_read(&fi->state);
	if (st >= fi->f->nr_states)
		return "Invalid";
	return fi->f->state_names[st];
}

static void
fsm_expire_timer(struct timer_list *t)
{
	fsm_timer *this = from_timer(this, t, tl);
#if FSM_TIMER_DEBUG
	printk(KERN_DEBUG "fsm(%s): Timer %p expired\n",
	       this->fi->name, this);
#endif
	fsm_event(this->fi, this->expire_event, this->event_arg);
}

void
fsm_settimer(fsm_instance *fi, fsm_timer *this)
{
	this->fi = fi;
#if FSM_TIMER_DEBUG
	printk(KERN_DEBUG "fsm(%s): Create timer %p\n", fi->name,
	       this);
#endif
	timer_setup(&this->tl, fsm_expire_timer, 0);
}

void
fsm_deltimer(fsm_timer *this)
{
#if FSM_TIMER_DEBUG
	printk(KERN_DEBUG "fsm(%s): Delete timer %p\n", this->fi->name,
		this);
#endif
	del_timer(&this->tl);
}

int
fsm_addtimer(fsm_timer *this, int millisec, int event, void *arg)
{

#if FSM_TIMER_DEBUG
	printk(KERN_DEBUG "fsm(%s): Add timer %p %dms\n",
	       this->fi->name, this, millisec);
#endif

	timer_setup(&this->tl, fsm_expire_timer, 0);
	this->expire_event = event;
	this->event_arg = arg;
	this->tl.expires = jiffies + (millisec * HZ) / 1000;
	add_timer(&this->tl);
	return 0;
}

/* FIXME: this function is never used, why */
void
fsm_modtimer(fsm_timer *this, int millisec, int event, void *arg)
{

#if FSM_TIMER_DEBUG
	printk(KERN_DEBUG "fsm(%s): Restart timer %p %dms\n",
		this->fi->name, this, millisec);
#endif

	del_timer(&this->tl);
	timer_setup(&this->tl, fsm_expire_timer, 0);
	this->expire_event = event;
	this->event_arg = arg;
	this->tl.expires = jiffies + (millisec * HZ) / 1000;
	add_timer(&this->tl);
}

EXPORT_SYMBOL(init_fsm);
EXPORT_SYMBOL(kfree_fsm);
EXPORT_SYMBOL(fsm_settimer);
EXPORT_SYMBOL(fsm_deltimer);
EXPORT_SYMBOL(fsm_addtimer);
EXPORT_SYMBOL(fsm_modtimer);
EXPORT_SYMBOL(fsm_getstate_str);

#if FSM_DEBUG_HISTORY
EXPORT_SYMBOL(fsm_print_history);
EXPORT_SYMBOL(fsm_record_history);
#endif

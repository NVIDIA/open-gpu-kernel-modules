/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _VT_KERN_H
#define _VT_KERN_H

/*
 * this really is an extension of the vc_cons structure in console.c, but
 * with information needed by the vt package
 */

#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/tty.h>
#include <linux/mutex.h>
#include <linux/console_struct.h>
#include <linux/mm.h>
#include <linux/consolemap.h>
#include <linux/notifier.h>

void kd_mksound(unsigned int hz, unsigned int ticks);
int kbd_rate(struct kbd_repeat *rep);

extern int fg_console, last_console, want_console;

/* console.c */

int vc_allocate(unsigned int console);
int vc_cons_allocated(unsigned int console);
int vc_resize(struct vc_data *vc, unsigned int cols, unsigned int lines);
struct vc_data *vc_deallocate(unsigned int console);
void reset_palette(struct vc_data *vc);
void do_blank_screen(int entering_gfx);
void do_unblank_screen(int leaving_gfx);
void unblank_screen(void);
void poke_blanked_console(void);
int con_font_op(struct vc_data *vc, struct console_font_op *op);
int con_set_cmap(unsigned char __user *cmap);
int con_get_cmap(unsigned char __user *cmap);
void scrollback(struct vc_data *vc);
void scrollfront(struct vc_data *vc, int lines);
void clear_buffer_attributes(struct vc_data *vc);
void update_region(struct vc_data *vc, unsigned long start, int count);
void redraw_screen(struct vc_data *vc, int is_switch);
#define update_screen(x) redraw_screen(x, 0)
#define switch_screen(x) redraw_screen(x, 1)

struct tty_struct;
int tioclinux(struct tty_struct *tty, unsigned long arg);

#ifdef CONFIG_CONSOLE_TRANSLATIONS
/* consolemap.c */

struct unipair;

int con_set_trans_old(unsigned char __user * table);
int con_get_trans_old(unsigned char __user * table);
int con_set_trans_new(unsigned short __user * table);
int con_get_trans_new(unsigned short __user * table);
int con_clear_unimap(struct vc_data *vc);
int con_set_unimap(struct vc_data *vc, ushort ct, struct unipair __user *list);
int con_get_unimap(struct vc_data *vc, ushort ct, ushort __user *uct, struct unipair __user *list);
int con_set_default_unimap(struct vc_data *vc);
void con_free_unimap(struct vc_data *vc);
int con_copy_unimap(struct vc_data *dst_vc, struct vc_data *src_vc);

#else
static inline int con_set_trans_old(unsigned char __user *table)
{
	return 0;
}
static inline int con_get_trans_old(unsigned char __user *table)
{
	return -EINVAL;
}
static inline int con_set_trans_new(unsigned short __user *table)
{
	return 0;
}
static inline int con_get_trans_new(unsigned short __user *table)
{
	return -EINVAL;
}
static inline int con_clear_unimap(struct vc_data *vc)
{
	return 0;
}
static inline
int con_set_unimap(struct vc_data *vc, ushort ct, struct unipair __user *list)
{
	return 0;
}
static inline
int con_get_unimap(struct vc_data *vc, ushort ct, ushort __user *uct,
		   struct unipair __user *list)
{
	return -EINVAL;
}
static inline int con_set_default_unimap(struct vc_data *vc)
{
	return 0;
}
static inline void con_free_unimap(struct vc_data *vc)
{
}
static inline void con_protect_unimap(struct vc_data *vc, int rdonly)
{
}
static inline
int con_copy_unimap(struct vc_data *dst_vc, struct vc_data *src_vc)
{
	return 0;
}

#endif

/* vt.c */
void vt_event_post(unsigned int event, unsigned int old, unsigned int new);
int vt_waitactive(int n);
void change_console(struct vc_data *new_vc);
void reset_vc(struct vc_data *vc);
int do_unbind_con_driver(const struct consw *csw, int first, int last,
			 int deflt);
int vty_init(const struct file_operations *console_fops);

extern bool vt_dont_switch;
extern int default_utf8;
extern int global_cursor_default;

struct vt_spawn_console {
	spinlock_t lock;
	struct pid *pid;
	int sig;
};
extern struct vt_spawn_console vt_spawn_con;

int vt_move_to_console(unsigned int vt, int alloc);

/* Interfaces for VC notification of character events (for accessibility etc) */

struct vt_notifier_param {
	struct vc_data *vc;	/* VC on which the update happened */
	unsigned int c;		/* Printed char */
};

int register_vt_notifier(struct notifier_block *nb);
int unregister_vt_notifier(struct notifier_block *nb);

void hide_boot_cursor(bool hide);

/* keyboard  provided interfaces */
int vt_do_diacrit(unsigned int cmd, void __user *up, int eperm);
int vt_do_kdskbmode(int console, unsigned int arg);
int vt_do_kdskbmeta(int console, unsigned int arg);
int vt_do_kbkeycode_ioctl(int cmd, struct kbkeycode __user *user_kbkc,
			  int perm);
int vt_do_kdsk_ioctl(int cmd, struct kbentry __user *user_kbe, int perm,
		     int console);
int vt_do_kdgkb_ioctl(int cmd, struct kbsentry __user *user_kdgkb, int perm);
int vt_do_kdskled(int console, int cmd, unsigned long arg, int perm);
int vt_do_kdgkbmode(int console);
int vt_do_kdgkbmeta(int console);
void vt_reset_unicode(int console);
int vt_get_shift_state(void);
void vt_reset_keyboard(int console);
int vt_get_leds(int console, int flag);
int vt_get_kbd_mode_bit(int console, int bit);
void vt_set_kbd_mode_bit(int console, int bit);
void vt_clr_kbd_mode_bit(int console, int bit);
void vt_set_led_state(int console, int leds);
void vt_set_led_state(int console, int leds);
void vt_kbd_con_start(int console);
void vt_kbd_con_stop(int console);

void vc_scrolldelta_helper(struct vc_data *c, int lines,
		unsigned int rolled_over, void *_base, unsigned int size);

#endif /* _VT_KERN_H */

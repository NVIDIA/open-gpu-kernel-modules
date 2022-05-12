/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KBD_KERN_H
#define _KBD_KERN_H

#include <linux/tty.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>

extern char *func_table[MAX_NR_FUNC];

/*
 * kbd->xxx contains the VC-local things (flag settings etc..)
 *
 * Note: externally visible are LED_SCR, LED_NUM, LED_CAP defined in kd.h
 *       The code in KDGETLED / KDSETLED depends on the internal and
 *       external order being the same.
 *
 * Note: lockstate is used as index in the array key_map.
 */
struct kbd_struct {

	unsigned char lockstate;
/* 8 modifiers - the names do not have any meaning at all;
   they can be associated to arbitrarily chosen keys */
#define VC_SHIFTLOCK	KG_SHIFT	/* shift lock mode */
#define VC_ALTGRLOCK	KG_ALTGR	/* altgr lock mode */
#define VC_CTRLLOCK	KG_CTRL 	/* control lock mode */
#define VC_ALTLOCK	KG_ALT  	/* alt lock mode */
#define VC_SHIFTLLOCK	KG_SHIFTL	/* shiftl lock mode */
#define VC_SHIFTRLOCK	KG_SHIFTR	/* shiftr lock mode */
#define VC_CTRLLLOCK	KG_CTRLL 	/* ctrll lock mode */
#define VC_CTRLRLOCK	KG_CTRLR 	/* ctrlr lock mode */
	unsigned char slockstate; 	/* for `sticky' Shift, Ctrl, etc. */

	unsigned char ledmode:1;
#define LED_SHOW_FLAGS 0        /* traditional state */
#define LED_SHOW_IOCTL 1        /* only change leds upon ioctl */

	unsigned char ledflagstate:4;	/* flags, not lights */
	unsigned char default_ledflagstate:4;
#define VC_SCROLLOCK	0	/* scroll-lock mode */
#define VC_NUMLOCK	1	/* numeric lock mode */
#define VC_CAPSLOCK	2	/* capslock mode */
#define VC_KANALOCK	3	/* kanalock mode */

	unsigned char kbdmode:3;	/* one 3-bit value */
#define VC_XLATE	0	/* translate keycodes using keymap */
#define VC_MEDIUMRAW	1	/* medium raw (keycode) mode */
#define VC_RAW		2	/* raw (scancode) mode */
#define VC_UNICODE	3	/* Unicode mode */
#define VC_OFF		4	/* disabled mode */

	unsigned char modeflags:5;
#define VC_APPLIC	0	/* application key mode */
#define VC_CKMODE	1	/* cursor key mode */
#define VC_REPEAT	2	/* keyboard repeat */
#define VC_CRLF		3	/* 0 - enter sends CR, 1 - enter sends CRLF */
#define VC_META		4	/* 0 - meta, 1 - meta=prefix with ESC */
};

extern int kbd_init(void);

extern void setledstate(struct kbd_struct *kbd, unsigned int led);

extern int do_poke_blanked_console;

extern void (*kbd_ledfunc)(unsigned int led);

extern int set_console(int nr);
extern void schedule_console_callback(void);

static inline int vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
	return ((kbd->modeflags >> flag) & 1);
}

static inline int vc_kbd_led(struct kbd_struct * kbd, int flag)
{
	return ((kbd->ledflagstate >> flag) & 1);
}

static inline void set_vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
	kbd->modeflags |= 1 << flag;
}

static inline void set_vc_kbd_led(struct kbd_struct * kbd, int flag)
{
	kbd->ledflagstate |= 1 << flag;
}

static inline void clr_vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
	kbd->modeflags &= ~(1 << flag);
}

static inline void clr_vc_kbd_led(struct kbd_struct * kbd, int flag)
{
	kbd->ledflagstate &= ~(1 << flag);
}

static inline void chg_vc_kbd_lock(struct kbd_struct * kbd, int flag)
{
	kbd->lockstate ^= 1 << flag;
}

static inline void chg_vc_kbd_slock(struct kbd_struct * kbd, int flag)
{
	kbd->slockstate ^= 1 << flag;
}

static inline void chg_vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
	kbd->modeflags ^= 1 << flag;
}

static inline void chg_vc_kbd_led(struct kbd_struct * kbd, int flag)
{
	kbd->ledflagstate ^= 1 << flag;
}

#define U(x) ((x) ^ 0xf000)

#define BRL_UC_ROW 0x2800

/* keyboard.c */

struct console;

void vt_set_leds_compute_shiftstate(void);

/* defkeymap.c */

extern unsigned int keymap_count;

#endif

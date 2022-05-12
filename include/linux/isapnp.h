/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  ISA Plug & Play support
 *  Copyright (c) by Jaroslav Kysela <perex@suse.cz>
 */

#ifndef LINUX_ISAPNP_H
#define LINUX_ISAPNP_H

#include <linux/errno.h>
#include <linux/pnp.h>

/*
 *
 */

#define ISAPNP_VENDOR(a,b,c)	(((((a)-'A'+1)&0x3f)<<2)|\
				((((b)-'A'+1)&0x18)>>3)|((((b)-'A'+1)&7)<<13)|\
				((((c)-'A'+1)&0x1f)<<8))
#define ISAPNP_DEVICE(x)	((((x)&0xf000)>>8)|\
				 (((x)&0x0f00)>>8)|\
				 (((x)&0x00f0)<<8)|\
				 (((x)&0x000f)<<8))
#define ISAPNP_FUNCTION(x)	ISAPNP_DEVICE(x)

/*
 *
 */

#ifdef __KERNEL__
#include <linux/mod_devicetable.h>

#define DEVICE_COUNT_COMPATIBLE 4

#define ISAPNP_CARD_DEVS	8

#define ISAPNP_CARD_ID(_va, _vb, _vc, _device) \
		.card_vendor = ISAPNP_VENDOR(_va, _vb, _vc), .card_device = ISAPNP_DEVICE(_device)
#define ISAPNP_CARD_END \
		.card_vendor = 0, .card_device = 0
#define ISAPNP_DEVICE_ID(_va, _vb, _vc, _function) \
		{ .vendor = ISAPNP_VENDOR(_va, _vb, _vc), .function = ISAPNP_FUNCTION(_function) }

struct isapnp_card_id {
	unsigned long driver_data;	/* data private to the driver */
	unsigned short card_vendor, card_device;
	struct {
		unsigned short vendor, function;
	} devs[ISAPNP_CARD_DEVS];	/* logical devices */
};

#define ISAPNP_DEVICE_SINGLE(_cva, _cvb, _cvc, _cdevice, _dva, _dvb, _dvc, _dfunction) \
		.card_vendor = ISAPNP_VENDOR(_cva, _cvb, _cvc), .card_device =  ISAPNP_DEVICE(_cdevice), \
		.vendor = ISAPNP_VENDOR(_dva, _dvb, _dvc), .function = ISAPNP_FUNCTION(_dfunction)
#define ISAPNP_DEVICE_SINGLE_END \
		.card_vendor = 0, .card_device = 0

#if defined(CONFIG_ISAPNP) || (defined(CONFIG_ISAPNP_MODULE) && defined(MODULE))

#define __ISAPNP__

/* lowlevel configuration */
int isapnp_present(void);
int isapnp_cfg_begin(int csn, int device);
int isapnp_cfg_end(void);
unsigned char isapnp_read_byte(unsigned char idx);
void isapnp_write_byte(unsigned char idx, unsigned char val);

#ifdef CONFIG_PROC_FS
int isapnp_proc_init(void);
int isapnp_proc_done(void);
#else
static inline int isapnp_proc_init(void) { return 0; }
static inline int isapnp_proc_done(void) { return 0; }
#endif

/* compat */
struct pnp_dev *pnp_find_dev(struct pnp_card *card,
			     unsigned short vendor,
			     unsigned short function,
			     struct pnp_dev *from);

#else /* !CONFIG_ISAPNP */

/* lowlevel configuration */
static inline int isapnp_present(void) { return 0; }
static inline int isapnp_cfg_begin(int csn, int device) { return -ENODEV; }
static inline int isapnp_cfg_end(void) { return -ENODEV; }
static inline unsigned char isapnp_read_byte(unsigned char idx) { return 0xff; }
static inline void isapnp_write_byte(unsigned char idx, unsigned char val) { ; }

static inline struct pnp_dev *pnp_find_dev(struct pnp_card *card,
					   unsigned short vendor,
					   unsigned short function,
					   struct pnp_dev *from) { return NULL; }

#endif /* CONFIG_ISAPNP */

#endif /* __KERNEL__ */
#endif /* LINUX_ISAPNP_H */

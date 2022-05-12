/* SPDX-License-Identifier: GPL-2.0-only */
/* include/linux/sm501.h
 *
 * Copyright (c) 2006 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *	Vincent Sanders <vince@simtec.co.uk>
*/

extern int sm501_unit_power(struct device *dev,
			    unsigned int unit, unsigned int to);

extern unsigned long sm501_set_clock(struct device *dev,
				     int clksrc, unsigned long freq);

extern unsigned long sm501_find_clock(struct device *dev,
				      int clksrc, unsigned long req_freq);

/* sm501_misc_control
 *
 * Modify the SM501's MISC_CONTROL register
*/

extern int sm501_misc_control(struct device *dev,
			      unsigned long set, unsigned long clear);

/* sm501_modify_reg
 *
 * Modify a register in the SM501 which may be shared with other
 * drivers.
*/

extern unsigned long sm501_modify_reg(struct device *dev,
				      unsigned long reg,
				      unsigned long set,
				      unsigned long clear);


/* Platform data definitions */

#define SM501FB_FLAG_USE_INIT_MODE	(1<<0)
#define SM501FB_FLAG_DISABLE_AT_EXIT	(1<<1)
#define SM501FB_FLAG_USE_HWCURSOR	(1<<2)
#define SM501FB_FLAG_USE_HWACCEL	(1<<3)
#define SM501FB_FLAG_PANEL_NO_FPEN	(1<<4)
#define SM501FB_FLAG_PANEL_NO_VBIASEN	(1<<5)
#define SM501FB_FLAG_PANEL_INV_FPEN	(1<<6)
#define SM501FB_FLAG_PANEL_INV_VBIASEN	(1<<7)

struct sm501_platdata_fbsub {
	struct fb_videomode	*def_mode;
	unsigned int		 def_bpp;
	unsigned long		 max_mem;
	unsigned int		 flags;
};

enum sm501_fb_routing {
	SM501_FB_OWN		= 0,	/* CRT=>CRT, Panel=>Panel */
	SM501_FB_CRT_PANEL	= 1,	/* Panel=>CRT, Panel=>Panel */
};

/* sm501_platdata_fb flag field bit definitions */

#define SM501_FBPD_SWAP_FB_ENDIAN	(1<<0)	/* need to endian swap */

/* sm501_platdata_fb
 *
 * configuration data for the framebuffer driver
*/

struct sm501_platdata_fb {
	enum sm501_fb_routing		 fb_route;
	unsigned int			 flags;
	struct sm501_platdata_fbsub	*fb_crt;
	struct sm501_platdata_fbsub	*fb_pnl;
};

/* gpio i2c
 *
 * Note, we have to pass in the bus number, as the number used will be
 * passed to the i2c-gpio driver's platform_device.id, subsequently used
 * to register the i2c bus.
*/

struct sm501_platdata_gpio_i2c {
	unsigned int		bus_num;
	unsigned int		pin_sda;
	unsigned int		pin_scl;
	int			udelay;
	int			timeout;
};

/* sm501_initdata
 *
 * use for initialising values that may not have been setup
 * before the driver is loaded.
*/

struct sm501_reg_init {
	unsigned long		set;
	unsigned long		mask;
};

#define SM501_USE_USB_HOST	(1<<0)
#define SM501_USE_USB_SLAVE	(1<<1)
#define SM501_USE_SSP0		(1<<2)
#define SM501_USE_SSP1		(1<<3)
#define SM501_USE_UART0		(1<<4)
#define SM501_USE_UART1		(1<<5)
#define SM501_USE_FBACCEL	(1<<6)
#define SM501_USE_AC97		(1<<7)
#define SM501_USE_I2S		(1<<8)
#define SM501_USE_GPIO		(1<<9)

#define SM501_USE_ALL		(0xffffffff)

struct sm501_initdata {
	struct sm501_reg_init	gpio_low;
	struct sm501_reg_init	gpio_high;
	struct sm501_reg_init	misc_timing;
	struct sm501_reg_init	misc_control;

	unsigned long		devices;
	unsigned long		mclk;		/* non-zero to modify */
	unsigned long		m1xclk;		/* non-zero to modify */
};

/* sm501_init_gpio
 *
 * default gpio settings
*/

struct sm501_init_gpio {
	struct sm501_reg_init	gpio_data_low;
	struct sm501_reg_init	gpio_data_high;
	struct sm501_reg_init	gpio_ddr_low;
	struct sm501_reg_init	gpio_ddr_high;
};

#define SM501_FLAG_SUSPEND_OFF		(1<<4)

/* sm501_platdata
 *
 * This is passed with the platform device to allow the board
 * to control the behaviour of the SM501 driver(s) which attach
 * to the device.
 *
*/

struct sm501_platdata {
	struct sm501_initdata		*init;
	struct sm501_init_gpio		*init_gpiop;
	struct sm501_platdata_fb	*fb;

	int				 flags;
	int				 gpio_base;

	int	(*get_power)(struct device *dev);
	int	(*set_power)(struct device *dev, unsigned int on);

	struct sm501_platdata_gpio_i2c	*gpio_i2c;
	unsigned int			 gpio_i2c_nr;
};

#if defined(CONFIG_PPC32)
#define smc501_readl(addr)		ioread32be((addr))
#define smc501_writel(val, addr)	iowrite32be((val), (addr))
#else
#define smc501_readl(addr)		readl(addr)
#define smc501_writel(val, addr)	writel(val, addr)
#endif

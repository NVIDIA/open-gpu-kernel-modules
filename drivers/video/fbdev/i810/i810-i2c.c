 /*-*- linux-c -*-
 *  linux/drivers/video/i810-i2c.c -- Intel 810/815 I2C support
 *
 *      Copyright (C) 2004 Antonino Daplas<adaplas@pol.net>
 *      All Rights Reserved
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gfp.h>
#include <linux/pci.h>
#include <linux/fb.h>
#include "i810.h"
#include "i810_regs.h"
#include "i810_main.h"
#include "../edid.h"

/* bit locations in the registers */
#define SCL_DIR_MASK		0x0001
#define SCL_DIR			0x0002
#define SCL_VAL_MASK		0x0004
#define SCL_VAL_OUT		0x0008
#define SCL_VAL_IN		0x0010
#define SDA_DIR_MASK		0x0100
#define SDA_DIR			0x0200
#define SDA_VAL_MASK		0x0400
#define SDA_VAL_OUT		0x0800
#define SDA_VAL_IN		0x1000

#define DEBUG  /* define this for verbose EDID parsing output */

#ifdef DEBUG
#define DPRINTK(fmt, args...) printk(fmt,## args)
#else
#define DPRINTK(fmt, args...)
#endif

static void i810i2c_setscl(void *data, int state)
{
        struct i810fb_i2c_chan    *chan = data;
        struct i810fb_par         *par = chan->par;
	u8                        __iomem *mmio = par->mmio_start_virtual;

	if (state)
		i810_writel(mmio, chan->ddc_base, SCL_DIR_MASK | SCL_VAL_MASK);
	else
		i810_writel(mmio, chan->ddc_base, SCL_DIR | SCL_DIR_MASK | SCL_VAL_MASK);
	i810_readl(mmio, chan->ddc_base);	/* flush posted write */
}

static void i810i2c_setsda(void *data, int state)
{
        struct i810fb_i2c_chan    *chan = data;
        struct i810fb_par         *par = chan->par;
	u8                        __iomem *mmio = par->mmio_start_virtual;

	if (state)
		i810_writel(mmio, chan->ddc_base, SDA_DIR_MASK | SDA_VAL_MASK);
	else
		i810_writel(mmio, chan->ddc_base, SDA_DIR | SDA_DIR_MASK | SDA_VAL_MASK);
	i810_readl(mmio, chan->ddc_base);	/* flush posted write */
}

static int i810i2c_getscl(void *data)
{
        struct i810fb_i2c_chan    *chan = data;
        struct i810fb_par         *par = chan->par;
	u8                        __iomem *mmio = par->mmio_start_virtual;

	i810_writel(mmio, chan->ddc_base, SCL_DIR_MASK);
	i810_writel(mmio, chan->ddc_base, 0);
	return ((i810_readl(mmio, chan->ddc_base) & SCL_VAL_IN) != 0);
}

static int i810i2c_getsda(void *data)
{
        struct i810fb_i2c_chan    *chan = data;
        struct i810fb_par         *par = chan->par;
	u8                        __iomem *mmio = par->mmio_start_virtual;

	i810_writel(mmio, chan->ddc_base, SDA_DIR_MASK);
	i810_writel(mmio, chan->ddc_base, 0);
	return ((i810_readl(mmio, chan->ddc_base) & SDA_VAL_IN) != 0);
}

static int i810_setup_i2c_bus(struct i810fb_i2c_chan *chan, const char *name)
{
        int rc;

        strcpy(chan->adapter.name, name);
        chan->adapter.owner             = THIS_MODULE;
        chan->adapter.algo_data         = &chan->algo;
        chan->adapter.dev.parent        = &chan->par->dev->dev;
	chan->algo.setsda               = i810i2c_setsda;
	chan->algo.setscl               = i810i2c_setscl;
	chan->algo.getsda               = i810i2c_getsda;
	chan->algo.getscl               = i810i2c_getscl;
	chan->algo.udelay               = 10;
        chan->algo.timeout              = (HZ/2);
        chan->algo.data                 = chan;

        i2c_set_adapdata(&chan->adapter, chan);

        /* Raise SCL and SDA */
        chan->algo.setsda(chan, 1);
        chan->algo.setscl(chan, 1);
        udelay(20);

        rc = i2c_bit_add_bus(&chan->adapter);

        if (rc == 0)
                dev_dbg(&chan->par->dev->dev, "I2C bus %s registered.\n",name);
        else {
                dev_warn(&chan->par->dev->dev, "Failed to register I2C bus "
			 "%s.\n", name);
		chan->par = NULL;
	}

        return rc;
}

void i810_create_i2c_busses(struct i810fb_par *par)
{
        par->chan[0].par        = par;
	par->chan[1].par        = par;
	par->chan[2].par        = par;

	par->chan[0].ddc_base = GPIOA;
	i810_setup_i2c_bus(&par->chan[0], "I810-DDC");
	par->chan[1].ddc_base = GPIOB;
	i810_setup_i2c_bus(&par->chan[1], "I810-I2C");
	par->chan[2].ddc_base = GPIOC;
	i810_setup_i2c_bus(&par->chan[2], "I810-GPIOC");
}

void i810_delete_i2c_busses(struct i810fb_par *par)
{
        if (par->chan[0].par)
		i2c_del_adapter(&par->chan[0].adapter);
        par->chan[0].par = NULL;

	if (par->chan[1].par)
		i2c_del_adapter(&par->chan[1].adapter);
	par->chan[1].par = NULL;

	if (par->chan[2].par)
		i2c_del_adapter(&par->chan[2].adapter);
	par->chan[2].par = NULL;
}

int i810_probe_i2c_connector(struct fb_info *info, u8 **out_edid, int conn)
{
	struct i810fb_par *par = info->par;
        u8 *edid = NULL;

	DPRINTK("i810-i2c: Probe DDC%i Bus\n", conn+1);
	if (conn < par->ddc_num) {
		edid = fb_ddc_read(&par->chan[conn].adapter);
	} else {
		const u8 *e = fb_firmware_edid(info->device);

		if (e != NULL) {
			DPRINTK("i810-i2c: Getting EDID from BIOS\n");
			edid = kmemdup(e, EDID_LENGTH, GFP_KERNEL);
		}
	}

	*out_edid = edid;

        return (edid) ? 0 : 1;
}

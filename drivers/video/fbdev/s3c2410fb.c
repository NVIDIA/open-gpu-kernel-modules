/* linux/drivers/video/s3c2410fb.c
 *	Copyright (c) 2004,2005 Arnaud Patard
 *	Copyright (c) 2004-2008 Ben Dooks
 *
 * S3C2410 LCD Framebuffer Driver
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
 * Driver based on skeletonfb.c, sa1100fb.c and others.
*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/io.h>
#include <linux/platform_data/fb-s3c2410.h>

#include <asm/div64.h>

#include <asm/mach/map.h>

#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#include "s3c2410fb.h"
#include "s3c2410fb-regs-lcd.h"

/* Debugging stuff */
static int debug = IS_BUILTIN(CONFIG_FB_S3C2410_DEBUG);

#define dprintk(msg...) \
do { \
	if (debug) \
		pr_debug(msg); \
} while (0)

/* useful functions */

static int is_s3c2412(struct s3c2410fb_info *fbi)
{
	return (fbi->drv_type == DRV_S3C2412);
}

/* s3c2410fb_set_lcdaddr
 *
 * initialise lcd controller address pointers
 */
static void s3c2410fb_set_lcdaddr(struct fb_info *info)
{
	unsigned long saddr1, saddr2, saddr3;
	struct s3c2410fb_info *fbi = info->par;
	void __iomem *regs = fbi->io;

	saddr1  = info->fix.smem_start >> 1;
	saddr2  = info->fix.smem_start;
	saddr2 += info->fix.line_length * info->var.yres;
	saddr2 >>= 1;

	saddr3 = S3C2410_OFFSIZE(0) |
		 S3C2410_PAGEWIDTH((info->fix.line_length / 2) & 0x3ff);

	dprintk("LCDSADDR1 = 0x%08lx\n", saddr1);
	dprintk("LCDSADDR2 = 0x%08lx\n", saddr2);
	dprintk("LCDSADDR3 = 0x%08lx\n", saddr3);

	writel(saddr1, regs + S3C2410_LCDSADDR1);
	writel(saddr2, regs + S3C2410_LCDSADDR2);
	writel(saddr3, regs + S3C2410_LCDSADDR3);
}

/* s3c2410fb_calc_pixclk()
 *
 * calculate divisor for clk->pixclk
 */
static unsigned int s3c2410fb_calc_pixclk(struct s3c2410fb_info *fbi,
					  unsigned long pixclk)
{
	unsigned long clk = fbi->clk_rate;
	unsigned long long div;

	/* pixclk is in picoseconds, our clock is in Hz
	 *
	 * Hz -> picoseconds is / 10^-12
	 */

	div = (unsigned long long)clk * pixclk;
	div >>= 12;			/* div / 2^12 */
	do_div(div, 625 * 625UL * 625); /* div / 5^12 */

	dprintk("pixclk %ld, divisor is %ld\n", pixclk, (long)div);
	return div;
}

/*
 *	s3c2410fb_check_var():
 *	Get the video params out of 'var'. If a value doesn't fit, round it up,
 *	if it's too big, return -EINVAL.
 *
 */
static int s3c2410fb_check_var(struct fb_var_screeninfo *var,
			       struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;
	struct s3c2410fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	struct s3c2410fb_display *display = NULL;
	struct s3c2410fb_display *default_display = mach_info->displays +
						    mach_info->default_display;
	int type = default_display->type;
	unsigned i;

	dprintk("check_var(var=%p, info=%p)\n", var, info);

	/* validate x/y resolution */
	/* choose default mode if possible */
	if (var->yres == default_display->yres &&
	    var->xres == default_display->xres &&
	    var->bits_per_pixel == default_display->bpp)
		display = default_display;
	else
		for (i = 0; i < mach_info->num_displays; i++)
			if (type == mach_info->displays[i].type &&
			    var->yres == mach_info->displays[i].yres &&
			    var->xres == mach_info->displays[i].xres &&
			    var->bits_per_pixel == mach_info->displays[i].bpp) {
				display = mach_info->displays + i;
				break;
			}

	if (!display) {
		dprintk("wrong resolution or depth %dx%d at %d bpp\n",
			var->xres, var->yres, var->bits_per_pixel);
		return -EINVAL;
	}

	/* it is always the size as the display */
	var->xres_virtual = display->xres;
	var->yres_virtual = display->yres;
	var->height = display->height;
	var->width = display->width;

	/* copy lcd settings */
	var->pixclock = display->pixclock;
	var->left_margin = display->left_margin;
	var->right_margin = display->right_margin;
	var->upper_margin = display->upper_margin;
	var->lower_margin = display->lower_margin;
	var->vsync_len = display->vsync_len;
	var->hsync_len = display->hsync_len;

	fbi->regs.lcdcon5 = display->lcdcon5;
	/* set display type */
	fbi->regs.lcdcon1 = display->type;

	var->transp.offset = 0;
	var->transp.length = 0;
	/* set r/g/b positions */
	switch (var->bits_per_pixel) {
	case 1:
	case 2:
	case 4:
		var->red.offset	= 0;
		var->red.length	= var->bits_per_pixel;
		var->green	= var->red;
		var->blue	= var->red;
		break;
	case 8:
		if (display->type != S3C2410_LCDCON1_TFT) {
			/* 8 bpp 332 */
			var->red.length		= 3;
			var->red.offset		= 5;
			var->green.length	= 3;
			var->green.offset	= 2;
			var->blue.length	= 2;
			var->blue.offset	= 0;
		} else {
			var->red.offset		= 0;
			var->red.length		= 8;
			var->green		= var->red;
			var->blue		= var->red;
		}
		break;
	case 12:
		/* 12 bpp 444 */
		var->red.length		= 4;
		var->red.offset		= 8;
		var->green.length	= 4;
		var->green.offset	= 4;
		var->blue.length	= 4;
		var->blue.offset	= 0;
		break;

	default:
	case 16:
		if (display->lcdcon5 & S3C2410_LCDCON5_FRM565) {
			/* 16 bpp, 565 format */
			var->red.offset		= 11;
			var->green.offset	= 5;
			var->blue.offset	= 0;
			var->red.length		= 5;
			var->green.length	= 6;
			var->blue.length	= 5;
		} else {
			/* 16 bpp, 5551 format */
			var->red.offset		= 11;
			var->green.offset	= 6;
			var->blue.offset	= 1;
			var->red.length		= 5;
			var->green.length	= 5;
			var->blue.length	= 5;
		}
		break;
	case 32:
		/* 24 bpp 888 and 8 dummy */
		var->red.length		= 8;
		var->red.offset		= 16;
		var->green.length	= 8;
		var->green.offset	= 8;
		var->blue.length	= 8;
		var->blue.offset	= 0;
		break;
	}
	return 0;
}

/* s3c2410fb_calculate_stn_lcd_regs
 *
 * calculate register values from var settings
 */
static void s3c2410fb_calculate_stn_lcd_regs(const struct fb_info *info,
					     struct s3c2410fb_hw *regs)
{
	const struct s3c2410fb_info *fbi = info->par;
	const struct fb_var_screeninfo *var = &info->var;
	int type = regs->lcdcon1 & ~S3C2410_LCDCON1_TFT;
	int hs = var->xres >> 2;
	unsigned wdly = (var->left_margin >> 4) - 1;
	unsigned wlh = (var->hsync_len >> 4) - 1;

	if (type != S3C2410_LCDCON1_STN4)
		hs >>= 1;

	switch (var->bits_per_pixel) {
	case 1:
		regs->lcdcon1 |= S3C2410_LCDCON1_STN1BPP;
		break;
	case 2:
		regs->lcdcon1 |= S3C2410_LCDCON1_STN2GREY;
		break;
	case 4:
		regs->lcdcon1 |= S3C2410_LCDCON1_STN4GREY;
		break;
	case 8:
		regs->lcdcon1 |= S3C2410_LCDCON1_STN8BPP;
		hs *= 3;
		break;
	case 12:
		regs->lcdcon1 |= S3C2410_LCDCON1_STN12BPP;
		hs *= 3;
		break;

	default:
		/* invalid pixel depth */
		dev_err(fbi->dev, "invalid bpp %d\n",
			var->bits_per_pixel);
	}
	/* update X/Y info */
	dprintk("setting horz: lft=%d, rt=%d, sync=%d\n",
		var->left_margin, var->right_margin, var->hsync_len);

	regs->lcdcon2 = S3C2410_LCDCON2_LINEVAL(var->yres - 1);

	if (wdly > 3)
		wdly = 3;

	if (wlh > 3)
		wlh = 3;

	regs->lcdcon3 =	S3C2410_LCDCON3_WDLY(wdly) |
			S3C2410_LCDCON3_LINEBLANK(var->right_margin / 8) |
			S3C2410_LCDCON3_HOZVAL(hs - 1);

	regs->lcdcon4 = S3C2410_LCDCON4_WLH(wlh);
}

/* s3c2410fb_calculate_tft_lcd_regs
 *
 * calculate register values from var settings
 */
static void s3c2410fb_calculate_tft_lcd_regs(const struct fb_info *info,
					     struct s3c2410fb_hw *regs)
{
	const struct s3c2410fb_info *fbi = info->par;
	const struct fb_var_screeninfo *var = &info->var;

	switch (var->bits_per_pixel) {
	case 1:
		regs->lcdcon1 |= S3C2410_LCDCON1_TFT1BPP;
		break;
	case 2:
		regs->lcdcon1 |= S3C2410_LCDCON1_TFT2BPP;
		break;
	case 4:
		regs->lcdcon1 |= S3C2410_LCDCON1_TFT4BPP;
		break;
	case 8:
		regs->lcdcon1 |= S3C2410_LCDCON1_TFT8BPP;
		regs->lcdcon5 |= S3C2410_LCDCON5_BSWP |
				 S3C2410_LCDCON5_FRM565;
		regs->lcdcon5 &= ~S3C2410_LCDCON5_HWSWP;
		break;
	case 16:
		regs->lcdcon1 |= S3C2410_LCDCON1_TFT16BPP;
		regs->lcdcon5 &= ~S3C2410_LCDCON5_BSWP;
		regs->lcdcon5 |= S3C2410_LCDCON5_HWSWP;
		break;
	case 32:
		regs->lcdcon1 |= S3C2410_LCDCON1_TFT24BPP;
		regs->lcdcon5 &= ~(S3C2410_LCDCON5_BSWP |
				   S3C2410_LCDCON5_HWSWP |
				   S3C2410_LCDCON5_BPP24BL);
		break;
	default:
		/* invalid pixel depth */
		dev_err(fbi->dev, "invalid bpp %d\n",
			var->bits_per_pixel);
	}
	/* update X/Y info */
	dprintk("setting vert: up=%d, low=%d, sync=%d\n",
		var->upper_margin, var->lower_margin, var->vsync_len);

	dprintk("setting horz: lft=%d, rt=%d, sync=%d\n",
		var->left_margin, var->right_margin, var->hsync_len);

	regs->lcdcon2 = S3C2410_LCDCON2_LINEVAL(var->yres - 1) |
			S3C2410_LCDCON2_VBPD(var->upper_margin - 1) |
			S3C2410_LCDCON2_VFPD(var->lower_margin - 1) |
			S3C2410_LCDCON2_VSPW(var->vsync_len - 1);

	regs->lcdcon3 = S3C2410_LCDCON3_HBPD(var->right_margin - 1) |
			S3C2410_LCDCON3_HFPD(var->left_margin - 1) |
			S3C2410_LCDCON3_HOZVAL(var->xres - 1);

	regs->lcdcon4 = S3C2410_LCDCON4_HSPW(var->hsync_len - 1);
}

/* s3c2410fb_activate_var
 *
 * activate (set) the controller from the given framebuffer
 * information
 */
static void s3c2410fb_activate_var(struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;
	void __iomem *regs = fbi->io;
	int type = fbi->regs.lcdcon1 & S3C2410_LCDCON1_TFT;
	struct fb_var_screeninfo *var = &info->var;
	int clkdiv;

	clkdiv = DIV_ROUND_UP(s3c2410fb_calc_pixclk(fbi, var->pixclock), 2);

	dprintk("%s: var->xres  = %d\n", __func__, var->xres);
	dprintk("%s: var->yres  = %d\n", __func__, var->yres);
	dprintk("%s: var->bpp   = %d\n", __func__, var->bits_per_pixel);

	if (type == S3C2410_LCDCON1_TFT) {
		s3c2410fb_calculate_tft_lcd_regs(info, &fbi->regs);
		--clkdiv;
		if (clkdiv < 0)
			clkdiv = 0;
	} else {
		s3c2410fb_calculate_stn_lcd_regs(info, &fbi->regs);
		if (clkdiv < 2)
			clkdiv = 2;
	}

	fbi->regs.lcdcon1 |=  S3C2410_LCDCON1_CLKVAL(clkdiv);

	/* write new registers */

	dprintk("new register set:\n");
	dprintk("lcdcon[1] = 0x%08lx\n", fbi->regs.lcdcon1);
	dprintk("lcdcon[2] = 0x%08lx\n", fbi->regs.lcdcon2);
	dprintk("lcdcon[3] = 0x%08lx\n", fbi->regs.lcdcon3);
	dprintk("lcdcon[4] = 0x%08lx\n", fbi->regs.lcdcon4);
	dprintk("lcdcon[5] = 0x%08lx\n", fbi->regs.lcdcon5);

	writel(fbi->regs.lcdcon1 & ~S3C2410_LCDCON1_ENVID,
		regs + S3C2410_LCDCON1);
	writel(fbi->regs.lcdcon2, regs + S3C2410_LCDCON2);
	writel(fbi->regs.lcdcon3, regs + S3C2410_LCDCON3);
	writel(fbi->regs.lcdcon4, regs + S3C2410_LCDCON4);
	writel(fbi->regs.lcdcon5, regs + S3C2410_LCDCON5);

	/* set lcd address pointers */
	s3c2410fb_set_lcdaddr(info);

	fbi->regs.lcdcon1 |= S3C2410_LCDCON1_ENVID,
	writel(fbi->regs.lcdcon1, regs + S3C2410_LCDCON1);
}

/*
 *      s3c2410fb_set_par - Alters the hardware state.
 *      @info: frame buffer structure that represents a single frame buffer
 *
 */
static int s3c2410fb_set_par(struct fb_info *info)
{
	struct fb_var_screeninfo *var = &info->var;

	switch (var->bits_per_pixel) {
	case 32:
	case 16:
	case 12:
		info->fix.visual = FB_VISUAL_TRUECOLOR;
		break;
	case 1:
		info->fix.visual = FB_VISUAL_MONO01;
		break;
	default:
		info->fix.visual = FB_VISUAL_PSEUDOCOLOR;
		break;
	}

	info->fix.line_length = (var->xres_virtual * var->bits_per_pixel) / 8;

	/* activate this new configuration */

	s3c2410fb_activate_var(info);
	return 0;
}

static void schedule_palette_update(struct s3c2410fb_info *fbi,
				    unsigned int regno, unsigned int val)
{
	unsigned long flags;
	unsigned long irqen;
	void __iomem *irq_base = fbi->irq_base;

	local_irq_save(flags);

	fbi->palette_buffer[regno] = val;

	if (!fbi->palette_ready) {
		fbi->palette_ready = 1;

		/* enable IRQ */
		irqen = readl(irq_base + S3C24XX_LCDINTMSK);
		irqen &= ~S3C2410_LCDINT_FRSYNC;
		writel(irqen, irq_base + S3C24XX_LCDINTMSK);
	}

	local_irq_restore(flags);
}

/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan,
					 struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int s3c2410fb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;
	void __iomem *regs = fbi->io;
	unsigned int val;

	/* dprintk("setcol: regno=%d, rgb=%d,%d,%d\n",
		   regno, red, green, blue); */

	switch (info->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
		/* true-colour, use pseudo-palette */

		if (regno < 16) {
			u32 *pal = info->pseudo_palette;

			val  = chan_to_field(red,   &info->var.red);
			val |= chan_to_field(green, &info->var.green);
			val |= chan_to_field(blue,  &info->var.blue);

			pal[regno] = val;
		}
		break;

	case FB_VISUAL_PSEUDOCOLOR:
		if (regno < 256) {
			/* currently assume RGB 5-6-5 mode */

			val  = (red   >>  0) & 0xf800;
			val |= (green >>  5) & 0x07e0;
			val |= (blue  >> 11) & 0x001f;

			writel(val, regs + S3C2410_TFTPAL(regno));
			schedule_palette_update(fbi, regno, val);
		}

		break;

	default:
		return 1;	/* unknown type */
	}

	return 0;
}

/* s3c2410fb_lcd_enable
 *
 * shutdown the lcd controller
 */
static void s3c2410fb_lcd_enable(struct s3c2410fb_info *fbi, int enable)
{
	unsigned long flags;

	local_irq_save(flags);

	if (enable)
		fbi->regs.lcdcon1 |= S3C2410_LCDCON1_ENVID;
	else
		fbi->regs.lcdcon1 &= ~S3C2410_LCDCON1_ENVID;

	writel(fbi->regs.lcdcon1, fbi->io + S3C2410_LCDCON1);

	local_irq_restore(flags);
}


/*
 *      s3c2410fb_blank
 *	@blank_mode: the blank mode we want.
 *	@info: frame buffer structure that represents a single frame buffer
 *
 *	Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *	video mode which doesn't support it. Implements VESA suspend
 *	and powerdown modes on hardware that supports disabling hsync/vsync:
 *
 *	Returns negative errno on error, or zero on success.
 *
 */
static int s3c2410fb_blank(int blank_mode, struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;
	void __iomem *tpal_reg = fbi->io;

	dprintk("blank(mode=%d, info=%p)\n", blank_mode, info);

	tpal_reg += is_s3c2412(fbi) ? S3C2412_TPAL : S3C2410_TPAL;

	if (blank_mode == FB_BLANK_POWERDOWN)
		s3c2410fb_lcd_enable(fbi, 0);
	else
		s3c2410fb_lcd_enable(fbi, 1);

	if (blank_mode == FB_BLANK_UNBLANK)
		writel(0x0, tpal_reg);
	else {
		dprintk("setting TPAL to output 0x000000\n");
		writel(S3C2410_TPAL_EN, tpal_reg);
	}

	return 0;
}

static int s3c2410fb_debug_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", debug ? "on" : "off");
}

static int s3c2410fb_debug_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t len)
{
	if (len < 1)
		return -EINVAL;

	if (strncasecmp(buf, "on", 2) == 0 ||
	    strncasecmp(buf, "1", 1) == 0) {
		debug = 1;
		dev_dbg(dev, "s3c2410fb: Debug On");
	} else if (strncasecmp(buf, "off", 3) == 0 ||
		   strncasecmp(buf, "0", 1) == 0) {
		debug = 0;
		dev_dbg(dev, "s3c2410fb: Debug Off");
	} else {
		return -EINVAL;
	}

	return len;
}

static DEVICE_ATTR(debug, 0664, s3c2410fb_debug_show, s3c2410fb_debug_store);

static const struct fb_ops s3c2410fb_ops = {
	.owner		= THIS_MODULE,
	.fb_check_var	= s3c2410fb_check_var,
	.fb_set_par	= s3c2410fb_set_par,
	.fb_blank	= s3c2410fb_blank,
	.fb_setcolreg	= s3c2410fb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

/*
 * s3c2410fb_map_video_memory():
 *	Allocates the DRAM memory for the frame buffer.  This buffer is
 *	remapped into a non-cached, non-buffered, memory region to
 *	allow palette and pixel writes to occur without flushing the
 *	cache.  Once this area is remapped, all virtual memory
 *	access to the video memory should occur at the new region.
 */
static int s3c2410fb_map_video_memory(struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;
	dma_addr_t map_dma;
	unsigned map_size = PAGE_ALIGN(info->fix.smem_len);

	dprintk("map_video_memory(fbi=%p) map_size %u\n", fbi, map_size);

	info->screen_base = dma_alloc_wc(fbi->dev, map_size, &map_dma,
					 GFP_KERNEL);

	if (info->screen_base) {
		/* prevent initial garbage on screen */
		dprintk("map_video_memory: clear %p:%08x\n",
			info->screen_base, map_size);
		memset(info->screen_base, 0x00, map_size);

		info->fix.smem_start = map_dma;

		dprintk("map_video_memory: dma=%08lx cpu=%p size=%08x\n",
			info->fix.smem_start, info->screen_base, map_size);
	}

	return info->screen_base ? 0 : -ENOMEM;
}

static inline void s3c2410fb_unmap_video_memory(struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;

	dma_free_wc(fbi->dev, PAGE_ALIGN(info->fix.smem_len),
		    info->screen_base, info->fix.smem_start);
}

static inline void modify_gpio(void __iomem *reg,
			       unsigned long set, unsigned long mask)
{
	unsigned long tmp;

	if (!reg)
		return;

	tmp = readl(reg) & ~mask;
	writel(tmp | set, reg);
}

/*
 * s3c2410fb_init_registers - Initialise all LCD-related registers
 */
static int s3c2410fb_init_registers(struct fb_info *info)
{
	struct s3c2410fb_info *fbi = info->par;
	struct s3c2410fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	unsigned long flags;
	void __iomem *regs = fbi->io;
	void __iomem *tpal;
	void __iomem *lpcsel;

	if (is_s3c2412(fbi)) {
		tpal = regs + S3C2412_TPAL;
		lpcsel = regs + S3C2412_TCONSEL;
	} else {
		tpal = regs + S3C2410_TPAL;
		lpcsel = regs + S3C2410_LPCSEL;
	}

	/* Initialise LCD with values from haret */

	local_irq_save(flags);

	/* modify the gpio(s) with interrupts set (bjd) */

	modify_gpio(mach_info->gpcup_reg,  mach_info->gpcup,  mach_info->gpcup_mask);
	modify_gpio(mach_info->gpccon_reg, mach_info->gpccon, mach_info->gpccon_mask);
	modify_gpio(mach_info->gpdup_reg,  mach_info->gpdup,  mach_info->gpdup_mask);
	modify_gpio(mach_info->gpdcon_reg, mach_info->gpdcon, mach_info->gpdcon_mask);

	local_irq_restore(flags);

	dprintk("LPCSEL    = 0x%08lx\n", mach_info->lpcsel);
	writel(mach_info->lpcsel, lpcsel);

	dprintk("replacing TPAL %08x\n", readl(tpal));

	/* ensure temporary palette disabled */
	writel(0x00, tpal);

	return 0;
}

static void s3c2410fb_write_palette(struct s3c2410fb_info *fbi)
{
	unsigned int i;
	void __iomem *regs = fbi->io;

	fbi->palette_ready = 0;

	for (i = 0; i < 256; i++) {
		unsigned long ent = fbi->palette_buffer[i];
		if (ent == PALETTE_BUFF_CLEAR)
			continue;

		writel(ent, regs + S3C2410_TFTPAL(i));

		/* it seems the only way to know exactly
		 * if the palette wrote ok, is to check
		 * to see if the value verifies ok
		 */

		if (readw(regs + S3C2410_TFTPAL(i)) == ent)
			fbi->palette_buffer[i] = PALETTE_BUFF_CLEAR;
		else
			fbi->palette_ready = 1;   /* retry */
	}
}

static irqreturn_t s3c2410fb_irq(int irq, void *dev_id)
{
	struct s3c2410fb_info *fbi = dev_id;
	void __iomem *irq_base = fbi->irq_base;
	unsigned long lcdirq = readl(irq_base + S3C24XX_LCDINTPND);

	if (lcdirq & S3C2410_LCDINT_FRSYNC) {
		if (fbi->palette_ready)
			s3c2410fb_write_palette(fbi);

		writel(S3C2410_LCDINT_FRSYNC, irq_base + S3C24XX_LCDINTPND);
		writel(S3C2410_LCDINT_FRSYNC, irq_base + S3C24XX_LCDSRCPND);
	}

	return IRQ_HANDLED;
}

#ifdef CONFIG_ARM_S3C24XX_CPUFREQ

static int s3c2410fb_cpufreq_transition(struct notifier_block *nb,
					unsigned long val, void *data)
{
	struct s3c2410fb_info *info;
	struct fb_info *fbinfo;
	long delta_f;

	info = container_of(nb, struct s3c2410fb_info, freq_transition);
	fbinfo = dev_get_drvdata(info->dev);

	/* work out change, <0 for speed-up */
	delta_f = info->clk_rate - clk_get_rate(info->clk);

	if ((val == CPUFREQ_POSTCHANGE && delta_f > 0) ||
	    (val == CPUFREQ_PRECHANGE && delta_f < 0)) {
		info->clk_rate = clk_get_rate(info->clk);
		s3c2410fb_activate_var(fbinfo);
	}

	return 0;
}

static inline int s3c2410fb_cpufreq_register(struct s3c2410fb_info *info)
{
	info->freq_transition.notifier_call = s3c2410fb_cpufreq_transition;

	return cpufreq_register_notifier(&info->freq_transition,
					 CPUFREQ_TRANSITION_NOTIFIER);
}

static inline void s3c2410fb_cpufreq_deregister(struct s3c2410fb_info *info)
{
	cpufreq_unregister_notifier(&info->freq_transition,
				    CPUFREQ_TRANSITION_NOTIFIER);
}

#else
static inline int s3c2410fb_cpufreq_register(struct s3c2410fb_info *info)
{
	return 0;
}

static inline void s3c2410fb_cpufreq_deregister(struct s3c2410fb_info *info)
{
}
#endif


static const char driver_name[] = "s3c2410fb";

static int s3c24xxfb_probe(struct platform_device *pdev,
			   enum s3c_drv_type drv_type)
{
	struct s3c2410fb_info *info;
	struct s3c2410fb_display *display;
	struct fb_info *fbinfo;
	struct s3c2410fb_mach_info *mach_info;
	struct resource *res;
	int ret;
	int irq;
	int i;
	int size;
	u32 lcdcon1;

	mach_info = dev_get_platdata(&pdev->dev);
	if (mach_info == NULL) {
		dev_err(&pdev->dev,
			"no platform data for lcd, cannot attach\n");
		return -EINVAL;
	}

	if (mach_info->default_display >= mach_info->num_displays) {
		dev_err(&pdev->dev, "default is %d but only %d displays\n",
			mach_info->default_display, mach_info->num_displays);
		return -EINVAL;
	}

	display = mach_info->displays + mach_info->default_display;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no irq for device\n");
		return -ENOENT;
	}

	fbinfo = framebuffer_alloc(sizeof(struct s3c2410fb_info), &pdev->dev);
	if (!fbinfo)
		return -ENOMEM;

	platform_set_drvdata(pdev, fbinfo);

	info = fbinfo->par;
	info->dev = &pdev->dev;
	info->drv_type = drv_type;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to get memory registers\n");
		ret = -ENXIO;
		goto dealloc_fb;
	}

	size = resource_size(res);
	info->mem = request_mem_region(res->start, size, pdev->name);
	if (info->mem == NULL) {
		dev_err(&pdev->dev, "failed to get memory region\n");
		ret = -ENOENT;
		goto dealloc_fb;
	}

	info->io = ioremap(res->start, size);
	if (info->io == NULL) {
		dev_err(&pdev->dev, "ioremap() of registers failed\n");
		ret = -ENXIO;
		goto release_mem;
	}

	if (drv_type == DRV_S3C2412)
		info->irq_base = info->io + S3C2412_LCDINTBASE;
	else
		info->irq_base = info->io + S3C2410_LCDINTBASE;

	dprintk("devinit\n");

	strcpy(fbinfo->fix.id, driver_name);

	/* Stop the video */
	lcdcon1 = readl(info->io + S3C2410_LCDCON1);
	writel(lcdcon1 & ~S3C2410_LCDCON1_ENVID, info->io + S3C2410_LCDCON1);

	fbinfo->fix.type	    = FB_TYPE_PACKED_PIXELS;
	fbinfo->fix.type_aux	    = 0;
	fbinfo->fix.xpanstep	    = 0;
	fbinfo->fix.ypanstep	    = 0;
	fbinfo->fix.ywrapstep	    = 0;
	fbinfo->fix.accel	    = FB_ACCEL_NONE;

	fbinfo->var.nonstd	    = 0;
	fbinfo->var.activate	    = FB_ACTIVATE_NOW;
	fbinfo->var.accel_flags     = 0;
	fbinfo->var.vmode	    = FB_VMODE_NONINTERLACED;

	fbinfo->fbops		    = &s3c2410fb_ops;
	fbinfo->flags		    = FBINFO_FLAG_DEFAULT;
	fbinfo->pseudo_palette      = &info->pseudo_pal;

	for (i = 0; i < 256; i++)
		info->palette_buffer[i] = PALETTE_BUFF_CLEAR;

	ret = request_irq(irq, s3c2410fb_irq, 0, pdev->name, info);
	if (ret) {
		dev_err(&pdev->dev, "cannot get irq %d - err %d\n", irq, ret);
		ret = -EBUSY;
		goto release_regs;
	}

	info->clk = clk_get(NULL, "lcd");
	if (IS_ERR(info->clk)) {
		dev_err(&pdev->dev, "failed to get lcd clock source\n");
		ret = PTR_ERR(info->clk);
		goto release_irq;
	}

	clk_prepare_enable(info->clk);
	dprintk("got and enabled clock\n");

	usleep_range(1000, 1100);

	info->clk_rate = clk_get_rate(info->clk);

	/* find maximum required memory size for display */
	for (i = 0; i < mach_info->num_displays; i++) {
		unsigned long smem_len = mach_info->displays[i].xres;

		smem_len *= mach_info->displays[i].yres;
		smem_len *= mach_info->displays[i].bpp;
		smem_len >>= 3;
		if (fbinfo->fix.smem_len < smem_len)
			fbinfo->fix.smem_len = smem_len;
	}

	/* Initialize video memory */
	ret = s3c2410fb_map_video_memory(fbinfo);
	if (ret) {
		dev_err(&pdev->dev, "Failed to allocate video RAM: %d\n", ret);
		ret = -ENOMEM;
		goto release_clock;
	}

	dprintk("got video memory\n");

	fbinfo->var.xres = display->xres;
	fbinfo->var.yres = display->yres;
	fbinfo->var.bits_per_pixel = display->bpp;

	s3c2410fb_init_registers(fbinfo);

	s3c2410fb_check_var(&fbinfo->var, fbinfo);

	ret = s3c2410fb_cpufreq_register(info);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register cpufreq\n");
		goto free_video_memory;
	}

	ret = register_framebuffer(fbinfo);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register framebuffer device: %d\n",
			ret);
		goto free_cpufreq;
	}

	/* create device files */
	ret = device_create_file(&pdev->dev, &dev_attr_debug);
	if (ret)
		dev_err(&pdev->dev, "failed to add debug attribute\n");

	dev_info(&pdev->dev, "fb%d: %s frame buffer device\n",
		fbinfo->node, fbinfo->fix.id);

	return 0;

 free_cpufreq:
	s3c2410fb_cpufreq_deregister(info);
free_video_memory:
	s3c2410fb_unmap_video_memory(fbinfo);
release_clock:
	clk_disable_unprepare(info->clk);
	clk_put(info->clk);
release_irq:
	free_irq(irq, info);
release_regs:
	iounmap(info->io);
release_mem:
	release_mem_region(res->start, size);
dealloc_fb:
	framebuffer_release(fbinfo);
	return ret;
}

static int s3c2410fb_probe(struct platform_device *pdev)
{
	return s3c24xxfb_probe(pdev, DRV_S3C2410);
}

static int s3c2412fb_probe(struct platform_device *pdev)
{
	return s3c24xxfb_probe(pdev, DRV_S3C2412);
}


/*
 *  Cleanup
 */
static int s3c2410fb_remove(struct platform_device *pdev)
{
	struct fb_info *fbinfo = platform_get_drvdata(pdev);
	struct s3c2410fb_info *info = fbinfo->par;
	int irq;

	unregister_framebuffer(fbinfo);
	s3c2410fb_cpufreq_deregister(info);

	s3c2410fb_lcd_enable(info, 0);
	usleep_range(1000, 1100);

	s3c2410fb_unmap_video_memory(fbinfo);

	if (info->clk) {
		clk_disable_unprepare(info->clk);
		clk_put(info->clk);
		info->clk = NULL;
	}

	irq = platform_get_irq(pdev, 0);
	free_irq(irq, info);

	iounmap(info->io);

	release_mem_region(info->mem->start, resource_size(info->mem));

	framebuffer_release(fbinfo);

	return 0;
}

#ifdef CONFIG_PM

/* suspend and resume support for the lcd controller */
static int s3c2410fb_suspend(struct platform_device *dev, pm_message_t state)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(dev);
	struct s3c2410fb_info *info = fbinfo->par;

	s3c2410fb_lcd_enable(info, 0);

	/* sleep before disabling the clock, we need to ensure
	 * the LCD DMA engine is not going to get back on the bus
	 * before the clock goes off again (bjd) */

	usleep_range(1000, 1100);
	clk_disable_unprepare(info->clk);

	return 0;
}

static int s3c2410fb_resume(struct platform_device *dev)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(dev);
	struct s3c2410fb_info *info = fbinfo->par;

	clk_prepare_enable(info->clk);
	usleep_range(1000, 1100);

	s3c2410fb_init_registers(fbinfo);

	/* re-activate our display after resume */
	s3c2410fb_activate_var(fbinfo);
	s3c2410fb_blank(FB_BLANK_UNBLANK, fbinfo);

	return 0;
}

#else
#define s3c2410fb_suspend NULL
#define s3c2410fb_resume  NULL
#endif

static struct platform_driver s3c2410fb_driver = {
	.probe		= s3c2410fb_probe,
	.remove		= s3c2410fb_remove,
	.suspend	= s3c2410fb_suspend,
	.resume		= s3c2410fb_resume,
	.driver		= {
		.name	= "s3c2410-lcd",
	},
};

static struct platform_driver s3c2412fb_driver = {
	.probe		= s3c2412fb_probe,
	.remove		= s3c2410fb_remove,
	.suspend	= s3c2410fb_suspend,
	.resume		= s3c2410fb_resume,
	.driver		= {
		.name	= "s3c2412-lcd",
	},
};

int __init s3c2410fb_init(void)
{
	int ret = platform_driver_register(&s3c2410fb_driver);

	if (ret == 0)
		ret = platform_driver_register(&s3c2412fb_driver);

	return ret;
}

static void __exit s3c2410fb_cleanup(void)
{
	platform_driver_unregister(&s3c2410fb_driver);
	platform_driver_unregister(&s3c2412fb_driver);
}

module_init(s3c2410fb_init);
module_exit(s3c2410fb_cleanup);

MODULE_AUTHOR("Arnaud Patard <arnaud.patard@rtp-net.org>");
MODULE_AUTHOR("Ben Dooks <ben-linux@fluff.org>");
MODULE_DESCRIPTION("Framebuffer driver for the s3c2410");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:s3c2410-lcd");
MODULE_ALIAS("platform:s3c2412-lcd");

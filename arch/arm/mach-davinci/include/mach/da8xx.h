/*
 * Chip specific defines for DA8XX/OMAP L1XX SoC
 *
 * Author: Mark A. Greer <mgreer@mvista.com>
 *
 * 2007, 2009-2010 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASM_ARCH_DAVINCI_DA8XX_H
#define __ASM_ARCH_DAVINCI_DA8XX_H

#include <video/da8xx-fb.h>

#include <linux/platform_device.h>
#include <linux/davinci_emac.h>
#include <linux/spi/spi.h>
#include <linux/platform_data/davinci_asp.h>
#include <linux/reboot.h>
#include <linux/regmap.h>
#include <linux/videodev2.h>

#include <mach/serial.h>
#include <mach/pm.h>
#include <linux/platform_data/edma.h>
#include <linux/platform_data/i2c-davinci.h>
#include <linux/platform_data/mmc-davinci.h>
#include <linux/platform_data/usb-davinci.h>
#include <linux/platform_data/spi-davinci.h>
#include <linux/platform_data/uio_pruss.h>

#include <media/davinci/vpif_types.h>

extern void __iomem *da8xx_syscfg0_base;
extern void __iomem *da8xx_syscfg1_base;

/*
 * If the DA850/OMAP-L138/AM18x SoC on board is of a higher speed grade
 * (than the regular 300MHz variant), the board code should set this up
 * with the supported speed before calling da850_register_cpufreq().
 */
extern unsigned int da850_max_speed;

/*
 * The cp_intc interrupt controller for the da8xx isn't in the same
 * chunk of physical memory space as the other registers (like it is
 * on the davincis) so it needs to be mapped separately.  It will be
 * mapped early on when the I/O space is mapped and we'll put it just
 * before the I/O space in the processor's virtual memory space.
 */
#define DA8XX_CP_INTC_BASE	0xfffee000
#define DA8XX_CP_INTC_SIZE	SZ_8K
#define DA8XX_CP_INTC_VIRT	(IO_VIRT - DA8XX_CP_INTC_SIZE - SZ_4K)

#define DA8XX_SYSCFG0_BASE	(IO_PHYS + 0x14000)
#define DA8XX_SYSCFG0_VIRT(x)	(da8xx_syscfg0_base + (x))
#define DA8XX_JTAG_ID_REG	0x18
#define DA8XX_HOST1CFG_REG	0x44
#define DA8XX_CHIPSIG_REG	0x174
#define DA8XX_CFGCHIP0_REG	0x17c
#define DA8XX_CFGCHIP1_REG	0x180
#define DA8XX_CFGCHIP2_REG	0x184
#define DA8XX_CFGCHIP3_REG	0x188
#define DA8XX_CFGCHIP4_REG	0x18c

#define DA8XX_SYSCFG1_BASE	(IO_PHYS + 0x22C000)
#define DA8XX_SYSCFG1_VIRT(x)	(da8xx_syscfg1_base + (x))
#define DA8XX_DEEPSLEEP_REG	0x8
#define DA8XX_PWRDN_REG		0x18

#define DA8XX_PSC0_BASE		0x01c10000
#define DA8XX_PLL0_BASE		0x01c11000
#define DA8XX_TIMER64P0_BASE	0x01c20000
#define DA8XX_TIMER64P1_BASE	0x01c21000
#define DA8XX_VPIF_BASE		0x01e17000
#define DA8XX_GPIO_BASE		0x01e26000
#define DA8XX_PSC1_BASE		0x01e27000

#define DA8XX_DSP_L2_RAM_BASE	0x11800000
#define DA8XX_DSP_L1P_RAM_BASE	(DA8XX_DSP_L2_RAM_BASE + 0x600000)
#define DA8XX_DSP_L1D_RAM_BASE	(DA8XX_DSP_L2_RAM_BASE + 0x700000)

#define DA8XX_AEMIF_CS2_BASE	0x60000000
#define DA8XX_AEMIF_CS3_BASE	0x62000000
#define DA8XX_AEMIF_CTL_BASE	0x68000000
#define DA8XX_SHARED_RAM_BASE	0x80000000
#define DA8XX_ARM_RAM_BASE	0xffff0000

void da830_init(void);
void da830_init_irq(void);
void da830_init_time(void);
void da830_register_clocks(void);

void da850_init(void);
void da850_init_irq(void);
void da850_init_time(void);
void da850_register_clocks(void);

int da830_register_edma(struct edma_rsv_info *rsv);
int da850_register_edma(struct edma_rsv_info *rsv[2]);
int da8xx_register_i2c(int instance, struct davinci_i2c_platform_data *pdata);
int da8xx_register_spi_bus(int instance, unsigned num_chipselect);
int da8xx_register_watchdog(void);
int da8xx_register_usb_phy(void);
int da8xx_register_usb20(unsigned mA, unsigned potpgt);
int da8xx_register_usb11(struct da8xx_ohci_root_hub *pdata);
int da8xx_register_usb_phy_clocks(void);
int da850_register_sata_refclk(int rate);
int da8xx_register_emac(void);
int da8xx_register_uio_pruss(void);
int da8xx_register_lcdc(struct da8xx_lcdc_platform_data *pdata);
int da8xx_register_mmcsd0(struct davinci_mmc_config *config);
int da850_register_mmcsd1(struct davinci_mmc_config *config);
void da8xx_register_mcasp(int id, struct snd_platform_data *pdata);
int da8xx_register_rtc(void);
int da8xx_register_gpio(void *pdata);
int da850_register_cpufreq(char *async_clk);
int da8xx_register_cpuidle(void);
void __iomem *da8xx_get_mem_ctlr(void);
int da850_register_sata(unsigned long refclkpn);
int da850_register_vpif(void);
int da850_register_vpif_display
			(struct vpif_display_config *display_config);
int da850_register_vpif_capture
			(struct vpif_capture_config *capture_config);
void da8xx_rproc_reserve_cma(void);
int da8xx_register_rproc(void);
int da850_register_gpio(void);
int da830_register_gpio(void);
struct regmap *da8xx_get_cfgchip(void);

extern struct platform_device da8xx_serial_device[];
extern struct emac_platform_data da8xx_emac_pdata;
extern struct da8xx_lcdc_platform_data sharp_lcd035q3dg01_pdata;
extern struct da8xx_lcdc_platform_data sharp_lk043t1dg01_pdata;


extern const short da830_emif25_pins[];
extern const short da830_spi0_pins[];
extern const short da830_spi1_pins[];
extern const short da830_mmc_sd_pins[];
extern const short da830_uart0_pins[];
extern const short da830_uart1_pins[];
extern const short da830_uart2_pins[];
extern const short da830_usb20_pins[];
extern const short da830_usb11_pins[];
extern const short da830_uhpi_pins[];
extern const short da830_cpgmac_pins[];
extern const short da830_emif3c_pins[];
extern const short da830_mcasp0_pins[];
extern const short da830_mcasp1_pins[];
extern const short da830_mcasp2_pins[];
extern const short da830_i2c0_pins[];
extern const short da830_i2c1_pins[];
extern const short da830_lcdcntl_pins[];
extern const short da830_pwm_pins[];
extern const short da830_ecap0_pins[];
extern const short da830_ecap1_pins[];
extern const short da830_ecap2_pins[];
extern const short da830_eqep0_pins[];
extern const short da830_eqep1_pins[];
extern const short da850_vpif_capture_pins[];
extern const short da850_vpif_display_pins[];

extern const short da850_i2c0_pins[];
extern const short da850_i2c1_pins[];
extern const short da850_lcdcntl_pins[];

#endif /* __ASM_ARCH_DAVINCI_DA8XX_H */

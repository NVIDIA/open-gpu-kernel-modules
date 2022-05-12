/*
 * Sonics Silicon Backplane
 * Broadcom ChipCommon core driver
 *
 * Copyright 2005, Broadcom Corporation
 * Copyright 2006, 2007, Michael Buesch <m@bues.ch>
 * Copyright 2012, Hauke Mehrtens <hauke@hauke-m.de>
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include "ssb_private.h"

#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_regs.h>
#include <linux/export.h>
#include <linux/pci.h>
#include <linux/bcm47xx_wdt.h>


/* Clock sources */
enum ssb_clksrc {
	/* PCI clock */
	SSB_CHIPCO_CLKSRC_PCI,
	/* Crystal slow clock oscillator */
	SSB_CHIPCO_CLKSRC_XTALOS,
	/* Low power oscillator */
	SSB_CHIPCO_CLKSRC_LOPWROS,
};


static inline u32 chipco_write32_masked(struct ssb_chipcommon *cc, u16 offset,
					u32 mask, u32 value)
{
	value &= mask;
	value |= chipco_read32(cc, offset) & ~mask;
	chipco_write32(cc, offset, value);

	return value;
}

void ssb_chipco_set_clockmode(struct ssb_chipcommon *cc,
			      enum ssb_clkmode mode)
{
	struct ssb_device *ccdev = cc->dev;
	struct ssb_bus *bus;
	u32 tmp;

	if (!ccdev)
		return;
	bus = ccdev->bus;

	/* We support SLOW only on 6..9 */
	if (ccdev->id.revision >= 10 && mode == SSB_CLKMODE_SLOW)
		mode = SSB_CLKMODE_DYNAMIC;

	if (cc->capabilities & SSB_CHIPCO_CAP_PMU)
		return; /* PMU controls clockmode, separated function needed */
	WARN_ON(ccdev->id.revision >= 20);

	/* chipcommon cores prior to rev6 don't support dynamic clock control */
	if (ccdev->id.revision < 6)
		return;

	/* ChipCommon cores rev10+ need testing */
	if (ccdev->id.revision >= 10)
		return;

	if (!(cc->capabilities & SSB_CHIPCO_CAP_PCTL))
		return;

	switch (mode) {
	case SSB_CLKMODE_SLOW: /* For revs 6..9 only */
		tmp = chipco_read32(cc, SSB_CHIPCO_SLOWCLKCTL);
		tmp |= SSB_CHIPCO_SLOWCLKCTL_FSLOW;
		chipco_write32(cc, SSB_CHIPCO_SLOWCLKCTL, tmp);
		break;
	case SSB_CLKMODE_FAST:
		if (ccdev->id.revision < 10) {
			ssb_pci_xtal(bus, SSB_GPIO_XTAL, 1); /* Force crystal on */
			tmp = chipco_read32(cc, SSB_CHIPCO_SLOWCLKCTL);
			tmp &= ~SSB_CHIPCO_SLOWCLKCTL_FSLOW;
			tmp |= SSB_CHIPCO_SLOWCLKCTL_IPLL;
			chipco_write32(cc, SSB_CHIPCO_SLOWCLKCTL, tmp);
		} else {
			chipco_write32(cc, SSB_CHIPCO_SYSCLKCTL,
				(chipco_read32(cc, SSB_CHIPCO_SYSCLKCTL) |
				 SSB_CHIPCO_SYSCLKCTL_FORCEHT));
			/* udelay(150); TODO: not available in early init */
		}
		break;
	case SSB_CLKMODE_DYNAMIC:
		if (ccdev->id.revision < 10) {
			tmp = chipco_read32(cc, SSB_CHIPCO_SLOWCLKCTL);
			tmp &= ~SSB_CHIPCO_SLOWCLKCTL_FSLOW;
			tmp &= ~SSB_CHIPCO_SLOWCLKCTL_IPLL;
			tmp &= ~SSB_CHIPCO_SLOWCLKCTL_ENXTAL;
			if ((tmp & SSB_CHIPCO_SLOWCLKCTL_SRC) !=
			    SSB_CHIPCO_SLOWCLKCTL_SRC_XTAL)
				tmp |= SSB_CHIPCO_SLOWCLKCTL_ENXTAL;
			chipco_write32(cc, SSB_CHIPCO_SLOWCLKCTL, tmp);

			/* For dynamic control, we have to release our xtal_pu
			 * "force on" */
			if (tmp & SSB_CHIPCO_SLOWCLKCTL_ENXTAL)
				ssb_pci_xtal(bus, SSB_GPIO_XTAL, 0);
		} else {
			chipco_write32(cc, SSB_CHIPCO_SYSCLKCTL,
				(chipco_read32(cc, SSB_CHIPCO_SYSCLKCTL) &
				 ~SSB_CHIPCO_SYSCLKCTL_FORCEHT));
		}
		break;
	default:
		WARN_ON(1);
	}
}

/* Get the Slow Clock Source */
static enum ssb_clksrc chipco_pctl_get_slowclksrc(struct ssb_chipcommon *cc)
{
	struct ssb_bus *bus = cc->dev->bus;
	u32 tmp;

	if (cc->dev->id.revision < 6) {
		if (bus->bustype == SSB_BUSTYPE_SSB ||
		    bus->bustype == SSB_BUSTYPE_PCMCIA)
			return SSB_CHIPCO_CLKSRC_XTALOS;
		if (bus->bustype == SSB_BUSTYPE_PCI) {
			pci_read_config_dword(bus->host_pci, SSB_GPIO_OUT, &tmp);
			if (tmp & 0x10)
				return SSB_CHIPCO_CLKSRC_PCI;
			return SSB_CHIPCO_CLKSRC_XTALOS;
		}
	}
	if (cc->dev->id.revision < 10) {
		tmp = chipco_read32(cc, SSB_CHIPCO_SLOWCLKCTL);
		tmp &= 0x7;
		if (tmp == 0)
			return SSB_CHIPCO_CLKSRC_LOPWROS;
		if (tmp == 1)
			return SSB_CHIPCO_CLKSRC_XTALOS;
		if (tmp == 2)
			return SSB_CHIPCO_CLKSRC_PCI;
	}

	return SSB_CHIPCO_CLKSRC_XTALOS;
}

/* Get maximum or minimum (depending on get_max flag) slowclock frequency. */
static int chipco_pctl_clockfreqlimit(struct ssb_chipcommon *cc, int get_max)
{
	int limit;
	enum ssb_clksrc clocksrc;
	int divisor = 1;
	u32 tmp;

	clocksrc = chipco_pctl_get_slowclksrc(cc);
	if (cc->dev->id.revision < 6) {
		switch (clocksrc) {
		case SSB_CHIPCO_CLKSRC_PCI:
			divisor = 64;
			break;
		case SSB_CHIPCO_CLKSRC_XTALOS:
			divisor = 32;
			break;
		default:
			WARN_ON(1);
		}
	} else if (cc->dev->id.revision < 10) {
		switch (clocksrc) {
		case SSB_CHIPCO_CLKSRC_LOPWROS:
			break;
		case SSB_CHIPCO_CLKSRC_XTALOS:
		case SSB_CHIPCO_CLKSRC_PCI:
			tmp = chipco_read32(cc, SSB_CHIPCO_SLOWCLKCTL);
			divisor = (tmp >> 16) + 1;
			divisor *= 4;
			break;
		}
	} else {
		tmp = chipco_read32(cc, SSB_CHIPCO_SYSCLKCTL);
		divisor = (tmp >> 16) + 1;
		divisor *= 4;
	}

	switch (clocksrc) {
	case SSB_CHIPCO_CLKSRC_LOPWROS:
		if (get_max)
			limit = 43000;
		else
			limit = 25000;
		break;
	case SSB_CHIPCO_CLKSRC_XTALOS:
		if (get_max)
			limit = 20200000;
		else
			limit = 19800000;
		break;
	case SSB_CHIPCO_CLKSRC_PCI:
		if (get_max)
			limit = 34000000;
		else
			limit = 25000000;
		break;
	}
	limit /= divisor;

	return limit;
}

static void chipco_powercontrol_init(struct ssb_chipcommon *cc)
{
	struct ssb_bus *bus = cc->dev->bus;

	if (bus->chip_id == 0x4321) {
		if (bus->chip_rev == 0)
			chipco_write32(cc, SSB_CHIPCO_CHIPCTL, 0x3A4);
		else if (bus->chip_rev == 1)
			chipco_write32(cc, SSB_CHIPCO_CHIPCTL, 0xA4);
	}

	if (!(cc->capabilities & SSB_CHIPCO_CAP_PCTL))
		return;

	if (cc->dev->id.revision >= 10) {
		/* Set Idle Power clock rate to 1Mhz */
		chipco_write32(cc, SSB_CHIPCO_SYSCLKCTL,
			       (chipco_read32(cc, SSB_CHIPCO_SYSCLKCTL) &
				0x0000FFFF) | 0x00040000);
	} else {
		int maxfreq;

		maxfreq = chipco_pctl_clockfreqlimit(cc, 1);
		chipco_write32(cc, SSB_CHIPCO_PLLONDELAY,
			       (maxfreq * 150 + 999999) / 1000000);
		chipco_write32(cc, SSB_CHIPCO_FREFSELDELAY,
			       (maxfreq * 15 + 999999) / 1000000);
	}
}

/* https://bcm-v4.sipsolutions.net/802.11/PmuFastPwrupDelay */
static u16 pmu_fast_powerup_delay(struct ssb_chipcommon *cc)
{
	struct ssb_bus *bus = cc->dev->bus;

	switch (bus->chip_id) {
	case 0x4312:
	case 0x4322:
	case 0x4328:
		return 7000;
	case 0x4325:
		/* TODO: */
	default:
		return 15000;
	}
}

/* https://bcm-v4.sipsolutions.net/802.11/ClkctlFastPwrupDelay */
static void calc_fast_powerup_delay(struct ssb_chipcommon *cc)
{
	struct ssb_bus *bus = cc->dev->bus;
	int minfreq;
	unsigned int tmp;
	u32 pll_on_delay;

	if (bus->bustype != SSB_BUSTYPE_PCI)
		return;

	if (cc->capabilities & SSB_CHIPCO_CAP_PMU) {
		cc->fast_pwrup_delay = pmu_fast_powerup_delay(cc);
		return;
	}

	if (!(cc->capabilities & SSB_CHIPCO_CAP_PCTL))
		return;

	minfreq = chipco_pctl_clockfreqlimit(cc, 0);
	pll_on_delay = chipco_read32(cc, SSB_CHIPCO_PLLONDELAY);
	tmp = (((pll_on_delay + 2) * 1000000) + (minfreq - 1)) / minfreq;
	WARN_ON(tmp & ~0xFFFF);

	cc->fast_pwrup_delay = tmp;
}

static u32 ssb_chipco_alp_clock(struct ssb_chipcommon *cc)
{
	if (cc->capabilities & SSB_CHIPCO_CAP_PMU)
		return ssb_pmu_get_alp_clock(cc);

	return 20000000;
}

static u32 ssb_chipco_watchdog_get_max_timer(struct ssb_chipcommon *cc)
{
	u32 nb;

	if (cc->capabilities & SSB_CHIPCO_CAP_PMU) {
		if (cc->dev->id.revision < 26)
			nb = 16;
		else
			nb = (cc->dev->id.revision >= 37) ? 32 : 24;
	} else {
		nb = 28;
	}
	if (nb == 32)
		return 0xffffffff;
	else
		return (1 << nb) - 1;
}

u32 ssb_chipco_watchdog_timer_set_wdt(struct bcm47xx_wdt *wdt, u32 ticks)
{
	struct ssb_chipcommon *cc = bcm47xx_wdt_get_drvdata(wdt);

	if (cc->dev->bus->bustype != SSB_BUSTYPE_SSB)
		return 0;

	return ssb_chipco_watchdog_timer_set(cc, ticks);
}

u32 ssb_chipco_watchdog_timer_set_ms(struct bcm47xx_wdt *wdt, u32 ms)
{
	struct ssb_chipcommon *cc = bcm47xx_wdt_get_drvdata(wdt);
	u32 ticks;

	if (cc->dev->bus->bustype != SSB_BUSTYPE_SSB)
		return 0;

	ticks = ssb_chipco_watchdog_timer_set(cc, cc->ticks_per_ms * ms);
	return ticks / cc->ticks_per_ms;
}

static int ssb_chipco_watchdog_ticks_per_ms(struct ssb_chipcommon *cc)
{
	struct ssb_bus *bus = cc->dev->bus;

	if (cc->capabilities & SSB_CHIPCO_CAP_PMU) {
			/* based on 32KHz ILP clock */
			return 32;
	} else {
		if (cc->dev->id.revision < 18)
			return ssb_clockspeed(bus) / 1000;
		else
			return ssb_chipco_alp_clock(cc) / 1000;
	}
}

void ssb_chipcommon_init(struct ssb_chipcommon *cc)
{
	if (!cc->dev)
		return; /* We don't have a ChipCommon */

	spin_lock_init(&cc->gpio_lock);

	if (cc->dev->id.revision >= 11)
		cc->status = chipco_read32(cc, SSB_CHIPCO_CHIPSTAT);
	dev_dbg(cc->dev->dev, "chipcommon status is 0x%x\n", cc->status);

	if (cc->dev->id.revision >= 20) {
		chipco_write32(cc, SSB_CHIPCO_GPIOPULLUP, 0);
		chipco_write32(cc, SSB_CHIPCO_GPIOPULLDOWN, 0);
	}

	ssb_pmu_init(cc);
	chipco_powercontrol_init(cc);
	ssb_chipco_set_clockmode(cc, SSB_CLKMODE_FAST);
	calc_fast_powerup_delay(cc);

	if (cc->dev->bus->bustype == SSB_BUSTYPE_SSB) {
		cc->ticks_per_ms = ssb_chipco_watchdog_ticks_per_ms(cc);
		cc->max_timer_ms = ssb_chipco_watchdog_get_max_timer(cc) / cc->ticks_per_ms;
	}
}

void ssb_chipco_suspend(struct ssb_chipcommon *cc)
{
	if (!cc->dev)
		return;
	ssb_chipco_set_clockmode(cc, SSB_CLKMODE_SLOW);
}

void ssb_chipco_resume(struct ssb_chipcommon *cc)
{
	if (!cc->dev)
		return;
	chipco_powercontrol_init(cc);
	ssb_chipco_set_clockmode(cc, SSB_CLKMODE_FAST);
}

/* Get the processor clock */
void ssb_chipco_get_clockcpu(struct ssb_chipcommon *cc,
                             u32 *plltype, u32 *n, u32 *m)
{
	*n = chipco_read32(cc, SSB_CHIPCO_CLOCK_N);
	*plltype = (cc->capabilities & SSB_CHIPCO_CAP_PLLT);
	switch (*plltype) {
	case SSB_PLLTYPE_2:
	case SSB_PLLTYPE_4:
	case SSB_PLLTYPE_6:
	case SSB_PLLTYPE_7:
		*m = chipco_read32(cc, SSB_CHIPCO_CLOCK_MIPS);
		break;
	case SSB_PLLTYPE_3:
		/* 5350 uses m2 to control mips */
		*m = chipco_read32(cc, SSB_CHIPCO_CLOCK_M2);
		break;
	default:
		*m = chipco_read32(cc, SSB_CHIPCO_CLOCK_SB);
		break;
	}
}

/* Get the bus clock */
void ssb_chipco_get_clockcontrol(struct ssb_chipcommon *cc,
				 u32 *plltype, u32 *n, u32 *m)
{
	*n = chipco_read32(cc, SSB_CHIPCO_CLOCK_N);
	*plltype = (cc->capabilities & SSB_CHIPCO_CAP_PLLT);
	switch (*plltype) {
	case SSB_PLLTYPE_6: /* 100/200 or 120/240 only */
		*m = chipco_read32(cc, SSB_CHIPCO_CLOCK_MIPS);
		break;
	case SSB_PLLTYPE_3: /* 25Mhz, 2 dividers */
		if (cc->dev->bus->chip_id != 0x5365) {
			*m = chipco_read32(cc, SSB_CHIPCO_CLOCK_M2);
			break;
		}
		fallthrough;
	default:
		*m = chipco_read32(cc, SSB_CHIPCO_CLOCK_SB);
	}
}

void ssb_chipco_timing_init(struct ssb_chipcommon *cc,
			    unsigned long ns)
{
	struct ssb_device *dev = cc->dev;
	struct ssb_bus *bus = dev->bus;
	u32 tmp;

	/* set register for external IO to control LED. */
	chipco_write32(cc, SSB_CHIPCO_PROG_CFG, 0x11);
	tmp = DIV_ROUND_UP(10, ns) << SSB_PROG_WCNT_3_SHIFT;		/* Waitcount-3 = 10ns */
	tmp |= DIV_ROUND_UP(40, ns) << SSB_PROG_WCNT_1_SHIFT;	/* Waitcount-1 = 40ns */
	tmp |= DIV_ROUND_UP(240, ns);				/* Waitcount-0 = 240ns */
	chipco_write32(cc, SSB_CHIPCO_PROG_WAITCNT, tmp);	/* 0x01020a0c for a 100Mhz clock */

	/* Set timing for the flash */
	tmp = DIV_ROUND_UP(10, ns) << SSB_FLASH_WCNT_3_SHIFT;	/* Waitcount-3 = 10nS */
	tmp |= DIV_ROUND_UP(10, ns) << SSB_FLASH_WCNT_1_SHIFT;	/* Waitcount-1 = 10nS */
	tmp |= DIV_ROUND_UP(120, ns);				/* Waitcount-0 = 120nS */
	if ((bus->chip_id == 0x5365) ||
	    (dev->id.revision < 9))
		chipco_write32(cc, SSB_CHIPCO_FLASH_WAITCNT, tmp);
	if ((bus->chip_id == 0x5365) ||
	    (dev->id.revision < 9) ||
	    ((bus->chip_id == 0x5350) && (bus->chip_rev == 0)))
		chipco_write32(cc, SSB_CHIPCO_PCMCIA_MEMWAIT, tmp);

	if (bus->chip_id == 0x5350) {
		/* Enable EXTIF */
		tmp = DIV_ROUND_UP(10, ns) << SSB_PROG_WCNT_3_SHIFT;	  /* Waitcount-3 = 10ns */
		tmp |= DIV_ROUND_UP(20, ns) << SSB_PROG_WCNT_2_SHIFT;  /* Waitcount-2 = 20ns */
		tmp |= DIV_ROUND_UP(100, ns) << SSB_PROG_WCNT_1_SHIFT; /* Waitcount-1 = 100ns */
		tmp |= DIV_ROUND_UP(120, ns);			  /* Waitcount-0 = 120ns */
		chipco_write32(cc, SSB_CHIPCO_PROG_WAITCNT, tmp); /* 0x01020a0c for a 100Mhz clock */
	}
}

/* Set chip watchdog reset timer to fire in 'ticks' backplane cycles */
u32 ssb_chipco_watchdog_timer_set(struct ssb_chipcommon *cc, u32 ticks)
{
	u32 maxt;
	enum ssb_clkmode clkmode;

	maxt = ssb_chipco_watchdog_get_max_timer(cc);
	if (cc->capabilities & SSB_CHIPCO_CAP_PMU) {
		if (ticks == 1)
			ticks = 2;
		else if (ticks > maxt)
			ticks = maxt;
		chipco_write32(cc, SSB_CHIPCO_PMU_WATCHDOG, ticks);
	} else {
		clkmode = ticks ? SSB_CLKMODE_FAST : SSB_CLKMODE_DYNAMIC;
		ssb_chipco_set_clockmode(cc, clkmode);
		if (ticks > maxt)
			ticks = maxt;
		/* instant NMI */
		chipco_write32(cc, SSB_CHIPCO_WATCHDOG, ticks);
	}
	return ticks;
}

void ssb_chipco_irq_mask(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	chipco_write32_masked(cc, SSB_CHIPCO_IRQMASK, mask, value);
}

u32 ssb_chipco_irq_status(struct ssb_chipcommon *cc, u32 mask)
{
	return chipco_read32(cc, SSB_CHIPCO_IRQSTAT) & mask;
}

u32 ssb_chipco_gpio_in(struct ssb_chipcommon *cc, u32 mask)
{
	return chipco_read32(cc, SSB_CHIPCO_GPIOIN) & mask;
}

u32 ssb_chipco_gpio_out(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOOUT, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}

u32 ssb_chipco_gpio_outen(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOOUTEN, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}

u32 ssb_chipco_gpio_control(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOCTL, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}
EXPORT_SYMBOL(ssb_chipco_gpio_control);

u32 ssb_chipco_gpio_intmask(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOIRQ, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}

u32 ssb_chipco_gpio_polarity(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOPOL, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}

u32 ssb_chipco_gpio_pullup(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	if (cc->dev->id.revision < 20)
		return 0xffffffff;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOPULLUP, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}

u32 ssb_chipco_gpio_pulldown(struct ssb_chipcommon *cc, u32 mask, u32 value)
{
	unsigned long flags;
	u32 res = 0;

	if (cc->dev->id.revision < 20)
		return 0xffffffff;

	spin_lock_irqsave(&cc->gpio_lock, flags);
	res = chipco_write32_masked(cc, SSB_CHIPCO_GPIOPULLDOWN, mask, value);
	spin_unlock_irqrestore(&cc->gpio_lock, flags);

	return res;
}

#ifdef CONFIG_SSB_SERIAL
int ssb_chipco_serial_init(struct ssb_chipcommon *cc,
			   struct ssb_serial_port *ports)
{
	struct ssb_bus *bus = cc->dev->bus;
	int nr_ports = 0;
	u32 plltype;
	unsigned int irq;
	u32 baud_base, div;
	u32 i, n;
	unsigned int ccrev = cc->dev->id.revision;

	plltype = (cc->capabilities & SSB_CHIPCO_CAP_PLLT);
	irq = ssb_mips_irq(cc->dev);

	if (plltype == SSB_PLLTYPE_1) {
		/* PLL clock */
		baud_base = ssb_calc_clock_rate(plltype,
						chipco_read32(cc, SSB_CHIPCO_CLOCK_N),
						chipco_read32(cc, SSB_CHIPCO_CLOCK_M2));
		div = 1;
	} else {
		if (ccrev == 20) {
			/* BCM5354 uses constant 25MHz clock */
			baud_base = 25000000;
			div = 48;
			/* Set the override bit so we don't divide it */
			chipco_write32(cc, SSB_CHIPCO_CORECTL,
				       chipco_read32(cc, SSB_CHIPCO_CORECTL)
				       | SSB_CHIPCO_CORECTL_UARTCLK0);
		} else if ((ccrev >= 11) && (ccrev != 15)) {
			baud_base = ssb_chipco_alp_clock(cc);
			div = 1;
			if (ccrev >= 21) {
				/* Turn off UART clock before switching clocksource. */
				chipco_write32(cc, SSB_CHIPCO_CORECTL,
					       chipco_read32(cc, SSB_CHIPCO_CORECTL)
					       & ~SSB_CHIPCO_CORECTL_UARTCLKEN);
			}
			/* Set the override bit so we don't divide it */
			chipco_write32(cc, SSB_CHIPCO_CORECTL,
				       chipco_read32(cc, SSB_CHIPCO_CORECTL)
				       | SSB_CHIPCO_CORECTL_UARTCLK0);
			if (ccrev >= 21) {
				/* Re-enable the UART clock. */
				chipco_write32(cc, SSB_CHIPCO_CORECTL,
					       chipco_read32(cc, SSB_CHIPCO_CORECTL)
					       | SSB_CHIPCO_CORECTL_UARTCLKEN);
			}
		} else if (ccrev >= 3) {
			/* Internal backplane clock */
			baud_base = ssb_clockspeed(bus);
			div = chipco_read32(cc, SSB_CHIPCO_CLKDIV)
			      & SSB_CHIPCO_CLKDIV_UART;
		} else {
			/* Fixed internal backplane clock */
			baud_base = 88000000;
			div = 48;
		}

		/* Clock source depends on strapping if UartClkOverride is unset */
		if ((ccrev > 0) &&
		    !(chipco_read32(cc, SSB_CHIPCO_CORECTL) & SSB_CHIPCO_CORECTL_UARTCLK0)) {
			if ((cc->capabilities & SSB_CHIPCO_CAP_UARTCLK) ==
			    SSB_CHIPCO_CAP_UARTCLK_INT) {
				/* Internal divided backplane clock */
				baud_base /= div;
			} else {
				/* Assume external clock of 1.8432 MHz */
				baud_base = 1843200;
			}
		}
	}

	/* Determine the registers of the UARTs */
	n = (cc->capabilities & SSB_CHIPCO_CAP_NRUART);
	for (i = 0; i < n; i++) {
		void __iomem *cc_mmio;
		void __iomem *uart_regs;

		cc_mmio = cc->dev->bus->mmio + (cc->dev->core_index * SSB_CORE_SIZE);
		uart_regs = cc_mmio + SSB_CHIPCO_UART0_DATA;
		/* Offset changed at after rev 0 */
		if (ccrev == 0)
			uart_regs += (i * 8);
		else
			uart_regs += (i * 256);

		nr_ports++;
		ports[i].regs = uart_regs;
		ports[i].irq = irq;
		ports[i].baud_base = baud_base;
		ports[i].reg_shift = 0;
	}

	return nr_ports;
}
#endif /* CONFIG_SSB_SERIAL */

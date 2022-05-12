/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_SSB_PRIVATE_H_
#define LINUX_SSB_PRIVATE_H_

#define PFX		"ssb: "
#define pr_fmt(fmt)	PFX fmt

#include <linux/ssb/ssb.h>
#include <linux/types.h>
#include <linux/bcm47xx_wdt.h>


/* pci.c */
#ifdef CONFIG_SSB_PCIHOST
extern int ssb_pci_switch_core(struct ssb_bus *bus,
			       struct ssb_device *dev);
extern int ssb_pci_switch_coreidx(struct ssb_bus *bus,
				  u8 coreidx);
extern int ssb_pci_xtal(struct ssb_bus *bus, u32 what,
			int turn_on);
extern int ssb_pci_get_invariants(struct ssb_bus *bus,
				  struct ssb_init_invariants *iv);
extern void ssb_pci_exit(struct ssb_bus *bus);
extern int ssb_pci_init(struct ssb_bus *bus);
extern const struct ssb_bus_ops ssb_pci_ops;

#else /* CONFIG_SSB_PCIHOST */

static inline int ssb_pci_switch_core(struct ssb_bus *bus,
				      struct ssb_device *dev)
{
	return 0;
}
static inline int ssb_pci_switch_coreidx(struct ssb_bus *bus,
					 u8 coreidx)
{
	return 0;
}
static inline int ssb_pci_xtal(struct ssb_bus *bus, u32 what,
			       int turn_on)
{
	return 0;
}
static inline void ssb_pci_exit(struct ssb_bus *bus)
{
}
static inline int ssb_pci_init(struct ssb_bus *bus)
{
	return 0;
}
#endif /* CONFIG_SSB_PCIHOST */


/* pcmcia.c */
#ifdef CONFIG_SSB_PCMCIAHOST
extern int ssb_pcmcia_switch_coreidx(struct ssb_bus *bus,
				     u8 coreidx);
extern int ssb_pcmcia_switch_segment(struct ssb_bus *bus,
				     u8 seg);
extern int ssb_pcmcia_get_invariants(struct ssb_bus *bus,
				     struct ssb_init_invariants *iv);
extern int ssb_pcmcia_hardware_setup(struct ssb_bus *bus);
extern void ssb_pcmcia_exit(struct ssb_bus *bus);
extern int ssb_pcmcia_init(struct ssb_bus *bus);
extern int ssb_host_pcmcia_init(void);
extern void ssb_host_pcmcia_exit(void);
extern const struct ssb_bus_ops ssb_pcmcia_ops;
#else /* CONFIG_SSB_PCMCIAHOST */
static inline int ssb_pcmcia_switch_coreidx(struct ssb_bus *bus,
					    u8 coreidx)
{
	return 0;
}
static inline int ssb_pcmcia_switch_segment(struct ssb_bus *bus,
					    u8 seg)
{
	return 0;
}
static inline int ssb_pcmcia_hardware_setup(struct ssb_bus *bus)
{
	return 0;
}
static inline void ssb_pcmcia_exit(struct ssb_bus *bus)
{
}
static inline int ssb_pcmcia_init(struct ssb_bus *bus)
{
	return 0;
}
static inline int ssb_host_pcmcia_init(void)
{
	return 0;
}
static inline void ssb_host_pcmcia_exit(void)
{
}
#endif /* CONFIG_SSB_PCMCIAHOST */

/* sdio.c */
#ifdef CONFIG_SSB_SDIOHOST
extern int ssb_sdio_get_invariants(struct ssb_bus *bus,
				     struct ssb_init_invariants *iv);

extern u32 ssb_sdio_scan_read32(struct ssb_bus *bus, u16 offset);
extern int ssb_sdio_scan_switch_coreidx(struct ssb_bus *bus, u8 coreidx);
extern void ssb_sdio_exit(struct ssb_bus *bus);
extern int ssb_sdio_init(struct ssb_bus *bus);

extern const struct ssb_bus_ops ssb_sdio_ops;
#else /* CONFIG_SSB_SDIOHOST */
static inline u32 ssb_sdio_scan_read32(struct ssb_bus *bus, u16 offset)
{
	return 0;
}
static inline int ssb_sdio_scan_switch_coreidx(struct ssb_bus *bus, u8 coreidx)
{
	return 0;
}
static inline void ssb_sdio_exit(struct ssb_bus *bus)
{
}
static inline int ssb_sdio_init(struct ssb_bus *bus)
{
	return 0;
}
#endif /* CONFIG_SSB_SDIOHOST */

/**************************************************
 * host_soc.c
 **************************************************/

#ifdef CONFIG_SSB_HOST_SOC
extern const struct ssb_bus_ops ssb_host_soc_ops;

extern int ssb_host_soc_get_invariants(struct ssb_bus *bus,
				       struct ssb_init_invariants *iv);
#endif

/* scan.c */
extern const char *ssb_core_name(u16 coreid);
extern int ssb_bus_scan(struct ssb_bus *bus,
			unsigned long baseaddr);
extern void ssb_iounmap(struct ssb_bus *ssb);


/* sprom.c */
extern
ssize_t ssb_attr_sprom_show(struct ssb_bus *bus, char *buf,
			    int (*sprom_read)(struct ssb_bus *bus, u16 *sprom));
extern
ssize_t ssb_attr_sprom_store(struct ssb_bus *bus,
			     const char *buf, size_t count,
			     int (*sprom_check_crc)(const u16 *sprom, size_t size),
			     int (*sprom_write)(struct ssb_bus *bus, const u16 *sprom));
extern int ssb_fill_sprom_with_fallback(struct ssb_bus *bus,
					struct ssb_sprom *out);


/* core.c */
extern u32 ssb_calc_clock_rate(u32 plltype, u32 n, u32 m);
extern struct ssb_bus *ssb_pci_dev_to_bus(struct pci_dev *pdev);
int ssb_for_each_bus_call(unsigned long data,
			  int (*func)(struct ssb_bus *bus, unsigned long data));
extern struct ssb_bus *ssb_pcmcia_dev_to_bus(struct pcmcia_device *pdev);

struct ssb_freeze_context {
	/* Pointer to the bus */
	struct ssb_bus *bus;
	/* Boolean list to indicate whether a device is frozen on this bus. */
	bool device_frozen[SSB_MAX_NR_CORES];
};
extern int ssb_devices_freeze(struct ssb_bus *bus, struct ssb_freeze_context *ctx);
extern int ssb_devices_thaw(struct ssb_freeze_context *ctx);



/* b43_pci_bridge.c */
#ifdef CONFIG_SSB_B43_PCI_BRIDGE
extern int __init b43_pci_ssb_bridge_init(void);
extern void __exit b43_pci_ssb_bridge_exit(void);
#else /* CONFIG_SSB_B43_PCI_BRIDGE */
static inline int b43_pci_ssb_bridge_init(void)
{
	return 0;
}
static inline void b43_pci_ssb_bridge_exit(void)
{
}
#endif /* CONFIG_SSB_B43_PCI_BRIDGE */

/* driver_chipcommon_pmu.c */
extern u32 ssb_pmu_get_cpu_clock(struct ssb_chipcommon *cc);
extern u32 ssb_pmu_get_controlclock(struct ssb_chipcommon *cc);
extern u32 ssb_pmu_get_alp_clock(struct ssb_chipcommon *cc);

extern u32 ssb_chipco_watchdog_timer_set_wdt(struct bcm47xx_wdt *wdt,
					     u32 ticks);
extern u32 ssb_chipco_watchdog_timer_set_ms(struct bcm47xx_wdt *wdt, u32 ms);

/* driver_chipcommon_sflash.c */
#ifdef CONFIG_SSB_SFLASH
int ssb_sflash_init(struct ssb_chipcommon *cc);
#else
static inline int ssb_sflash_init(struct ssb_chipcommon *cc)
{
	pr_err("Serial flash not supported\n");
	return 0;
}
#endif /* CONFIG_SSB_SFLASH */

#ifdef CONFIG_SSB_DRIVER_MIPS
extern struct platform_device ssb_pflash_dev;
#endif

#ifdef CONFIG_SSB_SFLASH
extern struct platform_device ssb_sflash_dev;
#endif

#ifdef CONFIG_SSB_DRIVER_EXTIF
extern u32 ssb_extif_watchdog_timer_set_wdt(struct bcm47xx_wdt *wdt, u32 ticks);
extern u32 ssb_extif_watchdog_timer_set_ms(struct bcm47xx_wdt *wdt, u32 ms);
#else
static inline u32 ssb_extif_watchdog_timer_set_wdt(struct bcm47xx_wdt *wdt,
						   u32 ticks)
{
	return 0;
}
static inline u32 ssb_extif_watchdog_timer_set_ms(struct bcm47xx_wdt *wdt,
						  u32 ms)
{
	return 0;
}
#endif

#ifdef CONFIG_SSB_EMBEDDED
extern int ssb_watchdog_register(struct ssb_bus *bus);
#else /* CONFIG_SSB_EMBEDDED */
static inline int ssb_watchdog_register(struct ssb_bus *bus)
{
	return 0;
}
#endif /* CONFIG_SSB_EMBEDDED */

#ifdef CONFIG_SSB_DRIVER_EXTIF
extern void ssb_extif_init(struct ssb_extif *extif);
#else
static inline void ssb_extif_init(struct ssb_extif *extif)
{
}
#endif

#ifdef CONFIG_SSB_DRIVER_GPIO
extern int ssb_gpio_init(struct ssb_bus *bus);
extern int ssb_gpio_unregister(struct ssb_bus *bus);
#else /* CONFIG_SSB_DRIVER_GPIO */
static inline int ssb_gpio_init(struct ssb_bus *bus)
{
	return -ENOTSUPP;
}
static inline int ssb_gpio_unregister(struct ssb_bus *bus)
{
	return 0;
}
#endif /* CONFIG_SSB_DRIVER_GPIO */

#endif /* LINUX_SSB_PRIVATE_H_ */

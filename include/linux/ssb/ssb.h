/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_SSB_H_
#define LINUX_SSB_H_

#include <linux/device.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <linux/mod_devicetable.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <linux/ssb/ssb_regs.h>


struct pcmcia_device;
struct ssb_bus;
struct ssb_driver;

struct ssb_sprom_core_pwr_info {
	u8 itssi_2g, itssi_5g;
	u8 maxpwr_2g, maxpwr_5gl, maxpwr_5g, maxpwr_5gh;
	u16 pa_2g[4], pa_5gl[4], pa_5g[4], pa_5gh[4];
};

struct ssb_sprom {
	u8 revision;
	u8 il0mac[6] __aligned(sizeof(u16));	/* MAC address for 802.11b/g */
	u8 et0mac[6] __aligned(sizeof(u16));	/* MAC address for Ethernet */
	u8 et1mac[6] __aligned(sizeof(u16));	/* MAC address for 802.11a */
	u8 et2mac[6] __aligned(sizeof(u16));	/* MAC address for extra Ethernet */
	u8 et0phyaddr;		/* MII address for enet0 */
	u8 et1phyaddr;		/* MII address for enet1 */
	u8 et2phyaddr;		/* MII address for enet2 */
	u8 et0mdcport;		/* MDIO for enet0 */
	u8 et1mdcport;		/* MDIO for enet1 */
	u8 et2mdcport;		/* MDIO for enet2 */
	u16 dev_id;		/* Device ID overriding e.g. PCI ID */
	u16 board_rev;		/* Board revision number from SPROM. */
	u16 board_num;		/* Board number from SPROM. */
	u16 board_type;		/* Board type from SPROM. */
	u8 country_code;	/* Country Code */
	char alpha2[2];		/* Country Code as two chars like EU or US */
	u8 leddc_on_time;	/* LED Powersave Duty Cycle On Count */
	u8 leddc_off_time;	/* LED Powersave Duty Cycle Off Count */
	u8 ant_available_a;	/* 2GHz antenna available bits (up to 4) */
	u8 ant_available_bg;	/* 5GHz antenna available bits (up to 4) */
	u16 pa0b0;
	u16 pa0b1;
	u16 pa0b2;
	u16 pa1b0;
	u16 pa1b1;
	u16 pa1b2;
	u16 pa1lob0;
	u16 pa1lob1;
	u16 pa1lob2;
	u16 pa1hib0;
	u16 pa1hib1;
	u16 pa1hib2;
	u8 gpio0;		/* GPIO pin 0 */
	u8 gpio1;		/* GPIO pin 1 */
	u8 gpio2;		/* GPIO pin 2 */
	u8 gpio3;		/* GPIO pin 3 */
	u8 maxpwr_bg;		/* 2.4GHz Amplifier Max Power (in dBm Q5.2) */
	u8 maxpwr_al;		/* 5.2GHz Amplifier Max Power (in dBm Q5.2) */
	u8 maxpwr_a;		/* 5.3GHz Amplifier Max Power (in dBm Q5.2) */
	u8 maxpwr_ah;		/* 5.8GHz Amplifier Max Power (in dBm Q5.2) */
	u8 itssi_a;		/* Idle TSSI Target for A-PHY */
	u8 itssi_bg;		/* Idle TSSI Target for B/G-PHY */
	u8 tri2g;		/* 2.4GHz TX isolation */
	u8 tri5gl;		/* 5.2GHz TX isolation */
	u8 tri5g;		/* 5.3GHz TX isolation */
	u8 tri5gh;		/* 5.8GHz TX isolation */
	u8 txpid2g[4];		/* 2GHz TX power index */
	u8 txpid5gl[4];		/* 4.9 - 5.1GHz TX power index */
	u8 txpid5g[4];		/* 5.1 - 5.5GHz TX power index */
	u8 txpid5gh[4];		/* 5.5 - ...GHz TX power index */
	s8 rxpo2g;		/* 2GHz RX power offset */
	s8 rxpo5g;		/* 5GHz RX power offset */
	u8 rssisav2g;		/* 2GHz RSSI params */
	u8 rssismc2g;
	u8 rssismf2g;
	u8 bxa2g;		/* 2GHz BX arch */
	u8 rssisav5g;		/* 5GHz RSSI params */
	u8 rssismc5g;
	u8 rssismf5g;
	u8 bxa5g;		/* 5GHz BX arch */
	u16 cck2gpo;		/* CCK power offset */
	u32 ofdm2gpo;		/* 2.4GHz OFDM power offset */
	u32 ofdm5glpo;		/* 5.2GHz OFDM power offset */
	u32 ofdm5gpo;		/* 5.3GHz OFDM power offset */
	u32 ofdm5ghpo;		/* 5.8GHz OFDM power offset */
	u32 boardflags;
	u32 boardflags2;
	u32 boardflags3;
	/* TODO: Switch all drivers to new u32 fields and drop below ones */
	u16 boardflags_lo;	/* Board flags (bits 0-15) */
	u16 boardflags_hi;	/* Board flags (bits 16-31) */
	u16 boardflags2_lo;	/* Board flags (bits 32-47) */
	u16 boardflags2_hi;	/* Board flags (bits 48-63) */

	struct ssb_sprom_core_pwr_info core_pwr_info[4];

	/* Antenna gain values for up to 4 antennas
	 * on each band. Values in dBm/4 (Q5.2). Negative gain means the
	 * loss in the connectors is bigger than the gain. */
	struct {
		s8 a0, a1, a2, a3;
	} antenna_gain;

	struct {
		struct {
			u8 tssipos, extpa_gain, pdet_range, tr_iso, antswlut;
		} ghz2;
		struct {
			u8 tssipos, extpa_gain, pdet_range, tr_iso, antswlut;
		} ghz5;
	} fem;

	u16 mcs2gpo[8];
	u16 mcs5gpo[8];
	u16 mcs5glpo[8];
	u16 mcs5ghpo[8];
	u8 opo;

	u8 rxgainerr2ga[3];
	u8 rxgainerr5gla[3];
	u8 rxgainerr5gma[3];
	u8 rxgainerr5gha[3];
	u8 rxgainerr5gua[3];

	u8 noiselvl2ga[3];
	u8 noiselvl5gla[3];
	u8 noiselvl5gma[3];
	u8 noiselvl5gha[3];
	u8 noiselvl5gua[3];

	u8 regrev;
	u8 txchain;
	u8 rxchain;
	u8 antswitch;
	u16 cddpo;
	u16 stbcpo;
	u16 bw40po;
	u16 bwduppo;

	u8 tempthresh;
	u8 tempoffset;
	u16 rawtempsense;
	u8 measpower;
	u8 tempsense_slope;
	u8 tempcorrx;
	u8 tempsense_option;
	u8 freqoffset_corr;
	u8 iqcal_swp_dis;
	u8 hw_iqcal_en;
	u8 elna2g;
	u8 elna5g;
	u8 phycal_tempdelta;
	u8 temps_period;
	u8 temps_hysteresis;
	u8 measpower1;
	u8 measpower2;
	u8 pcieingress_war;

	/* power per rate from sromrev 9 */
	u16 cckbw202gpo;
	u16 cckbw20ul2gpo;
	u32 legofdmbw202gpo;
	u32 legofdmbw20ul2gpo;
	u32 legofdmbw205glpo;
	u32 legofdmbw20ul5glpo;
	u32 legofdmbw205gmpo;
	u32 legofdmbw20ul5gmpo;
	u32 legofdmbw205ghpo;
	u32 legofdmbw20ul5ghpo;
	u32 mcsbw202gpo;
	u32 mcsbw20ul2gpo;
	u32 mcsbw402gpo;
	u32 mcsbw205glpo;
	u32 mcsbw20ul5glpo;
	u32 mcsbw405glpo;
	u32 mcsbw205gmpo;
	u32 mcsbw20ul5gmpo;
	u32 mcsbw405gmpo;
	u32 mcsbw205ghpo;
	u32 mcsbw20ul5ghpo;
	u32 mcsbw405ghpo;
	u16 mcs32po;
	u16 legofdm40duppo;
	u8 sar2g;
	u8 sar5g;
};

/* Information about the PCB the circuitry is soldered on. */
struct ssb_boardinfo {
	u16 vendor;
	u16 type;
};


struct ssb_device;
/* Lowlevel read/write operations on the device MMIO.
 * Internal, don't use that outside of ssb. */
struct ssb_bus_ops {
	u8 (*read8)(struct ssb_device *dev, u16 offset);
	u16 (*read16)(struct ssb_device *dev, u16 offset);
	u32 (*read32)(struct ssb_device *dev, u16 offset);
	void (*write8)(struct ssb_device *dev, u16 offset, u8 value);
	void (*write16)(struct ssb_device *dev, u16 offset, u16 value);
	void (*write32)(struct ssb_device *dev, u16 offset, u32 value);
#ifdef CONFIG_SSB_BLOCKIO
	void (*block_read)(struct ssb_device *dev, void *buffer,
			   size_t count, u16 offset, u8 reg_width);
	void (*block_write)(struct ssb_device *dev, const void *buffer,
			    size_t count, u16 offset, u8 reg_width);
#endif
};


/* Core-ID values. */
#define SSB_DEV_CHIPCOMMON	0x800
#define SSB_DEV_ILINE20		0x801
#define SSB_DEV_SDRAM		0x803
#define SSB_DEV_PCI		0x804
#define SSB_DEV_MIPS		0x805
#define SSB_DEV_ETHERNET	0x806
#define SSB_DEV_V90		0x807
#define SSB_DEV_USB11_HOSTDEV	0x808
#define SSB_DEV_ADSL		0x809
#define SSB_DEV_ILINE100	0x80A
#define SSB_DEV_IPSEC		0x80B
#define SSB_DEV_PCMCIA		0x80D
#define SSB_DEV_INTERNAL_MEM	0x80E
#define SSB_DEV_MEMC_SDRAM	0x80F
#define SSB_DEV_EXTIF		0x811
#define SSB_DEV_80211		0x812
#define SSB_DEV_MIPS_3302	0x816
#define SSB_DEV_USB11_HOST	0x817
#define SSB_DEV_USB11_DEV	0x818
#define SSB_DEV_USB20_HOST	0x819
#define SSB_DEV_USB20_DEV	0x81A
#define SSB_DEV_SDIO_HOST	0x81B
#define SSB_DEV_ROBOSWITCH	0x81C
#define SSB_DEV_PARA_ATA	0x81D
#define SSB_DEV_SATA_XORDMA	0x81E
#define SSB_DEV_ETHERNET_GBIT	0x81F
#define SSB_DEV_PCIE		0x820
#define SSB_DEV_MIMO_PHY	0x821
#define SSB_DEV_SRAM_CTRLR	0x822
#define SSB_DEV_MINI_MACPHY	0x823
#define SSB_DEV_ARM_1176	0x824
#define SSB_DEV_ARM_7TDMI	0x825
#define SSB_DEV_ARM_CM3		0x82A

/* Vendor-ID values */
#define SSB_VENDOR_BROADCOM	0x4243

/* Some kernel subsystems poke with dev->drvdata, so we must use the
 * following ugly workaround to get from struct device to struct ssb_device */
struct __ssb_dev_wrapper {
	struct device dev;
	struct ssb_device *sdev;
};

struct ssb_device {
	/* Having a copy of the ops pointer in each dev struct
	 * is an optimization. */
	const struct ssb_bus_ops *ops;

	struct device *dev, *dma_dev;

	struct ssb_bus *bus;
	struct ssb_device_id id;

	u8 core_index;
	unsigned int irq;

	/* Internal-only stuff follows. */
	void *drvdata;		/* Per-device data */
	void *devtypedata;	/* Per-devicetype (eg 802.11) data */
};

/* Go from struct device to struct ssb_device. */
static inline
struct ssb_device * dev_to_ssb_dev(struct device *dev)
{
	struct __ssb_dev_wrapper *wrap;
	wrap = container_of(dev, struct __ssb_dev_wrapper, dev);
	return wrap->sdev;
}

/* Device specific user data */
static inline
void ssb_set_drvdata(struct ssb_device *dev, void *data)
{
	dev->drvdata = data;
}
static inline
void * ssb_get_drvdata(struct ssb_device *dev)
{
	return dev->drvdata;
}

/* Devicetype specific user data. This is per device-type (not per device) */
void ssb_set_devtypedata(struct ssb_device *dev, void *data);
static inline
void * ssb_get_devtypedata(struct ssb_device *dev)
{
	return dev->devtypedata;
}


struct ssb_driver {
	const char *name;
	const struct ssb_device_id *id_table;

	int (*probe)(struct ssb_device *dev, const struct ssb_device_id *id);
	void (*remove)(struct ssb_device *dev);
	int (*suspend)(struct ssb_device *dev, pm_message_t state);
	int (*resume)(struct ssb_device *dev);
	void (*shutdown)(struct ssb_device *dev);

	struct device_driver drv;
};
#define drv_to_ssb_drv(_drv) container_of(_drv, struct ssb_driver, drv)

extern int __ssb_driver_register(struct ssb_driver *drv, struct module *owner);
#define ssb_driver_register(drv) \
	__ssb_driver_register(drv, THIS_MODULE)

extern void ssb_driver_unregister(struct ssb_driver *drv);




enum ssb_bustype {
	SSB_BUSTYPE_SSB,	/* This SSB bus is the system bus */
	SSB_BUSTYPE_PCI,	/* SSB is connected to PCI bus */
	SSB_BUSTYPE_PCMCIA,	/* SSB is connected to PCMCIA bus */
	SSB_BUSTYPE_SDIO,	/* SSB is connected to SDIO bus */
};

/* board_vendor */
#define SSB_BOARDVENDOR_BCM	0x14E4	/* Broadcom */
#define SSB_BOARDVENDOR_DELL	0x1028	/* Dell */
#define SSB_BOARDVENDOR_HP	0x0E11	/* HP */
/* board_type */
#define SSB_BOARD_BCM94301CB	0x0406
#define SSB_BOARD_BCM94301MP	0x0407
#define SSB_BOARD_BU4309	0x040A
#define SSB_BOARD_BCM94309CB	0x040B
#define SSB_BOARD_BCM4309MP	0x040C
#define SSB_BOARD_BU4306	0x0416
#define SSB_BOARD_BCM94306MP	0x0418
#define SSB_BOARD_BCM4309G	0x0421
#define SSB_BOARD_BCM4306CB	0x0417
#define SSB_BOARD_BCM94306PC	0x0425	/* pcmcia 3.3v 4306 card */
#define SSB_BOARD_BCM94306CBSG	0x042B	/* with SiGe PA */
#define SSB_BOARD_PCSG94306	0x042D	/* with SiGe PA */
#define SSB_BOARD_BU4704SD	0x042E	/* with sdram */
#define SSB_BOARD_BCM94704AGR	0x042F	/* dual 11a/11g Router */
#define SSB_BOARD_BCM94308MP	0x0430	/* 11a-only minipci */
#define SSB_BOARD_BU4318	0x0447
#define SSB_BOARD_CB4318	0x0448
#define SSB_BOARD_MPG4318	0x0449
#define SSB_BOARD_MP4318	0x044A
#define SSB_BOARD_SD4318	0x044B
#define SSB_BOARD_BCM94306P	0x044C	/* with SiGe */
#define SSB_BOARD_BCM94303MP	0x044E
#define SSB_BOARD_BCM94306MPM	0x0450
#define SSB_BOARD_BCM94306MPL	0x0453
#define SSB_BOARD_PC4303	0x0454	/* pcmcia */
#define SSB_BOARD_BCM94306MPLNA	0x0457
#define SSB_BOARD_BCM94306MPH	0x045B
#define SSB_BOARD_BCM94306PCIV	0x045C
#define SSB_BOARD_BCM94318MPGH	0x0463
#define SSB_BOARD_BU4311	0x0464
#define SSB_BOARD_BCM94311MC	0x0465
#define SSB_BOARD_BCM94311MCAG	0x0466
/* 4321 boards */
#define SSB_BOARD_BU4321	0x046B
#define SSB_BOARD_BU4321E	0x047C
#define SSB_BOARD_MP4321	0x046C
#define SSB_BOARD_CB2_4321	0x046D
#define SSB_BOARD_CB2_4321_AG	0x0066
#define SSB_BOARD_MC4321	0x046E
/* 4325 boards */
#define SSB_BOARD_BCM94325DEVBU	0x0490
#define SSB_BOARD_BCM94325BGABU	0x0491
#define SSB_BOARD_BCM94325SDGWB	0x0492
#define SSB_BOARD_BCM94325SDGMDL	0x04AA
#define SSB_BOARD_BCM94325SDGMDL2	0x04C6
#define SSB_BOARD_BCM94325SDGMDL3	0x04C9
#define SSB_BOARD_BCM94325SDABGWBA	0x04E1
/* 4322 boards */
#define SSB_BOARD_BCM94322MC	0x04A4
#define SSB_BOARD_BCM94322USB	0x04A8	/* dualband */
#define SSB_BOARD_BCM94322HM	0x04B0
#define SSB_BOARD_BCM94322USB2D	0x04Bf	/* single band discrete front end */
/* 4312 boards */
#define SSB_BOARD_BU4312	0x048A
#define SSB_BOARD_BCM4312MCGSG	0x04B5
/* chip_package */
#define SSB_CHIPPACK_BCM4712S	1	/* Small 200pin 4712 */
#define SSB_CHIPPACK_BCM4712M	2	/* Medium 225pin 4712 */
#define SSB_CHIPPACK_BCM4712L	0	/* Large 340pin 4712 */

#include <linux/ssb/ssb_driver_chipcommon.h>
#include <linux/ssb/ssb_driver_mips.h>
#include <linux/ssb/ssb_driver_extif.h>
#include <linux/ssb/ssb_driver_pci.h>

struct ssb_bus {
	/* The MMIO area. */
	void __iomem *mmio;

	const struct ssb_bus_ops *ops;

	/* The core currently mapped into the MMIO window.
	 * Not valid on all host-buses. So don't use outside of SSB. */
	struct ssb_device *mapped_device;
	union {
		/* Currently mapped PCMCIA segment. (bustype == SSB_BUSTYPE_PCMCIA only) */
		u8 mapped_pcmcia_seg;
		/* Current SSB base address window for SDIO. */
		u32 sdio_sbaddr;
	};
	/* Lock for core and segment switching.
	 * On PCMCIA-host busses this is used to protect the whole MMIO access. */
	spinlock_t bar_lock;

	/* The host-bus this backplane is running on. */
	enum ssb_bustype bustype;
	/* Pointers to the host-bus. Check bustype before using any of these pointers. */
	union {
		/* Pointer to the PCI bus (only valid if bustype == SSB_BUSTYPE_PCI). */
		struct pci_dev *host_pci;
		/* Pointer to the PCMCIA device (only if bustype == SSB_BUSTYPE_PCMCIA). */
		struct pcmcia_device *host_pcmcia;
		/* Pointer to the SDIO device (only if bustype == SSB_BUSTYPE_SDIO). */
		struct sdio_func *host_sdio;
	};

	/* See enum ssb_quirks */
	unsigned int quirks;

#ifdef CONFIG_SSB_SPROM
	/* Mutex to protect the SPROM writing. */
	struct mutex sprom_mutex;
#endif

	/* ID information about the Chip. */
	u16 chip_id;
	u8 chip_rev;
	u16 sprom_offset;
	u16 sprom_size;		/* number of words in sprom */
	u8 chip_package;

	/* List of devices (cores) on the backplane. */
	struct ssb_device devices[SSB_MAX_NR_CORES];
	u8 nr_devices;

	/* Software ID number for this bus. */
	unsigned int busnumber;

	/* The ChipCommon device (if available). */
	struct ssb_chipcommon chipco;
	/* The PCI-core device (if available). */
	struct ssb_pcicore pcicore;
	/* The MIPS-core device (if available). */
	struct ssb_mipscore mipscore;
	/* The EXTif-core device (if available). */
	struct ssb_extif extif;

	/* The following structure elements are not available in early
	 * SSB initialization. Though, they are available for regular
	 * registered drivers at any stage. So be careful when
	 * using them in the ssb core code. */

	/* ID information about the PCB. */
	struct ssb_boardinfo boardinfo;
	/* Contents of the SPROM. */
	struct ssb_sprom sprom;
	/* If the board has a cardbus slot, this is set to true. */
	bool has_cardbus_slot;

#ifdef CONFIG_SSB_EMBEDDED
	/* Lock for GPIO register access. */
	spinlock_t gpio_lock;
	struct platform_device *watchdog;
#endif /* EMBEDDED */
#ifdef CONFIG_SSB_DRIVER_GPIO
	struct gpio_chip gpio;
	struct irq_domain *irq_domain;
#endif /* DRIVER_GPIO */

	/* Internal-only stuff follows. Do not touch. */
	struct list_head list;
	/* Is the bus already powered up? */
	bool powered_up;
	int power_warn_count;
};

enum ssb_quirks {
	/* SDIO connected card requires performing a read after writing a 32-bit value */
	SSB_QUIRK_SDIO_READ_AFTER_WRITE32	= (1 << 0),
};

/* The initialization-invariants. */
struct ssb_init_invariants {
	/* Versioning information about the PCB. */
	struct ssb_boardinfo boardinfo;
	/* The SPROM information. That's either stored in an
	 * EEPROM or NVRAM on the board. */
	struct ssb_sprom sprom;
	/* If the board has a cardbus slot, this is set to true. */
	bool has_cardbus_slot;
};
/* Type of function to fetch the invariants. */
typedef int (*ssb_invariants_func_t)(struct ssb_bus *bus,
				     struct ssb_init_invariants *iv);

/* Register SoC bus. */
extern int ssb_bus_host_soc_register(struct ssb_bus *bus,
				     unsigned long baseaddr);
#ifdef CONFIG_SSB_PCIHOST
extern int ssb_bus_pcibus_register(struct ssb_bus *bus,
				   struct pci_dev *host_pci);
#endif /* CONFIG_SSB_PCIHOST */
#ifdef CONFIG_SSB_PCMCIAHOST
extern int ssb_bus_pcmciabus_register(struct ssb_bus *bus,
				      struct pcmcia_device *pcmcia_dev,
				      unsigned long baseaddr);
#endif /* CONFIG_SSB_PCMCIAHOST */
#ifdef CONFIG_SSB_SDIOHOST
extern int ssb_bus_sdiobus_register(struct ssb_bus *bus,
				    struct sdio_func *sdio_func,
				    unsigned int quirks);
#endif /* CONFIG_SSB_SDIOHOST */


extern void ssb_bus_unregister(struct ssb_bus *bus);

/* Does the device have an SPROM? */
extern bool ssb_is_sprom_available(struct ssb_bus *bus);

/* Set a fallback SPROM.
 * See kdoc at the function definition for complete documentation. */
extern int ssb_arch_register_fallback_sprom(
		int (*sprom_callback)(struct ssb_bus *bus,
		struct ssb_sprom *out));

/* Suspend a SSB bus.
 * Call this from the parent bus suspend routine. */
extern int ssb_bus_suspend(struct ssb_bus *bus);
/* Resume a SSB bus.
 * Call this from the parent bus resume routine. */
extern int ssb_bus_resume(struct ssb_bus *bus);

extern u32 ssb_clockspeed(struct ssb_bus *bus);

/* Is the device enabled in hardware? */
int ssb_device_is_enabled(struct ssb_device *dev);
/* Enable a device and pass device-specific SSB_TMSLOW flags.
 * If no device-specific flags are available, use 0. */
void ssb_device_enable(struct ssb_device *dev, u32 core_specific_flags);
/* Disable a device in hardware and pass SSB_TMSLOW flags (if any). */
void ssb_device_disable(struct ssb_device *dev, u32 core_specific_flags);


/* Device MMIO register read/write functions. */
static inline u8 ssb_read8(struct ssb_device *dev, u16 offset)
{
	return dev->ops->read8(dev, offset);
}
static inline u16 ssb_read16(struct ssb_device *dev, u16 offset)
{
	return dev->ops->read16(dev, offset);
}
static inline u32 ssb_read32(struct ssb_device *dev, u16 offset)
{
	return dev->ops->read32(dev, offset);
}
static inline void ssb_write8(struct ssb_device *dev, u16 offset, u8 value)
{
	dev->ops->write8(dev, offset, value);
}
static inline void ssb_write16(struct ssb_device *dev, u16 offset, u16 value)
{
	dev->ops->write16(dev, offset, value);
}
static inline void ssb_write32(struct ssb_device *dev, u16 offset, u32 value)
{
	dev->ops->write32(dev, offset, value);
}
#ifdef CONFIG_SSB_BLOCKIO
static inline void ssb_block_read(struct ssb_device *dev, void *buffer,
				  size_t count, u16 offset, u8 reg_width)
{
	dev->ops->block_read(dev, buffer, count, offset, reg_width);
}

static inline void ssb_block_write(struct ssb_device *dev, const void *buffer,
				   size_t count, u16 offset, u8 reg_width)
{
	dev->ops->block_write(dev, buffer, count, offset, reg_width);
}
#endif /* CONFIG_SSB_BLOCKIO */


/* The SSB DMA API. Use this API for any DMA operation on the device.
 * This API basically is a wrapper that calls the correct DMA API for
 * the host device type the SSB device is attached to. */

/* Translation (routing) bits that need to be ORed to DMA
 * addresses before they are given to a device. */
extern u32 ssb_dma_translation(struct ssb_device *dev);
#define SSB_DMA_TRANSLATION_MASK	0xC0000000
#define SSB_DMA_TRANSLATION_SHIFT	30

static inline void __cold __ssb_dma_not_implemented(struct ssb_device *dev)
{
#ifdef CONFIG_SSB_DEBUG
	printk(KERN_ERR "SSB: BUG! Calling DMA API for "
	       "unsupported bustype %d\n", dev->bus->bustype);
#endif /* DEBUG */
}

#ifdef CONFIG_SSB_PCIHOST
/* PCI-host wrapper driver */
extern int ssb_pcihost_register(struct pci_driver *driver);
static inline void ssb_pcihost_unregister(struct pci_driver *driver)
{
	pci_unregister_driver(driver);
}

static inline
void ssb_pcihost_set_power_state(struct ssb_device *sdev, pci_power_t state)
{
	if (sdev->bus->bustype == SSB_BUSTYPE_PCI)
		pci_set_power_state(sdev->bus->host_pci, state);
}
#else
static inline void ssb_pcihost_unregister(struct pci_driver *driver)
{
}

static inline
void ssb_pcihost_set_power_state(struct ssb_device *sdev, pci_power_t state)
{
}
#endif /* CONFIG_SSB_PCIHOST */


/* If a driver is shutdown or suspended, call this to signal
 * that the bus may be completely powered down. SSB will decide,
 * if it's really time to power down the bus, based on if there
 * are other devices that want to run. */
extern int ssb_bus_may_powerdown(struct ssb_bus *bus);
/* Before initializing and enabling a device, call this to power-up the bus.
 * If you want to allow use of dynamic-power-control, pass the flag.
 * Otherwise static always-on powercontrol will be used. */
extern int ssb_bus_powerup(struct ssb_bus *bus, bool dynamic_pctl);

extern void ssb_commit_settings(struct ssb_bus *bus);

/* Various helper functions */
extern u32 ssb_admatch_base(u32 adm);
extern u32 ssb_admatch_size(u32 adm);

/* PCI device mapping and fixup routines.
 * Called from the architecture pcibios init code.
 * These are only available on SSB_EMBEDDED configurations. */
#ifdef CONFIG_SSB_EMBEDDED
int ssb_pcibios_plat_dev_init(struct pci_dev *dev);
int ssb_pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin);
#endif /* CONFIG_SSB_EMBEDDED */

#endif /* LINUX_SSB_H_ */

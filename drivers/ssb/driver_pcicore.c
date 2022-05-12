/*
 * Sonics Silicon Backplane
 * Broadcom PCI-core driver
 *
 * Copyright 2005, Broadcom Corporation
 * Copyright 2006, 2007, Michael Buesch <m@bues.ch>
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include "ssb_private.h"

#include <linux/ssb/ssb.h>
#include <linux/pci.h>
#include <linux/export.h>
#include <linux/delay.h>
#include <linux/ssb/ssb_embedded.h>

static u32 ssb_pcie_read(struct ssb_pcicore *pc, u32 address);
static void ssb_pcie_write(struct ssb_pcicore *pc, u32 address, u32 data);
static u16 ssb_pcie_mdio_read(struct ssb_pcicore *pc, u8 device, u8 address);
static void ssb_pcie_mdio_write(struct ssb_pcicore *pc, u8 device,
				u8 address, u16 data);

static inline
u32 pcicore_read32(struct ssb_pcicore *pc, u16 offset)
{
	return ssb_read32(pc->dev, offset);
}

static inline
void pcicore_write32(struct ssb_pcicore *pc, u16 offset, u32 value)
{
	ssb_write32(pc->dev, offset, value);
}

static inline
u16 pcicore_read16(struct ssb_pcicore *pc, u16 offset)
{
	return ssb_read16(pc->dev, offset);
}

static inline
void pcicore_write16(struct ssb_pcicore *pc, u16 offset, u16 value)
{
	ssb_write16(pc->dev, offset, value);
}

/**************************************************
 * Code for hostmode operation.
 **************************************************/

#ifdef CONFIG_SSB_PCICORE_HOSTMODE

#include <asm/paccess.h>
/* Probe a 32bit value on the bus and catch bus exceptions.
 * Returns nonzero on a bus exception.
 * This is MIPS specific */
#define mips_busprobe32(val, addr)	get_dbe((val), ((u32 *)(addr)))

/* Assume one-hot slot wiring */
#define SSB_PCI_SLOT_MAX	16

/* Global lock is OK, as we won't have more than one extpci anyway. */
static DEFINE_SPINLOCK(cfgspace_lock);
/* Core to access the external PCI config space. Can only have one. */
static struct ssb_pcicore *extpci_core;


static u32 get_cfgspace_addr(struct ssb_pcicore *pc,
			     unsigned int bus, unsigned int dev,
			     unsigned int func, unsigned int off)
{
	u32 addr = 0;
	u32 tmp;

	/* We do only have one cardbus device behind the bridge. */
	if (pc->cardbusmode && (dev > 1))
		goto out;

	if (bus == 0) {
		/* Type 0 transaction */
		if (unlikely(dev >= SSB_PCI_SLOT_MAX))
			goto out;
		/* Slide the window */
		tmp = SSB_PCICORE_SBTOPCI_CFG0;
		tmp |= ((1 << (dev + 16)) & SSB_PCICORE_SBTOPCI1_MASK);
		pcicore_write32(pc, SSB_PCICORE_SBTOPCI1, tmp);
		/* Calculate the address */
		addr = SSB_PCI_CFG;
		addr |= ((1 << (dev + 16)) & ~SSB_PCICORE_SBTOPCI1_MASK);
		addr |= (func << 8);
		addr |= (off & ~3);
	} else {
		/* Type 1 transaction */
		pcicore_write32(pc, SSB_PCICORE_SBTOPCI1,
				SSB_PCICORE_SBTOPCI_CFG1);
		/* Calculate the address */
		addr = SSB_PCI_CFG;
		addr |= (bus << 16);
		addr |= (dev << 11);
		addr |= (func << 8);
		addr |= (off & ~3);
	}
out:
	return addr;
}

static int ssb_extpci_read_config(struct ssb_pcicore *pc,
				  unsigned int bus, unsigned int dev,
				  unsigned int func, unsigned int off,
				  void *buf, int len)
{
	int err = -EINVAL;
	u32 addr, val;
	void __iomem *mmio;

	WARN_ON(!pc->hostmode);
	if (unlikely(len != 1 && len != 2 && len != 4))
		goto out;
	addr = get_cfgspace_addr(pc, bus, dev, func, off);
	if (unlikely(!addr))
		goto out;
	err = -ENOMEM;
	mmio = ioremap(addr, len);
	if (!mmio)
		goto out;

	if (mips_busprobe32(val, mmio)) {
		val = 0xffffffff;
		goto unmap;
	}

	val = readl(mmio);
	val >>= (8 * (off & 3));

	switch (len) {
	case 1:
		*((u8 *)buf) = (u8)val;
		break;
	case 2:
		*((u16 *)buf) = (u16)val;
		break;
	case 4:
		*((u32 *)buf) = (u32)val;
		break;
	}
	err = 0;
unmap:
	iounmap(mmio);
out:
	return err;
}

static int ssb_extpci_write_config(struct ssb_pcicore *pc,
				   unsigned int bus, unsigned int dev,
				   unsigned int func, unsigned int off,
				   const void *buf, int len)
{
	int err = -EINVAL;
	u32 addr, val = 0;
	void __iomem *mmio;

	WARN_ON(!pc->hostmode);
	if (unlikely(len != 1 && len != 2 && len != 4))
		goto out;
	addr = get_cfgspace_addr(pc, bus, dev, func, off);
	if (unlikely(!addr))
		goto out;
	err = -ENOMEM;
	mmio = ioremap(addr, len);
	if (!mmio)
		goto out;

	if (mips_busprobe32(val, mmio)) {
		val = 0xffffffff;
		goto unmap;
	}

	switch (len) {
	case 1:
		val = readl(mmio);
		val &= ~(0xFF << (8 * (off & 3)));
		val |= *((const u8 *)buf) << (8 * (off & 3));
		break;
	case 2:
		val = readl(mmio);
		val &= ~(0xFFFF << (8 * (off & 3)));
		val |= *((const u16 *)buf) << (8 * (off & 3));
		break;
	case 4:
		val = *((const u32 *)buf);
		break;
	}
	writel(val, mmio);

	err = 0;
unmap:
	iounmap(mmio);
out:
	return err;
}

static int ssb_pcicore_read_config(struct pci_bus *bus, unsigned int devfn,
				   int reg, int size, u32 *val)
{
	unsigned long flags;
	int err;

	spin_lock_irqsave(&cfgspace_lock, flags);
	err = ssb_extpci_read_config(extpci_core, bus->number, PCI_SLOT(devfn),
				     PCI_FUNC(devfn), reg, val, size);
	spin_unlock_irqrestore(&cfgspace_lock, flags);

	return err ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static int ssb_pcicore_write_config(struct pci_bus *bus, unsigned int devfn,
				    int reg, int size, u32 val)
{
	unsigned long flags;
	int err;

	spin_lock_irqsave(&cfgspace_lock, flags);
	err = ssb_extpci_write_config(extpci_core, bus->number, PCI_SLOT(devfn),
				      PCI_FUNC(devfn), reg, &val, size);
	spin_unlock_irqrestore(&cfgspace_lock, flags);

	return err ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static struct pci_ops ssb_pcicore_pciops = {
	.read	= ssb_pcicore_read_config,
	.write	= ssb_pcicore_write_config,
};

static struct resource ssb_pcicore_mem_resource = {
	.name	= "SSB PCIcore external memory",
	.start	= SSB_PCI_DMA,
	.end	= SSB_PCI_DMA + SSB_PCI_DMA_SZ - 1,
	.flags	= IORESOURCE_MEM | IORESOURCE_PCI_FIXED,
};

static struct resource ssb_pcicore_io_resource = {
	.name	= "SSB PCIcore external I/O",
	.start	= 0x100,
	.end	= 0x7FF,
	.flags	= IORESOURCE_IO | IORESOURCE_PCI_FIXED,
};

static struct pci_controller ssb_pcicore_controller = {
	.pci_ops	= &ssb_pcicore_pciops,
	.io_resource	= &ssb_pcicore_io_resource,
	.mem_resource	= &ssb_pcicore_mem_resource,
};

/* This function is called when doing a pci_enable_device().
 * We must first check if the device is a device on the PCI-core bridge. */
int ssb_pcicore_plat_dev_init(struct pci_dev *d)
{
	if (d->bus->ops != &ssb_pcicore_pciops) {
		/* This is not a device on the PCI-core bridge. */
		return -ENODEV;
	}

	dev_info(&d->dev, "PCI: Fixing up device %s\n", pci_name(d));

	/* Fix up interrupt lines */
	d->irq = ssb_mips_irq(extpci_core->dev) + 2;
	pci_write_config_byte(d, PCI_INTERRUPT_LINE, d->irq);

	return 0;
}

/* Early PCI fixup for a device on the PCI-core bridge. */
static void ssb_pcicore_fixup_pcibridge(struct pci_dev *dev)
{
	u8 lat;

	if (dev->bus->ops != &ssb_pcicore_pciops) {
		/* This is not a device on the PCI-core bridge. */
		return;
	}
	if (dev->bus->number != 0 || PCI_SLOT(dev->devfn) != 0)
		return;

	dev_info(&dev->dev, "PCI: Fixing up bridge %s\n", pci_name(dev));

	/* Enable PCI bridge bus mastering and memory space */
	pci_set_master(dev);
	if (pcibios_enable_device(dev, ~0) < 0) {
		dev_err(&dev->dev, "PCI: SSB bridge enable failed\n");
		return;
	}

	/* Enable PCI bridge BAR1 prefetch and burst */
	pci_write_config_dword(dev, SSB_BAR1_CONTROL, 3);

	/* Make sure our latency is high enough to handle the devices behind us */
	lat = 168;
	dev_info(&dev->dev,
		 "PCI: Fixing latency timer of device %s to %u\n",
		 pci_name(dev), lat);
	pci_write_config_byte(dev, PCI_LATENCY_TIMER, lat);
}
DECLARE_PCI_FIXUP_EARLY(PCI_ANY_ID, PCI_ANY_ID, ssb_pcicore_fixup_pcibridge);

/* PCI device IRQ mapping. */
int ssb_pcicore_pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	if (dev->bus->ops != &ssb_pcicore_pciops) {
		/* This is not a device on the PCI-core bridge. */
		return -ENODEV;
	}
	return ssb_mips_irq(extpci_core->dev) + 2;
}

static void ssb_pcicore_init_hostmode(struct ssb_pcicore *pc)
{
	u32 val;

	if (WARN_ON(extpci_core))
		return;
	extpci_core = pc;

	dev_dbg(pc->dev->dev, "PCIcore in host mode found\n");
	/* Reset devices on the external PCI bus */
	val = SSB_PCICORE_CTL_RST_OE;
	val |= SSB_PCICORE_CTL_CLK_OE;
	pcicore_write32(pc, SSB_PCICORE_CTL, val);
	val |= SSB_PCICORE_CTL_CLK; /* Clock on */
	pcicore_write32(pc, SSB_PCICORE_CTL, val);
	udelay(150); /* Assertion time demanded by the PCI standard */
	val |= SSB_PCICORE_CTL_RST; /* Deassert RST# */
	pcicore_write32(pc, SSB_PCICORE_CTL, val);
	val = SSB_PCICORE_ARBCTL_INTERN;
	pcicore_write32(pc, SSB_PCICORE_ARBCTL, val);
	udelay(1); /* Assertion time demanded by the PCI standard */

	if (pc->dev->bus->has_cardbus_slot) {
		dev_dbg(pc->dev->dev, "CardBus slot detected\n");
		pc->cardbusmode = 1;
		/* GPIO 1 resets the bridge */
		ssb_gpio_out(pc->dev->bus, 1, 1);
		ssb_gpio_outen(pc->dev->bus, 1, 1);
		pcicore_write16(pc, SSB_PCICORE_SPROM(0),
				pcicore_read16(pc, SSB_PCICORE_SPROM(0))
				| 0x0400);
	}

	/* 64MB I/O window */
	pcicore_write32(pc, SSB_PCICORE_SBTOPCI0,
			SSB_PCICORE_SBTOPCI_IO);
	/* 64MB config space */
	pcicore_write32(pc, SSB_PCICORE_SBTOPCI1,
			SSB_PCICORE_SBTOPCI_CFG0);
	/* 1GB memory window */
	pcicore_write32(pc, SSB_PCICORE_SBTOPCI2,
			SSB_PCICORE_SBTOPCI_MEM | SSB_PCI_DMA);

	/*
	 * Accessing PCI config without a proper delay after devices reset (not
	 * GPIO reset) was causing reboots on WRT300N v1.0 (BCM4704).
	 * Tested delay 850 us lowered reboot chance to 50-80%, 1000 us fixed it
	 * completely. Flushing all writes was also tested but with no luck.
	 * The same problem was reported for WRT350N v1 (BCM4705), so we just
	 * sleep here unconditionally.
	 */
	usleep_range(1000, 2000);

	/* Enable PCI bridge BAR0 prefetch and burst */
	val = PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	ssb_extpci_write_config(pc, 0, 0, 0, PCI_COMMAND, &val, 2);
	/* Clear error conditions */
	val = 0;
	ssb_extpci_write_config(pc, 0, 0, 0, PCI_STATUS, &val, 2);

	/* Enable PCI interrupts */
	pcicore_write32(pc, SSB_PCICORE_IMASK,
			SSB_PCICORE_IMASK_INTA);

	/* Ok, ready to run, register it to the system.
	 * The following needs change, if we want to port hostmode
	 * to non-MIPS platform. */
	ssb_pcicore_controller.io_map_base = (unsigned long)ioremap(SSB_PCI_MEM, 0x04000000);
	set_io_port_base(ssb_pcicore_controller.io_map_base);
	/* Give some time to the PCI controller to configure itself with the new
	 * values. Not waiting at this point causes crashes of the machine. */
	mdelay(10);
	register_pci_controller(&ssb_pcicore_controller);
}

static int pcicore_is_in_hostmode(struct ssb_pcicore *pc)
{
	struct ssb_bus *bus = pc->dev->bus;
	u16 chipid_top;
	u32 tmp;

	chipid_top = (bus->chip_id & 0xFF00);
	if (chipid_top != 0x4700 &&
	    chipid_top != 0x5300)
		return 0;

	if (bus->sprom.boardflags_lo & SSB_PCICORE_BFL_NOPCI)
		return 0;

	/* The 200-pin BCM4712 package does not bond out PCI. Even when
	 * PCI is bonded out, some boards may leave the pins floating. */
	if (bus->chip_id == 0x4712) {
		if (bus->chip_package == SSB_CHIPPACK_BCM4712S)
			return 0;
		if (bus->chip_package == SSB_CHIPPACK_BCM4712M)
			return 0;
	}
	if (bus->chip_id == 0x5350)
		return 0;

	return !mips_busprobe32(tmp, (bus->mmio + (pc->dev->core_index * SSB_CORE_SIZE)));
}
#endif /* CONFIG_SSB_PCICORE_HOSTMODE */

/**************************************************
 * Workarounds.
 **************************************************/

static void ssb_pcicore_fix_sprom_core_index(struct ssb_pcicore *pc)
{
	u16 tmp = pcicore_read16(pc, SSB_PCICORE_SPROM(0));
	if (((tmp & 0xF000) >> 12) != pc->dev->core_index) {
		tmp &= ~0xF000;
		tmp |= (pc->dev->core_index << 12);
		pcicore_write16(pc, SSB_PCICORE_SPROM(0), tmp);
	}
}

static u8 ssb_pcicore_polarity_workaround(struct ssb_pcicore *pc)
{
	return (ssb_pcie_read(pc, 0x204) & 0x10) ? 0xC0 : 0x80;
}

static void ssb_pcicore_serdes_workaround(struct ssb_pcicore *pc)
{
	const u8 serdes_pll_device = 0x1D;
	const u8 serdes_rx_device = 0x1F;
	u16 tmp;

	ssb_pcie_mdio_write(pc, serdes_rx_device, 1 /* Control */,
			    ssb_pcicore_polarity_workaround(pc));
	tmp = ssb_pcie_mdio_read(pc, serdes_pll_device, 1 /* Control */);
	if (tmp & 0x4000)
		ssb_pcie_mdio_write(pc, serdes_pll_device, 1, tmp & ~0x4000);
}

static void ssb_pcicore_pci_setup_workarounds(struct ssb_pcicore *pc)
{
	struct ssb_device *pdev = pc->dev;
	struct ssb_bus *bus = pdev->bus;
	u32 tmp;

	tmp = pcicore_read32(pc, SSB_PCICORE_SBTOPCI2);
	tmp |= SSB_PCICORE_SBTOPCI_PREF;
	tmp |= SSB_PCICORE_SBTOPCI_BURST;
	pcicore_write32(pc, SSB_PCICORE_SBTOPCI2, tmp);

	if (pdev->id.revision < 5) {
		tmp = ssb_read32(pdev, SSB_IMCFGLO);
		tmp &= ~SSB_IMCFGLO_SERTO;
		tmp |= 2;
		tmp &= ~SSB_IMCFGLO_REQTO;
		tmp |= 3 << SSB_IMCFGLO_REQTO_SHIFT;
		ssb_write32(pdev, SSB_IMCFGLO, tmp);
		ssb_commit_settings(bus);
	} else if (pdev->id.revision >= 11) {
		tmp = pcicore_read32(pc, SSB_PCICORE_SBTOPCI2);
		tmp |= SSB_PCICORE_SBTOPCI_MRM;
		pcicore_write32(pc, SSB_PCICORE_SBTOPCI2, tmp);
	}
}

static void ssb_pcicore_pcie_setup_workarounds(struct ssb_pcicore *pc)
{
	u32 tmp;
	u8 rev = pc->dev->id.revision;

	if (rev == 0 || rev == 1) {
		/* TLP Workaround register. */
		tmp = ssb_pcie_read(pc, 0x4);
		tmp |= 0x8;
		ssb_pcie_write(pc, 0x4, tmp);
	}
	if (rev == 1) {
		/* DLLP Link Control register. */
		tmp = ssb_pcie_read(pc, 0x100);
		tmp |= 0x40;
		ssb_pcie_write(pc, 0x100, tmp);
	}

	if (rev == 0) {
		const u8 serdes_rx_device = 0x1F;

		ssb_pcie_mdio_write(pc, serdes_rx_device,
					2 /* Timer */, 0x8128);
		ssb_pcie_mdio_write(pc, serdes_rx_device,
					6 /* CDR */, 0x0100);
		ssb_pcie_mdio_write(pc, serdes_rx_device,
					7 /* CDR BW */, 0x1466);
	} else if (rev == 3 || rev == 4 || rev == 5) {
		/* TODO: DLLP Power Management Threshold */
		ssb_pcicore_serdes_workaround(pc);
		/* TODO: ASPM */
	} else if (rev == 7) {
		/* TODO: No PLL down */
	}

	if (rev >= 6) {
		/* Miscellaneous Configuration Fixup */
		tmp = pcicore_read16(pc, SSB_PCICORE_SPROM(5));
		if (!(tmp & 0x8000))
			pcicore_write16(pc, SSB_PCICORE_SPROM(5),
					tmp | 0x8000);
	}
}

/**************************************************
 * Generic and Clientmode operation code.
 **************************************************/

static void ssb_pcicore_init_clientmode(struct ssb_pcicore *pc)
{
	struct ssb_device *pdev = pc->dev;
	struct ssb_bus *bus = pdev->bus;

	if (bus->bustype == SSB_BUSTYPE_PCI)
		ssb_pcicore_fix_sprom_core_index(pc);

	/* Disable PCI interrupts. */
	ssb_write32(pdev, SSB_INTVEC, 0);

	/* Additional PCIe always once-executed workarounds */
	if (pc->dev->id.coreid == SSB_DEV_PCIE) {
		ssb_pcicore_serdes_workaround(pc);
		/* TODO: ASPM */
		/* TODO: Clock Request Update */
	}
}

void ssb_pcicore_init(struct ssb_pcicore *pc)
{
	struct ssb_device *dev = pc->dev;

	if (!dev)
		return;
	if (!ssb_device_is_enabled(dev))
		ssb_device_enable(dev, 0);

#ifdef CONFIG_SSB_PCICORE_HOSTMODE
	pc->hostmode = pcicore_is_in_hostmode(pc);
	if (pc->hostmode)
		ssb_pcicore_init_hostmode(pc);
#endif /* CONFIG_SSB_PCICORE_HOSTMODE */
	if (!pc->hostmode)
		ssb_pcicore_init_clientmode(pc);
}

static u32 ssb_pcie_read(struct ssb_pcicore *pc, u32 address)
{
	pcicore_write32(pc, 0x130, address);
	return pcicore_read32(pc, 0x134);
}

static void ssb_pcie_write(struct ssb_pcicore *pc, u32 address, u32 data)
{
	pcicore_write32(pc, 0x130, address);
	pcicore_write32(pc, 0x134, data);
}

static void ssb_pcie_mdio_set_phy(struct ssb_pcicore *pc, u8 phy)
{
	const u16 mdio_control = 0x128;
	const u16 mdio_data = 0x12C;
	u32 v;
	int i;

	v = (1 << 30); /* Start of Transaction */
	v |= (1 << 28); /* Write Transaction */
	v |= (1 << 17); /* Turnaround */
	v |= (0x1F << 18);
	v |= (phy << 4);
	pcicore_write32(pc, mdio_data, v);

	udelay(10);
	for (i = 0; i < 200; i++) {
		v = pcicore_read32(pc, mdio_control);
		if (v & 0x100 /* Trans complete */)
			break;
		msleep(1);
	}
}

static u16 ssb_pcie_mdio_read(struct ssb_pcicore *pc, u8 device, u8 address)
{
	const u16 mdio_control = 0x128;
	const u16 mdio_data = 0x12C;
	int max_retries = 10;
	u16 ret = 0;
	u32 v;
	int i;

	v = 0x80; /* Enable Preamble Sequence */
	v |= 0x2; /* MDIO Clock Divisor */
	pcicore_write32(pc, mdio_control, v);

	if (pc->dev->id.revision >= 10) {
		max_retries = 200;
		ssb_pcie_mdio_set_phy(pc, device);
	}

	v = (1 << 30); /* Start of Transaction */
	v |= (1 << 29); /* Read Transaction */
	v |= (1 << 17); /* Turnaround */
	if (pc->dev->id.revision < 10)
		v |= (u32)device << 22;
	v |= (u32)address << 18;
	pcicore_write32(pc, mdio_data, v);
	/* Wait for the device to complete the transaction */
	udelay(10);
	for (i = 0; i < max_retries; i++) {
		v = pcicore_read32(pc, mdio_control);
		if (v & 0x100 /* Trans complete */) {
			udelay(10);
			ret = pcicore_read32(pc, mdio_data);
			break;
		}
		msleep(1);
	}
	pcicore_write32(pc, mdio_control, 0);
	return ret;
}

static void ssb_pcie_mdio_write(struct ssb_pcicore *pc, u8 device,
				u8 address, u16 data)
{
	const u16 mdio_control = 0x128;
	const u16 mdio_data = 0x12C;
	int max_retries = 10;
	u32 v;
	int i;

	v = 0x80; /* Enable Preamble Sequence */
	v |= 0x2; /* MDIO Clock Divisor */
	pcicore_write32(pc, mdio_control, v);

	if (pc->dev->id.revision >= 10) {
		max_retries = 200;
		ssb_pcie_mdio_set_phy(pc, device);
	}

	v = (1 << 30); /* Start of Transaction */
	v |= (1 << 28); /* Write Transaction */
	v |= (1 << 17); /* Turnaround */
	if (pc->dev->id.revision < 10)
		v |= (u32)device << 22;
	v |= (u32)address << 18;
	v |= data;
	pcicore_write32(pc, mdio_data, v);
	/* Wait for the device to complete the transaction */
	udelay(10);
	for (i = 0; i < max_retries; i++) {
		v = pcicore_read32(pc, mdio_control);
		if (v & 0x100 /* Trans complete */)
			break;
		msleep(1);
	}
	pcicore_write32(pc, mdio_control, 0);
}

int ssb_pcicore_dev_irqvecs_enable(struct ssb_pcicore *pc,
				   struct ssb_device *dev)
{
	struct ssb_device *pdev = pc->dev;
	struct ssb_bus *bus;
	int err = 0;
	u32 tmp;

	if (dev->bus->bustype != SSB_BUSTYPE_PCI) {
		/* This SSB device is not on a PCI host-bus. So the IRQs are
		 * not routed through the PCI core.
		 * So we must not enable routing through the PCI core. */
		goto out;
	}

	if (!pdev)
		goto out;
	bus = pdev->bus;

	might_sleep_if(pdev->id.coreid != SSB_DEV_PCI);

	/* Enable interrupts for this device. */
	if ((pdev->id.revision >= 6) || (pdev->id.coreid == SSB_DEV_PCIE)) {
		u32 coremask;

		/* Calculate the "coremask" for the device. */
		coremask = (1 << dev->core_index);

		WARN_ON(bus->bustype != SSB_BUSTYPE_PCI);
		err = pci_read_config_dword(bus->host_pci, SSB_PCI_IRQMASK, &tmp);
		if (err)
			goto out;
		tmp |= coremask << 8;
		err = pci_write_config_dword(bus->host_pci, SSB_PCI_IRQMASK, tmp);
		if (err)
			goto out;
	} else {
		u32 intvec;

		intvec = ssb_read32(pdev, SSB_INTVEC);
		tmp = ssb_read32(dev, SSB_TPSFLAG);
		tmp &= SSB_TPSFLAG_BPFLAG;
		intvec |= (1 << tmp);
		ssb_write32(pdev, SSB_INTVEC, intvec);
	}

	/* Setup PCIcore operation. */
	if (pc->setup_done)
		goto out;
	if (pdev->id.coreid == SSB_DEV_PCI) {
		ssb_pcicore_pci_setup_workarounds(pc);
	} else {
		WARN_ON(pdev->id.coreid != SSB_DEV_PCIE);
		ssb_pcicore_pcie_setup_workarounds(pc);
	}
	pc->setup_done = 1;
out:
	return err;
}
EXPORT_SYMBOL(ssb_pcicore_dev_irqvecs_enable);

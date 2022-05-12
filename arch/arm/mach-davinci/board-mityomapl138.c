/*
 * Critical Link MityOMAP-L138 SoM
 *
 * Copyright (C) 2010 Critical Link LLC - https://www.criticallink.com
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of
 * any kind, whether express or implied.
 */

#define pr_fmt(fmt) "MityOMAPL138: " fmt

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/mtd/partitions.h>
#include <linux/notifier.h>
#include <linux/nvmem-consumer.h>
#include <linux/nvmem-provider.h>
#include <linux/regulator/machine.h>
#include <linux/i2c.h>
#include <linux/etherdevice.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/common.h>
#include <mach/da8xx.h>
#include <linux/platform_data/mtd-davinci.h>
#include <linux/platform_data/mtd-davinci-aemif.h>
#include <linux/platform_data/ti-aemif.h>
#include <mach/mux.h>
#include <linux/platform_data/spi-davinci.h>

#define MITYOMAPL138_PHY_ID		""

#define FACTORY_CONFIG_MAGIC	0x012C0138
#define FACTORY_CONFIG_VERSION	0x00010001

/* Data Held in On-Board I2C device */
struct factory_config {
	u32	magic;
	u32	version;
	u8	mac[6];
	u32	fpga_type;
	u32	spare;
	u32	serialnumber;
	char	partnum[32];
};

static struct factory_config factory_config;

#ifdef CONFIG_CPU_FREQ
struct part_no_info {
	const char	*part_no;	/* part number string of interest */
	int		max_freq;	/* khz */
};

static struct part_no_info mityomapl138_pn_info[] = {
	{
		.part_no	= "L138-C",
		.max_freq	= 300000,
	},
	{
		.part_no	= "L138-D",
		.max_freq	= 375000,
	},
	{
		.part_no	= "L138-F",
		.max_freq	= 456000,
	},
	{
		.part_no	= "1808-C",
		.max_freq	= 300000,
	},
	{
		.part_no	= "1808-D",
		.max_freq	= 375000,
	},
	{
		.part_no	= "1808-F",
		.max_freq	= 456000,
	},
	{
		.part_no	= "1810-D",
		.max_freq	= 375000,
	},
};

static void mityomapl138_cpufreq_init(const char *partnum)
{
	int i, ret;

	for (i = 0; partnum && i < ARRAY_SIZE(mityomapl138_pn_info); i++) {
		/*
		 * the part number has additional characters beyond what is
		 * stored in the table.  This information is not needed for
		 * determining the speed grade, and would require several
		 * more table entries.  Only check the first N characters
		 * for a match.
		 */
		if (!strncmp(partnum, mityomapl138_pn_info[i].part_no,
			     strlen(mityomapl138_pn_info[i].part_no))) {
			da850_max_speed = mityomapl138_pn_info[i].max_freq;
			break;
		}
	}

	ret = da850_register_cpufreq("pll0_sysclk3");
	if (ret)
		pr_warn("cpufreq registration failed: %d\n", ret);
}
#else
static void mityomapl138_cpufreq_init(const char *partnum) { }
#endif

static int read_factory_config(struct notifier_block *nb,
			       unsigned long event, void *data)
{
	int ret;
	const char *partnum = NULL;
	struct nvmem_device *nvmem = data;

	if (strcmp(nvmem_dev_name(nvmem), "1-00500") != 0)
		return NOTIFY_DONE;

	if (!IS_BUILTIN(CONFIG_NVMEM)) {
		pr_warn("Factory Config not available without CONFIG_NVMEM\n");
		goto bad_config;
	}

	ret = nvmem_device_read(nvmem, 0, sizeof(factory_config),
				&factory_config);
	if (ret != sizeof(struct factory_config)) {
		pr_warn("Read Factory Config Failed: %d\n", ret);
		goto bad_config;
	}

	if (factory_config.magic != FACTORY_CONFIG_MAGIC) {
		pr_warn("Factory Config Magic Wrong (%X)\n",
			factory_config.magic);
		goto bad_config;
	}

	if (factory_config.version != FACTORY_CONFIG_VERSION) {
		pr_warn("Factory Config Version Wrong (%X)\n",
			factory_config.version);
		goto bad_config;
	}

	partnum = factory_config.partnum;
	pr_info("Part Number = %s\n", partnum);

bad_config:
	/* default maximum speed is valid for all platforms */
	mityomapl138_cpufreq_init(partnum);

	return NOTIFY_STOP;
}

static struct notifier_block mityomapl138_nvmem_notifier = {
	.notifier_call = read_factory_config,
};

/*
 * We don't define a cell for factory config as it will be accessed from the
 * board file using the nvmem notifier chain.
 */
static struct nvmem_cell_info mityomapl138_nvmem_cells[] = {
	{
		.name		= "macaddr",
		.offset		= 0x64,
		.bytes		= ETH_ALEN,
	}
};

static struct nvmem_cell_table mityomapl138_nvmem_cell_table = {
	.nvmem_name	= "1-00500",
	.cells		= mityomapl138_nvmem_cells,
	.ncells		= ARRAY_SIZE(mityomapl138_nvmem_cells),
};

static struct nvmem_cell_lookup mityomapl138_nvmem_cell_lookup = {
	.nvmem_name	= "1-00500",
	.cell_name	= "macaddr",
	.dev_id		= "davinci_emac.1",
	.con_id		= "mac-address",
};

static const struct property_entry mityomapl138_fd_chip_properties[] = {
	PROPERTY_ENTRY_U32("pagesize", 8),
	PROPERTY_ENTRY_BOOL("read-only"),
	{ }
};

static const struct software_node mityomapl138_fd_chip_node = {
	.properties = mityomapl138_fd_chip_properties,
};

static struct davinci_i2c_platform_data mityomap_i2c_0_pdata = {
	.bus_freq	= 100,	/* kHz */
	.bus_delay	= 0,	/* usec */
};

/* TPS65023 voltage regulator support */
/* 1.2V Core */
static struct regulator_consumer_supply tps65023_dcdc1_consumers[] = {
	{
		.supply = "cvdd",
	},
};

/* 1.8V */
static struct regulator_consumer_supply tps65023_dcdc2_consumers[] = {
	{
		.supply = "usb0_vdda18",
	},
	{
		.supply = "usb1_vdda18",
	},
	{
		.supply = "ddr_dvdd18",
	},
	{
		.supply = "sata_vddr",
	},
};

/* 1.2V */
static struct regulator_consumer_supply tps65023_dcdc3_consumers[] = {
	{
		.supply = "sata_vdd",
	},
	{
		.supply = "usb_cvdd",
	},
	{
		.supply = "pll0_vdda",
	},
	{
		.supply = "pll1_vdda",
	},
};

/* 1.8V Aux LDO, not used */
static struct regulator_consumer_supply tps65023_ldo1_consumers[] = {
	{
		.supply = "1.8v_aux",
	},
};

/* FPGA VCC Aux (2.5 or 3.3) LDO */
static struct regulator_consumer_supply tps65023_ldo2_consumers[] = {
	{
		.supply = "vccaux",
	},
};

static struct regulator_init_data tps65023_regulator_data[] = {
	/* dcdc1 */
	{
		.constraints = {
			.min_uV = 1150000,
			.max_uV = 1350000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_dcdc1_consumers),
		.consumer_supplies = tps65023_dcdc1_consumers,
	},
	/* dcdc2 */
	{
		.constraints = {
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_dcdc2_consumers),
		.consumer_supplies = tps65023_dcdc2_consumers,
	},
	/* dcdc3 */
	{
		.constraints = {
			.min_uV = 1200000,
			.max_uV = 1200000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_dcdc3_consumers),
		.consumer_supplies = tps65023_dcdc3_consumers,
	},
	/* ldo1 */
	{
		.constraints = {
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_ldo1_consumers),
		.consumer_supplies = tps65023_ldo1_consumers,
	},
	/* ldo2 */
	{
		.constraints = {
			.min_uV = 2500000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_ldo2_consumers),
		.consumer_supplies = tps65023_ldo2_consumers,
	},
};

static struct i2c_board_info __initdata mityomap_tps65023_info[] = {
	{
		I2C_BOARD_INFO("tps65023", 0x48),
		.platform_data = &tps65023_regulator_data[0],
	},
	{
		I2C_BOARD_INFO("24c02", 0x50),
		.swnode = &mityomapl138_fd_chip_node,
	},
};

static int __init pmic_tps65023_init(void)
{
	return i2c_register_board_info(1, mityomap_tps65023_info,
					ARRAY_SIZE(mityomap_tps65023_info));
}

/*
 * SPI Devices:
 *	SPI1_CS0: 8M Flash ST-M25P64-VME6G
 */
static struct mtd_partition spi_flash_partitions[] = {
	[0] = {
		.name		= "ubl",
		.offset		= 0,
		.size		= SZ_64K,
		.mask_flags	= MTD_WRITEABLE,
	},
	[1] = {
		.name		= "u-boot",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_512K,
		.mask_flags	= MTD_WRITEABLE,
	},
	[2] = {
		.name		= "u-boot-env",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_64K,
		.mask_flags	= MTD_WRITEABLE,
	},
	[3] = {
		.name		= "periph-config",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_64K,
		.mask_flags	= MTD_WRITEABLE,
	},
	[4] = {
		.name		= "reserved",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_256K + SZ_64K,
	},
	[5] = {
		.name		= "kernel",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_2M + SZ_1M,
	},
	[6] = {
		.name		= "fpga",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_2M,
	},
	[7] = {
		.name		= "spare",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
	},
};

static struct flash_platform_data mityomapl138_spi_flash_data = {
	.name		= "m25p80",
	.parts		= spi_flash_partitions,
	.nr_parts	= ARRAY_SIZE(spi_flash_partitions),
	.type		= "m24p64",
};

static struct davinci_spi_config spi_eprom_config = {
	.io_type	= SPI_IO_TYPE_DMA,
	.c2tdelay	= 8,
	.t2cdelay	= 8,
};

static struct spi_board_info mityomapl138_spi_flash_info[] = {
	{
		.modalias		= "m25p80",
		.platform_data		= &mityomapl138_spi_flash_data,
		.controller_data	= &spi_eprom_config,
		.mode			= SPI_MODE_0,
		.max_speed_hz		= 30000000,
		.bus_num		= 1,
		.chip_select		= 0,
	},
};

/*
 * MityDSP-L138 includes a 256 MByte large-page NAND flash
 * (128K blocks).
 */
static struct mtd_partition mityomapl138_nandflash_partition[] = {
	{
		.name		= "rootfs",
		.offset		= 0,
		.size		= SZ_128M,
		.mask_flags	= 0, /* MTD_WRITEABLE, */
	},
	{
		.name		= "homefs",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
		.mask_flags	= 0,
	},
};

static struct davinci_nand_pdata mityomapl138_nandflash_data = {
	.core_chipsel	= 1,
	.parts		= mityomapl138_nandflash_partition,
	.nr_parts	= ARRAY_SIZE(mityomapl138_nandflash_partition),
	.engine_type	= NAND_ECC_ENGINE_TYPE_ON_HOST,
	.bbt_options	= NAND_BBT_USE_FLASH,
	.options	= NAND_BUSWIDTH_16,
	.ecc_bits	= 1, /* 4 bit mode is not supported with 16 bit NAND */
};

static struct resource mityomapl138_nandflash_resource[] = {
	{
		.start	= DA8XX_AEMIF_CS3_BASE,
		.end	= DA8XX_AEMIF_CS3_BASE + SZ_512K + 2 * SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= DA8XX_AEMIF_CTL_BASE,
		.end	= DA8XX_AEMIF_CTL_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device mityomapl138_aemif_devices[] = {
	{
		.name		= "davinci_nand",
		.id		= 1,
		.dev		= {
			.platform_data	= &mityomapl138_nandflash_data,
		},
		.num_resources	= ARRAY_SIZE(mityomapl138_nandflash_resource),
		.resource	= mityomapl138_nandflash_resource,
	},
};

static struct resource mityomapl138_aemif_resources[] = {
	{
		.start	= DA8XX_AEMIF_CTL_BASE,
		.end	= DA8XX_AEMIF_CTL_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct aemif_abus_data mityomapl138_aemif_abus_data[] = {
	{
		.cs	= 1,
	},
};

static struct aemif_platform_data mityomapl138_aemif_pdata = {
	.abus_data		= mityomapl138_aemif_abus_data,
	.num_abus_data		= ARRAY_SIZE(mityomapl138_aemif_abus_data),
	.sub_devices		= mityomapl138_aemif_devices,
	.num_sub_devices	= ARRAY_SIZE(mityomapl138_aemif_devices),
};

static struct platform_device mityomapl138_aemif_device = {
	.name		= "ti-aemif",
	.id		= -1,
	.dev = {
		.platform_data	= &mityomapl138_aemif_pdata,
	},
	.resource	= mityomapl138_aemif_resources,
	.num_resources	= ARRAY_SIZE(mityomapl138_aemif_resources),
};

static void __init mityomapl138_setup_nand(void)
{
	if (platform_device_register(&mityomapl138_aemif_device))
		pr_warn("%s: Cannot register AEMIF device\n", __func__);
}

static const short mityomap_mii_pins[] = {
	DA850_MII_TXEN, DA850_MII_TXCLK, DA850_MII_COL, DA850_MII_TXD_3,
	DA850_MII_TXD_2, DA850_MII_TXD_1, DA850_MII_TXD_0, DA850_MII_RXER,
	DA850_MII_CRS, DA850_MII_RXCLK, DA850_MII_RXDV, DA850_MII_RXD_3,
	DA850_MII_RXD_2, DA850_MII_RXD_1, DA850_MII_RXD_0, DA850_MDIO_CLK,
	DA850_MDIO_D,
	-1
};

static const short mityomap_rmii_pins[] = {
	DA850_RMII_TXD_0, DA850_RMII_TXD_1, DA850_RMII_TXEN,
	DA850_RMII_CRS_DV, DA850_RMII_RXD_0, DA850_RMII_RXD_1,
	DA850_RMII_RXER, DA850_RMII_MHZ_50_CLK, DA850_MDIO_CLK,
	DA850_MDIO_D,
	-1
};

static void __init mityomapl138_config_emac(void)
{
	void __iomem *cfg_chip3_base;
	int ret;
	u32 val;
	struct davinci_soc_info *soc_info = &davinci_soc_info;

	soc_info->emac_pdata->rmii_en = 0; /* hardcoded for now */

	cfg_chip3_base = DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP3_REG);
	val = __raw_readl(cfg_chip3_base);

	if (soc_info->emac_pdata->rmii_en) {
		val |= BIT(8);
		ret = davinci_cfg_reg_list(mityomap_rmii_pins);
		pr_info("RMII PHY configured\n");
	} else {
		val &= ~BIT(8);
		ret = davinci_cfg_reg_list(mityomap_mii_pins);
		pr_info("MII PHY configured\n");
	}

	if (ret) {
		pr_warn("mii/rmii mux setup failed: %d\n", ret);
		return;
	}

	/* configure the CFGCHIP3 register for RMII or MII */
	__raw_writel(val, cfg_chip3_base);

	soc_info->emac_pdata->phy_id = MITYOMAPL138_PHY_ID;

	ret = da8xx_register_emac();
	if (ret)
		pr_warn("emac registration failed: %d\n", ret);
}

static void __init mityomapl138_init(void)
{
	int ret;

	da850_register_clocks();

	/* for now, no special EDMA channels are reserved */
	ret = da850_register_edma(NULL);
	if (ret)
		pr_warn("edma registration failed: %d\n", ret);

	ret = da8xx_register_watchdog();
	if (ret)
		pr_warn("watchdog registration failed: %d\n", ret);

	davinci_serial_init(da8xx_serial_device);

	nvmem_register_notifier(&mityomapl138_nvmem_notifier);
	nvmem_add_cell_table(&mityomapl138_nvmem_cell_table);
	nvmem_add_cell_lookups(&mityomapl138_nvmem_cell_lookup, 1);

	ret = da8xx_register_i2c(0, &mityomap_i2c_0_pdata);
	if (ret)
		pr_warn("i2c0 registration failed: %d\n", ret);

	ret = pmic_tps65023_init();
	if (ret)
		pr_warn("TPS65023 PMIC init failed: %d\n", ret);

	mityomapl138_setup_nand();

	ret = spi_register_board_info(mityomapl138_spi_flash_info,
				      ARRAY_SIZE(mityomapl138_spi_flash_info));
	if (ret)
		pr_warn("spi info registration failed: %d\n", ret);

	ret = da8xx_register_spi_bus(1,
				     ARRAY_SIZE(mityomapl138_spi_flash_info));
	if (ret)
		pr_warn("spi 1 registration failed: %d\n", ret);

	mityomapl138_config_emac();

	ret = da8xx_register_rtc();
	if (ret)
		pr_warn("rtc setup failed: %d\n", ret);

	ret = da8xx_register_cpuidle();
	if (ret)
		pr_warn("cpuidle registration failed: %d\n", ret);

	davinci_pm_init();
}

#ifdef CONFIG_SERIAL_8250_CONSOLE
static int __init mityomapl138_console_init(void)
{
	if (!machine_is_mityomapl138())
		return 0;

	return add_preferred_console("ttyS", 1, "115200");
}
console_initcall(mityomapl138_console_init);
#endif

static void __init mityomapl138_map_io(void)
{
	da850_init();
}

MACHINE_START(MITYOMAPL138, "MityDSP-L138/MityARM-1808")
	.atag_offset	= 0x100,
	.map_io		= mityomapl138_map_io,
	.init_irq	= da850_init_irq,
	.init_time	= da850_init_time,
	.init_machine	= mityomapl138_init,
	.init_late	= davinci_init_late,
	.dma_zone_size	= SZ_128M,
MACHINE_END

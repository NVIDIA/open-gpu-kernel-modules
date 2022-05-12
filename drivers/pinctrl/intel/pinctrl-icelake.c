// SPDX-License-Identifier: GPL-2.0
/*
 * Intel Ice Lake PCH pinctrl/GPIO driver
 *
 * Copyright (C) 2018, Intel Corporation
 * Authors: Andy Shevchenko <andriy.shevchenko@linux.intel.com>
 *	    Mika Westerberg <mika.westerberg@linux.intel.com>
 */

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-intel.h"

#define ICL_PAD_OWN	0x020
#define ICL_PADCFGLOCK	0x080
#define ICL_HOSTSW_OWN	0x0b0
#define ICL_GPI_IS	0x100
#define ICL_GPI_IE	0x110

#define ICL_GPP(r, s, e, g)				\
	{						\
		.reg_num = (r),				\
		.base = (s),				\
		.size = ((e) - (s) + 1),		\
		.gpio_base = (g),			\
	}

#define ICL_COMMUNITY(b, s, e, g)			\
	{						\
		.barno = (b),				\
		.padown_offset = ICL_PAD_OWN,		\
		.padcfglock_offset = ICL_PADCFGLOCK,	\
		.hostown_offset = ICL_HOSTSW_OWN,	\
		.is_offset = ICL_GPI_IS,		\
		.ie_offset = ICL_GPI_IE,		\
		.pin_base = (s),			\
		.npins = ((e) - (s) + 1),		\
		.gpps = (g),				\
		.ngpps = ARRAY_SIZE(g),			\
	}

/* Ice Lake-LP */
static const struct pinctrl_pin_desc icllp_pins[] = {
	/* GPP_G */
	PINCTRL_PIN(0, "SD3_CMD"),
	PINCTRL_PIN(1, "SD3_D0"),
	PINCTRL_PIN(2, "SD3_D1"),
	PINCTRL_PIN(3, "SD3_D2"),
	PINCTRL_PIN(4, "SD3_D3"),
	PINCTRL_PIN(5, "SD3_CDB"),
	PINCTRL_PIN(6, "SD3_CLK"),
	PINCTRL_PIN(7, "SD3_WP"),
	/* GPP_B */
	PINCTRL_PIN(8, "CORE_VID_0"),
	PINCTRL_PIN(9, "CORE_VID_1"),
	PINCTRL_PIN(10, "VRALERTB"),
	PINCTRL_PIN(11, "CPU_GP_2"),
	PINCTRL_PIN(12, "CPU_GP_3"),
	PINCTRL_PIN(13, "ISH_I2C0_SDA"),
	PINCTRL_PIN(14, "ISH_I2C0_SCL"),
	PINCTRL_PIN(15, "ISH_I2C1_SDA"),
	PINCTRL_PIN(16, "ISH_I2C1_SCL"),
	PINCTRL_PIN(17, "I2C5_SDA"),
	PINCTRL_PIN(18, "I2C5_SCL"),
	PINCTRL_PIN(19, "PMCALERTB"),
	PINCTRL_PIN(20, "SLP_S0B"),
	PINCTRL_PIN(21, "PLTRSTB"),
	PINCTRL_PIN(22, "SPKR"),
	PINCTRL_PIN(23, "GSPI0_CS0B"),
	PINCTRL_PIN(24, "GSPI0_CLK"),
	PINCTRL_PIN(25, "GSPI0_MISO"),
	PINCTRL_PIN(26, "GSPI0_MOSI"),
	PINCTRL_PIN(27, "GSPI1_CS0B"),
	PINCTRL_PIN(28, "GSPI1_CLK"),
	PINCTRL_PIN(29, "GSPI1_MISO"),
	PINCTRL_PIN(30, "GSPI1_MOSI"),
	PINCTRL_PIN(31, "SML1ALERTB"),
	PINCTRL_PIN(32, "GSPI0_CLK_LOOPBK"),
	PINCTRL_PIN(33, "GSPI1_CLK_LOOPBK"),
	/* GPP_A */
	PINCTRL_PIN(34, "ESPI_IO_0"),
	PINCTRL_PIN(35, "ESPI_IO_1"),
	PINCTRL_PIN(36, "ESPI_IO_2"),
	PINCTRL_PIN(37, "ESPI_IO_3"),
	PINCTRL_PIN(38, "ESPI_CSB"),
	PINCTRL_PIN(39, "ESPI_CLK"),
	PINCTRL_PIN(40, "ESPI_RESETB"),
	PINCTRL_PIN(41, "I2S2_SCLK"),
	PINCTRL_PIN(42, "I2S2_SFRM"),
	PINCTRL_PIN(43, "I2S2_TXD"),
	PINCTRL_PIN(44, "I2S2_RXD"),
	PINCTRL_PIN(45, "SATA_DEVSLP_2"),
	PINCTRL_PIN(46, "SATAXPCIE_1"),
	PINCTRL_PIN(47, "SATAXPCIE_2"),
	PINCTRL_PIN(48, "USB2_OCB_1"),
	PINCTRL_PIN(49, "USB2_OCB_2"),
	PINCTRL_PIN(50, "USB2_OCB_3"),
	PINCTRL_PIN(51, "DDSP_HPD_C"),
	PINCTRL_PIN(52, "DDSP_HPD_B"),
	PINCTRL_PIN(53, "DDSP_HPD_1"),
	PINCTRL_PIN(54, "DDSP_HPD_2"),
	PINCTRL_PIN(55, "I2S5_TXD"),
	PINCTRL_PIN(56, "I2S5_RXD"),
	PINCTRL_PIN(57, "I2S1_SCLK"),
	PINCTRL_PIN(58, "ESPI_CLK_LOOPBK"),
	/* GPP_H */
	PINCTRL_PIN(59, "SD_1P8_SEL"),
	PINCTRL_PIN(60, "SD_PWR_EN_B"),
	PINCTRL_PIN(61, "GPPC_H_2"),
	PINCTRL_PIN(62, "SX_EXIT_HOLDOFFB"),
	PINCTRL_PIN(63, "I2C2_SDA"),
	PINCTRL_PIN(64, "I2C2_SCL"),
	PINCTRL_PIN(65, "I2C3_SDA"),
	PINCTRL_PIN(66, "I2C3_SCL"),
	PINCTRL_PIN(67, "I2C4_SDA"),
	PINCTRL_PIN(68, "I2C4_SCL"),
	PINCTRL_PIN(69, "SRCCLKREQB_4"),
	PINCTRL_PIN(70, "SRCCLKREQB_5"),
	PINCTRL_PIN(71, "M2_SKT2_CFG_0"),
	PINCTRL_PIN(72, "M2_SKT2_CFG_1"),
	PINCTRL_PIN(73, "M2_SKT2_CFG_2"),
	PINCTRL_PIN(74, "M2_SKT2_CFG_3"),
	PINCTRL_PIN(75, "DDPB_CTRLCLK"),
	PINCTRL_PIN(76, "DDPB_CTRLDATA"),
	PINCTRL_PIN(77, "CPU_VCCIO_PWR_GATEB"),
	PINCTRL_PIN(78, "TIME_SYNC_0"),
	PINCTRL_PIN(79, "IMGCLKOUT_1"),
	PINCTRL_PIN(80, "IMGCLKOUT_2"),
	PINCTRL_PIN(81, "IMGCLKOUT_3"),
	PINCTRL_PIN(82, "IMGCLKOUT_4"),
	/* GPP_D */
	PINCTRL_PIN(83, "ISH_GP_0"),
	PINCTRL_PIN(84, "ISH_GP_1"),
	PINCTRL_PIN(85, "ISH_GP_2"),
	PINCTRL_PIN(86, "ISH_GP_3"),
	PINCTRL_PIN(87, "IMGCLKOUT_0"),
	PINCTRL_PIN(88, "SRCCLKREQB_0"),
	PINCTRL_PIN(89, "SRCCLKREQB_1"),
	PINCTRL_PIN(90, "SRCCLKREQB_2"),
	PINCTRL_PIN(91, "SRCCLKREQB_3"),
	PINCTRL_PIN(92, "ISH_SPI_CSB"),
	PINCTRL_PIN(93, "ISH_SPI_CLK"),
	PINCTRL_PIN(94, "ISH_SPI_MISO"),
	PINCTRL_PIN(95, "ISH_SPI_MOSI"),
	PINCTRL_PIN(96, "ISH_UART0_RXD"),
	PINCTRL_PIN(97, "ISH_UART0_TXD"),
	PINCTRL_PIN(98, "ISH_UART0_RTSB"),
	PINCTRL_PIN(99, "ISH_UART0_CTSB"),
	PINCTRL_PIN(100, "ISH_GP_4"),
	PINCTRL_PIN(101, "ISH_GP_5"),
	PINCTRL_PIN(102, "I2S_MCLK"),
	PINCTRL_PIN(103, "GSPI2_CLK_LOOPBK"),
	/* GPP_F */
	PINCTRL_PIN(104, "CNV_BRI_DT"),
	PINCTRL_PIN(105, "CNV_BRI_RSP"),
	PINCTRL_PIN(106, "CNV_RGI_DT"),
	PINCTRL_PIN(107, "CNV_RGI_RSP"),
	PINCTRL_PIN(108, "CNV_RF_RESET_B"),
	PINCTRL_PIN(109, "EMMC_HIP_MON"),
	PINCTRL_PIN(110, "CNV_PA_BLANKING"),
	PINCTRL_PIN(111, "EMMC_CMD"),
	PINCTRL_PIN(112, "EMMC_DATA0"),
	PINCTRL_PIN(113, "EMMC_DATA1"),
	PINCTRL_PIN(114, "EMMC_DATA2"),
	PINCTRL_PIN(115, "EMMC_DATA3"),
	PINCTRL_PIN(116, "EMMC_DATA4"),
	PINCTRL_PIN(117, "EMMC_DATA5"),
	PINCTRL_PIN(118, "EMMC_DATA6"),
	PINCTRL_PIN(119, "EMMC_DATA7"),
	PINCTRL_PIN(120, "EMMC_RCLK"),
	PINCTRL_PIN(121, "EMMC_CLK"),
	PINCTRL_PIN(122, "EMMC_RESETB"),
	PINCTRL_PIN(123, "A4WP_PRESENT"),
	/* vGPIO */
	PINCTRL_PIN(124, "CNV_BTEN"),
	PINCTRL_PIN(125, "CNV_WCEN"),
	PINCTRL_PIN(126, "CNV_BT_HOST_WAKEB"),
	PINCTRL_PIN(127, "CNV_BT_IF_SELECT"),
	PINCTRL_PIN(128, "vCNV_BT_UART_TXD"),
	PINCTRL_PIN(129, "vCNV_BT_UART_RXD"),
	PINCTRL_PIN(130, "vCNV_BT_UART_CTS_B"),
	PINCTRL_PIN(131, "vCNV_BT_UART_RTS_B"),
	PINCTRL_PIN(132, "vCNV_MFUART1_TXD"),
	PINCTRL_PIN(133, "vCNV_MFUART1_RXD"),
	PINCTRL_PIN(134, "vCNV_MFUART1_CTS_B"),
	PINCTRL_PIN(135, "vCNV_MFUART1_RTS_B"),
	PINCTRL_PIN(136, "vUART0_TXD"),
	PINCTRL_PIN(137, "vUART0_RXD"),
	PINCTRL_PIN(138, "vUART0_CTS_B"),
	PINCTRL_PIN(139, "vUART0_RTS_B"),
	PINCTRL_PIN(140, "vISH_UART0_TXD"),
	PINCTRL_PIN(141, "vISH_UART0_RXD"),
	PINCTRL_PIN(142, "vISH_UART0_CTS_B"),
	PINCTRL_PIN(143, "vISH_UART0_RTS_B"),
	PINCTRL_PIN(144, "vCNV_BT_I2S_BCLK"),
	PINCTRL_PIN(145, "vCNV_BT_I2S_WS_SYNC"),
	PINCTRL_PIN(146, "vCNV_BT_I2S_SDO"),
	PINCTRL_PIN(147, "vCNV_BT_I2S_SDI"),
	PINCTRL_PIN(148, "vI2S2_SCLK"),
	PINCTRL_PIN(149, "vI2S2_SFRM"),
	PINCTRL_PIN(150, "vI2S2_TXD"),
	PINCTRL_PIN(151, "vI2S2_RXD"),
	PINCTRL_PIN(152, "vSD3_CD_B"),
	/* GPP_C */
	PINCTRL_PIN(153, "SMBCLK"),
	PINCTRL_PIN(154, "SMBDATA"),
	PINCTRL_PIN(155, "SMBALERTB"),
	PINCTRL_PIN(156, "SML0CLK"),
	PINCTRL_PIN(157, "SML0DATA"),
	PINCTRL_PIN(158, "SML0ALERTB"),
	PINCTRL_PIN(159, "SML1CLK"),
	PINCTRL_PIN(160, "SML1DATA"),
	PINCTRL_PIN(161, "UART0_RXD"),
	PINCTRL_PIN(162, "UART0_TXD"),
	PINCTRL_PIN(163, "UART0_RTSB"),
	PINCTRL_PIN(164, "UART0_CTSB"),
	PINCTRL_PIN(165, "UART1_RXD"),
	PINCTRL_PIN(166, "UART1_TXD"),
	PINCTRL_PIN(167, "UART1_RTSB"),
	PINCTRL_PIN(168, "UART1_CTSB"),
	PINCTRL_PIN(169, "I2C0_SDA"),
	PINCTRL_PIN(170, "I2C0_SCL"),
	PINCTRL_PIN(171, "I2C1_SDA"),
	PINCTRL_PIN(172, "I2C1_SCL"),
	PINCTRL_PIN(173, "UART2_RXD"),
	PINCTRL_PIN(174, "UART2_TXD"),
	PINCTRL_PIN(175, "UART2_RTSB"),
	PINCTRL_PIN(176, "UART2_CTSB"),
	/* HVCMOS */
	PINCTRL_PIN(177, "L_BKLTEN"),
	PINCTRL_PIN(178, "L_BKLTCTL"),
	PINCTRL_PIN(179, "L_VDDEN"),
	PINCTRL_PIN(180, "SYS_PWROK"),
	PINCTRL_PIN(181, "SYS_RESETB"),
	PINCTRL_PIN(182, "MLK_RSTB"),
	/* GPP_E */
	PINCTRL_PIN(183, "SATAXPCIE_0"),
	PINCTRL_PIN(184, "SPI1_IO_2"),
	PINCTRL_PIN(185, "SPI1_IO_3"),
	PINCTRL_PIN(186, "CPU_GP_0"),
	PINCTRL_PIN(187, "SATA_DEVSLP_0"),
	PINCTRL_PIN(188, "SATA_DEVSLP_1"),
	PINCTRL_PIN(189, "GPPC_E_6"),
	PINCTRL_PIN(190, "CPU_GP_1"),
	PINCTRL_PIN(191, "SATA_LEDB"),
	PINCTRL_PIN(192, "USB2_OCB_0"),
	PINCTRL_PIN(193, "SPI1_CSB"),
	PINCTRL_PIN(194, "SPI1_CLK"),
	PINCTRL_PIN(195, "SPI1_MISO_IO_1"),
	PINCTRL_PIN(196, "SPI1_MOSI_IO_0"),
	PINCTRL_PIN(197, "DDSP_HPD_A"),
	PINCTRL_PIN(198, "ISH_GP_6"),
	PINCTRL_PIN(199, "ISH_GP_7"),
	PINCTRL_PIN(200, "DISP_MISC_4"),
	PINCTRL_PIN(201, "DDP1_CTRLCLK"),
	PINCTRL_PIN(202, "DDP1_CTRLDATA"),
	PINCTRL_PIN(203, "DDP2_CTRLCLK"),
	PINCTRL_PIN(204, "DDP2_CTRLDATA"),
	PINCTRL_PIN(205, "DDPA_CTRLCLK"),
	PINCTRL_PIN(206, "DDPA_CTRLDATA"),
	/* JTAG */
	PINCTRL_PIN(207, "JTAG_TDO"),
	PINCTRL_PIN(208, "JTAGX"),
	PINCTRL_PIN(209, "PRDYB"),
	PINCTRL_PIN(210, "PREQB"),
	PINCTRL_PIN(211, "CPU_TRSTB"),
	PINCTRL_PIN(212, "JTAG_TDI"),
	PINCTRL_PIN(213, "JTAG_TMS"),
	PINCTRL_PIN(214, "JTAG_TCK"),
	PINCTRL_PIN(215, "ITP_PMODE"),
	/* GPP_R */
	PINCTRL_PIN(216, "HDA_BCLK"),
	PINCTRL_PIN(217, "HDA_SYNC"),
	PINCTRL_PIN(218, "HDA_SDO"),
	PINCTRL_PIN(219, "HDA_SDI_0"),
	PINCTRL_PIN(220, "HDA_RSTB"),
	PINCTRL_PIN(221, "HDA_SDI_1"),
	PINCTRL_PIN(222, "I2S1_TXD"),
	PINCTRL_PIN(223, "I2S1_RXD"),
	/* GPP_S */
	PINCTRL_PIN(224, "SNDW1_CLK"),
	PINCTRL_PIN(225, "SNDW1_DATA"),
	PINCTRL_PIN(226, "SNDW2_CLK"),
	PINCTRL_PIN(227, "SNDW2_DATA"),
	PINCTRL_PIN(228, "SNDW3_CLK"),
	PINCTRL_PIN(229, "SNDW3_DATA"),
	PINCTRL_PIN(230, "SNDW4_CLK"),
	PINCTRL_PIN(231, "SNDW4_DATA"),
	/* SPI */
	PINCTRL_PIN(232, "SPI0_IO_2"),
	PINCTRL_PIN(233, "SPI0_IO_3"),
	PINCTRL_PIN(234, "SPI0_MOSI_IO_0"),
	PINCTRL_PIN(235, "SPI0_MISO_IO_1"),
	PINCTRL_PIN(236, "SPI0_TPM_CSB"),
	PINCTRL_PIN(237, "SPI0_FLASH_0_CSB"),
	PINCTRL_PIN(238, "SPI0_FLASH_1_CSB"),
	PINCTRL_PIN(239, "SPI0_CLK"),
	PINCTRL_PIN(240, "SPI0_CLK_LOOPBK"),
};

static const struct intel_padgroup icllp_community0_gpps[] = {
	ICL_GPP(0, 0, 7, 0),				/* GPP_G */
	ICL_GPP(1, 8, 33, 32),				/* GPP_B */
	ICL_GPP(2, 34, 58, 64),				/* GPP_A */
};

static const struct intel_padgroup icllp_community1_gpps[] = {
	ICL_GPP(0, 59, 82, 96),				/* GPP_H */
	ICL_GPP(1, 83, 103, 128),			/* GPP_D */
	ICL_GPP(2, 104, 123, 160),			/* GPP_F */
	ICL_GPP(3, 124, 152, 192),			/* vGPIO */
};

static const struct intel_padgroup icllp_community4_gpps[] = {
	ICL_GPP(0, 153, 176, 224),			/* GPP_C */
	ICL_GPP(1, 177, 182, INTEL_GPIO_BASE_NOMAP),	/* HVCMOS */
	ICL_GPP(2, 183, 206, 256),			/* GPP_E */
	ICL_GPP(3, 207, 215, INTEL_GPIO_BASE_NOMAP),	/* JTAG */
};

static const struct intel_padgroup icllp_community5_gpps[] = {
	ICL_GPP(0, 216, 223, 288),			/* GPP_R */
	ICL_GPP(1, 224, 231, 320),			/* GPP_S */
	ICL_GPP(2, 232, 240, INTEL_GPIO_BASE_NOMAP),	/* SPI */
};

static const struct intel_community icllp_communities[] = {
	ICL_COMMUNITY(0, 0, 58, icllp_community0_gpps),
	ICL_COMMUNITY(1, 59, 152, icllp_community1_gpps),
	ICL_COMMUNITY(2, 153, 215, icllp_community4_gpps),
	ICL_COMMUNITY(3, 216, 240, icllp_community5_gpps),
};

static const unsigned int icllp_spi0_pins[] = { 22, 23, 24, 25, 26 };
static const unsigned int icllp_spi0_modes[] = { 3, 1, 1, 1, 1 };
static const unsigned int icllp_spi1_pins[] = { 27, 28, 29, 30, 31 };
static const unsigned int icllp_spi1_modes[] = { 1, 1, 1, 1, 3 };
static const unsigned int icllp_spi2_pins[] = { 92, 93, 94, 95, 98 };
static const unsigned int icllp_spi2_modes[] = { 3, 3, 3, 3, 2 };

static const unsigned int icllp_i2c0_pins[] = { 169, 170 };
static const unsigned int icllp_i2c1_pins[] = { 171, 172 };
static const unsigned int icllp_i2c2_pins[] = { 63, 64 };
static const unsigned int icllp_i2c3_pins[] = { 65, 66 };
static const unsigned int icllp_i2c4_pins[] = { 67, 68 };

static const unsigned int icllp_uart0_pins[] = { 161, 162, 163, 164 };
static const unsigned int icllp_uart1_pins[] = { 165, 166, 167, 168 };
static const unsigned int icllp_uart2_pins[] = { 173, 174, 175, 176 };

static const struct intel_pingroup icllp_groups[] = {
	PIN_GROUP("spi0_grp", icllp_spi0_pins, icllp_spi0_modes),
	PIN_GROUP("spi1_grp", icllp_spi1_pins, icllp_spi1_modes),
	PIN_GROUP("spi2_grp", icllp_spi2_pins, icllp_spi2_modes),
	PIN_GROUP("i2c0_grp", icllp_i2c0_pins, 1),
	PIN_GROUP("i2c1_grp", icllp_i2c1_pins, 1),
	PIN_GROUP("i2c2_grp", icllp_i2c2_pins, 1),
	PIN_GROUP("i2c3_grp", icllp_i2c3_pins, 1),
	PIN_GROUP("i2c4_grp", icllp_i2c4_pins, 1),
	PIN_GROUP("uart0_grp", icllp_uart0_pins, 1),
	PIN_GROUP("uart1_grp", icllp_uart1_pins, 1),
	PIN_GROUP("uart2_grp", icllp_uart2_pins, 1),
};

static const char * const icllp_spi0_groups[] = { "spi0_grp" };
static const char * const icllp_spi1_groups[] = { "spi1_grp" };
static const char * const icllp_spi2_groups[] = { "spi2_grp" };
static const char * const icllp_i2c0_groups[] = { "i2c0_grp" };
static const char * const icllp_i2c1_groups[] = { "i2c1_grp" };
static const char * const icllp_i2c2_groups[] = { "i2c2_grp" };
static const char * const icllp_i2c3_groups[] = { "i2c3_grp" };
static const char * const icllp_i2c4_groups[] = { "i2c4_grp" };
static const char * const icllp_uart0_groups[] = { "uart0_grp" };
static const char * const icllp_uart1_groups[] = { "uart1_grp" };
static const char * const icllp_uart2_groups[] = { "uart2_grp" };

static const struct intel_function icllp_functions[] = {
	FUNCTION("spi0", icllp_spi0_groups),
	FUNCTION("spi1", icllp_spi1_groups),
	FUNCTION("spi2", icllp_spi2_groups),
	FUNCTION("i2c0", icllp_i2c0_groups),
	FUNCTION("i2c1", icllp_i2c1_groups),
	FUNCTION("i2c2", icllp_i2c2_groups),
	FUNCTION("i2c3", icllp_i2c3_groups),
	FUNCTION("i2c4", icllp_i2c4_groups),
	FUNCTION("uart0", icllp_uart0_groups),
	FUNCTION("uart1", icllp_uart1_groups),
	FUNCTION("uart2", icllp_uart2_groups),
};

static const struct intel_pinctrl_soc_data icllp_soc_data = {
	.pins = icllp_pins,
	.npins = ARRAY_SIZE(icllp_pins),
	.groups = icllp_groups,
	.ngroups = ARRAY_SIZE(icllp_groups),
	.functions = icllp_functions,
	.nfunctions = ARRAY_SIZE(icllp_functions),
	.communities = icllp_communities,
	.ncommunities = ARRAY_SIZE(icllp_communities),
};

static INTEL_PINCTRL_PM_OPS(icl_pinctrl_pm_ops);

static const struct acpi_device_id icl_pinctrl_acpi_match[] = {
	{ "INT3455", (kernel_ulong_t)&icllp_soc_data },
	{ }
};
MODULE_DEVICE_TABLE(acpi, icl_pinctrl_acpi_match);

static struct platform_driver icl_pinctrl_driver = {
	.probe = intel_pinctrl_probe_by_hid,
	.driver = {
		.name = "icelake-pinctrl",
		.acpi_match_table = icl_pinctrl_acpi_match,
		.pm = &icl_pinctrl_pm_ops,
	},
};

module_platform_driver(icl_pinctrl_driver);

MODULE_AUTHOR("Andy Shevchenko <andriy.shevchenko@linux.intel.com>");
MODULE_AUTHOR("Mika Westerberg <mika.westerberg@linux.intel.com>");
MODULE_DESCRIPTION("Intel Ice Lake PCH pinctrl/GPIO driver");
MODULE_LICENSE("GPL v2");

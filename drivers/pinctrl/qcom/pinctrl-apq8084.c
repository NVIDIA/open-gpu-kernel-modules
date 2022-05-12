// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-msm.h"

static const struct pinctrl_pin_desc apq8084_pins[] = {
	PINCTRL_PIN(0, "GPIO_0"),
	PINCTRL_PIN(1, "GPIO_1"),
	PINCTRL_PIN(2, "GPIO_2"),
	PINCTRL_PIN(3, "GPIO_3"),
	PINCTRL_PIN(4, "GPIO_4"),
	PINCTRL_PIN(5, "GPIO_5"),
	PINCTRL_PIN(6, "GPIO_6"),
	PINCTRL_PIN(7, "GPIO_7"),
	PINCTRL_PIN(8, "GPIO_8"),
	PINCTRL_PIN(9, "GPIO_9"),
	PINCTRL_PIN(10, "GPIO_10"),
	PINCTRL_PIN(11, "GPIO_11"),
	PINCTRL_PIN(12, "GPIO_12"),
	PINCTRL_PIN(13, "GPIO_13"),
	PINCTRL_PIN(14, "GPIO_14"),
	PINCTRL_PIN(15, "GPIO_15"),
	PINCTRL_PIN(16, "GPIO_16"),
	PINCTRL_PIN(17, "GPIO_17"),
	PINCTRL_PIN(18, "GPIO_18"),
	PINCTRL_PIN(19, "GPIO_19"),
	PINCTRL_PIN(20, "GPIO_20"),
	PINCTRL_PIN(21, "GPIO_21"),
	PINCTRL_PIN(22, "GPIO_22"),
	PINCTRL_PIN(23, "GPIO_23"),
	PINCTRL_PIN(24, "GPIO_24"),
	PINCTRL_PIN(25, "GPIO_25"),
	PINCTRL_PIN(26, "GPIO_26"),
	PINCTRL_PIN(27, "GPIO_27"),
	PINCTRL_PIN(28, "GPIO_28"),
	PINCTRL_PIN(29, "GPIO_29"),
	PINCTRL_PIN(30, "GPIO_30"),
	PINCTRL_PIN(31, "GPIO_31"),
	PINCTRL_PIN(32, "GPIO_32"),
	PINCTRL_PIN(33, "GPIO_33"),
	PINCTRL_PIN(34, "GPIO_34"),
	PINCTRL_PIN(35, "GPIO_35"),
	PINCTRL_PIN(36, "GPIO_36"),
	PINCTRL_PIN(37, "GPIO_37"),
	PINCTRL_PIN(38, "GPIO_38"),
	PINCTRL_PIN(39, "GPIO_39"),
	PINCTRL_PIN(40, "GPIO_40"),
	PINCTRL_PIN(41, "GPIO_41"),
	PINCTRL_PIN(42, "GPIO_42"),
	PINCTRL_PIN(43, "GPIO_43"),
	PINCTRL_PIN(44, "GPIO_44"),
	PINCTRL_PIN(45, "GPIO_45"),
	PINCTRL_PIN(46, "GPIO_46"),
	PINCTRL_PIN(47, "GPIO_47"),
	PINCTRL_PIN(48, "GPIO_48"),
	PINCTRL_PIN(49, "GPIO_49"),
	PINCTRL_PIN(50, "GPIO_50"),
	PINCTRL_PIN(51, "GPIO_51"),
	PINCTRL_PIN(52, "GPIO_52"),
	PINCTRL_PIN(53, "GPIO_53"),
	PINCTRL_PIN(54, "GPIO_54"),
	PINCTRL_PIN(55, "GPIO_55"),
	PINCTRL_PIN(56, "GPIO_56"),
	PINCTRL_PIN(57, "GPIO_57"),
	PINCTRL_PIN(58, "GPIO_58"),
	PINCTRL_PIN(59, "GPIO_59"),
	PINCTRL_PIN(60, "GPIO_60"),
	PINCTRL_PIN(61, "GPIO_61"),
	PINCTRL_PIN(62, "GPIO_62"),
	PINCTRL_PIN(63, "GPIO_63"),
	PINCTRL_PIN(64, "GPIO_64"),
	PINCTRL_PIN(65, "GPIO_65"),
	PINCTRL_PIN(66, "GPIO_66"),
	PINCTRL_PIN(67, "GPIO_67"),
	PINCTRL_PIN(68, "GPIO_68"),
	PINCTRL_PIN(69, "GPIO_69"),
	PINCTRL_PIN(70, "GPIO_70"),
	PINCTRL_PIN(71, "GPIO_71"),
	PINCTRL_PIN(72, "GPIO_72"),
	PINCTRL_PIN(73, "GPIO_73"),
	PINCTRL_PIN(74, "GPIO_74"),
	PINCTRL_PIN(75, "GPIO_75"),
	PINCTRL_PIN(76, "GPIO_76"),
	PINCTRL_PIN(77, "GPIO_77"),
	PINCTRL_PIN(78, "GPIO_78"),
	PINCTRL_PIN(79, "GPIO_79"),
	PINCTRL_PIN(80, "GPIO_80"),
	PINCTRL_PIN(81, "GPIO_81"),
	PINCTRL_PIN(82, "GPIO_82"),
	PINCTRL_PIN(83, "GPIO_83"),
	PINCTRL_PIN(84, "GPIO_84"),
	PINCTRL_PIN(85, "GPIO_85"),
	PINCTRL_PIN(86, "GPIO_86"),
	PINCTRL_PIN(87, "GPIO_87"),
	PINCTRL_PIN(88, "GPIO_88"),
	PINCTRL_PIN(89, "GPIO_89"),
	PINCTRL_PIN(90, "GPIO_90"),
	PINCTRL_PIN(91, "GPIO_91"),
	PINCTRL_PIN(92, "GPIO_92"),
	PINCTRL_PIN(93, "GPIO_93"),
	PINCTRL_PIN(94, "GPIO_94"),
	PINCTRL_PIN(95, "GPIO_95"),
	PINCTRL_PIN(96, "GPIO_96"),
	PINCTRL_PIN(97, "GPIO_97"),
	PINCTRL_PIN(98, "GPIO_98"),
	PINCTRL_PIN(99, "GPIO_99"),
	PINCTRL_PIN(100, "GPIO_100"),
	PINCTRL_PIN(101, "GPIO_101"),
	PINCTRL_PIN(102, "GPIO_102"),
	PINCTRL_PIN(103, "GPIO_103"),
	PINCTRL_PIN(104, "GPIO_104"),
	PINCTRL_PIN(105, "GPIO_105"),
	PINCTRL_PIN(106, "GPIO_106"),
	PINCTRL_PIN(107, "GPIO_107"),
	PINCTRL_PIN(108, "GPIO_108"),
	PINCTRL_PIN(109, "GPIO_109"),
	PINCTRL_PIN(110, "GPIO_110"),
	PINCTRL_PIN(111, "GPIO_111"),
	PINCTRL_PIN(112, "GPIO_112"),
	PINCTRL_PIN(113, "GPIO_113"),
	PINCTRL_PIN(114, "GPIO_114"),
	PINCTRL_PIN(115, "GPIO_115"),
	PINCTRL_PIN(116, "GPIO_116"),
	PINCTRL_PIN(117, "GPIO_117"),
	PINCTRL_PIN(118, "GPIO_118"),
	PINCTRL_PIN(119, "GPIO_119"),
	PINCTRL_PIN(120, "GPIO_120"),
	PINCTRL_PIN(121, "GPIO_121"),
	PINCTRL_PIN(122, "GPIO_122"),
	PINCTRL_PIN(123, "GPIO_123"),
	PINCTRL_PIN(124, "GPIO_124"),
	PINCTRL_PIN(125, "GPIO_125"),
	PINCTRL_PIN(126, "GPIO_126"),
	PINCTRL_PIN(127, "GPIO_127"),
	PINCTRL_PIN(128, "GPIO_128"),
	PINCTRL_PIN(129, "GPIO_129"),
	PINCTRL_PIN(130, "GPIO_130"),
	PINCTRL_PIN(131, "GPIO_131"),
	PINCTRL_PIN(132, "GPIO_132"),
	PINCTRL_PIN(133, "GPIO_133"),
	PINCTRL_PIN(134, "GPIO_134"),
	PINCTRL_PIN(135, "GPIO_135"),
	PINCTRL_PIN(136, "GPIO_136"),
	PINCTRL_PIN(137, "GPIO_137"),
	PINCTRL_PIN(138, "GPIO_138"),
	PINCTRL_PIN(139, "GPIO_139"),
	PINCTRL_PIN(140, "GPIO_140"),
	PINCTRL_PIN(141, "GPIO_141"),
	PINCTRL_PIN(142, "GPIO_142"),
	PINCTRL_PIN(143, "GPIO_143"),
	PINCTRL_PIN(144, "GPIO_144"),
	PINCTRL_PIN(145, "GPIO_145"),
	PINCTRL_PIN(146, "GPIO_146"),

	PINCTRL_PIN(147, "SDC1_CLK"),
	PINCTRL_PIN(148, "SDC1_CMD"),
	PINCTRL_PIN(149, "SDC1_DATA"),
	PINCTRL_PIN(150, "SDC2_CLK"),
	PINCTRL_PIN(151, "SDC2_CMD"),
	PINCTRL_PIN(152, "SDC2_DATA"),
};

#define DECLARE_APQ_GPIO_PINS(pin) static const unsigned int gpio##pin##_pins[] = { pin }

DECLARE_APQ_GPIO_PINS(0);
DECLARE_APQ_GPIO_PINS(1);
DECLARE_APQ_GPIO_PINS(2);
DECLARE_APQ_GPIO_PINS(3);
DECLARE_APQ_GPIO_PINS(4);
DECLARE_APQ_GPIO_PINS(5);
DECLARE_APQ_GPIO_PINS(6);
DECLARE_APQ_GPIO_PINS(7);
DECLARE_APQ_GPIO_PINS(8);
DECLARE_APQ_GPIO_PINS(9);
DECLARE_APQ_GPIO_PINS(10);
DECLARE_APQ_GPIO_PINS(11);
DECLARE_APQ_GPIO_PINS(12);
DECLARE_APQ_GPIO_PINS(13);
DECLARE_APQ_GPIO_PINS(14);
DECLARE_APQ_GPIO_PINS(15);
DECLARE_APQ_GPIO_PINS(16);
DECLARE_APQ_GPIO_PINS(17);
DECLARE_APQ_GPIO_PINS(18);
DECLARE_APQ_GPIO_PINS(19);
DECLARE_APQ_GPIO_PINS(20);
DECLARE_APQ_GPIO_PINS(21);
DECLARE_APQ_GPIO_PINS(22);
DECLARE_APQ_GPIO_PINS(23);
DECLARE_APQ_GPIO_PINS(24);
DECLARE_APQ_GPIO_PINS(25);
DECLARE_APQ_GPIO_PINS(26);
DECLARE_APQ_GPIO_PINS(27);
DECLARE_APQ_GPIO_PINS(28);
DECLARE_APQ_GPIO_PINS(29);
DECLARE_APQ_GPIO_PINS(30);
DECLARE_APQ_GPIO_PINS(31);
DECLARE_APQ_GPIO_PINS(32);
DECLARE_APQ_GPIO_PINS(33);
DECLARE_APQ_GPIO_PINS(34);
DECLARE_APQ_GPIO_PINS(35);
DECLARE_APQ_GPIO_PINS(36);
DECLARE_APQ_GPIO_PINS(37);
DECLARE_APQ_GPIO_PINS(38);
DECLARE_APQ_GPIO_PINS(39);
DECLARE_APQ_GPIO_PINS(40);
DECLARE_APQ_GPIO_PINS(41);
DECLARE_APQ_GPIO_PINS(42);
DECLARE_APQ_GPIO_PINS(43);
DECLARE_APQ_GPIO_PINS(44);
DECLARE_APQ_GPIO_PINS(45);
DECLARE_APQ_GPIO_PINS(46);
DECLARE_APQ_GPIO_PINS(47);
DECLARE_APQ_GPIO_PINS(48);
DECLARE_APQ_GPIO_PINS(49);
DECLARE_APQ_GPIO_PINS(50);
DECLARE_APQ_GPIO_PINS(51);
DECLARE_APQ_GPIO_PINS(52);
DECLARE_APQ_GPIO_PINS(53);
DECLARE_APQ_GPIO_PINS(54);
DECLARE_APQ_GPIO_PINS(55);
DECLARE_APQ_GPIO_PINS(56);
DECLARE_APQ_GPIO_PINS(57);
DECLARE_APQ_GPIO_PINS(58);
DECLARE_APQ_GPIO_PINS(59);
DECLARE_APQ_GPIO_PINS(60);
DECLARE_APQ_GPIO_PINS(61);
DECLARE_APQ_GPIO_PINS(62);
DECLARE_APQ_GPIO_PINS(63);
DECLARE_APQ_GPIO_PINS(64);
DECLARE_APQ_GPIO_PINS(65);
DECLARE_APQ_GPIO_PINS(66);
DECLARE_APQ_GPIO_PINS(67);
DECLARE_APQ_GPIO_PINS(68);
DECLARE_APQ_GPIO_PINS(69);
DECLARE_APQ_GPIO_PINS(70);
DECLARE_APQ_GPIO_PINS(71);
DECLARE_APQ_GPIO_PINS(72);
DECLARE_APQ_GPIO_PINS(73);
DECLARE_APQ_GPIO_PINS(74);
DECLARE_APQ_GPIO_PINS(75);
DECLARE_APQ_GPIO_PINS(76);
DECLARE_APQ_GPIO_PINS(77);
DECLARE_APQ_GPIO_PINS(78);
DECLARE_APQ_GPIO_PINS(79);
DECLARE_APQ_GPIO_PINS(80);
DECLARE_APQ_GPIO_PINS(81);
DECLARE_APQ_GPIO_PINS(82);
DECLARE_APQ_GPIO_PINS(83);
DECLARE_APQ_GPIO_PINS(84);
DECLARE_APQ_GPIO_PINS(85);
DECLARE_APQ_GPIO_PINS(86);
DECLARE_APQ_GPIO_PINS(87);
DECLARE_APQ_GPIO_PINS(88);
DECLARE_APQ_GPIO_PINS(89);
DECLARE_APQ_GPIO_PINS(90);
DECLARE_APQ_GPIO_PINS(91);
DECLARE_APQ_GPIO_PINS(92);
DECLARE_APQ_GPIO_PINS(93);
DECLARE_APQ_GPIO_PINS(94);
DECLARE_APQ_GPIO_PINS(95);
DECLARE_APQ_GPIO_PINS(96);
DECLARE_APQ_GPIO_PINS(97);
DECLARE_APQ_GPIO_PINS(98);
DECLARE_APQ_GPIO_PINS(99);
DECLARE_APQ_GPIO_PINS(100);
DECLARE_APQ_GPIO_PINS(101);
DECLARE_APQ_GPIO_PINS(102);
DECLARE_APQ_GPIO_PINS(103);
DECLARE_APQ_GPIO_PINS(104);
DECLARE_APQ_GPIO_PINS(105);
DECLARE_APQ_GPIO_PINS(106);
DECLARE_APQ_GPIO_PINS(107);
DECLARE_APQ_GPIO_PINS(108);
DECLARE_APQ_GPIO_PINS(109);
DECLARE_APQ_GPIO_PINS(110);
DECLARE_APQ_GPIO_PINS(111);
DECLARE_APQ_GPIO_PINS(112);
DECLARE_APQ_GPIO_PINS(113);
DECLARE_APQ_GPIO_PINS(114);
DECLARE_APQ_GPIO_PINS(115);
DECLARE_APQ_GPIO_PINS(116);
DECLARE_APQ_GPIO_PINS(117);
DECLARE_APQ_GPIO_PINS(118);
DECLARE_APQ_GPIO_PINS(119);
DECLARE_APQ_GPIO_PINS(120);
DECLARE_APQ_GPIO_PINS(121);
DECLARE_APQ_GPIO_PINS(122);
DECLARE_APQ_GPIO_PINS(123);
DECLARE_APQ_GPIO_PINS(124);
DECLARE_APQ_GPIO_PINS(125);
DECLARE_APQ_GPIO_PINS(126);
DECLARE_APQ_GPIO_PINS(127);
DECLARE_APQ_GPIO_PINS(128);
DECLARE_APQ_GPIO_PINS(129);
DECLARE_APQ_GPIO_PINS(130);
DECLARE_APQ_GPIO_PINS(131);
DECLARE_APQ_GPIO_PINS(132);
DECLARE_APQ_GPIO_PINS(133);
DECLARE_APQ_GPIO_PINS(134);
DECLARE_APQ_GPIO_PINS(135);
DECLARE_APQ_GPIO_PINS(136);
DECLARE_APQ_GPIO_PINS(137);
DECLARE_APQ_GPIO_PINS(138);
DECLARE_APQ_GPIO_PINS(139);
DECLARE_APQ_GPIO_PINS(140);
DECLARE_APQ_GPIO_PINS(141);
DECLARE_APQ_GPIO_PINS(142);
DECLARE_APQ_GPIO_PINS(143);
DECLARE_APQ_GPIO_PINS(144);
DECLARE_APQ_GPIO_PINS(145);
DECLARE_APQ_GPIO_PINS(146);

static const unsigned int sdc1_clk_pins[] = { 147 };
static const unsigned int sdc1_cmd_pins[] = { 148 };
static const unsigned int sdc1_data_pins[] = { 149 };
static const unsigned int sdc2_clk_pins[] = { 150 };
static const unsigned int sdc2_cmd_pins[] = { 151 };
static const unsigned int sdc2_data_pins[] = { 152 };

#define FUNCTION(fname)					\
	[APQ_MUX_##fname] = {				\
		.name = #fname,				\
		.groups = fname##_groups,		\
		.ngroups = ARRAY_SIZE(fname##_groups),	\
	}

#define PINGROUP(id, f1, f2, f3, f4, f5, f6, f7)        \
	{						\
		.name = "gpio" #id,			\
		.pins = gpio##id##_pins,		\
		.npins = ARRAY_SIZE(gpio##id##_pins),	\
		.funcs = (int[]){			\
			APQ_MUX_gpio,			\
			APQ_MUX_##f1,			\
			APQ_MUX_##f2,			\
			APQ_MUX_##f3,			\
			APQ_MUX_##f4,			\
			APQ_MUX_##f5,			\
			APQ_MUX_##f6,			\
			APQ_MUX_##f7			\
		},					\
		.nfuncs = 8,				\
		.ctl_reg = 0x1000 + 0x10 * id,		\
		.io_reg = 0x1004 + 0x10 * id,		\
		.intr_cfg_reg = 0x1008 + 0x10 * id,	\
		.intr_status_reg = 0x100c + 0x10 * id,	\
		.intr_target_reg = 0x1008 + 0x10 * id,	\
		.mux_bit = 2,				\
		.pull_bit = 0,				\
		.drv_bit = 6,				\
		.oe_bit = 9,				\
		.in_bit = 0,				\
		.out_bit = 1,				\
		.intr_enable_bit = 0,			\
		.intr_status_bit = 0,			\
		.intr_ack_high = 0,			\
		.intr_target_bit = 5,			\
		.intr_target_kpss_val = 3,		\
		.intr_raw_status_bit = 4,		\
		.intr_polarity_bit = 1,			\
		.intr_detection_bit = 2,		\
		.intr_detection_width = 2,		\
	}

#define SDC_PINGROUP(pg_name, ctl, pull, drv)		\
	{						\
		.name = #pg_name,	                \
		.pins = pg_name##_pins,                 \
		.npins = ARRAY_SIZE(pg_name##_pins),    \
		.ctl_reg = ctl,                         \
		.io_reg = 0,                            \
		.intr_cfg_reg = 0,                      \
		.intr_status_reg = 0,                   \
		.intr_target_reg = 0,                   \
		.mux_bit = -1,                          \
		.pull_bit = pull,                       \
		.drv_bit = drv,                         \
		.oe_bit = -1,                           \
		.in_bit = -1,                           \
		.out_bit = -1,                          \
		.intr_enable_bit = -1,                  \
		.intr_status_bit = -1,                  \
		.intr_target_bit = -1,                  \
		.intr_target_kpss_val = -1,		\
		.intr_raw_status_bit = -1,              \
		.intr_polarity_bit = -1,                \
		.intr_detection_bit = -1,               \
		.intr_detection_width = -1,             \
	}

enum apq8084_functions {
	APQ_MUX_adsp_ext,
	APQ_MUX_audio_ref,
	APQ_MUX_blsp_i2c1,
	APQ_MUX_blsp_i2c2,
	APQ_MUX_blsp_i2c3,
	APQ_MUX_blsp_i2c4,
	APQ_MUX_blsp_i2c5,
	APQ_MUX_blsp_i2c6,
	APQ_MUX_blsp_i2c7,
	APQ_MUX_blsp_i2c8,
	APQ_MUX_blsp_i2c9,
	APQ_MUX_blsp_i2c10,
	APQ_MUX_blsp_i2c11,
	APQ_MUX_blsp_i2c12,
	APQ_MUX_blsp_spi1,
	APQ_MUX_blsp_spi1_cs1,
	APQ_MUX_blsp_spi1_cs2,
	APQ_MUX_blsp_spi1_cs3,
	APQ_MUX_blsp_spi2,
	APQ_MUX_blsp_spi3,
	APQ_MUX_blsp_spi3_cs1,
	APQ_MUX_blsp_spi3_cs2,
	APQ_MUX_blsp_spi3_cs3,
	APQ_MUX_blsp_spi4,
	APQ_MUX_blsp_spi5,
	APQ_MUX_blsp_spi6,
	APQ_MUX_blsp_spi7,
	APQ_MUX_blsp_spi8,
	APQ_MUX_blsp_spi9,
	APQ_MUX_blsp_spi10,
	APQ_MUX_blsp_spi10_cs1,
	APQ_MUX_blsp_spi10_cs2,
	APQ_MUX_blsp_spi10_cs3,
	APQ_MUX_blsp_spi11,
	APQ_MUX_blsp_spi12,
	APQ_MUX_blsp_uart1,
	APQ_MUX_blsp_uart2,
	APQ_MUX_blsp_uart3,
	APQ_MUX_blsp_uart4,
	APQ_MUX_blsp_uart5,
	APQ_MUX_blsp_uart6,
	APQ_MUX_blsp_uart7,
	APQ_MUX_blsp_uart8,
	APQ_MUX_blsp_uart9,
	APQ_MUX_blsp_uart10,
	APQ_MUX_blsp_uart11,
	APQ_MUX_blsp_uart12,
	APQ_MUX_blsp_uim1,
	APQ_MUX_blsp_uim2,
	APQ_MUX_blsp_uim3,
	APQ_MUX_blsp_uim4,
	APQ_MUX_blsp_uim5,
	APQ_MUX_blsp_uim6,
	APQ_MUX_blsp_uim7,
	APQ_MUX_blsp_uim8,
	APQ_MUX_blsp_uim9,
	APQ_MUX_blsp_uim10,
	APQ_MUX_blsp_uim11,
	APQ_MUX_blsp_uim12,
	APQ_MUX_cam_mclk0,
	APQ_MUX_cam_mclk1,
	APQ_MUX_cam_mclk2,
	APQ_MUX_cam_mclk3,
	APQ_MUX_cci_async,
	APQ_MUX_cci_async_in0,
	APQ_MUX_cci_i2c0,
	APQ_MUX_cci_i2c1,
	APQ_MUX_cci_timer0,
	APQ_MUX_cci_timer1,
	APQ_MUX_cci_timer2,
	APQ_MUX_cci_timer3,
	APQ_MUX_cci_timer4,
	APQ_MUX_edp_hpd,
	APQ_MUX_gcc_gp1,
	APQ_MUX_gcc_gp2,
	APQ_MUX_gcc_gp3,
	APQ_MUX_gcc_obt,
	APQ_MUX_gcc_vtt,
	APQ_MUX_gp_mn,
	APQ_MUX_gp_pdm0,
	APQ_MUX_gp_pdm1,
	APQ_MUX_gp_pdm2,
	APQ_MUX_gp0_clk,
	APQ_MUX_gp1_clk,
	APQ_MUX_gpio,
	APQ_MUX_hdmi_cec,
	APQ_MUX_hdmi_ddc,
	APQ_MUX_hdmi_dtest,
	APQ_MUX_hdmi_hpd,
	APQ_MUX_hdmi_rcv,
	APQ_MUX_hsic,
	APQ_MUX_ldo_en,
	APQ_MUX_ldo_update,
	APQ_MUX_mdp_vsync,
	APQ_MUX_pci_e0,
	APQ_MUX_pci_e0_n,
	APQ_MUX_pci_e0_rst,
	APQ_MUX_pci_e1,
	APQ_MUX_pci_e1_rst,
	APQ_MUX_pci_e1_rst_n,
	APQ_MUX_pci_e1_clkreq_n,
	APQ_MUX_pri_mi2s,
	APQ_MUX_qua_mi2s,
	APQ_MUX_sata_act,
	APQ_MUX_sata_devsleep,
	APQ_MUX_sata_devsleep_n,
	APQ_MUX_sd_write,
	APQ_MUX_sdc_emmc_mode,
	APQ_MUX_sdc3,
	APQ_MUX_sdc4,
	APQ_MUX_sec_mi2s,
	APQ_MUX_slimbus,
	APQ_MUX_spdif_tx,
	APQ_MUX_spkr_i2s,
	APQ_MUX_spkr_i2s_ws,
	APQ_MUX_spss_geni,
	APQ_MUX_ter_mi2s,
	APQ_MUX_tsif1,
	APQ_MUX_tsif2,
	APQ_MUX_uim,
	APQ_MUX_uim_batt_alarm,
	APQ_MUX_NA,
};

static const char * const gpio_groups[] = {
	"gpio0", "gpio1", "gpio2", "gpio3", "gpio4", "gpio5", "gpio6", "gpio7",
	"gpio8", "gpio9", "gpio10", "gpio11", "gpio12", "gpio13", "gpio14",
	"gpio15", "gpio16", "gpio17", "gpio18", "gpio19", "gpio20", "gpio21",
	"gpio22", "gpio23", "gpio24", "gpio25", "gpio26", "gpio27", "gpio28",
	"gpio29", "gpio30", "gpio31", "gpio32", "gpio33", "gpio34", "gpio35",
	"gpio36", "gpio37", "gpio38", "gpio39", "gpio40", "gpio41", "gpio42",
	"gpio43", "gpio44", "gpio45", "gpio46", "gpio47", "gpio48", "gpio49",
	"gpio50", "gpio51", "gpio52", "gpio53", "gpio54", "gpio55", "gpio56",
	"gpio57", "gpio58", "gpio59", "gpio60", "gpio61", "gpio62", "gpio63",
	"gpio64", "gpio65", "gpio66", "gpio67", "gpio68", "gpio69", "gpio70",
	"gpio71", "gpio72", "gpio73", "gpio74", "gpio75", "gpio76", "gpio77",
	"gpio78", "gpio79", "gpio80", "gpio81", "gpio82", "gpio83", "gpio84",
	"gpio85", "gpio86", "gpio87", "gpio88", "gpio89", "gpio90", "gpio91",
	"gpio92", "gpio93", "gpio94", "gpio95", "gpio96", "gpio97", "gpio98",
	"gpio99", "gpio100", "gpio101", "gpio102", "gpio103", "gpio104",
	"gpio105", "gpio106", "gpio107", "gpio108", "gpio109", "gpio110",
	"gpio111", "gpio112", "gpio113", "gpio114", "gpio115", "gpio116",
	"gpio117", "gpio118", "gpio119", "gpio120", "gpio121", "gpio122",
	"gpio123", "gpio124", "gpio125", "gpio126", "gpio127", "gpio128",
	"gpio129", "gpio130", "gpio131", "gpio132", "gpio133", "gpio134",
	"gpio135", "gpio136", "gpio137", "gpio138", "gpio139", "gpio140",
	"gpio141", "gpio142", "gpio143", "gpio144", "gpio145", "gpio146"
};

static const char * const adsp_ext_groups[] = {
	"gpio34"
};
static const char * const audio_ref_groups[] = {
	"gpio100"
};
static const char * const blsp_i2c1_groups[] = {
	"gpio2", "gpio3"
};
static const char * const blsp_i2c2_groups[] = {
	"gpio6", "gpio7"
};
static const char * const blsp_i2c3_groups[] = {
	"gpio10", "gpio11"
};
static const char * const blsp_i2c4_groups[] = {
	"gpio29", "gpio30"
};
static const char * const blsp_i2c5_groups[] = {
	"gpio41", "gpio42"
};
static const char * const blsp_i2c6_groups[] = {
	"gpio45", "gpio46"
};
static const char * const blsp_i2c7_groups[] = {
	"gpio132", "gpio133"
};
static const char * const blsp_i2c8_groups[] = {
	"gpio53", "gpio54"
};
static const char * const blsp_i2c9_groups[] = {
	"gpio57", "gpio58"
};
static const char * const blsp_i2c10_groups[] = {
	"gpio61", "gpio62"
};
static const char * const blsp_i2c11_groups[] = {
	"gpio65", "gpio66"
};
static const char * const blsp_i2c12_groups[] = {
	"gpio49", "gpio50"
};
static const char * const blsp_spi1_groups[] = {
	"gpio0", "gpio1", "gpio2", "gpio3"
};
static const char * const blsp_spi2_groups[] = {
	"gpio4", "gpio5", "gpio6", "gpio7"
};
static const char * const blsp_spi3_groups[] = {
	"gpio8", "gpio9", "gpio10", "gpio11"
};
static const char * const blsp_spi4_groups[] = {
	"gpio27", "gpio28", "gpio29", "gpio30"
};
static const char * const blsp_spi5_groups[] = {
	"gpio39", "gpio40", "gpio41", "gpio42"
};
static const char * const blsp_spi6_groups[] = {
	"gpio43", "gpio44", "gpio45", "gpio46"
};
static const char * const blsp_spi7_groups[] = {
	"gpio130", "gpio131", "gpio132", "gpio133"
};
static const char * const blsp_spi8_groups[] = {
	"gpio51", "gpio52", "gpio53", "gpio54"
};
static const char * const blsp_spi9_groups[] = {
	"gpio55", "gpio56", "gpio57", "gpio58"
};
static const char * const blsp_spi10_groups[] = {
	"gpio59", "gpio60", "gpio61", "gpio62"
};
static const char * const blsp_spi11_groups[] = {
	"gpio63", "gpio64", "gpio65", "gpio66"
};
static const char * const blsp_spi12_groups[] = {
	"gpio47", "gpio48", "gpio49", "gpio50"
};
static const char * const blsp_uart1_groups[] = {
	"gpio0", "gpio1", "gpio2", "gpio3"
};
static const char * const blsp_uart2_groups[] = {
	"gpio4", "gpio5", "gpio6", "gpio7"
};
static const char * const blsp_uart3_groups[] = {
	"gpio8"
};
static const char * const blsp_uart4_groups[] = {
	"gpio27", "gpio28", "gpio29", "gpio30"
};
static const char * const blsp_uart5_groups[] = {
	"gpio39", "gpio40", "gpio41", "gpio42"
};
static const char * const blsp_uart6_groups[] = {
	"gpio43", "gpio44", "gpio45", "gpio46"
};
static const char * const blsp_uart7_groups[] = {
	"gpio130", "gpio131", "gpio132", "gpio133"
};
static const char * const blsp_uart8_groups[] = {
	"gpio51", "gpio52", "gpio53", "gpio54"
};
static const char * const blsp_uart9_groups[] = {
	"gpio55", "gpio56", "gpio57", "gpio58"
};
static const char * const blsp_uart10_groups[] = {
	"gpio59", "gpio60", "gpio61", "gpio62"
};
static const char * const blsp_uart11_groups[] = {
	"gpio63", "gpio64", "gpio65", "gpio66"
};
static const char * const blsp_uart12_groups[] = {
	"gpio47", "gpio48", "gpio49", "gpio50"
};
static const char * const blsp_uim1_groups[] = {
	"gpio0", "gpio1"
};
static const char * const blsp_uim2_groups[] = {
	"gpio4", "gpio5"
};
static const char * const blsp_uim3_groups[] = {
	"gpio8", "gpio9"
};
static const char * const blsp_uim4_groups[] = {
	"gpio27", "gpio28"
};
static const char * const blsp_uim5_groups[] = {
	"gpio39", "gpio40"
};
static const char * const blsp_uim6_groups[] = {
	"gpio43", "gpio44"
};
static const char * const blsp_uim7_groups[] = {
	"gpio130", "gpio131"
};
static const char * const blsp_uim8_groups[] = {
	"gpio51", "gpio52"
};
static const char * const blsp_uim9_groups[] = {
	"gpio55", "gpio56"
};
static const char * const blsp_uim10_groups[] = {
	"gpio59", "gpio60"
};
static const char * const blsp_uim11_groups[] = {
	"gpio63", "gpio64"
};
static const char * const blsp_uim12_groups[] = {
	"gpio47", "gpio48"
};
static const char * const blsp_spi1_cs1_groups[] = {
	"gpio116"
};
static const char * const blsp_spi1_cs2_groups[] = {
	"gpio117"
};
static const char * const blsp_spi1_cs3_groups[] = {
	"gpio118"
};
static const char * const blsp_spi3_cs1_groups[] = {
	"gpio67"
};
static const char * const blsp_spi3_cs2_groups[] = {
	"gpio71"
};
static const char * const blsp_spi3_cs3_groups[] = {
	"gpio72"
};
static const char * const blsp_spi10_cs1_groups[] = {
	"gpio106"
};
static const char * const blsp_spi10_cs2_groups[] = {
	"gpio111"
};
static const char * const blsp_spi10_cs3_groups[] = {
	"gpio128"
};
static const char * const cam_mclk0_groups[] = {
	"gpio15"
};
static const char * const cam_mclk1_groups[] = {
	"gpio16"
};
static const char * const cam_mclk2_groups[] = {
	"gpio17"
};
static const char * const cam_mclk3_groups[] = {
	"gpio18"
};
static const char * const cci_async_groups[] = {
	"gpio26", "gpio119"
};
static const char * const cci_async_in0_groups[] = {
	"gpio120"
};
static const char * const cci_i2c0_groups[] = {
	"gpio19", "gpio20"
};
static const char * const cci_i2c1_groups[] = {
	"gpio21", "gpio22"
};
static const char * const cci_timer0_groups[] = {
	"gpio23"
};
static const char * const cci_timer1_groups[] = {
	"gpio24"
};
static const char * const cci_timer2_groups[] = {
	"gpio25"
};
static const char * const cci_timer3_groups[] = {
	"gpio26"
};
static const char * const cci_timer4_groups[] = {
	"gpio119"
};
static const char * const edp_hpd_groups[] = {
	"gpio103"
};
static const char * const gcc_gp1_groups[] = {
	"gpio37"
};
static const char * const gcc_gp2_groups[] = {
	"gpio38"
};
static const char * const gcc_gp3_groups[] = {
	"gpio86"
};
static const char * const gcc_obt_groups[] = {
	"gpio127"
};
static const char * const gcc_vtt_groups[] = {
	"gpio126"
};
static const char * const gp_mn_groups[] = {
	"gpio29"
};
static const char * const gp_pdm0_groups[] = {
	"gpio48", "gpio83"
};
static const char * const gp_pdm1_groups[] = {
	"gpio84", "gpio101"
};
static const char * const gp_pdm2_groups[] = {
	"gpio85", "gpio110"
};
static const char * const gp0_clk_groups[] = {
	"gpio25"
};
static const char * const gp1_clk_groups[] = {
	"gpio26"
};
static const char * const hdmi_cec_groups[] = {
	"gpio31"
};
static const char * const hdmi_ddc_groups[] = {
	"gpio32", "gpio33"
};
static const char * const hdmi_dtest_groups[] = {
	"gpio123"
};
static const char * const hdmi_hpd_groups[] = {
	"gpio34"
};
static const char * const hdmi_rcv_groups[] = {
	"gpio125"
};
static const char * const hsic_groups[] = {
	"gpio134", "gpio135"
};
static const char * const ldo_en_groups[] = {
	"gpio124"
};
static const char * const ldo_update_groups[] = {
	"gpio125"
};
static const char * const mdp_vsync_groups[] = {
	"gpio12", "gpio13", "gpio14"
};
static const char * const pci_e0_groups[] = {
	"gpio68", "gpio70"
};
static const char * const pci_e0_n_groups[] = {
	"gpio68", "gpio70"
};
static const char * const pci_e0_rst_groups[] = {
	"gpio70"
};
static const char * const pci_e1_groups[] = {
	"gpio140"
};
static const char * const pci_e1_rst_groups[] = {
	"gpio140"
};
static const char * const pci_e1_rst_n_groups[] = {
	"gpio140"
};
static const char * const pci_e1_clkreq_n_groups[] = {
	"gpio141"
};
static const char * const pri_mi2s_groups[] = {
	"gpio76", "gpio77", "gpio78", "gpio79", "gpio80"
};
static const char * const qua_mi2s_groups[] = {
	"gpio91", "gpio92", "gpio93", "gpio94", "gpio95", "gpio96", "gpio97"
};
static const char * const sata_act_groups[] = {
	"gpio129"
};
static const char * const sata_devsleep_groups[] = {
	"gpio119"
};
static const char * const sata_devsleep_n_groups[] = {
	"gpio119"
};
static const char * const sd_write_groups[] = {
	"gpio75"
};
static const char * const sdc_emmc_mode_groups[] = {
	"gpio146"
};
static const char * const sdc3_groups[] = {
	"gpio67", "gpio68", "gpio69", "gpio70", "gpio71", "gpio72"
};
static const char * const sdc4_groups[] = {
	"gpio82", "gpio83", "gpio84", "gpio85", "gpio86",
	"gpio91", "gpio95", "gpio96", "gpio97", "gpio101"
};
static const char * const sec_mi2s_groups[] = {
	"gpio81", "gpio82", "gpio83", "gpio84", "gpio85"
};
static const char * const slimbus_groups[] = {
	"gpio98", "gpio99"
};
static const char * const spdif_tx_groups[] = {
	"gpio124", "gpio136", "gpio142"
};
static const char * const spkr_i2s_groups[] = {
	"gpio98", "gpio99", "gpio100"
};
static const char * const spkr_i2s_ws_groups[] = {
	"gpio104"
};
static const char * const spss_geni_groups[] = {
	"gpio8", "gpio9"
};
static const char * const ter_mi2s_groups[] = {
	"gpio86", "gpio87", "gpio88", "gpio89", "gpio90"
};
static const char * const tsif1_groups[] = {
	"gpio82", "gpio83", "gpio84", "gpio85", "gpio86"
};
static const char * const tsif2_groups[] = {
	"gpio91", "gpio95", "gpio96", "gpio97", "gpio101"
};
static const char * const uim_groups[] = {
	"gpio130", "gpio131", "gpio132", "gpio133"
};
static const char * const uim_batt_alarm_groups[] = {
	"gpio102"
};
static const struct msm_function apq8084_functions[] = {
	FUNCTION(adsp_ext),
	FUNCTION(audio_ref),
	FUNCTION(blsp_i2c1),
	FUNCTION(blsp_i2c2),
	FUNCTION(blsp_i2c3),
	FUNCTION(blsp_i2c4),
	FUNCTION(blsp_i2c5),
	FUNCTION(blsp_i2c6),
	FUNCTION(blsp_i2c7),
	FUNCTION(blsp_i2c8),
	FUNCTION(blsp_i2c9),
	FUNCTION(blsp_i2c10),
	FUNCTION(blsp_i2c11),
	FUNCTION(blsp_i2c12),
	FUNCTION(blsp_spi1),
	FUNCTION(blsp_spi1_cs1),
	FUNCTION(blsp_spi1_cs2),
	FUNCTION(blsp_spi1_cs3),
	FUNCTION(blsp_spi2),
	FUNCTION(blsp_spi3),
	FUNCTION(blsp_spi3_cs1),
	FUNCTION(blsp_spi3_cs2),
	FUNCTION(blsp_spi3_cs3),
	FUNCTION(blsp_spi4),
	FUNCTION(blsp_spi5),
	FUNCTION(blsp_spi6),
	FUNCTION(blsp_spi7),
	FUNCTION(blsp_spi8),
	FUNCTION(blsp_spi9),
	FUNCTION(blsp_spi10),
	FUNCTION(blsp_spi10_cs1),
	FUNCTION(blsp_spi10_cs2),
	FUNCTION(blsp_spi10_cs3),
	FUNCTION(blsp_spi11),
	FUNCTION(blsp_spi12),
	FUNCTION(blsp_uart1),
	FUNCTION(blsp_uart2),
	FUNCTION(blsp_uart3),
	FUNCTION(blsp_uart4),
	FUNCTION(blsp_uart5),
	FUNCTION(blsp_uart6),
	FUNCTION(blsp_uart7),
	FUNCTION(blsp_uart8),
	FUNCTION(blsp_uart9),
	FUNCTION(blsp_uart10),
	FUNCTION(blsp_uart11),
	FUNCTION(blsp_uart12),
	FUNCTION(blsp_uim1),
	FUNCTION(blsp_uim2),
	FUNCTION(blsp_uim3),
	FUNCTION(blsp_uim4),
	FUNCTION(blsp_uim5),
	FUNCTION(blsp_uim6),
	FUNCTION(blsp_uim7),
	FUNCTION(blsp_uim8),
	FUNCTION(blsp_uim9),
	FUNCTION(blsp_uim10),
	FUNCTION(blsp_uim11),
	FUNCTION(blsp_uim12),
	FUNCTION(cam_mclk0),
	FUNCTION(cam_mclk1),
	FUNCTION(cam_mclk2),
	FUNCTION(cam_mclk3),
	FUNCTION(cci_async),
	FUNCTION(cci_async_in0),
	FUNCTION(cci_i2c0),
	FUNCTION(cci_i2c1),
	FUNCTION(cci_timer0),
	FUNCTION(cci_timer1),
	FUNCTION(cci_timer2),
	FUNCTION(cci_timer3),
	FUNCTION(cci_timer4),
	FUNCTION(edp_hpd),
	FUNCTION(gcc_gp1),
	FUNCTION(gcc_gp2),
	FUNCTION(gcc_gp3),
	FUNCTION(gcc_obt),
	FUNCTION(gcc_vtt),
	FUNCTION(gp_mn),
	FUNCTION(gp_pdm0),
	FUNCTION(gp_pdm1),
	FUNCTION(gp_pdm2),
	FUNCTION(gp0_clk),
	FUNCTION(gp1_clk),
	FUNCTION(gpio),
	FUNCTION(hdmi_cec),
	FUNCTION(hdmi_ddc),
	FUNCTION(hdmi_dtest),
	FUNCTION(hdmi_hpd),
	FUNCTION(hdmi_rcv),
	FUNCTION(hsic),
	FUNCTION(ldo_en),
	FUNCTION(ldo_update),
	FUNCTION(mdp_vsync),
	FUNCTION(pci_e0),
	FUNCTION(pci_e0_n),
	FUNCTION(pci_e0_rst),
	FUNCTION(pci_e1),
	FUNCTION(pci_e1_rst),
	FUNCTION(pci_e1_rst_n),
	FUNCTION(pci_e1_clkreq_n),
	FUNCTION(pri_mi2s),
	FUNCTION(qua_mi2s),
	FUNCTION(sata_act),
	FUNCTION(sata_devsleep),
	FUNCTION(sata_devsleep_n),
	FUNCTION(sd_write),
	FUNCTION(sdc_emmc_mode),
	FUNCTION(sdc3),
	FUNCTION(sdc4),
	FUNCTION(sec_mi2s),
	FUNCTION(slimbus),
	FUNCTION(spdif_tx),
	FUNCTION(spkr_i2s),
	FUNCTION(spkr_i2s_ws),
	FUNCTION(spss_geni),
	FUNCTION(ter_mi2s),
	FUNCTION(tsif1),
	FUNCTION(tsif2),
	FUNCTION(uim),
	FUNCTION(uim_batt_alarm),
};

static const struct msm_pingroup apq8084_groups[] = {
	PINGROUP(0,   blsp_spi1, blsp_uart1, blsp_uim1, NA, NA, NA, NA),
	PINGROUP(1,   blsp_spi1, blsp_uart1, blsp_uim1, NA, NA, NA, NA),
	PINGROUP(2,   blsp_spi1, blsp_uart1, blsp_i2c1, NA, NA, NA, NA),
	PINGROUP(3,   blsp_spi1, blsp_uart1, blsp_i2c1, NA, NA, NA, NA),
	PINGROUP(4,   blsp_spi2, blsp_uart2, blsp_uim2, NA, NA, NA, NA),
	PINGROUP(5,   blsp_spi2, blsp_uart2, blsp_uim2, NA, NA, NA, NA),
	PINGROUP(6,   blsp_spi2, blsp_uart2, blsp_i2c2, NA, NA, NA, NA),
	PINGROUP(7,   blsp_spi2, blsp_uart2, blsp_i2c2, NA, NA, NA, NA),
	PINGROUP(8,   blsp_spi3, blsp_uart3, blsp_uim3, spss_geni, NA, NA, NA),
	PINGROUP(9,   blsp_spi3, blsp_uim3, blsp_uart3, spss_geni, NA, NA, NA),
	PINGROUP(10,  blsp_spi3, blsp_uart3, blsp_i2c3, NA, NA, NA, NA),
	PINGROUP(11,  blsp_spi3, blsp_uart3, blsp_i2c3, NA, NA, NA, NA),
	PINGROUP(12,  mdp_vsync, NA, NA, NA, NA, NA, NA),
	PINGROUP(13,  mdp_vsync, NA, NA, NA, NA, NA, NA),
	PINGROUP(14,  mdp_vsync, NA, NA, NA, NA, NA, NA),
	PINGROUP(15,  cam_mclk0, NA, NA, NA, NA, NA, NA),
	PINGROUP(16,  cam_mclk1, NA, NA, NA, NA, NA, NA),
	PINGROUP(17,  cam_mclk2, NA, NA, NA, NA, NA, NA),
	PINGROUP(18,  cam_mclk3, NA, NA, NA, NA, NA, NA),
	PINGROUP(19,  cci_i2c0, NA, NA, NA, NA, NA, NA),
	PINGROUP(20,  cci_i2c0, NA, NA, NA, NA, NA, NA),
	PINGROUP(21,  cci_i2c1, NA, NA, NA, NA, NA, NA),
	PINGROUP(22,  cci_i2c1, NA, NA, NA, NA, NA, NA),
	PINGROUP(23,  cci_timer0, NA, NA, NA, NA, NA, NA),
	PINGROUP(24,  cci_timer1, NA, NA, NA, NA, NA, NA),
	PINGROUP(25,  cci_timer2, gp0_clk, NA, NA, NA, NA, NA),
	PINGROUP(26,  cci_timer3, cci_async, gp1_clk, NA, NA, NA, NA),
	PINGROUP(27,  blsp_spi4, blsp_uart4, blsp_uim4, NA, NA, NA, NA),
	PINGROUP(28,  blsp_spi4, blsp_uart4, blsp_uim4, NA, NA, NA, NA),
	PINGROUP(29,  blsp_spi4, blsp_uart4, blsp_i2c4, gp_mn, NA, NA, NA),
	PINGROUP(30,  blsp_spi4, blsp_uart4, blsp_i2c4, NA, NA, NA, NA),
	PINGROUP(31,  hdmi_cec, NA, NA, NA, NA, NA, NA),
	PINGROUP(32,  hdmi_ddc, NA, NA, NA, NA, NA, NA),
	PINGROUP(33,  hdmi_ddc, NA, NA, NA, NA, NA, NA),
	PINGROUP(34,  hdmi_hpd, NA, adsp_ext, NA, NA, NA, NA),
	PINGROUP(35,  NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(36,  NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(37,  gcc_gp1, NA, NA, NA, NA, NA, NA),
	PINGROUP(38,  gcc_gp2, NA, NA, NA, NA, NA, NA),
	PINGROUP(39,  blsp_spi5, blsp_uart5, blsp_uim5, NA, NA, NA, NA),
	PINGROUP(40,  blsp_spi5, blsp_uart5, blsp_uim5, NA, NA, NA, NA),
	PINGROUP(41,  blsp_spi5, blsp_uart5, blsp_i2c5, NA, NA, NA, NA),
	PINGROUP(42,  blsp_spi5, blsp_uart5, blsp_i2c5, NA, NA, NA, NA),
	PINGROUP(43,  blsp_spi6, blsp_uart6, blsp_uim6, NA, NA, NA, NA),
	PINGROUP(44,  blsp_spi6, blsp_uart6, blsp_uim6, NA, NA, NA, NA),
	PINGROUP(45,  blsp_spi6, blsp_uart6, blsp_i2c6, NA, NA, NA, NA),
	PINGROUP(46,  blsp_spi6, blsp_uart6, blsp_i2c6, NA, NA, NA, NA),
	PINGROUP(47,  blsp_spi12, blsp_uart12, blsp_uim12, NA, NA, NA, NA),
	PINGROUP(48,  blsp_spi12, blsp_uart12, blsp_uim12, gp_pdm0, NA, NA, NA),
	PINGROUP(49,  blsp_spi12, blsp_uart12, blsp_i2c12, NA, NA, NA, NA),
	PINGROUP(50,  blsp_spi12, blsp_uart12, blsp_i2c12, NA, NA, NA, NA),
	PINGROUP(51,  blsp_spi8, blsp_uart8, blsp_uim8, NA, NA, NA, NA),
	PINGROUP(52,  blsp_spi8, blsp_uart8, blsp_uim8, NA, NA, NA, NA),
	PINGROUP(53,  blsp_spi8, blsp_uart8, blsp_i2c8, NA, NA, NA, NA),
	PINGROUP(54,  blsp_spi8, blsp_uart8, blsp_i2c8, NA, NA, NA, NA),
	PINGROUP(55,  blsp_spi9, blsp_uart9, blsp_uim9, NA, NA, NA, NA),
	PINGROUP(56,  blsp_spi9, blsp_uart9, blsp_uim9, NA, NA, NA, NA),
	PINGROUP(57,  blsp_spi9, blsp_uart9, blsp_i2c9, NA, NA, NA, NA),
	PINGROUP(58,  blsp_spi9, blsp_uart9, blsp_i2c9, NA, NA, NA, NA),
	PINGROUP(59,  blsp_spi10, blsp_uart10, blsp_uim10, NA, NA, NA, NA),
	PINGROUP(60,  blsp_spi10, blsp_uart10, blsp_uim10, NA, NA, NA, NA),
	PINGROUP(61,  blsp_spi10, blsp_uart10, blsp_i2c10, NA, NA, NA, NA),
	PINGROUP(62,  blsp_spi10, blsp_uart10, blsp_i2c10, NA, NA, NA, NA),
	PINGROUP(63,  blsp_spi11, blsp_uart11, blsp_uim11, NA, NA, NA, NA),
	PINGROUP(64,  blsp_spi11, blsp_uart11, blsp_uim11, NA, NA, NA, NA),
	PINGROUP(65,  blsp_spi11, blsp_uart11, blsp_i2c11, NA, NA, NA, NA),
	PINGROUP(66,  blsp_spi11, blsp_uart11, blsp_i2c11, NA, NA, NA, NA),
	PINGROUP(67,  sdc3, blsp_spi3_cs1, NA, NA, NA, NA, NA),
	PINGROUP(68,  sdc3, pci_e0, NA, NA, NA, NA, NA),
	PINGROUP(69,  sdc3, NA, NA, NA, NA, NA, NA),
	PINGROUP(70,  sdc3, pci_e0_n, pci_e0, NA, NA, NA, NA),
	PINGROUP(71,  sdc3, blsp_spi3_cs2, NA, NA, NA, NA, NA),
	PINGROUP(72,  sdc3, blsp_spi3_cs3, NA, NA, NA, NA, NA),
	PINGROUP(73,  NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(74,  NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(75,  sd_write, NA, NA, NA, NA, NA, NA),
	PINGROUP(76,  pri_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(77,  pri_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(78,  pri_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(79,  pri_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(80,  pri_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(81,  sec_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(82,  sec_mi2s, sdc4, tsif1, NA, NA, NA, NA),
	PINGROUP(83,  sec_mi2s, sdc4, tsif1, NA, NA, NA, gp_pdm0),
	PINGROUP(84,  sec_mi2s, sdc4, tsif1, NA, NA, NA, gp_pdm1),
	PINGROUP(85,  sec_mi2s, sdc4, tsif1, NA, gp_pdm2, NA, NA),
	PINGROUP(86,  ter_mi2s, sdc4, tsif1, NA, NA, NA, gcc_gp3),
	PINGROUP(87,  ter_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(88,  ter_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(89,  ter_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(90,  ter_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(91,  qua_mi2s, sdc4, tsif2, NA, NA, NA, NA),
	PINGROUP(92,  qua_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(93,  qua_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(94,  qua_mi2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(95,  qua_mi2s, sdc4, tsif2, NA, NA, NA, gcc_gp1),
	PINGROUP(96,  qua_mi2s, sdc4, tsif2, NA, NA, NA, gcc_gp2),
	PINGROUP(97,  qua_mi2s, sdc4, tsif2, NA, gcc_gp3, NA, NA),
	PINGROUP(98,  slimbus, spkr_i2s, NA, NA, NA, NA, NA),
	PINGROUP(99,  slimbus, spkr_i2s, NA, NA, NA, NA, NA),
	PINGROUP(100, audio_ref, spkr_i2s, NA, NA, NA, NA, NA),
	PINGROUP(101, sdc4, tsif2, gp_pdm1, NA, NA, NA, NA),
	PINGROUP(102, uim_batt_alarm, NA, NA, NA, NA, NA, NA),
	PINGROUP(103, edp_hpd, NA, NA, NA, NA, NA, NA),
	PINGROUP(104, spkr_i2s, NA, NA, NA, NA, NA, NA),
	PINGROUP(105, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(106, blsp_spi10_cs1, NA, NA, NA, NA, NA, NA),
	PINGROUP(107, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(108, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(109, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(110, gp_pdm2, NA, NA, NA, NA, NA, NA),
	PINGROUP(111, blsp_spi10_cs2, NA, NA, NA, NA, NA, NA),
	PINGROUP(112, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(113, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(114, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(115, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(116, blsp_spi1_cs1, NA, NA, NA, NA, NA, NA),
	PINGROUP(117, blsp_spi1_cs2, NA, NA, NA, NA, NA, NA),
	PINGROUP(118, blsp_spi1_cs3, NA, NA, NA, NA, NA, NA),
	PINGROUP(119, cci_timer4, cci_async, sata_devsleep, sata_devsleep_n, NA, NA, NA),
	PINGROUP(120, cci_async, NA, NA, NA, NA, NA, NA),
	PINGROUP(121, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(122, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(123, hdmi_dtest, NA, NA, NA, NA, NA, NA),
	PINGROUP(124, spdif_tx, ldo_en, NA, NA, NA, NA, NA),
	PINGROUP(125, ldo_update, hdmi_rcv, NA, NA, NA, NA, NA),
	PINGROUP(126, gcc_vtt, NA, NA, NA, NA, NA, NA),
	PINGROUP(127, gcc_obt, NA, NA, NA, NA, NA, NA),
	PINGROUP(128, blsp_spi10_cs3, NA, NA, NA, NA, NA, NA),
	PINGROUP(129, sata_act, NA, NA, NA, NA, NA, NA),
	PINGROUP(130, uim, blsp_spi7, blsp_uart7, blsp_uim7, NA, NA, NA),
	PINGROUP(131, uim, blsp_spi7, blsp_uart7, blsp_uim7, NA, NA, NA),
	PINGROUP(132, uim, blsp_spi7, blsp_uart7, blsp_i2c7, NA, NA, NA),
	PINGROUP(133, uim, blsp_spi7, blsp_uart7, blsp_i2c7, NA, NA, NA),
	PINGROUP(134, hsic, NA, NA, NA, NA, NA, NA),
	PINGROUP(135, hsic, NA, NA, NA, NA, NA, NA),
	PINGROUP(136, spdif_tx, NA, NA, NA, NA, NA, NA),
	PINGROUP(137, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(138, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(139, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(140, pci_e1_rst_n, pci_e1_rst, NA, NA, NA, NA, NA),
	PINGROUP(141, pci_e1_clkreq_n, NA, NA, NA, NA, NA, NA),
	PINGROUP(142, spdif_tx, NA, NA, NA, NA, NA, NA),
	PINGROUP(143, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(144, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(145, NA, NA, NA, NA, NA, NA, NA),
	PINGROUP(146, sdc_emmc_mode, NA, NA, NA, NA, NA, NA),

	SDC_PINGROUP(sdc1_clk, 0x2044, 13, 6),
	SDC_PINGROUP(sdc1_cmd, 0x2044, 11, 3),
	SDC_PINGROUP(sdc1_data, 0x2044, 9, 0),
	SDC_PINGROUP(sdc2_clk, 0x2048, 14, 6),
	SDC_PINGROUP(sdc2_cmd, 0x2048, 11, 3),
	SDC_PINGROUP(sdc2_data, 0x2048, 9, 0),
};

#define NUM_GPIO_PINGROUPS 147

static const struct msm_pinctrl_soc_data apq8084_pinctrl = {
	.pins = apq8084_pins,
	.npins = ARRAY_SIZE(apq8084_pins),
	.functions = apq8084_functions,
	.nfunctions = ARRAY_SIZE(apq8084_functions),
	.groups = apq8084_groups,
	.ngroups = ARRAY_SIZE(apq8084_groups),
	.ngpios = NUM_GPIO_PINGROUPS,
};

static int apq8084_pinctrl_probe(struct platform_device *pdev)
{
	return msm_pinctrl_probe(pdev, &apq8084_pinctrl);
}

static const struct of_device_id apq8084_pinctrl_of_match[] = {
	{ .compatible = "qcom,apq8084-pinctrl", },
	{ },
};

static struct platform_driver apq8084_pinctrl_driver = {
	.driver = {
		.name = "apq8084-pinctrl",
		.of_match_table = apq8084_pinctrl_of_match,
	},
	.probe = apq8084_pinctrl_probe,
	.remove = msm_pinctrl_remove,
};

static int __init apq8084_pinctrl_init(void)
{
	return platform_driver_register(&apq8084_pinctrl_driver);
}
arch_initcall(apq8084_pinctrl_init);

static void __exit apq8084_pinctrl_exit(void)
{
	platform_driver_unregister(&apq8084_pinctrl_driver);
}
module_exit(apq8084_pinctrl_exit);

MODULE_DESCRIPTION("Qualcomm APQ8084 pinctrl driver");
MODULE_LICENSE("GPL v2");
MODULE_DEVICE_TABLE(of, apq8084_pinctrl_of_match);

// SPDX-License-Identifier: GPL-2.0-only
/*
 * Pin controller and GPIO driver for Amlogic Meson GXBB.
 *
 * Copyright (C) 2016 Endless Mobile, Inc.
 * Author: Carlo Caione <carlo@endlessm.com>
 */

#include <dt-bindings/gpio/meson-gxbb-gpio.h>
#include "pinctrl-meson.h"
#include "pinctrl-meson8-pmx.h"

static const struct pinctrl_pin_desc meson_gxbb_periphs_pins[] = {
	MESON_PIN(GPIOZ_0),
	MESON_PIN(GPIOZ_1),
	MESON_PIN(GPIOZ_2),
	MESON_PIN(GPIOZ_3),
	MESON_PIN(GPIOZ_4),
	MESON_PIN(GPIOZ_5),
	MESON_PIN(GPIOZ_6),
	MESON_PIN(GPIOZ_7),
	MESON_PIN(GPIOZ_8),
	MESON_PIN(GPIOZ_9),
	MESON_PIN(GPIOZ_10),
	MESON_PIN(GPIOZ_11),
	MESON_PIN(GPIOZ_12),
	MESON_PIN(GPIOZ_13),
	MESON_PIN(GPIOZ_14),
	MESON_PIN(GPIOZ_15),

	MESON_PIN(GPIOH_0),
	MESON_PIN(GPIOH_1),
	MESON_PIN(GPIOH_2),
	MESON_PIN(GPIOH_3),

	MESON_PIN(BOOT_0),
	MESON_PIN(BOOT_1),
	MESON_PIN(BOOT_2),
	MESON_PIN(BOOT_3),
	MESON_PIN(BOOT_4),
	MESON_PIN(BOOT_5),
	MESON_PIN(BOOT_6),
	MESON_PIN(BOOT_7),
	MESON_PIN(BOOT_8),
	MESON_PIN(BOOT_9),
	MESON_PIN(BOOT_10),
	MESON_PIN(BOOT_11),
	MESON_PIN(BOOT_12),
	MESON_PIN(BOOT_13),
	MESON_PIN(BOOT_14),
	MESON_PIN(BOOT_15),
	MESON_PIN(BOOT_16),
	MESON_PIN(BOOT_17),

	MESON_PIN(CARD_0),
	MESON_PIN(CARD_1),
	MESON_PIN(CARD_2),
	MESON_PIN(CARD_3),
	MESON_PIN(CARD_4),
	MESON_PIN(CARD_5),
	MESON_PIN(CARD_6),

	MESON_PIN(GPIODV_0),
	MESON_PIN(GPIODV_1),
	MESON_PIN(GPIODV_2),
	MESON_PIN(GPIODV_3),
	MESON_PIN(GPIODV_4),
	MESON_PIN(GPIODV_5),
	MESON_PIN(GPIODV_6),
	MESON_PIN(GPIODV_7),
	MESON_PIN(GPIODV_8),
	MESON_PIN(GPIODV_9),
	MESON_PIN(GPIODV_10),
	MESON_PIN(GPIODV_11),
	MESON_PIN(GPIODV_12),
	MESON_PIN(GPIODV_13),
	MESON_PIN(GPIODV_14),
	MESON_PIN(GPIODV_15),
	MESON_PIN(GPIODV_16),
	MESON_PIN(GPIODV_17),
	MESON_PIN(GPIODV_18),
	MESON_PIN(GPIODV_19),
	MESON_PIN(GPIODV_20),
	MESON_PIN(GPIODV_21),
	MESON_PIN(GPIODV_22),
	MESON_PIN(GPIODV_23),
	MESON_PIN(GPIODV_24),
	MESON_PIN(GPIODV_25),
	MESON_PIN(GPIODV_26),
	MESON_PIN(GPIODV_27),
	MESON_PIN(GPIODV_28),
	MESON_PIN(GPIODV_29),

	MESON_PIN(GPIOY_0),
	MESON_PIN(GPIOY_1),
	MESON_PIN(GPIOY_2),
	MESON_PIN(GPIOY_3),
	MESON_PIN(GPIOY_4),
	MESON_PIN(GPIOY_5),
	MESON_PIN(GPIOY_6),
	MESON_PIN(GPIOY_7),
	MESON_PIN(GPIOY_8),
	MESON_PIN(GPIOY_9),
	MESON_PIN(GPIOY_10),
	MESON_PIN(GPIOY_11),
	MESON_PIN(GPIOY_12),
	MESON_PIN(GPIOY_13),
	MESON_PIN(GPIOY_14),
	MESON_PIN(GPIOY_15),
	MESON_PIN(GPIOY_16),

	MESON_PIN(GPIOX_0),
	MESON_PIN(GPIOX_1),
	MESON_PIN(GPIOX_2),
	MESON_PIN(GPIOX_3),
	MESON_PIN(GPIOX_4),
	MESON_PIN(GPIOX_5),
	MESON_PIN(GPIOX_6),
	MESON_PIN(GPIOX_7),
	MESON_PIN(GPIOX_8),
	MESON_PIN(GPIOX_9),
	MESON_PIN(GPIOX_10),
	MESON_PIN(GPIOX_11),
	MESON_PIN(GPIOX_12),
	MESON_PIN(GPIOX_13),
	MESON_PIN(GPIOX_14),
	MESON_PIN(GPIOX_15),
	MESON_PIN(GPIOX_16),
	MESON_PIN(GPIOX_17),
	MESON_PIN(GPIOX_18),
	MESON_PIN(GPIOX_19),
	MESON_PIN(GPIOX_20),
	MESON_PIN(GPIOX_21),
	MESON_PIN(GPIOX_22),

	MESON_PIN(GPIOCLK_0),
	MESON_PIN(GPIOCLK_1),
	MESON_PIN(GPIOCLK_2),
	MESON_PIN(GPIOCLK_3),
};

static const unsigned int emmc_nand_d07_pins[] = {
	BOOT_0, BOOT_1, BOOT_2, BOOT_3, BOOT_4, BOOT_5, BOOT_6, BOOT_7,
};
static const unsigned int emmc_clk_pins[]	= { BOOT_8 };
static const unsigned int emmc_cmd_pins[]	= { BOOT_10 };
static const unsigned int emmc_ds_pins[]	= { BOOT_15 };

static const unsigned int nor_d_pins[]		= { BOOT_11 };
static const unsigned int nor_q_pins[]		= { BOOT_12 };
static const unsigned int nor_c_pins[]		= { BOOT_13 };
static const unsigned int nor_cs_pins[]		= { BOOT_15 };

static const unsigned int spi_sclk_pins[]	= { GPIOZ_6 };
static const unsigned int spi_ss0_pins[]	= { GPIOZ_7 };
static const unsigned int spi_miso_pins[]	= { GPIOZ_12 };
static const unsigned int spi_mosi_pins[]	= { GPIOZ_13 };

static const unsigned int sdcard_d0_pins[]	= { CARD_1 };
static const unsigned int sdcard_d1_pins[]	= { CARD_0 };
static const unsigned int sdcard_d2_pins[]	= { CARD_5 };
static const unsigned int sdcard_d3_pins[]	= { CARD_4 };
static const unsigned int sdcard_cmd_pins[]	= { CARD_3 };
static const unsigned int sdcard_clk_pins[]	= { CARD_2 };

static const unsigned int sdio_d0_pins[]	= { GPIOX_0 };
static const unsigned int sdio_d1_pins[]	= { GPIOX_1 };
static const unsigned int sdio_d2_pins[]	= { GPIOX_2 };
static const unsigned int sdio_d3_pins[]	= { GPIOX_3 };
static const unsigned int sdio_cmd_pins[]	= { GPIOX_4 };
static const unsigned int sdio_clk_pins[]	= { GPIOX_5 };
static const unsigned int sdio_irq_pins[]	= { GPIOX_7 };

static const unsigned int nand_ce0_pins[]	= { BOOT_8 };
static const unsigned int nand_ce1_pins[]	= { BOOT_9 };
static const unsigned int nand_rb0_pins[]	= { BOOT_10 };
static const unsigned int nand_ale_pins[]	= { BOOT_11 };
static const unsigned int nand_cle_pins[]	= { BOOT_12 };
static const unsigned int nand_wen_clk_pins[]	= { BOOT_13 };
static const unsigned int nand_ren_wr_pins[]	= { BOOT_14 };
static const unsigned int nand_dqs_pins[]	= { BOOT_15 };

static const unsigned int uart_tx_a_pins[]	= { GPIOX_12 };
static const unsigned int uart_rx_a_pins[]	= { GPIOX_13 };
static const unsigned int uart_cts_a_pins[]	= { GPIOX_14 };
static const unsigned int uart_rts_a_pins[]	= { GPIOX_15 };

static const unsigned int uart_tx_b_pins[]	= { GPIODV_24 };
static const unsigned int uart_rx_b_pins[]	= { GPIODV_25 };
static const unsigned int uart_cts_b_pins[]	= { GPIODV_26 };
static const unsigned int uart_rts_b_pins[]	= { GPIODV_27 };

static const unsigned int uart_tx_c_pins[]	= { GPIOY_13 };
static const unsigned int uart_rx_c_pins[]	= { GPIOY_14 };
static const unsigned int uart_cts_c_pins[]	= { GPIOY_11 };
static const unsigned int uart_rts_c_pins[]	= { GPIOY_12 };

static const unsigned int i2c_sck_a_pins[]	= { GPIODV_25 };
static const unsigned int i2c_sda_a_pins[]	= { GPIODV_24 };

static const unsigned int i2c_sck_b_pins[]	= { GPIODV_27 };
static const unsigned int i2c_sda_b_pins[]	= { GPIODV_26 };

static const unsigned int i2c_sck_c_pins[]	= { GPIODV_29 };
static const unsigned int i2c_sda_c_pins[]	= { GPIODV_28 };

static const unsigned int eth_mdio_pins[]	= { GPIOZ_0 };
static const unsigned int eth_mdc_pins[]	= { GPIOZ_1 };
static const unsigned int eth_clk_rx_clk_pins[]	= { GPIOZ_2 };
static const unsigned int eth_rx_dv_pins[]	= { GPIOZ_3 };
static const unsigned int eth_rxd0_pins[]	= { GPIOZ_4 };
static const unsigned int eth_rxd1_pins[]	= { GPIOZ_5 };
static const unsigned int eth_rxd2_pins[]	= { GPIOZ_6 };
static const unsigned int eth_rxd3_pins[]	= { GPIOZ_7 };
static const unsigned int eth_rgmii_tx_clk_pins[] = { GPIOZ_8 };
static const unsigned int eth_tx_en_pins[]	= { GPIOZ_9 };
static const unsigned int eth_txd0_pins[]	= { GPIOZ_10 };
static const unsigned int eth_txd1_pins[]	= { GPIOZ_11 };
static const unsigned int eth_txd2_pins[]	= { GPIOZ_12 };
static const unsigned int eth_txd3_pins[]	= { GPIOZ_13 };

static const unsigned int pwm_a_x_pins[]	= { GPIOX_6 };
static const unsigned int pwm_a_y_pins[]	= { GPIOY_16 };
static const unsigned int pwm_b_pins[]		= { GPIODV_29 };
static const unsigned int pwm_d_pins[]		= { GPIODV_28 };
static const unsigned int pwm_e_pins[]		= { GPIOX_19 };
static const unsigned int pwm_f_x_pins[]	= { GPIOX_7 };
static const unsigned int pwm_f_y_pins[]	= { GPIOY_15 };

static const unsigned int hdmi_hpd_pins[]	= { GPIOH_0 };
static const unsigned int hdmi_sda_pins[]	= { GPIOH_1 };
static const unsigned int hdmi_scl_pins[]	= { GPIOH_2 };

static const unsigned int tsin_a_d_valid_pins[] = { GPIOY_0 };
static const unsigned int tsin_a_sop_pins[]	= { GPIOY_1 };
static const unsigned int tsin_a_clk_pins[]	= { GPIOY_2 };
static const unsigned int tsin_a_d0_pins[]	= { GPIOY_3 };
static const unsigned int tsin_a_dp_pins[]	= {
	GPIOY_4, GPIOY_5, GPIOY_6, GPIOY_7, GPIOY_8, GPIOY_9, GPIOY_10
};

static const unsigned int tsin_a_fail_pins[]	= { GPIOY_11 };
static const unsigned int i2s_out_ch23_y_pins[]	= { GPIOY_8 };
static const unsigned int i2s_out_ch45_y_pins[]	= { GPIOY_9 };
static const unsigned int i2s_out_ch67_y_pins[]	= { GPIOY_10 };

static const unsigned int tsin_b_d_valid_pins[] = { GPIOX_6 };
static const unsigned int tsin_b_sop_pins[]	= { GPIOX_7 };
static const unsigned int tsin_b_clk_pins[]	= { GPIOX_8 };
static const unsigned int tsin_b_d0_pins[]	= { GPIOX_9 };

static const unsigned int spdif_out_y_pins[]	= { GPIOY_12 };

static const unsigned int gen_clk_out_pins[]	= { GPIOY_15 };

static const struct pinctrl_pin_desc meson_gxbb_aobus_pins[] = {
	MESON_PIN(GPIOAO_0),
	MESON_PIN(GPIOAO_1),
	MESON_PIN(GPIOAO_2),
	MESON_PIN(GPIOAO_3),
	MESON_PIN(GPIOAO_4),
	MESON_PIN(GPIOAO_5),
	MESON_PIN(GPIOAO_6),
	MESON_PIN(GPIOAO_7),
	MESON_PIN(GPIOAO_8),
	MESON_PIN(GPIOAO_9),
	MESON_PIN(GPIOAO_10),
	MESON_PIN(GPIOAO_11),
	MESON_PIN(GPIOAO_12),
	MESON_PIN(GPIOAO_13),

	MESON_PIN(GPIO_TEST_N),
};

static const unsigned int uart_tx_ao_a_pins[]	= { GPIOAO_0 };
static const unsigned int uart_rx_ao_a_pins[]	= { GPIOAO_1 };
static const unsigned int uart_cts_ao_a_pins[]	= { GPIOAO_2 };
static const unsigned int uart_rts_ao_a_pins[]	= { GPIOAO_3 };
static const unsigned int uart_tx_ao_b_pins[]	= { GPIOAO_4 };
static const unsigned int uart_rx_ao_b_pins[]	= { GPIOAO_5 };
static const unsigned int uart_cts_ao_b_pins[]	= { GPIOAO_2 };
static const unsigned int uart_rts_ao_b_pins[]	= { GPIOAO_3 };

static const unsigned int i2c_sck_ao_pins[]	= { GPIOAO_4 };
static const unsigned int i2c_sda_ao_pins[]	= { GPIOAO_5 };
static const unsigned int i2c_slave_sck_ao_pins[] = {GPIOAO_4 };
static const unsigned int i2c_slave_sda_ao_pins[] = {GPIOAO_5 };

static const unsigned int remote_input_ao_pins[] = { GPIOAO_7 };

static const unsigned int pwm_ao_a_3_pins[]	= { GPIOAO_3 };
static const unsigned int pwm_ao_a_6_pins[]	= { GPIOAO_6 };
static const unsigned int pwm_ao_a_12_pins[]	= { GPIOAO_12 };
static const unsigned int pwm_ao_b_pins[]	= { GPIOAO_13 };

static const unsigned int i2s_am_clk_pins[]	= { GPIOAO_8 };
static const unsigned int i2s_out_ao_clk_pins[]	= { GPIOAO_9 };
static const unsigned int i2s_out_lr_clk_pins[]	= { GPIOAO_10 };
static const unsigned int i2s_out_ch01_ao_pins[] = { GPIOAO_11 };
static const unsigned int i2s_out_ch23_ao_pins[] = { GPIOAO_12 };
static const unsigned int i2s_out_ch45_ao_pins[] = { GPIOAO_13 };
static const unsigned int i2s_out_ch67_ao_pins[] = { GPIO_TEST_N };

static const unsigned int spdif_out_ao_6_pins[]	= { GPIOAO_6 };
static const unsigned int spdif_out_ao_13_pins[] = { GPIOAO_13 };

static const unsigned int ao_cec_pins[]		= { GPIOAO_12 };
static const unsigned int ee_cec_pins[]		= { GPIOAO_12 };

static struct meson_pmx_group meson_gxbb_periphs_groups[] = {
	GPIO_GROUP(GPIOZ_0),
	GPIO_GROUP(GPIOZ_1),
	GPIO_GROUP(GPIOZ_2),
	GPIO_GROUP(GPIOZ_3),
	GPIO_GROUP(GPIOZ_4),
	GPIO_GROUP(GPIOZ_5),
	GPIO_GROUP(GPIOZ_6),
	GPIO_GROUP(GPIOZ_7),
	GPIO_GROUP(GPIOZ_8),
	GPIO_GROUP(GPIOZ_9),
	GPIO_GROUP(GPIOZ_10),
	GPIO_GROUP(GPIOZ_11),
	GPIO_GROUP(GPIOZ_12),
	GPIO_GROUP(GPIOZ_13),
	GPIO_GROUP(GPIOZ_14),
	GPIO_GROUP(GPIOZ_15),

	GPIO_GROUP(GPIOH_0),
	GPIO_GROUP(GPIOH_1),
	GPIO_GROUP(GPIOH_2),
	GPIO_GROUP(GPIOH_3),

	GPIO_GROUP(BOOT_0),
	GPIO_GROUP(BOOT_1),
	GPIO_GROUP(BOOT_2),
	GPIO_GROUP(BOOT_3),
	GPIO_GROUP(BOOT_4),
	GPIO_GROUP(BOOT_5),
	GPIO_GROUP(BOOT_6),
	GPIO_GROUP(BOOT_7),
	GPIO_GROUP(BOOT_8),
	GPIO_GROUP(BOOT_9),
	GPIO_GROUP(BOOT_10),
	GPIO_GROUP(BOOT_11),
	GPIO_GROUP(BOOT_12),
	GPIO_GROUP(BOOT_13),
	GPIO_GROUP(BOOT_14),
	GPIO_GROUP(BOOT_15),
	GPIO_GROUP(BOOT_16),
	GPIO_GROUP(BOOT_17),

	GPIO_GROUP(CARD_0),
	GPIO_GROUP(CARD_1),
	GPIO_GROUP(CARD_2),
	GPIO_GROUP(CARD_3),
	GPIO_GROUP(CARD_4),
	GPIO_GROUP(CARD_5),
	GPIO_GROUP(CARD_6),

	GPIO_GROUP(GPIODV_0),
	GPIO_GROUP(GPIODV_1),
	GPIO_GROUP(GPIODV_2),
	GPIO_GROUP(GPIODV_3),
	GPIO_GROUP(GPIODV_4),
	GPIO_GROUP(GPIODV_5),
	GPIO_GROUP(GPIODV_6),
	GPIO_GROUP(GPIODV_7),
	GPIO_GROUP(GPIODV_8),
	GPIO_GROUP(GPIODV_9),
	GPIO_GROUP(GPIODV_10),
	GPIO_GROUP(GPIODV_11),
	GPIO_GROUP(GPIODV_12),
	GPIO_GROUP(GPIODV_13),
	GPIO_GROUP(GPIODV_14),
	GPIO_GROUP(GPIODV_15),
	GPIO_GROUP(GPIODV_16),
	GPIO_GROUP(GPIODV_17),
	GPIO_GROUP(GPIODV_19),
	GPIO_GROUP(GPIODV_20),
	GPIO_GROUP(GPIODV_21),
	GPIO_GROUP(GPIODV_22),
	GPIO_GROUP(GPIODV_23),
	GPIO_GROUP(GPIODV_24),
	GPIO_GROUP(GPIODV_25),
	GPIO_GROUP(GPIODV_26),
	GPIO_GROUP(GPIODV_27),
	GPIO_GROUP(GPIODV_28),
	GPIO_GROUP(GPIODV_29),

	GPIO_GROUP(GPIOY_0),
	GPIO_GROUP(GPIOY_1),
	GPIO_GROUP(GPIOY_2),
	GPIO_GROUP(GPIOY_3),
	GPIO_GROUP(GPIOY_4),
	GPIO_GROUP(GPIOY_5),
	GPIO_GROUP(GPIOY_6),
	GPIO_GROUP(GPIOY_7),
	GPIO_GROUP(GPIOY_8),
	GPIO_GROUP(GPIOY_9),
	GPIO_GROUP(GPIOY_10),
	GPIO_GROUP(GPIOY_11),
	GPIO_GROUP(GPIOY_12),
	GPIO_GROUP(GPIOY_13),
	GPIO_GROUP(GPIOY_14),
	GPIO_GROUP(GPIOY_15),
	GPIO_GROUP(GPIOY_16),

	GPIO_GROUP(GPIOX_0),
	GPIO_GROUP(GPIOX_1),
	GPIO_GROUP(GPIOX_2),
	GPIO_GROUP(GPIOX_3),
	GPIO_GROUP(GPIOX_4),
	GPIO_GROUP(GPIOX_5),
	GPIO_GROUP(GPIOX_6),
	GPIO_GROUP(GPIOX_7),
	GPIO_GROUP(GPIOX_8),
	GPIO_GROUP(GPIOX_9),
	GPIO_GROUP(GPIOX_10),
	GPIO_GROUP(GPIOX_11),
	GPIO_GROUP(GPIOX_12),
	GPIO_GROUP(GPIOX_13),
	GPIO_GROUP(GPIOX_14),
	GPIO_GROUP(GPIOX_15),
	GPIO_GROUP(GPIOX_16),
	GPIO_GROUP(GPIOX_17),
	GPIO_GROUP(GPIOX_18),
	GPIO_GROUP(GPIOX_19),
	GPIO_GROUP(GPIOX_20),
	GPIO_GROUP(GPIOX_21),
	GPIO_GROUP(GPIOX_22),

	GPIO_GROUP(GPIOCLK_0),
	GPIO_GROUP(GPIOCLK_1),
	GPIO_GROUP(GPIOCLK_2),
	GPIO_GROUP(GPIOCLK_3),

	GPIO_GROUP(GPIO_TEST_N),

	/* Bank X */
	GROUP(sdio_d0,		8,	5),
	GROUP(sdio_d1,		8,	4),
	GROUP(sdio_d2,		8,	3),
	GROUP(sdio_d3,		8,	2),
	GROUP(sdio_cmd,		8,	1),
	GROUP(sdio_clk,		8,	0),
	GROUP(sdio_irq,		8,	11),
	GROUP(uart_tx_a,	4,	13),
	GROUP(uart_rx_a,	4,	12),
	GROUP(uart_cts_a,	4,	11),
	GROUP(uart_rts_a,	4,	10),
	GROUP(pwm_a_x,		3,	17),
	GROUP(pwm_e,		2,	30),
	GROUP(pwm_f_x,		3,	18),
	GROUP(tsin_b_d_valid,	3,	9),
	GROUP(tsin_b_sop,	3,	8),
	GROUP(tsin_b_clk,	3,	10),
	GROUP(tsin_b_d0,	3,	7),

	/* Bank Y */
	GROUP(uart_cts_c,	1,	17),
	GROUP(uart_rts_c,	1,	16),
	GROUP(uart_tx_c,	1,	19),
	GROUP(uart_rx_c,	1,	18),
	GROUP(tsin_a_fail,	3,	3),
	GROUP(tsin_a_d_valid,	3,	2),
	GROUP(tsin_a_sop,	3,	1),
	GROUP(tsin_a_clk,	3,	0),
	GROUP(tsin_a_d0,	3,	4),
	GROUP(tsin_a_dp,	3,	5),
	GROUP(pwm_a_y,		1,	21),
	GROUP(pwm_f_y,		1,	20),
	GROUP(i2s_out_ch23_y,	1,	5),
	GROUP(i2s_out_ch45_y,	1,	6),
	GROUP(i2s_out_ch67_y,	1,	7),
	GROUP(spdif_out_y,	1,	9),
	GROUP(gen_clk_out,	6,	15),

	/* Bank Z */
	GROUP(eth_mdio,		6,	1),
	GROUP(eth_mdc,		6,	0),
	GROUP(eth_clk_rx_clk,	6,	13),
	GROUP(eth_rx_dv,	6,	12),
	GROUP(eth_rxd0,		6,	11),
	GROUP(eth_rxd1,		6,	10),
	GROUP(eth_rxd2,		6,	9),
	GROUP(eth_rxd3,		6,	8),
	GROUP(eth_rgmii_tx_clk,	6,	7),
	GROUP(eth_tx_en,	6,	6),
	GROUP(eth_txd0,		6,	5),
	GROUP(eth_txd1,		6,	4),
	GROUP(eth_txd2,		6,	3),
	GROUP(eth_txd3,		6,	2),
	GROUP(spi_ss0,		5,	26),
	GROUP(spi_sclk,		5,	27),
	GROUP(spi_miso,		5,	28),
	GROUP(spi_mosi,		5,	29),

	/* Bank H */
	GROUP(hdmi_hpd,		1,	26),
	GROUP(hdmi_sda,		1,	25),
	GROUP(hdmi_scl,		1,	24),

	/* Bank DV */
	GROUP(uart_tx_b,	2,	29),
	GROUP(uart_rx_b,	2,	28),
	GROUP(uart_cts_b,	2,	27),
	GROUP(uart_rts_b,	2,	26),
	GROUP(pwm_b,		3,	21),
	GROUP(pwm_d,		3,	20),
	GROUP(i2c_sck_a,	7,	27),
	GROUP(i2c_sda_a,	7,	26),
	GROUP(i2c_sck_b,	7,	25),
	GROUP(i2c_sda_b,	7,	24),
	GROUP(i2c_sck_c,	7,	23),
	GROUP(i2c_sda_c,	7,	22),

	/* Bank BOOT */
	GROUP(emmc_nand_d07,	4,	30),
	GROUP(emmc_clk,		4,	18),
	GROUP(emmc_cmd,		4,	19),
	GROUP(emmc_ds,		4,	31),
	GROUP(nor_d,		5,	1),
	GROUP(nor_q,		5,	3),
	GROUP(nor_c,		5,	2),
	GROUP(nor_cs,		5,	0),
	GROUP(nand_ce0,		4,	26),
	GROUP(nand_ce1,		4,	27),
	GROUP(nand_rb0,		4,	25),
	GROUP(nand_ale,		4,	24),
	GROUP(nand_cle,		4,	23),
	GROUP(nand_wen_clk,	4,	22),
	GROUP(nand_ren_wr,	4,	21),
	GROUP(nand_dqs,		4,	20),

	/* Bank CARD */
	GROUP(sdcard_d1,	2,	14),
	GROUP(sdcard_d0,	2,	15),
	GROUP(sdcard_d3,	2,	12),
	GROUP(sdcard_d2,	2,	13),
	GROUP(sdcard_cmd,	2,	10),
	GROUP(sdcard_clk,	2,	11),
};

static struct meson_pmx_group meson_gxbb_aobus_groups[] = {
	GPIO_GROUP(GPIOAO_0),
	GPIO_GROUP(GPIOAO_1),
	GPIO_GROUP(GPIOAO_2),
	GPIO_GROUP(GPIOAO_3),
	GPIO_GROUP(GPIOAO_4),
	GPIO_GROUP(GPIOAO_5),
	GPIO_GROUP(GPIOAO_6),
	GPIO_GROUP(GPIOAO_7),
	GPIO_GROUP(GPIOAO_8),
	GPIO_GROUP(GPIOAO_9),
	GPIO_GROUP(GPIOAO_10),
	GPIO_GROUP(GPIOAO_11),
	GPIO_GROUP(GPIOAO_12),
	GPIO_GROUP(GPIOAO_13),

	/* bank AO */
	GROUP(uart_tx_ao_b,	0,	24),
	GROUP(uart_rx_ao_b,	0,	25),
	GROUP(uart_tx_ao_a,	0,	12),
	GROUP(uart_rx_ao_a,	0,	11),
	GROUP(uart_cts_ao_a,	0,	10),
	GROUP(uart_rts_ao_a,	0,	9),
	GROUP(uart_cts_ao_b,	0,	8),
	GROUP(uart_rts_ao_b,	0,	7),
	GROUP(i2c_sck_ao,	0,	6),
	GROUP(i2c_sda_ao,	0,	5),
	GROUP(i2c_slave_sck_ao, 0,	2),
	GROUP(i2c_slave_sda_ao, 0,	1),
	GROUP(remote_input_ao,	0,	0),
	GROUP(pwm_ao_a_3,	0,	22),
	GROUP(pwm_ao_a_6,	0,	18),
	GROUP(pwm_ao_a_12,	0,	17),
	GROUP(pwm_ao_b,		0,	3),
	GROUP(i2s_am_clk,	0,	30),
	GROUP(i2s_out_ao_clk,	0,	29),
	GROUP(i2s_out_lr_clk,	0,	28),
	GROUP(i2s_out_ch01_ao,	0,	27),
	GROUP(i2s_out_ch23_ao,	1,	0),
	GROUP(i2s_out_ch45_ao,	1,	1),
	GROUP(spdif_out_ao_6,	0,	16),
	GROUP(spdif_out_ao_13,	0,	4),
	GROUP(ao_cec,           0,      15),
	GROUP(ee_cec,           0,      14),

	/* test n pin */
	GROUP(i2s_out_ch67_ao,	1,	2),
};

static const char * const gpio_periphs_groups[] = {
	"GPIOZ_0", "GPIOZ_1", "GPIOZ_2", "GPIOZ_3", "GPIOZ_4",
	"GPIOZ_5", "GPIOZ_6", "GPIOZ_7", "GPIOZ_8", "GPIOZ_9",
	"GPIOZ_10", "GPIOZ_11", "GPIOZ_12", "GPIOZ_13", "GPIOZ_14",
	"GPIOZ_15",

	"GPIOH_0", "GPIOH_1", "GPIOH_2", "GPIOH_3",

	"BOOT_0", "BOOT_1", "BOOT_2", "BOOT_3", "BOOT_4",
	"BOOT_5", "BOOT_6", "BOOT_7", "BOOT_8", "BOOT_9",
	"BOOT_10", "BOOT_11", "BOOT_12", "BOOT_13", "BOOT_14",
	"BOOT_15", "BOOT_16", "BOOT_17",

	"CARD_0", "CARD_1", "CARD_2", "CARD_3", "CARD_4",
	"CARD_5", "CARD_6",

	"GPIODV_0", "GPIODV_1", "GPIODV_2", "GPIODV_3", "GPIODV_4",
	"GPIODV_5", "GPIODV_6", "GPIODV_7", "GPIODV_8", "GPIODV_9",
	"GPIODV_10", "GPIODV_11", "GPIODV_12", "GPIODV_13", "GPIODV_14",
	"GPIODV_15", "GPIODV_16", "GPIODV_17", "GPIODV_18", "GPIODV_19",
	"GPIODV_20", "GPIODV_21", "GPIODV_22", "GPIODV_23", "GPIODV_24",
	"GPIODV_25", "GPIODV_26", "GPIODV_27", "GPIODV_28", "GPIODV_29",

	"GPIOY_0", "GPIOY_1", "GPIOY_2", "GPIOY_3", "GPIOY_4",
	"GPIOY_5", "GPIOY_6", "GPIOY_7", "GPIOY_8", "GPIOY_9",
	"GPIOY_10", "GPIOY_11", "GPIOY_12", "GPIOY_13", "GPIOY_14",
	"GPIOY_15", "GPIOY_16",

	"GPIOX_0", "GPIOX_1", "GPIOX_2", "GPIOX_3", "GPIOX_4",
	"GPIOX_5", "GPIOX_6", "GPIOX_7", "GPIOX_8", "GPIOX_9",
	"GPIOX_10", "GPIOX_11", "GPIOX_12", "GPIOX_13", "GPIOX_14",
	"GPIOX_15", "GPIOX_16", "GPIOX_17", "GPIOX_18", "GPIOX_19",
	"GPIOX_20", "GPIOX_21", "GPIOX_22",
};

static const char * const tsin_a_groups[] = {
	"tsin_a_clk", "tsin_a_sop", "tsin_a_d_valid", "tsin_a_d0",
	"tsin_a_dp", "tsin_a_fail",
};

static const char * const tsin_b_groups[] = {
	"tsin_b_clk", "tsin_b_sop", "tsin_b_d_valid", "tsin_b_d0",
};

static const char * const emmc_groups[] = {
	"emmc_nand_d07", "emmc_clk", "emmc_cmd", "emmc_ds",
};

static const char * const nor_groups[] = {
	"nor_d", "nor_q", "nor_c", "nor_cs",
};

static const char * const spi_groups[] = {
	"spi_mosi", "spi_miso", "spi_ss0", "spi_sclk",
};

static const char * const sdcard_groups[] = {
	"sdcard_d0", "sdcard_d1", "sdcard_d2", "sdcard_d3",
	"sdcard_cmd", "sdcard_clk",
};

static const char * const sdio_groups[] = {
	"sdio_d0", "sdio_d1", "sdio_d2", "sdio_d3",
	"sdio_cmd", "sdio_clk", "sdio_irq",
};

static const char * const nand_groups[] = {
	"emmc_nand_d07", "nand_ce0", "nand_ce1", "nand_rb0", "nand_ale",
	"nand_cle", "nand_wen_clk", "nand_ren_wr", "nand_dqs",
};

static const char * const uart_a_groups[] = {
	"uart_tx_a", "uart_rx_a", "uart_cts_a", "uart_rts_a",
};

static const char * const uart_b_groups[] = {
	"uart_tx_b", "uart_rx_b", "uart_cts_b", "uart_rts_b",
};

static const char * const uart_c_groups[] = {
	"uart_tx_c", "uart_rx_c", "uart_cts_c", "uart_rts_c",
};

static const char * const i2c_a_groups[] = {
	"i2c_sck_a", "i2c_sda_a",
};

static const char * const i2c_b_groups[] = {
	"i2c_sck_b", "i2c_sda_b",
};

static const char * const i2c_c_groups[] = {
	"i2c_sck_c", "i2c_sda_c",
};

static const char * const eth_groups[] = {
	"eth_mdio", "eth_mdc", "eth_clk_rx_clk", "eth_rx_dv",
	"eth_rxd0", "eth_rxd1", "eth_rxd2", "eth_rxd3",
	"eth_rgmii_tx_clk", "eth_tx_en",
	"eth_txd0", "eth_txd1", "eth_txd2", "eth_txd3",
};

static const char * const pwm_a_x_groups[] = {
	"pwm_a_x",
};

static const char * const pwm_a_y_groups[] = {
	"pwm_a_y",
};

static const char * const pwm_b_groups[] = {
	"pwm_b",
};

static const char * const pwm_d_groups[] = {
	"pwm_d",
};

static const char * const pwm_e_groups[] = {
	"pwm_e",
};

static const char * const pwm_f_x_groups[] = {
	"pwm_f_x",
};

static const char * const pwm_f_y_groups[] = {
	"pwm_f_y",
};

static const char * const hdmi_hpd_groups[] = {
	"hdmi_hpd",
};

static const char * const hdmi_i2c_groups[] = {
	"hdmi_sda", "hdmi_scl",
};

static const char * const i2s_out_groups[] = {
	"i2s_out_ch23_y", "i2s_out_ch45_y", "i2s_out_ch67_y",
};

static const char * const spdif_out_groups[] = {
	"spdif_out_y",
};

static const char * const gen_clk_out_groups[] = {
	"gen_clk_out",
};

static const char * const gpio_aobus_groups[] = {
	"GPIOAO_0", "GPIOAO_1", "GPIOAO_2", "GPIOAO_3", "GPIOAO_4",
	"GPIOAO_5", "GPIOAO_6", "GPIOAO_7", "GPIOAO_8", "GPIOAO_9",
	"GPIOAO_10", "GPIOAO_11", "GPIOAO_12", "GPIOAO_13",

	"GPIO_TEST_N",
};

static const char * const uart_ao_groups[] = {
	"uart_tx_ao_a", "uart_rx_ao_a", "uart_cts_ao_a", "uart_rts_ao_a",
};

static const char * const uart_ao_b_groups[] = {
	"uart_tx_ao_b", "uart_rx_ao_b", "uart_cts_ao_b", "uart_rts_ao_b",
};

static const char * const i2c_ao_groups[] = {
	"i2c_sck_ao", "i2c_sda_ao",
};

static const char * const i2c_slave_ao_groups[] = {
	"i2c_slave_sck_ao", "i2c_slave_sda_ao",
};

static const char * const remote_input_ao_groups[] = {
	"remote_input_ao",
};

static const char * const pwm_ao_a_3_groups[] = {
	"pwm_ao_a_3",
};

static const char * const pwm_ao_a_6_groups[] = {
	"pwm_ao_a_6",
};

static const char * const pwm_ao_a_12_groups[] = {
	"pwm_ao_a_12",
};

static const char * const pwm_ao_b_groups[] = {
	"pwm_ao_b",
};

static const char * const i2s_out_ao_groups[] = {
	"i2s_am_clk", "i2s_out_ao_clk", "i2s_out_lr_clk",
	"i2s_out_ch01_ao", "i2s_out_ch23_ao", "i2s_out_ch45_ao",
	"i2s_out_ch67_ao",
};

static const char * const spdif_out_ao_groups[] = {
	"spdif_out_ao_6", "spdif_out_ao_13",
};

static const char * const cec_ao_groups[] = {
	"ao_cec", "ee_cec",
};

static struct meson_pmx_func meson_gxbb_periphs_functions[] = {
	FUNCTION(gpio_periphs),
	FUNCTION(emmc),
	FUNCTION(nor),
	FUNCTION(spi),
	FUNCTION(sdcard),
	FUNCTION(sdio),
	FUNCTION(nand),
	FUNCTION(uart_a),
	FUNCTION(uart_b),
	FUNCTION(uart_c),
	FUNCTION(i2c_a),
	FUNCTION(i2c_b),
	FUNCTION(i2c_c),
	FUNCTION(eth),
	FUNCTION(pwm_a_x),
	FUNCTION(pwm_a_y),
	FUNCTION(pwm_b),
	FUNCTION(pwm_d),
	FUNCTION(pwm_e),
	FUNCTION(pwm_f_x),
	FUNCTION(pwm_f_y),
	FUNCTION(hdmi_hpd),
	FUNCTION(hdmi_i2c),
	FUNCTION(i2s_out),
	FUNCTION(spdif_out),
	FUNCTION(gen_clk_out),
	FUNCTION(tsin_a),
	FUNCTION(tsin_b),
};

static struct meson_pmx_func meson_gxbb_aobus_functions[] = {
	FUNCTION(gpio_aobus),
	FUNCTION(uart_ao),
	FUNCTION(uart_ao_b),
	FUNCTION(i2c_ao),
	FUNCTION(i2c_slave_ao),
	FUNCTION(remote_input_ao),
	FUNCTION(pwm_ao_a_3),
	FUNCTION(pwm_ao_a_6),
	FUNCTION(pwm_ao_a_12),
	FUNCTION(pwm_ao_b),
	FUNCTION(i2s_out_ao),
	FUNCTION(spdif_out_ao),
	FUNCTION(cec_ao),
};

static struct meson_bank meson_gxbb_periphs_banks[] = {
	/*   name    first      last       irq       pullen  pull    dir     out     in  */
	BANK("X",    GPIOX_0,	GPIOX_22,  106, 128, 4,  0,  4,  0,  12, 0,  13, 0,  14, 0),
	BANK("Y",    GPIOY_0,	GPIOY_16,   89, 105, 1,  0,  1,  0,  3,  0,  4,  0,  5,  0),
	BANK("DV",   GPIODV_0,	GPIODV_29,  59,  88, 0,  0,  0,  0,  0,  0,  1,  0,  2,  0),
	BANK("H",    GPIOH_0,	GPIOH_3,    30,  33, 1, 20,  1, 20,  3, 20,  4, 20,  5, 20),
	BANK("Z",    GPIOZ_0,	GPIOZ_15,   14,  29, 3,  0,  3,  0,  9,  0,  10, 0, 11,  0),
	BANK("CARD", CARD_0,	CARD_6,     52,  58, 2, 20,  2, 20,  6, 20,  7, 20,  8, 20),
	BANK("BOOT", BOOT_0,	BOOT_17,    34,  51, 2,  0,  2,  0,  6,  0,  7,  0,  8,  0),
	BANK("CLK",  GPIOCLK_0,	GPIOCLK_3, 129, 132, 3, 28,  3, 28,  9, 28, 10, 28, 11, 28),
};

static struct meson_bank meson_gxbb_aobus_banks[] = {
	/*   name    first      last       irq    pullen  pull    dir     out     in  */
	BANK("AO",   GPIOAO_0,  GPIOAO_13, 0, 13, 0,  16, 0, 0,   0,  0,  0, 16,  1,  0),
};

static struct meson_pinctrl_data meson_gxbb_periphs_pinctrl_data = {
	.name		= "periphs-banks",
	.pins		= meson_gxbb_periphs_pins,
	.groups		= meson_gxbb_periphs_groups,
	.funcs		= meson_gxbb_periphs_functions,
	.banks		= meson_gxbb_periphs_banks,
	.num_pins	= ARRAY_SIZE(meson_gxbb_periphs_pins),
	.num_groups	= ARRAY_SIZE(meson_gxbb_periphs_groups),
	.num_funcs	= ARRAY_SIZE(meson_gxbb_periphs_functions),
	.num_banks	= ARRAY_SIZE(meson_gxbb_periphs_banks),
	.pmx_ops	= &meson8_pmx_ops,
};

static struct meson_pinctrl_data meson_gxbb_aobus_pinctrl_data = {
	.name		= "aobus-banks",
	.pins		= meson_gxbb_aobus_pins,
	.groups		= meson_gxbb_aobus_groups,
	.funcs		= meson_gxbb_aobus_functions,
	.banks		= meson_gxbb_aobus_banks,
	.num_pins	= ARRAY_SIZE(meson_gxbb_aobus_pins),
	.num_groups	= ARRAY_SIZE(meson_gxbb_aobus_groups),
	.num_funcs	= ARRAY_SIZE(meson_gxbb_aobus_functions),
	.num_banks	= ARRAY_SIZE(meson_gxbb_aobus_banks),
	.pmx_ops	= &meson8_pmx_ops,
	.parse_dt	= meson8_aobus_parse_dt_extra,
};

static const struct of_device_id meson_gxbb_pinctrl_dt_match[] = {
	{
		.compatible = "amlogic,meson-gxbb-periphs-pinctrl",
		.data = &meson_gxbb_periphs_pinctrl_data,
	},
	{
		.compatible = "amlogic,meson-gxbb-aobus-pinctrl",
		.data = &meson_gxbb_aobus_pinctrl_data,
	},
	{ },
};
MODULE_DEVICE_TABLE(of, meson_gxbb_pinctrl_dt_match);

static struct platform_driver meson_gxbb_pinctrl_driver = {
	.probe		= meson_pinctrl_probe,
	.driver = {
		.name	= "meson-gxbb-pinctrl",
		.of_match_table = meson_gxbb_pinctrl_dt_match,
	},
};
module_platform_driver(meson_gxbb_pinctrl_driver);
MODULE_LICENSE("GPL v2");

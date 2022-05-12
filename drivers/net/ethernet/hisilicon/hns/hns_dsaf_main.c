// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2014-2015 Hisilicon Limited.
 */

#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>

#include "hns_dsaf_mac.h"
#include "hns_dsaf_main.h"
#include "hns_dsaf_ppe.h"
#include "hns_dsaf_rcb.h"
#include "hns_dsaf_misc.h"

static const char *g_dsaf_mode_match[DSAF_MODE_MAX] = {
	[DSAF_MODE_DISABLE_2PORT_64VM] = "2port-64vf",
	[DSAF_MODE_DISABLE_6PORT_0VM] = "6port-16rss",
	[DSAF_MODE_DISABLE_6PORT_16VM] = "6port-16vf",
	[DSAF_MODE_DISABLE_SP] = "single-port",
};

static const struct acpi_device_id hns_dsaf_acpi_match[] = {
	{ "HISI00B1", 0 },
	{ "HISI00B2", 0 },
	{ },
};
MODULE_DEVICE_TABLE(acpi, hns_dsaf_acpi_match);

static int hns_dsaf_get_cfg(struct dsaf_device *dsaf_dev)
{
	int ret, i;
	u32 desc_num;
	u32 buf_size;
	u32 reset_offset = 0;
	u32 res_idx = 0;
	const char *mode_str;
	struct regmap *syscon;
	struct resource *res;
	struct device_node *np = dsaf_dev->dev->of_node, *np_temp;
	struct platform_device *pdev = to_platform_device(dsaf_dev->dev);

	if (dev_of_node(dsaf_dev->dev)) {
		if (of_device_is_compatible(np, "hisilicon,hns-dsaf-v1"))
			dsaf_dev->dsaf_ver = AE_VERSION_1;
		else
			dsaf_dev->dsaf_ver = AE_VERSION_2;
	} else if (is_acpi_node(dsaf_dev->dev->fwnode)) {
		if (acpi_dev_found(hns_dsaf_acpi_match[0].id))
			dsaf_dev->dsaf_ver = AE_VERSION_1;
		else if (acpi_dev_found(hns_dsaf_acpi_match[1].id))
			dsaf_dev->dsaf_ver = AE_VERSION_2;
		else
			return -ENXIO;
	} else {
		dev_err(dsaf_dev->dev, "cannot get cfg data from of or acpi\n");
		return -ENXIO;
	}

	ret = device_property_read_string(dsaf_dev->dev, "mode", &mode_str);
	if (ret) {
		dev_err(dsaf_dev->dev, "get dsaf mode fail, ret=%d!\n", ret);
		return ret;
	}
	for (i = 0; i < DSAF_MODE_MAX; i++) {
		if (g_dsaf_mode_match[i] &&
		    !strcmp(mode_str, g_dsaf_mode_match[i]))
			break;
	}
	if (i >= DSAF_MODE_MAX ||
	    i == DSAF_MODE_INVALID || i == DSAF_MODE_ENABLE) {
		dev_err(dsaf_dev->dev,
			"%s prs mode str fail!\n", dsaf_dev->ae_dev.name);
		return -EINVAL;
	}
	dsaf_dev->dsaf_mode = (enum dsaf_mode)i;

	if (dsaf_dev->dsaf_mode > DSAF_MODE_ENABLE)
		dsaf_dev->dsaf_en = HRD_DSAF_NO_DSAF_MODE;
	else
		dsaf_dev->dsaf_en = HRD_DSAF_MODE;

	if ((i == DSAF_MODE_ENABLE_16VM) ||
	    (i == DSAF_MODE_DISABLE_2PORT_8VM) ||
	    (i == DSAF_MODE_DISABLE_6PORT_2VM))
		dsaf_dev->dsaf_tc_mode = HRD_DSAF_8TC_MODE;
	else
		dsaf_dev->dsaf_tc_mode = HRD_DSAF_4TC_MODE;

	if (dev_of_node(dsaf_dev->dev)) {
		np_temp = of_parse_phandle(np, "subctrl-syscon", 0);
		syscon = syscon_node_to_regmap(np_temp);
		of_node_put(np_temp);
		if (IS_ERR_OR_NULL(syscon)) {
			res = platform_get_resource(pdev, IORESOURCE_MEM,
						    res_idx++);
			if (!res) {
				dev_err(dsaf_dev->dev, "subctrl info is needed!\n");
				return -ENOMEM;
			}

			dsaf_dev->sc_base = devm_ioremap_resource(&pdev->dev,
								  res);
			if (IS_ERR(dsaf_dev->sc_base))
				return PTR_ERR(dsaf_dev->sc_base);

			res = platform_get_resource(pdev, IORESOURCE_MEM,
						    res_idx++);
			if (!res) {
				dev_err(dsaf_dev->dev, "serdes-ctrl info is needed!\n");
				return -ENOMEM;
			}

			dsaf_dev->sds_base = devm_ioremap_resource(&pdev->dev,
								   res);
			if (IS_ERR(dsaf_dev->sds_base))
				return PTR_ERR(dsaf_dev->sds_base);
		} else {
			dsaf_dev->sub_ctrl = syscon;
		}
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "ppe-base");
	if (!res) {
		res = platform_get_resource(pdev, IORESOURCE_MEM, res_idx++);
		if (!res) {
			dev_err(dsaf_dev->dev, "ppe-base info is needed!\n");
			return -ENOMEM;
		}
	}
	dsaf_dev->ppe_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(dsaf_dev->ppe_base))
		return PTR_ERR(dsaf_dev->ppe_base);
	dsaf_dev->ppe_paddr = res->start;

	if (!HNS_DSAF_IS_DEBUG(dsaf_dev)) {
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						   "dsaf-base");
		if (!res) {
			res = platform_get_resource(pdev, IORESOURCE_MEM,
						    res_idx);
			if (!res) {
				dev_err(dsaf_dev->dev,
					"dsaf-base info is needed!\n");
				return -ENOMEM;
			}
		}
		dsaf_dev->io_base = devm_ioremap_resource(&pdev->dev, res);
		if (IS_ERR(dsaf_dev->io_base))
			return PTR_ERR(dsaf_dev->io_base);
	}

	ret = device_property_read_u32(dsaf_dev->dev, "desc-num", &desc_num);
	if (ret < 0 || desc_num < HNS_DSAF_MIN_DESC_CNT ||
	    desc_num > HNS_DSAF_MAX_DESC_CNT) {
		dev_err(dsaf_dev->dev, "get desc-num(%d) fail, ret=%d!\n",
			desc_num, ret);
		return -EINVAL;
	}
	dsaf_dev->desc_num = desc_num;

	ret = device_property_read_u32(dsaf_dev->dev, "reset-field-offset",
				       &reset_offset);
	if (ret < 0) {
		dev_dbg(dsaf_dev->dev,
			"get reset-field-offset fail, ret=%d!\r\n", ret);
	}
	dsaf_dev->reset_offset = reset_offset;

	ret = device_property_read_u32(dsaf_dev->dev, "buf-size", &buf_size);
	if (ret < 0) {
		dev_err(dsaf_dev->dev,
			"get buf-size fail, ret=%d!\r\n", ret);
		return ret;
	}
	dsaf_dev->buf_size = buf_size;

	dsaf_dev->buf_size_type = hns_rcb_buf_size2type(buf_size);
	if (dsaf_dev->buf_size_type < 0) {
		dev_err(dsaf_dev->dev,
			"buf_size(%d) is wrong!\n", buf_size);
		return -EINVAL;
	}

	dsaf_dev->misc_op = hns_misc_op_get(dsaf_dev);
	if (!dsaf_dev->misc_op)
		return -ENOMEM;

	if (!dma_set_mask_and_coherent(dsaf_dev->dev, DMA_BIT_MASK(64ULL)))
		dev_dbg(dsaf_dev->dev, "set mask to 64bit\n");
	else
		dev_err(dsaf_dev->dev, "set mask to 64bit fail!\n");

	return 0;
}

/**
 * hns_dsaf_sbm_link_sram_init_en - config dsaf_sbm_init_en
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_sbm_link_sram_init_en(struct dsaf_device *dsaf_dev)
{
	dsaf_set_dev_bit(dsaf_dev, DSAF_CFG_0_REG, DSAF_CFG_SBM_INIT_S, 1);
}

/**
 * hns_dsaf_reg_cnt_clr_ce - config hns_dsaf_reg_cnt_clr_ce
 * @dsaf_dev: dsa fabric id
 * @reg_cnt_clr_ce: config value
 */
static void
hns_dsaf_reg_cnt_clr_ce(struct dsaf_device *dsaf_dev, u32 reg_cnt_clr_ce)
{
	dsaf_set_dev_bit(dsaf_dev, DSAF_DSA_REG_CNT_CLR_CE_REG,
			 DSAF_CNT_CLR_CE_S, reg_cnt_clr_ce);
}

/**
 * hns_ppe_qid_cfg - config ppe qid
 * @dsaf_dev: dsa fabric id
 * @qid_cfg: value array
 */
static void
hns_dsaf_ppe_qid_cfg(struct dsaf_device *dsaf_dev, u32 qid_cfg)
{
	u32 i;

	for (i = 0; i < DSAF_COMM_CHN; i++) {
		dsaf_set_dev_field(dsaf_dev,
				   DSAF_PPE_QID_CFG_0_REG + 0x0004 * i,
				   DSAF_PPE_QID_CFG_M, DSAF_PPE_QID_CFG_S,
				   qid_cfg);
	}
}

static void hns_dsaf_mix_def_qid_cfg(struct dsaf_device *dsaf_dev)
{
	u16 max_q_per_vf, max_vfn;
	u32 q_id, q_num_per_port;
	u32 i;

	hns_rcb_get_queue_mode(dsaf_dev->dsaf_mode, &max_vfn, &max_q_per_vf);
	q_num_per_port = max_vfn * max_q_per_vf;

	for (i = 0, q_id = 0; i < DSAF_SERVICE_NW_NUM; i++) {
		dsaf_set_dev_field(dsaf_dev,
				   DSAF_MIX_DEF_QID_0_REG + 0x0004 * i,
				   0xff, 0, q_id);
		q_id += q_num_per_port;
	}
}

static void hns_dsaf_inner_qid_cfg(struct dsaf_device *dsaf_dev)
{
	u16 max_q_per_vf, max_vfn;
	u32 q_id, q_num_per_port;
	u32 mac_id;

	if (AE_IS_VER1(dsaf_dev->dsaf_ver))
		return;

	hns_rcb_get_queue_mode(dsaf_dev->dsaf_mode, &max_vfn, &max_q_per_vf);
	q_num_per_port = max_vfn * max_q_per_vf;

	for (mac_id = 0, q_id = 0; mac_id < DSAF_SERVICE_NW_NUM; mac_id++) {
		dsaf_set_dev_field(dsaf_dev,
				   DSAFV2_SERDES_LBK_0_REG + 4 * mac_id,
				   DSAFV2_SERDES_LBK_QID_M,
				   DSAFV2_SERDES_LBK_QID_S,
				   q_id);
		q_id += q_num_per_port;
	}
}

/**
 * hns_dsaf_sw_port_type_cfg - cfg sw type
 * @dsaf_dev: dsa fabric id
 * @port_type: array
 */
static void hns_dsaf_sw_port_type_cfg(struct dsaf_device *dsaf_dev,
				      enum dsaf_sw_port_type port_type)
{
	u32 i;

	for (i = 0; i < DSAF_SW_PORT_NUM; i++) {
		dsaf_set_dev_field(dsaf_dev,
				   DSAF_SW_PORT_TYPE_0_REG + 0x0004 * i,
				   DSAF_SW_PORT_TYPE_M, DSAF_SW_PORT_TYPE_S,
				   port_type);
	}
}

/**
 * hns_dsaf_stp_port_type_cfg - cfg stp type
 * @dsaf_dev: dsa fabric id
 * @port_type: array
 */
static void hns_dsaf_stp_port_type_cfg(struct dsaf_device *dsaf_dev,
				       enum dsaf_stp_port_type port_type)
{
	u32 i;

	for (i = 0; i < DSAF_COMM_CHN; i++) {
		dsaf_set_dev_field(dsaf_dev,
				   DSAF_STP_PORT_TYPE_0_REG + 0x0004 * i,
				   DSAF_STP_PORT_TYPE_M, DSAF_STP_PORT_TYPE_S,
				   port_type);
	}
}

#define HNS_DSAF_SBM_NUM(dev) \
	(AE_IS_VER1((dev)->dsaf_ver) ? DSAF_SBM_NUM : DSAFV2_SBM_NUM)
/**
 * hns_dsaf_sbm_cfg - config sbm
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_sbm_cfg(struct dsaf_device *dsaf_dev)
{
	u32 o_sbm_cfg;
	u32 i;

	for (i = 0; i < HNS_DSAF_SBM_NUM(dsaf_dev); i++) {
		o_sbm_cfg = dsaf_read_dev(dsaf_dev,
					  DSAF_SBM_CFG_REG_0_REG + 0x80 * i);
		dsaf_set_bit(o_sbm_cfg, DSAF_SBM_CFG_EN_S, 1);
		dsaf_set_bit(o_sbm_cfg, DSAF_SBM_CFG_SHCUT_EN_S, 0);
		dsaf_write_dev(dsaf_dev,
			       DSAF_SBM_CFG_REG_0_REG + 0x80 * i, o_sbm_cfg);
	}
}

/**
 * hns_dsaf_sbm_cfg_mib_en - config sbm
 * @dsaf_dev: dsa fabric id
 */
static int hns_dsaf_sbm_cfg_mib_en(struct dsaf_device *dsaf_dev)
{
	u32 sbm_cfg_mib_en;
	u32 i;
	u32 reg;
	u32 read_cnt;

	/* validate configure by setting SBM_CFG_MIB_EN bit from 0 to 1. */
	for (i = 0; i < HNS_DSAF_SBM_NUM(dsaf_dev); i++) {
		reg = DSAF_SBM_CFG_REG_0_REG + 0x80 * i;
		dsaf_set_dev_bit(dsaf_dev, reg, DSAF_SBM_CFG_MIB_EN_S, 0);
	}

	for (i = 0; i < HNS_DSAF_SBM_NUM(dsaf_dev); i++) {
		reg = DSAF_SBM_CFG_REG_0_REG + 0x80 * i;
		dsaf_set_dev_bit(dsaf_dev, reg, DSAF_SBM_CFG_MIB_EN_S, 1);
	}

	/* waitint for all sbm enable finished */
	for (i = 0; i < HNS_DSAF_SBM_NUM(dsaf_dev); i++) {
		read_cnt = 0;
		reg = DSAF_SBM_CFG_REG_0_REG + 0x80 * i;
		do {
			udelay(1);
			sbm_cfg_mib_en = dsaf_get_dev_bit(
					dsaf_dev, reg, DSAF_SBM_CFG_MIB_EN_S);
			read_cnt++;
		} while (sbm_cfg_mib_en == 0 &&
			read_cnt < DSAF_CFG_READ_CNT);

		if (sbm_cfg_mib_en == 0) {
			dev_err(dsaf_dev->dev,
				"sbm_cfg_mib_en fail,%s,sbm_num=%d\n",
				dsaf_dev->ae_dev.name, i);
			return -ENODEV;
		}
	}

	return 0;
}

/**
 * hns_dsaf_sbm_bp_wl_cfg - config sbm
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_sbm_bp_wl_cfg(struct dsaf_device *dsaf_dev)
{
	u32 o_sbm_bp_cfg;
	u32 reg;
	u32 i;

	/* XGE */
	for (i = 0; i < DSAF_XGE_NUM; i++) {
		reg = DSAF_SBM_BP_CFG_0_XGE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG0_COM_MAX_BUF_NUM_M,
			       DSAF_SBM_CFG0_COM_MAX_BUF_NUM_S, 512);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG0_VC0_MAX_BUF_NUM_M,
			       DSAF_SBM_CFG0_VC0_MAX_BUF_NUM_S, 0);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG0_VC1_MAX_BUF_NUM_M,
			       DSAF_SBM_CFG0_VC1_MAX_BUF_NUM_S, 0);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		reg = DSAF_SBM_BP_CFG_1_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG1_TC4_MAX_BUF_NUM_M,
			       DSAF_SBM_CFG1_TC4_MAX_BUF_NUM_S, 0);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG1_TC0_MAX_BUF_NUM_M,
			       DSAF_SBM_CFG1_TC0_MAX_BUF_NUM_S, 0);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		reg = DSAF_SBM_BP_CFG_2_XGE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG2_SET_BUF_NUM_M,
			       DSAF_SBM_CFG2_SET_BUF_NUM_S, 104);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG2_RESET_BUF_NUM_M,
			       DSAF_SBM_CFG2_RESET_BUF_NUM_S, 128);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		reg = DSAF_SBM_BP_CFG_3_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAF_SBM_CFG3_SET_BUF_NUM_NO_PFC_M,
			       DSAF_SBM_CFG3_SET_BUF_NUM_NO_PFC_S, 110);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAF_SBM_CFG3_RESET_BUF_NUM_NO_PFC_M,
			       DSAF_SBM_CFG3_RESET_BUF_NUM_NO_PFC_S, 160);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		/* for no enable pfc mode */
		reg = DSAF_SBM_BP_CFG_4_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAF_SBM_CFG3_SET_BUF_NUM_NO_PFC_M,
			       DSAF_SBM_CFG3_SET_BUF_NUM_NO_PFC_S, 128);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAF_SBM_CFG3_RESET_BUF_NUM_NO_PFC_M,
			       DSAF_SBM_CFG3_RESET_BUF_NUM_NO_PFC_S, 192);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);
	}

	/* PPE */
	for (i = 0; i < DSAF_COMM_CHN; i++) {
		reg = DSAF_SBM_BP_CFG_2_PPE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG2_SET_BUF_NUM_M,
			       DSAF_SBM_CFG2_SET_BUF_NUM_S, 10);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG2_RESET_BUF_NUM_M,
			       DSAF_SBM_CFG2_RESET_BUF_NUM_S, 12);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);
	}

	/* RoCEE */
	for (i = 0; i < DSAF_COMM_CHN; i++) {
		reg = DSAF_SBM_BP_CFG_2_ROCEE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG2_SET_BUF_NUM_M,
			       DSAF_SBM_CFG2_SET_BUF_NUM_S, 2);
		dsaf_set_field(o_sbm_bp_cfg, DSAF_SBM_CFG2_RESET_BUF_NUM_M,
			       DSAF_SBM_CFG2_RESET_BUF_NUM_S, 4);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);
	}
}

static void hns_dsafv2_sbm_bp_wl_cfg(struct dsaf_device *dsaf_dev)
{
	u32 o_sbm_bp_cfg;
	u32 reg;
	u32 i;

	/* XGE */
	for (i = 0; i < DSAFV2_SBM_XGE_CHN; i++) {
		reg = DSAF_SBM_BP_CFG_0_XGE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG0_COM_MAX_BUF_NUM_M,
			       DSAFV2_SBM_CFG0_COM_MAX_BUF_NUM_S, 256);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG0_VC0_MAX_BUF_NUM_M,
			       DSAFV2_SBM_CFG0_VC0_MAX_BUF_NUM_S, 0);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG0_VC1_MAX_BUF_NUM_M,
			       DSAFV2_SBM_CFG0_VC1_MAX_BUF_NUM_S, 0);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		reg = DSAF_SBM_BP_CFG_1_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG1_TC4_MAX_BUF_NUM_M,
			       DSAFV2_SBM_CFG1_TC4_MAX_BUF_NUM_S, 0);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG1_TC0_MAX_BUF_NUM_M,
			       DSAFV2_SBM_CFG1_TC0_MAX_BUF_NUM_S, 0);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		reg = DSAF_SBM_BP_CFG_2_XGE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG2_SET_BUF_NUM_M,
			       DSAFV2_SBM_CFG2_SET_BUF_NUM_S, 104);
		dsaf_set_field(o_sbm_bp_cfg, DSAFV2_SBM_CFG2_RESET_BUF_NUM_M,
			       DSAFV2_SBM_CFG2_RESET_BUF_NUM_S, 128);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		reg = DSAF_SBM_BP_CFG_3_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG3_SET_BUF_NUM_NO_PFC_M,
			       DSAFV2_SBM_CFG3_SET_BUF_NUM_NO_PFC_S, 55);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG3_RESET_BUF_NUM_NO_PFC_M,
			       DSAFV2_SBM_CFG3_RESET_BUF_NUM_NO_PFC_S, 110);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);

		/* for no enable pfc mode */
		reg = DSAF_SBM_BP_CFG_4_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG4_SET_BUF_NUM_NO_PFC_M,
			       DSAFV2_SBM_CFG4_SET_BUF_NUM_NO_PFC_S, 128);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG4_RESET_BUF_NUM_NO_PFC_M,
			       DSAFV2_SBM_CFG4_RESET_BUF_NUM_NO_PFC_S, 192);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);
	}

	/* PPE */
	for (i = 0; i < DSAFV2_SBM_PPE_CHN; i++) {
		reg = DSAF_SBM_BP_CFG_2_PPE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG2_PPE_SET_BUF_NUM_M,
			       DSAFV2_SBM_CFG2_PPE_SET_BUF_NUM_S, 2);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG2_PPE_RESET_BUF_NUM_M,
			       DSAFV2_SBM_CFG2_PPE_RESET_BUF_NUM_S, 3);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG2_PPE_CFG_USEFUL_NUM_M,
			       DSAFV2_SBM_CFG2_PPE_CFG_USEFUL_NUM_S, 52);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);
	}

	/* RoCEE */
	for (i = 0; i < DASFV2_ROCEE_CRD_NUM; i++) {
		reg = DSAFV2_SBM_BP_CFG_2_ROCEE_REG_0_REG + 0x80 * i;
		o_sbm_bp_cfg = dsaf_read_dev(dsaf_dev, reg);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG2_ROCEE_SET_BUF_NUM_M,
			       DSAFV2_SBM_CFG2_ROCEE_SET_BUF_NUM_S, 2);
		dsaf_set_field(o_sbm_bp_cfg,
			       DSAFV2_SBM_CFG2_ROCEE_RESET_BUF_NUM_M,
			       DSAFV2_SBM_CFG2_ROCEE_RESET_BUF_NUM_S, 4);
		dsaf_write_dev(dsaf_dev, reg, o_sbm_bp_cfg);
	}
}

/**
 * hns_dsaf_voq_bp_all_thrd_cfg -  voq
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_voq_bp_all_thrd_cfg(struct dsaf_device *dsaf_dev)
{
	u32 voq_bp_all_thrd;
	u32 i;

	for (i = 0; i < DSAF_VOQ_NUM; i++) {
		voq_bp_all_thrd = dsaf_read_dev(
			dsaf_dev, DSAF_VOQ_BP_ALL_THRD_0_REG + 0x40 * i);
		if (i < DSAF_XGE_NUM) {
			dsaf_set_field(voq_bp_all_thrd,
				       DSAF_VOQ_BP_ALL_DOWNTHRD_M,
				       DSAF_VOQ_BP_ALL_DOWNTHRD_S, 930);
			dsaf_set_field(voq_bp_all_thrd,
				       DSAF_VOQ_BP_ALL_UPTHRD_M,
				       DSAF_VOQ_BP_ALL_UPTHRD_S, 950);
		} else {
			dsaf_set_field(voq_bp_all_thrd,
				       DSAF_VOQ_BP_ALL_DOWNTHRD_M,
				       DSAF_VOQ_BP_ALL_DOWNTHRD_S, 220);
			dsaf_set_field(voq_bp_all_thrd,
				       DSAF_VOQ_BP_ALL_UPTHRD_M,
				       DSAF_VOQ_BP_ALL_UPTHRD_S, 230);
		}
		dsaf_write_dev(
			dsaf_dev, DSAF_VOQ_BP_ALL_THRD_0_REG + 0x40 * i,
			voq_bp_all_thrd);
	}
}

static void hns_dsaf_tbl_tcam_match_cfg(
	struct dsaf_device *dsaf_dev,
	struct dsaf_tbl_tcam_data *ptbl_tcam_data)
{
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MATCH_CFG_L_REG,
		       ptbl_tcam_data->tbl_tcam_data_low);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MATCH_CFG_H_REG,
		       ptbl_tcam_data->tbl_tcam_data_high);
}

/**
 * hns_dsaf_tbl_tcam_data_cfg - tbl
 * @dsaf_dev: dsa fabric id
 * @ptbl_tcam_data: addr
 */
static void hns_dsaf_tbl_tcam_data_cfg(
	struct dsaf_device *dsaf_dev,
	struct dsaf_tbl_tcam_data *ptbl_tcam_data)
{
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_LOW_0_REG,
		       ptbl_tcam_data->tbl_tcam_data_low);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_HIGH_0_REG,
		       ptbl_tcam_data->tbl_tcam_data_high);
}

/**
 * dsaf_tbl_tcam_mcast_cfg - tbl
 * @dsaf_dev: dsa fabric id
 * @mcast: addr
 */
static void hns_dsaf_tbl_tcam_mcast_cfg(
	struct dsaf_device *dsaf_dev,
	struct dsaf_tbl_tcam_mcast_cfg *mcast)
{
	u32 mcast_cfg4;

	mcast_cfg4 = dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_4_0_REG);
	dsaf_set_bit(mcast_cfg4, DSAF_TBL_MCAST_CFG4_ITEM_VLD_S,
		     mcast->tbl_mcast_item_vld);
	dsaf_set_bit(mcast_cfg4, DSAF_TBL_MCAST_CFG4_OLD_EN_S,
		     mcast->tbl_mcast_old_en);
	dsaf_set_field(mcast_cfg4, DSAF_TBL_MCAST_CFG4_VM128_112_M,
		       DSAF_TBL_MCAST_CFG4_VM128_112_S,
		       mcast->tbl_mcast_port_msk[4]);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_4_0_REG, mcast_cfg4);

	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_3_0_REG,
		       mcast->tbl_mcast_port_msk[3]);

	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_2_0_REG,
		       mcast->tbl_mcast_port_msk[2]);

	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_1_0_REG,
		       mcast->tbl_mcast_port_msk[1]);

	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_0_0_REG,
		       mcast->tbl_mcast_port_msk[0]);
}

/**
 * hns_dsaf_tbl_tcam_ucast_cfg - tbl
 * @dsaf_dev: dsa fabric id
 * @tbl_tcam_ucast: addr
 */
static void hns_dsaf_tbl_tcam_ucast_cfg(
	struct dsaf_device *dsaf_dev,
	struct dsaf_tbl_tcam_ucast_cfg *tbl_tcam_ucast)
{
	u32 ucast_cfg1;

	ucast_cfg1 = dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_UCAST_CFG_0_REG);
	dsaf_set_bit(ucast_cfg1, DSAF_TBL_UCAST_CFG1_MAC_DISCARD_S,
		     tbl_tcam_ucast->tbl_ucast_mac_discard);
	dsaf_set_bit(ucast_cfg1, DSAF_TBL_UCAST_CFG1_ITEM_VLD_S,
		     tbl_tcam_ucast->tbl_ucast_item_vld);
	dsaf_set_bit(ucast_cfg1, DSAF_TBL_UCAST_CFG1_OLD_EN_S,
		     tbl_tcam_ucast->tbl_ucast_old_en);
	dsaf_set_bit(ucast_cfg1, DSAF_TBL_UCAST_CFG1_DVC_S,
		     tbl_tcam_ucast->tbl_ucast_dvc);
	dsaf_set_field(ucast_cfg1, DSAF_TBL_UCAST_CFG1_OUT_PORT_M,
		       DSAF_TBL_UCAST_CFG1_OUT_PORT_S,
		       tbl_tcam_ucast->tbl_ucast_out_port);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_UCAST_CFG_0_REG, ucast_cfg1);
}

/**
 * hns_dsaf_tbl_line_cfg - tbl
 * @dsaf_dev: dsa fabric id
 * @tbl_lin: addr
 */
static void hns_dsaf_tbl_line_cfg(struct dsaf_device *dsaf_dev,
				  struct dsaf_tbl_line_cfg *tbl_lin)
{
	u32 tbl_line;

	tbl_line = dsaf_read_dev(dsaf_dev, DSAF_TBL_LIN_CFG_0_REG);
	dsaf_set_bit(tbl_line, DSAF_TBL_LINE_CFG_MAC_DISCARD_S,
		     tbl_lin->tbl_line_mac_discard);
	dsaf_set_bit(tbl_line, DSAF_TBL_LINE_CFG_DVC_S,
		     tbl_lin->tbl_line_dvc);
	dsaf_set_field(tbl_line, DSAF_TBL_LINE_CFG_OUT_PORT_M,
		       DSAF_TBL_LINE_CFG_OUT_PORT_S,
		       tbl_lin->tbl_line_out_port);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_LIN_CFG_0_REG, tbl_line);
}

/**
 * hns_dsaf_tbl_tcam_mcast_pul - tbl
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_tcam_mcast_pul(struct dsaf_device *dsaf_dev)
{
	u32 o_tbl_pul;

	o_tbl_pul = dsaf_read_dev(dsaf_dev, DSAF_TBL_PUL_0_REG);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_MCAST_VLD_S, 1);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, o_tbl_pul);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_MCAST_VLD_S, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, o_tbl_pul);
}

/**
 * hns_dsaf_tbl_line_pul - tbl
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_line_pul(struct dsaf_device *dsaf_dev)
{
	u32 tbl_pul;

	tbl_pul = dsaf_read_dev(dsaf_dev, DSAF_TBL_PUL_0_REG);
	dsaf_set_bit(tbl_pul, DSAF_TBL_PUL_LINE_VLD_S, 1);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, tbl_pul);
	dsaf_set_bit(tbl_pul, DSAF_TBL_PUL_LINE_VLD_S, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, tbl_pul);
}

/**
 * hns_dsaf_tbl_tcam_data_mcast_pul - tbl
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_tcam_data_mcast_pul(
	struct dsaf_device *dsaf_dev)
{
	u32 o_tbl_pul;

	o_tbl_pul = dsaf_read_dev(dsaf_dev, DSAF_TBL_PUL_0_REG);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_TCAM_DATA_VLD_S, 1);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_MCAST_VLD_S, 1);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, o_tbl_pul);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_TCAM_DATA_VLD_S, 0);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_MCAST_VLD_S, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, o_tbl_pul);
}

/**
 * hns_dsaf_tbl_tcam_data_ucast_pul - tbl
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_tcam_data_ucast_pul(
	struct dsaf_device *dsaf_dev)
{
	u32 o_tbl_pul;

	o_tbl_pul = dsaf_read_dev(dsaf_dev, DSAF_TBL_PUL_0_REG);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_TCAM_DATA_VLD_S, 1);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_UCAST_VLD_S, 1);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, o_tbl_pul);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_TCAM_DATA_VLD_S, 0);
	dsaf_set_bit(o_tbl_pul, DSAF_TBL_PUL_UCAST_VLD_S, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_PUL_0_REG, o_tbl_pul);
}

void hns_dsaf_set_promisc_mode(struct dsaf_device *dsaf_dev, u32 en)
{
	if (AE_IS_VER1(dsaf_dev->dsaf_ver) && !HNS_DSAF_IS_DEBUG(dsaf_dev))
		dsaf_set_dev_bit(dsaf_dev, DSAF_CFG_0_REG,
				 DSAF_CFG_MIX_MODE_S, !!en);
}

/**
 * hns_dsaf_tbl_stat_en - tbl
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_stat_en(struct dsaf_device *dsaf_dev)
{
	u32 o_tbl_ctrl;

	o_tbl_ctrl = dsaf_read_dev(dsaf_dev, DSAF_TBL_DFX_CTRL_0_REG);
	dsaf_set_bit(o_tbl_ctrl, DSAF_TBL_DFX_LINE_LKUP_NUM_EN_S, 1);
	dsaf_set_bit(o_tbl_ctrl, DSAF_TBL_DFX_UC_LKUP_NUM_EN_S, 1);
	dsaf_set_bit(o_tbl_ctrl, DSAF_TBL_DFX_MC_LKUP_NUM_EN_S, 1);
	dsaf_set_bit(o_tbl_ctrl, DSAF_TBL_DFX_BC_LKUP_NUM_EN_S, 1);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_DFX_CTRL_0_REG, o_tbl_ctrl);
}

/**
 * hns_dsaf_rocee_bp_en - rocee back press enable
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_rocee_bp_en(struct dsaf_device *dsaf_dev)
{
	if (AE_IS_VER1(dsaf_dev->dsaf_ver))
		dsaf_set_dev_bit(dsaf_dev, DSAF_XGE_CTRL_SIG_CFG_0_REG,
				 DSAF_FC_XGE_TX_PAUSE_S, 1);
}

/* set msk for dsaf exception irq*/
static void hns_dsaf_int_xge_msk_set(struct dsaf_device *dsaf_dev,
				     u32 chnn_num, u32 mask_set)
{
	dsaf_write_dev(dsaf_dev,
		       DSAF_XGE_INT_MSK_0_REG + 0x4 * chnn_num, mask_set);
}

static void hns_dsaf_int_ppe_msk_set(struct dsaf_device *dsaf_dev,
				     u32 chnn_num, u32 msk_set)
{
	dsaf_write_dev(dsaf_dev,
		       DSAF_PPE_INT_MSK_0_REG + 0x4 * chnn_num, msk_set);
}

static void hns_dsaf_int_rocee_msk_set(struct dsaf_device *dsaf_dev,
				       u32 chnn, u32 msk_set)
{
	dsaf_write_dev(dsaf_dev,
		       DSAF_ROCEE_INT_MSK_0_REG + 0x4 * chnn, msk_set);
}

static void
hns_dsaf_int_tbl_msk_set(struct dsaf_device *dsaf_dev, u32 msk_set)
{
	dsaf_write_dev(dsaf_dev, DSAF_TBL_INT_MSK_0_REG, msk_set);
}

/* clr dsaf exception irq*/
static void hns_dsaf_int_xge_src_clr(struct dsaf_device *dsaf_dev,
				     u32 chnn_num, u32 int_src)
{
	dsaf_write_dev(dsaf_dev,
		       DSAF_XGE_INT_SRC_0_REG + 0x4 * chnn_num, int_src);
}

static void hns_dsaf_int_ppe_src_clr(struct dsaf_device *dsaf_dev,
				     u32 chnn, u32 int_src)
{
	dsaf_write_dev(dsaf_dev,
		       DSAF_PPE_INT_SRC_0_REG + 0x4 * chnn, int_src);
}

static void hns_dsaf_int_rocee_src_clr(struct dsaf_device *dsaf_dev,
				       u32 chnn, u32 int_src)
{
	dsaf_write_dev(dsaf_dev,
		       DSAF_ROCEE_INT_SRC_0_REG + 0x4 * chnn, int_src);
}

static void hns_dsaf_int_tbl_src_clr(struct dsaf_device *dsaf_dev,
				     u32 int_src)
{
	dsaf_write_dev(dsaf_dev, DSAF_TBL_INT_SRC_0_REG, int_src);
}

/**
 * hns_dsaf_single_line_tbl_cfg - INT
 * @dsaf_dev: dsa fabric id
 * @address: the address
 * @ptbl_line: the line
 */
static void hns_dsaf_single_line_tbl_cfg(
	struct dsaf_device *dsaf_dev,
	u32 address, struct dsaf_tbl_line_cfg *ptbl_line)
{
	spin_lock_bh(&dsaf_dev->tcam_lock);

	/*Write Addr*/
	hns_dsaf_tbl_line_addr_cfg(dsaf_dev, address);

	/*Write Line*/
	hns_dsaf_tbl_line_cfg(dsaf_dev, ptbl_line);

	/*Write Plus*/
	hns_dsaf_tbl_line_pul(dsaf_dev);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tcam_uc_cfg - INT
 * @dsaf_dev: dsa fabric id
 * @address: the address
 * @ptbl_tcam_data: the data
 * @ptbl_tcam_ucast: unicast
 */
static void hns_dsaf_tcam_uc_cfg(
	struct dsaf_device *dsaf_dev, u32 address,
	struct dsaf_tbl_tcam_data *ptbl_tcam_data,
	struct dsaf_tbl_tcam_ucast_cfg *ptbl_tcam_ucast)
{
	spin_lock_bh(&dsaf_dev->tcam_lock);

	/*Write Addr*/
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);
	/*Write Tcam Data*/
	hns_dsaf_tbl_tcam_data_cfg(dsaf_dev, ptbl_tcam_data);
	/*Write Tcam Ucast*/
	hns_dsaf_tbl_tcam_ucast_cfg(dsaf_dev, ptbl_tcam_ucast);
	/*Write Plus*/
	hns_dsaf_tbl_tcam_data_ucast_pul(dsaf_dev);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tcam_mc_cfg - cfg the tcam for mc
 * @dsaf_dev: dsa fabric device struct pointer
 * @address: tcam index
 * @ptbl_tcam_data: tcam data struct pointer
 * @ptbl_tcam_mask: tcam mask struct pointer, it must be null for HNSv1
 * @ptbl_tcam_mcast: tcam data struct pointer
 */
static void hns_dsaf_tcam_mc_cfg(
	struct dsaf_device *dsaf_dev, u32 address,
	struct dsaf_tbl_tcam_data *ptbl_tcam_data,
	struct dsaf_tbl_tcam_data *ptbl_tcam_mask,
	struct dsaf_tbl_tcam_mcast_cfg *ptbl_tcam_mcast)
{
	spin_lock_bh(&dsaf_dev->tcam_lock);

	/*Write Addr*/
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);
	/*Write Tcam Data*/
	hns_dsaf_tbl_tcam_data_cfg(dsaf_dev, ptbl_tcam_data);
	/*Write Tcam Mcast*/
	hns_dsaf_tbl_tcam_mcast_cfg(dsaf_dev, ptbl_tcam_mcast);
	/* Write Match Data */
	if (ptbl_tcam_mask)
		hns_dsaf_tbl_tcam_match_cfg(dsaf_dev, ptbl_tcam_mask);

	/* Write Puls */
	hns_dsaf_tbl_tcam_data_mcast_pul(dsaf_dev);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tcam_uc_cfg_vague - INT
 * @dsaf_dev: dsa fabric device struct pointer
 * @address: the address
 * @tcam_data: the data
 * @tcam_mask: the mask
 * @tcam_uc: the unicast data
 */
static void hns_dsaf_tcam_uc_cfg_vague(struct dsaf_device *dsaf_dev,
				       u32 address,
				       struct dsaf_tbl_tcam_data *tcam_data,
				       struct dsaf_tbl_tcam_data *tcam_mask,
				       struct dsaf_tbl_tcam_ucast_cfg *tcam_uc)
{
	spin_lock_bh(&dsaf_dev->tcam_lock);
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);
	hns_dsaf_tbl_tcam_data_cfg(dsaf_dev, tcam_data);
	hns_dsaf_tbl_tcam_ucast_cfg(dsaf_dev, tcam_uc);
	hns_dsaf_tbl_tcam_match_cfg(dsaf_dev, tcam_mask);
	hns_dsaf_tbl_tcam_data_ucast_pul(dsaf_dev);

	/*Restore Match Data*/
	tcam_mask->tbl_tcam_data_high = 0xffffffff;
	tcam_mask->tbl_tcam_data_low = 0xffffffff;
	hns_dsaf_tbl_tcam_match_cfg(dsaf_dev, tcam_mask);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tcam_mc_cfg_vague - INT
 * @dsaf_dev: dsa fabric device struct pointer
 * @address: the address
 * @tcam_data: the data
 * @tcam_mask: the mask
 * @tcam_mc: the multicast data
 */
static void hns_dsaf_tcam_mc_cfg_vague(struct dsaf_device *dsaf_dev,
				       u32 address,
				       struct dsaf_tbl_tcam_data *tcam_data,
				       struct dsaf_tbl_tcam_data *tcam_mask,
				       struct dsaf_tbl_tcam_mcast_cfg *tcam_mc)
{
	spin_lock_bh(&dsaf_dev->tcam_lock);
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);
	hns_dsaf_tbl_tcam_data_cfg(dsaf_dev, tcam_data);
	hns_dsaf_tbl_tcam_mcast_cfg(dsaf_dev, tcam_mc);
	hns_dsaf_tbl_tcam_match_cfg(dsaf_dev, tcam_mask);
	hns_dsaf_tbl_tcam_data_mcast_pul(dsaf_dev);

	/*Restore Match Data*/
	tcam_mask->tbl_tcam_data_high = 0xffffffff;
	tcam_mask->tbl_tcam_data_low = 0xffffffff;
	hns_dsaf_tbl_tcam_match_cfg(dsaf_dev, tcam_mask);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tcam_mc_invld - INT
 * @dsaf_dev: dsa fabric id
 * @address: the address
 */
static void hns_dsaf_tcam_mc_invld(struct dsaf_device *dsaf_dev, u32 address)
{
	spin_lock_bh(&dsaf_dev->tcam_lock);

	/*Write Addr*/
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);

	/*write tcam mcast*/
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_0_0_REG, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_1_0_REG, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_2_0_REG, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_3_0_REG, 0);
	dsaf_write_dev(dsaf_dev, DSAF_TBL_TCAM_MCAST_CFG_4_0_REG, 0);

	/*Write Plus*/
	hns_dsaf_tbl_tcam_mcast_pul(dsaf_dev);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

static void
hns_dsaf_tcam_addr_get(struct dsaf_drv_tbl_tcam_key *mac_key, u8 *addr)
{
	addr[0] = mac_key->high.bits.mac_0;
	addr[1] = mac_key->high.bits.mac_1;
	addr[2] = mac_key->high.bits.mac_2;
	addr[3] = mac_key->high.bits.mac_3;
	addr[4] = mac_key->low.bits.mac_4;
	addr[5] = mac_key->low.bits.mac_5;
}

/**
 * hns_dsaf_tcam_uc_get - INT
 * @dsaf_dev: dsa fabric id
 * @address: the address
 * @ptbl_tcam_data: the data
 * @ptbl_tcam_ucast: unicast
 */
static void hns_dsaf_tcam_uc_get(
	struct dsaf_device *dsaf_dev, u32 address,
	struct dsaf_tbl_tcam_data *ptbl_tcam_data,
	struct dsaf_tbl_tcam_ucast_cfg *ptbl_tcam_ucast)
{
	u32 tcam_read_data0;
	u32 tcam_read_data4;

	spin_lock_bh(&dsaf_dev->tcam_lock);

	/*Write Addr*/
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);

	/*read tcam item puls*/
	hns_dsaf_tbl_tcam_load_pul(dsaf_dev);

	/*read tcam data*/
	ptbl_tcam_data->tbl_tcam_data_high
		= dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RDATA_HIGH_0_REG);
	ptbl_tcam_data->tbl_tcam_data_low
		= dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RDATA_LOW_0_REG);

	/*read tcam mcast*/
	tcam_read_data0 = dsaf_read_dev(dsaf_dev,
					DSAF_TBL_TCAM_RAM_RDATA0_0_REG);
	tcam_read_data4 = dsaf_read_dev(dsaf_dev,
					DSAF_TBL_TCAM_RAM_RDATA4_0_REG);

	ptbl_tcam_ucast->tbl_ucast_item_vld
		= dsaf_get_bit(tcam_read_data4,
			       DSAF_TBL_MCAST_CFG4_ITEM_VLD_S);
	ptbl_tcam_ucast->tbl_ucast_old_en
		= dsaf_get_bit(tcam_read_data4, DSAF_TBL_MCAST_CFG4_OLD_EN_S);
	ptbl_tcam_ucast->tbl_ucast_mac_discard
		= dsaf_get_bit(tcam_read_data0,
			       DSAF_TBL_UCAST_CFG1_MAC_DISCARD_S);
	ptbl_tcam_ucast->tbl_ucast_out_port
		= dsaf_get_field(tcam_read_data0,
				 DSAF_TBL_UCAST_CFG1_OUT_PORT_M,
				 DSAF_TBL_UCAST_CFG1_OUT_PORT_S);
	ptbl_tcam_ucast->tbl_ucast_dvc
		= dsaf_get_bit(tcam_read_data0, DSAF_TBL_UCAST_CFG1_DVC_S);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tcam_mc_get - INT
 * @dsaf_dev: dsa fabric id
 * @address: the address
 * @ptbl_tcam_data: the data
 * @ptbl_tcam_mcast: tcam multicast data
 */
static void hns_dsaf_tcam_mc_get(
	struct dsaf_device *dsaf_dev, u32 address,
	struct dsaf_tbl_tcam_data *ptbl_tcam_data,
	struct dsaf_tbl_tcam_mcast_cfg *ptbl_tcam_mcast)
{
	u32 data_tmp;

	spin_lock_bh(&dsaf_dev->tcam_lock);

	/*Write Addr*/
	hns_dsaf_tbl_tcam_addr_cfg(dsaf_dev, address);

	/*read tcam item puls*/
	hns_dsaf_tbl_tcam_load_pul(dsaf_dev);

	/*read tcam data*/
	ptbl_tcam_data->tbl_tcam_data_high =
		dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RDATA_HIGH_0_REG);
	ptbl_tcam_data->tbl_tcam_data_low =
		dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RDATA_LOW_0_REG);

	/*read tcam mcast*/
	ptbl_tcam_mcast->tbl_mcast_port_msk[0] =
		dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RAM_RDATA0_0_REG);
	ptbl_tcam_mcast->tbl_mcast_port_msk[1] =
		dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RAM_RDATA1_0_REG);
	ptbl_tcam_mcast->tbl_mcast_port_msk[2] =
		dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RAM_RDATA2_0_REG);
	ptbl_tcam_mcast->tbl_mcast_port_msk[3] =
		dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RAM_RDATA3_0_REG);

	data_tmp = dsaf_read_dev(dsaf_dev, DSAF_TBL_TCAM_RAM_RDATA4_0_REG);
	ptbl_tcam_mcast->tbl_mcast_item_vld =
		dsaf_get_bit(data_tmp, DSAF_TBL_MCAST_CFG4_ITEM_VLD_S);
	ptbl_tcam_mcast->tbl_mcast_old_en =
		dsaf_get_bit(data_tmp, DSAF_TBL_MCAST_CFG4_OLD_EN_S);
	ptbl_tcam_mcast->tbl_mcast_port_msk[4] =
		dsaf_get_field(data_tmp, DSAF_TBL_MCAST_CFG4_VM128_112_M,
			       DSAF_TBL_MCAST_CFG4_VM128_112_S);

	spin_unlock_bh(&dsaf_dev->tcam_lock);
}

/**
 * hns_dsaf_tbl_line_init - INT
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_line_init(struct dsaf_device *dsaf_dev)
{
	u32 i;
	/* defaultly set all lineal mac table entry resulting discard */
	struct dsaf_tbl_line_cfg tbl_line[] = {{1, 0, 0} };

	for (i = 0; i < DSAF_LINE_SUM; i++)
		hns_dsaf_single_line_tbl_cfg(dsaf_dev, i, tbl_line);
}

/**
 * hns_dsaf_tbl_tcam_init - INT
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_tcam_init(struct dsaf_device *dsaf_dev)
{
	u32 i;
	struct dsaf_tbl_tcam_data tcam_data[] = {{0, 0} };
	struct dsaf_tbl_tcam_ucast_cfg tcam_ucast[] = {{0, 0, 0, 0, 0} };

	/*tcam tbl*/
	for (i = 0; i < DSAF_TCAM_SUM; i++)
		hns_dsaf_tcam_uc_cfg(dsaf_dev, i, tcam_data, tcam_ucast);
}

/**
 * hns_dsaf_pfc_en_cfg - dsaf pfc pause cfg
 * @dsaf_dev: dsa fabric id
 * @mac_id: mac contrl block
 * @tc_en: traffic class
 */
static void hns_dsaf_pfc_en_cfg(struct dsaf_device *dsaf_dev,
				int mac_id, int tc_en)
{
	dsaf_write_dev(dsaf_dev, DSAF_PFC_EN_0_REG + mac_id * 4, tc_en);
}

static void hns_dsaf_set_pfc_pause(struct dsaf_device *dsaf_dev,
				   int mac_id, int tx_en, int rx_en)
{
	if (AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		if (!tx_en || !rx_en)
			dev_err(dsaf_dev->dev, "dsaf v1 can not close pfc!\n");

		return;
	}

	dsaf_set_dev_bit(dsaf_dev, DSAF_PAUSE_CFG_REG + mac_id * 4,
			 DSAF_PFC_PAUSE_RX_EN_B, !!rx_en);
	dsaf_set_dev_bit(dsaf_dev, DSAF_PAUSE_CFG_REG + mac_id * 4,
			 DSAF_PFC_PAUSE_TX_EN_B, !!tx_en);
}

int hns_dsaf_set_rx_mac_pause_en(struct dsaf_device *dsaf_dev, int mac_id,
				 u32 en)
{
	if (AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		if (!en) {
			dev_err(dsaf_dev->dev, "dsafv1 can't close rx_pause!\n");
			return -EINVAL;
		}
	}

	dsaf_set_dev_bit(dsaf_dev, DSAF_PAUSE_CFG_REG + mac_id * 4,
			 DSAF_MAC_PAUSE_RX_EN_B, !!en);

	return 0;
}

void hns_dsaf_get_rx_mac_pause_en(struct dsaf_device *dsaf_dev, int mac_id,
				  u32 *en)
{
	if (AE_IS_VER1(dsaf_dev->dsaf_ver))
		*en = 1;
	else
		*en = dsaf_get_dev_bit(dsaf_dev,
				       DSAF_PAUSE_CFG_REG + mac_id * 4,
				       DSAF_MAC_PAUSE_RX_EN_B);
}

/**
 * hns_dsaf_tbl_tcam_init - INT
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_comm_init(struct dsaf_device *dsaf_dev)
{
	u32 i;
	u32 o_dsaf_cfg;
	bool is_ver1 = AE_IS_VER1(dsaf_dev->dsaf_ver);

	o_dsaf_cfg = dsaf_read_dev(dsaf_dev, DSAF_CFG_0_REG);
	dsaf_set_bit(o_dsaf_cfg, DSAF_CFG_EN_S, dsaf_dev->dsaf_en);
	dsaf_set_bit(o_dsaf_cfg, DSAF_CFG_TC_MODE_S, dsaf_dev->dsaf_tc_mode);
	dsaf_set_bit(o_dsaf_cfg, DSAF_CFG_CRC_EN_S, 0);
	dsaf_set_bit(o_dsaf_cfg, DSAF_CFG_MIX_MODE_S, 0);
	dsaf_set_bit(o_dsaf_cfg, DSAF_CFG_LOCA_ADDR_EN_S, 0);
	dsaf_write_dev(dsaf_dev, DSAF_CFG_0_REG, o_dsaf_cfg);

	hns_dsaf_reg_cnt_clr_ce(dsaf_dev, 1);
	hns_dsaf_stp_port_type_cfg(dsaf_dev, DSAF_STP_PORT_TYPE_FORWARD);

	/* set 22 queue per tx ppe engine, only used in switch mode */
	hns_dsaf_ppe_qid_cfg(dsaf_dev, DSAF_DEFAUTL_QUEUE_NUM_PER_PPE);

	/* set promisc def queue id */
	hns_dsaf_mix_def_qid_cfg(dsaf_dev);

	/* set inner loopback queue id */
	hns_dsaf_inner_qid_cfg(dsaf_dev);

	/* in non switch mode, set all port to access mode */
	hns_dsaf_sw_port_type_cfg(dsaf_dev, DSAF_SW_PORT_TYPE_NON_VLAN);

	/*set dsaf pfc  to 0 for parseing rx pause*/
	for (i = 0; i < DSAF_COMM_CHN; i++) {
		hns_dsaf_pfc_en_cfg(dsaf_dev, i, 0);
		hns_dsaf_set_pfc_pause(dsaf_dev, i, is_ver1, is_ver1);
	}

	/*msk and  clr exception irqs */
	for (i = 0; i < DSAF_COMM_CHN; i++) {
		hns_dsaf_int_xge_src_clr(dsaf_dev, i, 0xfffffffful);
		hns_dsaf_int_ppe_src_clr(dsaf_dev, i, 0xfffffffful);
		hns_dsaf_int_rocee_src_clr(dsaf_dev, i, 0xfffffffful);

		hns_dsaf_int_xge_msk_set(dsaf_dev, i, 0xfffffffful);
		hns_dsaf_int_ppe_msk_set(dsaf_dev, i, 0xfffffffful);
		hns_dsaf_int_rocee_msk_set(dsaf_dev, i, 0xfffffffful);
	}
	hns_dsaf_int_tbl_src_clr(dsaf_dev, 0xfffffffful);
	hns_dsaf_int_tbl_msk_set(dsaf_dev, 0xfffffffful);
}

/**
 * hns_dsaf_inode_init - INT
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_inode_init(struct dsaf_device *dsaf_dev)
{
	u32 reg;
	u32 tc_cfg;
	u32 i;

	if (dsaf_dev->dsaf_tc_mode == HRD_DSAF_4TC_MODE)
		tc_cfg = HNS_DSAF_I4TC_CFG;
	else
		tc_cfg = HNS_DSAF_I8TC_CFG;

	if (AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		for (i = 0; i < DSAF_INODE_NUM; i++) {
			reg = DSAF_INODE_IN_PORT_NUM_0_REG + 0x80 * i;
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAF_INODE_IN_PORT_NUM_M,
					   DSAF_INODE_IN_PORT_NUM_S,
					   i % DSAF_XGE_NUM);
		}
	} else {
		for (i = 0; i < DSAF_PORT_TYPE_NUM; i++) {
			reg = DSAF_INODE_IN_PORT_NUM_0_REG + 0x80 * i;
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAF_INODE_IN_PORT_NUM_M,
					   DSAF_INODE_IN_PORT_NUM_S, 0);
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAFV2_INODE_IN_PORT1_NUM_M,
					   DSAFV2_INODE_IN_PORT1_NUM_S, 1);
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAFV2_INODE_IN_PORT2_NUM_M,
					   DSAFV2_INODE_IN_PORT2_NUM_S, 2);
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAFV2_INODE_IN_PORT3_NUM_M,
					   DSAFV2_INODE_IN_PORT3_NUM_S, 3);
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAFV2_INODE_IN_PORT4_NUM_M,
					   DSAFV2_INODE_IN_PORT4_NUM_S, 4);
			dsaf_set_dev_field(dsaf_dev, reg,
					   DSAFV2_INODE_IN_PORT5_NUM_M,
					   DSAFV2_INODE_IN_PORT5_NUM_S, 5);
		}
	}
	for (i = 0; i < DSAF_INODE_NUM; i++) {
		reg = DSAF_INODE_PRI_TC_CFG_0_REG + 0x80 * i;
		dsaf_write_dev(dsaf_dev, reg, tc_cfg);
	}
}

/**
 * hns_dsaf_sbm_init - INT
 * @dsaf_dev: dsa fabric id
 */
static int hns_dsaf_sbm_init(struct dsaf_device *dsaf_dev)
{
	u32 flag;
	u32 finish_msk;
	u32 cnt = 0;
	int ret;

	if (AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		hns_dsaf_sbm_bp_wl_cfg(dsaf_dev);
		finish_msk = DSAF_SRAM_INIT_OVER_M;
	} else {
		hns_dsafv2_sbm_bp_wl_cfg(dsaf_dev);
		finish_msk = DSAFV2_SRAM_INIT_OVER_M;
	}

	/* enable sbm chanel, disable sbm chanel shcut function*/
	hns_dsaf_sbm_cfg(dsaf_dev);

	/* enable sbm mib */
	ret = hns_dsaf_sbm_cfg_mib_en(dsaf_dev);
	if (ret) {
		dev_err(dsaf_dev->dev,
			"hns_dsaf_sbm_cfg_mib_en fail,%s, ret=%d\n",
			dsaf_dev->ae_dev.name, ret);
		return ret;
	}

	/* enable sbm initial link sram */
	hns_dsaf_sbm_link_sram_init_en(dsaf_dev);

	do {
		usleep_range(200, 210);/*udelay(200);*/
		flag = dsaf_get_dev_field(dsaf_dev, DSAF_SRAM_INIT_OVER_0_REG,
					  finish_msk, DSAF_SRAM_INIT_OVER_S);
		cnt++;
	} while (flag != (finish_msk >> DSAF_SRAM_INIT_OVER_S) &&
		 cnt < DSAF_CFG_READ_CNT);

	if (flag != (finish_msk >> DSAF_SRAM_INIT_OVER_S)) {
		dev_err(dsaf_dev->dev,
			"hns_dsaf_sbm_init fail %s, flag=%d, cnt=%d\n",
			dsaf_dev->ae_dev.name, flag, cnt);
		return -ENODEV;
	}

	hns_dsaf_rocee_bp_en(dsaf_dev);

	return 0;
}

/**
 * hns_dsaf_tbl_init - INT
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_tbl_init(struct dsaf_device *dsaf_dev)
{
	hns_dsaf_tbl_stat_en(dsaf_dev);

	hns_dsaf_tbl_tcam_init(dsaf_dev);
	hns_dsaf_tbl_line_init(dsaf_dev);
}

/**
 * hns_dsaf_voq_init - INT
 * @dsaf_dev: dsa fabric id
 */
static void hns_dsaf_voq_init(struct dsaf_device *dsaf_dev)
{
	hns_dsaf_voq_bp_all_thrd_cfg(dsaf_dev);
}

/**
 * hns_dsaf_init_hw - init dsa fabric hardware
 * @dsaf_dev: dsa fabric device struct pointer
 */
static int hns_dsaf_init_hw(struct dsaf_device *dsaf_dev)
{
	int ret;

	dev_dbg(dsaf_dev->dev,
		"hns_dsaf_init_hw begin %s !\n", dsaf_dev->ae_dev.name);

	dsaf_dev->misc_op->dsaf_reset(dsaf_dev, 0);
	mdelay(10);
	dsaf_dev->misc_op->dsaf_reset(dsaf_dev, 1);

	hns_dsaf_comm_init(dsaf_dev);

	/*init XBAR_INODE*/
	hns_dsaf_inode_init(dsaf_dev);

	/*init SBM*/
	ret = hns_dsaf_sbm_init(dsaf_dev);
	if (ret)
		return ret;

	/*init TBL*/
	hns_dsaf_tbl_init(dsaf_dev);

	/*init VOQ*/
	hns_dsaf_voq_init(dsaf_dev);

	return 0;
}

/**
 * hns_dsaf_remove_hw - uninit dsa fabric hardware
 * @dsaf_dev: dsa fabric device struct pointer
 */
static void hns_dsaf_remove_hw(struct dsaf_device *dsaf_dev)
{
	/*reset*/
	dsaf_dev->misc_op->dsaf_reset(dsaf_dev, 0);
}

/**
 * hns_dsaf_init - init dsa fabric
 * @dsaf_dev: dsa fabric device struct pointer
 * return 0 - success , negative --fail
 */
static int hns_dsaf_init(struct dsaf_device *dsaf_dev)
{
	struct dsaf_drv_priv *priv =
	    (struct dsaf_drv_priv *)hns_dsaf_dev_priv(dsaf_dev);
	u32 i;
	int ret;

	if (HNS_DSAF_IS_DEBUG(dsaf_dev))
		return 0;

	if (AE_IS_VER1(dsaf_dev->dsaf_ver))
		dsaf_dev->tcam_max_num = DSAF_TCAM_SUM;
	else
		dsaf_dev->tcam_max_num =
			DSAF_TCAM_SUM - DSAFV2_MAC_FUZZY_TCAM_NUM;

	spin_lock_init(&dsaf_dev->tcam_lock);
	ret = hns_dsaf_init_hw(dsaf_dev);
	if (ret)
		return ret;

	/* malloc mem for tcam mac key(vlan+mac) */
	priv->soft_mac_tbl = vzalloc(array_size(DSAF_TCAM_SUM,
						sizeof(*priv->soft_mac_tbl)));
	if (!priv->soft_mac_tbl) {
		ret = -ENOMEM;
		goto remove_hw;
	}

	/*all entry invall */
	for (i = 0; i < DSAF_TCAM_SUM; i++)
		(priv->soft_mac_tbl + i)->index = DSAF_INVALID_ENTRY_IDX;

	return 0;

remove_hw:
	hns_dsaf_remove_hw(dsaf_dev);
	return ret;
}

/**
 * hns_dsaf_free - free dsa fabric
 * @dsaf_dev: dsa fabric device struct pointer
 */
static void hns_dsaf_free(struct dsaf_device *dsaf_dev)
{
	struct dsaf_drv_priv *priv =
	    (struct dsaf_drv_priv *)hns_dsaf_dev_priv(dsaf_dev);

	hns_dsaf_remove_hw(dsaf_dev);

	/* free all mac mem */
	vfree(priv->soft_mac_tbl);
	priv->soft_mac_tbl = NULL;
}

/**
 * hns_dsaf_find_soft_mac_entry - find dsa fabric soft entry
 * @dsaf_dev: dsa fabric device struct pointer
 * @mac_key: mac entry struct pointer
 */
static u16 hns_dsaf_find_soft_mac_entry(
	struct dsaf_device *dsaf_dev,
	struct dsaf_drv_tbl_tcam_key *mac_key)
{
	struct dsaf_drv_priv *priv =
	    (struct dsaf_drv_priv *)hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry;
	u32 i;

	soft_mac_entry = priv->soft_mac_tbl;
	for (i = 0; i < dsaf_dev->tcam_max_num; i++) {
		/* invall tab entry */
		if ((soft_mac_entry->index != DSAF_INVALID_ENTRY_IDX) &&
		    (soft_mac_entry->tcam_key.high.val == mac_key->high.val) &&
		    (soft_mac_entry->tcam_key.low.val == mac_key->low.val))
			/* return find result --soft index */
			return soft_mac_entry->index;

		soft_mac_entry++;
	}
	return DSAF_INVALID_ENTRY_IDX;
}

/**
 * hns_dsaf_find_empty_mac_entry - search dsa fabric soft empty-entry
 * @dsaf_dev: dsa fabric device struct pointer
 */
static u16 hns_dsaf_find_empty_mac_entry(struct dsaf_device *dsaf_dev)
{
	struct dsaf_drv_priv *priv =
	    (struct dsaf_drv_priv *)hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry;
	u32 i;

	soft_mac_entry = priv->soft_mac_tbl;
	for (i = 0; i < dsaf_dev->tcam_max_num; i++) {
		/* inv all entry */
		if (soft_mac_entry->index == DSAF_INVALID_ENTRY_IDX)
			/* return find result --soft index */
			return i;

		soft_mac_entry++;
	}
	return DSAF_INVALID_ENTRY_IDX;
}

/**
 * hns_dsaf_find_empty_mac_entry_reverse
 * search dsa fabric soft empty-entry from the end
 * @dsaf_dev: dsa fabric device struct pointer
 */
static u16 hns_dsaf_find_empty_mac_entry_reverse(struct dsaf_device *dsaf_dev)
{
	struct dsaf_drv_priv *priv = hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry;
	int i;

	soft_mac_entry = priv->soft_mac_tbl + (DSAF_TCAM_SUM - 1);
	for (i = (DSAF_TCAM_SUM - 1); i > 0; i--) {
		/* search all entry from end to start.*/
		if (soft_mac_entry->index == DSAF_INVALID_ENTRY_IDX)
			return i;
		soft_mac_entry--;
	}
	return DSAF_INVALID_ENTRY_IDX;
}

/**
 * hns_dsaf_set_mac_key - set mac key
 * @dsaf_dev: dsa fabric device struct pointer
 * @mac_key: tcam key pointer
 * @vlan_id: vlan id
 * @in_port_num: input port num
 * @addr: mac addr
 */
static void hns_dsaf_set_mac_key(
	struct dsaf_device *dsaf_dev,
	struct dsaf_drv_tbl_tcam_key *mac_key, u16 vlan_id, u8 in_port_num,
	u8 *addr)
{
	u8 port;

	if (dsaf_dev->dsaf_mode <= DSAF_MODE_ENABLE)
		/*DSAF mode : in port id fixed 0*/
		port = 0;
	else
		/*non-dsaf mode*/
		port = in_port_num;

	mac_key->high.bits.mac_0 = addr[0];
	mac_key->high.bits.mac_1 = addr[1];
	mac_key->high.bits.mac_2 = addr[2];
	mac_key->high.bits.mac_3 = addr[3];
	mac_key->low.bits.mac_4 = addr[4];
	mac_key->low.bits.mac_5 = addr[5];
	mac_key->low.bits.port_vlan = 0;
	dsaf_set_field(mac_key->low.bits.port_vlan, DSAF_TBL_TCAM_KEY_VLAN_M,
		       DSAF_TBL_TCAM_KEY_VLAN_S, vlan_id);
	dsaf_set_field(mac_key->low.bits.port_vlan, DSAF_TBL_TCAM_KEY_PORT_M,
		       DSAF_TBL_TCAM_KEY_PORT_S, port);
}

/**
 * hns_dsaf_set_mac_uc_entry - set mac uc-entry
 * @dsaf_dev: dsa fabric device struct pointer
 * @mac_entry: uc-mac entry
 */
int hns_dsaf_set_mac_uc_entry(
	struct dsaf_device *dsaf_dev,
	struct dsaf_drv_mac_single_dest_entry *mac_entry)
{
	u16 entry_index;
	struct dsaf_drv_tbl_tcam_key mac_key;
	struct dsaf_tbl_tcam_ucast_cfg mac_data;
	struct dsaf_drv_priv *priv =
	    (struct dsaf_drv_priv *)hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry = priv->soft_mac_tbl;
	struct dsaf_tbl_tcam_data tcam_data;

	/* mac addr check */
	if (MAC_IS_ALL_ZEROS(mac_entry->addr) ||
	    MAC_IS_BROADCAST(mac_entry->addr) ||
	    MAC_IS_MULTICAST(mac_entry->addr)) {
		dev_err(dsaf_dev->dev, "set_uc %s Mac %pM err!\n",
			dsaf_dev->ae_dev.name, mac_entry->addr);
		return -EINVAL;
	}

	/* config key */
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, mac_entry->in_vlan_id,
			     mac_entry->in_port_num, mac_entry->addr);

	/* entry ie exist? */
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		/*if has not inv entry,find a empty entry */
		entry_index = hns_dsaf_find_empty_mac_entry(dsaf_dev);
		if (entry_index == DSAF_INVALID_ENTRY_IDX) {
			/* has not empty,return error */
			dev_err(dsaf_dev->dev,
				"set_uc_entry failed, %s Mac key(%#x:%#x)\n",
				dsaf_dev->ae_dev.name,
				mac_key.high.val, mac_key.low.val);
			return -EINVAL;
		}
	}

	dev_dbg(dsaf_dev->dev,
		"set_uc_entry, %s Mac key(%#x:%#x) entry_index%d\n",
		dsaf_dev->ae_dev.name, mac_key.high.val,
		mac_key.low.val, entry_index);

	/* config hardware entry */
	mac_data.tbl_ucast_item_vld = 1;
	mac_data.tbl_ucast_mac_discard = 0;
	mac_data.tbl_ucast_old_en = 0;
	/* default config dvc to 0 */
	mac_data.tbl_ucast_dvc = 0;
	mac_data.tbl_ucast_out_port = mac_entry->port_num;
	tcam_data.tbl_tcam_data_high = mac_key.high.val;
	tcam_data.tbl_tcam_data_low = mac_key.low.val;

	hns_dsaf_tcam_uc_cfg(dsaf_dev, entry_index, &tcam_data, &mac_data);

	/* config software entry */
	soft_mac_entry += entry_index;
	soft_mac_entry->index = entry_index;
	soft_mac_entry->tcam_key.high.val = mac_key.high.val;
	soft_mac_entry->tcam_key.low.val = mac_key.low.val;

	return 0;
}

int hns_dsaf_rm_mac_addr(
	struct dsaf_device *dsaf_dev,
	struct dsaf_drv_mac_single_dest_entry *mac_entry)
{
	u16 entry_index;
	struct dsaf_tbl_tcam_ucast_cfg mac_data;
	struct dsaf_drv_tbl_tcam_key mac_key;

	/* mac addr check */
	if (!is_valid_ether_addr(mac_entry->addr)) {
		dev_err(dsaf_dev->dev, "rm_uc_addr %s Mac %pM err!\n",
			dsaf_dev->ae_dev.name, mac_entry->addr);
		return -EINVAL;
	}

	/* config key */
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, mac_entry->in_vlan_id,
			     mac_entry->in_port_num, mac_entry->addr);

	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		/* can not find the tcam entry, return 0 */
		dev_info(dsaf_dev->dev,
			 "rm_uc_addr no tcam, %s Mac key(%#x:%#x)\n",
			 dsaf_dev->ae_dev.name,
			 mac_key.high.val, mac_key.low.val);
		return 0;
	}

	dev_dbg(dsaf_dev->dev,
		"rm_uc_addr, %s Mac key(%#x:%#x) entry_index%d\n",
		dsaf_dev->ae_dev.name, mac_key.high.val,
		mac_key.low.val, entry_index);

	hns_dsaf_tcam_uc_get(
			dsaf_dev, entry_index,
			(struct dsaf_tbl_tcam_data *)&mac_key,
			&mac_data);

	/* unicast entry not used locally should not clear */
	if (mac_entry->port_num != mac_data.tbl_ucast_out_port)
		return -EFAULT;

	return hns_dsaf_del_mac_entry(dsaf_dev,
				      mac_entry->in_vlan_id,
				      mac_entry->in_port_num,
				      mac_entry->addr);
}

static void hns_dsaf_setup_mc_mask(struct dsaf_device *dsaf_dev,
				   u8 port_num, u8 *mask, u8 *addr)
{
	if (MAC_IS_BROADCAST(addr))
		eth_broadcast_addr(mask);
	else
		memcpy(mask, dsaf_dev->mac_cb[port_num]->mc_mask, ETH_ALEN);
}

static void hns_dsaf_mc_mask_bit_clear(char *dst, const char *src)
{
	u16 *a = (u16 *)dst;
	const u16 *b = (const u16 *)src;

	a[0] &= b[0];
	a[1] &= b[1];
	a[2] &= b[2];
}

/**
 * hns_dsaf_add_mac_mc_port - add mac mc-port
 * @dsaf_dev: dsa fabric device struct pointer
 * @mac_entry: mc-mac entry
 */
int hns_dsaf_add_mac_mc_port(struct dsaf_device *dsaf_dev,
			     struct dsaf_drv_mac_single_dest_entry *mac_entry)
{
	u16 entry_index;
	struct dsaf_drv_tbl_tcam_key mac_key;
	struct dsaf_drv_tbl_tcam_key mask_key;
	struct dsaf_tbl_tcam_data *pmask_key = NULL;
	struct dsaf_tbl_tcam_mcast_cfg mac_data;
	struct dsaf_drv_priv *priv = hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry = priv->soft_mac_tbl;
	struct dsaf_tbl_tcam_data tcam_data;
	u8 mc_addr[ETH_ALEN];
	int mskid;

	/*chechk mac addr */
	if (MAC_IS_ALL_ZEROS(mac_entry->addr)) {
		dev_err(dsaf_dev->dev, "set_entry failed,addr %pM!\n",
			mac_entry->addr);
		return -EINVAL;
	}

	ether_addr_copy(mc_addr, mac_entry->addr);
	if (!AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		u8 mc_mask[ETH_ALEN];

		/* prepare for key data setting */
		hns_dsaf_setup_mc_mask(dsaf_dev, mac_entry->in_port_num,
				       mc_mask, mac_entry->addr);
		hns_dsaf_mc_mask_bit_clear(mc_addr, mc_mask);

		/* config key mask */
		hns_dsaf_set_mac_key(dsaf_dev, &mask_key,
				     0x0,
				     0xff,
				     mc_mask);

		pmask_key = (struct dsaf_tbl_tcam_data *)(&mask_key);
	}

	/*config key */
	hns_dsaf_set_mac_key(
		dsaf_dev, &mac_key, mac_entry->in_vlan_id,
		mac_entry->in_port_num, mc_addr);

	memset(&mac_data, 0, sizeof(struct dsaf_tbl_tcam_mcast_cfg));

	/* check if the tcam is exist */
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		/*if hasnot , find a empty*/
		entry_index = hns_dsaf_find_empty_mac_entry(dsaf_dev);
		if (entry_index == DSAF_INVALID_ENTRY_IDX) {
			/*if hasnot empty, error*/
			dev_err(dsaf_dev->dev,
				"set_uc_entry failed, %s Mac key(%#x:%#x)\n",
				dsaf_dev->ae_dev.name, mac_key.high.val,
				mac_key.low.val);
			return -EINVAL;
		}
	} else {
		/* if exist, add in */
		hns_dsaf_tcam_mc_get(dsaf_dev, entry_index, &tcam_data,
				     &mac_data);
	}

	/* config hardware entry */
	if (mac_entry->port_num < DSAF_SERVICE_NW_NUM) {
		mskid = mac_entry->port_num;
	} else if (mac_entry->port_num >= DSAF_BASE_INNER_PORT_NUM) {
		mskid = mac_entry->port_num -
			DSAF_BASE_INNER_PORT_NUM + DSAF_SERVICE_NW_NUM;
	} else {
		dev_err(dsaf_dev->dev,
			"%s,pnum(%d)error,key(%#x:%#x)\n",
			dsaf_dev->ae_dev.name, mac_entry->port_num,
			mac_key.high.val, mac_key.low.val);
		return -EINVAL;
	}
	dsaf_set_bit(mac_data.tbl_mcast_port_msk[mskid / 32], mskid % 32, 1);
	mac_data.tbl_mcast_old_en = 0;
	mac_data.tbl_mcast_item_vld = 1;

	dev_dbg(dsaf_dev->dev,
		"set_uc_entry, %s Mac key(%#x:%#x) entry_index%d\n",
		dsaf_dev->ae_dev.name, mac_key.high.val,
		mac_key.low.val, entry_index);

	tcam_data.tbl_tcam_data_high = mac_key.high.val;
	tcam_data.tbl_tcam_data_low = mac_key.low.val;

	/* config mc entry with mask */
	hns_dsaf_tcam_mc_cfg(dsaf_dev, entry_index, &tcam_data,
			     pmask_key, &mac_data);

	/*config software entry */
	soft_mac_entry += entry_index;
	soft_mac_entry->index = entry_index;
	soft_mac_entry->tcam_key.high.val = mac_key.high.val;
	soft_mac_entry->tcam_key.low.val = mac_key.low.val;

	return 0;
}

/**
 * hns_dsaf_del_mac_entry - del mac mc-port
 * @dsaf_dev: dsa fabric device struct pointer
 * @vlan_id: vlian id
 * @in_port_num: input port num
 * @addr : mac addr
 */
int hns_dsaf_del_mac_entry(struct dsaf_device *dsaf_dev, u16 vlan_id,
			   u8 in_port_num, u8 *addr)
{
	u16 entry_index;
	struct dsaf_drv_tbl_tcam_key mac_key;
	struct dsaf_drv_priv *priv =
	    (struct dsaf_drv_priv *)hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry = priv->soft_mac_tbl;

	/*check mac addr */
	if (MAC_IS_ALL_ZEROS(addr) || MAC_IS_BROADCAST(addr)) {
		dev_err(dsaf_dev->dev, "del_entry failed,addr %pM!\n",
			addr);
		return -EINVAL;
	}

	/*config key */
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, vlan_id, in_port_num, addr);

	/*exist ?*/
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		/*not exist, error */
		dev_err(dsaf_dev->dev,
			"del_mac_entry failed, %s Mac key(%#x:%#x)\n",
			dsaf_dev->ae_dev.name,
			mac_key.high.val, mac_key.low.val);
		return -EINVAL;
	}
	dev_dbg(dsaf_dev->dev,
		"del_mac_entry, %s Mac key(%#x:%#x) entry_index%d\n",
		dsaf_dev->ae_dev.name, mac_key.high.val,
		mac_key.low.val, entry_index);

	/*do del opt*/
	hns_dsaf_tcam_mc_invld(dsaf_dev, entry_index);

	/*del soft emtry */
	soft_mac_entry += entry_index;
	soft_mac_entry->index = DSAF_INVALID_ENTRY_IDX;

	return 0;
}

/**
 * hns_dsaf_del_mac_mc_port - del mac mc- port
 * @dsaf_dev: dsa fabric device struct pointer
 * @mac_entry: mac entry
 */
int hns_dsaf_del_mac_mc_port(struct dsaf_device *dsaf_dev,
			     struct dsaf_drv_mac_single_dest_entry *mac_entry)
{
	u16 entry_index;
	struct dsaf_drv_tbl_tcam_key mac_key;
	struct dsaf_drv_priv *priv = hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry = priv->soft_mac_tbl;
	u16 vlan_id;
	u8 in_port_num;
	struct dsaf_tbl_tcam_mcast_cfg mac_data;
	struct dsaf_tbl_tcam_data tcam_data;
	int mskid;
	const u8 empty_msk[sizeof(mac_data.tbl_mcast_port_msk)] = {0};
	struct dsaf_drv_tbl_tcam_key mask_key;
	struct dsaf_tbl_tcam_data *pmask_key = NULL;
	u8 mc_addr[ETH_ALEN];

	if (!(void *)mac_entry) {
		dev_err(dsaf_dev->dev,
			"hns_dsaf_del_mac_mc_port mac_entry is NULL\n");
		return -EINVAL;
	}

	/*check mac addr */
	if (MAC_IS_ALL_ZEROS(mac_entry->addr)) {
		dev_err(dsaf_dev->dev, "del_port failed, addr %pM!\n",
			mac_entry->addr);
		return -EINVAL;
	}

	/* always mask vlan_id field */
	ether_addr_copy(mc_addr, mac_entry->addr);

	if (!AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		u8 mc_mask[ETH_ALEN];

		/* prepare for key data setting */
		hns_dsaf_setup_mc_mask(dsaf_dev, mac_entry->in_port_num,
				       mc_mask, mac_entry->addr);
		hns_dsaf_mc_mask_bit_clear(mc_addr, mc_mask);

		/* config key mask */
		hns_dsaf_set_mac_key(dsaf_dev, &mask_key, 0x00, 0xff, mc_mask);

		pmask_key = (struct dsaf_tbl_tcam_data *)(&mask_key);
	}

	/* get key info */
	vlan_id = mac_entry->in_vlan_id;
	in_port_num = mac_entry->in_port_num;

	/* config key */
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, vlan_id, in_port_num, mc_addr);

	/* check if the tcam entry is exist */
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		/*find none */
		dev_err(dsaf_dev->dev,
			"find_soft_mac_entry failed, %s Mac key(%#x:%#x)\n",
			dsaf_dev->ae_dev.name,
			mac_key.high.val, mac_key.low.val);
		return -EINVAL;
	}

	dev_dbg(dsaf_dev->dev,
		"del_mac_mc_port, %s key(%#x:%#x) index%d\n",
		dsaf_dev->ae_dev.name, mac_key.high.val,
		mac_key.low.val, entry_index);

	/* read entry */
	hns_dsaf_tcam_mc_get(dsaf_dev, entry_index, &tcam_data, &mac_data);

	/*del the port*/
	if (mac_entry->port_num < DSAF_SERVICE_NW_NUM) {
		mskid = mac_entry->port_num;
	} else if (mac_entry->port_num >= DSAF_BASE_INNER_PORT_NUM) {
		mskid = mac_entry->port_num -
			DSAF_BASE_INNER_PORT_NUM + DSAF_SERVICE_NW_NUM;
	} else {
		dev_err(dsaf_dev->dev,
			"%s,pnum(%d)error,key(%#x:%#x)\n",
			dsaf_dev->ae_dev.name, mac_entry->port_num,
			mac_key.high.val, mac_key.low.val);
		return -EINVAL;
	}
	dsaf_set_bit(mac_data.tbl_mcast_port_msk[mskid / 32], mskid % 32, 0);

	/*check non port, do del entry */
	if (!memcmp(mac_data.tbl_mcast_port_msk, empty_msk,
		    sizeof(mac_data.tbl_mcast_port_msk))) {
		hns_dsaf_tcam_mc_invld(dsaf_dev, entry_index);

		/* del soft entry */
		soft_mac_entry += entry_index;
		soft_mac_entry->index = DSAF_INVALID_ENTRY_IDX;
	} else { /* not zero, just del port, update */
		tcam_data.tbl_tcam_data_high = mac_key.high.val;
		tcam_data.tbl_tcam_data_low = mac_key.low.val;

		hns_dsaf_tcam_mc_cfg(dsaf_dev, entry_index,
				     &tcam_data,
				     pmask_key, &mac_data);
	}

	return 0;
}

int hns_dsaf_clr_mac_mc_port(struct dsaf_device *dsaf_dev, u8 mac_id,
			     u8 port_num)
{
	struct dsaf_drv_priv *priv = hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry;
	struct dsaf_tbl_tcam_mcast_cfg mac_data;
	int ret = 0, i;

	if (HNS_DSAF_IS_DEBUG(dsaf_dev))
		return 0;

	for (i = 0; i < DSAF_TCAM_SUM - DSAFV2_MAC_FUZZY_TCAM_NUM; i++) {
		u8 addr[ETH_ALEN];
		u8 port;

		soft_mac_entry = priv->soft_mac_tbl + i;

		hns_dsaf_tcam_addr_get(&soft_mac_entry->tcam_key, addr);
		port = dsaf_get_field(
				soft_mac_entry->tcam_key.low.bits.port_vlan,
				DSAF_TBL_TCAM_KEY_PORT_M,
				DSAF_TBL_TCAM_KEY_PORT_S);
		/* check valid tcam mc entry */
		if (soft_mac_entry->index != DSAF_INVALID_ENTRY_IDX &&
		    port == mac_id &&
		    is_multicast_ether_addr(addr) &&
		    !is_broadcast_ether_addr(addr)) {
			const u32 empty_msk[DSAF_PORT_MSK_NUM] = {0};
			struct dsaf_drv_mac_single_dest_entry mac_entry;

			/* disable receiving of this multicast address for
			 * the VF.
			 */
			ether_addr_copy(mac_entry.addr, addr);
			mac_entry.in_vlan_id = dsaf_get_field(
				soft_mac_entry->tcam_key.low.bits.port_vlan,
				DSAF_TBL_TCAM_KEY_VLAN_M,
				DSAF_TBL_TCAM_KEY_VLAN_S);
			mac_entry.in_port_num = mac_id;
			mac_entry.port_num = port_num;
			if (hns_dsaf_del_mac_mc_port(dsaf_dev, &mac_entry)) {
				ret = -EINVAL;
				continue;
			}

			/* disable receiving of this multicast address for
			 * the mac port if all VF are disable
			 */
			hns_dsaf_tcam_mc_get(dsaf_dev, i,
					     (struct dsaf_tbl_tcam_data *)
					     (&soft_mac_entry->tcam_key),
					     &mac_data);
			dsaf_set_bit(mac_data.tbl_mcast_port_msk[mac_id / 32],
				     mac_id % 32, 0);
			if (!memcmp(mac_data.tbl_mcast_port_msk, empty_msk,
				    sizeof(u32) * DSAF_PORT_MSK_NUM)) {
				mac_entry.port_num = mac_id;
				if (hns_dsaf_del_mac_mc_port(dsaf_dev,
							     &mac_entry)) {
					ret = -EINVAL;
					continue;
				}
			}
		}
	}

	return ret;
}

static struct dsaf_device *hns_dsaf_alloc_dev(struct device *dev,
					      size_t sizeof_priv)
{
	struct dsaf_device *dsaf_dev;

	dsaf_dev = devm_kzalloc(dev,
				sizeof(*dsaf_dev) + sizeof_priv, GFP_KERNEL);
	if (unlikely(!dsaf_dev)) {
		dsaf_dev = ERR_PTR(-ENOMEM);
	} else {
		dsaf_dev->dev = dev;
		dev_set_drvdata(dev, dsaf_dev);
	}

	return dsaf_dev;
}

/**
 * hns_dsaf_free_dev - free dev mem
 * @dsaf_dev: struct device pointer
 */
static void hns_dsaf_free_dev(struct dsaf_device *dsaf_dev)
{
	(void)dev_set_drvdata(dsaf_dev->dev, NULL);
}

/**
 * dsaf_pfc_unit_cnt - set pfc unit count
 * @dsaf_dev: dsa fabric id
 * @mac_id: id in use
 * @rate:  value array
 */
static void hns_dsaf_pfc_unit_cnt(struct dsaf_device *dsaf_dev, int  mac_id,
				  enum dsaf_port_rate_mode rate)
{
	u32 unit_cnt;

	switch (rate) {
	case DSAF_PORT_RATE_10000:
		unit_cnt = HNS_DSAF_PFC_UNIT_CNT_FOR_XGE;
		break;
	case DSAF_PORT_RATE_1000:
		unit_cnt = HNS_DSAF_PFC_UNIT_CNT_FOR_GE_1000;
		break;
	case DSAF_PORT_RATE_2500:
		unit_cnt = HNS_DSAF_PFC_UNIT_CNT_FOR_GE_1000;
		break;
	default:
		unit_cnt = HNS_DSAF_PFC_UNIT_CNT_FOR_XGE;
	}

	dsaf_set_dev_field(dsaf_dev,
			   (DSAF_PFC_UNIT_CNT_0_REG + 0x4 * (u64)mac_id),
			   DSAF_PFC_UNINT_CNT_M, DSAF_PFC_UNINT_CNT_S,
			   unit_cnt);
}

/**
 * dsaf_port_work_rate_cfg - fifo
 * @dsaf_dev: dsa fabric id
 * @mac_id: mac contrl block
 * @rate_mode: value array
 */
static void
hns_dsaf_port_work_rate_cfg(struct dsaf_device *dsaf_dev, int mac_id,
			    enum dsaf_port_rate_mode rate_mode)
{
	u32 port_work_mode;

	port_work_mode = dsaf_read_dev(
		dsaf_dev, DSAF_XGE_GE_WORK_MODE_0_REG + 0x4 * (u64)mac_id);

	if (rate_mode == DSAF_PORT_RATE_10000)
		dsaf_set_bit(port_work_mode, DSAF_XGE_GE_WORK_MODE_S, 1);
	else
		dsaf_set_bit(port_work_mode, DSAF_XGE_GE_WORK_MODE_S, 0);

	dsaf_write_dev(dsaf_dev,
		       DSAF_XGE_GE_WORK_MODE_0_REG + 0x4 * (u64)mac_id,
		       port_work_mode);

	hns_dsaf_pfc_unit_cnt(dsaf_dev, mac_id, rate_mode);
}

/**
 * hns_dsaf_fix_mac_mode - dsaf modify mac mode
 * @mac_cb: mac contrl block
 */
void hns_dsaf_fix_mac_mode(struct hns_mac_cb *mac_cb)
{
	enum dsaf_port_rate_mode mode;
	struct dsaf_device *dsaf_dev = mac_cb->dsaf_dev;
	int mac_id = mac_cb->mac_id;

	if (mac_cb->mac_type != HNAE_PORT_SERVICE)
		return;
	if (mac_cb->phy_if == PHY_INTERFACE_MODE_XGMII)
		mode = DSAF_PORT_RATE_10000;
	else
		mode = DSAF_PORT_RATE_1000;

	hns_dsaf_port_work_rate_cfg(dsaf_dev, mac_id, mode);
}

static u32 hns_dsaf_get_inode_prio_reg(int index)
{
	int base_index, offset;
	u32 base_addr = DSAF_INODE_IN_PRIO_PAUSE_BASE_REG;

	base_index = (index + 1) / DSAF_REG_PER_ZONE;
	offset = (index + 1) % DSAF_REG_PER_ZONE;

	return base_addr + DSAF_INODE_IN_PRIO_PAUSE_BASE_OFFSET * base_index +
		DSAF_INODE_IN_PRIO_PAUSE_OFFSET * offset;
}

void hns_dsaf_update_stats(struct dsaf_device *dsaf_dev, u32 node_num)
{
	struct dsaf_hw_stats *hw_stats
		= &dsaf_dev->hw_stats[node_num];
	bool is_ver1 = AE_IS_VER1(dsaf_dev->dsaf_ver);
	int i;
	u32 reg_tmp;

	hw_stats->pad_drop += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_PAD_DISCARD_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->man_pkts += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_FINAL_IN_MAN_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->rx_pkts += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_FINAL_IN_PKT_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->rx_pkt_id += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_SBM_PID_NUM_0_REG + 0x80 * (u64)node_num);

	reg_tmp = is_ver1 ? DSAF_INODE_FINAL_IN_PAUSE_NUM_0_REG :
			    DSAFV2_INODE_FINAL_IN_PAUSE_NUM_0_REG;
	hw_stats->rx_pause_frame +=
		dsaf_read_dev(dsaf_dev, reg_tmp + 0x80 * (u64)node_num);

	hw_stats->release_buf_num += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_SBM_RELS_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->sbm_drop += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_SBM_DROP_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->crc_false += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_CRC_FALSE_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->bp_drop += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_BP_DISCARD_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->rslt_drop += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_RSLT_DISCARD_NUM_0_REG + 0x80 * (u64)node_num);
	hw_stats->local_addr_false += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_LOCAL_ADDR_FALSE_NUM_0_REG + 0x80 * (u64)node_num);

	hw_stats->vlan_drop += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_SW_VLAN_TAG_DISC_0_REG + 4 * (u64)node_num);
	hw_stats->stp_drop += dsaf_read_dev(dsaf_dev,
		DSAF_INODE_IN_DATA_STP_DISC_0_REG + 4 * (u64)node_num);

	/* pfc pause frame statistics stored in dsaf inode*/
	if ((node_num < DSAF_SERVICE_NW_NUM) && !is_ver1) {
		for (i = 0; i < DSAF_PRIO_NR; i++) {
			reg_tmp = hns_dsaf_get_inode_prio_reg(i);
			hw_stats->rx_pfc[i] += dsaf_read_dev(dsaf_dev,
				reg_tmp + 0x4 * (u64)node_num);
			hw_stats->tx_pfc[i] += dsaf_read_dev(dsaf_dev,
				DSAF_XOD_XGE_PFC_PRIO_CNT_BASE_REG +
				DSAF_XOD_XGE_PFC_PRIO_CNT_OFFSET * i +
				0xF0 * (u64)node_num);
		}
	}
	hw_stats->tx_pkts += dsaf_read_dev(dsaf_dev,
		DSAF_XOD_RCVPKT_CNT_0_REG + 0x90 * (u64)node_num);
}

/**
 *hns_dsaf_get_regs - dump dsaf regs
 *@ddev: dsaf device
 *@port: port
 *@data:data for value of regs
 */
void hns_dsaf_get_regs(struct dsaf_device *ddev, u32 port, void *data)
{
	u32 i;
	u32 j;
	u32 *p = data;
	u32 reg_tmp;
	bool is_ver1 = AE_IS_VER1(ddev->dsaf_ver);

	/* dsaf common registers */
	p[0] = dsaf_read_dev(ddev, DSAF_SRAM_INIT_OVER_0_REG);
	p[1] = dsaf_read_dev(ddev, DSAF_CFG_0_REG);
	p[2] = dsaf_read_dev(ddev, DSAF_ECC_ERR_INVERT_0_REG);
	p[3] = dsaf_read_dev(ddev, DSAF_ABNORMAL_TIMEOUT_0_REG);
	p[4] = dsaf_read_dev(ddev, DSAF_FSM_TIMEOUT_0_REG);
	p[5] = dsaf_read_dev(ddev, DSAF_DSA_REG_CNT_CLR_CE_REG);
	p[6] = dsaf_read_dev(ddev, DSAF_DSA_SBM_INF_FIFO_THRD_REG);
	p[7] = dsaf_read_dev(ddev, DSAF_DSA_SRAM_1BIT_ECC_SEL_REG);
	p[8] = dsaf_read_dev(ddev, DSAF_DSA_SRAM_1BIT_ECC_CNT_REG);

	p[9] = dsaf_read_dev(ddev, DSAF_PFC_EN_0_REG + port * 4);
	p[10] = dsaf_read_dev(ddev, DSAF_PFC_UNIT_CNT_0_REG + port * 4);
	p[11] = dsaf_read_dev(ddev, DSAF_XGE_INT_MSK_0_REG + port * 4);
	p[12] = dsaf_read_dev(ddev, DSAF_XGE_INT_SRC_0_REG + port * 4);
	p[13] = dsaf_read_dev(ddev, DSAF_XGE_INT_STS_0_REG + port * 4);
	p[14] = dsaf_read_dev(ddev, DSAF_XGE_INT_MSK_0_REG + port * 4);
	p[15] = dsaf_read_dev(ddev, DSAF_PPE_INT_MSK_0_REG + port * 4);
	p[16] = dsaf_read_dev(ddev, DSAF_ROCEE_INT_MSK_0_REG + port * 4);
	p[17] = dsaf_read_dev(ddev, DSAF_XGE_INT_SRC_0_REG + port * 4);
	p[18] = dsaf_read_dev(ddev, DSAF_PPE_INT_SRC_0_REG + port * 4);
	p[19] =  dsaf_read_dev(ddev, DSAF_ROCEE_INT_SRC_0_REG + port * 4);
	p[20] = dsaf_read_dev(ddev, DSAF_XGE_INT_STS_0_REG + port * 4);
	p[21] = dsaf_read_dev(ddev, DSAF_PPE_INT_STS_0_REG + port * 4);
	p[22] = dsaf_read_dev(ddev, DSAF_ROCEE_INT_STS_0_REG + port * 4);
	p[23] = dsaf_read_dev(ddev, DSAF_PPE_QID_CFG_0_REG + port * 4);

	for (i = 0; i < DSAF_SW_PORT_NUM; i++)
		p[24 + i] = dsaf_read_dev(ddev,
				DSAF_SW_PORT_TYPE_0_REG + i * 4);

	p[32] = dsaf_read_dev(ddev, DSAF_MIX_DEF_QID_0_REG + port * 4);

	for (i = 0; i < DSAF_SW_PORT_NUM; i++)
		p[33 + i] = dsaf_read_dev(ddev,
				DSAF_PORT_DEF_VLAN_0_REG + i * 4);

	for (i = 0; i < DSAF_TOTAL_QUEUE_NUM; i++)
		p[41 + i] = dsaf_read_dev(ddev,
				DSAF_VM_DEF_VLAN_0_REG + i * 4);

	/* dsaf inode registers */
	p[170] = dsaf_read_dev(ddev, DSAF_INODE_CUT_THROUGH_CFG_0_REG);

	p[171] = dsaf_read_dev(ddev,
			DSAF_INODE_ECC_ERR_ADDR_0_REG + port * 0x80);

	for (i = 0; i < DSAF_INODE_NUM / DSAF_COMM_CHN; i++) {
		j = i * DSAF_COMM_CHN + port;
		p[172 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_IN_PORT_NUM_0_REG + j * 0x80);
		p[175 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_PRI_TC_CFG_0_REG + j * 0x80);
		p[178 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_BP_STATUS_0_REG + j * 0x80);
		p[181 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_PAD_DISCARD_NUM_0_REG + j * 0x80);
		p[184 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_FINAL_IN_MAN_NUM_0_REG + j * 0x80);
		p[187 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_FINAL_IN_PKT_NUM_0_REG + j * 0x80);
		p[190 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_SBM_PID_NUM_0_REG + j * 0x80);
		reg_tmp = is_ver1 ? DSAF_INODE_FINAL_IN_PAUSE_NUM_0_REG :
				    DSAFV2_INODE_FINAL_IN_PAUSE_NUM_0_REG;
		p[193 + i] = dsaf_read_dev(ddev, reg_tmp + j * 0x80);
		p[196 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_SBM_RELS_NUM_0_REG + j * 0x80);
		p[199 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_SBM_DROP_NUM_0_REG + j * 0x80);
		p[202 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_CRC_FALSE_NUM_0_REG + j * 0x80);
		p[205 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_BP_DISCARD_NUM_0_REG + j * 0x80);
		p[208 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_RSLT_DISCARD_NUM_0_REG + j * 0x80);
		p[211 + i] = dsaf_read_dev(ddev,
			DSAF_INODE_LOCAL_ADDR_FALSE_NUM_0_REG + j * 0x80);
		p[214 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_VOQ_OVER_NUM_0_REG + j * 0x80);
		p[217 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_BD_SAVE_STATUS_0_REG + j * 4);
		p[220 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_BD_ORDER_STATUS_0_REG + j * 4);
		p[223 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_SW_VLAN_TAG_DISC_0_REG + j * 4);
		p[226 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_IN_DATA_STP_DISC_0_REG + j * 4);
	}

	p[229] = dsaf_read_dev(ddev, DSAF_INODE_GE_FC_EN_0_REG + port * 4);

	for (i = 0; i < DSAF_INODE_NUM / DSAF_COMM_CHN; i++) {
		j = i * DSAF_COMM_CHN + port;
		p[230 + i] = dsaf_read_dev(ddev,
				DSAF_INODE_VC0_IN_PKT_NUM_0_REG + j * 4);
	}

	p[233] = dsaf_read_dev(ddev,
		DSAF_INODE_VC1_IN_PKT_NUM_0_REG + port * 0x80);

	/* dsaf inode registers */
	for (i = 0; i < HNS_DSAF_SBM_NUM(ddev) / DSAF_COMM_CHN; i++) {
		j = i * DSAF_COMM_CHN + port;
		p[234 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_CFG_REG_0_REG + j * 0x80);
		p[237 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CFG_0_XGE_REG_0_REG + j * 0x80);
		p[240 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CFG_1_REG_0_REG + j * 0x80);
		p[243 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CFG_2_XGE_REG_0_REG + j * 0x80);
		p[246 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_FREE_CNT_0_0_REG + j * 0x80);
		p[249 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_FREE_CNT_1_0_REG + j * 0x80);
		p[252 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CNT_0_0_REG + j * 0x80);
		p[255 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CNT_1_0_REG + j * 0x80);
		p[258 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CNT_2_0_REG + j * 0x80);
		p[261 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CNT_3_0_REG + j * 0x80);
		p[264 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_INER_ST_0_REG + j * 0x80);
		p[267 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_MIB_REQ_FAILED_TC_0_REG + j * 0x80);
		p[270 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_CNT_0_REG + j * 0x80);
		p[273 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_DROP_CNT_0_REG + j * 0x80);
		p[276 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_INF_OUTPORT_CNT_0_REG + j * 0x80);
		p[279 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC0_CNT_0_REG + j * 0x80);
		p[282 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC1_CNT_0_REG + j * 0x80);
		p[285 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC2_CNT_0_REG + j * 0x80);
		p[288 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC3_CNT_0_REG + j * 0x80);
		p[291 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC4_CNT_0_REG + j * 0x80);
		p[294 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC5_CNT_0_REG + j * 0x80);
		p[297 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC6_CNT_0_REG + j * 0x80);
		p[300 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_INPORT_TC7_CNT_0_REG + j * 0x80);
		p[303 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_REQ_CNT_0_REG + j * 0x80);
		p[306 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_LNK_RELS_CNT_0_REG + j * 0x80);
		p[309 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CFG_3_REG_0_REG + j * 0x80);
		p[312 + i] = dsaf_read_dev(ddev,
				DSAF_SBM_BP_CFG_4_REG_0_REG + j * 0x80);
	}

	/* dsaf onode registers */
	for (i = 0; i < DSAF_XOD_NUM; i++) {
		p[315 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_ETS_TSA_TC0_TC3_CFG_0_REG + i * 0x90);
		p[323 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_ETS_TSA_TC4_TC7_CFG_0_REG + i * 0x90);
		p[331 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_ETS_BW_TC0_TC3_CFG_0_REG + i * 0x90);
		p[339 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_ETS_BW_TC4_TC7_CFG_0_REG + i * 0x90);
		p[347 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_ETS_BW_OFFSET_CFG_0_REG + i * 0x90);
		p[355 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_ETS_TOKEN_CFG_0_REG + i * 0x90);
	}

	p[363] = dsaf_read_dev(ddev, DSAF_XOD_PFS_CFG_0_0_REG + port * 0x90);
	p[364] = dsaf_read_dev(ddev, DSAF_XOD_PFS_CFG_1_0_REG + port * 0x90);
	p[365] = dsaf_read_dev(ddev, DSAF_XOD_PFS_CFG_2_0_REG + port * 0x90);

	for (i = 0; i < DSAF_XOD_BIG_NUM / DSAF_COMM_CHN; i++) {
		j = i * DSAF_COMM_CHN + port;
		p[366 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_GNT_L_0_REG + j * 0x90);
		p[369 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_GNT_H_0_REG + j * 0x90);
		p[372 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_CONNECT_STATE_0_REG + j * 0x90);
		p[375 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVPKT_CNT_0_REG + j * 0x90);
		p[378 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVTC0_CNT_0_REG + j * 0x90);
		p[381 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVTC1_CNT_0_REG + j * 0x90);
		p[384 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVTC2_CNT_0_REG + j * 0x90);
		p[387 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVTC3_CNT_0_REG + j * 0x90);
		p[390 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVVC0_CNT_0_REG + j * 0x90);
		p[393 + i] = dsaf_read_dev(ddev,
				DSAF_XOD_RCVVC1_CNT_0_REG + j * 0x90);
	}

	p[396] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN0_CNT_0_REG + port * 0x90);
	p[397] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN1_CNT_0_REG + port * 0x90);
	p[398] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN2_CNT_0_REG + port * 0x90);
	p[399] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN3_CNT_0_REG + port * 0x90);
	p[400] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN4_CNT_0_REG + port * 0x90);
	p[401] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN5_CNT_0_REG + port * 0x90);
	p[402] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN6_CNT_0_REG + port * 0x90);
	p[403] = dsaf_read_dev(ddev,
		DSAF_XOD_XGE_RCVIN7_CNT_0_REG + port * 0x90);
	p[404] = dsaf_read_dev(ddev,
		DSAF_XOD_PPE_RCVIN0_CNT_0_REG + port * 0x90);
	p[405] = dsaf_read_dev(ddev,
		DSAF_XOD_PPE_RCVIN1_CNT_0_REG + port * 0x90);
	p[406] = dsaf_read_dev(ddev,
		DSAF_XOD_ROCEE_RCVIN0_CNT_0_REG + port * 0x90);
	p[407] = dsaf_read_dev(ddev,
		DSAF_XOD_ROCEE_RCVIN1_CNT_0_REG + port * 0x90);
	p[408] = dsaf_read_dev(ddev,
		DSAF_XOD_FIFO_STATUS_0_REG + port * 0x90);

	/* dsaf voq registers */
	for (i = 0; i < DSAF_VOQ_NUM / DSAF_COMM_CHN; i++) {
		j = (i * DSAF_COMM_CHN + port) * 0x90;
		p[409 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_ECC_INVERT_EN_0_REG + j);
		p[412 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_SRAM_PKT_NUM_0_REG + j);
		p[415 + i] = dsaf_read_dev(ddev, DSAF_VOQ_IN_PKT_NUM_0_REG + j);
		p[418 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_OUT_PKT_NUM_0_REG + j);
		p[421 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_ECC_ERR_ADDR_0_REG + j);
		p[424 + i] = dsaf_read_dev(ddev, DSAF_VOQ_BP_STATUS_0_REG + j);
		p[427 + i] = dsaf_read_dev(ddev, DSAF_VOQ_SPUP_IDLE_0_REG + j);
		p[430 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_XGE_XOD_REQ_0_0_REG + j);
		p[433 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_XGE_XOD_REQ_1_0_REG + j);
		p[436 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_PPE_XOD_REQ_0_REG + j);
		p[439 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_ROCEE_XOD_REQ_0_REG + j);
		p[442 + i] = dsaf_read_dev(ddev,
			DSAF_VOQ_BP_ALL_THRD_0_REG + j);
	}

	/* dsaf tbl registers */
	p[445] = dsaf_read_dev(ddev, DSAF_TBL_CTRL_0_REG);
	p[446] = dsaf_read_dev(ddev, DSAF_TBL_INT_MSK_0_REG);
	p[447] = dsaf_read_dev(ddev, DSAF_TBL_INT_SRC_0_REG);
	p[448] = dsaf_read_dev(ddev, DSAF_TBL_INT_STS_0_REG);
	p[449] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_ADDR_0_REG);
	p[450] = dsaf_read_dev(ddev, DSAF_TBL_LINE_ADDR_0_REG);
	p[451] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_HIGH_0_REG);
	p[452] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_LOW_0_REG);
	p[453] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_MCAST_CFG_4_0_REG);
	p[454] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_MCAST_CFG_3_0_REG);
	p[455] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_MCAST_CFG_2_0_REG);
	p[456] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_MCAST_CFG_1_0_REG);
	p[457] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_MCAST_CFG_0_0_REG);
	p[458] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_UCAST_CFG_0_REG);
	p[459] = dsaf_read_dev(ddev, DSAF_TBL_LIN_CFG_0_REG);
	p[460] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RDATA_HIGH_0_REG);
	p[461] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RDATA_LOW_0_REG);
	p[462] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RAM_RDATA4_0_REG);
	p[463] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RAM_RDATA3_0_REG);
	p[464] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RAM_RDATA2_0_REG);
	p[465] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RAM_RDATA1_0_REG);
	p[466] = dsaf_read_dev(ddev, DSAF_TBL_TCAM_RAM_RDATA0_0_REG);
	p[467] = dsaf_read_dev(ddev, DSAF_TBL_LIN_RDATA_0_REG);

	for (i = 0; i < DSAF_SW_PORT_NUM; i++) {
		j = i * 0x8;
		p[468 + 2 * i] = dsaf_read_dev(ddev,
			DSAF_TBL_DA0_MIS_INFO1_0_REG + j);
		p[469 + 2 * i] = dsaf_read_dev(ddev,
			DSAF_TBL_DA0_MIS_INFO0_0_REG + j);
	}

	p[484] = dsaf_read_dev(ddev, DSAF_TBL_SA_MIS_INFO2_0_REG);
	p[485] = dsaf_read_dev(ddev, DSAF_TBL_SA_MIS_INFO1_0_REG);
	p[486] = dsaf_read_dev(ddev, DSAF_TBL_SA_MIS_INFO0_0_REG);
	p[487] = dsaf_read_dev(ddev, DSAF_TBL_PUL_0_REG);
	p[488] = dsaf_read_dev(ddev, DSAF_TBL_OLD_RSLT_0_REG);
	p[489] = dsaf_read_dev(ddev, DSAF_TBL_OLD_SCAN_VAL_0_REG);
	p[490] = dsaf_read_dev(ddev, DSAF_TBL_DFX_CTRL_0_REG);
	p[491] = dsaf_read_dev(ddev, DSAF_TBL_DFX_STAT_0_REG);
	p[492] = dsaf_read_dev(ddev, DSAF_TBL_DFX_STAT_2_0_REG);
	p[493] = dsaf_read_dev(ddev, DSAF_TBL_LKUP_NUM_I_0_REG);
	p[494] = dsaf_read_dev(ddev, DSAF_TBL_LKUP_NUM_O_0_REG);
	p[495] = dsaf_read_dev(ddev, DSAF_TBL_UCAST_BCAST_MIS_INFO_0_0_REG);

	/* dsaf other registers */
	p[496] = dsaf_read_dev(ddev, DSAF_INODE_FIFO_WL_0_REG + port * 0x4);
	p[497] = dsaf_read_dev(ddev, DSAF_ONODE_FIFO_WL_0_REG + port * 0x4);
	p[498] = dsaf_read_dev(ddev, DSAF_XGE_GE_WORK_MODE_0_REG + port * 0x4);
	p[499] = dsaf_read_dev(ddev,
		DSAF_XGE_APP_RX_LINK_UP_0_REG + port * 0x4);
	p[500] = dsaf_read_dev(ddev, DSAF_NETPORT_CTRL_SIG_0_REG + port * 0x4);
	p[501] = dsaf_read_dev(ddev, DSAF_XGE_CTRL_SIG_CFG_0_REG + port * 0x4);

	if (!is_ver1)
		p[502] = dsaf_read_dev(ddev, DSAF_PAUSE_CFG_REG + port * 0x4);

	/* mark end of dsaf regs */
	for (i = 503; i < 504; i++)
		p[i] = 0xdddddddd;
}

static char *hns_dsaf_get_node_stats_strings(char *data, int node,
					     struct dsaf_device *dsaf_dev)
{
	char *buff = data;
	int i;
	bool is_ver1 = AE_IS_VER1(dsaf_dev->dsaf_ver);

	snprintf(buff, ETH_GSTRING_LEN, "innod%d_pad_drop_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_manage_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_rx_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_rx_pkt_id", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_rx_pause_frame", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_release_buf_num", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_sbm_drop_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_crc_false_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_bp_drop_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_lookup_rslt_drop_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_local_rslt_fail_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_vlan_drop_pkts", node);
	buff += ETH_GSTRING_LEN;
	snprintf(buff, ETH_GSTRING_LEN, "innod%d_stp_drop_pkts", node);
	buff += ETH_GSTRING_LEN;
	if (node < DSAF_SERVICE_NW_NUM && !is_ver1) {
		for (i = 0; i < DSAF_PRIO_NR; i++) {
			snprintf(buff + 0 * ETH_GSTRING_LEN * DSAF_PRIO_NR,
				 ETH_GSTRING_LEN, "inod%d_pfc_prio%d_pkts",
				 node, i);
			snprintf(buff + 1 * ETH_GSTRING_LEN * DSAF_PRIO_NR,
				 ETH_GSTRING_LEN, "onod%d_pfc_prio%d_pkts",
				 node, i);
			buff += ETH_GSTRING_LEN;
		}
		buff += 1 * DSAF_PRIO_NR * ETH_GSTRING_LEN;
	}
	snprintf(buff, ETH_GSTRING_LEN, "onnod%d_tx_pkts", node);
	buff += ETH_GSTRING_LEN;

	return buff;
}

static u64 *hns_dsaf_get_node_stats(struct dsaf_device *ddev, u64 *data,
				    int node_num)
{
	u64 *p = data;
	int i;
	struct dsaf_hw_stats *hw_stats = &ddev->hw_stats[node_num];
	bool is_ver1 = AE_IS_VER1(ddev->dsaf_ver);

	p[0] = hw_stats->pad_drop;
	p[1] = hw_stats->man_pkts;
	p[2] = hw_stats->rx_pkts;
	p[3] = hw_stats->rx_pkt_id;
	p[4] = hw_stats->rx_pause_frame;
	p[5] = hw_stats->release_buf_num;
	p[6] = hw_stats->sbm_drop;
	p[7] = hw_stats->crc_false;
	p[8] = hw_stats->bp_drop;
	p[9] = hw_stats->rslt_drop;
	p[10] = hw_stats->local_addr_false;
	p[11] = hw_stats->vlan_drop;
	p[12] = hw_stats->stp_drop;
	if (node_num < DSAF_SERVICE_NW_NUM && !is_ver1) {
		for (i = 0; i < DSAF_PRIO_NR; i++) {
			p[13 + i + 0 * DSAF_PRIO_NR] = hw_stats->rx_pfc[i];
			p[13 + i + 1 * DSAF_PRIO_NR] = hw_stats->tx_pfc[i];
		}
		p[29] = hw_stats->tx_pkts;
		return &p[30];
	}

	p[13] = hw_stats->tx_pkts;
	return &p[14];
}

/**
 *hns_dsaf_get_stats - get dsaf statistic
 *@ddev: dsaf device
 *@data:statistic value
 *@port: port num
 */
void hns_dsaf_get_stats(struct dsaf_device *ddev, u64 *data, int port)
{
	u64 *p = data;
	int node_num = port;

	/* for ge/xge node info */
	p = hns_dsaf_get_node_stats(ddev, p, node_num);

	/* for ppe node info */
	node_num = port + DSAF_PPE_INODE_BASE;
	(void)hns_dsaf_get_node_stats(ddev, p, node_num);
}

/**
 *hns_dsaf_get_sset_count - get dsaf string set count
 *@dsaf_dev: dsaf device
 *@stringset: type of values in data
 *return dsaf string name count
 */
int hns_dsaf_get_sset_count(struct dsaf_device *dsaf_dev, int stringset)
{
	bool is_ver1 = AE_IS_VER1(dsaf_dev->dsaf_ver);

	if (stringset == ETH_SS_STATS) {
		if (is_ver1)
			return DSAF_STATIC_NUM;
		else
			return DSAF_V2_STATIC_NUM;
	}
	return 0;
}

/**
 *hns_dsaf_get_strings - get dsaf string set
 *@stringset:srting set index
 *@data:strings name value
 *@port:port index
 *@dsaf_dev: dsaf device
 */
void hns_dsaf_get_strings(int stringset, u8 *data, int port,
			  struct dsaf_device *dsaf_dev)
{
	char *buff = (char *)data;
	int node = port;

	if (stringset != ETH_SS_STATS)
		return;

	/* for ge/xge node info */
	buff = hns_dsaf_get_node_stats_strings(buff, node, dsaf_dev);

	/* for ppe node info */
	node = port + DSAF_PPE_INODE_BASE;
	(void)hns_dsaf_get_node_stats_strings(buff, node, dsaf_dev);
}

/**
 *hns_dsaf_get_sset_count - get dsaf regs count
 *return dsaf regs count
 */
int hns_dsaf_get_regs_count(void)
{
	return DSAF_DUMP_REGS_NUM;
}

static int hns_dsaf_get_port_id(u8 port)
{
	if (port < DSAF_SERVICE_NW_NUM)
		return port;

	if (port >= DSAF_BASE_INNER_PORT_NUM)
		return port - DSAF_BASE_INNER_PORT_NUM + DSAF_SERVICE_NW_NUM;

	return -EINVAL;
}

static void set_promisc_tcam_enable(struct dsaf_device *dsaf_dev, u32 port)
{
	struct dsaf_tbl_tcam_ucast_cfg tbl_tcam_ucast = {0, 1, 0, 0, 0x80};
	struct dsaf_tbl_tcam_data tbl_tcam_data_mc = {0x01000000, port};
	struct dsaf_tbl_tcam_data tbl_tcam_mask_uc = {0x01000000, 0xf};
	struct dsaf_tbl_tcam_mcast_cfg tbl_tcam_mcast = {0, 0, {0} };
	struct dsaf_drv_priv *priv = hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_tbl_tcam_data tbl_tcam_data_uc = {0, port};
	struct dsaf_drv_mac_single_dest_entry mask_entry;
	struct dsaf_drv_tbl_tcam_key temp_key, mask_key;
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry;
	u16 entry_index;
	struct dsaf_drv_tbl_tcam_key mac_key;
	struct hns_mac_cb *mac_cb;
	u8 addr[ETH_ALEN] = {0};
	u8 port_num;
	int mskid;

	/* promisc use vague table match with vlanid = 0 & macaddr = 0 */
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, 0x00, port, addr);
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);
	if (entry_index != DSAF_INVALID_ENTRY_IDX)
		return;

	/* put promisc tcam entry in the end. */
	/* 1. set promisc unicast vague tcam entry. */
	entry_index = hns_dsaf_find_empty_mac_entry_reverse(dsaf_dev);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		dev_err(dsaf_dev->dev,
			"enable uc promisc failed (port:%#x)\n",
			port);
		return;
	}

	mac_cb = dsaf_dev->mac_cb[port];
	(void)hns_mac_get_inner_port_num(mac_cb, 0, &port_num);
	tbl_tcam_ucast.tbl_ucast_out_port = port_num;

	/* config uc vague table */
	hns_dsaf_tcam_uc_cfg_vague(dsaf_dev, entry_index, &tbl_tcam_data_uc,
				   &tbl_tcam_mask_uc, &tbl_tcam_ucast);

	/* update software entry */
	soft_mac_entry = priv->soft_mac_tbl;
	soft_mac_entry += entry_index;
	soft_mac_entry->index = entry_index;
	soft_mac_entry->tcam_key.high.val = mac_key.high.val;
	soft_mac_entry->tcam_key.low.val = mac_key.low.val;
	/* step back to the START for mc. */
	soft_mac_entry = priv->soft_mac_tbl;

	/* 2. set promisc multicast vague tcam entry. */
	entry_index = hns_dsaf_find_empty_mac_entry_reverse(dsaf_dev);
	if (entry_index == DSAF_INVALID_ENTRY_IDX) {
		dev_err(dsaf_dev->dev,
			"enable mc promisc failed (port:%#x)\n",
			port);
		return;
	}

	memset(&mask_entry, 0x0, sizeof(mask_entry));
	memset(&mask_key, 0x0, sizeof(mask_key));
	memset(&temp_key, 0x0, sizeof(temp_key));
	mask_entry.addr[0] = 0x01;
	hns_dsaf_set_mac_key(dsaf_dev, &mask_key, mask_entry.in_vlan_id,
			     0xf, mask_entry.addr);
	tbl_tcam_mcast.tbl_mcast_item_vld = 1;
	tbl_tcam_mcast.tbl_mcast_old_en = 0;

	/* set MAC port to handle multicast */
	mskid = hns_dsaf_get_port_id(port);
	if (mskid == -EINVAL) {
		dev_err(dsaf_dev->dev, "%s,pnum(%d)error,key(%#x:%#x)\n",
			dsaf_dev->ae_dev.name, port,
			mask_key.high.val, mask_key.low.val);
		return;
	}
	dsaf_set_bit(tbl_tcam_mcast.tbl_mcast_port_msk[mskid / 32],
		     mskid % 32, 1);

	/* set pool bit map to handle multicast */
	mskid = hns_dsaf_get_port_id(port_num);
	if (mskid == -EINVAL) {
		dev_err(dsaf_dev->dev,
			"%s, pool bit map pnum(%d)error,key(%#x:%#x)\n",
			dsaf_dev->ae_dev.name, port_num,
			mask_key.high.val, mask_key.low.val);
		return;
	}
	dsaf_set_bit(tbl_tcam_mcast.tbl_mcast_port_msk[mskid / 32],
		     mskid % 32, 1);

	memcpy(&temp_key, &mask_key, sizeof(mask_key));
	hns_dsaf_tcam_mc_cfg_vague(dsaf_dev, entry_index, &tbl_tcam_data_mc,
				   (struct dsaf_tbl_tcam_data *)(&mask_key),
				   &tbl_tcam_mcast);

	/* update software entry */
	soft_mac_entry += entry_index;
	soft_mac_entry->index = entry_index;
	soft_mac_entry->tcam_key.high.val = temp_key.high.val;
	soft_mac_entry->tcam_key.low.val = temp_key.low.val;
}

static void set_promisc_tcam_disable(struct dsaf_device *dsaf_dev, u32 port)
{
	struct dsaf_tbl_tcam_data tbl_tcam_data_mc = {0x01000000, port};
	struct dsaf_tbl_tcam_ucast_cfg tbl_tcam_ucast = {0, 0, 0, 0, 0};
	struct dsaf_tbl_tcam_mcast_cfg tbl_tcam_mcast = {0, 0, {0} };
	struct dsaf_drv_priv *priv = hns_dsaf_dev_priv(dsaf_dev);
	struct dsaf_tbl_tcam_data tbl_tcam_data_uc = {0, 0};
	struct dsaf_tbl_tcam_data tbl_tcam_mask = {0, 0};
	struct dsaf_drv_soft_mac_tbl *soft_mac_entry;
	u16 entry_index;
	struct dsaf_drv_tbl_tcam_key mac_key;
	u8 addr[ETH_ALEN] = {0};

	/* 1. delete uc vague tcam entry. */
	/* promisc use vague table match with vlanid = 0 & macaddr = 0 */
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, 0x00, port, addr);
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);

	if (entry_index == DSAF_INVALID_ENTRY_IDX)
		return;

	/* config uc vague table */
	hns_dsaf_tcam_uc_cfg_vague(dsaf_dev, entry_index, &tbl_tcam_data_uc,
				   &tbl_tcam_mask, &tbl_tcam_ucast);
	/* update soft management table. */
	soft_mac_entry = priv->soft_mac_tbl;
	soft_mac_entry += entry_index;
	soft_mac_entry->index = DSAF_INVALID_ENTRY_IDX;
	/* step back to the START for mc. */
	soft_mac_entry = priv->soft_mac_tbl;

	/* 2. delete mc vague tcam entry. */
	addr[0] = 0x01;
	memset(&mac_key, 0x0, sizeof(mac_key));
	hns_dsaf_set_mac_key(dsaf_dev, &mac_key, 0x00, port, addr);
	entry_index = hns_dsaf_find_soft_mac_entry(dsaf_dev, &mac_key);

	if (entry_index == DSAF_INVALID_ENTRY_IDX)
		return;

	/* config mc vague table */
	hns_dsaf_tcam_mc_cfg_vague(dsaf_dev, entry_index, &tbl_tcam_data_mc,
				   &tbl_tcam_mask, &tbl_tcam_mcast);
	/* update soft management table. */
	soft_mac_entry += entry_index;
	soft_mac_entry->index = DSAF_INVALID_ENTRY_IDX;
}

/* Reserve the last TCAM entry for promisc support */
void hns_dsaf_set_promisc_tcam(struct dsaf_device *dsaf_dev,
			       u32 port, bool enable)
{
	if (enable)
		set_promisc_tcam_enable(dsaf_dev, port);
	else
		set_promisc_tcam_disable(dsaf_dev, port);
}

int hns_dsaf_wait_pkt_clean(struct dsaf_device *dsaf_dev, int port)
{
	u32 val, val_tmp;
	int wait_cnt;

	if (port >= DSAF_SERVICE_NW_NUM)
		return 0;

	wait_cnt = 0;
	while (wait_cnt++ < HNS_MAX_WAIT_CNT) {
		val = dsaf_read_dev(dsaf_dev, DSAF_VOQ_IN_PKT_NUM_0_REG +
			(port + DSAF_XGE_NUM) * 0x40);
		val_tmp = dsaf_read_dev(dsaf_dev, DSAF_VOQ_OUT_PKT_NUM_0_REG +
			(port + DSAF_XGE_NUM) * 0x40);
		if (val == val_tmp)
			break;

		usleep_range(100, 200);
	}

	if (wait_cnt >= HNS_MAX_WAIT_CNT) {
		dev_err(dsaf_dev->dev, "hns dsaf clean wait timeout(%u - %u).\n",
			val, val_tmp);
		return -EBUSY;
	}

	return 0;
}

/**
 * dsaf_probe - probo dsaf dev
 * @pdev: dasf platform device
 * return 0 - success , negative --fail
 */
static int hns_dsaf_probe(struct platform_device *pdev)
{
	struct dsaf_device *dsaf_dev;
	int ret;

	dsaf_dev = hns_dsaf_alloc_dev(&pdev->dev, sizeof(struct dsaf_drv_priv));
	if (IS_ERR(dsaf_dev)) {
		ret = PTR_ERR(dsaf_dev);
		dev_err(&pdev->dev,
			"dsaf_probe dsaf_alloc_dev failed, ret = %#x!\n", ret);
		return ret;
	}

	ret = hns_dsaf_get_cfg(dsaf_dev);
	if (ret)
		goto free_dev;

	ret = hns_dsaf_init(dsaf_dev);
	if (ret)
		goto free_dev;

	ret = hns_mac_init(dsaf_dev);
	if (ret)
		goto uninit_dsaf;

	ret = hns_ppe_init(dsaf_dev);
	if (ret)
		goto uninit_mac;

	ret = hns_dsaf_ae_init(dsaf_dev);
	if (ret)
		goto uninit_ppe;

	return 0;

uninit_ppe:
	hns_ppe_uninit(dsaf_dev);

uninit_mac:
	hns_mac_uninit(dsaf_dev);

uninit_dsaf:
	hns_dsaf_free(dsaf_dev);

free_dev:
	hns_dsaf_free_dev(dsaf_dev);

	return ret;
}

/**
 * dsaf_remove - remove dsaf dev
 * @pdev: dasf platform device
 */
static int hns_dsaf_remove(struct platform_device *pdev)
{
	struct dsaf_device *dsaf_dev = dev_get_drvdata(&pdev->dev);

	hns_dsaf_ae_uninit(dsaf_dev);

	hns_ppe_uninit(dsaf_dev);

	hns_mac_uninit(dsaf_dev);

	hns_dsaf_free(dsaf_dev);

	hns_dsaf_free_dev(dsaf_dev);

	return 0;
}

static const struct of_device_id g_dsaf_match[] = {
	{.compatible = "hisilicon,hns-dsaf-v1"},
	{.compatible = "hisilicon,hns-dsaf-v2"},
	{}
};
MODULE_DEVICE_TABLE(of, g_dsaf_match);

static struct platform_driver g_dsaf_driver = {
	.probe = hns_dsaf_probe,
	.remove = hns_dsaf_remove,
	.driver = {
		.name = DSAF_DRV_NAME,
		.of_match_table = g_dsaf_match,
		.acpi_match_table = hns_dsaf_acpi_match,
	},
};

module_platform_driver(g_dsaf_driver);

/**
 * hns_dsaf_roce_reset - reset dsaf and roce
 * @dsaf_fwnode: Pointer to framework node for the dasf
 * @dereset: false - request reset , true - drop reset
 * return 0 - success , negative -fail
 */
int hns_dsaf_roce_reset(struct fwnode_handle *dsaf_fwnode, bool dereset)
{
	struct dsaf_device *dsaf_dev;
	struct platform_device *pdev;
	u32 mp;
	u32 sl;
	u32 credit;
	int i;
	static const u32 port_map[DSAF_ROCE_CREDIT_CHN][DSAF_ROCE_CHAN_MODE_NUM] = {
		{DSAF_ROCE_PORT_0, DSAF_ROCE_PORT_0, DSAF_ROCE_PORT_0},
		{DSAF_ROCE_PORT_1, DSAF_ROCE_PORT_0, DSAF_ROCE_PORT_0},
		{DSAF_ROCE_PORT_2, DSAF_ROCE_PORT_1, DSAF_ROCE_PORT_0},
		{DSAF_ROCE_PORT_3, DSAF_ROCE_PORT_1, DSAF_ROCE_PORT_0},
		{DSAF_ROCE_PORT_4, DSAF_ROCE_PORT_2, DSAF_ROCE_PORT_1},
		{DSAF_ROCE_PORT_4, DSAF_ROCE_PORT_2, DSAF_ROCE_PORT_1},
		{DSAF_ROCE_PORT_5, DSAF_ROCE_PORT_3, DSAF_ROCE_PORT_1},
		{DSAF_ROCE_PORT_5, DSAF_ROCE_PORT_3, DSAF_ROCE_PORT_1},
	};
	static const u32 sl_map[DSAF_ROCE_CREDIT_CHN][DSAF_ROCE_CHAN_MODE_NUM] = {
		{DSAF_ROCE_SL_0, DSAF_ROCE_SL_0, DSAF_ROCE_SL_0},
		{DSAF_ROCE_SL_0, DSAF_ROCE_SL_1, DSAF_ROCE_SL_1},
		{DSAF_ROCE_SL_0, DSAF_ROCE_SL_0, DSAF_ROCE_SL_2},
		{DSAF_ROCE_SL_0, DSAF_ROCE_SL_1, DSAF_ROCE_SL_3},
		{DSAF_ROCE_SL_0, DSAF_ROCE_SL_0, DSAF_ROCE_SL_0},
		{DSAF_ROCE_SL_1, DSAF_ROCE_SL_1, DSAF_ROCE_SL_1},
		{DSAF_ROCE_SL_0, DSAF_ROCE_SL_0, DSAF_ROCE_SL_2},
		{DSAF_ROCE_SL_1, DSAF_ROCE_SL_1, DSAF_ROCE_SL_3},
	};

	/* find the platform device corresponding to fwnode */
	if (is_of_node(dsaf_fwnode)) {
		pdev = of_find_device_by_node(to_of_node(dsaf_fwnode));
	} else if (is_acpi_device_node(dsaf_fwnode)) {
		pdev = hns_dsaf_find_platform_device(dsaf_fwnode);
	} else {
		pr_err("fwnode is neither OF or ACPI type\n");
		return -EINVAL;
	}

	/* check if we were a success in fetching pdev */
	if (!pdev) {
		pr_err("couldn't find platform device for node\n");
		return -ENODEV;
	}

	/* retrieve the dsaf_device from the driver data */
	dsaf_dev = dev_get_drvdata(&pdev->dev);
	if (!dsaf_dev) {
		dev_err(&pdev->dev, "dsaf_dev is NULL\n");
		put_device(&pdev->dev);
		return -ENODEV;
	}

	/* now, make sure we are running on compatible SoC */
	if (AE_IS_VER1(dsaf_dev->dsaf_ver)) {
		dev_err(dsaf_dev->dev, "%s v1 chip doesn't support RoCE!\n",
			dsaf_dev->ae_dev.name);
		put_device(&pdev->dev);
		return -ENODEV;
	}

	/* do reset or de-reset according to the flag */
	if (!dereset) {
		/* reset rocee-channels in dsaf and rocee */
		dsaf_dev->misc_op->hns_dsaf_srst_chns(dsaf_dev, DSAF_CHNS_MASK,
						      false);
		dsaf_dev->misc_op->hns_dsaf_roce_srst(dsaf_dev, false);
	} else {
		/* configure dsaf tx roce correspond to port map and sl map */
		mp = dsaf_read_dev(dsaf_dev, DSAF_ROCE_PORT_MAP_REG);
		for (i = 0; i < DSAF_ROCE_CREDIT_CHN; i++)
			dsaf_set_field(mp, 7 << i * 3, i * 3,
				       port_map[i][DSAF_ROCE_6PORT_MODE]);
		dsaf_set_field(mp, 3 << i * 3, i * 3, 0);
		dsaf_write_dev(dsaf_dev, DSAF_ROCE_PORT_MAP_REG, mp);

		sl = dsaf_read_dev(dsaf_dev, DSAF_ROCE_SL_MAP_REG);
		for (i = 0; i < DSAF_ROCE_CREDIT_CHN; i++)
			dsaf_set_field(sl, 3 << i * 2, i * 2,
				       sl_map[i][DSAF_ROCE_6PORT_MODE]);
		dsaf_write_dev(dsaf_dev, DSAF_ROCE_SL_MAP_REG, sl);

		/* de-reset rocee-channels in dsaf and rocee */
		dsaf_dev->misc_op->hns_dsaf_srst_chns(dsaf_dev, DSAF_CHNS_MASK,
						      true);
		msleep(SRST_TIME_INTERVAL);
		dsaf_dev->misc_op->hns_dsaf_roce_srst(dsaf_dev, true);

		/* enable dsaf channel rocee credit */
		credit = dsaf_read_dev(dsaf_dev, DSAF_SBM_ROCEE_CFG_REG_REG);
		dsaf_set_bit(credit, DSAF_SBM_ROCEE_CFG_CRD_EN_B, 0);
		dsaf_write_dev(dsaf_dev, DSAF_SBM_ROCEE_CFG_REG_REG, credit);

		dsaf_set_bit(credit, DSAF_SBM_ROCEE_CFG_CRD_EN_B, 1);
		dsaf_write_dev(dsaf_dev, DSAF_SBM_ROCEE_CFG_REG_REG, credit);
	}

	put_device(&pdev->dev);

	return 0;
}
EXPORT_SYMBOL(hns_dsaf_roce_reset);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huawei Tech. Co., Ltd.");
MODULE_DESCRIPTION("HNS DSAF driver");
MODULE_VERSION(DSAF_MOD_VERSION);

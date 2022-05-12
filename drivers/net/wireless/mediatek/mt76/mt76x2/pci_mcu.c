// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 */

#include <linux/kernel.h>
#include <linux/firmware.h>
#include <linux/delay.h>

#include "mt76x2.h"
#include "mcu.h"
#include "eeprom.h"

static int
mt76pci_load_rom_patch(struct mt76x02_dev *dev)
{
	const struct firmware *fw = NULL;
	struct mt76x02_patch_header *hdr;
	bool rom_protect = !is_mt7612(dev);
	int len, ret = 0;
	__le32 *cur;
	u32 patch_mask, patch_reg;

	if (rom_protect && !mt76_poll(dev, MT_MCU_SEMAPHORE_03, 1, 1, 600)) {
		dev_err(dev->mt76.dev,
			"Could not get hardware semaphore for ROM PATCH\n");
		return -ETIMEDOUT;
	}

	if (mt76xx_rev(dev) >= MT76XX_REV_E3) {
		patch_mask = BIT(0);
		patch_reg = MT_MCU_CLOCK_CTL;
	} else {
		patch_mask = BIT(1);
		patch_reg = MT_MCU_COM_REG0;
	}

	if (rom_protect && (mt76_rr(dev, patch_reg) & patch_mask)) {
		dev_info(dev->mt76.dev, "ROM patch already applied\n");
		goto out;
	}

	ret = request_firmware(&fw, MT7662_ROM_PATCH, dev->mt76.dev);
	if (ret)
		goto out;

	if (!fw || !fw->data || fw->size <= sizeof(*hdr)) {
		ret = -EIO;
		dev_err(dev->mt76.dev, "Failed to load firmware\n");
		goto out;
	}

	hdr = (struct mt76x02_patch_header *)fw->data;
	dev_info(dev->mt76.dev, "ROM patch build: %.15s\n", hdr->build_time);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, MT_MCU_ROM_PATCH_OFFSET);

	cur = (__le32 *)(fw->data + sizeof(*hdr));
	len = fw->size - sizeof(*hdr);
	mt76_wr_copy(dev, MT_MCU_ROM_PATCH_ADDR, cur, len);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, 0);

	/* Trigger ROM */
	mt76_wr(dev, MT_MCU_INT_LEVEL, 4);

	if (!mt76_poll_msec(dev, patch_reg, patch_mask, patch_mask, 2000)) {
		dev_err(dev->mt76.dev, "Failed to load ROM patch\n");
		ret = -ETIMEDOUT;
	}

out:
	/* release semaphore */
	if (rom_protect)
		mt76_wr(dev, MT_MCU_SEMAPHORE_03, 1);
	release_firmware(fw);
	return ret;
}

static int
mt76pci_load_firmware(struct mt76x02_dev *dev)
{
	const struct firmware *fw;
	const struct mt76x02_fw_header *hdr;
	int len, ret;
	__le32 *cur;
	u32 offset, val;

	ret = request_firmware(&fw, MT7662_FIRMWARE, dev->mt76.dev);
	if (ret)
		return ret;

	if (!fw || !fw->data || fw->size < sizeof(*hdr))
		goto error;

	hdr = (const struct mt76x02_fw_header *)fw->data;

	len = sizeof(*hdr);
	len += le32_to_cpu(hdr->ilm_len);
	len += le32_to_cpu(hdr->dlm_len);

	if (fw->size != len)
		goto error;

	val = le16_to_cpu(hdr->fw_ver);
	dev_info(dev->mt76.dev, "Firmware Version: %d.%d.%02d\n",
		 (val >> 12) & 0xf, (val >> 8) & 0xf, val & 0xf);

	val = le16_to_cpu(hdr->build_ver);
	dev_info(dev->mt76.dev, "Build: %x\n", val);
	dev_info(dev->mt76.dev, "Build Time: %.16s\n", hdr->build_time);

	cur = (__le32 *)(fw->data + sizeof(*hdr));
	len = le32_to_cpu(hdr->ilm_len);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, MT_MCU_ILM_OFFSET);
	mt76_wr_copy(dev, MT_MCU_ILM_ADDR, cur, len);

	cur += len / sizeof(*cur);
	len = le32_to_cpu(hdr->dlm_len);

	if (mt76xx_rev(dev) >= MT76XX_REV_E3)
		offset = MT_MCU_DLM_ADDR_E3;
	else
		offset = MT_MCU_DLM_ADDR;

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, MT_MCU_DLM_OFFSET);
	mt76_wr_copy(dev, offset, cur, len);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, 0);

	val = mt76x02_eeprom_get(dev, MT_EE_NIC_CONF_2);
	if (FIELD_GET(MT_EE_NIC_CONF_2_XTAL_OPTION, val) == 1)
		mt76_set(dev, MT_MCU_COM_REG0, BIT(30));

	/* trigger firmware */
	mt76_wr(dev, MT_MCU_INT_LEVEL, 2);
	if (!mt76_poll_msec(dev, MT_MCU_COM_REG0, 1, 1, 200)) {
		dev_err(dev->mt76.dev, "Firmware failed to start\n");
		release_firmware(fw);
		return -ETIMEDOUT;
	}

	mt76x02_set_ethtool_fwver(dev, hdr);
	dev_info(dev->mt76.dev, "Firmware running!\n");

	release_firmware(fw);

	return ret;

error:
	dev_err(dev->mt76.dev, "Invalid firmware\n");
	release_firmware(fw);
	return -ENOENT;
}

static int
mt76pci_mcu_restart(struct mt76_dev *mdev)
{
	struct mt76x02_dev *dev;
	int ret;

	dev = container_of(mdev, struct mt76x02_dev, mt76);

	mt76x02_mcu_cleanup(dev);
	mt76x2_mac_reset(dev, true);

	ret = mt76pci_load_firmware(dev);
	if (ret)
		return ret;

	mt76_wr(dev, MT_WPDMA_RST_IDX, ~0);

	return 0;
}

int mt76x2_mcu_init(struct mt76x02_dev *dev)
{
	static const struct mt76_mcu_ops mt76x2_mcu_ops = {
		.mcu_restart = mt76pci_mcu_restart,
		.mcu_send_msg = mt76x02_mcu_msg_send,
		.mcu_parse_response = mt76x02_mcu_parse_response,
	};
	int ret;

	dev->mt76.mcu_ops = &mt76x2_mcu_ops;

	ret = mt76pci_load_rom_patch(dev);
	if (ret)
		return ret;

	ret = mt76pci_load_firmware(dev);
	if (ret)
		return ret;

	mt76x02_mcu_function_select(dev, Q_SELECT, 1);
	return 0;
}

// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 * Copyright (C) 2018 Lorenzo Bianconi <lorenzo.bianconi83@gmail.com>
 */

#include <linux/kernel.h>
#include <linux/firmware.h>
#include <linux/delay.h>

#include "mt76x2.h"
#include "mcu.h"
#include "eeprom.h"

int mt76x2_mcu_set_channel(struct mt76x02_dev *dev, u8 channel, u8 bw,
			   u8 bw_index, bool scan)
{
	struct {
		u8 idx;
		u8 scan;
		u8 bw;
		u8 _pad0;

		__le16 chainmask;
		u8 ext_chan;
		u8 _pad1;

	} __packed __aligned(4) msg = {
		.idx = channel,
		.scan = scan,
		.bw = bw,
		.chainmask = cpu_to_le16(dev->mphy.chainmask),
	};

	/* first set the channel without the extension channel info */
	mt76_mcu_send_msg(&dev->mt76, CMD_SWITCH_CHANNEL_OP, &msg,
			  sizeof(msg), true);

	usleep_range(5000, 10000);

	msg.ext_chan = 0xe0 + bw_index;
	return mt76_mcu_send_msg(&dev->mt76, CMD_SWITCH_CHANNEL_OP, &msg,
				 sizeof(msg), true);
}
EXPORT_SYMBOL_GPL(mt76x2_mcu_set_channel);

int mt76x2_mcu_load_cr(struct mt76x02_dev *dev, u8 type, u8 temp_level,
		       u8 channel)
{
	struct {
		u8 cr_mode;
		u8 temp;
		u8 ch;
		u8 _pad0;

		__le32 cfg;
	} __packed __aligned(4) msg = {
		.cr_mode = type,
		.temp = temp_level,
		.ch = channel,
	};
	u32 val;

	val = BIT(31);
	val |= (mt76x02_eeprom_get(dev, MT_EE_NIC_CONF_0) >> 8) & 0x00ff;
	val |= (mt76x02_eeprom_get(dev, MT_EE_NIC_CONF_1) << 8) & 0xff00;
	msg.cfg = cpu_to_le32(val);

	/* first set the channel without the extension channel info */
	return mt76_mcu_send_msg(&dev->mt76, CMD_LOAD_CR, &msg, sizeof(msg),
				 true);
}
EXPORT_SYMBOL_GPL(mt76x2_mcu_load_cr);

int mt76x2_mcu_init_gain(struct mt76x02_dev *dev, u8 channel, u32 gain,
			 bool force)
{
	struct {
		__le32 channel;
		__le32 gain_val;
	} __packed __aligned(4) msg = {
		.channel = cpu_to_le32(channel),
		.gain_val = cpu_to_le32(gain),
	};

	if (force)
		msg.channel |= cpu_to_le32(BIT(31));

	return mt76_mcu_send_msg(&dev->mt76, CMD_INIT_GAIN_OP, &msg,
				 sizeof(msg), true);
}
EXPORT_SYMBOL_GPL(mt76x2_mcu_init_gain);

int mt76x2_mcu_tssi_comp(struct mt76x02_dev *dev,
			 struct mt76x2_tssi_comp *tssi_data)
{
	struct {
		__le32 id;
		struct mt76x2_tssi_comp data;
	} __packed __aligned(4) msg = {
		.id = cpu_to_le32(MCU_CAL_TSSI_COMP),
		.data = *tssi_data,
	};

	return mt76_mcu_send_msg(&dev->mt76, CMD_CALIBRATION_OP, &msg,
				 sizeof(msg), true);
}
EXPORT_SYMBOL_GPL(mt76x2_mcu_tssi_comp);

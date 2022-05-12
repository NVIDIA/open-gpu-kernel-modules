// SPDX-License-Identifier: GPL-2.0
//
// mcp251xfd - Microchip MCP251xFD Family CAN controller driver
//
// Copyright (c) 2019, 2020 Pengutronix,
//                          Marc Kleine-Budde <kernel@pengutronix.de>
//

#include "mcp251xfd.h"

#include <asm/unaligned.h>

static const struct regmap_config mcp251xfd_regmap_crc;

static int
mcp251xfd_regmap_nocrc_write(void *context, const void *data, size_t count)
{
	struct spi_device *spi = context;

	return spi_write(spi, data, count);
}

static int
mcp251xfd_regmap_nocrc_gather_write(void *context,
				    const void *reg, size_t reg_len,
				    const void *val, size_t val_len)
{
	struct spi_device *spi = context;
	struct mcp251xfd_priv *priv = spi_get_drvdata(spi);
	struct mcp251xfd_map_buf_nocrc *buf_tx = priv->map_buf_nocrc_tx;
	struct spi_transfer xfer[] = {
		{
			.tx_buf = buf_tx,
			.len = sizeof(buf_tx->cmd) + val_len,
		},
	};

	BUILD_BUG_ON(sizeof(buf_tx->cmd) != sizeof(__be16));

	if (IS_ENABLED(CONFIG_CAN_MCP251XFD_SANITY) &&
	    reg_len != sizeof(buf_tx->cmd.cmd))
		return -EINVAL;

	memcpy(&buf_tx->cmd, reg, sizeof(buf_tx->cmd));
	memcpy(buf_tx->data, val, val_len);

	return spi_sync_transfer(spi, xfer, ARRAY_SIZE(xfer));
}

static inline bool mcp251xfd_update_bits_read_reg(unsigned int reg)
{
	switch (reg) {
	case MCP251XFD_REG_INT:
	case MCP251XFD_REG_TEFCON:
	case MCP251XFD_REG_FIFOCON(MCP251XFD_RX_FIFO(0)):
	case MCP251XFD_REG_FLTCON(0):
	case MCP251XFD_REG_ECCSTAT:
	case MCP251XFD_REG_CRC:
		return false;
	case MCP251XFD_REG_CON:
	case MCP251XFD_REG_FIFOSTA(MCP251XFD_RX_FIFO(0)):
	case MCP251XFD_REG_OSC:
	case MCP251XFD_REG_ECCCON:
		return true;
	default:
		WARN(1, "Status of reg 0x%04x unknown.\n", reg);
	}

	return true;
}

static int
mcp251xfd_regmap_nocrc_update_bits(void *context, unsigned int reg,
				   unsigned int mask, unsigned int val)
{
	struct spi_device *spi = context;
	struct mcp251xfd_priv *priv = spi_get_drvdata(spi);
	struct mcp251xfd_map_buf_nocrc *buf_rx = priv->map_buf_nocrc_rx;
	struct mcp251xfd_map_buf_nocrc *buf_tx = priv->map_buf_nocrc_tx;
	__le32 orig_le32 = 0, mask_le32, val_le32, tmp_le32;
	u8 first_byte, last_byte, len;
	int err;

	BUILD_BUG_ON(sizeof(buf_rx->cmd) != sizeof(__be16));
	BUILD_BUG_ON(sizeof(buf_tx->cmd) != sizeof(__be16));

	if (IS_ENABLED(CONFIG_CAN_MCP251XFD_SANITY) &&
	    mask == 0)
		return -EINVAL;

	first_byte = mcp251xfd_first_byte_set(mask);
	last_byte = mcp251xfd_last_byte_set(mask);
	len = last_byte - first_byte + 1;

	if (mcp251xfd_update_bits_read_reg(reg)) {
		struct spi_transfer xfer[2] = { };
		struct spi_message msg;

		spi_message_init(&msg);
		spi_message_add_tail(&xfer[0], &msg);

		if (priv->devtype_data.quirks & MCP251XFD_QUIRK_HALF_DUPLEX) {
			xfer[0].tx_buf = buf_tx;
			xfer[0].len = sizeof(buf_tx->cmd);

			xfer[1].rx_buf = buf_rx->data;
			xfer[1].len = len;
			spi_message_add_tail(&xfer[1], &msg);
		} else {
			xfer[0].tx_buf = buf_tx;
			xfer[0].rx_buf = buf_rx;
			xfer[0].len = sizeof(buf_tx->cmd) + len;

			if (MCP251XFD_SANITIZE_SPI)
				memset(buf_tx->data, 0x0, len);
		}

		mcp251xfd_spi_cmd_read_nocrc(&buf_tx->cmd, reg + first_byte);
		err = spi_sync(spi, &msg);
		if (err)
			return err;

		memcpy(&orig_le32, buf_rx->data, len);
	}

	mask_le32 = cpu_to_le32(mask >> BITS_PER_BYTE * first_byte);
	val_le32 = cpu_to_le32(val >> BITS_PER_BYTE * first_byte);

	tmp_le32 = orig_le32 & ~mask_le32;
	tmp_le32 |= val_le32 & mask_le32;

	mcp251xfd_spi_cmd_write_nocrc(&buf_tx->cmd, reg + first_byte);
	memcpy(buf_tx->data, &tmp_le32, len);

	return spi_write(spi, buf_tx, sizeof(buf_tx->cmd) + len);
}

static int
mcp251xfd_regmap_nocrc_read(void *context,
			    const void *reg, size_t reg_len,
			    void *val_buf, size_t val_len)
{
	struct spi_device *spi = context;
	struct mcp251xfd_priv *priv = spi_get_drvdata(spi);
	struct mcp251xfd_map_buf_nocrc *buf_rx = priv->map_buf_nocrc_rx;
	struct mcp251xfd_map_buf_nocrc *buf_tx = priv->map_buf_nocrc_tx;
	struct spi_transfer xfer[2] = { };
	struct spi_message msg;
	int err;

	BUILD_BUG_ON(sizeof(buf_rx->cmd) != sizeof(__be16));
	BUILD_BUG_ON(sizeof(buf_tx->cmd) != sizeof(__be16));

	if (IS_ENABLED(CONFIG_CAN_MCP251XFD_SANITY) &&
	    reg_len != sizeof(buf_tx->cmd.cmd))
		return -EINVAL;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer[0], &msg);

	if (priv->devtype_data.quirks & MCP251XFD_QUIRK_HALF_DUPLEX) {
		xfer[0].tx_buf = reg;
		xfer[0].len = sizeof(buf_tx->cmd);

		xfer[1].rx_buf = val_buf;
		xfer[1].len = val_len;
		spi_message_add_tail(&xfer[1], &msg);
	} else {
		xfer[0].tx_buf = buf_tx;
		xfer[0].rx_buf = buf_rx;
		xfer[0].len = sizeof(buf_tx->cmd) + val_len;

		memcpy(&buf_tx->cmd, reg, sizeof(buf_tx->cmd));
		if (MCP251XFD_SANITIZE_SPI)
			memset(buf_tx->data, 0x0, val_len);
	}

	err = spi_sync(spi, &msg);
	if (err)
		return err;

	if (!(priv->devtype_data.quirks & MCP251XFD_QUIRK_HALF_DUPLEX))
		memcpy(val_buf, buf_rx->data, val_len);

	return 0;
}

static int
mcp251xfd_regmap_crc_gather_write(void *context,
				  const void *reg_p, size_t reg_len,
				  const void *val, size_t val_len)
{
	struct spi_device *spi = context;
	struct mcp251xfd_priv *priv = spi_get_drvdata(spi);
	struct mcp251xfd_map_buf_crc *buf_tx = priv->map_buf_crc_tx;
	struct spi_transfer xfer[] = {
		{
			.tx_buf = buf_tx,
			.len = sizeof(buf_tx->cmd) + val_len +
				sizeof(buf_tx->crc),
		},
	};
	u16 reg = *(u16 *)reg_p;
	u16 crc;

	BUILD_BUG_ON(sizeof(buf_tx->cmd) != sizeof(__be16) + sizeof(u8));

	if (IS_ENABLED(CONFIG_CAN_MCP251XFD_SANITY) &&
	    reg_len != sizeof(buf_tx->cmd.cmd) +
	    mcp251xfd_regmap_crc.pad_bits / BITS_PER_BYTE)
		return -EINVAL;

	mcp251xfd_spi_cmd_write_crc(&buf_tx->cmd, reg, val_len);
	memcpy(buf_tx->data, val, val_len);

	crc = mcp251xfd_crc16_compute(buf_tx, sizeof(buf_tx->cmd) + val_len);
	put_unaligned_be16(crc, buf_tx->data + val_len);

	return spi_sync_transfer(spi, xfer, ARRAY_SIZE(xfer));
}

static int
mcp251xfd_regmap_crc_write(void *context,
			   const void *data, size_t count)
{
	const size_t data_offset = sizeof(__be16) +
		mcp251xfd_regmap_crc.pad_bits / BITS_PER_BYTE;

	return mcp251xfd_regmap_crc_gather_write(context,
						 data, data_offset,
						 data + data_offset,
						 count - data_offset);
}

static int
mcp251xfd_regmap_crc_read_check_crc(const struct mcp251xfd_map_buf_crc * const buf_rx,
				    const struct mcp251xfd_map_buf_crc * const buf_tx,
				    unsigned int data_len)
{
	u16 crc_received, crc_calculated;

	crc_received = get_unaligned_be16(buf_rx->data + data_len);
	crc_calculated = mcp251xfd_crc16_compute2(&buf_tx->cmd,
						  sizeof(buf_tx->cmd),
						  buf_rx->data,
						  data_len);
	if (crc_received != crc_calculated)
		return -EBADMSG;

	return 0;
}


static int
mcp251xfd_regmap_crc_read_one(struct mcp251xfd_priv *priv,
			      struct spi_message *msg, unsigned int data_len)
{
	const struct mcp251xfd_map_buf_crc *buf_rx = priv->map_buf_crc_rx;
	const struct mcp251xfd_map_buf_crc *buf_tx = priv->map_buf_crc_tx;
	int err;

	BUILD_BUG_ON(sizeof(buf_rx->cmd) != sizeof(__be16) + sizeof(u8));
	BUILD_BUG_ON(sizeof(buf_tx->cmd) != sizeof(__be16) + sizeof(u8));

	err = spi_sync(priv->spi, msg);
	if (err)
		return err;

	return mcp251xfd_regmap_crc_read_check_crc(buf_rx, buf_tx, data_len);
}

static int
mcp251xfd_regmap_crc_read(void *context,
			  const void *reg_p, size_t reg_len,
			  void *val_buf, size_t val_len)
{
	struct spi_device *spi = context;
	struct mcp251xfd_priv *priv = spi_get_drvdata(spi);
	struct mcp251xfd_map_buf_crc *buf_rx = priv->map_buf_crc_rx;
	struct mcp251xfd_map_buf_crc *buf_tx = priv->map_buf_crc_tx;
	struct spi_transfer xfer[2] = { };
	struct spi_message msg;
	u16 reg = *(u16 *)reg_p;
	int i, err;

	BUILD_BUG_ON(sizeof(buf_rx->cmd) != sizeof(__be16) + sizeof(u8));
	BUILD_BUG_ON(sizeof(buf_tx->cmd) != sizeof(__be16) + sizeof(u8));

	if (IS_ENABLED(CONFIG_CAN_MCP251XFD_SANITY) &&
	    reg_len != sizeof(buf_tx->cmd.cmd) +
	    mcp251xfd_regmap_crc.pad_bits / BITS_PER_BYTE)
		return -EINVAL;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer[0], &msg);

	if (priv->devtype_data.quirks & MCP251XFD_QUIRK_HALF_DUPLEX) {
		xfer[0].tx_buf = buf_tx;
		xfer[0].len = sizeof(buf_tx->cmd);

		xfer[1].rx_buf = buf_rx->data;
		xfer[1].len = val_len + sizeof(buf_tx->crc);
		spi_message_add_tail(&xfer[1], &msg);
	} else {
		xfer[0].tx_buf = buf_tx;
		xfer[0].rx_buf = buf_rx;
		xfer[0].len = sizeof(buf_tx->cmd) + val_len +
			sizeof(buf_tx->crc);

		if (MCP251XFD_SANITIZE_SPI)
			memset(buf_tx->data, 0x0, val_len +
			       sizeof(buf_tx->crc));
	}

	mcp251xfd_spi_cmd_read_crc(&buf_tx->cmd, reg, val_len);

	for (i = 0; i < MCP251XFD_READ_CRC_RETRIES_MAX; i++) {
		err = mcp251xfd_regmap_crc_read_one(priv, &msg, val_len);
		if (!err)
			goto out;
		if (err != -EBADMSG)
			return err;

		/* MCP251XFD_REG_TBC is the time base counter
		 * register. It increments once per SYS clock tick,
		 * which is 20 or 40 MHz.
		 *
		 * Observation shows that if the lowest byte (which is
		 * transferred first on the SPI bus) of that register
		 * is 0x00 or 0x80 the calculated CRC doesn't always
		 * match the transferred one.
		 *
		 * If the highest bit in the lowest byte is flipped
		 * the transferred CRC matches the calculated one. We
		 * assume for now the CRC calculation in the chip
		 * works on wrong data and the transferred data is
		 * correct.
		 */
		if (reg == MCP251XFD_REG_TBC &&
		    (buf_rx->data[0] == 0x0 || buf_rx->data[0] == 0x80)) {
			/* Flip highest bit in lowest byte of le32 */
			buf_rx->data[0] ^= 0x80;

			/* re-check CRC */
			err = mcp251xfd_regmap_crc_read_check_crc(buf_rx,
								  buf_tx,
								  val_len);
			if (!err) {
				/* If CRC is now correct, assume
				 * transferred data was OK, flip bit
				 * back to original value.
				 */
				buf_rx->data[0] ^= 0x80;
				goto out;
			}
		}

		/* MCP251XFD_REG_OSC is the first ever reg we read from.
		 *
		 * The chip may be in deep sleep and this SPI transfer
		 * (i.e. the assertion of the CS) will wake the chip
		 * up. This takes about 3ms. The CRC of this transfer
		 * is wrong.
		 *
		 * Or there isn't a chip at all, in this case the CRC
		 * will be wrong, too.
		 *
		 * In both cases ignore the CRC and copy the read data
		 * to the caller. It will take care of both cases.
		 *
		 */
		if (reg == MCP251XFD_REG_OSC) {
			err = 0;
			goto out;
		}

		netdev_info(priv->ndev,
			    "CRC read error at address 0x%04x (length=%zd, data=%*ph, CRC=0x%04x) retrying.\n",
			    reg, val_len, (int)val_len, buf_rx->data,
			    get_unaligned_be16(buf_rx->data + val_len));
	}

	if (err) {
		netdev_err(priv->ndev,
			   "CRC read error at address 0x%04x (length=%zd, data=%*ph, CRC=0x%04x).\n",
			   reg, val_len, (int)val_len, buf_rx->data,
			   get_unaligned_be16(buf_rx->data + val_len));

		return err;
	}
 out:
	memcpy(val_buf, buf_rx->data, val_len);

	return 0;
}

static const struct regmap_range mcp251xfd_reg_table_yes_range[] = {
	regmap_reg_range(0x000, 0x2ec),	/* CAN FD Controller Module SFR */
	regmap_reg_range(0x400, 0xbfc),	/* RAM */
	regmap_reg_range(0xe00, 0xe14),	/* MCP2517/18FD SFR */
};

static const struct regmap_access_table mcp251xfd_reg_table = {
	.yes_ranges = mcp251xfd_reg_table_yes_range,
	.n_yes_ranges = ARRAY_SIZE(mcp251xfd_reg_table_yes_range),
};

static const struct regmap_config mcp251xfd_regmap_nocrc = {
	.name = "nocrc",
	.reg_bits = 16,
	.reg_stride = 4,
	.pad_bits = 0,
	.val_bits = 32,
	.max_register = 0xffc,
	.wr_table = &mcp251xfd_reg_table,
	.rd_table = &mcp251xfd_reg_table,
	.cache_type = REGCACHE_NONE,
	.read_flag_mask = (__force unsigned long)
		cpu_to_be16(MCP251XFD_SPI_INSTRUCTION_READ),
	.write_flag_mask = (__force unsigned long)
		cpu_to_be16(MCP251XFD_SPI_INSTRUCTION_WRITE),
};

static const struct regmap_bus mcp251xfd_bus_nocrc = {
	.write = mcp251xfd_regmap_nocrc_write,
	.gather_write = mcp251xfd_regmap_nocrc_gather_write,
	.reg_update_bits = mcp251xfd_regmap_nocrc_update_bits,
	.read = mcp251xfd_regmap_nocrc_read,
	.reg_format_endian_default = REGMAP_ENDIAN_BIG,
	.val_format_endian_default = REGMAP_ENDIAN_LITTLE,
	.max_raw_read = sizeof_field(struct mcp251xfd_map_buf_nocrc, data),
	.max_raw_write = sizeof_field(struct mcp251xfd_map_buf_nocrc, data),
};

static const struct regmap_config mcp251xfd_regmap_crc = {
	.name = "crc",
	.reg_bits = 16,
	.reg_stride = 4,
	.pad_bits = 16,		/* keep data bits aligned */
	.val_bits = 32,
	.max_register = 0xffc,
	.wr_table = &mcp251xfd_reg_table,
	.rd_table = &mcp251xfd_reg_table,
	.cache_type = REGCACHE_NONE,
};

static const struct regmap_bus mcp251xfd_bus_crc = {
	.write = mcp251xfd_regmap_crc_write,
	.gather_write = mcp251xfd_regmap_crc_gather_write,
	.read = mcp251xfd_regmap_crc_read,
	.reg_format_endian_default = REGMAP_ENDIAN_NATIVE,
	.val_format_endian_default = REGMAP_ENDIAN_LITTLE,
	.max_raw_read = sizeof_field(struct mcp251xfd_map_buf_crc, data),
	.max_raw_write = sizeof_field(struct mcp251xfd_map_buf_crc, data),
};

static inline bool
mcp251xfd_regmap_use_nocrc(struct mcp251xfd_priv *priv)
{
	return (!(priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_REG)) ||
		(!(priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_RX));
}

static inline bool
mcp251xfd_regmap_use_crc(struct mcp251xfd_priv *priv)
{
	return (priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_REG) ||
		(priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_RX);
}

static int
mcp251xfd_regmap_init_nocrc(struct mcp251xfd_priv *priv)
{
	if (!priv->map_nocrc) {
		struct regmap *map;

		map = devm_regmap_init(&priv->spi->dev, &mcp251xfd_bus_nocrc,
				       priv->spi, &mcp251xfd_regmap_nocrc);
		if (IS_ERR(map))
			return PTR_ERR(map);

		priv->map_nocrc = map;
	}

	if (!priv->map_buf_nocrc_rx) {
		priv->map_buf_nocrc_rx =
			devm_kzalloc(&priv->spi->dev,
				     sizeof(*priv->map_buf_nocrc_rx),
				     GFP_KERNEL);
		if (!priv->map_buf_nocrc_rx)
			return -ENOMEM;
	}

	if (!priv->map_buf_nocrc_tx) {
		priv->map_buf_nocrc_tx =
			devm_kzalloc(&priv->spi->dev,
				     sizeof(*priv->map_buf_nocrc_tx),
				     GFP_KERNEL);
		if (!priv->map_buf_nocrc_tx)
			return -ENOMEM;
	}

	if (!(priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_REG))
		priv->map_reg = priv->map_nocrc;

	if (!(priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_RX))
		priv->map_rx = priv->map_nocrc;

	return 0;
}

static void mcp251xfd_regmap_destroy_nocrc(struct mcp251xfd_priv *priv)
{
	if (priv->map_buf_nocrc_rx) {
		devm_kfree(&priv->spi->dev, priv->map_buf_nocrc_rx);
		priv->map_buf_nocrc_rx = NULL;
	}
	if (priv->map_buf_nocrc_tx) {
		devm_kfree(&priv->spi->dev, priv->map_buf_nocrc_tx);
		priv->map_buf_nocrc_tx = NULL;
	}
}

static int
mcp251xfd_regmap_init_crc(struct mcp251xfd_priv *priv)
{
	if (!priv->map_crc) {
		struct regmap *map;

		map = devm_regmap_init(&priv->spi->dev, &mcp251xfd_bus_crc,
				       priv->spi, &mcp251xfd_regmap_crc);
		if (IS_ERR(map))
			return PTR_ERR(map);

		priv->map_crc = map;
	}

	if (!priv->map_buf_crc_rx) {
		priv->map_buf_crc_rx =
			devm_kzalloc(&priv->spi->dev,
				     sizeof(*priv->map_buf_crc_rx),
				     GFP_KERNEL);
		if (!priv->map_buf_crc_rx)
			return -ENOMEM;
	}

	if (!priv->map_buf_crc_tx) {
		priv->map_buf_crc_tx =
			devm_kzalloc(&priv->spi->dev,
				     sizeof(*priv->map_buf_crc_tx),
				     GFP_KERNEL);
		if (!priv->map_buf_crc_tx)
			return -ENOMEM;
	}

	if (priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_REG)
		priv->map_reg = priv->map_crc;

	if (priv->devtype_data.quirks & MCP251XFD_QUIRK_CRC_RX)
		priv->map_rx = priv->map_crc;

	return 0;
}

static void mcp251xfd_regmap_destroy_crc(struct mcp251xfd_priv *priv)
{
	if (priv->map_buf_crc_rx) {
		devm_kfree(&priv->spi->dev, priv->map_buf_crc_rx);
		priv->map_buf_crc_rx = NULL;
	}
	if (priv->map_buf_crc_tx) {
		devm_kfree(&priv->spi->dev, priv->map_buf_crc_tx);
		priv->map_buf_crc_tx = NULL;
	}
}

int mcp251xfd_regmap_init(struct mcp251xfd_priv *priv)
{
	int err;

	if (mcp251xfd_regmap_use_nocrc(priv)) {
		err = mcp251xfd_regmap_init_nocrc(priv);

		if (err)
			return err;
	} else {
		mcp251xfd_regmap_destroy_nocrc(priv);
	}

	if (mcp251xfd_regmap_use_crc(priv)) {
		err = mcp251xfd_regmap_init_crc(priv);

		if (err)
			return err;
	} else {
		mcp251xfd_regmap_destroy_crc(priv);
	}

	return 0;
}

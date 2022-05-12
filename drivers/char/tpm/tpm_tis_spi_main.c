// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2015 Infineon Technologies AG
 * Copyright (C) 2016 STMicroelectronics SAS
 *
 * Authors:
 * Peter Huewe <peter.huewe@infineon.com>
 * Christophe Ricard <christophe-h.ricard@st.com>
 *
 * Maintained by: <tpmdd-devel@lists.sourceforge.net>
 *
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * This device driver implements the TPM interface as defined in
 * the TCG TPM Interface Spec version 1.3, revision 27 via _raw/native
 * SPI access_.
 *
 * It is based on the original tpm_tis device driver from Leendert van
 * Dorn and Kyleen Hall and Jarko Sakkinnen.
 */

#include <linux/acpi.h>
#include <linux/completion.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/of_device.h>
#include <linux/spi/spi.h>
#include <linux/tpm.h>

#include "tpm.h"
#include "tpm_tis_core.h"
#include "tpm_tis_spi.h"

#define MAX_SPI_FRAMESIZE 64

/*
 * TCG SPI flow control is documented in section 6.4 of the spec[1]. In short,
 * keep trying to read from the device until MISO goes high indicating the
 * wait state has ended.
 *
 * [1] https://trustedcomputinggroup.org/resource/pc-client-platform-tpm-profile-ptp-specification/
 */
static int tpm_tis_spi_flow_control(struct tpm_tis_spi_phy *phy,
				    struct spi_transfer *spi_xfer)
{
	struct spi_message m;
	int ret, i;

	if ((phy->iobuf[3] & 0x01) == 0) {
		// handle SPI wait states
		for (i = 0; i < TPM_RETRY; i++) {
			spi_xfer->len = 1;
			spi_message_init(&m);
			spi_message_add_tail(spi_xfer, &m);
			ret = spi_sync_locked(phy->spi_device, &m);
			if (ret < 0)
				return ret;
			if (phy->iobuf[0] & 0x01)
				break;
		}

		if (i == TPM_RETRY)
			return -ETIMEDOUT;
	}

	return 0;
}

int tpm_tis_spi_transfer(struct tpm_tis_data *data, u32 addr, u16 len,
			 u8 *in, const u8 *out)
{
	struct tpm_tis_spi_phy *phy = to_tpm_tis_spi_phy(data);
	int ret = 0;
	struct spi_message m;
	struct spi_transfer spi_xfer;
	u8 transfer_len;

	spi_bus_lock(phy->spi_device->master);

	while (len) {
		transfer_len = min_t(u16, len, MAX_SPI_FRAMESIZE);

		phy->iobuf[0] = (in ? 0x80 : 0) | (transfer_len - 1);
		phy->iobuf[1] = 0xd4;
		phy->iobuf[2] = addr >> 8;
		phy->iobuf[3] = addr;

		memset(&spi_xfer, 0, sizeof(spi_xfer));
		spi_xfer.tx_buf = phy->iobuf;
		spi_xfer.rx_buf = phy->iobuf;
		spi_xfer.len = 4;
		spi_xfer.cs_change = 1;

		spi_message_init(&m);
		spi_message_add_tail(&spi_xfer, &m);
		ret = spi_sync_locked(phy->spi_device, &m);
		if (ret < 0)
			goto exit;

		/* Flow control transfers are receive only */
		spi_xfer.tx_buf = NULL;
		ret = phy->flow_control(phy, &spi_xfer);
		if (ret < 0)
			goto exit;

		spi_xfer.cs_change = 0;
		spi_xfer.len = transfer_len;
		spi_xfer.delay.value = 5;
		spi_xfer.delay.unit = SPI_DELAY_UNIT_USECS;

		if (out) {
			spi_xfer.tx_buf = phy->iobuf;
			spi_xfer.rx_buf = NULL;
			memcpy(phy->iobuf, out, transfer_len);
			out += transfer_len;
		}

		spi_message_init(&m);
		spi_message_add_tail(&spi_xfer, &m);
		reinit_completion(&phy->ready);
		ret = spi_sync_locked(phy->spi_device, &m);
		if (ret < 0)
			goto exit;

		if (in) {
			memcpy(in, phy->iobuf, transfer_len);
			in += transfer_len;
		}

		len -= transfer_len;
	}

exit:
	spi_bus_unlock(phy->spi_device->master);
	return ret;
}

static int tpm_tis_spi_read_bytes(struct tpm_tis_data *data, u32 addr,
				  u16 len, u8 *result)
{
	return tpm_tis_spi_transfer(data, addr, len, result, NULL);
}

static int tpm_tis_spi_write_bytes(struct tpm_tis_data *data, u32 addr,
				   u16 len, const u8 *value)
{
	return tpm_tis_spi_transfer(data, addr, len, NULL, value);
}

int tpm_tis_spi_read16(struct tpm_tis_data *data, u32 addr, u16 *result)
{
	__le16 result_le;
	int rc;

	rc = data->phy_ops->read_bytes(data, addr, sizeof(u16),
				       (u8 *)&result_le);
	if (!rc)
		*result = le16_to_cpu(result_le);

	return rc;
}

int tpm_tis_spi_read32(struct tpm_tis_data *data, u32 addr, u32 *result)
{
	__le32 result_le;
	int rc;

	rc = data->phy_ops->read_bytes(data, addr, sizeof(u32),
				       (u8 *)&result_le);
	if (!rc)
		*result = le32_to_cpu(result_le);

	return rc;
}

int tpm_tis_spi_write32(struct tpm_tis_data *data, u32 addr, u32 value)
{
	__le32 value_le;
	int rc;

	value_le = cpu_to_le32(value);
	rc = data->phy_ops->write_bytes(data, addr, sizeof(u32),
					(u8 *)&value_le);

	return rc;
}

int tpm_tis_spi_init(struct spi_device *spi, struct tpm_tis_spi_phy *phy,
		     int irq, const struct tpm_tis_phy_ops *phy_ops)
{
	phy->iobuf = devm_kmalloc(&spi->dev, MAX_SPI_FRAMESIZE, GFP_KERNEL);
	if (!phy->iobuf)
		return -ENOMEM;

	phy->spi_device = spi;

	return tpm_tis_core_init(&spi->dev, &phy->priv, irq, phy_ops, NULL);
}

static const struct tpm_tis_phy_ops tpm_spi_phy_ops = {
	.read_bytes = tpm_tis_spi_read_bytes,
	.write_bytes = tpm_tis_spi_write_bytes,
	.read16 = tpm_tis_spi_read16,
	.read32 = tpm_tis_spi_read32,
	.write32 = tpm_tis_spi_write32,
};

static int tpm_tis_spi_probe(struct spi_device *dev)
{
	struct tpm_tis_spi_phy *phy;
	int irq;

	phy = devm_kzalloc(&dev->dev, sizeof(struct tpm_tis_spi_phy),
			   GFP_KERNEL);
	if (!phy)
		return -ENOMEM;

	phy->flow_control = tpm_tis_spi_flow_control;

	/* If the SPI device has an IRQ then use that */
	if (dev->irq > 0)
		irq = dev->irq;
	else
		irq = -1;

	init_completion(&phy->ready);
	return tpm_tis_spi_init(dev, phy, irq, &tpm_spi_phy_ops);
}

typedef int (*tpm_tis_spi_probe_func)(struct spi_device *);

static int tpm_tis_spi_driver_probe(struct spi_device *spi)
{
	const struct spi_device_id *spi_dev_id = spi_get_device_id(spi);
	tpm_tis_spi_probe_func probe_func;

	probe_func = of_device_get_match_data(&spi->dev);
	if (!probe_func && spi_dev_id)
		probe_func = (tpm_tis_spi_probe_func)spi_dev_id->driver_data;
	if (!probe_func)
		return -ENODEV;

	return probe_func(spi);
}

static SIMPLE_DEV_PM_OPS(tpm_tis_pm, tpm_pm_suspend, tpm_tis_spi_resume);

static int tpm_tis_spi_remove(struct spi_device *dev)
{
	struct tpm_chip *chip = spi_get_drvdata(dev);

	tpm_chip_unregister(chip);
	tpm_tis_remove(chip);
	return 0;
}

static const struct spi_device_id tpm_tis_spi_id[] = {
	{ "tpm_tis_spi", (unsigned long)tpm_tis_spi_probe },
	{ "cr50", (unsigned long)cr50_spi_probe },
	{}
};
MODULE_DEVICE_TABLE(spi, tpm_tis_spi_id);

static const struct of_device_id of_tis_spi_match[] = {
	{ .compatible = "st,st33htpm-spi", .data = tpm_tis_spi_probe },
	{ .compatible = "infineon,slb9670", .data = tpm_tis_spi_probe },
	{ .compatible = "tcg,tpm_tis-spi", .data = tpm_tis_spi_probe },
	{ .compatible = "google,cr50", .data = cr50_spi_probe },
	{}
};
MODULE_DEVICE_TABLE(of, of_tis_spi_match);

static const struct acpi_device_id acpi_tis_spi_match[] = {
	{"SMO0768", 0},
	{}
};
MODULE_DEVICE_TABLE(acpi, acpi_tis_spi_match);

static struct spi_driver tpm_tis_spi_driver = {
	.driver = {
		.name = "tpm_tis_spi",
		.pm = &tpm_tis_pm,
		.of_match_table = of_match_ptr(of_tis_spi_match),
		.acpi_match_table = ACPI_PTR(acpi_tis_spi_match),
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
	},
	.probe = tpm_tis_spi_driver_probe,
	.remove = tpm_tis_spi_remove,
	.id_table = tpm_tis_spi_id,
};
module_spi_driver(tpm_tis_spi_driver);

MODULE_DESCRIPTION("TPM Driver for native SPI access");
MODULE_LICENSE("GPL");

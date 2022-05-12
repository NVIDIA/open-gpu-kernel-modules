// SPDX-License-Identifier: GPL-2.0-or-later
/*
    Copyright (c) 2003 Mark M. Hoffman <mhoffman@lightlink.com>

*/

/*
    This module must be considered BETA unless and until
    the chipset manufacturer releases a datasheet.
    The register definitions are based on the SiS630.

    This module relies on quirk_sis_96x_smbus (drivers/pci/quirks.c)
    for just about every machine for which users have reported.
    If this module isn't detecting your 96x south bridge, have a 
    look there.

    We assume there can only be one SiS96x with one SMBus interface.
*/

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/stddef.h>
#include <linux/ioport.h>
#include <linux/i2c.h>
#include <linux/acpi.h>
#include <linux/io.h>

/* base address register in PCI config space */
#define SIS96x_BAR 0x04

/* SiS96x SMBus registers */
#define SMB_STS      0x00
#define SMB_EN       0x01
#define SMB_CNT      0x02
#define SMB_HOST_CNT 0x03
#define SMB_ADDR     0x04
#define SMB_CMD      0x05
#define SMB_PCOUNT   0x06
#define SMB_COUNT    0x07
#define SMB_BYTE     0x08
#define SMB_DEV_ADDR 0x10
#define SMB_DB0      0x11
#define SMB_DB1      0x12
#define SMB_SAA      0x13

/* register count for request_region */
#define SMB_IOSIZE 0x20

/* Other settings */
#define MAX_TIMEOUT 500

/* SiS96x SMBus constants */
#define SIS96x_QUICK      0x00
#define SIS96x_BYTE       0x01
#define SIS96x_BYTE_DATA  0x02
#define SIS96x_WORD_DATA  0x03
#define SIS96x_PROC_CALL  0x04
#define SIS96x_BLOCK_DATA 0x05

static struct pci_driver sis96x_driver;
static struct i2c_adapter sis96x_adapter;
static u16 sis96x_smbus_base;

static inline u8 sis96x_read(u8 reg)
{
	return inb(sis96x_smbus_base + reg) ;
}

static inline void sis96x_write(u8 reg, u8 data)
{
	outb(data, sis96x_smbus_base + reg) ;
}

/* Execute a SMBus transaction.
   int size is from SIS96x_QUICK to SIS96x_BLOCK_DATA
 */
static int sis96x_transaction(int size)
{
	int temp;
	int result = 0;
	int timeout = 0;

	dev_dbg(&sis96x_adapter.dev, "SMBus transaction %d\n", size);

	/* Make sure the SMBus host is ready to start transmitting */
	if (((temp = sis96x_read(SMB_CNT)) & 0x03) != 0x00) {

		dev_dbg(&sis96x_adapter.dev, "SMBus busy (0x%02x). "
			"Resetting...\n", temp);

		/* kill the transaction */
		sis96x_write(SMB_HOST_CNT, 0x20);

		/* check it again */
		if (((temp = sis96x_read(SMB_CNT)) & 0x03) != 0x00) {
			dev_dbg(&sis96x_adapter.dev, "Failed (0x%02x)\n", temp);
			return -EBUSY;
		} else {
			dev_dbg(&sis96x_adapter.dev, "Successful\n");
		}
	}

	/* Turn off timeout interrupts, set fast host clock */
	sis96x_write(SMB_CNT, 0x20);

	/* clear all (sticky) status flags */
	temp = sis96x_read(SMB_STS);
	sis96x_write(SMB_STS, temp & 0x1e);

	/* start the transaction by setting bit 4 and size bits */
	sis96x_write(SMB_HOST_CNT, 0x10 | (size & 0x07));

	/* We will always wait for a fraction of a second! */
	do {
		msleep(1);
		temp = sis96x_read(SMB_STS);
	} while (!(temp & 0x0e) && (timeout++ < MAX_TIMEOUT));

	/* If the SMBus is still busy, we give up */
	if (timeout > MAX_TIMEOUT) {
		dev_dbg(&sis96x_adapter.dev, "SMBus Timeout! (0x%02x)\n", temp);
		result = -ETIMEDOUT;
	}

	/* device error - probably missing ACK */
	if (temp & 0x02) {
		dev_dbg(&sis96x_adapter.dev, "Failed bus transaction!\n");
		result = -ENXIO;
	}

	/* bus collision */
	if (temp & 0x04) {
		dev_dbg(&sis96x_adapter.dev, "Bus collision!\n");
		result = -EIO;
	}

	/* Finish up by resetting the bus */
	sis96x_write(SMB_STS, temp);
	if ((temp = sis96x_read(SMB_STS))) {
		dev_dbg(&sis96x_adapter.dev, "Failed reset at "
			"end of transaction! (0x%02x)\n", temp);
	}

	return result;
}

/* Return negative errno on error. */
static s32 sis96x_access(struct i2c_adapter * adap, u16 addr,
			 unsigned short flags, char read_write,
			 u8 command, int size, union i2c_smbus_data * data)
{
	int status;

	switch (size) {
	case I2C_SMBUS_QUICK:
		sis96x_write(SMB_ADDR, ((addr & 0x7f) << 1) | (read_write & 0x01));
		size = SIS96x_QUICK;
		break;

	case I2C_SMBUS_BYTE:
		sis96x_write(SMB_ADDR, ((addr & 0x7f) << 1) | (read_write & 0x01));
		if (read_write == I2C_SMBUS_WRITE)
			sis96x_write(SMB_CMD, command);
		size = SIS96x_BYTE;
		break;

	case I2C_SMBUS_BYTE_DATA:
		sis96x_write(SMB_ADDR, ((addr & 0x7f) << 1) | (read_write & 0x01));
		sis96x_write(SMB_CMD, command);
		if (read_write == I2C_SMBUS_WRITE)
			sis96x_write(SMB_BYTE, data->byte);
		size = SIS96x_BYTE_DATA;
		break;

	case I2C_SMBUS_PROC_CALL:
	case I2C_SMBUS_WORD_DATA:
		sis96x_write(SMB_ADDR, ((addr & 0x7f) << 1) | (read_write & 0x01));
		sis96x_write(SMB_CMD, command);
		if (read_write == I2C_SMBUS_WRITE) {
			sis96x_write(SMB_BYTE, data->word & 0xff);
			sis96x_write(SMB_BYTE + 1, (data->word & 0xff00) >> 8);
		}
		size = (size == I2C_SMBUS_PROC_CALL ? 
			SIS96x_PROC_CALL : SIS96x_WORD_DATA);
		break;

	default:
		dev_warn(&adap->dev, "Unsupported transaction %d\n", size);
		return -EOPNOTSUPP;
	}

	status = sis96x_transaction(size);
	if (status)
		return status;

	if ((size != SIS96x_PROC_CALL) &&
		((read_write == I2C_SMBUS_WRITE) || (size == SIS96x_QUICK)))
		return 0;

	switch (size) {
	case SIS96x_BYTE:
	case SIS96x_BYTE_DATA:
		data->byte = sis96x_read(SMB_BYTE);
		break;

	case SIS96x_WORD_DATA:
	case SIS96x_PROC_CALL:
		data->word = sis96x_read(SMB_BYTE) +
				(sis96x_read(SMB_BYTE + 1) << 8);
		break;
	}
	return 0;
}

static u32 sis96x_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE |
	    I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |
	    I2C_FUNC_SMBUS_PROC_CALL;
}

static const struct i2c_algorithm smbus_algorithm = {
	.smbus_xfer	= sis96x_access,
	.functionality	= sis96x_func,
};

static struct i2c_adapter sis96x_adapter = {
	.owner		= THIS_MODULE,
	.class		= I2C_CLASS_HWMON | I2C_CLASS_SPD,
	.algo		= &smbus_algorithm,
};

static const struct pci_device_id sis96x_ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_SMBUS) },
	{ 0, }
};

MODULE_DEVICE_TABLE (pci, sis96x_ids);

static int sis96x_probe(struct pci_dev *dev,
				const struct pci_device_id *id)
{
	u16 ww = 0;
	int retval;

	if (sis96x_smbus_base) {
		dev_err(&dev->dev, "Only one device supported.\n");
		return -EBUSY;
	}

	pci_read_config_word(dev, PCI_CLASS_DEVICE, &ww);
	if (PCI_CLASS_SERIAL_SMBUS != ww) {
		dev_err(&dev->dev, "Unsupported device class 0x%04x!\n", ww);
		return -ENODEV;
	}

	sis96x_smbus_base = pci_resource_start(dev, SIS96x_BAR);
	if (!sis96x_smbus_base) {
		dev_err(&dev->dev, "SiS96x SMBus base address "
			"not initialized!\n");
		return -EINVAL;
	}
	dev_info(&dev->dev, "SiS96x SMBus base address: 0x%04x\n",
			sis96x_smbus_base);

	retval = acpi_check_resource_conflict(&dev->resource[SIS96x_BAR]);
	if (retval)
		return -ENODEV;

	/* Everything is happy, let's grab the memory and set things up. */
	if (!request_region(sis96x_smbus_base, SMB_IOSIZE,
			    sis96x_driver.name)) {
		dev_err(&dev->dev, "SMBus registers 0x%04x-0x%04x "
			"already in use!\n", sis96x_smbus_base,
			sis96x_smbus_base + SMB_IOSIZE - 1);

		sis96x_smbus_base = 0;
		return -EINVAL;
	}

	/* set up the sysfs linkage to our parent device */
	sis96x_adapter.dev.parent = &dev->dev;

	snprintf(sis96x_adapter.name, sizeof(sis96x_adapter.name),
		"SiS96x SMBus adapter at 0x%04x", sis96x_smbus_base);

	if ((retval = i2c_add_adapter(&sis96x_adapter))) {
		dev_err(&dev->dev, "Couldn't register adapter!\n");
		release_region(sis96x_smbus_base, SMB_IOSIZE);
		sis96x_smbus_base = 0;
	}

	return retval;
}

static void sis96x_remove(struct pci_dev *dev)
{
	if (sis96x_smbus_base) {
		i2c_del_adapter(&sis96x_adapter);
		release_region(sis96x_smbus_base, SMB_IOSIZE);
		sis96x_smbus_base = 0;
	}
}

static struct pci_driver sis96x_driver = {
	.name		= "sis96x_smbus",
	.id_table	= sis96x_ids,
	.probe		= sis96x_probe,
	.remove		= sis96x_remove,
};

module_pci_driver(sis96x_driver);

MODULE_AUTHOR("Mark M. Hoffman <mhoffman@lightlink.com>");
MODULE_DESCRIPTION("SiS96x SMBus driver");
MODULE_LICENSE("GPL");

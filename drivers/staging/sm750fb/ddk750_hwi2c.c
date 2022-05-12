// SPDX-License-Identifier: GPL-2.0
#define USE_HW_I2C
#ifdef USE_HW_I2C
#include "ddk750_chip.h"
#include "ddk750_reg.h"
#include "ddk750_hwi2c.h"
#include "ddk750_power.h"

#define MAX_HWI2C_FIFO                  16
#define HWI2C_WAIT_TIMEOUT              0xF0000

int sm750_hw_i2c_init(unsigned char bus_speed_mode)
{
	unsigned int value;

	/* Enable GPIO 30 & 31 as IIC clock & data */
	value = peek32(GPIO_MUX);

	value |= (GPIO_MUX_30 | GPIO_MUX_31);
	poke32(GPIO_MUX, value);

	/*
	 * Enable Hardware I2C power.
	 * TODO: Check if we need to enable GPIO power?
	 */
	sm750_enable_i2c(1);

	/* Enable the I2C Controller and set the bus speed mode */
	value = peek32(I2C_CTRL) & ~(I2C_CTRL_MODE | I2C_CTRL_EN);
	if (bus_speed_mode)
		value |= I2C_CTRL_MODE;
	value |= I2C_CTRL_EN;
	poke32(I2C_CTRL, value);

	return 0;
}

void sm750_hw_i2c_close(void)
{
	unsigned int value;

	/* Disable I2C controller */
	value = peek32(I2C_CTRL) & ~I2C_CTRL_EN;
	poke32(I2C_CTRL, value);

	/* Disable I2C Power */
	sm750_enable_i2c(0);

	/* Set GPIO 30 & 31 back as GPIO pins */
	value = peek32(GPIO_MUX);
	value &= ~GPIO_MUX_30;
	value &= ~GPIO_MUX_31;
	poke32(GPIO_MUX, value);
}

static long hw_i2c_wait_tx_done(void)
{
	unsigned int timeout;

	/* Wait until the transfer is completed. */
	timeout = HWI2C_WAIT_TIMEOUT;
	while (!(peek32(I2C_STATUS) & I2C_STATUS_TX) && (timeout != 0))
		timeout--;

	if (timeout == 0)
		return -1;

	return 0;
}

/*
 *  This function writes data to the i2c slave device registers.
 *
 *  Parameters:
 *      addr            - i2c Slave device address
 *      length          - Total number of bytes to be written to the device
 *      buf             - The buffer that contains the data to be written to the
 *                     i2c device.
 *
 *  Return Value:
 *      Total number of bytes those are actually written.
 */
static unsigned int hw_i2c_write_data(unsigned char addr,
				      unsigned int length,
				      unsigned char *buf)
{
	unsigned char count, i;
	unsigned int total_bytes = 0;

	/* Set the Device Address */
	poke32(I2C_SLAVE_ADDRESS, addr & ~0x01);

	/*
	 * Write data.
	 * Note:
	 *      Only 16 byte can be accessed per i2c start instruction.
	 */
	do {
		/*
		 * Reset I2C by writing 0 to I2C_RESET register to
		 * clear the previous status.
		 */
		poke32(I2C_RESET, 0);

		/* Set the number of bytes to be written */
		if (length < MAX_HWI2C_FIFO)
			count = length - 1;
		else
			count = MAX_HWI2C_FIFO - 1;
		poke32(I2C_BYTE_COUNT, count);

		/* Move the data to the I2C data register */
		for (i = 0; i <= count; i++)
			poke32(I2C_DATA0 + i, *buf++);

		/* Start the I2C */
		poke32(I2C_CTRL, peek32(I2C_CTRL) | I2C_CTRL_CTRL);

		/* Wait until the transfer is completed. */
		if (hw_i2c_wait_tx_done() != 0)
			break;

		/* Subtract length */
		length -= (count + 1);

		/* Total byte written */
		total_bytes += (count + 1);

	} while (length > 0);

	return total_bytes;
}

/*
 *  This function reads data from the slave device and stores them
 *  in the given buffer
 *
 *  Parameters:
 *      addr            - i2c Slave device address
 *      length          - Total number of bytes to be read
 *      buf             - Pointer to a buffer to be filled with the data read
 *                     from the slave device. It has to be the same size as the
 *                     length to make sure that it can keep all the data read.
 *
 *  Return Value:
 *      Total number of actual bytes read from the slave device
 */
static unsigned int hw_i2c_read_data(unsigned char addr,
				     unsigned int length,
				     unsigned char *buf)
{
	unsigned char count, i;
	unsigned int total_bytes = 0;

	/* Set the Device Address */
	poke32(I2C_SLAVE_ADDRESS, addr | 0x01);

	/*
	 * Read data and save them to the buffer.
	 * Note:
	 *      Only 16 byte can be accessed per i2c start instruction.
	 */
	do {
		/*
		 * Reset I2C by writing 0 to I2C_RESET register to
		 * clear all the status.
		 */
		poke32(I2C_RESET, 0);

		/* Set the number of bytes to be read */
		if (length <= MAX_HWI2C_FIFO)
			count = length - 1;
		else
			count = MAX_HWI2C_FIFO - 1;
		poke32(I2C_BYTE_COUNT, count);

		/* Start the I2C */
		poke32(I2C_CTRL, peek32(I2C_CTRL) | I2C_CTRL_CTRL);

		/* Wait until transaction done. */
		if (hw_i2c_wait_tx_done() != 0)
			break;

		/* Save the data to the given buffer */
		for (i = 0; i <= count; i++)
			*buf++ = peek32(I2C_DATA0 + i);

		/* Subtract length by 16 */
		length -= (count + 1);

		/* Number of bytes read. */
		total_bytes += (count + 1);

	} while (length > 0);

	return total_bytes;
}

/*
 *  This function reads the slave device's register
 *
 *  Parameters:
 *      deviceAddress   - i2c Slave device address which register
 *                        to be read from
 *      registerIndex   - Slave device's register to be read
 *
 *  Return Value:
 *      Register value
 */
unsigned char sm750_hw_i2c_read_reg(unsigned char addr, unsigned char reg)
{
	unsigned char value = 0xFF;

	if (hw_i2c_write_data(addr, 1, &reg) == 1)
		hw_i2c_read_data(addr, 1, &value);

	return value;
}

/*
 *  This function writes a value to the slave device's register
 *
 *  Parameters:
 *      deviceAddress   - i2c Slave device address which register
 *                        to be written
 *      registerIndex   - Slave device's register to be written
 *      data            - Data to be written to the register
 *
 *  Result:
 *          0   - Success
 *         -1   - Fail
 */
int sm750_hw_i2c_write_reg(unsigned char addr,
			   unsigned char reg,
			   unsigned char data)
{
	unsigned char value[2];

	value[0] = reg;
	value[1] = data;
	if (hw_i2c_write_data(addr, 2, value) == 2)
		return 0;

	return -1;
}

#endif

// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Cadence SPI controller driver (master mode only)
 *
 * Copyright (C) 2008 - 2014 Xilinx, Inc.
 *
 * based on Blackfin On-Chip SPI Driver (spi_bfin5xx.c)
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/spi/spi.h>

/* Name of this driver */
#define CDNS_SPI_NAME		"cdns-spi"

/* Register offset definitions */
#define CDNS_SPI_CR	0x00 /* Configuration  Register, RW */
#define CDNS_SPI_ISR	0x04 /* Interrupt Status Register, RO */
#define CDNS_SPI_IER	0x08 /* Interrupt Enable Register, WO */
#define CDNS_SPI_IDR	0x0c /* Interrupt Disable Register, WO */
#define CDNS_SPI_IMR	0x10 /* Interrupt Enabled Mask Register, RO */
#define CDNS_SPI_ER	0x14 /* Enable/Disable Register, RW */
#define CDNS_SPI_DR	0x18 /* Delay Register, RW */
#define CDNS_SPI_TXD	0x1C /* Data Transmit Register, WO */
#define CDNS_SPI_RXD	0x20 /* Data Receive Register, RO */
#define CDNS_SPI_SICR	0x24 /* Slave Idle Count Register, RW */
#define CDNS_SPI_THLD	0x28 /* Transmit FIFO Watermark Register,RW */

#define SPI_AUTOSUSPEND_TIMEOUT		3000
/*
 * SPI Configuration Register bit Masks
 *
 * This register contains various control bits that affect the operation
 * of the SPI controller
 */
#define CDNS_SPI_CR_MANSTRT	0x00010000 /* Manual TX Start */
#define CDNS_SPI_CR_CPHA		0x00000004 /* Clock Phase Control */
#define CDNS_SPI_CR_CPOL		0x00000002 /* Clock Polarity Control */
#define CDNS_SPI_CR_SSCTRL		0x00003C00 /* Slave Select Mask */
#define CDNS_SPI_CR_PERI_SEL	0x00000200 /* Peripheral Select Decode */
#define CDNS_SPI_CR_BAUD_DIV	0x00000038 /* Baud Rate Divisor Mask */
#define CDNS_SPI_CR_MSTREN		0x00000001 /* Master Enable Mask */
#define CDNS_SPI_CR_MANSTRTEN	0x00008000 /* Manual TX Enable Mask */
#define CDNS_SPI_CR_SSFORCE	0x00004000 /* Manual SS Enable Mask */
#define CDNS_SPI_CR_BAUD_DIV_4	0x00000008 /* Default Baud Div Mask */
#define CDNS_SPI_CR_DEFAULT	(CDNS_SPI_CR_MSTREN | \
					CDNS_SPI_CR_SSCTRL | \
					CDNS_SPI_CR_SSFORCE | \
					CDNS_SPI_CR_BAUD_DIV_4)

/*
 * SPI Configuration Register - Baud rate and slave select
 *
 * These are the values used in the calculation of baud rate divisor and
 * setting the slave select.
 */

#define CDNS_SPI_BAUD_DIV_MAX		7 /* Baud rate divisor maximum */
#define CDNS_SPI_BAUD_DIV_MIN		1 /* Baud rate divisor minimum */
#define CDNS_SPI_BAUD_DIV_SHIFT		3 /* Baud rate divisor shift in CR */
#define CDNS_SPI_SS_SHIFT		10 /* Slave Select field shift in CR */
#define CDNS_SPI_SS0			0x1 /* Slave Select zero */

/*
 * SPI Interrupt Registers bit Masks
 *
 * All the four interrupt registers (Status/Mask/Enable/Disable) have the same
 * bit definitions.
 */
#define CDNS_SPI_IXR_TXOW	0x00000004 /* SPI TX FIFO Overwater */
#define CDNS_SPI_IXR_MODF	0x00000002 /* SPI Mode Fault */
#define CDNS_SPI_IXR_RXNEMTY 0x00000010 /* SPI RX FIFO Not Empty */
#define CDNS_SPI_IXR_DEFAULT	(CDNS_SPI_IXR_TXOW | \
					CDNS_SPI_IXR_MODF)
#define CDNS_SPI_IXR_TXFULL	0x00000008 /* SPI TX Full */
#define CDNS_SPI_IXR_ALL	0x0000007F /* SPI all interrupts */

/*
 * SPI Enable Register bit Masks
 *
 * This register is used to enable or disable the SPI controller
 */
#define CDNS_SPI_ER_ENABLE	0x00000001 /* SPI Enable Bit Mask */
#define CDNS_SPI_ER_DISABLE	0x0 /* SPI Disable Bit Mask */

/* SPI FIFO depth in bytes */
#define CDNS_SPI_FIFO_DEPTH	128

/* Default number of chip select lines */
#define CDNS_SPI_DEFAULT_NUM_CS		4

/**
 * struct cdns_spi - This definition defines spi driver instance
 * @regs:		Virtual address of the SPI controller registers
 * @ref_clk:		Pointer to the peripheral clock
 * @pclk:		Pointer to the APB clock
 * @speed_hz:		Current SPI bus clock speed in Hz
 * @txbuf:		Pointer	to the TX buffer
 * @rxbuf:		Pointer to the RX buffer
 * @tx_bytes:		Number of bytes left to transfer
 * @rx_bytes:		Number of bytes requested
 * @dev_busy:		Device busy flag
 * @is_decoded_cs:	Flag for decoder property set or not
 */
struct cdns_spi {
	void __iomem *regs;
	struct clk *ref_clk;
	struct clk *pclk;
	unsigned int clk_rate;
	u32 speed_hz;
	const u8 *txbuf;
	u8 *rxbuf;
	int tx_bytes;
	int rx_bytes;
	u8 dev_busy;
	u32 is_decoded_cs;
};

/* Macros for the SPI controller read/write */
static inline u32 cdns_spi_read(struct cdns_spi *xspi, u32 offset)
{
	return readl_relaxed(xspi->regs + offset);
}

static inline void cdns_spi_write(struct cdns_spi *xspi, u32 offset, u32 val)
{
	writel_relaxed(val, xspi->regs + offset);
}

/**
 * cdns_spi_init_hw - Initialize the hardware and configure the SPI controller
 * @xspi:	Pointer to the cdns_spi structure
 *
 * On reset the SPI controller is configured to be in master mode, baud rate
 * divisor is set to 4, threshold value for TX FIFO not full interrupt is set
 * to 1 and size of the word to be transferred as 8 bit.
 * This function initializes the SPI controller to disable and clear all the
 * interrupts, enable manual slave select and manual start, deselect all the
 * chip select lines, and enable the SPI controller.
 */
static void cdns_spi_init_hw(struct cdns_spi *xspi)
{
	u32 ctrl_reg = CDNS_SPI_CR_DEFAULT;

	if (xspi->is_decoded_cs)
		ctrl_reg |= CDNS_SPI_CR_PERI_SEL;

	cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_DISABLE);
	cdns_spi_write(xspi, CDNS_SPI_IDR, CDNS_SPI_IXR_ALL);

	/* Clear the RX FIFO */
	while (cdns_spi_read(xspi, CDNS_SPI_ISR) & CDNS_SPI_IXR_RXNEMTY)
		cdns_spi_read(xspi, CDNS_SPI_RXD);

	cdns_spi_write(xspi, CDNS_SPI_ISR, CDNS_SPI_IXR_ALL);
	cdns_spi_write(xspi, CDNS_SPI_CR, ctrl_reg);
	cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_ENABLE);
}

/**
 * cdns_spi_chipselect - Select or deselect the chip select line
 * @spi:	Pointer to the spi_device structure
 * @is_high:	Select(0) or deselect (1) the chip select line
 */
static void cdns_spi_chipselect(struct spi_device *spi, bool is_high)
{
	struct cdns_spi *xspi = spi_master_get_devdata(spi->master);
	u32 ctrl_reg;

	ctrl_reg = cdns_spi_read(xspi, CDNS_SPI_CR);

	if (is_high) {
		/* Deselect the slave */
		ctrl_reg |= CDNS_SPI_CR_SSCTRL;
	} else {
		/* Select the slave */
		ctrl_reg &= ~CDNS_SPI_CR_SSCTRL;
		if (!(xspi->is_decoded_cs))
			ctrl_reg |= ((~(CDNS_SPI_SS0 << spi->chip_select)) <<
				     CDNS_SPI_SS_SHIFT) &
				     CDNS_SPI_CR_SSCTRL;
		else
			ctrl_reg |= (spi->chip_select << CDNS_SPI_SS_SHIFT) &
				     CDNS_SPI_CR_SSCTRL;
	}

	cdns_spi_write(xspi, CDNS_SPI_CR, ctrl_reg);
}

/**
 * cdns_spi_config_clock_mode - Sets clock polarity and phase
 * @spi:	Pointer to the spi_device structure
 *
 * Sets the requested clock polarity and phase.
 */
static void cdns_spi_config_clock_mode(struct spi_device *spi)
{
	struct cdns_spi *xspi = spi_master_get_devdata(spi->master);
	u32 ctrl_reg, new_ctrl_reg;

	new_ctrl_reg = cdns_spi_read(xspi, CDNS_SPI_CR);
	ctrl_reg = new_ctrl_reg;

	/* Set the SPI clock phase and clock polarity */
	new_ctrl_reg &= ~(CDNS_SPI_CR_CPHA | CDNS_SPI_CR_CPOL);
	if (spi->mode & SPI_CPHA)
		new_ctrl_reg |= CDNS_SPI_CR_CPHA;
	if (spi->mode & SPI_CPOL)
		new_ctrl_reg |= CDNS_SPI_CR_CPOL;

	if (new_ctrl_reg != ctrl_reg) {
		/*
		 * Just writing the CR register does not seem to apply the clock
		 * setting changes. This is problematic when changing the clock
		 * polarity as it will cause the SPI slave to see spurious clock
		 * transitions. To workaround the issue toggle the ER register.
		 */
		cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_DISABLE);
		cdns_spi_write(xspi, CDNS_SPI_CR, new_ctrl_reg);
		cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_ENABLE);
	}
}

/**
 * cdns_spi_config_clock_freq - Sets clock frequency
 * @spi:	Pointer to the spi_device structure
 * @transfer:	Pointer to the spi_transfer structure which provides
 *		information about next transfer setup parameters
 *
 * Sets the requested clock frequency.
 * Note: If the requested frequency is not an exact match with what can be
 * obtained using the prescalar value the driver sets the clock frequency which
 * is lower than the requested frequency (maximum lower) for the transfer. If
 * the requested frequency is higher or lower than that is supported by the SPI
 * controller the driver will set the highest or lowest frequency supported by
 * controller.
 */
static void cdns_spi_config_clock_freq(struct spi_device *spi,
				       struct spi_transfer *transfer)
{
	struct cdns_spi *xspi = spi_master_get_devdata(spi->master);
	u32 ctrl_reg, baud_rate_val;
	unsigned long frequency;

	frequency = xspi->clk_rate;

	ctrl_reg = cdns_spi_read(xspi, CDNS_SPI_CR);

	/* Set the clock frequency */
	if (xspi->speed_hz != transfer->speed_hz) {
		/* first valid value is 1 */
		baud_rate_val = CDNS_SPI_BAUD_DIV_MIN;
		while ((baud_rate_val < CDNS_SPI_BAUD_DIV_MAX) &&
		       (frequency / (2 << baud_rate_val)) > transfer->speed_hz)
			baud_rate_val++;

		ctrl_reg &= ~CDNS_SPI_CR_BAUD_DIV;
		ctrl_reg |= baud_rate_val << CDNS_SPI_BAUD_DIV_SHIFT;

		xspi->speed_hz = frequency / (2 << baud_rate_val);
	}
	cdns_spi_write(xspi, CDNS_SPI_CR, ctrl_reg);
}

/**
 * cdns_spi_setup_transfer - Configure SPI controller for specified transfer
 * @spi:	Pointer to the spi_device structure
 * @transfer:	Pointer to the spi_transfer structure which provides
 *		information about next transfer setup parameters
 *
 * Sets the operational mode of SPI controller for the next SPI transfer and
 * sets the requested clock frequency.
 *
 * Return:	Always 0
 */
static int cdns_spi_setup_transfer(struct spi_device *spi,
				   struct spi_transfer *transfer)
{
	struct cdns_spi *xspi = spi_master_get_devdata(spi->master);

	cdns_spi_config_clock_freq(spi, transfer);

	dev_dbg(&spi->dev, "%s, mode %d, %u bits/w, %u clock speed\n",
		__func__, spi->mode, spi->bits_per_word,
		xspi->speed_hz);

	return 0;
}

/**
 * cdns_spi_fill_tx_fifo - Fills the TX FIFO with as many bytes as possible
 * @xspi:	Pointer to the cdns_spi structure
 */
static void cdns_spi_fill_tx_fifo(struct cdns_spi *xspi)
{
	unsigned long trans_cnt = 0;

	while ((trans_cnt < CDNS_SPI_FIFO_DEPTH) &&
	       (xspi->tx_bytes > 0)) {

		/* When xspi in busy condition, bytes may send failed,
		 * then spi control did't work thoroughly, add one byte delay
		 */
		if (cdns_spi_read(xspi, CDNS_SPI_ISR) &
		    CDNS_SPI_IXR_TXFULL)
			udelay(10);

		if (xspi->txbuf)
			cdns_spi_write(xspi, CDNS_SPI_TXD, *xspi->txbuf++);
		else
			cdns_spi_write(xspi, CDNS_SPI_TXD, 0);

		xspi->tx_bytes--;
		trans_cnt++;
	}
}

/**
 * cdns_spi_irq - Interrupt service routine of the SPI controller
 * @irq:	IRQ number
 * @dev_id:	Pointer to the xspi structure
 *
 * This function handles TX empty and Mode Fault interrupts only.
 * On TX empty interrupt this function reads the received data from RX FIFO and
 * fills the TX FIFO if there is any data remaining to be transferred.
 * On Mode Fault interrupt this function indicates that transfer is completed,
 * the SPI subsystem will identify the error as the remaining bytes to be
 * transferred is non-zero.
 *
 * Return:	IRQ_HANDLED when handled; IRQ_NONE otherwise.
 */
static irqreturn_t cdns_spi_irq(int irq, void *dev_id)
{
	struct spi_master *master = dev_id;
	struct cdns_spi *xspi = spi_master_get_devdata(master);
	u32 intr_status, status;

	status = IRQ_NONE;
	intr_status = cdns_spi_read(xspi, CDNS_SPI_ISR);
	cdns_spi_write(xspi, CDNS_SPI_ISR, intr_status);

	if (intr_status & CDNS_SPI_IXR_MODF) {
		/* Indicate that transfer is completed, the SPI subsystem will
		 * identify the error as the remaining bytes to be
		 * transferred is non-zero
		 */
		cdns_spi_write(xspi, CDNS_SPI_IDR, CDNS_SPI_IXR_DEFAULT);
		spi_finalize_current_transfer(master);
		status = IRQ_HANDLED;
	} else if (intr_status & CDNS_SPI_IXR_TXOW) {
		unsigned long trans_cnt;

		trans_cnt = xspi->rx_bytes - xspi->tx_bytes;

		/* Read out the data from the RX FIFO */
		while (trans_cnt) {
			u8 data;

			data = cdns_spi_read(xspi, CDNS_SPI_RXD);
			if (xspi->rxbuf)
				*xspi->rxbuf++ = data;

			xspi->rx_bytes--;
			trans_cnt--;
		}

		if (xspi->tx_bytes) {
			/* There is more data to send */
			cdns_spi_fill_tx_fifo(xspi);
		} else {
			/* Transfer is completed */
			cdns_spi_write(xspi, CDNS_SPI_IDR,
				       CDNS_SPI_IXR_DEFAULT);
			spi_finalize_current_transfer(master);
		}
		status = IRQ_HANDLED;
	}

	return status;
}

static int cdns_prepare_message(struct spi_master *master,
				struct spi_message *msg)
{
	cdns_spi_config_clock_mode(msg->spi);
	return 0;
}

/**
 * cdns_transfer_one - Initiates the SPI transfer
 * @master:	Pointer to spi_master structure
 * @spi:	Pointer to the spi_device structure
 * @transfer:	Pointer to the spi_transfer structure which provides
 *		information about next transfer parameters
 *
 * This function fills the TX FIFO, starts the SPI transfer and
 * returns a positive transfer count so that core will wait for completion.
 *
 * Return:	Number of bytes transferred in the last transfer
 */
static int cdns_transfer_one(struct spi_master *master,
			     struct spi_device *spi,
			     struct spi_transfer *transfer)
{
	struct cdns_spi *xspi = spi_master_get_devdata(master);

	xspi->txbuf = transfer->tx_buf;
	xspi->rxbuf = transfer->rx_buf;
	xspi->tx_bytes = transfer->len;
	xspi->rx_bytes = transfer->len;

	cdns_spi_setup_transfer(spi, transfer);
	cdns_spi_fill_tx_fifo(xspi);
	spi_transfer_delay_exec(transfer);

	cdns_spi_write(xspi, CDNS_SPI_IER, CDNS_SPI_IXR_DEFAULT);
	return transfer->len;
}

/**
 * cdns_prepare_transfer_hardware - Prepares hardware for transfer.
 * @master:	Pointer to the spi_master structure which provides
 *		information about the controller.
 *
 * This function enables SPI master controller.
 *
 * Return:	0 always
 */
static int cdns_prepare_transfer_hardware(struct spi_master *master)
{
	struct cdns_spi *xspi = spi_master_get_devdata(master);

	cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_ENABLE);

	return 0;
}

/**
 * cdns_unprepare_transfer_hardware - Relaxes hardware after transfer
 * @master:	Pointer to the spi_master structure which provides
 *		information about the controller.
 *
 * This function disables the SPI master controller.
 *
 * Return:	0 always
 */
static int cdns_unprepare_transfer_hardware(struct spi_master *master)
{
	struct cdns_spi *xspi = spi_master_get_devdata(master);

	cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_DISABLE);

	return 0;
}

/**
 * cdns_spi_probe - Probe method for the SPI driver
 * @pdev:	Pointer to the platform_device structure
 *
 * This function initializes the driver data structures and the hardware.
 *
 * Return:	0 on success and error value on error
 */
static int cdns_spi_probe(struct platform_device *pdev)
{
	int ret = 0, irq;
	struct spi_master *master;
	struct cdns_spi *xspi;
	u32 num_cs;

	master = spi_alloc_master(&pdev->dev, sizeof(*xspi));
	if (!master)
		return -ENOMEM;

	xspi = spi_master_get_devdata(master);
	master->dev.of_node = pdev->dev.of_node;
	platform_set_drvdata(pdev, master);

	xspi->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(xspi->regs)) {
		ret = PTR_ERR(xspi->regs);
		goto remove_master;
	}

	xspi->pclk = devm_clk_get(&pdev->dev, "pclk");
	if (IS_ERR(xspi->pclk)) {
		dev_err(&pdev->dev, "pclk clock not found.\n");
		ret = PTR_ERR(xspi->pclk);
		goto remove_master;
	}

	xspi->ref_clk = devm_clk_get(&pdev->dev, "ref_clk");
	if (IS_ERR(xspi->ref_clk)) {
		dev_err(&pdev->dev, "ref_clk clock not found.\n");
		ret = PTR_ERR(xspi->ref_clk);
		goto remove_master;
	}

	ret = clk_prepare_enable(xspi->pclk);
	if (ret) {
		dev_err(&pdev->dev, "Unable to enable APB clock.\n");
		goto remove_master;
	}

	ret = clk_prepare_enable(xspi->ref_clk);
	if (ret) {
		dev_err(&pdev->dev, "Unable to enable device clock.\n");
		goto clk_dis_apb;
	}

	ret = of_property_read_u32(pdev->dev.of_node, "num-cs", &num_cs);
	if (ret < 0)
		master->num_chipselect = CDNS_SPI_DEFAULT_NUM_CS;
	else
		master->num_chipselect = num_cs;

	ret = of_property_read_u32(pdev->dev.of_node, "is-decoded-cs",
				   &xspi->is_decoded_cs);
	if (ret < 0)
		xspi->is_decoded_cs = 0;

	/* SPI controller initializations */
	cdns_spi_init_hw(xspi);

	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_runtime_set_autosuspend_delay(&pdev->dev, SPI_AUTOSUSPEND_TIMEOUT);

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0) {
		ret = -ENXIO;
		goto clk_dis_all;
	}

	ret = devm_request_irq(&pdev->dev, irq, cdns_spi_irq,
			       0, pdev->name, master);
	if (ret != 0) {
		ret = -ENXIO;
		dev_err(&pdev->dev, "request_irq failed\n");
		goto clk_dis_all;
	}

	master->use_gpio_descriptors = true;
	master->prepare_transfer_hardware = cdns_prepare_transfer_hardware;
	master->prepare_message = cdns_prepare_message;
	master->transfer_one = cdns_transfer_one;
	master->unprepare_transfer_hardware = cdns_unprepare_transfer_hardware;
	master->set_cs = cdns_spi_chipselect;
	master->auto_runtime_pm = true;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;

	xspi->clk_rate = clk_get_rate(xspi->ref_clk);
	/* Set to default valid value */
	master->max_speed_hz = xspi->clk_rate / 4;
	xspi->speed_hz = master->max_speed_hz;

	master->bits_per_word_mask = SPI_BPW_MASK(8);

	ret = spi_register_master(master);
	if (ret) {
		dev_err(&pdev->dev, "spi_register_master failed\n");
		goto clk_dis_all;
	}

	return ret;

clk_dis_all:
	pm_runtime_set_suspended(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
	clk_disable_unprepare(xspi->ref_clk);
clk_dis_apb:
	clk_disable_unprepare(xspi->pclk);
remove_master:
	spi_master_put(master);
	return ret;
}

/**
 * cdns_spi_remove - Remove method for the SPI driver
 * @pdev:	Pointer to the platform_device structure
 *
 * This function is called if a device is physically removed from the system or
 * if the driver module is being unloaded. It frees all resources allocated to
 * the device.
 *
 * Return:	0 on success and error value on error
 */
static int cdns_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct cdns_spi *xspi = spi_master_get_devdata(master);

	cdns_spi_write(xspi, CDNS_SPI_ER, CDNS_SPI_ER_DISABLE);

	clk_disable_unprepare(xspi->ref_clk);
	clk_disable_unprepare(xspi->pclk);
	pm_runtime_set_suspended(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	spi_unregister_master(master);

	return 0;
}

/**
 * cdns_spi_suspend - Suspend method for the SPI driver
 * @dev:	Address of the platform_device structure
 *
 * This function disables the SPI controller and
 * changes the driver state to "suspend"
 *
 * Return:	0 on success and error value on error
 */
static int __maybe_unused cdns_spi_suspend(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);

	return spi_master_suspend(master);
}

/**
 * cdns_spi_resume - Resume method for the SPI driver
 * @dev:	Address of the platform_device structure
 *
 * This function changes the driver state to "ready"
 *
 * Return:	0 on success and error value on error
 */
static int __maybe_unused cdns_spi_resume(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct cdns_spi *xspi = spi_master_get_devdata(master);

	cdns_spi_init_hw(xspi);
	return spi_master_resume(master);
}

/**
 * cdns_spi_runtime_resume - Runtime resume method for the SPI driver
 * @dev:	Address of the platform_device structure
 *
 * This function enables the clocks
 *
 * Return:	0 on success and error value on error
 */
static int __maybe_unused cnds_runtime_resume(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct cdns_spi *xspi = spi_master_get_devdata(master);
	int ret;

	ret = clk_prepare_enable(xspi->pclk);
	if (ret) {
		dev_err(dev, "Cannot enable APB clock.\n");
		return ret;
	}

	ret = clk_prepare_enable(xspi->ref_clk);
	if (ret) {
		dev_err(dev, "Cannot enable device clock.\n");
		clk_disable_unprepare(xspi->pclk);
		return ret;
	}
	return 0;
}

/**
 * cdns_spi_runtime_suspend - Runtime suspend method for the SPI driver
 * @dev:	Address of the platform_device structure
 *
 * This function disables the clocks
 *
 * Return:	Always 0
 */
static int __maybe_unused cnds_runtime_suspend(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct cdns_spi *xspi = spi_master_get_devdata(master);

	clk_disable_unprepare(xspi->ref_clk);
	clk_disable_unprepare(xspi->pclk);

	return 0;
}

static const struct dev_pm_ops cdns_spi_dev_pm_ops = {
	SET_RUNTIME_PM_OPS(cnds_runtime_suspend,
			   cnds_runtime_resume, NULL)
	SET_SYSTEM_SLEEP_PM_OPS(cdns_spi_suspend, cdns_spi_resume)
};

static const struct of_device_id cdns_spi_of_match[] = {
	{ .compatible = "xlnx,zynq-spi-r1p6" },
	{ .compatible = "cdns,spi-r1p6" },
	{ /* end of table */ }
};
MODULE_DEVICE_TABLE(of, cdns_spi_of_match);

/* cdns_spi_driver - This structure defines the SPI subsystem platform driver */
static struct platform_driver cdns_spi_driver = {
	.probe	= cdns_spi_probe,
	.remove	= cdns_spi_remove,
	.driver = {
		.name = CDNS_SPI_NAME,
		.of_match_table = cdns_spi_of_match,
		.pm = &cdns_spi_dev_pm_ops,
	},
};

module_platform_driver(cdns_spi_driver);

MODULE_AUTHOR("Xilinx, Inc.");
MODULE_DESCRIPTION("Cadence SPI driver");
MODULE_LICENSE("GPL");

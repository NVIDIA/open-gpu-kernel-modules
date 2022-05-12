// SPDX-License-Identifier: GPL-2.0-only
/*
 * Marvell Armada-3700 SPI controller driver
 *
 * Copyright (C) 2016 Marvell Ltd.
 *
 * Author: Wilson Ding <dingwei@marvell.com>
 * Author: Romain Perier <romain.perier@free-electrons.com>
 */

#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/spi/spi.h>

#define DRIVER_NAME			"armada_3700_spi"

#define A3700_SPI_MAX_SPEED_HZ		100000000
#define A3700_SPI_MAX_PRESCALE		30
#define A3700_SPI_TIMEOUT		10

/* SPI Register Offest */
#define A3700_SPI_IF_CTRL_REG		0x00
#define A3700_SPI_IF_CFG_REG		0x04
#define A3700_SPI_DATA_OUT_REG		0x08
#define A3700_SPI_DATA_IN_REG		0x0C
#define A3700_SPI_IF_INST_REG		0x10
#define A3700_SPI_IF_ADDR_REG		0x14
#define A3700_SPI_IF_RMODE_REG		0x18
#define A3700_SPI_IF_HDR_CNT_REG	0x1C
#define A3700_SPI_IF_DIN_CNT_REG	0x20
#define A3700_SPI_IF_TIME_REG		0x24
#define A3700_SPI_INT_STAT_REG		0x28
#define A3700_SPI_INT_MASK_REG		0x2C

/* A3700_SPI_IF_CTRL_REG */
#define A3700_SPI_EN			BIT(16)
#define A3700_SPI_ADDR_NOT_CONFIG	BIT(12)
#define A3700_SPI_WFIFO_OVERFLOW	BIT(11)
#define A3700_SPI_WFIFO_UNDERFLOW	BIT(10)
#define A3700_SPI_RFIFO_OVERFLOW	BIT(9)
#define A3700_SPI_RFIFO_UNDERFLOW	BIT(8)
#define A3700_SPI_WFIFO_FULL		BIT(7)
#define A3700_SPI_WFIFO_EMPTY		BIT(6)
#define A3700_SPI_RFIFO_FULL		BIT(5)
#define A3700_SPI_RFIFO_EMPTY		BIT(4)
#define A3700_SPI_WFIFO_RDY		BIT(3)
#define A3700_SPI_RFIFO_RDY		BIT(2)
#define A3700_SPI_XFER_RDY		BIT(1)
#define A3700_SPI_XFER_DONE		BIT(0)

/* A3700_SPI_IF_CFG_REG */
#define A3700_SPI_WFIFO_THRS		BIT(28)
#define A3700_SPI_RFIFO_THRS		BIT(24)
#define A3700_SPI_AUTO_CS		BIT(20)
#define A3700_SPI_DMA_RD_EN		BIT(18)
#define A3700_SPI_FIFO_MODE		BIT(17)
#define A3700_SPI_SRST			BIT(16)
#define A3700_SPI_XFER_START		BIT(15)
#define A3700_SPI_XFER_STOP		BIT(14)
#define A3700_SPI_INST_PIN		BIT(13)
#define A3700_SPI_ADDR_PIN		BIT(12)
#define A3700_SPI_DATA_PIN1		BIT(11)
#define A3700_SPI_DATA_PIN0		BIT(10)
#define A3700_SPI_FIFO_FLUSH		BIT(9)
#define A3700_SPI_RW_EN			BIT(8)
#define A3700_SPI_CLK_POL		BIT(7)
#define A3700_SPI_CLK_PHA		BIT(6)
#define A3700_SPI_BYTE_LEN		BIT(5)
#define A3700_SPI_CLK_PRESCALE		BIT(0)
#define A3700_SPI_CLK_PRESCALE_MASK	(0x1f)
#define A3700_SPI_CLK_EVEN_OFFS		(0x10)

#define A3700_SPI_WFIFO_THRS_BIT	28
#define A3700_SPI_RFIFO_THRS_BIT	24
#define A3700_SPI_FIFO_THRS_MASK	0x7

#define A3700_SPI_DATA_PIN_MASK		0x3

/* A3700_SPI_IF_HDR_CNT_REG */
#define A3700_SPI_DUMMY_CNT_BIT		12
#define A3700_SPI_DUMMY_CNT_MASK	0x7
#define A3700_SPI_RMODE_CNT_BIT		8
#define A3700_SPI_RMODE_CNT_MASK	0x3
#define A3700_SPI_ADDR_CNT_BIT		4
#define A3700_SPI_ADDR_CNT_MASK		0x7
#define A3700_SPI_INSTR_CNT_BIT		0
#define A3700_SPI_INSTR_CNT_MASK	0x3

/* A3700_SPI_IF_TIME_REG */
#define A3700_SPI_CLK_CAPT_EDGE		BIT(7)

struct a3700_spi {
	struct spi_master *master;
	void __iomem *base;
	struct clk *clk;
	unsigned int irq;
	unsigned int flags;
	bool xmit_data;
	const u8 *tx_buf;
	u8 *rx_buf;
	size_t buf_len;
	u8 byte_len;
	u32 wait_mask;
	struct completion done;
};

static u32 spireg_read(struct a3700_spi *a3700_spi, u32 offset)
{
	return readl(a3700_spi->base + offset);
}

static void spireg_write(struct a3700_spi *a3700_spi, u32 offset, u32 data)
{
	writel(data, a3700_spi->base + offset);
}

static void a3700_spi_auto_cs_unset(struct a3700_spi *a3700_spi)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val &= ~A3700_SPI_AUTO_CS;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
}

static void a3700_spi_activate_cs(struct a3700_spi *a3700_spi, unsigned int cs)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG);
	val |= (A3700_SPI_EN << cs);
	spireg_write(a3700_spi, A3700_SPI_IF_CTRL_REG, val);
}

static void a3700_spi_deactivate_cs(struct a3700_spi *a3700_spi,
				    unsigned int cs)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG);
	val &= ~(A3700_SPI_EN << cs);
	spireg_write(a3700_spi, A3700_SPI_IF_CTRL_REG, val);
}

static int a3700_spi_pin_mode_set(struct a3700_spi *a3700_spi,
				  unsigned int pin_mode, bool receiving)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val &= ~(A3700_SPI_INST_PIN | A3700_SPI_ADDR_PIN);
	val &= ~(A3700_SPI_DATA_PIN0 | A3700_SPI_DATA_PIN1);

	switch (pin_mode) {
	case SPI_NBITS_SINGLE:
		break;
	case SPI_NBITS_DUAL:
		val |= A3700_SPI_DATA_PIN0;
		break;
	case SPI_NBITS_QUAD:
		val |= A3700_SPI_DATA_PIN1;
		/* RX during address reception uses 4-pin */
		if (receiving)
			val |= A3700_SPI_ADDR_PIN;
		break;
	default:
		dev_err(&a3700_spi->master->dev, "wrong pin mode %u", pin_mode);
		return -EINVAL;
	}

	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	return 0;
}

static void a3700_spi_fifo_mode_set(struct a3700_spi *a3700_spi, bool enable)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	if (enable)
		val |= A3700_SPI_FIFO_MODE;
	else
		val &= ~A3700_SPI_FIFO_MODE;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
}

static void a3700_spi_mode_set(struct a3700_spi *a3700_spi,
			       unsigned int mode_bits)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);

	if (mode_bits & SPI_CPOL)
		val |= A3700_SPI_CLK_POL;
	else
		val &= ~A3700_SPI_CLK_POL;

	if (mode_bits & SPI_CPHA)
		val |= A3700_SPI_CLK_PHA;
	else
		val &= ~A3700_SPI_CLK_PHA;

	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
}

static void a3700_spi_clock_set(struct a3700_spi *a3700_spi,
				unsigned int speed_hz)
{
	u32 val;
	u32 prescale;

	prescale = DIV_ROUND_UP(clk_get_rate(a3700_spi->clk), speed_hz);

	/* For prescaler values over 15, we can only set it by steps of 2.
	 * Starting from A3700_SPI_CLK_EVEN_OFFS, we set values from 0 up to
	 * 30. We only use this range from 16 to 30.
	 */
	if (prescale > 15)
		prescale = A3700_SPI_CLK_EVEN_OFFS + DIV_ROUND_UP(prescale, 2);

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val = val & ~A3700_SPI_CLK_PRESCALE_MASK;

	val = val | (prescale & A3700_SPI_CLK_PRESCALE_MASK);
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	if (prescale <= 2) {
		val = spireg_read(a3700_spi, A3700_SPI_IF_TIME_REG);
		val |= A3700_SPI_CLK_CAPT_EDGE;
		spireg_write(a3700_spi, A3700_SPI_IF_TIME_REG, val);
	}
}

static void a3700_spi_bytelen_set(struct a3700_spi *a3700_spi, unsigned int len)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	if (len == 4)
		val |= A3700_SPI_BYTE_LEN;
	else
		val &= ~A3700_SPI_BYTE_LEN;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	a3700_spi->byte_len = len;
}

static int a3700_spi_fifo_flush(struct a3700_spi *a3700_spi)
{
	int timeout = A3700_SPI_TIMEOUT;
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val |= A3700_SPI_FIFO_FLUSH;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	while (--timeout) {
		val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
		if (!(val & A3700_SPI_FIFO_FLUSH))
			return 0;
		udelay(1);
	}

	return -ETIMEDOUT;
}

static void a3700_spi_init(struct a3700_spi *a3700_spi)
{
	struct spi_master *master = a3700_spi->master;
	u32 val;
	int i;

	/* Reset SPI unit */
	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val |= A3700_SPI_SRST;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	udelay(A3700_SPI_TIMEOUT);

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val &= ~A3700_SPI_SRST;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	/* Disable AUTO_CS and deactivate all chip-selects */
	a3700_spi_auto_cs_unset(a3700_spi);
	for (i = 0; i < master->num_chipselect; i++)
		a3700_spi_deactivate_cs(a3700_spi, i);

	/* Enable FIFO mode */
	a3700_spi_fifo_mode_set(a3700_spi, true);

	/* Set SPI mode */
	a3700_spi_mode_set(a3700_spi, master->mode_bits);

	/* Reset counters */
	spireg_write(a3700_spi, A3700_SPI_IF_HDR_CNT_REG, 0);
	spireg_write(a3700_spi, A3700_SPI_IF_DIN_CNT_REG, 0);

	/* Mask the interrupts and clear cause bits */
	spireg_write(a3700_spi, A3700_SPI_INT_MASK_REG, 0);
	spireg_write(a3700_spi, A3700_SPI_INT_STAT_REG, ~0U);
}

static irqreturn_t a3700_spi_interrupt(int irq, void *dev_id)
{
	struct spi_master *master = dev_id;
	struct a3700_spi *a3700_spi;
	u32 cause;

	a3700_spi = spi_master_get_devdata(master);

	/* Get interrupt causes */
	cause = spireg_read(a3700_spi, A3700_SPI_INT_STAT_REG);

	if (!cause || !(a3700_spi->wait_mask & cause))
		return IRQ_NONE;

	/* mask and acknowledge the SPI interrupts */
	spireg_write(a3700_spi, A3700_SPI_INT_MASK_REG, 0);
	spireg_write(a3700_spi, A3700_SPI_INT_STAT_REG, cause);

	/* Wake up the transfer */
	complete(&a3700_spi->done);

	return IRQ_HANDLED;
}

static bool a3700_spi_wait_completion(struct spi_device *spi)
{
	struct a3700_spi *a3700_spi;
	unsigned int timeout;
	unsigned int ctrl_reg;
	unsigned long timeout_jiffies;

	a3700_spi = spi_master_get_devdata(spi->master);

	/* SPI interrupt is edge-triggered, which means an interrupt will
	 * be generated only when detecting a specific status bit changed
	 * from '0' to '1'. So when we start waiting for a interrupt, we
	 * need to check status bit in control reg first, if it is already 1,
	 * then we do not need to wait for interrupt
	 */
	ctrl_reg = spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG);
	if (a3700_spi->wait_mask & ctrl_reg)
		return true;

	reinit_completion(&a3700_spi->done);

	spireg_write(a3700_spi, A3700_SPI_INT_MASK_REG,
		     a3700_spi->wait_mask);

	timeout_jiffies = msecs_to_jiffies(A3700_SPI_TIMEOUT);
	timeout = wait_for_completion_timeout(&a3700_spi->done,
					      timeout_jiffies);

	a3700_spi->wait_mask = 0;

	if (timeout)
		return true;

	/* there might be the case that right after we checked the
	 * status bits in this routine and before start to wait for
	 * interrupt by wait_for_completion_timeout, the interrupt
	 * happens, to avoid missing it we need to double check
	 * status bits in control reg, if it is already 1, then
	 * consider that we have the interrupt successfully and
	 * return true.
	 */
	ctrl_reg = spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG);
	if (a3700_spi->wait_mask & ctrl_reg)
		return true;

	spireg_write(a3700_spi, A3700_SPI_INT_MASK_REG, 0);

	/* Timeout was reached */
	return false;
}

static bool a3700_spi_transfer_wait(struct spi_device *spi,
				    unsigned int bit_mask)
{
	struct a3700_spi *a3700_spi;

	a3700_spi = spi_master_get_devdata(spi->master);
	a3700_spi->wait_mask = bit_mask;

	return a3700_spi_wait_completion(spi);
}

static void a3700_spi_fifo_thres_set(struct a3700_spi *a3700_spi,
				     unsigned int bytes)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val &= ~(A3700_SPI_FIFO_THRS_MASK << A3700_SPI_RFIFO_THRS_BIT);
	val |= (bytes - 1) << A3700_SPI_RFIFO_THRS_BIT;
	val &= ~(A3700_SPI_FIFO_THRS_MASK << A3700_SPI_WFIFO_THRS_BIT);
	val |= (7 - bytes) << A3700_SPI_WFIFO_THRS_BIT;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
}

static void a3700_spi_transfer_setup(struct spi_device *spi,
				     struct spi_transfer *xfer)
{
	struct a3700_spi *a3700_spi;

	a3700_spi = spi_master_get_devdata(spi->master);

	a3700_spi_clock_set(a3700_spi, xfer->speed_hz);

	/* Use 4 bytes long transfers. Each transfer method has its way to deal
	 * with the remaining bytes for non 4-bytes aligned transfers.
	 */
	a3700_spi_bytelen_set(a3700_spi, 4);

	/* Initialize the working buffers */
	a3700_spi->tx_buf  = xfer->tx_buf;
	a3700_spi->rx_buf  = xfer->rx_buf;
	a3700_spi->buf_len = xfer->len;
}

static void a3700_spi_set_cs(struct spi_device *spi, bool enable)
{
	struct a3700_spi *a3700_spi = spi_master_get_devdata(spi->master);

	if (!enable)
		a3700_spi_activate_cs(a3700_spi, spi->chip_select);
	else
		a3700_spi_deactivate_cs(a3700_spi, spi->chip_select);
}

static void a3700_spi_header_set(struct a3700_spi *a3700_spi)
{
	unsigned int addr_cnt;
	u32 val = 0;

	/* Clear the header registers */
	spireg_write(a3700_spi, A3700_SPI_IF_INST_REG, 0);
	spireg_write(a3700_spi, A3700_SPI_IF_ADDR_REG, 0);
	spireg_write(a3700_spi, A3700_SPI_IF_RMODE_REG, 0);
	spireg_write(a3700_spi, A3700_SPI_IF_HDR_CNT_REG, 0);

	/* Set header counters */
	if (a3700_spi->tx_buf) {
		/*
		 * when tx data is not 4 bytes aligned, there will be unexpected
		 * bytes out of SPI output register, since it always shifts out
		 * as whole 4 bytes. This might cause incorrect transaction with
		 * some devices. To avoid that, use SPI header count feature to
		 * transfer up to 3 bytes of data first, and then make the rest
		 * of data 4-byte aligned.
		 */
		addr_cnt = a3700_spi->buf_len % 4;
		if (addr_cnt) {
			val = (addr_cnt & A3700_SPI_ADDR_CNT_MASK)
				<< A3700_SPI_ADDR_CNT_BIT;
			spireg_write(a3700_spi, A3700_SPI_IF_HDR_CNT_REG, val);

			/* Update the buffer length to be transferred */
			a3700_spi->buf_len -= addr_cnt;

			/* transfer 1~3 bytes through address count */
			val = 0;
			while (addr_cnt--) {
				val = (val << 8) | a3700_spi->tx_buf[0];
				a3700_spi->tx_buf++;
			}
			spireg_write(a3700_spi, A3700_SPI_IF_ADDR_REG, val);
		}
	}
}

static int a3700_is_wfifo_full(struct a3700_spi *a3700_spi)
{
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG);
	return (val & A3700_SPI_WFIFO_FULL);
}

static int a3700_spi_fifo_write(struct a3700_spi *a3700_spi)
{
	u32 val;

	while (!a3700_is_wfifo_full(a3700_spi) && a3700_spi->buf_len) {
		val = *(u32 *)a3700_spi->tx_buf;
		spireg_write(a3700_spi, A3700_SPI_DATA_OUT_REG, val);
		a3700_spi->buf_len -= 4;
		a3700_spi->tx_buf += 4;
	}

	return 0;
}

static int a3700_is_rfifo_empty(struct a3700_spi *a3700_spi)
{
	u32 val = spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG);

	return (val & A3700_SPI_RFIFO_EMPTY);
}

static int a3700_spi_fifo_read(struct a3700_spi *a3700_spi)
{
	u32 val;

	while (!a3700_is_rfifo_empty(a3700_spi) && a3700_spi->buf_len) {
		val = spireg_read(a3700_spi, A3700_SPI_DATA_IN_REG);
		if (a3700_spi->buf_len >= 4) {

			memcpy(a3700_spi->rx_buf, &val, 4);

			a3700_spi->buf_len -= 4;
			a3700_spi->rx_buf += 4;
		} else {
			/*
			 * When remain bytes is not larger than 4, we should
			 * avoid memory overwriting and just write the left rx
			 * buffer bytes.
			 */
			while (a3700_spi->buf_len) {
				*a3700_spi->rx_buf = val & 0xff;
				val >>= 8;

				a3700_spi->buf_len--;
				a3700_spi->rx_buf++;
			}
		}
	}

	return 0;
}

static void a3700_spi_transfer_abort_fifo(struct a3700_spi *a3700_spi)
{
	int timeout = A3700_SPI_TIMEOUT;
	u32 val;

	val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
	val |= A3700_SPI_XFER_STOP;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

	while (--timeout) {
		val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
		if (!(val & A3700_SPI_XFER_START))
			break;
		udelay(1);
	}

	a3700_spi_fifo_flush(a3700_spi);

	val &= ~A3700_SPI_XFER_STOP;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
}

static int a3700_spi_prepare_message(struct spi_master *master,
				     struct spi_message *message)
{
	struct a3700_spi *a3700_spi = spi_master_get_devdata(master);
	struct spi_device *spi = message->spi;
	int ret;

	ret = clk_enable(a3700_spi->clk);
	if (ret) {
		dev_err(&spi->dev, "failed to enable clk with error %d\n", ret);
		return ret;
	}

	/* Flush the FIFOs */
	ret = a3700_spi_fifo_flush(a3700_spi);
	if (ret)
		return ret;

	a3700_spi_mode_set(a3700_spi, spi->mode);

	return 0;
}

static int a3700_spi_transfer_one_fifo(struct spi_master *master,
				  struct spi_device *spi,
				  struct spi_transfer *xfer)
{
	struct a3700_spi *a3700_spi = spi_master_get_devdata(master);
	int ret = 0, timeout = A3700_SPI_TIMEOUT;
	unsigned int nbits = 0, byte_len;
	u32 val;

	/* Make sure we use FIFO mode */
	a3700_spi_fifo_mode_set(a3700_spi, true);

	/* Configure FIFO thresholds */
	byte_len = xfer->bits_per_word >> 3;
	a3700_spi_fifo_thres_set(a3700_spi, byte_len);

	if (xfer->tx_buf)
		nbits = xfer->tx_nbits;
	else if (xfer->rx_buf)
		nbits = xfer->rx_nbits;

	a3700_spi_pin_mode_set(a3700_spi, nbits, xfer->rx_buf ? true : false);

	/* Flush the FIFOs */
	a3700_spi_fifo_flush(a3700_spi);

	/* Transfer first bytes of data when buffer is not 4-byte aligned */
	a3700_spi_header_set(a3700_spi);

	if (xfer->rx_buf) {
		/* Clear WFIFO, since it's last 2 bytes are shifted out during
		 * a read operation
		 */
		spireg_write(a3700_spi, A3700_SPI_DATA_OUT_REG, 0);

		/* Set read data length */
		spireg_write(a3700_spi, A3700_SPI_IF_DIN_CNT_REG,
			     a3700_spi->buf_len);
		/* Start READ transfer */
		val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
		val &= ~A3700_SPI_RW_EN;
		val |= A3700_SPI_XFER_START;
		spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
	} else if (xfer->tx_buf) {
		/* Start Write transfer */
		val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
		val |= (A3700_SPI_XFER_START | A3700_SPI_RW_EN);
		spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);

		/*
		 * If there are data to be written to the SPI device, xmit_data
		 * flag is set true; otherwise the instruction in SPI_INSTR does
		 * not require data to be written to the SPI device, then
		 * xmit_data flag is set false.
		 */
		a3700_spi->xmit_data = (a3700_spi->buf_len != 0);
	}

	while (a3700_spi->buf_len) {
		if (a3700_spi->tx_buf) {
			/* Wait wfifo ready */
			if (!a3700_spi_transfer_wait(spi,
						     A3700_SPI_WFIFO_RDY)) {
				dev_err(&spi->dev,
					"wait wfifo ready timed out\n");
				ret = -ETIMEDOUT;
				goto error;
			}
			/* Fill up the wfifo */
			ret = a3700_spi_fifo_write(a3700_spi);
			if (ret)
				goto error;
		} else if (a3700_spi->rx_buf) {
			/* Wait rfifo ready */
			if (!a3700_spi_transfer_wait(spi,
						     A3700_SPI_RFIFO_RDY)) {
				dev_err(&spi->dev,
					"wait rfifo ready timed out\n");
				ret = -ETIMEDOUT;
				goto error;
			}
			/* Drain out the rfifo */
			ret = a3700_spi_fifo_read(a3700_spi);
			if (ret)
				goto error;
		}
	}

	/*
	 * Stop a write transfer in fifo mode:
	 *	- wait all the bytes in wfifo to be shifted out
	 *	 - set XFER_STOP bit
	 *	- wait XFER_START bit clear
	 *	- clear XFER_STOP bit
	 * Stop a read transfer in fifo mode:
	 *	- the hardware is to reset the XFER_START bit
	 *	   after the number of bytes indicated in DIN_CNT
	 *	   register
	 *	- just wait XFER_START bit clear
	 */
	if (a3700_spi->tx_buf) {
		if (a3700_spi->xmit_data) {
			/*
			 * If there are data written to the SPI device, wait
			 * until SPI_WFIFO_EMPTY is 1 to wait for all data to
			 * transfer out of write FIFO.
			 */
			if (!a3700_spi_transfer_wait(spi,
						     A3700_SPI_WFIFO_EMPTY)) {
				dev_err(&spi->dev, "wait wfifo empty timed out\n");
				return -ETIMEDOUT;
			}
		}

		if (!a3700_spi_transfer_wait(spi, A3700_SPI_XFER_RDY)) {
			dev_err(&spi->dev, "wait xfer ready timed out\n");
			return -ETIMEDOUT;
		}

		val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
		val |= A3700_SPI_XFER_STOP;
		spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
	}

	while (--timeout) {
		val = spireg_read(a3700_spi, A3700_SPI_IF_CFG_REG);
		if (!(val & A3700_SPI_XFER_START))
			break;
		udelay(1);
	}

	if (timeout == 0) {
		dev_err(&spi->dev, "wait transfer start clear timed out\n");
		ret = -ETIMEDOUT;
		goto error;
	}

	val &= ~A3700_SPI_XFER_STOP;
	spireg_write(a3700_spi, A3700_SPI_IF_CFG_REG, val);
	goto out;

error:
	a3700_spi_transfer_abort_fifo(a3700_spi);
out:
	spi_finalize_current_transfer(master);

	return ret;
}

static int a3700_spi_transfer_one_full_duplex(struct spi_master *master,
				  struct spi_device *spi,
				  struct spi_transfer *xfer)
{
	struct a3700_spi *a3700_spi = spi_master_get_devdata(master);
	u32 val;

	/* Disable FIFO mode */
	a3700_spi_fifo_mode_set(a3700_spi, false);

	while (a3700_spi->buf_len) {

		/* When we have less than 4 bytes to transfer, switch to 1 byte
		 * mode. This is reset after each transfer
		 */
		if (a3700_spi->buf_len < 4)
			a3700_spi_bytelen_set(a3700_spi, 1);

		if (a3700_spi->byte_len == 1)
			val = *a3700_spi->tx_buf;
		else
			val = *(u32 *)a3700_spi->tx_buf;

		spireg_write(a3700_spi, A3700_SPI_DATA_OUT_REG, val);

		/* Wait for all the data to be shifted in / out */
		while (!(spireg_read(a3700_spi, A3700_SPI_IF_CTRL_REG) &
				A3700_SPI_XFER_DONE))
			cpu_relax();

		val = spireg_read(a3700_spi, A3700_SPI_DATA_IN_REG);

		memcpy(a3700_spi->rx_buf, &val, a3700_spi->byte_len);

		a3700_spi->buf_len -= a3700_spi->byte_len;
		a3700_spi->tx_buf += a3700_spi->byte_len;
		a3700_spi->rx_buf += a3700_spi->byte_len;

	}

	spi_finalize_current_transfer(master);

	return 0;
}

static int a3700_spi_transfer_one(struct spi_master *master,
				  struct spi_device *spi,
				  struct spi_transfer *xfer)
{
	a3700_spi_transfer_setup(spi, xfer);

	if (xfer->tx_buf && xfer->rx_buf)
		return a3700_spi_transfer_one_full_duplex(master, spi, xfer);

	return a3700_spi_transfer_one_fifo(master, spi, xfer);
}

static int a3700_spi_unprepare_message(struct spi_master *master,
				       struct spi_message *message)
{
	struct a3700_spi *a3700_spi = spi_master_get_devdata(master);

	clk_disable(a3700_spi->clk);

	return 0;
}

static const struct of_device_id a3700_spi_dt_ids[] = {
	{ .compatible = "marvell,armada-3700-spi", .data = NULL },
	{},
};

MODULE_DEVICE_TABLE(of, a3700_spi_dt_ids);

static int a3700_spi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *of_node = dev->of_node;
	struct spi_master *master;
	struct a3700_spi *spi;
	u32 num_cs = 0;
	int irq, ret = 0;

	master = spi_alloc_master(dev, sizeof(*spi));
	if (!master) {
		dev_err(dev, "master allocation failed\n");
		ret = -ENOMEM;
		goto out;
	}

	if (of_property_read_u32(of_node, "num-cs", &num_cs)) {
		dev_err(dev, "could not find num-cs\n");
		ret = -ENXIO;
		goto error;
	}

	master->bus_num = pdev->id;
	master->dev.of_node = of_node;
	master->mode_bits = SPI_MODE_3;
	master->num_chipselect = num_cs;
	master->bits_per_word_mask = SPI_BPW_MASK(8) | SPI_BPW_MASK(32);
	master->prepare_message =  a3700_spi_prepare_message;
	master->transfer_one = a3700_spi_transfer_one;
	master->unprepare_message = a3700_spi_unprepare_message;
	master->set_cs = a3700_spi_set_cs;
	master->mode_bits |= (SPI_RX_DUAL | SPI_TX_DUAL |
			      SPI_RX_QUAD | SPI_TX_QUAD);

	platform_set_drvdata(pdev, master);

	spi = spi_master_get_devdata(master);

	spi->master = master;

	spi->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(spi->base)) {
		ret = PTR_ERR(spi->base);
		goto error;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		ret = -ENXIO;
		goto error;
	}
	spi->irq = irq;

	init_completion(&spi->done);

	spi->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(spi->clk)) {
		dev_err(dev, "could not find clk: %ld\n", PTR_ERR(spi->clk));
		goto error;
	}

	ret = clk_prepare(spi->clk);
	if (ret) {
		dev_err(dev, "could not prepare clk: %d\n", ret);
		goto error;
	}

	master->max_speed_hz = min_t(unsigned long, A3700_SPI_MAX_SPEED_HZ,
					clk_get_rate(spi->clk));
	master->min_speed_hz = DIV_ROUND_UP(clk_get_rate(spi->clk),
						A3700_SPI_MAX_PRESCALE);

	a3700_spi_init(spi);

	ret = devm_request_irq(dev, spi->irq, a3700_spi_interrupt, 0,
			       dev_name(dev), master);
	if (ret) {
		dev_err(dev, "could not request IRQ: %d\n", ret);
		goto error_clk;
	}

	ret = devm_spi_register_master(dev, master);
	if (ret) {
		dev_err(dev, "Failed to register master\n");
		goto error_clk;
	}

	return 0;

error_clk:
	clk_disable_unprepare(spi->clk);
error:
	spi_master_put(master);
out:
	return ret;
}

static int a3700_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct a3700_spi *spi = spi_master_get_devdata(master);

	clk_unprepare(spi->clk);

	return 0;
}

static struct platform_driver a3700_spi_driver = {
	.driver = {
		.name	= DRIVER_NAME,
		.of_match_table = of_match_ptr(a3700_spi_dt_ids),
	},
	.probe		= a3700_spi_probe,
	.remove		= a3700_spi_remove,
};

module_platform_driver(a3700_spi_driver);

MODULE_DESCRIPTION("Armada-3700 SPI driver");
MODULE_AUTHOR("Wilson Ding <dingwei@marvell.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);

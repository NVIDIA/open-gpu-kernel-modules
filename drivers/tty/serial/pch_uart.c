// SPDX-License-Identifier: GPL-2.0
/*
 *Copyright (C) 2011 LAPIS Semiconductor Co., Ltd.
 */
#include <linux/kernel.h>
#include <linux/serial_reg.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/dmi.h>
#include <linux/nmi.h>
#include <linux/delay.h>
#include <linux/of.h>

#include <linux/debugfs.h>
#include <linux/dmaengine.h>
#include <linux/pch_dma.h>

enum {
	PCH_UART_HANDLED_RX_INT_SHIFT,
	PCH_UART_HANDLED_TX_INT_SHIFT,
	PCH_UART_HANDLED_RX_ERR_INT_SHIFT,
	PCH_UART_HANDLED_RX_TRG_INT_SHIFT,
	PCH_UART_HANDLED_MS_INT_SHIFT,
	PCH_UART_HANDLED_LS_INT_SHIFT,
};

#define PCH_UART_DRIVER_DEVICE "ttyPCH"

/* Set the max number of UART port
 * Intel EG20T PCH: 4 port
 * LAPIS Semiconductor ML7213 IOH: 3 port
 * LAPIS Semiconductor ML7223 IOH: 2 port
*/
#define PCH_UART_NR	4

#define PCH_UART_HANDLED_RX_INT	(1<<((PCH_UART_HANDLED_RX_INT_SHIFT)<<1))
#define PCH_UART_HANDLED_TX_INT	(1<<((PCH_UART_HANDLED_TX_INT_SHIFT)<<1))
#define PCH_UART_HANDLED_RX_ERR_INT	(1<<((\
					PCH_UART_HANDLED_RX_ERR_INT_SHIFT)<<1))
#define PCH_UART_HANDLED_RX_TRG_INT	(1<<((\
					PCH_UART_HANDLED_RX_TRG_INT_SHIFT)<<1))
#define PCH_UART_HANDLED_MS_INT	(1<<((PCH_UART_HANDLED_MS_INT_SHIFT)<<1))

#define PCH_UART_HANDLED_LS_INT	(1<<((PCH_UART_HANDLED_LS_INT_SHIFT)<<1))

#define PCH_UART_RBR		0x00
#define PCH_UART_THR		0x00

#define PCH_UART_IER_MASK	(PCH_UART_IER_ERBFI|PCH_UART_IER_ETBEI|\
				PCH_UART_IER_ELSI|PCH_UART_IER_EDSSI)
#define PCH_UART_IER_ERBFI	0x00000001
#define PCH_UART_IER_ETBEI	0x00000002
#define PCH_UART_IER_ELSI	0x00000004
#define PCH_UART_IER_EDSSI	0x00000008

#define PCH_UART_IIR_IP			0x00000001
#define PCH_UART_IIR_IID		0x00000006
#define PCH_UART_IIR_MSI		0x00000000
#define PCH_UART_IIR_TRI		0x00000002
#define PCH_UART_IIR_RRI		0x00000004
#define PCH_UART_IIR_REI		0x00000006
#define PCH_UART_IIR_TOI		0x00000008
#define PCH_UART_IIR_FIFO256		0x00000020
#define PCH_UART_IIR_FIFO64		PCH_UART_IIR_FIFO256
#define PCH_UART_IIR_FE			0x000000C0

#define PCH_UART_FCR_FIFOE		0x00000001
#define PCH_UART_FCR_RFR		0x00000002
#define PCH_UART_FCR_TFR		0x00000004
#define PCH_UART_FCR_DMS		0x00000008
#define PCH_UART_FCR_FIFO256		0x00000020
#define PCH_UART_FCR_RFTL		0x000000C0

#define PCH_UART_FCR_RFTL1		0x00000000
#define PCH_UART_FCR_RFTL64		0x00000040
#define PCH_UART_FCR_RFTL128		0x00000080
#define PCH_UART_FCR_RFTL224		0x000000C0
#define PCH_UART_FCR_RFTL16		PCH_UART_FCR_RFTL64
#define PCH_UART_FCR_RFTL32		PCH_UART_FCR_RFTL128
#define PCH_UART_FCR_RFTL56		PCH_UART_FCR_RFTL224
#define PCH_UART_FCR_RFTL4		PCH_UART_FCR_RFTL64
#define PCH_UART_FCR_RFTL8		PCH_UART_FCR_RFTL128
#define PCH_UART_FCR_RFTL14		PCH_UART_FCR_RFTL224
#define PCH_UART_FCR_RFTL_SHIFT		6

#define PCH_UART_LCR_WLS	0x00000003
#define PCH_UART_LCR_STB	0x00000004
#define PCH_UART_LCR_PEN	0x00000008
#define PCH_UART_LCR_EPS	0x00000010
#define PCH_UART_LCR_SP		0x00000020
#define PCH_UART_LCR_SB		0x00000040
#define PCH_UART_LCR_DLAB	0x00000080
#define PCH_UART_LCR_NP		0x00000000
#define PCH_UART_LCR_OP		PCH_UART_LCR_PEN
#define PCH_UART_LCR_EP		(PCH_UART_LCR_PEN | PCH_UART_LCR_EPS)
#define PCH_UART_LCR_1P		(PCH_UART_LCR_PEN | PCH_UART_LCR_SP)
#define PCH_UART_LCR_0P		(PCH_UART_LCR_PEN | PCH_UART_LCR_EPS |\
				PCH_UART_LCR_SP)

#define PCH_UART_LCR_5BIT	0x00000000
#define PCH_UART_LCR_6BIT	0x00000001
#define PCH_UART_LCR_7BIT	0x00000002
#define PCH_UART_LCR_8BIT	0x00000003

#define PCH_UART_MCR_DTR	0x00000001
#define PCH_UART_MCR_RTS	0x00000002
#define PCH_UART_MCR_OUT	0x0000000C
#define PCH_UART_MCR_LOOP	0x00000010
#define PCH_UART_MCR_AFE	0x00000020

#define PCH_UART_LSR_DR		0x00000001
#define PCH_UART_LSR_ERR	(1<<7)

#define PCH_UART_MSR_DCTS	0x00000001
#define PCH_UART_MSR_DDSR	0x00000002
#define PCH_UART_MSR_TERI	0x00000004
#define PCH_UART_MSR_DDCD	0x00000008
#define PCH_UART_MSR_CTS	0x00000010
#define PCH_UART_MSR_DSR	0x00000020
#define PCH_UART_MSR_RI		0x00000040
#define PCH_UART_MSR_DCD	0x00000080
#define PCH_UART_MSR_DELTA	(PCH_UART_MSR_DCTS | PCH_UART_MSR_DDSR |\
				PCH_UART_MSR_TERI | PCH_UART_MSR_DDCD)

#define PCH_UART_DLL		0x00
#define PCH_UART_DLM		0x01

#define PCH_UART_BRCSR		0x0E

#define PCH_UART_IID_RLS	(PCH_UART_IIR_REI)
#define PCH_UART_IID_RDR	(PCH_UART_IIR_RRI)
#define PCH_UART_IID_RDR_TO	(PCH_UART_IIR_RRI | PCH_UART_IIR_TOI)
#define PCH_UART_IID_THRE	(PCH_UART_IIR_TRI)
#define PCH_UART_IID_MS		(PCH_UART_IIR_MSI)

#define PCH_UART_HAL_PARITY_NONE	(PCH_UART_LCR_NP)
#define PCH_UART_HAL_PARITY_ODD		(PCH_UART_LCR_OP)
#define PCH_UART_HAL_PARITY_EVEN	(PCH_UART_LCR_EP)
#define PCH_UART_HAL_PARITY_FIX1	(PCH_UART_LCR_1P)
#define PCH_UART_HAL_PARITY_FIX0	(PCH_UART_LCR_0P)
#define PCH_UART_HAL_5BIT		(PCH_UART_LCR_5BIT)
#define PCH_UART_HAL_6BIT		(PCH_UART_LCR_6BIT)
#define PCH_UART_HAL_7BIT		(PCH_UART_LCR_7BIT)
#define PCH_UART_HAL_8BIT		(PCH_UART_LCR_8BIT)
#define PCH_UART_HAL_STB1		0
#define PCH_UART_HAL_STB2		(PCH_UART_LCR_STB)

#define PCH_UART_HAL_CLR_TX_FIFO	(PCH_UART_FCR_TFR)
#define PCH_UART_HAL_CLR_RX_FIFO	(PCH_UART_FCR_RFR)
#define PCH_UART_HAL_CLR_ALL_FIFO	(PCH_UART_HAL_CLR_TX_FIFO | \
					PCH_UART_HAL_CLR_RX_FIFO)

#define PCH_UART_HAL_DMA_MODE0		0
#define PCH_UART_HAL_FIFO_DIS		0
#define PCH_UART_HAL_FIFO16		(PCH_UART_FCR_FIFOE)
#define PCH_UART_HAL_FIFO256		(PCH_UART_FCR_FIFOE | \
					PCH_UART_FCR_FIFO256)
#define PCH_UART_HAL_FIFO64		(PCH_UART_HAL_FIFO256)
#define PCH_UART_HAL_TRIGGER1		(PCH_UART_FCR_RFTL1)
#define PCH_UART_HAL_TRIGGER64		(PCH_UART_FCR_RFTL64)
#define PCH_UART_HAL_TRIGGER128		(PCH_UART_FCR_RFTL128)
#define PCH_UART_HAL_TRIGGER224		(PCH_UART_FCR_RFTL224)
#define PCH_UART_HAL_TRIGGER16		(PCH_UART_FCR_RFTL16)
#define PCH_UART_HAL_TRIGGER32		(PCH_UART_FCR_RFTL32)
#define PCH_UART_HAL_TRIGGER56		(PCH_UART_FCR_RFTL56)
#define PCH_UART_HAL_TRIGGER4		(PCH_UART_FCR_RFTL4)
#define PCH_UART_HAL_TRIGGER8		(PCH_UART_FCR_RFTL8)
#define PCH_UART_HAL_TRIGGER14		(PCH_UART_FCR_RFTL14)
#define PCH_UART_HAL_TRIGGER_L		(PCH_UART_FCR_RFTL64)
#define PCH_UART_HAL_TRIGGER_M		(PCH_UART_FCR_RFTL128)
#define PCH_UART_HAL_TRIGGER_H		(PCH_UART_FCR_RFTL224)

#define PCH_UART_HAL_RX_INT		(PCH_UART_IER_ERBFI)
#define PCH_UART_HAL_TX_INT		(PCH_UART_IER_ETBEI)
#define PCH_UART_HAL_RX_ERR_INT		(PCH_UART_IER_ELSI)
#define PCH_UART_HAL_MS_INT		(PCH_UART_IER_EDSSI)
#define PCH_UART_HAL_ALL_INT		(PCH_UART_IER_MASK)

#define PCH_UART_HAL_DTR		(PCH_UART_MCR_DTR)
#define PCH_UART_HAL_RTS		(PCH_UART_MCR_RTS)
#define PCH_UART_HAL_OUT		(PCH_UART_MCR_OUT)
#define PCH_UART_HAL_LOOP		(PCH_UART_MCR_LOOP)
#define PCH_UART_HAL_AFE		(PCH_UART_MCR_AFE)

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

#define DEFAULT_UARTCLK   1843200 /*   1.8432 MHz */
#define CMITC_UARTCLK   192000000 /* 192.0000 MHz */
#define FRI2_64_UARTCLK  64000000 /*  64.0000 MHz */
#define FRI2_48_UARTCLK  48000000 /*  48.0000 MHz */
#define NTC1_UARTCLK     64000000 /*  64.0000 MHz */
#define MINNOW_UARTCLK   50000000 /*  50.0000 MHz */

struct pch_uart_buffer {
	unsigned char *buf;
	int size;
};

struct eg20t_port {
	struct uart_port port;
	int port_type;
	void __iomem *membase;
	resource_size_t mapbase;
	unsigned int iobase;
	struct pci_dev *pdev;
	int fifo_size;
	unsigned int uartclk;
	int start_tx;
	int start_rx;
	int tx_empty;
	int trigger;
	int trigger_level;
	struct pch_uart_buffer rxbuf;
	unsigned int dmsr;
	unsigned int fcr;
	unsigned int mcr;
	unsigned int use_dma;
	struct dma_async_tx_descriptor	*desc_tx;
	struct dma_async_tx_descriptor	*desc_rx;
	struct pch_dma_slave		param_tx;
	struct pch_dma_slave		param_rx;
	struct dma_chan			*chan_tx;
	struct dma_chan			*chan_rx;
	struct scatterlist		*sg_tx_p;
	int				nent;
	int				orig_nent;
	struct scatterlist		sg_rx;
	int				tx_dma_use;
	void				*rx_buf_virt;
	dma_addr_t			rx_buf_dma;

#define IRQ_NAME_SIZE 17
	char				irq_name[IRQ_NAME_SIZE];

	/* protect the eg20t_port private structure and io access to membase */
	spinlock_t lock;
};

/**
 * struct pch_uart_driver_data - private data structure for UART-DMA
 * @port_type:			The type of UART port
 * @line_no:			UART port line number (0, 1, 2...)
 */
struct pch_uart_driver_data {
	int port_type;
	int line_no;
};

enum pch_uart_num_t {
	pch_et20t_uart0 = 0,
	pch_et20t_uart1,
	pch_et20t_uart2,
	pch_et20t_uart3,
	pch_ml7213_uart0,
	pch_ml7213_uart1,
	pch_ml7213_uart2,
	pch_ml7223_uart0,
	pch_ml7223_uart1,
	pch_ml7831_uart0,
	pch_ml7831_uart1,
};

static struct pch_uart_driver_data drv_dat[] = {
	[pch_et20t_uart0] = {PORT_PCH_8LINE, 0},
	[pch_et20t_uart1] = {PORT_PCH_2LINE, 1},
	[pch_et20t_uart2] = {PORT_PCH_2LINE, 2},
	[pch_et20t_uart3] = {PORT_PCH_2LINE, 3},
	[pch_ml7213_uart0] = {PORT_PCH_8LINE, 0},
	[pch_ml7213_uart1] = {PORT_PCH_2LINE, 1},
	[pch_ml7213_uart2] = {PORT_PCH_2LINE, 2},
	[pch_ml7223_uart0] = {PORT_PCH_8LINE, 0},
	[pch_ml7223_uart1] = {PORT_PCH_2LINE, 1},
	[pch_ml7831_uart0] = {PORT_PCH_8LINE, 0},
	[pch_ml7831_uart1] = {PORT_PCH_2LINE, 1},
};

#ifdef CONFIG_SERIAL_PCH_UART_CONSOLE
static struct eg20t_port *pch_uart_ports[PCH_UART_NR];
#endif
static unsigned int default_baud = 9600;
static unsigned int user_uartclk = 0;
static const int trigger_level_256[4] = { 1, 64, 128, 224 };
static const int trigger_level_64[4] = { 1, 16, 32, 56 };
static const int trigger_level_16[4] = { 1, 4, 8, 14 };
static const int trigger_level_1[4] = { 1, 1, 1, 1 };

#define PCH_REGS_BUFSIZE	1024


static ssize_t port_show_regs(struct file *file, char __user *user_buf,
				size_t count, loff_t *ppos)
{
	struct eg20t_port *priv = file->private_data;
	char *buf;
	u32 len = 0;
	ssize_t ret;
	unsigned char lcr;

	buf = kzalloc(PCH_REGS_BUFSIZE, GFP_KERNEL);
	if (!buf)
		return 0;

	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"PCH EG20T port[%d] regs:\n", priv->port.line);

	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"=================================\n");
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"IER: \t0x%02x\n", ioread8(priv->membase + UART_IER));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"IIR: \t0x%02x\n", ioread8(priv->membase + UART_IIR));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"LCR: \t0x%02x\n", ioread8(priv->membase + UART_LCR));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"MCR: \t0x%02x\n", ioread8(priv->membase + UART_MCR));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"LSR: \t0x%02x\n", ioread8(priv->membase + UART_LSR));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"MSR: \t0x%02x\n", ioread8(priv->membase + UART_MSR));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"BRCSR: \t0x%02x\n",
			ioread8(priv->membase + PCH_UART_BRCSR));

	lcr = ioread8(priv->membase + UART_LCR);
	iowrite8(PCH_UART_LCR_DLAB, priv->membase + UART_LCR);
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"DLL: \t0x%02x\n", ioread8(priv->membase + UART_DLL));
	len += scnprintf(buf + len, PCH_REGS_BUFSIZE - len,
			"DLM: \t0x%02x\n", ioread8(priv->membase + UART_DLM));
	iowrite8(lcr, priv->membase + UART_LCR);

	if (len > PCH_REGS_BUFSIZE)
		len = PCH_REGS_BUFSIZE;

	ret =  simple_read_from_buffer(user_buf, count, ppos, buf, len);
	kfree(buf);
	return ret;
}

static const struct file_operations port_regs_ops = {
	.owner		= THIS_MODULE,
	.open		= simple_open,
	.read		= port_show_regs,
	.llseek		= default_llseek,
};

static const struct dmi_system_id pch_uart_dmi_table[] = {
	{
		.ident = "CM-iTC",
		{
			DMI_MATCH(DMI_BOARD_NAME, "CM-iTC"),
		},
		(void *)CMITC_UARTCLK,
	},
	{
		.ident = "FRI2",
		{
			DMI_MATCH(DMI_BIOS_VERSION, "FRI2"),
		},
		(void *)FRI2_64_UARTCLK,
	},
	{
		.ident = "Fish River Island II",
		{
			DMI_MATCH(DMI_PRODUCT_NAME, "Fish River Island II"),
		},
		(void *)FRI2_48_UARTCLK,
	},
	{
		.ident = "COMe-mTT",
		{
			DMI_MATCH(DMI_BOARD_NAME, "COMe-mTT"),
		},
		(void *)NTC1_UARTCLK,
	},
	{
		.ident = "nanoETXexpress-TT",
		{
			DMI_MATCH(DMI_BOARD_NAME, "nanoETXexpress-TT"),
		},
		(void *)NTC1_UARTCLK,
	},
	{
		.ident = "MinnowBoard",
		{
			DMI_MATCH(DMI_BOARD_NAME, "MinnowBoard"),
		},
		(void *)MINNOW_UARTCLK,
	},
	{ }
};

/* Return UART clock, checking for board specific clocks. */
static unsigned int pch_uart_get_uartclk(void)
{
	const struct dmi_system_id *d;

	if (user_uartclk)
		return user_uartclk;

	d = dmi_first_match(pch_uart_dmi_table);
	if (d)
		return (unsigned long)d->driver_data;

	return DEFAULT_UARTCLK;
}

static void pch_uart_hal_enable_interrupt(struct eg20t_port *priv,
					  unsigned int flag)
{
	u8 ier = ioread8(priv->membase + UART_IER);
	ier |= flag & PCH_UART_IER_MASK;
	iowrite8(ier, priv->membase + UART_IER);
}

static void pch_uart_hal_disable_interrupt(struct eg20t_port *priv,
					   unsigned int flag)
{
	u8 ier = ioread8(priv->membase + UART_IER);
	ier &= ~(flag & PCH_UART_IER_MASK);
	iowrite8(ier, priv->membase + UART_IER);
}

static int pch_uart_hal_set_line(struct eg20t_port *priv, unsigned int baud,
				 unsigned int parity, unsigned int bits,
				 unsigned int stb)
{
	unsigned int dll, dlm, lcr;
	int div;

	div = DIV_ROUND_CLOSEST(priv->uartclk / 16, baud);
	if (div < 0 || USHRT_MAX <= div) {
		dev_err(priv->port.dev, "Invalid Baud(div=0x%x)\n", div);
		return -EINVAL;
	}

	dll = (unsigned int)div & 0x00FFU;
	dlm = ((unsigned int)div >> 8) & 0x00FFU;

	if (parity & ~(PCH_UART_LCR_PEN | PCH_UART_LCR_EPS | PCH_UART_LCR_SP)) {
		dev_err(priv->port.dev, "Invalid parity(0x%x)\n", parity);
		return -EINVAL;
	}

	if (bits & ~PCH_UART_LCR_WLS) {
		dev_err(priv->port.dev, "Invalid bits(0x%x)\n", bits);
		return -EINVAL;
	}

	if (stb & ~PCH_UART_LCR_STB) {
		dev_err(priv->port.dev, "Invalid STB(0x%x)\n", stb);
		return -EINVAL;
	}

	lcr = parity;
	lcr |= bits;
	lcr |= stb;

	dev_dbg(priv->port.dev, "%s:baud = %u, div = %04x, lcr = %02x (%lu)\n",
		 __func__, baud, div, lcr, jiffies);
	iowrite8(PCH_UART_LCR_DLAB, priv->membase + UART_LCR);
	iowrite8(dll, priv->membase + PCH_UART_DLL);
	iowrite8(dlm, priv->membase + PCH_UART_DLM);
	iowrite8(lcr, priv->membase + UART_LCR);

	return 0;
}

static int pch_uart_hal_fifo_reset(struct eg20t_port *priv,
				    unsigned int flag)
{
	if (flag & ~(PCH_UART_FCR_TFR | PCH_UART_FCR_RFR)) {
		dev_err(priv->port.dev, "%s:Invalid flag(0x%x)\n",
			__func__, flag);
		return -EINVAL;
	}

	iowrite8(PCH_UART_FCR_FIFOE | priv->fcr, priv->membase + UART_FCR);
	iowrite8(PCH_UART_FCR_FIFOE | priv->fcr | flag,
		 priv->membase + UART_FCR);
	iowrite8(priv->fcr, priv->membase + UART_FCR);

	return 0;
}

static int pch_uart_hal_set_fifo(struct eg20t_port *priv,
				 unsigned int dmamode,
				 unsigned int fifo_size, unsigned int trigger)
{
	u8 fcr;

	if (dmamode & ~PCH_UART_FCR_DMS) {
		dev_err(priv->port.dev, "%s:Invalid DMA Mode(0x%x)\n",
			__func__, dmamode);
		return -EINVAL;
	}

	if (fifo_size & ~(PCH_UART_FCR_FIFOE | PCH_UART_FCR_FIFO256)) {
		dev_err(priv->port.dev, "%s:Invalid FIFO SIZE(0x%x)\n",
			__func__, fifo_size);
		return -EINVAL;
	}

	if (trigger & ~PCH_UART_FCR_RFTL) {
		dev_err(priv->port.dev, "%s:Invalid TRIGGER(0x%x)\n",
			__func__, trigger);
		return -EINVAL;
	}

	switch (priv->fifo_size) {
	case 256:
		priv->trigger_level =
		    trigger_level_256[trigger >> PCH_UART_FCR_RFTL_SHIFT];
		break;
	case 64:
		priv->trigger_level =
		    trigger_level_64[trigger >> PCH_UART_FCR_RFTL_SHIFT];
		break;
	case 16:
		priv->trigger_level =
		    trigger_level_16[trigger >> PCH_UART_FCR_RFTL_SHIFT];
		break;
	default:
		priv->trigger_level =
		    trigger_level_1[trigger >> PCH_UART_FCR_RFTL_SHIFT];
		break;
	}
	fcr =
	    dmamode | fifo_size | trigger | PCH_UART_FCR_RFR | PCH_UART_FCR_TFR;
	iowrite8(PCH_UART_FCR_FIFOE, priv->membase + UART_FCR);
	iowrite8(PCH_UART_FCR_FIFOE | PCH_UART_FCR_RFR | PCH_UART_FCR_TFR,
		 priv->membase + UART_FCR);
	iowrite8(fcr, priv->membase + UART_FCR);
	priv->fcr = fcr;

	return 0;
}

static u8 pch_uart_hal_get_modem(struct eg20t_port *priv)
{
	unsigned int msr = ioread8(priv->membase + UART_MSR);
	priv->dmsr = msr & PCH_UART_MSR_DELTA;
	return (u8)msr;
}

static void pch_uart_hal_write(struct eg20t_port *priv,
			      const unsigned char *buf, int tx_size)
{
	int i;
	unsigned int thr;

	for (i = 0; i < tx_size;) {
		thr = buf[i++];
		iowrite8(thr, priv->membase + PCH_UART_THR);
	}
}

static int pch_uart_hal_read(struct eg20t_port *priv, unsigned char *buf,
			     int rx_size)
{
	int i;
	u8 rbr, lsr;
	struct uart_port *port = &priv->port;

	lsr = ioread8(priv->membase + UART_LSR);
	for (i = 0, lsr = ioread8(priv->membase + UART_LSR);
	     i < rx_size && lsr & (UART_LSR_DR | UART_LSR_BI);
	     lsr = ioread8(priv->membase + UART_LSR)) {
		rbr = ioread8(priv->membase + PCH_UART_RBR);

		if (lsr & UART_LSR_BI) {
			port->icount.brk++;
			if (uart_handle_break(port))
				continue;
		}
		if (uart_handle_sysrq_char(port, rbr))
			continue;

		buf[i++] = rbr;
	}
	return i;
}

static unsigned char pch_uart_hal_get_iid(struct eg20t_port *priv)
{
	return ioread8(priv->membase + UART_IIR) &\
		      (PCH_UART_IIR_IID | PCH_UART_IIR_TOI | PCH_UART_IIR_IP);
}

static u8 pch_uart_hal_get_line_status(struct eg20t_port *priv)
{
	return ioread8(priv->membase + UART_LSR);
}

static void pch_uart_hal_set_break(struct eg20t_port *priv, int on)
{
	unsigned int lcr;

	lcr = ioread8(priv->membase + UART_LCR);
	if (on)
		lcr |= PCH_UART_LCR_SB;
	else
		lcr &= ~PCH_UART_LCR_SB;

	iowrite8(lcr, priv->membase + UART_LCR);
}

static int push_rx(struct eg20t_port *priv, const unsigned char *buf,
		   int size)
{
	struct uart_port *port = &priv->port;
	struct tty_port *tport = &port->state->port;

	tty_insert_flip_string(tport, buf, size);
	tty_flip_buffer_push(tport);

	return 0;
}

static int pop_tx_x(struct eg20t_port *priv, unsigned char *buf)
{
	int ret = 0;
	struct uart_port *port = &priv->port;

	if (port->x_char) {
		dev_dbg(priv->port.dev, "%s:X character send %02x (%lu)\n",
			__func__, port->x_char, jiffies);
		buf[0] = port->x_char;
		port->x_char = 0;
		ret = 1;
	}

	return ret;
}

static int dma_push_rx(struct eg20t_port *priv, int size)
{
	int room;
	struct uart_port *port = &priv->port;
	struct tty_port *tport = &port->state->port;

	room = tty_buffer_request_room(tport, size);

	if (room < size)
		dev_warn(port->dev, "Rx overrun: dropping %u bytes\n",
			 size - room);
	if (!room)
		return 0;

	tty_insert_flip_string(tport, sg_virt(&priv->sg_rx), size);

	port->icount.rx += room;

	return room;
}

static void pch_free_dma(struct uart_port *port)
{
	struct eg20t_port *priv;
	priv = container_of(port, struct eg20t_port, port);

	if (priv->chan_tx) {
		dma_release_channel(priv->chan_tx);
		priv->chan_tx = NULL;
	}
	if (priv->chan_rx) {
		dma_release_channel(priv->chan_rx);
		priv->chan_rx = NULL;
	}

	if (priv->rx_buf_dma) {
		dma_free_coherent(port->dev, port->fifosize, priv->rx_buf_virt,
				  priv->rx_buf_dma);
		priv->rx_buf_virt = NULL;
		priv->rx_buf_dma = 0;
	}

	return;
}

static bool filter(struct dma_chan *chan, void *slave)
{
	struct pch_dma_slave *param = slave;

	if ((chan->chan_id == param->chan_id) && (param->dma_dev ==
						  chan->device->dev)) {
		chan->private = param;
		return true;
	} else {
		return false;
	}
}

static void pch_request_dma(struct uart_port *port)
{
	dma_cap_mask_t mask;
	struct dma_chan *chan;
	struct pci_dev *dma_dev;
	struct pch_dma_slave *param;
	struct eg20t_port *priv =
				container_of(port, struct eg20t_port, port);
	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	/* Get DMA's dev information */
	dma_dev = pci_get_slot(priv->pdev->bus,
			PCI_DEVFN(PCI_SLOT(priv->pdev->devfn), 0));

	/* Set Tx DMA */
	param = &priv->param_tx;
	param->dma_dev = &dma_dev->dev;
	param->chan_id = priv->port.line * 2; /* Tx = 0, 2, 4, ... */

	param->tx_reg = port->mapbase + UART_TX;
	chan = dma_request_channel(mask, filter, param);
	if (!chan) {
		dev_err(priv->port.dev, "%s:dma_request_channel FAILS(Tx)\n",
			__func__);
		return;
	}
	priv->chan_tx = chan;

	/* Set Rx DMA */
	param = &priv->param_rx;
	param->dma_dev = &dma_dev->dev;
	param->chan_id = priv->port.line * 2 + 1; /* Rx = Tx + 1 */

	param->rx_reg = port->mapbase + UART_RX;
	chan = dma_request_channel(mask, filter, param);
	if (!chan) {
		dev_err(priv->port.dev, "%s:dma_request_channel FAILS(Rx)\n",
			__func__);
		dma_release_channel(priv->chan_tx);
		priv->chan_tx = NULL;
		return;
	}

	/* Get Consistent memory for DMA */
	priv->rx_buf_virt = dma_alloc_coherent(port->dev, port->fifosize,
				    &priv->rx_buf_dma, GFP_KERNEL);
	priv->chan_rx = chan;
}

static void pch_dma_rx_complete(void *arg)
{
	struct eg20t_port *priv = arg;
	struct uart_port *port = &priv->port;
	int count;

	dma_sync_sg_for_cpu(port->dev, &priv->sg_rx, 1, DMA_FROM_DEVICE);
	count = dma_push_rx(priv, priv->trigger_level);
	if (count)
		tty_flip_buffer_push(&port->state->port);
	async_tx_ack(priv->desc_rx);
	pch_uart_hal_enable_interrupt(priv, PCH_UART_HAL_RX_INT |
					    PCH_UART_HAL_RX_ERR_INT);
}

static void pch_dma_tx_complete(void *arg)
{
	struct eg20t_port *priv = arg;
	struct uart_port *port = &priv->port;
	struct circ_buf *xmit = &port->state->xmit;
	struct scatterlist *sg = priv->sg_tx_p;
	int i;

	for (i = 0; i < priv->nent; i++, sg++) {
		xmit->tail += sg_dma_len(sg);
		port->icount.tx += sg_dma_len(sg);
	}
	xmit->tail &= UART_XMIT_SIZE - 1;
	async_tx_ack(priv->desc_tx);
	dma_unmap_sg(port->dev, sg, priv->orig_nent, DMA_TO_DEVICE);
	priv->tx_dma_use = 0;
	priv->nent = 0;
	priv->orig_nent = 0;
	kfree(priv->sg_tx_p);
	pch_uart_hal_enable_interrupt(priv, PCH_UART_HAL_TX_INT);
}

static int pop_tx(struct eg20t_port *priv, int size)
{
	int count = 0;
	struct uart_port *port = &priv->port;
	struct circ_buf *xmit = &port->state->xmit;

	if (uart_tx_stopped(port) || uart_circ_empty(xmit) || count >= size)
		goto pop_tx_end;

	do {
		int cnt_to_end =
		    CIRC_CNT_TO_END(xmit->head, xmit->tail, UART_XMIT_SIZE);
		int sz = min(size - count, cnt_to_end);
		pch_uart_hal_write(priv, &xmit->buf[xmit->tail], sz);
		xmit->tail = (xmit->tail + sz) & (UART_XMIT_SIZE - 1);
		count += sz;
	} while (!uart_circ_empty(xmit) && count < size);

pop_tx_end:
	dev_dbg(priv->port.dev, "%d characters. Remained %d characters.(%lu)\n",
		 count, size - count, jiffies);

	return count;
}

static int handle_rx_to(struct eg20t_port *priv)
{
	struct pch_uart_buffer *buf;
	int rx_size;
	int ret;
	if (!priv->start_rx) {
		pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_RX_INT |
						     PCH_UART_HAL_RX_ERR_INT);
		return 0;
	}
	buf = &priv->rxbuf;
	do {
		rx_size = pch_uart_hal_read(priv, buf->buf, buf->size);
		ret = push_rx(priv, buf->buf, rx_size);
		if (ret)
			return 0;
	} while (rx_size == buf->size);

	return PCH_UART_HANDLED_RX_INT;
}

static int handle_rx(struct eg20t_port *priv)
{
	return handle_rx_to(priv);
}

static int dma_handle_rx(struct eg20t_port *priv)
{
	struct uart_port *port = &priv->port;
	struct dma_async_tx_descriptor *desc;
	struct scatterlist *sg;

	priv = container_of(port, struct eg20t_port, port);
	sg = &priv->sg_rx;

	sg_init_table(&priv->sg_rx, 1); /* Initialize SG table */

	sg_dma_len(sg) = priv->trigger_level;

	sg_set_page(&priv->sg_rx, virt_to_page(priv->rx_buf_virt),
		     sg_dma_len(sg), offset_in_page(priv->rx_buf_virt));

	sg_dma_address(sg) = priv->rx_buf_dma;

	desc = dmaengine_prep_slave_sg(priv->chan_rx,
			sg, 1, DMA_DEV_TO_MEM,
			DMA_PREP_INTERRUPT | DMA_CTRL_ACK);

	if (!desc)
		return 0;

	priv->desc_rx = desc;
	desc->callback = pch_dma_rx_complete;
	desc->callback_param = priv;
	desc->tx_submit(desc);
	dma_async_issue_pending(priv->chan_rx);

	return PCH_UART_HANDLED_RX_INT;
}

static unsigned int handle_tx(struct eg20t_port *priv)
{
	struct uart_port *port = &priv->port;
	struct circ_buf *xmit = &port->state->xmit;
	int fifo_size;
	int tx_size;
	int size;
	int tx_empty;

	if (!priv->start_tx) {
		dev_info(priv->port.dev, "%s:Tx isn't started. (%lu)\n",
			__func__, jiffies);
		pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_TX_INT);
		priv->tx_empty = 1;
		return 0;
	}

	fifo_size = max(priv->fifo_size, 1);
	tx_empty = 1;
	if (pop_tx_x(priv, xmit->buf)) {
		pch_uart_hal_write(priv, xmit->buf, 1);
		port->icount.tx++;
		tx_empty = 0;
		fifo_size--;
	}
	size = min(xmit->head - xmit->tail, fifo_size);
	if (size < 0)
		size = fifo_size;

	tx_size = pop_tx(priv, size);
	if (tx_size > 0) {
		port->icount.tx += tx_size;
		tx_empty = 0;
	}

	priv->tx_empty = tx_empty;

	if (tx_empty) {
		pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_TX_INT);
		uart_write_wakeup(port);
	}

	return PCH_UART_HANDLED_TX_INT;
}

static unsigned int dma_handle_tx(struct eg20t_port *priv)
{
	struct uart_port *port = &priv->port;
	struct circ_buf *xmit = &port->state->xmit;
	struct scatterlist *sg;
	int nent;
	int fifo_size;
	struct dma_async_tx_descriptor *desc;
	int num;
	int i;
	int bytes;
	int size;
	int rem;

	if (!priv->start_tx) {
		dev_info(priv->port.dev, "%s:Tx isn't started. (%lu)\n",
			__func__, jiffies);
		pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_TX_INT);
		priv->tx_empty = 1;
		return 0;
	}

	if (priv->tx_dma_use) {
		dev_dbg(priv->port.dev, "%s:Tx is not completed. (%lu)\n",
			__func__, jiffies);
		pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_TX_INT);
		priv->tx_empty = 1;
		return 0;
	}

	fifo_size = max(priv->fifo_size, 1);
	if (pop_tx_x(priv, xmit->buf)) {
		pch_uart_hal_write(priv, xmit->buf, 1);
		port->icount.tx++;
		fifo_size--;
	}

	bytes = min((int)CIRC_CNT(xmit->head, xmit->tail,
			     UART_XMIT_SIZE), CIRC_CNT_TO_END(xmit->head,
			     xmit->tail, UART_XMIT_SIZE));
	if (!bytes) {
		dev_dbg(priv->port.dev, "%s 0 bytes return\n", __func__);
		pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_TX_INT);
		uart_write_wakeup(port);
		return 0;
	}

	if (bytes > fifo_size) {
		num = bytes / fifo_size + 1;
		size = fifo_size;
		rem = bytes % fifo_size;
	} else {
		num = 1;
		size = bytes;
		rem = bytes;
	}

	dev_dbg(priv->port.dev, "%s num=%d size=%d rem=%d\n",
		__func__, num, size, rem);

	priv->tx_dma_use = 1;

	priv->sg_tx_p = kmalloc_array(num, sizeof(struct scatterlist), GFP_ATOMIC);
	if (!priv->sg_tx_p) {
		dev_err(priv->port.dev, "%s:kzalloc Failed\n", __func__);
		return 0;
	}

	sg_init_table(priv->sg_tx_p, num); /* Initialize SG table */
	sg = priv->sg_tx_p;

	for (i = 0; i < num; i++, sg++) {
		if (i == (num - 1))
			sg_set_page(sg, virt_to_page(xmit->buf),
				    rem, fifo_size * i);
		else
			sg_set_page(sg, virt_to_page(xmit->buf),
				    size, fifo_size * i);
	}

	sg = priv->sg_tx_p;
	nent = dma_map_sg(port->dev, sg, num, DMA_TO_DEVICE);
	if (!nent) {
		dev_err(priv->port.dev, "%s:dma_map_sg Failed\n", __func__);
		return 0;
	}
	priv->orig_nent = num;
	priv->nent = nent;

	for (i = 0; i < nent; i++, sg++) {
		sg->offset = (xmit->tail & (UART_XMIT_SIZE - 1)) +
			      fifo_size * i;
		sg_dma_address(sg) = (sg_dma_address(sg) &
				    ~(UART_XMIT_SIZE - 1)) + sg->offset;
		if (i == (nent - 1))
			sg_dma_len(sg) = rem;
		else
			sg_dma_len(sg) = size;
	}

	desc = dmaengine_prep_slave_sg(priv->chan_tx,
					priv->sg_tx_p, nent, DMA_MEM_TO_DEV,
					DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc) {
		dev_err(priv->port.dev, "%s:dmaengine_prep_slave_sg Failed\n",
			__func__);
		return 0;
	}
	dma_sync_sg_for_device(port->dev, priv->sg_tx_p, nent, DMA_TO_DEVICE);
	priv->desc_tx = desc;
	desc->callback = pch_dma_tx_complete;
	desc->callback_param = priv;

	desc->tx_submit(desc);

	dma_async_issue_pending(priv->chan_tx);

	return PCH_UART_HANDLED_TX_INT;
}

static void pch_uart_err_ir(struct eg20t_port *priv, unsigned int lsr)
{
	struct uart_port *port = &priv->port;
	struct tty_struct *tty = tty_port_tty_get(&port->state->port);
	char   *error_msg[5] = {};
	int    i = 0;

	if (lsr & PCH_UART_LSR_ERR)
		error_msg[i++] = "Error data in FIFO\n";

	if (lsr & UART_LSR_FE) {
		port->icount.frame++;
		error_msg[i++] = "  Framing Error\n";
	}

	if (lsr & UART_LSR_PE) {
		port->icount.parity++;
		error_msg[i++] = "  Parity Error\n";
	}

	if (lsr & UART_LSR_OE) {
		port->icount.overrun++;
		error_msg[i++] = "  Overrun Error\n";
	}

	if (tty == NULL) {
		for (i = 0; error_msg[i] != NULL; i++)
			dev_err(&priv->pdev->dev, error_msg[i]);
	} else {
		tty_kref_put(tty);
	}
}

static irqreturn_t pch_uart_interrupt(int irq, void *dev_id)
{
	struct eg20t_port *priv = dev_id;
	unsigned int handled;
	u8 lsr;
	int ret = 0;
	unsigned char iid;
	unsigned long flags;
	int next = 1;
	u8 msr;

	spin_lock_irqsave(&priv->lock, flags);
	handled = 0;
	while (next) {
		iid = pch_uart_hal_get_iid(priv);
		if (iid & PCH_UART_IIR_IP) /* No Interrupt */
			break;
		switch (iid) {
		case PCH_UART_IID_RLS:	/* Receiver Line Status */
			lsr = pch_uart_hal_get_line_status(priv);
			if (lsr & (PCH_UART_LSR_ERR | UART_LSR_FE |
						UART_LSR_PE | UART_LSR_OE)) {
				pch_uart_err_ir(priv, lsr);
				ret = PCH_UART_HANDLED_RX_ERR_INT;
			} else {
				ret = PCH_UART_HANDLED_LS_INT;
			}
			break;
		case PCH_UART_IID_RDR:	/* Received Data Ready */
			if (priv->use_dma) {
				pch_uart_hal_disable_interrupt(priv,
						PCH_UART_HAL_RX_INT |
						PCH_UART_HAL_RX_ERR_INT);
				ret = dma_handle_rx(priv);
				if (!ret)
					pch_uart_hal_enable_interrupt(priv,
						PCH_UART_HAL_RX_INT |
						PCH_UART_HAL_RX_ERR_INT);
			} else {
				ret = handle_rx(priv);
			}
			break;
		case PCH_UART_IID_RDR_TO:	/* Received Data Ready
						   (FIFO Timeout) */
			ret = handle_rx_to(priv);
			break;
		case PCH_UART_IID_THRE:	/* Transmitter Holding Register
						   Empty */
			if (priv->use_dma)
				ret = dma_handle_tx(priv);
			else
				ret = handle_tx(priv);
			break;
		case PCH_UART_IID_MS:	/* Modem Status */
			msr = pch_uart_hal_get_modem(priv);
			next = 0; /* MS ir prioirty is the lowest. So, MS ir
				     means final interrupt */
			if ((msr & UART_MSR_ANY_DELTA) == 0)
				break;
			ret |= PCH_UART_HANDLED_MS_INT;
			break;
		default:	/* Never junp to this label */
			dev_err(priv->port.dev, "%s:iid=%02x (%lu)\n", __func__,
				iid, jiffies);
			ret = -1;
			next = 0;
			break;
		}
		handled |= (unsigned int)ret;
	}

	spin_unlock_irqrestore(&priv->lock, flags);
	return IRQ_RETVAL(handled);
}

/* This function tests whether the transmitter fifo and shifter for the port
						described by 'port' is empty. */
static unsigned int pch_uart_tx_empty(struct uart_port *port)
{
	struct eg20t_port *priv;

	priv = container_of(port, struct eg20t_port, port);
	if (priv->tx_empty)
		return TIOCSER_TEMT;
	else
		return 0;
}

/* Returns the current state of modem control inputs. */
static unsigned int pch_uart_get_mctrl(struct uart_port *port)
{
	struct eg20t_port *priv;
	u8 modem;
	unsigned int ret = 0;

	priv = container_of(port, struct eg20t_port, port);
	modem = pch_uart_hal_get_modem(priv);

	if (modem & UART_MSR_DCD)
		ret |= TIOCM_CAR;

	if (modem & UART_MSR_RI)
		ret |= TIOCM_RNG;

	if (modem & UART_MSR_DSR)
		ret |= TIOCM_DSR;

	if (modem & UART_MSR_CTS)
		ret |= TIOCM_CTS;

	return ret;
}

static void pch_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	u32 mcr = 0;
	struct eg20t_port *priv = container_of(port, struct eg20t_port, port);

	if (mctrl & TIOCM_DTR)
		mcr |= UART_MCR_DTR;
	if (mctrl & TIOCM_RTS)
		mcr |= UART_MCR_RTS;
	if (mctrl & TIOCM_LOOP)
		mcr |= UART_MCR_LOOP;

	if (priv->mcr & UART_MCR_AFE)
		mcr |= UART_MCR_AFE;

	if (mctrl)
		iowrite8(mcr, priv->membase + UART_MCR);
}

static void pch_uart_stop_tx(struct uart_port *port)
{
	struct eg20t_port *priv;
	priv = container_of(port, struct eg20t_port, port);
	priv->start_tx = 0;
	priv->tx_dma_use = 0;
}

static void pch_uart_start_tx(struct uart_port *port)
{
	struct eg20t_port *priv;

	priv = container_of(port, struct eg20t_port, port);

	if (priv->use_dma) {
		if (priv->tx_dma_use) {
			dev_dbg(priv->port.dev, "%s : Tx DMA is NOT empty.\n",
				__func__);
			return;
		}
	}

	priv->start_tx = 1;
	pch_uart_hal_enable_interrupt(priv, PCH_UART_HAL_TX_INT);
}

static void pch_uart_stop_rx(struct uart_port *port)
{
	struct eg20t_port *priv;
	priv = container_of(port, struct eg20t_port, port);
	priv->start_rx = 0;
	pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_RX_INT |
					     PCH_UART_HAL_RX_ERR_INT);
}

/* Enable the modem status interrupts. */
static void pch_uart_enable_ms(struct uart_port *port)
{
	struct eg20t_port *priv;
	priv = container_of(port, struct eg20t_port, port);
	pch_uart_hal_enable_interrupt(priv, PCH_UART_HAL_MS_INT);
}

/* Control the transmission of a break signal. */
static void pch_uart_break_ctl(struct uart_port *port, int ctl)
{
	struct eg20t_port *priv;
	unsigned long flags;

	priv = container_of(port, struct eg20t_port, port);
	spin_lock_irqsave(&priv->lock, flags);
	pch_uart_hal_set_break(priv, ctl);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* Grab any interrupt resources and initialise any low level driver state. */
static int pch_uart_startup(struct uart_port *port)
{
	struct eg20t_port *priv;
	int ret;
	int fifo_size;
	int trigger_level;

	priv = container_of(port, struct eg20t_port, port);
	priv->tx_empty = 1;

	if (port->uartclk)
		priv->uartclk = port->uartclk;
	else
		port->uartclk = priv->uartclk;

	pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_ALL_INT);
	ret = pch_uart_hal_set_line(priv, default_baud,
			      PCH_UART_HAL_PARITY_NONE, PCH_UART_HAL_8BIT,
			      PCH_UART_HAL_STB1);
	if (ret)
		return ret;

	switch (priv->fifo_size) {
	case 256:
		fifo_size = PCH_UART_HAL_FIFO256;
		break;
	case 64:
		fifo_size = PCH_UART_HAL_FIFO64;
		break;
	case 16:
		fifo_size = PCH_UART_HAL_FIFO16;
		break;
	case 1:
	default:
		fifo_size = PCH_UART_HAL_FIFO_DIS;
		break;
	}

	switch (priv->trigger) {
	case PCH_UART_HAL_TRIGGER1:
		trigger_level = 1;
		break;
	case PCH_UART_HAL_TRIGGER_L:
		trigger_level = priv->fifo_size / 4;
		break;
	case PCH_UART_HAL_TRIGGER_M:
		trigger_level = priv->fifo_size / 2;
		break;
	case PCH_UART_HAL_TRIGGER_H:
	default:
		trigger_level = priv->fifo_size - (priv->fifo_size / 8);
		break;
	}

	priv->trigger_level = trigger_level;
	ret = pch_uart_hal_set_fifo(priv, PCH_UART_HAL_DMA_MODE0,
				    fifo_size, priv->trigger);
	if (ret < 0)
		return ret;

	ret = request_irq(priv->port.irq, pch_uart_interrupt, IRQF_SHARED,
			priv->irq_name, priv);
	if (ret < 0)
		return ret;

	if (priv->use_dma)
		pch_request_dma(port);

	priv->start_rx = 1;
	pch_uart_hal_enable_interrupt(priv, PCH_UART_HAL_RX_INT |
					    PCH_UART_HAL_RX_ERR_INT);
	uart_update_timeout(port, CS8, default_baud);

	return 0;
}

static void pch_uart_shutdown(struct uart_port *port)
{
	struct eg20t_port *priv;
	int ret;

	priv = container_of(port, struct eg20t_port, port);
	pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_ALL_INT);
	pch_uart_hal_fifo_reset(priv, PCH_UART_HAL_CLR_ALL_FIFO);
	ret = pch_uart_hal_set_fifo(priv, PCH_UART_HAL_DMA_MODE0,
			      PCH_UART_HAL_FIFO_DIS, PCH_UART_HAL_TRIGGER1);
	if (ret)
		dev_err(priv->port.dev,
			"pch_uart_hal_set_fifo Failed(ret=%d)\n", ret);

	pch_free_dma(port);

	free_irq(priv->port.irq, priv);
}

/* Change the port parameters, including word length, parity, stop
 *bits.  Update read_status_mask and ignore_status_mask to indicate
 *the types of events we are interested in receiving.  */
static void pch_uart_set_termios(struct uart_port *port,
				 struct ktermios *termios, struct ktermios *old)
{
	int rtn;
	unsigned int baud, parity, bits, stb;
	struct eg20t_port *priv;
	unsigned long flags;

	priv = container_of(port, struct eg20t_port, port);
	switch (termios->c_cflag & CSIZE) {
	case CS5:
		bits = PCH_UART_HAL_5BIT;
		break;
	case CS6:
		bits = PCH_UART_HAL_6BIT;
		break;
	case CS7:
		bits = PCH_UART_HAL_7BIT;
		break;
	default:		/* CS8 */
		bits = PCH_UART_HAL_8BIT;
		break;
	}
	if (termios->c_cflag & CSTOPB)
		stb = PCH_UART_HAL_STB2;
	else
		stb = PCH_UART_HAL_STB1;

	if (termios->c_cflag & PARENB) {
		if (termios->c_cflag & PARODD)
			parity = PCH_UART_HAL_PARITY_ODD;
		else
			parity = PCH_UART_HAL_PARITY_EVEN;

	} else
		parity = PCH_UART_HAL_PARITY_NONE;

	/* Only UART0 has auto hardware flow function */
	if ((termios->c_cflag & CRTSCTS) && (priv->fifo_size == 256))
		priv->mcr |= UART_MCR_AFE;
	else
		priv->mcr &= ~UART_MCR_AFE;

	termios->c_cflag &= ~CMSPAR; /* Mark/Space parity is not supported */

	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk / 16);

	spin_lock_irqsave(&priv->lock, flags);
	spin_lock(&port->lock);

	uart_update_timeout(port, termios->c_cflag, baud);
	rtn = pch_uart_hal_set_line(priv, baud, parity, bits, stb);
	if (rtn)
		goto out;

	pch_uart_set_mctrl(&priv->port, priv->port.mctrl);
	/* Don't rewrite B0 */
	if (tty_termios_baud_rate(termios))
		tty_termios_encode_baud_rate(termios, baud, baud);

out:
	spin_unlock(&port->lock);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static const char *pch_uart_type(struct uart_port *port)
{
	return KBUILD_MODNAME;
}

static void pch_uart_release_port(struct uart_port *port)
{
	struct eg20t_port *priv;

	priv = container_of(port, struct eg20t_port, port);
	pci_iounmap(priv->pdev, priv->membase);
	pci_release_regions(priv->pdev);
}

static int pch_uart_request_port(struct uart_port *port)
{
	struct eg20t_port *priv;
	int ret;
	void __iomem *membase;

	priv = container_of(port, struct eg20t_port, port);
	ret = pci_request_regions(priv->pdev, KBUILD_MODNAME);
	if (ret < 0)
		return -EBUSY;

	membase = pci_iomap(priv->pdev, 1, 0);
	if (!membase) {
		pci_release_regions(priv->pdev);
		return -EBUSY;
	}
	priv->membase = port->membase = membase;

	return 0;
}

static void pch_uart_config_port(struct uart_port *port, int type)
{
	struct eg20t_port *priv;

	priv = container_of(port, struct eg20t_port, port);
	if (type & UART_CONFIG_TYPE) {
		port->type = priv->port_type;
		pch_uart_request_port(port);
	}
}

static int pch_uart_verify_port(struct uart_port *port,
				struct serial_struct *serinfo)
{
	struct eg20t_port *priv;

	priv = container_of(port, struct eg20t_port, port);
	if (serinfo->flags & UPF_LOW_LATENCY) {
		dev_info(priv->port.dev,
			"PCH UART : Use PIO Mode (without DMA)\n");
		priv->use_dma = 0;
		serinfo->flags &= ~UPF_LOW_LATENCY;
	} else {
#ifndef CONFIG_PCH_DMA
		dev_err(priv->port.dev, "%s : PCH DMA is not Loaded.\n",
			__func__);
		return -EOPNOTSUPP;
#endif
		if (!priv->use_dma) {
			pch_request_dma(port);
			if (priv->chan_rx)
				priv->use_dma = 1;
		}
		dev_info(priv->port.dev, "PCH UART: %s\n",
				priv->use_dma ?
				"Use DMA Mode" : "No DMA");
	}

	return 0;
}

#if defined(CONFIG_CONSOLE_POLL) || defined(CONFIG_SERIAL_PCH_UART_CONSOLE)
/*
 *	Wait for transmitter & holding register to empty
 */
static void wait_for_xmitr(struct eg20t_port *up, int bits)
{
	unsigned int status, tmout = 10000;

	/* Wait up to 10ms for the character(s) to be sent. */
	for (;;) {
		status = ioread8(up->membase + UART_LSR);

		if ((status & bits) == bits)
			break;
		if (--tmout == 0)
			break;
		udelay(1);
	}

	/* Wait up to 1s for flow control if necessary */
	if (up->port.flags & UPF_CONS_FLOW) {
		unsigned int tmout;
		for (tmout = 1000000; tmout; tmout--) {
			unsigned int msr = ioread8(up->membase + UART_MSR);
			if (msr & UART_MSR_CTS)
				break;
			udelay(1);
			touch_nmi_watchdog();
		}
	}
}
#endif /* CONFIG_CONSOLE_POLL || CONFIG_SERIAL_PCH_UART_CONSOLE */

#ifdef CONFIG_CONSOLE_POLL
/*
 * Console polling routines for communicate via uart while
 * in an interrupt or debug context.
 */
static int pch_uart_get_poll_char(struct uart_port *port)
{
	struct eg20t_port *priv =
		container_of(port, struct eg20t_port, port);
	u8 lsr = ioread8(priv->membase + UART_LSR);

	if (!(lsr & UART_LSR_DR))
		return NO_POLL_CHAR;

	return ioread8(priv->membase + PCH_UART_RBR);
}


static void pch_uart_put_poll_char(struct uart_port *port,
			 unsigned char c)
{
	unsigned int ier;
	struct eg20t_port *priv =
		container_of(port, struct eg20t_port, port);

	/*
	 * First save the IER then disable the interrupts
	 */
	ier = ioread8(priv->membase + UART_IER);
	pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_ALL_INT);

	wait_for_xmitr(priv, UART_LSR_THRE);
	/*
	 * Send the character out.
	 */
	iowrite8(c, priv->membase + PCH_UART_THR);

	/*
	 * Finally, wait for transmitter to become empty
	 * and restore the IER
	 */
	wait_for_xmitr(priv, BOTH_EMPTY);
	iowrite8(ier, priv->membase + UART_IER);
}
#endif /* CONFIG_CONSOLE_POLL */

static const struct uart_ops pch_uart_ops = {
	.tx_empty = pch_uart_tx_empty,
	.set_mctrl = pch_uart_set_mctrl,
	.get_mctrl = pch_uart_get_mctrl,
	.stop_tx = pch_uart_stop_tx,
	.start_tx = pch_uart_start_tx,
	.stop_rx = pch_uart_stop_rx,
	.enable_ms = pch_uart_enable_ms,
	.break_ctl = pch_uart_break_ctl,
	.startup = pch_uart_startup,
	.shutdown = pch_uart_shutdown,
	.set_termios = pch_uart_set_termios,
/*	.pm		= pch_uart_pm,		Not supported yet */
	.type = pch_uart_type,
	.release_port = pch_uart_release_port,
	.request_port = pch_uart_request_port,
	.config_port = pch_uart_config_port,
	.verify_port = pch_uart_verify_port,
#ifdef CONFIG_CONSOLE_POLL
	.poll_get_char = pch_uart_get_poll_char,
	.poll_put_char = pch_uart_put_poll_char,
#endif
};

#ifdef CONFIG_SERIAL_PCH_UART_CONSOLE

static void pch_console_putchar(struct uart_port *port, int ch)
{
	struct eg20t_port *priv =
		container_of(port, struct eg20t_port, port);

	wait_for_xmitr(priv, UART_LSR_THRE);
	iowrite8(ch, priv->membase + PCH_UART_THR);
}

/*
 *	Print a string to the serial port trying not to disturb
 *	any possible real use of the port...
 *
 *	The console_lock must be held when we get here.
 */
static void
pch_console_write(struct console *co, const char *s, unsigned int count)
{
	struct eg20t_port *priv;
	unsigned long flags;
	int priv_locked = 1;
	int port_locked = 1;
	u8 ier;

	priv = pch_uart_ports[co->index];

	touch_nmi_watchdog();

	local_irq_save(flags);
	if (priv->port.sysrq) {
		/* call to uart_handle_sysrq_char already took the priv lock */
		priv_locked = 0;
		/* serial8250_handle_port() already took the port lock */
		port_locked = 0;
	} else if (oops_in_progress) {
		priv_locked = spin_trylock(&priv->lock);
		port_locked = spin_trylock(&priv->port.lock);
	} else {
		spin_lock(&priv->lock);
		spin_lock(&priv->port.lock);
	}

	/*
	 *	First save the IER then disable the interrupts
	 */
	ier = ioread8(priv->membase + UART_IER);

	pch_uart_hal_disable_interrupt(priv, PCH_UART_HAL_ALL_INT);

	uart_console_write(&priv->port, s, count, pch_console_putchar);

	/*
	 *	Finally, wait for transmitter to become empty
	 *	and restore the IER
	 */
	wait_for_xmitr(priv, BOTH_EMPTY);
	iowrite8(ier, priv->membase + UART_IER);

	if (port_locked)
		spin_unlock(&priv->port.lock);
	if (priv_locked)
		spin_unlock(&priv->lock);
	local_irq_restore(flags);
}

static int __init pch_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int baud = default_baud;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index >= PCH_UART_NR)
		co->index = 0;
	port = &pch_uart_ports[co->index]->port;

	if (!port || (!port->iobase && !port->membase))
		return -ENODEV;

	port->uartclk = pch_uart_get_uartclk();

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver pch_uart_driver;

static struct console pch_console = {
	.name		= PCH_UART_DRIVER_DEVICE,
	.write		= pch_console_write,
	.device		= uart_console_device,
	.setup		= pch_console_setup,
	.flags		= CON_PRINTBUFFER | CON_ANYTIME,
	.index		= -1,
	.data		= &pch_uart_driver,
};

#define PCH_CONSOLE	(&pch_console)
#else
#define PCH_CONSOLE	NULL
#endif	/* CONFIG_SERIAL_PCH_UART_CONSOLE */

static struct uart_driver pch_uart_driver = {
	.owner = THIS_MODULE,
	.driver_name = KBUILD_MODNAME,
	.dev_name = PCH_UART_DRIVER_DEVICE,
	.major = 0,
	.minor = 0,
	.nr = PCH_UART_NR,
	.cons = PCH_CONSOLE,
};

static struct eg20t_port *pch_uart_init_port(struct pci_dev *pdev,
					     const struct pci_device_id *id)
{
	struct eg20t_port *priv;
	int ret;
	unsigned int iobase;
	unsigned int mapbase;
	unsigned char *rxbuf;
	int fifosize;
	int port_type;
	struct pch_uart_driver_data *board;
	char name[32];

	board = &drv_dat[id->driver_data];
	port_type = board->port_type;

	priv = kzalloc(sizeof(struct eg20t_port), GFP_KERNEL);
	if (priv == NULL)
		goto init_port_alloc_err;

	rxbuf = (unsigned char *)__get_free_page(GFP_KERNEL);
	if (!rxbuf)
		goto init_port_free_txbuf;

	switch (port_type) {
	case PORT_PCH_8LINE:
		fifosize = 256; /* EG20T/ML7213: UART0 */
		break;
	case PORT_PCH_2LINE:
		fifosize = 64; /* EG20T:UART1~3  ML7213: UART1~2*/
		break;
	default:
		dev_err(&pdev->dev, "Invalid Port Type(=%d)\n", port_type);
		goto init_port_hal_free;
	}

	pci_enable_msi(pdev);
	pci_set_master(pdev);

	spin_lock_init(&priv->lock);

	iobase = pci_resource_start(pdev, 0);
	mapbase = pci_resource_start(pdev, 1);
	priv->mapbase = mapbase;
	priv->iobase = iobase;
	priv->pdev = pdev;
	priv->tx_empty = 1;
	priv->rxbuf.buf = rxbuf;
	priv->rxbuf.size = PAGE_SIZE;

	priv->fifo_size = fifosize;
	priv->uartclk = pch_uart_get_uartclk();
	priv->port_type = port_type;
	priv->port.dev = &pdev->dev;
	priv->port.iobase = iobase;
	priv->port.membase = NULL;
	priv->port.mapbase = mapbase;
	priv->port.irq = pdev->irq;
	priv->port.iotype = UPIO_PORT;
	priv->port.ops = &pch_uart_ops;
	priv->port.flags = UPF_BOOT_AUTOCONF;
	priv->port.fifosize = fifosize;
	priv->port.line = board->line_no;
	priv->port.has_sysrq = IS_ENABLED(CONFIG_SERIAL_PCH_UART_CONSOLE);
	priv->trigger = PCH_UART_HAL_TRIGGER_M;

	snprintf(priv->irq_name, IRQ_NAME_SIZE,
		 KBUILD_MODNAME ":" PCH_UART_DRIVER_DEVICE "%d",
		 priv->port.line);

	spin_lock_init(&priv->port.lock);

	pci_set_drvdata(pdev, priv);
	priv->trigger_level = 1;
	priv->fcr = 0;

	if (pdev->dev.of_node)
		of_property_read_u32(pdev->dev.of_node, "clock-frequency"
					 , &user_uartclk);

#ifdef CONFIG_SERIAL_PCH_UART_CONSOLE
	pch_uart_ports[board->line_no] = priv;
#endif
	ret = uart_add_one_port(&pch_uart_driver, &priv->port);
	if (ret < 0)
		goto init_port_hal_free;

	snprintf(name, sizeof(name), "uart%d_regs", priv->port.line);
	debugfs_create_file(name, S_IFREG | S_IRUGO, NULL, priv,
			    &port_regs_ops);

	return priv;

init_port_hal_free:
#ifdef CONFIG_SERIAL_PCH_UART_CONSOLE
	pch_uart_ports[board->line_no] = NULL;
#endif
	free_page((unsigned long)rxbuf);
init_port_free_txbuf:
	kfree(priv);
init_port_alloc_err:

	return NULL;
}

static void pch_uart_exit_port(struct eg20t_port *priv)
{
	char name[32];

	snprintf(name, sizeof(name), "uart%d_regs", priv->port.line);
	debugfs_remove(debugfs_lookup(name, NULL));
	uart_remove_one_port(&pch_uart_driver, &priv->port);
	free_page((unsigned long)priv->rxbuf.buf);
}

static void pch_uart_pci_remove(struct pci_dev *pdev)
{
	struct eg20t_port *priv = pci_get_drvdata(pdev);

	pci_disable_msi(pdev);

#ifdef CONFIG_SERIAL_PCH_UART_CONSOLE
	pch_uart_ports[priv->port.line] = NULL;
#endif
	pch_uart_exit_port(priv);
	pci_disable_device(pdev);
	kfree(priv);
	return;
}

static int __maybe_unused pch_uart_pci_suspend(struct device *dev)
{
	struct eg20t_port *priv = dev_get_drvdata(dev);

	uart_suspend_port(&pch_uart_driver, &priv->port);

	return 0;
}

static int __maybe_unused pch_uart_pci_resume(struct device *dev)
{
	struct eg20t_port *priv = dev_get_drvdata(dev);

	uart_resume_port(&pch_uart_driver, &priv->port);

	return 0;
}

static const struct pci_device_id pch_uart_pci_id[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8811),
	 .driver_data = pch_et20t_uart0},
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8812),
	 .driver_data = pch_et20t_uart1},
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8813),
	 .driver_data = pch_et20t_uart2},
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8814),
	 .driver_data = pch_et20t_uart3},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x8027),
	 .driver_data = pch_ml7213_uart0},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x8028),
	 .driver_data = pch_ml7213_uart1},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x8029),
	 .driver_data = pch_ml7213_uart2},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x800C),
	 .driver_data = pch_ml7223_uart0},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x800D),
	 .driver_data = pch_ml7223_uart1},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x8811),
	 .driver_data = pch_ml7831_uart0},
	{PCI_DEVICE(PCI_VENDOR_ID_ROHM, 0x8812),
	 .driver_data = pch_ml7831_uart1},
	{0,},
};

static int pch_uart_pci_probe(struct pci_dev *pdev,
					const struct pci_device_id *id)
{
	int ret;
	struct eg20t_port *priv;

	ret = pci_enable_device(pdev);
	if (ret < 0)
		goto probe_error;

	priv = pch_uart_init_port(pdev, id);
	if (!priv) {
		ret = -EBUSY;
		goto probe_disable_device;
	}
	pci_set_drvdata(pdev, priv);

	return ret;

probe_disable_device:
	pci_disable_msi(pdev);
	pci_disable_device(pdev);
probe_error:
	return ret;
}

static SIMPLE_DEV_PM_OPS(pch_uart_pci_pm_ops,
			 pch_uart_pci_suspend,
			 pch_uart_pci_resume);

static struct pci_driver pch_uart_pci_driver = {
	.name = "pch_uart",
	.id_table = pch_uart_pci_id,
	.probe = pch_uart_pci_probe,
	.remove = pch_uart_pci_remove,
	.driver.pm = &pch_uart_pci_pm_ops,
};

static int __init pch_uart_module_init(void)
{
	int ret;

	/* register as UART driver */
	ret = uart_register_driver(&pch_uart_driver);
	if (ret < 0)
		return ret;

	/* register as PCI driver */
	ret = pci_register_driver(&pch_uart_pci_driver);
	if (ret < 0)
		uart_unregister_driver(&pch_uart_driver);

	return ret;
}
module_init(pch_uart_module_init);

static void __exit pch_uart_module_exit(void)
{
	pci_unregister_driver(&pch_uart_pci_driver);
	uart_unregister_driver(&pch_uart_driver);
}
module_exit(pch_uart_module_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Intel EG20T PCH UART PCI Driver");
MODULE_DEVICE_TABLE(pci, pch_uart_pci_id);

module_param(default_baud, uint, S_IRUGO);
MODULE_PARM_DESC(default_baud,
                 "Default BAUD for initial driver state and console (default 9600)");
module_param(user_uartclk, uint, S_IRUGO);
MODULE_PARM_DESC(user_uartclk,
                 "Override UART default or board specific UART clock");

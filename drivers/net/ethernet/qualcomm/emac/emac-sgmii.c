// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 */

/* Qualcomm Technologies, Inc. EMAC SGMII Controller driver.
 */

#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/acpi.h>
#include <linux/of_device.h>
#include "emac.h"
#include "emac-mac.h"
#include "emac-sgmii.h"

/* EMAC_SGMII register offsets */
#define EMAC_SGMII_PHY_AUTONEG_CFG2		0x0048
#define EMAC_SGMII_PHY_SPEED_CFG1		0x0074
#define EMAC_SGMII_PHY_IRQ_CMD			0x00ac
#define EMAC_SGMII_PHY_INTERRUPT_CLEAR		0x00b0
#define EMAC_SGMII_PHY_INTERRUPT_MASK		0x00b4
#define EMAC_SGMII_PHY_INTERRUPT_STATUS		0x00b8
#define EMAC_SGMII_PHY_RX_CHK_STATUS		0x00d4

#define FORCE_AN_TX_CFG				BIT(5)
#define FORCE_AN_RX_CFG				BIT(4)
#define AN_ENABLE				BIT(0)

#define DUPLEX_MODE				BIT(4)
#define SPDMODE_1000				BIT(1)
#define SPDMODE_100				BIT(0)
#define SPDMODE_10				0

#define CDR_ALIGN_DET				BIT(6)

#define IRQ_GLOBAL_CLEAR			BIT(0)

#define DECODE_CODE_ERR				BIT(7)
#define DECODE_DISP_ERR				BIT(6)

#define SGMII_PHY_IRQ_CLR_WAIT_TIME		10

#define SGMII_PHY_INTERRUPT_ERR		(DECODE_CODE_ERR | DECODE_DISP_ERR)
#define SGMII_ISR_MASK  		(SGMII_PHY_INTERRUPT_ERR)

#define SERDES_START_WAIT_TIMES			100

int emac_sgmii_init(struct emac_adapter *adpt)
{
	if (!(adpt->phy.sgmii_ops && adpt->phy.sgmii_ops->init))
		return 0;

	return adpt->phy.sgmii_ops->init(adpt);
}

int emac_sgmii_open(struct emac_adapter *adpt)
{
	if (!(adpt->phy.sgmii_ops && adpt->phy.sgmii_ops->open))
		return 0;

	return adpt->phy.sgmii_ops->open(adpt);
}

void emac_sgmii_close(struct emac_adapter *adpt)
{
	if (!(adpt->phy.sgmii_ops && adpt->phy.sgmii_ops->close))
		return;

	adpt->phy.sgmii_ops->close(adpt);
}

int emac_sgmii_link_change(struct emac_adapter *adpt, bool link_state)
{
	if (!(adpt->phy.sgmii_ops && adpt->phy.sgmii_ops->link_change))
		return 0;

	return adpt->phy.sgmii_ops->link_change(adpt, link_state);
}

void emac_sgmii_reset(struct emac_adapter *adpt)
{
	if (!(adpt->phy.sgmii_ops && adpt->phy.sgmii_ops->reset))
		return;

	adpt->phy.sgmii_ops->reset(adpt);
}

/* Initialize the SGMII link between the internal and external PHYs. */
static void emac_sgmii_link_init(struct emac_adapter *adpt)
{
	struct emac_sgmii *phy = &adpt->phy;
	u32 val;

	/* Always use autonegotiation. It works no matter how the external
	 * PHY is configured.
	 */
	val = readl(phy->base + EMAC_SGMII_PHY_AUTONEG_CFG2);
	val &= ~(FORCE_AN_RX_CFG | FORCE_AN_TX_CFG);
	val |= AN_ENABLE;
	writel(val, phy->base + EMAC_SGMII_PHY_AUTONEG_CFG2);
}

static int emac_sgmii_irq_clear(struct emac_adapter *adpt, u8 irq_bits)
{
	struct emac_sgmii *phy = &adpt->phy;
	u8 status;

	writel_relaxed(irq_bits, phy->base + EMAC_SGMII_PHY_INTERRUPT_CLEAR);
	writel_relaxed(IRQ_GLOBAL_CLEAR, phy->base + EMAC_SGMII_PHY_IRQ_CMD);
	/* Ensure interrupt clear command is written to HW */
	wmb();

	/* After set the IRQ_GLOBAL_CLEAR bit, the status clearing must
	 * be confirmed before clearing the bits in other registers.
	 * It takes a few cycles for hw to clear the interrupt status.
	 */
	if (readl_poll_timeout_atomic(phy->base +
				      EMAC_SGMII_PHY_INTERRUPT_STATUS,
				      status, !(status & irq_bits), 1,
				      SGMII_PHY_IRQ_CLR_WAIT_TIME)) {
		net_err_ratelimited("%s: failed to clear SGMII irq: status:0x%x bits:0x%x\n",
				    adpt->netdev->name, status, irq_bits);
		return -EIO;
	}

	/* Finalize clearing procedure */
	writel_relaxed(0, phy->base + EMAC_SGMII_PHY_IRQ_CMD);
	writel_relaxed(0, phy->base + EMAC_SGMII_PHY_INTERRUPT_CLEAR);

	/* Ensure that clearing procedure finalization is written to HW */
	wmb();

	return 0;
}

/* The number of decode errors that triggers a reset */
#define DECODE_ERROR_LIMIT	2

static irqreturn_t emac_sgmii_interrupt(int irq, void *data)
{
	struct emac_adapter *adpt = data;
	struct emac_sgmii *phy = &adpt->phy;
	u8 status;

	status = readl(phy->base + EMAC_SGMII_PHY_INTERRUPT_STATUS);
	status &= SGMII_ISR_MASK;
	if (!status)
		return IRQ_HANDLED;

	/* If we get a decoding error and CDR is not locked, then try
	 * resetting the internal PHY.  The internal PHY uses an embedded
	 * clock with Clock and Data Recovery (CDR) to recover the
	 * clock and data.
	 */
	if (status & SGMII_PHY_INTERRUPT_ERR) {
		int count;

		/* The SGMII is capable of recovering from some decode
		 * errors automatically.  However, if we get multiple
		 * decode errors in a row, then assume that something
		 * is wrong and reset the interface.
		 */
		count = atomic_inc_return(&phy->decode_error_count);
		if (count == DECODE_ERROR_LIMIT) {
			schedule_work(&adpt->work_thread);
			atomic_set(&phy->decode_error_count, 0);
		}
	} else {
		/* We only care about consecutive decode errors. */
		atomic_set(&phy->decode_error_count, 0);
	}

	if (emac_sgmii_irq_clear(adpt, status))
		schedule_work(&adpt->work_thread);

	return IRQ_HANDLED;
}

static void emac_sgmii_reset_prepare(struct emac_adapter *adpt)
{
	struct emac_sgmii *phy = &adpt->phy;
	u32 val;

	/* Reset PHY */
	val = readl(phy->base + EMAC_EMAC_WRAPPER_CSR2);
	writel(((val & ~PHY_RESET) | PHY_RESET), phy->base +
	       EMAC_EMAC_WRAPPER_CSR2);
	/* Ensure phy-reset command is written to HW before the release cmd */
	msleep(50);
	val = readl(phy->base + EMAC_EMAC_WRAPPER_CSR2);
	writel((val & ~PHY_RESET), phy->base + EMAC_EMAC_WRAPPER_CSR2);
	/* Ensure phy-reset release command is written to HW before initializing
	 * SGMII
	 */
	msleep(50);
}

static void emac_sgmii_common_reset(struct emac_adapter *adpt)
{
	int ret;

	emac_sgmii_reset_prepare(adpt);
	emac_sgmii_link_init(adpt);

	ret = emac_sgmii_init(adpt);
	if (ret)
		netdev_err(adpt->netdev,
			   "could not reinitialize internal PHY (error=%i)\n",
			   ret);
}

static int emac_sgmii_common_open(struct emac_adapter *adpt)
{
	struct emac_sgmii *sgmii = &adpt->phy;
	int ret;

	if (sgmii->irq) {
		/* Make sure interrupts are cleared and disabled first */
		ret = emac_sgmii_irq_clear(adpt, 0xff);
		if (ret)
			return ret;
		writel(0, sgmii->base + EMAC_SGMII_PHY_INTERRUPT_MASK);

		ret = request_irq(sgmii->irq, emac_sgmii_interrupt, 0,
				  "emac-sgmii", adpt);
		if (ret) {
			netdev_err(adpt->netdev,
				   "could not register handler for internal PHY\n");
			return ret;
		}
	}

	return 0;
}

static void emac_sgmii_common_close(struct emac_adapter *adpt)
{
	struct emac_sgmii *sgmii = &adpt->phy;

	/* Make sure interrupts are disabled */
	writel(0, sgmii->base + EMAC_SGMII_PHY_INTERRUPT_MASK);
	free_irq(sgmii->irq, adpt);
}

/* The error interrupts are only valid after the link is up */
static int emac_sgmii_common_link_change(struct emac_adapter *adpt, bool linkup)
{
	struct emac_sgmii *sgmii = &adpt->phy;
	int ret;

	if (linkup) {
		/* Clear and enable interrupts */
		ret = emac_sgmii_irq_clear(adpt, 0xff);
		if (ret)
			return ret;

		writel(SGMII_ISR_MASK,
		       sgmii->base + EMAC_SGMII_PHY_INTERRUPT_MASK);
	} else {
		/* Disable interrupts */
		writel(0, sgmii->base + EMAC_SGMII_PHY_INTERRUPT_MASK);
		synchronize_irq(sgmii->irq);
	}

	return 0;
}

static struct sgmii_ops fsm9900_ops = {
	.init = emac_sgmii_init_fsm9900,
	.open = emac_sgmii_common_open,
	.close = emac_sgmii_common_close,
	.link_change = emac_sgmii_common_link_change,
	.reset = emac_sgmii_common_reset,
};

static struct sgmii_ops qdf2432_ops = {
	.init = emac_sgmii_init_qdf2432,
	.open = emac_sgmii_common_open,
	.close = emac_sgmii_common_close,
	.link_change = emac_sgmii_common_link_change,
	.reset = emac_sgmii_common_reset,
};

#ifdef CONFIG_ACPI
static struct sgmii_ops qdf2400_ops = {
	.init = emac_sgmii_init_qdf2400,
	.open = emac_sgmii_common_open,
	.close = emac_sgmii_common_close,
	.link_change = emac_sgmii_common_link_change,
	.reset = emac_sgmii_common_reset,
};
#endif

static int emac_sgmii_acpi_match(struct device *dev, void *data)
{
#ifdef CONFIG_ACPI
	static const struct acpi_device_id match_table[] = {
		{
			.id = "QCOM8071",
		},
		{}
	};
	const struct acpi_device_id *id = acpi_match_device(match_table, dev);
	struct sgmii_ops **ops = data;

	if (id) {
		acpi_handle handle = ACPI_HANDLE(dev);
		unsigned long long hrv;
		acpi_status status;

		status = acpi_evaluate_integer(handle, "_HRV", NULL, &hrv);
		if (status) {
			if (status == AE_NOT_FOUND)
				/* Older versions of the QDF2432 ACPI tables do
				 * not have an _HRV property.
				 */
				hrv = 1;
			else
				/* Something is wrong with the tables */
				return 0;
		}

		switch (hrv) {
		case 1:
			*ops = &qdf2432_ops;
			return 1;
		case 2:
			*ops = &qdf2400_ops;
			return 1;
		}
	}
#endif

	return 0;
}

static const struct of_device_id emac_sgmii_dt_match[] = {
	{
		.compatible = "qcom,fsm9900-emac-sgmii",
		.data = &fsm9900_ops,
	},
	{
		.compatible = "qcom,qdf2432-emac-sgmii",
		.data = &qdf2432_ops,
	},
	{}
};

int emac_sgmii_config(struct platform_device *pdev, struct emac_adapter *adpt)
{
	struct platform_device *sgmii_pdev = NULL;
	struct emac_sgmii *phy = &adpt->phy;
	struct resource *res;
	int ret;

	if (has_acpi_companion(&pdev->dev)) {
		struct device *dev;

		dev = device_find_child(&pdev->dev, &phy->sgmii_ops,
					emac_sgmii_acpi_match);

		if (!dev) {
			dev_warn(&pdev->dev, "cannot find internal phy node\n");
			return 0;
		}

		sgmii_pdev = to_platform_device(dev);
	} else {
		const struct of_device_id *match;
		struct device_node *np;

		np = of_parse_phandle(pdev->dev.of_node, "internal-phy", 0);
		if (!np) {
			dev_err(&pdev->dev, "missing internal-phy property\n");
			return -ENODEV;
		}

		sgmii_pdev = of_find_device_by_node(np);
		of_node_put(np);
		if (!sgmii_pdev) {
			dev_err(&pdev->dev, "invalid internal-phy property\n");
			return -ENODEV;
		}

		match = of_match_device(emac_sgmii_dt_match, &sgmii_pdev->dev);
		if (!match) {
			dev_err(&pdev->dev, "unrecognized internal phy node\n");
			ret = -ENODEV;
			goto error_put_device;
		}

		phy->sgmii_ops = (struct sgmii_ops *)match->data;
	}

	/* Base address is the first address */
	res = platform_get_resource(sgmii_pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ret = -EINVAL;
		goto error_put_device;
	}

	phy->base = ioremap(res->start, resource_size(res));
	if (!phy->base) {
		ret = -ENOMEM;
		goto error_put_device;
	}

	/* v2 SGMII has a per-lane digital digital, so parse it if it exists */
	res = platform_get_resource(sgmii_pdev, IORESOURCE_MEM, 1);
	if (res) {
		phy->digital = ioremap(res->start, resource_size(res));
		if (!phy->digital) {
			ret = -ENOMEM;
			goto error_unmap_base;
		}
	}

	ret = emac_sgmii_init(adpt);
	if (ret)
		goto error;

	emac_sgmii_link_init(adpt);

	ret = platform_get_irq(sgmii_pdev, 0);
	if (ret > 0)
		phy->irq = ret;

	/* We've remapped the addresses, so we don't need the device any
	 * more.  of_find_device_by_node() says we should release it.
	 */
	put_device(&sgmii_pdev->dev);

	return 0;

error:
	if (phy->digital)
		iounmap(phy->digital);
error_unmap_base:
	iounmap(phy->base);
error_put_device:
	put_device(&sgmii_pdev->dev);

	return ret;
}

// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2005, 2006 IBM Corporation
 * Copyright (C) 2014, 2015 Intel Corporation
 *
 * Authors:
 * Leendert van Doorn <leendert@watson.ibm.com>
 * Kylene Hall <kjhall@us.ibm.com>
 *
 * Maintained by: <tpmdd-devel@lists.sourceforge.net>
 *
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * This device driver implements the TPM interface as defined in
 * the TCG TPM Interface Spec version 1.2, revision 1.0.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/pnp.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/acpi.h>
#include <linux/freezer.h>
#include "tpm.h"
#include "tpm_tis_core.h"

static void tpm_tis_clkrun_enable(struct tpm_chip *chip, bool value);

static bool wait_for_tpm_stat_cond(struct tpm_chip *chip, u8 mask,
					bool check_cancel, bool *canceled)
{
	u8 status = chip->ops->status(chip);

	*canceled = false;
	if ((status & mask) == mask)
		return true;
	if (check_cancel && chip->ops->req_canceled(chip, status)) {
		*canceled = true;
		return true;
	}
	return false;
}

static int wait_for_tpm_stat(struct tpm_chip *chip, u8 mask,
		unsigned long timeout, wait_queue_head_t *queue,
		bool check_cancel)
{
	unsigned long stop;
	long rc;
	u8 status;
	bool canceled = false;

	/* check current status */
	status = chip->ops->status(chip);
	if ((status & mask) == mask)
		return 0;

	stop = jiffies + timeout;

	if (chip->flags & TPM_CHIP_FLAG_IRQ) {
again:
		timeout = stop - jiffies;
		if ((long)timeout <= 0)
			return -ETIME;
		rc = wait_event_interruptible_timeout(*queue,
			wait_for_tpm_stat_cond(chip, mask, check_cancel,
					       &canceled),
			timeout);
		if (rc > 0) {
			if (canceled)
				return -ECANCELED;
			return 0;
		}
		if (rc == -ERESTARTSYS && freezing(current)) {
			clear_thread_flag(TIF_SIGPENDING);
			goto again;
		}
	} else {
		do {
			usleep_range(TPM_TIMEOUT_USECS_MIN,
				     TPM_TIMEOUT_USECS_MAX);
			status = chip->ops->status(chip);
			if ((status & mask) == mask)
				return 0;
		} while (time_before(jiffies, stop));
	}
	return -ETIME;
}

/* Before we attempt to access the TPM we must see that the valid bit is set.
 * The specification says that this bit is 0 at reset and remains 0 until the
 * 'TPM has gone through its self test and initialization and has established
 * correct values in the other bits.'
 */
static int wait_startup(struct tpm_chip *chip, int l)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	unsigned long stop = jiffies + chip->timeout_a;

	do {
		int rc;
		u8 access;

		rc = tpm_tis_read8(priv, TPM_ACCESS(l), &access);
		if (rc < 0)
			return rc;

		if (access & TPM_ACCESS_VALID)
			return 0;
		tpm_msleep(TPM_TIMEOUT);
	} while (time_before(jiffies, stop));
	return -1;
}

static bool check_locality(struct tpm_chip *chip, int l)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int rc;
	u8 access;

	rc = tpm_tis_read8(priv, TPM_ACCESS(l), &access);
	if (rc < 0)
		return false;

	if ((access & (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID
		       | TPM_ACCESS_REQUEST_USE)) ==
	    (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) {
		priv->locality = l;
		return true;
	}

	return false;
}

static int release_locality(struct tpm_chip *chip, int l)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);

	tpm_tis_write8(priv, TPM_ACCESS(l), TPM_ACCESS_ACTIVE_LOCALITY);

	return 0;
}

static int request_locality(struct tpm_chip *chip, int l)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	unsigned long stop, timeout;
	long rc;

	if (check_locality(chip, l))
		return l;

	rc = tpm_tis_write8(priv, TPM_ACCESS(l), TPM_ACCESS_REQUEST_USE);
	if (rc < 0)
		return rc;

	stop = jiffies + chip->timeout_a;

	if (chip->flags & TPM_CHIP_FLAG_IRQ) {
again:
		timeout = stop - jiffies;
		if ((long)timeout <= 0)
			return -1;
		rc = wait_event_interruptible_timeout(priv->int_queue,
						      (check_locality
						       (chip, l)),
						      timeout);
		if (rc > 0)
			return l;
		if (rc == -ERESTARTSYS && freezing(current)) {
			clear_thread_flag(TIF_SIGPENDING);
			goto again;
		}
	} else {
		/* wait for burstcount */
		do {
			if (check_locality(chip, l))
				return l;
			tpm_msleep(TPM_TIMEOUT);
		} while (time_before(jiffies, stop));
	}
	return -1;
}

static u8 tpm_tis_status(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int rc;
	u8 status;

	rc = tpm_tis_read8(priv, TPM_STS(priv->locality), &status);
	if (rc < 0)
		return 0;

	if (unlikely((status & TPM_STS_READ_ZERO) != 0)) {
		/*
		 * If this trips, the chances are the read is
		 * returning 0xff because the locality hasn't been
		 * acquired.  Usually because tpm_try_get_ops() hasn't
		 * been called before doing a TPM operation.
		 */
		WARN_ONCE(1, "TPM returned invalid status\n");
		return 0;
	}

	return status;
}

static void tpm_tis_ready(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);

	/* this causes the current command to be aborted */
	tpm_tis_write8(priv, TPM_STS(priv->locality), TPM_STS_COMMAND_READY);
}

static int get_burstcount(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	unsigned long stop;
	int burstcnt, rc;
	u32 value;

	/* wait for burstcount */
	if (chip->flags & TPM_CHIP_FLAG_TPM2)
		stop = jiffies + chip->timeout_a;
	else
		stop = jiffies + chip->timeout_d;
	do {
		rc = tpm_tis_read32(priv, TPM_STS(priv->locality), &value);
		if (rc < 0)
			return rc;

		burstcnt = (value >> 8) & 0xFFFF;
		if (burstcnt)
			return burstcnt;
		usleep_range(TPM_TIMEOUT_USECS_MIN, TPM_TIMEOUT_USECS_MAX);
	} while (time_before(jiffies, stop));
	return -EBUSY;
}

static int recv_data(struct tpm_chip *chip, u8 *buf, size_t count)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int size = 0, burstcnt, rc;

	while (size < count) {
		rc = wait_for_tpm_stat(chip,
				 TPM_STS_DATA_AVAIL | TPM_STS_VALID,
				 chip->timeout_c,
				 &priv->read_queue, true);
		if (rc < 0)
			return rc;
		burstcnt = get_burstcount(chip);
		if (burstcnt < 0) {
			dev_err(&chip->dev, "Unable to read burstcount\n");
			return burstcnt;
		}
		burstcnt = min_t(int, burstcnt, count - size);

		rc = tpm_tis_read_bytes(priv, TPM_DATA_FIFO(priv->locality),
					burstcnt, buf + size);
		if (rc < 0)
			return rc;

		size += burstcnt;
	}
	return size;
}

static int tpm_tis_recv(struct tpm_chip *chip, u8 *buf, size_t count)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int size = 0;
	int status;
	u32 expected;

	if (count < TPM_HEADER_SIZE) {
		size = -EIO;
		goto out;
	}

	size = recv_data(chip, buf, TPM_HEADER_SIZE);
	/* read first 10 bytes, including tag, paramsize, and result */
	if (size < TPM_HEADER_SIZE) {
		dev_err(&chip->dev, "Unable to read header\n");
		goto out;
	}

	expected = be32_to_cpu(*(__be32 *) (buf + 2));
	if (expected > count || expected < TPM_HEADER_SIZE) {
		size = -EIO;
		goto out;
	}

	size += recv_data(chip, &buf[TPM_HEADER_SIZE],
			  expected - TPM_HEADER_SIZE);
	if (size < expected) {
		dev_err(&chip->dev, "Unable to read remainder of result\n");
		size = -ETIME;
		goto out;
	}

	if (wait_for_tpm_stat(chip, TPM_STS_VALID, chip->timeout_c,
				&priv->int_queue, false) < 0) {
		size = -ETIME;
		goto out;
	}
	status = tpm_tis_status(chip);
	if (status & TPM_STS_DATA_AVAIL) {	/* retry? */
		dev_err(&chip->dev, "Error left over data\n");
		size = -EIO;
		goto out;
	}

out:
	tpm_tis_ready(chip);
	return size;
}

/*
 * If interrupts are used (signaled by an irq set in the vendor structure)
 * tpm.c can skip polling for the data to be available as the interrupt is
 * waited for here
 */
static int tpm_tis_send_data(struct tpm_chip *chip, const u8 *buf, size_t len)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int rc, status, burstcnt;
	size_t count = 0;
	bool itpm = priv->flags & TPM_TIS_ITPM_WORKAROUND;

	status = tpm_tis_status(chip);
	if ((status & TPM_STS_COMMAND_READY) == 0) {
		tpm_tis_ready(chip);
		if (wait_for_tpm_stat
		    (chip, TPM_STS_COMMAND_READY, chip->timeout_b,
		     &priv->int_queue, false) < 0) {
			rc = -ETIME;
			goto out_err;
		}
	}

	while (count < len - 1) {
		burstcnt = get_burstcount(chip);
		if (burstcnt < 0) {
			dev_err(&chip->dev, "Unable to read burstcount\n");
			rc = burstcnt;
			goto out_err;
		}
		burstcnt = min_t(int, burstcnt, len - count - 1);
		rc = tpm_tis_write_bytes(priv, TPM_DATA_FIFO(priv->locality),
					 burstcnt, buf + count);
		if (rc < 0)
			goto out_err;

		count += burstcnt;

		if (wait_for_tpm_stat(chip, TPM_STS_VALID, chip->timeout_c,
					&priv->int_queue, false) < 0) {
			rc = -ETIME;
			goto out_err;
		}
		status = tpm_tis_status(chip);
		if (!itpm && (status & TPM_STS_DATA_EXPECT) == 0) {
			rc = -EIO;
			goto out_err;
		}
	}

	/* write last byte */
	rc = tpm_tis_write8(priv, TPM_DATA_FIFO(priv->locality), buf[count]);
	if (rc < 0)
		goto out_err;

	if (wait_for_tpm_stat(chip, TPM_STS_VALID, chip->timeout_c,
				&priv->int_queue, false) < 0) {
		rc = -ETIME;
		goto out_err;
	}
	status = tpm_tis_status(chip);
	if (!itpm && (status & TPM_STS_DATA_EXPECT) != 0) {
		rc = -EIO;
		goto out_err;
	}

	return 0;

out_err:
	tpm_tis_ready(chip);
	return rc;
}

static void disable_interrupts(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	u32 intmask;
	int rc;

	if (priv->irq == 0)
		return;

	rc = tpm_tis_read32(priv, TPM_INT_ENABLE(priv->locality), &intmask);
	if (rc < 0)
		intmask = 0;

	intmask &= ~TPM_GLOBAL_INT_ENABLE;
	rc = tpm_tis_write32(priv, TPM_INT_ENABLE(priv->locality), intmask);

	devm_free_irq(chip->dev.parent, priv->irq, chip);
	priv->irq = 0;
	chip->flags &= ~TPM_CHIP_FLAG_IRQ;
}

/*
 * If interrupts are used (signaled by an irq set in the vendor structure)
 * tpm.c can skip polling for the data to be available as the interrupt is
 * waited for here
 */
static int tpm_tis_send_main(struct tpm_chip *chip, const u8 *buf, size_t len)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int rc;
	u32 ordinal;
	unsigned long dur;

	rc = tpm_tis_send_data(chip, buf, len);
	if (rc < 0)
		return rc;

	/* go and do it */
	rc = tpm_tis_write8(priv, TPM_STS(priv->locality), TPM_STS_GO);
	if (rc < 0)
		goto out_err;

	if (chip->flags & TPM_CHIP_FLAG_IRQ) {
		ordinal = be32_to_cpu(*((__be32 *) (buf + 6)));

		dur = tpm_calc_ordinal_duration(chip, ordinal);
		if (wait_for_tpm_stat
		    (chip, TPM_STS_DATA_AVAIL | TPM_STS_VALID, dur,
		     &priv->read_queue, false) < 0) {
			rc = -ETIME;
			goto out_err;
		}
	}
	return 0;
out_err:
	tpm_tis_ready(chip);
	return rc;
}

static int tpm_tis_send(struct tpm_chip *chip, u8 *buf, size_t len)
{
	int rc, irq;
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);

	if (!(chip->flags & TPM_CHIP_FLAG_IRQ) || priv->irq_tested)
		return tpm_tis_send_main(chip, buf, len);

	/* Verify receipt of the expected IRQ */
	irq = priv->irq;
	priv->irq = 0;
	chip->flags &= ~TPM_CHIP_FLAG_IRQ;
	rc = tpm_tis_send_main(chip, buf, len);
	priv->irq = irq;
	chip->flags |= TPM_CHIP_FLAG_IRQ;
	if (!priv->irq_tested)
		tpm_msleep(1);
	if (!priv->irq_tested)
		disable_interrupts(chip);
	priv->irq_tested = true;
	return rc;
}

struct tis_vendor_durations_override {
	u32 did_vid;
	struct tpm1_version version;
	unsigned long durations[3];
};

static const struct  tis_vendor_durations_override vendor_dur_overrides[] = {
	/* STMicroelectronics 0x104a */
	{ 0x0000104a,
	  { 1, 2, 8, 28 },
	  { (2 * 60 * HZ), (2 * 60 * HZ), (2 * 60 * HZ) } },
};

static void tpm_tis_update_durations(struct tpm_chip *chip,
				     unsigned long *duration_cap)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	struct tpm1_version *version;
	u32 did_vid;
	int i, rc;
	cap_t cap;

	chip->duration_adjusted = false;

	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, true);

	rc = tpm_tis_read32(priv, TPM_DID_VID(0), &did_vid);
	if (rc < 0) {
		dev_warn(&chip->dev, "%s: failed to read did_vid. %d\n",
			 __func__, rc);
		goto out;
	}

	/* Try to get a TPM version 1.2 or 1.1 TPM_CAP_VERSION_INFO */
	rc = tpm1_getcap(chip, TPM_CAP_VERSION_1_2, &cap,
			 "attempting to determine the 1.2 version",
			 sizeof(cap.version2));
	if (!rc) {
		version = &cap.version2.version;
	} else {
		rc = tpm1_getcap(chip, TPM_CAP_VERSION_1_1, &cap,
				 "attempting to determine the 1.1 version",
				 sizeof(cap.version1));

		if (rc)
			goto out;

		version = &cap.version1;
	}

	for (i = 0; i != ARRAY_SIZE(vendor_dur_overrides); i++) {
		if (vendor_dur_overrides[i].did_vid != did_vid)
			continue;

		if ((version->major ==
		     vendor_dur_overrides[i].version.major) &&
		    (version->minor ==
		     vendor_dur_overrides[i].version.minor) &&
		    (version->rev_major ==
		     vendor_dur_overrides[i].version.rev_major) &&
		    (version->rev_minor ==
		     vendor_dur_overrides[i].version.rev_minor)) {

			memcpy(duration_cap,
			       vendor_dur_overrides[i].durations,
			       sizeof(vendor_dur_overrides[i].durations));

			chip->duration_adjusted = true;
			goto out;
		}
	}

out:
	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, false);
}

struct tis_vendor_timeout_override {
	u32 did_vid;
	unsigned long timeout_us[4];
};

static const struct tis_vendor_timeout_override vendor_timeout_overrides[] = {
	/* Atmel 3204 */
	{ 0x32041114, { (TIS_SHORT_TIMEOUT*1000), (TIS_LONG_TIMEOUT*1000),
			(TIS_SHORT_TIMEOUT*1000), (TIS_SHORT_TIMEOUT*1000) } },
};

static void tpm_tis_update_timeouts(struct tpm_chip *chip,
				    unsigned long *timeout_cap)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int i, rc;
	u32 did_vid;

	chip->timeout_adjusted = false;

	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, true);

	rc = tpm_tis_read32(priv, TPM_DID_VID(0), &did_vid);
	if (rc < 0) {
		dev_warn(&chip->dev, "%s: failed to read did_vid: %d\n",
			 __func__, rc);
		goto out;
	}

	for (i = 0; i != ARRAY_SIZE(vendor_timeout_overrides); i++) {
		if (vendor_timeout_overrides[i].did_vid != did_vid)
			continue;
		memcpy(timeout_cap, vendor_timeout_overrides[i].timeout_us,
		       sizeof(vendor_timeout_overrides[i].timeout_us));
		chip->timeout_adjusted = true;
	}

out:
	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, false);

	return;
}

/*
 * Early probing for iTPM with STS_DATA_EXPECT flaw.
 * Try sending command without itpm flag set and if that
 * fails, repeat with itpm flag set.
 */
static int probe_itpm(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	int rc = 0;
	static const u8 cmd_getticks[] = {
		0x00, 0xc1, 0x00, 0x00, 0x00, 0x0a,
		0x00, 0x00, 0x00, 0xf1
	};
	size_t len = sizeof(cmd_getticks);
	u16 vendor;

	if (priv->flags & TPM_TIS_ITPM_WORKAROUND)
		return 0;

	rc = tpm_tis_read16(priv, TPM_DID_VID(0), &vendor);
	if (rc < 0)
		return rc;

	/* probe only iTPMS */
	if (vendor != TPM_VID_INTEL)
		return 0;

	if (request_locality(chip, 0) != 0)
		return -EBUSY;

	rc = tpm_tis_send_data(chip, cmd_getticks, len);
	if (rc == 0)
		goto out;

	tpm_tis_ready(chip);

	priv->flags |= TPM_TIS_ITPM_WORKAROUND;

	rc = tpm_tis_send_data(chip, cmd_getticks, len);
	if (rc == 0)
		dev_info(&chip->dev, "Detected an iTPM.\n");
	else {
		priv->flags &= ~TPM_TIS_ITPM_WORKAROUND;
		rc = -EFAULT;
	}

out:
	tpm_tis_ready(chip);
	release_locality(chip, priv->locality);

	return rc;
}

static bool tpm_tis_req_canceled(struct tpm_chip *chip, u8 status)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);

	switch (priv->manufacturer_id) {
	case TPM_VID_WINBOND:
		return ((status == TPM_STS_VALID) ||
			(status == (TPM_STS_VALID | TPM_STS_COMMAND_READY)));
	case TPM_VID_STM:
		return (status == (TPM_STS_VALID | TPM_STS_COMMAND_READY));
	default:
		return (status == TPM_STS_COMMAND_READY);
	}
}

static irqreturn_t tis_int_handler(int dummy, void *dev_id)
{
	struct tpm_chip *chip = dev_id;
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	u32 interrupt;
	int i, rc;

	rc = tpm_tis_read32(priv, TPM_INT_STATUS(priv->locality), &interrupt);
	if (rc < 0)
		return IRQ_NONE;

	if (interrupt == 0)
		return IRQ_NONE;

	priv->irq_tested = true;
	if (interrupt & TPM_INTF_DATA_AVAIL_INT)
		wake_up_interruptible(&priv->read_queue);
	if (interrupt & TPM_INTF_LOCALITY_CHANGE_INT)
		for (i = 0; i < 5; i++)
			if (check_locality(chip, i))
				break;
	if (interrupt &
	    (TPM_INTF_LOCALITY_CHANGE_INT | TPM_INTF_STS_VALID_INT |
	     TPM_INTF_CMD_READY_INT))
		wake_up_interruptible(&priv->int_queue);

	/* Clear interrupts handled with TPM_EOI */
	rc = tpm_tis_write32(priv, TPM_INT_STATUS(priv->locality), interrupt);
	if (rc < 0)
		return IRQ_NONE;

	tpm_tis_read32(priv, TPM_INT_STATUS(priv->locality), &interrupt);
	return IRQ_HANDLED;
}

static int tpm_tis_gen_interrupt(struct tpm_chip *chip)
{
	const char *desc = "attempting to generate an interrupt";
	u32 cap2;
	cap_t cap;
	int ret;

	ret = request_locality(chip, 0);
	if (ret < 0)
		return ret;

	if (chip->flags & TPM_CHIP_FLAG_TPM2)
		ret = tpm2_get_tpm_pt(chip, 0x100, &cap2, desc);
	else
		ret = tpm1_getcap(chip, TPM_CAP_PROP_TIS_TIMEOUT, &cap, desc, 0);

	release_locality(chip, 0);

	return ret;
}

/* Register the IRQ and issue a command that will cause an interrupt. If an
 * irq is seen then leave the chip setup for IRQ operation, otherwise reverse
 * everything and leave in polling mode. Returns 0 on success.
 */
static int tpm_tis_probe_irq_single(struct tpm_chip *chip, u32 intmask,
				    int flags, int irq)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	u8 original_int_vec;
	int rc;
	u32 int_status;

	if (devm_request_irq(chip->dev.parent, irq, tis_int_handler, flags,
			     dev_name(&chip->dev), chip) != 0) {
		dev_info(&chip->dev, "Unable to request irq: %d for probe\n",
			 irq);
		return -1;
	}
	priv->irq = irq;

	rc = tpm_tis_read8(priv, TPM_INT_VECTOR(priv->locality),
			   &original_int_vec);
	if (rc < 0)
		return rc;

	rc = tpm_tis_write8(priv, TPM_INT_VECTOR(priv->locality), irq);
	if (rc < 0)
		return rc;

	rc = tpm_tis_read32(priv, TPM_INT_STATUS(priv->locality), &int_status);
	if (rc < 0)
		return rc;

	/* Clear all existing */
	rc = tpm_tis_write32(priv, TPM_INT_STATUS(priv->locality), int_status);
	if (rc < 0)
		return rc;

	/* Turn on */
	rc = tpm_tis_write32(priv, TPM_INT_ENABLE(priv->locality),
			     intmask | TPM_GLOBAL_INT_ENABLE);
	if (rc < 0)
		return rc;

	priv->irq_tested = false;

	/* Generate an interrupt by having the core call through to
	 * tpm_tis_send
	 */
	rc = tpm_tis_gen_interrupt(chip);
	if (rc < 0)
		return rc;

	/* tpm_tis_send will either confirm the interrupt is working or it
	 * will call disable_irq which undoes all of the above.
	 */
	if (!(chip->flags & TPM_CHIP_FLAG_IRQ)) {
		rc = tpm_tis_write8(priv, original_int_vec,
				TPM_INT_VECTOR(priv->locality));
		if (rc < 0)
			return rc;

		return 1;
	}

	return 0;
}

/* Try to find the IRQ the TPM is using. This is for legacy x86 systems that
 * do not have ACPI/etc. We typically expect the interrupt to be declared if
 * present.
 */
static void tpm_tis_probe_irq(struct tpm_chip *chip, u32 intmask)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	u8 original_int_vec;
	int i, rc;

	rc = tpm_tis_read8(priv, TPM_INT_VECTOR(priv->locality),
			   &original_int_vec);
	if (rc < 0)
		return;

	if (!original_int_vec) {
		if (IS_ENABLED(CONFIG_X86))
			for (i = 3; i <= 15; i++)
				if (!tpm_tis_probe_irq_single(chip, intmask, 0,
							      i))
					return;
	} else if (!tpm_tis_probe_irq_single(chip, intmask, 0,
					     original_int_vec))
		return;
}

void tpm_tis_remove(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	u32 reg = TPM_INT_ENABLE(priv->locality);
	u32 interrupt;
	int rc;

	tpm_tis_clkrun_enable(chip, true);

	rc = tpm_tis_read32(priv, reg, &interrupt);
	if (rc < 0)
		interrupt = 0;

	tpm_tis_write32(priv, reg, ~TPM_GLOBAL_INT_ENABLE & interrupt);

	tpm_tis_clkrun_enable(chip, false);

	if (priv->ilb_base_addr)
		iounmap(priv->ilb_base_addr);
}
EXPORT_SYMBOL_GPL(tpm_tis_remove);

/**
 * tpm_tis_clkrun_enable() - Keep clkrun protocol disabled for entire duration
 *                           of a single TPM command
 * @chip:	TPM chip to use
 * @value:	1 - Disable CLKRUN protocol, so that clocks are free running
 *		0 - Enable CLKRUN protocol
 * Call this function directly in tpm_tis_remove() in error or driver removal
 * path, since the chip->ops is set to NULL in tpm_chip_unregister().
 */
static void tpm_tis_clkrun_enable(struct tpm_chip *chip, bool value)
{
	struct tpm_tis_data *data = dev_get_drvdata(&chip->dev);
	u32 clkrun_val;

	if (!IS_ENABLED(CONFIG_X86) || !is_bsw() ||
	    !data->ilb_base_addr)
		return;

	if (value) {
		data->clkrun_enabled++;
		if (data->clkrun_enabled > 1)
			return;
		clkrun_val = ioread32(data->ilb_base_addr + LPC_CNTRL_OFFSET);

		/* Disable LPC CLKRUN# */
		clkrun_val &= ~LPC_CLKRUN_EN;
		iowrite32(clkrun_val, data->ilb_base_addr + LPC_CNTRL_OFFSET);

		/*
		 * Write any random value on port 0x80 which is on LPC, to make
		 * sure LPC clock is running before sending any TPM command.
		 */
		outb(0xCC, 0x80);
	} else {
		data->clkrun_enabled--;
		if (data->clkrun_enabled)
			return;

		clkrun_val = ioread32(data->ilb_base_addr + LPC_CNTRL_OFFSET);

		/* Enable LPC CLKRUN# */
		clkrun_val |= LPC_CLKRUN_EN;
		iowrite32(clkrun_val, data->ilb_base_addr + LPC_CNTRL_OFFSET);

		/*
		 * Write any random value on port 0x80 which is on LPC, to make
		 * sure LPC clock is running before sending any TPM command.
		 */
		outb(0xCC, 0x80);
	}
}

static const struct tpm_class_ops tpm_tis = {
	.flags = TPM_OPS_AUTO_STARTUP,
	.status = tpm_tis_status,
	.recv = tpm_tis_recv,
	.send = tpm_tis_send,
	.cancel = tpm_tis_ready,
	.update_timeouts = tpm_tis_update_timeouts,
	.update_durations = tpm_tis_update_durations,
	.req_complete_mask = TPM_STS_DATA_AVAIL | TPM_STS_VALID,
	.req_complete_val = TPM_STS_DATA_AVAIL | TPM_STS_VALID,
	.req_canceled = tpm_tis_req_canceled,
	.request_locality = request_locality,
	.relinquish_locality = release_locality,
	.clk_enable = tpm_tis_clkrun_enable,
};

int tpm_tis_core_init(struct device *dev, struct tpm_tis_data *priv, int irq,
		      const struct tpm_tis_phy_ops *phy_ops,
		      acpi_handle acpi_dev_handle)
{
	u32 vendor;
	u32 intfcaps;
	u32 intmask;
	u32 clkrun_val;
	u8 rid;
	int rc, probe;
	struct tpm_chip *chip;

	chip = tpmm_chip_alloc(dev, &tpm_tis);
	if (IS_ERR(chip))
		return PTR_ERR(chip);

#ifdef CONFIG_ACPI
	chip->acpi_dev_handle = acpi_dev_handle;
#endif

	chip->hwrng.quality = priv->rng_quality;

	/* Maximum timeouts */
	chip->timeout_a = msecs_to_jiffies(TIS_TIMEOUT_A_MAX);
	chip->timeout_b = msecs_to_jiffies(TIS_TIMEOUT_B_MAX);
	chip->timeout_c = msecs_to_jiffies(TIS_TIMEOUT_C_MAX);
	chip->timeout_d = msecs_to_jiffies(TIS_TIMEOUT_D_MAX);
	priv->phy_ops = phy_ops;
	dev_set_drvdata(&chip->dev, priv);

	if (is_bsw()) {
		priv->ilb_base_addr = ioremap(INTEL_LEGACY_BLK_BASE_ADDR,
					ILB_REMAP_SIZE);
		if (!priv->ilb_base_addr)
			return -ENOMEM;

		clkrun_val = ioread32(priv->ilb_base_addr + LPC_CNTRL_OFFSET);
		/* Check if CLKRUN# is already not enabled in the LPC bus */
		if (!(clkrun_val & LPC_CLKRUN_EN)) {
			iounmap(priv->ilb_base_addr);
			priv->ilb_base_addr = NULL;
		}
	}

	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, true);

	if (wait_startup(chip, 0) != 0) {
		rc = -ENODEV;
		goto out_err;
	}

	/* Take control of the TPM's interrupt hardware and shut it off */
	rc = tpm_tis_read32(priv, TPM_INT_ENABLE(priv->locality), &intmask);
	if (rc < 0)
		goto out_err;

	intmask |= TPM_INTF_CMD_READY_INT | TPM_INTF_LOCALITY_CHANGE_INT |
		   TPM_INTF_DATA_AVAIL_INT | TPM_INTF_STS_VALID_INT;
	intmask &= ~TPM_GLOBAL_INT_ENABLE;
	tpm_tis_write32(priv, TPM_INT_ENABLE(priv->locality), intmask);

	rc = tpm_chip_start(chip);
	if (rc)
		goto out_err;
	rc = tpm2_probe(chip);
	tpm_chip_stop(chip);
	if (rc)
		goto out_err;

	rc = tpm_tis_read32(priv, TPM_DID_VID(0), &vendor);
	if (rc < 0)
		goto out_err;

	priv->manufacturer_id = vendor;

	rc = tpm_tis_read8(priv, TPM_RID(0), &rid);
	if (rc < 0)
		goto out_err;

	dev_info(dev, "%s TPM (device-id 0x%X, rev-id %d)\n",
		 (chip->flags & TPM_CHIP_FLAG_TPM2) ? "2.0" : "1.2",
		 vendor >> 16, rid);

	probe = probe_itpm(chip);
	if (probe < 0) {
		rc = -ENODEV;
		goto out_err;
	}

	/* Figure out the capabilities */
	rc = tpm_tis_read32(priv, TPM_INTF_CAPS(priv->locality), &intfcaps);
	if (rc < 0)
		goto out_err;

	dev_dbg(dev, "TPM interface capabilities (0x%x):\n",
		intfcaps);
	if (intfcaps & TPM_INTF_BURST_COUNT_STATIC)
		dev_dbg(dev, "\tBurst Count Static\n");
	if (intfcaps & TPM_INTF_CMD_READY_INT)
		dev_dbg(dev, "\tCommand Ready Int Support\n");
	if (intfcaps & TPM_INTF_INT_EDGE_FALLING)
		dev_dbg(dev, "\tInterrupt Edge Falling\n");
	if (intfcaps & TPM_INTF_INT_EDGE_RISING)
		dev_dbg(dev, "\tInterrupt Edge Rising\n");
	if (intfcaps & TPM_INTF_INT_LEVEL_LOW)
		dev_dbg(dev, "\tInterrupt Level Low\n");
	if (intfcaps & TPM_INTF_INT_LEVEL_HIGH)
		dev_dbg(dev, "\tInterrupt Level High\n");
	if (intfcaps & TPM_INTF_LOCALITY_CHANGE_INT)
		dev_dbg(dev, "\tLocality Change Int Support\n");
	if (intfcaps & TPM_INTF_STS_VALID_INT)
		dev_dbg(dev, "\tSts Valid Int Support\n");
	if (intfcaps & TPM_INTF_DATA_AVAIL_INT)
		dev_dbg(dev, "\tData Avail Int Support\n");

	/* INTERRUPT Setup */
	init_waitqueue_head(&priv->read_queue);
	init_waitqueue_head(&priv->int_queue);
	if (irq != -1) {
		/*
		 * Before doing irq testing issue a command to the TPM in polling mode
		 * to make sure it works. May as well use that command to set the
		 * proper timeouts for the driver.
		 */

		rc = request_locality(chip, 0);
		if (rc < 0)
			goto out_err;

		rc = tpm_get_timeouts(chip);

		release_locality(chip, 0);

		if (rc) {
			dev_err(dev, "Could not get TPM timeouts and durations\n");
			rc = -ENODEV;
			goto out_err;
		}

		if (irq) {
			tpm_tis_probe_irq_single(chip, intmask, IRQF_SHARED,
						 irq);
			if (!(chip->flags & TPM_CHIP_FLAG_IRQ)) {
				dev_err(&chip->dev, FW_BUG
					"TPM interrupt not working, polling instead\n");

				disable_interrupts(chip);
			}
		} else {
			tpm_tis_probe_irq(chip, intmask);
		}
	}

	rc = tpm_chip_register(chip);
	if (rc)
		goto out_err;

	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, false);

	return 0;
out_err:
	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, false);

	tpm_tis_remove(chip);

	return rc;
}
EXPORT_SYMBOL_GPL(tpm_tis_core_init);

#ifdef CONFIG_PM_SLEEP
static void tpm_tis_reenable_interrupts(struct tpm_chip *chip)
{
	struct tpm_tis_data *priv = dev_get_drvdata(&chip->dev);
	u32 intmask;
	int rc;

	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, true);

	/* reenable interrupts that device may have lost or
	 * BIOS/firmware may have disabled
	 */
	rc = tpm_tis_write8(priv, TPM_INT_VECTOR(priv->locality), priv->irq);
	if (rc < 0)
		goto out;

	rc = tpm_tis_read32(priv, TPM_INT_ENABLE(priv->locality), &intmask);
	if (rc < 0)
		goto out;

	intmask |= TPM_INTF_CMD_READY_INT
	    | TPM_INTF_LOCALITY_CHANGE_INT | TPM_INTF_DATA_AVAIL_INT
	    | TPM_INTF_STS_VALID_INT | TPM_GLOBAL_INT_ENABLE;

	tpm_tis_write32(priv, TPM_INT_ENABLE(priv->locality), intmask);

out:
	if (chip->ops->clk_enable != NULL)
		chip->ops->clk_enable(chip, false);

	return;
}

int tpm_tis_resume(struct device *dev)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);
	int ret;

	if (chip->flags & TPM_CHIP_FLAG_IRQ)
		tpm_tis_reenable_interrupts(chip);

	ret = tpm_pm_resume(dev);
	if (ret)
		return ret;

	/*
	 * TPM 1.2 requires self-test on resume. This function actually returns
	 * an error code but for unknown reason it isn't handled.
	 */
	if (!(chip->flags & TPM_CHIP_FLAG_TPM2)) {
		ret = request_locality(chip, 0);
		if (ret < 0)
			return ret;

		tpm1_do_selftest(chip);

		release_locality(chip, 0);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(tpm_tis_resume);
#endif

MODULE_AUTHOR("Leendert van Doorn (leendert@watson.ibm.com)");
MODULE_DESCRIPTION("TPM Driver");
MODULE_VERSION("2.0");
MODULE_LICENSE("GPL");

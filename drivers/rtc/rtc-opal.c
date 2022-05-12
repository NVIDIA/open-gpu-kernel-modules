// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * IBM OPAL RTC driver
 * Copyright (C) 2014 IBM
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define DRVNAME		"rtc-opal"

#include <linux/module.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <linux/bcd.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <asm/opal.h>
#include <asm/firmware.h>

static void opal_to_tm(u32 y_m_d, u64 h_m_s_ms, struct rtc_time *tm)
{
	tm->tm_year = ((bcd2bin(y_m_d >> 24) * 100) +
		       bcd2bin((y_m_d >> 16) & 0xff)) - 1900;
	tm->tm_mon  = bcd2bin((y_m_d >> 8) & 0xff) - 1;
	tm->tm_mday = bcd2bin(y_m_d & 0xff);
	tm->tm_hour = bcd2bin((h_m_s_ms >> 56) & 0xff);
	tm->tm_min  = bcd2bin((h_m_s_ms >> 48) & 0xff);
	tm->tm_sec  = bcd2bin((h_m_s_ms >> 40) & 0xff);

	tm->tm_wday = -1;
}

static void tm_to_opal(struct rtc_time *tm, u32 *y_m_d, u64 *h_m_s_ms)
{
	*y_m_d |= ((u32)bin2bcd((tm->tm_year + 1900) / 100)) << 24;
	*y_m_d |= ((u32)bin2bcd((tm->tm_year + 1900) % 100)) << 16;
	*y_m_d |= ((u32)bin2bcd((tm->tm_mon + 1))) << 8;
	*y_m_d |= ((u32)bin2bcd(tm->tm_mday));

	*h_m_s_ms |= ((u64)bin2bcd(tm->tm_hour)) << 56;
	*h_m_s_ms |= ((u64)bin2bcd(tm->tm_min)) << 48;
	*h_m_s_ms |= ((u64)bin2bcd(tm->tm_sec)) << 40;
}

static int opal_get_rtc_time(struct device *dev, struct rtc_time *tm)
{
	s64 rc = OPAL_BUSY;
	int retries = 10;
	u32 y_m_d;
	u64 h_m_s_ms;
	__be32 __y_m_d;
	__be64 __h_m_s_ms;

	while (rc == OPAL_BUSY || rc == OPAL_BUSY_EVENT) {
		rc = opal_rtc_read(&__y_m_d, &__h_m_s_ms);
		if (rc == OPAL_BUSY_EVENT) {
			msleep(OPAL_BUSY_DELAY_MS);
			opal_poll_events(NULL);
		} else if (rc == OPAL_BUSY) {
			msleep(OPAL_BUSY_DELAY_MS);
		} else if (rc == OPAL_HARDWARE || rc == OPAL_INTERNAL_ERROR) {
			if (retries--) {
				msleep(10); /* Wait 10ms before retry */
				rc = OPAL_BUSY; /* go around again */
			}
		}
	}

	if (rc != OPAL_SUCCESS)
		return -EIO;

	y_m_d = be32_to_cpu(__y_m_d);
	h_m_s_ms = be64_to_cpu(__h_m_s_ms);
	opal_to_tm(y_m_d, h_m_s_ms, tm);

	return 0;
}

static int opal_set_rtc_time(struct device *dev, struct rtc_time *tm)
{
	s64 rc = OPAL_BUSY;
	int retries = 10;
	u32 y_m_d = 0;
	u64 h_m_s_ms = 0;

	tm_to_opal(tm, &y_m_d, &h_m_s_ms);

	while (rc == OPAL_BUSY || rc == OPAL_BUSY_EVENT) {
		rc = opal_rtc_write(y_m_d, h_m_s_ms);
		if (rc == OPAL_BUSY_EVENT) {
			msleep(OPAL_BUSY_DELAY_MS);
			opal_poll_events(NULL);
		} else if (rc == OPAL_BUSY) {
			msleep(OPAL_BUSY_DELAY_MS);
		} else if (rc == OPAL_HARDWARE || rc == OPAL_INTERNAL_ERROR) {
			if (retries--) {
				msleep(10); /* Wait 10ms before retry */
				rc = OPAL_BUSY; /* go around again */
			}
		}
	}

	return rc == OPAL_SUCCESS ? 0 : -EIO;
}

/*
 * TPO	Timed Power-On
 *
 * TPO get/set OPAL calls care about the hour and min and to make it consistent
 * with the rtc utility time conversion functions, we use the 'u64' to store
 * its value and perform bit shift by 32 before use..
 */
static int opal_get_tpo_time(struct device *dev, struct rtc_wkalrm *alarm)
{
	__be32 __y_m_d, __h_m;
	struct opal_msg msg;
	int rc, token;
	u64 h_m_s_ms;
	u32 y_m_d;

	token = opal_async_get_token_interruptible();
	if (token < 0) {
		if (token != -ERESTARTSYS)
			pr_err("Failed to get the async token\n");

		return token;
	}

	rc = opal_tpo_read(token, &__y_m_d, &__h_m);
	if (rc != OPAL_ASYNC_COMPLETION) {
		rc = -EIO;
		goto exit;
	}

	rc = opal_async_wait_response(token, &msg);
	if (rc) {
		rc = -EIO;
		goto exit;
	}

	rc = opal_get_async_rc(msg);
	if (rc != OPAL_SUCCESS) {
		rc = -EIO;
		goto exit;
	}

	y_m_d = be32_to_cpu(__y_m_d);
	h_m_s_ms = ((u64)be32_to_cpu(__h_m) << 32);

	/* check if no alarm is set */
	if (y_m_d == 0 && h_m_s_ms == 0) {
		pr_debug("No alarm is set\n");
		rc = -ENOENT;
		goto exit;
	} else {
		pr_debug("Alarm set to %x %llx\n", y_m_d, h_m_s_ms);
	}

	opal_to_tm(y_m_d, h_m_s_ms, &alarm->time);

exit:
	opal_async_release_token(token);
	return rc;
}

/* Set Timed Power-On */
static int opal_set_tpo_time(struct device *dev, struct rtc_wkalrm *alarm)
{
	u64 h_m_s_ms = 0;
	struct opal_msg msg;
	u32 y_m_d = 0;
	int token, rc;

	/* if alarm is enabled */
	if (alarm->enabled) {
		tm_to_opal(&alarm->time, &y_m_d, &h_m_s_ms);
		pr_debug("Alarm set to %x %llx\n", y_m_d, h_m_s_ms);

	} else {
		pr_debug("Alarm getting disabled\n");
	}

	token = opal_async_get_token_interruptible();
	if (token < 0) {
		if (token != -ERESTARTSYS)
			pr_err("Failed to get the async token\n");

		return token;
	}

	/* TPO, we care about hour and minute */
	rc = opal_tpo_write(token, y_m_d,
			    (u32)((h_m_s_ms >> 32) & 0xffff0000));
	if (rc != OPAL_ASYNC_COMPLETION) {
		rc = -EIO;
		goto exit;
	}

	rc = opal_async_wait_response(token, &msg);
	if (rc) {
		rc = -EIO;
		goto exit;
	}

	rc = opal_get_async_rc(msg);
	if (rc != OPAL_SUCCESS)
		rc = -EIO;

exit:
	opal_async_release_token(token);
	return rc;
}

static int opal_tpo_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct rtc_wkalrm alarm = { .enabled = 0 };

	/*
	 * TPO is automatically enabled when opal_set_tpo_time() is called with
	 * non-zero rtc-time. We only handle disable case which needs to be
	 * explicitly told to opal.
	 */
	return enabled ? 0 : opal_set_tpo_time(dev, &alarm);
}

static const struct rtc_class_ops opal_rtc_ops = {
	.read_time	= opal_get_rtc_time,
	.set_time	= opal_set_rtc_time,
	.read_alarm	= opal_get_tpo_time,
	.set_alarm	= opal_set_tpo_time,
	.alarm_irq_enable = opal_tpo_alarm_irq_enable,
};

static int opal_rtc_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;

	rtc = devm_rtc_allocate_device(&pdev->dev);
	if (IS_ERR(rtc))
		return PTR_ERR(rtc);

	if (pdev->dev.of_node &&
	    (of_property_read_bool(pdev->dev.of_node, "wakeup-source") ||
	     of_property_read_bool(pdev->dev.of_node, "has-tpo")/* legacy */))
		device_set_wakeup_capable(&pdev->dev, true);
	else
		clear_bit(RTC_FEATURE_ALARM, rtc->features);

	rtc->ops = &opal_rtc_ops;
	rtc->range_min = RTC_TIMESTAMP_BEGIN_0000;
	rtc->range_max = RTC_TIMESTAMP_END_9999;
	rtc->uie_unsupported = 1;

	return devm_rtc_register_device(rtc);
}

static const struct of_device_id opal_rtc_match[] = {
	{
		.compatible	= "ibm,opal-rtc",
	},
	{ }
};
MODULE_DEVICE_TABLE(of, opal_rtc_match);

static const struct platform_device_id opal_rtc_driver_ids[] = {
	{
		.name		= "opal-rtc",
	},
	{ }
};
MODULE_DEVICE_TABLE(platform, opal_rtc_driver_ids);

static struct platform_driver opal_rtc_driver = {
	.probe		= opal_rtc_probe,
	.id_table	= opal_rtc_driver_ids,
	.driver		= {
		.name		= DRVNAME,
		.of_match_table	= opal_rtc_match,
	},
};

static int __init opal_rtc_init(void)
{
	if (!firmware_has_feature(FW_FEATURE_OPAL))
		return -ENODEV;

	return platform_driver_register(&opal_rtc_driver);
}

static void __exit opal_rtc_exit(void)
{
	platform_driver_unregister(&opal_rtc_driver);
}

MODULE_AUTHOR("Neelesh Gupta <neelegup@linux.vnet.ibm.com>");
MODULE_DESCRIPTION("IBM OPAL RTC driver");
MODULE_LICENSE("GPL");

module_init(opal_rtc_init);
module_exit(opal_rtc_exit);

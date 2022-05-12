/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ChromeOS Embedded Controller core interface.
 *
 * Copyright (C) 2020 Google LLC
 */

#ifndef __CROS_EC_H
#define __CROS_EC_H

#include <linux/interrupt.h>

int cros_ec_register(struct cros_ec_device *ec_dev);
int cros_ec_unregister(struct cros_ec_device *ec_dev);

int cros_ec_suspend(struct cros_ec_device *ec_dev);
int cros_ec_resume(struct cros_ec_device *ec_dev);

irqreturn_t cros_ec_irq_thread(int irq, void *data);

#endif /* __CROS_EC_H */

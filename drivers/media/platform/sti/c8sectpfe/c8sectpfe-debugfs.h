/* SPDX-License-Identifier: GPL-2.0 */
/*
 * c8sectpfe-debugfs.h - C8SECTPFE STi DVB driver debugfs header
 *
 * Copyright (c) STMicroelectronics 2015
 *
 * Authors: Peter Griffin <peter.griffin@linaro.org>
 */

#ifndef __C8SECTPFE_DEBUG_H
#define __C8SECTPFE_DEBUG_H

#include "c8sectpfe-core.h"

void c8sectpfe_debugfs_init(struct c8sectpfei *);
void c8sectpfe_debugfs_exit(struct c8sectpfei *);

#endif /* __C8SECTPFE_DEBUG_H */

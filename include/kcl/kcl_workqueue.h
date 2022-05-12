/* SPDX-License-Identifier: GPL-2.0 */

#ifndef KCL_LINUX_WORKQUEUE_H
#define KCL_LINUX_WORKQUEUE_H

#include <linux/workqueue.h>

/*
 * System-wide workqueues which are always present.
 *
 * system_highpri_wq is similar to system_wq but for work items which
 * require WQ_HIGHPRI.
 *
 * v3.15-rc1-18-g73e4354444ee workqueue: declare system_highpri_wq
 * v3.6-rc1-20-g1aabe902ca36 workqueue: introduce system_highpri_wq
 */
extern struct workqueue_struct *system_highpri_wq;

#endif

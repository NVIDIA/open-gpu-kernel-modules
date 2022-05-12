/* SPDX-License-Identifier: GPL-2.0 */
#ifndef KCL_KCL_RESERVATION_H
#define KCL_KCL_RESERVATION_H

#include <linux/dma-resv.h>

#ifndef HAVE_LINUX_DMA_RESV_H
#define reservation_object dma_resv
#define reservation_object_list dma_resv_list
#endif

#endif /* AMDKCL_RESERVATION_H */

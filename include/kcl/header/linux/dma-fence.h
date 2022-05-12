/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER__LINUX_DMA_FENCE_H_H_
#define _KCL_HEADER__LINUX_DMA_FENCE_H_H_

#if defined(HAVE_LINUX_DMA_FENCE_H)
#include_next <linux/dma-fence.h>
#else
#include <linux/fence.h>
#endif

#endif

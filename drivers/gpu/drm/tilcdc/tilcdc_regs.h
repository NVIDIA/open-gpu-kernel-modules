/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2012 Texas Instruments
 * Author: Rob Clark <robdclark@gmail.com>
 */

#ifndef __TILCDC_REGS_H__
#define __TILCDC_REGS_H__

/* LCDC register definitions, based on da8xx-fb */

#include <linux/bitops.h>

#include "tilcdc_drv.h"

/* LCDC Status Register */
#define LCDC_END_OF_FRAME1                       BIT(9)
#define LCDC_END_OF_FRAME0                       BIT(8)
#define LCDC_PL_LOAD_DONE                        BIT(6)
#define LCDC_FIFO_UNDERFLOW                      BIT(5)
#define LCDC_SYNC_LOST                           BIT(2)
#define LCDC_FRAME_DONE                          BIT(0)

/* LCDC DMA Control Register */
#define LCDC_DMA_BURST_SIZE(x)                   ((x) << 4)
#define LCDC_DMA_BURST_SIZE_MASK                 ((0x7) << 4)
#define LCDC_DMA_BURST_1                         0x0
#define LCDC_DMA_BURST_2                         0x1
#define LCDC_DMA_BURST_4                         0x2
#define LCDC_DMA_BURST_8                         0x3
#define LCDC_DMA_BURST_16                        0x4
#define LCDC_DMA_FIFO_THRESHOLD(x)               ((x) << 8)
#define LCDC_DMA_FIFO_THRESHOLD_MASK             ((0x3) << 8)
#define LCDC_V1_END_OF_FRAME_INT_ENA             BIT(2)
#define LCDC_V2_END_OF_FRAME0_INT_ENA            BIT(8)
#define LCDC_V2_END_OF_FRAME1_INT_ENA            BIT(9)
#define LCDC_DUAL_FRAME_BUFFER_ENABLE            BIT(0)

/* LCDC Control Register */
#define LCDC_CLK_DIVISOR(x)                      ((x) << 8)
#define LCDC_CLK_DIVISOR_MASK                    ((0xFF) << 8)
#define LCDC_RASTER_MODE                         0x01

/* LCDC Raster Control Register */
#define LCDC_PALETTE_LOAD_MODE(x)                ((x) << 20)
#define LCDC_PALETTE_LOAD_MODE_MASK              ((0x3) << 20)
#define PALETTE_AND_DATA                         0x00
#define PALETTE_ONLY                             0x01
#define DATA_ONLY                                0x02

#define LCDC_MONO_8BIT_MODE                      BIT(9)
#define LCDC_RASTER_ORDER                        BIT(8)
#define LCDC_TFT_MODE                            BIT(7)
#define LCDC_V1_UNDERFLOW_INT_ENA                BIT(6)
#define LCDC_V2_UNDERFLOW_INT_ENA                BIT(5)
#define LCDC_V1_PL_INT_ENA                       BIT(4)
#define LCDC_V2_PL_INT_ENA                       BIT(6)
#define LCDC_V1_SYNC_LOST_INT_ENA                BIT(5)
#define LCDC_V1_FRAME_DONE_INT_ENA               BIT(3)
#define LCDC_MONOCHROME_MODE                     BIT(1)
#define LCDC_RASTER_ENABLE                       BIT(0)
#define LCDC_TFT_ALT_ENABLE                      BIT(23)
#define LCDC_STN_565_ENABLE                      BIT(24)
#define LCDC_V2_DMA_CLK_EN                       BIT(2)
#define LCDC_V2_LIDD_CLK_EN                      BIT(1)
#define LCDC_V2_CORE_CLK_EN                      BIT(0)
#define LCDC_V2_LPP_B10                          26
#define LCDC_V2_TFT_24BPP_MODE                   BIT(25)
#define LCDC_V2_TFT_24BPP_UNPACK                 BIT(26)

/* LCDC Raster Timing 2 Register */
#define LCDC_AC_BIAS_TRANSITIONS_PER_INT(x)      ((x) << 16)
#define LCDC_AC_BIAS_TRANSITIONS_PER_INT_MASK    ((0xF) << 16)
#define LCDC_AC_BIAS_FREQUENCY(x)                ((x) << 8)
#define LCDC_AC_BIAS_FREQUENCY_MASK              ((0xFF) << 8)
#define LCDC_SYNC_CTRL                           BIT(25)
#define LCDC_SYNC_EDGE                           BIT(24)
#define LCDC_INVERT_PIXEL_CLOCK                  BIT(22)
#define LCDC_INVERT_HSYNC                        BIT(21)
#define LCDC_INVERT_VSYNC                        BIT(20)
#define LCDC_LPP_B10                             BIT(26)

/* LCDC Block */
#define LCDC_PID_REG                             0x0
#define LCDC_CTRL_REG                            0x4
#define LCDC_STAT_REG                            0x8
#define LCDC_RASTER_CTRL_REG                     0x28
#define LCDC_RASTER_TIMING_0_REG                 0x2c
#define LCDC_RASTER_TIMING_1_REG                 0x30
#define LCDC_RASTER_TIMING_2_REG                 0x34
#define LCDC_DMA_CTRL_REG                        0x40
#define LCDC_DMA_FB_BASE_ADDR_0_REG              0x44
#define LCDC_DMA_FB_CEILING_ADDR_0_REG           0x48
#define LCDC_DMA_FB_BASE_ADDR_1_REG              0x4c
#define LCDC_DMA_FB_CEILING_ADDR_1_REG           0x50

/* Interrupt Registers available only in Version 2 */
#define LCDC_RAW_STAT_REG                        0x58
#define LCDC_MASKED_STAT_REG                     0x5c
#define LCDC_INT_ENABLE_SET_REG                  0x60
#define LCDC_INT_ENABLE_CLR_REG                  0x64
#define LCDC_END_OF_INT_IND_REG                  0x68

/* Clock registers available only on Version 2 */
#define LCDC_CLK_ENABLE_REG                      0x6c
#define LCDC_CLK_RESET_REG                       0x70
#define LCDC_CLK_MAIN_RESET                      BIT(3)


/*
 * Helpers:
 */

static inline void tilcdc_write(struct drm_device *dev, u32 reg, u32 data)
{
	struct tilcdc_drm_private *priv = dev->dev_private;
	iowrite32(data, priv->mmio + reg);
}

static inline void tilcdc_write64(struct drm_device *dev, u32 reg, u64 data)
{
	struct tilcdc_drm_private *priv = dev->dev_private;
	volatile void __iomem *addr = priv->mmio + reg;

#if defined(iowrite64) && !defined(iowrite64_is_nonatomic)
	iowrite64(data, addr);
#else
	__iowmb();
	/* This compiles to strd (=64-bit write) on ARM7 */
	*(volatile u64 __force *)addr = __cpu_to_le64(data);
#endif
}

static inline u32 tilcdc_read(struct drm_device *dev, u32 reg)
{
	struct tilcdc_drm_private *priv = dev->dev_private;
	return ioread32(priv->mmio + reg);
}

static inline void tilcdc_write_mask(struct drm_device *dev, u32 reg,
				     u32 val, u32 mask)
{
	tilcdc_write(dev, reg, (tilcdc_read(dev, reg) & ~mask) | (val & mask));
}

static inline void tilcdc_set(struct drm_device *dev, u32 reg, u32 mask)
{
	tilcdc_write(dev, reg, tilcdc_read(dev, reg) | mask);
}

static inline void tilcdc_clear(struct drm_device *dev, u32 reg, u32 mask)
{
	tilcdc_write(dev, reg, tilcdc_read(dev, reg) & ~mask);
}

/* the register to read/clear irqstatus differs between v1 and v2 of the IP */
static inline u32 tilcdc_irqstatus_reg(struct drm_device *dev)
{
	struct tilcdc_drm_private *priv = dev->dev_private;
	return (priv->rev == 2) ? LCDC_MASKED_STAT_REG : LCDC_STAT_REG;
}

static inline u32 tilcdc_read_irqstatus(struct drm_device *dev)
{
	return tilcdc_read(dev, tilcdc_irqstatus_reg(dev));
}

static inline void tilcdc_clear_irqstatus(struct drm_device *dev, u32 mask)
{
	tilcdc_write(dev, tilcdc_irqstatus_reg(dev), mask);
}

#endif /* __TILCDC_REGS_H__ */

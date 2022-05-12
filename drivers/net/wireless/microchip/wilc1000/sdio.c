// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2012 - 2018 Microchip Technology Inc., and its subsidiaries.
 * All rights reserved.
 */

#include <linux/clk.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio.h>
#include <linux/of_irq.h>

#include "netdev.h"
#include "cfg80211.h"

#define SDIO_MODALIAS "wilc1000_sdio"

static const struct sdio_device_id wilc_sdio_ids[] = {
	{ SDIO_DEVICE(SDIO_VENDOR_ID_MICROCHIP_WILC, SDIO_DEVICE_ID_MICROCHIP_WILC1000) },
	{ },
};

#define WILC_SDIO_BLOCK_SIZE 512

struct wilc_sdio {
	bool irq_gpio;
	u32 block_size;
	int has_thrpt_enh3;
};

struct sdio_cmd52 {
	u32 read_write:		1;
	u32 function:		3;
	u32 raw:		1;
	u32 address:		17;
	u32 data:		8;
};

struct sdio_cmd53 {
	u32 read_write:		1;
	u32 function:		3;
	u32 block_mode:		1;
	u32 increment:		1;
	u32 address:		17;
	u32 count:		9;
	u8 *buffer;
	u32 block_size;
};

static const struct wilc_hif_func wilc_hif_sdio;

static void wilc_sdio_interrupt(struct sdio_func *func)
{
	sdio_release_host(func);
	wilc_handle_isr(sdio_get_drvdata(func));
	sdio_claim_host(func);
}

static int wilc_sdio_cmd52(struct wilc *wilc, struct sdio_cmd52 *cmd)
{
	struct sdio_func *func = container_of(wilc->dev, struct sdio_func, dev);
	int ret;
	u8 data;

	sdio_claim_host(func);

	func->num = cmd->function;
	if (cmd->read_write) {  /* write */
		if (cmd->raw) {
			sdio_writeb(func, cmd->data, cmd->address, &ret);
			data = sdio_readb(func, cmd->address, &ret);
			cmd->data = data;
		} else {
			sdio_writeb(func, cmd->data, cmd->address, &ret);
		}
	} else {        /* read */
		data = sdio_readb(func, cmd->address, &ret);
		cmd->data = data;
	}

	sdio_release_host(func);

	if (ret)
		dev_err(&func->dev, "%s..failed, err(%d)\n", __func__, ret);
	return ret;
}

static int wilc_sdio_cmd53(struct wilc *wilc, struct sdio_cmd53 *cmd)
{
	struct sdio_func *func = container_of(wilc->dev, struct sdio_func, dev);
	int size, ret;

	sdio_claim_host(func);

	func->num = cmd->function;
	func->cur_blksize = cmd->block_size;
	if (cmd->block_mode)
		size = cmd->count * cmd->block_size;
	else
		size = cmd->count;

	if (cmd->read_write) {  /* write */
		ret = sdio_memcpy_toio(func, cmd->address,
				       (void *)cmd->buffer, size);
	} else {        /* read */
		ret = sdio_memcpy_fromio(func, (void *)cmd->buffer,
					 cmd->address,  size);
	}

	sdio_release_host(func);

	if (ret)
		dev_err(&func->dev, "%s..failed, err(%d)\n", __func__,  ret);

	return ret;
}

static int wilc_sdio_probe(struct sdio_func *func,
			   const struct sdio_device_id *id)
{
	struct wilc *wilc;
	int ret;
	struct wilc_sdio *sdio_priv;

	sdio_priv = kzalloc(sizeof(*sdio_priv), GFP_KERNEL);
	if (!sdio_priv)
		return -ENOMEM;

	ret = wilc_cfg80211_init(&wilc, &func->dev, WILC_HIF_SDIO,
				 &wilc_hif_sdio);
	if (ret) {
		kfree(sdio_priv);
		return ret;
	}

	if (IS_ENABLED(CONFIG_WILC1000_HW_OOB_INTR)) {
		struct device_node *np = func->card->dev.of_node;
		int irq_num = of_irq_get(np, 0);

		if (irq_num > 0) {
			wilc->dev_irq_num = irq_num;
			sdio_priv->irq_gpio = true;
		}
	}

	sdio_set_drvdata(func, wilc);
	wilc->bus_data = sdio_priv;
	wilc->dev = &func->dev;

	wilc->rtc_clk = devm_clk_get(&func->card->dev, "rtc");
	if (PTR_ERR_OR_ZERO(wilc->rtc_clk) == -EPROBE_DEFER) {
		kfree(sdio_priv);
		return -EPROBE_DEFER;
	} else if (!IS_ERR(wilc->rtc_clk))
		clk_prepare_enable(wilc->rtc_clk);

	dev_info(&func->dev, "Driver Initializing success\n");
	return 0;
}

static void wilc_sdio_remove(struct sdio_func *func)
{
	struct wilc *wilc = sdio_get_drvdata(func);

	if (!IS_ERR(wilc->rtc_clk))
		clk_disable_unprepare(wilc->rtc_clk);

	wilc_netdev_cleanup(wilc);
}

static int wilc_sdio_reset(struct wilc *wilc)
{
	struct sdio_cmd52 cmd;
	int ret;
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);

	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = SDIO_CCCR_ABORT;
	cmd.data = WILC_SDIO_CCCR_ABORT_RESET;
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Fail cmd 52, reset cmd ...\n");
		return ret;
	}
	return 0;
}

static int wilc_sdio_suspend(struct device *dev)
{
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct wilc *wilc = sdio_get_drvdata(func);
	int ret;

	dev_info(dev, "sdio suspend\n");
	chip_wakeup(wilc);

	if (!IS_ERR(wilc->rtc_clk))
		clk_disable_unprepare(wilc->rtc_clk);

	if (wilc->suspend_event) {
		host_sleep_notify(wilc);
		chip_allow_sleep(wilc);
	}

	ret = wilc_sdio_reset(wilc);
	if (ret) {
		dev_err(&func->dev, "Fail reset sdio\n");
		return ret;
	}
	sdio_claim_host(func);

	return 0;
}

static int wilc_sdio_enable_interrupt(struct wilc *dev)
{
	struct sdio_func *func = container_of(dev->dev, struct sdio_func, dev);
	int ret = 0;

	sdio_claim_host(func);
	ret = sdio_claim_irq(func, wilc_sdio_interrupt);
	sdio_release_host(func);

	if (ret < 0) {
		dev_err(&func->dev, "can't claim sdio_irq, err(%d)\n", ret);
		ret = -EIO;
	}
	return ret;
}

static void wilc_sdio_disable_interrupt(struct wilc *dev)
{
	struct sdio_func *func = container_of(dev->dev, struct sdio_func, dev);
	int ret;

	sdio_claim_host(func);
	ret = sdio_release_irq(func);
	if (ret < 0)
		dev_err(&func->dev, "can't release sdio_irq, err(%d)\n", ret);
	sdio_release_host(func);
}

/********************************************
 *
 *      Function 0
 *
 ********************************************/

static int wilc_sdio_set_func0_csa_address(struct wilc *wilc, u32 adr)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct sdio_cmd52 cmd;
	int ret;

	/**
	 *      Review: BIG ENDIAN
	 **/
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = WILC_SDIO_FBR_CSA_REG;
	cmd.data = (u8)adr;
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Failed cmd52, set %04x data...\n",
			cmd.address);
		return ret;
	}

	cmd.address = WILC_SDIO_FBR_CSA_REG + 1;
	cmd.data = (u8)(adr >> 8);
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Failed cmd52, set %04x data...\n",
			cmd.address);
		return ret;
	}

	cmd.address = WILC_SDIO_FBR_CSA_REG + 2;
	cmd.data = (u8)(adr >> 16);
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Failed cmd52, set %04x data...\n",
			cmd.address);
		return ret;
	}

	return 0;
}

static int wilc_sdio_set_block_size(struct wilc *wilc, u8 func_num,
				    u32 block_size)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct sdio_cmd52 cmd;
	int ret;

	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = SDIO_FBR_BASE(func_num) + SDIO_CCCR_BLKSIZE;
	cmd.data = (u8)block_size;
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Failed cmd52, set %04x data...\n",
			cmd.address);
		return ret;
	}

	cmd.address = SDIO_FBR_BASE(func_num) + SDIO_CCCR_BLKSIZE +  1;
	cmd.data = (u8)(block_size >> 8);
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Failed cmd52, set %04x data...\n",
			cmd.address);
		return ret;
	}

	return 0;
}

/********************************************
 *
 *      Sdio interfaces
 *
 ********************************************/
static int wilc_sdio_write_reg(struct wilc *wilc, u32 addr, u32 data)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	int ret;

	cpu_to_le32s(&data);

	if (addr >= 0xf0 && addr <= 0xff) { /* only vendor specific registers */
		struct sdio_cmd52 cmd;

		cmd.read_write = 1;
		cmd.function = 0;
		cmd.raw = 0;
		cmd.address = addr;
		cmd.data = data;
		ret = wilc_sdio_cmd52(wilc, &cmd);
		if (ret)
			dev_err(&func->dev,
				"Failed cmd 52, read reg (%08x) ...\n", addr);
	} else {
		struct sdio_cmd53 cmd;

		/**
		 *      set the AHB address
		 **/
		ret = wilc_sdio_set_func0_csa_address(wilc, addr);
		if (ret)
			return ret;

		cmd.read_write = 1;
		cmd.function = 0;
		cmd.address = WILC_SDIO_FBR_DATA_REG;
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = 4;
		cmd.buffer = (u8 *)&data;
		cmd.block_size = sdio_priv->block_size;
		ret = wilc_sdio_cmd53(wilc, &cmd);
		if (ret)
			dev_err(&func->dev,
				"Failed cmd53, write reg (%08x)...\n", addr);
	}

	return ret;
}

static int wilc_sdio_write(struct wilc *wilc, u32 addr, u8 *buf, u32 size)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	u32 block_size = sdio_priv->block_size;
	struct sdio_cmd53 cmd;
	int nblk, nleft, ret;

	cmd.read_write = 1;
	if (addr > 0) {
		/**
		 *      func 0 access
		 **/
		cmd.function = 0;
		cmd.address = WILC_SDIO_FBR_DATA_REG;
	} else {
		/**
		 *      func 1 access
		 **/
		cmd.function = 1;
		cmd.address = WILC_SDIO_F1_DATA_REG;
	}

	size = ALIGN(size, 4);
	nblk = size / block_size;
	nleft = size % block_size;

	if (nblk > 0) {
		cmd.block_mode = 1;
		cmd.increment = 1;
		cmd.count = nblk;
		cmd.buffer = buf;
		cmd.block_size = block_size;
		if (addr > 0) {
			ret = wilc_sdio_set_func0_csa_address(wilc, addr);
			if (ret)
				return ret;
		}
		ret = wilc_sdio_cmd53(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd53 [%x], block send...\n", addr);
			return ret;
		}
		if (addr > 0)
			addr += nblk * block_size;
		buf += nblk * block_size;
	}

	if (nleft > 0) {
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = nleft;
		cmd.buffer = buf;

		cmd.block_size = block_size;

		if (addr > 0) {
			ret = wilc_sdio_set_func0_csa_address(wilc, addr);
			if (ret)
				return ret;
		}
		ret = wilc_sdio_cmd53(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd53 [%x], bytes send...\n", addr);
			return ret;
		}
	}

	return 0;
}

static int wilc_sdio_read_reg(struct wilc *wilc, u32 addr, u32 *data)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	int ret;

	if (addr >= 0xf0 && addr <= 0xff) { /* only vendor specific registers */
		struct sdio_cmd52 cmd;

		cmd.read_write = 0;
		cmd.function = 0;
		cmd.raw = 0;
		cmd.address = addr;
		ret = wilc_sdio_cmd52(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd 52, read reg (%08x) ...\n", addr);
			return ret;
		}
		*data = cmd.data;
	} else {
		struct sdio_cmd53 cmd;

		ret = wilc_sdio_set_func0_csa_address(wilc, addr);
		if (ret)
			return ret;

		cmd.read_write = 0;
		cmd.function = 0;
		cmd.address = WILC_SDIO_FBR_DATA_REG;
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = 4;
		cmd.buffer = (u8 *)data;

		cmd.block_size = sdio_priv->block_size;
		ret = wilc_sdio_cmd53(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd53, read reg (%08x)...\n", addr);
			return ret;
		}
	}

	le32_to_cpus(data);
	return 0;
}

static int wilc_sdio_read(struct wilc *wilc, u32 addr, u8 *buf, u32 size)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	u32 block_size = sdio_priv->block_size;
	struct sdio_cmd53 cmd;
	int nblk, nleft, ret;

	cmd.read_write = 0;
	if (addr > 0) {
		/**
		 *      func 0 access
		 **/
		cmd.function = 0;
		cmd.address = WILC_SDIO_FBR_DATA_REG;
	} else {
		/**
		 *      func 1 access
		 **/
		cmd.function = 1;
		cmd.address = WILC_SDIO_F1_DATA_REG;
	}

	size = ALIGN(size, 4);
	nblk = size / block_size;
	nleft = size % block_size;

	if (nblk > 0) {
		cmd.block_mode = 1;
		cmd.increment = 1;
		cmd.count = nblk;
		cmd.buffer = buf;
		cmd.block_size = block_size;
		if (addr > 0) {
			ret = wilc_sdio_set_func0_csa_address(wilc, addr);
			if (ret)
				return ret;
		}
		ret = wilc_sdio_cmd53(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd53 [%x], block read...\n", addr);
			return ret;
		}
		if (addr > 0)
			addr += nblk * block_size;
		buf += nblk * block_size;
	}       /* if (nblk > 0) */

	if (nleft > 0) {
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = nleft;
		cmd.buffer = buf;

		cmd.block_size = block_size;

		if (addr > 0) {
			ret = wilc_sdio_set_func0_csa_address(wilc, addr);
			if (ret)
				return ret;
		}
		ret = wilc_sdio_cmd53(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd53 [%x], bytes read...\n", addr);
			return ret;
		}
	}

	return 0;
}

/********************************************
 *
 *      Bus interfaces
 *
 ********************************************/

static int wilc_sdio_deinit(struct wilc *wilc)
{
	return 0;
}

static int wilc_sdio_init(struct wilc *wilc, bool resume)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	struct sdio_cmd52 cmd;
	int loop, ret;
	u32 chipid;

	/**
	 *      function 0 csa enable
	 **/
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 1;
	cmd.address = SDIO_FBR_BASE(func->num);
	cmd.data = SDIO_FBR_ENABLE_CSA;
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Fail cmd 52, enable csa...\n");
		return ret;
	}

	/**
	 *      function 0 block size
	 **/
	ret = wilc_sdio_set_block_size(wilc, 0, WILC_SDIO_BLOCK_SIZE);
	if (ret) {
		dev_err(&func->dev, "Fail cmd 52, set func 0 block size...\n");
		return ret;
	}
	sdio_priv->block_size = WILC_SDIO_BLOCK_SIZE;

	/**
	 *      enable func1 IO
	 **/
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 1;
	cmd.address = SDIO_CCCR_IOEx;
	cmd.data = WILC_SDIO_CCCR_IO_EN_FUNC1;
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev,
			"Fail cmd 52, set IOE register...\n");
		return ret;
	}

	/**
	 *      make sure func 1 is up
	 **/
	cmd.read_write = 0;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = SDIO_CCCR_IORx;
	loop = 3;
	do {
		cmd.data = 0;
		ret = wilc_sdio_cmd52(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Fail cmd 52, get IOR register...\n");
			return ret;
		}
		if (cmd.data == WILC_SDIO_CCCR_IO_EN_FUNC1)
			break;
	} while (loop--);

	if (loop <= 0) {
		dev_err(&func->dev, "Fail func 1 is not ready...\n");
		return -EINVAL;
	}

	/**
	 *      func 1 is ready, set func 1 block size
	 **/
	ret = wilc_sdio_set_block_size(wilc, 1, WILC_SDIO_BLOCK_SIZE);
	if (ret) {
		dev_err(&func->dev, "Fail set func 1 block size...\n");
		return ret;
	}

	/**
	 *      func 1 interrupt enable
	 **/
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 1;
	cmd.address = SDIO_CCCR_IENx;
	cmd.data = WILC_SDIO_CCCR_IEN_MASTER | WILC_SDIO_CCCR_IEN_FUNC1;
	ret = wilc_sdio_cmd52(wilc, &cmd);
	if (ret) {
		dev_err(&func->dev, "Fail cmd 52, set IEN register...\n");
		return ret;
	}

	/**
	 *      make sure can read back chip id correctly
	 **/
	if (!resume) {
		int rev;

		ret = wilc_sdio_read_reg(wilc, WILC_CHIPID, &chipid);
		if (ret) {
			dev_err(&func->dev, "Fail cmd read chip id...\n");
			return ret;
		}
		dev_err(&func->dev, "chipid (%08x)\n", chipid);
		rev = FIELD_GET(WILC_CHIP_REV_FIELD, chipid);
		if (rev > FIELD_GET(WILC_CHIP_REV_FIELD, WILC_1000_BASE_ID_2A))
			sdio_priv->has_thrpt_enh3 = 1;
		else
			sdio_priv->has_thrpt_enh3 = 0;
		dev_info(&func->dev, "has_thrpt_enh3 = %d...\n",
			 sdio_priv->has_thrpt_enh3);
	}

	return 0;
}

static int wilc_sdio_read_size(struct wilc *wilc, u32 *size)
{
	u32 tmp;
	struct sdio_cmd52 cmd;

	/**
	 *      Read DMA count in words
	 **/
	cmd.read_write = 0;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = WILC_SDIO_INTERRUPT_DATA_SZ_REG;
	cmd.data = 0;
	wilc_sdio_cmd52(wilc, &cmd);
	tmp = cmd.data;

	cmd.address = WILC_SDIO_INTERRUPT_DATA_SZ_REG + 1;
	cmd.data = 0;
	wilc_sdio_cmd52(wilc, &cmd);
	tmp |= (cmd.data << 8);

	*size = tmp;
	return 0;
}

static int wilc_sdio_read_int(struct wilc *wilc, u32 *int_status)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	u32 tmp;
	u8 irq_flags;
	struct sdio_cmd52 cmd;

	wilc_sdio_read_size(wilc, &tmp);

	/**
	 *      Read IRQ flags
	 **/
	if (!sdio_priv->irq_gpio) {
		cmd.function = 1;
		cmd.address = WILC_SDIO_EXT_IRQ_FLAG_REG;
	} else {
		cmd.function = 0;
		cmd.address = WILC_SDIO_IRQ_FLAG_REG;
	}
	cmd.raw = 0;
	cmd.read_write = 0;
	cmd.data = 0;
	wilc_sdio_cmd52(wilc, &cmd);
	irq_flags = cmd.data;
	tmp |= FIELD_PREP(IRG_FLAGS_MASK, cmd.data);

	if (FIELD_GET(UNHANDLED_IRQ_MASK, irq_flags))
		dev_err(&func->dev, "Unexpected interrupt (1) int=%lx\n",
			FIELD_GET(UNHANDLED_IRQ_MASK, irq_flags));

	*int_status = tmp;

	return 0;
}

static int wilc_sdio_clear_int_ext(struct wilc *wilc, u32 val)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	int ret;
	int vmm_ctl;

	if (sdio_priv->has_thrpt_enh3) {
		u32 reg = 0;

		if (sdio_priv->irq_gpio)
			reg = val & (BIT(MAX_NUM_INT) - 1);

		/* select VMM table 0 */
		if (val & SEL_VMM_TBL0)
			reg |= BIT(5);
		/* select VMM table 1 */
		if (val & SEL_VMM_TBL1)
			reg |= BIT(6);
		/* enable VMM */
		if (val & EN_VMM)
			reg |= BIT(7);
		if (reg) {
			struct sdio_cmd52 cmd;

			cmd.read_write = 1;
			cmd.function = 0;
			cmd.raw = 0;
			cmd.address = WILC_SDIO_IRQ_CLEAR_FLAG_REG;
			cmd.data = reg;

			ret = wilc_sdio_cmd52(wilc, &cmd);
			if (ret) {
				dev_err(&func->dev,
					"Failed cmd52, set (%02x) data (%d) ...\n",
					cmd.address, __LINE__);
				return ret;
			}
		}
		return 0;
	}
	if (sdio_priv->irq_gpio) {
		/* has_thrpt_enh2 uses register 0xf8 to clear interrupts. */
		/*
		 * Cannot clear multiple interrupts.
		 * Must clear each interrupt individually.
		 */
		u32 flags;
		int i;

		flags = val & (BIT(MAX_NUM_INT) - 1);
		for (i = 0; i < NUM_INT_EXT && flags; i++) {
			if (flags & BIT(i)) {
				struct sdio_cmd52 cmd;

				cmd.read_write = 1;
				cmd.function = 0;
				cmd.raw = 0;
				cmd.address = WILC_SDIO_IRQ_CLEAR_FLAG_REG;
				cmd.data = BIT(i);

				ret = wilc_sdio_cmd52(wilc, &cmd);
				if (ret) {
					dev_err(&func->dev,
						"Failed cmd52, set (%02x) data (%d) ...\n",
						cmd.address, __LINE__);
					return ret;
				}
				flags &= ~BIT(i);
			}
		}

		for (i = NUM_INT_EXT; i < MAX_NUM_INT && flags; i++) {
			if (flags & BIT(i)) {
				dev_err(&func->dev,
					"Unexpected interrupt cleared %d...\n",
					i);
				flags &= ~BIT(i);
			}
		}
	}

	vmm_ctl = 0;
	/* select VMM table 0 */
	if (val & SEL_VMM_TBL0)
		vmm_ctl |= BIT(0);
	/* select VMM table 1 */
	if (val & SEL_VMM_TBL1)
		vmm_ctl |= BIT(1);
	/* enable VMM */
	if (val & EN_VMM)
		vmm_ctl |= BIT(2);

	if (vmm_ctl) {
		struct sdio_cmd52 cmd;

		cmd.read_write = 1;
		cmd.function = 0;
		cmd.raw = 0;
		cmd.address = WILC_SDIO_VMM_TBL_CTRL_REG;
		cmd.data = vmm_ctl;
		ret = wilc_sdio_cmd52(wilc, &cmd);
		if (ret) {
			dev_err(&func->dev,
				"Failed cmd52, set (%02x) data (%d) ...\n",
				cmd.address, __LINE__);
			return ret;
		}
	}
	return 0;
}

static int wilc_sdio_sync_ext(struct wilc *wilc, int nint)
{
	struct sdio_func *func = dev_to_sdio_func(wilc->dev);
	struct wilc_sdio *sdio_priv = wilc->bus_data;
	u32 reg;

	if (nint > MAX_NUM_INT) {
		dev_err(&func->dev, "Too many interrupts (%d)...\n", nint);
		return -EINVAL;
	}

	/**
	 *      Disable power sequencer
	 **/
	if (wilc_sdio_read_reg(wilc, WILC_MISC, &reg)) {
		dev_err(&func->dev, "Failed read misc reg...\n");
		return -EINVAL;
	}

	reg &= ~BIT(8);
	if (wilc_sdio_write_reg(wilc, WILC_MISC, reg)) {
		dev_err(&func->dev, "Failed write misc reg...\n");
		return -EINVAL;
	}

	if (sdio_priv->irq_gpio) {
		u32 reg;
		int ret, i;

		/**
		 *      interrupt pin mux select
		 **/
		ret = wilc_sdio_read_reg(wilc, WILC_PIN_MUX_0, &reg);
		if (ret) {
			dev_err(&func->dev, "Failed read reg (%08x)...\n",
				WILC_PIN_MUX_0);
			return ret;
		}
		reg |= BIT(8);
		ret = wilc_sdio_write_reg(wilc, WILC_PIN_MUX_0, reg);
		if (ret) {
			dev_err(&func->dev, "Failed write reg (%08x)...\n",
				WILC_PIN_MUX_0);
			return ret;
		}

		/**
		 *      interrupt enable
		 **/
		ret = wilc_sdio_read_reg(wilc, WILC_INTR_ENABLE, &reg);
		if (ret) {
			dev_err(&func->dev, "Failed read reg (%08x)...\n",
				WILC_INTR_ENABLE);
			return ret;
		}

		for (i = 0; (i < 5) && (nint > 0); i++, nint--)
			reg |= BIT((27 + i));
		ret = wilc_sdio_write_reg(wilc, WILC_INTR_ENABLE, reg);
		if (ret) {
			dev_err(&func->dev, "Failed write reg (%08x)...\n",
				WILC_INTR_ENABLE);
			return ret;
		}
		if (nint) {
			ret = wilc_sdio_read_reg(wilc, WILC_INTR2_ENABLE, &reg);
			if (ret) {
				dev_err(&func->dev,
					"Failed read reg (%08x)...\n",
					WILC_INTR2_ENABLE);
				return ret;
			}

			for (i = 0; (i < 3) && (nint > 0); i++, nint--)
				reg |= BIT(i);

			ret = wilc_sdio_write_reg(wilc, WILC_INTR2_ENABLE, reg);
			if (ret) {
				dev_err(&func->dev,
					"Failed write reg (%08x)...\n",
					WILC_INTR2_ENABLE);
				return ret;
			}
		}
	}
	return 0;
}

/* Global sdio HIF function table */
static const struct wilc_hif_func wilc_hif_sdio = {
	.hif_init = wilc_sdio_init,
	.hif_deinit = wilc_sdio_deinit,
	.hif_read_reg = wilc_sdio_read_reg,
	.hif_write_reg = wilc_sdio_write_reg,
	.hif_block_rx = wilc_sdio_read,
	.hif_block_tx = wilc_sdio_write,
	.hif_read_int = wilc_sdio_read_int,
	.hif_clear_int_ext = wilc_sdio_clear_int_ext,
	.hif_read_size = wilc_sdio_read_size,
	.hif_block_tx_ext = wilc_sdio_write,
	.hif_block_rx_ext = wilc_sdio_read,
	.hif_sync_ext = wilc_sdio_sync_ext,
	.enable_interrupt = wilc_sdio_enable_interrupt,
	.disable_interrupt = wilc_sdio_disable_interrupt,
};

static int wilc_sdio_resume(struct device *dev)
{
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct wilc *wilc = sdio_get_drvdata(func);

	dev_info(dev, "sdio resume\n");
	sdio_release_host(func);
	chip_wakeup(wilc);
	wilc_sdio_init(wilc, true);

	if (wilc->suspend_event)
		host_wakeup_notify(wilc);

	chip_allow_sleep(wilc);

	return 0;
}

static const struct of_device_id wilc_of_match[] = {
	{ .compatible = "microchip,wilc1000", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, wilc_of_match);

static const struct dev_pm_ops wilc_sdio_pm_ops = {
	.suspend = wilc_sdio_suspend,
	.resume = wilc_sdio_resume,
};

static struct sdio_driver wilc_sdio_driver = {
	.name		= SDIO_MODALIAS,
	.id_table	= wilc_sdio_ids,
	.probe		= wilc_sdio_probe,
	.remove		= wilc_sdio_remove,
	.drv = {
		.pm = &wilc_sdio_pm_ops,
		.of_match_table = wilc_of_match,
	}
};
module_driver(wilc_sdio_driver,
	      sdio_register_driver,
	      sdio_unregister_driver);
MODULE_LICENSE("GPL");

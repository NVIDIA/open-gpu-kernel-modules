// SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)
//
// This file is provided under a dual BSD/GPLv2 license.  When using or
// redistributing this file, you may do so under either license.
//
// Copyright(c) 2018 Intel Corporation. All rights reserved.
//
// Authors: Liam Girdwood <liam.r.girdwood@linux.intel.com>
//	    Ranjani Sridharan <ranjani.sridharan@linux.intel.com>
//	    Rander Wang <rander.wang@intel.com>
//          Keyon Jie <yang.jie@linux.intel.com>
//

/*
 * Hardware interface for HDA DSP code loader
 */

#include <linux/firmware.h>
#include <sound/hdaudio_ext.h>
#include <sound/hda_register.h>
#include <sound/sof.h>
#include "ext_manifest.h"
#include "../ops.h"
#include "hda.h"

#define HDA_FW_BOOT_ATTEMPTS	3
#define HDA_CL_STREAM_FORMAT 0x40

static struct hdac_ext_stream *cl_stream_prepare(struct snd_sof_dev *sdev, unsigned int format,
						 unsigned int size, struct snd_dma_buffer *dmab,
						 int direction)
{
	struct hdac_ext_stream *dsp_stream;
	struct hdac_stream *hstream;
	struct pci_dev *pci = to_pci_dev(sdev->dev);
	int ret;

	dsp_stream = hda_dsp_stream_get(sdev, direction, 0);

	if (!dsp_stream) {
		dev_err(sdev->dev, "error: no stream available\n");
		return ERR_PTR(-ENODEV);
	}
	hstream = &dsp_stream->hstream;
	hstream->substream = NULL;

	/* allocate DMA buffer */
	ret = snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV_SG, &pci->dev, size, dmab);
	if (ret < 0) {
		dev_err(sdev->dev, "error: memory alloc failed: %d\n", ret);
		goto error;
	}

	hstream->period_bytes = 0;/* initialize period_bytes */
	hstream->format_val = format;
	hstream->bufsize = size;

	if (direction == SNDRV_PCM_STREAM_CAPTURE) {
		ret = hda_dsp_iccmax_stream_hw_params(sdev, dsp_stream, dmab, NULL);
		if (ret < 0) {
			dev_err(sdev->dev, "error: iccmax stream prepare failed: %d\n", ret);
			goto error;
		}
	} else {
		ret = hda_dsp_stream_hw_params(sdev, dsp_stream, dmab, NULL);
		if (ret < 0) {
			dev_err(sdev->dev, "error: hdac prepare failed: %d\n", ret);
			goto error;
		}
		hda_dsp_stream_spib_config(sdev, dsp_stream, HDA_DSP_SPIB_ENABLE, size);
	}

	return dsp_stream;

error:
	hda_dsp_stream_put(sdev, direction, hstream->stream_tag);
	snd_dma_free_pages(dmab);
	return ERR_PTR(ret);
}

/*
 * first boot sequence has some extra steps. core 0 waits for power
 * status on core 1, so power up core 1 also momentarily, keep it in
 * reset/stall and then turn it off
 */
static int cl_dsp_init(struct snd_sof_dev *sdev, int stream_tag)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	const struct sof_intel_dsp_desc *chip = hda->desc;
	unsigned int status;
	u32 flags;
	int ret;
	int i;

	/* step 1: power up corex */
	ret = snd_sof_dsp_core_power_up(sdev, chip->host_managed_cores_mask);
	if (ret < 0) {
		if (hda->boot_iteration == HDA_FW_BOOT_ATTEMPTS)
			dev_err(sdev->dev, "error: dsp core 0/1 power up failed\n");
		goto err;
	}

	/* DSP is powered up, set all SSPs to slave mode */
	for (i = 0; i < chip->ssp_count; i++) {
		snd_sof_dsp_update_bits_unlocked(sdev, HDA_DSP_BAR,
						 chip->ssp_base_offset
						 + i * SSP_DEV_MEM_SIZE
						 + SSP_SSC1_OFFSET,
						 SSP_SET_SLAVE,
						 SSP_SET_SLAVE);
	}

	/* step 2: purge FW request */
	snd_sof_dsp_write(sdev, HDA_DSP_BAR, chip->ipc_req,
			  chip->ipc_req_mask | (HDA_DSP_IPC_PURGE_FW |
			  ((stream_tag - 1) << 9)));

	/* step 3: unset core 0 reset state & unstall/run core 0 */
	ret = hda_dsp_core_run(sdev, BIT(0));
	if (ret < 0) {
		if (hda->boot_iteration == HDA_FW_BOOT_ATTEMPTS)
			dev_err(sdev->dev,
				"error: dsp core start failed %d\n", ret);
		ret = -EIO;
		goto err;
	}

	/* step 4: wait for IPC DONE bit from ROM */
	ret = snd_sof_dsp_read_poll_timeout(sdev, HDA_DSP_BAR,
					    chip->ipc_ack, status,
					    ((status & chip->ipc_ack_mask)
						    == chip->ipc_ack_mask),
					    HDA_DSP_REG_POLL_INTERVAL_US,
					    HDA_DSP_INIT_TIMEOUT_US);

	if (ret < 0) {
		if (hda->boot_iteration == HDA_FW_BOOT_ATTEMPTS)
			dev_err(sdev->dev,
				"error: %s: timeout for HIPCIE done\n",
				__func__);
		goto err;
	}

	/* set DONE bit to clear the reply IPC message */
	snd_sof_dsp_update_bits_forced(sdev, HDA_DSP_BAR,
				       chip->ipc_ack,
				       chip->ipc_ack_mask,
				       chip->ipc_ack_mask);

	/* step 5: power down cores that are no longer needed */
	ret = snd_sof_dsp_core_power_down(sdev, chip->host_managed_cores_mask &
					  ~(chip->init_core_mask));
	if (ret < 0) {
		if (hda->boot_iteration == HDA_FW_BOOT_ATTEMPTS)
			dev_err(sdev->dev,
				"error: dsp core x power down failed\n");
		goto err;
	}

	/* step 6: enable IPC interrupts */
	hda_dsp_ipc_int_enable(sdev);

	/* step 7: wait for ROM init */
	ret = snd_sof_dsp_read_poll_timeout(sdev, HDA_DSP_BAR,
					HDA_DSP_SRAM_REG_ROM_STATUS, status,
					((status & HDA_DSP_ROM_STS_MASK)
						== HDA_DSP_ROM_INIT),
					HDA_DSP_REG_POLL_INTERVAL_US,
					chip->rom_init_timeout *
					USEC_PER_MSEC);
	if (!ret)
		return 0;

	if (hda->boot_iteration == HDA_FW_BOOT_ATTEMPTS)
		dev_err(sdev->dev,
			"error: %s: timeout HDA_DSP_SRAM_REG_ROM_STATUS read\n",
			__func__);

err:
	flags = SOF_DBG_DUMP_REGS | SOF_DBG_DUMP_PCI | SOF_DBG_DUMP_MBOX;

	/* force error log level after max boot attempts */
	if (hda->boot_iteration == HDA_FW_BOOT_ATTEMPTS)
		flags |= SOF_DBG_DUMP_FORCE_ERR_LEVEL;

	hda_dsp_dump(sdev, flags);
	snd_sof_dsp_core_power_down(sdev, chip->host_managed_cores_mask);

	return ret;
}

static int cl_trigger(struct snd_sof_dev *sdev,
		      struct hdac_ext_stream *stream, int cmd)
{
	struct hdac_stream *hstream = &stream->hstream;
	int sd_offset = SOF_STREAM_SD_OFFSET(hstream);

	/* code loader is special case that reuses stream ops */
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		snd_sof_dsp_update_bits(sdev, HDA_DSP_HDA_BAR, SOF_HDA_INTCTL,
					1 << hstream->index,
					1 << hstream->index);

		snd_sof_dsp_update_bits(sdev, HDA_DSP_HDA_BAR,
					sd_offset,
					SOF_HDA_SD_CTL_DMA_START |
					SOF_HDA_CL_DMA_SD_INT_MASK,
					SOF_HDA_SD_CTL_DMA_START |
					SOF_HDA_CL_DMA_SD_INT_MASK);

		hstream->running = true;
		return 0;
	default:
		return hda_dsp_stream_trigger(sdev, stream, cmd);
	}
}

static int cl_cleanup(struct snd_sof_dev *sdev, struct snd_dma_buffer *dmab,
		      struct hdac_ext_stream *stream)
{
	struct hdac_stream *hstream = &stream->hstream;
	int sd_offset = SOF_STREAM_SD_OFFSET(hstream);
	int ret = 0;

	if (hstream->direction == SNDRV_PCM_STREAM_PLAYBACK)
		ret = hda_dsp_stream_spib_config(sdev, stream, HDA_DSP_SPIB_DISABLE, 0);
	else
		snd_sof_dsp_update_bits(sdev, HDA_DSP_HDA_BAR, sd_offset,
					SOF_HDA_SD_CTL_DMA_START, 0);

	hda_dsp_stream_put(sdev, hstream->direction, hstream->stream_tag);
	hstream->running = 0;
	hstream->substream = NULL;

	/* reset BDL address */
	snd_sof_dsp_write(sdev, HDA_DSP_HDA_BAR,
			  sd_offset + SOF_HDA_ADSP_REG_CL_SD_BDLPL, 0);
	snd_sof_dsp_write(sdev, HDA_DSP_HDA_BAR,
			  sd_offset + SOF_HDA_ADSP_REG_CL_SD_BDLPU, 0);

	snd_sof_dsp_write(sdev, HDA_DSP_HDA_BAR, sd_offset, 0);
	snd_dma_free_pages(dmab);
	dmab->area = NULL;
	hstream->bufsize = 0;
	hstream->format_val = 0;

	return ret;
}

static int cl_copy_fw(struct snd_sof_dev *sdev, struct hdac_ext_stream *stream)
{
	unsigned int reg;
	int ret, status;

	ret = cl_trigger(sdev, stream, SNDRV_PCM_TRIGGER_START);
	if (ret < 0) {
		dev_err(sdev->dev, "error: DMA trigger start failed\n");
		return ret;
	}

	status = snd_sof_dsp_read_poll_timeout(sdev, HDA_DSP_BAR,
					HDA_DSP_SRAM_REG_ROM_STATUS, reg,
					((reg & HDA_DSP_ROM_STS_MASK)
						== HDA_DSP_ROM_FW_ENTERED),
					HDA_DSP_REG_POLL_INTERVAL_US,
					HDA_DSP_BASEFW_TIMEOUT_US);

	/*
	 * even in case of errors we still need to stop the DMAs,
	 * but we return the initial error should the DMA stop also fail
	 */

	if (status < 0) {
		dev_err(sdev->dev,
			"error: %s: timeout HDA_DSP_SRAM_REG_ROM_STATUS read\n",
			__func__);
	}

	ret = cl_trigger(sdev, stream, SNDRV_PCM_TRIGGER_STOP);
	if (ret < 0) {
		dev_err(sdev->dev, "error: DMA trigger stop failed\n");
		if (!status)
			status = ret;
	}

	return status;
}

int hda_dsp_cl_boot_firmware_iccmax(struct snd_sof_dev *sdev)
{
	struct snd_sof_pdata *plat_data = sdev->pdata;
	struct hdac_ext_stream *iccmax_stream;
	struct hdac_bus *bus = sof_to_bus(sdev);
	struct firmware stripped_firmware;
	int ret, ret1;
	u8 original_gb;

	/* save the original LTRP guardband value */
	original_gb = snd_hdac_chip_readb(bus, VS_LTRP) & HDA_VS_INTEL_LTRP_GB_MASK;

	if (plat_data->fw->size <= plat_data->fw_offset) {
		dev_err(sdev->dev, "error: firmware size must be greater than firmware offset\n");
		return -EINVAL;
	}

	stripped_firmware.size = plat_data->fw->size - plat_data->fw_offset;

	/* prepare capture stream for ICCMAX */
	iccmax_stream = cl_stream_prepare(sdev, HDA_CL_STREAM_FORMAT, stripped_firmware.size,
					  &sdev->dmab_bdl, SNDRV_PCM_STREAM_CAPTURE);
	if (IS_ERR(iccmax_stream)) {
		dev_err(sdev->dev, "error: dma prepare for ICCMAX stream failed\n");
		return PTR_ERR(iccmax_stream);
	}

	ret = hda_dsp_cl_boot_firmware(sdev);

	/*
	 * Perform iccmax stream cleanup. This should be done even if firmware loading fails.
	 * If the cleanup also fails, we return the initial error
	 */
	ret1 = cl_cleanup(sdev, &sdev->dmab_bdl, iccmax_stream);
	if (ret1 < 0) {
		dev_err(sdev->dev, "error: ICCMAX stream cleanup failed\n");

		/* set return value to indicate cleanup failure */
		if (!ret)
			ret = ret1;
	}

	/* restore the original guardband value after FW boot */
	snd_hdac_chip_updateb(bus, VS_LTRP, HDA_VS_INTEL_LTRP_GB_MASK, original_gb);

	return ret;
}

int hda_dsp_cl_boot_firmware(struct snd_sof_dev *sdev)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	struct snd_sof_pdata *plat_data = sdev->pdata;
	const struct sof_dev_desc *desc = plat_data->desc;
	const struct sof_intel_dsp_desc *chip_info;
	struct hdac_ext_stream *stream;
	struct firmware stripped_firmware;
	int ret, ret1, i;

	chip_info = desc->chip_info;

	if (plat_data->fw->size <= plat_data->fw_offset) {
		dev_err(sdev->dev, "error: firmware size must be greater than firmware offset\n");
		return -EINVAL;
	}

	stripped_firmware.data = plat_data->fw->data + plat_data->fw_offset;
	stripped_firmware.size = plat_data->fw->size - plat_data->fw_offset;

	/* init for booting wait */
	init_waitqueue_head(&sdev->boot_wait);

	/* prepare DMA for code loader stream */
	stream = cl_stream_prepare(sdev, HDA_CL_STREAM_FORMAT, stripped_firmware.size,
				   &sdev->dmab, SNDRV_PCM_STREAM_PLAYBACK);
	if (IS_ERR(stream)) {
		dev_err(sdev->dev, "error: dma prepare for fw loading failed\n");
		return PTR_ERR(stream);
	}

	memcpy(sdev->dmab.area, stripped_firmware.data,
	       stripped_firmware.size);

	/* try ROM init a few times before giving up */
	for (i = 0; i < HDA_FW_BOOT_ATTEMPTS; i++) {
		dev_dbg(sdev->dev,
			"Attempting iteration %d of Core En/ROM load...\n", i);

		hda->boot_iteration = i + 1;
		ret = cl_dsp_init(sdev, stream->hstream.stream_tag);

		/* don't retry anymore if successful */
		if (!ret)
			break;
	}

	if (i == HDA_FW_BOOT_ATTEMPTS) {
		dev_err(sdev->dev, "error: dsp init failed after %d attempts with err: %d\n",
			i, ret);
		dev_err(sdev->dev, "ROM error=0x%x: FW status=0x%x\n",
			snd_sof_dsp_read(sdev, HDA_DSP_BAR,
					 HDA_DSP_SRAM_REG_ROM_ERROR),
			snd_sof_dsp_read(sdev, HDA_DSP_BAR,
					 HDA_DSP_SRAM_REG_ROM_STATUS));
		goto cleanup;
	}

	/*
	 * When a SoundWire link is in clock stop state, a Slave
	 * device may trigger in-band wakes for events such as jack
	 * insertion or acoustic event detection. This event will lead
	 * to a WAKEEN interrupt, handled by the PCI device and routed
	 * to PME if the PCI device is in D3. The resume function in
	 * audio PCI driver will be invoked by ACPI for PME event and
	 * initialize the device and process WAKEEN interrupt.
	 *
	 * The WAKEEN interrupt should be processed ASAP to prevent an
	 * interrupt flood, otherwise other interrupts, such IPC,
	 * cannot work normally.  The WAKEEN is handled after the ROM
	 * is initialized successfully, which ensures power rails are
	 * enabled before accessing the SoundWire SHIM registers
	 */
	if (!sdev->first_boot)
		hda_sdw_process_wakeen(sdev);

	/*
	 * at this point DSP ROM has been initialized and
	 * should be ready for code loading and firmware boot
	 */
	ret = cl_copy_fw(sdev, stream);
	if (!ret) {
		dev_dbg(sdev->dev, "Firmware download successful, booting...\n");
	} else {
		hda_dsp_dump(sdev, SOF_DBG_DUMP_REGS | SOF_DBG_DUMP_PCI | SOF_DBG_DUMP_MBOX |
			     SOF_DBG_DUMP_FORCE_ERR_LEVEL);
		dev_err(sdev->dev, "error: load fw failed ret: %d\n", ret);
	}

cleanup:
	/*
	 * Perform codeloader stream cleanup.
	 * This should be done even if firmware loading fails.
	 * If the cleanup also fails, we return the initial error
	 */
	ret1 = cl_cleanup(sdev, &sdev->dmab, stream);
	if (ret1 < 0) {
		dev_err(sdev->dev, "error: Code loader DSP cleanup failed\n");

		/* set return value to indicate cleanup failure */
		if (!ret)
			ret = ret1;
	}

	/*
	 * return primary core id if both fw copy
	 * and stream clean up are successful
	 */
	if (!ret)
		return chip_info->init_core_mask;

	/* disable DSP */
	snd_sof_dsp_update_bits(sdev, HDA_DSP_PP_BAR,
				SOF_HDA_REG_PP_PPCTL,
				SOF_HDA_PPCTL_GPROCEN, 0);
	return ret;
}

/* pre fw run operations */
int hda_dsp_pre_fw_run(struct snd_sof_dev *sdev)
{
	/* disable clock gating and power gating */
	return hda_dsp_ctrl_clock_power_gating(sdev, false);
}

/* post fw run operations */
int hda_dsp_post_fw_run(struct snd_sof_dev *sdev)
{
	int ret;

	if (sdev->first_boot) {
		ret = hda_sdw_startup(sdev);
		if (ret < 0) {
			dev_err(sdev->dev,
				"error: could not startup SoundWire links\n");
			return ret;
		}
	}

	hda_sdw_int_enable(sdev, true);

	/* re-enable clock gating and power gating */
	return hda_dsp_ctrl_clock_power_gating(sdev, true);
}

/*
 * post fw run operations for ICL,
 * Core 3 will be powered up and in stall when HPRO is enabled
 */
int hda_dsp_post_fw_run_icl(struct snd_sof_dev *sdev)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	int ret;

	if (sdev->first_boot) {
		ret = hda_sdw_startup(sdev);
		if (ret < 0) {
			dev_err(sdev->dev,
				"error: could not startup SoundWire links\n");
			return ret;
		}
	}

	hda_sdw_int_enable(sdev, true);

	/*
	 * The recommended HW programming sequence for ICL is to
	 * power up core 3 and keep it in stall if HPRO is enabled.
	 * Major difference between ICL and TGL, on ICL core 3 is managed by
	 * the host whereas on TGL it is handled by the firmware.
	 */
	if (!hda->clk_config_lpro) {
		ret = snd_sof_dsp_core_power_up(sdev, BIT(3));
		if (ret < 0) {
			dev_err(sdev->dev, "error: dsp core power up failed on core 3\n");
			return ret;
		}

		snd_sof_dsp_stall(sdev, BIT(3));
	}

	/* re-enable clock gating and power gating */
	return hda_dsp_ctrl_clock_power_gating(sdev, true);
}

int hda_dsp_ext_man_get_cavs_config_data(struct snd_sof_dev *sdev,
					 const struct sof_ext_man_elem_header *hdr)
{
	const struct sof_ext_man_cavs_config_data *config_data =
		container_of(hdr, struct sof_ext_man_cavs_config_data, hdr);
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	int i, elem_num;

	/* calculate total number of config data elements */
	elem_num = (hdr->size - sizeof(struct sof_ext_man_elem_header))
		   / sizeof(struct sof_config_elem);
	if (elem_num <= 0) {
		dev_err(sdev->dev, "cavs config data is inconsistent: %d\n", elem_num);
		return -EINVAL;
	}

	for (i = 0; i < elem_num; i++)
		switch (config_data->elems[i].token) {
		case SOF_EXT_MAN_CAVS_CONFIG_EMPTY:
			/* skip empty token */
			break;
		case SOF_EXT_MAN_CAVS_CONFIG_CAVS_LPRO:
			hda->clk_config_lpro = config_data->elems[i].value;
			dev_dbg(sdev->dev, "FW clock config: %s\n",
				hda->clk_config_lpro ? "LPRO" : "HPRO");
			break;
		case SOF_EXT_MAN_CAVS_CONFIG_OUTBOX_SIZE:
		case SOF_EXT_MAN_CAVS_CONFIG_INBOX_SIZE:
			/* These elements are defined but not being used yet. No warn is required */
			break;
		default:
			dev_info(sdev->dev, "unsupported token type: %d\n",
				 config_data->elems[i].token);
		}

	return 0;
}

int hda_dsp_core_stall_icl(struct snd_sof_dev *sdev, unsigned int core_mask)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	const struct sof_intel_dsp_desc *chip = hda->desc;

	/* make sure core_mask in host managed cores */
	core_mask &= chip->host_managed_cores_mask;
	if (!core_mask) {
		dev_err(sdev->dev, "error: core_mask is not in host managed cores\n");
		return -EINVAL;
	}

	/* stall core */
	snd_sof_dsp_update_bits_unlocked(sdev, HDA_DSP_BAR,
					 HDA_DSP_REG_ADSPCS,
					 HDA_DSP_ADSPCS_CSTALL_MASK(core_mask),
					 HDA_DSP_ADSPCS_CSTALL_MASK(core_mask));

	return 0;
}

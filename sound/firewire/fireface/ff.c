// SPDX-License-Identifier: GPL-2.0-only
/*
 * ff.c - a part of driver for RME Fireface series
 *
 * Copyright (c) 2015-2017 Takashi Sakamoto
 */

#include "ff.h"

#define OUI_RME	0x000a35

MODULE_DESCRIPTION("RME Fireface series Driver");
MODULE_AUTHOR("Takashi Sakamoto <o-takashi@sakamocchi.jp>");
MODULE_LICENSE("GPL v2");

static void name_card(struct snd_ff *ff)
{
	struct fw_device *fw_dev = fw_parent_device(ff->unit);
	const char *const names[] = {
		[SND_FF_UNIT_VERSION_FF800]	= "Fireface800",
		[SND_FF_UNIT_VERSION_FF400]	= "Fireface400",
		[SND_FF_UNIT_VERSION_UFX]	= "FirefaceUFX",
		[SND_FF_UNIT_VERSION_UCX]	= "FirefaceUCX",
		[SND_FF_UNIT_VERSION_802]	= "Fireface802",
	};
	const char *name;

	name = names[ff->unit_version];

	strcpy(ff->card->driver, "Fireface");
	strcpy(ff->card->shortname, name);
	strcpy(ff->card->mixername, name);
	snprintf(ff->card->longname, sizeof(ff->card->longname),
		 "RME %s, GUID %08x%08x at %s, S%d", name,
		 fw_dev->config_rom[3], fw_dev->config_rom[4],
		 dev_name(&ff->unit->device), 100 << fw_dev->max_speed);
}

static void ff_card_free(struct snd_card *card)
{
	struct snd_ff *ff = card->private_data;

	snd_ff_stream_destroy_duplex(ff);
	snd_ff_transaction_unregister(ff);
}

static void do_registration(struct work_struct *work)
{
	struct snd_ff *ff = container_of(work, struct snd_ff, dwork.work);
	int err;

	if (ff->registered)
		return;

	err = snd_card_new(&ff->unit->device, -1, NULL, THIS_MODULE, 0,
			   &ff->card);
	if (err < 0)
		return;
	ff->card->private_free = ff_card_free;
	ff->card->private_data = ff;

	err = snd_ff_transaction_register(ff);
	if (err < 0)
		goto error;

	name_card(ff);

	err = snd_ff_stream_init_duplex(ff);
	if (err < 0)
		goto error;

	snd_ff_proc_init(ff);

	err = snd_ff_create_midi_devices(ff);
	if (err < 0)
		goto error;

	err = snd_ff_create_pcm_devices(ff);
	if (err < 0)
		goto error;

	err = snd_ff_create_hwdep_devices(ff);
	if (err < 0)
		goto error;

	err = snd_card_register(ff->card);
	if (err < 0)
		goto error;

	ff->registered = true;

	return;
error:
	snd_card_free(ff->card);
	dev_info(&ff->unit->device,
		 "Sound card registration failed: %d\n", err);
}

static int snd_ff_probe(struct fw_unit *unit,
			   const struct ieee1394_device_id *entry)
{
	struct snd_ff *ff;

	ff = devm_kzalloc(&unit->device, sizeof(struct snd_ff), GFP_KERNEL);
	if (!ff)
		return -ENOMEM;
	ff->unit = fw_unit_get(unit);
	dev_set_drvdata(&unit->device, ff);

	mutex_init(&ff->mutex);
	spin_lock_init(&ff->lock);
	init_waitqueue_head(&ff->hwdep_wait);

	ff->unit_version = entry->version;
	ff->spec = (const struct snd_ff_spec *)entry->driver_data;

	/* Register this sound card later. */
	INIT_DEFERRABLE_WORK(&ff->dwork, do_registration);
	snd_fw_schedule_registration(unit, &ff->dwork);

	return 0;
}

static void snd_ff_update(struct fw_unit *unit)
{
	struct snd_ff *ff = dev_get_drvdata(&unit->device);

	/* Postpone a workqueue for deferred registration. */
	if (!ff->registered)
		snd_fw_schedule_registration(unit, &ff->dwork);

	snd_ff_transaction_reregister(ff);

	if (ff->registered)
		snd_ff_stream_update_duplex(ff);
}

static void snd_ff_remove(struct fw_unit *unit)
{
	struct snd_ff *ff = dev_get_drvdata(&unit->device);

	/*
	 * Confirm to stop the work for registration before the sound card is
	 * going to be released. The work is not scheduled again because bus
	 * reset handler is not called anymore.
	 */
	cancel_work_sync(&ff->dwork.work);

	if (ff->registered) {
		// Block till all of ALSA character devices are released.
		snd_card_free(ff->card);
	}

	mutex_destroy(&ff->mutex);
	fw_unit_put(ff->unit);
}

static const struct snd_ff_spec spec_ff800 = {
	.pcm_capture_channels = {28, 20, 12},
	.pcm_playback_channels = {28, 20, 12},
	.midi_in_ports = 1,
	.midi_out_ports = 1,
	.protocol = &snd_ff_protocol_ff800,
	.midi_high_addr = 0x000200000320ull,
	.midi_addr_range = 12,
	.midi_rx_addrs = {0x000080180000ull, 0},
};

static const struct snd_ff_spec spec_ff400 = {
	.pcm_capture_channels = {18, 14, 10},
	.pcm_playback_channels = {18, 14, 10},
	.midi_in_ports = 2,
	.midi_out_ports = 2,
	.protocol = &snd_ff_protocol_ff400,
	.midi_high_addr = 0x0000801003f4ull,
	.midi_addr_range = SND_FF_MAXIMIM_MIDI_QUADS * 4,
	.midi_rx_addrs = {0x000080180000ull, 0x000080190000ull},
};

static const struct snd_ff_spec spec_ucx = {
	.pcm_capture_channels = {18, 14, 12},
	.pcm_playback_channels = {18, 14, 12},
	.midi_in_ports = 2,
	.midi_out_ports = 2,
	.protocol = &snd_ff_protocol_latter,
	.midi_high_addr = 0xffff00000034ull,
	.midi_addr_range = 0x80,
	.midi_rx_addrs = {0xffff00000030ull, 0xffff00000030ull},
};

static const struct snd_ff_spec spec_ufx_802 = {
	.pcm_capture_channels = {30, 22, 14},
	.pcm_playback_channels = {30, 22, 14},
	.midi_in_ports = 1,
	.midi_out_ports = 1,
	.protocol = &snd_ff_protocol_latter,
	.midi_high_addr = 0xffff00000034ull,
	.midi_addr_range = 0x80,
	.midi_rx_addrs = {0xffff00000030ull, 0xffff00000030ull},
};

static const struct ieee1394_device_id snd_ff_id_table[] = {
	/* Fireface 800 */
	{
		.match_flags	= IEEE1394_MATCH_VENDOR_ID |
				  IEEE1394_MATCH_SPECIFIER_ID |
				  IEEE1394_MATCH_VERSION |
				  IEEE1394_MATCH_MODEL_ID,
		.vendor_id	= OUI_RME,
		.specifier_id	= OUI_RME,
		.version	= SND_FF_UNIT_VERSION_FF800,
		.model_id	= 0x101800,
		.driver_data	= (kernel_ulong_t)&spec_ff800,
	},
	/* Fireface 400 */
	{
		.match_flags	= IEEE1394_MATCH_VENDOR_ID |
				  IEEE1394_MATCH_SPECIFIER_ID |
				  IEEE1394_MATCH_VERSION |
				  IEEE1394_MATCH_MODEL_ID,
		.vendor_id	= OUI_RME,
		.specifier_id	= OUI_RME,
		.version	= SND_FF_UNIT_VERSION_FF400,
		.model_id	= 0x101800,
		.driver_data	= (kernel_ulong_t)&spec_ff400,
	},
	// Fireface UFX.
	{
		.match_flags	= IEEE1394_MATCH_VENDOR_ID |
				  IEEE1394_MATCH_SPECIFIER_ID |
				  IEEE1394_MATCH_VERSION |
				  IEEE1394_MATCH_MODEL_ID,
		.vendor_id	= OUI_RME,
		.specifier_id	= OUI_RME,
		.version	= SND_FF_UNIT_VERSION_UFX,
		.model_id	= 0x101800,
		.driver_data	= (kernel_ulong_t)&spec_ufx_802,
	},
	// Fireface UCX.
	{
		.match_flags	= IEEE1394_MATCH_VENDOR_ID |
				  IEEE1394_MATCH_SPECIFIER_ID |
				  IEEE1394_MATCH_VERSION |
				  IEEE1394_MATCH_MODEL_ID,
		.vendor_id	= OUI_RME,
		.specifier_id	= OUI_RME,
		.version	= SND_FF_UNIT_VERSION_UCX,
		.model_id	= 0x101800,
		.driver_data	= (kernel_ulong_t)&spec_ucx,
	},
	// Fireface 802.
	{
		.match_flags	= IEEE1394_MATCH_VENDOR_ID |
				  IEEE1394_MATCH_SPECIFIER_ID |
				  IEEE1394_MATCH_VERSION |
				  IEEE1394_MATCH_MODEL_ID,
		.vendor_id	= OUI_RME,
		.specifier_id	= OUI_RME,
		.version	= SND_FF_UNIT_VERSION_802,
		.model_id	= 0x101800,
		.driver_data	= (kernel_ulong_t)&spec_ufx_802,
	},
	{}
};
MODULE_DEVICE_TABLE(ieee1394, snd_ff_id_table);

static struct fw_driver ff_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= KBUILD_MODNAME,
		.bus	= &fw_bus_type,
	},
	.probe    = snd_ff_probe,
	.update   = snd_ff_update,
	.remove   = snd_ff_remove,
	.id_table = snd_ff_id_table,
};

static int __init snd_ff_init(void)
{
	return driver_register(&ff_driver.driver);
}

static void __exit snd_ff_exit(void)
{
	driver_unregister(&ff_driver.driver);
}

module_init(snd_ff_init);
module_exit(snd_ff_exit);

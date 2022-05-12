// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Routines for control of the AK4117 via 4-wire serial interface
 *  IEC958 (S/PDIF) receiver by Asahi Kasei
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 */

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/ak4117.h>
#include <sound/asoundef.h>

MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_DESCRIPTION("AK4117 IEC958 (S/PDIF) receiver by Asahi Kasei");
MODULE_LICENSE("GPL");

#define AK4117_ADDR			0x00 /* fixed address */

static void snd_ak4117_timer(struct timer_list *t);

static void reg_write(struct ak4117 *ak4117, unsigned char reg, unsigned char val)
{
	ak4117->write(ak4117->private_data, reg, val);
	if (reg < sizeof(ak4117->regmap))
		ak4117->regmap[reg] = val;
}

static inline unsigned char reg_read(struct ak4117 *ak4117, unsigned char reg)
{
	return ak4117->read(ak4117->private_data, reg);
}

#if 0
static void reg_dump(struct ak4117 *ak4117)
{
	int i;

	printk(KERN_DEBUG "AK4117 REG DUMP:\n");
	for (i = 0; i < 0x1b; i++)
		printk(KERN_DEBUG "reg[%02x] = %02x (%02x)\n", i, reg_read(ak4117, i), i < sizeof(ak4117->regmap) ? ak4117->regmap[i] : 0);
}
#endif

static void snd_ak4117_free(struct ak4117 *chip)
{
	del_timer_sync(&chip->timer);
	kfree(chip);
}

static int snd_ak4117_dev_free(struct snd_device *device)
{
	struct ak4117 *chip = device->device_data;
	snd_ak4117_free(chip);
	return 0;
}

int snd_ak4117_create(struct snd_card *card, ak4117_read_t *read, ak4117_write_t *write,
		      const unsigned char pgm[5], void *private_data, struct ak4117 **r_ak4117)
{
	struct ak4117 *chip;
	int err = 0;
	unsigned char reg;
	static const struct snd_device_ops ops = {
		.dev_free =     snd_ak4117_dev_free,
	};

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;
	spin_lock_init(&chip->lock);
	chip->card = card;
	chip->read = read;
	chip->write = write;
	chip->private_data = private_data;
	timer_setup(&chip->timer, snd_ak4117_timer, 0);

	for (reg = 0; reg < 5; reg++)
		chip->regmap[reg] = pgm[reg];
	snd_ak4117_reinit(chip);

	chip->rcs0 = reg_read(chip, AK4117_REG_RCS0) & ~(AK4117_QINT | AK4117_CINT | AK4117_STC);
	chip->rcs1 = reg_read(chip, AK4117_REG_RCS1);
	chip->rcs2 = reg_read(chip, AK4117_REG_RCS2);

	if ((err = snd_device_new(card, SNDRV_DEV_CODEC, chip, &ops)) < 0)
		goto __fail;

	if (r_ak4117)
		*r_ak4117 = chip;
	return 0;

      __fail:
	snd_ak4117_free(chip);
	return err;
}

void snd_ak4117_reg_write(struct ak4117 *chip, unsigned char reg, unsigned char mask, unsigned char val)
{
	if (reg >= 5)
		return;
	reg_write(chip, reg, (chip->regmap[reg] & ~mask) | val);
}

void snd_ak4117_reinit(struct ak4117 *chip)
{
	unsigned char old = chip->regmap[AK4117_REG_PWRDN], reg;

	del_timer(&chip->timer);
	chip->init = 1;
	/* bring the chip to reset state and powerdown state */
	reg_write(chip, AK4117_REG_PWRDN, 0);
	udelay(200);
	/* release reset, but leave powerdown */
	reg_write(chip, AK4117_REG_PWRDN, (old | AK4117_RST) & ~AK4117_PWN);
	udelay(200);
	for (reg = 1; reg < 5; reg++)
		reg_write(chip, reg, chip->regmap[reg]);
	/* release powerdown, everything is initialized now */
	reg_write(chip, AK4117_REG_PWRDN, old | AK4117_RST | AK4117_PWN);
	chip->init = 0;
	mod_timer(&chip->timer, 1 + jiffies);
}

static unsigned int external_rate(unsigned char rcs1)
{
	switch (rcs1 & (AK4117_FS0|AK4117_FS1|AK4117_FS2|AK4117_FS3)) {
	case AK4117_FS_32000HZ: return 32000;
	case AK4117_FS_44100HZ: return 44100;
	case AK4117_FS_48000HZ: return 48000;
	case AK4117_FS_88200HZ: return 88200;
	case AK4117_FS_96000HZ: return 96000;
	case AK4117_FS_176400HZ: return 176400;
	case AK4117_FS_192000HZ: return 192000;
	default:		return 0;
	}
}

static int snd_ak4117_in_error_info(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = LONG_MAX;
	return 0;
}

static int snd_ak4117_in_error_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);

	spin_lock_irq(&chip->lock);
	ucontrol->value.integer.value[0] =
		       chip->errors[kcontrol->private_value];
	chip->errors[kcontrol->private_value] = 0;
	spin_unlock_irq(&chip->lock);
	return 0;
}

#define snd_ak4117_in_bit_info		snd_ctl_boolean_mono_info

static int snd_ak4117_in_bit_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);
	unsigned char reg = kcontrol->private_value & 0xff;
	unsigned char bit = (kcontrol->private_value >> 8) & 0xff;
	unsigned char inv = (kcontrol->private_value >> 31) & 1;

	ucontrol->value.integer.value[0] = ((reg_read(chip, reg) & (1 << bit)) ? 1 : 0) ^ inv;
	return 0;
}

static int snd_ak4117_rx_info(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int snd_ak4117_rx_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);

	ucontrol->value.integer.value[0] = (chip->regmap[AK4117_REG_IO] & AK4117_IPS) ? 1 : 0;
	return 0;
}

static int snd_ak4117_rx_put(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);
	int change;
	u8 old_val;
	
	spin_lock_irq(&chip->lock);
	old_val = chip->regmap[AK4117_REG_IO];
	change = !!ucontrol->value.integer.value[0] != ((old_val & AK4117_IPS) ? 1 : 0);
	if (change)
		reg_write(chip, AK4117_REG_IO, (old_val & ~AK4117_IPS) | (ucontrol->value.integer.value[0] ? AK4117_IPS : 0));
	spin_unlock_irq(&chip->lock);
	return change;
}

static int snd_ak4117_rate_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 192000;
	return 0;
}

static int snd_ak4117_rate_get(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);

	ucontrol->value.integer.value[0] = external_rate(reg_read(chip, AK4117_REG_RCS1));
	return 0;
}

static int snd_ak4117_spdif_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_IEC958;
	uinfo->count = 1;
	return 0;
}

static int snd_ak4117_spdif_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);
	unsigned i;

	for (i = 0; i < AK4117_REG_RXCSB_SIZE; i++)
		ucontrol->value.iec958.status[i] = reg_read(chip, AK4117_REG_RXCSB0 + i);
	return 0;
}

static int snd_ak4117_spdif_mask_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_IEC958;
	uinfo->count = 1;
	return 0;
}

static int snd_ak4117_spdif_mask_get(struct snd_kcontrol *kcontrol,
				      struct snd_ctl_elem_value *ucontrol)
{
	memset(ucontrol->value.iec958.status, 0xff, AK4117_REG_RXCSB_SIZE);
	return 0;
}

static int snd_ak4117_spdif_pinfo(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 0xffff;
	uinfo->count = 4;
	return 0;
}

static int snd_ak4117_spdif_pget(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);
	unsigned short tmp;

	ucontrol->value.integer.value[0] = 0xf8f2;
	ucontrol->value.integer.value[1] = 0x4e1f;
	tmp = reg_read(chip, AK4117_REG_Pc0) | (reg_read(chip, AK4117_REG_Pc1) << 8);
	ucontrol->value.integer.value[2] = tmp;
	tmp = reg_read(chip, AK4117_REG_Pd0) | (reg_read(chip, AK4117_REG_Pd1) << 8);
	ucontrol->value.integer.value[3] = tmp;
	return 0;
}

static int snd_ak4117_spdif_qinfo(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BYTES;
	uinfo->count = AK4117_REG_QSUB_SIZE;
	return 0;
}

static int snd_ak4117_spdif_qget(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ak4117 *chip = snd_kcontrol_chip(kcontrol);
	unsigned i;

	for (i = 0; i < AK4117_REG_QSUB_SIZE; i++)
		ucontrol->value.bytes.data[i] = reg_read(chip, AK4117_REG_QSUB_ADDR + i);
	return 0;
}

/* Don't forget to change AK4117_CONTROLS define!!! */
static const struct snd_kcontrol_new snd_ak4117_iec958_controls[] = {
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 Parity Errors",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_error_info,
	.get =		snd_ak4117_in_error_get,
	.private_value = AK4117_PARITY_ERRORS,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 V-Bit Errors",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_error_info,
	.get =		snd_ak4117_in_error_get,
	.private_value = AK4117_V_BIT_ERRORS,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 C-CRC Errors",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_error_info,
	.get =		snd_ak4117_in_error_get,
	.private_value = AK4117_CCRC_ERRORS,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 Q-CRC Errors",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_error_info,
	.get =		snd_ak4117_in_error_get,
	.private_value = AK4117_QCRC_ERRORS,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 External Rate",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_rate_info,
	.get =		snd_ak4117_rate_get,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		SNDRV_CTL_NAME_IEC958("",CAPTURE,MASK),
	.access =	SNDRV_CTL_ELEM_ACCESS_READ,
	.info =		snd_ak4117_spdif_mask_info,
	.get =		snd_ak4117_spdif_mask_get,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		SNDRV_CTL_NAME_IEC958("",CAPTURE,DEFAULT),
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_spdif_info,
	.get =		snd_ak4117_spdif_get,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 Preamble Capture Default",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_spdif_pinfo,
	.get =		snd_ak4117_spdif_pget,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 Q-subcode Capture Default",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_spdif_qinfo,
	.get =		snd_ak4117_spdif_qget,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 Audio",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_bit_info,
	.get =		snd_ak4117_in_bit_get,
	.private_value = (1<<31) | (3<<8) | AK4117_REG_RCS0,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 Non-PCM Bitstream",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_bit_info,
	.get =		snd_ak4117_in_bit_get,
	.private_value = (5<<8) | AK4117_REG_RCS1,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"IEC958 DTS Bitstream",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	.info =		snd_ak4117_in_bit_info,
	.get =		snd_ak4117_in_bit_get,
	.private_value = (6<<8) | AK4117_REG_RCS1,
},
{
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		"AK4117 Input Select",
	.access =	SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_WRITE,
	.info =		snd_ak4117_rx_info,
	.get =		snd_ak4117_rx_get,
	.put =		snd_ak4117_rx_put,
}
};

int snd_ak4117_build(struct ak4117 *ak4117, struct snd_pcm_substream *cap_substream)
{
	struct snd_kcontrol *kctl;
	unsigned int idx;
	int err;

	if (snd_BUG_ON(!cap_substream))
		return -EINVAL;
	ak4117->substream = cap_substream;
	for (idx = 0; idx < AK4117_CONTROLS; idx++) {
		kctl = snd_ctl_new1(&snd_ak4117_iec958_controls[idx], ak4117);
		if (kctl == NULL)
			return -ENOMEM;
		kctl->id.device = cap_substream->pcm->device;
		kctl->id.subdevice = cap_substream->number;
		err = snd_ctl_add(ak4117->card, kctl);
		if (err < 0)
			return err;
		ak4117->kctls[idx] = kctl;
	}
	return 0;
}

int snd_ak4117_external_rate(struct ak4117 *ak4117)
{
	unsigned char rcs1;

	rcs1 = reg_read(ak4117, AK4117_REG_RCS1);
	return external_rate(rcs1);
}

int snd_ak4117_check_rate_and_errors(struct ak4117 *ak4117, unsigned int flags)
{
	struct snd_pcm_runtime *runtime = ak4117->substream ? ak4117->substream->runtime : NULL;
	unsigned long _flags;
	int res = 0;
	unsigned char rcs0, rcs1, rcs2;
	unsigned char c0, c1;

	rcs1 = reg_read(ak4117, AK4117_REG_RCS1);
	if (flags & AK4117_CHECK_NO_STAT)
		goto __rate;
	rcs0 = reg_read(ak4117, AK4117_REG_RCS0);
	rcs2 = reg_read(ak4117, AK4117_REG_RCS2);
	// printk(KERN_DEBUG "AK IRQ: rcs0 = 0x%x, rcs1 = 0x%x, rcs2 = 0x%x\n", rcs0, rcs1, rcs2);
	spin_lock_irqsave(&ak4117->lock, _flags);
	if (rcs0 & AK4117_PAR)
		ak4117->errors[AK4117_PARITY_ERRORS]++;
	if (rcs0 & AK4117_V)
		ak4117->errors[AK4117_V_BIT_ERRORS]++;
	if (rcs2 & AK4117_CCRC)
		ak4117->errors[AK4117_CCRC_ERRORS]++;
	if (rcs2 & AK4117_QCRC)
		ak4117->errors[AK4117_QCRC_ERRORS]++;
	c0 = (ak4117->rcs0 & (AK4117_QINT | AK4117_CINT | AK4117_STC | AK4117_AUDION | AK4117_AUTO | AK4117_UNLCK)) ^
                     (rcs0 & (AK4117_QINT | AK4117_CINT | AK4117_STC | AK4117_AUDION | AK4117_AUTO | AK4117_UNLCK));
	c1 = (ak4117->rcs1 & (AK4117_DTSCD | AK4117_NPCM | AK4117_PEM | 0x0f)) ^
	             (rcs1 & (AK4117_DTSCD | AK4117_NPCM | AK4117_PEM | 0x0f));
	ak4117->rcs0 = rcs0 & ~(AK4117_QINT | AK4117_CINT | AK4117_STC);
	ak4117->rcs1 = rcs1;
	ak4117->rcs2 = rcs2;
	spin_unlock_irqrestore(&ak4117->lock, _flags);

	if (rcs0 & AK4117_PAR)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[0]->id);
	if (rcs0 & AK4117_V)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[1]->id);
	if (rcs2 & AK4117_CCRC)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[2]->id);
	if (rcs2 & AK4117_QCRC)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[3]->id);

	/* rate change */
	if (c1 & 0x0f)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[4]->id);

	if ((c1 & AK4117_PEM) | (c0 & AK4117_CINT))
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[6]->id);
	if (c0 & AK4117_QINT)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[8]->id);

	if (c0 & AK4117_AUDION)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[9]->id);
	if (c1 & AK4117_NPCM)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[10]->id);
	if (c1 & AK4117_DTSCD)
		snd_ctl_notify(ak4117->card, SNDRV_CTL_EVENT_MASK_VALUE, &ak4117->kctls[11]->id);
		
	if (ak4117->change_callback && (c0 | c1) != 0)
		ak4117->change_callback(ak4117, c0, c1);

      __rate:
	/* compare rate */
	res = external_rate(rcs1);
	if (!(flags & AK4117_CHECK_NO_RATE) && runtime && runtime->rate != res) {
		snd_pcm_stream_lock_irqsave(ak4117->substream, _flags);
		if (snd_pcm_running(ak4117->substream)) {
			// printk(KERN_DEBUG "rate changed (%i <- %i)\n", runtime->rate, res);
			snd_pcm_stop(ak4117->substream, SNDRV_PCM_STATE_DRAINING);
			wake_up(&runtime->sleep);
			res = 1;
		}
		snd_pcm_stream_unlock_irqrestore(ak4117->substream, _flags);
	}
	return res;
}

static void snd_ak4117_timer(struct timer_list *t)
{
	struct ak4117 *chip = from_timer(chip, t, timer);

	if (chip->init)
		return;
	snd_ak4117_check_rate_and_errors(chip, 0);
	mod_timer(&chip->timer, 1 + jiffies);
}

EXPORT_SYMBOL(snd_ak4117_create);
EXPORT_SYMBOL(snd_ak4117_reg_write);
EXPORT_SYMBOL(snd_ak4117_reinit);
EXPORT_SYMBOL(snd_ak4117_build);
EXPORT_SYMBOL(snd_ak4117_external_rate);
EXPORT_SYMBOL(snd_ak4117_check_rate_and_errors);

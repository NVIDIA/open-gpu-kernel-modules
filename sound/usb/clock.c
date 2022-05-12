// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   Clock domain and sample rate management functions
 */

#include <linux/bitops.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/usb.h>
#include <linux/usb/audio.h>
#include <linux/usb/audio-v2.h>
#include <linux/usb/audio-v3.h>

#include <sound/core.h>
#include <sound/info.h>
#include <sound/pcm.h>

#include "usbaudio.h"
#include "card.h"
#include "helper.h"
#include "clock.h"
#include "quirks.h"

static void *find_uac_clock_desc(struct usb_host_interface *iface, int id,
				 bool (*validator)(void *, int), u8 type)
{
	void *cs = NULL;

	while ((cs = snd_usb_find_csint_desc(iface->extra, iface->extralen,
					     cs, type))) {
		if (validator(cs, id))
			return cs;
	}

	return NULL;
}

static bool validate_clock_source_v2(void *p, int id)
{
	struct uac_clock_source_descriptor *cs = p;
	return cs->bClockID == id;
}

static bool validate_clock_source_v3(void *p, int id)
{
	struct uac3_clock_source_descriptor *cs = p;
	return cs->bClockID == id;
}

static bool validate_clock_selector_v2(void *p, int id)
{
	struct uac_clock_selector_descriptor *cs = p;
	return cs->bClockID == id;
}

static bool validate_clock_selector_v3(void *p, int id)
{
	struct uac3_clock_selector_descriptor *cs = p;
	return cs->bClockID == id;
}

static bool validate_clock_multiplier_v2(void *p, int id)
{
	struct uac_clock_multiplier_descriptor *cs = p;
	return cs->bClockID == id;
}

static bool validate_clock_multiplier_v3(void *p, int id)
{
	struct uac3_clock_multiplier_descriptor *cs = p;
	return cs->bClockID == id;
}

#define DEFINE_FIND_HELPER(name, obj, validator, type)		\
static obj *name(struct usb_host_interface *iface, int id)	\
{								\
	return find_uac_clock_desc(iface, id, validator, type);	\
}

DEFINE_FIND_HELPER(snd_usb_find_clock_source,
		   struct uac_clock_source_descriptor,
		   validate_clock_source_v2, UAC2_CLOCK_SOURCE);
DEFINE_FIND_HELPER(snd_usb_find_clock_source_v3,
		   struct uac3_clock_source_descriptor,
		   validate_clock_source_v3, UAC3_CLOCK_SOURCE);

DEFINE_FIND_HELPER(snd_usb_find_clock_selector,
		   struct uac_clock_selector_descriptor,
		   validate_clock_selector_v2, UAC2_CLOCK_SELECTOR);
DEFINE_FIND_HELPER(snd_usb_find_clock_selector_v3,
		   struct uac3_clock_selector_descriptor,
		   validate_clock_selector_v3, UAC3_CLOCK_SELECTOR);

DEFINE_FIND_HELPER(snd_usb_find_clock_multiplier,
		   struct uac_clock_multiplier_descriptor,
		   validate_clock_multiplier_v2, UAC2_CLOCK_MULTIPLIER);
DEFINE_FIND_HELPER(snd_usb_find_clock_multiplier_v3,
		   struct uac3_clock_multiplier_descriptor,
		   validate_clock_multiplier_v3, UAC3_CLOCK_MULTIPLIER);

static int uac_clock_selector_get_val(struct snd_usb_audio *chip, int selector_id)
{
	unsigned char buf;
	int ret;

	ret = snd_usb_ctl_msg(chip->dev, usb_rcvctrlpipe(chip->dev, 0),
			      UAC2_CS_CUR,
			      USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
			      UAC2_CX_CLOCK_SELECTOR << 8,
			      snd_usb_ctrl_intf(chip) | (selector_id << 8),
			      &buf, sizeof(buf));

	if (ret < 0)
		return ret;

	return buf;
}

static int uac_clock_selector_set_val(struct snd_usb_audio *chip, int selector_id,
					unsigned char pin)
{
	int ret;

	ret = snd_usb_ctl_msg(chip->dev, usb_sndctrlpipe(chip->dev, 0),
			      UAC2_CS_CUR,
			      USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_OUT,
			      UAC2_CX_CLOCK_SELECTOR << 8,
			      snd_usb_ctrl_intf(chip) | (selector_id << 8),
			      &pin, sizeof(pin));
	if (ret < 0)
		return ret;

	if (ret != sizeof(pin)) {
		usb_audio_err(chip,
			"setting selector (id %d) unexpected length %d\n",
			selector_id, ret);
		return -EINVAL;
	}

	ret = uac_clock_selector_get_val(chip, selector_id);
	if (ret < 0)
		return ret;

	if (ret != pin) {
		usb_audio_err(chip,
			"setting selector (id %d) to %x failed (current: %d)\n",
			selector_id, pin, ret);
		return -EINVAL;
	}

	return ret;
}

static bool uac_clock_source_is_valid_quirk(struct snd_usb_audio *chip,
					    const struct audioformat *fmt,
					    int source_id)
{
	bool ret = false;
	int count;
	unsigned char data;
	struct usb_device *dev = chip->dev;

	if (fmt->protocol == UAC_VERSION_2) {
		struct uac_clock_source_descriptor *cs_desc =
			snd_usb_find_clock_source(chip->ctrl_intf, source_id);

		if (!cs_desc)
			return false;

		/*
		 * Assume the clock is valid if clock source supports only one
		 * single sample rate, the terminal is connected directly to it
		 * (there is no clock selector) and clock type is internal.
		 * This is to deal with some Denon DJ controllers that always
		 * reports that clock is invalid.
		 */
		if (fmt->nr_rates == 1 &&
		    (fmt->clock & 0xff) == cs_desc->bClockID &&
		    (cs_desc->bmAttributes & 0x3) !=
				UAC_CLOCK_SOURCE_TYPE_EXT)
			return true;
	}

	/*
	 * MOTU MicroBook IIc
	 * Sample rate changes takes more than 2 seconds for this device. Clock
	 * validity request returns false during that period.
	 */
	if (chip->usb_id == USB_ID(0x07fd, 0x0004)) {
		count = 0;

		while ((!ret) && (count < 50)) {
			int err;

			msleep(100);

			err = snd_usb_ctl_msg(dev, usb_rcvctrlpipe(dev, 0), UAC2_CS_CUR,
					      USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
					      UAC2_CS_CONTROL_CLOCK_VALID << 8,
					      snd_usb_ctrl_intf(chip) | (source_id << 8),
					      &data, sizeof(data));
			if (err < 0) {
				dev_warn(&dev->dev,
					 "%s(): cannot get clock validity for id %d\n",
					   __func__, source_id);
				return false;
			}

			ret = !!data;
			count++;
		}
	}

	return ret;
}

static bool uac_clock_source_is_valid(struct snd_usb_audio *chip,
				      const struct audioformat *fmt,
				      int source_id)
{
	int err;
	unsigned char data;
	struct usb_device *dev = chip->dev;
	u32 bmControls;

	if (fmt->protocol == UAC_VERSION_3) {
		struct uac3_clock_source_descriptor *cs_desc =
			snd_usb_find_clock_source_v3(chip->ctrl_intf, source_id);

		if (!cs_desc)
			return false;
		bmControls = le32_to_cpu(cs_desc->bmControls);
	} else { /* UAC_VERSION_1/2 */
		struct uac_clock_source_descriptor *cs_desc =
			snd_usb_find_clock_source(chip->ctrl_intf, source_id);

		if (!cs_desc)
			return false;
		bmControls = cs_desc->bmControls;
	}

	/* If a clock source can't tell us whether it's valid, we assume it is */
	if (!uac_v2v3_control_is_readable(bmControls,
				      UAC2_CS_CONTROL_CLOCK_VALID))
		return true;

	err = snd_usb_ctl_msg(dev, usb_rcvctrlpipe(dev, 0), UAC2_CS_CUR,
			      USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
			      UAC2_CS_CONTROL_CLOCK_VALID << 8,
			      snd_usb_ctrl_intf(chip) | (source_id << 8),
			      &data, sizeof(data));

	if (err < 0) {
		dev_warn(&dev->dev,
			 "%s(): cannot get clock validity for id %d\n",
			   __func__, source_id);
		return false;
	}

	if (data)
		return true;
	else
		return uac_clock_source_is_valid_quirk(chip, fmt, source_id);
}

static int __uac_clock_find_source(struct snd_usb_audio *chip,
				   const struct audioformat *fmt, int entity_id,
				   unsigned long *visited, bool validate)
{
	struct uac_clock_source_descriptor *source;
	struct uac_clock_selector_descriptor *selector;
	struct uac_clock_multiplier_descriptor *multiplier;

	entity_id &= 0xff;

	if (test_and_set_bit(entity_id, visited)) {
		usb_audio_warn(chip,
			 "%s(): recursive clock topology detected, id %d.\n",
			 __func__, entity_id);
		return -EINVAL;
	}

	/* first, see if the ID we're looking for is a clock source already */
	source = snd_usb_find_clock_source(chip->ctrl_intf, entity_id);
	if (source) {
		entity_id = source->bClockID;
		if (validate && !uac_clock_source_is_valid(chip, fmt,
								entity_id)) {
			usb_audio_err(chip,
				"clock source %d is not valid, cannot use\n",
				entity_id);
			return -ENXIO;
		}
		return entity_id;
	}

	selector = snd_usb_find_clock_selector(chip->ctrl_intf, entity_id);
	if (selector) {
		int ret, i, cur, err;

		if (selector->bNrInPins == 1) {
			ret = 1;
			goto find_source;
		}

		/* the entity ID we are looking for is a selector.
		 * find out what it currently selects */
		ret = uac_clock_selector_get_val(chip, selector->bClockID);
		if (ret < 0)
			return ret;

		/* Selector values are one-based */

		if (ret > selector->bNrInPins || ret < 1) {
			usb_audio_err(chip,
				"%s(): selector reported illegal value, id %d, ret %d\n",
				__func__, selector->bClockID, ret);

			return -EINVAL;
		}

	find_source:
		cur = ret;
		ret = __uac_clock_find_source(chip, fmt,
					      selector->baCSourceID[ret - 1],
					      visited, validate);
		if (ret > 0) {
			err = uac_clock_selector_set_val(chip, entity_id, cur);
			if (err < 0)
				return err;
		}

		if (!validate || ret > 0 || !chip->autoclock)
			return ret;

		/* The current clock source is invalid, try others. */
		for (i = 1; i <= selector->bNrInPins; i++) {
			if (i == cur)
				continue;

			ret = __uac_clock_find_source(chip, fmt,
						      selector->baCSourceID[i - 1],
						      visited, true);
			if (ret < 0)
				continue;

			err = uac_clock_selector_set_val(chip, entity_id, i);
			if (err < 0)
				continue;

			usb_audio_info(chip,
				 "found and selected valid clock source %d\n",
				 ret);
			return ret;
		}

		return -ENXIO;
	}

	/* FIXME: multipliers only act as pass-thru element for now */
	multiplier = snd_usb_find_clock_multiplier(chip->ctrl_intf, entity_id);
	if (multiplier)
		return __uac_clock_find_source(chip, fmt,
					       multiplier->bCSourceID,
					       visited, validate);

	return -EINVAL;
}

static int __uac3_clock_find_source(struct snd_usb_audio *chip,
				    const struct audioformat *fmt, int entity_id,
				    unsigned long *visited, bool validate)
{
	struct uac3_clock_source_descriptor *source;
	struct uac3_clock_selector_descriptor *selector;
	struct uac3_clock_multiplier_descriptor *multiplier;

	entity_id &= 0xff;

	if (test_and_set_bit(entity_id, visited)) {
		usb_audio_warn(chip,
			 "%s(): recursive clock topology detected, id %d.\n",
			 __func__, entity_id);
		return -EINVAL;
	}

	/* first, see if the ID we're looking for is a clock source already */
	source = snd_usb_find_clock_source_v3(chip->ctrl_intf, entity_id);
	if (source) {
		entity_id = source->bClockID;
		if (validate && !uac_clock_source_is_valid(chip, fmt,
								entity_id)) {
			usb_audio_err(chip,
				"clock source %d is not valid, cannot use\n",
				entity_id);
			return -ENXIO;
		}
		return entity_id;
	}

	selector = snd_usb_find_clock_selector_v3(chip->ctrl_intf, entity_id);
	if (selector) {
		int ret, i, cur, err;

		/* the entity ID we are looking for is a selector.
		 * find out what it currently selects */
		ret = uac_clock_selector_get_val(chip, selector->bClockID);
		if (ret < 0)
			return ret;

		/* Selector values are one-based */

		if (ret > selector->bNrInPins || ret < 1) {
			usb_audio_err(chip,
				"%s(): selector reported illegal value, id %d, ret %d\n",
				__func__, selector->bClockID, ret);

			return -EINVAL;
		}

		cur = ret;
		ret = __uac3_clock_find_source(chip, fmt,
					       selector->baCSourceID[ret - 1],
					       visited, validate);
		if (ret > 0) {
			err = uac_clock_selector_set_val(chip, entity_id, cur);
			if (err < 0)
				return err;
		}

		if (!validate || ret > 0 || !chip->autoclock)
			return ret;

		/* The current clock source is invalid, try others. */
		for (i = 1; i <= selector->bNrInPins; i++) {
			int err;

			if (i == cur)
				continue;

			ret = __uac3_clock_find_source(chip, fmt,
						       selector->baCSourceID[i - 1],
						       visited, true);
			if (ret < 0)
				continue;

			err = uac_clock_selector_set_val(chip, entity_id, i);
			if (err < 0)
				continue;

			usb_audio_info(chip,
				 "found and selected valid clock source %d\n",
				 ret);
			return ret;
		}

		return -ENXIO;
	}

	/* FIXME: multipliers only act as pass-thru element for now */
	multiplier = snd_usb_find_clock_multiplier_v3(chip->ctrl_intf,
						      entity_id);
	if (multiplier)
		return __uac3_clock_find_source(chip, fmt,
						multiplier->bCSourceID,
						visited, validate);

	return -EINVAL;
}

/*
 * For all kinds of sample rate settings and other device queries,
 * the clock source (end-leaf) must be used. However, clock selectors,
 * clock multipliers and sample rate converters may be specified as
 * clock source input to terminal. This functions walks the clock path
 * to its end and tries to find the source.
 *
 * The 'visited' bitfield is used internally to detect recursive loops.
 *
 * Returns the clock source UnitID (>=0) on success, or an error.
 */
int snd_usb_clock_find_source(struct snd_usb_audio *chip,
			      const struct audioformat *fmt, bool validate)
{
	DECLARE_BITMAP(visited, 256);
	memset(visited, 0, sizeof(visited));

	switch (fmt->protocol) {
	case UAC_VERSION_2:
		return __uac_clock_find_source(chip, fmt, fmt->clock, visited,
					       validate);
	case UAC_VERSION_3:
		return __uac3_clock_find_source(chip, fmt, fmt->clock, visited,
					       validate);
	default:
		return -EINVAL;
	}
}

static int set_sample_rate_v1(struct snd_usb_audio *chip,
			      const struct audioformat *fmt, int rate)
{
	struct usb_device *dev = chip->dev;
	unsigned char data[3];
	int err, crate;

	/* if endpoint doesn't have sampling rate control, bail out */
	if (!(fmt->attributes & UAC_EP_CS_ATTR_SAMPLE_RATE))
		return 0;

	data[0] = rate;
	data[1] = rate >> 8;
	data[2] = rate >> 16;
	err = snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0), UAC_SET_CUR,
			      USB_TYPE_CLASS | USB_RECIP_ENDPOINT | USB_DIR_OUT,
			      UAC_EP_CS_ATTR_SAMPLE_RATE << 8,
			      fmt->endpoint, data, sizeof(data));
	if (err < 0) {
		dev_err(&dev->dev, "%d:%d: cannot set freq %d to ep %#x\n",
			fmt->iface, fmt->altsetting, rate, fmt->endpoint);
		return err;
	}

	/* Don't check the sample rate for devices which we know don't
	 * support reading */
	if (snd_usb_get_sample_rate_quirk(chip))
		return 0;
	/* the firmware is likely buggy, don't repeat to fail too many times */
	if (chip->sample_rate_read_error > 2)
		return 0;

	err = snd_usb_ctl_msg(dev, usb_rcvctrlpipe(dev, 0), UAC_GET_CUR,
			      USB_TYPE_CLASS | USB_RECIP_ENDPOINT | USB_DIR_IN,
			      UAC_EP_CS_ATTR_SAMPLE_RATE << 8,
			      fmt->endpoint, data, sizeof(data));
	if (err < 0) {
		dev_err(&dev->dev, "%d:%d: cannot get freq at ep %#x\n",
			fmt->iface, fmt->altsetting, fmt->endpoint);
		chip->sample_rate_read_error++;
		return 0; /* some devices don't support reading */
	}

	crate = data[0] | (data[1] << 8) | (data[2] << 16);
	if (!crate) {
		dev_info(&dev->dev, "failed to read current rate; disabling the check\n");
		chip->sample_rate_read_error = 3; /* three strikes, see above */
		return 0;
	}

	if (crate != rate) {
		dev_warn(&dev->dev, "current rate %d is different from the runtime rate %d\n", crate, rate);
		// runtime->rate = crate;
	}

	return 0;
}

static int get_sample_rate_v2v3(struct snd_usb_audio *chip, int iface,
			      int altsetting, int clock)
{
	struct usb_device *dev = chip->dev;
	__le32 data;
	int err;

	err = snd_usb_ctl_msg(dev, usb_rcvctrlpipe(dev, 0), UAC2_CS_CUR,
			      USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
			      UAC2_CS_CONTROL_SAM_FREQ << 8,
			      snd_usb_ctrl_intf(chip) | (clock << 8),
			      &data, sizeof(data));
	if (err < 0) {
		dev_warn(&dev->dev, "%d:%d: cannot get freq (v2/v3): err %d\n",
			 iface, altsetting, err);
		return 0;
	}

	return le32_to_cpu(data);
}

/*
 * Try to set the given sample rate:
 *
 * Return 0 if the clock source is read-only, the actual rate on success,
 * or a negative error code.
 *
 * This function gets called from format.c to validate each sample rate, too.
 * Hence no message is shown upon error
 */
int snd_usb_set_sample_rate_v2v3(struct snd_usb_audio *chip,
				 const struct audioformat *fmt,
				 int clock, int rate)
{
	bool writeable;
	u32 bmControls;
	__le32 data;
	int err;

	if (fmt->protocol == UAC_VERSION_3) {
		struct uac3_clock_source_descriptor *cs_desc;

		cs_desc = snd_usb_find_clock_source_v3(chip->ctrl_intf, clock);
		bmControls = le32_to_cpu(cs_desc->bmControls);
	} else {
		struct uac_clock_source_descriptor *cs_desc;

		cs_desc = snd_usb_find_clock_source(chip->ctrl_intf, clock);
		bmControls = cs_desc->bmControls;
	}

	writeable = uac_v2v3_control_is_writeable(bmControls,
						  UAC2_CS_CONTROL_SAM_FREQ);
	if (!writeable)
		return 0;

	data = cpu_to_le32(rate);
	err = snd_usb_ctl_msg(chip->dev, usb_sndctrlpipe(chip->dev, 0), UAC2_CS_CUR,
			      USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
			      UAC2_CS_CONTROL_SAM_FREQ << 8,
			      snd_usb_ctrl_intf(chip) | (clock << 8),
			      &data, sizeof(data));
	if (err < 0)
		return err;

	return get_sample_rate_v2v3(chip, fmt->iface, fmt->altsetting, clock);
}

static int set_sample_rate_v2v3(struct snd_usb_audio *chip,
				const struct audioformat *fmt, int rate)
{
	int cur_rate, prev_rate;
	int clock;

	/* First, try to find a valid clock. This may trigger
	 * automatic clock selection if the current clock is not
	 * valid.
	 */
	clock = snd_usb_clock_find_source(chip, fmt, true);
	if (clock < 0) {
		/* We did not find a valid clock, but that might be
		 * because the current sample rate does not match an
		 * external clock source. Try again without validation
		 * and we will do another validation after setting the
		 * rate.
		 */
		clock = snd_usb_clock_find_source(chip, fmt, false);
		if (clock < 0)
			return clock;
	}

	prev_rate = get_sample_rate_v2v3(chip, fmt->iface, fmt->altsetting, clock);
	if (prev_rate == rate)
		goto validation;

	cur_rate = snd_usb_set_sample_rate_v2v3(chip, fmt, clock, rate);
	if (cur_rate < 0) {
		usb_audio_err(chip,
			      "%d:%d: cannot set freq %d (v2/v3): err %d\n",
			      fmt->iface, fmt->altsetting, rate, cur_rate);
		return cur_rate;
	}

	if (!cur_rate)
		cur_rate = prev_rate;

	if (cur_rate != rate) {
		usb_audio_dbg(chip,
			      "%d:%d: freq mismatch: req %d, clock runs @%d\n",
			      fmt->iface, fmt->altsetting, rate, cur_rate);
		/* continue processing */
	}

validation:
	/* validate clock after rate change */
	if (!uac_clock_source_is_valid(chip, fmt, clock))
		return -ENXIO;
	return 0;
}

int snd_usb_init_sample_rate(struct snd_usb_audio *chip,
			     const struct audioformat *fmt, int rate)
{
	usb_audio_dbg(chip, "%d:%d Set sample rate %d, clock %d\n",
		      fmt->iface, fmt->altsetting, rate, fmt->clock);

	switch (fmt->protocol) {
	case UAC_VERSION_1:
	default:
		return set_sample_rate_v1(chip, fmt, rate);

	case UAC_VERSION_3:
		if (chip->badd_profile >= UAC3_FUNCTION_SUBCLASS_GENERIC_IO) {
			if (rate != UAC3_BADD_SAMPLING_RATE)
				return -ENXIO;
			else
				return 0;
		}
		fallthrough;
	case UAC_VERSION_2:
		return set_sample_rate_v2v3(chip, fmt, rate);
	}
}


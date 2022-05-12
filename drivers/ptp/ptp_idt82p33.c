// SPDX-License-Identifier: GPL-2.0
//
// Copyright (C) 2018 Integrated Device Technology, Inc
//

#define pr_fmt(fmt) "IDT_82p33xxx: " fmt

#include <linux/firmware.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/timekeeping.h>
#include <linux/bitops.h>

#include "ptp_private.h"
#include "ptp_idt82p33.h"

MODULE_DESCRIPTION("Driver for IDT 82p33xxx clock devices");
MODULE_AUTHOR("IDT support-1588 <IDT-support-1588@lm.renesas.com>");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(FW_FILENAME);

/* Module Parameters */
static u32 sync_tod_timeout = SYNC_TOD_TIMEOUT_SEC;
module_param(sync_tod_timeout, uint, 0);
MODULE_PARM_DESC(sync_tod_timeout,
"duration in second to keep SYNC_TOD on (set to 0 to keep it always on)");

static u32 phase_snap_threshold = SNAP_THRESHOLD_NS;
module_param(phase_snap_threshold, uint, 0);
MODULE_PARM_DESC(phase_snap_threshold,
"threshold (150000ns by default) below which adjtime would ignore");

static void idt82p33_byte_array_to_timespec(struct timespec64 *ts,
					    u8 buf[TOD_BYTE_COUNT])
{
	time64_t sec;
	s32 nsec;
	u8 i;

	nsec = buf[3];
	for (i = 0; i < 3; i++) {
		nsec <<= 8;
		nsec |= buf[2 - i];
	}

	sec = buf[9];
	for (i = 0; i < 5; i++) {
		sec <<= 8;
		sec |= buf[8 - i];
	}

	ts->tv_sec = sec;
	ts->tv_nsec = nsec;
}

static void idt82p33_timespec_to_byte_array(struct timespec64 const *ts,
					    u8 buf[TOD_BYTE_COUNT])
{
	time64_t sec;
	s32 nsec;
	u8 i;

	nsec = ts->tv_nsec;
	sec = ts->tv_sec;

	for (i = 0; i < 4; i++) {
		buf[i] = nsec & 0xff;
		nsec >>= 8;
	}

	for (i = 4; i < TOD_BYTE_COUNT; i++) {
		buf[i] = sec & 0xff;
		sec >>= 8;
	}
}

static int idt82p33_xfer_read(struct idt82p33 *idt82p33,
			      unsigned char regaddr,
			      unsigned char *buf,
			      unsigned int count)
{
	struct i2c_client *client = idt82p33->client;
	struct i2c_msg msg[2];
	int cnt;

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &regaddr;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = count;
	msg[1].buf = buf;

	cnt = i2c_transfer(client->adapter, msg, 2);
	if (cnt < 0) {
		dev_err(&client->dev, "i2c_transfer returned %d\n", cnt);
		return cnt;
	} else if (cnt != 2) {
		dev_err(&client->dev,
			"i2c_transfer sent only %d of %d messages\n", cnt, 2);
		return -EIO;
	}
	return 0;
}

static int idt82p33_xfer_write(struct idt82p33 *idt82p33,
			       u8 regaddr,
			       u8 *buf,
			       u16 count)
{
	struct i2c_client *client = idt82p33->client;
	/* we add 1 byte for device register */
	u8 msg[IDT82P33_MAX_WRITE_COUNT + 1];
	int err;

	if (count > IDT82P33_MAX_WRITE_COUNT)
		return -EINVAL;

	msg[0] = regaddr;
	memcpy(&msg[1], buf, count);

	err = i2c_master_send(client, msg, count + 1);
	if (err < 0) {
		dev_err(&client->dev, "i2c_master_send returned %d\n", err);
		return err;
	}

	return 0;
}

static int idt82p33_page_offset(struct idt82p33 *idt82p33, unsigned char val)
{
	int err;

	if (idt82p33->page_offset == val)
		return 0;

	err = idt82p33_xfer_write(idt82p33, PAGE_ADDR, &val, sizeof(val));
	if (err)
		dev_err(&idt82p33->client->dev,
			"failed to set page offset %d\n", val);
	else
		idt82p33->page_offset = val;

	return err;
}

static int idt82p33_rdwr(struct idt82p33 *idt82p33, unsigned int regaddr,
			 unsigned char *buf, unsigned int count, bool write)
{
	u8 offset, page;
	int err;

	page = _PAGE(regaddr);
	offset = _OFFSET(regaddr);

	err = idt82p33_page_offset(idt82p33, page);
	if (err)
		return err;

	if (write)
		return idt82p33_xfer_write(idt82p33, offset, buf, count);

	return idt82p33_xfer_read(idt82p33, offset, buf, count);
}

static int idt82p33_read(struct idt82p33 *idt82p33, unsigned int regaddr,
			unsigned char *buf, unsigned int count)
{
	return idt82p33_rdwr(idt82p33, regaddr, buf, count, false);
}

static int idt82p33_write(struct idt82p33 *idt82p33, unsigned int regaddr,
			unsigned char *buf, unsigned int count)
{
	return idt82p33_rdwr(idt82p33, regaddr, buf, count, true);
}

static int idt82p33_dpll_set_mode(struct idt82p33_channel *channel,
				  enum pll_mode mode)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	u8 dpll_mode;
	int err;

	if (channel->pll_mode == mode)
		return 0;

	err = idt82p33_read(idt82p33, channel->dpll_mode_cnfg,
			    &dpll_mode, sizeof(dpll_mode));
	if (err)
		return err;

	dpll_mode &= ~(PLL_MODE_MASK << PLL_MODE_SHIFT);

	dpll_mode |= (mode << PLL_MODE_SHIFT);

	err = idt82p33_write(idt82p33, channel->dpll_mode_cnfg,
			     &dpll_mode, sizeof(dpll_mode));
	if (err)
		return err;

	channel->pll_mode = dpll_mode;

	return 0;
}

static int _idt82p33_gettime(struct idt82p33_channel *channel,
			     struct timespec64 *ts)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	u8 buf[TOD_BYTE_COUNT];
	u8 trigger;
	int err;

	trigger = TOD_TRIGGER(HW_TOD_WR_TRIG_SEL_MSB_TOD_CNFG,
			      HW_TOD_RD_TRIG_SEL_LSB_TOD_STS);


	err = idt82p33_write(idt82p33, channel->dpll_tod_trigger,
			     &trigger, sizeof(trigger));

	if (err)
		return err;

	if (idt82p33->calculate_overhead_flag)
		idt82p33->start_time = ktime_get_raw();

	err = idt82p33_read(idt82p33, channel->dpll_tod_sts, buf, sizeof(buf));

	if (err)
		return err;

	idt82p33_byte_array_to_timespec(ts, buf);

	return 0;
}

/*
 *   TOD Trigger:
 *   Bits[7:4] Write 0x9, MSB write
 *   Bits[3:0] Read 0x9, LSB read
 */

static int _idt82p33_settime(struct idt82p33_channel *channel,
			     struct timespec64 const *ts)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	struct timespec64 local_ts = *ts;
	char buf[TOD_BYTE_COUNT];
	s64 dynamic_overhead_ns;
	unsigned char trigger;
	int err;
	u8 i;

	trigger = TOD_TRIGGER(HW_TOD_WR_TRIG_SEL_MSB_TOD_CNFG,
			      HW_TOD_RD_TRIG_SEL_LSB_TOD_STS);

	err = idt82p33_write(idt82p33, channel->dpll_tod_trigger,
			&trigger, sizeof(trigger));

	if (err)
		return err;

	if (idt82p33->calculate_overhead_flag) {
		dynamic_overhead_ns = ktime_to_ns(ktime_get_raw())
					- ktime_to_ns(idt82p33->start_time);

		timespec64_add_ns(&local_ts, dynamic_overhead_ns);

		idt82p33->calculate_overhead_flag = 0;
	}

	idt82p33_timespec_to_byte_array(&local_ts, buf);

	/*
	 * Store the new time value.
	 */
	for (i = 0; i < TOD_BYTE_COUNT; i++) {
		err = idt82p33_write(idt82p33, channel->dpll_tod_cnfg + i,
				     &buf[i], sizeof(buf[i]));
		if (err)
			return err;
	}

	return err;
}

static int _idt82p33_adjtime(struct idt82p33_channel *channel, s64 delta_ns)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	struct timespec64 ts;
	s64 now_ns;
	int err;

	idt82p33->calculate_overhead_flag = 1;

	err = _idt82p33_gettime(channel, &ts);

	if (err)
		return err;

	now_ns = timespec64_to_ns(&ts);
	now_ns += delta_ns + idt82p33->tod_write_overhead_ns;

	ts = ns_to_timespec64(now_ns);

	err = _idt82p33_settime(channel, &ts);

	return err;
}

static int _idt82p33_adjfine(struct idt82p33_channel *channel, long scaled_ppm)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	unsigned char buf[5] = {0};
	int err, i;
	s64 fcw;

	if (scaled_ppm == channel->current_freq_ppb)
		return 0;

	/*
	 * Frequency Control Word unit is: 1.68 * 10^-10 ppm
	 *
	 * adjfreq:
	 *       ppb * 10^9
	 * FCW = ----------
	 *          168
	 *
	 * adjfine:
	 *       scaled_ppm * 5^12
	 * FCW = -------------
	 *         168 * 2^4
	 */

	fcw = scaled_ppm * 244140625ULL;
	fcw = div_s64(fcw, 2688);

	for (i = 0; i < 5; i++) {
		buf[i] = fcw & 0xff;
		fcw >>= 8;
	}

	err = idt82p33_dpll_set_mode(channel, PLL_MODE_DCO);

	if (err)
		return err;

	err = idt82p33_write(idt82p33, channel->dpll_freq_cnfg,
			     buf, sizeof(buf));

	if (err == 0)
		channel->current_freq_ppb = scaled_ppm;

	return err;
}

static int idt82p33_measure_one_byte_write_overhead(
		struct idt82p33_channel *channel, s64 *overhead_ns)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	ktime_t start, stop;
	s64 total_ns;
	u8 trigger;
	int err;
	u8 i;

	total_ns = 0;
	*overhead_ns = 0;
	trigger = TOD_TRIGGER(HW_TOD_WR_TRIG_SEL_MSB_TOD_CNFG,
			      HW_TOD_RD_TRIG_SEL_LSB_TOD_STS);

	for (i = 0; i < MAX_MEASURMENT_COUNT; i++) {

		start = ktime_get_raw();

		err = idt82p33_write(idt82p33, channel->dpll_tod_trigger,
				     &trigger, sizeof(trigger));

		stop = ktime_get_raw();

		if (err)
			return err;

		total_ns += ktime_to_ns(stop) - ktime_to_ns(start);
	}

	*overhead_ns = div_s64(total_ns, MAX_MEASURMENT_COUNT);

	return err;
}

static int idt82p33_measure_tod_write_9_byte_overhead(
			struct idt82p33_channel *channel)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	u8 buf[TOD_BYTE_COUNT];
	ktime_t start, stop;
	s64 total_ns;
	int err = 0;
	u8 i, j;

	total_ns = 0;
	idt82p33->tod_write_overhead_ns = 0;

	for (i = 0; i < MAX_MEASURMENT_COUNT; i++) {

		start = ktime_get_raw();

		/* Need one less byte for applicable overhead */
		for (j = 0; j < (TOD_BYTE_COUNT - 1); j++) {
			err = idt82p33_write(idt82p33,
					     channel->dpll_tod_cnfg + i,
					     &buf[i], sizeof(buf[i]));
			if (err)
				return err;
		}

		stop = ktime_get_raw();

		total_ns += ktime_to_ns(stop) - ktime_to_ns(start);
	}

	idt82p33->tod_write_overhead_ns = div_s64(total_ns,
						  MAX_MEASURMENT_COUNT);

	return err;
}

static int idt82p33_measure_settime_gettime_gap_overhead(
		struct idt82p33_channel *channel, s64 *overhead_ns)
{
	struct timespec64 ts1 = {0, 0};
	struct timespec64 ts2;
	int err;

	*overhead_ns = 0;

	err = _idt82p33_settime(channel, &ts1);

	if (err)
		return err;

	err = _idt82p33_gettime(channel, &ts2);

	if (!err)
		*overhead_ns = timespec64_to_ns(&ts2) - timespec64_to_ns(&ts1);

	return err;
}

static int idt82p33_measure_tod_write_overhead(struct idt82p33_channel *channel)
{
	s64 trailing_overhead_ns, one_byte_write_ns, gap_ns;
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;

	idt82p33->tod_write_overhead_ns = 0;

	err = idt82p33_measure_settime_gettime_gap_overhead(channel, &gap_ns);

	if (err) {
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
		return err;
	}

	err = idt82p33_measure_one_byte_write_overhead(channel,
						       &one_byte_write_ns);

	if (err)
		return err;

	err = idt82p33_measure_tod_write_9_byte_overhead(channel);

	if (err)
		return err;

	trailing_overhead_ns = gap_ns - (2 * one_byte_write_ns);

	idt82p33->tod_write_overhead_ns -= trailing_overhead_ns;

	return err;
}

static int idt82p33_check_and_set_masks(struct idt82p33 *idt82p33,
					u8 page,
					u8 offset,
					u8 val)
{
	int err = 0;

	if (page == PLLMASK_ADDR_HI && offset == PLLMASK_ADDR_LO) {
		if ((val & 0xfc) || !(val & 0x3)) {
			dev_err(&idt82p33->client->dev,
				"Invalid PLL mask 0x%hhx\n", val);
			err = -EINVAL;
		} else {
			idt82p33->pll_mask = val;
		}
	} else if (page == PLL0_OUTMASK_ADDR_HI &&
		offset == PLL0_OUTMASK_ADDR_LO) {
		idt82p33->channel[0].output_mask = val;
	} else if (page == PLL1_OUTMASK_ADDR_HI &&
		offset == PLL1_OUTMASK_ADDR_LO) {
		idt82p33->channel[1].output_mask = val;
	}

	return err;
}

static void idt82p33_display_masks(struct idt82p33 *idt82p33)
{
	u8 mask, i;

	dev_info(&idt82p33->client->dev,
		 "pllmask = 0x%02x\n", idt82p33->pll_mask);

	for (i = 0; i < MAX_PHC_PLL; i++) {
		mask = 1 << i;

		if (mask & idt82p33->pll_mask)
			dev_info(&idt82p33->client->dev,
				 "PLL%d output_mask = 0x%04x\n",
				 i, idt82p33->channel[i].output_mask);
	}
}

static int idt82p33_sync_tod(struct idt82p33_channel *channel, bool enable)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	u8 sync_cnfg;
	int err;

	/* Turn it off after sync_tod_timeout seconds */
	if (enable && sync_tod_timeout)
		ptp_schedule_worker(channel->ptp_clock,
				    sync_tod_timeout * HZ);

	err = idt82p33_read(idt82p33, channel->dpll_sync_cnfg,
			    &sync_cnfg, sizeof(sync_cnfg));
	if (err)
		return err;

	sync_cnfg &= ~SYNC_TOD;
	if (enable)
		sync_cnfg |= SYNC_TOD;

	return idt82p33_write(idt82p33, channel->dpll_sync_cnfg,
			      &sync_cnfg, sizeof(sync_cnfg));
}

static long idt82p33_sync_tod_work_handler(struct ptp_clock_info *ptp)
{
	struct idt82p33_channel *channel =
			container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;

	mutex_lock(&idt82p33->reg_lock);

	(void)idt82p33_sync_tod(channel, false);

	mutex_unlock(&idt82p33->reg_lock);

	/* Return a negative value here to not reschedule */
	return -1;
}

static int idt82p33_output_enable(struct idt82p33_channel *channel,
				  bool enable, unsigned int outn)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;
	u8 val;

	err = idt82p33_read(idt82p33, OUT_MUX_CNFG(outn), &val, sizeof(val));
	if (err)
		return err;
	if (enable)
		val &= ~SQUELCH_ENABLE;
	else
		val |= SQUELCH_ENABLE;

	return idt82p33_write(idt82p33, OUT_MUX_CNFG(outn), &val, sizeof(val));
}

static int idt82p33_output_mask_enable(struct idt82p33_channel *channel,
				       bool enable)
{
	u16 mask;
	int err;
	u8 outn;

	mask = channel->output_mask;
	outn = 0;

	while (mask) {
		if (mask & 0x1) {
			err = idt82p33_output_enable(channel, enable, outn);
			if (err)
				return err;
		}

		mask >>= 0x1;
		outn++;
	}

	return 0;
}

static int idt82p33_perout_enable(struct idt82p33_channel *channel,
				  bool enable,
				  struct ptp_perout_request *perout)
{
	unsigned int flags = perout->flags;

	/* Enable/disable output based on output_mask */
	if (flags == PEROUT_ENABLE_OUTPUT_MASK)
		return idt82p33_output_mask_enable(channel, enable);

	/* Enable/disable individual output instead */
	return idt82p33_output_enable(channel, enable, perout->index);
}

static int idt82p33_enable_tod(struct idt82p33_channel *channel)
{
	struct idt82p33 *idt82p33 = channel->idt82p33;
	struct timespec64 ts = {0, 0};
	int err;
	u8 val;

	val = 0;
	err = idt82p33_write(idt82p33, channel->dpll_input_mode_cnfg,
			     &val, sizeof(val));
	if (err)
		return err;

	err = idt82p33_measure_tod_write_overhead(channel);

	if (err) {
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
		return err;
	}

	err = _idt82p33_settime(channel, &ts);

	if (err)
		return err;

	return idt82p33_sync_tod(channel, true);
}

static void idt82p33_ptp_clock_unregister_all(struct idt82p33 *idt82p33)
{
	struct idt82p33_channel *channel;
	u8 i;

	for (i = 0; i < MAX_PHC_PLL; i++) {

		channel = &idt82p33->channel[i];

		if (channel->ptp_clock)
			ptp_clock_unregister(channel->ptp_clock);
	}
}

static int idt82p33_enable(struct ptp_clock_info *ptp,
			 struct ptp_clock_request *rq, int on)
{
	struct idt82p33_channel *channel =
			container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;

	err = -EOPNOTSUPP;

	mutex_lock(&idt82p33->reg_lock);

	if (rq->type == PTP_CLK_REQ_PEROUT) {
		if (!on)
			err = idt82p33_perout_enable(channel, false,
						     &rq->perout);
		/* Only accept a 1-PPS aligned to the second. */
		else if (rq->perout.start.nsec || rq->perout.period.sec != 1 ||
		    rq->perout.period.nsec) {
			err = -ERANGE;
		} else
			err = idt82p33_perout_enable(channel, true,
						     &rq->perout);
	}

	mutex_unlock(&idt82p33->reg_lock);

	return err;
}

static int idt82p33_adjwritephase(struct ptp_clock_info *ptp, s32 offset_ns)
{
	struct idt82p33_channel *channel =
		container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;
	s64 offset_regval, offset_fs;
	u8 val[4] = {0};
	int err;

	offset_fs = (s64)(-offset_ns) * 1000000;

	if (offset_fs > WRITE_PHASE_OFFSET_LIMIT)
		offset_fs = WRITE_PHASE_OFFSET_LIMIT;
	else if (offset_fs < -WRITE_PHASE_OFFSET_LIMIT)
		offset_fs = -WRITE_PHASE_OFFSET_LIMIT;

	/* Convert from phaseoffset_fs to register value */
	offset_regval = div_s64(offset_fs * 1000, IDT_T0DPLL_PHASE_RESOL);

	val[0] = offset_regval & 0xFF;
	val[1] = (offset_regval >> 8) & 0xFF;
	val[2] = (offset_regval >> 16) & 0xFF;
	val[3] = (offset_regval >> 24) & 0x1F;
	val[3] |= PH_OFFSET_EN;

	mutex_lock(&idt82p33->reg_lock);

	err = idt82p33_dpll_set_mode(channel, PLL_MODE_WPH);
	if (err) {
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
		goto out;
	}

	err = idt82p33_write(idt82p33, channel->dpll_phase_cnfg, val,
			     sizeof(val));

out:
	mutex_unlock(&idt82p33->reg_lock);
	return err;
}

static int idt82p33_adjfine(struct ptp_clock_info *ptp, long scaled_ppm)
{
	struct idt82p33_channel *channel =
			container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;

	mutex_lock(&idt82p33->reg_lock);
	err = _idt82p33_adjfine(channel, scaled_ppm);
	if (err)
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
	mutex_unlock(&idt82p33->reg_lock);

	return err;
}

static int idt82p33_adjtime(struct ptp_clock_info *ptp, s64 delta_ns)
{
	struct idt82p33_channel *channel =
			container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;

	mutex_lock(&idt82p33->reg_lock);

	if (abs(delta_ns) < phase_snap_threshold) {
		mutex_unlock(&idt82p33->reg_lock);
		return 0;
	}

	err = _idt82p33_adjtime(channel, delta_ns);

	if (err) {
		mutex_unlock(&idt82p33->reg_lock);
		dev_err(&idt82p33->client->dev,
			"Adjtime failed in %s with err %d!\n", __func__, err);
		return err;
	}

	err = idt82p33_sync_tod(channel, true);
	if (err)
		dev_err(&idt82p33->client->dev,
			"Sync_tod failed in %s with err %d!\n", __func__, err);

	mutex_unlock(&idt82p33->reg_lock);

	return err;
}

static int idt82p33_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	struct idt82p33_channel *channel =
			container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;

	mutex_lock(&idt82p33->reg_lock);
	err = _idt82p33_gettime(channel, ts);
	if (err)
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
	mutex_unlock(&idt82p33->reg_lock);

	return err;
}

static int idt82p33_settime(struct ptp_clock_info *ptp,
			const struct timespec64 *ts)
{
	struct idt82p33_channel *channel =
			container_of(ptp, struct idt82p33_channel, caps);
	struct idt82p33 *idt82p33 = channel->idt82p33;
	int err;

	mutex_lock(&idt82p33->reg_lock);
	err = _idt82p33_settime(channel, ts);
	if (err)
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
	mutex_unlock(&idt82p33->reg_lock);

	return err;
}

static int idt82p33_channel_init(struct idt82p33_channel *channel, int index)
{
	switch (index) {
	case 0:
		channel->dpll_tod_cnfg = DPLL1_TOD_CNFG;
		channel->dpll_tod_trigger = DPLL1_TOD_TRIGGER;
		channel->dpll_tod_sts = DPLL1_TOD_STS;
		channel->dpll_mode_cnfg = DPLL1_OPERATING_MODE_CNFG;
		channel->dpll_freq_cnfg = DPLL1_HOLDOVER_FREQ_CNFG;
		channel->dpll_phase_cnfg = DPLL1_PHASE_OFFSET_CNFG;
		channel->dpll_sync_cnfg = DPLL1_SYNC_EDGE_CNFG;
		channel->dpll_input_mode_cnfg = DPLL1_INPUT_MODE_CNFG;
		break;
	case 1:
		channel->dpll_tod_cnfg = DPLL2_TOD_CNFG;
		channel->dpll_tod_trigger = DPLL2_TOD_TRIGGER;
		channel->dpll_tod_sts = DPLL2_TOD_STS;
		channel->dpll_mode_cnfg = DPLL2_OPERATING_MODE_CNFG;
		channel->dpll_freq_cnfg = DPLL2_HOLDOVER_FREQ_CNFG;
		channel->dpll_phase_cnfg = DPLL2_PHASE_OFFSET_CNFG;
		channel->dpll_sync_cnfg = DPLL2_SYNC_EDGE_CNFG;
		channel->dpll_input_mode_cnfg = DPLL2_INPUT_MODE_CNFG;
		break;
	default:
		return -EINVAL;
	}

	channel->current_freq_ppb = 0;

	return 0;
}

static void idt82p33_caps_init(struct ptp_clock_info *caps)
{
	caps->owner = THIS_MODULE;
	caps->max_adj = 92000;
	caps->n_per_out = 11;
	caps->adjphase = idt82p33_adjwritephase;
	caps->adjfine = idt82p33_adjfine;
	caps->adjtime = idt82p33_adjtime;
	caps->gettime64 = idt82p33_gettime;
	caps->settime64 = idt82p33_settime;
	caps->enable = idt82p33_enable;
	caps->do_aux_work = idt82p33_sync_tod_work_handler;
}

static int idt82p33_enable_channel(struct idt82p33 *idt82p33, u32 index)
{
	struct idt82p33_channel *channel;
	int err;

	if (!(index < MAX_PHC_PLL))
		return -EINVAL;

	channel = &idt82p33->channel[index];

	err = idt82p33_channel_init(channel, index);
	if (err) {
		dev_err(&idt82p33->client->dev,
			"Channel_init failed in %s with err %d!\n",
			__func__, err);
		return err;
	}

	channel->idt82p33 = idt82p33;

	idt82p33_caps_init(&channel->caps);
	snprintf(channel->caps.name, sizeof(channel->caps.name),
		 "IDT 82P33 PLL%u", index);

	channel->ptp_clock = ptp_clock_register(&channel->caps, NULL);

	if (IS_ERR(channel->ptp_clock)) {
		err = PTR_ERR(channel->ptp_clock);
		channel->ptp_clock = NULL;
		return err;
	}

	if (!channel->ptp_clock)
		return -ENOTSUPP;

	err = idt82p33_dpll_set_mode(channel, PLL_MODE_DCO);
	if (err) {
		dev_err(&idt82p33->client->dev,
			"Dpll_set_mode failed in %s with err %d!\n",
			__func__, err);
		return err;
	}

	err = idt82p33_enable_tod(channel);
	if (err) {
		dev_err(&idt82p33->client->dev,
			"Enable_tod failed in %s with err %d!\n",
			__func__, err);
		return err;
	}

	dev_info(&idt82p33->client->dev, "PLL%d registered as ptp%d\n",
		 index, channel->ptp_clock->index);

	return 0;
}

static int idt82p33_load_firmware(struct idt82p33 *idt82p33)
{
	const struct firmware *fw;
	struct idt82p33_fwrc *rec;
	u8 loaddr, page, val;
	int err;
	s32 len;

	dev_dbg(&idt82p33->client->dev,
		"requesting firmware '%s'\n", FW_FILENAME);

	err = request_firmware(&fw, FW_FILENAME, &idt82p33->client->dev);

	if (err) {
		dev_err(&idt82p33->client->dev,
			"Failed in %s with err %d!\n", __func__, err);
		return err;
	}

	dev_dbg(&idt82p33->client->dev, "firmware size %zu bytes\n", fw->size);

	rec = (struct idt82p33_fwrc *) fw->data;

	for (len = fw->size; len > 0; len -= sizeof(*rec)) {

		if (rec->reserved) {
			dev_err(&idt82p33->client->dev,
				"bad firmware, reserved field non-zero\n");
			err = -EINVAL;
		} else {
			val = rec->value;
			loaddr = rec->loaddr;
			page = rec->hiaddr;

			rec++;

			err = idt82p33_check_and_set_masks(idt82p33, page,
							   loaddr, val);
		}

		if (err == 0) {
			/* maximum 8 pages  */
			if (page >= PAGE_NUM)
				continue;

			/* Page size 128, last 4 bytes of page skipped */
			if (((loaddr > 0x7b) && (loaddr <= 0x7f))
			     || loaddr > 0xfb)
				continue;

			err = idt82p33_write(idt82p33, _ADDR(page, loaddr),
					     &val, sizeof(val));
		}

		if (err)
			goto out;
	}

	idt82p33_display_masks(idt82p33);
out:
	release_firmware(fw);
	return err;
}


static int idt82p33_probe(struct i2c_client *client,
			  const struct i2c_device_id *id)
{
	struct idt82p33 *idt82p33;
	int err;
	u8 i;

	(void)id;

	idt82p33 = devm_kzalloc(&client->dev,
				sizeof(struct idt82p33), GFP_KERNEL);
	if (!idt82p33)
		return -ENOMEM;

	mutex_init(&idt82p33->reg_lock);

	idt82p33->client = client;
	idt82p33->page_offset = 0xff;
	idt82p33->tod_write_overhead_ns = 0;
	idt82p33->calculate_overhead_flag = 0;
	idt82p33->pll_mask = DEFAULT_PLL_MASK;
	idt82p33->channel[0].output_mask = DEFAULT_OUTPUT_MASK_PLL0;
	idt82p33->channel[1].output_mask = DEFAULT_OUTPUT_MASK_PLL1;

	mutex_lock(&idt82p33->reg_lock);

	err = idt82p33_load_firmware(idt82p33);

	if (err)
		dev_warn(&idt82p33->client->dev,
			 "loading firmware failed with %d\n", err);

	if (idt82p33->pll_mask) {
		for (i = 0; i < MAX_PHC_PLL; i++) {
			if (idt82p33->pll_mask & (1 << i)) {
				err = idt82p33_enable_channel(idt82p33, i);
				if (err) {
					dev_err(&idt82p33->client->dev,
						"Failed in %s with err %d!\n",
						__func__, err);
					break;
				}
			}
		}
	} else {
		dev_err(&idt82p33->client->dev,
			"no PLLs flagged as PHCs, nothing to do\n");
		err = -ENODEV;
	}

	mutex_unlock(&idt82p33->reg_lock);

	if (err) {
		idt82p33_ptp_clock_unregister_all(idt82p33);
		return err;
	}

	i2c_set_clientdata(client, idt82p33);

	return 0;
}

static int idt82p33_remove(struct i2c_client *client)
{
	struct idt82p33 *idt82p33 = i2c_get_clientdata(client);

	idt82p33_ptp_clock_unregister_all(idt82p33);
	mutex_destroy(&idt82p33->reg_lock);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id idt82p33_dt_id[] = {
	{ .compatible = "idt,82p33810" },
	{ .compatible = "idt,82p33813" },
	{ .compatible = "idt,82p33814" },
	{ .compatible = "idt,82p33831" },
	{ .compatible = "idt,82p33910" },
	{ .compatible = "idt,82p33913" },
	{ .compatible = "idt,82p33914" },
	{ .compatible = "idt,82p33931" },
	{},
};
MODULE_DEVICE_TABLE(of, idt82p33_dt_id);
#endif

static const struct i2c_device_id idt82p33_i2c_id[] = {
	{ "idt82p33810", },
	{ "idt82p33813", },
	{ "idt82p33814", },
	{ "idt82p33831", },
	{ "idt82p33910", },
	{ "idt82p33913", },
	{ "idt82p33914", },
	{ "idt82p33931", },
	{},
};
MODULE_DEVICE_TABLE(i2c, idt82p33_i2c_id);

static struct i2c_driver idt82p33_driver = {
	.driver = {
		.of_match_table	= of_match_ptr(idt82p33_dt_id),
		.name		= "idt82p33",
	},
	.probe		= idt82p33_probe,
	.remove		= idt82p33_remove,
	.id_table	= idt82p33_i2c_id,
};

module_i2c_driver(idt82p33_driver);

// SPDX-License-Identifier: GPL-2.0-or-later
/* -*- linux-c -*- *
 *
 * ALSA driver for the digigram lx6464es interface
 * low-level interface
 *
 * Copyright (c) 2009 Tim Blechmann <tim@klingt.org>
 */

/* #define RMH_DEBUG 1 */

#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>

#include "lx6464es.h"
#include "lx_core.h"

/* low-level register access */

static const unsigned long dsp_port_offsets[] = {
	0,
	0x400,
	0x401,
	0x402,
	0x403,
	0x404,
	0x405,
	0x406,
	0x407,
	0x408,
	0x409,
	0x40a,
	0x40b,
	0x40c,

	0x410,
	0x411,
	0x412,
	0x413,
	0x414,
	0x415,
	0x416,

	0x420,
	0x430,
	0x431,
	0x432,
	0x433,
	0x434,
	0x440
};

static void __iomem *lx_dsp_register(struct lx6464es *chip, int port)
{
	void __iomem *base_address = chip->port_dsp_bar;
	return base_address + dsp_port_offsets[port]*4;
}

unsigned long lx_dsp_reg_read(struct lx6464es *chip, int port)
{
	void __iomem *address = lx_dsp_register(chip, port);
	return ioread32(address);
}

static void lx_dsp_reg_readbuf(struct lx6464es *chip, int port, u32 *data,
			       u32 len)
{
	u32 __iomem *address = lx_dsp_register(chip, port);
	int i;

	/* we cannot use memcpy_fromio */
	for (i = 0; i != len; ++i)
		data[i] = ioread32(address + i);
}


void lx_dsp_reg_write(struct lx6464es *chip, int port, unsigned data)
{
	void __iomem *address = lx_dsp_register(chip, port);
	iowrite32(data, address);
}

static void lx_dsp_reg_writebuf(struct lx6464es *chip, int port,
				const u32 *data, u32 len)
{
	u32 __iomem *address = lx_dsp_register(chip, port);
	int i;

	/* we cannot use memcpy_to */
	for (i = 0; i != len; ++i)
		iowrite32(data[i], address + i);
}


static const unsigned long plx_port_offsets[] = {
	0x04,
	0x40,
	0x44,
	0x48,
	0x4c,
	0x50,
	0x54,
	0x58,
	0x5c,
	0x64,
	0x68,
	0x6C
};

static void __iomem *lx_plx_register(struct lx6464es *chip, int port)
{
	void __iomem *base_address = chip->port_plx_remapped;
	return base_address + plx_port_offsets[port];
}

unsigned long lx_plx_reg_read(struct lx6464es *chip, int port)
{
	void __iomem *address = lx_plx_register(chip, port);
	return ioread32(address);
}

void lx_plx_reg_write(struct lx6464es *chip, int port, u32 data)
{
	void __iomem *address = lx_plx_register(chip, port);
	iowrite32(data, address);
}

/* rmh */

#ifdef CONFIG_SND_DEBUG
#define CMD_NAME(a) a
#else
#define CMD_NAME(a) NULL
#endif

#define Reg_CSM_MR			0x00000002
#define Reg_CSM_MC			0x00000001

struct dsp_cmd_info {
	u32    dcCodeOp;	/* Op Code of the command (usually 1st 24-bits
				 * word).*/
	u16    dcCmdLength;	/* Command length in words of 24 bits.*/
	u16    dcStatusType;	/* Status type: 0 for fixed length, 1 for
				 * random. */
	u16    dcStatusLength;	/* Status length (if fixed).*/
	char  *dcOpName;
};

/*
  Initialization and control data for the Microblaze interface
  - OpCode:
    the opcode field of the command set at the proper offset
  - CmdLength
    the number of command words
  - StatusType
    offset in the status registers: 0 means that the return value may be
    different from 0, and must be read
  - StatusLength
    the number of status words (in addition to the return value)
*/

static const struct dsp_cmd_info dsp_commands[] =
{
	{ (CMD_00_INFO_DEBUG << OPCODE_OFFSET)			, 1 /*custom*/
	  , 1	, 0 /**/		    , CMD_NAME("INFO_DEBUG") },
	{ (CMD_01_GET_SYS_CFG << OPCODE_OFFSET) 		, 1 /**/
	  , 1      , 2 /**/		    , CMD_NAME("GET_SYS_CFG") },
	{ (CMD_02_SET_GRANULARITY << OPCODE_OFFSET)	        , 1 /**/
	  , 1      , 0 /**/		    , CMD_NAME("SET_GRANULARITY") },
	{ (CMD_03_SET_TIMER_IRQ << OPCODE_OFFSET)		, 1 /**/
	  , 1      , 0 /**/		    , CMD_NAME("SET_TIMER_IRQ") },
	{ (CMD_04_GET_EVENT << OPCODE_OFFSET)			, 1 /**/
	  , 1      , 0 /*up to 10*/     , CMD_NAME("GET_EVENT") },
	{ (CMD_05_GET_PIPES << OPCODE_OFFSET)			, 1 /**/
	  , 1      , 2 /*up to 4*/      , CMD_NAME("GET_PIPES") },
	{ (CMD_06_ALLOCATE_PIPE << OPCODE_OFFSET)		, 1 /**/
	  , 0      , 0 /**/		    , CMD_NAME("ALLOCATE_PIPE") },
	{ (CMD_07_RELEASE_PIPE << OPCODE_OFFSET)		, 1 /**/
	  , 0      , 0 /**/		    , CMD_NAME("RELEASE_PIPE") },
	{ (CMD_08_ASK_BUFFERS << OPCODE_OFFSET) 		, 1 /**/
	  , 1      , MAX_STREAM_BUFFER  , CMD_NAME("ASK_BUFFERS") },
	{ (CMD_09_STOP_PIPE << OPCODE_OFFSET)			, 1 /**/
	  , 0      , 0 /*up to 2*/      , CMD_NAME("STOP_PIPE") },
	{ (CMD_0A_GET_PIPE_SPL_COUNT << OPCODE_OFFSET)	        , 1 /**/
	  , 1      , 1 /*up to 2*/      , CMD_NAME("GET_PIPE_SPL_COUNT") },
	{ (CMD_0B_TOGGLE_PIPE_STATE << OPCODE_OFFSET)           , 1 /*up to 5*/
	  , 1      , 0 /**/		    , CMD_NAME("TOGGLE_PIPE_STATE") },
	{ (CMD_0C_DEF_STREAM << OPCODE_OFFSET)			, 1 /*up to 4*/
	  , 1      , 0 /**/		    , CMD_NAME("DEF_STREAM") },
	{ (CMD_0D_SET_MUTE  << OPCODE_OFFSET)			, 3 /**/
	  , 1      , 0 /**/		    , CMD_NAME("SET_MUTE") },
	{ (CMD_0E_GET_STREAM_SPL_COUNT << OPCODE_OFFSET)        , 1/**/
	  , 1      , 2 /**/		    , CMD_NAME("GET_STREAM_SPL_COUNT") },
	{ (CMD_0F_UPDATE_BUFFER << OPCODE_OFFSET)		, 3 /*up to 4*/
	  , 0      , 1 /**/		    , CMD_NAME("UPDATE_BUFFER") },
	{ (CMD_10_GET_BUFFER << OPCODE_OFFSET)			, 1 /**/
	  , 1      , 4 /**/		    , CMD_NAME("GET_BUFFER") },
	{ (CMD_11_CANCEL_BUFFER << OPCODE_OFFSET)		, 1 /**/
	  , 1      , 1 /*up to 4*/      , CMD_NAME("CANCEL_BUFFER") },
	{ (CMD_12_GET_PEAK << OPCODE_OFFSET)			, 1 /**/
	  , 1      , 1 /**/		    , CMD_NAME("GET_PEAK") },
	{ (CMD_13_SET_STREAM_STATE << OPCODE_OFFSET)	        , 1 /**/
	  , 1      , 0 /**/		    , CMD_NAME("SET_STREAM_STATE") },
};

static void lx_message_init(struct lx_rmh *rmh, enum cmd_mb_opcodes cmd)
{
	snd_BUG_ON(cmd >= CMD_14_INVALID);

	rmh->cmd[0] = dsp_commands[cmd].dcCodeOp;
	rmh->cmd_len = dsp_commands[cmd].dcCmdLength;
	rmh->stat_len = dsp_commands[cmd].dcStatusLength;
	rmh->dsp_stat = dsp_commands[cmd].dcStatusType;
	rmh->cmd_idx = cmd;
	memset(&rmh->cmd[1], 0, (REG_CRM_NUMBER - 1) * sizeof(u32));

#ifdef CONFIG_SND_DEBUG
	memset(rmh->stat, 0, REG_CRM_NUMBER * sizeof(u32));
#endif
#ifdef RMH_DEBUG
	rmh->cmd_idx = cmd;
#endif
}

#ifdef RMH_DEBUG
#define LXRMH "lx6464es rmh: "
static void lx_message_dump(struct lx_rmh *rmh)
{
	u8 idx = rmh->cmd_idx;
	int i;

	snd_printk(LXRMH "command %s\n", dsp_commands[idx].dcOpName);

	for (i = 0; i != rmh->cmd_len; ++i)
		snd_printk(LXRMH "\tcmd[%d] %08x\n", i, rmh->cmd[i]);

	for (i = 0; i != rmh->stat_len; ++i)
		snd_printk(LXRMH "\tstat[%d]: %08x\n", i, rmh->stat[i]);
	snd_printk("\n");
}
#else
static inline void lx_message_dump(struct lx_rmh *rmh)
{}
#endif



/* sleep 500 - 100 = 400 times 100us -> the timeout is >= 40 ms */
#define XILINX_TIMEOUT_MS       40
#define XILINX_POLL_NO_SLEEP    100
#define XILINX_POLL_ITERATIONS  150


static int lx_message_send_atomic(struct lx6464es *chip, struct lx_rmh *rmh)
{
	u32 reg = ED_DSP_TIMED_OUT;
	int dwloop;

	if (lx_dsp_reg_read(chip, eReg_CSM) & (Reg_CSM_MC | Reg_CSM_MR)) {
		dev_err(chip->card->dev, "PIOSendMessage eReg_CSM %x\n", reg);
		return -EBUSY;
	}

	/* write command */
	lx_dsp_reg_writebuf(chip, eReg_CRM1, rmh->cmd, rmh->cmd_len);

	/* MicoBlaze gogogo */
	lx_dsp_reg_write(chip, eReg_CSM, Reg_CSM_MC);

	/* wait for device to answer */
	for (dwloop = 0; dwloop != XILINX_TIMEOUT_MS * 1000; ++dwloop) {
		if (lx_dsp_reg_read(chip, eReg_CSM) & Reg_CSM_MR) {
			if (rmh->dsp_stat == 0)
				reg = lx_dsp_reg_read(chip, eReg_CRM1);
			else
				reg = 0;
			goto polling_successful;
		} else
			udelay(1);
	}
	dev_warn(chip->card->dev, "TIMEOUT lx_message_send_atomic! "
		   "polling failed\n");

polling_successful:
	if ((reg & ERROR_VALUE) == 0) {
		/* read response */
		if (rmh->stat_len) {
			snd_BUG_ON(rmh->stat_len >= (REG_CRM_NUMBER-1));
			lx_dsp_reg_readbuf(chip, eReg_CRM2, rmh->stat,
					   rmh->stat_len);
		}
	} else
		dev_err(chip->card->dev, "rmh error: %08x\n", reg);

	/* clear Reg_CSM_MR */
	lx_dsp_reg_write(chip, eReg_CSM, 0);

	switch (reg) {
	case ED_DSP_TIMED_OUT:
		dev_warn(chip->card->dev, "lx_message_send: dsp timeout\n");
		return -ETIMEDOUT;

	case ED_DSP_CRASHED:
		dev_warn(chip->card->dev, "lx_message_send: dsp crashed\n");
		return -EAGAIN;
	}

	lx_message_dump(rmh);

	return reg;
}


/* low-level dsp access */
int lx_dsp_get_version(struct lx6464es *chip, u32 *rdsp_version)
{
	u16 ret;

	mutex_lock(&chip->msg_lock);

	lx_message_init(&chip->rmh, CMD_01_GET_SYS_CFG);
	ret = lx_message_send_atomic(chip, &chip->rmh);

	*rdsp_version = chip->rmh.stat[1];
	mutex_unlock(&chip->msg_lock);
	return ret;
}

int lx_dsp_get_clock_frequency(struct lx6464es *chip, u32 *rfreq)
{
	u16 ret = 0;
	u32 freq_raw = 0;
	u32 freq = 0;
	u32 frequency = 0;

	mutex_lock(&chip->msg_lock);

	lx_message_init(&chip->rmh, CMD_01_GET_SYS_CFG);
	ret = lx_message_send_atomic(chip, &chip->rmh);

	if (ret == 0) {
		freq_raw = chip->rmh.stat[0] >> FREQ_FIELD_OFFSET;
		freq = freq_raw & XES_FREQ_COUNT8_MASK;

		if ((freq < XES_FREQ_COUNT8_48_MAX) ||
		    (freq > XES_FREQ_COUNT8_44_MIN))
			frequency = 0; /* unknown */
		else if (freq >= XES_FREQ_COUNT8_44_MAX)
			frequency = 44100;
		else
			frequency = 48000;
	}

	mutex_unlock(&chip->msg_lock);

	*rfreq = frequency * chip->freq_ratio;

	return ret;
}

int lx_dsp_get_mac(struct lx6464es *chip)
{
	u32 macmsb, maclsb;

	macmsb = lx_dsp_reg_read(chip, eReg_ADMACESMSB) & 0x00FFFFFF;
	maclsb = lx_dsp_reg_read(chip, eReg_ADMACESLSB) & 0x00FFFFFF;

	/* todo: endianess handling */
	chip->mac_address[5] = ((u8 *)(&maclsb))[0];
	chip->mac_address[4] = ((u8 *)(&maclsb))[1];
	chip->mac_address[3] = ((u8 *)(&maclsb))[2];
	chip->mac_address[2] = ((u8 *)(&macmsb))[0];
	chip->mac_address[1] = ((u8 *)(&macmsb))[1];
	chip->mac_address[0] = ((u8 *)(&macmsb))[2];

	return 0;
}


int lx_dsp_set_granularity(struct lx6464es *chip, u32 gran)
{
	int ret;

	mutex_lock(&chip->msg_lock);

	lx_message_init(&chip->rmh, CMD_02_SET_GRANULARITY);
	chip->rmh.cmd[0] |= gran;

	ret = lx_message_send_atomic(chip, &chip->rmh);
	mutex_unlock(&chip->msg_lock);
	return ret;
}

int lx_dsp_read_async_events(struct lx6464es *chip, u32 *data)
{
	int ret;

	mutex_lock(&chip->msg_lock);

	lx_message_init(&chip->rmh, CMD_04_GET_EVENT);
	chip->rmh.stat_len = 9;	/* we don't necessarily need the full length */

	ret = lx_message_send_atomic(chip, &chip->rmh);

	if (!ret)
		memcpy(data, chip->rmh.stat, chip->rmh.stat_len * sizeof(u32));

	mutex_unlock(&chip->msg_lock);
	return ret;
}

#define PIPE_INFO_TO_CMD(capture, pipe)					\
	((u32)((u32)(pipe) | ((capture) ? ID_IS_CAPTURE : 0L)) << ID_OFFSET)



/* low-level pipe handling */
int lx_pipe_allocate(struct lx6464es *chip, u32 pipe, int is_capture,
		     int channels)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_06_ALLOCATE_PIPE);

	chip->rmh.cmd[0] |= pipe_cmd;
	chip->rmh.cmd[0] |= channels;

	err = lx_message_send_atomic(chip, &chip->rmh);
	mutex_unlock(&chip->msg_lock);

	if (err != 0)
		dev_err(chip->card->dev, "could not allocate pipe\n");

	return err;
}

int lx_pipe_release(struct lx6464es *chip, u32 pipe, int is_capture)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_07_RELEASE_PIPE);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);
	mutex_unlock(&chip->msg_lock);

	return err;
}

int lx_buffer_ask(struct lx6464es *chip, u32 pipe, int is_capture,
		  u32 *r_needed, u32 *r_freed, u32 *size_array)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

#ifdef CONFIG_SND_DEBUG
	if (size_array)
		memset(size_array, 0, sizeof(u32)*MAX_STREAM_BUFFER);
#endif

	*r_needed = 0;
	*r_freed = 0;

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_08_ASK_BUFFERS);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);

	if (!err) {
		int i;
		for (i = 0; i < MAX_STREAM_BUFFER; ++i) {
			u32 stat = chip->rmh.stat[i];
			if (stat & (BF_EOB << BUFF_FLAGS_OFFSET)) {
				/* finished */
				*r_freed += 1;
				if (size_array)
					size_array[i] = stat & MASK_DATA_SIZE;
			} else if ((stat & (BF_VALID << BUFF_FLAGS_OFFSET))
				   == 0)
				/* free */
				*r_needed += 1;
		}

		dev_dbg(chip->card->dev,
			"CMD_08_ASK_BUFFERS: needed %d, freed %d\n",
			    *r_needed, *r_freed);
		for (i = 0; i < MAX_STREAM_BUFFER; ++i) {
			for (i = 0; i != chip->rmh.stat_len; ++i)
				dev_dbg(chip->card->dev,
					"  stat[%d]: %x, %x\n", i,
					    chip->rmh.stat[i],
					    chip->rmh.stat[i] & MASK_DATA_SIZE);
		}
	}

	mutex_unlock(&chip->msg_lock);
	return err;
}


int lx_pipe_stop(struct lx6464es *chip, u32 pipe, int is_capture)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_09_STOP_PIPE);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);

	mutex_unlock(&chip->msg_lock);
	return err;
}

static int lx_pipe_toggle_state(struct lx6464es *chip, u32 pipe, int is_capture)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0B_TOGGLE_PIPE_STATE);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);

	mutex_unlock(&chip->msg_lock);
	return err;
}


int lx_pipe_start(struct lx6464es *chip, u32 pipe, int is_capture)
{
	int err;

	err = lx_pipe_wait_for_idle(chip, pipe, is_capture);
	if (err < 0)
		return err;

	err = lx_pipe_toggle_state(chip, pipe, is_capture);

	return err;
}

int lx_pipe_pause(struct lx6464es *chip, u32 pipe, int is_capture)
{
	int err = 0;

	err = lx_pipe_wait_for_start(chip, pipe, is_capture);
	if (err < 0)
		return err;

	err = lx_pipe_toggle_state(chip, pipe, is_capture);

	return err;
}


int lx_pipe_sample_count(struct lx6464es *chip, u32 pipe, int is_capture,
			 u64 *rsample_count)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0A_GET_PIPE_SPL_COUNT);

	chip->rmh.cmd[0] |= pipe_cmd;
	chip->rmh.stat_len = 2;	/* need all words here! */

	err = lx_message_send_atomic(chip, &chip->rmh); /* don't sleep! */

	if (err != 0)
		dev_err(chip->card->dev,
			"could not query pipe's sample count\n");
	else {
		*rsample_count = ((u64)(chip->rmh.stat[0] & MASK_SPL_COUNT_HI)
				  << 24)     /* hi part */
			+ chip->rmh.stat[1]; /* lo part */
	}

	mutex_unlock(&chip->msg_lock);
	return err;
}

int lx_pipe_state(struct lx6464es *chip, u32 pipe, int is_capture, u16 *rstate)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0A_GET_PIPE_SPL_COUNT);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);

	if (err != 0)
		dev_err(chip->card->dev, "could not query pipe's state\n");
	else
		*rstate = (chip->rmh.stat[0] >> PSTATE_OFFSET) & 0x0F;

	mutex_unlock(&chip->msg_lock);
	return err;
}

static int lx_pipe_wait_for_state(struct lx6464es *chip, u32 pipe,
				  int is_capture, u16 state)
{
	int i;

	/* max 2*PCMOnlyGranularity = 2*1024 at 44100 = < 50 ms:
	 * timeout 50 ms */
	for (i = 0; i != 50; ++i) {
		u16 current_state;
		int err = lx_pipe_state(chip, pipe, is_capture, &current_state);

		if (err < 0)
			return err;

		if (!err && current_state == state)
			return 0;

		mdelay(1);
	}

	return -ETIMEDOUT;
}

int lx_pipe_wait_for_start(struct lx6464es *chip, u32 pipe, int is_capture)
{
	return lx_pipe_wait_for_state(chip, pipe, is_capture, PSTATE_RUN);
}

int lx_pipe_wait_for_idle(struct lx6464es *chip, u32 pipe, int is_capture)
{
	return lx_pipe_wait_for_state(chip, pipe, is_capture, PSTATE_IDLE);
}

/* low-level stream handling */
int lx_stream_set_state(struct lx6464es *chip, u32 pipe,
			       int is_capture, enum stream_state_t state)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_13_SET_STREAM_STATE);

	chip->rmh.cmd[0] |= pipe_cmd;
	chip->rmh.cmd[0] |= state;

	err = lx_message_send_atomic(chip, &chip->rmh);
	mutex_unlock(&chip->msg_lock);

	return err;
}

int lx_stream_set_format(struct lx6464es *chip, struct snd_pcm_runtime *runtime,
			 u32 pipe, int is_capture)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);
	u32 channels = runtime->channels;

	if (runtime->channels != channels)
		dev_err(chip->card->dev, "channel count mismatch: %d vs %d",
			   runtime->channels, channels);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0C_DEF_STREAM);

	chip->rmh.cmd[0] |= pipe_cmd;

	if (runtime->sample_bits == 16)
		/* 16 bit format */
		chip->rmh.cmd[0] |= (STREAM_FMT_16b << STREAM_FMT_OFFSET);

	if (snd_pcm_format_little_endian(runtime->format))
		/* little endian/intel format */
		chip->rmh.cmd[0] |= (STREAM_FMT_intel << STREAM_FMT_OFFSET);

	chip->rmh.cmd[0] |= channels-1;

	err = lx_message_send_atomic(chip, &chip->rmh);
	mutex_unlock(&chip->msg_lock);

	return err;
}

int lx_stream_state(struct lx6464es *chip, u32 pipe, int is_capture,
		    int *rstate)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0E_GET_STREAM_SPL_COUNT);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);

	*rstate = (chip->rmh.stat[0] & SF_START) ? START_STATE : PAUSE_STATE;

	mutex_unlock(&chip->msg_lock);
	return err;
}

int lx_stream_sample_position(struct lx6464es *chip, u32 pipe, int is_capture,
			      u64 *r_bytepos)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0E_GET_STREAM_SPL_COUNT);

	chip->rmh.cmd[0] |= pipe_cmd;

	err = lx_message_send_atomic(chip, &chip->rmh);

	*r_bytepos = ((u64) (chip->rmh.stat[0] & MASK_SPL_COUNT_HI)
		      << 32)	     /* hi part */
		+ chip->rmh.stat[1]; /* lo part */

	mutex_unlock(&chip->msg_lock);
	return err;
}

/* low-level buffer handling */
int lx_buffer_give(struct lx6464es *chip, u32 pipe, int is_capture,
		   u32 buffer_size, u32 buf_address_lo, u32 buf_address_hi,
		   u32 *r_buffer_index)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0F_UPDATE_BUFFER);

	chip->rmh.cmd[0] |= pipe_cmd;
	chip->rmh.cmd[0] |= BF_NOTIFY_EOB; /* request interrupt notification */

	/* todo: pause request, circular buffer */

	chip->rmh.cmd[1] = buffer_size & MASK_DATA_SIZE;
	chip->rmh.cmd[2] = buf_address_lo;

	if (buf_address_hi) {
		chip->rmh.cmd_len = 4;
		chip->rmh.cmd[3] = buf_address_hi;
		chip->rmh.cmd[0] |= BF_64BITS_ADR;
	}

	err = lx_message_send_atomic(chip, &chip->rmh);

	if (err == 0) {
		*r_buffer_index = chip->rmh.stat[0];
		goto done;
	}

	if (err == EB_RBUFFERS_TABLE_OVERFLOW)
		dev_err(chip->card->dev,
			"lx_buffer_give EB_RBUFFERS_TABLE_OVERFLOW\n");

	if (err == EB_INVALID_STREAM)
		dev_err(chip->card->dev,
			"lx_buffer_give EB_INVALID_STREAM\n");

	if (err == EB_CMD_REFUSED)
		dev_err(chip->card->dev,
			"lx_buffer_give EB_CMD_REFUSED\n");

 done:
	mutex_unlock(&chip->msg_lock);
	return err;
}

int lx_buffer_free(struct lx6464es *chip, u32 pipe, int is_capture,
		   u32 *r_buffer_size)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_11_CANCEL_BUFFER);

	chip->rmh.cmd[0] |= pipe_cmd;
	chip->rmh.cmd[0] |= MASK_BUFFER_ID; /* ask for the current buffer: the
					     * microblaze will seek for it */

	err = lx_message_send_atomic(chip, &chip->rmh);

	if (err == 0)
		*r_buffer_size = chip->rmh.stat[0]  & MASK_DATA_SIZE;

	mutex_unlock(&chip->msg_lock);
	return err;
}

int lx_buffer_cancel(struct lx6464es *chip, u32 pipe, int is_capture,
		     u32 buffer_index)
{
	int err;
	u32 pipe_cmd = PIPE_INFO_TO_CMD(is_capture, pipe);

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_11_CANCEL_BUFFER);

	chip->rmh.cmd[0] |= pipe_cmd;
	chip->rmh.cmd[0] |= buffer_index;

	err = lx_message_send_atomic(chip, &chip->rmh);

	mutex_unlock(&chip->msg_lock);
	return err;
}


/* low-level gain/peak handling
 *
 * \todo: can we unmute capture/playback channels independently?
 *
 * */
int lx_level_unmute(struct lx6464es *chip, int is_capture, int unmute)
{
	int err;
	/* bit set to 1: channel muted */
	u64 mute_mask = unmute ? 0 : 0xFFFFFFFFFFFFFFFFLLU;

	mutex_lock(&chip->msg_lock);
	lx_message_init(&chip->rmh, CMD_0D_SET_MUTE);

	chip->rmh.cmd[0] |= PIPE_INFO_TO_CMD(is_capture, 0);

	chip->rmh.cmd[1] = (u32)(mute_mask >> (u64)32);	       /* hi part */
	chip->rmh.cmd[2] = (u32)(mute_mask & (u64)0xFFFFFFFF); /* lo part */

	dev_dbg(chip->card->dev,
		"mute %x %x %x\n", chip->rmh.cmd[0], chip->rmh.cmd[1],
		   chip->rmh.cmd[2]);

	err = lx_message_send_atomic(chip, &chip->rmh);

	mutex_unlock(&chip->msg_lock);
	return err;
}

static const u32 peak_map[] = {
	0x00000109, /* -90.308dB */
	0x0000083B, /* -72.247dB */
	0x000020C4, /* -60.205dB */
	0x00008273, /* -48.030dB */
	0x00020756, /* -36.005dB */
	0x00040C37, /* -30.001dB */
	0x00081385, /* -24.002dB */
	0x00101D3F, /* -18.000dB */
	0x0016C310, /* -15.000dB */
	0x002026F2, /* -12.001dB */
	0x002D6A86, /* -9.000dB */
	0x004026E6, /* -6.004dB */
	0x005A9DF6, /* -3.000dB */
	0x0065AC8B, /* -2.000dB */
	0x00721481, /* -1.000dB */
	0x007FFFFF, /* FS */
};

int lx_level_peaks(struct lx6464es *chip, int is_capture, int channels,
		   u32 *r_levels)
{
	int err = 0;
	int i;

	mutex_lock(&chip->msg_lock);
	for (i = 0; i < channels; i += 4) {
		u32 s0, s1, s2, s3;

		lx_message_init(&chip->rmh, CMD_12_GET_PEAK);
		chip->rmh.cmd[0] |= PIPE_INFO_TO_CMD(is_capture, i);

		err = lx_message_send_atomic(chip, &chip->rmh);

		if (err == 0) {
			s0 = peak_map[chip->rmh.stat[0] & 0x0F];
			s1 = peak_map[(chip->rmh.stat[0] >>  4) & 0xf];
			s2 = peak_map[(chip->rmh.stat[0] >>  8) & 0xf];
			s3 = peak_map[(chip->rmh.stat[0] >>  12) & 0xf];
		} else
			s0 = s1 = s2 = s3 = 0;

		r_levels[0] = s0;
		r_levels[1] = s1;
		r_levels[2] = s2;
		r_levels[3] = s3;

		r_levels += 4;
	}

	mutex_unlock(&chip->msg_lock);
	return err;
}

/* interrupt handling */
#define PCX_IRQ_NONE 0
#define IRQCS_ACTIVE_PCIDB	BIT(13)
#define IRQCS_ENABLE_PCIIRQ	BIT(8)
#define IRQCS_ENABLE_PCIDB	BIT(9)

static u32 lx_interrupt_test_ack(struct lx6464es *chip)
{
	u32 irqcs = lx_plx_reg_read(chip, ePLX_IRQCS);

	/* Test if PCI Doorbell interrupt is active */
	if (irqcs & IRQCS_ACTIVE_PCIDB)	{
		u32 temp;
		irqcs = PCX_IRQ_NONE;

		while ((temp = lx_plx_reg_read(chip, ePLX_L2PCIDB))) {
			/* RAZ interrupt */
			irqcs |= temp;
			lx_plx_reg_write(chip, ePLX_L2PCIDB, temp);
		}

		return irqcs;
	}
	return PCX_IRQ_NONE;
}

static int lx_interrupt_ack(struct lx6464es *chip, u32 *r_irqsrc,
			    int *r_async_pending, int *r_async_escmd)
{
	u32 irq_async;
	u32 irqsrc = lx_interrupt_test_ack(chip);

	if (irqsrc == PCX_IRQ_NONE)
		return 0;

	*r_irqsrc = irqsrc;

	irq_async = irqsrc & MASK_SYS_ASYNC_EVENTS; /* + EtherSound response
						     * (set by xilinx) + EOB */

	if (irq_async & MASK_SYS_STATUS_ESA) {
		irq_async &= ~MASK_SYS_STATUS_ESA;
		*r_async_escmd = 1;
	}

	if (irq_async) {
		/* dev_dbg(chip->card->dev, "interrupt: async event pending\n"); */
		*r_async_pending = 1;
	}

	return 1;
}

static int lx_interrupt_handle_async_events(struct lx6464es *chip, u32 irqsrc,
					    int *r_freq_changed,
					    u64 *r_notified_in_pipe_mask,
					    u64 *r_notified_out_pipe_mask)
{
	int err;
	u32 stat[9];		/* answer from CMD_04_GET_EVENT */

	/* We can optimize this to not read dumb events.
	 * Answer words are in the following order:
	 * Stat[0]	general status
	 * Stat[1]	end of buffer OUT pF
	 * Stat[2]	end of buffer OUT pf
	 * Stat[3]	end of buffer IN pF
	 * Stat[4]	end of buffer IN pf
	 * Stat[5]	MSB underrun
	 * Stat[6]	LSB underrun
	 * Stat[7]	MSB overrun
	 * Stat[8]	LSB overrun
	 * */

	int eb_pending_out = (irqsrc & MASK_SYS_STATUS_EOBO) ? 1 : 0;
	int eb_pending_in  = (irqsrc & MASK_SYS_STATUS_EOBI) ? 1 : 0;

	*r_freq_changed = (irqsrc & MASK_SYS_STATUS_FREQ) ? 1 : 0;

	err = lx_dsp_read_async_events(chip, stat);
	if (err < 0)
		return err;

	if (eb_pending_in) {
		*r_notified_in_pipe_mask = ((u64)stat[3] << 32)
			+ stat[4];
		dev_dbg(chip->card->dev, "interrupt: EOBI pending %llx\n",
			    *r_notified_in_pipe_mask);
	}
	if (eb_pending_out) {
		*r_notified_out_pipe_mask = ((u64)stat[1] << 32)
			+ stat[2];
		dev_dbg(chip->card->dev, "interrupt: EOBO pending %llx\n",
			    *r_notified_out_pipe_mask);
	}

	/* todo: handle xrun notification */

	return err;
}

static int lx_interrupt_request_new_buffer(struct lx6464es *chip,
					   struct lx_stream *lx_stream)
{
	struct snd_pcm_substream *substream = lx_stream->stream;
	const unsigned int is_capture = lx_stream->is_capture;
	int err;

	const u32 channels = substream->runtime->channels;
	const u32 bytes_per_frame = channels * 3;
	const u32 period_size = substream->runtime->period_size;
	const u32 period_bytes = period_size * bytes_per_frame;
	const u32 pos = lx_stream->frame_pos;
	const u32 next_pos = ((pos+1) == substream->runtime->periods) ?
		0 : pos + 1;

	dma_addr_t buf = substream->dma_buffer.addr + pos * period_bytes;
	u32 buf_hi = 0;
	u32 buf_lo = 0;
	u32 buffer_index = 0;

	u32 needed, freed;
	u32 size_array[MAX_STREAM_BUFFER];

	dev_dbg(chip->card->dev, "->lx_interrupt_request_new_buffer\n");

	mutex_lock(&chip->lock);

	err = lx_buffer_ask(chip, 0, is_capture, &needed, &freed, size_array);
	dev_dbg(chip->card->dev,
		"interrupt: needed %d, freed %d\n", needed, freed);

	unpack_pointer(buf, &buf_lo, &buf_hi);
	err = lx_buffer_give(chip, 0, is_capture, period_bytes, buf_lo, buf_hi,
			     &buffer_index);
	dev_dbg(chip->card->dev,
		"interrupt: gave buffer index %x on 0x%lx (%d bytes)\n",
		    buffer_index, (unsigned long)buf, period_bytes);

	lx_stream->frame_pos = next_pos;
	mutex_unlock(&chip->lock);

	return err;
}

irqreturn_t lx_interrupt(int irq, void *dev_id)
{
	struct lx6464es *chip = dev_id;
	int async_pending, async_escmd;
	u32 irqsrc;
	bool wake_thread = false;

	dev_dbg(chip->card->dev,
		"**************************************************\n");

	if (!lx_interrupt_ack(chip, &irqsrc, &async_pending, &async_escmd)) {
		dev_dbg(chip->card->dev, "IRQ_NONE\n");
		return IRQ_NONE; /* this device did not cause the interrupt */
	}

	if (irqsrc & MASK_SYS_STATUS_CMD_DONE)
		return IRQ_HANDLED;

	if (irqsrc & MASK_SYS_STATUS_EOBI)
		dev_dbg(chip->card->dev, "interrupt: EOBI\n");

	if (irqsrc & MASK_SYS_STATUS_EOBO)
		dev_dbg(chip->card->dev, "interrupt: EOBO\n");

	if (irqsrc & MASK_SYS_STATUS_URUN)
		dev_dbg(chip->card->dev, "interrupt: URUN\n");

	if (irqsrc & MASK_SYS_STATUS_ORUN)
		dev_dbg(chip->card->dev, "interrupt: ORUN\n");

	if (async_pending) {
		wake_thread = true;
		chip->irqsrc = irqsrc;
	}

	if (async_escmd) {
		/* backdoor for ethersound commands
		 *
		 * for now, we do not need this
		 *
		 * */

		dev_dbg(chip->card->dev, "interrupt requests escmd handling\n");
	}

	return wake_thread ? IRQ_WAKE_THREAD : IRQ_HANDLED;
}

irqreturn_t lx_threaded_irq(int irq, void *dev_id)
{
	struct lx6464es *chip = dev_id;
	u64 notified_in_pipe_mask = 0;
	u64 notified_out_pipe_mask = 0;
	int freq_changed;
	int err;

	/* handle async events */
	err = lx_interrupt_handle_async_events(chip, chip->irqsrc,
					       &freq_changed,
					       &notified_in_pipe_mask,
					       &notified_out_pipe_mask);
	if (err)
		dev_err(chip->card->dev, "error handling async events\n");

	if (notified_in_pipe_mask) {
		struct lx_stream *lx_stream = &chip->capture_stream;

		dev_dbg(chip->card->dev,
			"requesting audio transfer for capture\n");
		err = lx_interrupt_request_new_buffer(chip, lx_stream);
		if (err < 0)
			dev_err(chip->card->dev,
				"cannot request new buffer for capture\n");
		snd_pcm_period_elapsed(lx_stream->stream);
	}

	if (notified_out_pipe_mask) {
		struct lx_stream *lx_stream = &chip->playback_stream;

		dev_dbg(chip->card->dev,
			"requesting audio transfer for playback\n");
		err = lx_interrupt_request_new_buffer(chip, lx_stream);
		if (err < 0)
			dev_err(chip->card->dev,
				"cannot request new buffer for playback\n");
		snd_pcm_period_elapsed(lx_stream->stream);
	}

	return IRQ_HANDLED;
}


static void lx_irq_set(struct lx6464es *chip, int enable)
{
	u32 reg = lx_plx_reg_read(chip, ePLX_IRQCS);

	/* enable/disable interrupts
	 *
	 * Set the Doorbell and PCI interrupt enable bits
	 *
	 * */
	if (enable)
		reg |=  (IRQCS_ENABLE_PCIIRQ | IRQCS_ENABLE_PCIDB);
	else
		reg &= ~(IRQCS_ENABLE_PCIIRQ | IRQCS_ENABLE_PCIDB);
	lx_plx_reg_write(chip, ePLX_IRQCS, reg);
}

void lx_irq_enable(struct lx6464es *chip)
{
	dev_dbg(chip->card->dev, "->lx_irq_enable\n");
	lx_irq_set(chip, 1);
}

void lx_irq_disable(struct lx6464es *chip)
{
	dev_dbg(chip->card->dev, "->lx_irq_disable\n");
	lx_irq_set(chip, 0);
}

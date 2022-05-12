// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *  Copyright (C) 2004 Aurelien Alleaume <slts@free.fr>
 */

#include <linux/slab.h>
#include "pvrusb2-eeprom.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"

#define trace_eeprom(...) pvr2_trace(PVR2_TRACE_EEPROM,__VA_ARGS__)



/*

   Read and analyze data in the eeprom.  Use tveeprom to figure out
   the packet structure, since this is another Hauppauge device and
   internally it has a family resemblance to ivtv-type devices

*/

#include <media/tveeprom.h>

/* We seem to only be interested in the last 128 bytes of the EEPROM */
#define EEPROM_SIZE 128

/* Grab EEPROM contents, needed for direct method. */
static u8 *pvr2_eeprom_fetch(struct pvr2_hdw *hdw)
{
	struct i2c_msg msg[2];
	u8 *eeprom;
	u8 iadd[2];
	u8 addr;
	u16 eepromSize;
	unsigned int offs;
	int ret;
	int mode16 = 0;
	unsigned pcnt,tcnt;
	eeprom = kzalloc(EEPROM_SIZE, GFP_KERNEL);
	if (!eeprom) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Failed to allocate memory required to read eeprom");
		return NULL;
	}

	trace_eeprom("Value for eeprom addr from controller was 0x%x",
		     hdw->eeprom_addr);
	addr = hdw->eeprom_addr;
	/* Seems that if the high bit is set, then the *real* eeprom
	   address is shifted right now bit position (noticed this in
	   newer PVR USB2 hardware) */
	if (addr & 0x80) addr >>= 1;

	/* FX2 documentation states that a 16bit-addressed eeprom is
	   expected if the I2C address is an odd number (yeah, this is
	   strange but it's what they do) */
	mode16 = (addr & 1);
	eepromSize = (mode16 ? 4096 : 256);
	trace_eeprom("Examining %d byte eeprom at location 0x%x using %d bit addressing",
		     eepromSize, addr,
		     mode16 ? 16 : 8);

	msg[0].addr = addr;
	msg[0].flags = 0;
	msg[0].len = mode16 ? 2 : 1;
	msg[0].buf = iadd;
	msg[1].addr = addr;
	msg[1].flags = I2C_M_RD;

	/* We have to do the actual eeprom data fetch ourselves, because
	   (1) we're only fetching part of the eeprom, and (2) if we were
	   getting the whole thing our I2C driver can't grab it in one
	   pass - which is what tveeprom is otherwise going to attempt */
	for (tcnt = 0; tcnt < EEPROM_SIZE; tcnt += pcnt) {
		pcnt = 16;
		if (pcnt + tcnt > EEPROM_SIZE) pcnt = EEPROM_SIZE-tcnt;
		offs = tcnt + (eepromSize - EEPROM_SIZE);
		if (mode16) {
			iadd[0] = offs >> 8;
			iadd[1] = offs;
		} else {
			iadd[0] = offs;
		}
		msg[1].len = pcnt;
		msg[1].buf = eeprom+tcnt;
		if ((ret = i2c_transfer(&hdw->i2c_adap,
					msg,ARRAY_SIZE(msg))) != 2) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "eeprom fetch set offs err=%d",ret);
			kfree(eeprom);
			return NULL;
		}
	}
	return eeprom;
}


/* Directly call eeprom analysis function within tveeprom. */
int pvr2_eeprom_analyze(struct pvr2_hdw *hdw)
{
	u8 *eeprom;
	struct tveeprom tvdata;

	memset(&tvdata,0,sizeof(tvdata));

	eeprom = pvr2_eeprom_fetch(hdw);
	if (!eeprom)
		return -EINVAL;

	tveeprom_hauppauge_analog(&tvdata, eeprom);

	trace_eeprom("eeprom assumed v4l tveeprom module");
	trace_eeprom("eeprom direct call results:");
	trace_eeprom("has_radio=%d",tvdata.has_radio);
	trace_eeprom("tuner_type=%d",tvdata.tuner_type);
	trace_eeprom("tuner_formats=0x%x",tvdata.tuner_formats);
	trace_eeprom("audio_processor=%d",tvdata.audio_processor);
	trace_eeprom("model=%d",tvdata.model);
	trace_eeprom("revision=%d",tvdata.revision);
	trace_eeprom("serial_number=%d",tvdata.serial_number);
	trace_eeprom("rev_str=%s",tvdata.rev_str);
	hdw->tuner_type = tvdata.tuner_type;
	hdw->tuner_updated = !0;
	hdw->serial_number = tvdata.serial_number;
	hdw->std_mask_eeprom = tvdata.tuner_formats;

	kfree(eeprom);

	return 0;
}

/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * dds.h - sysfs attributes associated with DDS devices
 *
 * Copyright (c) 2010 Analog Devices Inc.
 */
#ifndef IIO_DDS_H_
#define IIO_DDS_H_

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_frequencyY
 */

#define IIO_DEV_ATTR_FREQ(_channel, _num, _mode, _show, _store, _addr)	\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_frequency##_num,	\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_frequencyY_scale
 */

#define IIO_CONST_ATTR_FREQ_SCALE(_channel, _string)			\
	IIO_CONST_ATTR(out_altvoltage##_channel##_frequency_scale, _string)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_frequencysymbol
 */

#define IIO_DEV_ATTR_FREQSYMBOL(_channel, _mode, _show, _store, _addr)	\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_frequencysymbol,	\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_phaseY
 */

#define IIO_DEV_ATTR_PHASE(_channel, _num, _mode, _show, _store, _addr)	\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_phase##_num,		\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_phaseY_scale
 */

#define IIO_CONST_ATTR_PHASE_SCALE(_channel, _string)			\
	IIO_CONST_ATTR(out_altvoltage##_channel##_phase_scale, _string)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_phasesymbol
 */

#define IIO_DEV_ATTR_PHASESYMBOL(_channel, _mode, _show, _store, _addr)	\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_phasesymbol,		\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_pincontrol_en
 */

#define IIO_DEV_ATTR_PINCONTROL_EN(_channel, _mode, _show, _store, _addr)\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_pincontrol_en,	\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_pincontrol_frequency_en
 */

#define IIO_DEV_ATTR_PINCONTROL_FREQ_EN(_channel, _mode, _show, _store, _addr)\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_pincontrol_frequency_en,\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_pincontrol_phase_en
 */

#define IIO_DEV_ATTR_PINCONTROL_PHASE_EN(_channel, _mode, _show, _store, _addr)\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_pincontrol_phase_en,	\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_out_enable
 */

#define IIO_DEV_ATTR_OUT_ENABLE(_channel, _mode, _show, _store, _addr)	\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_out_enable,		\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_outY_enable
 */

#define IIO_DEV_ATTR_OUTY_ENABLE(_channel, _output,			\
			_mode, _show, _store, _addr)			\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_out##_output##_enable,\
			_mode, _show, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_outY_wavetype
 */

#define IIO_DEV_ATTR_OUT_WAVETYPE(_channel, _output, _store, _addr)	\
	IIO_DEVICE_ATTR(out_altvoltage##_channel##_out##_output##_wavetype,\
			0200, NULL, _store, _addr)

/**
 * /sys/bus/iio/devices/.../out_altvoltageX_outY_wavetype_available
 */

#define IIO_CONST_ATTR_OUT_WAVETYPES_AVAILABLE(_channel, _output, _modes)\
	IIO_CONST_ATTR(							\
	out_altvoltage##_channel##_out##_output##_wavetype_available, _modes)

#endif /* IIO_DDS_H_ */

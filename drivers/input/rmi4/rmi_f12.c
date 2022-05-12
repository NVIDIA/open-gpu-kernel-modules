// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012-2016 Synaptics Incorporated
 */
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/rmi.h>
#include "rmi_driver.h"
#include "rmi_2d_sensor.h"

enum rmi_f12_object_type {
	RMI_F12_OBJECT_NONE			= 0x00,
	RMI_F12_OBJECT_FINGER			= 0x01,
	RMI_F12_OBJECT_STYLUS			= 0x02,
	RMI_F12_OBJECT_PALM			= 0x03,
	RMI_F12_OBJECT_UNCLASSIFIED		= 0x04,
	RMI_F12_OBJECT_GLOVED_FINGER		= 0x06,
	RMI_F12_OBJECT_NARROW_OBJECT		= 0x07,
	RMI_F12_OBJECT_HAND_EDGE		= 0x08,
	RMI_F12_OBJECT_COVER			= 0x0A,
	RMI_F12_OBJECT_STYLUS_2			= 0x0B,
	RMI_F12_OBJECT_ERASER			= 0x0C,
	RMI_F12_OBJECT_SMALL_OBJECT		= 0x0D,
};

#define F12_DATA1_BYTES_PER_OBJ			8

struct f12_data {
	struct rmi_2d_sensor sensor;
	struct rmi_2d_sensor_platform_data sensor_pdata;
	bool has_dribble;

	u16 data_addr;

	struct rmi_register_descriptor query_reg_desc;
	struct rmi_register_descriptor control_reg_desc;
	struct rmi_register_descriptor data_reg_desc;

	/* F12 Data1 describes sensed objects */
	const struct rmi_register_desc_item *data1;
	u16 data1_offset;

	/* F12 Data5 describes finger ACM */
	const struct rmi_register_desc_item *data5;
	u16 data5_offset;

	/* F12 Data5 describes Pen */
	const struct rmi_register_desc_item *data6;
	u16 data6_offset;


	/* F12 Data9 reports relative data */
	const struct rmi_register_desc_item *data9;
	u16 data9_offset;

	const struct rmi_register_desc_item *data15;
	u16 data15_offset;

	unsigned long *abs_mask;
	unsigned long *rel_mask;
};

static int rmi_f12_read_sensor_tuning(struct f12_data *f12)
{
	const struct rmi_register_desc_item *item;
	struct rmi_2d_sensor *sensor = &f12->sensor;
	struct rmi_function *fn = sensor->fn;
	struct rmi_device *rmi_dev = fn->rmi_dev;
	int ret;
	int offset;
	u8 buf[15];
	int pitch_x = 0;
	int pitch_y = 0;
	int rx_receivers = 0;
	int tx_receivers = 0;

	item = rmi_get_register_desc_item(&f12->control_reg_desc, 8);
	if (!item) {
		dev_err(&fn->dev,
			"F12 does not have the sensor tuning control register\n");
		return -ENODEV;
	}

	offset = rmi_register_desc_calc_reg_offset(&f12->control_reg_desc, 8);

	if (item->reg_size > sizeof(buf)) {
		dev_err(&fn->dev,
			"F12 control8 should be no bigger than %zd bytes, not: %ld\n",
			sizeof(buf), item->reg_size);
		return -ENODEV;
	}

	ret = rmi_read_block(rmi_dev, fn->fd.control_base_addr + offset, buf,
				item->reg_size);
	if (ret)
		return ret;

	offset = 0;
	if (rmi_register_desc_has_subpacket(item, 0)) {
		sensor->max_x = (buf[offset + 1] << 8) | buf[offset];
		sensor->max_y = (buf[offset + 3] << 8) | buf[offset + 2];
		offset += 4;
	}

	rmi_dbg(RMI_DEBUG_FN, &fn->dev, "%s: max_x: %d max_y: %d\n", __func__,
		sensor->max_x, sensor->max_y);

	if (rmi_register_desc_has_subpacket(item, 1)) {
		pitch_x = (buf[offset + 1] << 8) | buf[offset];
		pitch_y	= (buf[offset + 3] << 8) | buf[offset + 2];
		offset += 4;
	}

	if (rmi_register_desc_has_subpacket(item, 2)) {
		/* Units 1/128 sensor pitch */
		rmi_dbg(RMI_DEBUG_FN, &fn->dev,
			"%s: Inactive Border xlo:%d xhi:%d ylo:%d yhi:%d\n",
			__func__,
			buf[offset], buf[offset + 1],
			buf[offset + 2], buf[offset + 3]);

		offset += 4;
	}

	if (rmi_register_desc_has_subpacket(item, 3)) {
		rx_receivers = buf[offset];
		tx_receivers = buf[offset + 1];
		offset += 2;
	}

	/* Skip over sensor flags */
	if (rmi_register_desc_has_subpacket(item, 4))
		offset += 1;

	sensor->x_mm = (pitch_x * rx_receivers) >> 12;
	sensor->y_mm = (pitch_y * tx_receivers) >> 12;

	rmi_dbg(RMI_DEBUG_FN, &fn->dev, "%s: x_mm: %d y_mm: %d\n", __func__,
		sensor->x_mm, sensor->y_mm);

	return 0;
}

static void rmi_f12_process_objects(struct f12_data *f12, u8 *data1, int size)
{
	int i;
	struct rmi_2d_sensor *sensor = &f12->sensor;
	int objects = f12->data1->num_subpackets;

	if ((f12->data1->num_subpackets * F12_DATA1_BYTES_PER_OBJ) > size)
		objects = size / F12_DATA1_BYTES_PER_OBJ;

	for (i = 0; i < objects; i++) {
		struct rmi_2d_sensor_abs_object *obj = &sensor->objs[i];

		obj->type = RMI_2D_OBJECT_NONE;
		obj->mt_tool = MT_TOOL_FINGER;

		switch (data1[0]) {
		case RMI_F12_OBJECT_FINGER:
			obj->type = RMI_2D_OBJECT_FINGER;
			break;
		case RMI_F12_OBJECT_STYLUS:
			obj->type = RMI_2D_OBJECT_STYLUS;
			obj->mt_tool = MT_TOOL_PEN;
			break;
		case RMI_F12_OBJECT_PALM:
			obj->type = RMI_2D_OBJECT_PALM;
			obj->mt_tool = MT_TOOL_PALM;
			break;
		case RMI_F12_OBJECT_UNCLASSIFIED:
			obj->type = RMI_2D_OBJECT_UNCLASSIFIED;
			break;
		}

		obj->x = (data1[2] << 8) | data1[1];
		obj->y = (data1[4] << 8) | data1[3];
		obj->z = data1[5];
		obj->wx = data1[6];
		obj->wy = data1[7];

		rmi_2d_sensor_abs_process(sensor, obj, i);

		data1 += F12_DATA1_BYTES_PER_OBJ;
	}

	if (sensor->kernel_tracking)
		input_mt_assign_slots(sensor->input,
				      sensor->tracking_slots,
				      sensor->tracking_pos,
				      sensor->nbr_fingers,
				      sensor->dmax);

	for (i = 0; i < objects; i++)
		rmi_2d_sensor_abs_report(sensor, &sensor->objs[i], i);
}

static irqreturn_t rmi_f12_attention(int irq, void *ctx)
{
	int retval;
	struct rmi_function *fn = ctx;
	struct rmi_device *rmi_dev = fn->rmi_dev;
	struct rmi_driver_data *drvdata = dev_get_drvdata(&rmi_dev->dev);
	struct f12_data *f12 = dev_get_drvdata(&fn->dev);
	struct rmi_2d_sensor *sensor = &f12->sensor;
	int valid_bytes = sensor->pkt_size;

	if (drvdata->attn_data.data) {
		if (sensor->attn_size > drvdata->attn_data.size)
			valid_bytes = drvdata->attn_data.size;
		else
			valid_bytes = sensor->attn_size;
		memcpy(sensor->data_pkt, drvdata->attn_data.data,
			valid_bytes);
		drvdata->attn_data.data += valid_bytes;
		drvdata->attn_data.size -= valid_bytes;
	} else {
		retval = rmi_read_block(rmi_dev, f12->data_addr,
					sensor->data_pkt, sensor->pkt_size);
		if (retval < 0) {
			dev_err(&fn->dev, "Failed to read object data. Code: %d.\n",
				retval);
			return IRQ_RETVAL(retval);
		}
	}

	if (f12->data1)
		rmi_f12_process_objects(f12,
			&sensor->data_pkt[f12->data1_offset], valid_bytes);

	input_mt_sync_frame(sensor->input);

	return IRQ_HANDLED;
}

static int rmi_f12_write_control_regs(struct rmi_function *fn)
{
	int ret;
	const struct rmi_register_desc_item *item;
	struct rmi_device *rmi_dev = fn->rmi_dev;
	struct f12_data *f12 = dev_get_drvdata(&fn->dev);
	int control_size;
	char buf[3];
	u16 control_offset = 0;
	u8 subpacket_offset = 0;

	if (f12->has_dribble
	    && (f12->sensor.dribble != RMI_REG_STATE_DEFAULT)) {
		item = rmi_get_register_desc_item(&f12->control_reg_desc, 20);
		if (item) {
			control_offset = rmi_register_desc_calc_reg_offset(
						&f12->control_reg_desc, 20);

			/*
			 * The byte containing the EnableDribble bit will be
			 * in either byte 0 or byte 2 of control 20. Depending
			 * on the existence of subpacket 0. If control 20 is
			 * larger then 3 bytes, just read the first 3.
			 */
			control_size = min(item->reg_size, 3UL);

			ret = rmi_read_block(rmi_dev, fn->fd.control_base_addr
					+ control_offset, buf, control_size);
			if (ret)
				return ret;

			if (rmi_register_desc_has_subpacket(item, 0))
				subpacket_offset += 1;

			switch (f12->sensor.dribble) {
			case RMI_REG_STATE_OFF:
				buf[subpacket_offset] &= ~BIT(2);
				break;
			case RMI_REG_STATE_ON:
				buf[subpacket_offset] |= BIT(2);
				break;
			case RMI_REG_STATE_DEFAULT:
			default:
				break;
			}

			ret = rmi_write_block(rmi_dev,
				fn->fd.control_base_addr + control_offset,
				buf, control_size);
			if (ret)
				return ret;
		}
	}

	return 0;

}

static int rmi_f12_config(struct rmi_function *fn)
{
	struct rmi_driver *drv = fn->rmi_dev->driver;
	struct f12_data *f12 = dev_get_drvdata(&fn->dev);
	struct rmi_2d_sensor *sensor;
	int ret;

	sensor = &f12->sensor;

	if (!sensor->report_abs)
		drv->clear_irq_bits(fn->rmi_dev, f12->abs_mask);
	else
		drv->set_irq_bits(fn->rmi_dev, f12->abs_mask);

	drv->clear_irq_bits(fn->rmi_dev, f12->rel_mask);

	ret = rmi_f12_write_control_regs(fn);
	if (ret)
		dev_warn(&fn->dev,
			"Failed to write F12 control registers: %d\n", ret);

	return 0;
}

static int rmi_f12_probe(struct rmi_function *fn)
{
	struct f12_data *f12;
	int ret;
	struct rmi_device *rmi_dev = fn->rmi_dev;
	char buf;
	u16 query_addr = fn->fd.query_base_addr;
	const struct rmi_register_desc_item *item;
	struct rmi_2d_sensor *sensor;
	struct rmi_device_platform_data *pdata = rmi_get_platform_data(rmi_dev);
	struct rmi_driver_data *drvdata = dev_get_drvdata(&rmi_dev->dev);
	u16 data_offset = 0;
	int mask_size;

	rmi_dbg(RMI_DEBUG_FN, &fn->dev, "%s\n", __func__);

	mask_size = BITS_TO_LONGS(drvdata->irq_count) * sizeof(unsigned long);

	ret = rmi_read(fn->rmi_dev, query_addr, &buf);
	if (ret < 0) {
		dev_err(&fn->dev, "Failed to read general info register: %d\n",
			ret);
		return -ENODEV;
	}
	++query_addr;

	if (!(buf & BIT(0))) {
		dev_err(&fn->dev,
			"Behavior of F12 without register descriptors is undefined.\n");
		return -ENODEV;
	}

	f12 = devm_kzalloc(&fn->dev, sizeof(struct f12_data) + mask_size * 2,
			GFP_KERNEL);
	if (!f12)
		return -ENOMEM;

	f12->abs_mask = (unsigned long *)((char *)f12
			+ sizeof(struct f12_data));
	f12->rel_mask = (unsigned long *)((char *)f12
			+ sizeof(struct f12_data) + mask_size);

	set_bit(fn->irq_pos, f12->abs_mask);
	set_bit(fn->irq_pos + 1, f12->rel_mask);

	f12->has_dribble = !!(buf & BIT(3));

	if (fn->dev.of_node) {
		ret = rmi_2d_sensor_of_probe(&fn->dev, &f12->sensor_pdata);
		if (ret)
			return ret;
	} else {
		f12->sensor_pdata = pdata->sensor_pdata;
	}

	ret = rmi_read_register_desc(rmi_dev, query_addr,
					&f12->query_reg_desc);
	if (ret) {
		dev_err(&fn->dev,
			"Failed to read the Query Register Descriptor: %d\n",
			ret);
		return ret;
	}
	query_addr += 3;

	ret = rmi_read_register_desc(rmi_dev, query_addr,
						&f12->control_reg_desc);
	if (ret) {
		dev_err(&fn->dev,
			"Failed to read the Control Register Descriptor: %d\n",
			ret);
		return ret;
	}
	query_addr += 3;

	ret = rmi_read_register_desc(rmi_dev, query_addr,
						&f12->data_reg_desc);
	if (ret) {
		dev_err(&fn->dev,
			"Failed to read the Data Register Descriptor: %d\n",
			ret);
		return ret;
	}
	query_addr += 3;

	sensor = &f12->sensor;
	sensor->fn = fn;
	f12->data_addr = fn->fd.data_base_addr;
	sensor->pkt_size = rmi_register_desc_calc_size(&f12->data_reg_desc);

	sensor->axis_align =
		f12->sensor_pdata.axis_align;

	sensor->x_mm = f12->sensor_pdata.x_mm;
	sensor->y_mm = f12->sensor_pdata.y_mm;
	sensor->dribble = f12->sensor_pdata.dribble;

	if (sensor->sensor_type == rmi_sensor_default)
		sensor->sensor_type =
			f12->sensor_pdata.sensor_type;

	rmi_dbg(RMI_DEBUG_FN, &fn->dev, "%s: data packet size: %d\n", __func__,
		sensor->pkt_size);
	sensor->data_pkt = devm_kzalloc(&fn->dev, sensor->pkt_size, GFP_KERNEL);
	if (!sensor->data_pkt)
		return -ENOMEM;

	dev_set_drvdata(&fn->dev, f12);

	ret = rmi_f12_read_sensor_tuning(f12);
	if (ret)
		return ret;

	/*
	 * Figure out what data is contained in the data registers. HID devices
	 * may have registers defined, but their data is not reported in the
	 * HID attention report. Registers which are not reported in the HID
	 * attention report check to see if the device is receiving data from
	 * HID attention reports.
	 */
	item = rmi_get_register_desc_item(&f12->data_reg_desc, 0);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 1);
	if (item) {
		f12->data1 = item;
		f12->data1_offset = data_offset;
		data_offset += item->reg_size;
		sensor->nbr_fingers = item->num_subpackets;
		sensor->report_abs = 1;
		sensor->attn_size += item->reg_size;
	}

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 2);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 3);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 4);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 5);
	if (item) {
		f12->data5 = item;
		f12->data5_offset = data_offset;
		data_offset += item->reg_size;
		sensor->attn_size += item->reg_size;
	}

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 6);
	if (item && !drvdata->attn_data.data) {
		f12->data6 = item;
		f12->data6_offset = data_offset;
		data_offset += item->reg_size;
	}

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 7);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 8);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 9);
	if (item && !drvdata->attn_data.data) {
		f12->data9 = item;
		f12->data9_offset = data_offset;
		data_offset += item->reg_size;
		if (!sensor->report_abs)
			sensor->report_rel = 1;
	}

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 10);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 11);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 12);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 13);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 14);
	if (item && !drvdata->attn_data.data)
		data_offset += item->reg_size;

	item = rmi_get_register_desc_item(&f12->data_reg_desc, 15);
	if (item && !drvdata->attn_data.data) {
		f12->data15 = item;
		f12->data15_offset = data_offset;
		data_offset += item->reg_size;
	}

	/* allocate the in-kernel tracking buffers */
	sensor->tracking_pos = devm_kcalloc(&fn->dev,
			sensor->nbr_fingers, sizeof(struct input_mt_pos),
			GFP_KERNEL);
	sensor->tracking_slots = devm_kcalloc(&fn->dev,
			sensor->nbr_fingers, sizeof(int), GFP_KERNEL);
	sensor->objs = devm_kcalloc(&fn->dev,
			sensor->nbr_fingers,
			sizeof(struct rmi_2d_sensor_abs_object),
			GFP_KERNEL);
	if (!sensor->tracking_pos || !sensor->tracking_slots || !sensor->objs)
		return -ENOMEM;

	ret = rmi_2d_sensor_configure_input(fn, sensor);
	if (ret)
		return ret;

	return 0;
}

struct rmi_function_handler rmi_f12_handler = {
	.driver = {
		.name = "rmi4_f12",
	},
	.func = 0x12,
	.probe = rmi_f12_probe,
	.config = rmi_f12_config,
	.attention = rmi_f12_attention,
};

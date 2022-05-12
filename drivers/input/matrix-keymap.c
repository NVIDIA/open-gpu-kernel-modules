// SPDX-License-Identifier: GPL-2.0-only
/*
 * Helpers for matrix keyboard bindings
 *
 * Copyright (C) 2012 Google, Inc
 *
 * Author:
 *	Olof Johansson <olof@lixom.net>
 */

#include <linux/device.h>
#include <linux/export.h>
#include <linux/gfp.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/types.h>

static bool matrix_keypad_map_key(struct input_dev *input_dev,
				  unsigned int rows, unsigned int cols,
				  unsigned int row_shift, unsigned int key)
{
	unsigned short *keymap = input_dev->keycode;
	unsigned int row = KEY_ROW(key);
	unsigned int col = KEY_COL(key);
	unsigned short code = KEY_VAL(key);

	if (row >= rows || col >= cols) {
		dev_err(input_dev->dev.parent,
			"%s: invalid keymap entry 0x%x (row: %d, col: %d, rows: %d, cols: %d)\n",
			__func__, key, row, col, rows, cols);
		return false;
	}

	keymap[MATRIX_SCAN_CODE(row, col, row_shift)] = code;
	__set_bit(code, input_dev->keybit);

	return true;
}

/**
 * matrix_keypad_parse_properties() - Read properties of matrix keypad
 *
 * @dev: Device containing properties
 * @rows: Returns number of matrix rows
 * @cols: Returns number of matrix columns
 * @return 0 if OK, <0 on error
 */
int matrix_keypad_parse_properties(struct device *dev,
				   unsigned int *rows, unsigned int *cols)
{
	*rows = *cols = 0;

	device_property_read_u32(dev, "keypad,num-rows", rows);
	device_property_read_u32(dev, "keypad,num-columns", cols);

	if (!*rows || !*cols) {
		dev_err(dev, "number of keypad rows/columns not specified\n");
		return -EINVAL;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(matrix_keypad_parse_properties);

static int matrix_keypad_parse_keymap(const char *propname,
				      unsigned int rows, unsigned int cols,
				      struct input_dev *input_dev)
{
	struct device *dev = input_dev->dev.parent;
	unsigned int row_shift = get_count_order(cols);
	unsigned int max_keys = rows << row_shift;
	u32 *keys;
	int i;
	int size;
	int retval;

	if (!propname)
		propname = "linux,keymap";

	size = device_property_count_u32(dev, propname);
	if (size <= 0) {
		dev_err(dev, "missing or malformed property %s: %d\n",
			propname, size);
		return size < 0 ? size : -EINVAL;
	}

	if (size > max_keys) {
		dev_err(dev, "%s size overflow (%d vs max %u)\n",
			propname, size, max_keys);
		return -EINVAL;
	}

	keys = kmalloc_array(size, sizeof(u32), GFP_KERNEL);
	if (!keys)
		return -ENOMEM;

	retval = device_property_read_u32_array(dev, propname, keys, size);
	if (retval) {
		dev_err(dev, "failed to read %s property: %d\n",
			propname, retval);
		goto out;
	}

	for (i = 0; i < size; i++) {
		if (!matrix_keypad_map_key(input_dev, rows, cols,
					   row_shift, keys[i])) {
			retval = -EINVAL;
			goto out;
		}
	}

	retval = 0;

out:
	kfree(keys);
	return retval;
}

/**
 * matrix_keypad_build_keymap - convert platform keymap into matrix keymap
 * @keymap_data: keymap supplied by the platform code
 * @keymap_name: name of device tree property containing keymap (if device
 *	tree support is enabled).
 * @rows: number of rows in target keymap array
 * @cols: number of cols in target keymap array
 * @keymap: expanded version of keymap that is suitable for use by
 * matrix keyboard driver
 * @input_dev: input devices for which we are setting up the keymap
 *
 * This function converts platform keymap (encoded with KEY() macro) into
 * an array of keycodes that is suitable for using in a standard matrix
 * keyboard driver that uses row and col as indices.
 *
 * If @keymap_data is not supplied and device tree support is enabled
 * it will attempt load the keymap from property specified by @keymap_name
 * argument (or "linux,keymap" if @keymap_name is %NULL).
 *
 * If @keymap is %NULL the function will automatically allocate managed
 * block of memory to store the keymap. This memory will be associated with
 * the parent device and automatically freed when device unbinds from the
 * driver.
 *
 * Callers are expected to set up input_dev->dev.parent before calling this
 * function.
 */
int matrix_keypad_build_keymap(const struct matrix_keymap_data *keymap_data,
			       const char *keymap_name,
			       unsigned int rows, unsigned int cols,
			       unsigned short *keymap,
			       struct input_dev *input_dev)
{
	unsigned int row_shift = get_count_order(cols);
	size_t max_keys = rows << row_shift;
	int i;
	int error;

	if (WARN_ON(!input_dev->dev.parent))
		return -EINVAL;

	if (!keymap) {
		keymap = devm_kcalloc(input_dev->dev.parent,
				      max_keys, sizeof(*keymap),
				      GFP_KERNEL);
		if (!keymap) {
			dev_err(input_dev->dev.parent,
				"Unable to allocate memory for keymap");
			return -ENOMEM;
		}
	}

	input_dev->keycode = keymap;
	input_dev->keycodesize = sizeof(*keymap);
	input_dev->keycodemax = max_keys;

	__set_bit(EV_KEY, input_dev->evbit);

	if (keymap_data) {
		for (i = 0; i < keymap_data->keymap_size; i++) {
			unsigned int key = keymap_data->keymap[i];

			if (!matrix_keypad_map_key(input_dev, rows, cols,
						   row_shift, key))
				return -EINVAL;
		}
	} else {
		error = matrix_keypad_parse_keymap(keymap_name, rows, cols,
						   input_dev);
		if (error)
			return error;
	}

	__clear_bit(KEY_RESERVED, input_dev->keybit);

	return 0;
}
EXPORT_SYMBOL(matrix_keypad_build_keymap);

MODULE_LICENSE("GPL");

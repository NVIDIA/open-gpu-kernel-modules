// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/drivers/input/keyboard/pxa27x_keypad.c
 *
 * Driver for the pxa27x matrix keyboard controller.
 *
 * Created:	Feb 22, 2007
 * Author:	Rodolfo Giometti <giometti@linux.it>
 *
 * Based on a previous implementations by Kevin O'Connor
 * <kevin_at_koconnor.net> and Alex Osborne <bobofdoom@gmail.com> and
 * on some suggestions by Nicolas Pitre <nico@fluxnic.net>.
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/input/matrix_keypad.h>
#include <linux/slab.h>
#include <linux/of.h>

#include <linux/platform_data/keypad-pxa27x.h>
/*
 * Keypad Controller registers
 */
#define KPC             0x0000 /* Keypad Control register */
#define KPDK            0x0008 /* Keypad Direct Key register */
#define KPREC           0x0010 /* Keypad Rotary Encoder register */
#define KPMK            0x0018 /* Keypad Matrix Key register */
#define KPAS            0x0020 /* Keypad Automatic Scan register */

/* Keypad Automatic Scan Multiple Key Presser register 0-3 */
#define KPASMKP0        0x0028
#define KPASMKP1        0x0030
#define KPASMKP2        0x0038
#define KPASMKP3        0x0040
#define KPKDI           0x0048

/* bit definitions */
#define KPC_MKRN(n)	((((n) - 1) & 0x7) << 26) /* matrix key row number */
#define KPC_MKCN(n)	((((n) - 1) & 0x7) << 23) /* matrix key column number */
#define KPC_DKN(n)	((((n) - 1) & 0x7) << 6)  /* direct key number */

#define KPC_AS          (0x1 << 30)  /* Automatic Scan bit */
#define KPC_ASACT       (0x1 << 29)  /* Automatic Scan on Activity */
#define KPC_MI          (0x1 << 22)  /* Matrix interrupt bit */
#define KPC_IMKP        (0x1 << 21)  /* Ignore Multiple Key Press */

#define KPC_MS(n)	(0x1 << (13 + (n)))	/* Matrix scan line 'n' */
#define KPC_MS_ALL      (0xff << 13)

#define KPC_ME          (0x1 << 12)  /* Matrix Keypad Enable */
#define KPC_MIE         (0x1 << 11)  /* Matrix Interrupt Enable */
#define KPC_DK_DEB_SEL	(0x1 <<  9)  /* Direct Keypad Debounce Select */
#define KPC_DI          (0x1 <<  5)  /* Direct key interrupt bit */
#define KPC_RE_ZERO_DEB (0x1 <<  4)  /* Rotary Encoder Zero Debounce */
#define KPC_REE1        (0x1 <<  3)  /* Rotary Encoder1 Enable */
#define KPC_REE0        (0x1 <<  2)  /* Rotary Encoder0 Enable */
#define KPC_DE          (0x1 <<  1)  /* Direct Keypad Enable */
#define KPC_DIE         (0x1 <<  0)  /* Direct Keypad interrupt Enable */

#define KPDK_DKP        (0x1 << 31)
#define KPDK_DK(n)	((n) & 0xff)

#define KPREC_OF1       (0x1 << 31)
#define kPREC_UF1       (0x1 << 30)
#define KPREC_OF0       (0x1 << 15)
#define KPREC_UF0       (0x1 << 14)

#define KPREC_RECOUNT0(n)	((n) & 0xff)
#define KPREC_RECOUNT1(n)	(((n) >> 16) & 0xff)

#define KPMK_MKP        (0x1 << 31)
#define KPAS_SO         (0x1 << 31)
#define KPASMKPx_SO     (0x1 << 31)

#define KPAS_MUKP(n)	(((n) >> 26) & 0x1f)
#define KPAS_RP(n)	(((n) >> 4) & 0xf)
#define KPAS_CP(n)	((n) & 0xf)

#define KPASMKP_MKC_MASK	(0xff)

#define keypad_readl(off)	__raw_readl(keypad->mmio_base + (off))
#define keypad_writel(off, v)	__raw_writel((v), keypad->mmio_base + (off))

#define MAX_MATRIX_KEY_NUM	(MAX_MATRIX_KEY_ROWS * MAX_MATRIX_KEY_COLS)
#define MAX_KEYPAD_KEYS		(MAX_MATRIX_KEY_NUM + MAX_DIRECT_KEY_NUM)

struct pxa27x_keypad {
	const struct pxa27x_keypad_platform_data *pdata;

	struct clk *clk;
	struct input_dev *input_dev;
	void __iomem *mmio_base;

	int irq;

	unsigned short keycodes[MAX_KEYPAD_KEYS];
	int rotary_rel_code[2];

	unsigned int row_shift;

	/* state row bits of each column scan */
	uint32_t matrix_key_state[MAX_MATRIX_KEY_COLS];
	uint32_t direct_key_state;

	unsigned int direct_key_mask;
};

#ifdef CONFIG_OF
static int pxa27x_keypad_matrix_key_parse_dt(struct pxa27x_keypad *keypad,
				struct pxa27x_keypad_platform_data *pdata)
{
	struct input_dev *input_dev = keypad->input_dev;
	struct device *dev = input_dev->dev.parent;
	u32 rows, cols;
	int error;

	error = matrix_keypad_parse_properties(dev, &rows, &cols);
	if (error)
		return error;

	if (rows > MAX_MATRIX_KEY_ROWS || cols > MAX_MATRIX_KEY_COLS) {
		dev_err(dev, "rows or cols exceeds maximum value\n");
		return -EINVAL;
	}

	pdata->matrix_key_rows = rows;
	pdata->matrix_key_cols = cols;

	error = matrix_keypad_build_keymap(NULL, NULL,
					   pdata->matrix_key_rows,
					   pdata->matrix_key_cols,
					   keypad->keycodes, input_dev);
	if (error)
		return error;

	return 0;
}

static int pxa27x_keypad_direct_key_parse_dt(struct pxa27x_keypad *keypad,
				struct pxa27x_keypad_platform_data *pdata)
{
	struct input_dev *input_dev = keypad->input_dev;
	struct device *dev = input_dev->dev.parent;
	struct device_node *np = dev->of_node;
	const __be16 *prop;
	unsigned short code;
	unsigned int proplen, size;
	int i;
	int error;

	error = of_property_read_u32(np, "marvell,direct-key-count",
				     &pdata->direct_key_num);
	if (error) {
		/*
		 * If do not have marvel,direct-key-count defined,
		 * it means direct key is not supported.
		 */
		return error == -EINVAL ? 0 : error;
	}

	error = of_property_read_u32(np, "marvell,direct-key-mask",
				     &pdata->direct_key_mask);
	if (error) {
		if (error != -EINVAL)
			return error;

		/*
		 * If marvell,direct-key-mask is not defined, driver will use
		 * default value. Default value is set when configure the keypad.
		 */
		pdata->direct_key_mask = 0;
	}

	pdata->direct_key_low_active = of_property_read_bool(np,
					"marvell,direct-key-low-active");

	prop = of_get_property(np, "marvell,direct-key-map", &proplen);
	if (!prop)
		return -EINVAL;

	if (proplen % sizeof(u16))
		return -EINVAL;

	size = proplen / sizeof(u16);

	/* Only MAX_DIRECT_KEY_NUM is accepted.*/
	if (size > MAX_DIRECT_KEY_NUM)
		return -EINVAL;

	for (i = 0; i < size; i++) {
		code = be16_to_cpup(prop + i);
		keypad->keycodes[MAX_MATRIX_KEY_NUM + i] = code;
		__set_bit(code, input_dev->keybit);
	}

	return 0;
}

static int pxa27x_keypad_rotary_parse_dt(struct pxa27x_keypad *keypad,
				struct pxa27x_keypad_platform_data *pdata)
{
	const __be32 *prop;
	int i, relkey_ret;
	unsigned int code, proplen;
	const char *rotaryname[2] = {
			"marvell,rotary0", "marvell,rotary1"};
	const char relkeyname[] = {"marvell,rotary-rel-key"};
	struct input_dev *input_dev = keypad->input_dev;
	struct device *dev = input_dev->dev.parent;
	struct device_node *np = dev->of_node;

	relkey_ret = of_property_read_u32(np, relkeyname, &code);
	/* if can read correct rotary key-code, we do not need this. */
	if (relkey_ret == 0) {
		unsigned short relcode;

		/* rotary0 taks lower half, rotary1 taks upper half. */
		relcode = code & 0xffff;
		pdata->rotary0_rel_code = (code & 0xffff);
		__set_bit(relcode, input_dev->relbit);

		relcode = code >> 16;
		pdata->rotary1_rel_code = relcode;
		__set_bit(relcode, input_dev->relbit);
	}

	for (i = 0; i < 2; i++) {
		prop = of_get_property(np, rotaryname[i], &proplen);
		/*
		 * If the prop is not set, it means keypad does not need
		 * initialize the rotaryX.
		 */
		if (!prop)
			continue;

		code = be32_to_cpup(prop);
		/*
		 * Not all up/down key code are valid.
		 * Now we depends on direct-rel-code.
		 */
		if ((!(code & 0xffff) || !(code >> 16)) && relkey_ret) {
			return relkey_ret;
		} else {
			unsigned int n = MAX_MATRIX_KEY_NUM + (i << 1);
			unsigned short keycode;

			keycode = code & 0xffff;
			keypad->keycodes[n] = keycode;
			__set_bit(keycode, input_dev->keybit);

			keycode = code >> 16;
			keypad->keycodes[n + 1] = keycode;
			__set_bit(keycode, input_dev->keybit);

			if (i == 0)
				pdata->rotary0_rel_code = -1;
			else
				pdata->rotary1_rel_code = -1;
		}
		if (i == 0)
			pdata->enable_rotary0 = 1;
		else
			pdata->enable_rotary1 = 1;
	}

	keypad->rotary_rel_code[0] = pdata->rotary0_rel_code;
	keypad->rotary_rel_code[1] = pdata->rotary1_rel_code;

	return 0;
}

static int pxa27x_keypad_build_keycode_from_dt(struct pxa27x_keypad *keypad)
{
	struct input_dev *input_dev = keypad->input_dev;
	struct device *dev = input_dev->dev.parent;
	struct device_node *np = dev->of_node;
	struct pxa27x_keypad_platform_data *pdata;
	int error;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		dev_err(dev, "failed to allocate memory for pdata\n");
		return -ENOMEM;
	}

	error = pxa27x_keypad_matrix_key_parse_dt(keypad, pdata);
	if (error) {
		dev_err(dev, "failed to parse matrix key\n");
		return error;
	}

	error = pxa27x_keypad_direct_key_parse_dt(keypad, pdata);
	if (error) {
		dev_err(dev, "failed to parse direct key\n");
		return error;
	}

	error = pxa27x_keypad_rotary_parse_dt(keypad, pdata);
	if (error) {
		dev_err(dev, "failed to parse rotary key\n");
		return error;
	}

	error = of_property_read_u32(np, "marvell,debounce-interval",
				     &pdata->debounce_interval);
	if (error) {
		dev_err(dev, "failed to parse debounce-interval\n");
		return error;
	}

	/*
	 * The keycodes may not only includes matrix key but also the direct
	 * key or rotary key.
	 */
	input_dev->keycodemax = ARRAY_SIZE(keypad->keycodes);

	keypad->pdata = pdata;
	return 0;
}

#else

static int pxa27x_keypad_build_keycode_from_dt(struct pxa27x_keypad *keypad)
{
	dev_info(keypad->input_dev->dev.parent, "missing platform data\n");

	return -EINVAL;
}

#endif

static int pxa27x_keypad_build_keycode(struct pxa27x_keypad *keypad)
{
	const struct pxa27x_keypad_platform_data *pdata = keypad->pdata;
	struct input_dev *input_dev = keypad->input_dev;
	unsigned short keycode;
	int i;
	int error;

	error = matrix_keypad_build_keymap(pdata->matrix_keymap_data, NULL,
					   pdata->matrix_key_rows,
					   pdata->matrix_key_cols,
					   keypad->keycodes, input_dev);
	if (error)
		return error;

	/*
	 * The keycodes may not only include matrix keys but also the direct
	 * or rotary keys.
	 */
	input_dev->keycodemax = ARRAY_SIZE(keypad->keycodes);

	/* For direct keys. */
	for (i = 0; i < pdata->direct_key_num; i++) {
		keycode = pdata->direct_key_map[i];
		keypad->keycodes[MAX_MATRIX_KEY_NUM + i] = keycode;
		__set_bit(keycode, input_dev->keybit);
	}

	if (pdata->enable_rotary0) {
		if (pdata->rotary0_up_key && pdata->rotary0_down_key) {
			keycode = pdata->rotary0_up_key;
			keypad->keycodes[MAX_MATRIX_KEY_NUM + 0] = keycode;
			__set_bit(keycode, input_dev->keybit);

			keycode = pdata->rotary0_down_key;
			keypad->keycodes[MAX_MATRIX_KEY_NUM + 1] = keycode;
			__set_bit(keycode, input_dev->keybit);

			keypad->rotary_rel_code[0] = -1;
		} else {
			keypad->rotary_rel_code[0] = pdata->rotary0_rel_code;
			__set_bit(pdata->rotary0_rel_code, input_dev->relbit);
		}
	}

	if (pdata->enable_rotary1) {
		if (pdata->rotary1_up_key && pdata->rotary1_down_key) {
			keycode = pdata->rotary1_up_key;
			keypad->keycodes[MAX_MATRIX_KEY_NUM + 2] = keycode;
			__set_bit(keycode, input_dev->keybit);

			keycode = pdata->rotary1_down_key;
			keypad->keycodes[MAX_MATRIX_KEY_NUM + 3] = keycode;
			__set_bit(keycode, input_dev->keybit);

			keypad->rotary_rel_code[1] = -1;
		} else {
			keypad->rotary_rel_code[1] = pdata->rotary1_rel_code;
			__set_bit(pdata->rotary1_rel_code, input_dev->relbit);
		}
	}

	__clear_bit(KEY_RESERVED, input_dev->keybit);

	return 0;
}

static void pxa27x_keypad_scan_matrix(struct pxa27x_keypad *keypad)
{
	const struct pxa27x_keypad_platform_data *pdata = keypad->pdata;
	struct input_dev *input_dev = keypad->input_dev;
	int row, col, num_keys_pressed = 0;
	uint32_t new_state[MAX_MATRIX_KEY_COLS];
	uint32_t kpas = keypad_readl(KPAS);

	num_keys_pressed = KPAS_MUKP(kpas);

	memset(new_state, 0, sizeof(new_state));

	if (num_keys_pressed == 0)
		goto scan;

	if (num_keys_pressed == 1) {
		col = KPAS_CP(kpas);
		row = KPAS_RP(kpas);

		/* if invalid row/col, treat as no key pressed */
		if (col >= pdata->matrix_key_cols ||
		    row >= pdata->matrix_key_rows)
			goto scan;

		new_state[col] = (1 << row);
		goto scan;
	}

	if (num_keys_pressed > 1) {
		uint32_t kpasmkp0 = keypad_readl(KPASMKP0);
		uint32_t kpasmkp1 = keypad_readl(KPASMKP1);
		uint32_t kpasmkp2 = keypad_readl(KPASMKP2);
		uint32_t kpasmkp3 = keypad_readl(KPASMKP3);

		new_state[0] = kpasmkp0 & KPASMKP_MKC_MASK;
		new_state[1] = (kpasmkp0 >> 16) & KPASMKP_MKC_MASK;
		new_state[2] = kpasmkp1 & KPASMKP_MKC_MASK;
		new_state[3] = (kpasmkp1 >> 16) & KPASMKP_MKC_MASK;
		new_state[4] = kpasmkp2 & KPASMKP_MKC_MASK;
		new_state[5] = (kpasmkp2 >> 16) & KPASMKP_MKC_MASK;
		new_state[6] = kpasmkp3 & KPASMKP_MKC_MASK;
		new_state[7] = (kpasmkp3 >> 16) & KPASMKP_MKC_MASK;
	}
scan:
	for (col = 0; col < pdata->matrix_key_cols; col++) {
		uint32_t bits_changed;
		int code;

		bits_changed = keypad->matrix_key_state[col] ^ new_state[col];
		if (bits_changed == 0)
			continue;

		for (row = 0; row < pdata->matrix_key_rows; row++) {
			if ((bits_changed & (1 << row)) == 0)
				continue;

			code = MATRIX_SCAN_CODE(row, col, keypad->row_shift);

			input_event(input_dev, EV_MSC, MSC_SCAN, code);
			input_report_key(input_dev, keypad->keycodes[code],
					 new_state[col] & (1 << row));
		}
	}
	input_sync(input_dev);
	memcpy(keypad->matrix_key_state, new_state, sizeof(new_state));
}

#define DEFAULT_KPREC	(0x007f007f)

static inline int rotary_delta(uint32_t kprec)
{
	if (kprec & KPREC_OF0)
		return (kprec & 0xff) + 0x7f;
	else if (kprec & KPREC_UF0)
		return (kprec & 0xff) - 0x7f - 0xff;
	else
		return (kprec & 0xff) - 0x7f;
}

static void report_rotary_event(struct pxa27x_keypad *keypad, int r, int delta)
{
	struct input_dev *dev = keypad->input_dev;

	if (delta == 0)
		return;

	if (keypad->rotary_rel_code[r] == -1) {
		int code = MAX_MATRIX_KEY_NUM + 2 * r + (delta > 0 ? 0 : 1);
		unsigned char keycode = keypad->keycodes[code];

		/* simulate a press-n-release */
		input_event(dev, EV_MSC, MSC_SCAN, code);
		input_report_key(dev, keycode, 1);
		input_sync(dev);
		input_event(dev, EV_MSC, MSC_SCAN, code);
		input_report_key(dev, keycode, 0);
		input_sync(dev);
	} else {
		input_report_rel(dev, keypad->rotary_rel_code[r], delta);
		input_sync(dev);
	}
}

static void pxa27x_keypad_scan_rotary(struct pxa27x_keypad *keypad)
{
	const struct pxa27x_keypad_platform_data *pdata = keypad->pdata;
	uint32_t kprec;

	/* read and reset to default count value */
	kprec = keypad_readl(KPREC);
	keypad_writel(KPREC, DEFAULT_KPREC);

	if (pdata->enable_rotary0)
		report_rotary_event(keypad, 0, rotary_delta(kprec));

	if (pdata->enable_rotary1)
		report_rotary_event(keypad, 1, rotary_delta(kprec >> 16));
}

static void pxa27x_keypad_scan_direct(struct pxa27x_keypad *keypad)
{
	const struct pxa27x_keypad_platform_data *pdata = keypad->pdata;
	struct input_dev *input_dev = keypad->input_dev;
	unsigned int new_state;
	uint32_t kpdk, bits_changed;
	int i;

	kpdk = keypad_readl(KPDK);

	if (pdata->enable_rotary0 || pdata->enable_rotary1)
		pxa27x_keypad_scan_rotary(keypad);

	/*
	 * The KPDR_DK only output the key pin level, so it relates to board,
	 * and low level may be active.
	 */
	if (pdata->direct_key_low_active)
		new_state = ~KPDK_DK(kpdk) & keypad->direct_key_mask;
	else
		new_state = KPDK_DK(kpdk) & keypad->direct_key_mask;

	bits_changed = keypad->direct_key_state ^ new_state;

	if (bits_changed == 0)
		return;

	for (i = 0; i < pdata->direct_key_num; i++) {
		if (bits_changed & (1 << i)) {
			int code = MAX_MATRIX_KEY_NUM + i;

			input_event(input_dev, EV_MSC, MSC_SCAN, code);
			input_report_key(input_dev, keypad->keycodes[code],
					 new_state & (1 << i));
		}
	}
	input_sync(input_dev);
	keypad->direct_key_state = new_state;
}

static void clear_wakeup_event(struct pxa27x_keypad *keypad)
{
	const struct pxa27x_keypad_platform_data *pdata = keypad->pdata;

	if (pdata->clear_wakeup_event)
		(pdata->clear_wakeup_event)();
}

static irqreturn_t pxa27x_keypad_irq_handler(int irq, void *dev_id)
{
	struct pxa27x_keypad *keypad = dev_id;
	unsigned long kpc = keypad_readl(KPC);

	clear_wakeup_event(keypad);

	if (kpc & KPC_DI)
		pxa27x_keypad_scan_direct(keypad);

	if (kpc & KPC_MI)
		pxa27x_keypad_scan_matrix(keypad);

	return IRQ_HANDLED;
}

static void pxa27x_keypad_config(struct pxa27x_keypad *keypad)
{
	const struct pxa27x_keypad_platform_data *pdata = keypad->pdata;
	unsigned int mask = 0, direct_key_num = 0;
	unsigned long kpc = 0;

	/* clear pending interrupt bit */
	keypad_readl(KPC);

	/* enable matrix keys with automatic scan */
	if (pdata->matrix_key_rows && pdata->matrix_key_cols) {
		kpc |= KPC_ASACT | KPC_MIE | KPC_ME | KPC_MS_ALL;
		kpc |= KPC_MKRN(pdata->matrix_key_rows) |
		       KPC_MKCN(pdata->matrix_key_cols);
	}

	/* enable rotary key, debounce interval same as direct keys */
	if (pdata->enable_rotary0) {
		mask |= 0x03;
		direct_key_num = 2;
		kpc |= KPC_REE0;
	}

	if (pdata->enable_rotary1) {
		mask |= 0x0c;
		direct_key_num = 4;
		kpc |= KPC_REE1;
	}

	if (pdata->direct_key_num > direct_key_num)
		direct_key_num = pdata->direct_key_num;

	/*
	 * Direct keys usage may not start from KP_DKIN0, check the platfrom
	 * mask data to config the specific.
	 */
	if (pdata->direct_key_mask)
		keypad->direct_key_mask = pdata->direct_key_mask;
	else
		keypad->direct_key_mask = ((1 << direct_key_num) - 1) & ~mask;

	/* enable direct key */
	if (direct_key_num)
		kpc |= KPC_DE | KPC_DIE | KPC_DKN(direct_key_num);

	keypad_writel(KPC, kpc | KPC_RE_ZERO_DEB);
	keypad_writel(KPREC, DEFAULT_KPREC);
	keypad_writel(KPKDI, pdata->debounce_interval);
}

static int pxa27x_keypad_open(struct input_dev *dev)
{
	struct pxa27x_keypad *keypad = input_get_drvdata(dev);
	int ret;
	/* Enable unit clock */
	ret = clk_prepare_enable(keypad->clk);
	if (ret)
		return ret;

	pxa27x_keypad_config(keypad);

	return 0;
}

static void pxa27x_keypad_close(struct input_dev *dev)
{
	struct pxa27x_keypad *keypad = input_get_drvdata(dev);

	/* Disable clock unit */
	clk_disable_unprepare(keypad->clk);
}

#ifdef CONFIG_PM_SLEEP
static int pxa27x_keypad_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct pxa27x_keypad *keypad = platform_get_drvdata(pdev);

	/*
	 * If the keypad is used a wake up source, clock can not be disabled.
	 * Or it can not detect the key pressing.
	 */
	if (device_may_wakeup(&pdev->dev))
		enable_irq_wake(keypad->irq);
	else
		clk_disable_unprepare(keypad->clk);

	return 0;
}

static int pxa27x_keypad_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct pxa27x_keypad *keypad = platform_get_drvdata(pdev);
	struct input_dev *input_dev = keypad->input_dev;
	int ret = 0;

	/*
	 * If the keypad is used as wake up source, the clock is not turned
	 * off. So do not need configure it again.
	 */
	if (device_may_wakeup(&pdev->dev)) {
		disable_irq_wake(keypad->irq);
	} else {
		mutex_lock(&input_dev->mutex);

		if (input_device_enabled(input_dev)) {
			/* Enable unit clock */
			ret = clk_prepare_enable(keypad->clk);
			if (!ret)
				pxa27x_keypad_config(keypad);
		}

		mutex_unlock(&input_dev->mutex);
	}

	return ret;
}
#endif

static SIMPLE_DEV_PM_OPS(pxa27x_keypad_pm_ops,
			 pxa27x_keypad_suspend, pxa27x_keypad_resume);


static int pxa27x_keypad_probe(struct platform_device *pdev)
{
	const struct pxa27x_keypad_platform_data *pdata =
					dev_get_platdata(&pdev->dev);
	struct device_node *np = pdev->dev.of_node;
	struct pxa27x_keypad *keypad;
	struct input_dev *input_dev;
	struct resource *res;
	int irq, error;

	/* Driver need build keycode from device tree or pdata */
	if (!np && !pdata)
		return -EINVAL;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return -ENXIO;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to get I/O memory\n");
		return -ENXIO;
	}

	keypad = devm_kzalloc(&pdev->dev, sizeof(*keypad),
			      GFP_KERNEL);
	if (!keypad)
		return -ENOMEM;

	input_dev = devm_input_allocate_device(&pdev->dev);
	if (!input_dev)
		return -ENOMEM;

	keypad->pdata = pdata;
	keypad->input_dev = input_dev;
	keypad->irq = irq;

	keypad->mmio_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(keypad->mmio_base))
		return PTR_ERR(keypad->mmio_base);

	keypad->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(keypad->clk)) {
		dev_err(&pdev->dev, "failed to get keypad clock\n");
		return PTR_ERR(keypad->clk);
	}

	input_dev->name = pdev->name;
	input_dev->id.bustype = BUS_HOST;
	input_dev->open = pxa27x_keypad_open;
	input_dev->close = pxa27x_keypad_close;
	input_dev->dev.parent = &pdev->dev;

	input_dev->keycode = keypad->keycodes;
	input_dev->keycodesize = sizeof(keypad->keycodes[0]);
	input_dev->keycodemax = ARRAY_SIZE(keypad->keycodes);

	input_set_drvdata(input_dev, keypad);

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
	input_set_capability(input_dev, EV_MSC, MSC_SCAN);

	if (pdata) {
		error = pxa27x_keypad_build_keycode(keypad);
	} else {
		error = pxa27x_keypad_build_keycode_from_dt(keypad);
		/*
		 * Data that we get from DT resides in dynamically
		 * allocated memory so we need to update our pdata
		 * pointer.
		 */
		pdata = keypad->pdata;
	}
	if (error) {
		dev_err(&pdev->dev, "failed to build keycode\n");
		return error;
	}

	keypad->row_shift = get_count_order(pdata->matrix_key_cols);

	if ((pdata->enable_rotary0 && keypad->rotary_rel_code[0] != -1) ||
	    (pdata->enable_rotary1 && keypad->rotary_rel_code[1] != -1)) {
		input_dev->evbit[0] |= BIT_MASK(EV_REL);
	}

	error = devm_request_irq(&pdev->dev, irq, pxa27x_keypad_irq_handler,
				 0, pdev->name, keypad);
	if (error) {
		dev_err(&pdev->dev, "failed to request IRQ\n");
		return error;
	}

	/* Register the input device */
	error = input_register_device(input_dev);
	if (error) {
		dev_err(&pdev->dev, "failed to register input device\n");
		return error;
	}

	platform_set_drvdata(pdev, keypad);
	device_init_wakeup(&pdev->dev, 1);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id pxa27x_keypad_dt_match[] = {
	{ .compatible = "marvell,pxa27x-keypad" },
	{},
};
MODULE_DEVICE_TABLE(of, pxa27x_keypad_dt_match);
#endif

static struct platform_driver pxa27x_keypad_driver = {
	.probe		= pxa27x_keypad_probe,
	.driver		= {
		.name	= "pxa27x-keypad",
		.of_match_table = of_match_ptr(pxa27x_keypad_dt_match),
		.pm	= &pxa27x_keypad_pm_ops,
	},
};
module_platform_driver(pxa27x_keypad_driver);

MODULE_DESCRIPTION("PXA27x Keypad Controller Driver");
MODULE_LICENSE("GPL");
/* work with hotplug and coldplug */
MODULE_ALIAS("platform:pxa27x-keypad");

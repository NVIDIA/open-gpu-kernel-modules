/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Samsung CF-ATA platform_device info
*/

#ifndef __ATA_SAMSUNG_CF_H
#define __ATA_SAMSUNG_CF_H __FILE__

/**
 * struct s3c_ide_platdata - S3C IDE driver platform data.
 * @setup_gpio: Setup the external GPIO pins to the right state for data
 * transfer in true-ide mode.
 */
struct s3c_ide_platdata {
	void (*setup_gpio)(void);
};

/*
 * s3c_ide_set_platdata() - Setup the platform specifc data for IDE driver.
 * @pdata: Platform data for IDE driver.
 */
extern void s3c_ide_set_platdata(struct s3c_ide_platdata *pdata);

/* architecture-specific IDE configuration */
extern void s3c64xx_ide_setup_gpio(void);
extern void s5pv210_ide_setup_gpio(void);

#endif /*__ATA_SAMSUNG_CF_H */

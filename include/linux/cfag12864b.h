/* SPDX-License-Identifier: GPL-2.0 */
/*
 *    Filename: cfag12864b.h
 *     Version: 0.1.0
 * Description: cfag12864b LCD driver header
 *
 *      Author: Copyright (C) Miguel Ojeda <ojeda@kernel.org>
 *        Date: 2006-10-12
 */

#ifndef _CFAG12864B_H_
#define _CFAG12864B_H_

#define CFAG12864B_WIDTH	(128)
#define CFAG12864B_HEIGHT	(64)
#define CFAG12864B_CONTROLLERS	(2)
#define CFAG12864B_PAGES	(8)
#define CFAG12864B_ADDRESSES	(64)
#define CFAG12864B_SIZE		((CFAG12864B_CONTROLLERS) * \
				(CFAG12864B_PAGES) * \
				(CFAG12864B_ADDRESSES))

/*
 * The driver will blit this buffer to the LCD
 *
 * Its size is CFAG12864B_SIZE.
 */
extern unsigned char * cfag12864b_buffer;

/*
 * Get the refresh rate of the LCD
 *
 * Returns the refresh rate (hertz).
 */
extern unsigned int cfag12864b_getrate(void);

/*
 * Enable refreshing
 *
 * Returns 0 if successful (anyone was using it),
 * or != 0 if failed (someone is using it).
 */
extern unsigned char cfag12864b_enable(void);

/*
 * Disable refreshing
 *
 * You should call this only when you finish using the LCD.
 */
extern void cfag12864b_disable(void);

/*
 * Is enabled refreshing? (is anyone using the module?)
 *
 * Returns 0 if refreshing is not enabled (anyone is using it),
 * or != 0 if refreshing is enabled (someone is using it).
 *
 * Useful for buffer read-only modules.
 */
extern unsigned char cfag12864b_isenabled(void);

/*
 * Is the module inited?
 */
extern unsigned char cfag12864b_isinited(void);

#endif /* _CFAG12864B_H_ */


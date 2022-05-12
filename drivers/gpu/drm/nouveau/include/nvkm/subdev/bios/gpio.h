/* SPDX-License-Identifier: MIT */
#ifndef __NVBIOS_GPIO_H__
#define __NVBIOS_GPIO_H__
enum dcb_gpio_func_name {
	DCB_GPIO_PANEL_POWER = 0x01,
	DCB_GPIO_FAN = 0x09,
	DCB_GPIO_TVDAC0 = 0x0c,
	DCB_GPIO_THERM_EXT_POWER_EVENT = 0x10,
	DCB_GPIO_TVDAC1 = 0x2d,
	DCB_GPIO_FAN_SENSE = 0x3d,
	DCB_GPIO_POWER_ALERT = 0x4c,
	DCB_GPIO_EXT_POWER_LOW = 0x79,
	DCB_GPIO_LOGO_LED_PWM = 0x84,
	DCB_GPIO_UNUSED = 0xff,
	DCB_GPIO_VID0 = 0x04,
	DCB_GPIO_VID1 = 0x05,
	DCB_GPIO_VID2 = 0x06,
	DCB_GPIO_VID3 = 0x1a,
	DCB_GPIO_VID4 = 0x73,
	DCB_GPIO_VID5 = 0x74,
	DCB_GPIO_VID6 = 0x75,
	DCB_GPIO_VID7 = 0x76,
	DCB_GPIO_VID_PWM = 0x81,
};

#define DCB_GPIO_LOG_DIR     0x02
#define DCB_GPIO_LOG_DIR_OUT 0x00
#define DCB_GPIO_LOG_DIR_IN  0x02
#define DCB_GPIO_LOG_VAL     0x01
#define DCB_GPIO_LOG_VAL_LO  0x00
#define DCB_GPIO_LOG_VAL_HI  0x01

struct dcb_gpio_func {
	u8 func;
	u8 line;
	u8 log[2];

	/* so far, "param" seems to only have an influence on PWM-related
	 * GPIOs such as FAN_CONTROL and PANEL_BACKLIGHT_LEVEL.
	 * if param equals 1, hardware PWM is available
	 * if param equals 0, the host should toggle the GPIO itself
	 */
	u8 param;
};

u16 dcb_gpio_table(struct nvkm_bios *, u8 *ver, u8 *hdr, u8 *cnt, u8 *len);
u16 dcb_gpio_entry(struct nvkm_bios *, int idx, int ent, u8 *ver, u8 *len);
u16 dcb_gpio_parse(struct nvkm_bios *, int idx, int ent, u8 *ver, u8 *len,
		   struct dcb_gpio_func *);
u16 dcb_gpio_match(struct nvkm_bios *, int idx, u8 func, u8 line,
		   u8 *ver, u8 *len, struct dcb_gpio_func *);
#endif

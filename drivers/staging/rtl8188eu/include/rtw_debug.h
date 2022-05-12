/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTW_DEBUG_H__
#define __RTW_DEBUG_H__

#include <osdep_service.h>
#include <drv_types.h>

#define DRIVERVERSION	"v4.1.4_6773.20130222"
#define _drv_always_			1
#define _drv_emerg_			2
#define _drv_alert_			3
#define _drv_crit_			4
#define _drv_err_			5
#define	_drv_warning_			6
#define _drv_notice_			7
#define _drv_info_			8
#define	_drv_debug_			9

#define _module_rtl871x_xmit_c_		BIT(0)
#define _module_xmit_osdep_c_		BIT(1)
#define _module_rtl871x_recv_c_		BIT(2)
#define _module_recv_osdep_c_		BIT(3)
#define _module_rtl871x_mlme_c_		BIT(4)
#define _module_mlme_osdep_c_		BIT(5)
#define _module_rtl871x_sta_mgt_c_	BIT(6)
#define _module_rtl871x_cmd_c_		BIT(7)
#define _module_cmd_osdep_c_		BIT(8)
#define _module_rtl871x_io_c_		BIT(9)
#define _module_io_osdep_c_		BIT(10)
#define _module_os_intfs_c_		BIT(11)
#define _module_rtl871x_security_c_	BIT(12)
#define _module_rtl871x_eeprom_c_	BIT(13)
#define _module_hal_init_c_		BIT(14)
#define _module_hci_hal_init_c_		BIT(15)
#define _module_rtl871x_ioctl_c_	BIT(16)
#define _module_rtl871x_ioctl_set_c_	BIT(17)
#define _module_rtl871x_ioctl_query_c_	BIT(18)
#define _module_rtl871x_pwrctrl_c_	BIT(19)
#define _module_hci_intfs_c_		BIT(20)
#define _module_hci_ops_c_		BIT(21)
#define _module_osdep_service_c_	BIT(22)
#define _module_mp_			BIT(23)
#define _module_hci_ops_os_c_		BIT(24)
#define _module_rtl871x_ioctl_os_c	BIT(25)
#define _module_rtl8712_cmd_c_		BIT(26)
#define	_module_rtl8192c_xmit_c_	BIT(27)
#define _module_hal_xmit_c_		BIT(28)
#define _module_efuse_			BIT(29)
#define _module_rtl8712_recv_c_		BIT(30)
#define _module_rtl8712_led_c_		BIT(31)

#define DRIVER_PREFIX	"R8188EU: "

extern u32 GlobalDebugLevel;

#define DBG_88E_LEVEL(_level, fmt, arg...)				\
	do {								\
		if (_level <= GlobalDebugLevel)				\
			pr_info(DRIVER_PREFIX fmt, ##arg);	\
	} while (0)

#define DBG_88E(...)							\
	do {								\
		if (_drv_err_ <= GlobalDebugLevel)			\
			pr_info(DRIVER_PREFIX __VA_ARGS__);		\
	} while (0)

#define MSG_88E(...)							\
	do {								\
		if (_drv_err_ <= GlobalDebugLevel)			\
			pr_info(DRIVER_PREFIX __VA_ARGS__);			\
	} while (0)

#define RT_TRACE(_comp, _level, fmt)					\
	do {								\
		if (_level <= GlobalDebugLevel) {			\
			pr_info("%s [0x%08x,%d]", DRIVER_PREFIX,	\
				 (unsigned int)_comp, _level);		\
			pr_info fmt;					\
		}							\
	} while (0)

#define RT_PRINT_DATA(_comp, _level, _titlestring, _hexdata, _hexdatalen)\
	do {								\
		if (_level <= GlobalDebugLevel) {			\
			int __i;					\
			u8	*ptr = (u8 *)_hexdata;			\
			pr_info("%s", DRIVER_PREFIX);			\
			pr_info(_titlestring);				\
			for (__i = 0; __i < (int)_hexdatalen; __i++) {	\
				pr_info("%02X%s", ptr[__i],		\
					 (((__i + 1) % 4) == 0) ?	\
					 "  " : " ");	\
				if (((__i + 1) % 16) == 0)		\
					pr_cont("\n");			\
			}						\
			pr_cont("\n");					\
		}							\
	} while (0)

int proc_get_drv_version(char *page, char **start,
			 off_t offset, int count,
			 int *eof, void *data);

int proc_get_write_reg(char *page, char **start,
		       off_t offset, int count,
		       int *eof, void *data);

int proc_set_write_reg(struct file *file, const char __user *buffer,
		       unsigned long count, void *data);
int proc_get_read_reg(char *page, char **start,
		      off_t offset, int count,
		      int *eof, void *data);

int proc_set_read_reg(struct file *file, const char __user *buffer,
		      unsigned long count, void *data);

int proc_get_adapter_state(char *page, char **start,
			   off_t offset, int count,
			   int *eof, void *data);

int proc_get_best_channel(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

#endif	/* __RTW_DEBUG_H__ */

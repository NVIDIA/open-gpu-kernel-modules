/* SPDX-License-Identifier: ISC */
/*
 * Copyright (c) 2014-2017 Qualcomm Atheros, Inc.
 */

#ifndef __WIL_PLATFORM_H__
#define __WIL_PLATFORM_H__

struct device;

enum wil_platform_event {
	WIL_PLATFORM_EVT_FW_CRASH = 0,
	WIL_PLATFORM_EVT_PRE_RESET = 1,
	WIL_PLATFORM_EVT_FW_RDY = 2,
	WIL_PLATFORM_EVT_PRE_SUSPEND = 3,
	WIL_PLATFORM_EVT_POST_SUSPEND = 4,
};

enum wil_platform_features {
	WIL_PLATFORM_FEATURE_FW_EXT_CLK_CONTROL = 0,
	WIL_PLATFORM_FEATURE_TRIPLE_MSI = 1,
	WIL_PLATFORM_FEATURE_MAX,
};

enum wil_platform_capa {
	WIL_PLATFORM_CAPA_RADIO_ON_IN_SUSPEND = 0,
	WIL_PLATFORM_CAPA_T_PWR_ON_0 = 1,
	WIL_PLATFORM_CAPA_EXT_CLK = 2,
	WIL_PLATFORM_CAPA_MAX,
};

/**
 * struct wil_platform_ops - wil platform module calls from this
 * driver to platform driver
 */
struct wil_platform_ops {
	int (*bus_request)(void *handle, uint32_t kbps /* KBytes/Sec */);
	int (*suspend)(void *handle, bool keep_device_power);
	int (*resume)(void *handle, bool device_powered_on);
	void (*uninit)(void *handle);
	int (*notify)(void *handle, enum wil_platform_event evt);
	int (*get_capa)(void *handle);
	void (*set_features)(void *handle, int features);
};

/**
 * struct wil_platform_rops - wil platform module callbacks from
 * platform driver to this driver
 * @ramdump: store a ramdump from the wil firmware. The platform
 *	driver may add additional data to the ramdump to
 *	generate the final crash dump.
 * @fw_recovery: start a firmware recovery process. Called as
 *      part of a crash recovery process which may include other
 *      related platform subsystems.
 */
struct wil_platform_rops {
	int (*ramdump)(void *wil_handle, void *buf, uint32_t size);
	int (*fw_recovery)(void *wil_handle);
};

/**
 * wil_platform_init - initialize the platform driver
 *
 * @dev - pointer to the wil6210 device
 * @ops - structure with platform driver operations. Platform
 *	driver will fill this structure with function pointers.
 * @rops - structure with callbacks from platform driver to
 *	this driver. The platform driver copies the structure to
 *	its own storage. Can be NULL if this driver does not
 *	support crash recovery.
 * @wil_handle - context for this driver that will be passed
 *      when platform driver invokes one of the callbacks in
 *      rops. May be NULL if rops is NULL.
 */
void *wil_platform_init(struct device *dev, struct wil_platform_ops *ops,
			const struct wil_platform_rops *rops, void *wil_handle);

int __init wil_platform_modinit(void);
void wil_platform_modexit(void);

#endif /* __WIL_PLATFORM_H__ */

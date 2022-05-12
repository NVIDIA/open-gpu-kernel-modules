// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/* Copyright(c) 2018-2019  Realtek Corporation
 */

#include <linux/module.h>
#include <linux/pci.h>
#include "pci.h"
#include "rtw8723de.h"

static const struct pci_device_id rtw_8723de_id_table[] = {
	{
		PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0xD723),
		.driver_data = (kernel_ulong_t)&rtw8723d_hw_spec
	},
	{}
};
MODULE_DEVICE_TABLE(pci, rtw_8723de_id_table);

static struct pci_driver rtw_8723de_driver = {
	.name = "rtw_8723de",
	.id_table = rtw_8723de_id_table,
	.probe = rtw_pci_probe,
	.remove = rtw_pci_remove,
	.driver.pm = &rtw_pm_ops,
	.shutdown = rtw_pci_shutdown,
};
module_pci_driver(rtw_8723de_driver);

MODULE_AUTHOR("Realtek Corporation");
MODULE_DESCRIPTION("Realtek 802.11n wireless 8723de driver");
MODULE_LICENSE("Dual BSD/GPL");

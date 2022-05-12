/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_FIRMWARE_H
#define AMDKCL_FIRMWARE_H

#if !defined(HAVE_REQUEST_FIRMWARE_DIRECT)
#include <linux/firmware.h>

#define request_firmware_direct   request_firmware

#endif
#endif /* AMDKCL_FIRMWARE_H */


/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_KCL_LINUX_MODULE_PARAMS_H_H
#define _KCL_KCL_LINUX_MODULE_PARAMS_H_H

#include <linux/module.h>
#include <linux/moduleparam.h>

/* Copied from v5.8-rc2-514-g7d8365771ffb include/linux/moduleparam.h */
#ifndef param_check_hexint
#define _kcl_param_check_hexint
extern const struct kernel_param_ops param_ops_hexint;
extern int param_set_hexint(const char *val, const struct kernel_param *kp);
extern int param_get_hexint(char *buffer, const struct kernel_param *kp);
#define param_check_hexint(name, p) param_check_uint(name, p)
#endif /* param_check_hexint */

#endif

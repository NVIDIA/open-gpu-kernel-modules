/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */

#ifndef __IMX_COMMON_H__
#define __IMX_COMMON_H__

#define EXCEPT_MAX_HDR_SIZE	0x400
#define IMX8_STACK_DUMP_SIZE 32

void imx8_get_registers(struct snd_sof_dev *sdev,
			struct sof_ipc_dsp_oops_xtensa *xoops,
			struct sof_ipc_panic_info *panic_info,
			u32 *stack, size_t stack_words);

void imx8_dump(struct snd_sof_dev *sdev, u32 flags);

#endif

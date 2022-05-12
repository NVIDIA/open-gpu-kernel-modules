/* SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only) */
/* Copyright(c) 2015 - 2020 Intel Corporation */
#ifndef ADF_C62XVF_HW_DATA_H_
#define ADF_C62XVF_HW_DATA_H_

#define ADF_C62XIOV_PMISC_BAR 1
#define ADF_C62XIOV_ACCELERATORS_MASK 0x1
#define ADF_C62XIOV_ACCELENGINES_MASK 0x1
#define ADF_C62XIOV_MAX_ACCELERATORS 1
#define ADF_C62XIOV_MAX_ACCELENGINES 1
#define ADF_C62XIOV_RX_RINGS_OFFSET 8
#define ADF_C62XIOV_TX_RINGS_MASK 0xFF
#define ADF_C62XIOV_ETR_BAR 0
#define ADF_C62XIOV_ETR_MAX_BANKS 1
#define ADF_C62XIOV_PF2VF_OFFSET	0x200
#define ADF_C62XIOV_VINTMSK_OFFSET	0x208

void adf_init_hw_data_c62xiov(struct adf_hw_device_data *hw_data);
void adf_clean_hw_data_c62xiov(struct adf_hw_device_data *hw_data);
#endif

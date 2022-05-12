/* SPDX-License-Identifier: ISC */
/*
 * Copyright (c) 2015-2016 Qualcomm Atheros, Inc.
 */

#ifndef _SWAP_H_
#define _SWAP_H_

#define ATH10K_SWAP_CODE_SEG_BIN_LEN_MAX	(512 * 1024)
#define ATH10K_SWAP_CODE_SEG_MAGIC_BYTES_SZ	12
#define ATH10K_SWAP_CODE_SEG_NUM_MAX		16
/* Currently only one swap segment is supported */
#define ATH10K_SWAP_CODE_SEG_NUM_SUPPORTED	1

struct ath10k_fw_file;

struct ath10k_swap_code_seg_tlv {
	__le32 address;
	__le32 length;
	u8 data[0];
} __packed;

struct ath10k_swap_code_seg_tail {
	u8 magic_signature[ATH10K_SWAP_CODE_SEG_MAGIC_BYTES_SZ];
	__le32 bmi_write_addr;
} __packed;

union ath10k_swap_code_seg_item {
	struct ath10k_swap_code_seg_tlv tlv;
	struct ath10k_swap_code_seg_tail tail;
} __packed;

struct ath10k_swap_code_seg_hw_info {
	/* Swap binary image size */
	__le32 swap_size;
	__le32 num_segs;

	/* Swap data size */
	__le32 size;
	__le32 size_log2;
	__le32 bus_addr[ATH10K_SWAP_CODE_SEG_NUM_MAX];
	__le64 reserved[ATH10K_SWAP_CODE_SEG_NUM_MAX];
} __packed;

struct ath10k_swap_code_seg_info {
	struct ath10k_swap_code_seg_hw_info seg_hw_info;
	void *virt_address[ATH10K_SWAP_CODE_SEG_NUM_SUPPORTED];
	u32 target_addr;
	dma_addr_t paddr[ATH10K_SWAP_CODE_SEG_NUM_SUPPORTED];
};

int ath10k_swap_code_seg_configure(struct ath10k *ar,
				   const struct ath10k_fw_file *fw_file);
void ath10k_swap_code_seg_release(struct ath10k *ar,
				  struct ath10k_fw_file *fw_file);
int ath10k_swap_code_seg_init(struct ath10k *ar,
			      struct ath10k_fw_file *fw_file);

#endif

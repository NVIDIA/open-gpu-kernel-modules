/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) 2012-2019 ARM Limited (or its affiliates). */

#ifndef __CC_HW_QUEUE_DEFS_H__
#define __CC_HW_QUEUE_DEFS_H__

#include <linux/types.h>

#include "cc_kernel_regs.h"
#include <linux/bitfield.h>

/******************************************************************************
 *				DEFINITIONS
 ******************************************************************************/

#define HW_DESC_SIZE_WORDS		6
/* Define max. available slots in HW queue */
#define HW_QUEUE_SLOTS_MAX              15

#define CC_REG_LOW(name)  (name ## _BIT_SHIFT)
#define CC_REG_HIGH(name) (CC_REG_LOW(name) + name ## _BIT_SIZE - 1)
#define CC_GENMASK(name)  GENMASK(CC_REG_HIGH(name), CC_REG_LOW(name))

#define CC_HWQ_GENMASK(word, field) \
	CC_GENMASK(CC_DSCRPTR_QUEUE_WORD ## word ## _ ## field)

#define WORD0_VALUE		CC_HWQ_GENMASK(0, VALUE)
#define	WORD0_CPP_CIPHER_MODE	CC_HWQ_GENMASK(0, CPP_CIPHER_MODE)
#define WORD1_DIN_CONST_VALUE	CC_HWQ_GENMASK(1, DIN_CONST_VALUE)
#define WORD1_DIN_DMA_MODE	CC_HWQ_GENMASK(1, DIN_DMA_MODE)
#define WORD1_DIN_SIZE		CC_HWQ_GENMASK(1, DIN_SIZE)
#define WORD1_NOT_LAST		CC_HWQ_GENMASK(1, NOT_LAST)
#define WORD1_NS_BIT		CC_HWQ_GENMASK(1, NS_BIT)
#define WORD1_LOCK_QUEUE	CC_HWQ_GENMASK(1, LOCK_QUEUE)
#define WORD2_VALUE		CC_HWQ_GENMASK(2, VALUE)
#define WORD3_DOUT_DMA_MODE	CC_HWQ_GENMASK(3, DOUT_DMA_MODE)
#define WORD3_DOUT_LAST_IND	CC_HWQ_GENMASK(3, DOUT_LAST_IND)
#define WORD3_DOUT_SIZE		CC_HWQ_GENMASK(3, DOUT_SIZE)
#define WORD3_HASH_XOR_BIT	CC_HWQ_GENMASK(3, HASH_XOR_BIT)
#define WORD3_NS_BIT		CC_HWQ_GENMASK(3, NS_BIT)
#define WORD3_QUEUE_LAST_IND	CC_HWQ_GENMASK(3, QUEUE_LAST_IND)
#define WORD4_ACK_NEEDED	CC_HWQ_GENMASK(4, ACK_NEEDED)
#define WORD4_AES_SEL_N_HASH	CC_HWQ_GENMASK(4, AES_SEL_N_HASH)
#define WORD4_AES_XOR_CRYPTO_KEY CC_HWQ_GENMASK(4, AES_XOR_CRYPTO_KEY)
#define WORD4_BYTES_SWAP	CC_HWQ_GENMASK(4, BYTES_SWAP)
#define WORD4_CIPHER_CONF0	CC_HWQ_GENMASK(4, CIPHER_CONF0)
#define WORD4_CIPHER_CONF1	CC_HWQ_GENMASK(4, CIPHER_CONF1)
#define WORD4_CIPHER_CONF2	CC_HWQ_GENMASK(4, CIPHER_CONF2)
#define WORD4_CIPHER_DO		CC_HWQ_GENMASK(4, CIPHER_DO)
#define WORD4_CIPHER_MODE	CC_HWQ_GENMASK(4, CIPHER_MODE)
#define WORD4_CMAC_SIZE0	CC_HWQ_GENMASK(4, CMAC_SIZE0)
#define WORD4_DATA_FLOW_MODE	CC_HWQ_GENMASK(4, DATA_FLOW_MODE)
#define WORD4_KEY_SIZE		CC_HWQ_GENMASK(4, KEY_SIZE)
#define WORD4_SETUP_OPERATION	CC_HWQ_GENMASK(4, SETUP_OPERATION)
#define WORD5_DIN_ADDR_HIGH	CC_HWQ_GENMASK(5, DIN_ADDR_HIGH)
#define WORD5_DOUT_ADDR_HIGH	CC_HWQ_GENMASK(5, DOUT_ADDR_HIGH)

/******************************************************************************
 *				TYPE DEFINITIONS
 ******************************************************************************/

struct cc_hw_desc {
	union {
		u32 word[HW_DESC_SIZE_WORDS];
		u16 hword[HW_DESC_SIZE_WORDS * 2];
	};
};

enum cc_axi_sec {
	AXI_SECURE = 0,
	AXI_NOT_SECURE = 1
};

enum cc_desc_direction {
	DESC_DIRECTION_ILLEGAL = -1,
	DESC_DIRECTION_ENCRYPT_ENCRYPT = 0,
	DESC_DIRECTION_DECRYPT_DECRYPT = 1,
	DESC_DIRECTION_DECRYPT_ENCRYPT = 3,
	DESC_DIRECTION_END = S32_MAX,
};

enum cc_dma_mode {
	DMA_MODE_NULL		= -1,
	NO_DMA			= 0,
	DMA_SRAM		= 1,
	DMA_DLLI		= 2,
	DMA_MLLI		= 3,
	DMA_MODE_END		= S32_MAX,
};

enum cc_flow_mode {
	FLOW_MODE_NULL		= -1,
	/* data flows */
	BYPASS			= 0,
	DIN_AES_DOUT		= 1,
	AES_to_HASH		= 2,
	AES_and_HASH		= 3,
	DIN_DES_DOUT		= 4,
	DES_to_HASH		= 5,
	DES_and_HASH		= 6,
	DIN_HASH		= 7,
	DIN_HASH_and_BYPASS	= 8,
	AESMAC_and_BYPASS	= 9,
	AES_to_HASH_and_DOUT	= 10,
	DIN_RC4_DOUT		= 11,
	DES_to_HASH_and_DOUT	= 12,
	AES_to_AES_to_HASH_and_DOUT	= 13,
	AES_to_AES_to_HASH	= 14,
	AES_to_HASH_and_AES	= 15,
	DIN_SM4_DOUT		= 16,
	DIN_AES_AESMAC		= 17,
	HASH_to_DOUT		= 18,
	/* setup flows */
	S_DIN_to_AES		= 32,
	S_DIN_to_AES2		= 33,
	S_DIN_to_DES		= 34,
	S_DIN_to_RC4		= 35,
	S_DIN_to_SM4		= 36,
	S_DIN_to_HASH		= 37,
	S_AES_to_DOUT		= 38,
	S_AES2_to_DOUT		= 39,
	S_SM4_to_DOUT		= 40,
	S_RC4_to_DOUT		= 41,
	S_DES_to_DOUT		= 42,
	S_HASH_to_DOUT		= 43,
	SET_FLOW_ID		= 44,
	FLOW_MODE_END = S32_MAX,
};

enum cc_setup_op {
	SETUP_LOAD_NOP		= 0,
	SETUP_LOAD_STATE0	= 1,
	SETUP_LOAD_STATE1	= 2,
	SETUP_LOAD_STATE2	= 3,
	SETUP_LOAD_KEY0		= 4,
	SETUP_LOAD_XEX_KEY	= 5,
	SETUP_WRITE_STATE0	= 8,
	SETUP_WRITE_STATE1	= 9,
	SETUP_WRITE_STATE2	= 10,
	SETUP_WRITE_STATE3	= 11,
	SETUP_OP_END = S32_MAX,
};

enum cc_hash_conf_pad {
	HASH_PADDING_DISABLED = 0,
	HASH_PADDING_ENABLED = 1,
	HASH_DIGEST_RESULT_LITTLE_ENDIAN = 2,
	HASH_CONFIG1_PADDING_RESERVE32 = S32_MAX,
};

enum cc_aes_mac_selector {
	AES_SK = 1,
	AES_CMAC_INIT = 2,
	AES_CMAC_SIZE0 = 3,
	AES_MAC_END = S32_MAX,
};

#define HW_KEY_MASK_CIPHER_DO	  0x3
#define HW_KEY_SHIFT_CIPHER_CFG2  2

/* HwCryptoKey[1:0] is mapped to cipher_do[1:0] */
/* HwCryptoKey[2:3] is mapped to cipher_config2[1:0] */
enum cc_hw_crypto_key {
	USER_KEY = 0,			/* 0x0000 */
	ROOT_KEY = 1,			/* 0x0001 */
	PROVISIONING_KEY = 2,		/* 0x0010 */ /* ==KCP */
	SESSION_KEY = 3,		/* 0x0011 */
	RESERVED_KEY = 4,		/* NA */
	PLATFORM_KEY = 5,		/* 0x0101 */
	CUSTOMER_KEY = 6,		/* 0x0110 */
	KFDE0_KEY = 7,			/* 0x0111 */
	KFDE1_KEY = 9,			/* 0x1001 */
	KFDE2_KEY = 10,			/* 0x1010 */
	KFDE3_KEY = 11,			/* 0x1011 */
	END_OF_KEYS = S32_MAX,
};

#define CC_NUM_HW_KEY_SLOTS	4
#define CC_FIRST_HW_KEY_SLOT	0
#define CC_LAST_HW_KEY_SLOT	(CC_FIRST_HW_KEY_SLOT + CC_NUM_HW_KEY_SLOTS - 1)

#define CC_NUM_CPP_KEY_SLOTS	8
#define CC_FIRST_CPP_KEY_SLOT	16
#define CC_LAST_CPP_KEY_SLOT	(CC_FIRST_CPP_KEY_SLOT + \
					CC_NUM_CPP_KEY_SLOTS - 1)

enum cc_hw_aes_key_size {
	AES_128_KEY = 0,
	AES_192_KEY = 1,
	AES_256_KEY = 2,
	END_OF_AES_KEYS = S32_MAX,
};

enum cc_hash_cipher_pad {
	DO_NOT_PAD = 0,
	DO_PAD = 1,
	HASH_CIPHER_DO_PADDING_RESERVE32 = S32_MAX,
};

#define CC_CPP_DIN_ADDR	0xFF00FF00UL
#define CC_CPP_DIN_SIZE 0xFF00FFUL

/*****************************/
/* Descriptor packing macros */
/*****************************/

/**
 * hw_desc_init() - Init a HW descriptor struct
 * @pdesc: pointer to HW descriptor struct
 */
static inline void hw_desc_init(struct cc_hw_desc *pdesc)
{
	memset(pdesc, 0, sizeof(struct cc_hw_desc));
}

/**
 * set_queue_last_ind_bit() - Indicate the end of current HW descriptors flow
 * and release the HW engines.
 *
 * @pdesc: Pointer to HW descriptor struct
 */
static inline void set_queue_last_ind_bit(struct cc_hw_desc *pdesc)
{
	pdesc->word[3] |= FIELD_PREP(WORD3_QUEUE_LAST_IND, 1);
}

/**
 * set_din_type() - Set the DIN field of a HW descriptor
 *
 * @pdesc: Pointer to HW descriptor struct
 * @dma_mode: The DMA mode: NO_DMA, SRAM, DLLI, MLLI, CONSTANT
 * @addr: DIN address
 * @size: Data size in bytes
 * @axi_sec: AXI secure bit
 */
static inline void set_din_type(struct cc_hw_desc *pdesc,
				enum cc_dma_mode dma_mode, dma_addr_t addr,
				u32 size, enum cc_axi_sec axi_sec)
{
	pdesc->word[0] = lower_32_bits(addr);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	pdesc->word[5] |= FIELD_PREP(WORD5_DIN_ADDR_HIGH, upper_32_bits(addr));
#endif
	pdesc->word[1] |= FIELD_PREP(WORD1_DIN_DMA_MODE, dma_mode) |
				FIELD_PREP(WORD1_DIN_SIZE, size) |
				FIELD_PREP(WORD1_NS_BIT, axi_sec);
}

/**
 * set_din_no_dma() - Set the DIN field of a HW descriptor to NO DMA mode.
 * Used for NOP descriptor, register patches and other special modes.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @addr: DIN address
 * @size: Data size in bytes
 */
static inline void set_din_no_dma(struct cc_hw_desc *pdesc, u32 addr, u32 size)
{
	pdesc->word[0] = addr;
	pdesc->word[1] |= FIELD_PREP(WORD1_DIN_SIZE, size);
}

/**
 * set_cpp_crypto_key() - Setup the special CPP descriptor
 *
 * @pdesc: Pointer to HW descriptor struct
 * @slot: Slot number
 */
static inline void set_cpp_crypto_key(struct cc_hw_desc *pdesc, u8 slot)
{
	pdesc->word[0] |= CC_CPP_DIN_ADDR;

	pdesc->word[1] |= FIELD_PREP(WORD1_DIN_SIZE, CC_CPP_DIN_SIZE);
	pdesc->word[1] |= FIELD_PREP(WORD1_LOCK_QUEUE, 1);

	pdesc->word[4] |= FIELD_PREP(WORD4_SETUP_OPERATION, slot);
}

/**
 * set_din_sram() - Set the DIN field of a HW descriptor to SRAM mode.
 * Note: No need to check SRAM alignment since host requests do not use SRAM and
 * the adaptor will enforce alignment checks.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @addr: DIN address
 * @size: Data size in bytes
 */
static inline void set_din_sram(struct cc_hw_desc *pdesc, u32 addr, u32 size)
{
	pdesc->word[0] = addr;
	pdesc->word[1] |= FIELD_PREP(WORD1_DIN_SIZE, size) |
				FIELD_PREP(WORD1_DIN_DMA_MODE, DMA_SRAM);
}

/**
 * set_din_const() - Set the DIN field of a HW descriptor to CONST mode
 *
 * @pdesc: Pointer to HW descriptor struct
 * @val: DIN const value
 * @size: Data size in bytes
 */
static inline void set_din_const(struct cc_hw_desc *pdesc, u32 val, u32 size)
{
	pdesc->word[0] = val;
	pdesc->word[1] |= FIELD_PREP(WORD1_DIN_CONST_VALUE, 1) |
			FIELD_PREP(WORD1_DIN_DMA_MODE, DMA_SRAM) |
			FIELD_PREP(WORD1_DIN_SIZE, size);
}

/**
 * set_din_not_last_indication() - Set the DIN not last input data indicator
 *
 * @pdesc: Pointer to HW descriptor struct
 */
static inline void set_din_not_last_indication(struct cc_hw_desc *pdesc)
{
	pdesc->word[1] |= FIELD_PREP(WORD1_NOT_LAST, 1);
}

/**
 * set_dout_type() - Set the DOUT field of a HW descriptor
 *
 * @pdesc: Pointer to HW descriptor struct
 * @dma_mode: The DMA mode: NO_DMA, SRAM, DLLI, MLLI, CONSTANT
 * @addr: DOUT address
 * @size: Data size in bytes
 * @axi_sec: AXI secure bit
 */
static inline void set_dout_type(struct cc_hw_desc *pdesc,
				 enum cc_dma_mode dma_mode, dma_addr_t addr,
				 u32 size, enum cc_axi_sec axi_sec)
{
	pdesc->word[2] = lower_32_bits(addr);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	pdesc->word[5] |= FIELD_PREP(WORD5_DOUT_ADDR_HIGH, upper_32_bits(addr));
#endif
	pdesc->word[3] |= FIELD_PREP(WORD3_DOUT_DMA_MODE, dma_mode) |
				FIELD_PREP(WORD3_DOUT_SIZE, size) |
				FIELD_PREP(WORD3_NS_BIT, axi_sec);
}

/**
 * set_dout_dlli() - Set the DOUT field of a HW descriptor to DLLI type
 * The LAST INDICATION is provided by the user
 *
 * @pdesc: Pointer to HW descriptor struct
 * @addr: DOUT address
 * @size: Data size in bytes
 * @axi_sec: AXI secure bit
 * @last_ind: The last indication bit
 */
static inline void set_dout_dlli(struct cc_hw_desc *pdesc, dma_addr_t addr,
				 u32 size, enum cc_axi_sec axi_sec,
				 u32 last_ind)
{
	set_dout_type(pdesc, DMA_DLLI, addr, size, axi_sec);
	pdesc->word[3] |= FIELD_PREP(WORD3_DOUT_LAST_IND, last_ind);
}

/**
 * set_dout_mlli() - Set the DOUT field of a HW descriptor to MLLI type
 * The LAST INDICATION is provided by the user
 *
 * @pdesc: Pointer to HW descriptor struct
 * @addr: DOUT address
 * @size: Data size in bytes
 * @axi_sec: AXI secure bit
 * @last_ind: The last indication bit
 */
static inline void set_dout_mlli(struct cc_hw_desc *pdesc, u32 addr, u32 size,
				 enum cc_axi_sec axi_sec, bool last_ind)
{
	set_dout_type(pdesc, DMA_MLLI, addr, size, axi_sec);
	pdesc->word[3] |= FIELD_PREP(WORD3_DOUT_LAST_IND, last_ind);
}

/**
 * set_dout_no_dma() - Set the DOUT field of a HW descriptor to NO DMA mode.
 * Used for NOP descriptor, register patches and other special modes.
 *
 * @pdesc: pointer to HW descriptor struct
 * @addr: DOUT address
 * @size: Data size in bytes
 * @write_enable: Enables a write operation to a register
 */
static inline void set_dout_no_dma(struct cc_hw_desc *pdesc, u32 addr,
				   u32 size, bool write_enable)
{
	pdesc->word[2] = addr;
	pdesc->word[3] |= FIELD_PREP(WORD3_DOUT_SIZE, size) |
			FIELD_PREP(WORD3_DOUT_LAST_IND, write_enable);
}

/**
 * set_xor_val() - Set the word for the XOR operation.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @val: XOR data value
 */
static inline void set_xor_val(struct cc_hw_desc *pdesc, u32 val)
{
	pdesc->word[2] = val;
}

/**
 * set_xor_active() - Set the XOR indicator bit in the descriptor
 *
 * @pdesc: Pointer to HW descriptor struct
 */
static inline void set_xor_active(struct cc_hw_desc *pdesc)
{
	pdesc->word[3] |= FIELD_PREP(WORD3_HASH_XOR_BIT, 1);
}

/**
 * set_aes_not_hash_mode() - Select the AES engine instead of HASH engine when
 * setting up combined mode with AES XCBC MAC
 *
 * @pdesc: Pointer to HW descriptor struct
 */
static inline void set_aes_not_hash_mode(struct cc_hw_desc *pdesc)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_AES_SEL_N_HASH, 1);
}

/**
 * set_aes_xor_crypto_key() - Set aes xor crypto key, which in some scenarios
 * selects the SM3 engine
 *
 * @pdesc: Pointer to HW descriptor struct
 */
static inline void set_aes_xor_crypto_key(struct cc_hw_desc *pdesc)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_AES_XOR_CRYPTO_KEY, 1);
}

/**
 * set_dout_sram() - Set the DOUT field of a HW descriptor to SRAM mode
 * Note: No need to check SRAM alignment since host requests do not use SRAM and
 * the adaptor will enforce alignment checks.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @addr: DOUT address
 * @size: Data size in bytes
 */
static inline void set_dout_sram(struct cc_hw_desc *pdesc, u32 addr, u32 size)
{
	pdesc->word[2] = addr;
	pdesc->word[3] |= FIELD_PREP(WORD3_DOUT_DMA_MODE, DMA_SRAM) |
			FIELD_PREP(WORD3_DOUT_SIZE, size);
}

/**
 * set_xex_data_unit_size() - Set the data unit size for XEX mode in
 * data_out_addr[15:0]
 *
 * @pdesc: Pointer to HW descriptor struct
 * @size: Data unit size for XEX mode
 */
static inline void set_xex_data_unit_size(struct cc_hw_desc *pdesc, u32 size)
{
	pdesc->word[2] = size;
}

/**
 * set_multi2_num_rounds() - Set the number of rounds for Multi2 in
 * data_out_addr[15:0]
 *
 * @pdesc: Pointer to HW descriptor struct
 * @num: Number of rounds for Multi2
 */
static inline void set_multi2_num_rounds(struct cc_hw_desc *pdesc, u32 num)
{
	pdesc->word[2] = num;
}

/**
 * set_flow_mode() - Set the flow mode.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @mode: Any one of the modes defined in [CC7x-DESC]
 */
static inline void set_flow_mode(struct cc_hw_desc *pdesc,
				 enum cc_flow_mode mode)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_DATA_FLOW_MODE, mode);
}

/**
 * set_cipher_mode() - Set the cipher mode.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @mode: Any one of the modes defined in [CC7x-DESC]
 */
static inline void set_cipher_mode(struct cc_hw_desc *pdesc, int mode)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_CIPHER_MODE, mode);
}

/**
 * set_hash_cipher_mode() - Set the cipher mode for hash algorithms.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @cipher_mode: Any one of the modes defined in [CC7x-DESC]
 * @hash_mode: specifies which hash is being handled
 */
static inline void set_hash_cipher_mode(struct cc_hw_desc *pdesc,
					enum drv_cipher_mode cipher_mode,
					enum drv_hash_mode hash_mode)
{
	set_cipher_mode(pdesc, cipher_mode);
	if (hash_mode == DRV_HASH_SM3)
		set_aes_xor_crypto_key(pdesc);
}

/**
 * set_cipher_config0() - Set the cipher configuration fields.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @mode: Any one of the modes defined in [CC7x-DESC]
 */
static inline void set_cipher_config0(struct cc_hw_desc *pdesc, int mode)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_CIPHER_CONF0, mode);
}

/**
 * set_cipher_config1() - Set the cipher configuration fields.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @config: Padding mode
 */
static inline void set_cipher_config1(struct cc_hw_desc *pdesc,
				      enum cc_hash_conf_pad config)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_CIPHER_CONF1, config);
}

/**
 * set_hw_crypto_key() - Set HW key configuration fields.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @hw_key: The HW key slot asdefined in enum cc_hw_crypto_key
 */
static inline void set_hw_crypto_key(struct cc_hw_desc *pdesc,
				     enum cc_hw_crypto_key hw_key)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_CIPHER_DO,
				     (hw_key & HW_KEY_MASK_CIPHER_DO)) |
			FIELD_PREP(WORD4_CIPHER_CONF2,
				   (hw_key >> HW_KEY_SHIFT_CIPHER_CFG2));
}

/**
 * set_bytes_swap() - Set byte order of all setup-finalize descriptors.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @config: True to enable byte swapping
 */
static inline void set_bytes_swap(struct cc_hw_desc *pdesc, bool config)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_BYTES_SWAP, config);
}

/**
 * set_cmac_size0_mode() - Set CMAC_SIZE0 mode.
 *
 * @pdesc: Pointer to HW descriptor struct
 */
static inline void set_cmac_size0_mode(struct cc_hw_desc *pdesc)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_CMAC_SIZE0, 1);
}

/**
 * set_key_size() - Set key size descriptor field.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @size: Key size in bytes (NOT size code)
 */
static inline void set_key_size(struct cc_hw_desc *pdesc, u32 size)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_KEY_SIZE, size);
}

/**
 * set_key_size_aes() - Set AES key size.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @size: Key size in bytes (NOT size code)
 */
static inline void set_key_size_aes(struct cc_hw_desc *pdesc, u32 size)
{
	set_key_size(pdesc, ((size >> 3) - 2));
}

/**
 * set_key_size_des() - Set DES key size.
 *
 * @pdesc: Pointer to HW descriptor struct
 * @size: Key size in bytes (NOT size code)
 */
static inline void set_key_size_des(struct cc_hw_desc *pdesc, u32 size)
{
	set_key_size(pdesc, ((size >> 3) - 1));
}

/**
 * set_setup_mode() - Set the descriptor setup mode
 *
 * @pdesc: Pointer to HW descriptor struct
 * @mode: Any one of the setup modes defined in [CC7x-DESC]
 */
static inline void set_setup_mode(struct cc_hw_desc *pdesc,
				  enum cc_setup_op mode)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_SETUP_OPERATION, mode);
}

/**
 * set_cipher_do() - Set the descriptor cipher DO
 *
 * @pdesc: Pointer to HW descriptor struct
 * @config: Any one of the cipher do defined in [CC7x-DESC]
 */
static inline void set_cipher_do(struct cc_hw_desc *pdesc,
				 enum cc_hash_cipher_pad config)
{
	pdesc->word[4] |= FIELD_PREP(WORD4_CIPHER_DO,
				(config & HW_KEY_MASK_CIPHER_DO));
}

#endif /*__CC_HW_QUEUE_DEFS_H__*/

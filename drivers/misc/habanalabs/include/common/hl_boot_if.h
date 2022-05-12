/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright 2018-2020 HabanaLabs, Ltd.
 * All Rights Reserved.
 *
 */

#ifndef HL_BOOT_IF_H
#define HL_BOOT_IF_H

#define LKD_HARD_RESET_MAGIC		0xED7BD694
#define HL_POWER9_HOST_MAGIC		0x1DA30009

#define BOOT_FIT_SRAM_OFFSET		0x200000

#define VERSION_MAX_LEN			128

/*
 * CPU error bits in BOOT_ERROR registers
 *
 * CPU_BOOT_ERR0_DRAM_INIT_FAIL		DRAM initialization failed.
 *					DRAM is not reliable to use.
 *
 * CPU_BOOT_ERR0_FIT_CORRUPTED		FIT data integrity verification of the
 *					image provided by the host has failed.
 *
 * CPU_BOOT_ERR0_TS_INIT_FAIL		Thermal Sensor initialization failed.
 *					Boot continues as usual, but keep in
 *					mind this is a warning.
 *
 * CPU_BOOT_ERR0_DRAM_SKIPPED		DRAM initialization has been skipped.
 *					Skipping DRAM initialization has been
 *					requested (e.g. strap, command, etc.)
 *					and FW skipped the DRAM initialization.
 *					Host can initialize the DRAM.
 *
 * CPU_BOOT_ERR0_BMC_WAIT_SKIPPED	Waiting for BMC data will be skipped.
 *					Meaning the BMC data might not be
 *					available until reset.
 *
 * CPU_BOOT_ERR0_NIC_DATA_NOT_RDY	NIC data from BMC is not ready.
 *					BMC has not provided the NIC data yet.
 *					Once provided this bit will be cleared.
 *
 * CPU_BOOT_ERR0_NIC_FW_FAIL		NIC FW loading failed.
 *					The NIC FW loading and initialization
 *					failed. This means NICs are not usable.
 *
 * CPU_BOOT_ERR0_SECURITY_NOT_RDY	Chip security initialization has been
 *					started, but is not ready yet - chip
 *					cannot be accessed.
 *
 * CPU_BOOT_ERR0_SECURITY_FAIL		Security related tasks have failed.
 *					The tasks are security init (root of
 *					trust), boot authentication (chain of
 *					trust), data packets authentication.
 *
 * CPU_BOOT_ERR0_EFUSE_FAIL		Reading from eFuse failed.
 *					The PCI device ID might be wrong.
 *
 * CPU_BOOT_ERR0_PRI_IMG_VER_FAIL	Verification of primary image failed.
 *					It mean that ppboot checksum
 *					verification for the preboot primary
 *					image has failed to match expected
 *					checksum. Trying to program image again
 *					might solve this.
 *
 * CPU_BOOT_ERR0_SEC_IMG_VER_FAIL	Verification of secondary image failed.
 *					It mean that ppboot checksum
 *					verification for the preboot secondary
 *					image has failed to match expected
 *					checksum. Trying to program image again
 *					might solve this.
 *
 * CPU_BOOT_ERR0_PLL_FAIL		PLL settings failed, meaning that one
 *					of the PLLs remains in REF_CLK
 *
 * CPU_BOOT_ERR0_DEVICE_UNUSABLE_FAIL	Device is unusable and customer support
 *					should be contacted.
 *
 * CPU_BOOT_ERR0_ENABLED		Error registers enabled.
 *					This is a main indication that the
 *					running FW populates the error
 *					registers. Meaning the error bits are
 *					not garbage, but actual error statuses.
 */
#define CPU_BOOT_ERR0_DRAM_INIT_FAIL		(1 << 0)
#define CPU_BOOT_ERR0_FIT_CORRUPTED		(1 << 1)
#define CPU_BOOT_ERR0_TS_INIT_FAIL		(1 << 2)
#define CPU_BOOT_ERR0_DRAM_SKIPPED		(1 << 3)
#define CPU_BOOT_ERR0_BMC_WAIT_SKIPPED		(1 << 4)
#define CPU_BOOT_ERR0_NIC_DATA_NOT_RDY		(1 << 5)
#define CPU_BOOT_ERR0_NIC_FW_FAIL		(1 << 6)
#define CPU_BOOT_ERR0_SECURITY_NOT_RDY		(1 << 7)
#define CPU_BOOT_ERR0_SECURITY_FAIL		(1 << 8)
#define CPU_BOOT_ERR0_EFUSE_FAIL		(1 << 9)
#define CPU_BOOT_ERR0_PRI_IMG_VER_FAIL		(1 << 10)
#define CPU_BOOT_ERR0_SEC_IMG_VER_FAIL		(1 << 11)
#define CPU_BOOT_ERR0_PLL_FAIL			(1 << 12)
#define CPU_BOOT_ERR0_DEVICE_UNUSABLE_FAIL	(1 << 13)
#define CPU_BOOT_ERR0_ENABLED			(1 << 31)

/*
 * BOOT DEVICE STATUS bits in BOOT_DEVICE_STS registers
 *
 * CPU_BOOT_DEV_STS0_SECURITY_EN	Security is Enabled.
 *					This is an indication for security
 *					enabled in FW, which means that
 *					all conditions for security are met:
 *					device is indicated as security enabled,
 *					registers are protected, and device
 *					uses keys for image verification.
 *					Initialized in: preboot
 *
 * CPU_BOOT_DEV_STS0_DEBUG_EN		Debug is enabled.
 *					Enabled when JTAG or DEBUG is enabled
 *					in FW.
 *					Initialized in: preboot
 *
 * CPU_BOOT_DEV_STS0_WATCHDOG_EN	Watchdog is enabled.
 *					Watchdog is enabled in FW.
 *					Initialized in: preboot
 *
 * CPU_BOOT_DEV_STS0_DRAM_INIT_EN	DRAM initialization is enabled.
 *					DRAM initialization has been done in FW.
 *					Initialized in: u-boot
 *
 * CPU_BOOT_DEV_STS0_BMC_WAIT_EN	Waiting for BMC data enabled.
 *					If set, it means that during boot,
 *					FW waited for BMC data.
 *					Initialized in: u-boot
 *
 * CPU_BOOT_DEV_STS0_E2E_CRED_EN	E2E credits initialized.
 *					FW initialized E2E credits.
 *					Initialized in: u-boot
 *
 * CPU_BOOT_DEV_STS0_HBM_CRED_EN	HBM credits initialized.
 *					FW initialized HBM credits.
 *					Initialized in: u-boot
 *
 * CPU_BOOT_DEV_STS0_RL_EN		Rate limiter initialized.
 *					FW initialized rate limiter.
 *					Initialized in: u-boot
 *
 * CPU_BOOT_DEV_STS0_SRAM_SCR_EN	SRAM scrambler enabled.
 *					FW initialized SRAM scrambler.
 *					Initialized in: linux
 *
 * CPU_BOOT_DEV_STS0_DRAM_SCR_EN	DRAM scrambler enabled.
 *					FW initialized DRAM scrambler.
 *					Initialized in: u-boot
 *
 * CPU_BOOT_DEV_STS0_FW_HARD_RST_EN	FW hard reset procedure is enabled.
 *					FW has the hard reset procedure
 *					implemented. This means that FW will
 *					perform hard reset procedure on
 *					receiving the halt-machine event.
 *					Initialized in: preboot, u-boot, linux
 *
 * CPU_BOOT_DEV_STS0_PLL_INFO_EN	FW retrieval of PLL info is enabled.
 *					Initialized in: linux
 *
 * CPU_BOOT_DEV_STS0_SP_SRAM_EN		SP SRAM is initialized and available
 *					for use.
 *					Initialized in: preboot
 *
 * CPU_BOOT_DEV_STS0_CLK_GATE_EN	Clock Gating enabled.
 *					FW initialized Clock Gating.
 *					Initialized in: preboot
 *
 * CPU_BOOT_DEV_STS0_HBM_ECC_EN		HBM ECC handling Enabled.
 *					FW handles HBM ECC indications.
 *					Initialized in: linux
 *
 * CPU_BOOT_DEV_STS0_PKT_PI_ACK_EN	Packets ack value used in the armcpd
 *					is set to the PI counter.
 *					Initialized in: linux
 *
 * CPU_BOOT_DEV_STS0_FW_LD_COM_EN	Flexible FW loading communication
 *					protocol is enabled.
 *					Initialized in: preboot
 *
 * CPU_BOOT_DEV_STS0_FW_IATU_CONF_EN	FW iATU configuration is enabled.
 *					This bit if set, means the iATU has been
 *					configured and is ready for use.
 *					Initialized in: ppboot
 *
 * CPU_BOOT_DEV_STS0_DYN_PLL_EN		Dynamic PLL configuration is enabled.
 *					FW sends to host a bitmap of supported
 *					PLLs.
 *					Initialized in: linux
 *
 * CPU_BOOT_DEV_STS0_ENABLED		Device status register enabled.
 *					This is a main indication that the
 *					running FW populates the device status
 *					register. Meaning the device status
 *					bits are not garbage, but actual
 *					statuses.
 *					Initialized in: preboot
 *
 */
#define CPU_BOOT_DEV_STS0_SECURITY_EN			(1 << 0)
#define CPU_BOOT_DEV_STS0_DEBUG_EN			(1 << 1)
#define CPU_BOOT_DEV_STS0_WATCHDOG_EN			(1 << 2)
#define CPU_BOOT_DEV_STS0_DRAM_INIT_EN			(1 << 3)
#define CPU_BOOT_DEV_STS0_BMC_WAIT_EN			(1 << 4)
#define CPU_BOOT_DEV_STS0_E2E_CRED_EN			(1 << 5)
#define CPU_BOOT_DEV_STS0_HBM_CRED_EN			(1 << 6)
#define CPU_BOOT_DEV_STS0_RL_EN				(1 << 7)
#define CPU_BOOT_DEV_STS0_SRAM_SCR_EN			(1 << 8)
#define CPU_BOOT_DEV_STS0_DRAM_SCR_EN			(1 << 9)
#define CPU_BOOT_DEV_STS0_FW_HARD_RST_EN		(1 << 10)
#define CPU_BOOT_DEV_STS0_PLL_INFO_EN			(1 << 11)
#define CPU_BOOT_DEV_STS0_SP_SRAM_EN			(1 << 12)
#define CPU_BOOT_DEV_STS0_CLK_GATE_EN			(1 << 13)
#define CPU_BOOT_DEV_STS0_HBM_ECC_EN			(1 << 14)
#define CPU_BOOT_DEV_STS0_PKT_PI_ACK_EN			(1 << 15)
#define CPU_BOOT_DEV_STS0_FW_LD_COM_EN			(1 << 16)
#define CPU_BOOT_DEV_STS0_FW_IATU_CONF_EN		(1 << 17)
#define CPU_BOOT_DEV_STS0_DYN_PLL_EN			(1 << 19)
#define CPU_BOOT_DEV_STS0_ENABLED			(1 << 31)

enum cpu_boot_status {
	CPU_BOOT_STATUS_NA = 0,		/* Default value after reset of chip */
	CPU_BOOT_STATUS_IN_WFE = 1,
	CPU_BOOT_STATUS_DRAM_RDY = 2,
	CPU_BOOT_STATUS_SRAM_AVAIL = 3,
	CPU_BOOT_STATUS_IN_BTL = 4,	/* BTL is H/W FSM */
	CPU_BOOT_STATUS_IN_PREBOOT = 5,
	CPU_BOOT_STATUS_IN_SPL,		/* deprecated - not reported */
	CPU_BOOT_STATUS_IN_UBOOT = 7,
	CPU_BOOT_STATUS_DRAM_INIT_FAIL,	/* deprecated - will be removed */
	CPU_BOOT_STATUS_FIT_CORRUPTED,	/* deprecated - will be removed */
	/* U-Boot console prompt activated, commands are not processed */
	CPU_BOOT_STATUS_UBOOT_NOT_READY = 10,
	/* Finished NICs init, reported after DRAM and NICs */
	CPU_BOOT_STATUS_NIC_FW_RDY = 11,
	CPU_BOOT_STATUS_TS_INIT_FAIL,	/* deprecated - will be removed */
	CPU_BOOT_STATUS_DRAM_SKIPPED,	/* deprecated - will be removed */
	CPU_BOOT_STATUS_BMC_WAITING_SKIPPED, /* deprecated - will be removed */
	/* Last boot loader progress status, ready to receive commands */
	CPU_BOOT_STATUS_READY_TO_BOOT = 15,
	/* Internal Boot finished, ready for boot-fit */
	CPU_BOOT_STATUS_WAITING_FOR_BOOT_FIT = 16,
	/* Internal Security has been initialized, device can be accessed */
	CPU_BOOT_STATUS_SECURITY_READY = 17,
};

enum kmd_msg {
	KMD_MSG_NA = 0,
	KMD_MSG_GOTO_WFE,
	KMD_MSG_FIT_RDY,
	KMD_MSG_SKIP_BMC,
	RESERVED,
	KMD_MSG_RST_DEV,
	KMD_MSG_LAST
};

enum cpu_msg_status {
	CPU_MSG_CLR = 0,
	CPU_MSG_OK,
	CPU_MSG_ERR,
};

/* communication registers mapping - consider ABI when changing */
struct cpu_dyn_regs {
	uint32_t cpu_pq_base_addr_low;
	uint32_t cpu_pq_base_addr_high;
	uint32_t cpu_pq_length;
	uint32_t cpu_pq_init_status;
	uint32_t cpu_eq_base_addr_low;
	uint32_t cpu_eq_base_addr_high;
	uint32_t cpu_eq_length;
	uint32_t cpu_eq_ci;
	uint32_t cpu_cq_base_addr_low;
	uint32_t cpu_cq_base_addr_high;
	uint32_t cpu_cq_length;
	uint32_t cpu_pf_pq_pi;
	uint32_t cpu_boot_dev_sts0;
	uint32_t cpu_boot_dev_sts1;
	uint32_t cpu_boot_err0;
	uint32_t cpu_boot_err1;
	uint32_t cpu_boot_status;
	uint32_t fw_upd_sts;
	uint32_t fw_upd_cmd;
	uint32_t fw_upd_pending_sts;
	uint32_t fuse_ver_offset;
	uint32_t preboot_ver_offset;
	uint32_t uboot_ver_offset;
	uint32_t hw_state;
	uint32_t kmd_msg_to_cpu;
	uint32_t cpu_cmd_status_to_host;
	uint32_t reserved1[32];		/* reserve for future use */
};

/* HCDM - Habana Communications Descriptor Magic */
#define HL_COMMS_DESC_MAGIC	0x4843444D
#define HL_COMMS_DESC_VER	1

/* this is the comms descriptor header - meta data */
struct comms_desc_header {
	uint32_t magic;		/* magic for validation */
	uint32_t crc32;		/* CRC32 of the descriptor w/o header */
	uint16_t size;		/* size of the descriptor w/o header */
	uint8_t version;	/* descriptor version */
	uint8_t reserved[5];	/* pad to 64 bit */
};

/* this is the main FW descriptor - consider ABI when changing */
struct lkd_fw_comms_desc {
	struct comms_desc_header header;
	struct cpu_dyn_regs cpu_dyn_regs;
	char fuse_ver[VERSION_MAX_LEN];
	char cur_fw_ver[VERSION_MAX_LEN];
	/* can be used for 1 more version w/o ABI change */
	char reserved0[VERSION_MAX_LEN];
	uint64_t img_addr;	/* address for next FW component load */
};

/*
 * LKD commands:
 *
 * COMMS_NOOP			Used to clear the command register and no actual
 *				command is send.
 *
 * COMMS_CLR_STS		Clear status command - FW should clear the
 *				status register. Used for synchronization
 *				between the commands as part of the race free
 *				protocol.
 *
 * COMMS_RST_STATE		Reset the current communication state which is
 *				kept by FW for proper responses.
 *				Should be used in the beginning of the
 *				communication cycle to clean any leftovers from
 *				previous communication attempts.
 *
 * COMMS_PREP_DESC		Prepare descriptor for setting up the
 *				communication and other dynamic data:
 *				struct lkd_fw_comms_desc.
 *				This command has a parameter stating the next FW
 *				component size, so the FW can actually prepare a
 *				space for it and in the status response provide
 *				the descriptor offset. The Offset of the next FW
 *				data component is a part of the descriptor
 *				structure.
 *
 * COMMS_DATA_RDY		The FW data has been uploaded and is ready for
 *				validation.
 *
 * COMMS_EXEC			Execute the next FW component.
 *
 * COMMS_RST_DEV		Reset the device.
 *
 * COMMS_GOTO_WFE		Execute WFE command. Allowed only on non-secure
 *				devices.
 *
 * COMMS_SKIP_BMC		Perform actions required for BMC-less servers.
 *				Do not wait for BMC response.
 *
 * COMMS_LOW_PLL_OPP		Initialize PLLs for low OPP.
 */
enum comms_cmd {
	COMMS_NOOP = 0,
	COMMS_CLR_STS = 1,
	COMMS_RST_STATE = 2,
	COMMS_PREP_DESC = 3,
	COMMS_DATA_RDY = 4,
	COMMS_EXEC = 5,
	COMMS_RST_DEV = 6,
	COMMS_GOTO_WFE = 7,
	COMMS_SKIP_BMC = 8,
	COMMS_LOW_PLL_OPP = 9,
	COMMS_INVLD_LAST
};

#define COMMS_COMMAND_SIZE_SHIFT	0
#define COMMS_COMMAND_SIZE_MASK		0x1FFFFFF
#define COMMS_COMMAND_CMD_SHIFT		27
#define COMMS_COMMAND_CMD_MASK		0xF8000000

/*
 * LKD command to FW register structure
 * @size	- FW component size
 * @cmd		- command from enum comms_cmd
 */
struct comms_command {
	union {		/* bit fields are only for FW use */
		struct {
			unsigned int size :25;		/* 32MB max. */
			unsigned int reserved :2;
			enum comms_cmd cmd :5;		/* 32 commands */
		};
		unsigned int val;
	};
};

/*
 * FW status
 *
 * COMMS_STS_NOOP		Used to clear the status register and no actual
 *				status is provided.
 *
 * COMMS_STS_ACK		Command has been received and recognized.
 *
 * COMMS_STS_OK			Command execution has finished successfully.
 *
 * COMMS_STS_ERR		Command execution was unsuccessful and resulted
 *				in error.
 *
 * COMMS_STS_VALID_ERR		FW validation has failed.
 *
 * COMMS_STS_TIMEOUT_ERR	Command execution has timed out.
 */
enum comms_sts {
	COMMS_STS_NOOP = 0,
	COMMS_STS_ACK = 1,
	COMMS_STS_OK = 2,
	COMMS_STS_ERR = 3,
	COMMS_STS_VALID_ERR = 4,
	COMMS_STS_TIMEOUT_ERR = 5,
	COMMS_STS_INVLD_LAST
};

/* RAM types for FW components loading - defines the base address */
enum comms_ram_types {
	COMMS_SRAM = 0,
	COMMS_DRAM = 1,
};

#define COMMS_STATUS_OFFSET_SHIFT	0
#define COMMS_STATUS_OFFSET_MASK	0x03FFFFFF
#define COMMS_STATUS_OFFSET_ALIGN_SHIFT	2
#define COMMS_STATUS_RAM_TYPE_SHIFT	26
#define COMMS_STATUS_RAM_TYPE_MASK	0x0C000000
#define COMMS_STATUS_STATUS_SHIFT	28
#define COMMS_STATUS_STATUS_MASK	0xF0000000

/*
 * FW status to LKD register structure
 * @offset	- an offset from the base of the ram_type shifted right by
 *		  2 bits (always aligned to 32 bits).
 *		  Allows a maximum addressable offset of 256MB from RAM base.
 *		  Example: for real offset in RAM of 0x800000 (8MB), the value
 *		  in offset field is (0x800000 >> 2) = 0x200000.
 * @ram_type	- the RAM type that should be used for offset from
 *		  enum comms_ram_types
 * @status	- status from enum comms_sts
 */
struct comms_status {
	union {		/* bit fields are only for FW use */
		struct {
			unsigned int offset :26;
			unsigned int ram_type :2;
			enum comms_sts status :4;	/* 16 statuses */
		};
		unsigned int val;
	};
};

#endif /* HL_BOOT_IF_H */

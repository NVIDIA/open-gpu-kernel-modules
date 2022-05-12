/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2010-2015, 2018-2019 The Linux Foundation. All rights reserved.
 * Copyright (C) 2015 Linaro Ltd.
 */
#ifndef __QCOM_SCM_H
#define __QCOM_SCM_H

#include <linux/err.h>
#include <linux/types.h>
#include <linux/cpumask.h>

#define QCOM_SCM_VERSION(major, minor)	(((major) << 16) | ((minor) & 0xFF))
#define QCOM_SCM_CPU_PWR_DOWN_L2_ON	0x0
#define QCOM_SCM_CPU_PWR_DOWN_L2_OFF	0x1
#define QCOM_SCM_HDCP_MAX_REQ_CNT	5

struct qcom_scm_hdcp_req {
	u32 addr;
	u32 val;
};

struct qcom_scm_vmperm {
	int vmid;
	int perm;
};

enum qcom_scm_ocmem_client {
	QCOM_SCM_OCMEM_UNUSED_ID = 0x0,
	QCOM_SCM_OCMEM_GRAPHICS_ID,
	QCOM_SCM_OCMEM_VIDEO_ID,
	QCOM_SCM_OCMEM_LP_AUDIO_ID,
	QCOM_SCM_OCMEM_SENSORS_ID,
	QCOM_SCM_OCMEM_OTHER_OS_ID,
	QCOM_SCM_OCMEM_DEBUG_ID,
};

enum qcom_scm_sec_dev_id {
	QCOM_SCM_MDSS_DEV_ID    = 1,
	QCOM_SCM_OCMEM_DEV_ID   = 5,
	QCOM_SCM_PCIE0_DEV_ID   = 11,
	QCOM_SCM_PCIE1_DEV_ID   = 12,
	QCOM_SCM_GFX_DEV_ID     = 18,
	QCOM_SCM_UFS_DEV_ID     = 19,
	QCOM_SCM_ICE_DEV_ID     = 20,
};

enum qcom_scm_ice_cipher {
	QCOM_SCM_ICE_CIPHER_AES_128_XTS = 0,
	QCOM_SCM_ICE_CIPHER_AES_128_CBC = 1,
	QCOM_SCM_ICE_CIPHER_AES_256_XTS = 3,
	QCOM_SCM_ICE_CIPHER_AES_256_CBC = 4,
};

#define QCOM_SCM_VMID_HLOS       0x3
#define QCOM_SCM_VMID_MSS_MSA    0xF
#define QCOM_SCM_VMID_WLAN       0x18
#define QCOM_SCM_VMID_WLAN_CE    0x19
#define QCOM_SCM_PERM_READ       0x4
#define QCOM_SCM_PERM_WRITE      0x2
#define QCOM_SCM_PERM_EXEC       0x1
#define QCOM_SCM_PERM_RW (QCOM_SCM_PERM_READ | QCOM_SCM_PERM_WRITE)
#define QCOM_SCM_PERM_RWX (QCOM_SCM_PERM_RW | QCOM_SCM_PERM_EXEC)

#if IS_ENABLED(CONFIG_QCOM_SCM)
extern bool qcom_scm_is_available(void);

extern int qcom_scm_set_cold_boot_addr(void *entry, const cpumask_t *cpus);
extern int qcom_scm_set_warm_boot_addr(void *entry, const cpumask_t *cpus);
extern void qcom_scm_cpu_power_down(u32 flags);
extern int qcom_scm_set_remote_state(u32 state, u32 id);

extern int qcom_scm_pas_init_image(u32 peripheral, const void *metadata,
				   size_t size);
extern int qcom_scm_pas_mem_setup(u32 peripheral, phys_addr_t addr,
				  phys_addr_t size);
extern int qcom_scm_pas_auth_and_reset(u32 peripheral);
extern int qcom_scm_pas_shutdown(u32 peripheral);
extern bool qcom_scm_pas_supported(u32 peripheral);

extern int qcom_scm_io_readl(phys_addr_t addr, unsigned int *val);
extern int qcom_scm_io_writel(phys_addr_t addr, unsigned int val);

extern bool qcom_scm_restore_sec_cfg_available(void);
extern int qcom_scm_restore_sec_cfg(u32 device_id, u32 spare);
extern int qcom_scm_iommu_secure_ptbl_size(u32 spare, size_t *size);
extern int qcom_scm_iommu_secure_ptbl_init(u64 addr, u32 size, u32 spare);
extern int qcom_scm_mem_protect_video_var(u32 cp_start, u32 cp_size,
					  u32 cp_nonpixel_start,
					  u32 cp_nonpixel_size);
extern int qcom_scm_assign_mem(phys_addr_t mem_addr, size_t mem_sz,
			       unsigned int *src,
			       const struct qcom_scm_vmperm *newvm,
			       unsigned int dest_cnt);

extern bool qcom_scm_ocmem_lock_available(void);
extern int qcom_scm_ocmem_lock(enum qcom_scm_ocmem_client id, u32 offset,
			       u32 size, u32 mode);
extern int qcom_scm_ocmem_unlock(enum qcom_scm_ocmem_client id, u32 offset,
				 u32 size);

extern bool qcom_scm_ice_available(void);
extern int qcom_scm_ice_invalidate_key(u32 index);
extern int qcom_scm_ice_set_key(u32 index, const u8 *key, u32 key_size,
				enum qcom_scm_ice_cipher cipher,
				u32 data_unit_size);

extern bool qcom_scm_hdcp_available(void);
extern int qcom_scm_hdcp_req(struct qcom_scm_hdcp_req *req, u32 req_cnt,
			     u32 *resp);

extern int qcom_scm_qsmmu500_wait_safe_toggle(bool en);
#else

#include <linux/errno.h>

static inline bool qcom_scm_is_available(void) { return false; }

static inline int qcom_scm_set_cold_boot_addr(void *entry,
		const cpumask_t *cpus) { return -ENODEV; }
static inline int qcom_scm_set_warm_boot_addr(void *entry,
		const cpumask_t *cpus) { return -ENODEV; }
static inline void qcom_scm_cpu_power_down(u32 flags) {}
static inline u32 qcom_scm_set_remote_state(u32 state,u32 id)
		{ return -ENODEV; }

static inline int qcom_scm_pas_init_image(u32 peripheral, const void *metadata,
		size_t size) { return -ENODEV; }
static inline int qcom_scm_pas_mem_setup(u32 peripheral, phys_addr_t addr,
		phys_addr_t size) { return -ENODEV; }
static inline int qcom_scm_pas_auth_and_reset(u32 peripheral)
		{ return -ENODEV; }
static inline int qcom_scm_pas_shutdown(u32 peripheral) { return -ENODEV; }
static inline bool qcom_scm_pas_supported(u32 peripheral) { return false; }

static inline int qcom_scm_io_readl(phys_addr_t addr, unsigned int *val)
		{ return -ENODEV; }
static inline int qcom_scm_io_writel(phys_addr_t addr, unsigned int val)
		{ return -ENODEV; }

static inline bool qcom_scm_restore_sec_cfg_available(void) { return false; }
static inline int qcom_scm_restore_sec_cfg(u32 device_id, u32 spare)
		{ return -ENODEV; }
static inline int qcom_scm_iommu_secure_ptbl_size(u32 spare, size_t *size)
		{ return -ENODEV; }
static inline int qcom_scm_iommu_secure_ptbl_init(u64 addr, u32 size, u32 spare)
		{ return -ENODEV; }
extern inline int qcom_scm_mem_protect_video_var(u32 cp_start, u32 cp_size,
						 u32 cp_nonpixel_start,
						 u32 cp_nonpixel_size)
		{ return -ENODEV; }
static inline int qcom_scm_assign_mem(phys_addr_t mem_addr, size_t mem_sz,
		unsigned int *src, const struct qcom_scm_vmperm *newvm,
		unsigned int dest_cnt) { return -ENODEV; }

static inline bool qcom_scm_ocmem_lock_available(void) { return false; }
static inline int qcom_scm_ocmem_lock(enum qcom_scm_ocmem_client id, u32 offset,
		u32 size, u32 mode) { return -ENODEV; }
static inline int qcom_scm_ocmem_unlock(enum qcom_scm_ocmem_client id,
		u32 offset, u32 size) { return -ENODEV; }

static inline bool qcom_scm_ice_available(void) { return false; }
static inline int qcom_scm_ice_invalidate_key(u32 index) { return -ENODEV; }
static inline int qcom_scm_ice_set_key(u32 index, const u8 *key, u32 key_size,
				       enum qcom_scm_ice_cipher cipher,
				       u32 data_unit_size) { return -ENODEV; }

static inline bool qcom_scm_hdcp_available(void) { return false; }
static inline int qcom_scm_hdcp_req(struct qcom_scm_hdcp_req *req, u32 req_cnt,
		u32 *resp) { return -ENODEV; }

static inline int qcom_scm_qsmmu500_wait_safe_toggle(bool en)
		{ return -ENODEV; }
#endif
#endif

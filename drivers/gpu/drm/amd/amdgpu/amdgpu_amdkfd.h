/*
 * Copyright 2014 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/* amdgpu_amdkfd.h defines the private interface between amdgpu and amdkfd. */

#ifndef AMDGPU_AMDKFD_H_INCLUDED
#define AMDGPU_AMDKFD_H_INCLUDED

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <kgd_kfd_interface.h>
#include <drm/ttm/ttm_execbuf_util.h>
#include "amdgpu_gfx.h"
#include "amdgpu_sync.h"
#include "amdgpu_vm.h"

extern uint64_t amdgpu_amdkfd_total_mem_size;

enum TLB_FLUSH_TYPE {
	TLB_FLUSH_LEGACY = 0,
	TLB_FLUSH_LIGHTWEIGHT,
	TLB_FLUSH_HEAVYWEIGHT
};

struct amdgpu_device;

enum kfd_mem_attachment_type {
	KFD_MEM_ATT_SHARED,	/* Share kgd_mem->bo or another attachment's */
	KFD_MEM_ATT_USERPTR,	/* SG bo to DMA map pages from a userptr bo */
	KFD_MEM_ATT_DMABUF,	/* DMAbuf to DMA map TTM BOs */
};

struct kfd_mem_attachment {
	struct list_head list;
	enum kfd_mem_attachment_type type;
	bool is_mapped;
	struct amdgpu_bo_va *bo_va;
	struct amdgpu_device *adev;
	uint64_t va;
	uint64_t pte_flags;
};

struct kgd_mem {
	struct mutex lock;
	struct amdgpu_bo *bo;
	struct kfd_ipc_obj *ipc_obj;
	struct dma_buf *dmabuf;
	struct list_head attachments;
	/* protected by amdkfd_process_info.lock */
	struct ttm_validate_buffer validate_list;
	struct ttm_validate_buffer resv_list;
	uint32_t domain;
	unsigned int mapped_to_gpu_memory;
	uint64_t va;

	uint32_t alloc_flags;

	atomic_t invalid;
	struct amdkfd_process_info *process_info;
#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	struct page **user_pages;
#endif

	struct amdgpu_sync sync;

	bool aql_queue;
	bool is_imported;
};

/* KFD Memory Eviction */
struct amdgpu_amdkfd_fence {
	struct dma_fence base;
	struct mm_struct *mm;
	spinlock_t lock;
	char timeline_name[TASK_COMM_LEN];
	struct svm_range_bo *svm_bo;
};

struct amdgpu_kfd_dev {
	struct kfd_dev *dev;
	uint64_t vram_used;
	bool init_complete;
};

enum kgd_engine_type {
	KGD_ENGINE_PFP = 1,
	KGD_ENGINE_ME,
	KGD_ENGINE_CE,
	KGD_ENGINE_MEC1,
	KGD_ENGINE_MEC2,
	KGD_ENGINE_RLC,
	KGD_ENGINE_SDMA1,
	KGD_ENGINE_SDMA2,
	KGD_ENGINE_MAX
};


struct amdkfd_process_info {
	/* List head of all VMs that belong to a KFD process */
	struct list_head vm_list_head;
	/* List head for all KFD BOs that belong to a KFD process. */
	struct list_head kfd_bo_list;
	/* List of userptr BOs that are valid or invalid */
	struct list_head userptr_valid_list;
	struct list_head userptr_inval_list;
	/* Lock to protect kfd_bo_list */
	struct mutex lock;

	/* Number of VMs */
	unsigned int n_vms;
	/* Eviction Fence */
	struct amdgpu_amdkfd_fence *eviction_fence;

	/* MMU-notifier related fields */
	atomic_t evicted_bos;
	struct delayed_work restore_userptr_work;
	struct pid *pid;
	bool block_mmu_notifications;
};

int amdgpu_amdkfd_init(void);
void amdgpu_amdkfd_fini(void);

void amdgpu_amdkfd_suspend(struct amdgpu_device *adev, bool run_pm);
int amdgpu_amdkfd_resume_iommu(struct amdgpu_device *adev);
int amdgpu_amdkfd_resume(struct amdgpu_device *adev, bool run_pm, bool sync);
void amdgpu_amdkfd_interrupt(struct amdgpu_device *adev,
			const void *ih_ring_entry);
void amdgpu_amdkfd_device_probe(struct amdgpu_device *adev);
void amdgpu_amdkfd_device_init(struct amdgpu_device *adev);
void amdgpu_amdkfd_device_fini_sw(struct amdgpu_device *adev);
int amdgpu_amdkfd_submit_ib(struct amdgpu_device *adev,
				enum kgd_engine_type engine,
				uint32_t vmid, uint64_t gpu_addr,
				uint32_t *ib_cmd, uint32_t ib_len);
void amdgpu_amdkfd_set_compute_idle(struct amdgpu_device *adev, bool idle);
bool amdgpu_amdkfd_have_atomics_support(struct amdgpu_device *adev);
int amdgpu_amdkfd_flush_gpu_tlb_vmid(struct amdgpu_device *adev,
				uint16_t vmid);
int amdgpu_amdkfd_flush_gpu_tlb_pasid(struct amdgpu_device *adev,
				uint16_t pasid, enum TLB_FLUSH_TYPE flush_type);

int amdgpu_amdkfd_copy_mem_to_mem(struct amdgpu_device *adev, struct kgd_mem *src_mem,
		uint64_t src_offset, struct kgd_mem *dst_mem,
		uint64_t dest_offset, uint64_t size, struct dma_fence **f,
		uint64_t *actual_size);

bool amdgpu_amdkfd_is_kfd_vmid(struct amdgpu_device *adev, u32 vmid);

int amdgpu_amdkfd_pre_reset(struct amdgpu_device *adev);

int amdgpu_amdkfd_post_reset(struct amdgpu_device *adev);

void amdgpu_amdkfd_gpu_reset(struct amdgpu_device *adev);

int amdgpu_queue_mask_bit_to_set_resource_bit(struct amdgpu_device *adev,
					int queue_bit);

struct amdgpu_amdkfd_fence *amdgpu_amdkfd_fence_create(u64 context,
				struct mm_struct *mm,
				struct svm_range_bo *svm_bo);
#if IS_ENABLED(CONFIG_HSA_AMD)
bool amdkfd_fence_check_mm(struct dma_fence *f, struct mm_struct *mm);
struct amdgpu_amdkfd_fence *to_amdgpu_amdkfd_fence(struct dma_fence *f);
int amdgpu_amdkfd_remove_fence_on_pt_pd_bos(struct amdgpu_bo *bo);
int amdgpu_amdkfd_evict_userptr(struct kgd_mem *mem, struct mm_struct *mm);
#else
static inline
bool amdkfd_fence_check_mm(struct dma_fence *f, struct mm_struct *mm)
{
	return false;
}

static inline
struct amdgpu_amdkfd_fence *to_amdgpu_amdkfd_fence(struct dma_fence *f)
{
	return NULL;
}

static inline
int amdgpu_amdkfd_remove_fence_on_pt_pd_bos(struct amdgpu_bo *bo)
{
	return 0;
}

static inline
int amdgpu_amdkfd_evict_userptr(struct kgd_mem *mem, struct mm_struct *mm)
{
	return 0;
}
#endif
/* Shared API */
int amdgpu_amdkfd_alloc_gtt_mem(struct amdgpu_device *adev, size_t size,
				void **mem_obj, uint64_t *gpu_addr,
				void **cpu_ptr, bool mqd_gfx9, bool is_uswc_mode);
void amdgpu_amdkfd_free_gtt_mem(struct amdgpu_device *adev, void *mem_obj);
int amdgpu_amdkfd_alloc_gws(struct amdgpu_device *adev, size_t size,
				void **mem_obj);
void amdgpu_amdkfd_free_gws(struct amdgpu_device *adev, void *mem_obj);
int amdgpu_amdkfd_add_gws_to_process(void *info, void *gws, struct kgd_mem **mem);
int amdgpu_amdkfd_remove_gws_from_process(void *info, void *mem);
uint32_t amdgpu_amdkfd_get_fw_version(struct amdgpu_device *adev,
				      enum kgd_engine_type type);
void amdgpu_amdkfd_get_local_mem_info(struct amdgpu_device *adev,
				      struct kfd_local_mem_info *mem_info);
uint64_t amdgpu_amdkfd_get_gpu_clock_counter(struct amdgpu_device *adev);

uint32_t amdgpu_amdkfd_get_max_engine_clock_in_mhz(struct amdgpu_device *adev);
void amdgpu_amdkfd_get_cu_info(struct amdgpu_device *adev,
			       struct kfd_cu_info *cu_info);
int amdgpu_amdkfd_get_dmabuf_info(struct amdgpu_device *adev, int dma_buf_fd,
				  struct amdgpu_device **dmabuf_adev,
				  uint64_t *bo_size, void *metadata_buffer,
				  size_t buffer_size, uint32_t *metadata_size,
				  uint32_t *flags);
uint64_t amdgpu_amdkfd_get_vram_usage(struct amdgpu_device *adev);
uint8_t amdgpu_amdkfd_get_xgmi_hops_count(struct amdgpu_device *dst,
					  struct amdgpu_device *src);
int amdgpu_amdkfd_send_close_event_drain_irq(struct amdgpu_device *adev,
					uint32_t *payload);
int amdgpu_amdkfd_get_xgmi_bandwidth_mbytes(struct amdgpu_device *dst,
					    struct amdgpu_device *src,
					    bool is_min);
int amdgpu_amdkfd_get_pcie_bandwidth_mbytes(struct amdgpu_device *adev, bool is_min);
static inline void amdgpu_amdkfd_gfx_off_ctrl(struct amdgpu_device *adev, bool enable)
{
	amdgpu_gfx_off_ctrl(adev, enable);
}

/* Read user wptr from a specified user address space with page fault
 * disabled. The memory must be pinned and mapped to the hardware when
 * this is called in hqd_load functions, so it should never fault in
 * the first place. This resolves a circular lock dependency involving
 * four locks, including the DQM lock and mmap_lock.
 */
#define read_user_wptr(mmptr, wptr, dst)				\
	({								\
		bool valid = false;					\
		if ((mmptr) && (wptr)) {				\
			pagefault_disable();				\
			if ((mmptr) == current->mm) {			\
				valid = !get_user((dst), (wptr));	\
			} else if (current->flags & PF_KTHREAD) {	\
				kthread_use_mm(mmptr);			\
				valid = !get_user((dst), (wptr));	\
				kthread_unuse_mm(mmptr);		\
			}						\
			pagefault_enable();				\
		}							\
		valid;							\
	})

/* GPUVM API */
#define drm_priv_to_vm(drm_priv)					\
	(&((struct amdgpu_fpriv *)					\
		((struct drm_file *)(drm_priv))->driver_priv)->vm)

int amdgpu_amdkfd_gpuvm_acquire_process_vm(struct amdgpu_device *adev,
					struct file *filp, u32 pasid,
					void **process_info,
					struct dma_fence **ef);
void amdgpu_amdkfd_gpuvm_release_process_vm(struct amdgpu_device *adev,
					void *drm_priv);
uint64_t amdgpu_amdkfd_gpuvm_get_process_page_dir(void *drm_priv);
int amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(
		struct amdgpu_device *adev, uint64_t va, uint64_t size,
		void *drm_priv, struct sg_table *sg, struct kgd_mem **mem,
		uint64_t *offset, uint32_t flags, bool criu_resume);
int amdgpu_amdkfd_gpuvm_free_memory_of_gpu(
		struct amdgpu_device *adev, struct kgd_mem *mem, void *drm_priv,
		uint64_t *size);
int amdgpu_amdkfd_gpuvm_map_memory_to_gpu(
		struct amdgpu_device *adev, struct kgd_mem *mem, void *drm_priv,
		bool *table_freed);
int amdgpu_amdkfd_gpuvm_unmap_memory_from_gpu(
		struct amdgpu_device *adev, struct kgd_mem *mem, void *drm_priv);
int amdgpu_amdkfd_gpuvm_sync_memory(
		struct amdgpu_device *adev, struct kgd_mem *mem, bool intr);
int amdgpu_amdkfd_gpuvm_map_gtt_bo_to_kernel(struct amdgpu_device *adev,
		struct kgd_mem *mem, void **kptr, uint64_t *size);
void amdgpu_amdkfd_gpuvm_unmap_gtt_bo_from_kernel(struct amdgpu_device *adev,
		struct kgd_mem *mem);

int amdgpu_amdkfd_gpuvm_restore_process_bos(void *process_info,
					    struct dma_fence **ef);
int amdgpu_amdkfd_gpuvm_get_vm_fault_info(struct amdgpu_device *adev,
					      struct kfd_vm_fault_info *info);

struct amdgpu_bo *amdgpu_amdkfd_gpuvm_get_bo_ref(struct kgd_mem *mem,
						 uint32_t *flags);
void amdgpu_amdkfd_gpuvm_put_bo_ref(struct amdgpu_bo *bo);

/**
 * amdgpu_amdkfd_gpuvm_pin_bo() - Pins a BO using following criteria
 * @bo: Handle of buffer object being pinned
 * @domain: Domain into which BO should be pinned
 *
 *   - USERPTR BOs are UNPINNABLE and will return error
 *   - All other BO types (GTT, VRAM, MMIO and DOORBELL) will have their
 *     PIN count incremented. It is valid to PIN a BO multiple times
 *
 * Return: ZERO if successful in pinning, Non-Zero in case of error.
 */
int amdgpu_amdkfd_gpuvm_pin_bo(struct amdgpu_bo *bo, u32 domain);

/**
 * amdgpu_amdkfd_gpuvm_unpin_bo() - Unpins BO using following criteria
 * @bo: Handle of buffer object being unpinned
 *
 *   - Is a illegal request for USERPTR BOs and is ignored
 *   - All other BO types (GTT, VRAM, MMIO and DOORBELL) will have their
 *     PIN count decremented. Calls to UNPIN must balance calls to PIN
 */
void amdgpu_amdkfd_gpuvm_unpin_bo(struct amdgpu_bo *bo);

int amdgpu_amdkfd_gpuvm_get_sg_table(struct amdgpu_device *adev,
		struct amdgpu_bo *bo, uint32_t flags,
		uint64_t offset, uint64_t size,
		struct device *dma_dev, enum dma_data_direction dir,
		struct sg_table **ret_sg);
void amdgpu_amdkfd_gpuvm_put_sg_table(struct amdgpu_bo *bo,
		struct device *dma_dev, enum dma_data_direction dir,
		struct sg_table *sg);

int amdgpu_amdkfd_gpuvm_import_dmabuf(struct amdgpu_device *adev,
				      struct dma_buf *dmabuf,
				      struct kfd_ipc_obj *ipc_obj,
				      uint64_t va, void *drm_priv,
				      struct kgd_mem **mem, uint64_t *size,
				      uint64_t *mmap_offset);
int amdgpu_amdkfd_gpuvm_export_ipc_obj(struct amdgpu_device *adev, void *vm,
				       struct kgd_mem *mem,
				       struct kfd_ipc_obj **ipc_obj,
				       uint32_t flags);

void amdgpu_amdkfd_debug_mem_fence(struct amdgpu_device *adev);

int amdgpu_amdkfd_get_tile_config(struct amdgpu_device *adev,
				struct tile_config *config);
void amdgpu_amdkfd_ras_poison_consumption_handler(struct amdgpu_device *adev,
				bool reset);
bool amdgpu_amdkfd_bo_mapped_to_dev(struct amdgpu_device *adev, struct kgd_mem *mem);
void amdgpu_amdkfd_block_mmu_notifications(void *p);
int amdgpu_amdkfd_criu_resume(void *p);

#if IS_ENABLED(CONFIG_HSA_AMD)
void amdgpu_amdkfd_gpuvm_init_mem_limits(void);
void amdgpu_amdkfd_gpuvm_destroy_cb(struct amdgpu_device *adev,
				struct amdgpu_vm *vm);

/**
 * @amdgpu_amdkfd_release_notify() - Notify KFD when GEM object is released
 *
 * Allows KFD to release its resources associated with the GEM object.
 */
void amdgpu_amdkfd_release_notify(struct amdgpu_bo *bo);
void amdgpu_amdkfd_reserve_system_mem(uint64_t size);
#else
static inline
void amdgpu_amdkfd_gpuvm_init_mem_limits(void)
{
}

static inline
void amdgpu_amdkfd_gpuvm_destroy_cb(struct amdgpu_device *adev,
					struct amdgpu_vm *vm)
{
}

static inline
void amdgpu_amdkfd_release_notify(struct amdgpu_bo *bo)
{
}
#endif

void amdgpu_amdkfd_rlc_spm_cntl(struct amdgpu_device *adev, bool cntl);
int amdgpu_amdkfd_rlc_spm_acquire(struct amdgpu_device *adev,
		struct amdgpu_vm *vm, u64 gpu_addr, u32 size);
void amdgpu_amdkfd_rlc_spm_release(struct amdgpu_device *adev, struct amdgpu_vm *vm);
void amdgpu_amdkfd_rlc_spm_set_rdptr(struct amdgpu_device *adev, u32 rptr);
void amdgpu_amdkfd_rlc_spm_interrupt(struct amdgpu_device *adev);

/* KGD2KFD callbacks */
void kgd2kfd_spm_interrupt(struct kfd_dev *kfd);
int kgd2kfd_quiesce_mm(struct mm_struct *mm);
int kgd2kfd_resume_mm(struct mm_struct *mm);
int kgd2kfd_schedule_evict_and_restore_process(struct mm_struct *mm,
						struct dma_fence *fence);
#if IS_ENABLED(CONFIG_HSA_AMD)
int kgd2kfd_init(void);
void kgd2kfd_exit(void);
struct kfd_dev *kgd2kfd_probe(struct amdgpu_device *adev, bool vf);
bool kgd2kfd_device_init(struct kfd_dev *kfd,
			 struct drm_device *ddev,
			 const struct kgd2kfd_shared_resources *gpu_resources);
void kgd2kfd_device_exit(struct kfd_dev *kfd);
void kgd2kfd_suspend(struct kfd_dev *kfd, bool run_pm, bool force);
int kgd2kfd_resume_iommu(struct kfd_dev *kfd);
int kgd2kfd_resume(struct kfd_dev *kfd, bool run_pm, bool sync);
int kgd2kfd_pre_reset(struct kfd_dev *kfd);
int kgd2kfd_post_reset(struct kfd_dev *kfd);
void kgd2kfd_interrupt(struct kfd_dev *kfd, const void *ih_ring_entry);
void kgd2kfd_set_sram_ecc_flag(struct kfd_dev *kfd);
void kgd2kfd_smi_event_throttle(struct kfd_dev *kfd, uint64_t throttle_bitmask);
#else
static inline int kgd2kfd_init(void)
{
	return -ENOENT;
}

static inline void kgd2kfd_exit(void)
{
}

static inline
struct kfd_dev *kgd2kfd_probe(struct amdgpu_device *adev, bool vf)
{
	return NULL;
}

static inline
bool kgd2kfd_device_init(struct kfd_dev *kfd, struct drm_device *ddev,
				const struct kgd2kfd_shared_resources *gpu_resources)
{
	return false;
}

static inline void kgd2kfd_device_exit(struct kfd_dev *kfd)
{
}

static inline void kgd2kfd_suspend(struct kfd_dev *kfd, bool run_pm, bool force)
{
}

static int __maybe_unused kgd2kfd_resume_iommu(struct kfd_dev *kfd)
{
	return 0;
}

static inline int kgd2kfd_resume(struct kfd_dev *kfd, bool run_pm, bool sync)
{
	return 0;
}

static inline int kgd2kfd_pre_reset(struct kfd_dev *kfd)
{
	return 0;
}

static inline int kgd2kfd_post_reset(struct kfd_dev *kfd)
{
	return 0;
}

static inline
void kgd2kfd_interrupt(struct kfd_dev *kfd, const void *ih_ring_entry)
{
}

static inline
void kgd2kfd_set_sram_ecc_flag(struct kfd_dev *kfd)
{
}

static inline
void kgd2kfd_smi_event_throttle(struct kfd_dev *kfd, uint64_t throttle_bitmask)
{
}
#endif
#endif /* AMDGPU_AMDKFD_H_INCLUDED */

// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Copyright 2016-2022 Advanced Micro Devices, Inc.
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
 *
 */

#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "kfd_priv.h"
#include "kfd_mqd_manager.h"
#include "v9_structs.h"
#include "gc/gc_9_0_offset.h"
#include "gc/gc_9_0_sh_mask.h"
#include "sdma0/sdma0_4_0_sh_mask.h"
#include "amdgpu_amdkfd.h"

static inline struct v9_mqd *get_mqd(void *mqd)
{
	return (struct v9_mqd *)mqd;
}

static inline struct v9_sdma_mqd *get_sdma_mqd(void *mqd)
{
	return (struct v9_sdma_mqd *)mqd;
}

static bool check_sdma_queue_active(struct queue *q)
{
	uint32_t rptr, wptr;
	uint32_t rptr_hi, wptr_hi;
	struct v9_sdma_mqd *m = get_sdma_mqd(q->mqd);

	rptr = m->sdmax_rlcx_rb_rptr;
	wptr = m->sdmax_rlcx_rb_wptr;
	rptr_hi = m->sdmax_rlcx_rb_rptr_hi;
	wptr_hi = m->sdmax_rlcx_rb_wptr_hi;
	pr_debug("rptr=%d, wptr=%d\n", rptr, wptr);
	pr_debug("rptr_hi=%d, wptr_hi=%d\n", rptr_hi, wptr_hi);

	return (rptr != wptr || rptr_hi != wptr_hi);
}

static bool check_queue_active(struct queue *q)
{
	uint32_t rptr, wptr;
	uint32_t cntl_stack_offset, cntl_stack_size;
	struct v9_mqd *m = get_mqd(q->mqd);

	rptr = m->cp_hqd_pq_rptr;
	wptr = m->cp_hqd_pq_wptr_lo % q->properties.queue_size;
	cntl_stack_offset = m->cp_hqd_cntl_stack_offset;
	cntl_stack_size = m->cp_hqd_cntl_stack_size;

	pr_debug("rptr=%d, wptr=%d\n", rptr, wptr);
	pr_debug("m->cp_hqd_cntl_stack_offset=0x%08x\n", cntl_stack_offset);
	pr_debug("m->cp_hqd_cntl_stack_size=0x%08x\n", cntl_stack_size);

	if ((rptr == 0 && wptr == 0) ||
		cntl_stack_offset == 0xffffffff ||
		cntl_stack_size > 0x5000)
		return false;

	/* Process is idle if both conditions are meet:
	 * queue's rptr equals to wptr
	 * control stack is empty, cntl_stack_offset = cntl_stack_size
	 */
	return (rptr != wptr || cntl_stack_offset != cntl_stack_size);
}

static void update_cu_mask(struct mqd_manager *mm, void *mqd,
			struct mqd_update_info *minfo)
{
	struct v9_mqd *m;
	uint32_t se_mask[KFD_MAX_NUM_SE] = {0};

	if (!minfo || (minfo->update_flag != UPDATE_FLAG_CU_MASK) ||
	    !minfo->cu_mask.ptr)
		return;

	mqd_symmetrically_map_cu_mask(mm,
		minfo->cu_mask.ptr, minfo->cu_mask.count, se_mask);

	m = get_mqd(mqd);
	m->compute_static_thread_mgmt_se0 = se_mask[0];
	m->compute_static_thread_mgmt_se1 = se_mask[1];
	m->compute_static_thread_mgmt_se2 = se_mask[2];
	m->compute_static_thread_mgmt_se3 = se_mask[3];
	m->compute_static_thread_mgmt_se4 = se_mask[4];
	m->compute_static_thread_mgmt_se5 = se_mask[5];
	m->compute_static_thread_mgmt_se6 = se_mask[6];
	m->compute_static_thread_mgmt_se7 = se_mask[7];

	pr_debug("update cu mask to %#x %#x %#x %#x %#x %#x %#x %#x\n",
		m->compute_static_thread_mgmt_se0,
		m->compute_static_thread_mgmt_se1,
		m->compute_static_thread_mgmt_se2,
		m->compute_static_thread_mgmt_se3,
		m->compute_static_thread_mgmt_se4,
		m->compute_static_thread_mgmt_se5,
		m->compute_static_thread_mgmt_se6,
		m->compute_static_thread_mgmt_se7);
}

static void set_priority(struct v9_mqd *m, struct queue_properties *q)
{
	m->cp_hqd_pipe_priority = pipe_priority_map[q->priority];
	m->cp_hqd_queue_priority = q->priority;
}

static struct kfd_mem_obj *allocate_mqd(struct kfd_dev *kfd,
		struct queue_properties *q)
{
	int retval;
	struct kfd_mem_obj *mqd_mem_obj = NULL;

	/* For V9 only, due to a HW bug, the control stack of a user mode
	 * compute queue needs to be allocated just behind the page boundary
	 * of its regular MQD buffer. So we allocate an enlarged MQD buffer:
	 * the first page of the buffer serves as the regular MQD buffer
	 * purpose and the remaining is for control stack. Although the two
	 * parts are in the same buffer object, they need different memory
	 * types: MQD part needs UC (uncached) as usual, while control stack
	 * needs NC (non coherent), which is different from the UC type which
	 * is used when control stack is allocated in user space.
	 *
	 * Because of all those, we use the gtt allocation function instead
	 * of sub-allocation function for this enlarged MQD buffer. Moreover,
	 * in order to achieve two memory types in a single buffer object, we
	 * pass a special bo flag AMDGPU_GEM_CREATE_CP_MQD_GFX9 to instruct
	 * amdgpu memory functions to do so.
	 */
	if (kfd->cwsr_enabled && (q->type == KFD_QUEUE_TYPE_COMPUTE)) {
		mqd_mem_obj = kzalloc(sizeof(struct kfd_mem_obj), GFP_KERNEL);
		if (!mqd_mem_obj)
			return NULL;
		retval = amdgpu_amdkfd_alloc_gtt_mem(kfd->adev,
			ALIGN(q->ctl_stack_size, PAGE_SIZE) +
				ALIGN(sizeof(struct v9_mqd), PAGE_SIZE),
			&(mqd_mem_obj->gtt_mem),
			&(mqd_mem_obj->gpu_addr),
			(void *)&(mqd_mem_obj->cpu_ptr), true, true);
	} else {
		retval = kfd_gtt_sa_allocate(kfd, sizeof(struct v9_mqd),
				&mqd_mem_obj);
	}

	if (retval) {
		kfree(mqd_mem_obj);
		return NULL;
	}

	return mqd_mem_obj;

}

static void init_mqd(struct mqd_manager *mm, void **mqd,
			struct kfd_mem_obj *mqd_mem_obj, uint64_t *gart_addr,
			struct queue_properties *q)
{
	uint64_t addr;
	struct v9_mqd *m;

	m = (struct v9_mqd *) mqd_mem_obj->cpu_ptr;
	addr = mqd_mem_obj->gpu_addr;

	memset(m, 0, sizeof(struct v9_mqd));

	m->header = 0xC0310800;
	m->compute_pipelinestat_enable = 1;
	m->compute_static_thread_mgmt_se0 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se1 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se2 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se3 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se4 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se5 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se6 = 0xFFFFFFFF;
	m->compute_static_thread_mgmt_se7 = 0xFFFFFFFF;

	m->cp_hqd_persistent_state = CP_HQD_PERSISTENT_STATE__PRELOAD_REQ_MASK |
			0x53 << CP_HQD_PERSISTENT_STATE__PRELOAD_SIZE__SHIFT;

	m->cp_mqd_control = 1 << CP_MQD_CONTROL__PRIV_STATE__SHIFT;

	m->cp_mqd_base_addr_lo        = lower_32_bits(addr);
	m->cp_mqd_base_addr_hi        = upper_32_bits(addr);

	m->cp_hqd_quantum = 1 << CP_HQD_QUANTUM__QUANTUM_EN__SHIFT |
			1 << CP_HQD_QUANTUM__QUANTUM_SCALE__SHIFT |
			1 << CP_HQD_QUANTUM__QUANTUM_DURATION__SHIFT;

	/* Set cp_hqd_hq_status0 bit 14 to 1 to have the CP set up the
	 * DISPATCH_PTR.  This is required for the kfd debugger
	 */
	m->cp_hqd_hq_status0 = 1 << 14;

	if (q->format == KFD_QUEUE_FORMAT_AQL) {
		m->cp_hqd_aql_control =
			1 << CP_HQD_AQL_CONTROL__CONTROL0__SHIFT;
	}

	if (q->tba_addr) {
		m->compute_pgm_rsrc2 |=
			(1 << COMPUTE_PGM_RSRC2__TRAP_PRESENT__SHIFT);
	}

	if (mm->dev->cwsr_enabled && q->ctx_save_restore_area_address) {
		m->cp_hqd_persistent_state |=
			(1 << CP_HQD_PERSISTENT_STATE__QSWITCH_MODE__SHIFT);
		m->cp_hqd_ctx_save_base_addr_lo =
			lower_32_bits(q->ctx_save_restore_area_address);
		m->cp_hqd_ctx_save_base_addr_hi =
			upper_32_bits(q->ctx_save_restore_area_address);
		m->cp_hqd_ctx_save_size = q->ctx_save_restore_area_size;
		m->cp_hqd_cntl_stack_size = q->ctl_stack_size;
		m->cp_hqd_cntl_stack_offset = q->ctl_stack_size;
		m->cp_hqd_wg_state_offset = q->ctl_stack_size;
	}

	*mqd = m;
	if (gart_addr)
		*gart_addr = addr;
	mm->update_mqd(mm, m, q, NULL);
}

static int load_mqd(struct mqd_manager *mm, void *mqd,
			uint32_t pipe_id, uint32_t queue_id,
			struct queue_properties *p, struct mm_struct *mms)
{
	/* AQL write pointer counts in 64B packets, PM4/CP counts in dwords. */
	uint32_t wptr_shift = (p->format == KFD_QUEUE_FORMAT_AQL ? 4 : 0);

	return mm->dev->kfd2kgd->hqd_load(mm->dev->adev, mqd, pipe_id, queue_id,
					  (uint32_t __user *)p->write_ptr,
					  wptr_shift, 0, mms);
}

static void update_mqd(struct mqd_manager *mm, void *mqd,
			struct queue_properties *q,
			struct mqd_update_info *minfo)
{
	struct v9_mqd *m;

	m = get_mqd(mqd);

	m->cp_hqd_pq_control = 5 << CP_HQD_PQ_CONTROL__RPTR_BLOCK_SIZE__SHIFT;
	m->cp_hqd_pq_control |= order_base_2(q->queue_size / 4) - 1;
	pr_debug("cp_hqd_pq_control 0x%x\n", m->cp_hqd_pq_control);

	m->cp_hqd_pq_base_lo = lower_32_bits((uint64_t)q->queue_address >> 8);
	m->cp_hqd_pq_base_hi = upper_32_bits((uint64_t)q->queue_address >> 8);

	m->cp_hqd_pq_rptr_report_addr_lo = lower_32_bits((uint64_t)q->read_ptr);
	m->cp_hqd_pq_rptr_report_addr_hi = upper_32_bits((uint64_t)q->read_ptr);
	m->cp_hqd_pq_wptr_poll_addr_lo = lower_32_bits((uint64_t)q->write_ptr);
	m->cp_hqd_pq_wptr_poll_addr_hi = upper_32_bits((uint64_t)q->write_ptr);

	m->cp_hqd_pq_doorbell_control =
		q->doorbell_off <<
			CP_HQD_PQ_DOORBELL_CONTROL__DOORBELL_OFFSET__SHIFT;
	pr_debug("cp_hqd_pq_doorbell_control 0x%x\n",
			m->cp_hqd_pq_doorbell_control);

	m->cp_hqd_ib_control =
		3 << CP_HQD_IB_CONTROL__MIN_IB_AVAIL_SIZE__SHIFT |
		1 << CP_HQD_IB_CONTROL__IB_EXE_DISABLE__SHIFT;

	/*
	 * HW does not clamp this field correctly. Maximum EOP queue size
	 * is constrained by per-SE EOP done signal count, which is 8-bit.
	 * Limit is 0xFF EOP entries (= 0x7F8 dwords). CP will not submit
	 * more than (EOP entry count - 1) so a queue size of 0x800 dwords
	 * is safe, giving a maximum field value of 0xA.
	 */
	m->cp_hqd_eop_control = min(0xA,
		order_base_2(q->eop_ring_buffer_size / 4) - 1);
	m->cp_hqd_eop_base_addr_lo =
			lower_32_bits(q->eop_ring_buffer_address >> 8);
	m->cp_hqd_eop_base_addr_hi =
			upper_32_bits(q->eop_ring_buffer_address >> 8);

	m->cp_hqd_iq_timer = 0;

	m->cp_hqd_vmid = q->vmid;

	if (q->format == KFD_QUEUE_FORMAT_AQL) {
		m->cp_hqd_pq_control |= CP_HQD_PQ_CONTROL__NO_UPDATE_RPTR_MASK |
				2 << CP_HQD_PQ_CONTROL__SLOT_BASED_WPTR__SHIFT |
				1 << CP_HQD_PQ_CONTROL__QUEUE_FULL_EN__SHIFT |
				1 << CP_HQD_PQ_CONTROL__WPP_CLAMP_EN__SHIFT;
		m->cp_hqd_pq_doorbell_control |= 1 <<
			CP_HQD_PQ_DOORBELL_CONTROL__DOORBELL_BIF_DROP__SHIFT;
	}
	if (priv_cp_queues)
		m->cp_hqd_pq_control |=
			1 << CP_HQD_PQ_CONTROL__PRIV_STATE__SHIFT;
	if (mm->dev->cwsr_enabled && q->ctx_save_restore_area_address)
		m->cp_hqd_ctx_save_control = 0;

	update_cu_mask(mm, mqd, minfo);
	set_priority(m, q);

	q->is_active = QUEUE_IS_ACTIVE(*q);
}


static uint32_t read_doorbell_id(void *mqd)
{
	struct v9_mqd *m = (struct v9_mqd *)mqd;

	return m->queue_doorbell_id0;
}

static int get_wave_state(struct mqd_manager *mm, void *mqd,
			  void __user *ctl_stack,
			  u32 *ctl_stack_used_size,
			  u32 *save_area_used_size)
{
	struct v9_mqd *m;
	struct mqd_user_context_save_area_header header;

	/* Control stack is located one page after MQD. */
	void *mqd_ctl_stack = (void *)((uintptr_t)mqd + PAGE_SIZE);

	m = get_mqd(mqd);

	*ctl_stack_used_size = m->cp_hqd_cntl_stack_size -
		m->cp_hqd_cntl_stack_offset;
	*save_area_used_size = m->cp_hqd_wg_state_offset -
		m->cp_hqd_cntl_stack_size;

	header.control_stack_size = *ctl_stack_used_size;
	header.wave_state_size = *save_area_used_size;

	header.wave_state_offset = m->cp_hqd_wg_state_offset;
	header.control_stack_offset = m->cp_hqd_cntl_stack_offset;

	if (copy_to_user(ctl_stack, &header, sizeof(header)))
		return -EFAULT;

	if (copy_to_user(ctl_stack + m->cp_hqd_cntl_stack_offset,
				mqd_ctl_stack + m->cp_hqd_cntl_stack_offset,
				*ctl_stack_used_size))

		return -EFAULT;

	return 0;
}

static void get_checkpoint_info(struct mqd_manager *mm, void *mqd, u32 *ctl_stack_size)
{
	struct v9_mqd *m = get_mqd(mqd);

	*ctl_stack_size = m->cp_hqd_cntl_stack_size;
}

static void checkpoint_mqd(struct mqd_manager *mm, void *mqd, void *mqd_dst, void *ctl_stack_dst)
{
	struct v9_mqd *m;
	/* Control stack is located one page after MQD. */
	void *ctl_stack = (void *)((uintptr_t)mqd + PAGE_SIZE);

	m = get_mqd(mqd);

	memcpy(mqd_dst, m, sizeof(struct v9_mqd));
	memcpy(ctl_stack_dst, ctl_stack, m->cp_hqd_cntl_stack_size);
}

static void restore_mqd(struct mqd_manager *mm, void **mqd,
			struct kfd_mem_obj *mqd_mem_obj, uint64_t *gart_addr,
			struct queue_properties *qp,
			const void *mqd_src,
			const void *ctl_stack_src, u32 ctl_stack_size)
{
	uint64_t addr;
	struct v9_mqd *m;
	void *ctl_stack;

	m = (struct v9_mqd *) mqd_mem_obj->cpu_ptr;
	addr = mqd_mem_obj->gpu_addr;

	memcpy(m, mqd_src, sizeof(*m));

	*mqd = m;
	if (gart_addr)
		*gart_addr = addr;

	/* Control stack is located one page after MQD. */
	ctl_stack = (void *)((uintptr_t)*mqd + PAGE_SIZE);
	memcpy(ctl_stack, ctl_stack_src, ctl_stack_size);

	m->cp_hqd_pq_doorbell_control =
		qp->doorbell_off <<
			CP_HQD_PQ_DOORBELL_CONTROL__DOORBELL_OFFSET__SHIFT;
	pr_debug("cp_hqd_pq_doorbell_control 0x%x\n",
				m->cp_hqd_pq_doorbell_control);

	qp->is_active = 0;
}

static void init_mqd_hiq(struct mqd_manager *mm, void **mqd,
			struct kfd_mem_obj *mqd_mem_obj, uint64_t *gart_addr,
			struct queue_properties *q)
{
	struct v9_mqd *m;

	init_mqd(mm, mqd, mqd_mem_obj, gart_addr, q);

	m = get_mqd(*mqd);

	m->cp_hqd_pq_control |= 1 << CP_HQD_PQ_CONTROL__PRIV_STATE__SHIFT |
			1 << CP_HQD_PQ_CONTROL__KMD_QUEUE__SHIFT;
}

static void init_mqd_sdma(struct mqd_manager *mm, void **mqd,
		struct kfd_mem_obj *mqd_mem_obj, uint64_t *gart_addr,
		struct queue_properties *q)
{
	struct v9_sdma_mqd *m;

	m = (struct v9_sdma_mqd *) mqd_mem_obj->cpu_ptr;

	memset(m, 0, sizeof(struct v9_sdma_mqd));

	*mqd = m;
	if (gart_addr)
		*gart_addr = mqd_mem_obj->gpu_addr;

	mm->update_mqd(mm, m, q, NULL);
}

#define SDMA_RLC_DUMMY_DEFAULT 0xf

static void update_mqd_sdma(struct mqd_manager *mm, void *mqd,
			struct queue_properties *q,
			struct mqd_update_info *minfo)
{
	struct v9_sdma_mqd *m;

	m = get_sdma_mqd(mqd);
	m->sdmax_rlcx_rb_cntl = order_base_2(q->queue_size / 4)
		<< SDMA0_RLC0_RB_CNTL__RB_SIZE__SHIFT |
		q->vmid << SDMA0_RLC0_RB_CNTL__RB_VMID__SHIFT |
		1 << SDMA0_RLC0_RB_CNTL__RPTR_WRITEBACK_ENABLE__SHIFT |
		6 << SDMA0_RLC0_RB_CNTL__RPTR_WRITEBACK_TIMER__SHIFT;

	m->sdmax_rlcx_rb_base = lower_32_bits(q->queue_address >> 8);
	m->sdmax_rlcx_rb_base_hi = upper_32_bits(q->queue_address >> 8);
	m->sdmax_rlcx_rb_rptr_addr_lo = lower_32_bits((uint64_t)q->read_ptr);
	m->sdmax_rlcx_rb_rptr_addr_hi = upper_32_bits((uint64_t)q->read_ptr);
	m->sdmax_rlcx_doorbell_offset =
		q->doorbell_off << SDMA0_RLC0_DOORBELL_OFFSET__OFFSET__SHIFT;

	m->sdma_engine_id = q->sdma_engine_id;
	m->sdma_queue_id = q->sdma_queue_id;
	m->sdmax_rlcx_dummy_reg = SDMA_RLC_DUMMY_DEFAULT;

	q->is_active = QUEUE_IS_ACTIVE(*q);
}

static void checkpoint_mqd_sdma(struct mqd_manager *mm,
				void *mqd,
				void *mqd_dst,
				void *ctl_stack_dst)
{
	struct v9_sdma_mqd *m;

	m = get_sdma_mqd(mqd);

	memcpy(mqd_dst, m, sizeof(struct v9_sdma_mqd));
}

static void restore_mqd_sdma(struct mqd_manager *mm, void **mqd,
			     struct kfd_mem_obj *mqd_mem_obj, uint64_t *gart_addr,
			     struct queue_properties *qp,
			     const void *mqd_src,
			     const void *ctl_stack_src, const u32 ctl_stack_size)
{
	uint64_t addr;
	struct v9_sdma_mqd *m;

	m = (struct v9_sdma_mqd *) mqd_mem_obj->cpu_ptr;
	addr = mqd_mem_obj->gpu_addr;

	memcpy(m, mqd_src, sizeof(*m));

	m->sdmax_rlcx_doorbell_offset =
		qp->doorbell_off << SDMA0_RLC0_DOORBELL_OFFSET__OFFSET__SHIFT;

	*mqd = m;
	if (gart_addr)
		*gart_addr = addr;

	qp->is_active = 0;
}

#if defined(CONFIG_DEBUG_FS)

static int debugfs_show_mqd(struct seq_file *m, void *data)
{
	seq_hex_dump(m, "    ", DUMP_PREFIX_OFFSET, 32, 4,
		     data, sizeof(struct v9_mqd), false);
	return 0;
}

static int debugfs_show_mqd_sdma(struct seq_file *m, void *data)
{
	seq_hex_dump(m, "    ", DUMP_PREFIX_OFFSET, 32, 4,
		     data, sizeof(struct v9_sdma_mqd), false);
	return 0;
}

#endif

struct mqd_manager *mqd_manager_init_v9(enum KFD_MQD_TYPE type,
		struct kfd_dev *dev)
{
	struct mqd_manager *mqd;

	if (WARN_ON(type >= KFD_MQD_TYPE_MAX))
		return NULL;

	mqd = kzalloc(sizeof(*mqd), GFP_KERNEL);
	if (!mqd)
		return NULL;

	mqd->dev = dev;

	switch (type) {
	case KFD_MQD_TYPE_CP:
		mqd->allocate_mqd = allocate_mqd;
		mqd->init_mqd = init_mqd;
		mqd->free_mqd = kfd_free_mqd_cp;
		mqd->load_mqd = load_mqd;
		mqd->update_mqd = update_mqd;
		mqd->destroy_mqd = kfd_destroy_mqd_cp;
		mqd->is_occupied = kfd_is_occupied_cp;
		mqd->get_wave_state = get_wave_state;
		mqd->check_queue_active = check_queue_active;
		mqd->get_checkpoint_info = get_checkpoint_info;
		mqd->checkpoint_mqd = checkpoint_mqd;
		mqd->restore_mqd = restore_mqd;
		mqd->mqd_size = sizeof(struct v9_mqd);
#if defined(CONFIG_DEBUG_FS)
		mqd->debugfs_show_mqd = debugfs_show_mqd;
#endif
		break;
	case KFD_MQD_TYPE_HIQ:
		mqd->allocate_mqd = allocate_hiq_mqd;
		mqd->init_mqd = init_mqd_hiq;
		mqd->free_mqd = free_mqd_hiq_sdma;
		mqd->load_mqd = kfd_hiq_load_mqd_kiq;
		mqd->update_mqd = update_mqd;
		mqd->check_queue_active = check_queue_active;
		mqd->destroy_mqd = kfd_destroy_mqd_cp;
		mqd->is_occupied = kfd_is_occupied_cp;
		mqd->mqd_size = sizeof(struct v9_mqd);
#if defined(CONFIG_DEBUG_FS)
		mqd->debugfs_show_mqd = debugfs_show_mqd;
#endif
		mqd->read_doorbell_id = read_doorbell_id;
		break;
	case KFD_MQD_TYPE_DIQ:
		mqd->allocate_mqd = allocate_mqd;
		mqd->init_mqd = init_mqd_hiq;
		mqd->free_mqd = kfd_free_mqd_cp;
		mqd->load_mqd = load_mqd;
		mqd->update_mqd = update_mqd;
		mqd->check_queue_active = check_queue_active;
		mqd->destroy_mqd = kfd_destroy_mqd_cp;
		mqd->is_occupied = kfd_is_occupied_cp;
		mqd->mqd_size = sizeof(struct v9_mqd);
#if defined(CONFIG_DEBUG_FS)
		mqd->debugfs_show_mqd = debugfs_show_mqd;
#endif
		break;
	case KFD_MQD_TYPE_SDMA:
		mqd->allocate_mqd = allocate_sdma_mqd;
		mqd->init_mqd = init_mqd_sdma;
		mqd->free_mqd = free_mqd_hiq_sdma;
		mqd->load_mqd = kfd_load_mqd_sdma;
		mqd->update_mqd = update_mqd_sdma;
		mqd->check_queue_active = check_sdma_queue_active;
		mqd->destroy_mqd = kfd_destroy_mqd_sdma;
		mqd->is_occupied = kfd_is_occupied_sdma;
		mqd->checkpoint_mqd = checkpoint_mqd_sdma;
		mqd->restore_mqd = restore_mqd_sdma;
		mqd->mqd_size = sizeof(struct v9_sdma_mqd);
#if defined(CONFIG_DEBUG_FS)
		mqd->debugfs_show_mqd = debugfs_show_mqd_sdma;
#endif
		break;
	default:
		kfree(mqd);
		return NULL;
	}

	return mqd;
}

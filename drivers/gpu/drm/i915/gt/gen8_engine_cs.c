// SPDX-License-Identifier: MIT
/*
 * Copyright © 2014 Intel Corporation
 */

#include "gen8_engine_cs.h"
#include "i915_drv.h"
#include "intel_lrc.h"
#include "intel_gpu_commands.h"
#include "intel_ring.h"

int gen8_emit_flush_rcs(struct i915_request *rq, u32 mode)
{
	bool vf_flush_wa = false, dc_flush_wa = false;
	u32 *cs, flags = 0;
	int len;

	flags |= PIPE_CONTROL_CS_STALL;

	if (mode & EMIT_FLUSH) {
		flags |= PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DEPTH_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DC_FLUSH_ENABLE;
		flags |= PIPE_CONTROL_FLUSH_ENABLE;
	}

	if (mode & EMIT_INVALIDATE) {
		flags |= PIPE_CONTROL_TLB_INVALIDATE;
		flags |= PIPE_CONTROL_INSTRUCTION_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_VF_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_CONST_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_STATE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_QW_WRITE;
		flags |= PIPE_CONTROL_STORE_DATA_INDEX;

		/*
		 * On GEN9: before VF_CACHE_INVALIDATE we need to emit a NULL
		 * pipe control.
		 */
		if (GRAPHICS_VER(rq->engine->i915) == 9)
			vf_flush_wa = true;

		/* WaForGAMHang:kbl */
		if (IS_KBL_GT_STEP(rq->engine->i915, 0, STEP_B0))
			dc_flush_wa = true;
	}

	len = 6;

	if (vf_flush_wa)
		len += 6;

	if (dc_flush_wa)
		len += 12;

	cs = intel_ring_begin(rq, len);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	if (vf_flush_wa)
		cs = gen8_emit_pipe_control(cs, 0, 0);

	if (dc_flush_wa)
		cs = gen8_emit_pipe_control(cs, PIPE_CONTROL_DC_FLUSH_ENABLE,
					    0);

	cs = gen8_emit_pipe_control(cs, flags, LRC_PPHWSP_SCRATCH_ADDR);

	if (dc_flush_wa)
		cs = gen8_emit_pipe_control(cs, PIPE_CONTROL_CS_STALL, 0);

	intel_ring_advance(rq, cs);

	return 0;
}

int gen8_emit_flush_xcs(struct i915_request *rq, u32 mode)
{
	u32 cmd, *cs;

	cs = intel_ring_begin(rq, 4);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	cmd = MI_FLUSH_DW + 1;

	/*
	 * We always require a command barrier so that subsequent
	 * commands, such as breadcrumb interrupts, are strictly ordered
	 * wrt the contents of the write cache being flushed to memory
	 * (and thus being coherent from the CPU).
	 */
	cmd |= MI_FLUSH_DW_STORE_INDEX | MI_FLUSH_DW_OP_STOREDW;

	if (mode & EMIT_INVALIDATE) {
		cmd |= MI_INVALIDATE_TLB;
		if (rq->engine->class == VIDEO_DECODE_CLASS)
			cmd |= MI_INVALIDATE_BSD;
	}

	*cs++ = cmd;
	*cs++ = LRC_PPHWSP_SCRATCH_ADDR;
	*cs++ = 0; /* upper addr */
	*cs++ = 0; /* value */
	intel_ring_advance(rq, cs);

	return 0;
}

int gen11_emit_flush_rcs(struct i915_request *rq, u32 mode)
{
	if (mode & EMIT_FLUSH) {
		u32 *cs;
		u32 flags = 0;

		flags |= PIPE_CONTROL_CS_STALL;

		flags |= PIPE_CONTROL_TILE_CACHE_FLUSH;
		flags |= PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DEPTH_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DC_FLUSH_ENABLE;
		flags |= PIPE_CONTROL_FLUSH_ENABLE;
		flags |= PIPE_CONTROL_QW_WRITE;
		flags |= PIPE_CONTROL_STORE_DATA_INDEX;

		cs = intel_ring_begin(rq, 6);
		if (IS_ERR(cs))
			return PTR_ERR(cs);

		cs = gen8_emit_pipe_control(cs, flags, LRC_PPHWSP_SCRATCH_ADDR);
		intel_ring_advance(rq, cs);
	}

	if (mode & EMIT_INVALIDATE) {
		u32 *cs;
		u32 flags = 0;

		flags |= PIPE_CONTROL_CS_STALL;

		flags |= PIPE_CONTROL_COMMAND_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TLB_INVALIDATE;
		flags |= PIPE_CONTROL_INSTRUCTION_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_VF_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_CONST_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_STATE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_QW_WRITE;
		flags |= PIPE_CONTROL_STORE_DATA_INDEX;

		cs = intel_ring_begin(rq, 6);
		if (IS_ERR(cs))
			return PTR_ERR(cs);

		cs = gen8_emit_pipe_control(cs, flags, LRC_PPHWSP_SCRATCH_ADDR);
		intel_ring_advance(rq, cs);
	}

	return 0;
}

static u32 preparser_disable(bool state)
{
	return MI_ARB_CHECK | 1 << 8 | state;
}

static i915_reg_t aux_inv_reg(const struct intel_engine_cs *engine)
{
	static const i915_reg_t vd[] = {
		GEN12_VD0_AUX_NV,
		GEN12_VD1_AUX_NV,
		GEN12_VD2_AUX_NV,
		GEN12_VD3_AUX_NV,
	};

	static const i915_reg_t ve[] = {
		GEN12_VE0_AUX_NV,
		GEN12_VE1_AUX_NV,
	};

	if (engine->class == VIDEO_DECODE_CLASS)
		return vd[engine->instance];

	if (engine->class == VIDEO_ENHANCEMENT_CLASS)
		return ve[engine->instance];

	GEM_BUG_ON("unknown aux_inv reg\n");
	return INVALID_MMIO_REG;
}

static u32 *gen12_emit_aux_table_inv(const i915_reg_t inv_reg, u32 *cs)
{
	*cs++ = MI_LOAD_REGISTER_IMM(1);
	*cs++ = i915_mmio_reg_offset(inv_reg);
	*cs++ = AUX_INV;
	*cs++ = MI_NOOP;

	return cs;
}

int gen12_emit_flush_rcs(struct i915_request *rq, u32 mode)
{
	if (mode & EMIT_FLUSH) {
		u32 flags = 0;
		u32 *cs;

		flags |= PIPE_CONTROL_TILE_CACHE_FLUSH;
		flags |= PIPE_CONTROL_FLUSH_L3;
		flags |= PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DEPTH_CACHE_FLUSH;
		/* Wa_1409600907:tgl */
		flags |= PIPE_CONTROL_DEPTH_STALL;
		flags |= PIPE_CONTROL_DC_FLUSH_ENABLE;
		flags |= PIPE_CONTROL_FLUSH_ENABLE;

		flags |= PIPE_CONTROL_STORE_DATA_INDEX;
		flags |= PIPE_CONTROL_QW_WRITE;

		flags |= PIPE_CONTROL_CS_STALL;

		cs = intel_ring_begin(rq, 6);
		if (IS_ERR(cs))
			return PTR_ERR(cs);

		cs = gen12_emit_pipe_control(cs,
					     PIPE_CONTROL0_HDC_PIPELINE_FLUSH,
					     flags, LRC_PPHWSP_SCRATCH_ADDR);
		intel_ring_advance(rq, cs);
	}

	if (mode & EMIT_INVALIDATE) {
		u32 flags = 0;
		u32 *cs;

		flags |= PIPE_CONTROL_COMMAND_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TLB_INVALIDATE;
		flags |= PIPE_CONTROL_INSTRUCTION_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_VF_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_CONST_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_STATE_CACHE_INVALIDATE;

		flags |= PIPE_CONTROL_STORE_DATA_INDEX;
		flags |= PIPE_CONTROL_QW_WRITE;

		flags |= PIPE_CONTROL_CS_STALL;

		cs = intel_ring_begin(rq, 8 + 4);
		if (IS_ERR(cs))
			return PTR_ERR(cs);

		/*
		 * Prevent the pre-parser from skipping past the TLB
		 * invalidate and loading a stale page for the batch
		 * buffer / request payload.
		 */
		*cs++ = preparser_disable(true);

		cs = gen8_emit_pipe_control(cs, flags, LRC_PPHWSP_SCRATCH_ADDR);

		/* hsdes: 1809175790 */
		cs = gen12_emit_aux_table_inv(GEN12_GFX_CCS_AUX_NV, cs);

		*cs++ = preparser_disable(false);
		intel_ring_advance(rq, cs);
	}

	return 0;
}

int gen12_emit_flush_xcs(struct i915_request *rq, u32 mode)
{
	intel_engine_mask_t aux_inv = 0;
	u32 cmd, *cs;

	cmd = 4;
	if (mode & EMIT_INVALIDATE)
		cmd += 2;
	if (mode & EMIT_INVALIDATE)
		aux_inv = rq->engine->mask & ~BIT(BCS0);
	if (aux_inv)
		cmd += 2 * hweight8(aux_inv) + 2;

	cs = intel_ring_begin(rq, cmd);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	if (mode & EMIT_INVALIDATE)
		*cs++ = preparser_disable(true);

	cmd = MI_FLUSH_DW + 1;

	/*
	 * We always require a command barrier so that subsequent
	 * commands, such as breadcrumb interrupts, are strictly ordered
	 * wrt the contents of the write cache being flushed to memory
	 * (and thus being coherent from the CPU).
	 */
	cmd |= MI_FLUSH_DW_STORE_INDEX | MI_FLUSH_DW_OP_STOREDW;

	if (mode & EMIT_INVALIDATE) {
		cmd |= MI_INVALIDATE_TLB;
		if (rq->engine->class == VIDEO_DECODE_CLASS)
			cmd |= MI_INVALIDATE_BSD;
	}

	*cs++ = cmd;
	*cs++ = LRC_PPHWSP_SCRATCH_ADDR;
	*cs++ = 0; /* upper addr */
	*cs++ = 0; /* value */

	if (aux_inv) { /* hsdes: 1809175790 */
		struct intel_engine_cs *engine;
		unsigned int tmp;

		*cs++ = MI_LOAD_REGISTER_IMM(hweight8(aux_inv));
		for_each_engine_masked(engine, rq->engine->gt,
				       aux_inv, tmp) {
			*cs++ = i915_mmio_reg_offset(aux_inv_reg(engine));
			*cs++ = AUX_INV;
		}
		*cs++ = MI_NOOP;
	}

	if (mode & EMIT_INVALIDATE)
		*cs++ = preparser_disable(false);

	intel_ring_advance(rq, cs);

	return 0;
}

static u32 preempt_address(struct intel_engine_cs *engine)
{
	return (i915_ggtt_offset(engine->status_page.vma) +
		I915_GEM_HWS_PREEMPT_ADDR);
}

static u32 hwsp_offset(const struct i915_request *rq)
{
	const struct intel_timeline *tl;

	/* Before the request is executed, the timeline is fixed */
	tl = rcu_dereference_protected(rq->timeline,
				       !i915_request_signaled(rq));

	/* See the comment in i915_request_active_seqno(). */
	return page_mask_bits(tl->hwsp_offset) + offset_in_page(rq->hwsp_seqno);
}

int gen8_emit_init_breadcrumb(struct i915_request *rq)
{
	u32 *cs;

	GEM_BUG_ON(i915_request_has_initial_breadcrumb(rq));
	if (!i915_request_timeline(rq)->has_initial_breadcrumb)
		return 0;

	cs = intel_ring_begin(rq, 6);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	*cs++ = MI_STORE_DWORD_IMM_GEN4 | MI_USE_GGTT;
	*cs++ = hwsp_offset(rq);
	*cs++ = 0;
	*cs++ = rq->fence.seqno - 1;

	/*
	 * Check if we have been preempted before we even get started.
	 *
	 * After this point i915_request_started() reports true, even if
	 * we get preempted and so are no longer running.
	 *
	 * i915_request_started() is used during preemption processing
	 * to decide if the request is currently inside the user payload
	 * or spinning on a kernel semaphore (or earlier). For no-preemption
	 * requests, we do allow preemption on the semaphore before the user
	 * payload, but do not allow preemption once the request is started.
	 *
	 * i915_request_started() is similarly used during GPU hangs to
	 * determine if the user's payload was guilty, and if so, the
	 * request is banned. Before the request is started, it is assumed
	 * to be unharmed and an innocent victim of another's hang.
	 */
	*cs++ = MI_NOOP;
	*cs++ = MI_ARB_CHECK;

	intel_ring_advance(rq, cs);

	/* Record the updated position of the request's payload */
	rq->infix = intel_ring_offset(rq, cs);

	__set_bit(I915_FENCE_FLAG_INITIAL_BREADCRUMB, &rq->fence.flags);

	return 0;
}

int gen8_emit_bb_start_noarb(struct i915_request *rq,
			     u64 offset, u32 len,
			     const unsigned int flags)
{
	u32 *cs;

	cs = intel_ring_begin(rq, 4);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	/*
	 * WaDisableCtxRestoreArbitration:bdw,chv
	 *
	 * We don't need to perform MI_ARB_ENABLE as often as we do (in
	 * particular all the gen that do not need the w/a at all!), if we
	 * took care to make sure that on every switch into this context
	 * (both ordinary and for preemption) that arbitrartion was enabled
	 * we would be fine.  However, for gen8 there is another w/a that
	 * requires us to not preempt inside GPGPU execution, so we keep
	 * arbitration disabled for gen8 batches. Arbitration will be
	 * re-enabled before we close the request
	 * (engine->emit_fini_breadcrumb).
	 */
	*cs++ = MI_ARB_ON_OFF | MI_ARB_DISABLE;

	/* FIXME(BDW+): Address space and security selectors. */
	*cs++ = MI_BATCH_BUFFER_START_GEN8 |
		(flags & I915_DISPATCH_SECURE ? 0 : BIT(8));
	*cs++ = lower_32_bits(offset);
	*cs++ = upper_32_bits(offset);

	intel_ring_advance(rq, cs);

	return 0;
}

int gen8_emit_bb_start(struct i915_request *rq,
		       u64 offset, u32 len,
		       const unsigned int flags)
{
	u32 *cs;

	if (unlikely(i915_request_has_nopreempt(rq)))
		return gen8_emit_bb_start_noarb(rq, offset, len, flags);

	cs = intel_ring_begin(rq, 6);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	*cs++ = MI_ARB_ON_OFF | MI_ARB_ENABLE;

	*cs++ = MI_BATCH_BUFFER_START_GEN8 |
		(flags & I915_DISPATCH_SECURE ? 0 : BIT(8));
	*cs++ = lower_32_bits(offset);
	*cs++ = upper_32_bits(offset);

	*cs++ = MI_ARB_ON_OFF | MI_ARB_DISABLE;
	*cs++ = MI_NOOP;

	intel_ring_advance(rq, cs);

	return 0;
}

static void assert_request_valid(struct i915_request *rq)
{
	struct intel_ring *ring __maybe_unused = rq->ring;

	/* Can we unwind this request without appearing to go forwards? */
	GEM_BUG_ON(intel_ring_direction(ring, rq->wa_tail, rq->head) <= 0);
}

/*
 * Reserve space for 2 NOOPs at the end of each request to be
 * used as a workaround for not being allowed to do lite
 * restore with HEAD==TAIL (WaIdleLiteRestore).
 */
static u32 *gen8_emit_wa_tail(struct i915_request *rq, u32 *cs)
{
	/* Ensure there's always at least one preemption point per-request. */
	*cs++ = MI_ARB_CHECK;
	*cs++ = MI_NOOP;
	rq->wa_tail = intel_ring_offset(rq, cs);

	/* Check that entire request is less than half the ring */
	assert_request_valid(rq);

	return cs;
}

static u32 *emit_preempt_busywait(struct i915_request *rq, u32 *cs)
{
	*cs++ = MI_ARB_CHECK; /* trigger IDLE->ACTIVE first */
	*cs++ = MI_SEMAPHORE_WAIT |
		MI_SEMAPHORE_GLOBAL_GTT |
		MI_SEMAPHORE_POLL |
		MI_SEMAPHORE_SAD_EQ_SDD;
	*cs++ = 0;
	*cs++ = preempt_address(rq->engine);
	*cs++ = 0;
	*cs++ = MI_NOOP;

	return cs;
}

static __always_inline u32*
gen8_emit_fini_breadcrumb_tail(struct i915_request *rq, u32 *cs)
{
	*cs++ = MI_USER_INTERRUPT;

	*cs++ = MI_ARB_ON_OFF | MI_ARB_ENABLE;
	if (intel_engine_has_semaphores(rq->engine))
		cs = emit_preempt_busywait(rq, cs);

	rq->tail = intel_ring_offset(rq, cs);
	assert_ring_tail_valid(rq->ring, rq->tail);

	return gen8_emit_wa_tail(rq, cs);
}

static u32 *emit_xcs_breadcrumb(struct i915_request *rq, u32 *cs)
{
	return gen8_emit_ggtt_write(cs, rq->fence.seqno, hwsp_offset(rq), 0);
}

u32 *gen8_emit_fini_breadcrumb_xcs(struct i915_request *rq, u32 *cs)
{
	return gen8_emit_fini_breadcrumb_tail(rq, emit_xcs_breadcrumb(rq, cs));
}

u32 *gen8_emit_fini_breadcrumb_rcs(struct i915_request *rq, u32 *cs)
{
	cs = gen8_emit_pipe_control(cs,
				    PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
				    PIPE_CONTROL_DEPTH_CACHE_FLUSH |
				    PIPE_CONTROL_DC_FLUSH_ENABLE,
				    0);

	/* XXX flush+write+CS_STALL all in one upsets gem_concurrent_blt:kbl */
	cs = gen8_emit_ggtt_write_rcs(cs,
				      rq->fence.seqno,
				      hwsp_offset(rq),
				      PIPE_CONTROL_FLUSH_ENABLE |
				      PIPE_CONTROL_CS_STALL);

	return gen8_emit_fini_breadcrumb_tail(rq, cs);
}

u32 *gen11_emit_fini_breadcrumb_rcs(struct i915_request *rq, u32 *cs)
{
	cs = gen8_emit_ggtt_write_rcs(cs,
				      rq->fence.seqno,
				      hwsp_offset(rq),
				      PIPE_CONTROL_CS_STALL |
				      PIPE_CONTROL_TILE_CACHE_FLUSH |
				      PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
				      PIPE_CONTROL_DEPTH_CACHE_FLUSH |
				      PIPE_CONTROL_DC_FLUSH_ENABLE |
				      PIPE_CONTROL_FLUSH_ENABLE);

	return gen8_emit_fini_breadcrumb_tail(rq, cs);
}

/*
 * Note that the CS instruction pre-parser will not stall on the breadcrumb
 * flush and will continue pre-fetching the instructions after it before the
 * memory sync is completed. On pre-gen12 HW, the pre-parser will stop at
 * BB_START/END instructions, so, even though we might pre-fetch the pre-amble
 * of the next request before the memory has been flushed, we're guaranteed that
 * we won't access the batch itself too early.
 * However, on gen12+ the parser can pre-fetch across the BB_START/END commands,
 * so, if the current request is modifying an instruction in the next request on
 * the same intel_context, we might pre-fetch and then execute the pre-update
 * instruction. To avoid this, the users of self-modifying code should either
 * disable the parser around the code emitting the memory writes, via a new flag
 * added to MI_ARB_CHECK, or emit the writes from a different intel_context. For
 * the in-kernel use-cases we've opted to use a separate context, see
 * reloc_gpu() as an example.
 * All the above applies only to the instructions themselves. Non-inline data
 * used by the instructions is not pre-fetched.
 */

static u32 *gen12_emit_preempt_busywait(struct i915_request *rq, u32 *cs)
{
	*cs++ = MI_ARB_CHECK; /* trigger IDLE->ACTIVE first */
	*cs++ = MI_SEMAPHORE_WAIT_TOKEN |
		MI_SEMAPHORE_GLOBAL_GTT |
		MI_SEMAPHORE_POLL |
		MI_SEMAPHORE_SAD_EQ_SDD;
	*cs++ = 0;
	*cs++ = preempt_address(rq->engine);
	*cs++ = 0;
	*cs++ = 0;

	return cs;
}

static __always_inline u32*
gen12_emit_fini_breadcrumb_tail(struct i915_request *rq, u32 *cs)
{
	*cs++ = MI_USER_INTERRUPT;

	*cs++ = MI_ARB_ON_OFF | MI_ARB_ENABLE;
	if (intel_engine_has_semaphores(rq->engine))
		cs = gen12_emit_preempt_busywait(rq, cs);

	rq->tail = intel_ring_offset(rq, cs);
	assert_ring_tail_valid(rq->ring, rq->tail);

	return gen8_emit_wa_tail(rq, cs);
}

u32 *gen12_emit_fini_breadcrumb_xcs(struct i915_request *rq, u32 *cs)
{
	/* XXX Stalling flush before seqno write; post-sync not */
	cs = emit_xcs_breadcrumb(rq, __gen8_emit_flush_dw(cs, 0, 0, 0));
	return gen12_emit_fini_breadcrumb_tail(rq, cs);
}

u32 *gen12_emit_fini_breadcrumb_rcs(struct i915_request *rq, u32 *cs)
{
	cs = gen12_emit_ggtt_write_rcs(cs,
				       rq->fence.seqno,
				       hwsp_offset(rq),
				       PIPE_CONTROL0_HDC_PIPELINE_FLUSH,
				       PIPE_CONTROL_CS_STALL |
				       PIPE_CONTROL_TILE_CACHE_FLUSH |
				       PIPE_CONTROL_FLUSH_L3 |
				       PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
				       PIPE_CONTROL_DEPTH_CACHE_FLUSH |
				       /* Wa_1409600907:tgl */
				       PIPE_CONTROL_DEPTH_STALL |
				       PIPE_CONTROL_DC_FLUSH_ENABLE |
				       PIPE_CONTROL_FLUSH_ENABLE);

	return gen12_emit_fini_breadcrumb_tail(rq, cs);
}

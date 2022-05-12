// SPDX-License-Identifier: GPL-2.0
#include <linux/export.h>
#include <linux/percpu.h>
#include <linux/preempt.h>
#include <asm/msr.h>
#define CREATE_TRACE_POINTS
#include <asm/msr-trace.h>

struct msr *msrs_alloc(void)
{
	struct msr *msrs = NULL;

	msrs = alloc_percpu(struct msr);
	if (!msrs) {
		pr_warn("%s: error allocating msrs\n", __func__);
		return NULL;
	}

	return msrs;
}
EXPORT_SYMBOL(msrs_alloc);

void msrs_free(struct msr *msrs)
{
	free_percpu(msrs);
}
EXPORT_SYMBOL(msrs_free);

/**
 * Read an MSR with error handling
 *
 * @msr: MSR to read
 * @m: value to read into
 *
 * It returns read data only on success, otherwise it doesn't change the output
 * argument @m.
 *
 */
static int msr_read(u32 msr, struct msr *m)
{
	int err;
	u64 val;

	err = rdmsrl_safe(msr, &val);
	if (!err)
		m->q = val;

	return err;
}

/**
 * Write an MSR with error handling
 *
 * @msr: MSR to write
 * @m: value to write
 */
static int msr_write(u32 msr, struct msr *m)
{
	return wrmsrl_safe(msr, m->q);
}

static inline int __flip_bit(u32 msr, u8 bit, bool set)
{
	struct msr m, m1;
	int err = -EINVAL;

	if (bit > 63)
		return err;

	err = msr_read(msr, &m);
	if (err)
		return err;

	m1 = m;
	if (set)
		m1.q |=  BIT_64(bit);
	else
		m1.q &= ~BIT_64(bit);

	if (m1.q == m.q)
		return 0;

	err = msr_write(msr, &m1);
	if (err)
		return err;

	return 1;
}

/**
 * Set @bit in a MSR @msr.
 *
 * Retval:
 * < 0: An error was encountered.
 * = 0: Bit was already set.
 * > 0: Hardware accepted the MSR write.
 */
int msr_set_bit(u32 msr, u8 bit)
{
	return __flip_bit(msr, bit, true);
}

/**
 * Clear @bit in a MSR @msr.
 *
 * Retval:
 * < 0: An error was encountered.
 * = 0: Bit was already cleared.
 * > 0: Hardware accepted the MSR write.
 */
int msr_clear_bit(u32 msr, u8 bit)
{
	return __flip_bit(msr, bit, false);
}

#ifdef CONFIG_TRACEPOINTS
void do_trace_write_msr(unsigned int msr, u64 val, int failed)
{
	trace_write_msr(msr, val, failed);
}
EXPORT_SYMBOL(do_trace_write_msr);
EXPORT_TRACEPOINT_SYMBOL(write_msr);

void do_trace_read_msr(unsigned int msr, u64 val, int failed)
{
	trace_read_msr(msr, val, failed);
}
EXPORT_SYMBOL(do_trace_read_msr);
EXPORT_TRACEPOINT_SYMBOL(read_msr);

void do_trace_rdpmc(unsigned counter, u64 val, int failed)
{
	trace_rdpmc(counter, val, failed);
}
EXPORT_SYMBOL(do_trace_rdpmc);
EXPORT_TRACEPOINT_SYMBOL(rdpmc);

#endif

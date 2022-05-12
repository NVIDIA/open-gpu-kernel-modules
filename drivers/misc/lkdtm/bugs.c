// SPDX-License-Identifier: GPL-2.0
/*
 * This is for all the tests related to logic bugs (e.g. bad dereferences,
 * bad alignment, bad loops, bad locking, bad scheduling, deep stacks, and
 * lockups) along with other things that don't fit well into existing LKDTM
 * test source files.
 */
#include "lkdtm.h"
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/task_stack.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#if IS_ENABLED(CONFIG_X86_32) && !IS_ENABLED(CONFIG_UML)
#include <asm/desc.h>
#endif

struct lkdtm_list {
	struct list_head node;
};

/*
 * Make sure our attempts to over run the kernel stack doesn't trigger
 * a compiler warning when CONFIG_FRAME_WARN is set. Then make sure we
 * recurse past the end of THREAD_SIZE by default.
 */
#if defined(CONFIG_FRAME_WARN) && (CONFIG_FRAME_WARN > 0)
#define REC_STACK_SIZE (_AC(CONFIG_FRAME_WARN, UL) / 2)
#else
#define REC_STACK_SIZE (THREAD_SIZE / 8)
#endif
#define REC_NUM_DEFAULT ((THREAD_SIZE / REC_STACK_SIZE) * 2)

static int recur_count = REC_NUM_DEFAULT;

static DEFINE_SPINLOCK(lock_me_up);

/*
 * Make sure compiler does not optimize this function or stack frame away:
 * - function marked noinline
 * - stack variables are marked volatile
 * - stack variables are written (memset()) and read (pr_info())
 * - function has external effects (pr_info())
 * */
static int noinline recursive_loop(int remaining)
{
	volatile char buf[REC_STACK_SIZE];

	memset((void *)buf, remaining & 0xFF, sizeof(buf));
	pr_info("loop %d/%d ...\n", (int)buf[remaining % sizeof(buf)],
		recur_count);
	if (!remaining)
		return 0;
	else
		return recursive_loop(remaining - 1);
}

/* If the depth is negative, use the default, otherwise keep parameter. */
void __init lkdtm_bugs_init(int *recur_param)
{
	if (*recur_param < 0)
		*recur_param = recur_count;
	else
		recur_count = *recur_param;
}

void lkdtm_PANIC(void)
{
	panic("dumptest");
}

void lkdtm_BUG(void)
{
	BUG();
}

static int warn_counter;

void lkdtm_WARNING(void)
{
	WARN_ON(++warn_counter);
}

void lkdtm_WARNING_MESSAGE(void)
{
	WARN(1, "Warning message trigger count: %d\n", ++warn_counter);
}

void lkdtm_EXCEPTION(void)
{
	*((volatile int *) 0) = 0;
}

void lkdtm_LOOP(void)
{
	for (;;)
		;
}

void lkdtm_EXHAUST_STACK(void)
{
	pr_info("Calling function with %lu frame size to depth %d ...\n",
		REC_STACK_SIZE, recur_count);
	recursive_loop(recur_count);
	pr_info("FAIL: survived without exhausting stack?!\n");
}

static noinline void __lkdtm_CORRUPT_STACK(void *stack)
{
	memset(stack, '\xff', 64);
}

/* This should trip the stack canary, not corrupt the return address. */
noinline void lkdtm_CORRUPT_STACK(void)
{
	/* Use default char array length that triggers stack protection. */
	char data[8] __aligned(sizeof(void *));

	pr_info("Corrupting stack containing char array ...\n");
	__lkdtm_CORRUPT_STACK((void *)&data);
}

/* Same as above but will only get a canary with -fstack-protector-strong */
noinline void lkdtm_CORRUPT_STACK_STRONG(void)
{
	union {
		unsigned short shorts[4];
		unsigned long *ptr;
	} data __aligned(sizeof(void *));

	pr_info("Corrupting stack containing union ...\n");
	__lkdtm_CORRUPT_STACK((void *)&data);
}

static pid_t stack_pid;
static unsigned long stack_addr;

void lkdtm_REPORT_STACK(void)
{
	volatile uintptr_t magic;
	pid_t pid = task_pid_nr(current);

	if (pid != stack_pid) {
		pr_info("Starting stack offset tracking for pid %d\n", pid);
		stack_pid = pid;
		stack_addr = (uintptr_t)&magic;
	}

	pr_info("Stack offset: %d\n", (int)(stack_addr - (uintptr_t)&magic));
}

void lkdtm_UNALIGNED_LOAD_STORE_WRITE(void)
{
	static u8 data[5] __attribute__((aligned(4))) = {1, 2, 3, 4, 5};
	u32 *p;
	u32 val = 0x12345678;

	p = (u32 *)(data + 1);
	if (*p == 0)
		val = 0x87654321;
	*p = val;
}

void lkdtm_SOFTLOCKUP(void)
{
	preempt_disable();
	for (;;)
		cpu_relax();
}

void lkdtm_HARDLOCKUP(void)
{
	local_irq_disable();
	for (;;)
		cpu_relax();
}

void lkdtm_SPINLOCKUP(void)
{
	/* Must be called twice to trigger. */
	spin_lock(&lock_me_up);
	/* Let sparse know we intended to exit holding the lock. */
	__release(&lock_me_up);
}

void lkdtm_HUNG_TASK(void)
{
	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule();
}

volatile unsigned int huge = INT_MAX - 2;
volatile unsigned int ignored;

void lkdtm_OVERFLOW_SIGNED(void)
{
	int value;

	value = huge;
	pr_info("Normal signed addition ...\n");
	value += 1;
	ignored = value;

	pr_info("Overflowing signed addition ...\n");
	value += 4;
	ignored = value;
}


void lkdtm_OVERFLOW_UNSIGNED(void)
{
	unsigned int value;

	value = huge;
	pr_info("Normal unsigned addition ...\n");
	value += 1;
	ignored = value;

	pr_info("Overflowing unsigned addition ...\n");
	value += 4;
	ignored = value;
}

/* Intentionally using old-style flex array definition of 1 byte. */
struct array_bounds_flex_array {
	int one;
	int two;
	char data[1];
};

struct array_bounds {
	int one;
	int two;
	char data[8];
	int three;
};

void lkdtm_ARRAY_BOUNDS(void)
{
	struct array_bounds_flex_array *not_checked;
	struct array_bounds *checked;
	volatile int i;

	not_checked = kmalloc(sizeof(*not_checked) * 2, GFP_KERNEL);
	checked = kmalloc(sizeof(*checked) * 2, GFP_KERNEL);

	pr_info("Array access within bounds ...\n");
	/* For both, touch all bytes in the actual member size. */
	for (i = 0; i < sizeof(checked->data); i++)
		checked->data[i] = 'A';
	/*
	 * For the uninstrumented flex array member, also touch 1 byte
	 * beyond to verify it is correctly uninstrumented.
	 */
	for (i = 0; i < sizeof(not_checked->data) + 1; i++)
		not_checked->data[i] = 'A';

	pr_info("Array access beyond bounds ...\n");
	for (i = 0; i < sizeof(checked->data) + 1; i++)
		checked->data[i] = 'B';

	kfree(not_checked);
	kfree(checked);
	pr_err("FAIL: survived array bounds overflow!\n");
}

void lkdtm_CORRUPT_LIST_ADD(void)
{
	/*
	 * Initially, an empty list via LIST_HEAD:
	 *	test_head.next = &test_head
	 *	test_head.prev = &test_head
	 */
	LIST_HEAD(test_head);
	struct lkdtm_list good, bad;
	void *target[2] = { };
	void *redirection = &target;

	pr_info("attempting good list addition\n");

	/*
	 * Adding to the list performs these actions:
	 *	test_head.next->prev = &good.node
	 *	good.node.next = test_head.next
	 *	good.node.prev = test_head
	 *	test_head.next = good.node
	 */
	list_add(&good.node, &test_head);

	pr_info("attempting corrupted list addition\n");
	/*
	 * In simulating this "write what where" primitive, the "what" is
	 * the address of &bad.node, and the "where" is the address held
	 * by "redirection".
	 */
	test_head.next = redirection;
	list_add(&bad.node, &test_head);

	if (target[0] == NULL && target[1] == NULL)
		pr_err("Overwrite did not happen, but no BUG?!\n");
	else
		pr_err("list_add() corruption not detected!\n");
}

void lkdtm_CORRUPT_LIST_DEL(void)
{
	LIST_HEAD(test_head);
	struct lkdtm_list item;
	void *target[2] = { };
	void *redirection = &target;

	list_add(&item.node, &test_head);

	pr_info("attempting good list removal\n");
	list_del(&item.node);

	pr_info("attempting corrupted list removal\n");
	list_add(&item.node, &test_head);

	/* As with the list_add() test above, this corrupts "next". */
	item.node.next = redirection;
	list_del(&item.node);

	if (target[0] == NULL && target[1] == NULL)
		pr_err("Overwrite did not happen, but no BUG?!\n");
	else
		pr_err("list_del() corruption not detected!\n");
}

/* Test that VMAP_STACK is actually allocating with a leading guard page */
void lkdtm_STACK_GUARD_PAGE_LEADING(void)
{
	const unsigned char *stack = task_stack_page(current);
	const unsigned char *ptr = stack - 1;
	volatile unsigned char byte;

	pr_info("attempting bad read from page below current stack\n");

	byte = *ptr;

	pr_err("FAIL: accessed page before stack! (byte: %x)\n", byte);
}

/* Test that VMAP_STACK is actually allocating with a trailing guard page */
void lkdtm_STACK_GUARD_PAGE_TRAILING(void)
{
	const unsigned char *stack = task_stack_page(current);
	const unsigned char *ptr = stack + THREAD_SIZE;
	volatile unsigned char byte;

	pr_info("attempting bad read from page above current stack\n");

	byte = *ptr;

	pr_err("FAIL: accessed page after stack! (byte: %x)\n", byte);
}

void lkdtm_UNSET_SMEP(void)
{
#if IS_ENABLED(CONFIG_X86_64) && !IS_ENABLED(CONFIG_UML)
#define MOV_CR4_DEPTH	64
	void (*direct_write_cr4)(unsigned long val);
	unsigned char *insn;
	unsigned long cr4;
	int i;

	cr4 = native_read_cr4();

	if ((cr4 & X86_CR4_SMEP) != X86_CR4_SMEP) {
		pr_err("FAIL: SMEP not in use\n");
		return;
	}
	cr4 &= ~(X86_CR4_SMEP);

	pr_info("trying to clear SMEP normally\n");
	native_write_cr4(cr4);
	if (cr4 == native_read_cr4()) {
		pr_err("FAIL: pinning SMEP failed!\n");
		cr4 |= X86_CR4_SMEP;
		pr_info("restoring SMEP\n");
		native_write_cr4(cr4);
		return;
	}
	pr_info("ok: SMEP did not get cleared\n");

	/*
	 * To test the post-write pinning verification we need to call
	 * directly into the middle of native_write_cr4() where the
	 * cr4 write happens, skipping any pinning. This searches for
	 * the cr4 writing instruction.
	 */
	insn = (unsigned char *)native_write_cr4;
	for (i = 0; i < MOV_CR4_DEPTH; i++) {
		/* mov %rdi, %cr4 */
		if (insn[i] == 0x0f && insn[i+1] == 0x22 && insn[i+2] == 0xe7)
			break;
		/* mov %rdi,%rax; mov %rax, %cr4 */
		if (insn[i]   == 0x48 && insn[i+1] == 0x89 &&
		    insn[i+2] == 0xf8 && insn[i+3] == 0x0f &&
		    insn[i+4] == 0x22 && insn[i+5] == 0xe0)
			break;
	}
	if (i >= MOV_CR4_DEPTH) {
		pr_info("ok: cannot locate cr4 writing call gadget\n");
		return;
	}
	direct_write_cr4 = (void *)(insn + i);

	pr_info("trying to clear SMEP with call gadget\n");
	direct_write_cr4(cr4);
	if (native_read_cr4() & X86_CR4_SMEP) {
		pr_info("ok: SMEP removal was reverted\n");
	} else {
		pr_err("FAIL: cleared SMEP not detected!\n");
		cr4 |= X86_CR4_SMEP;
		pr_info("restoring SMEP\n");
		native_write_cr4(cr4);
	}
#else
	pr_err("XFAIL: this test is x86_64-only\n");
#endif
}

void lkdtm_DOUBLE_FAULT(void)
{
#if IS_ENABLED(CONFIG_X86_32) && !IS_ENABLED(CONFIG_UML)
	/*
	 * Trigger #DF by setting the stack limit to zero.  This clobbers
	 * a GDT TLS slot, which is okay because the current task will die
	 * anyway due to the double fault.
	 */
	struct desc_struct d = {
		.type = 3,	/* expand-up, writable, accessed data */
		.p = 1,		/* present */
		.d = 1,		/* 32-bit */
		.g = 0,		/* limit in bytes */
		.s = 1,		/* not system */
	};

	local_irq_disable();
	write_gdt_entry(get_cpu_gdt_rw(smp_processor_id()),
			GDT_ENTRY_TLS_MIN, &d, DESCTYPE_S);

	/*
	 * Put our zero-limit segment in SS and then trigger a fault.  The
	 * 4-byte access to (%esp) will fault with #SS, and the attempt to
	 * deliver the fault will recursively cause #SS and result in #DF.
	 * This whole process happens while NMIs and MCEs are blocked by the
	 * MOV SS window.  This is nice because an NMI with an invalid SS
	 * would also double-fault, resulting in the NMI or MCE being lost.
	 */
	asm volatile ("movw %0, %%ss; addl $0, (%%esp)" ::
		      "r" ((unsigned short)(GDT_ENTRY_TLS_MIN << 3)));

	pr_err("FAIL: tried to double fault but didn't die\n");
#else
	pr_err("XFAIL: this test is ia32-only\n");
#endif
}

#ifdef CONFIG_ARM64
static noinline void change_pac_parameters(void)
{
	if (IS_ENABLED(CONFIG_ARM64_PTR_AUTH)) {
		/* Reset the keys of current task */
		ptrauth_thread_init_kernel(current);
		ptrauth_thread_switch_kernel(current);
	}
}
#endif

noinline void lkdtm_CORRUPT_PAC(void)
{
#ifdef CONFIG_ARM64
#define CORRUPT_PAC_ITERATE	10
	int i;

	if (!IS_ENABLED(CONFIG_ARM64_PTR_AUTH))
		pr_err("FAIL: kernel not built with CONFIG_ARM64_PTR_AUTH\n");

	if (!system_supports_address_auth()) {
		pr_err("FAIL: CPU lacks pointer authentication feature\n");
		return;
	}

	pr_info("changing PAC parameters to force function return failure...\n");
	/*
	 * PAC is a hash value computed from input keys, return address and
	 * stack pointer. As pac has fewer bits so there is a chance of
	 * collision, so iterate few times to reduce the collision probability.
	 */
	for (i = 0; i < CORRUPT_PAC_ITERATE; i++)
		change_pac_parameters();

	pr_err("FAIL: survived PAC changes! Kernel may be unstable from here\n");
#else
	pr_err("XFAIL: this test is arm64-only\n");
#endif
}

void lkdtm_FORTIFY_OBJECT(void)
{
	struct target {
		char a[10];
	} target[2] = {};
	int result;

	/*
	 * Using volatile prevents the compiler from determining the value of
	 * 'size' at compile time. Without that, we would get a compile error
	 * rather than a runtime error.
	 */
	volatile int size = 11;

	pr_info("trying to read past the end of a struct\n");

	result = memcmp(&target[0], &target[1], size);

	/* Print result to prevent the code from being eliminated */
	pr_err("FAIL: fortify did not catch an object overread!\n"
	       "\"%d\" was the memcmp result.\n", result);
}

void lkdtm_FORTIFY_SUBOBJECT(void)
{
	struct target {
		char a[10];
		char b[10];
	} target;
	char *src;

	src = kmalloc(20, GFP_KERNEL);
	strscpy(src, "over ten bytes", 20);

	pr_info("trying to strcpy past the end of a member of a struct\n");

	/*
	 * strncpy(target.a, src, 20); will hit a compile error because the
	 * compiler knows at build time that target.a < 20 bytes. Use strcpy()
	 * to force a runtime error.
	 */
	strcpy(target.a, src);

	/* Use target.a to prevent the code from being eliminated */
	pr_err("FAIL: fortify did not catch an sub-object overrun!\n"
	       "\"%s\" was copied.\n", target.a);

	kfree(src);
}

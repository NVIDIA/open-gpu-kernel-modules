/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LKDTM_H
#define __LKDTM_H

#define pr_fmt(fmt) "lkdtm: " fmt

#include <linux/kernel.h>

/* bugs.c */
void __init lkdtm_bugs_init(int *recur_param);
void lkdtm_PANIC(void);
void lkdtm_BUG(void);
void lkdtm_WARNING(void);
void lkdtm_WARNING_MESSAGE(void);
void lkdtm_EXCEPTION(void);
void lkdtm_LOOP(void);
void lkdtm_EXHAUST_STACK(void);
void lkdtm_CORRUPT_STACK(void);
void lkdtm_CORRUPT_STACK_STRONG(void);
void lkdtm_REPORT_STACK(void);
void lkdtm_UNALIGNED_LOAD_STORE_WRITE(void);
void lkdtm_SOFTLOCKUP(void);
void lkdtm_HARDLOCKUP(void);
void lkdtm_SPINLOCKUP(void);
void lkdtm_HUNG_TASK(void);
void lkdtm_OVERFLOW_SIGNED(void);
void lkdtm_OVERFLOW_UNSIGNED(void);
void lkdtm_ARRAY_BOUNDS(void);
void lkdtm_CORRUPT_LIST_ADD(void);
void lkdtm_CORRUPT_LIST_DEL(void);
void lkdtm_STACK_GUARD_PAGE_LEADING(void);
void lkdtm_STACK_GUARD_PAGE_TRAILING(void);
void lkdtm_UNSET_SMEP(void);
void lkdtm_DOUBLE_FAULT(void);
void lkdtm_CORRUPT_PAC(void);
void lkdtm_FORTIFY_OBJECT(void);
void lkdtm_FORTIFY_SUBOBJECT(void);

/* heap.c */
void __init lkdtm_heap_init(void);
void __exit lkdtm_heap_exit(void);
void lkdtm_OVERWRITE_ALLOCATION(void);
void lkdtm_WRITE_AFTER_FREE(void);
void lkdtm_READ_AFTER_FREE(void);
void lkdtm_WRITE_BUDDY_AFTER_FREE(void);
void lkdtm_READ_BUDDY_AFTER_FREE(void);
void lkdtm_SLAB_FREE_DOUBLE(void);
void lkdtm_SLAB_FREE_CROSS(void);
void lkdtm_SLAB_FREE_PAGE(void);

/* perms.c */
void __init lkdtm_perms_init(void);
void lkdtm_WRITE_RO(void);
void lkdtm_WRITE_RO_AFTER_INIT(void);
void lkdtm_WRITE_KERN(void);
void lkdtm_EXEC_DATA(void);
void lkdtm_EXEC_STACK(void);
void lkdtm_EXEC_KMALLOC(void);
void lkdtm_EXEC_VMALLOC(void);
void lkdtm_EXEC_RODATA(void);
void lkdtm_EXEC_USERSPACE(void);
void lkdtm_EXEC_NULL(void);
void lkdtm_ACCESS_USERSPACE(void);
void lkdtm_ACCESS_NULL(void);

/* refcount.c */
void lkdtm_REFCOUNT_INC_OVERFLOW(void);
void lkdtm_REFCOUNT_ADD_OVERFLOW(void);
void lkdtm_REFCOUNT_INC_NOT_ZERO_OVERFLOW(void);
void lkdtm_REFCOUNT_ADD_NOT_ZERO_OVERFLOW(void);
void lkdtm_REFCOUNT_DEC_ZERO(void);
void lkdtm_REFCOUNT_DEC_NEGATIVE(void);
void lkdtm_REFCOUNT_DEC_AND_TEST_NEGATIVE(void);
void lkdtm_REFCOUNT_SUB_AND_TEST_NEGATIVE(void);
void lkdtm_REFCOUNT_INC_ZERO(void);
void lkdtm_REFCOUNT_ADD_ZERO(void);
void lkdtm_REFCOUNT_INC_SATURATED(void);
void lkdtm_REFCOUNT_DEC_SATURATED(void);
void lkdtm_REFCOUNT_ADD_SATURATED(void);
void lkdtm_REFCOUNT_INC_NOT_ZERO_SATURATED(void);
void lkdtm_REFCOUNT_ADD_NOT_ZERO_SATURATED(void);
void lkdtm_REFCOUNT_DEC_AND_TEST_SATURATED(void);
void lkdtm_REFCOUNT_SUB_AND_TEST_SATURATED(void);
void lkdtm_REFCOUNT_TIMING(void);
void lkdtm_ATOMIC_TIMING(void);

/* rodata.c */
void lkdtm_rodata_do_nothing(void);

/* usercopy.c */
void __init lkdtm_usercopy_init(void);
void __exit lkdtm_usercopy_exit(void);
void lkdtm_USERCOPY_HEAP_SIZE_TO(void);
void lkdtm_USERCOPY_HEAP_SIZE_FROM(void);
void lkdtm_USERCOPY_HEAP_WHITELIST_TO(void);
void lkdtm_USERCOPY_HEAP_WHITELIST_FROM(void);
void lkdtm_USERCOPY_STACK_FRAME_TO(void);
void lkdtm_USERCOPY_STACK_FRAME_FROM(void);
void lkdtm_USERCOPY_STACK_BEYOND(void);
void lkdtm_USERCOPY_KERNEL(void);

/* stackleak.c */
void lkdtm_STACKLEAK_ERASING(void);

/* cfi.c */
void lkdtm_CFI_FORWARD_PROTO(void);

/* fortify.c */
void lkdtm_FORTIFIED_STRSCPY(void);

/* powerpc.c */
void lkdtm_PPC_SLB_MULTIHIT(void);

#endif

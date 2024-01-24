/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _NV_PRIV_H_
#define _NV_PRIV_H_

#include <nv.h>
#include <os/os.h>
#include <ctrl/ctrl402c.h>
#include <gpu/disp/kern_disp_max.h>
#include <gpu/disp/kern_disp_type.h>

#define NV_PRIV_REG_WR08(b,o,d)   (*((volatile NvV8*)&(b)->Reg008[(o)/1])=(NvV8)(d))
#define NV_PRIV_REG_WR16(b,o,d)   (*((volatile NvV16*)&(b)->Reg016[(o)/2])=(NvV16)(d))
#define NV_PRIV_REG_WR32(b,o,d)   (*((volatile NvV32*)&(b)->Reg032[(o)/4])=(NvV32)(d))

#define NV_PRIV_REG_RD08(b,o)     ((b)->Reg008[(o)/1])
#define NV_PRIV_REG_RD16(b,o)     ((b)->Reg016[(o)/2])
#define NV_PRIV_REG_RD32(b,o)     ((b)->Reg032[(o)/4])

struct OBJGPU;

typedef struct
{
    NvBool              baseValid;
    VGAADDRDESC         base;
    NvBool              workspaceBaseValid;
    VGAADDRDESC         workspaceBase;
    NvU32               vesaMode;
} nv_vga_t;

/*
* device state during Power Management
*/
typedef struct nv_pm_state_s
{
    NvU32  IntrEn;
    NvBool InHibernate;
} nv_pm_state_t;

/*
* data structure for the UNIX workqueues
*/
typedef struct nv_work_item_s
{
    NvU32   flags;
    NvU32   gpuInstance;
    union
    {
        OSWorkItemFunction *pGpuFunction;
        OSSystemWorkItemFunction *pSystemFunction;
    } func;
    void *pData;
} nv_work_item_t;

#define NV_WORK_ITEM_FLAGS_NONE             0x0
#define NV_WORK_ITEM_FLAGS_REQUIRES_GPU     0x1
#define NV_WORK_ITEM_FLAGS_DONT_FREE_DATA   0x2

#define INVALID_DISP_ID 0xFFFFFFFF
#define MAX_DISP_ID_PER_ADAPTER 0x2

typedef struct nv_i2c_adapter_entry_s
{
    void  *pOsAdapter;
    NvU32  port;
    NvU32  displayId[MAX_DISP_ID_PER_ADAPTER];
} nv_i2c_adapter_entry_t;

#define NV_INIT_FLAG_HAL                  0x0001
#define NV_INIT_FLAG_HAL_COMPONENTS       0x0002
#define NV_INIT_FLAG_GPU_STATE            0x0004
#define NV_INIT_FLAG_GPU_STATE_LOAD       0x0008
#define NV_INIT_FLAG_FIFO_WATCHDOG        0x0010
#define NV_INIT_FLAG_CORE_LOGIC           0x0020
#define NV_INIT_FLAG_GPUMGR_ATTACH        0x0040
#define NV_INIT_FLAG_PUBLIC_I2C           0x0080
#define NV_INIT_FLAG_SCALABILITY          0x0100
#define NV_INIT_FLAG_DMA                  0x0200

#define MAX_I2C_ADAPTERS    NV402C_CTRL_NUM_I2C_PORTS

/*
 * GPU dynamic power state machine.
 *
 * The GPU is in exactly one of these states at at time.  Only certain state
 * transitions are valid, as documented by the DAGs below.
 *
 * When in "instant idle" or COARSE mode:
 *
 *                   +----------------------+
 *                   v                      |
 * +---------+     +----------------+     +--------+
 * | UNKNOWN | --> | IDLE_INDICATED | --> | IN_USE |
 * +---------+     +----------------+     +--------+
 *
 * The transition from UNKNOWN to IDLE_INDICATED happens in
 * rm_init_dynamic_power_management().
 *
 * Thereafter, transitions from IDLE_INDICATED to IN_USE happen when
 * os_ref_dynamic_power() is called and the refcount transitions from 0 to 1;
 * transitions from IN_USE to IDLE_INDICATED happen when
 * os_unref_dynamic_power() is called and the refcount transitions from 1 to 0.
 * Note that only calls to os_(un)ref_dynamic_power() with the mode == COARSE
 * are considered in this mode; calls with mode == FINE are ignored.  Since
 * COARSE calls are placed only in rm_init_adapter/rm_shutdown_adapter, the GPU
 * effectively stays in the IN_USE state any time any client has initialized
 * it.
 *
 *
 * When in "deferred idle" or FINE mode:
 *
 *  +----------------------------------------------------------------+
 *  |                                                                |
 *  |                                                                |
 *  |                    +-------------------------------------------+----------------------+
 *  |                    |                                           |                      v
 *  |  +---------+     +----------------+     +--------------+     +----------------+     +--------+
 *  |  | UNKNOWN | --> | IDLE_INDICATED | --> |              | --> | IDLE_SUSTAINED | --> | IN_USE | -+
 *  |  +---------+     +----------------+     |              |     +----------------+     +--------+  |
 *  |                    ^                    |              |       |                      ^         |
 *  +--------------------+                    | IDLE_INSTANT | ------+----------------------+         |
 *                                            |              |       |                                |
 *                                            |              |       |                                |
 *                                            |              | <-----+                                |
 *                                            +--------------+                                        |
 *                                              ^                                                     |
 *                                              +-----------------------------------------------------+
 *
 * As before, the transition from UNKNOWN to IDLE_INDICATED happens in
 * rm_init_dynamic_power_management().  This is not ideal: it means the GPU may
 * be powered down immediately upon loading the RM module, even if
 * rm_init_adapter() is going to be called soon thereafter.  However, we can't
 * rely on deferred idle callbacks yet, since those currently rely on core RM
 * being initialized.
 *
 * At the beginning of rm_init_adapter(), the GPU transitions to the IN_USE
 * state; during the rm_init_adapter() sequence,
 * RmInitDeferredDynamicPowerManagement() will be called which will schedule
 * timer callbacks and set the "deferred_idle_enabled" boolean.
 *
 * While in "deferred idle" mode, one of the callbacks
 * timerCallbackForIdlePreConditions(), timerCallbackToIndicateIdle(), or
 * RmIndicateIdle() should be scheduled when in the states:
 * - IN_USE
 * - IDLE_INSTANT
 * - IDLE_SUSTAINED
 * Note that since we may transition from IN_USE to IDLE_INSTANT rapidly (e.g.,
 * for a series of RM calls), we don't attempt to schedule the callbacks and
 * cancel them on each of these transitions.  The
 * timerCallbackForIdlePreConditions() callback will simply exit early if in
 * the IN_USE state.
 *
 * As before, the GPU will remain in the IN_USE state until
 * os_unref_dynamic_power() is called and the count transitions from 1 to 0
 * (calls with mode == FINE are honored, in this mode, and these transitions
 * can happen frequently).  When the refcount reaches 0, rather than going
 * directly to the IDLE_INDICATED state, it transitions to the IDLE_INSTANT
 * state.
 *
 * Then, when the next timerCallbackForIdlePreConditions() callback executes,
 * if all preconditions are met, the state will transition to IDLE_SUSTAINED.
 *
 * If, when in the IDLE_SUSTAINED state, os_ref_dynamic_power() is called, the
 * GPU will transition back to the IN_USE state and return to the IDLE_INSTANT
 * state.  This ensures that there is a suitable delay between any activity
 * that requires bumping the refcount and indicating idleness.
 *
 * If the timerCallbackForIdlePreConditions() callback executes again and the
 * GPU is still in the IDLE_SUSTAINED state, userspace mappings will be revoked
 * and the timerCallbackToIndicateIdle() callback will be scheduled.
 *
 * If, before the timerCallbackToIndicateIdle() callback executes, either
 * os_ref_dynamic_power() is called or a mapping which has been revoked is
 * accessed (which triggers the RmForceGpuNotIdle() callback), the GPU will
 * transition back to the IN_USE or IDLE_INSTANT state, respectively.
 *
 * Then, when the timerCallbackToIndicateIdle() callback executes, if all
 * mappings are still revoked, and the GPU is still in the IDLE_SUSTAINED
 * state, and all GPU idleness preconditions remain satisfied, the
 * RmIndicateIdle() work item will be enqueued.  (Else, the GPU will transition
 * back to the IDLE_INSTANT state and the callback for preconditions is
 * scheduled again.)
 *
 * Finally, once the RmIndicateIdle() work item is called, if all of the same
 * conditions still hold, the state will transition to IDLE_INDICATED.  No
 * callbacks will be scheduled from here; the callbacks for preconditions
 * should be re-scheduled when transitioning out of the IDLE_INDICATED state.
 *
 * Once in the IDLE_INDICATED state, the kernel is free to call the RM to
 * perform the GC6 entry sequence then turn off power to the GPU (although it
 * may not, if the audio function is being used for example).
 *
 * There are two paths to exit the IDLE_INDICATED state:
 * (a) If os_ref_dynamic_power() is called, in which case it transitions
 *     directly to the IN_USE state;
 * (b) If RmForceGpuNotIdle() is called, in which case it transitions back to
 *     the IDLE_INSTANT state.
 */
typedef enum
{
    NV_DYNAMIC_POWER_STATE_UNKNOWN = 0,

    NV_DYNAMIC_POWER_STATE_IN_USE,

    NV_DYNAMIC_POWER_STATE_IDLE_INSTANT,
    NV_DYNAMIC_POWER_STATE_IDLE_SUSTAINED,
    NV_DYNAMIC_POWER_STATE_IDLE_INDICATED,
} nv_dynamic_power_state_t;

typedef struct nv_dynamic_power_s
{
    /*
     * mode is read without the mutex -- should be read-only outside of
     * rm_init_dynamic_power_management, called during probe only.
     */
    nv_dynamic_power_mode_t mode;
    /*
     * Whether to indicate idle immediately when the refcount reaches 0, or
     * only go to the IDLE_INSTANT state, and expect timer callbacks to
     * transition through IDLE_SUSTAINED -> IDLE_INDICATED.
     */
    NvBool deferred_idle_enabled;

    nv_dynamic_power_state_t state;
    NvS32 refcount;

    /*
     * A word on lock ordering.  These locks must be taken in the order:
     *
     *      RM API lock > this dynamic_power mutex > RM GPUs lock
     *
     * Skipping any of those locks is fine (if they aren't required to protect
     * whatever state is being accessed or modified), so long as the order is
     * not violated.
     */
    PORT_MUTEX *mutex;

    /*
     * callback handles for deferred dynamic power management.
     */
    NvP64 idle_precondition_check_event;
    NvP64 indicate_idle_event;
    NvBool idle_precondition_check_callback_scheduled;

    /*
     * callback handle for kernel initiated gc6 entry/exit.
     * these will be protected by the gpu lock.
     */
    NvP64 remove_idle_holdoff;
    NvBool b_idle_holdoff;

    /*
     * flag set if the platform does not support fine grain dynamic power
     * management.
     */
    NvBool b_fine_not_supported;

    /*
     * This flag is used to check if a workitem is queued for 
     * RmQueueIdleSustainedWorkitem().
     */
    NvBool b_idle_sustained_workitem_queued;

    /*
     * Counter to track clients disallowing GCOFF.
     */
    NvU32 clients_gcoff_disallow_refcount;

    /*
     * Maximum FB allocation size which can be saved in system memory
     * while doing GCOFF based dynamic PM.
     */
    NvU64 gcoff_max_fb_size;

    /*
     * NVreg_DynamicPowerManagement regkey value set by the user
     */
    NvU32 dynamic_power_regkey;
} nv_dynamic_power_t;

typedef struct
{
    OBJGPU *pGpu;

    NvU32 pmc_boot_0;

    nv_vga_t vga;

    NvU32 flags;
    NvU32 status;

    nv_i2c_adapter_entry_t i2c_adapters[MAX_I2C_ADAPTERS];

    void *pVbiosCopy;
    NvU32 vbiosSize;

    nv_pm_state_t pm_state;

    nv_reg_entry_t *pRegistry;

    nv_dynamic_power_t dynamic_power;

    /* Flag to check if the GPU needs 4K page isolation. */
    NvBool b_4k_page_isolation_required;

    /* Flag to check if GPU mobile config is enabled */
    NvBool b_mobile_config_enabled;

    /* Flag to check if S0ix-based power management is enabled. */
    NvBool s0ix_pm_enabled;

    /*
     * Maximum FB allocation size which can be saved in system memory
     * during system supened with S0ix-based power management.
     */
    NvU64 s0ix_gcoff_max_fb_size;

    NvU32 pmc_boot_1;
    NvU32 pmc_boot_42;
} nv_priv_t;

#define NV_SET_NV_PRIV(nv,p) ((nv)->priv = (p))
#define NV_GET_NV_PRIV(nv)   ((nv) ? (nv)->priv : NULL)

/*
 * Make sure that your stack has taken API Lock before using this macro.
 */
#define NV_GET_NV_PRIV_PGPU(nv) \
    (NV_GET_NV_PRIV(nv) ? ((nv_priv_t *)NV_GET_NV_PRIV(nv))->pGpu : NULL)

#endif // _NV_PRIV_H_

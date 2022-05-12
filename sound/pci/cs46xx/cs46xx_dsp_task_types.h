/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  The driver for the Cirrus Logic's Sound Fusion CS46XX based soundcards
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *
 * NOTE: comments are copy/paste from cwcemb80.lst 
 * provided by Tom Woller at Cirrus (my only
 * documentation about the SP OS running inside
 * the DSP) 
 */

#ifndef __CS46XX_DSP_TASK_TYPES_H__
#define __CS46XX_DSP_TASK_TYPES_H__

#include "cs46xx_dsp_scb_types.h"

/*********************************************************************************************
Example hierarchy of stream control blocks in the SP

hfgTree
Ptr____Call (c)
       \
 -------+------         -------------      -------------      -------------      -----
| SBlaster IF  |______\| Foreground  |___\| Middlegr'nd |___\| Background  |___\| Nul |
|              |Goto  /| tree header |g  /| tree header |g  /| tree header |g  /| SCB |r
 -------------- (g)     -------------      -------------      -------------      -----
       |c                     |c                 |c                 |c
       |                      |                  |                  |
      \/                  -------------      -------------      -------------   
                       | Foreground  |_\  | Middlegr'nd |_\  | Background  |_\
                       |     tree    |g/  |    tree     |g/  |     tree    |g/
                        -------------      -------------      -------------   
                              |c                 |c                 |c
                              |                  |                  | 
                             \/                 \/                 \/ 

*********************************************************************************************/

#define		HFG_FIRST_EXECUTE_MODE			0x0001
#define		HFG_FIRST_EXECUTE_MODE_BIT		0
#define		HFG_CONTEXT_SWITCH_MODE			0x0002
#define		HFG_CONTEXT_SWITCH_MODE_BIT		1

#define MAX_FG_STACK_SIZE 	32			/* THESE NEED TO BE COMPUTED PROPERLY */
#define MAX_MG_STACK_SIZE 	16
#define MAX_BG_STACK_SIZE 	9
#define MAX_HFG_STACK_SIZE	4

#define SLEEP_ACTIVE_INCREMENT		0		/* Enable task tree thread to go to sleep
											   This should only ever be used on the Background thread */
#define STANDARD_ACTIVE_INCREMENT	1		/* Task tree thread normal operation */
#define SUSPEND_ACTIVE_INCREMENT	2		/* Cause execution to suspend in the task tree thread
                                               This should only ever be used on the Background thread */

#define HOSTFLAGS_DISABLE_BG_SLEEP  0       /* Host-controlled flag that determines whether we go to sleep
                                               at the end of BG */

/* Minimal context save area for Hyper Forground */
struct dsp_hf_save_area {
	u32	r10_save;
	u32	r54_save;
	u32	r98_save;

	___DSP_DUAL_16BIT_ALLOC(
	    status_save,
	    ind_save
	)

	___DSP_DUAL_16BIT_ALLOC(
	    rci1_save,
	    rci0_save
	)

	u32	r32_save;
	u32	r76_save;
	u32	rsd2_save;

       	___DSP_DUAL_16BIT_ALLOC(
	      rsi2_save,	  /* See TaskTreeParameterBlock for 
				     remainder of registers  */
	      rsa2Save
	)
	/* saved as part of HFG context  */
};


/* Task link data structure */
struct dsp_tree_link {
	___DSP_DUAL_16BIT_ALLOC(
	/* Pointer to sibling task control block */
	    next_scb,
	/* Pointer to child task control block */
	    sub_ptr
	)
  
	___DSP_DUAL_16BIT_ALLOC(
	/* Pointer to code entry point */
	    entry_point, 
	/* Pointer to local data */
	    this_spb
	)
};


struct dsp_task_tree_data {
	___DSP_DUAL_16BIT_ALLOC(
	/* Initial tock count; controls task tree execution rate */
	    tock_count_limit,
	/* Tock down counter */
	    tock_count
	)

	/* Add to ActiveCount when TockCountLimit reached: 
	   Subtract on task tree termination */
	___DSP_DUAL_16BIT_ALLOC(
	    active_tncrement,		
	/* Number of pending activations for task tree */
	    active_count
	)

        ___DSP_DUAL_16BIT_ALLOC(
	/* BitNumber to enable modification of correct bit in ActiveTaskFlags */
	    active_bit,	    
	/* Pointer to OS location for indicating current activity on task level */
	    active_task_flags_ptr
	)

	/* Data structure for controlling movement of memory blocks:- 
	   currently unused */
	___DSP_DUAL_16BIT_ALLOC(
	    mem_upd_ptr,
	/* Data structure for controlling synchronous link update */
	    link_upd_ptr
	)
  
	___DSP_DUAL_16BIT_ALLOC(
	/* Save area for remainder of full context. */
	    save_area,
	/* Address of start of local stack for data storage */
	    data_stack_base_ptr
	)

};


struct dsp_interval_timer_data
{
	/* These data items have the same relative locations to those */
	___DSP_DUAL_16BIT_ALLOC(
	     interval_timer_period,
	     itd_unused
	)

	/* used for this data in the SPOS control block for SPOS 1.0 */
	___DSP_DUAL_16BIT_ALLOC(
	     num_FG_ticks_this_interval,        
	     num_intervals
	)
};


/* This structure contains extra storage for the task tree
   Currently, this additional data is related only to a full context save */
struct dsp_task_tree_context_block {
	/* Up to 10 values are saved onto the stack.  8 for the task tree, 1 for
	   The access to the context switch (call or interrupt), and 1 spare that
	   users should never use.  This last may be required by the system */
	___DSP_DUAL_16BIT_ALLOC(
	     stack1,
	     stack0
	)
	___DSP_DUAL_16BIT_ALLOC(
	     stack3,
	     stack2
	)
	___DSP_DUAL_16BIT_ALLOC(
	     stack5,
	     stack4
	)
	___DSP_DUAL_16BIT_ALLOC(
	     stack7,
	     stack6
	)
	___DSP_DUAL_16BIT_ALLOC(
	     stack9,
	     stack8
	)

	u32	  saverfe;					

	/* Value may be overwritten by stack save algorithm.
	   Retain the size of the stack data saved here if used */
	___DSP_DUAL_16BIT_ALLOC(
             reserved1,	
  	     stack_size
	)
	u32		saverba;	  /* (HFG) */
	u32		saverdc;
	u32		savers_config_23; /* (HFG) */
	u32		savers_DMA23;	  /* (HFG) */
	u32		saversa0;
	u32		saversi0;
	u32		saversa1;
	u32		saversi1;
	u32		saversa3;
	u32		saversd0;
	u32		saversd1;
	u32		saversd3;
	u32		savers_config01;
	u32		savers_DMA01;
	u32		saveacc0hl;
	u32		saveacc1hl;
	u32		saveacc0xacc1x;
	u32		saveacc2hl;
	u32		saveacc3hl;
	u32		saveacc2xacc3x;
	u32		saveaux0hl;
	u32		saveaux1hl;
	u32		saveaux0xaux1x;
	u32		saveaux2hl;
	u32		saveaux3hl;
	u32		saveaux2xaux3x;
	u32		savershouthl;
	u32		savershoutxmacmode;
};
                

struct dsp_task_tree_control_block {
	struct dsp_hf_save_area			context;
	struct dsp_tree_link			links;
	struct dsp_task_tree_data		data;
	struct dsp_task_tree_context_block	context_blk;
	struct dsp_interval_timer_data		int_timer;
};


#endif /* __DSP_TASK_TYPES_H__ */

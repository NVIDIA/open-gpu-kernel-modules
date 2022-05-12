/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * ipmi_si_sm.h
 *
 * State machine interface for low-level IPMI system management
 * interface state machines.  This code is the interface between
 * the ipmi_smi code (that handles the policy of a KCS, SMIC, or
 * BT interface) and the actual low-level state machine.
 *
 * Author: MontaVista Software, Inc.
 *         Corey Minyard <minyard@mvista.com>
 *         source@mvista.com
 *
 * Copyright 2002 MontaVista Software Inc.
 */

#ifndef __IPMI_SI_SM_H__
#define __IPMI_SI_SM_H__

#include "ipmi_si.h"

/*
 * This is defined by the state machines themselves, it is an opaque
 * data type for them to use.
 */
struct si_sm_data;

/* Results of SMI events. */
enum si_sm_result {
	SI_SM_CALL_WITHOUT_DELAY, /* Call the driver again immediately */
	SI_SM_CALL_WITH_DELAY,	/* Delay some before calling again. */
	SI_SM_CALL_WITH_TICK_DELAY,/* Delay >=1 tick before calling again. */
	SI_SM_TRANSACTION_COMPLETE, /* A transaction is finished. */
	SI_SM_IDLE,		/* The SM is in idle state. */
	SI_SM_HOSED,		/* The hardware violated the state machine. */

	/*
	 * The hardware is asserting attn and the state machine is
	 * idle.
	 */
	SI_SM_ATTN
};

/* Handlers for the SMI state machine. */
struct si_sm_handlers {
	/*
	 * Put the version number of the state machine here so the
	 * upper layer can print it.
	 */
	char *version;

	/*
	 * Initialize the data and return the amount of I/O space to
	 * reserve for the space.
	 */
	unsigned int (*init_data)(struct si_sm_data *smi,
				  struct si_sm_io   *io);

	/*
	 * Start a new transaction in the state machine.  This will
	 * return -2 if the state machine is not idle, -1 if the size
	 * is invalid (to large or too small), or 0 if the transaction
	 * is successfully completed.
	 */
	int (*start_transaction)(struct si_sm_data *smi,
				 unsigned char *data, unsigned int size);

	/*
	 * Return the results after the transaction.  This will return
	 * -1 if the buffer is too small, zero if no transaction is
	 * present, or the actual length of the result data.
	 */
	int (*get_result)(struct si_sm_data *smi,
			  unsigned char *data, unsigned int length);

	/*
	 * Call this periodically (for a polled interface) or upon
	 * receiving an interrupt (for a interrupt-driven interface).
	 * If interrupt driven, you should probably poll this
	 * periodically when not in idle state.  This should be called
	 * with the time that passed since the last call, if it is
	 * significant.  Time is in microseconds.
	 */
	enum si_sm_result (*event)(struct si_sm_data *smi, long time);

	/*
	 * Attempt to detect an SMI.  Returns 0 on success or nonzero
	 * on failure.
	 */
	int (*detect)(struct si_sm_data *smi);

	/* The interface is shutting down, so clean it up. */
	void (*cleanup)(struct si_sm_data *smi);

	/* Return the size of the SMI structure in bytes. */
	int (*size)(void);
};

/* Current state machines that we can use. */
extern const struct si_sm_handlers kcs_smi_handlers;
extern const struct si_sm_handlers smic_smi_handlers;
extern const struct si_sm_handlers bt_smi_handlers;

#endif /* __IPMI_SI_SM_H__ */

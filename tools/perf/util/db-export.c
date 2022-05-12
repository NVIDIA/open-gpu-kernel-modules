// SPDX-License-Identifier: GPL-2.0-only
/*
 * db-export.c: Support for exporting data suitable for import to a database
 * Copyright (c) 2014, Intel Corporation.
 */

#include <errno.h>
#include <stdlib.h>

#include "dso.h"
#include "evsel.h"
#include "machine.h"
#include "thread.h"
#include "comm.h"
#include "symbol.h"
#include "map.h"
#include "event.h"
#include "thread-stack.h"
#include "callchain.h"
#include "call-path.h"
#include "db-export.h"
#include <linux/zalloc.h>

int db_export__init(struct db_export *dbe)
{
	memset(dbe, 0, sizeof(struct db_export));
	return 0;
}

void db_export__exit(struct db_export *dbe)
{
	call_return_processor__free(dbe->crp);
	dbe->crp = NULL;
}

int db_export__evsel(struct db_export *dbe, struct evsel *evsel)
{
	if (evsel->db_id)
		return 0;

	evsel->db_id = ++dbe->evsel_last_db_id;

	if (dbe->export_evsel)
		return dbe->export_evsel(dbe, evsel);

	return 0;
}

int db_export__machine(struct db_export *dbe, struct machine *machine)
{
	if (machine->db_id)
		return 0;

	machine->db_id = ++dbe->machine_last_db_id;

	if (dbe->export_machine)
		return dbe->export_machine(dbe, machine);

	return 0;
}

int db_export__thread(struct db_export *dbe, struct thread *thread,
		      struct machine *machine, struct thread *main_thread)
{
	u64 main_thread_db_id = 0;

	if (thread->db_id)
		return 0;

	thread->db_id = ++dbe->thread_last_db_id;

	if (main_thread)
		main_thread_db_id = main_thread->db_id;

	if (dbe->export_thread)
		return dbe->export_thread(dbe, thread, main_thread_db_id,
					  machine);

	return 0;
}

static int __db_export__comm(struct db_export *dbe, struct comm *comm,
			     struct thread *thread)
{
	comm->db_id = ++dbe->comm_last_db_id;

	if (dbe->export_comm)
		return dbe->export_comm(dbe, comm, thread);

	return 0;
}

int db_export__comm(struct db_export *dbe, struct comm *comm,
		    struct thread *thread)
{
	if (comm->db_id)
		return 0;

	return __db_export__comm(dbe, comm, thread);
}

/*
 * Export the "exec" comm. The "exec" comm is the program / application command
 * name at the time it first executes. It is used to group threads for the same
 * program. Note that the main thread pid (or thread group id tgid) cannot be
 * used because it does not change when a new program is exec'ed.
 */
int db_export__exec_comm(struct db_export *dbe, struct comm *comm,
			 struct thread *main_thread)
{
	int err;

	if (comm->db_id)
		return 0;

	err = __db_export__comm(dbe, comm, main_thread);
	if (err)
		return err;

	/*
	 * Record the main thread for this comm. Note that the main thread can
	 * have many "exec" comms because there will be a new one every time it
	 * exec's. An "exec" comm however will only ever have 1 main thread.
	 * That is different to any other threads for that same program because
	 * exec() will effectively kill them, so the relationship between the
	 * "exec" comm and non-main threads is 1-to-1. That is why
	 * db_export__comm_thread() is called here for the main thread, but it
	 * is called for non-main threads when they are exported.
	 */
	return db_export__comm_thread(dbe, comm, main_thread);
}

int db_export__comm_thread(struct db_export *dbe, struct comm *comm,
			   struct thread *thread)
{
	u64 db_id;

	db_id = ++dbe->comm_thread_last_db_id;

	if (dbe->export_comm_thread)
		return dbe->export_comm_thread(dbe, db_id, comm, thread);

	return 0;
}

int db_export__dso(struct db_export *dbe, struct dso *dso,
		   struct machine *machine)
{
	if (dso->db_id)
		return 0;

	dso->db_id = ++dbe->dso_last_db_id;

	if (dbe->export_dso)
		return dbe->export_dso(dbe, dso, machine);

	return 0;
}

int db_export__symbol(struct db_export *dbe, struct symbol *sym,
		      struct dso *dso)
{
	u64 *sym_db_id = symbol__priv(sym);

	if (*sym_db_id)
		return 0;

	*sym_db_id = ++dbe->symbol_last_db_id;

	if (dbe->export_symbol)
		return dbe->export_symbol(dbe, sym, dso);

	return 0;
}

static int db_ids_from_al(struct db_export *dbe, struct addr_location *al,
			  u64 *dso_db_id, u64 *sym_db_id, u64 *offset)
{
	int err;

	if (al->map) {
		struct dso *dso = al->map->dso;

		err = db_export__dso(dbe, dso, al->maps->machine);
		if (err)
			return err;
		*dso_db_id = dso->db_id;

		if (!al->sym) {
			al->sym = symbol__new(al->addr, 0, 0, 0, "unknown");
			if (al->sym)
				dso__insert_symbol(dso, al->sym);
		}

		if (al->sym) {
			u64 *db_id = symbol__priv(al->sym);

			err = db_export__symbol(dbe, al->sym, dso);
			if (err)
				return err;
			*sym_db_id = *db_id;
			*offset = al->addr - al->sym->start;
		}
	}

	return 0;
}

static struct call_path *call_path_from_sample(struct db_export *dbe,
					       struct machine *machine,
					       struct thread *thread,
					       struct perf_sample *sample,
					       struct evsel *evsel)
{
	u64 kernel_start = machine__kernel_start(machine);
	struct call_path *current = &dbe->cpr->call_path;
	enum chain_order saved_order = callchain_param.order;
	int err;

	if (!symbol_conf.use_callchain || !sample->callchain)
		return NULL;

	/*
	 * Since the call path tree must be built starting with the root, we
	 * must use ORDER_CALL for call chain resolution, in order to process
	 * the callchain starting with the root node and ending with the leaf.
	 */
	callchain_param.order = ORDER_CALLER;
	err = thread__resolve_callchain(thread, &callchain_cursor, evsel,
					sample, NULL, NULL, PERF_MAX_STACK_DEPTH);
	if (err) {
		callchain_param.order = saved_order;
		return NULL;
	}
	callchain_cursor_commit(&callchain_cursor);

	while (1) {
		struct callchain_cursor_node *node;
		struct addr_location al;
		u64 dso_db_id = 0, sym_db_id = 0, offset = 0;

		memset(&al, 0, sizeof(al));

		node = callchain_cursor_current(&callchain_cursor);
		if (!node)
			break;
		/*
		 * Handle export of symbol and dso for this node by
		 * constructing an addr_location struct and then passing it to
		 * db_ids_from_al() to perform the export.
		 */
		al.sym = node->ms.sym;
		al.map = node->ms.map;
		al.maps = thread->maps;
		al.addr = node->ip;

		if (al.map && !al.sym)
			al.sym = dso__find_symbol(al.map->dso, al.addr);

		db_ids_from_al(dbe, &al, &dso_db_id, &sym_db_id, &offset);

		/* add node to the call path tree if it doesn't exist */
		current = call_path__findnew(dbe->cpr, current,
					     al.sym, node->ip,
					     kernel_start);

		callchain_cursor_advance(&callchain_cursor);
	}

	/* Reset the callchain order to its prior value. */
	callchain_param.order = saved_order;

	if (current == &dbe->cpr->call_path) {
		/* Bail because the callchain was empty. */
		return NULL;
	}

	return current;
}

int db_export__branch_type(struct db_export *dbe, u32 branch_type,
			   const char *name)
{
	if (dbe->export_branch_type)
		return dbe->export_branch_type(dbe, branch_type, name);

	return 0;
}

static int db_export__threads(struct db_export *dbe, struct thread *thread,
			      struct thread *main_thread,
			      struct machine *machine, struct comm **comm_ptr)
{
	struct comm *comm = NULL;
	struct comm *curr_comm;
	int err;

	if (main_thread) {
		/*
		 * A thread has a reference to the main thread, so export the
		 * main thread first.
		 */
		err = db_export__thread(dbe, main_thread, machine, main_thread);
		if (err)
			return err;
		/*
		 * Export comm before exporting the non-main thread because
		 * db_export__comm_thread() can be called further below.
		 */
		comm = machine__thread_exec_comm(machine, main_thread);
		if (comm) {
			err = db_export__exec_comm(dbe, comm, main_thread);
			if (err)
				return err;
			*comm_ptr = comm;
		}
	}

	if (thread != main_thread) {
		/*
		 * For a non-main thread, db_export__comm_thread() must be
		 * called only if thread has not previously been exported.
		 */
		bool export_comm_thread = comm && !thread->db_id;

		err = db_export__thread(dbe, thread, machine, main_thread);
		if (err)
			return err;

		if (export_comm_thread) {
			err = db_export__comm_thread(dbe, comm, thread);
			if (err)
				return err;
		}
	}

	curr_comm = thread__comm(thread);
	if (curr_comm)
		return db_export__comm(dbe, curr_comm, thread);

	return 0;
}

int db_export__sample(struct db_export *dbe, union perf_event *event,
		      struct perf_sample *sample, struct evsel *evsel,
		      struct addr_location *al)
{
	struct thread *thread = al->thread;
	struct export_sample es = {
		.event = event,
		.sample = sample,
		.evsel = evsel,
		.al = al,
	};
	struct thread *main_thread;
	struct comm *comm = NULL;
	int err;

	err = db_export__evsel(dbe, evsel);
	if (err)
		return err;

	err = db_export__machine(dbe, al->maps->machine);
	if (err)
		return err;

	main_thread = thread__main_thread(al->maps->machine, thread);

	err = db_export__threads(dbe, thread, main_thread, al->maps->machine, &comm);
	if (err)
		goto out_put;

	if (comm)
		es.comm_db_id = comm->db_id;

	es.db_id = ++dbe->sample_last_db_id;

	err = db_ids_from_al(dbe, al, &es.dso_db_id, &es.sym_db_id, &es.offset);
	if (err)
		goto out_put;

	if (dbe->cpr) {
		struct call_path *cp = call_path_from_sample(dbe, al->maps->machine,
							     thread, sample,
							     evsel);
		if (cp) {
			db_export__call_path(dbe, cp);
			es.call_path_id = cp->db_id;
		}
	}

	if ((evsel->core.attr.sample_type & PERF_SAMPLE_ADDR) &&
	    sample_addr_correlates_sym(&evsel->core.attr)) {
		struct addr_location addr_al;

		thread__resolve(thread, &addr_al, sample);
		err = db_ids_from_al(dbe, &addr_al, &es.addr_dso_db_id,
				     &es.addr_sym_db_id, &es.addr_offset);
		if (err)
			goto out_put;
		if (dbe->crp) {
			err = thread_stack__process(thread, comm, sample, al,
						    &addr_al, es.db_id,
						    dbe->crp);
			if (err)
				goto out_put;
		}
	}

	if (dbe->export_sample)
		err = dbe->export_sample(dbe, &es);

out_put:
	thread__put(main_thread);
	return err;
}

static struct {
	u32 branch_type;
	const char *name;
} branch_types[] = {
	{0, "no branch"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL, "call"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_RETURN, "return"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CONDITIONAL, "conditional jump"},
	{PERF_IP_FLAG_BRANCH, "unconditional jump"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_INTERRUPT,
	 "software interrupt"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_RETURN | PERF_IP_FLAG_INTERRUPT,
	 "return from interrupt"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_SYSCALLRET,
	 "system call"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_RETURN | PERF_IP_FLAG_SYSCALLRET,
	 "return from system call"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_ASYNC, "asynchronous branch"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_ASYNC |
	 PERF_IP_FLAG_INTERRUPT, "hardware interrupt"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_TX_ABORT, "transaction abort"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_TRACE_BEGIN, "trace begin"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_TRACE_END, "trace end"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_VMENTRY, "vm entry"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_VMEXIT, "vm exit"},
	{0, NULL}
};

int db_export__branch_types(struct db_export *dbe)
{
	int i, err = 0;

	for (i = 0; branch_types[i].name ; i++) {
		err = db_export__branch_type(dbe, branch_types[i].branch_type,
					     branch_types[i].name);
		if (err)
			break;
	}

	/* Add trace begin / end variants */
	for (i = 0; branch_types[i].name ; i++) {
		const char *name = branch_types[i].name;
		u32 type = branch_types[i].branch_type;
		char buf[64];

		if (type == PERF_IP_FLAG_BRANCH ||
		    (type & (PERF_IP_FLAG_TRACE_BEGIN | PERF_IP_FLAG_TRACE_END)))
			continue;

		snprintf(buf, sizeof(buf), "trace begin / %s", name);
		err = db_export__branch_type(dbe, type | PERF_IP_FLAG_TRACE_BEGIN, buf);
		if (err)
			break;

		snprintf(buf, sizeof(buf), "%s / trace end", name);
		err = db_export__branch_type(dbe, type | PERF_IP_FLAG_TRACE_END, buf);
		if (err)
			break;
	}

	return err;
}

int db_export__call_path(struct db_export *dbe, struct call_path *cp)
{
	int err;

	if (cp->db_id)
		return 0;

	if (cp->parent) {
		err = db_export__call_path(dbe, cp->parent);
		if (err)
			return err;
	}

	cp->db_id = ++dbe->call_path_last_db_id;

	if (dbe->export_call_path)
		return dbe->export_call_path(dbe, cp);

	return 0;
}

int db_export__call_return(struct db_export *dbe, struct call_return *cr,
			   u64 *parent_db_id)
{
	int err;

	err = db_export__call_path(dbe, cr->cp);
	if (err)
		return err;

	if (!cr->db_id)
		cr->db_id = ++dbe->call_return_last_db_id;

	if (parent_db_id) {
		if (!*parent_db_id)
			*parent_db_id = ++dbe->call_return_last_db_id;
		cr->parent_db_id = *parent_db_id;
	}

	if (dbe->export_call_return)
		return dbe->export_call_return(dbe, cr);

	return 0;
}

static int db_export__pid_tid(struct db_export *dbe, struct machine *machine,
			      pid_t pid, pid_t tid, u64 *db_id,
			      struct comm **comm_ptr, bool *is_idle)
{
	struct thread *thread = machine__find_thread(machine, pid, tid);
	struct thread *main_thread;
	int err = 0;

	if (!thread || !thread->comm_set)
		goto out_put;

	*is_idle = !thread->pid_ && !thread->tid;

	main_thread = thread__main_thread(machine, thread);

	err = db_export__threads(dbe, thread, main_thread, machine, comm_ptr);

	*db_id = thread->db_id;

	thread__put(main_thread);
out_put:
	thread__put(thread);

	return err;
}

int db_export__switch(struct db_export *dbe, union perf_event *event,
		      struct perf_sample *sample, struct machine *machine)
{
	bool out = event->header.misc & PERF_RECORD_MISC_SWITCH_OUT;
	bool out_preempt = out &&
		(event->header.misc & PERF_RECORD_MISC_SWITCH_OUT_PREEMPT);
	int flags = out | (out_preempt << 1);
	bool is_idle_a = false, is_idle_b = false;
	u64 th_a_id = 0, th_b_id = 0;
	u64 comm_out_id, comm_in_id;
	struct comm *comm_a = NULL;
	struct comm *comm_b = NULL;
	u64 th_out_id, th_in_id;
	u64 db_id;
	int err;

	err = db_export__machine(dbe, machine);
	if (err)
		return err;

	err = db_export__pid_tid(dbe, machine, sample->pid, sample->tid,
				 &th_a_id, &comm_a, &is_idle_a);
	if (err)
		return err;

	if (event->header.type == PERF_RECORD_SWITCH_CPU_WIDE) {
		pid_t pid = event->context_switch.next_prev_pid;
		pid_t tid = event->context_switch.next_prev_tid;

		err = db_export__pid_tid(dbe, machine, pid, tid, &th_b_id,
					 &comm_b, &is_idle_b);
		if (err)
			return err;
	}

	/*
	 * Do not export if both threads are unknown (i.e. not being traced),
	 * or one is unknown and the other is the idle task.
	 */
	if ((!th_a_id || is_idle_a) && (!th_b_id || is_idle_b))
		return 0;

	db_id = ++dbe->context_switch_last_db_id;

	if (out) {
		th_out_id   = th_a_id;
		th_in_id    = th_b_id;
		comm_out_id = comm_a ? comm_a->db_id : 0;
		comm_in_id  = comm_b ? comm_b->db_id : 0;
	} else {
		th_out_id   = th_b_id;
		th_in_id    = th_a_id;
		comm_out_id = comm_b ? comm_b->db_id : 0;
		comm_in_id  = comm_a ? comm_a->db_id : 0;
	}

	if (dbe->export_context_switch)
		return dbe->export_context_switch(dbe, db_id, machine, sample,
						  th_out_id, comm_out_id,
						  th_in_id, comm_in_id, flags);
	return 0;
}

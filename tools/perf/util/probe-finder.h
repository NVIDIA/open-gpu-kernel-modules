/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _PROBE_FINDER_H
#define _PROBE_FINDER_H

#include <stdbool.h>
#include "intlist.h"
#include "build-id.h"
#include "probe-event.h"
#include <linux/ctype.h>

#define MAX_PROBE_BUFFER	1024
#define MAX_PROBES		 128
#define MAX_PROBE_ARGS		 128

#define PROBE_ARG_VARS		"$vars"
#define PROBE_ARG_PARAMS	"$params"

static inline int is_c_varname(const char *name)
{
	/* TODO */
	return isalpha(name[0]) || name[0] == '_';
}

#ifdef HAVE_DWARF_SUPPORT

#include "dwarf-aux.h"

/* TODO: export debuginfo data structure even if no dwarf support */

/* debug information structure */
struct debuginfo {
	Dwarf		*dbg;
	Dwfl_Module	*mod;
	Dwfl		*dwfl;
	Dwarf_Addr	bias;
	const unsigned char	*build_id;
};

/* This also tries to open distro debuginfo */
struct debuginfo *debuginfo__new(const char *path);
void debuginfo__delete(struct debuginfo *dbg);

/* Find probe_trace_events specified by perf_probe_event from debuginfo */
int debuginfo__find_trace_events(struct debuginfo *dbg,
				 struct perf_probe_event *pev,
				 struct probe_trace_event **tevs);

/* Find a perf_probe_point from debuginfo */
int debuginfo__find_probe_point(struct debuginfo *dbg, unsigned long addr,
				struct perf_probe_point *ppt);

int debuginfo__get_text_offset(struct debuginfo *dbg, Dwarf_Addr *offs,
			       bool adjust_offset);

/* Find a line range */
int debuginfo__find_line_range(struct debuginfo *dbg, struct line_range *lr);

/* Find available variables */
int debuginfo__find_available_vars_at(struct debuginfo *dbg,
				      struct perf_probe_event *pev,
				      struct variable_list **vls);

/* Find a src file from a DWARF tag path */
int find_source_path(const char *raw_path, const char *sbuild_id,
		     const char *comp_dir, char **new_path);

struct probe_finder {
	struct perf_probe_event	*pev;		/* Target probe event */
	struct debuginfo	*dbg;

	/* Callback when a probe point is found */
	int (*callback)(Dwarf_Die *sc_die, struct probe_finder *pf);

	/* For function searching */
	int			lno;		/* Line number */
	Dwarf_Addr		addr;		/* Address */
	const char		*fname;		/* Real file name */
	Dwarf_Die		cu_die;		/* Current CU */
	Dwarf_Die		sp_die;
	struct intlist		*lcache;	/* Line cache for lazy match */

	/* For variable searching */
#if _ELFUTILS_PREREQ(0, 142)
	/* Call Frame Information from .eh_frame */
	Dwarf_CFI		*cfi_eh;
	/* Call Frame Information from .debug_frame */
	Dwarf_CFI		*cfi_dbg;
#endif
	Dwarf_Op		*fb_ops;	/* Frame base attribute */
	unsigned int		machine;	/* Target machine arch */
	struct perf_probe_arg	*pvar;		/* Current target variable */
	struct probe_trace_arg	*tvar;		/* Current result variable */
	bool			skip_empty_arg;	/* Skip non-exist args */
};

struct trace_event_finder {
	struct probe_finder	pf;
	Dwfl_Module		*mod;		/* For solving symbols */
	struct probe_trace_event *tevs;		/* Found trace events */
	int			ntevs;		/* Number of trace events */
	int			max_tevs;	/* Max number of trace events */
};

struct available_var_finder {
	struct probe_finder	pf;
	Dwfl_Module		*mod;		/* For solving symbols */
	struct variable_list	*vls;		/* Found variable lists */
	int			nvls;		/* Number of variable lists */
	int			max_vls;	/* Max no. of variable lists */
	bool			child;		/* Search child scopes */
};

struct line_finder {
	struct line_range	*lr;		/* Target line range */

	const char		*fname;		/* File name */
	int			lno_s;		/* Start line number */
	int			lno_e;		/* End line number */
	Dwarf_Die		cu_die;		/* Current CU */
	Dwarf_Die		sp_die;
	int			found;
};

#endif /* HAVE_DWARF_SUPPORT */

#endif /*_PROBE_FINDER_H */

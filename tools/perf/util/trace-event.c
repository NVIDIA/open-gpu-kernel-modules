// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <traceevent/event-parse.h>
#include <api/fs/tracing_path.h>
#include <api/fs/fs.h>
#include "trace-event.h"
#include "machine.h"

/*
 * global trace_event object used by trace_event__tp_format
 *
 * TODO There's no cleanup call for this. Add some sort of
 * __exit function support and call trace_event__cleanup
 * there.
 */
static struct trace_event tevent;
static bool tevent_initialized;

int trace_event__init(struct trace_event *t)
{
	struct tep_handle *pevent = tep_alloc();

	if (pevent) {
		t->plugin_list = tep_load_plugins(pevent);
		t->pevent  = pevent;
	}

	return pevent ? 0 : -1;
}

static int trace_event__init2(void)
{
	int be = tep_is_bigendian();
	struct tep_handle *pevent;

	if (trace_event__init(&tevent))
		return -1;

	pevent = tevent.pevent;
	tep_set_flag(pevent, TEP_NSEC_OUTPUT);
	tep_set_file_bigendian(pevent, be);
	tep_set_local_bigendian(pevent, be);
	tevent_initialized = true;
	return 0;
}

int trace_event__register_resolver(struct machine *machine,
				   tep_func_resolver_t *func)
{
	if (!tevent_initialized && trace_event__init2())
		return -1;

	return tep_set_function_resolver(tevent.pevent, func, machine);
}

void trace_event__cleanup(struct trace_event *t)
{
	tep_unload_plugins(t->plugin_list, t->pevent);
	tep_free(t->pevent);
}

/*
 * Returns pointer with encoded error via <linux/err.h> interface.
 */
static struct tep_event*
tp_format(const char *sys, const char *name)
{
	char *tp_dir = get_events_file(sys);
	struct tep_handle *pevent = tevent.pevent;
	struct tep_event *event = NULL;
	char path[PATH_MAX];
	size_t size;
	char *data;
	int err;

	if (!tp_dir)
		return ERR_PTR(-errno);

	scnprintf(path, PATH_MAX, "%s/%s/format", tp_dir, name);
	put_events_file(tp_dir);

	err = filename__read_str(path, &data, &size);
	if (err)
		return ERR_PTR(err);

	tep_parse_format(pevent, &event, data, size, sys);

	free(data);
	return event;
}

/*
 * Returns pointer with encoded error via <linux/err.h> interface.
 */
struct tep_event*
trace_event__tp_format(const char *sys, const char *name)
{
	if (!tevent_initialized && trace_event__init2())
		return ERR_PTR(-ENOMEM);

	return tp_format(sys, name);
}

struct tep_event *trace_event__tp_format_id(int id)
{
	if (!tevent_initialized && trace_event__init2())
		return ERR_PTR(-ENOMEM);

	return tep_find_event(tevent.pevent, id);
}

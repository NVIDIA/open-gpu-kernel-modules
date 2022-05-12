// SPDX-License-Identifier: GPL-2.0
#include <linux/types.h>
#include <linux/string.h>
#include <linux/zalloc.h>

#include "../../../util/event.h"
#include "../../../util/synthetic-events.h"
#include "../../../util/machine.h"
#include "../../../util/tool.h"
#include "../../../util/map.h"
#include "../../../util/debug.h"

#if defined(__x86_64__)

int perf_event__synthesize_extra_kmaps(struct perf_tool *tool,
				       perf_event__handler_t process,
				       struct machine *machine)
{
	int rc = 0;
	struct map *pos;
	struct maps *kmaps = &machine->kmaps;
	union perf_event *event = zalloc(sizeof(event->mmap) +
					 machine->id_hdr_size);

	if (!event) {
		pr_debug("Not enough memory synthesizing mmap event "
			 "for extra kernel maps\n");
		return -1;
	}

	maps__for_each_entry(kmaps, pos) {
		struct kmap *kmap;
		size_t size;

		if (!__map__is_extra_kernel_map(pos))
			continue;

		kmap = map__kmap(pos);

		size = sizeof(event->mmap) - sizeof(event->mmap.filename) +
		       PERF_ALIGN(strlen(kmap->name) + 1, sizeof(u64)) +
		       machine->id_hdr_size;

		memset(event, 0, size);

		event->mmap.header.type = PERF_RECORD_MMAP;

		/*
		 * kernel uses 0 for user space maps, see kernel/perf_event.c
		 * __perf_event_mmap
		 */
		if (machine__is_host(machine))
			event->header.misc = PERF_RECORD_MISC_KERNEL;
		else
			event->header.misc = PERF_RECORD_MISC_GUEST_KERNEL;

		event->mmap.header.size = size;

		event->mmap.start = pos->start;
		event->mmap.len   = pos->end - pos->start;
		event->mmap.pgoff = pos->pgoff;
		event->mmap.pid   = machine->pid;

		strlcpy(event->mmap.filename, kmap->name, PATH_MAX);

		if (perf_tool__process_synth_event(tool, event, machine,
						   process) != 0) {
			rc = -1;
			break;
		}
	}

	free(event);
	return rc;
}

#endif

void arch_perf_parse_sample_weight(struct perf_sample *data,
				   const __u64 *array, u64 type)
{
	union perf_sample_weight weight;

	weight.full = *array;
	if (type & PERF_SAMPLE_WEIGHT)
		data->weight = weight.full;
	else {
		data->weight = weight.var1_dw;
		data->ins_lat = weight.var2_w;
	}
}

void arch_perf_synthesize_sample_weight(const struct perf_sample *data,
					__u64 *array, u64 type)
{
	*array = data->weight;

	if (type & PERF_SAMPLE_WEIGHT_STRUCT) {
		*array &= 0xffffffff;
		*array |= ((u64)data->ins_lat << 32);
	}
}

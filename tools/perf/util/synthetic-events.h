/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_SYNTHETIC_EVENTS_H
#define __PERF_SYNTHETIC_EVENTS_H

#include <stdbool.h>
#include <sys/types.h> // pid_t
#include <linux/compiler.h>
#include <linux/types.h>

struct auxtrace_record;
struct dso;
struct evlist;
struct evsel;
struct machine;
struct perf_counts_values;
struct perf_cpu_map;
struct perf_event_attr;
struct perf_event_mmap_page;
struct perf_sample;
struct perf_session;
struct perf_stat_config;
struct perf_thread_map;
struct perf_tool;
struct record_opts;
struct target;

union perf_event;

typedef int (*perf_event__handler_t)(struct perf_tool *tool, union perf_event *event,
				     struct perf_sample *sample, struct machine *machine);

int perf_event__synthesize_attrs(struct perf_tool *tool, struct evlist *evlist, perf_event__handler_t process);
int perf_event__synthesize_attr(struct perf_tool *tool, struct perf_event_attr *attr, u32 ids, u64 *id, perf_event__handler_t process);
int perf_event__synthesize_build_id(struct perf_tool *tool, struct dso *pos, u16 misc, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_cpu_map(struct perf_tool *tool, struct perf_cpu_map *cpus, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_event_update_cpus(struct perf_tool *tool, struct evsel *evsel, perf_event__handler_t process);
int perf_event__synthesize_event_update_name(struct perf_tool *tool, struct evsel *evsel, perf_event__handler_t process);
int perf_event__synthesize_event_update_scale(struct perf_tool *tool, struct evsel *evsel, perf_event__handler_t process);
int perf_event__synthesize_event_update_unit(struct perf_tool *tool, struct evsel *evsel, perf_event__handler_t process);
int perf_event__synthesize_extra_attr(struct perf_tool *tool, struct evlist *evsel_list, perf_event__handler_t process, bool is_pipe);
int perf_event__synthesize_extra_kmaps(struct perf_tool *tool, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_features(struct perf_tool *tool, struct perf_session *session, struct evlist *evlist, perf_event__handler_t process);
int perf_event__synthesize_id_index(struct perf_tool *tool, perf_event__handler_t process, struct evlist *evlist, struct machine *machine);
int perf_event__synthesize_kernel_mmap(struct perf_tool *tool, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_mmap_events(struct perf_tool *tool, union perf_event *event, pid_t pid, pid_t tgid, perf_event__handler_t process, struct machine *machine, bool mmap_data);
int perf_event__synthesize_modules(struct perf_tool *tool, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_namespaces(struct perf_tool *tool, union perf_event *event, pid_t pid, pid_t tgid, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_cgroups(struct perf_tool *tool, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_sample(union perf_event *event, u64 type, u64 read_format, const struct perf_sample *sample);
int perf_event__synthesize_stat_config(struct perf_tool *tool, struct perf_stat_config *config, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_stat_events(struct perf_stat_config *config, struct perf_tool *tool, struct evlist *evlist, perf_event__handler_t process, bool attrs);
int perf_event__synthesize_stat_round(struct perf_tool *tool, u64 time, u64 type, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_stat(struct perf_tool *tool, u32 cpu, u32 thread, u64 id, struct perf_counts_values *count, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_thread_map2(struct perf_tool *tool, struct perf_thread_map *threads, perf_event__handler_t process, struct machine *machine);
int perf_event__synthesize_thread_map(struct perf_tool *tool, struct perf_thread_map *threads, perf_event__handler_t process, struct machine *machine, bool mmap_data);
int perf_event__synthesize_threads(struct perf_tool *tool, perf_event__handler_t process, struct machine *machine, bool mmap_data, unsigned int nr_threads_synthesize);
int perf_event__synthesize_tracing_data(struct perf_tool *tool, int fd, struct evlist *evlist, perf_event__handler_t process);
int perf_event__synth_time_conv(const struct perf_event_mmap_page *pc, struct perf_tool *tool, perf_event__handler_t process, struct machine *machine);
pid_t perf_event__synthesize_comm(struct perf_tool *tool, union perf_event *event, pid_t pid, perf_event__handler_t process, struct machine *machine);

int perf_tool__process_synth_event(struct perf_tool *tool, union perf_event *event, struct machine *machine, perf_event__handler_t process);

size_t perf_event__sample_event_size(const struct perf_sample *sample, u64 type, u64 read_format);

int __machine__synthesize_threads(struct machine *machine, struct perf_tool *tool,
				  struct target *target, struct perf_thread_map *threads,
				  perf_event__handler_t process, bool data_mmap,
				  unsigned int nr_threads_synthesize);
int machine__synthesize_threads(struct machine *machine, struct target *target,
				struct perf_thread_map *threads, bool data_mmap,
				unsigned int nr_threads_synthesize);

#ifdef HAVE_AUXTRACE_SUPPORT
int perf_event__synthesize_auxtrace_info(struct auxtrace_record *itr, struct perf_tool *tool,
					 struct perf_session *session, perf_event__handler_t process);

#else // HAVE_AUXTRACE_SUPPORT

#include <errno.h>

static inline int
perf_event__synthesize_auxtrace_info(struct auxtrace_record *itr __maybe_unused,
				     struct perf_tool *tool __maybe_unused,
				     struct perf_session *session __maybe_unused,
				     perf_event__handler_t process __maybe_unused)
{
	return -EINVAL;
}
#endif // HAVE_AUXTRACE_SUPPORT

#ifdef HAVE_LIBBPF_SUPPORT
int perf_event__synthesize_bpf_events(struct perf_session *session, perf_event__handler_t process,
				      struct machine *machine, struct record_opts *opts);
#else // HAVE_LIBBPF_SUPPORT
static inline int perf_event__synthesize_bpf_events(struct perf_session *session __maybe_unused,
						    perf_event__handler_t process __maybe_unused,
						    struct machine *machine __maybe_unused,
						    struct record_opts *opts __maybe_unused)
{
	return 0;
}
#endif // HAVE_LIBBPF_SUPPORT

#endif // __PERF_SYNTHETIC_EVENTS_H

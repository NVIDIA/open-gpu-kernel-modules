// SPDX-License-Identifier: GPL-2.0
#include "builtin.h"

#include "util/counts.h"
#include "util/debug.h"
#include "util/dso.h"
#include <subcmd/exec-cmd.h>
#include "util/header.h"
#include <subcmd/parse-options.h>
#include "util/perf_regs.h"
#include "util/session.h"
#include "util/tool.h"
#include "util/map.h"
#include "util/srcline.h"
#include "util/symbol.h"
#include "util/thread.h"
#include "util/trace-event.h"
#include "util/evlist.h"
#include "util/evsel.h"
#include "util/evsel_fprintf.h"
#include "util/evswitch.h"
#include "util/sort.h"
#include "util/data.h"
#include "util/auxtrace.h"
#include "util/cpumap.h"
#include "util/thread_map.h"
#include "util/stat.h"
#include "util/color.h"
#include "util/string2.h"
#include "util/thread-stack.h"
#include "util/time-utils.h"
#include "util/path.h"
#include "util/event.h"
#include "ui/ui.h"
#include "print_binary.h"
#include "archinsn.h"
#include <linux/bitmap.h>
#include <linux/kernel.h>
#include <linux/stringify.h>
#include <linux/time64.h>
#include <linux/zalloc.h>
#include <sys/utsname.h>
#include "asm/bug.h"
#include "util/mem-events.h"
#include "util/dump-insn.h"
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <subcmd/pager.h>
#include <perf/evlist.h>
#include <linux/err.h>
#include "util/record.h"
#include "util/util.h"
#include "perf.h"

#include <linux/ctype.h>

static char const		*script_name;
static char const		*generate_script_lang;
static bool			reltime;
static bool			deltatime;
static u64			initial_time;
static u64			previous_time;
static bool			debug_mode;
static u64			last_timestamp;
static u64			nr_unordered;
static bool			no_callchain;
static bool			latency_format;
static bool			system_wide;
static bool			print_flags;
static const char		*cpu_list;
static DECLARE_BITMAP(cpu_bitmap, MAX_NR_CPUS);
static struct perf_stat_config	stat_config;
static int			max_blocks;
static bool			native_arch;

unsigned int scripting_max_stack = PERF_MAX_STACK_DEPTH;

enum perf_output_field {
	PERF_OUTPUT_COMM            = 1ULL << 0,
	PERF_OUTPUT_TID             = 1ULL << 1,
	PERF_OUTPUT_PID             = 1ULL << 2,
	PERF_OUTPUT_TIME            = 1ULL << 3,
	PERF_OUTPUT_CPU             = 1ULL << 4,
	PERF_OUTPUT_EVNAME          = 1ULL << 5,
	PERF_OUTPUT_TRACE           = 1ULL << 6,
	PERF_OUTPUT_IP              = 1ULL << 7,
	PERF_OUTPUT_SYM             = 1ULL << 8,
	PERF_OUTPUT_DSO             = 1ULL << 9,
	PERF_OUTPUT_ADDR            = 1ULL << 10,
	PERF_OUTPUT_SYMOFFSET       = 1ULL << 11,
	PERF_OUTPUT_SRCLINE         = 1ULL << 12,
	PERF_OUTPUT_PERIOD          = 1ULL << 13,
	PERF_OUTPUT_IREGS	    = 1ULL << 14,
	PERF_OUTPUT_BRSTACK	    = 1ULL << 15,
	PERF_OUTPUT_BRSTACKSYM	    = 1ULL << 16,
	PERF_OUTPUT_DATA_SRC	    = 1ULL << 17,
	PERF_OUTPUT_WEIGHT	    = 1ULL << 18,
	PERF_OUTPUT_BPF_OUTPUT	    = 1ULL << 19,
	PERF_OUTPUT_CALLINDENT	    = 1ULL << 20,
	PERF_OUTPUT_INSN	    = 1ULL << 21,
	PERF_OUTPUT_INSNLEN	    = 1ULL << 22,
	PERF_OUTPUT_BRSTACKINSN	    = 1ULL << 23,
	PERF_OUTPUT_BRSTACKOFF	    = 1ULL << 24,
	PERF_OUTPUT_SYNTH           = 1ULL << 25,
	PERF_OUTPUT_PHYS_ADDR       = 1ULL << 26,
	PERF_OUTPUT_UREGS	    = 1ULL << 27,
	PERF_OUTPUT_METRIC	    = 1ULL << 28,
	PERF_OUTPUT_MISC            = 1ULL << 29,
	PERF_OUTPUT_SRCCODE	    = 1ULL << 30,
	PERF_OUTPUT_IPC             = 1ULL << 31,
	PERF_OUTPUT_TOD             = 1ULL << 32,
	PERF_OUTPUT_DATA_PAGE_SIZE  = 1ULL << 33,
	PERF_OUTPUT_CODE_PAGE_SIZE  = 1ULL << 34,
};

struct perf_script {
	struct perf_tool	tool;
	struct perf_session	*session;
	bool			show_task_events;
	bool			show_mmap_events;
	bool			show_switch_events;
	bool			show_namespace_events;
	bool			show_lost_events;
	bool			show_round_events;
	bool			show_bpf_events;
	bool			show_cgroup_events;
	bool			show_text_poke_events;
	bool			allocated;
	bool			per_event_dump;
	bool			stitch_lbr;
	struct evswitch		evswitch;
	struct perf_cpu_map	*cpus;
	struct perf_thread_map *threads;
	int			name_width;
	const char              *time_str;
	struct perf_time_interval *ptime_range;
	int			range_size;
	int			range_num;
};

struct output_option {
	const char *str;
	enum perf_output_field field;
} all_output_options[] = {
	{.str = "comm",  .field = PERF_OUTPUT_COMM},
	{.str = "tid",   .field = PERF_OUTPUT_TID},
	{.str = "pid",   .field = PERF_OUTPUT_PID},
	{.str = "time",  .field = PERF_OUTPUT_TIME},
	{.str = "cpu",   .field = PERF_OUTPUT_CPU},
	{.str = "event", .field = PERF_OUTPUT_EVNAME},
	{.str = "trace", .field = PERF_OUTPUT_TRACE},
	{.str = "ip",    .field = PERF_OUTPUT_IP},
	{.str = "sym",   .field = PERF_OUTPUT_SYM},
	{.str = "dso",   .field = PERF_OUTPUT_DSO},
	{.str = "addr",  .field = PERF_OUTPUT_ADDR},
	{.str = "symoff", .field = PERF_OUTPUT_SYMOFFSET},
	{.str = "srcline", .field = PERF_OUTPUT_SRCLINE},
	{.str = "period", .field = PERF_OUTPUT_PERIOD},
	{.str = "iregs", .field = PERF_OUTPUT_IREGS},
	{.str = "uregs", .field = PERF_OUTPUT_UREGS},
	{.str = "brstack", .field = PERF_OUTPUT_BRSTACK},
	{.str = "brstacksym", .field = PERF_OUTPUT_BRSTACKSYM},
	{.str = "data_src", .field = PERF_OUTPUT_DATA_SRC},
	{.str = "weight",   .field = PERF_OUTPUT_WEIGHT},
	{.str = "bpf-output",   .field = PERF_OUTPUT_BPF_OUTPUT},
	{.str = "callindent", .field = PERF_OUTPUT_CALLINDENT},
	{.str = "insn", .field = PERF_OUTPUT_INSN},
	{.str = "insnlen", .field = PERF_OUTPUT_INSNLEN},
	{.str = "brstackinsn", .field = PERF_OUTPUT_BRSTACKINSN},
	{.str = "brstackoff", .field = PERF_OUTPUT_BRSTACKOFF},
	{.str = "synth", .field = PERF_OUTPUT_SYNTH},
	{.str = "phys_addr", .field = PERF_OUTPUT_PHYS_ADDR},
	{.str = "metric", .field = PERF_OUTPUT_METRIC},
	{.str = "misc", .field = PERF_OUTPUT_MISC},
	{.str = "srccode", .field = PERF_OUTPUT_SRCCODE},
	{.str = "ipc", .field = PERF_OUTPUT_IPC},
	{.str = "tod", .field = PERF_OUTPUT_TOD},
	{.str = "data_page_size", .field = PERF_OUTPUT_DATA_PAGE_SIZE},
	{.str = "code_page_size", .field = PERF_OUTPUT_CODE_PAGE_SIZE},
};

enum {
	OUTPUT_TYPE_SYNTH = PERF_TYPE_MAX,
	OUTPUT_TYPE_OTHER,
	OUTPUT_TYPE_MAX
};

/* default set to maintain compatibility with current format */
static struct {
	bool user_set;
	bool wildcard_set;
	unsigned int print_ip_opts;
	u64 fields;
	u64 invalid_fields;
	u64 user_set_fields;
	u64 user_unset_fields;
} output[OUTPUT_TYPE_MAX] = {

	[PERF_TYPE_HARDWARE] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_PERIOD,

		.invalid_fields = PERF_OUTPUT_TRACE | PERF_OUTPUT_BPF_OUTPUT,
	},

	[PERF_TYPE_SOFTWARE] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_PERIOD |
			      PERF_OUTPUT_BPF_OUTPUT,

		.invalid_fields = PERF_OUTPUT_TRACE,
	},

	[PERF_TYPE_TRACEPOINT] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
				  PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
				  PERF_OUTPUT_EVNAME | PERF_OUTPUT_TRACE
	},

	[PERF_TYPE_HW_CACHE] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_PERIOD,

		.invalid_fields = PERF_OUTPUT_TRACE | PERF_OUTPUT_BPF_OUTPUT,
	},

	[PERF_TYPE_RAW] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_PERIOD |
			      PERF_OUTPUT_ADDR | PERF_OUTPUT_DATA_SRC |
			      PERF_OUTPUT_WEIGHT | PERF_OUTPUT_PHYS_ADDR |
			      PERF_OUTPUT_DATA_PAGE_SIZE | PERF_OUTPUT_CODE_PAGE_SIZE,

		.invalid_fields = PERF_OUTPUT_TRACE | PERF_OUTPUT_BPF_OUTPUT,
	},

	[PERF_TYPE_BREAKPOINT] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_PERIOD,

		.invalid_fields = PERF_OUTPUT_TRACE | PERF_OUTPUT_BPF_OUTPUT,
	},

	[OUTPUT_TYPE_SYNTH] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_SYNTH,

		.invalid_fields = PERF_OUTPUT_TRACE | PERF_OUTPUT_BPF_OUTPUT,
	},

	[OUTPUT_TYPE_OTHER] = {
		.user_set = false,

		.fields = PERF_OUTPUT_COMM | PERF_OUTPUT_TID |
			      PERF_OUTPUT_CPU | PERF_OUTPUT_TIME |
			      PERF_OUTPUT_EVNAME | PERF_OUTPUT_IP |
			      PERF_OUTPUT_SYM | PERF_OUTPUT_SYMOFFSET |
			      PERF_OUTPUT_DSO | PERF_OUTPUT_PERIOD,

		.invalid_fields = PERF_OUTPUT_TRACE | PERF_OUTPUT_BPF_OUTPUT,
	},
};

struct evsel_script {
       char *filename;
       FILE *fp;
       u64  samples;
       /* For metric output */
       u64  val;
       int  gnum;
};

static inline struct evsel_script *evsel_script(struct evsel *evsel)
{
	return (struct evsel_script *)evsel->priv;
}

static struct evsel_script *evsel_script__new(struct evsel *evsel, struct perf_data *data)
{
	struct evsel_script *es = zalloc(sizeof(*es));

	if (es != NULL) {
		if (asprintf(&es->filename, "%s.%s.dump", data->file.path, evsel__name(evsel)) < 0)
			goto out_free;
		es->fp = fopen(es->filename, "w");
		if (es->fp == NULL)
			goto out_free_filename;
	}

	return es;
out_free_filename:
	zfree(&es->filename);
out_free:
	free(es);
	return NULL;
}

static void evsel_script__delete(struct evsel_script *es)
{
	zfree(&es->filename);
	fclose(es->fp);
	es->fp = NULL;
	free(es);
}

static int evsel_script__fprintf(struct evsel_script *es, FILE *fp)
{
	struct stat st;

	fstat(fileno(es->fp), &st);
	return fprintf(fp, "[ perf script: Wrote %.3f MB %s (%" PRIu64 " samples) ]\n",
		       st.st_size / 1024.0 / 1024.0, es->filename, es->samples);
}

static inline int output_type(unsigned int type)
{
	switch (type) {
	case PERF_TYPE_SYNTH:
		return OUTPUT_TYPE_SYNTH;
	default:
		if (type < PERF_TYPE_MAX)
			return type;
	}

	return OUTPUT_TYPE_OTHER;
}

static inline unsigned int attr_type(unsigned int type)
{
	switch (type) {
	case OUTPUT_TYPE_SYNTH:
		return PERF_TYPE_SYNTH;
	default:
		return type;
	}
}

static bool output_set_by_user(void)
{
	int j;
	for (j = 0; j < OUTPUT_TYPE_MAX; ++j) {
		if (output[j].user_set)
			return true;
	}
	return false;
}

static const char *output_field2str(enum perf_output_field field)
{
	int i, imax = ARRAY_SIZE(all_output_options);
	const char *str = "";

	for (i = 0; i < imax; ++i) {
		if (all_output_options[i].field == field) {
			str = all_output_options[i].str;
			break;
		}
	}
	return str;
}

#define PRINT_FIELD(x)  (output[output_type(attr->type)].fields & PERF_OUTPUT_##x)

static int evsel__do_check_stype(struct evsel *evsel, u64 sample_type, const char *sample_msg,
				 enum perf_output_field field, bool allow_user_set)
{
	struct perf_event_attr *attr = &evsel->core.attr;
	int type = output_type(attr->type);
	const char *evname;

	if (attr->sample_type & sample_type)
		return 0;

	if (output[type].user_set_fields & field) {
		if (allow_user_set)
			return 0;
		evname = evsel__name(evsel);
		pr_err("Samples for '%s' event do not have %s attribute set. "
		       "Cannot print '%s' field.\n",
		       evname, sample_msg, output_field2str(field));
		return -1;
	}

	/* user did not ask for it explicitly so remove from the default list */
	output[type].fields &= ~field;
	evname = evsel__name(evsel);
	pr_debug("Samples for '%s' event do not have %s attribute set. "
		 "Skipping '%s' field.\n",
		 evname, sample_msg, output_field2str(field));

	return 0;
}

static int evsel__check_stype(struct evsel *evsel, u64 sample_type, const char *sample_msg,
			      enum perf_output_field field)
{
	return evsel__do_check_stype(evsel, sample_type, sample_msg, field, false);
}

static int evsel__check_attr(struct evsel *evsel, struct perf_session *session)
{
	struct perf_event_attr *attr = &evsel->core.attr;
	bool allow_user_set;

	if (perf_header__has_feat(&session->header, HEADER_STAT))
		return 0;

	allow_user_set = perf_header__has_feat(&session->header,
					       HEADER_AUXTRACE);

	if (PRINT_FIELD(TRACE) &&
	    !perf_session__has_traces(session, "record -R"))
		return -EINVAL;

	if (PRINT_FIELD(IP)) {
		if (evsel__check_stype(evsel, PERF_SAMPLE_IP, "IP", PERF_OUTPUT_IP))
			return -EINVAL;
	}

	if (PRINT_FIELD(ADDR) &&
	    evsel__do_check_stype(evsel, PERF_SAMPLE_ADDR, "ADDR", PERF_OUTPUT_ADDR, allow_user_set))
		return -EINVAL;

	if (PRINT_FIELD(DATA_SRC) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_DATA_SRC, "DATA_SRC", PERF_OUTPUT_DATA_SRC))
		return -EINVAL;

	if (PRINT_FIELD(WEIGHT) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_WEIGHT, "WEIGHT", PERF_OUTPUT_WEIGHT))
		return -EINVAL;

	if (PRINT_FIELD(SYM) &&
	    !(evsel->core.attr.sample_type & (PERF_SAMPLE_IP|PERF_SAMPLE_ADDR))) {
		pr_err("Display of symbols requested but neither sample IP nor "
			   "sample address\navailable. Hence, no addresses to convert "
		       "to symbols.\n");
		return -EINVAL;
	}
	if (PRINT_FIELD(SYMOFFSET) && !PRINT_FIELD(SYM)) {
		pr_err("Display of offsets requested but symbol is not"
		       "selected.\n");
		return -EINVAL;
	}
	if (PRINT_FIELD(DSO) &&
	    !(evsel->core.attr.sample_type & (PERF_SAMPLE_IP|PERF_SAMPLE_ADDR))) {
		pr_err("Display of DSO requested but no address to convert.\n");
		return -EINVAL;
	}
	if ((PRINT_FIELD(SRCLINE) || PRINT_FIELD(SRCCODE)) && !PRINT_FIELD(IP)) {
		pr_err("Display of source line number requested but sample IP is not\n"
		       "selected. Hence, no address to lookup the source line number.\n");
		return -EINVAL;
	}
	if (PRINT_FIELD(BRSTACKINSN) && !allow_user_set &&
	    !(evlist__combined_branch_type(session->evlist) & PERF_SAMPLE_BRANCH_ANY)) {
		pr_err("Display of branch stack assembler requested, but non all-branch filter set\n"
		       "Hint: run 'perf record -b ...'\n");
		return -EINVAL;
	}
	if ((PRINT_FIELD(PID) || PRINT_FIELD(TID)) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_TID, "TID", PERF_OUTPUT_TID|PERF_OUTPUT_PID))
		return -EINVAL;

	if (PRINT_FIELD(TIME) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_TIME, "TIME", PERF_OUTPUT_TIME))
		return -EINVAL;

	if (PRINT_FIELD(CPU) &&
	    evsel__do_check_stype(evsel, PERF_SAMPLE_CPU, "CPU", PERF_OUTPUT_CPU, allow_user_set))
		return -EINVAL;

	if (PRINT_FIELD(IREGS) &&
	    evsel__do_check_stype(evsel, PERF_SAMPLE_REGS_INTR, "IREGS", PERF_OUTPUT_IREGS, allow_user_set))
		return -EINVAL;

	if (PRINT_FIELD(UREGS) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_REGS_USER, "UREGS", PERF_OUTPUT_UREGS))
		return -EINVAL;

	if (PRINT_FIELD(PHYS_ADDR) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_PHYS_ADDR, "PHYS_ADDR", PERF_OUTPUT_PHYS_ADDR))
		return -EINVAL;

	if (PRINT_FIELD(DATA_PAGE_SIZE) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_DATA_PAGE_SIZE, "DATA_PAGE_SIZE", PERF_OUTPUT_DATA_PAGE_SIZE))
		return -EINVAL;

	if (PRINT_FIELD(CODE_PAGE_SIZE) &&
	    evsel__check_stype(evsel, PERF_SAMPLE_CODE_PAGE_SIZE, "CODE_PAGE_SIZE", PERF_OUTPUT_CODE_PAGE_SIZE))
		return -EINVAL;

	return 0;
}

static void set_print_ip_opts(struct perf_event_attr *attr)
{
	unsigned int type = output_type(attr->type);

	output[type].print_ip_opts = 0;
	if (PRINT_FIELD(IP))
		output[type].print_ip_opts |= EVSEL__PRINT_IP;

	if (PRINT_FIELD(SYM))
		output[type].print_ip_opts |= EVSEL__PRINT_SYM;

	if (PRINT_FIELD(DSO))
		output[type].print_ip_opts |= EVSEL__PRINT_DSO;

	if (PRINT_FIELD(SYMOFFSET))
		output[type].print_ip_opts |= EVSEL__PRINT_SYMOFFSET;

	if (PRINT_FIELD(SRCLINE))
		output[type].print_ip_opts |= EVSEL__PRINT_SRCLINE;
}

/*
 * verify all user requested events exist and the samples
 * have the expected data
 */
static int perf_session__check_output_opt(struct perf_session *session)
{
	bool tod = false;
	unsigned int j;
	struct evsel *evsel;

	for (j = 0; j < OUTPUT_TYPE_MAX; ++j) {
		evsel = perf_session__find_first_evtype(session, attr_type(j));

		/*
		 * even if fields is set to 0 (ie., show nothing) event must
		 * exist if user explicitly includes it on the command line
		 */
		if (!evsel && output[j].user_set && !output[j].wildcard_set &&
		    j != OUTPUT_TYPE_SYNTH) {
			pr_err("%s events do not exist. "
			       "Remove corresponding -F option to proceed.\n",
			       event_type(j));
			return -1;
		}

		if (evsel && output[j].fields &&
			evsel__check_attr(evsel, session))
			return -1;

		if (evsel == NULL)
			continue;

		set_print_ip_opts(&evsel->core.attr);
		tod |= output[j].fields & PERF_OUTPUT_TOD;
	}

	if (!no_callchain) {
		bool use_callchain = false;
		bool not_pipe = false;

		evlist__for_each_entry(session->evlist, evsel) {
			not_pipe = true;
			if (evsel__has_callchain(evsel)) {
				use_callchain = true;
				break;
			}
		}
		if (not_pipe && !use_callchain)
			symbol_conf.use_callchain = false;
	}

	/*
	 * set default for tracepoints to print symbols only
	 * if callchains are present
	 */
	if (symbol_conf.use_callchain &&
	    !output[PERF_TYPE_TRACEPOINT].user_set) {
		j = PERF_TYPE_TRACEPOINT;

		evlist__for_each_entry(session->evlist, evsel) {
			if (evsel->core.attr.type != j)
				continue;

			if (evsel__has_callchain(evsel)) {
				output[j].fields |= PERF_OUTPUT_IP;
				output[j].fields |= PERF_OUTPUT_SYM;
				output[j].fields |= PERF_OUTPUT_SYMOFFSET;
				output[j].fields |= PERF_OUTPUT_DSO;
				set_print_ip_opts(&evsel->core.attr);
				goto out;
			}
		}
	}

	if (tod && !session->header.env.clock.enabled) {
		pr_err("Can't provide 'tod' time, missing clock data. "
		       "Please record with -k/--clockid option.\n");
		return -1;
	}
out:
	return 0;
}

static int perf_sample__fprintf_regs(struct regs_dump *regs, uint64_t mask,
				     FILE *fp)
{
	unsigned i = 0, r;
	int printed = 0;

	if (!regs || !regs->regs)
		return 0;

	printed += fprintf(fp, " ABI:%" PRIu64 " ", regs->abi);

	for_each_set_bit(r, (unsigned long *) &mask, sizeof(mask) * 8) {
		u64 val = regs->regs[i++];
		printed += fprintf(fp, "%5s:0x%"PRIx64" ", perf_reg_name(r), val);
	}

	return printed;
}

#define DEFAULT_TOD_FMT "%F %H:%M:%S"

static char*
tod_scnprintf(struct perf_script *script, char *buf, int buflen,
	     u64 timestamp)
{
	u64 tod_ns, clockid_ns;
	struct perf_env *env;
	unsigned long nsec;
	struct tm ltime;
	char date[64];
	time_t sec;

	buf[0] = '\0';
	if (buflen < 64 || !script)
		return buf;

	env = &script->session->header.env;
	if (!env->clock.enabled) {
		scnprintf(buf, buflen, "disabled");
		return buf;
	}

	clockid_ns = env->clock.clockid_ns;
	tod_ns     = env->clock.tod_ns;

	if (timestamp > clockid_ns)
		tod_ns += timestamp - clockid_ns;
	else
		tod_ns -= clockid_ns - timestamp;

	sec  = (time_t) (tod_ns / NSEC_PER_SEC);
	nsec = tod_ns - sec * NSEC_PER_SEC;

	if (localtime_r(&sec, &ltime) == NULL) {
		scnprintf(buf, buflen, "failed");
	} else {
		strftime(date, sizeof(date), DEFAULT_TOD_FMT, &ltime);

		if (symbol_conf.nanosecs) {
			snprintf(buf, buflen, "%s.%09lu", date, nsec);
		} else {
			snprintf(buf, buflen, "%s.%06lu",
				 date, nsec / NSEC_PER_USEC);
		}
	}

	return buf;
}

static int perf_sample__fprintf_iregs(struct perf_sample *sample,
				      struct perf_event_attr *attr, FILE *fp)
{
	return perf_sample__fprintf_regs(&sample->intr_regs,
					 attr->sample_regs_intr, fp);
}

static int perf_sample__fprintf_uregs(struct perf_sample *sample,
				      struct perf_event_attr *attr, FILE *fp)
{
	return perf_sample__fprintf_regs(&sample->user_regs,
					 attr->sample_regs_user, fp);
}

static int perf_sample__fprintf_start(struct perf_script *script,
				      struct perf_sample *sample,
				      struct thread *thread,
				      struct evsel *evsel,
				      u32 type, FILE *fp)
{
	struct perf_event_attr *attr = &evsel->core.attr;
	unsigned long secs;
	unsigned long long nsecs;
	int printed = 0;
	char tstr[128];

	if (PRINT_FIELD(COMM)) {
		const char *comm = thread ? thread__comm_str(thread) : ":-1";

		if (latency_format)
			printed += fprintf(fp, "%8.8s ", comm);
		else if (PRINT_FIELD(IP) && evsel__has_callchain(evsel) && symbol_conf.use_callchain)
			printed += fprintf(fp, "%s ", comm);
		else
			printed += fprintf(fp, "%16s ", comm);
	}

	if (PRINT_FIELD(PID) && PRINT_FIELD(TID))
		printed += fprintf(fp, "%5d/%-5d ", sample->pid, sample->tid);
	else if (PRINT_FIELD(PID))
		printed += fprintf(fp, "%5d ", sample->pid);
	else if (PRINT_FIELD(TID))
		printed += fprintf(fp, "%5d ", sample->tid);

	if (PRINT_FIELD(CPU)) {
		if (latency_format)
			printed += fprintf(fp, "%3d ", sample->cpu);
		else
			printed += fprintf(fp, "[%03d] ", sample->cpu);
	}

	if (PRINT_FIELD(MISC)) {
		int ret = 0;

		#define has(m) \
			(sample->misc & PERF_RECORD_MISC_##m) == PERF_RECORD_MISC_##m

		if (has(KERNEL))
			ret += fprintf(fp, "K");
		if (has(USER))
			ret += fprintf(fp, "U");
		if (has(HYPERVISOR))
			ret += fprintf(fp, "H");
		if (has(GUEST_KERNEL))
			ret += fprintf(fp, "G");
		if (has(GUEST_USER))
			ret += fprintf(fp, "g");

		switch (type) {
		case PERF_RECORD_MMAP:
		case PERF_RECORD_MMAP2:
			if (has(MMAP_DATA))
				ret += fprintf(fp, "M");
			break;
		case PERF_RECORD_COMM:
			if (has(COMM_EXEC))
				ret += fprintf(fp, "E");
			break;
		case PERF_RECORD_SWITCH:
		case PERF_RECORD_SWITCH_CPU_WIDE:
			if (has(SWITCH_OUT)) {
				ret += fprintf(fp, "S");
				if (sample->misc & PERF_RECORD_MISC_SWITCH_OUT_PREEMPT)
					ret += fprintf(fp, "p");
			}
		default:
			break;
		}

		#undef has

		ret += fprintf(fp, "%*s", 6 - ret, " ");
		printed += ret;
	}

	if (PRINT_FIELD(TOD)) {
		tod_scnprintf(script, tstr, sizeof(tstr), sample->time);
		printed += fprintf(fp, "%s ", tstr);
	}

	if (PRINT_FIELD(TIME)) {
		u64 t = sample->time;
		if (reltime) {
			if (!initial_time)
				initial_time = sample->time;
			t = sample->time - initial_time;
		} else if (deltatime) {
			if (previous_time)
				t = sample->time - previous_time;
			else {
				t = 0;
			}
			previous_time = sample->time;
		}
		nsecs = t;
		secs = nsecs / NSEC_PER_SEC;
		nsecs -= secs * NSEC_PER_SEC;

		if (symbol_conf.nanosecs)
			printed += fprintf(fp, "%5lu.%09llu: ", secs, nsecs);
		else {
			char sample_time[32];
			timestamp__scnprintf_usec(t, sample_time, sizeof(sample_time));
			printed += fprintf(fp, "%12s: ", sample_time);
		}
	}

	return printed;
}

static inline char
mispred_str(struct branch_entry *br)
{
	if (!(br->flags.mispred  || br->flags.predicted))
		return '-';

	return br->flags.predicted ? 'P' : 'M';
}

static int perf_sample__fprintf_brstack(struct perf_sample *sample,
					struct thread *thread,
					struct perf_event_attr *attr, FILE *fp)
{
	struct branch_stack *br = sample->branch_stack;
	struct branch_entry *entries = perf_sample__branch_entries(sample);
	struct addr_location alf, alt;
	u64 i, from, to;
	int printed = 0;

	if (!(br && br->nr))
		return 0;

	for (i = 0; i < br->nr; i++) {
		from = entries[i].from;
		to   = entries[i].to;

		if (PRINT_FIELD(DSO)) {
			memset(&alf, 0, sizeof(alf));
			memset(&alt, 0, sizeof(alt));
			thread__find_map_fb(thread, sample->cpumode, from, &alf);
			thread__find_map_fb(thread, sample->cpumode, to, &alt);
		}

		printed += fprintf(fp, " 0x%"PRIx64, from);
		if (PRINT_FIELD(DSO)) {
			printed += fprintf(fp, "(");
			printed += map__fprintf_dsoname(alf.map, fp);
			printed += fprintf(fp, ")");
		}

		printed += fprintf(fp, "/0x%"PRIx64, to);
		if (PRINT_FIELD(DSO)) {
			printed += fprintf(fp, "(");
			printed += map__fprintf_dsoname(alt.map, fp);
			printed += fprintf(fp, ")");
		}

		printed += fprintf(fp, "/%c/%c/%c/%d ",
			mispred_str(entries + i),
			entries[i].flags.in_tx ? 'X' : '-',
			entries[i].flags.abort ? 'A' : '-',
			entries[i].flags.cycles);
	}

	return printed;
}

static int perf_sample__fprintf_brstacksym(struct perf_sample *sample,
					   struct thread *thread,
					   struct perf_event_attr *attr, FILE *fp)
{
	struct branch_stack *br = sample->branch_stack;
	struct branch_entry *entries = perf_sample__branch_entries(sample);
	struct addr_location alf, alt;
	u64 i, from, to;
	int printed = 0;

	if (!(br && br->nr))
		return 0;

	for (i = 0; i < br->nr; i++) {

		memset(&alf, 0, sizeof(alf));
		memset(&alt, 0, sizeof(alt));
		from = entries[i].from;
		to   = entries[i].to;

		thread__find_symbol_fb(thread, sample->cpumode, from, &alf);
		thread__find_symbol_fb(thread, sample->cpumode, to, &alt);

		printed += symbol__fprintf_symname_offs(alf.sym, &alf, fp);
		if (PRINT_FIELD(DSO)) {
			printed += fprintf(fp, "(");
			printed += map__fprintf_dsoname(alf.map, fp);
			printed += fprintf(fp, ")");
		}
		printed += fprintf(fp, "%c", '/');
		printed += symbol__fprintf_symname_offs(alt.sym, &alt, fp);
		if (PRINT_FIELD(DSO)) {
			printed += fprintf(fp, "(");
			printed += map__fprintf_dsoname(alt.map, fp);
			printed += fprintf(fp, ")");
		}
		printed += fprintf(fp, "/%c/%c/%c/%d ",
			mispred_str(entries + i),
			entries[i].flags.in_tx ? 'X' : '-',
			entries[i].flags.abort ? 'A' : '-',
			entries[i].flags.cycles);
	}

	return printed;
}

static int perf_sample__fprintf_brstackoff(struct perf_sample *sample,
					   struct thread *thread,
					   struct perf_event_attr *attr, FILE *fp)
{
	struct branch_stack *br = sample->branch_stack;
	struct branch_entry *entries = perf_sample__branch_entries(sample);
	struct addr_location alf, alt;
	u64 i, from, to;
	int printed = 0;

	if (!(br && br->nr))
		return 0;

	for (i = 0; i < br->nr; i++) {

		memset(&alf, 0, sizeof(alf));
		memset(&alt, 0, sizeof(alt));
		from = entries[i].from;
		to   = entries[i].to;

		if (thread__find_map_fb(thread, sample->cpumode, from, &alf) &&
		    !alf.map->dso->adjust_symbols)
			from = map__map_ip(alf.map, from);

		if (thread__find_map_fb(thread, sample->cpumode, to, &alt) &&
		    !alt.map->dso->adjust_symbols)
			to = map__map_ip(alt.map, to);

		printed += fprintf(fp, " 0x%"PRIx64, from);
		if (PRINT_FIELD(DSO)) {
			printed += fprintf(fp, "(");
			printed += map__fprintf_dsoname(alf.map, fp);
			printed += fprintf(fp, ")");
		}
		printed += fprintf(fp, "/0x%"PRIx64, to);
		if (PRINT_FIELD(DSO)) {
			printed += fprintf(fp, "(");
			printed += map__fprintf_dsoname(alt.map, fp);
			printed += fprintf(fp, ")");
		}
		printed += fprintf(fp, "/%c/%c/%c/%d ",
			mispred_str(entries + i),
			entries[i].flags.in_tx ? 'X' : '-',
			entries[i].flags.abort ? 'A' : '-',
			entries[i].flags.cycles);
	}

	return printed;
}
#define MAXBB 16384UL

static int grab_bb(u8 *buffer, u64 start, u64 end,
		    struct machine *machine, struct thread *thread,
		    bool *is64bit, u8 *cpumode, bool last)
{
	long offset, len;
	struct addr_location al;
	bool kernel;

	if (!start || !end)
		return 0;

	kernel = machine__kernel_ip(machine, start);
	if (kernel)
		*cpumode = PERF_RECORD_MISC_KERNEL;
	else
		*cpumode = PERF_RECORD_MISC_USER;

	/*
	 * Block overlaps between kernel and user.
	 * This can happen due to ring filtering
	 * On Intel CPUs the entry into the kernel is filtered,
	 * but the exit is not. Let the caller patch it up.
	 */
	if (kernel != machine__kernel_ip(machine, end)) {
		pr_debug("\tblock %" PRIx64 "-%" PRIx64 " transfers between kernel and user\n", start, end);
		return -ENXIO;
	}

	memset(&al, 0, sizeof(al));
	if (end - start > MAXBB - MAXINSN) {
		if (last)
			pr_debug("\tbrstack does not reach to final jump (%" PRIx64 "-%" PRIx64 ")\n", start, end);
		else
			pr_debug("\tblock %" PRIx64 "-%" PRIx64 " (%" PRIu64 ") too long to dump\n", start, end, end - start);
		return 0;
	}

	if (!thread__find_map(thread, *cpumode, start, &al) || !al.map->dso) {
		pr_debug("\tcannot resolve %" PRIx64 "-%" PRIx64 "\n", start, end);
		return 0;
	}
	if (al.map->dso->data.status == DSO_DATA_STATUS_ERROR) {
		pr_debug("\tcannot resolve %" PRIx64 "-%" PRIx64 "\n", start, end);
		return 0;
	}

	/* Load maps to ensure dso->is_64_bit has been updated */
	map__load(al.map);

	offset = al.map->map_ip(al.map, start);
	len = dso__data_read_offset(al.map->dso, machine, offset, (u8 *)buffer,
				    end - start + MAXINSN);

	*is64bit = al.map->dso->is_64_bit;
	if (len <= 0)
		pr_debug("\tcannot fetch code for block at %" PRIx64 "-%" PRIx64 "\n",
			start, end);
	return len;
}

static int map__fprintf_srccode(struct map *map, u64 addr, FILE *fp, struct srccode_state *state)
{
	char *srcfile;
	int ret = 0;
	unsigned line;
	int len;
	char *srccode;

	if (!map || !map->dso)
		return 0;
	srcfile = get_srcline_split(map->dso,
				    map__rip_2objdump(map, addr),
				    &line);
	if (!srcfile)
		return 0;

	/* Avoid redundant printing */
	if (state &&
	    state->srcfile &&
	    !strcmp(state->srcfile, srcfile) &&
	    state->line == line) {
		free(srcfile);
		return 0;
	}

	srccode = find_sourceline(srcfile, line, &len);
	if (!srccode)
		goto out_free_line;

	ret = fprintf(fp, "|%-8d %.*s", line, len, srccode);

	if (state) {
		state->srcfile = srcfile;
		state->line = line;
	}
	return ret;

out_free_line:
	free(srcfile);
	return ret;
}

static int print_srccode(struct thread *thread, u8 cpumode, uint64_t addr)
{
	struct addr_location al;
	int ret = 0;

	memset(&al, 0, sizeof(al));
	thread__find_map(thread, cpumode, addr, &al);
	if (!al.map)
		return 0;
	ret = map__fprintf_srccode(al.map, al.addr, stdout,
		    &thread->srccode_state);
	if (ret)
		ret += printf("\n");
	return ret;
}

static int ip__fprintf_jump(uint64_t ip, struct branch_entry *en,
			    struct perf_insn *x, u8 *inbuf, int len,
			    int insn, FILE *fp, int *total_cycles)
{
	int printed = fprintf(fp, "\t%016" PRIx64 "\t%-30s\t#%s%s%s%s", ip,
			      dump_insn(x, ip, inbuf, len, NULL),
			      en->flags.predicted ? " PRED" : "",
			      en->flags.mispred ? " MISPRED" : "",
			      en->flags.in_tx ? " INTX" : "",
			      en->flags.abort ? " ABORT" : "");
	if (en->flags.cycles) {
		*total_cycles += en->flags.cycles;
		printed += fprintf(fp, " %d cycles [%d]", en->flags.cycles, *total_cycles);
		if (insn)
			printed += fprintf(fp, " %.2f IPC", (float)insn / en->flags.cycles);
	}
	return printed + fprintf(fp, "\n");
}

static int ip__fprintf_sym(uint64_t addr, struct thread *thread,
			   u8 cpumode, int cpu, struct symbol **lastsym,
			   struct perf_event_attr *attr, FILE *fp)
{
	struct addr_location al;
	int off, printed = 0;

	memset(&al, 0, sizeof(al));

	thread__find_map(thread, cpumode, addr, &al);

	if ((*lastsym) && al.addr >= (*lastsym)->start && al.addr < (*lastsym)->end)
		return 0;

	al.cpu = cpu;
	al.sym = NULL;
	if (al.map)
		al.sym = map__find_symbol(al.map, al.addr);

	if (!al.sym)
		return 0;

	if (al.addr < al.sym->end)
		off = al.addr - al.sym->start;
	else
		off = al.addr - al.map->start - al.sym->start;
	printed += fprintf(fp, "\t%s", al.sym->name);
	if (off)
		printed += fprintf(fp, "%+d", off);
	printed += fprintf(fp, ":");
	if (PRINT_FIELD(SRCLINE))
		printed += map__fprintf_srcline(al.map, al.addr, "\t", fp);
	printed += fprintf(fp, "\n");
	*lastsym = al.sym;

	return printed;
}

static int perf_sample__fprintf_brstackinsn(struct perf_sample *sample,
					    struct thread *thread,
					    struct perf_event_attr *attr,
					    struct machine *machine, FILE *fp)
{
	struct branch_stack *br = sample->branch_stack;
	struct branch_entry *entries = perf_sample__branch_entries(sample);
	u64 start, end;
	int i, insn, len, nr, ilen, printed = 0;
	struct perf_insn x;
	u8 buffer[MAXBB];
	unsigned off;
	struct symbol *lastsym = NULL;
	int total_cycles = 0;

	if (!(br && br->nr))
		return 0;
	nr = br->nr;
	if (max_blocks && nr > max_blocks + 1)
		nr = max_blocks + 1;

	x.thread = thread;
	x.cpu = sample->cpu;

	printed += fprintf(fp, "%c", '\n');

	/* Handle first from jump, of which we don't know the entry. */
	len = grab_bb(buffer, entries[nr-1].from,
			entries[nr-1].from,
			machine, thread, &x.is64bit, &x.cpumode, false);
	if (len > 0) {
		printed += ip__fprintf_sym(entries[nr - 1].from, thread,
					   x.cpumode, x.cpu, &lastsym, attr, fp);
		printed += ip__fprintf_jump(entries[nr - 1].from, &entries[nr - 1],
					    &x, buffer, len, 0, fp, &total_cycles);
		if (PRINT_FIELD(SRCCODE))
			printed += print_srccode(thread, x.cpumode, entries[nr - 1].from);
	}

	/* Print all blocks */
	for (i = nr - 2; i >= 0; i--) {
		if (entries[i].from || entries[i].to)
			pr_debug("%d: %" PRIx64 "-%" PRIx64 "\n", i,
				 entries[i].from,
				 entries[i].to);
		start = entries[i + 1].to;
		end   = entries[i].from;

		len = grab_bb(buffer, start, end, machine, thread, &x.is64bit, &x.cpumode, false);
		/* Patch up missing kernel transfers due to ring filters */
		if (len == -ENXIO && i > 0) {
			end = entries[--i].from;
			pr_debug("\tpatching up to %" PRIx64 "-%" PRIx64 "\n", start, end);
			len = grab_bb(buffer, start, end, machine, thread, &x.is64bit, &x.cpumode, false);
		}
		if (len <= 0)
			continue;

		insn = 0;
		for (off = 0; off < (unsigned)len; off += ilen) {
			uint64_t ip = start + off;

			printed += ip__fprintf_sym(ip, thread, x.cpumode, x.cpu, &lastsym, attr, fp);
			if (ip == end) {
				printed += ip__fprintf_jump(ip, &entries[i], &x, buffer + off, len - off, ++insn, fp,
							    &total_cycles);
				if (PRINT_FIELD(SRCCODE))
					printed += print_srccode(thread, x.cpumode, ip);
				break;
			} else {
				ilen = 0;
				printed += fprintf(fp, "\t%016" PRIx64 "\t%s\n", ip,
						   dump_insn(&x, ip, buffer + off, len - off, &ilen));
				if (ilen == 0)
					break;
				if (PRINT_FIELD(SRCCODE))
					print_srccode(thread, x.cpumode, ip);
				insn++;
			}
		}
		if (off != end - start)
			printed += fprintf(fp, "\tmismatch of LBR data and executable\n");
	}

	/*
	 * Hit the branch? In this case we are already done, and the target
	 * has not been executed yet.
	 */
	if (entries[0].from == sample->ip)
		goto out;
	if (entries[0].flags.abort)
		goto out;

	/*
	 * Print final block upto sample
	 *
	 * Due to pipeline delays the LBRs might be missing a branch
	 * or two, which can result in very large or negative blocks
	 * between final branch and sample. When this happens just
	 * continue walking after the last TO until we hit a branch.
	 */
	start = entries[0].to;
	end = sample->ip;
	if (end < start) {
		/* Missing jump. Scan 128 bytes for the next branch */
		end = start + 128;
	}
	len = grab_bb(buffer, start, end, machine, thread, &x.is64bit, &x.cpumode, true);
	printed += ip__fprintf_sym(start, thread, x.cpumode, x.cpu, &lastsym, attr, fp);
	if (len <= 0) {
		/* Print at least last IP if basic block did not work */
		len = grab_bb(buffer, sample->ip, sample->ip,
			      machine, thread, &x.is64bit, &x.cpumode, false);
		if (len <= 0)
			goto out;
		printed += fprintf(fp, "\t%016" PRIx64 "\t%s\n", sample->ip,
			dump_insn(&x, sample->ip, buffer, len, NULL));
		if (PRINT_FIELD(SRCCODE))
			print_srccode(thread, x.cpumode, sample->ip);
		goto out;
	}
	for (off = 0; off <= end - start; off += ilen) {
		ilen = 0;
		printed += fprintf(fp, "\t%016" PRIx64 "\t%s\n", start + off,
				   dump_insn(&x, start + off, buffer + off, len - off, &ilen));
		if (ilen == 0)
			break;
		if (arch_is_branch(buffer + off, len - off, x.is64bit) && start + off != sample->ip) {
			/*
			 * Hit a missing branch. Just stop.
			 */
			printed += fprintf(fp, "\t... not reaching sample ...\n");
			break;
		}
		if (PRINT_FIELD(SRCCODE))
			print_srccode(thread, x.cpumode, start + off);
	}
out:
	return printed;
}

static int perf_sample__fprintf_addr(struct perf_sample *sample,
				     struct thread *thread,
				     struct perf_event_attr *attr, FILE *fp)
{
	struct addr_location al;
	int printed = fprintf(fp, "%16" PRIx64, sample->addr);

	if (!sample_addr_correlates_sym(attr))
		goto out;

	thread__resolve(thread, &al, sample);

	if (PRINT_FIELD(SYM)) {
		printed += fprintf(fp, " ");
		if (PRINT_FIELD(SYMOFFSET))
			printed += symbol__fprintf_symname_offs(al.sym, &al, fp);
		else
			printed += symbol__fprintf_symname(al.sym, fp);
	}

	if (PRINT_FIELD(DSO)) {
		printed += fprintf(fp, " (");
		printed += map__fprintf_dsoname(al.map, fp);
		printed += fprintf(fp, ")");
	}
out:
	return printed;
}

static const char *resolve_branch_sym(struct perf_sample *sample,
				      struct evsel *evsel,
				      struct thread *thread,
				      struct addr_location *al,
				      u64 *ip)
{
	struct addr_location addr_al;
	struct perf_event_attr *attr = &evsel->core.attr;
	const char *name = NULL;

	if (sample->flags & (PERF_IP_FLAG_CALL | PERF_IP_FLAG_TRACE_BEGIN)) {
		if (sample_addr_correlates_sym(attr)) {
			thread__resolve(thread, &addr_al, sample);
			if (addr_al.sym)
				name = addr_al.sym->name;
			else
				*ip = sample->addr;
		} else {
			*ip = sample->addr;
		}
	} else if (sample->flags & (PERF_IP_FLAG_RETURN | PERF_IP_FLAG_TRACE_END)) {
		if (al->sym)
			name = al->sym->name;
		else
			*ip = sample->ip;
	}
	return name;
}

static int perf_sample__fprintf_callindent(struct perf_sample *sample,
					   struct evsel *evsel,
					   struct thread *thread,
					   struct addr_location *al, FILE *fp)
{
	struct perf_event_attr *attr = &evsel->core.attr;
	size_t depth = thread_stack__depth(thread, sample->cpu);
	const char *name = NULL;
	static int spacing;
	int len = 0;
	int dlen = 0;
	u64 ip = 0;

	/*
	 * The 'return' has already been popped off the stack so the depth has
	 * to be adjusted to match the 'call'.
	 */
	if (thread->ts && sample->flags & PERF_IP_FLAG_RETURN)
		depth += 1;

	name = resolve_branch_sym(sample, evsel, thread, al, &ip);

	if (PRINT_FIELD(DSO) && !(PRINT_FIELD(IP) || PRINT_FIELD(ADDR))) {
		dlen += fprintf(fp, "(");
		dlen += map__fprintf_dsoname(al->map, fp);
		dlen += fprintf(fp, ")\t");
	}

	if (name)
		len = fprintf(fp, "%*s%s", (int)depth * 4, "", name);
	else if (ip)
		len = fprintf(fp, "%*s%16" PRIx64, (int)depth * 4, "", ip);

	if (len < 0)
		return len;

	/*
	 * Try to keep the output length from changing frequently so that the
	 * output lines up more nicely.
	 */
	if (len > spacing || (len && len < spacing - 52))
		spacing = round_up(len + 4, 32);

	if (len < spacing)
		len += fprintf(fp, "%*s", spacing - len, "");

	return len + dlen;
}

__weak void arch_fetch_insn(struct perf_sample *sample __maybe_unused,
			    struct thread *thread __maybe_unused,
			    struct machine *machine __maybe_unused)
{
}

static int perf_sample__fprintf_insn(struct perf_sample *sample,
				     struct perf_event_attr *attr,
				     struct thread *thread,
				     struct machine *machine, FILE *fp)
{
	int printed = 0;

	if (sample->insn_len == 0 && native_arch)
		arch_fetch_insn(sample, thread, machine);

	if (PRINT_FIELD(INSNLEN))
		printed += fprintf(fp, " ilen: %d", sample->insn_len);
	if (PRINT_FIELD(INSN) && sample->insn_len) {
		int i;

		printed += fprintf(fp, " insn:");
		for (i = 0; i < sample->insn_len; i++)
			printed += fprintf(fp, " %02x", (unsigned char)sample->insn[i]);
	}
	if (PRINT_FIELD(BRSTACKINSN))
		printed += perf_sample__fprintf_brstackinsn(sample, thread, attr, machine, fp);

	return printed;
}

static int perf_sample__fprintf_ipc(struct perf_sample *sample,
				    struct perf_event_attr *attr, FILE *fp)
{
	unsigned int ipc;

	if (!PRINT_FIELD(IPC) || !sample->cyc_cnt || !sample->insn_cnt)
		return 0;

	ipc = (sample->insn_cnt * 100) / sample->cyc_cnt;

	return fprintf(fp, " \t IPC: %u.%02u (%" PRIu64 "/%" PRIu64 ") ",
		       ipc / 100, ipc % 100, sample->insn_cnt, sample->cyc_cnt);
}

static int perf_sample__fprintf_bts(struct perf_sample *sample,
				    struct evsel *evsel,
				    struct thread *thread,
				    struct addr_location *al,
				    struct machine *machine, FILE *fp)
{
	struct perf_event_attr *attr = &evsel->core.attr;
	unsigned int type = output_type(attr->type);
	bool print_srcline_last = false;
	int printed = 0;

	if (PRINT_FIELD(CALLINDENT))
		printed += perf_sample__fprintf_callindent(sample, evsel, thread, al, fp);

	/* print branch_from information */
	if (PRINT_FIELD(IP)) {
		unsigned int print_opts = output[type].print_ip_opts;
		struct callchain_cursor *cursor = NULL;

		if (symbol_conf.use_callchain && sample->callchain &&
		    thread__resolve_callchain(al->thread, &callchain_cursor, evsel,
					      sample, NULL, NULL, scripting_max_stack) == 0)
			cursor = &callchain_cursor;

		if (cursor == NULL) {
			printed += fprintf(fp, " ");
			if (print_opts & EVSEL__PRINT_SRCLINE) {
				print_srcline_last = true;
				print_opts &= ~EVSEL__PRINT_SRCLINE;
			}
		} else
			printed += fprintf(fp, "\n");

		printed += sample__fprintf_sym(sample, al, 0, print_opts, cursor,
					       symbol_conf.bt_stop_list, fp);
	}

	/* print branch_to information */
	if (PRINT_FIELD(ADDR) ||
	    ((evsel->core.attr.sample_type & PERF_SAMPLE_ADDR) &&
	     !output[type].user_set)) {
		printed += fprintf(fp, " => ");
		printed += perf_sample__fprintf_addr(sample, thread, attr, fp);
	}

	printed += perf_sample__fprintf_ipc(sample, attr, fp);

	if (print_srcline_last)
		printed += map__fprintf_srcline(al->map, al->addr, "\n  ", fp);

	printed += perf_sample__fprintf_insn(sample, attr, thread, machine, fp);
	printed += fprintf(fp, "\n");
	if (PRINT_FIELD(SRCCODE)) {
		int ret = map__fprintf_srccode(al->map, al->addr, stdout,
					 &thread->srccode_state);
		if (ret) {
			printed += ret;
			printed += printf("\n");
		}
	}
	return printed;
}

static struct {
	u32 flags;
	const char *name;
} sample_flags[] = {
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL, "call"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_RETURN, "return"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CONDITIONAL, "jcc"},
	{PERF_IP_FLAG_BRANCH, "jmp"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_INTERRUPT, "int"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_RETURN | PERF_IP_FLAG_INTERRUPT, "iret"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_SYSCALLRET, "syscall"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_RETURN | PERF_IP_FLAG_SYSCALLRET, "sysret"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_ASYNC, "async"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_ASYNC |	PERF_IP_FLAG_INTERRUPT, "hw int"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_TX_ABORT, "tx abrt"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_TRACE_BEGIN, "tr strt"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_TRACE_END, "tr end"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_VMENTRY, "vmentry"},
	{PERF_IP_FLAG_BRANCH | PERF_IP_FLAG_CALL | PERF_IP_FLAG_VMEXIT, "vmexit"},
	{0, NULL}
};

static const char *sample_flags_to_name(u32 flags)
{
	int i;

	for (i = 0; sample_flags[i].name ; i++) {
		if (sample_flags[i].flags == flags)
			return sample_flags[i].name;
	}

	return NULL;
}

static int perf_sample__fprintf_flags(u32 flags, FILE *fp)
{
	const char *chars = PERF_IP_FLAG_CHARS;
	const int n = strlen(PERF_IP_FLAG_CHARS);
	bool in_tx = flags & PERF_IP_FLAG_IN_TX;
	const char *name = NULL;
	char str[33];
	int i, pos = 0;

	name = sample_flags_to_name(flags & ~PERF_IP_FLAG_IN_TX);
	if (name)
		return fprintf(fp, "  %-15s%4s ", name, in_tx ? "(x)" : "");

	if (flags & PERF_IP_FLAG_TRACE_BEGIN) {
		name = sample_flags_to_name(flags & ~(PERF_IP_FLAG_IN_TX | PERF_IP_FLAG_TRACE_BEGIN));
		if (name)
			return fprintf(fp, "  tr strt %-7s%4s ", name, in_tx ? "(x)" : "");
	}

	if (flags & PERF_IP_FLAG_TRACE_END) {
		name = sample_flags_to_name(flags & ~(PERF_IP_FLAG_IN_TX | PERF_IP_FLAG_TRACE_END));
		if (name)
			return fprintf(fp, "  tr end  %-7s%4s ", name, in_tx ? "(x)" : "");
	}

	for (i = 0; i < n; i++, flags >>= 1) {
		if (flags & 1)
			str[pos++] = chars[i];
	}
	for (; i < 32; i++, flags >>= 1) {
		if (flags & 1)
			str[pos++] = '?';
	}
	str[pos] = 0;

	return fprintf(fp, "  %-19s ", str);
}

struct printer_data {
	int line_no;
	bool hit_nul;
	bool is_printable;
};

static int sample__fprintf_bpf_output(enum binary_printer_ops op,
				      unsigned int val,
				      void *extra, FILE *fp)
{
	unsigned char ch = (unsigned char)val;
	struct printer_data *printer_data = extra;
	int printed = 0;

	switch (op) {
	case BINARY_PRINT_DATA_BEGIN:
		printed += fprintf(fp, "\n");
		break;
	case BINARY_PRINT_LINE_BEGIN:
		printed += fprintf(fp, "%17s", !printer_data->line_no ? "BPF output:" :
						        "           ");
		break;
	case BINARY_PRINT_ADDR:
		printed += fprintf(fp, " %04x:", val);
		break;
	case BINARY_PRINT_NUM_DATA:
		printed += fprintf(fp, " %02x", val);
		break;
	case BINARY_PRINT_NUM_PAD:
		printed += fprintf(fp, "   ");
		break;
	case BINARY_PRINT_SEP:
		printed += fprintf(fp, "  ");
		break;
	case BINARY_PRINT_CHAR_DATA:
		if (printer_data->hit_nul && ch)
			printer_data->is_printable = false;

		if (!isprint(ch)) {
			printed += fprintf(fp, "%c", '.');

			if (!printer_data->is_printable)
				break;

			if (ch == '\0')
				printer_data->hit_nul = true;
			else
				printer_data->is_printable = false;
		} else {
			printed += fprintf(fp, "%c", ch);
		}
		break;
	case BINARY_PRINT_CHAR_PAD:
		printed += fprintf(fp, " ");
		break;
	case BINARY_PRINT_LINE_END:
		printed += fprintf(fp, "\n");
		printer_data->line_no++;
		break;
	case BINARY_PRINT_DATA_END:
	default:
		break;
	}

	return printed;
}

static int perf_sample__fprintf_bpf_output(struct perf_sample *sample, FILE *fp)
{
	unsigned int nr_bytes = sample->raw_size;
	struct printer_data printer_data = {0, false, true};
	int printed = binary__fprintf(sample->raw_data, nr_bytes, 8,
				      sample__fprintf_bpf_output, &printer_data, fp);

	if (printer_data.is_printable && printer_data.hit_nul)
		printed += fprintf(fp, "%17s \"%s\"\n", "BPF string:", (char *)(sample->raw_data));

	return printed;
}

static int perf_sample__fprintf_spacing(int len, int spacing, FILE *fp)
{
	if (len > 0 && len < spacing)
		return fprintf(fp, "%*s", spacing - len, "");

	return 0;
}

static int perf_sample__fprintf_pt_spacing(int len, FILE *fp)
{
	return perf_sample__fprintf_spacing(len, 34, fp);
}

static int perf_sample__fprintf_synth_ptwrite(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_ptwrite *data = perf_sample__synth_ptr(sample);
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	len = fprintf(fp, " IP: %u payload: %#" PRIx64 " ",
		     data->ip, le64_to_cpu(data->payload));
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth_mwait(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_mwait *data = perf_sample__synth_ptr(sample);
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	len = fprintf(fp, " hints: %#x extensions: %#x ",
		      data->hints, data->extensions);
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth_pwre(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_pwre *data = perf_sample__synth_ptr(sample);
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	len = fprintf(fp, " hw: %u cstate: %u sub-cstate: %u ",
		      data->hw, data->cstate, data->subcstate);
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth_exstop(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_exstop *data = perf_sample__synth_ptr(sample);
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	len = fprintf(fp, " IP: %u ", data->ip);
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth_pwrx(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_pwrx *data = perf_sample__synth_ptr(sample);
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	len = fprintf(fp, " deepest cstate: %u last cstate: %u wake reason: %#x ",
		     data->deepest_cstate, data->last_cstate,
		     data->wake_reason);
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth_cbr(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_cbr *data = perf_sample__synth_ptr(sample);
	unsigned int percent, freq;
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	freq = (le32_to_cpu(data->freq) + 500) / 1000;
	len = fprintf(fp, " cbr: %2u freq: %4u MHz ", data->cbr, freq);
	if (data->max_nonturbo) {
		percent = (5 + (1000 * data->cbr) / data->max_nonturbo) / 10;
		len += fprintf(fp, "(%3u%%) ", percent);
	}
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth_psb(struct perf_sample *sample, FILE *fp)
{
	struct perf_synth_intel_psb *data = perf_sample__synth_ptr(sample);
	int len;

	if (perf_sample__bad_synth_size(sample, *data))
		return 0;

	len = fprintf(fp, " psb offs: %#" PRIx64, data->offset);
	return len + perf_sample__fprintf_pt_spacing(len, fp);
}

static int perf_sample__fprintf_synth(struct perf_sample *sample,
				      struct evsel *evsel, FILE *fp)
{
	switch (evsel->core.attr.config) {
	case PERF_SYNTH_INTEL_PTWRITE:
		return perf_sample__fprintf_synth_ptwrite(sample, fp);
	case PERF_SYNTH_INTEL_MWAIT:
		return perf_sample__fprintf_synth_mwait(sample, fp);
	case PERF_SYNTH_INTEL_PWRE:
		return perf_sample__fprintf_synth_pwre(sample, fp);
	case PERF_SYNTH_INTEL_EXSTOP:
		return perf_sample__fprintf_synth_exstop(sample, fp);
	case PERF_SYNTH_INTEL_PWRX:
		return perf_sample__fprintf_synth_pwrx(sample, fp);
	case PERF_SYNTH_INTEL_CBR:
		return perf_sample__fprintf_synth_cbr(sample, fp);
	case PERF_SYNTH_INTEL_PSB:
		return perf_sample__fprintf_synth_psb(sample, fp);
	default:
		break;
	}

	return 0;
}

static int evlist__max_name_len(struct evlist *evlist)
{
	struct evsel *evsel;
	int max = 0;

	evlist__for_each_entry(evlist, evsel) {
		int len = strlen(evsel__name(evsel));

		max = MAX(len, max);
	}

	return max;
}

static int data_src__fprintf(u64 data_src, FILE *fp)
{
	struct mem_info mi = { .data_src.val = data_src };
	char decode[100];
	char out[100];
	static int maxlen;
	int len;

	perf_script__meminfo_scnprintf(decode, 100, &mi);

	len = scnprintf(out, 100, "%16" PRIx64 " %s", data_src, decode);
	if (maxlen < len)
		maxlen = len;

	return fprintf(fp, "%-*s", maxlen, out);
}

struct metric_ctx {
	struct perf_sample	*sample;
	struct thread		*thread;
	struct evsel	*evsel;
	FILE 			*fp;
};

static void script_print_metric(struct perf_stat_config *config __maybe_unused,
				void *ctx, const char *color,
			        const char *fmt,
			        const char *unit, double val)
{
	struct metric_ctx *mctx = ctx;

	if (!fmt)
		return;
	perf_sample__fprintf_start(NULL, mctx->sample, mctx->thread, mctx->evsel,
				   PERF_RECORD_SAMPLE, mctx->fp);
	fputs("\tmetric: ", mctx->fp);
	if (color)
		color_fprintf(mctx->fp, color, fmt, val);
	else
		printf(fmt, val);
	fprintf(mctx->fp, " %s\n", unit);
}

static void script_new_line(struct perf_stat_config *config __maybe_unused,
			    void *ctx)
{
	struct metric_ctx *mctx = ctx;

	perf_sample__fprintf_start(NULL, mctx->sample, mctx->thread, mctx->evsel,
				   PERF_RECORD_SAMPLE, mctx->fp);
	fputs("\tmetric: ", mctx->fp);
}

static void perf_sample__fprint_metric(struct perf_script *script,
				       struct thread *thread,
				       struct evsel *evsel,
				       struct perf_sample *sample,
				       FILE *fp)
{
	struct perf_stat_output_ctx ctx = {
		.print_metric = script_print_metric,
		.new_line = script_new_line,
		.ctx = &(struct metric_ctx) {
				.sample = sample,
				.thread = thread,
				.evsel  = evsel,
				.fp     = fp,
			 },
		.force_header = false,
	};
	struct evsel *ev2;
	u64 val;

	if (!evsel->stats)
		evlist__alloc_stats(script->session->evlist, false);
	if (evsel_script(evsel->leader)->gnum++ == 0)
		perf_stat__reset_shadow_stats();
	val = sample->period * evsel->scale;
	perf_stat__update_shadow_stats(evsel,
				       val,
				       sample->cpu,
				       &rt_stat);
	evsel_script(evsel)->val = val;
	if (evsel_script(evsel->leader)->gnum == evsel->leader->core.nr_members) {
		for_each_group_member (ev2, evsel->leader) {
			perf_stat__print_shadow_stats(&stat_config, ev2,
						      evsel_script(ev2)->val,
						      sample->cpu,
						      &ctx,
						      NULL,
						      &rt_stat);
		}
		evsel_script(evsel->leader)->gnum = 0;
	}
}

static bool show_event(struct perf_sample *sample,
		       struct evsel *evsel,
		       struct thread *thread,
		       struct addr_location *al)
{
	int depth = thread_stack__depth(thread, sample->cpu);

	if (!symbol_conf.graph_function)
		return true;

	if (thread->filter) {
		if (depth <= thread->filter_entry_depth) {
			thread->filter = false;
			return false;
		}
		return true;
	} else {
		const char *s = symbol_conf.graph_function;
		u64 ip;
		const char *name = resolve_branch_sym(sample, evsel, thread, al,
				&ip);
		unsigned nlen;

		if (!name)
			return false;
		nlen = strlen(name);
		while (*s) {
			unsigned len = strcspn(s, ",");
			if (nlen == len && !strncmp(name, s, len)) {
				thread->filter = true;
				thread->filter_entry_depth = depth;
				return true;
			}
			s += len;
			if (*s == ',')
				s++;
		}
		return false;
	}
}

static void process_event(struct perf_script *script,
			  struct perf_sample *sample, struct evsel *evsel,
			  struct addr_location *al,
			  struct machine *machine)
{
	struct thread *thread = al->thread;
	struct perf_event_attr *attr = &evsel->core.attr;
	unsigned int type = output_type(attr->type);
	struct evsel_script *es = evsel->priv;
	FILE *fp = es->fp;
	char str[PAGE_SIZE_NAME_LEN];

	if (output[type].fields == 0)
		return;

	if (!show_event(sample, evsel, thread, al))
		return;

	if (evswitch__discard(&script->evswitch, evsel))
		return;

	++es->samples;

	perf_sample__fprintf_start(script, sample, thread, evsel,
				   PERF_RECORD_SAMPLE, fp);

	if (PRINT_FIELD(PERIOD))
		fprintf(fp, "%10" PRIu64 " ", sample->period);

	if (PRINT_FIELD(EVNAME)) {
		const char *evname = evsel__name(evsel);

		if (!script->name_width)
			script->name_width = evlist__max_name_len(script->session->evlist);

		fprintf(fp, "%*s: ", script->name_width, evname ?: "[unknown]");
	}

	if (print_flags)
		perf_sample__fprintf_flags(sample->flags, fp);

	if (is_bts_event(attr)) {
		perf_sample__fprintf_bts(sample, evsel, thread, al, machine, fp);
		return;
	}

	if (PRINT_FIELD(TRACE) && sample->raw_data) {
		event_format__fprintf(evsel->tp_format, sample->cpu,
				      sample->raw_data, sample->raw_size, fp);
	}

	if (attr->type == PERF_TYPE_SYNTH && PRINT_FIELD(SYNTH))
		perf_sample__fprintf_synth(sample, evsel, fp);

	if (PRINT_FIELD(ADDR))
		perf_sample__fprintf_addr(sample, thread, attr, fp);

	if (PRINT_FIELD(DATA_SRC))
		data_src__fprintf(sample->data_src, fp);

	if (PRINT_FIELD(WEIGHT))
		fprintf(fp, "%16" PRIu64, sample->weight);

	if (PRINT_FIELD(IP)) {
		struct callchain_cursor *cursor = NULL;

		if (script->stitch_lbr)
			al->thread->lbr_stitch_enable = true;

		if (symbol_conf.use_callchain && sample->callchain &&
		    thread__resolve_callchain(al->thread, &callchain_cursor, evsel,
					      sample, NULL, NULL, scripting_max_stack) == 0)
			cursor = &callchain_cursor;

		fputc(cursor ? '\n' : ' ', fp);
		sample__fprintf_sym(sample, al, 0, output[type].print_ip_opts, cursor,
				    symbol_conf.bt_stop_list, fp);
	}

	if (PRINT_FIELD(IREGS))
		perf_sample__fprintf_iregs(sample, attr, fp);

	if (PRINT_FIELD(UREGS))
		perf_sample__fprintf_uregs(sample, attr, fp);

	if (PRINT_FIELD(BRSTACK))
		perf_sample__fprintf_brstack(sample, thread, attr, fp);
	else if (PRINT_FIELD(BRSTACKSYM))
		perf_sample__fprintf_brstacksym(sample, thread, attr, fp);
	else if (PRINT_FIELD(BRSTACKOFF))
		perf_sample__fprintf_brstackoff(sample, thread, attr, fp);

	if (evsel__is_bpf_output(evsel) && PRINT_FIELD(BPF_OUTPUT))
		perf_sample__fprintf_bpf_output(sample, fp);
	perf_sample__fprintf_insn(sample, attr, thread, machine, fp);

	if (PRINT_FIELD(PHYS_ADDR))
		fprintf(fp, "%16" PRIx64, sample->phys_addr);

	if (PRINT_FIELD(DATA_PAGE_SIZE))
		fprintf(fp, " %s", get_page_size_name(sample->data_page_size, str));

	if (PRINT_FIELD(CODE_PAGE_SIZE))
		fprintf(fp, " %s", get_page_size_name(sample->code_page_size, str));

	perf_sample__fprintf_ipc(sample, attr, fp);

	fprintf(fp, "\n");

	if (PRINT_FIELD(SRCCODE)) {
		if (map__fprintf_srccode(al->map, al->addr, stdout,
					 &thread->srccode_state))
			printf("\n");
	}

	if (PRINT_FIELD(METRIC))
		perf_sample__fprint_metric(script, thread, evsel, sample, fp);

	if (verbose)
		fflush(fp);
}

static struct scripting_ops	*scripting_ops;

static void __process_stat(struct evsel *counter, u64 tstamp)
{
	int nthreads = perf_thread_map__nr(counter->core.threads);
	int ncpus = evsel__nr_cpus(counter);
	int cpu, thread;
	static int header_printed;

	if (counter->core.system_wide)
		nthreads = 1;

	if (!header_printed) {
		printf("%3s %8s %15s %15s %15s %15s %s\n",
		       "CPU", "THREAD", "VAL", "ENA", "RUN", "TIME", "EVENT");
		header_printed = 1;
	}

	for (thread = 0; thread < nthreads; thread++) {
		for (cpu = 0; cpu < ncpus; cpu++) {
			struct perf_counts_values *counts;

			counts = perf_counts(counter->counts, cpu, thread);

			printf("%3d %8d %15" PRIu64 " %15" PRIu64 " %15" PRIu64 " %15" PRIu64 " %s\n",
				counter->core.cpus->map[cpu],
				perf_thread_map__pid(counter->core.threads, thread),
				counts->val,
				counts->ena,
				counts->run,
				tstamp,
				evsel__name(counter));
		}
	}
}

static void process_stat(struct evsel *counter, u64 tstamp)
{
	if (scripting_ops && scripting_ops->process_stat)
		scripting_ops->process_stat(&stat_config, counter, tstamp);
	else
		__process_stat(counter, tstamp);
}

static void process_stat_interval(u64 tstamp)
{
	if (scripting_ops && scripting_ops->process_stat_interval)
		scripting_ops->process_stat_interval(tstamp);
}

static void setup_scripting(void)
{
	setup_perl_scripting();
	setup_python_scripting();
}

static int flush_scripting(void)
{
	return scripting_ops ? scripting_ops->flush_script() : 0;
}

static int cleanup_scripting(void)
{
	pr_debug("\nperf script stopped\n");

	return scripting_ops ? scripting_ops->stop_script() : 0;
}

static bool filter_cpu(struct perf_sample *sample)
{
	if (cpu_list && sample->cpu != (u32)-1)
		return !test_bit(sample->cpu, cpu_bitmap);
	return false;
}

static int process_sample_event(struct perf_tool *tool,
				union perf_event *event,
				struct perf_sample *sample,
				struct evsel *evsel,
				struct machine *machine)
{
	struct perf_script *scr = container_of(tool, struct perf_script, tool);
	struct addr_location al;

	if (perf_time__ranges_skip_sample(scr->ptime_range, scr->range_num,
					  sample->time)) {
		return 0;
	}

	if (debug_mode) {
		if (sample->time < last_timestamp) {
			pr_err("Samples misordered, previous: %" PRIu64
				" this: %" PRIu64 "\n", last_timestamp,
				sample->time);
			nr_unordered++;
		}
		last_timestamp = sample->time;
		return 0;
	}

	if (machine__resolve(machine, &al, sample) < 0) {
		pr_err("problem processing %d event, skipping it.\n",
		       event->header.type);
		return -1;
	}

	if (al.filtered)
		goto out_put;

	if (filter_cpu(sample))
		goto out_put;

	if (scripting_ops)
		scripting_ops->process_event(event, sample, evsel, &al);
	else
		process_event(scr, sample, evsel, &al, machine);

out_put:
	addr_location__put(&al);
	return 0;
}

static int process_attr(struct perf_tool *tool, union perf_event *event,
			struct evlist **pevlist)
{
	struct perf_script *scr = container_of(tool, struct perf_script, tool);
	struct evlist *evlist;
	struct evsel *evsel, *pos;
	u64 sample_type;
	int err;
	static struct evsel_script *es;

	err = perf_event__process_attr(tool, event, pevlist);
	if (err)
		return err;

	evlist = *pevlist;
	evsel = evlist__last(*pevlist);

	if (!evsel->priv) {
		if (scr->per_event_dump) {
			evsel->priv = evsel_script__new(evsel, scr->session->data);
		} else {
			es = zalloc(sizeof(*es));
			if (!es)
				return -ENOMEM;
			es->fp = stdout;
			evsel->priv = es;
		}
	}

	if (evsel->core.attr.type >= PERF_TYPE_MAX &&
	    evsel->core.attr.type != PERF_TYPE_SYNTH)
		return 0;

	evlist__for_each_entry(evlist, pos) {
		if (pos->core.attr.type == evsel->core.attr.type && pos != evsel)
			return 0;
	}

	if (evsel->core.attr.sample_type) {
		err = evsel__check_attr(evsel, scr->session);
		if (err)
			return err;
	}

	/*
	 * Check if we need to enable callchains based
	 * on events sample_type.
	 */
	sample_type = evlist__combined_sample_type(evlist);
	callchain_param_setup(sample_type);

	/* Enable fields for callchain entries */
	if (symbol_conf.use_callchain &&
	    (sample_type & PERF_SAMPLE_CALLCHAIN ||
	     sample_type & PERF_SAMPLE_BRANCH_STACK ||
	     (sample_type & PERF_SAMPLE_REGS_USER &&
	      sample_type & PERF_SAMPLE_STACK_USER))) {
		int type = output_type(evsel->core.attr.type);

		if (!(output[type].user_unset_fields & PERF_OUTPUT_IP))
			output[type].fields |= PERF_OUTPUT_IP;
		if (!(output[type].user_unset_fields & PERF_OUTPUT_SYM))
			output[type].fields |= PERF_OUTPUT_SYM;
	}
	set_print_ip_opts(&evsel->core.attr);
	return 0;
}

static int print_event_with_time(struct perf_tool *tool,
				 union perf_event *event,
				 struct perf_sample *sample,
				 struct machine *machine,
				 pid_t pid, pid_t tid, u64 timestamp)
{
	struct perf_script *script = container_of(tool, struct perf_script, tool);
	struct perf_session *session = script->session;
	struct evsel *evsel = evlist__id2evsel(session->evlist, sample->id);
	struct thread *thread = NULL;

	if (evsel && !evsel->core.attr.sample_id_all) {
		sample->cpu = 0;
		sample->time = timestamp;
		sample->pid = pid;
		sample->tid = tid;
	}

	if (filter_cpu(sample))
		return 0;

	if (tid != -1)
		thread = machine__findnew_thread(machine, pid, tid);

	if (evsel) {
		perf_sample__fprintf_start(script, sample, thread, evsel,
					   event->header.type, stdout);
	}

	perf_event__fprintf(event, machine, stdout);

	thread__put(thread);

	return 0;
}

static int print_event(struct perf_tool *tool, union perf_event *event,
		       struct perf_sample *sample, struct machine *machine,
		       pid_t pid, pid_t tid)
{
	return print_event_with_time(tool, event, sample, machine, pid, tid, 0);
}

static int process_comm_event(struct perf_tool *tool,
			      union perf_event *event,
			      struct perf_sample *sample,
			      struct machine *machine)
{
	if (perf_event__process_comm(tool, event, sample, machine) < 0)
		return -1;

	return print_event(tool, event, sample, machine, event->comm.pid,
			   event->comm.tid);
}

static int process_namespaces_event(struct perf_tool *tool,
				    union perf_event *event,
				    struct perf_sample *sample,
				    struct machine *machine)
{
	if (perf_event__process_namespaces(tool, event, sample, machine) < 0)
		return -1;

	return print_event(tool, event, sample, machine, event->namespaces.pid,
			   event->namespaces.tid);
}

static int process_cgroup_event(struct perf_tool *tool,
				union perf_event *event,
				struct perf_sample *sample,
				struct machine *machine)
{
	if (perf_event__process_cgroup(tool, event, sample, machine) < 0)
		return -1;

	return print_event(tool, event, sample, machine, sample->pid,
			    sample->tid);
}

static int process_fork_event(struct perf_tool *tool,
			      union perf_event *event,
			      struct perf_sample *sample,
			      struct machine *machine)
{
	if (perf_event__process_fork(tool, event, sample, machine) < 0)
		return -1;

	return print_event_with_time(tool, event, sample, machine,
				     event->fork.pid, event->fork.tid,
				     event->fork.time);
}
static int process_exit_event(struct perf_tool *tool,
			      union perf_event *event,
			      struct perf_sample *sample,
			      struct machine *machine)
{
	/* Print before 'exit' deletes anything */
	if (print_event_with_time(tool, event, sample, machine, event->fork.pid,
				  event->fork.tid, event->fork.time))
		return -1;

	return perf_event__process_exit(tool, event, sample, machine);
}

static int process_mmap_event(struct perf_tool *tool,
			      union perf_event *event,
			      struct perf_sample *sample,
			      struct machine *machine)
{
	if (perf_event__process_mmap(tool, event, sample, machine) < 0)
		return -1;

	return print_event(tool, event, sample, machine, event->mmap.pid,
			   event->mmap.tid);
}

static int process_mmap2_event(struct perf_tool *tool,
			      union perf_event *event,
			      struct perf_sample *sample,
			      struct machine *machine)
{
	if (perf_event__process_mmap2(tool, event, sample, machine) < 0)
		return -1;

	return print_event(tool, event, sample, machine, event->mmap2.pid,
			   event->mmap2.tid);
}

static int process_switch_event(struct perf_tool *tool,
				union perf_event *event,
				struct perf_sample *sample,
				struct machine *machine)
{
	struct perf_script *script = container_of(tool, struct perf_script, tool);

	if (perf_event__process_switch(tool, event, sample, machine) < 0)
		return -1;

	if (scripting_ops && scripting_ops->process_switch)
		scripting_ops->process_switch(event, sample, machine);

	if (!script->show_switch_events)
		return 0;

	return print_event(tool, event, sample, machine, sample->pid,
			   sample->tid);
}

static int
process_lost_event(struct perf_tool *tool,
		   union perf_event *event,
		   struct perf_sample *sample,
		   struct machine *machine)
{
	return print_event(tool, event, sample, machine, sample->pid,
			   sample->tid);
}

static int
process_finished_round_event(struct perf_tool *tool __maybe_unused,
			     union perf_event *event,
			     struct ordered_events *oe __maybe_unused)

{
	perf_event__fprintf(event, NULL, stdout);
	return 0;
}

static int
process_bpf_events(struct perf_tool *tool __maybe_unused,
		   union perf_event *event,
		   struct perf_sample *sample,
		   struct machine *machine)
{
	if (machine__process_ksymbol(machine, event, sample) < 0)
		return -1;

	return print_event(tool, event, sample, machine, sample->pid,
			   sample->tid);
}

static int process_text_poke_events(struct perf_tool *tool,
				    union perf_event *event,
				    struct perf_sample *sample,
				    struct machine *machine)
{
	if (perf_event__process_text_poke(tool, event, sample, machine) < 0)
		return -1;

	return print_event(tool, event, sample, machine, sample->pid,
			   sample->tid);
}

static void sig_handler(int sig __maybe_unused)
{
	session_done = 1;
}

static void perf_script__fclose_per_event_dump(struct perf_script *script)
{
	struct evlist *evlist = script->session->evlist;
	struct evsel *evsel;

	evlist__for_each_entry(evlist, evsel) {
		if (!evsel->priv)
			break;
		evsel_script__delete(evsel->priv);
		evsel->priv = NULL;
	}
}

static int perf_script__fopen_per_event_dump(struct perf_script *script)
{
	struct evsel *evsel;

	evlist__for_each_entry(script->session->evlist, evsel) {
		/*
		 * Already setup? I.e. we may be called twice in cases like
		 * Intel PT, one for the intel_pt// and dummy events, then
		 * for the evsels synthesized from the auxtrace info.
		 *
		 * Ses perf_script__process_auxtrace_info.
		 */
		if (evsel->priv != NULL)
			continue;

		evsel->priv = evsel_script__new(evsel, script->session->data);
		if (evsel->priv == NULL)
			goto out_err_fclose;
	}

	return 0;

out_err_fclose:
	perf_script__fclose_per_event_dump(script);
	return -1;
}

static int perf_script__setup_per_event_dump(struct perf_script *script)
{
	struct evsel *evsel;
	static struct evsel_script es_stdout;

	if (script->per_event_dump)
		return perf_script__fopen_per_event_dump(script);

	es_stdout.fp = stdout;

	evlist__for_each_entry(script->session->evlist, evsel)
		evsel->priv = &es_stdout;

	return 0;
}

static void perf_script__exit_per_event_dump_stats(struct perf_script *script)
{
	struct evsel *evsel;

	evlist__for_each_entry(script->session->evlist, evsel) {
		struct evsel_script *es = evsel->priv;

		evsel_script__fprintf(es, stdout);
		evsel_script__delete(es);
		evsel->priv = NULL;
	}
}

static int __cmd_script(struct perf_script *script)
{
	int ret;

	signal(SIGINT, sig_handler);

	perf_stat__init_shadow_stats();

	/* override event processing functions */
	if (script->show_task_events) {
		script->tool.comm = process_comm_event;
		script->tool.fork = process_fork_event;
		script->tool.exit = process_exit_event;
	}
	if (script->show_mmap_events) {
		script->tool.mmap = process_mmap_event;
		script->tool.mmap2 = process_mmap2_event;
	}
	if (script->show_switch_events || (scripting_ops && scripting_ops->process_switch))
		script->tool.context_switch = process_switch_event;
	if (script->show_namespace_events)
		script->tool.namespaces = process_namespaces_event;
	if (script->show_cgroup_events)
		script->tool.cgroup = process_cgroup_event;
	if (script->show_lost_events)
		script->tool.lost = process_lost_event;
	if (script->show_round_events) {
		script->tool.ordered_events = false;
		script->tool.finished_round = process_finished_round_event;
	}
	if (script->show_bpf_events) {
		script->tool.ksymbol = process_bpf_events;
		script->tool.bpf     = process_bpf_events;
	}
	if (script->show_text_poke_events) {
		script->tool.ksymbol   = process_bpf_events;
		script->tool.text_poke = process_text_poke_events;
	}

	if (perf_script__setup_per_event_dump(script)) {
		pr_err("Couldn't create the per event dump files\n");
		return -1;
	}

	ret = perf_session__process_events(script->session);

	if (script->per_event_dump)
		perf_script__exit_per_event_dump_stats(script);

	if (debug_mode)
		pr_err("Misordered timestamps: %" PRIu64 "\n", nr_unordered);

	return ret;
}

struct script_spec {
	struct list_head	node;
	struct scripting_ops	*ops;
	char			spec[];
};

static LIST_HEAD(script_specs);

static struct script_spec *script_spec__new(const char *spec,
					    struct scripting_ops *ops)
{
	struct script_spec *s = malloc(sizeof(*s) + strlen(spec) + 1);

	if (s != NULL) {
		strcpy(s->spec, spec);
		s->ops = ops;
	}

	return s;
}

static void script_spec__add(struct script_spec *s)
{
	list_add_tail(&s->node, &script_specs);
}

static struct script_spec *script_spec__find(const char *spec)
{
	struct script_spec *s;

	list_for_each_entry(s, &script_specs, node)
		if (strcasecmp(s->spec, spec) == 0)
			return s;
	return NULL;
}

int script_spec_register(const char *spec, struct scripting_ops *ops)
{
	struct script_spec *s;

	s = script_spec__find(spec);
	if (s)
		return -1;

	s = script_spec__new(spec, ops);
	if (!s)
		return -1;
	else
		script_spec__add(s);

	return 0;
}

static struct scripting_ops *script_spec__lookup(const char *spec)
{
	struct script_spec *s = script_spec__find(spec);
	if (!s)
		return NULL;

	return s->ops;
}

static void list_available_languages(void)
{
	struct script_spec *s;

	fprintf(stderr, "\n");
	fprintf(stderr, "Scripting language extensions (used in "
		"perf script -s [spec:]script.[spec]):\n\n");

	list_for_each_entry(s, &script_specs, node)
		fprintf(stderr, "  %-42s [%s]\n", s->spec, s->ops->name);

	fprintf(stderr, "\n");
}

static int parse_scriptname(const struct option *opt __maybe_unused,
			    const char *str, int unset __maybe_unused)
{
	char spec[PATH_MAX];
	const char *script, *ext;
	int len;

	if (strcmp(str, "lang") == 0) {
		list_available_languages();
		exit(0);
	}

	script = strchr(str, ':');
	if (script) {
		len = script - str;
		if (len >= PATH_MAX) {
			fprintf(stderr, "invalid language specifier");
			return -1;
		}
		strncpy(spec, str, len);
		spec[len] = '\0';
		scripting_ops = script_spec__lookup(spec);
		if (!scripting_ops) {
			fprintf(stderr, "invalid language specifier");
			return -1;
		}
		script++;
	} else {
		script = str;
		ext = strrchr(script, '.');
		if (!ext) {
			fprintf(stderr, "invalid script extension");
			return -1;
		}
		scripting_ops = script_spec__lookup(++ext);
		if (!scripting_ops) {
			fprintf(stderr, "invalid script extension");
			return -1;
		}
	}

	script_name = strdup(script);

	return 0;
}

static int parse_output_fields(const struct option *opt __maybe_unused,
			    const char *arg, int unset __maybe_unused)
{
	char *tok, *strtok_saveptr = NULL;
	int i, imax = ARRAY_SIZE(all_output_options);
	int j;
	int rc = 0;
	char *str = strdup(arg);
	int type = -1;
	enum { DEFAULT, SET, ADD, REMOVE } change = DEFAULT;

	if (!str)
		return -ENOMEM;

	/* first word can state for which event type the user is specifying
	 * the fields. If no type exists, the specified fields apply to all
	 * event types found in the file minus the invalid fields for a type.
	 */
	tok = strchr(str, ':');
	if (tok) {
		*tok = '\0';
		tok++;
		if (!strcmp(str, "hw"))
			type = PERF_TYPE_HARDWARE;
		else if (!strcmp(str, "sw"))
			type = PERF_TYPE_SOFTWARE;
		else if (!strcmp(str, "trace"))
			type = PERF_TYPE_TRACEPOINT;
		else if (!strcmp(str, "raw"))
			type = PERF_TYPE_RAW;
		else if (!strcmp(str, "break"))
			type = PERF_TYPE_BREAKPOINT;
		else if (!strcmp(str, "synth"))
			type = OUTPUT_TYPE_SYNTH;
		else {
			fprintf(stderr, "Invalid event type in field string.\n");
			rc = -EINVAL;
			goto out;
		}

		if (output[type].user_set)
			pr_warning("Overriding previous field request for %s events.\n",
				   event_type(type));

		/* Don't override defaults for +- */
		if (strchr(tok, '+') || strchr(tok, '-'))
			goto parse;

		output[type].fields = 0;
		output[type].user_set = true;
		output[type].wildcard_set = false;

	} else {
		tok = str;
		if (strlen(str) == 0) {
			fprintf(stderr,
				"Cannot set fields to 'none' for all event types.\n");
			rc = -EINVAL;
			goto out;
		}

		/* Don't override defaults for +- */
		if (strchr(str, '+') || strchr(str, '-'))
			goto parse;

		if (output_set_by_user())
			pr_warning("Overriding previous field request for all events.\n");

		for (j = 0; j < OUTPUT_TYPE_MAX; ++j) {
			output[j].fields = 0;
			output[j].user_set = true;
			output[j].wildcard_set = true;
		}
	}

parse:
	for (tok = strtok_r(tok, ",", &strtok_saveptr); tok; tok = strtok_r(NULL, ",", &strtok_saveptr)) {
		if (*tok == '+') {
			if (change == SET)
				goto out_badmix;
			change = ADD;
			tok++;
		} else if (*tok == '-') {
			if (change == SET)
				goto out_badmix;
			change = REMOVE;
			tok++;
		} else {
			if (change != SET && change != DEFAULT)
				goto out_badmix;
			change = SET;
		}

		for (i = 0; i < imax; ++i) {
			if (strcmp(tok, all_output_options[i].str) == 0)
				break;
		}
		if (i == imax && strcmp(tok, "flags") == 0) {
			print_flags = change != REMOVE;
			continue;
		}
		if (i == imax) {
			fprintf(stderr, "Invalid field requested.\n");
			rc = -EINVAL;
			goto out;
		}

		if (type == -1) {
			/* add user option to all events types for
			 * which it is valid
			 */
			for (j = 0; j < OUTPUT_TYPE_MAX; ++j) {
				if (output[j].invalid_fields & all_output_options[i].field) {
					pr_warning("\'%s\' not valid for %s events. Ignoring.\n",
						   all_output_options[i].str, event_type(j));
				} else {
					if (change == REMOVE) {
						output[j].fields &= ~all_output_options[i].field;
						output[j].user_set_fields &= ~all_output_options[i].field;
						output[j].user_unset_fields |= all_output_options[i].field;
					} else {
						output[j].fields |= all_output_options[i].field;
						output[j].user_set_fields |= all_output_options[i].field;
						output[j].user_unset_fields &= ~all_output_options[i].field;
					}
					output[j].user_set = true;
					output[j].wildcard_set = true;
				}
			}
		} else {
			if (output[type].invalid_fields & all_output_options[i].field) {
				fprintf(stderr, "\'%s\' not valid for %s events.\n",
					 all_output_options[i].str, event_type(type));

				rc = -EINVAL;
				goto out;
			}
			if (change == REMOVE)
				output[type].fields &= ~all_output_options[i].field;
			else
				output[type].fields |= all_output_options[i].field;
			output[type].user_set = true;
			output[type].wildcard_set = true;
		}
	}

	if (type >= 0) {
		if (output[type].fields == 0) {
			pr_debug("No fields requested for %s type. "
				 "Events will not be displayed.\n", event_type(type));
		}
	}
	goto out;

out_badmix:
	fprintf(stderr, "Cannot mix +-field with overridden fields\n");
	rc = -EINVAL;
out:
	free(str);
	return rc;
}

#define for_each_lang(scripts_path, scripts_dir, lang_dirent)		\
	while ((lang_dirent = readdir(scripts_dir)) != NULL)		\
		if ((lang_dirent->d_type == DT_DIR ||			\
		     (lang_dirent->d_type == DT_UNKNOWN &&		\
		      is_directory(scripts_path, lang_dirent))) &&	\
		    (strcmp(lang_dirent->d_name, ".")) &&		\
		    (strcmp(lang_dirent->d_name, "..")))

#define for_each_script(lang_path, lang_dir, script_dirent)		\
	while ((script_dirent = readdir(lang_dir)) != NULL)		\
		if (script_dirent->d_type != DT_DIR &&			\
		    (script_dirent->d_type != DT_UNKNOWN ||		\
		     !is_directory(lang_path, script_dirent)))


#define RECORD_SUFFIX			"-record"
#define REPORT_SUFFIX			"-report"

struct script_desc {
	struct list_head	node;
	char			*name;
	char			*half_liner;
	char			*args;
};

static LIST_HEAD(script_descs);

static struct script_desc *script_desc__new(const char *name)
{
	struct script_desc *s = zalloc(sizeof(*s));

	if (s != NULL && name)
		s->name = strdup(name);

	return s;
}

static void script_desc__delete(struct script_desc *s)
{
	zfree(&s->name);
	zfree(&s->half_liner);
	zfree(&s->args);
	free(s);
}

static void script_desc__add(struct script_desc *s)
{
	list_add_tail(&s->node, &script_descs);
}

static struct script_desc *script_desc__find(const char *name)
{
	struct script_desc *s;

	list_for_each_entry(s, &script_descs, node)
		if (strcasecmp(s->name, name) == 0)
			return s;
	return NULL;
}

static struct script_desc *script_desc__findnew(const char *name)
{
	struct script_desc *s = script_desc__find(name);

	if (s)
		return s;

	s = script_desc__new(name);
	if (!s)
		return NULL;

	script_desc__add(s);

	return s;
}

static const char *ends_with(const char *str, const char *suffix)
{
	size_t suffix_len = strlen(suffix);
	const char *p = str;

	if (strlen(str) > suffix_len) {
		p = str + strlen(str) - suffix_len;
		if (!strncmp(p, suffix, suffix_len))
			return p;
	}

	return NULL;
}

static int read_script_info(struct script_desc *desc, const char *filename)
{
	char line[BUFSIZ], *p;
	FILE *fp;

	fp = fopen(filename, "r");
	if (!fp)
		return -1;

	while (fgets(line, sizeof(line), fp)) {
		p = skip_spaces(line);
		if (strlen(p) == 0)
			continue;
		if (*p != '#')
			continue;
		p++;
		if (strlen(p) && *p == '!')
			continue;

		p = skip_spaces(p);
		if (strlen(p) && p[strlen(p) - 1] == '\n')
			p[strlen(p) - 1] = '\0';

		if (!strncmp(p, "description:", strlen("description:"))) {
			p += strlen("description:");
			desc->half_liner = strdup(skip_spaces(p));
			continue;
		}

		if (!strncmp(p, "args:", strlen("args:"))) {
			p += strlen("args:");
			desc->args = strdup(skip_spaces(p));
			continue;
		}
	}

	fclose(fp);

	return 0;
}

static char *get_script_root(struct dirent *script_dirent, const char *suffix)
{
	char *script_root, *str;

	script_root = strdup(script_dirent->d_name);
	if (!script_root)
		return NULL;

	str = (char *)ends_with(script_root, suffix);
	if (!str) {
		free(script_root);
		return NULL;
	}

	*str = '\0';
	return script_root;
}

static int list_available_scripts(const struct option *opt __maybe_unused,
				  const char *s __maybe_unused,
				  int unset __maybe_unused)
{
	struct dirent *script_dirent, *lang_dirent;
	char scripts_path[MAXPATHLEN];
	DIR *scripts_dir, *lang_dir;
	char script_path[MAXPATHLEN];
	char lang_path[MAXPATHLEN];
	struct script_desc *desc;
	char first_half[BUFSIZ];
	char *script_root;

	snprintf(scripts_path, MAXPATHLEN, "%s/scripts", get_argv_exec_path());

	scripts_dir = opendir(scripts_path);
	if (!scripts_dir) {
		fprintf(stdout,
			"open(%s) failed.\n"
			"Check \"PERF_EXEC_PATH\" env to set scripts dir.\n",
			scripts_path);
		exit(-1);
	}

	for_each_lang(scripts_path, scripts_dir, lang_dirent) {
		scnprintf(lang_path, MAXPATHLEN, "%s/%s/bin", scripts_path,
			  lang_dirent->d_name);
		lang_dir = opendir(lang_path);
		if (!lang_dir)
			continue;

		for_each_script(lang_path, lang_dir, script_dirent) {
			script_root = get_script_root(script_dirent, REPORT_SUFFIX);
			if (script_root) {
				desc = script_desc__findnew(script_root);
				scnprintf(script_path, MAXPATHLEN, "%s/%s",
					  lang_path, script_dirent->d_name);
				read_script_info(desc, script_path);
				free(script_root);
			}
		}
	}

	fprintf(stdout, "List of available trace scripts:\n");
	list_for_each_entry(desc, &script_descs, node) {
		sprintf(first_half, "%s %s", desc->name,
			desc->args ? desc->args : "");
		fprintf(stdout, "  %-36s %s\n", first_half,
			desc->half_liner ? desc->half_liner : "");
	}

	exit(0);
}

/*
 * Some scripts specify the required events in their "xxx-record" file,
 * this function will check if the events in perf.data match those
 * mentioned in the "xxx-record".
 *
 * Fixme: All existing "xxx-record" are all in good formats "-e event ",
 * which is covered well now. And new parsing code should be added to
 * cover the future complex formats like event groups etc.
 */
static int check_ev_match(char *dir_name, char *scriptname,
			struct perf_session *session)
{
	char filename[MAXPATHLEN], evname[128];
	char line[BUFSIZ], *p;
	struct evsel *pos;
	int match, len;
	FILE *fp;

	scnprintf(filename, MAXPATHLEN, "%s/bin/%s-record", dir_name, scriptname);

	fp = fopen(filename, "r");
	if (!fp)
		return -1;

	while (fgets(line, sizeof(line), fp)) {
		p = skip_spaces(line);
		if (*p == '#')
			continue;

		while (strlen(p)) {
			p = strstr(p, "-e");
			if (!p)
				break;

			p += 2;
			p = skip_spaces(p);
			len = strcspn(p, " \t");
			if (!len)
				break;

			snprintf(evname, len + 1, "%s", p);

			match = 0;
			evlist__for_each_entry(session->evlist, pos) {
				if (!strcmp(evsel__name(pos), evname)) {
					match = 1;
					break;
				}
			}

			if (!match) {
				fclose(fp);
				return -1;
			}
		}
	}

	fclose(fp);
	return 0;
}

/*
 * Return -1 if none is found, otherwise the actual scripts number.
 *
 * Currently the only user of this function is the script browser, which
 * will list all statically runnable scripts, select one, execute it and
 * show the output in a perf browser.
 */
int find_scripts(char **scripts_array, char **scripts_path_array, int num,
		 int pathlen)
{
	struct dirent *script_dirent, *lang_dirent;
	char scripts_path[MAXPATHLEN], lang_path[MAXPATHLEN];
	DIR *scripts_dir, *lang_dir;
	struct perf_session *session;
	struct perf_data data = {
		.path = input_name,
		.mode = PERF_DATA_MODE_READ,
	};
	char *temp;
	int i = 0;

	session = perf_session__new(&data, false, NULL);
	if (IS_ERR(session))
		return PTR_ERR(session);

	snprintf(scripts_path, MAXPATHLEN, "%s/scripts", get_argv_exec_path());

	scripts_dir = opendir(scripts_path);
	if (!scripts_dir) {
		perf_session__delete(session);
		return -1;
	}

	for_each_lang(scripts_path, scripts_dir, lang_dirent) {
		scnprintf(lang_path, MAXPATHLEN, "%s/%s", scripts_path,
			  lang_dirent->d_name);
#ifndef HAVE_LIBPERL_SUPPORT
		if (strstr(lang_path, "perl"))
			continue;
#endif
#ifndef HAVE_LIBPYTHON_SUPPORT
		if (strstr(lang_path, "python"))
			continue;
#endif

		lang_dir = opendir(lang_path);
		if (!lang_dir)
			continue;

		for_each_script(lang_path, lang_dir, script_dirent) {
			/* Skip those real time scripts: xxxtop.p[yl] */
			if (strstr(script_dirent->d_name, "top."))
				continue;
			if (i >= num)
				break;
			snprintf(scripts_path_array[i], pathlen, "%s/%s",
				lang_path,
				script_dirent->d_name);
			temp = strchr(script_dirent->d_name, '.');
			snprintf(scripts_array[i],
				(temp - script_dirent->d_name) + 1,
				"%s", script_dirent->d_name);

			if (check_ev_match(lang_path,
					scripts_array[i], session))
				continue;

			i++;
		}
		closedir(lang_dir);
	}

	closedir(scripts_dir);
	perf_session__delete(session);
	return i;
}

static char *get_script_path(const char *script_root, const char *suffix)
{
	struct dirent *script_dirent, *lang_dirent;
	char scripts_path[MAXPATHLEN];
	char script_path[MAXPATHLEN];
	DIR *scripts_dir, *lang_dir;
	char lang_path[MAXPATHLEN];
	char *__script_root;

	snprintf(scripts_path, MAXPATHLEN, "%s/scripts", get_argv_exec_path());

	scripts_dir = opendir(scripts_path);
	if (!scripts_dir)
		return NULL;

	for_each_lang(scripts_path, scripts_dir, lang_dirent) {
		scnprintf(lang_path, MAXPATHLEN, "%s/%s/bin", scripts_path,
			  lang_dirent->d_name);
		lang_dir = opendir(lang_path);
		if (!lang_dir)
			continue;

		for_each_script(lang_path, lang_dir, script_dirent) {
			__script_root = get_script_root(script_dirent, suffix);
			if (__script_root && !strcmp(script_root, __script_root)) {
				free(__script_root);
				closedir(scripts_dir);
				scnprintf(script_path, MAXPATHLEN, "%s/%s",
					  lang_path, script_dirent->d_name);
				closedir(lang_dir);
				return strdup(script_path);
			}
			free(__script_root);
		}
		closedir(lang_dir);
	}
	closedir(scripts_dir);

	return NULL;
}

static bool is_top_script(const char *script_path)
{
	return ends_with(script_path, "top") != NULL;
}

static int has_required_arg(char *script_path)
{
	struct script_desc *desc;
	int n_args = 0;
	char *p;

	desc = script_desc__new(NULL);

	if (read_script_info(desc, script_path))
		goto out;

	if (!desc->args)
		goto out;

	for (p = desc->args; *p; p++)
		if (*p == '<')
			n_args++;
out:
	script_desc__delete(desc);

	return n_args;
}

static int have_cmd(int argc, const char **argv)
{
	char **__argv = malloc(sizeof(const char *) * argc);

	if (!__argv) {
		pr_err("malloc failed\n");
		return -1;
	}

	memcpy(__argv, argv, sizeof(const char *) * argc);
	argc = parse_options(argc, (const char **)__argv, record_options,
			     NULL, PARSE_OPT_STOP_AT_NON_OPTION);
	free(__argv);

	system_wide = (argc == 0);

	return 0;
}

static void script__setup_sample_type(struct perf_script *script)
{
	struct perf_session *session = script->session;
	u64 sample_type = evlist__combined_sample_type(session->evlist);

	if (symbol_conf.use_callchain || symbol_conf.cumulate_callchain) {
		if ((sample_type & PERF_SAMPLE_REGS_USER) &&
		    (sample_type & PERF_SAMPLE_STACK_USER)) {
			callchain_param.record_mode = CALLCHAIN_DWARF;
			dwarf_callchain_users = true;
		} else if (sample_type & PERF_SAMPLE_BRANCH_STACK)
			callchain_param.record_mode = CALLCHAIN_LBR;
		else
			callchain_param.record_mode = CALLCHAIN_FP;
	}

	if (script->stitch_lbr && (callchain_param.record_mode != CALLCHAIN_LBR)) {
		pr_warning("Can't find LBR callchain. Switch off --stitch-lbr.\n"
			   "Please apply --call-graph lbr when recording.\n");
		script->stitch_lbr = false;
	}
}

static int process_stat_round_event(struct perf_session *session,
				    union perf_event *event)
{
	struct perf_record_stat_round *round = &event->stat_round;
	struct evsel *counter;

	evlist__for_each_entry(session->evlist, counter) {
		perf_stat_process_counter(&stat_config, counter);
		process_stat(counter, round->time);
	}

	process_stat_interval(round->time);
	return 0;
}

static int process_stat_config_event(struct perf_session *session __maybe_unused,
				     union perf_event *event)
{
	perf_event__read_stat_config(&stat_config, &event->stat_config);
	return 0;
}

static int set_maps(struct perf_script *script)
{
	struct evlist *evlist = script->session->evlist;

	if (!script->cpus || !script->threads)
		return 0;

	if (WARN_ONCE(script->allocated, "stats double allocation\n"))
		return -EINVAL;

	perf_evlist__set_maps(&evlist->core, script->cpus, script->threads);

	if (evlist__alloc_stats(evlist, true))
		return -ENOMEM;

	script->allocated = true;
	return 0;
}

static
int process_thread_map_event(struct perf_session *session,
			     union perf_event *event)
{
	struct perf_tool *tool = session->tool;
	struct perf_script *script = container_of(tool, struct perf_script, tool);

	if (script->threads) {
		pr_warning("Extra thread map event, ignoring.\n");
		return 0;
	}

	script->threads = thread_map__new_event(&event->thread_map);
	if (!script->threads)
		return -ENOMEM;

	return set_maps(script);
}

static
int process_cpu_map_event(struct perf_session *session,
			  union perf_event *event)
{
	struct perf_tool *tool = session->tool;
	struct perf_script *script = container_of(tool, struct perf_script, tool);

	if (script->cpus) {
		pr_warning("Extra cpu map event, ignoring.\n");
		return 0;
	}

	script->cpus = cpu_map__new_data(&event->cpu_map.data);
	if (!script->cpus)
		return -ENOMEM;

	return set_maps(script);
}

static int process_feature_event(struct perf_session *session,
				 union perf_event *event)
{
	if (event->feat.feat_id < HEADER_LAST_FEATURE)
		return perf_event__process_feature(session, event);
	return 0;
}

#ifdef HAVE_AUXTRACE_SUPPORT
static int perf_script__process_auxtrace_info(struct perf_session *session,
					      union perf_event *event)
{
	struct perf_tool *tool = session->tool;

	int ret = perf_event__process_auxtrace_info(session, event);

	if (ret == 0) {
		struct perf_script *script = container_of(tool, struct perf_script, tool);

		ret = perf_script__setup_per_event_dump(script);
	}

	return ret;
}
#else
#define perf_script__process_auxtrace_info 0
#endif

static int parse_insn_trace(const struct option *opt __maybe_unused,
			    const char *str __maybe_unused,
			    int unset __maybe_unused)
{
	parse_output_fields(NULL, "+insn,-event,-period", 0);
	itrace_parse_synth_opts(opt, "i0ns", 0);
	symbol_conf.nanosecs = true;
	return 0;
}

static int parse_xed(const struct option *opt __maybe_unused,
		     const char *str __maybe_unused,
		     int unset __maybe_unused)
{
	if (isatty(1))
		force_pager("xed -F insn: -A -64 | less");
	else
		force_pager("xed -F insn: -A -64");
	return 0;
}

static int parse_call_trace(const struct option *opt __maybe_unused,
			    const char *str __maybe_unused,
			    int unset __maybe_unused)
{
	parse_output_fields(NULL, "-ip,-addr,-event,-period,+callindent", 0);
	itrace_parse_synth_opts(opt, "cewp", 0);
	symbol_conf.nanosecs = true;
	symbol_conf.pad_output_len_dso = 50;
	return 0;
}

static int parse_callret_trace(const struct option *opt __maybe_unused,
			    const char *str __maybe_unused,
			    int unset __maybe_unused)
{
	parse_output_fields(NULL, "-ip,-addr,-event,-period,+callindent,+flags", 0);
	itrace_parse_synth_opts(opt, "crewp", 0);
	symbol_conf.nanosecs = true;
	return 0;
}

int cmd_script(int argc, const char **argv)
{
	bool show_full_info = false;
	bool header = false;
	bool header_only = false;
	bool script_started = false;
	char *rec_script_path = NULL;
	char *rep_script_path = NULL;
	struct perf_session *session;
	struct itrace_synth_opts itrace_synth_opts = {
		.set = false,
		.default_no_sample = true,
	};
	struct utsname uts;
	char *script_path = NULL;
	const char **__argv;
	int i, j, err = 0;
	struct perf_script script = {
		.tool = {
			.sample		 = process_sample_event,
			.mmap		 = perf_event__process_mmap,
			.mmap2		 = perf_event__process_mmap2,
			.comm		 = perf_event__process_comm,
			.namespaces	 = perf_event__process_namespaces,
			.cgroup		 = perf_event__process_cgroup,
			.exit		 = perf_event__process_exit,
			.fork		 = perf_event__process_fork,
			.attr		 = process_attr,
			.event_update   = perf_event__process_event_update,
			.tracing_data	 = perf_event__process_tracing_data,
			.feature	 = process_feature_event,
			.build_id	 = perf_event__process_build_id,
			.id_index	 = perf_event__process_id_index,
			.auxtrace_info	 = perf_script__process_auxtrace_info,
			.auxtrace	 = perf_event__process_auxtrace,
			.auxtrace_error	 = perf_event__process_auxtrace_error,
			.stat		 = perf_event__process_stat_event,
			.stat_round	 = process_stat_round_event,
			.stat_config	 = process_stat_config_event,
			.thread_map	 = process_thread_map_event,
			.cpu_map	 = process_cpu_map_event,
			.ordered_events	 = true,
			.ordering_requires_timestamps = true,
		},
	};
	struct perf_data data = {
		.mode = PERF_DATA_MODE_READ,
	};
	const struct option options[] = {
	OPT_BOOLEAN('D', "dump-raw-trace", &dump_trace,
		    "dump raw trace in ASCII"),
	OPT_INCR('v', "verbose", &verbose,
		 "be more verbose (show symbol address, etc)"),
	OPT_BOOLEAN('L', "Latency", &latency_format,
		    "show latency attributes (irqs/preemption disabled, etc)"),
	OPT_CALLBACK_NOOPT('l', "list", NULL, NULL, "list available scripts",
			   list_available_scripts),
	OPT_CALLBACK('s', "script", NULL, "name",
		     "script file name (lang:script name, script name, or *)",
		     parse_scriptname),
	OPT_STRING('g', "gen-script", &generate_script_lang, "lang",
		   "generate perf-script.xx script in specified language"),
	OPT_STRING('i', "input", &input_name, "file", "input file name"),
	OPT_BOOLEAN('d', "debug-mode", &debug_mode,
		   "do various checks like samples ordering and lost events"),
	OPT_BOOLEAN(0, "header", &header, "Show data header."),
	OPT_BOOLEAN(0, "header-only", &header_only, "Show only data header."),
	OPT_STRING('k', "vmlinux", &symbol_conf.vmlinux_name,
		   "file", "vmlinux pathname"),
	OPT_STRING(0, "kallsyms", &symbol_conf.kallsyms_name,
		   "file", "kallsyms pathname"),
	OPT_BOOLEAN('G', "hide-call-graph", &no_callchain,
		    "When printing symbols do not display call chain"),
	OPT_CALLBACK(0, "symfs", NULL, "directory",
		     "Look for files with symbols relative to this directory",
		     symbol__config_symfs),
	OPT_CALLBACK('F', "fields", NULL, "str",
		     "comma separated output fields prepend with 'type:'. "
		     "+field to add and -field to remove."
		     "Valid types: hw,sw,trace,raw,synth. "
		     "Fields: comm,tid,pid,time,cpu,event,trace,ip,sym,dso,"
		     "addr,symoff,srcline,period,iregs,uregs,brstack,"
		     "brstacksym,flags,bpf-output,brstackinsn,brstackoff,"
		     "callindent,insn,insnlen,synth,phys_addr,metric,misc,ipc,tod,"
		     "data_page_size,code_page_size",
		     parse_output_fields),
	OPT_BOOLEAN('a', "all-cpus", &system_wide,
		    "system-wide collection from all CPUs"),
	OPT_STRING(0, "dsos", &symbol_conf.dso_list_str, "dso[,dso...]",
		   "only consider symbols in these DSOs"),
	OPT_STRING('S', "symbols", &symbol_conf.sym_list_str, "symbol[,symbol...]",
		   "only consider these symbols"),
	OPT_INTEGER(0, "addr-range", &symbol_conf.addr_range,
		    "Use with -S to list traced records within address range"),
	OPT_CALLBACK_OPTARG(0, "insn-trace", &itrace_synth_opts, NULL, NULL,
			"Decode instructions from itrace", parse_insn_trace),
	OPT_CALLBACK_OPTARG(0, "xed", NULL, NULL, NULL,
			"Run xed disassembler on output", parse_xed),
	OPT_CALLBACK_OPTARG(0, "call-trace", &itrace_synth_opts, NULL, NULL,
			"Decode calls from from itrace", parse_call_trace),
	OPT_CALLBACK_OPTARG(0, "call-ret-trace", &itrace_synth_opts, NULL, NULL,
			"Decode calls and returns from itrace", parse_callret_trace),
	OPT_STRING(0, "graph-function", &symbol_conf.graph_function, "symbol[,symbol...]",
			"Only print symbols and callees with --call-trace/--call-ret-trace"),
	OPT_STRING(0, "stop-bt", &symbol_conf.bt_stop_list_str, "symbol[,symbol...]",
		   "Stop display of callgraph at these symbols"),
	OPT_STRING('C', "cpu", &cpu_list, "cpu", "list of cpus to profile"),
	OPT_STRING('c', "comms", &symbol_conf.comm_list_str, "comm[,comm...]",
		   "only display events for these comms"),
	OPT_STRING(0, "pid", &symbol_conf.pid_list_str, "pid[,pid...]",
		   "only consider symbols in these pids"),
	OPT_STRING(0, "tid", &symbol_conf.tid_list_str, "tid[,tid...]",
		   "only consider symbols in these tids"),
	OPT_UINTEGER(0, "max-stack", &scripting_max_stack,
		     "Set the maximum stack depth when parsing the callchain, "
		     "anything beyond the specified depth will be ignored. "
		     "Default: kernel.perf_event_max_stack or " __stringify(PERF_MAX_STACK_DEPTH)),
	OPT_BOOLEAN(0, "reltime", &reltime, "Show time stamps relative to start"),
	OPT_BOOLEAN(0, "deltatime", &deltatime, "Show time stamps relative to previous event"),
	OPT_BOOLEAN('I', "show-info", &show_full_info,
		    "display extended information from perf.data file"),
	OPT_BOOLEAN('\0', "show-kernel-path", &symbol_conf.show_kernel_path,
		    "Show the path of [kernel.kallsyms]"),
	OPT_BOOLEAN('\0', "show-task-events", &script.show_task_events,
		    "Show the fork/comm/exit events"),
	OPT_BOOLEAN('\0', "show-mmap-events", &script.show_mmap_events,
		    "Show the mmap events"),
	OPT_BOOLEAN('\0', "show-switch-events", &script.show_switch_events,
		    "Show context switch events (if recorded)"),
	OPT_BOOLEAN('\0', "show-namespace-events", &script.show_namespace_events,
		    "Show namespace events (if recorded)"),
	OPT_BOOLEAN('\0', "show-cgroup-events", &script.show_cgroup_events,
		    "Show cgroup events (if recorded)"),
	OPT_BOOLEAN('\0', "show-lost-events", &script.show_lost_events,
		    "Show lost events (if recorded)"),
	OPT_BOOLEAN('\0', "show-round-events", &script.show_round_events,
		    "Show round events (if recorded)"),
	OPT_BOOLEAN('\0', "show-bpf-events", &script.show_bpf_events,
		    "Show bpf related events (if recorded)"),
	OPT_BOOLEAN('\0', "show-text-poke-events", &script.show_text_poke_events,
		    "Show text poke related events (if recorded)"),
	OPT_BOOLEAN('\0', "per-event-dump", &script.per_event_dump,
		    "Dump trace output to files named by the monitored events"),
	OPT_BOOLEAN('f', "force", &symbol_conf.force, "don't complain, do it"),
	OPT_INTEGER(0, "max-blocks", &max_blocks,
		    "Maximum number of code blocks to dump with brstackinsn"),
	OPT_BOOLEAN(0, "ns", &symbol_conf.nanosecs,
		    "Use 9 decimal places when displaying time"),
	OPT_CALLBACK_OPTARG(0, "itrace", &itrace_synth_opts, NULL, "opts",
			    "Instruction Tracing options\n" ITRACE_HELP,
			    itrace_parse_synth_opts),
	OPT_BOOLEAN(0, "full-source-path", &srcline_full_filename,
			"Show full source file name path for source lines"),
	OPT_BOOLEAN(0, "demangle", &symbol_conf.demangle,
			"Enable symbol demangling"),
	OPT_BOOLEAN(0, "demangle-kernel", &symbol_conf.demangle_kernel,
			"Enable kernel symbol demangling"),
	OPT_STRING(0, "time", &script.time_str, "str",
		   "Time span of interest (start,stop)"),
	OPT_BOOLEAN(0, "inline", &symbol_conf.inline_name,
		    "Show inline function"),
	OPT_STRING(0, "guestmount", &symbol_conf.guestmount, "directory",
		   "guest mount directory under which every guest os"
		   " instance has a subdir"),
	OPT_STRING(0, "guestvmlinux", &symbol_conf.default_guest_vmlinux_name,
		   "file", "file saving guest os vmlinux"),
	OPT_STRING(0, "guestkallsyms", &symbol_conf.default_guest_kallsyms,
		   "file", "file saving guest os /proc/kallsyms"),
	OPT_STRING(0, "guestmodules", &symbol_conf.default_guest_modules,
		   "file", "file saving guest os /proc/modules"),
	OPT_BOOLEAN('\0', "stitch-lbr", &script.stitch_lbr,
		    "Enable LBR callgraph stitching approach"),
	OPTS_EVSWITCH(&script.evswitch),
	OPT_END()
	};
	const char * const script_subcommands[] = { "record", "report", NULL };
	const char *script_usage[] = {
		"perf script [<options>]",
		"perf script [<options>] record <script> [<record-options>] <command>",
		"perf script [<options>] report <script> [script-args]",
		"perf script [<options>] <script> [<record-options>] <command>",
		"perf script [<options>] <top-script> [script-args]",
		NULL
	};

	perf_set_singlethreaded();

	setup_scripting();

	argc = parse_options_subcommand(argc, argv, options, script_subcommands, script_usage,
			     PARSE_OPT_STOP_AT_NON_OPTION);

	if (symbol_conf.guestmount ||
	    symbol_conf.default_guest_vmlinux_name ||
	    symbol_conf.default_guest_kallsyms ||
	    symbol_conf.default_guest_modules) {
		/*
		 * Enable guest sample processing.
		 */
		perf_guest = true;
	}

	data.path  = input_name;
	data.force = symbol_conf.force;

	if (argc > 1 && !strncmp(argv[0], "rec", strlen("rec"))) {
		rec_script_path = get_script_path(argv[1], RECORD_SUFFIX);
		if (!rec_script_path)
			return cmd_record(argc, argv);
	}

	if (argc > 1 && !strncmp(argv[0], "rep", strlen("rep"))) {
		rep_script_path = get_script_path(argv[1], REPORT_SUFFIX);
		if (!rep_script_path) {
			fprintf(stderr,
				"Please specify a valid report script"
				"(see 'perf script -l' for listing)\n");
			return -1;
		}
	}

	if (reltime && deltatime) {
		fprintf(stderr,
			"reltime and deltatime - the two don't get along well. "
			"Please limit to --reltime or --deltatime.\n");
		return -1;
	}

	if ((itrace_synth_opts.callchain || itrace_synth_opts.add_callchain) &&
	    itrace_synth_opts.callchain_sz > scripting_max_stack)
		scripting_max_stack = itrace_synth_opts.callchain_sz;

	/* make sure PERF_EXEC_PATH is set for scripts */
	set_argv_exec_path(get_argv_exec_path());

	if (argc && !script_name && !rec_script_path && !rep_script_path) {
		int live_pipe[2];
		int rep_args;
		pid_t pid;

		rec_script_path = get_script_path(argv[0], RECORD_SUFFIX);
		rep_script_path = get_script_path(argv[0], REPORT_SUFFIX);

		if (!rec_script_path && !rep_script_path) {
			usage_with_options_msg(script_usage, options,
				"Couldn't find script `%s'\n\n See perf"
				" script -l for available scripts.\n", argv[0]);
		}

		if (is_top_script(argv[0])) {
			rep_args = argc - 1;
		} else {
			int rec_args;

			rep_args = has_required_arg(rep_script_path);
			rec_args = (argc - 1) - rep_args;
			if (rec_args < 0) {
				usage_with_options_msg(script_usage, options,
					"`%s' script requires options."
					"\n\n See perf script -l for available "
					"scripts and options.\n", argv[0]);
			}
		}

		if (pipe(live_pipe) < 0) {
			perror("failed to create pipe");
			return -1;
		}

		pid = fork();
		if (pid < 0) {
			perror("failed to fork");
			return -1;
		}

		if (!pid) {
			j = 0;

			dup2(live_pipe[1], 1);
			close(live_pipe[0]);

			if (is_top_script(argv[0])) {
				system_wide = true;
			} else if (!system_wide) {
				if (have_cmd(argc - rep_args, &argv[rep_args]) != 0) {
					err = -1;
					goto out;
				}
			}

			__argv = malloc((argc + 6) * sizeof(const char *));
			if (!__argv) {
				pr_err("malloc failed\n");
				err = -ENOMEM;
				goto out;
			}

			__argv[j++] = "/bin/sh";
			__argv[j++] = rec_script_path;
			if (system_wide)
				__argv[j++] = "-a";
			__argv[j++] = "-q";
			__argv[j++] = "-o";
			__argv[j++] = "-";
			for (i = rep_args + 1; i < argc; i++)
				__argv[j++] = argv[i];
			__argv[j++] = NULL;

			execvp("/bin/sh", (char **)__argv);
			free(__argv);
			exit(-1);
		}

		dup2(live_pipe[0], 0);
		close(live_pipe[1]);

		__argv = malloc((argc + 4) * sizeof(const char *));
		if (!__argv) {
			pr_err("malloc failed\n");
			err = -ENOMEM;
			goto out;
		}

		j = 0;
		__argv[j++] = "/bin/sh";
		__argv[j++] = rep_script_path;
		for (i = 1; i < rep_args + 1; i++)
			__argv[j++] = argv[i];
		__argv[j++] = "-i";
		__argv[j++] = "-";
		__argv[j++] = NULL;

		execvp("/bin/sh", (char **)__argv);
		free(__argv);
		exit(-1);
	}

	if (rec_script_path)
		script_path = rec_script_path;
	if (rep_script_path)
		script_path = rep_script_path;

	if (script_path) {
		j = 0;

		if (!rec_script_path)
			system_wide = false;
		else if (!system_wide) {
			if (have_cmd(argc - 1, &argv[1]) != 0) {
				err = -1;
				goto out;
			}
		}

		__argv = malloc((argc + 2) * sizeof(const char *));
		if (!__argv) {
			pr_err("malloc failed\n");
			err = -ENOMEM;
			goto out;
		}

		__argv[j++] = "/bin/sh";
		__argv[j++] = script_path;
		if (system_wide)
			__argv[j++] = "-a";
		for (i = 2; i < argc; i++)
			__argv[j++] = argv[i];
		__argv[j++] = NULL;

		execvp("/bin/sh", (char **)__argv);
		free(__argv);
		exit(-1);
	}

	if (!script_name) {
		setup_pager();
		use_browser = 0;
	}

	session = perf_session__new(&data, false, &script.tool);
	if (IS_ERR(session))
		return PTR_ERR(session);

	if (header || header_only) {
		script.tool.show_feat_hdr = SHOW_FEAT_HEADER;
		perf_session__fprintf_info(session, stdout, show_full_info);
		if (header_only)
			goto out_delete;
	}
	if (show_full_info)
		script.tool.show_feat_hdr = SHOW_FEAT_HEADER_FULL_INFO;

	if (symbol__init(&session->header.env) < 0)
		goto out_delete;

	uname(&uts);
	if (data.is_pipe ||  /* assume pipe_mode indicates native_arch */
	    !strcmp(uts.machine, session->header.env.arch) ||
	    (!strcmp(uts.machine, "x86_64") &&
	     !strcmp(session->header.env.arch, "i386")))
		native_arch = true;

	script.session = session;
	script__setup_sample_type(&script);

	if ((output[PERF_TYPE_HARDWARE].fields & PERF_OUTPUT_CALLINDENT) ||
	    symbol_conf.graph_function)
		itrace_synth_opts.thread_stack = true;

	session->itrace_synth_opts = &itrace_synth_opts;

	if (cpu_list) {
		err = perf_session__cpu_bitmap(session, cpu_list, cpu_bitmap);
		if (err < 0)
			goto out_delete;
		itrace_synth_opts.cpu_bitmap = cpu_bitmap;
	}

	if (!no_callchain)
		symbol_conf.use_callchain = true;
	else
		symbol_conf.use_callchain = false;

	if (session->tevent.pevent &&
	    tep_set_function_resolver(session->tevent.pevent,
				      machine__resolve_kernel_addr,
				      &session->machines.host) < 0) {
		pr_err("%s: failed to set libtraceevent function resolver\n", __func__);
		err = -1;
		goto out_delete;
	}

	if (generate_script_lang) {
		struct stat perf_stat;
		int input;

		if (output_set_by_user()) {
			fprintf(stderr,
				"custom fields not supported for generated scripts");
			err = -EINVAL;
			goto out_delete;
		}

		input = open(data.path, O_RDONLY);	/* input_name */
		if (input < 0) {
			err = -errno;
			perror("failed to open file");
			goto out_delete;
		}

		err = fstat(input, &perf_stat);
		if (err < 0) {
			perror("failed to stat file");
			goto out_delete;
		}

		if (!perf_stat.st_size) {
			fprintf(stderr, "zero-sized file, nothing to do!\n");
			goto out_delete;
		}

		scripting_ops = script_spec__lookup(generate_script_lang);
		if (!scripting_ops) {
			fprintf(stderr, "invalid language specifier");
			err = -ENOENT;
			goto out_delete;
		}

		err = scripting_ops->generate_script(session->tevent.pevent,
						     "perf-script");
		goto out_delete;
	}

	if (script_name) {
		err = scripting_ops->start_script(script_name, argc, argv);
		if (err)
			goto out_delete;
		pr_debug("perf script started with script %s\n\n", script_name);
		script_started = true;
	}


	err = perf_session__check_output_opt(session);
	if (err < 0)
		goto out_delete;

	if (script.time_str) {
		err = perf_time__parse_for_ranges_reltime(script.time_str, session,
						  &script.ptime_range,
						  &script.range_size,
						  &script.range_num,
						  reltime);
		if (err < 0)
			goto out_delete;

		itrace_synth_opts__set_time_range(&itrace_synth_opts,
						  script.ptime_range,
						  script.range_num);
	}

	err = evswitch__init(&script.evswitch, session->evlist, stderr);
	if (err)
		goto out_delete;

	if (zstd_init(&(session->zstd_data), 0) < 0)
		pr_warning("Decompression initialization failed. Reported data may be incomplete.\n");

	err = __cmd_script(&script);

	flush_scripting();

out_delete:
	if (script.ptime_range) {
		itrace_synth_opts__clear_time_range(&itrace_synth_opts);
		zfree(&script.ptime_range);
	}

	evlist__free_stats(session->evlist);
	perf_session__delete(session);

	if (script_started)
		cleanup_scripting();
out:
	return err;
}

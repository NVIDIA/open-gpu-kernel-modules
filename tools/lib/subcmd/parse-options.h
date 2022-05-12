/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __SUBCMD_PARSE_OPTIONS_H
#define __SUBCMD_PARSE_OPTIONS_H

#include <linux/kernel.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef NORETURN
#define NORETURN __attribute__((__noreturn__))
#endif

enum parse_opt_type {
	/* special types */
	OPTION_END,
	OPTION_ARGUMENT,
	OPTION_GROUP,
	/* options with no arguments */
	OPTION_BIT,
	OPTION_BOOLEAN,
	OPTION_INCR,
	OPTION_SET_UINT,
	OPTION_SET_PTR,
	/* options with arguments (usually) */
	OPTION_STRING,
	OPTION_INTEGER,
	OPTION_LONG,
	OPTION_ULONG,
	OPTION_CALLBACK,
	OPTION_U64,
	OPTION_UINTEGER,
};

enum parse_opt_flags {
	PARSE_OPT_KEEP_DASHDASH = 1,
	PARSE_OPT_STOP_AT_NON_OPTION = 2,
	PARSE_OPT_KEEP_ARGV0 = 4,
	PARSE_OPT_KEEP_UNKNOWN = 8,
	PARSE_OPT_NO_INTERNAL_HELP = 16,
};

enum parse_opt_option_flags {
	PARSE_OPT_OPTARG  = 1,
	PARSE_OPT_NOARG   = 2,
	PARSE_OPT_NONEG   = 4,
	PARSE_OPT_HIDDEN  = 8,
	PARSE_OPT_LASTARG_DEFAULT = 16,
	PARSE_OPT_DISABLED = 32,
	PARSE_OPT_EXCLUSIVE = 64,
	PARSE_OPT_NOEMPTY  = 128,
	PARSE_OPT_NOBUILD  = 256,
	PARSE_OPT_CANSKIP  = 512,
};

struct option;
typedef int parse_opt_cb(const struct option *, const char *arg, int unset);

/*
 * `type`::
 *   holds the type of the option, you must have an OPTION_END last in your
 *   array.
 *
 * `short_name`::
 *   the character to use as a short option name, '\0' if none.
 *
 * `long_name`::
 *   the long option name, without the leading dashes, NULL if none.
 *
 * `value`::
 *   stores pointers to the values to be filled.
 *
 * `argh`::
 *   token to explain the kind of argument this option wants. Keep it
 *   homogeneous across the repository.
 *
 * `help`::
 *   the short help associated to what the option does.
 *   Must never be NULL (except for OPTION_END).
 *   OPTION_GROUP uses this pointer to store the group header.
 *
 * `flags`::
 *   mask of parse_opt_option_flags.
 *   PARSE_OPT_OPTARG: says that the argument is optional (not for BOOLEANs)
 *   PARSE_OPT_NOARG: says that this option takes no argument, for CALLBACKs
 *   PARSE_OPT_NONEG: says that this option cannot be negated
 *   PARSE_OPT_HIDDEN this option is skipped in the default usage, showed in
 *                    the long one.
 *
 * `callback`::
 *   pointer to the callback to use for OPTION_CALLBACK.
 *
 * `defval`::
 *   default value to fill (*->value) with for PARSE_OPT_OPTARG.
 *   OPTION_{BIT,SET_UINT,SET_PTR} store the {mask,integer,pointer} to put in
 *   the value when met.
 *   CALLBACKS can use it like they want.
 *
 * `set`::
 *   whether an option was set by the user
 */
struct option {
	enum parse_opt_type type;
	int short_name;
	const char *long_name;
	void *value;
	const char *argh;
	const char *help;
	const char *build_opt;

	int flags;
	parse_opt_cb *callback;
	intptr_t defval;
	bool *set;
	void *data;
	const struct option *parent;
};

#define check_vtype(v, type) ( BUILD_BUG_ON_ZERO(!__builtin_types_compatible_p(typeof(v), type)) + v )

#define OPT_END()                   { .type = OPTION_END }
#define OPT_PARENT(p)               { .type = OPTION_END, .parent = (p) }
#define OPT_ARGUMENT(l, h)          { .type = OPTION_ARGUMENT, .long_name = (l), .help = (h) }
#define OPT_GROUP(h)                { .type = OPTION_GROUP, .help = (h) }
#define OPT_BIT(s, l, v, h, b)      { .type = OPTION_BIT, .short_name = (s), .long_name = (l), .value = check_vtype(v, int *), .help = (h), .defval = (b) }
#define OPT_BOOLEAN(s, l, v, h)     { .type = OPTION_BOOLEAN, .short_name = (s), .long_name = (l), .value = check_vtype(v, bool *), .help = (h) }
#define OPT_BOOLEAN_FLAG(s, l, v, h, f)     { .type = OPTION_BOOLEAN, .short_name = (s), .long_name = (l), .value = check_vtype(v, bool *), .help = (h), .flags = (f) }
#define OPT_BOOLEAN_SET(s, l, v, os, h) \
	{ .type = OPTION_BOOLEAN, .short_name = (s), .long_name = (l), \
	.value = check_vtype(v, bool *), .help = (h), \
	.set = check_vtype(os, bool *)}
#define OPT_INCR(s, l, v, h)        { .type = OPTION_INCR, .short_name = (s), .long_name = (l), .value = check_vtype(v, int *), .help = (h) }
#define OPT_SET_UINT(s, l, v, h, i)  { .type = OPTION_SET_UINT, .short_name = (s), .long_name = (l), .value = check_vtype(v, unsigned int *), .help = (h), .defval = (i) }
#define OPT_SET_PTR(s, l, v, h, p)  { .type = OPTION_SET_PTR, .short_name = (s), .long_name = (l), .value = (v), .help = (h), .defval = (p) }
#define OPT_INTEGER(s, l, v, h)     { .type = OPTION_INTEGER, .short_name = (s), .long_name = (l), .value = check_vtype(v, int *), .help = (h) }
#define OPT_UINTEGER(s, l, v, h)    { .type = OPTION_UINTEGER, .short_name = (s), .long_name = (l), .value = check_vtype(v, unsigned int *), .help = (h) }
#define OPT_LONG(s, l, v, h)        { .type = OPTION_LONG, .short_name = (s), .long_name = (l), .value = check_vtype(v, long *), .help = (h) }
#define OPT_ULONG(s, l, v, h)        { .type = OPTION_ULONG, .short_name = (s), .long_name = (l), .value = check_vtype(v, unsigned long *), .help = (h) }
#define OPT_U64(s, l, v, h)         { .type = OPTION_U64, .short_name = (s), .long_name = (l), .value = check_vtype(v, u64 *), .help = (h) }
#define OPT_STRING(s, l, v, a, h)   { .type = OPTION_STRING,  .short_name = (s), .long_name = (l), .value = check_vtype(v, const char **), .argh = (a), .help = (h) }
#define OPT_STRING_OPTARG(s, l, v, a, h, d) \
	{ .type = OPTION_STRING,  .short_name = (s), .long_name = (l), \
	  .value = check_vtype(v, const char **), .argh =(a), .help = (h), \
	  .flags = PARSE_OPT_OPTARG, .defval = (intptr_t)(d) }
#define OPT_STRING_OPTARG_SET(s, l, v, os, a, h, d) \
	{ .type = OPTION_STRING, .short_name = (s), .long_name = (l), \
	  .value = check_vtype(v, const char **), .argh = (a), .help = (h), \
	  .flags = PARSE_OPT_OPTARG, .defval = (intptr_t)(d), \
	  .set = check_vtype(os, bool *)}
#define OPT_STRING_NOEMPTY(s, l, v, a, h)   { .type = OPTION_STRING,  .short_name = (s), .long_name = (l), .value = check_vtype(v, const char **), .argh = (a), .help = (h), .flags = PARSE_OPT_NOEMPTY}
#define OPT_DATE(s, l, v, h) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l), .value = (v), .argh = "time", .help = (h), .callback = parse_opt_approxidate_cb }
#define OPT_CALLBACK(s, l, v, a, h, f) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l), .value = (v), .argh = (a), .help = (h), .callback = (f) }
#define OPT_CALLBACK_SET(s, l, v, os, a, h, f) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l), .value = (v), .argh = (a), .help = (h), .callback = (f), .set = check_vtype(os, bool *)}
#define OPT_CALLBACK_NOOPT(s, l, v, a, h, f) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l), .value = (v), .argh = (a), .help = (h), .callback = (f), .flags = PARSE_OPT_NOARG }
#define OPT_CALLBACK_DEFAULT(s, l, v, a, h, f, d) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l), .value = (v), .argh = (a), .help = (h), .callback = (f), .defval = (intptr_t)d, .flags = PARSE_OPT_LASTARG_DEFAULT }
#define OPT_CALLBACK_DEFAULT_NOOPT(s, l, v, a, h, f, d) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l),\
	.value = (v), .arg = (a), .help = (h), .callback = (f), .defval = (intptr_t)d,\
	.flags = PARSE_OPT_LASTARG_DEFAULT | PARSE_OPT_NOARG}
#define OPT_CALLBACK_OPTARG(s, l, v, d, a, h, f) \
	{ .type = OPTION_CALLBACK, .short_name = (s), .long_name = (l), \
	  .value = (v), .argh = (a), .help = (h), .callback = (f), \
	  .flags = PARSE_OPT_OPTARG, .data = (d) }

/* parse_options() will filter out the processed options and leave the
 * non-option argments in argv[].
 * Returns the number of arguments left in argv[].
 *
 * NOTE: parse_options() and parse_options_subcommand() may call exit() in the
 * case of an error (or for 'special' options like --list-cmds or --list-opts).
 */
extern int parse_options(int argc, const char **argv,
                         const struct option *options,
                         const char * const usagestr[], int flags);

extern int parse_options_subcommand(int argc, const char **argv,
				const struct option *options,
				const char *const subcommands[],
				const char *usagestr[], int flags);

extern NORETURN void usage_with_options(const char * const *usagestr,
                                        const struct option *options);
extern NORETURN __attribute__((format(printf,3,4)))
void usage_with_options_msg(const char * const *usagestr,
			    const struct option *options,
			    const char *fmt, ...);

/*----- incremantal advanced APIs -----*/

enum {
	PARSE_OPT_HELP = -1,
	PARSE_OPT_DONE,
	PARSE_OPT_LIST_OPTS,
	PARSE_OPT_LIST_SUBCMDS,
	PARSE_OPT_UNKNOWN,
};

/*
 * It's okay for the caller to consume argv/argc in the usual way.
 * Other fields of that structure are private to parse-options and should not
 * be modified in any way.
 */
struct parse_opt_ctx_t {
	const char **argv;
	const char **out;
	int argc, cpidx;
	const char *opt;
	const struct option *excl_opt;
	int flags;
};

extern int parse_options_usage(const char * const *usagestr,
			       const struct option *opts,
			       const char *optstr,
			       bool short_opt);


/*----- some often used options -----*/
extern int parse_opt_abbrev_cb(const struct option *, const char *, int);
extern int parse_opt_approxidate_cb(const struct option *, const char *, int);
extern int parse_opt_verbosity_cb(const struct option *, const char *, int);

#define OPT__VERBOSE(var)  OPT_BOOLEAN('v', "verbose", (var), "be verbose")
#define OPT__QUIET(var)    OPT_BOOLEAN('q', "quiet",   (var), "be quiet")
#define OPT__VERBOSITY(var) \
	{ OPTION_CALLBACK, 'v', "verbose", (var), NULL, "be more verbose", \
	  PARSE_OPT_NOARG, &parse_opt_verbosity_cb, 0 }, \
	{ OPTION_CALLBACK, 'q', "quiet", (var), NULL, "be more quiet", \
	  PARSE_OPT_NOARG, &parse_opt_verbosity_cb, 0 }
#define OPT__DRY_RUN(var)  OPT_BOOLEAN('n', "dry-run", (var), "dry run")
#define OPT__ABBREV(var)  \
	{ OPTION_CALLBACK, 0, "abbrev", (var), "n", \
	  "use <n> digits to display SHA-1s", \
	  PARSE_OPT_OPTARG, &parse_opt_abbrev_cb, 0 }

extern const char *parse_options_fix_filename(const char *prefix, const char *file);

void set_option_flag(struct option *opts, int sopt, const char *lopt, int flag);
void set_option_nobuild(struct option *opts, int shortopt, const char *longopt,
			const char *build_opt, bool can_skip);

#endif /* __SUBCMD_PARSE_OPTIONS_H */

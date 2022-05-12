// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/hfsplus/options.c
 *
 * Copyright (C) 2001
 * Brad Boyer (flar@allandria.com)
 * (C) 2003 Ardis Technologies <roman@ardistech.com>
 *
 * Option parsing
 */

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/nls.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include "hfsplus_fs.h"

enum {
	opt_creator, opt_type,
	opt_umask, opt_uid, opt_gid,
	opt_part, opt_session, opt_nls,
	opt_nodecompose, opt_decompose,
	opt_barrier, opt_nobarrier,
	opt_force, opt_err
};

static const match_table_t tokens = {
	{ opt_creator, "creator=%s" },
	{ opt_type, "type=%s" },
	{ opt_umask, "umask=%o" },
	{ opt_uid, "uid=%u" },
	{ opt_gid, "gid=%u" },
	{ opt_part, "part=%u" },
	{ opt_session, "session=%u" },
	{ opt_nls, "nls=%s" },
	{ opt_decompose, "decompose" },
	{ opt_nodecompose, "nodecompose" },
	{ opt_barrier, "barrier" },
	{ opt_nobarrier, "nobarrier" },
	{ opt_force, "force" },
	{ opt_err, NULL }
};

/* Initialize an options object to reasonable defaults */
void hfsplus_fill_defaults(struct hfsplus_sb_info *opts)
{
	if (!opts)
		return;

	opts->creator = HFSPLUS_DEF_CR_TYPE;
	opts->type = HFSPLUS_DEF_CR_TYPE;
	opts->umask = current_umask();
	opts->uid = current_uid();
	opts->gid = current_gid();
	opts->part = -1;
	opts->session = -1;
}

/* convert a "four byte character" to a 32 bit int with error checks */
static inline int match_fourchar(substring_t *arg, u32 *result)
{
	if (arg->to - arg->from != 4)
		return -EINVAL;
	memcpy(result, arg->from, 4);
	return 0;
}

int hfsplus_parse_options_remount(char *input, int *force)
{
	char *p;
	substring_t args[MAX_OPT_ARGS];
	int token;

	if (!input)
		return 1;

	while ((p = strsep(&input, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case opt_force:
			*force = 1;
			break;
		default:
			break;
		}
	}

	return 1;
}

/* Parse options from mount. Returns 0 on failure */
/* input is the options passed to mount() as a string */
int hfsplus_parse_options(char *input, struct hfsplus_sb_info *sbi)
{
	char *p;
	substring_t args[MAX_OPT_ARGS];
	int tmp, token;

	if (!input)
		goto done;

	while ((p = strsep(&input, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case opt_creator:
			if (match_fourchar(&args[0], &sbi->creator)) {
				pr_err("creator requires a 4 character value\n");
				return 0;
			}
			break;
		case opt_type:
			if (match_fourchar(&args[0], &sbi->type)) {
				pr_err("type requires a 4 character value\n");
				return 0;
			}
			break;
		case opt_umask:
			if (match_octal(&args[0], &tmp)) {
				pr_err("umask requires a value\n");
				return 0;
			}
			sbi->umask = (umode_t)tmp;
			break;
		case opt_uid:
			if (match_int(&args[0], &tmp)) {
				pr_err("uid requires an argument\n");
				return 0;
			}
			sbi->uid = make_kuid(current_user_ns(), (uid_t)tmp);
			if (!uid_valid(sbi->uid)) {
				pr_err("invalid uid specified\n");
				return 0;
			}
			break;
		case opt_gid:
			if (match_int(&args[0], &tmp)) {
				pr_err("gid requires an argument\n");
				return 0;
			}
			sbi->gid = make_kgid(current_user_ns(), (gid_t)tmp);
			if (!gid_valid(sbi->gid)) {
				pr_err("invalid gid specified\n");
				return 0;
			}
			break;
		case opt_part:
			if (match_int(&args[0], &sbi->part)) {
				pr_err("part requires an argument\n");
				return 0;
			}
			break;
		case opt_session:
			if (match_int(&args[0], &sbi->session)) {
				pr_err("session requires an argument\n");
				return 0;
			}
			break;
		case opt_nls:
			if (sbi->nls) {
				pr_err("unable to change nls mapping\n");
				return 0;
			}
			p = match_strdup(&args[0]);
			if (p)
				sbi->nls = load_nls(p);
			if (!sbi->nls) {
				pr_err("unable to load nls mapping \"%s\"\n",
				       p);
				kfree(p);
				return 0;
			}
			kfree(p);
			break;
		case opt_decompose:
			clear_bit(HFSPLUS_SB_NODECOMPOSE, &sbi->flags);
			break;
		case opt_nodecompose:
			set_bit(HFSPLUS_SB_NODECOMPOSE, &sbi->flags);
			break;
		case opt_barrier:
			clear_bit(HFSPLUS_SB_NOBARRIER, &sbi->flags);
			break;
		case opt_nobarrier:
			set_bit(HFSPLUS_SB_NOBARRIER, &sbi->flags);
			break;
		case opt_force:
			set_bit(HFSPLUS_SB_FORCE, &sbi->flags);
			break;
		default:
			return 0;
		}
	}

done:
	if (!sbi->nls) {
		/* try utf8 first, as this is the old default behaviour */
		sbi->nls = load_nls("utf8");
		if (!sbi->nls)
			sbi->nls = load_nls_default();
		if (!sbi->nls)
			return 0;
	}

	return 1;
}

int hfsplus_show_options(struct seq_file *seq, struct dentry *root)
{
	struct hfsplus_sb_info *sbi = HFSPLUS_SB(root->d_sb);

	if (sbi->creator != HFSPLUS_DEF_CR_TYPE)
		seq_show_option_n(seq, "creator", (char *)&sbi->creator, 4);
	if (sbi->type != HFSPLUS_DEF_CR_TYPE)
		seq_show_option_n(seq, "type", (char *)&sbi->type, 4);
	seq_printf(seq, ",umask=%o,uid=%u,gid=%u", sbi->umask,
			from_kuid_munged(&init_user_ns, sbi->uid),
			from_kgid_munged(&init_user_ns, sbi->gid));
	if (sbi->part >= 0)
		seq_printf(seq, ",part=%u", sbi->part);
	if (sbi->session >= 0)
		seq_printf(seq, ",session=%u", sbi->session);
	if (sbi->nls)
		seq_printf(seq, ",nls=%s", sbi->nls->charset);
	if (test_bit(HFSPLUS_SB_NODECOMPOSE, &sbi->flags))
		seq_puts(seq, ",nodecompose");
	if (test_bit(HFSPLUS_SB_NOBARRIER, &sbi->flags))
		seq_puts(seq, ",nobarrier");
	return 0;
}

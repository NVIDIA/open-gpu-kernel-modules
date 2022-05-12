// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * (C) Copyright David Gibson <dwg@au1.ibm.com>, IBM Corporation.  2005.
 */

#include <sys/stat.h>

#include "dtc.h"
#include "srcpos.h"

/*
 * Command line options
 */
int quiet;		/* Level of quietness */
int reservenum;		/* Number of memory reservation slots */
int minsize;		/* Minimum blob size */
int padsize;		/* Additional padding to blob */
int alignsize;		/* Additional padding to blob accroding to the alignsize */
int phandle_format = PHANDLE_EPAPR;	/* Use linux,phandle or phandle properties */
int generate_symbols;	/* enable symbols & fixup support */
int generate_fixups;		/* suppress generation of fixups on symbol support */
int auto_label_aliases;		/* auto generate labels -> aliases */
int annotate;		/* Level of annotation: 1 for input source location
			   >1 for full input source location. */

static int is_power_of_2(int x)
{
	return (x > 0) && ((x & (x - 1)) == 0);
}

static void fill_fullpaths(struct node *tree, const char *prefix)
{
	struct node *child;
	const char *unit;

	tree->fullpath = join_path(prefix, tree->name);

	unit = strchr(tree->name, '@');
	if (unit)
		tree->basenamelen = unit - tree->name;
	else
		tree->basenamelen = strlen(tree->name);

	for_each_child(tree, child)
		fill_fullpaths(child, tree->fullpath);
}

/* Usage related data. */
static const char usage_synopsis[] = "dtc [options] <input file>";
static const char usage_short_opts[] = "qI:O:o:V:d:R:S:p:a:fb:i:H:sW:E:@AThv";
static struct option const usage_long_opts[] = {
	{"quiet",            no_argument, NULL, 'q'},
	{"in-format",         a_argument, NULL, 'I'},
	{"out",               a_argument, NULL, 'o'},
	{"out-format",        a_argument, NULL, 'O'},
	{"out-version",       a_argument, NULL, 'V'},
	{"out-dependency",    a_argument, NULL, 'd'},
	{"reserve",           a_argument, NULL, 'R'},
	{"space",             a_argument, NULL, 'S'},
	{"pad",               a_argument, NULL, 'p'},
	{"align",             a_argument, NULL, 'a'},
	{"boot-cpu",          a_argument, NULL, 'b'},
	{"force",            no_argument, NULL, 'f'},
	{"include",           a_argument, NULL, 'i'},
	{"sort",             no_argument, NULL, 's'},
	{"phandle",           a_argument, NULL, 'H'},
	{"warning",           a_argument, NULL, 'W'},
	{"error",             a_argument, NULL, 'E'},
	{"symbols",	     no_argument, NULL, '@'},
	{"auto-alias",       no_argument, NULL, 'A'},
	{"annotate",         no_argument, NULL, 'T'},
	{"help",             no_argument, NULL, 'h'},
	{"version",          no_argument, NULL, 'v'},
	{NULL,               no_argument, NULL, 0x0},
};
static const char * const usage_opts_help[] = {
	"\n\tQuiet: -q suppress warnings, -qq errors, -qqq all",
	"\n\tInput formats are:\n"
	 "\t\tdts - device tree source text\n"
	 "\t\tdtb - device tree blob\n"
	 "\t\tfs  - /proc/device-tree style directory",
	"\n\tOutput file",
	"\n\tOutput formats are:\n"
	 "\t\tdts - device tree source text\n"
	 "\t\tdtb - device tree blob\n"
#ifndef NO_YAML
	 "\t\tyaml - device tree encoded as YAML\n"
#endif
	 "\t\tasm - assembler source",
	"\n\tBlob version to produce, defaults to "stringify(DEFAULT_FDT_VERSION)" (for dtb and asm output)",
	"\n\tOutput dependency file",
	"\n\tMake space for <number> reserve map entries (for dtb and asm output)",
	"\n\tMake the blob at least <bytes> long (extra space)",
	"\n\tAdd padding to the blob of <bytes> long (extra space)",
	"\n\tMake the blob align to the <bytes> (extra space)",
	"\n\tSet the physical boot cpu",
	"\n\tTry to produce output even if the input tree has errors",
	"\n\tAdd a path to search for include files",
	"\n\tSort nodes and properties before outputting (useful for comparing trees)",
	"\n\tValid phandle formats are:\n"
	 "\t\tlegacy - \"linux,phandle\" properties only\n"
	 "\t\tepapr  - \"phandle\" properties only\n"
	 "\t\tboth   - Both \"linux,phandle\" and \"phandle\" properties",
	"\n\tEnable/disable warnings (prefix with \"no-\")",
	"\n\tEnable/disable errors (prefix with \"no-\")",
	"\n\tEnable generation of symbols",
	"\n\tEnable auto-alias of labels",
	"\n\tAnnotate output .dts with input source file and line (-T -T for more details)",
	"\n\tPrint this help and exit",
	"\n\tPrint version and exit",
	NULL,
};

static const char *guess_type_by_name(const char *fname, const char *fallback)
{
	const char *s;

	s = strrchr(fname, '.');
	if (s == NULL)
		return fallback;
	if (!strcasecmp(s, ".dts"))
		return "dts";
	if (!strcasecmp(s, ".yaml"))
		return "yaml";
	if (!strcasecmp(s, ".dtbo"))
		return "dtb";
	if (!strcasecmp(s, ".dtb"))
		return "dtb";
	return fallback;
}

static const char *guess_input_format(const char *fname, const char *fallback)
{
	struct stat statbuf;
	fdt32_t magic;
	FILE *f;

	if (stat(fname, &statbuf) != 0)
		return fallback;

	if (S_ISDIR(statbuf.st_mode))
		return "fs";

	if (!S_ISREG(statbuf.st_mode))
		return fallback;

	f = fopen(fname, "r");
	if (f == NULL)
		return fallback;
	if (fread(&magic, 4, 1, f) != 1) {
		fclose(f);
		return fallback;
	}
	fclose(f);

	if (fdt32_to_cpu(magic) == FDT_MAGIC)
		return "dtb";

	return guess_type_by_name(fname, fallback);
}

int main(int argc, char *argv[])
{
	struct dt_info *dti;
	const char *inform = NULL;
	const char *outform = NULL;
	const char *outname = "-";
	const char *depname = NULL;
	bool force = false, sort = false;
	const char *arg;
	int opt;
	FILE *outf = NULL;
	int outversion = DEFAULT_FDT_VERSION;
	long long cmdline_boot_cpuid = -1;

	quiet      = 0;
	reservenum = 0;
	minsize    = 0;
	padsize    = 0;
	alignsize  = 0;

	while ((opt = util_getopt_long()) != EOF) {
		switch (opt) {
		case 'I':
			inform = optarg;
			break;
		case 'O':
			outform = optarg;
			break;
		case 'o':
			outname = optarg;
			break;
		case 'V':
			outversion = strtol(optarg, NULL, 0);
			break;
		case 'd':
			depname = optarg;
			break;
		case 'R':
			reservenum = strtol(optarg, NULL, 0);
			break;
		case 'S':
			minsize = strtol(optarg, NULL, 0);
			break;
		case 'p':
			padsize = strtol(optarg, NULL, 0);
			break;
		case 'a':
			alignsize = strtol(optarg, NULL, 0);
			if (!is_power_of_2(alignsize))
				die("Invalid argument \"%d\" to -a option\n",
				    alignsize);
			break;
		case 'f':
			force = true;
			break;
		case 'q':
			quiet++;
			break;
		case 'b':
			cmdline_boot_cpuid = strtoll(optarg, NULL, 0);
			break;
		case 'i':
			srcfile_add_search_path(optarg);
			break;
		case 'v':
			util_version();
		case 'H':
			if (streq(optarg, "legacy"))
				phandle_format = PHANDLE_LEGACY;
			else if (streq(optarg, "epapr"))
				phandle_format = PHANDLE_EPAPR;
			else if (streq(optarg, "both"))
				phandle_format = PHANDLE_BOTH;
			else
				die("Invalid argument \"%s\" to -H option\n",
				    optarg);
			break;

		case 's':
			sort = true;
			break;

		case 'W':
			parse_checks_option(true, false, optarg);
			break;

		case 'E':
			parse_checks_option(false, true, optarg);
			break;

		case '@':
			generate_symbols = 1;
			break;
		case 'A':
			auto_label_aliases = 1;
			break;
		case 'T':
			annotate++;
			break;

		case 'h':
			usage(NULL);
		default:
			usage("unknown option");
		}
	}

	if (argc > (optind+1))
		usage("missing files");
	else if (argc < (optind+1))
		arg = "-";
	else
		arg = argv[optind];

	/* minsize and padsize are mutually exclusive */
	if (minsize && padsize)
		die("Can't set both -p and -S\n");

	if (depname) {
		depfile = fopen(depname, "w");
		if (!depfile)
			die("Couldn't open dependency file %s: %s\n", depname,
			    strerror(errno));
		fprintf(depfile, "%s:", outname);
	}

	if (inform == NULL)
		inform = guess_input_format(arg, "dts");
	if (outform == NULL) {
		outform = guess_type_by_name(outname, NULL);
		if (outform == NULL) {
			if (streq(inform, "dts"))
				outform = "dtb";
			else
				outform = "dts";
		}
	}
	if (annotate && (!streq(inform, "dts") || !streq(outform, "dts")))
		die("--annotate requires -I dts -O dts\n");
	if (streq(inform, "dts"))
		dti = dt_from_source(arg);
	else if (streq(inform, "fs"))
		dti = dt_from_fs(arg);
	else if(streq(inform, "dtb"))
		dti = dt_from_blob(arg);
	else
		die("Unknown input format \"%s\"\n", inform);

	dti->outname = outname;

	if (depfile) {
		fputc('\n', depfile);
		fclose(depfile);
	}

	if (cmdline_boot_cpuid != -1)
		dti->boot_cpuid_phys = cmdline_boot_cpuid;

	fill_fullpaths(dti->dt, "");

	/* on a plugin, generate by default */
	if (dti->dtsflags & DTSF_PLUGIN) {
		generate_fixups = 1;
	}

	process_checks(force, dti);

	if (auto_label_aliases)
		generate_label_tree(dti, "aliases", false);

	if (generate_symbols)
		generate_label_tree(dti, "__symbols__", true);

	if (generate_fixups) {
		generate_fixups_tree(dti, "__fixups__");
		generate_local_fixups_tree(dti, "__local_fixups__");
	}

	if (sort)
		sort_tree(dti);

	if (streq(outname, "-")) {
		outf = stdout;
	} else {
		outf = fopen(outname, "wb");
		if (! outf)
			die("Couldn't open output file %s: %s\n",
			    outname, strerror(errno));
	}

	if (streq(outform, "dts")) {
		dt_to_source(outf, dti);
#ifndef NO_YAML
	} else if (streq(outform, "yaml")) {
		if (!streq(inform, "dts"))
			die("YAML output format requires dts input format\n");
		dt_to_yaml(outf, dti);
#endif
	} else if (streq(outform, "dtb")) {
		dt_to_blob(outf, dti, outversion);
	} else if (streq(outform, "dtbo")) {
		dt_to_blob(outf, dti, outversion);
	} else if (streq(outform, "asm")) {
		dt_to_asm(outf, dti, outversion);
	} else if (streq(outform, "null")) {
		/* do nothing */
	} else {
		die("Unknown output format \"%s\"\n", outform);
	}

	exit(0);
}

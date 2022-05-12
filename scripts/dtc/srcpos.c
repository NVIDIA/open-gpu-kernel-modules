// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2007 Jon Loeliger, Freescale Semiconductor, Inc.
 */

#define _GNU_SOURCE

#include <stdio.h>

#include "dtc.h"
#include "srcpos.h"

/* A node in our list of directories to search for source/include files */
struct search_path {
	struct search_path *next;	/* next node in list, NULL for end */
	const char *dirname;		/* name of directory to search */
};

/* This is the list of directories that we search for source files */
static struct search_path *search_path_head, **search_path_tail;

/* Detect infinite include recursion. */
#define MAX_SRCFILE_DEPTH     (200)
static int srcfile_depth; /* = 0 */

static char *get_dirname(const char *path)
{
	const char *slash = strrchr(path, '/');

	if (slash) {
		int len = slash - path;
		char *dir = xmalloc(len + 1);

		memcpy(dir, path, len);
		dir[len] = '\0';
		return dir;
	}
	return NULL;
}

FILE *depfile; /* = NULL */
struct srcfile_state *current_srcfile; /* = NULL */
static char *initial_path; /* = NULL */
static int initial_pathlen; /* = 0 */
static bool initial_cpp = true;

static void set_initial_path(char *fname)
{
	int i, len = strlen(fname);

	xasprintf(&initial_path, "%s", fname);
	initial_pathlen = 0;
	for (i = 0; i != len; i++)
		if (initial_path[i] == '/')
			initial_pathlen++;
}

static char *shorten_to_initial_path(char *fname)
{
	char *p1, *p2, *prevslash1 = NULL;
	int slashes = 0;

	for (p1 = fname, p2 = initial_path; *p1 && *p2; p1++, p2++) {
		if (*p1 != *p2)
			break;
		if (*p1 == '/') {
			prevslash1 = p1;
			slashes++;
		}
	}
	p1 = prevslash1 + 1;
	if (prevslash1) {
		int diff = initial_pathlen - slashes, i, j;
		int restlen = strlen(fname) - (p1 - fname);
		char *res;

		res = xmalloc((3 * diff) + restlen + 1);
		for (i = 0, j = 0; i != diff; i++) {
			res[j++] = '.';
			res[j++] = '.';
			res[j++] = '/';
		}
		strcpy(res + j, p1);
		return res;
	}
	return NULL;
}

/**
 * Try to open a file in a given directory.
 *
 * If the filename is an absolute path, then dirname is ignored. If it is a
 * relative path, then we look in that directory for the file.
 *
 * @param dirname	Directory to look in, or NULL for none
 * @param fname		Filename to look for
 * @param fp		Set to NULL if file did not open
 * @return allocated filename on success (caller must free), NULL on failure
 */
static char *try_open(const char *dirname, const char *fname, FILE **fp)
{
	char *fullname;

	if (!dirname || fname[0] == '/')
		fullname = xstrdup(fname);
	else
		fullname = join_path(dirname, fname);

	*fp = fopen(fullname, "rb");
	if (!*fp) {
		free(fullname);
		fullname = NULL;
	}

	return fullname;
}

/**
 * Open a file for read access
 *
 * If it is a relative filename, we search the full search path for it.
 *
 * @param fname	Filename to open
 * @param fp	Returns pointer to opened FILE, or NULL on failure
 * @return pointer to allocated filename, which caller must free
 */
static char *fopen_any_on_path(const char *fname, FILE **fp)
{
	const char *cur_dir = NULL;
	struct search_path *node;
	char *fullname;

	/* Try current directory first */
	assert(fp);
	if (current_srcfile)
		cur_dir = current_srcfile->dir;
	fullname = try_open(cur_dir, fname, fp);

	/* Failing that, try each search path in turn */
	for (node = search_path_head; !*fp && node; node = node->next)
		fullname = try_open(node->dirname, fname, fp);

	return fullname;
}

FILE *srcfile_relative_open(const char *fname, char **fullnamep)
{
	FILE *f;
	char *fullname;

	if (streq(fname, "-")) {
		f = stdin;
		fullname = xstrdup("<stdin>");
	} else {
		fullname = fopen_any_on_path(fname, &f);
		if (!f)
			die("Couldn't open \"%s\": %s\n", fname,
			    strerror(errno));
	}

	if (depfile)
		fprintf(depfile, " %s", fullname);

	if (fullnamep)
		*fullnamep = fullname;
	else
		free(fullname);

	return f;
}

void srcfile_push(const char *fname)
{
	struct srcfile_state *srcfile;

	if (srcfile_depth++ >= MAX_SRCFILE_DEPTH)
		die("Includes nested too deeply");

	srcfile = xmalloc(sizeof(*srcfile));

	srcfile->f = srcfile_relative_open(fname, &srcfile->name);
	srcfile->dir = get_dirname(srcfile->name);
	srcfile->prev = current_srcfile;

	srcfile->lineno = 1;
	srcfile->colno = 1;

	current_srcfile = srcfile;

	if (srcfile_depth == 1)
		set_initial_path(srcfile->name);
}

bool srcfile_pop(void)
{
	struct srcfile_state *srcfile = current_srcfile;

	assert(srcfile);

	current_srcfile = srcfile->prev;

	if (fclose(srcfile->f))
		die("Error closing \"%s\": %s\n", srcfile->name,
		    strerror(errno));

	/* FIXME: We allow the srcfile_state structure to leak,
	 * because it could still be referenced from a location
	 * variable being carried through the parser somewhere.  To
	 * fix this we could either allocate all the files from a
	 * table, or use a pool allocator. */

	return current_srcfile ? true : false;
}

void srcfile_add_search_path(const char *dirname)
{
	struct search_path *node;

	/* Create the node */
	node = xmalloc(sizeof(*node));
	node->next = NULL;
	node->dirname = xstrdup(dirname);

	/* Add to the end of our list */
	if (search_path_tail)
		*search_path_tail = node;
	else
		search_path_head = node;
	search_path_tail = &node->next;
}

void srcpos_update(struct srcpos *pos, const char *text, int len)
{
	int i;

	pos->file = current_srcfile;

	pos->first_line = current_srcfile->lineno;
	pos->first_column = current_srcfile->colno;

	for (i = 0; i < len; i++)
		if (text[i] == '\n') {
			current_srcfile->lineno++;
			current_srcfile->colno = 1;
		} else {
			current_srcfile->colno++;
		}

	pos->last_line = current_srcfile->lineno;
	pos->last_column = current_srcfile->colno;
}

struct srcpos *
srcpos_copy(struct srcpos *pos)
{
	struct srcpos *pos_new;
	struct srcfile_state *srcfile_state;

	if (!pos)
		return NULL;

	pos_new = xmalloc(sizeof(struct srcpos));
	assert(pos->next == NULL);
	memcpy(pos_new, pos, sizeof(struct srcpos));

	/* allocate without free */
	srcfile_state = xmalloc(sizeof(struct srcfile_state));
	memcpy(srcfile_state, pos->file, sizeof(struct srcfile_state));
	pos_new->file = srcfile_state;

	return pos_new;
}

struct srcpos *srcpos_extend(struct srcpos *pos, struct srcpos *newtail)
{
	struct srcpos *p;

	if (!pos)
		return newtail;

	for (p = pos; p->next != NULL; p = p->next);
	p->next = newtail;
	return pos;
}

char *
srcpos_string(struct srcpos *pos)
{
	const char *fname = "<no-file>";
	char *pos_str;

	if (pos->file && pos->file->name)
		fname = pos->file->name;


	if (pos->first_line != pos->last_line)
		xasprintf(&pos_str, "%s:%d.%d-%d.%d", fname,
			  pos->first_line, pos->first_column,
			  pos->last_line, pos->last_column);
	else if (pos->first_column != pos->last_column)
		xasprintf(&pos_str, "%s:%d.%d-%d", fname,
			  pos->first_line, pos->first_column,
			  pos->last_column);
	else
		xasprintf(&pos_str, "%s:%d.%d", fname,
			  pos->first_line, pos->first_column);

	return pos_str;
}

static char *
srcpos_string_comment(struct srcpos *pos, bool first_line, int level)
{
	char *pos_str, *fname, *first, *rest;
	bool fresh_fname = false;

	if (!pos) {
		if (level > 1) {
			xasprintf(&pos_str, "<no-file>:<no-line>");
			return pos_str;
		} else {
			return NULL;
		}
	}

	if (!pos->file)
		fname = "<no-file>";
	else if (!pos->file->name)
		fname = "<no-filename>";
	else if (level > 1)
		fname = pos->file->name;
	else {
		fname = shorten_to_initial_path(pos->file->name);
		if (fname)
			fresh_fname = true;
		else
			fname = pos->file->name;
	}

	if (level > 1)
		xasprintf(&first, "%s:%d:%d-%d:%d", fname,
			  pos->first_line, pos->first_column,
			  pos->last_line, pos->last_column);
	else
		xasprintf(&first, "%s:%d", fname,
			  first_line ? pos->first_line : pos->last_line);

	if (fresh_fname)
		free(fname);

	if (pos->next != NULL) {
		rest = srcpos_string_comment(pos->next, first_line, level);
		xasprintf(&pos_str, "%s, %s", first, rest);
		free(first);
		free(rest);
	} else {
		pos_str = first;
	}

	return pos_str;
}

char *srcpos_string_first(struct srcpos *pos, int level)
{
	return srcpos_string_comment(pos, true, level);
}

char *srcpos_string_last(struct srcpos *pos, int level)
{
	return srcpos_string_comment(pos, false, level);
}

void srcpos_verror(struct srcpos *pos, const char *prefix,
		   const char *fmt, va_list va)
{
	char *srcstr;

	srcstr = srcpos_string(pos);

	fprintf(stderr, "%s: %s ", prefix, srcstr);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");

	free(srcstr);
}

void srcpos_error(struct srcpos *pos, const char *prefix,
		  const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	srcpos_verror(pos, prefix, fmt, va);
	va_end(va);
}

void srcpos_set_line(char *f, int l)
{
	current_srcfile->name = f;
	current_srcfile->lineno = l;

	if (initial_cpp) {
		initial_cpp = false;
		set_initial_path(f);
	}
}

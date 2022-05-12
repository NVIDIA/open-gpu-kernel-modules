/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_COLOR_H
#define __PERF_COLOR_H

#include <stdio.h>
#include <stdarg.h>

/* "\033[1;38;5;2xx;48;5;2xxm\0" is 23 bytes */
#define COLOR_MAXLEN 24

#define PERF_COLOR_NORMAL	""
#define PERF_COLOR_RESET	"\033[m"
#define PERF_COLOR_BOLD		"\033[1m"
#define PERF_COLOR_RED		"\033[31m"
#define PERF_COLOR_GREEN	"\033[32m"
#define PERF_COLOR_YELLOW	"\033[33m"
#define PERF_COLOR_BLUE		"\033[34m"
#define PERF_COLOR_MAGENTA	"\033[35m"
#define PERF_COLOR_CYAN		"\033[36m"
#define PERF_COLOR_BG_RED	"\033[41m"

#define MIN_GREEN	0.5
#define MIN_RED		5.0

/*
 * This variable stores the value of color.ui
 */
extern int perf_use_color_default;


/*
 * Use this instead of perf_default_config if you need the value of color.ui.
 */
int perf_color_default_config(const char *var, const char *value, void *cb);

int perf_config_colorbool(const char *var, const char *value, int stdout_is_tty);
int color_vsnprintf(char *bf, size_t size, const char *color,
		    const char *fmt, va_list args);
int color_vfprintf(FILE *fp, const char *color, const char *fmt, va_list args);
int color_fprintf(FILE *fp, const char *color, const char *fmt, ...);
int color_snprintf(char *bf, size_t size, const char *color, const char *fmt, ...);
int color_fwrite_lines(FILE *fp, const char *color, size_t count, const char *buf);
int value_color_snprintf(char *bf, size_t size, const char *fmt, double value);
int percent_color_snprintf(char *bf, size_t size, const char *fmt, ...);
int percent_color_len_snprintf(char *bf, size_t size, const char *fmt, ...);
int percent_color_fprintf(FILE *fp, const char *fmt, double percent);
const char *get_percent_color(double percent);

#endif /* __PERF_COLOR_H */

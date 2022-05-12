// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include "pmu.h"
#include "topdown.h"

int topdown_filter_events(const char **attr, char **str, bool use_group)
{
	int off = 0;
	int i;
	int len = 0;
	char *s;

	for (i = 0; attr[i]; i++) {
		if (pmu_have_event("cpu", attr[i])) {
			len += strlen(attr[i]) + 1;
			attr[i - off] = attr[i];
		} else
			off++;
	}
	attr[i - off] = NULL;

	*str = malloc(len + 1 + 2);
	if (!*str)
		return -1;
	s = *str;
	if (i - off == 0) {
		*s = 0;
		return 0;
	}
	if (use_group)
		*s++ = '{';
	for (i = 0; attr[i]; i++) {
		strcpy(s, attr[i]);
		s += strlen(s);
		*s++ = ',';
	}
	if (use_group) {
		s[-1] = '}';
		*s = 0;
	} else
		s[-1] = 0;
	return 0;
}

__weak bool arch_topdown_check_group(bool *warn)
{
	*warn = false;
	return false;
}

__weak void arch_topdown_group_warn(void)
{
}

__weak bool arch_topdown_sample_read(struct evsel *leader __maybe_unused)
{
	return false;
}

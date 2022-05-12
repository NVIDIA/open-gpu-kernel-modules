// SPDX-License-Identifier: GPL-2.0-or-later
/*
 */

#include <linux/export.h>

#include <linux/libgcc.h>

word_type notrace __cmpdi2(long long a, long long b)
{
	const DWunion au = {
		.ll = a
	};
	const DWunion bu = {
		.ll = b
	};

	if (au.s.high < bu.s.high)
		return 0;
	else if (au.s.high > bu.s.high)
		return 2;

	if ((unsigned int) au.s.low < (unsigned int) bu.s.low)
		return 0;
	else if ((unsigned int) au.s.low > (unsigned int) bu.s.low)
		return 2;

	return 1;
}
EXPORT_SYMBOL(__cmpdi2);

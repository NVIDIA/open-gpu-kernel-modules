/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER_LINUX_PROCESSOR_H_H_
#define _KCL_HEADER_LINUX_PROCESSOR_H_H_

#ifdef HAVE_LINUX_PROCESSOR_H
#include_next <linux/processor.h>
#else
#include <asm/processor.h>
#endif

#endif

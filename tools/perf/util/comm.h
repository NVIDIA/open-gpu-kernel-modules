/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_COMM_H
#define __PERF_COMM_H

#include <linux/list.h>
#include <linux/types.h>
#include <stdbool.h>

struct comm_str;

struct comm {
	struct comm_str *comm_str;
	u64 start;
	struct list_head list;
	bool exec;
	union { /* Tool specific area */
		void	*priv;
		u64	db_id;
	};
};

void comm__free(struct comm *comm);
struct comm *comm__new(const char *str, u64 timestamp, bool exec);
const char *comm__str(const struct comm *comm);
int comm__override(struct comm *comm, const char *str, u64 timestamp,
		   bool exec);

#endif  /* __PERF_COMM_H */

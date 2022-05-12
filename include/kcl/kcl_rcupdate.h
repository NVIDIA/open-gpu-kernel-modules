/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Read-Copy Update mechanism for mutual exclusion
 *
 * Copyright IBM Corporation, 2001
 *
 * Author: Dipankar Sarma <dipankar@in.ibm.com>
 *
 * Based on the original work by Paul McKenney <paulmck@vnet.ibm.com>
 * and inputs from Rusty Russell, Andrea Arcangeli and Andi Kleen.
 * Papers:
 * http://www.rdrop.com/users/paulmck/paper/rclockpdcsproof.pdf
 * http://lse.sourceforge.net/locking/rclock_OLS.2001.05.01c.sc.pdf (OLS2001)
 *
 * For detailed explanation of Read-Copy Update mechanism see -
 *		http://lse.sourceforge.net/locking/rcupdate.html
 *
 */
#ifndef AMDKCL_RCUPDATE_H
#define AMDKCL_RCUPDATE_H

#include <linux/rcupdate.h>
#include <linux/version.h>

#ifndef rcu_pointer_handoff
#define rcu_pointer_handoff(p) (p)
#endif

#endif /* AMDKCL_RCUPDATE_H */

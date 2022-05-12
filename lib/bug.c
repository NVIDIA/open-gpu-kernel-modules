// SPDX-License-Identifier: GPL-2.0
/*
  Generic support for BUG()

  This respects the following config options:

  CONFIG_BUG - emit BUG traps.  Nothing happens without this.
  CONFIG_GENERIC_BUG - enable this code.
  CONFIG_GENERIC_BUG_RELATIVE_POINTERS - use 32-bit pointers relative to
	the containing struct bug_entry for bug_addr and file.
  CONFIG_DEBUG_BUGVERBOSE - emit full file+line information for each BUG

  CONFIG_BUG and CONFIG_DEBUG_BUGVERBOSE are potentially user-settable
  (though they're generally always on).

  CONFIG_GENERIC_BUG is set by each architecture using this code.

  To use this, your architecture must:

  1. Set up the config options:
     - Enable CONFIG_GENERIC_BUG if CONFIG_BUG

  2. Implement BUG (and optionally BUG_ON, WARN, WARN_ON)
     - Define HAVE_ARCH_BUG
     - Implement BUG() to generate a faulting instruction
     - NOTE: struct bug_entry does not have "file" or "line" entries
       when CONFIG_DEBUG_BUGVERBOSE is not enabled, so you must generate
       the values accordingly.

  3. Implement the trap
     - In the illegal instruction trap handler (typically), verify
       that the fault was in kernel mode, and call report_bug()
     - report_bug() will return whether it was a false alarm, a warning,
       or an actual bug.
     - You must implement the is_valid_bugaddr(bugaddr) callback which
       returns true if the eip is a real kernel address, and it points
       to the expected BUG trap instruction.

    Jeremy Fitzhardinge <jeremy@goop.org> 2006
 */

#define pr_fmt(fmt) fmt

#include <linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/sched.h>
#include <linux/rculist.h>
#include <linux/ftrace.h>

extern struct bug_entry __start___bug_table[], __stop___bug_table[];

static inline unsigned long bug_addr(const struct bug_entry *bug)
{
#ifndef CONFIG_GENERIC_BUG_RELATIVE_POINTERS
	return bug->bug_addr;
#else
	return (unsigned long)bug + bug->bug_addr_disp;
#endif
}

#ifdef CONFIG_MODULES
/* Updates are protected by module mutex */
static LIST_HEAD(module_bug_list);

static struct bug_entry *module_find_bug(unsigned long bugaddr)
{
	struct module *mod;
	struct bug_entry *bug = NULL;

	rcu_read_lock_sched();
	list_for_each_entry_rcu(mod, &module_bug_list, bug_list) {
		unsigned i;

		bug = mod->bug_table;
		for (i = 0; i < mod->num_bugs; ++i, ++bug)
			if (bugaddr == bug_addr(bug))
				goto out;
	}
	bug = NULL;
out:
	rcu_read_unlock_sched();

	return bug;
}

void module_bug_finalize(const Elf_Ehdr *hdr, const Elf_Shdr *sechdrs,
			 struct module *mod)
{
	char *secstrings;
	unsigned int i;

	mod->bug_table = NULL;
	mod->num_bugs = 0;

	/* Find the __bug_table section, if present */
	secstrings = (char *)hdr + sechdrs[hdr->e_shstrndx].sh_offset;
	for (i = 1; i < hdr->e_shnum; i++) {
		if (strcmp(secstrings+sechdrs[i].sh_name, "__bug_table"))
			continue;
		mod->bug_table = (void *) sechdrs[i].sh_addr;
		mod->num_bugs = sechdrs[i].sh_size / sizeof(struct bug_entry);
		break;
	}

	/*
	 * Strictly speaking this should have a spinlock to protect against
	 * traversals, but since we only traverse on BUG()s, a spinlock
	 * could potentially lead to deadlock and thus be counter-productive.
	 * Thus, this uses RCU to safely manipulate the bug list, since BUG
	 * must run in non-interruptive state.
	 */
	list_add_rcu(&mod->bug_list, &module_bug_list);
}

void module_bug_cleanup(struct module *mod)
{
	list_del_rcu(&mod->bug_list);
}

#else

static inline struct bug_entry *module_find_bug(unsigned long bugaddr)
{
	return NULL;
}
#endif

void bug_get_file_line(struct bug_entry *bug, const char **file,
		       unsigned int *line)
{
#ifdef CONFIG_DEBUG_BUGVERBOSE
#ifndef CONFIG_GENERIC_BUG_RELATIVE_POINTERS
	*file = bug->file;
#else
	*file = (const char *)bug + bug->file_disp;
#endif
	*line = bug->line;
#else
	*file = NULL;
	*line = 0;
#endif
}

struct bug_entry *find_bug(unsigned long bugaddr)
{
	struct bug_entry *bug;

	for (bug = __start___bug_table; bug < __stop___bug_table; ++bug)
		if (bugaddr == bug_addr(bug))
			return bug;

	return module_find_bug(bugaddr);
}

enum bug_trap_type report_bug(unsigned long bugaddr, struct pt_regs *regs)
{
	struct bug_entry *bug;
	const char *file;
	unsigned line, warning, once, done;

	if (!is_valid_bugaddr(bugaddr))
		return BUG_TRAP_TYPE_NONE;

	bug = find_bug(bugaddr);
	if (!bug)
		return BUG_TRAP_TYPE_NONE;

	disable_trace_on_warning();

	bug_get_file_line(bug, &file, &line);

	warning = (bug->flags & BUGFLAG_WARNING) != 0;
	once = (bug->flags & BUGFLAG_ONCE) != 0;
	done = (bug->flags & BUGFLAG_DONE) != 0;

	if (warning && once) {
		if (done)
			return BUG_TRAP_TYPE_WARN;

		/*
		 * Since this is the only store, concurrency is not an issue.
		 */
		bug->flags |= BUGFLAG_DONE;
	}

	/*
	 * BUG() and WARN_ON() families don't print a custom debug message
	 * before triggering the exception handler, so we must add the
	 * "cut here" line now. WARN() issues its own "cut here" before the
	 * extra debugging message it writes before triggering the handler.
	 */
	if ((bug->flags & BUGFLAG_NO_CUT_HERE) == 0)
		printk(KERN_DEFAULT CUT_HERE);

	if (warning) {
		/* this is a WARN_ON rather than BUG/BUG_ON */
		__warn(file, line, (void *)bugaddr, BUG_GET_TAINT(bug), regs,
		       NULL);
		return BUG_TRAP_TYPE_WARN;
	}

	if (file)
		pr_crit("kernel BUG at %s:%u!\n", file, line);
	else
		pr_crit("Kernel BUG at %pB [verbose debug info unavailable]\n",
			(void *)bugaddr);

	return BUG_TRAP_TYPE_BUG;
}

static void clear_once_table(struct bug_entry *start, struct bug_entry *end)
{
	struct bug_entry *bug;

	for (bug = start; bug < end; bug++)
		bug->flags &= ~BUGFLAG_DONE;
}

void generic_bug_clear_once(void)
{
#ifdef CONFIG_MODULES
	struct module *mod;

	rcu_read_lock_sched();
	list_for_each_entry_rcu(mod, &module_bug_list, bug_list)
		clear_once_table(mod->bug_table,
				 mod->bug_table + mod->num_bugs);
	rcu_read_unlock_sched();
#endif

	clear_once_table(__start___bug_table, __stop___bug_table);
}

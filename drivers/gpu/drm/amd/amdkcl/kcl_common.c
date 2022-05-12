/* SPDX-License-Identifier: MIT */
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <linux/printk.h>
#include <linux/bug.h>

unsigned long (*_kcl_kallsyms_lookup_name)(const char *name);

void *amdkcl_fp_setup(const char *symbol, void *dummy)
{
       unsigned long addr;
       void *fp = dummy;

       addr = _kcl_kallsyms_lookup_name(symbol);
       if (addr == 0) {
	       if (fp)
		       pr_warn("Warning: fail to get symbol %s, replace it with kcl stub\n", symbol);
	       else {
		       pr_err("Error: fail to get symbol %s, abort...\n", symbol);
		       BUG();
	       }
       } else {
	       fp = (void *)addr;
       }

       return fp;
}

void amdkcl_symbol_init(void)
{
#ifndef HAVE_KALLSYMS_LOOKUP_NAME
	struct kprobe kp;
	int r;

	memset(&kp, 0, sizeof(kp));
	kp.symbol_name = "kallsyms_lookup_name";
	r = register_kprobe(&kp);
	if (!r) {
		_kcl_kallsyms_lookup_name = (void *)kp.addr;
		unregister_kprobe(&kp);
	} else {
		pr_err("fail to get kallsyms_lookup_name, abort...\n");
		BUG();
	}
#else
	_kcl_kallsyms_lookup_name = kallsyms_lookup_name;
#endif
}

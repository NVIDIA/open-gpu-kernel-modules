// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Create default crypto algorithm instances.
 *
 * Copyright (c) 2006 Herbert Xu <herbert@gondor.apana.org.au>
 */

#include <crypto/internal/aead.h>
#include <linux/completion.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/rtnetlink.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/string.h>

#include "internal.h"

struct cryptomgr_param {
	struct rtattr *tb[CRYPTO_MAX_ATTRS + 2];

	struct {
		struct rtattr attr;
		struct crypto_attr_type data;
	} type;

	union {
		struct rtattr attr;
		struct {
			struct rtattr attr;
			struct crypto_attr_alg data;
		} alg;
		struct {
			struct rtattr attr;
			struct crypto_attr_u32 data;
		} nu32;
	} attrs[CRYPTO_MAX_ATTRS];

	char template[CRYPTO_MAX_ALG_NAME];

	struct crypto_larval *larval;

	u32 otype;
	u32 omask;
};

struct crypto_test_param {
	char driver[CRYPTO_MAX_ALG_NAME];
	char alg[CRYPTO_MAX_ALG_NAME];
	u32 type;
};

static int cryptomgr_probe(void *data)
{
	struct cryptomgr_param *param = data;
	struct crypto_template *tmpl;
	int err;

	tmpl = crypto_lookup_template(param->template);
	if (!tmpl)
		goto out;

	do {
		err = tmpl->create(tmpl, param->tb);
	} while (err == -EAGAIN && !signal_pending(current));

	crypto_tmpl_put(tmpl);

out:
	complete_all(&param->larval->completion);
	crypto_alg_put(&param->larval->alg);
	kfree(param);
	module_put_and_exit(0);
}

static int cryptomgr_schedule_probe(struct crypto_larval *larval)
{
	struct task_struct *thread;
	struct cryptomgr_param *param;
	const char *name = larval->alg.cra_name;
	const char *p;
	unsigned int len;
	int i;

	if (!try_module_get(THIS_MODULE))
		goto err;

	param = kzalloc(sizeof(*param), GFP_KERNEL);
	if (!param)
		goto err_put_module;

	for (p = name; isalnum(*p) || *p == '-' || *p == '_'; p++)
		;

	len = p - name;
	if (!len || *p != '(')
		goto err_free_param;

	memcpy(param->template, name, len);

	i = 0;
	for (;;) {
		int notnum = 0;

		name = ++p;

		for (; isalnum(*p) || *p == '-' || *p == '_'; p++)
			notnum |= !isdigit(*p);

		if (*p == '(') {
			int recursion = 0;

			for (;;) {
				if (!*++p)
					goto err_free_param;
				if (*p == '(')
					recursion++;
				else if (*p == ')' && !recursion--)
					break;
			}

			notnum = 1;
			p++;
		}

		len = p - name;
		if (!len)
			goto err_free_param;

		if (notnum) {
			param->attrs[i].alg.attr.rta_len =
				sizeof(param->attrs[i].alg);
			param->attrs[i].alg.attr.rta_type = CRYPTOA_ALG;
			memcpy(param->attrs[i].alg.data.name, name, len);
		} else {
			param->attrs[i].nu32.attr.rta_len =
				sizeof(param->attrs[i].nu32);
			param->attrs[i].nu32.attr.rta_type = CRYPTOA_U32;
			param->attrs[i].nu32.data.num =
				simple_strtol(name, NULL, 0);
		}

		param->tb[i + 1] = &param->attrs[i].attr;
		i++;

		if (i >= CRYPTO_MAX_ATTRS)
			goto err_free_param;

		if (*p == ')')
			break;

		if (*p != ',')
			goto err_free_param;
	}

	if (!i)
		goto err_free_param;

	param->tb[i + 1] = NULL;

	param->type.attr.rta_len = sizeof(param->type);
	param->type.attr.rta_type = CRYPTOA_TYPE;
	param->type.data.type = larval->alg.cra_flags & ~CRYPTO_ALG_TESTED;
	param->type.data.mask = larval->mask & ~CRYPTO_ALG_TESTED;
	param->tb[0] = &param->type.attr;

	param->otype = larval->alg.cra_flags;
	param->omask = larval->mask;

	crypto_alg_get(&larval->alg);
	param->larval = larval;

	thread = kthread_run(cryptomgr_probe, param, "cryptomgr_probe");
	if (IS_ERR(thread))
		goto err_put_larval;

	return NOTIFY_STOP;

err_put_larval:
	crypto_alg_put(&larval->alg);
err_free_param:
	kfree(param);
err_put_module:
	module_put(THIS_MODULE);
err:
	return NOTIFY_OK;
}

static int cryptomgr_test(void *data)
{
	struct crypto_test_param *param = data;
	u32 type = param->type;
	int err = 0;

#ifdef CONFIG_CRYPTO_MANAGER_DISABLE_TESTS
	goto skiptest;
#endif

	if (type & CRYPTO_ALG_TESTED)
		goto skiptest;

	err = alg_test(param->driver, param->alg, type, CRYPTO_ALG_TESTED);

skiptest:
	crypto_alg_tested(param->driver, err);

	kfree(param);
	module_put_and_exit(0);
}

static int cryptomgr_schedule_test(struct crypto_alg *alg)
{
	struct task_struct *thread;
	struct crypto_test_param *param;
	u32 type;

	if (!try_module_get(THIS_MODULE))
		goto err;

	param = kzalloc(sizeof(*param), GFP_KERNEL);
	if (!param)
		goto err_put_module;

	memcpy(param->driver, alg->cra_driver_name, sizeof(param->driver));
	memcpy(param->alg, alg->cra_name, sizeof(param->alg));
	type = alg->cra_flags;

	/* Do not test internal algorithms. */
	if (type & CRYPTO_ALG_INTERNAL)
		type |= CRYPTO_ALG_TESTED;

	param->type = type;

	thread = kthread_run(cryptomgr_test, param, "cryptomgr_test");
	if (IS_ERR(thread))
		goto err_free_param;

	return NOTIFY_STOP;

err_free_param:
	kfree(param);
err_put_module:
	module_put(THIS_MODULE);
err:
	return NOTIFY_OK;
}

static int cryptomgr_notify(struct notifier_block *this, unsigned long msg,
			    void *data)
{
	switch (msg) {
	case CRYPTO_MSG_ALG_REQUEST:
		return cryptomgr_schedule_probe(data);
	case CRYPTO_MSG_ALG_REGISTER:
		return cryptomgr_schedule_test(data);
	case CRYPTO_MSG_ALG_LOADED:
		break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block cryptomgr_notifier = {
	.notifier_call = cryptomgr_notify,
};

static int __init cryptomgr_init(void)
{
	return crypto_register_notifier(&cryptomgr_notifier);
}

static void __exit cryptomgr_exit(void)
{
	int err = crypto_unregister_notifier(&cryptomgr_notifier);
	BUG_ON(err);
}

/*
 * This is arch_initcall() so that the crypto self-tests are run on algorithms
 * registered early by subsys_initcall().  subsys_initcall() is needed for
 * generic implementations so that they're available for comparison tests when
 * other implementations are registered later by module_init().
 */
arch_initcall(cryptomgr_init);
module_exit(cryptomgr_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Crypto Algorithm Manager");

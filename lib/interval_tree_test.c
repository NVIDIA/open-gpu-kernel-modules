// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interval_tree.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <asm/timex.h>

#define __param(type, name, init, msg)		\
	static type name = init;		\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, nnodes, 100, "Number of nodes in the interval tree");
__param(int, perf_loops, 1000, "Number of iterations modifying the tree");

__param(int, nsearches, 100, "Number of searches to the interval tree");
__param(int, search_loops, 1000, "Number of iterations searching the tree");
__param(bool, search_all, false, "Searches will iterate all nodes in the tree");

__param(uint, max_endpoint, ~0, "Largest value for the interval's endpoint");

static struct rb_root_cached root = RB_ROOT_CACHED;
static struct interval_tree_node *nodes = NULL;
static u32 *queries = NULL;

static struct rnd_state rnd;

static inline unsigned long
search(struct rb_root_cached *root, unsigned long start, unsigned long last)
{
	struct interval_tree_node *node;
	unsigned long results = 0;

	for (node = interval_tree_iter_first(root, start, last); node;
	     node = interval_tree_iter_next(node, start, last))
		results++;
	return results;
}

static void init(void)
{
	int i;

	for (i = 0; i < nnodes; i++) {
		u32 b = (prandom_u32_state(&rnd) >> 4) % max_endpoint;
		u32 a = (prandom_u32_state(&rnd) >> 4) % b;

		nodes[i].start = a;
		nodes[i].last = b;
	}

	/*
	 * Limit the search scope to what the user defined.
	 * Otherwise we are merely measuring empty walks,
	 * which is pointless.
	 */
	for (i = 0; i < nsearches; i++)
		queries[i] = (prandom_u32_state(&rnd) >> 4) % max_endpoint;
}

static int interval_tree_test_init(void)
{
	int i, j;
	unsigned long results;
	cycles_t time1, time2, time;

	nodes = kmalloc_array(nnodes, sizeof(struct interval_tree_node),
			      GFP_KERNEL);
	if (!nodes)
		return -ENOMEM;

	queries = kmalloc_array(nsearches, sizeof(int), GFP_KERNEL);
	if (!queries) {
		kfree(nodes);
		return -ENOMEM;
	}

	printk(KERN_ALERT "interval tree insert/remove");

	prandom_seed_state(&rnd, 3141592653589793238ULL);
	init();

	time1 = get_cycles();

	for (i = 0; i < perf_loops; i++) {
		for (j = 0; j < nnodes; j++)
			interval_tree_insert(nodes + j, &root);
		for (j = 0; j < nnodes; j++)
			interval_tree_remove(nodes + j, &root);
	}

	time2 = get_cycles();
	time = time2 - time1;

	time = div_u64(time, perf_loops);
	printk(" -> %llu cycles\n", (unsigned long long)time);

	printk(KERN_ALERT "interval tree search");

	for (j = 0; j < nnodes; j++)
		interval_tree_insert(nodes + j, &root);

	time1 = get_cycles();

	results = 0;
	for (i = 0; i < search_loops; i++)
		for (j = 0; j < nsearches; j++) {
			unsigned long start = search_all ? 0 : queries[j];
			unsigned long last = search_all ? max_endpoint : queries[j];

			results += search(&root, start, last);
		}

	time2 = get_cycles();
	time = time2 - time1;

	time = div_u64(time, search_loops);
	results = div_u64(results, search_loops);
	printk(" -> %llu cycles (%lu results)\n",
	       (unsigned long long)time, results);

	kfree(queries);
	kfree(nodes);

	return -EAGAIN; /* Fail will directly unload the module */
}

static void interval_tree_test_exit(void)
{
	printk(KERN_ALERT "test exit\n");
}

module_init(interval_tree_test_init)
module_exit(interval_tree_test_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michel Lespinasse");
MODULE_DESCRIPTION("Interval Tree test");

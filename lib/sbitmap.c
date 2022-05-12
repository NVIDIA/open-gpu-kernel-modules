// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2016 Facebook
 * Copyright (C) 2013-2014 Jens Axboe
 */

#include <linux/sched.h>
#include <linux/random.h>
#include <linux/sbitmap.h>
#include <linux/seq_file.h>

static int init_alloc_hint(struct sbitmap *sb, gfp_t flags)
{
	unsigned depth = sb->depth;

	sb->alloc_hint = alloc_percpu_gfp(unsigned int, flags);
	if (!sb->alloc_hint)
		return -ENOMEM;

	if (depth && !sb->round_robin) {
		int i;

		for_each_possible_cpu(i)
			*per_cpu_ptr(sb->alloc_hint, i) = prandom_u32() % depth;
	}
	return 0;
}

static inline unsigned update_alloc_hint_before_get(struct sbitmap *sb,
						    unsigned int depth)
{
	unsigned hint;

	hint = this_cpu_read(*sb->alloc_hint);
	if (unlikely(hint >= depth)) {
		hint = depth ? prandom_u32() % depth : 0;
		this_cpu_write(*sb->alloc_hint, hint);
	}

	return hint;
}

static inline void update_alloc_hint_after_get(struct sbitmap *sb,
					       unsigned int depth,
					       unsigned int hint,
					       unsigned int nr)
{
	if (nr == -1) {
		/* If the map is full, a hint won't do us much good. */
		this_cpu_write(*sb->alloc_hint, 0);
	} else if (nr == hint || unlikely(sb->round_robin)) {
		/* Only update the hint if we used it. */
		hint = nr + 1;
		if (hint >= depth - 1)
			hint = 0;
		this_cpu_write(*sb->alloc_hint, hint);
	}
}

/*
 * See if we have deferred clears that we can batch move
 */
static inline bool sbitmap_deferred_clear(struct sbitmap_word *map)
{
	unsigned long mask;

	if (!READ_ONCE(map->cleared))
		return false;

	/*
	 * First get a stable cleared mask, setting the old mask to 0.
	 */
	mask = xchg(&map->cleared, 0);

	/*
	 * Now clear the masked bits in our free word
	 */
	atomic_long_andnot(mask, (atomic_long_t *)&map->word);
	BUILD_BUG_ON(sizeof(atomic_long_t) != sizeof(map->word));
	return true;
}

int sbitmap_init_node(struct sbitmap *sb, unsigned int depth, int shift,
		      gfp_t flags, int node, bool round_robin,
		      bool alloc_hint)
{
	unsigned int bits_per_word;
	unsigned int i;

	if (shift < 0)
		shift = sbitmap_calculate_shift(depth);

	bits_per_word = 1U << shift;
	if (bits_per_word > BITS_PER_LONG)
		return -EINVAL;

	sb->shift = shift;
	sb->depth = depth;
	sb->map_nr = DIV_ROUND_UP(sb->depth, bits_per_word);
	sb->round_robin = round_robin;

	if (depth == 0) {
		sb->map = NULL;
		return 0;
	}

	if (alloc_hint) {
		if (init_alloc_hint(sb, flags))
			return -ENOMEM;
	} else {
		sb->alloc_hint = NULL;
	}

	sb->map = kcalloc_node(sb->map_nr, sizeof(*sb->map), flags, node);
	if (!sb->map) {
		free_percpu(sb->alloc_hint);
		return -ENOMEM;
	}

	for (i = 0; i < sb->map_nr; i++) {
		sb->map[i].depth = min(depth, bits_per_word);
		depth -= sb->map[i].depth;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(sbitmap_init_node);

void sbitmap_resize(struct sbitmap *sb, unsigned int depth)
{
	unsigned int bits_per_word = 1U << sb->shift;
	unsigned int i;

	for (i = 0; i < sb->map_nr; i++)
		sbitmap_deferred_clear(&sb->map[i]);

	sb->depth = depth;
	sb->map_nr = DIV_ROUND_UP(sb->depth, bits_per_word);

	for (i = 0; i < sb->map_nr; i++) {
		sb->map[i].depth = min(depth, bits_per_word);
		depth -= sb->map[i].depth;
	}
}
EXPORT_SYMBOL_GPL(sbitmap_resize);

static int __sbitmap_get_word(unsigned long *word, unsigned long depth,
			      unsigned int hint, bool wrap)
{
	int nr;

	/* don't wrap if starting from 0 */
	wrap = wrap && hint;

	while (1) {
		nr = find_next_zero_bit(word, depth, hint);
		if (unlikely(nr >= depth)) {
			/*
			 * We started with an offset, and we didn't reset the
			 * offset to 0 in a failure case, so start from 0 to
			 * exhaust the map.
			 */
			if (hint && wrap) {
				hint = 0;
				continue;
			}
			return -1;
		}

		if (!test_and_set_bit_lock(nr, word))
			break;

		hint = nr + 1;
		if (hint >= depth - 1)
			hint = 0;
	}

	return nr;
}

static int sbitmap_find_bit_in_index(struct sbitmap *sb, int index,
				     unsigned int alloc_hint)
{
	struct sbitmap_word *map = &sb->map[index];
	int nr;

	do {
		nr = __sbitmap_get_word(&map->word, map->depth, alloc_hint,
					!sb->round_robin);
		if (nr != -1)
			break;
		if (!sbitmap_deferred_clear(map))
			break;
	} while (1);

	return nr;
}

static int __sbitmap_get(struct sbitmap *sb, unsigned int alloc_hint)
{
	unsigned int i, index;
	int nr = -1;

	index = SB_NR_TO_INDEX(sb, alloc_hint);

	/*
	 * Unless we're doing round robin tag allocation, just use the
	 * alloc_hint to find the right word index. No point in looping
	 * twice in find_next_zero_bit() for that case.
	 */
	if (sb->round_robin)
		alloc_hint = SB_NR_TO_BIT(sb, alloc_hint);
	else
		alloc_hint = 0;

	for (i = 0; i < sb->map_nr; i++) {
		nr = sbitmap_find_bit_in_index(sb, index, alloc_hint);
		if (nr != -1) {
			nr += index << sb->shift;
			break;
		}

		/* Jump to next index. */
		alloc_hint = 0;
		if (++index >= sb->map_nr)
			index = 0;
	}

	return nr;
}

int sbitmap_get(struct sbitmap *sb)
{
	int nr;
	unsigned int hint, depth;

	if (WARN_ON_ONCE(unlikely(!sb->alloc_hint)))
		return -1;

	depth = READ_ONCE(sb->depth);
	hint = update_alloc_hint_before_get(sb, depth);
	nr = __sbitmap_get(sb, hint);
	update_alloc_hint_after_get(sb, depth, hint, nr);

	return nr;
}
EXPORT_SYMBOL_GPL(sbitmap_get);

static int __sbitmap_get_shallow(struct sbitmap *sb,
				 unsigned int alloc_hint,
				 unsigned long shallow_depth)
{
	unsigned int i, index;
	int nr = -1;

	index = SB_NR_TO_INDEX(sb, alloc_hint);

	for (i = 0; i < sb->map_nr; i++) {
again:
		nr = __sbitmap_get_word(&sb->map[index].word,
					min(sb->map[index].depth, shallow_depth),
					SB_NR_TO_BIT(sb, alloc_hint), true);
		if (nr != -1) {
			nr += index << sb->shift;
			break;
		}

		if (sbitmap_deferred_clear(&sb->map[index]))
			goto again;

		/* Jump to next index. */
		index++;
		alloc_hint = index << sb->shift;

		if (index >= sb->map_nr) {
			index = 0;
			alloc_hint = 0;
		}
	}

	return nr;
}

int sbitmap_get_shallow(struct sbitmap *sb, unsigned long shallow_depth)
{
	int nr;
	unsigned int hint, depth;

	if (WARN_ON_ONCE(unlikely(!sb->alloc_hint)))
		return -1;

	depth = READ_ONCE(sb->depth);
	hint = update_alloc_hint_before_get(sb, depth);
	nr = __sbitmap_get_shallow(sb, hint, shallow_depth);
	update_alloc_hint_after_get(sb, depth, hint, nr);

	return nr;
}
EXPORT_SYMBOL_GPL(sbitmap_get_shallow);

bool sbitmap_any_bit_set(const struct sbitmap *sb)
{
	unsigned int i;

	for (i = 0; i < sb->map_nr; i++) {
		if (sb->map[i].word & ~sb->map[i].cleared)
			return true;
	}
	return false;
}
EXPORT_SYMBOL_GPL(sbitmap_any_bit_set);

static unsigned int __sbitmap_weight(const struct sbitmap *sb, bool set)
{
	unsigned int i, weight = 0;

	for (i = 0; i < sb->map_nr; i++) {
		const struct sbitmap_word *word = &sb->map[i];

		if (set)
			weight += bitmap_weight(&word->word, word->depth);
		else
			weight += bitmap_weight(&word->cleared, word->depth);
	}
	return weight;
}

static unsigned int sbitmap_cleared(const struct sbitmap *sb)
{
	return __sbitmap_weight(sb, false);
}

unsigned int sbitmap_weight(const struct sbitmap *sb)
{
	return __sbitmap_weight(sb, true) - sbitmap_cleared(sb);
}
EXPORT_SYMBOL_GPL(sbitmap_weight);

void sbitmap_show(struct sbitmap *sb, struct seq_file *m)
{
	seq_printf(m, "depth=%u\n", sb->depth);
	seq_printf(m, "busy=%u\n", sbitmap_weight(sb));
	seq_printf(m, "cleared=%u\n", sbitmap_cleared(sb));
	seq_printf(m, "bits_per_word=%u\n", 1U << sb->shift);
	seq_printf(m, "map_nr=%u\n", sb->map_nr);
}
EXPORT_SYMBOL_GPL(sbitmap_show);

static inline void emit_byte(struct seq_file *m, unsigned int offset, u8 byte)
{
	if ((offset & 0xf) == 0) {
		if (offset != 0)
			seq_putc(m, '\n');
		seq_printf(m, "%08x:", offset);
	}
	if ((offset & 0x1) == 0)
		seq_putc(m, ' ');
	seq_printf(m, "%02x", byte);
}

void sbitmap_bitmap_show(struct sbitmap *sb, struct seq_file *m)
{
	u8 byte = 0;
	unsigned int byte_bits = 0;
	unsigned int offset = 0;
	int i;

	for (i = 0; i < sb->map_nr; i++) {
		unsigned long word = READ_ONCE(sb->map[i].word);
		unsigned long cleared = READ_ONCE(sb->map[i].cleared);
		unsigned int word_bits = READ_ONCE(sb->map[i].depth);

		word &= ~cleared;

		while (word_bits > 0) {
			unsigned int bits = min(8 - byte_bits, word_bits);

			byte |= (word & (BIT(bits) - 1)) << byte_bits;
			byte_bits += bits;
			if (byte_bits == 8) {
				emit_byte(m, offset, byte);
				byte = 0;
				byte_bits = 0;
				offset++;
			}
			word >>= bits;
			word_bits -= bits;
		}
	}
	if (byte_bits) {
		emit_byte(m, offset, byte);
		offset++;
	}
	if (offset)
		seq_putc(m, '\n');
}
EXPORT_SYMBOL_GPL(sbitmap_bitmap_show);

static unsigned int sbq_calc_wake_batch(struct sbitmap_queue *sbq,
					unsigned int depth)
{
	unsigned int wake_batch;
	unsigned int shallow_depth;

	/*
	 * For each batch, we wake up one queue. We need to make sure that our
	 * batch size is small enough that the full depth of the bitmap,
	 * potentially limited by a shallow depth, is enough to wake up all of
	 * the queues.
	 *
	 * Each full word of the bitmap has bits_per_word bits, and there might
	 * be a partial word. There are depth / bits_per_word full words and
	 * depth % bits_per_word bits left over. In bitwise arithmetic:
	 *
	 * bits_per_word = 1 << shift
	 * depth / bits_per_word = depth >> shift
	 * depth % bits_per_word = depth & ((1 << shift) - 1)
	 *
	 * Each word can be limited to sbq->min_shallow_depth bits.
	 */
	shallow_depth = min(1U << sbq->sb.shift, sbq->min_shallow_depth);
	depth = ((depth >> sbq->sb.shift) * shallow_depth +
		 min(depth & ((1U << sbq->sb.shift) - 1), shallow_depth));
	wake_batch = clamp_t(unsigned int, depth / SBQ_WAIT_QUEUES, 1,
			     SBQ_WAKE_BATCH);

	return wake_batch;
}

int sbitmap_queue_init_node(struct sbitmap_queue *sbq, unsigned int depth,
			    int shift, bool round_robin, gfp_t flags, int node)
{
	int ret;
	int i;

	ret = sbitmap_init_node(&sbq->sb, depth, shift, flags, node,
				round_robin, true);
	if (ret)
		return ret;

	sbq->min_shallow_depth = UINT_MAX;
	sbq->wake_batch = sbq_calc_wake_batch(sbq, depth);
	atomic_set(&sbq->wake_index, 0);
	atomic_set(&sbq->ws_active, 0);

	sbq->ws = kzalloc_node(SBQ_WAIT_QUEUES * sizeof(*sbq->ws), flags, node);
	if (!sbq->ws) {
		sbitmap_free(&sbq->sb);
		return -ENOMEM;
	}

	for (i = 0; i < SBQ_WAIT_QUEUES; i++) {
		init_waitqueue_head(&sbq->ws[i].wait);
		atomic_set(&sbq->ws[i].wait_cnt, sbq->wake_batch);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(sbitmap_queue_init_node);

static void sbitmap_queue_update_wake_batch(struct sbitmap_queue *sbq,
					    unsigned int depth)
{
	unsigned int wake_batch = sbq_calc_wake_batch(sbq, depth);
	int i;

	if (sbq->wake_batch != wake_batch) {
		WRITE_ONCE(sbq->wake_batch, wake_batch);
		/*
		 * Pairs with the memory barrier in sbitmap_queue_wake_up()
		 * to ensure that the batch size is updated before the wait
		 * counts.
		 */
		smp_mb();
		for (i = 0; i < SBQ_WAIT_QUEUES; i++)
			atomic_set(&sbq->ws[i].wait_cnt, 1);
	}
}

void sbitmap_queue_resize(struct sbitmap_queue *sbq, unsigned int depth)
{
	sbitmap_queue_update_wake_batch(sbq, depth);
	sbitmap_resize(&sbq->sb, depth);
}
EXPORT_SYMBOL_GPL(sbitmap_queue_resize);

int __sbitmap_queue_get(struct sbitmap_queue *sbq)
{
	return sbitmap_get(&sbq->sb);
}
EXPORT_SYMBOL_GPL(__sbitmap_queue_get);

int __sbitmap_queue_get_shallow(struct sbitmap_queue *sbq,
				unsigned int shallow_depth)
{
	WARN_ON_ONCE(shallow_depth < sbq->min_shallow_depth);

	return sbitmap_get_shallow(&sbq->sb, shallow_depth);
}
EXPORT_SYMBOL_GPL(__sbitmap_queue_get_shallow);

void sbitmap_queue_min_shallow_depth(struct sbitmap_queue *sbq,
				     unsigned int min_shallow_depth)
{
	sbq->min_shallow_depth = min_shallow_depth;
	sbitmap_queue_update_wake_batch(sbq, sbq->sb.depth);
}
EXPORT_SYMBOL_GPL(sbitmap_queue_min_shallow_depth);

static struct sbq_wait_state *sbq_wake_ptr(struct sbitmap_queue *sbq)
{
	int i, wake_index;

	if (!atomic_read(&sbq->ws_active))
		return NULL;

	wake_index = atomic_read(&sbq->wake_index);
	for (i = 0; i < SBQ_WAIT_QUEUES; i++) {
		struct sbq_wait_state *ws = &sbq->ws[wake_index];

		if (waitqueue_active(&ws->wait)) {
			if (wake_index != atomic_read(&sbq->wake_index))
				atomic_set(&sbq->wake_index, wake_index);
			return ws;
		}

		wake_index = sbq_index_inc(wake_index);
	}

	return NULL;
}

static bool __sbq_wake_up(struct sbitmap_queue *sbq)
{
	struct sbq_wait_state *ws;
	unsigned int wake_batch;
	int wait_cnt;

	ws = sbq_wake_ptr(sbq);
	if (!ws)
		return false;

	wait_cnt = atomic_dec_return(&ws->wait_cnt);
	if (wait_cnt <= 0) {
		int ret;

		wake_batch = READ_ONCE(sbq->wake_batch);

		/*
		 * Pairs with the memory barrier in sbitmap_queue_resize() to
		 * ensure that we see the batch size update before the wait
		 * count is reset.
		 */
		smp_mb__before_atomic();

		/*
		 * For concurrent callers of this, the one that failed the
		 * atomic_cmpxhcg() race should call this function again
		 * to wakeup a new batch on a different 'ws'.
		 */
		ret = atomic_cmpxchg(&ws->wait_cnt, wait_cnt, wake_batch);
		if (ret == wait_cnt) {
			sbq_index_atomic_inc(&sbq->wake_index);
			wake_up_nr(&ws->wait, wake_batch);
			return false;
		}

		return true;
	}

	return false;
}

void sbitmap_queue_wake_up(struct sbitmap_queue *sbq)
{
	while (__sbq_wake_up(sbq))
		;
}
EXPORT_SYMBOL_GPL(sbitmap_queue_wake_up);

void sbitmap_queue_clear(struct sbitmap_queue *sbq, unsigned int nr,
			 unsigned int cpu)
{
	/*
	 * Once the clear bit is set, the bit may be allocated out.
	 *
	 * Orders READ/WRITE on the asssociated instance(such as request
	 * of blk_mq) by this bit for avoiding race with re-allocation,
	 * and its pair is the memory barrier implied in __sbitmap_get_word.
	 *
	 * One invariant is that the clear bit has to be zero when the bit
	 * is in use.
	 */
	smp_mb__before_atomic();
	sbitmap_deferred_clear_bit(&sbq->sb, nr);

	/*
	 * Pairs with the memory barrier in set_current_state() to ensure the
	 * proper ordering of clear_bit_unlock()/waitqueue_active() in the waker
	 * and test_and_set_bit_lock()/prepare_to_wait()/finish_wait() in the
	 * waiter. See the comment on waitqueue_active().
	 */
	smp_mb__after_atomic();
	sbitmap_queue_wake_up(sbq);

	if (likely(!sbq->sb.round_robin && nr < sbq->sb.depth))
		*per_cpu_ptr(sbq->sb.alloc_hint, cpu) = nr;
}
EXPORT_SYMBOL_GPL(sbitmap_queue_clear);

void sbitmap_queue_wake_all(struct sbitmap_queue *sbq)
{
	int i, wake_index;

	/*
	 * Pairs with the memory barrier in set_current_state() like in
	 * sbitmap_queue_wake_up().
	 */
	smp_mb();
	wake_index = atomic_read(&sbq->wake_index);
	for (i = 0; i < SBQ_WAIT_QUEUES; i++) {
		struct sbq_wait_state *ws = &sbq->ws[wake_index];

		if (waitqueue_active(&ws->wait))
			wake_up(&ws->wait);

		wake_index = sbq_index_inc(wake_index);
	}
}
EXPORT_SYMBOL_GPL(sbitmap_queue_wake_all);

void sbitmap_queue_show(struct sbitmap_queue *sbq, struct seq_file *m)
{
	bool first;
	int i;

	sbitmap_show(&sbq->sb, m);

	seq_puts(m, "alloc_hint={");
	first = true;
	for_each_possible_cpu(i) {
		if (!first)
			seq_puts(m, ", ");
		first = false;
		seq_printf(m, "%u", *per_cpu_ptr(sbq->sb.alloc_hint, i));
	}
	seq_puts(m, "}\n");

	seq_printf(m, "wake_batch=%u\n", sbq->wake_batch);
	seq_printf(m, "wake_index=%d\n", atomic_read(&sbq->wake_index));
	seq_printf(m, "ws_active=%d\n", atomic_read(&sbq->ws_active));

	seq_puts(m, "ws={\n");
	for (i = 0; i < SBQ_WAIT_QUEUES; i++) {
		struct sbq_wait_state *ws = &sbq->ws[i];

		seq_printf(m, "\t{.wait_cnt=%d, .wait=%s},\n",
			   atomic_read(&ws->wait_cnt),
			   waitqueue_active(&ws->wait) ? "active" : "inactive");
	}
	seq_puts(m, "}\n");

	seq_printf(m, "round_robin=%d\n", sbq->sb.round_robin);
	seq_printf(m, "min_shallow_depth=%u\n", sbq->min_shallow_depth);
}
EXPORT_SYMBOL_GPL(sbitmap_queue_show);

void sbitmap_add_wait_queue(struct sbitmap_queue *sbq,
			    struct sbq_wait_state *ws,
			    struct sbq_wait *sbq_wait)
{
	if (!sbq_wait->sbq) {
		sbq_wait->sbq = sbq;
		atomic_inc(&sbq->ws_active);
		add_wait_queue(&ws->wait, &sbq_wait->wait);
	}
}
EXPORT_SYMBOL_GPL(sbitmap_add_wait_queue);

void sbitmap_del_wait_queue(struct sbq_wait *sbq_wait)
{
	list_del_init(&sbq_wait->wait.entry);
	if (sbq_wait->sbq) {
		atomic_dec(&sbq_wait->sbq->ws_active);
		sbq_wait->sbq = NULL;
	}
}
EXPORT_SYMBOL_GPL(sbitmap_del_wait_queue);

void sbitmap_prepare_to_wait(struct sbitmap_queue *sbq,
			     struct sbq_wait_state *ws,
			     struct sbq_wait *sbq_wait, int state)
{
	if (!sbq_wait->sbq) {
		atomic_inc(&sbq->ws_active);
		sbq_wait->sbq = sbq;
	}
	prepare_to_wait_exclusive(&ws->wait, &sbq_wait->wait, state);
}
EXPORT_SYMBOL_GPL(sbitmap_prepare_to_wait);

void sbitmap_finish_wait(struct sbitmap_queue *sbq, struct sbq_wait_state *ws,
			 struct sbq_wait *sbq_wait)
{
	finish_wait(&ws->wait, &sbq_wait->wait);
	if (sbq_wait->sbq) {
		atomic_dec(&sbq->ws_active);
		sbq_wait->sbq = NULL;
	}
}
EXPORT_SYMBOL_GPL(sbitmap_finish_wait);

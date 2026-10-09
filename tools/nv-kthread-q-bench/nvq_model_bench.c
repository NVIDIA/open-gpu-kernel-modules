#define _GNU_SOURCE

#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef void (*bench_fn_t)(void *arg);

typedef struct bench_item
{
    struct bench_item *next;
    bench_fn_t fn;
    void *arg;
    bool pending;
} bench_item_t;

typedef struct completion_state
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    atomic_ullong completed;
    unsigned long long target;
} completion_state_t;

typedef struct producer_gate
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    bool start;
} producer_gate_t;

typedef struct producer_args
{
    void *queue;
    bench_item_t *items;
    size_t item_count;
    producer_gate_t *gate;
    int (*schedule)(void *queue, bench_item_t *item);
} producer_args_t;

typedef struct legacy_queue
{
    pthread_mutex_t lock;
    sem_t sem;
    pthread_t thread;
    bench_item_t *head;
    bench_item_t *tail;
    bool stop;
} legacy_queue_t;

typedef struct batched_queue
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t thread;
    bench_item_t *head;
    bench_item_t *tail;
    size_t pending_count;
    bool stop;
} batched_queue_t;

typedef struct bench_impl
{
    const char *name;
    int (*init)(void *queue);
    void (*stop)(void *queue);
    int (*schedule)(void *queue, bench_item_t *item);
    size_t queue_size;
} bench_impl_t;

typedef struct bench_result
{
    uint64_t best_ns;
    uint64_t median_ns;
    uint64_t total_ns;
} bench_result_t;

static uint64_t monotonic_ns(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0) {
        perror("clock_gettime");
        exit(1);
    }

    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static void completion_state_init(completion_state_t *state, unsigned long long target)
{
    pthread_mutex_init(&state->lock, NULL);
    pthread_cond_init(&state->cond, NULL);
    atomic_store(&state->completed, 0);
    state->target = target;
}

static void completion_state_destroy(completion_state_t *state)
{
    pthread_cond_destroy(&state->cond);
    pthread_mutex_destroy(&state->lock);
}

static void completion_wait(completion_state_t *state)
{
    pthread_mutex_lock(&state->lock);

    while (atomic_load(&state->completed) != state->target)
        pthread_cond_wait(&state->cond, &state->lock);

    pthread_mutex_unlock(&state->lock);
}

static void completion_callback(void *arg)
{
    completion_state_t *state = arg;
    unsigned long long completed = atomic_fetch_add(&state->completed, 1) + 1;

    if (completed == state->target) {
        pthread_mutex_lock(&state->lock);
        pthread_cond_signal(&state->cond);
        pthread_mutex_unlock(&state->lock);
    }
}

static void producer_gate_init(producer_gate_t *gate)
{
    pthread_mutex_init(&gate->lock, NULL);
    pthread_cond_init(&gate->cond, NULL);
    gate->start = false;
}

static void producer_gate_destroy(producer_gate_t *gate)
{
    pthread_cond_destroy(&gate->cond);
    pthread_mutex_destroy(&gate->lock);
}

static void producer_gate_open(producer_gate_t *gate)
{
    pthread_mutex_lock(&gate->lock);
    gate->start = true;
    pthread_cond_broadcast(&gate->cond);
    pthread_mutex_unlock(&gate->lock);
}

static void *producer_main(void *arg)
{
    producer_args_t *producer = arg;
    size_t i;

    pthread_mutex_lock(&producer->gate->lock);
    while (!producer->gate->start)
        pthread_cond_wait(&producer->gate->cond, &producer->gate->lock);
    pthread_mutex_unlock(&producer->gate->lock);

    for (i = 0; i < producer->item_count; ++i) {
        if (!producer->schedule(producer->queue, &producer->items[i])) {
            fprintf(stderr, "schedule failed at item %zu\n", i);
            return (void *)1;
        }
    }

    return NULL;
}

static void queue_push_locked(bench_item_t **head, bench_item_t **tail, bench_item_t *item)
{
    item->next = NULL;

    if (*tail)
        (*tail)->next = item;
    else
        *head = item;

    *tail = item;
}

static bench_item_t *queue_pop_locked(bench_item_t **head, bench_item_t **tail)
{
    bench_item_t *item = *head;

    if (!item)
        return NULL;

    *head = item->next;
    if (!*head)
        *tail = NULL;

    item->next = NULL;
    item->pending = false;
    return item;
}

static bench_item_t *queue_take_all_locked(bench_item_t **head, bench_item_t **tail)
{
    bench_item_t *items = *head;

    *head = NULL;
    *tail = NULL;

    return items;
}

static void *legacy_worker_main(void *arg)
{
    legacy_queue_t *queue = arg;

    for (;;) {
        bench_item_t *item;

        while (sem_wait(&queue->sem) != 0) {
            if (errno != EINTR) {
                perror("sem_wait");
                return (void *)1;
            }
        }

        pthread_mutex_lock(&queue->lock);

        item = queue_pop_locked(&queue->head, &queue->tail);
        if (!item && queue->stop) {
            pthread_mutex_unlock(&queue->lock);
            break;
        }

        pthread_mutex_unlock(&queue->lock);

        if (item)
            item->fn(item->arg);
    }

    return NULL;
}

static int legacy_init(void *opaque)
{
    legacy_queue_t *queue = opaque;

    memset(queue, 0, sizeof(*queue));

    if (pthread_mutex_init(&queue->lock, NULL) != 0)
        return -1;
    if (sem_init(&queue->sem, 0, 0) != 0)
        return -1;
    if (pthread_create(&queue->thread, NULL, legacy_worker_main, queue) != 0)
        return -1;

    return 0;
}

static void legacy_stop(void *opaque)
{
    legacy_queue_t *queue = opaque;

    pthread_mutex_lock(&queue->lock);
    queue->stop = true;
    pthread_mutex_unlock(&queue->lock);

    sem_post(&queue->sem);
    pthread_join(queue->thread, NULL);
    sem_destroy(&queue->sem);
    pthread_mutex_destroy(&queue->lock);
}

static int legacy_schedule(void *opaque, bench_item_t *item)
{
    legacy_queue_t *queue = opaque;

    pthread_mutex_lock(&queue->lock);

    if (item->pending) {
        pthread_mutex_unlock(&queue->lock);
        return 0;
    }

    item->pending = true;
    queue_push_locked(&queue->head, &queue->tail, item);

    pthread_mutex_unlock(&queue->lock);

    sem_post(&queue->sem);
    return 1;
}

static void *batched_worker_main(void *arg)
{
    batched_queue_t *queue = arg;

    for (;;) {
        bench_item_t *items;

        pthread_mutex_lock(&queue->lock);

        while (!queue->stop && queue->pending_count == 0)
            pthread_cond_wait(&queue->cond, &queue->lock);

        if (queue->stop && queue->pending_count == 0) {
            pthread_mutex_unlock(&queue->lock);
            break;
        }

        items = queue_take_all_locked(&queue->head, &queue->tail);
        queue->pending_count = 0;

        pthread_mutex_unlock(&queue->lock);

        while (items) {
            bench_item_t *item = items;

            items = items->next;
            item->next = NULL;
            item->pending = false;
            item->fn(item->arg);
        }
    }

    return NULL;
}

static int batched_init(void *opaque)
{
    batched_queue_t *queue = opaque;

    memset(queue, 0, sizeof(*queue));

    if (pthread_mutex_init(&queue->lock, NULL) != 0)
        return -1;
    if (pthread_cond_init(&queue->cond, NULL) != 0)
        return -1;
    if (pthread_create(&queue->thread, NULL, batched_worker_main, queue) != 0)
        return -1;

    return 0;
}

static void batched_stop(void *opaque)
{
    batched_queue_t *queue = opaque;

    pthread_mutex_lock(&queue->lock);
    queue->stop = true;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);

    pthread_join(queue->thread, NULL);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->lock);
}

static int batched_schedule(void *opaque, bench_item_t *item)
{
    batched_queue_t *queue = opaque;
    bool should_wake = false;

    pthread_mutex_lock(&queue->lock);

    if (item->pending) {
        pthread_mutex_unlock(&queue->lock);
        return 0;
    }

    item->pending = true;
    queue_push_locked(&queue->head, &queue->tail, item);
    should_wake = (queue->pending_count++ == 0);

    if (should_wake)
        pthread_cond_signal(&queue->cond);

    pthread_mutex_unlock(&queue->lock);

    return 1;
}

static const bench_impl_t g_impls[] = {
    {
        .name = "legacy",
        .init = legacy_init,
        .stop = legacy_stop,
        .schedule = legacy_schedule,
        .queue_size = sizeof(legacy_queue_t),
    },
    {
        .name = "batched",
        .init = batched_init,
        .stop = batched_stop,
        .schedule = batched_schedule,
        .queue_size = sizeof(batched_queue_t),
    },
};

static int compare_u64(const void *lhs, const void *rhs)
{
    const uint64_t left = *(const uint64_t *)lhs;
    const uint64_t right = *(const uint64_t *)rhs;

    return (left > right) - (left < right);
}

static bench_result_t run_single_bench(const bench_impl_t *impl, size_t item_count, unsigned int repeats)
{
    bench_result_t result = {
        .best_ns = UINT64_MAX,
        .median_ns = 0,
        .total_ns = 0,
    };
    bench_item_t *items = calloc(item_count, sizeof(*items));
    char *queue_storage = calloc(1, impl->queue_size);
    uint64_t *samples = calloc(repeats, sizeof(*samples));
    unsigned int repeat;

    if (!items || !queue_storage || !samples) {
        fprintf(stderr, "allocation failed in single benchmark\n");
        exit(1);
    }

    if (impl->init(queue_storage) != 0) {
        fprintf(stderr, "queue init failed for %s\n", impl->name);
        exit(1);
    }

    for (repeat = 0; repeat < repeats; ++repeat) {
        completion_state_t completion;
        uint64_t start_ns;
        uint64_t end_ns;
        size_t i;

        completion_state_init(&completion, item_count);

        for (i = 0; i < item_count; ++i) {
            items[i].next = NULL;
            items[i].fn = completion_callback;
            items[i].arg = &completion;
            items[i].pending = false;
        }

        start_ns = monotonic_ns();

        for (i = 0; i < item_count; ++i) {
            if (!impl->schedule(queue_storage, &items[i])) {
                fprintf(stderr, "single schedule failed for %s\n", impl->name);
                exit(1);
            }
        }

        completion_wait(&completion);
        end_ns = monotonic_ns();

        samples[repeat] = end_ns - start_ns;
        if (samples[repeat] < result.best_ns)
            result.best_ns = samples[repeat];
        result.total_ns += samples[repeat];

        completion_state_destroy(&completion);
    }

    qsort(samples, repeats, sizeof(*samples), compare_u64);
    result.median_ns = samples[repeats / 2];

    impl->stop(queue_storage);
    free(samples);
    free(queue_storage);
    free(items);

    return result;
}

static bench_result_t run_multi_bench(const bench_impl_t *impl,
                                      size_t item_count,
                                      unsigned int producers,
                                      unsigned int repeats)
{
    bench_result_t result = {
        .best_ns = UINT64_MAX,
        .median_ns = 0,
        .total_ns = 0,
    };
    bench_item_t *items = calloc(item_count, sizeof(*items));
    char *queue_storage = calloc(1, impl->queue_size);
    uint64_t *samples = calloc(repeats, sizeof(*samples));
    unsigned int repeat;

    if (!items || !queue_storage || !samples) {
        fprintf(stderr, "allocation failed in multi benchmark\n");
        exit(1);
    }

    if (impl->init(queue_storage) != 0) {
        fprintf(stderr, "queue init failed for %s\n", impl->name);
        exit(1);
    }

    for (repeat = 0; repeat < repeats; ++repeat) {
        completion_state_t completion;
        producer_gate_t gate;
        pthread_t *threads;
        producer_args_t *thread_args;
        size_t base_chunk = item_count / producers;
        size_t remainder = item_count % producers;
        uint64_t start_ns;
        uint64_t end_ns;
        unsigned int i;
        size_t offset = 0;

        threads = calloc(producers, sizeof(*threads));
        thread_args = calloc(producers, sizeof(*thread_args));
        if (!threads || !thread_args) {
            fprintf(stderr, "thread allocation failed\n");
            exit(1);
        }

        completion_state_init(&completion, item_count);
        producer_gate_init(&gate);

        for (i = 0; i < item_count; ++i) {
            items[i].next = NULL;
            items[i].fn = completion_callback;
            items[i].arg = &completion;
            items[i].pending = false;
        }

        for (i = 0; i < producers; ++i) {
            size_t chunk = base_chunk + (i < remainder ? 1 : 0);

            thread_args[i].queue = queue_storage;
            thread_args[i].items = &items[offset];
            thread_args[i].item_count = chunk;
            thread_args[i].gate = &gate;
            thread_args[i].schedule = impl->schedule;
            offset += chunk;

            if (pthread_create(&threads[i], NULL, producer_main, &thread_args[i]) != 0) {
                fprintf(stderr, "pthread_create failed\n");
                exit(1);
            }
        }

        start_ns = monotonic_ns();
        producer_gate_open(&gate);
        completion_wait(&completion);
        end_ns = monotonic_ns();

        for (i = 0; i < producers; ++i) {
            void *thread_ret = NULL;

            pthread_join(threads[i], &thread_ret);
            if (thread_ret != NULL) {
                fprintf(stderr, "producer thread failed\n");
                exit(1);
            }
        }

        samples[repeat] = end_ns - start_ns;
        if (samples[repeat] < result.best_ns)
            result.best_ns = samples[repeat];
        result.total_ns += samples[repeat];

        producer_gate_destroy(&gate);
        completion_state_destroy(&completion);
        free(thread_args);
        free(threads);
    }

    qsort(samples, repeats, sizeof(*samples), compare_u64);
    result.median_ns = samples[repeats / 2];

    impl->stop(queue_storage);
    free(samples);
    free(queue_storage);
    free(items);

    return result;
}

static void print_result(const char *label,
                         const bench_impl_t *impl,
                         size_t item_count,
                         unsigned int repeats,
                         bench_result_t result)
{
    const double best_ns_per_item = (double)result.best_ns / (double)item_count;
    const double median_ns_per_item = (double)result.median_ns / (double)item_count;
    const double avg_ns_per_item = (double)result.total_ns / (double)(item_count * repeats);

    printf("%-7s %-6s items=%zu repeats=%u best_ns=%" PRIu64 " best_ns_per_item=%.2f median_ns_per_item=%.2f avg_ns_per_item=%.2f\n",
           impl->name,
           label,
           item_count,
           repeats,
           result.best_ns,
           best_ns_per_item,
            median_ns_per_item,
           avg_ns_per_item);
}

int main(int argc, char **argv)
{
    size_t single_items = 200000;
    size_t multi_items = 400000;
    unsigned int producers = 8;
    unsigned int repeats = 5;
    bench_result_t single_results[sizeof(g_impls) / sizeof(g_impls[0])];
    bench_result_t multi_results[sizeof(g_impls) / sizeof(g_impls[0])];
    size_t i;

    if (argc > 1)
        single_items = strtoull(argv[1], NULL, 0);
    if (argc > 2)
        multi_items = strtoull(argv[2], NULL, 0);
    if (argc > 3)
        producers = (unsigned int)strtoul(argv[3], NULL, 0);
    if (argc > 4)
        repeats = (unsigned int)strtoul(argv[4], NULL, 0);

    if (single_items == 0 || multi_items == 0 || producers == 0 || repeats == 0) {
        fprintf(stderr, "usage: %s [single_items] [multi_items] [producers] [repeats]\n", argv[0]);
        return 1;
    }

    puts("Benchmarking queue model");

    for (i = 0; i < sizeof(g_impls) / sizeof(g_impls[0]); ++i) {
        single_results[i] = run_single_bench(&g_impls[i], single_items, repeats);
        multi_results[i] = run_multi_bench(&g_impls[i], multi_items, producers, repeats);

        print_result("single", &g_impls[i], single_items, repeats, single_results[i]);
        print_result("multi", &g_impls[i], multi_items, repeats, multi_results[i]);
    }

    if (single_results[1].best_ns != 0 && multi_results[1].best_ns != 0) {
        printf("speedup single best=%.2fx avg=%.2fx\n",
               (double)single_results[0].best_ns / (double)single_results[1].best_ns,
               (double)single_results[0].total_ns / (double)single_results[1].total_ns);
        printf("speedup single median=%.2fx\n",
               (double)single_results[0].median_ns / (double)single_results[1].median_ns);
        printf("speedup multi  best=%.2fx avg=%.2fx\n",
               (double)multi_results[0].best_ns / (double)multi_results[1].best_ns,
               (double)multi_results[0].total_ns / (double)multi_results[1].total_ns);
        printf("speedup multi  median=%.2fx\n",
               (double)multi_results[0].median_ns / (double)multi_results[1].median_ns);
    }

    return 0;
}

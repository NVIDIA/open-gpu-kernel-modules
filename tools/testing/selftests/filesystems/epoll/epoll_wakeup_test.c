// SPDX-License-Identifier: GPL-2.0

#define _GNU_SOURCE
#include <asm/unistd.h>
#include <linux/time_types.h>
#include <poll.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include "../../kselftest_harness.h"

struct epoll_mtcontext
{
	int efd[3];
	int sfd[4];
	volatile int count;

	pthread_t main;
	pthread_t waiter;
};

#ifndef __NR_epoll_pwait2
#define __NR_epoll_pwait2 -1
#endif

static inline int sys_epoll_pwait2(int fd, struct epoll_event *events,
				   int maxevents,
				   const struct __kernel_timespec *timeout,
				   const sigset_t *sigset, size_t sigsetsize)
{
	return syscall(__NR_epoll_pwait2, fd, events, maxevents, timeout,
		       sigset, sigsetsize);
}

static void signal_handler(int signum)
{
}

static void kill_timeout(struct epoll_mtcontext *ctx)
{
	usleep(1000000);
	pthread_kill(ctx->main, SIGUSR1);
	pthread_kill(ctx->waiter, SIGUSR1);
}

static void *waiter_entry1a(void *data)
{
	struct epoll_event e;
	struct epoll_mtcontext *ctx = data;

	if (epoll_wait(ctx->efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx->count, 1);

	return NULL;
}

static void *waiter_entry1ap(void *data)
{
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext *ctx = data;

	pfd.fd = ctx->efd[0];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx->efd[0], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx->count, 1);
	}

	return NULL;
}

static void *waiter_entry1o(void *data)
{
	struct epoll_event e;
	struct epoll_mtcontext *ctx = data;

	if (epoll_wait(ctx->efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx->count, 1);

	return NULL;
}

static void *waiter_entry1op(void *data)
{
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext *ctx = data;

	pfd.fd = ctx->efd[0];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx->efd[0], &e, 1, 0) > 0)
			__sync_fetch_and_or(&ctx->count, 1);
	}

	return NULL;
}

static void *waiter_entry2a(void *data)
{
	struct epoll_event events[2];
	struct epoll_mtcontext *ctx = data;

	if (epoll_wait(ctx->efd[0], events, 2, -1) > 0)
		__sync_fetch_and_add(&ctx->count, 1);

	return NULL;
}

static void *waiter_entry2ap(void *data)
{
	struct pollfd pfd;
	struct epoll_event events[2];
	struct epoll_mtcontext *ctx = data;

	pfd.fd = ctx->efd[0];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx->efd[0], events, 2, 0) > 0)
			__sync_fetch_and_add(&ctx->count, 1);
	}

	return NULL;
}

static void *emitter_entry1(void *data)
{
	struct epoll_mtcontext *ctx = data;

	usleep(100000);
	write(ctx->sfd[1], "w", 1);

	kill_timeout(ctx);

	return NULL;
}

static void *emitter_entry2(void *data)
{
	struct epoll_mtcontext *ctx = data;

	usleep(100000);
	write(ctx->sfd[1], "w", 1);
	write(ctx->sfd[3], "w", 1);

	kill_timeout(ctx);

	return NULL;
}

/*
 *          t0
 *           | (ew)
 *          e0
 *           | (lt)
 *          s0
 */
TEST(epoll1)
{
	int efd;
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd, &e, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd, &e, 1, 0), 1);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (ew)
 *          e0
 *           | (et)
 *          s0
 */
TEST(epoll2)
{
	int efd;
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd, &e, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd, &e, 1, 0), 0);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *           t0
 *            | (ew)
 *           e0
 *     (lt) /  \ (lt)
 *        s0    s2
 */
TEST(epoll3)
{
	int efd;
	int sfd[4];
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 2);
	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 2);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *           t0
 *            | (ew)
 *           e0
 *     (et) /  \ (et)
 *        s0    s2
 */
TEST(epoll4)
{
	int efd;
	int sfd[4];
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 2);
	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 0);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *          t0
 *           | (p)
 *          e0
 *           | (lt)
 *          s0
 */
TEST(epoll5)
{
	int efd;
	int sfd[2];
	struct pollfd pfd;
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	pfd.fd = efd;
	pfd.events = POLLIN;
	ASSERT_EQ(poll(&pfd, 1, 0), 1);
	ASSERT_EQ(epoll_wait(efd, &e, 1, 0), 1);

	pfd.fd = efd;
	pfd.events = POLLIN;
	ASSERT_EQ(poll(&pfd, 1, 0), 1);
	ASSERT_EQ(epoll_wait(efd, &e, 1, 0), 1);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (p)
 *          e0
 *           | (et)
 *          s0
 */
TEST(epoll6)
{
	int efd;
	int sfd[2];
	struct pollfd pfd;
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	pfd.fd = efd;
	pfd.events = POLLIN;
	ASSERT_EQ(poll(&pfd, 1, 0), 1);
	ASSERT_EQ(epoll_wait(efd, &e, 1, 0), 1);

	pfd.fd = efd;
	pfd.events = POLLIN;
	ASSERT_EQ(poll(&pfd, 1, 0), 0);
	ASSERT_EQ(epoll_wait(efd, &e, 1, 0), 0);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *           t0
 *            | (p)
 *           e0
 *     (lt) /  \ (lt)
 *        s0    s2
 */

TEST(epoll7)
{
	int efd;
	int sfd[4];
	struct pollfd pfd;
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	pfd.fd = efd;
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 2);

	pfd.fd = efd;
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 2);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *           t0
 *            | (p)
 *           e0
 *     (et) /  \ (et)
 *        s0    s2
 */
TEST(epoll8)
{
	int efd;
	int sfd[4];
	struct pollfd pfd;
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	pfd.fd = efd;
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 2);

	pfd.fd = efd;
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 0);
	EXPECT_EQ(epoll_wait(efd, events, 2, 0), 0);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (lt)
 *           s0
 */
TEST(epoll9)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (et)
 *           s0
 */
TEST(epoll10)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 1);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *     (lt) /  \ (lt)
 *        s0    s2
 */
TEST(epoll11)
{
	pthread_t emitter;
	struct epoll_event events[2];
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[2], events), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry2a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], events, 2, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *     (et) /  \ (et)
 *        s0    s2
 */
TEST(epoll12)
{
	pthread_t emitter;
	struct epoll_event events[2];
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[2], events), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], events, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *            | (lt)
 *           s0
 */
TEST(epoll13)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *            | (et)
 *           s0
 */
TEST(epoll14)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 1);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *     (lt) /  \ (lt)
 *        s0    s2
 */
TEST(epoll15)
{
	pthread_t emitter;
	struct epoll_event events[2];
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[2], events), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry2ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], events, 2, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *     (et) /  \ (et)
 *        s0    s2
 */
TEST(epoll16)
{
	pthread_t emitter;
	struct epoll_event events[2];
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[2], events), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], events, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *          t0
 *           | (ew)
 *          e0
 *           | (lt)
 *          e1
 *           | (lt)
 *          s0
 */
TEST(epoll17)
{
	int efd[2];
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (ew)
 *          e0
 *           | (lt)
 *          e1
 *           | (et)
 *          s0
 */
TEST(epoll18)
{
	int efd[2];
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *           t0
 *            | (ew)
 *           e0
 *            | (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll19)
{
	int efd[2];
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 0);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *           t0
 *            | (ew)
 *           e0
 *            | (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll20)
{
	int efd[2];
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 0);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (p)
 *          e0
 *           | (lt)
 *          e1
 *           | (lt)
 *          s0
 */
TEST(epoll21)
{
	int efd[2];
	int sfd[2];
	struct pollfd pfd;
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (p)
 *          e0
 *           | (lt)
 *          e1
 *           | (et)
 *          s0
 */
TEST(epoll22)
{
	int efd[2];
	int sfd[2];
	struct pollfd pfd;
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (p)
 *          e0
 *           | (et)
 *          e1
 *           | (lt)
 *          s0
 */
TEST(epoll23)
{
	int efd[2];
	int sfd[2];
	struct pollfd pfd;
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 0);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 0);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *          t0
 *           | (p)
 *          e0
 *           | (et)
 *          e1
 *           | (et)
 *          s0
 */
TEST(epoll24)
{
	int efd[2];
	int sfd[2];
	struct pollfd pfd;
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 0);
	EXPECT_EQ(epoll_wait(efd[0], &e, 1, 0), 0);

	close(efd[0]);
	close(efd[1]);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (lt)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll25)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (lt)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll26)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll27)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 1);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll28)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 1);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *            | (lt)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll29)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *            | (lt)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll30)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *            | (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll31)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 1);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *            | (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll32)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 1);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (ew)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll33)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (ew)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll34)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1o, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx.count, 2);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (ew)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll35)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (ew)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll36)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1o, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx.count, 2);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (ew)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll37)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	pfd.fd = ctx.efd[1];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[1], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx.count, 1);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (ew)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll38)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1o, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	pfd.fd = ctx.efd[1];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[1], &e, 1, 0) > 0)
			__sync_fetch_and_or(&ctx.count, 2);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (ew)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll39)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	pfd.fd = ctx.efd[1];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[1], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx.count, 1);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (ew)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll40)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1o, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	pfd.fd = ctx.efd[1];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[1], &e, 1, 0) > 0)
			__sync_fetch_and_or(&ctx.count, 2);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (p)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll41)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (p)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll42)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1op, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx.count, 2);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (p)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll43)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *    (ew) |    | (p)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll44)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1op, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx.count, 2);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (p)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll45)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	pfd.fd = ctx.efd[1];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[1], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx.count, 1);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (p)
 *         |   e0
 *          \  / (lt)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll46)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1op, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx.count, 2);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (p)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (lt)
 *           s0
 */
TEST(epoll47)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	pfd.fd = ctx.efd[1];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[1], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx.count, 1);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *        t0   t1
 *     (p) |    | (p)
 *         |   e0
 *          \  / (et)
 *           e1
 *            | (et)
 *           s0
 */
TEST(epoll48)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1op, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry1, &ctx), 0);

	if (epoll_wait(ctx.efd[1], &e, 1, -1) > 0)
		__sync_fetch_and_or(&ctx.count, 2);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_TRUE((ctx.count == 2) || (ctx.count == 3));

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

/*
 *           t0
 *            | (ew)
 *           e0
 *     (lt) /  \ (lt)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll49)
{
	int efd[3];
	int sfd[4];
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	efd[2] = epoll_create(1);
	ASSERT_GE(efd[2], 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[2], EPOLL_CTL_ADD, sfd[2], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 2);
	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 2);

	close(efd[0]);
	close(efd[1]);
	close(efd[2]);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *           t0
 *            | (ew)
 *           e0
 *     (et) /  \ (et)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll50)
{
	int efd[3];
	int sfd[4];
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	efd[2] = epoll_create(1);
	ASSERT_GE(efd[2], 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[2], EPOLL_CTL_ADD, sfd[2], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 2);
	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 0);

	close(efd[0]);
	close(efd[1]);
	close(efd[2]);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *           t0
 *            | (p)
 *           e0
 *     (lt) /  \ (lt)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll51)
{
	int efd[3];
	int sfd[4];
	struct pollfd pfd;
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	efd[2] = epoll_create(1);
	ASSERT_GE(efd[2], 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[2], EPOLL_CTL_ADD, sfd[2], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 2);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 2);

	close(efd[0]);
	close(efd[1]);
	close(efd[2]);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *           t0
 *            | (p)
 *           e0
 *     (et) /  \ (et)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll52)
{
	int efd[3];
	int sfd[4];
	struct pollfd pfd;
	struct epoll_event events[2];

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &sfd[2]), 0);

	efd[0] = epoll_create(1);
	ASSERT_GE(efd[0], 0);

	efd[1] = epoll_create(1);
	ASSERT_GE(efd[1], 0);

	efd[2] = epoll_create(1);
	ASSERT_GE(efd[2], 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[1], EPOLL_CTL_ADD, sfd[0], events), 0);

	events[0].events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd[2], EPOLL_CTL_ADD, sfd[2], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[1], events), 0);

	events[0].events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(efd[0], EPOLL_CTL_ADD, efd[2], events), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);
	ASSERT_EQ(write(sfd[3], "w", 1), 1);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 1);
	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 2);

	pfd.fd = efd[0];
	pfd.events = POLLIN;
	EXPECT_EQ(poll(&pfd, 1, 0), 0);
	EXPECT_EQ(epoll_wait(efd[0], events, 2, 0), 0);

	close(efd[0]);
	close(efd[1]);
	close(efd[2]);
	close(sfd[0]);
	close(sfd[1]);
	close(sfd[2]);
	close(sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *     (lt) /  \ (lt)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll53)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	ctx.efd[2] = epoll_create(1);
	ASSERT_GE(ctx.efd[2], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[2], EPOLL_CTL_ADD, ctx.sfd[2], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[2], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.efd[2]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *     (et) /  \ (et)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll54)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	ctx.efd[2] = epoll_create(1);
	ASSERT_GE(ctx.efd[2], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[2], EPOLL_CTL_ADD, ctx.sfd[2], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[2], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.efd[2]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *     (lt) /  \ (lt)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll55)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	ctx.efd[2] = epoll_create(1);
	ASSERT_GE(ctx.efd[2], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[2], EPOLL_CTL_ADD, ctx.sfd[2], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[2], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.efd[2]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *     (ew) \  / (p)
 *           e0
 *     (et) /  \ (et)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll56)
{
	pthread_t emitter;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	ctx.efd[2] = epoll_create(1);
	ASSERT_GE(ctx.efd[2], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[2], EPOLL_CTL_ADD, ctx.sfd[2], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[2], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	if (epoll_wait(ctx.efd[0], &e, 1, -1) > 0)
		__sync_fetch_and_add(&ctx.count, 1);

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.efd[2]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *      (p) \  / (p)
 *           e0
 *     (lt) /  \ (lt)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll57)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	ctx.efd[2] = epoll_create(1);
	ASSERT_GE(ctx.efd[2], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[2], EPOLL_CTL_ADD, ctx.sfd[2], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[2], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	pfd.fd = ctx.efd[0];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[0], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx.count, 1);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.efd[2]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

/*
 *        t0    t1
 *      (p) \  / (p)
 *           e0
 *     (et) /  \ (et)
 *        e1    e2
 *    (lt) |     | (lt)
 *        s0    s2
 */
TEST(epoll58)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[0]), 0);
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, &ctx.sfd[2]), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.efd[1] = epoll_create(1);
	ASSERT_GE(ctx.efd[1], 0);

	ctx.efd[2] = epoll_create(1);
	ASSERT_GE(ctx.efd[2], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[1], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[2], EPOLL_CTL_ADD, ctx.sfd[2], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[1], &e), 0);

	e.events = EPOLLIN | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.efd[2], &e), 0);

	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&ctx.waiter, NULL, waiter_entry1ap, &ctx), 0);
	ASSERT_EQ(pthread_create(&emitter, NULL, emitter_entry2, &ctx), 0);

	pfd.fd = ctx.efd[0];
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) > 0) {
		if (epoll_wait(ctx.efd[0], &e, 1, 0) > 0)
			__sync_fetch_and_add(&ctx.count, 1);
	}

	ASSERT_EQ(pthread_join(ctx.waiter, NULL), 0);
	EXPECT_EQ(ctx.count, 2);

	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}

	close(ctx.efd[0]);
	close(ctx.efd[1]);
	close(ctx.efd[2]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
	close(ctx.sfd[2]);
	close(ctx.sfd[3]);
}

static void *epoll59_thread(void *ctx_)
{
	struct epoll_mtcontext *ctx = ctx_;
	struct epoll_event e;
	int i;

	for (i = 0; i < 100000; i++) {
		while (ctx->count == 0)
			;

		e.events = EPOLLIN | EPOLLERR | EPOLLET;
		epoll_ctl(ctx->efd[0], EPOLL_CTL_MOD, ctx->sfd[0], &e);
		ctx->count = 0;
	}

	return NULL;
}

/*
 *        t0
 *      (p) \
 *           e0
 *     (et) /
 *        e0
 *
 * Based on https://bugzilla.kernel.org/show_bug.cgi?id=205933
 */
TEST(epoll59)
{
	pthread_t emitter;
	struct pollfd pfd;
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };
	int i, ret;

	signal(SIGUSR1, signal_handler);

	ctx.efd[0] = epoll_create1(0);
	ASSERT_GE(ctx.efd[0], 0);

	ctx.sfd[0] = eventfd(1, 0);
	ASSERT_GE(ctx.sfd[0], 0);

	e.events = EPOLLIN | EPOLLERR | EPOLLET;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	ASSERT_EQ(pthread_create(&emitter, NULL, epoll59_thread, &ctx), 0);

	for (i = 0; i < 100000; i++) {
		ret = epoll_wait(ctx.efd[0], &e, 1, 1000);
		ASSERT_GT(ret, 0);

		while (ctx.count != 0)
			;
		ctx.count = 1;
	}
	if (pthread_tryjoin_np(emitter, NULL) < 0) {
		pthread_kill(emitter, SIGUSR1);
		pthread_join(emitter, NULL);
	}
	close(ctx.efd[0]);
	close(ctx.sfd[0]);
}

enum {
	EPOLL60_EVENTS_NR = 10,
};

struct epoll60_ctx {
	volatile int stopped;
	int ready;
	int waiters;
	int epfd;
	int evfd[EPOLL60_EVENTS_NR];
};

static void *epoll60_wait_thread(void *ctx_)
{
	struct epoll60_ctx *ctx = ctx_;
	struct epoll_event e;
	sigset_t sigmask;
	uint64_t v;
	int ret;

	/* Block SIGUSR1 */
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
	sigprocmask(SIG_SETMASK, &sigmask, NULL);

	/* Prepare empty mask for epoll_pwait() */
	sigemptyset(&sigmask);

	while (!ctx->stopped) {
		/* Mark we are ready */
		__atomic_fetch_add(&ctx->ready, 1, __ATOMIC_ACQUIRE);

		/* Start when all are ready */
		while (__atomic_load_n(&ctx->ready, __ATOMIC_ACQUIRE) &&
		       !ctx->stopped);

		/* Account this waiter */
		__atomic_fetch_add(&ctx->waiters, 1, __ATOMIC_ACQUIRE);

		ret = epoll_pwait(ctx->epfd, &e, 1, 2000, &sigmask);
		if (ret != 1) {
			/* We expect only signal delivery on stop */
			assert(ret < 0 && errno == EINTR && "Lost wakeup!\n");
			assert(ctx->stopped);
			break;
		}

		ret = read(e.data.fd, &v, sizeof(v));
		/* Since we are on ET mode, thus each thread gets its own fd. */
		assert(ret == sizeof(v));

		__atomic_fetch_sub(&ctx->waiters, 1, __ATOMIC_RELEASE);
	}

	return NULL;
}

static inline unsigned long long msecs(void)
{
	struct timespec ts;
	unsigned long long msecs;

	clock_gettime(CLOCK_REALTIME, &ts);
	msecs = ts.tv_sec * 1000ull;
	msecs += ts.tv_nsec / 1000000ull;

	return msecs;
}

static inline int count_waiters(struct epoll60_ctx *ctx)
{
	return __atomic_load_n(&ctx->waiters, __ATOMIC_ACQUIRE);
}

TEST(epoll60)
{
	struct epoll60_ctx ctx = { 0 };
	pthread_t waiters[ARRAY_SIZE(ctx.evfd)];
	struct epoll_event e;
	int i, n, ret;

	signal(SIGUSR1, signal_handler);

	ctx.epfd = epoll_create1(0);
	ASSERT_GE(ctx.epfd, 0);

	/* Create event fds */
	for (i = 0; i < ARRAY_SIZE(ctx.evfd); i++) {
		ctx.evfd[i] = eventfd(0, EFD_NONBLOCK);
		ASSERT_GE(ctx.evfd[i], 0);

		e.events = EPOLLIN | EPOLLET;
		e.data.fd = ctx.evfd[i];
		ASSERT_EQ(epoll_ctl(ctx.epfd, EPOLL_CTL_ADD, ctx.evfd[i], &e), 0);
	}

	/* Create waiter threads */
	for (i = 0; i < ARRAY_SIZE(waiters); i++)
		ASSERT_EQ(pthread_create(&waiters[i], NULL,
					 epoll60_wait_thread, &ctx), 0);

	for (i = 0; i < 300; i++) {
		uint64_t v = 1, ms;

		/* Wait for all to be ready */
		while (__atomic_load_n(&ctx.ready, __ATOMIC_ACQUIRE) !=
		       ARRAY_SIZE(ctx.evfd))
			;

		/* Steady, go */
		__atomic_fetch_sub(&ctx.ready, ARRAY_SIZE(ctx.evfd),
				   __ATOMIC_ACQUIRE);

		/* Wait all have gone to kernel */
		while (count_waiters(&ctx) != ARRAY_SIZE(ctx.evfd))
			;

		/* 1ms should be enough to schedule away */
		usleep(1000);

		/* Quickly signal all handles at once */
		for (n = 0; n < ARRAY_SIZE(ctx.evfd); n++) {
			ret = write(ctx.evfd[n], &v, sizeof(v));
			ASSERT_EQ(ret, sizeof(v));
		}

		/* Busy loop for 1s and wait for all waiters to wake up */
		ms = msecs();
		while (count_waiters(&ctx) && msecs() < ms + 1000)
			;

		ASSERT_EQ(count_waiters(&ctx), 0);
	}
	ctx.stopped = 1;
	/* Stop waiters */
	for (i = 0; i < ARRAY_SIZE(waiters); i++)
		ret = pthread_kill(waiters[i], SIGUSR1);
	for (i = 0; i < ARRAY_SIZE(waiters); i++)
		pthread_join(waiters[i], NULL);

	for (i = 0; i < ARRAY_SIZE(waiters); i++)
		close(ctx.evfd[i]);
	close(ctx.epfd);
}

struct epoll61_ctx {
	int epfd;
	int evfd;
};

static void *epoll61_write_eventfd(void *ctx_)
{
	struct epoll61_ctx *ctx = ctx_;
	int64_t l = 1;

	usleep(10950);
	write(ctx->evfd, &l, sizeof(l));
	return NULL;
}

static void *epoll61_epoll_with_timeout(void *ctx_)
{
	struct epoll61_ctx *ctx = ctx_;
	struct epoll_event events[1];
	int n;

	n = epoll_wait(ctx->epfd, events, 1, 11);
	/*
	 * If epoll returned the eventfd, write on the eventfd to wake up the
	 * blocking poller.
	 */
	if (n == 1) {
		int64_t l = 1;

		write(ctx->evfd, &l, sizeof(l));
	}
	return NULL;
}

static void *epoll61_blocking_epoll(void *ctx_)
{
	struct epoll61_ctx *ctx = ctx_;
	struct epoll_event events[1];

	epoll_wait(ctx->epfd, events, 1, -1);
	return NULL;
}

TEST(epoll61)
{
	struct epoll61_ctx ctx;
	struct epoll_event ev;
	int i, r;

	ctx.epfd = epoll_create1(0);
	ASSERT_GE(ctx.epfd, 0);
	ctx.evfd = eventfd(0, EFD_NONBLOCK);
	ASSERT_GE(ctx.evfd, 0);

	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
	ev.data.ptr = NULL;
	r = epoll_ctl(ctx.epfd, EPOLL_CTL_ADD, ctx.evfd, &ev);
	ASSERT_EQ(r, 0);

	/*
	 * We are testing a race.  Repeat the test case 1000 times to make it
	 * more likely to fail in case of a bug.
	 */
	for (i = 0; i < 1000; i++) {
		pthread_t threads[3];
		int n;

		/*
		 * Start 3 threads:
		 * Thread 1 sleeps for 10.9ms and writes to the evenfd.
		 * Thread 2 calls epoll with a timeout of 11ms.
		 * Thread 3 calls epoll with a timeout of -1.
		 *
		 * The eventfd write by Thread 1 should either wakeup Thread 2
		 * or Thread 3.  If it wakes up Thread 2, Thread 2 writes on the
		 * eventfd to wake up Thread 3.
		 *
		 * If no events are missed, all three threads should eventually
		 * be joinable.
		 */
		ASSERT_EQ(pthread_create(&threads[0], NULL,
					 epoll61_write_eventfd, &ctx), 0);
		ASSERT_EQ(pthread_create(&threads[1], NULL,
					 epoll61_epoll_with_timeout, &ctx), 0);
		ASSERT_EQ(pthread_create(&threads[2], NULL,
					 epoll61_blocking_epoll, &ctx), 0);

		for (n = 0; n < ARRAY_SIZE(threads); ++n)
			ASSERT_EQ(pthread_join(threads[n], NULL), 0);
	}

	close(ctx.epfd);
	close(ctx.evfd);
}

/* Equivalent to basic test epoll1, but exercising epoll_pwait2. */
TEST(epoll62)
{
	int efd;
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e), 0);

	ASSERT_EQ(write(sfd[1], "w", 1), 1);

	EXPECT_EQ(sys_epoll_pwait2(efd, &e, 1, NULL, NULL, 0), 1);
	EXPECT_EQ(sys_epoll_pwait2(efd, &e, 1, NULL, NULL, 0), 1);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
}

/* Epoll_pwait2 basic timeout test. */
TEST(epoll63)
{
	const int cfg_delay_ms = 10;
	unsigned long long tdiff;
	struct __kernel_timespec ts;
	int efd;
	int sfd[2];
	struct epoll_event e;

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sfd), 0);

	efd = epoll_create(1);
	ASSERT_GE(efd, 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e), 0);

	ts.tv_sec = 0;
	ts.tv_nsec = cfg_delay_ms * 1000 * 1000;

	tdiff = msecs();
	EXPECT_EQ(sys_epoll_pwait2(efd, &e, 1, &ts, NULL, 0), 0);
	tdiff = msecs() - tdiff;

	EXPECT_GE(tdiff, cfg_delay_ms);

	close(efd);
	close(sfd[0]);
	close(sfd[1]);
}

/*
 *        t0    t1
 *     (ew) \  / (ew)
 *           e0
 *            | (lt)
 *           s0
 */
TEST(epoll64)
{
	pthread_t waiter[2];
	struct epoll_event e;
	struct epoll_mtcontext ctx = { 0 };

	signal(SIGUSR1, signal_handler);

	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, ctx.sfd), 0);

	ctx.efd[0] = epoll_create(1);
	ASSERT_GE(ctx.efd[0], 0);

	e.events = EPOLLIN;
	ASSERT_EQ(epoll_ctl(ctx.efd[0], EPOLL_CTL_ADD, ctx.sfd[0], &e), 0);

	/*
	 * main will act as the emitter once both waiter threads are
	 * blocked and expects to both be awoken upon the ready event.
	 */
	ctx.main = pthread_self();
	ASSERT_EQ(pthread_create(&waiter[0], NULL, waiter_entry1a, &ctx), 0);
	ASSERT_EQ(pthread_create(&waiter[1], NULL, waiter_entry1a, &ctx), 0);

	usleep(100000);
	ASSERT_EQ(write(ctx.sfd[1], "w", 1), 1);

	ASSERT_EQ(pthread_join(waiter[0], NULL), 0);
	ASSERT_EQ(pthread_join(waiter[1], NULL), 0);

	EXPECT_EQ(ctx.count, 2);

	close(ctx.efd[0]);
	close(ctx.sfd[0]);
	close(ctx.sfd[1]);
}

TEST_HARNESS_MAIN

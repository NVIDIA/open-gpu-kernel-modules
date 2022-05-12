/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_VIRTIO_VSOCK_H
#define _LINUX_VIRTIO_VSOCK_H

#include <uapi/linux/virtio_vsock.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/af_vsock.h>

#define VIRTIO_VSOCK_DEFAULT_RX_BUF_SIZE	(1024 * 4)
#define VIRTIO_VSOCK_MAX_BUF_SIZE		0xFFFFFFFFUL
#define VIRTIO_VSOCK_MAX_PKT_BUF_SIZE		(1024 * 64)

enum {
	VSOCK_VQ_RX     = 0, /* for host to guest data */
	VSOCK_VQ_TX     = 1, /* for guest to host data */
	VSOCK_VQ_EVENT  = 2,
	VSOCK_VQ_MAX    = 3,
};

/* Per-socket state (accessed via vsk->trans) */
struct virtio_vsock_sock {
	struct vsock_sock *vsk;

	spinlock_t tx_lock;
	spinlock_t rx_lock;

	/* Protected by tx_lock */
	u32 tx_cnt;
	u32 peer_fwd_cnt;
	u32 peer_buf_alloc;

	/* Protected by rx_lock */
	u32 fwd_cnt;
	u32 last_fwd_cnt;
	u32 rx_bytes;
	u32 buf_alloc;
	struct list_head rx_queue;
};

struct virtio_vsock_pkt {
	struct virtio_vsock_hdr	hdr;
	struct list_head list;
	/* socket refcnt not held, only use for cancellation */
	struct vsock_sock *vsk;
	void *buf;
	u32 buf_len;
	u32 len;
	u32 off;
	bool reply;
	bool tap_delivered;
};

struct virtio_vsock_pkt_info {
	u32 remote_cid, remote_port;
	struct vsock_sock *vsk;
	struct msghdr *msg;
	u32 pkt_len;
	u16 type;
	u16 op;
	u32 flags;
	bool reply;
};

struct virtio_transport {
	/* This must be the first field */
	struct vsock_transport transport;

	/* Takes ownership of the packet */
	int (*send_pkt)(struct virtio_vsock_pkt *pkt);
};

ssize_t
virtio_transport_stream_dequeue(struct vsock_sock *vsk,
				struct msghdr *msg,
				size_t len,
				int type);
int
virtio_transport_dgram_dequeue(struct vsock_sock *vsk,
			       struct msghdr *msg,
			       size_t len, int flags);

s64 virtio_transport_stream_has_data(struct vsock_sock *vsk);
s64 virtio_transport_stream_has_space(struct vsock_sock *vsk);

int virtio_transport_do_socket_init(struct vsock_sock *vsk,
				 struct vsock_sock *psk);
int
virtio_transport_notify_poll_in(struct vsock_sock *vsk,
				size_t target,
				bool *data_ready_now);
int
virtio_transport_notify_poll_out(struct vsock_sock *vsk,
				 size_t target,
				 bool *space_available_now);

int virtio_transport_notify_recv_init(struct vsock_sock *vsk,
	size_t target, struct vsock_transport_recv_notify_data *data);
int virtio_transport_notify_recv_pre_block(struct vsock_sock *vsk,
	size_t target, struct vsock_transport_recv_notify_data *data);
int virtio_transport_notify_recv_pre_dequeue(struct vsock_sock *vsk,
	size_t target, struct vsock_transport_recv_notify_data *data);
int virtio_transport_notify_recv_post_dequeue(struct vsock_sock *vsk,
	size_t target, ssize_t copied, bool data_read,
	struct vsock_transport_recv_notify_data *data);
int virtio_transport_notify_send_init(struct vsock_sock *vsk,
	struct vsock_transport_send_notify_data *data);
int virtio_transport_notify_send_pre_block(struct vsock_sock *vsk,
	struct vsock_transport_send_notify_data *data);
int virtio_transport_notify_send_pre_enqueue(struct vsock_sock *vsk,
	struct vsock_transport_send_notify_data *data);
int virtio_transport_notify_send_post_enqueue(struct vsock_sock *vsk,
	ssize_t written, struct vsock_transport_send_notify_data *data);
void virtio_transport_notify_buffer_size(struct vsock_sock *vsk, u64 *val);

u64 virtio_transport_stream_rcvhiwat(struct vsock_sock *vsk);
bool virtio_transport_stream_is_active(struct vsock_sock *vsk);
bool virtio_transport_stream_allow(u32 cid, u32 port);
int virtio_transport_dgram_bind(struct vsock_sock *vsk,
				struct sockaddr_vm *addr);
bool virtio_transport_dgram_allow(u32 cid, u32 port);

int virtio_transport_connect(struct vsock_sock *vsk);

int virtio_transport_shutdown(struct vsock_sock *vsk, int mode);

void virtio_transport_release(struct vsock_sock *vsk);

ssize_t
virtio_transport_stream_enqueue(struct vsock_sock *vsk,
				struct msghdr *msg,
				size_t len);
int
virtio_transport_dgram_enqueue(struct vsock_sock *vsk,
			       struct sockaddr_vm *remote_addr,
			       struct msghdr *msg,
			       size_t len);

void virtio_transport_destruct(struct vsock_sock *vsk);

void virtio_transport_recv_pkt(struct virtio_transport *t,
			       struct virtio_vsock_pkt *pkt);
void virtio_transport_free_pkt(struct virtio_vsock_pkt *pkt);
void virtio_transport_inc_tx_pkt(struct virtio_vsock_sock *vvs, struct virtio_vsock_pkt *pkt);
u32 virtio_transport_get_credit(struct virtio_vsock_sock *vvs, u32 wanted);
void virtio_transport_put_credit(struct virtio_vsock_sock *vvs, u32 credit);
void virtio_transport_deliver_tap_pkt(struct virtio_vsock_pkt *pkt);

#endif /* _LINUX_VIRTIO_VSOCK_H */

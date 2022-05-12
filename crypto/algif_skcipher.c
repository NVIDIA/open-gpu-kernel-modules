// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * algif_skcipher: User-space interface for skcipher algorithms
 *
 * This file provides the user-space API for symmetric key ciphers.
 *
 * Copyright (c) 2010 Herbert Xu <herbert@gondor.apana.org.au>
 *
 * The following concept of the memory management is used:
 *
 * The kernel maintains two SGLs, the TX SGL and the RX SGL. The TX SGL is
 * filled by user space with the data submitted via sendpage/sendmsg. Filling
 * up the TX SGL does not cause a crypto operation -- the data will only be
 * tracked by the kernel. Upon receipt of one recvmsg call, the caller must
 * provide a buffer which is tracked with the RX SGL.
 *
 * During the processing of the recvmsg operation, the cipher request is
 * allocated and prepared. As part of the recvmsg operation, the processed
 * TX buffers are extracted from the TX SGL into a separate SGL.
 *
 * After the completion of the crypto operation, the RX SGL and the cipher
 * request is released. The extracted TX SGL parts are released together with
 * the RX SGL release.
 */

#include <crypto/scatterwalk.h>
#include <crypto/skcipher.h>
#include <crypto/if_alg.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/net.h>
#include <net/sock.h>

static int skcipher_sendmsg(struct socket *sock, struct msghdr *msg,
			    size_t size)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct sock *psk = ask->parent;
	struct alg_sock *pask = alg_sk(psk);
	struct crypto_skcipher *tfm = pask->private;
	unsigned ivsize = crypto_skcipher_ivsize(tfm);

	return af_alg_sendmsg(sock, msg, size, ivsize);
}

static int _skcipher_recvmsg(struct socket *sock, struct msghdr *msg,
			     size_t ignored, int flags)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct sock *psk = ask->parent;
	struct alg_sock *pask = alg_sk(psk);
	struct af_alg_ctx *ctx = ask->private;
	struct crypto_skcipher *tfm = pask->private;
	unsigned int bs = crypto_skcipher_chunksize(tfm);
	struct af_alg_async_req *areq;
	int err = 0;
	size_t len = 0;

	if (!ctx->init || (ctx->more && ctx->used < bs)) {
		err = af_alg_wait_for_data(sk, flags, bs);
		if (err)
			return err;
	}

	/* Allocate cipher request for current operation. */
	areq = af_alg_alloc_areq(sk, sizeof(struct af_alg_async_req) +
				     crypto_skcipher_reqsize(tfm));
	if (IS_ERR(areq))
		return PTR_ERR(areq);

	/* convert iovecs of output buffers into RX SGL */
	err = af_alg_get_rsgl(sk, msg, flags, areq, ctx->used, &len);
	if (err)
		goto free;

	/*
	 * If more buffers are to be expected to be processed, process only
	 * full block size buffers.
	 */
	if (ctx->more || len < ctx->used)
		len -= len % bs;

	/*
	 * Create a per request TX SGL for this request which tracks the
	 * SG entries from the global TX SGL.
	 */
	areq->tsgl_entries = af_alg_count_tsgl(sk, len, 0);
	if (!areq->tsgl_entries)
		areq->tsgl_entries = 1;
	areq->tsgl = sock_kmalloc(sk, array_size(sizeof(*areq->tsgl),
						 areq->tsgl_entries),
				  GFP_KERNEL);
	if (!areq->tsgl) {
		err = -ENOMEM;
		goto free;
	}
	sg_init_table(areq->tsgl, areq->tsgl_entries);
	af_alg_pull_tsgl(sk, len, areq->tsgl, 0);

	/* Initialize the crypto operation */
	skcipher_request_set_tfm(&areq->cra_u.skcipher_req, tfm);
	skcipher_request_set_crypt(&areq->cra_u.skcipher_req, areq->tsgl,
				   areq->first_rsgl.sgl.sg, len, ctx->iv);

	if (msg->msg_iocb && !is_sync_kiocb(msg->msg_iocb)) {
		/* AIO operation */
		sock_hold(sk);
		areq->iocb = msg->msg_iocb;

		/* Remember output size that will be generated. */
		areq->outlen = len;

		skcipher_request_set_callback(&areq->cra_u.skcipher_req,
					      CRYPTO_TFM_REQ_MAY_SLEEP,
					      af_alg_async_cb, areq);
		err = ctx->enc ?
			crypto_skcipher_encrypt(&areq->cra_u.skcipher_req) :
			crypto_skcipher_decrypt(&areq->cra_u.skcipher_req);

		/* AIO operation in progress */
		if (err == -EINPROGRESS)
			return -EIOCBQUEUED;

		sock_put(sk);
	} else {
		/* Synchronous operation */
		skcipher_request_set_callback(&areq->cra_u.skcipher_req,
					      CRYPTO_TFM_REQ_MAY_SLEEP |
					      CRYPTO_TFM_REQ_MAY_BACKLOG,
					      crypto_req_done, &ctx->wait);
		err = crypto_wait_req(ctx->enc ?
			crypto_skcipher_encrypt(&areq->cra_u.skcipher_req) :
			crypto_skcipher_decrypt(&areq->cra_u.skcipher_req),
						 &ctx->wait);
	}


free:
	af_alg_free_resources(areq);

	return err ? err : len;
}

static int skcipher_recvmsg(struct socket *sock, struct msghdr *msg,
			    size_t ignored, int flags)
{
	struct sock *sk = sock->sk;
	int ret = 0;

	lock_sock(sk);
	while (msg_data_left(msg)) {
		int err = _skcipher_recvmsg(sock, msg, ignored, flags);

		/*
		 * This error covers -EIOCBQUEUED which implies that we can
		 * only handle one AIO request. If the caller wants to have
		 * multiple AIO requests in parallel, he must make multiple
		 * separate AIO calls.
		 *
		 * Also return the error if no data has been processed so far.
		 */
		if (err <= 0) {
			if (err == -EIOCBQUEUED || !ret)
				ret = err;
			goto out;
		}

		ret += err;
	}

out:
	af_alg_wmem_wakeup(sk);
	release_sock(sk);
	return ret;
}

static struct proto_ops algif_skcipher_ops = {
	.family		=	PF_ALG,

	.connect	=	sock_no_connect,
	.socketpair	=	sock_no_socketpair,
	.getname	=	sock_no_getname,
	.ioctl		=	sock_no_ioctl,
	.listen		=	sock_no_listen,
	.shutdown	=	sock_no_shutdown,
	.mmap		=	sock_no_mmap,
	.bind		=	sock_no_bind,
	.accept		=	sock_no_accept,

	.release	=	af_alg_release,
	.sendmsg	=	skcipher_sendmsg,
	.sendpage	=	af_alg_sendpage,
	.recvmsg	=	skcipher_recvmsg,
	.poll		=	af_alg_poll,
};

static int skcipher_check_key(struct socket *sock)
{
	int err = 0;
	struct sock *psk;
	struct alg_sock *pask;
	struct crypto_skcipher *tfm;
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);

	lock_sock(sk);
	if (!atomic_read(&ask->nokey_refcnt))
		goto unlock_child;

	psk = ask->parent;
	pask = alg_sk(ask->parent);
	tfm = pask->private;

	err = -ENOKEY;
	lock_sock_nested(psk, SINGLE_DEPTH_NESTING);
	if (crypto_skcipher_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		goto unlock;

	atomic_dec(&pask->nokey_refcnt);
	atomic_set(&ask->nokey_refcnt, 0);

	err = 0;

unlock:
	release_sock(psk);
unlock_child:
	release_sock(sk);

	return err;
}

static int skcipher_sendmsg_nokey(struct socket *sock, struct msghdr *msg,
				  size_t size)
{
	int err;

	err = skcipher_check_key(sock);
	if (err)
		return err;

	return skcipher_sendmsg(sock, msg, size);
}

static ssize_t skcipher_sendpage_nokey(struct socket *sock, struct page *page,
				       int offset, size_t size, int flags)
{
	int err;

	err = skcipher_check_key(sock);
	if (err)
		return err;

	return af_alg_sendpage(sock, page, offset, size, flags);
}

static int skcipher_recvmsg_nokey(struct socket *sock, struct msghdr *msg,
				  size_t ignored, int flags)
{
	int err;

	err = skcipher_check_key(sock);
	if (err)
		return err;

	return skcipher_recvmsg(sock, msg, ignored, flags);
}

static struct proto_ops algif_skcipher_ops_nokey = {
	.family		=	PF_ALG,

	.connect	=	sock_no_connect,
	.socketpair	=	sock_no_socketpair,
	.getname	=	sock_no_getname,
	.ioctl		=	sock_no_ioctl,
	.listen		=	sock_no_listen,
	.shutdown	=	sock_no_shutdown,
	.mmap		=	sock_no_mmap,
	.bind		=	sock_no_bind,
	.accept		=	sock_no_accept,

	.release	=	af_alg_release,
	.sendmsg	=	skcipher_sendmsg_nokey,
	.sendpage	=	skcipher_sendpage_nokey,
	.recvmsg	=	skcipher_recvmsg_nokey,
	.poll		=	af_alg_poll,
};

static void *skcipher_bind(const char *name, u32 type, u32 mask)
{
	return crypto_alloc_skcipher(name, type, mask);
}

static void skcipher_release(void *private)
{
	crypto_free_skcipher(private);
}

static int skcipher_setkey(void *private, const u8 *key, unsigned int keylen)
{
	return crypto_skcipher_setkey(private, key, keylen);
}

static void skcipher_sock_destruct(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct sock *psk = ask->parent;
	struct alg_sock *pask = alg_sk(psk);
	struct crypto_skcipher *tfm = pask->private;

	af_alg_pull_tsgl(sk, ctx->used, NULL, 0);
	sock_kzfree_s(sk, ctx->iv, crypto_skcipher_ivsize(tfm));
	sock_kfree_s(sk, ctx, ctx->len);
	af_alg_release_parent(sk);
}

static int skcipher_accept_parent_nokey(void *private, struct sock *sk)
{
	struct af_alg_ctx *ctx;
	struct alg_sock *ask = alg_sk(sk);
	struct crypto_skcipher *tfm = private;
	unsigned int len = sizeof(*ctx);

	ctx = sock_kmalloc(sk, len, GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;
	memset(ctx, 0, len);

	ctx->iv = sock_kmalloc(sk, crypto_skcipher_ivsize(tfm),
			       GFP_KERNEL);
	if (!ctx->iv) {
		sock_kfree_s(sk, ctx, len);
		return -ENOMEM;
	}
	memset(ctx->iv, 0, crypto_skcipher_ivsize(tfm));

	INIT_LIST_HEAD(&ctx->tsgl_list);
	ctx->len = len;
	crypto_init_wait(&ctx->wait);

	ask->private = ctx;

	sk->sk_destruct = skcipher_sock_destruct;

	return 0;
}

static int skcipher_accept_parent(void *private, struct sock *sk)
{
	struct crypto_skcipher *tfm = private;

	if (crypto_skcipher_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		return -ENOKEY;

	return skcipher_accept_parent_nokey(private, sk);
}

static const struct af_alg_type algif_type_skcipher = {
	.bind		=	skcipher_bind,
	.release	=	skcipher_release,
	.setkey		=	skcipher_setkey,
	.accept		=	skcipher_accept_parent,
	.accept_nokey	=	skcipher_accept_parent_nokey,
	.ops		=	&algif_skcipher_ops,
	.ops_nokey	=	&algif_skcipher_ops_nokey,
	.name		=	"skcipher",
	.owner		=	THIS_MODULE
};

static int __init algif_skcipher_init(void)
{
	return af_alg_register_type(&algif_type_skcipher);
}

static void __exit algif_skcipher_exit(void)
{
	int err = af_alg_unregister_type(&algif_type_skcipher);
	BUG_ON(err);
}

module_init(algif_skcipher_init);
module_exit(algif_skcipher_exit);
MODULE_LICENSE("GPL");

/*
 * Copyright (c) 2013-2015, Mellanox Technologies, Ltd.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/mlx5/driver.h>
#include "wq.h"
#include "mlx5_core.h"

int mlx5_wq_cyc_create(struct mlx5_core_dev *mdev, struct mlx5_wq_param *param,
		       void *wqc, struct mlx5_wq_cyc *wq,
		       struct mlx5_wq_ctrl *wq_ctrl)
{
	u8 log_wq_stride = MLX5_GET(wq, wqc, log_wq_stride);
	u8 log_wq_sz     = MLX5_GET(wq, wqc, log_wq_sz);
	struct mlx5_frag_buf_ctrl *fbc = &wq->fbc;
	int err;

	err = mlx5_db_alloc_node(mdev, &wq_ctrl->db, param->db_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_db_alloc_node() failed, %d\n", err);
		return err;
	}

	wq->db  = wq_ctrl->db.db;

	err = mlx5_frag_buf_alloc_node(mdev, wq_get_byte_sz(log_wq_sz, log_wq_stride),
				       &wq_ctrl->buf, param->buf_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_frag_buf_alloc_node() failed, %d\n", err);
		goto err_db_free;
	}

	mlx5_init_fbc(wq_ctrl->buf.frags, log_wq_stride, log_wq_sz, fbc);
	wq->sz = mlx5_wq_cyc_get_size(wq);

	wq_ctrl->mdev = mdev;

	return 0;

err_db_free:
	mlx5_db_free(mdev, &wq_ctrl->db);

	return err;
}

void mlx5_wq_cyc_wqe_dump(struct mlx5_wq_cyc *wq, u16 ix, u8 nstrides)
{
	size_t len;
	void *wqe;

	if (!net_ratelimit())
		return;

	nstrides = max_t(u8, nstrides, 1);

	len = nstrides << wq->fbc.log_stride;
	wqe = mlx5_wq_cyc_get_wqe(wq, ix);

	pr_info("WQE DUMP: WQ size %d WQ cur size %d, WQE index 0x%x, len: %zu\n",
		mlx5_wq_cyc_get_size(wq), wq->cur_sz, ix, len);
	print_hex_dump(KERN_WARNING, "", DUMP_PREFIX_OFFSET, 16, 1, wqe, len, false);
}

void mlx5_wq_cyc_reset(struct mlx5_wq_cyc *wq)
{
	wq->wqe_ctr = 0;
	wq->cur_sz = 0;
	mlx5_wq_cyc_update_db_record(wq);
}

int mlx5_wq_qp_create(struct mlx5_core_dev *mdev, struct mlx5_wq_param *param,
		      void *qpc, struct mlx5_wq_qp *wq,
		      struct mlx5_wq_ctrl *wq_ctrl)
{
	u8 log_rq_stride = MLX5_GET(qpc, qpc, log_rq_stride) + 4;
	u8 log_rq_sz     = MLX5_GET(qpc, qpc, log_rq_size);
	u8 log_sq_stride = ilog2(MLX5_SEND_WQE_BB);
	u8 log_sq_sz     = MLX5_GET(qpc, qpc, log_sq_size);

	u32 rq_byte_size;
	int err;



	err = mlx5_db_alloc_node(mdev, &wq_ctrl->db, param->db_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_db_alloc_node() failed, %d\n", err);
		return err;
	}

	err = mlx5_frag_buf_alloc_node(mdev,
				       wq_get_byte_sz(log_rq_sz, log_rq_stride) +
				       wq_get_byte_sz(log_sq_sz, log_sq_stride),
				       &wq_ctrl->buf, param->buf_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_frag_buf_alloc_node() failed, %d\n", err);
		goto err_db_free;
	}

	mlx5_init_fbc(wq_ctrl->buf.frags, log_rq_stride, log_rq_sz, &wq->rq.fbc);

	rq_byte_size = wq_get_byte_sz(log_rq_sz, log_rq_stride);

	if (rq_byte_size < PAGE_SIZE) {
		/* SQ starts within the same page of the RQ */
		u16 sq_strides_offset = rq_byte_size / MLX5_SEND_WQE_BB;

		mlx5_init_fbc_offset(wq_ctrl->buf.frags,
				     log_sq_stride, log_sq_sz, sq_strides_offset,
				     &wq->sq.fbc);
	} else {
		u16 rq_npages = rq_byte_size >> PAGE_SHIFT;

		mlx5_init_fbc(wq_ctrl->buf.frags + rq_npages,
			      log_sq_stride, log_sq_sz, &wq->sq.fbc);
	}

	wq->rq.db  = &wq_ctrl->db.db[MLX5_RCV_DBR];
	wq->sq.db  = &wq_ctrl->db.db[MLX5_SND_DBR];

	wq_ctrl->mdev = mdev;

	return 0;

err_db_free:
	mlx5_db_free(mdev, &wq_ctrl->db);

	return err;
}

int mlx5_cqwq_create(struct mlx5_core_dev *mdev, struct mlx5_wq_param *param,
		     void *cqc, struct mlx5_cqwq *wq,
		     struct mlx5_wq_ctrl *wq_ctrl)
{
	/* CQE_STRIDE_128 and CQE_STRIDE_128_PAD both mean 128B stride */
	u8 log_wq_stride = MLX5_GET(cqc, cqc, cqe_sz) == CQE_STRIDE_64 ? 6 : 7;
	u8 log_wq_sz     = MLX5_GET(cqc, cqc, log_cq_size);
	int err;

	err = mlx5_db_alloc_node(mdev, &wq_ctrl->db, param->db_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_db_alloc_node() failed, %d\n", err);
		return err;
	}

	wq->db  = wq_ctrl->db.db;

	err = mlx5_frag_buf_alloc_node(mdev, wq_get_byte_sz(log_wq_sz, log_wq_stride),
				       &wq_ctrl->buf,
				       param->buf_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_frag_buf_alloc_node() failed, %d\n",
			       err);
		goto err_db_free;
	}

	mlx5_init_fbc(wq_ctrl->buf.frags, log_wq_stride, log_wq_sz, &wq->fbc);

	wq_ctrl->mdev = mdev;

	return 0;

err_db_free:
	mlx5_db_free(mdev, &wq_ctrl->db);

	return err;
}

static void mlx5_wq_ll_init_list(struct mlx5_wq_ll *wq)
{
	struct mlx5_wqe_srq_next_seg *next_seg;
	int i;

	for (i = 0; i < wq->fbc.sz_m1; i++) {
		next_seg = mlx5_wq_ll_get_wqe(wq, i);
		next_seg->next_wqe_index = cpu_to_be16(i + 1);
	}
	next_seg = mlx5_wq_ll_get_wqe(wq, i);
	wq->tail_next = &next_seg->next_wqe_index;
}

int mlx5_wq_ll_create(struct mlx5_core_dev *mdev, struct mlx5_wq_param *param,
		      void *wqc, struct mlx5_wq_ll *wq,
		      struct mlx5_wq_ctrl *wq_ctrl)
{
	u8 log_wq_stride = MLX5_GET(wq, wqc, log_wq_stride);
	u8 log_wq_sz     = MLX5_GET(wq, wqc, log_wq_sz);
	struct mlx5_frag_buf_ctrl *fbc = &wq->fbc;
	int err;

	err = mlx5_db_alloc_node(mdev, &wq_ctrl->db, param->db_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_db_alloc_node() failed, %d\n", err);
		return err;
	}

	wq->db  = wq_ctrl->db.db;

	err = mlx5_frag_buf_alloc_node(mdev, wq_get_byte_sz(log_wq_sz, log_wq_stride),
				       &wq_ctrl->buf, param->buf_numa_node);
	if (err) {
		mlx5_core_warn(mdev, "mlx5_frag_buf_alloc_node() failed, %d\n", err);
		goto err_db_free;
	}

	mlx5_init_fbc(wq_ctrl->buf.frags, log_wq_stride, log_wq_sz, fbc);

	mlx5_wq_ll_init_list(wq);
	wq_ctrl->mdev = mdev;

	return 0;

err_db_free:
	mlx5_db_free(mdev, &wq_ctrl->db);

	return err;
}

void mlx5_wq_ll_reset(struct mlx5_wq_ll *wq)
{
	wq->head = 0;
	wq->wqe_ctr = 0;
	wq->cur_sz = 0;
	mlx5_wq_ll_init_list(wq);
	mlx5_wq_ll_update_db_record(wq);
}

void mlx5_wq_destroy(struct mlx5_wq_ctrl *wq_ctrl)
{
	mlx5_frag_buf_free(wq_ctrl->mdev, &wq_ctrl->buf);
	mlx5_db_free(wq_ctrl->mdev, &wq_ctrl->db);
}


/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/* Copyright (c) 2018 Mellanox Technologies. */

#ifndef MLX5_CORE_EQ_H
#define MLX5_CORE_EQ_H

#define MLX5_IRQ_VEC_COMP_BASE 1
#define MLX5_NUM_CMD_EQE   (32)
#define MLX5_NUM_ASYNC_EQE (0x1000)
#define MLX5_NUM_SPARE_EQE (0x80)

struct mlx5_eq;
struct mlx5_core_dev;

struct mlx5_eq_param {
	u8             irq_index;
	int            nent;
	u64            mask[4];
};

struct mlx5_eq *
mlx5_eq_create_generic(struct mlx5_core_dev *dev, struct mlx5_eq_param *param);
int
mlx5_eq_destroy_generic(struct mlx5_core_dev *dev, struct mlx5_eq *eq);
int mlx5_eq_enable(struct mlx5_core_dev *dev, struct mlx5_eq *eq,
		   struct notifier_block *nb);
void mlx5_eq_disable(struct mlx5_core_dev *dev, struct mlx5_eq *eq,
		     struct notifier_block *nb);

struct mlx5_eqe *mlx5_eq_get_eqe(struct mlx5_eq *eq, u32 cc);
void mlx5_eq_update_ci(struct mlx5_eq *eq, u32 cc, bool arm);

/* The HCA will think the queue has overflowed if we
 * don't tell it we've been processing events.  We
 * create EQs with MLX5_NUM_SPARE_EQE extra entries,
 * so we must update our consumer index at
 * least that often.
 *
 * mlx5_eq_update_cc must be called on every EQE @EQ irq handler
 */
static inline u32 mlx5_eq_update_cc(struct mlx5_eq *eq, u32 cc)
{
	if (unlikely(cc >= MLX5_NUM_SPARE_EQE)) {
		mlx5_eq_update_ci(eq, cc, 0);
		cc = 0;
	}
	return cc;
}

struct mlx5_nb {
	struct notifier_block nb;
	u8 event_type;
};

#define mlx5_nb_cof(ptr, type, member) \
	(container_of(container_of(ptr, struct mlx5_nb, nb), type, member))

#define MLX5_NB_INIT(name, handler, event) do {              \
	(name)->nb.notifier_call = handler;                  \
	(name)->event_type = MLX5_EVENT_TYPE_##event;        \
} while (0)

#endif /* MLX5_CORE_EQ_H */

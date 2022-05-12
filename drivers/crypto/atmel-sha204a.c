// SPDX-License-Identifier: GPL-2.0
/*
 * Microchip / Atmel SHA204A (I2C) driver.
 *
 * Copyright (c) 2019 Linaro, Ltd. <ard.biesheuvel@linaro.org>
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include "atmel-i2c.h"

static void atmel_sha204a_rng_done(struct atmel_i2c_work_data *work_data,
				   void *areq, int status)
{
	struct atmel_i2c_client_priv *i2c_priv = work_data->ctx;
	struct hwrng *rng = areq;

	if (status)
		dev_warn_ratelimited(&i2c_priv->client->dev,
				     "i2c transaction failed (%d)\n",
				     status);

	rng->priv = (unsigned long)work_data;
	atomic_dec(&i2c_priv->tfm_count);
}

static int atmel_sha204a_rng_read_nonblocking(struct hwrng *rng, void *data,
					      size_t max)
{
	struct atmel_i2c_client_priv *i2c_priv;
	struct atmel_i2c_work_data *work_data;

	i2c_priv = container_of(rng, struct atmel_i2c_client_priv, hwrng);

	/* keep maximum 1 asynchronous read in flight at any time */
	if (!atomic_add_unless(&i2c_priv->tfm_count, 1, 1))
		return 0;

	if (rng->priv) {
		work_data = (struct atmel_i2c_work_data *)rng->priv;
		max = min(sizeof(work_data->cmd.data), max);
		memcpy(data, &work_data->cmd.data, max);
		rng->priv = 0;
	} else {
		work_data = kmalloc(sizeof(*work_data), GFP_ATOMIC);
		if (!work_data)
			return -ENOMEM;

		work_data->ctx = i2c_priv;
		work_data->client = i2c_priv->client;

		max = 0;
	}

	atmel_i2c_init_random_cmd(&work_data->cmd);
	atmel_i2c_enqueue(work_data, atmel_sha204a_rng_done, rng);

	return max;
}

static int atmel_sha204a_rng_read(struct hwrng *rng, void *data, size_t max,
				  bool wait)
{
	struct atmel_i2c_client_priv *i2c_priv;
	struct atmel_i2c_cmd cmd;
	int ret;

	if (!wait)
		return atmel_sha204a_rng_read_nonblocking(rng, data, max);

	i2c_priv = container_of(rng, struct atmel_i2c_client_priv, hwrng);

	atmel_i2c_init_random_cmd(&cmd);

	ret = atmel_i2c_send_receive(i2c_priv->client, &cmd);
	if (ret)
		return ret;

	max = min(sizeof(cmd.data), max);
	memcpy(data, cmd.data, max);

	return max;
}

static int atmel_sha204a_probe(struct i2c_client *client,
			       const struct i2c_device_id *id)
{
	struct atmel_i2c_client_priv *i2c_priv;
	int ret;

	ret = atmel_i2c_probe(client, id);
	if (ret)
		return ret;

	i2c_priv = i2c_get_clientdata(client);

	memset(&i2c_priv->hwrng, 0, sizeof(i2c_priv->hwrng));

	i2c_priv->hwrng.name = dev_name(&client->dev);
	i2c_priv->hwrng.read = atmel_sha204a_rng_read;
	i2c_priv->hwrng.quality = 1024;

	ret = devm_hwrng_register(&client->dev, &i2c_priv->hwrng);
	if (ret)
		dev_warn(&client->dev, "failed to register RNG (%d)\n", ret);

	return ret;
}

static int atmel_sha204a_remove(struct i2c_client *client)
{
	struct atmel_i2c_client_priv *i2c_priv = i2c_get_clientdata(client);

	if (atomic_read(&i2c_priv->tfm_count)) {
		dev_err(&client->dev, "Device is busy\n");
		return -EBUSY;
	}

	if (i2c_priv->hwrng.priv)
		kfree((void *)i2c_priv->hwrng.priv);

	return 0;
}

static const struct of_device_id atmel_sha204a_dt_ids[] = {
	{ .compatible = "atmel,atsha204a", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, atmel_sha204a_dt_ids);

static const struct i2c_device_id atmel_sha204a_id[] = {
	{ "atsha204a", 0 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, atmel_sha204a_id);

static struct i2c_driver atmel_sha204a_driver = {
	.probe			= atmel_sha204a_probe,
	.remove			= atmel_sha204a_remove,
	.id_table		= atmel_sha204a_id,

	.driver.name		= "atmel-sha204a",
	.driver.of_match_table	= of_match_ptr(atmel_sha204a_dt_ids),
};

static int __init atmel_sha204a_init(void)
{
	return i2c_add_driver(&atmel_sha204a_driver);
}

static void __exit atmel_sha204a_exit(void)
{
	flush_scheduled_work();
	i2c_del_driver(&atmel_sha204a_driver);
}

module_init(atmel_sha204a_init);
module_exit(atmel_sha204a_exit);

MODULE_AUTHOR("Ard Biesheuvel <ard.biesheuvel@linaro.org>");
MODULE_LICENSE("GPL v2");

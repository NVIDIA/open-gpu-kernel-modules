// SPDX-License-Identifier: GPL-2.0+
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/rwsem.h>
#include "kpc_dma_driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matt.Sickler@daktronics.com");

#define KPC_DMA_CHAR_MAJOR    UNNAMED_MAJOR
#define KPC_DMA_NUM_MINORS    BIT(MINORBITS)
static DEFINE_MUTEX(kpc_dma_mtx);
static int assigned_major_num;
static LIST_HEAD(kpc_dma_list);

/**********  kpc_dma_list list management  **********/
struct kpc_dma_device *kpc_dma_lookup_device(int minor)
{
	struct kpc_dma_device *c;

	mutex_lock(&kpc_dma_mtx);
	list_for_each_entry(c, &kpc_dma_list, list) {
		if (c->pldev->id == minor)
			goto out;
	}
	c = NULL; // not-found case
out:
	mutex_unlock(&kpc_dma_mtx);
	return c;
}

static void kpc_dma_add_device(struct kpc_dma_device *ldev)
{
	mutex_lock(&kpc_dma_mtx);
	list_add(&ldev->list, &kpc_dma_list);
	mutex_unlock(&kpc_dma_mtx);
}

static void kpc_dma_del_device(struct kpc_dma_device *ldev)
{
	mutex_lock(&kpc_dma_mtx);
	list_del(&ldev->list);
	mutex_unlock(&kpc_dma_mtx);
}

/**********  SysFS Attributes **********/
static ssize_t  engine_regs_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct kpc_dma_device *ldev;
	struct platform_device *pldev = to_platform_device(dev);

	if (!pldev)
		return 0;
	ldev = platform_get_drvdata(pldev);
	if (!ldev)
		return 0;

	return scnprintf(buf, PAGE_SIZE,
		"EngineControlStatus      = 0x%08x\n"
		"RegNextDescPtr           = 0x%08x\n"
		"RegSWDescPtr             = 0x%08x\n"
		"RegCompletedDescPtr      = 0x%08x\n"
		"desc_pool_first          = %p\n"
		"desc_pool_last           = %p\n"
		"desc_next                = %p\n"
		"desc_completed           = %p\n",
		readl(ldev->eng_regs + 1),
		readl(ldev->eng_regs + 2),
		readl(ldev->eng_regs + 3),
		readl(ldev->eng_regs + 4),
		ldev->desc_pool_first,
		ldev->desc_pool_last,
		ldev->desc_next,
		ldev->desc_completed
	);
}
static DEVICE_ATTR_RO(engine_regs);

static const struct attribute *ndd_attr_list[] = {
	&dev_attr_engine_regs.attr,
	NULL,
};

static struct class *kpc_dma_class;

/**********  Platform Driver Functions  **********/
static
int  kpc_dma_probe(struct platform_device *pldev)
{
	struct resource *r = NULL;
	int rv = 0;
	dev_t dev;

	struct kpc_dma_device *ldev = kzalloc(sizeof(*ldev), GFP_KERNEL);

	if (!ldev) {
		dev_err(&pldev->dev, "%s: unable to kzalloc space for kpc_dma_device\n", __func__);
		rv = -ENOMEM;
		goto err_rv;
	}

	INIT_LIST_HEAD(&ldev->list);

	ldev->pldev = pldev;
	platform_set_drvdata(pldev, ldev);
	atomic_set(&ldev->open_count, 1);

	mutex_init(&ldev->sem);
	lock_engine(ldev);

	// Get Engine regs resource
	r = platform_get_resource(pldev, IORESOURCE_MEM, 0);
	if (!r) {
		dev_err(&ldev->pldev->dev, "%s: didn't get the engine regs resource!\n", __func__);
		rv = -ENXIO;
		goto err_kfree;
	}
	ldev->eng_regs = ioremap(r->start, resource_size(r));
	if (!ldev->eng_regs) {
		dev_err(&ldev->pldev->dev, "%s: failed to ioremap engine regs!\n", __func__);
		rv = -ENXIO;
		goto err_kfree;
	}

	r = platform_get_resource(pldev, IORESOURCE_IRQ, 0);
	if (!r) {
		dev_err(&ldev->pldev->dev, "%s: didn't get the IRQ resource!\n", __func__);
		rv = -ENXIO;
		goto err_kfree;
	}
	ldev->irq = r->start;

	// Setup miscdev struct
	dev = MKDEV(assigned_major_num, pldev->id);
	ldev->kpc_dma_dev = device_create(kpc_dma_class, &pldev->dev, dev, ldev,
					  "kpc_dma%d", pldev->id);
	if (IS_ERR(ldev->kpc_dma_dev)) {
		rv = PTR_ERR(ldev->kpc_dma_dev);
		dev_err(&ldev->pldev->dev, "%s: device_create failed: %d\n", __func__, rv);
		goto err_kfree;
	}

	// Setup the DMA engine
	rv = setup_dma_engine(ldev, 30);
	if (rv) {
		dev_err(&ldev->pldev->dev, "%s: failed to setup_dma_engine: %d\n", __func__, rv);
		goto err_misc_dereg;
	}

	// Setup the sysfs files
	rv = sysfs_create_files(&(ldev->pldev->dev.kobj), ndd_attr_list);
	if (rv) {
		dev_err(&ldev->pldev->dev, "%s: Failed to add sysfs files: %d\n", __func__, rv);
		goto err_destroy_eng;
	}

	kpc_dma_add_device(ldev);

	return 0;

 err_destroy_eng:
	destroy_dma_engine(ldev);
 err_misc_dereg:
	device_destroy(kpc_dma_class, dev);
 err_kfree:
	kfree(ldev);
 err_rv:
	return rv;
}

static
int  kpc_dma_remove(struct platform_device *pldev)
{
	struct kpc_dma_device *ldev = platform_get_drvdata(pldev);

	if (!ldev)
		return -ENXIO;

	lock_engine(ldev);
	sysfs_remove_files(&(ldev->pldev->dev.kobj), ndd_attr_list);
	destroy_dma_engine(ldev);
	kpc_dma_del_device(ldev);
	device_destroy(kpc_dma_class, MKDEV(assigned_major_num, ldev->pldev->id));
	kfree(ldev);

	return 0;
}

/**********  Driver Functions  **********/
static struct platform_driver kpc_dma_plat_driver_i = {
	.probe        = kpc_dma_probe,
	.remove       = kpc_dma_remove,
	.driver = {
		.name   = KP_DRIVER_NAME_DMA_CONTROLLER,
	},
};

static
int __init kpc_dma_driver_init(void)
{
	int err;

	err = __register_chrdev(KPC_DMA_CHAR_MAJOR, 0, KPC_DMA_NUM_MINORS,
				"kpc_dma", &kpc_dma_fops);
	if (err < 0) {
		pr_err("Can't allocate a major number (%d) for kpc_dma (err = %d)\n",
		       KPC_DMA_CHAR_MAJOR, err);
		goto fail_chrdev_register;
	}
	assigned_major_num = err;

	kpc_dma_class = class_create(THIS_MODULE, "kpc_dma");
	err = PTR_ERR(kpc_dma_class);
	if (IS_ERR(kpc_dma_class)) {
		pr_err("Can't create class kpc_dma (err = %d)\n", err);
		goto fail_class_create;
	}

	err = platform_driver_register(&kpc_dma_plat_driver_i);
	if (err) {
		pr_err("Can't register platform driver for kpc_dma (err = %d)\n", err);
		goto fail_platdriver_register;
	}

	return err;

fail_platdriver_register:
	class_destroy(kpc_dma_class);
fail_class_create:
	__unregister_chrdev(KPC_DMA_CHAR_MAJOR, 0, KPC_DMA_NUM_MINORS, "kpc_dma");
fail_chrdev_register:
	return err;
}
module_init(kpc_dma_driver_init);

static
void __exit kpc_dma_driver_exit(void)
{
	platform_driver_unregister(&kpc_dma_plat_driver_i);
	class_destroy(kpc_dma_class);
	__unregister_chrdev(KPC_DMA_CHAR_MAJOR, 0, KPC_DMA_NUM_MINORS, "kpc_dma");
}
module_exit(kpc_dma_driver_exit);

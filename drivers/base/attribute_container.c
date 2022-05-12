// SPDX-License-Identifier: GPL-2.0
/*
 * attribute_container.c - implementation of a simple container for classes
 *
 * Copyright (c) 2005 - James Bottomley <James.Bottomley@steeleye.com>
 *
 * The basic idea here is to enable a device to be attached to an
 * aritrary numer of classes without having to allocate storage for them.
 * Instead, the contained classes select the devices they need to attach
 * to via a matching function.
 */

#include <linux/attribute_container.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>

#include "base.h"

/* This is a private structure used to tie the classdev and the
 * container .. it should never be visible outside this file */
struct internal_container {
	struct klist_node node;
	struct attribute_container *cont;
	struct device classdev;
};

static void internal_container_klist_get(struct klist_node *n)
{
	struct internal_container *ic =
		container_of(n, struct internal_container, node);
	get_device(&ic->classdev);
}

static void internal_container_klist_put(struct klist_node *n)
{
	struct internal_container *ic =
		container_of(n, struct internal_container, node);
	put_device(&ic->classdev);
}


/**
 * attribute_container_classdev_to_container - given a classdev, return the container
 *
 * @classdev: the class device created by attribute_container_add_device.
 *
 * Returns the container associated with this classdev.
 */
struct attribute_container *
attribute_container_classdev_to_container(struct device *classdev)
{
	struct internal_container *ic =
		container_of(classdev, struct internal_container, classdev);
	return ic->cont;
}
EXPORT_SYMBOL_GPL(attribute_container_classdev_to_container);

static LIST_HEAD(attribute_container_list);

static DEFINE_MUTEX(attribute_container_mutex);

/**
 * attribute_container_register - register an attribute container
 *
 * @cont: The container to register.  This must be allocated by the
 *        callee and should also be zeroed by it.
 */
int
attribute_container_register(struct attribute_container *cont)
{
	INIT_LIST_HEAD(&cont->node);
	klist_init(&cont->containers, internal_container_klist_get,
		   internal_container_klist_put);

	mutex_lock(&attribute_container_mutex);
	list_add_tail(&cont->node, &attribute_container_list);
	mutex_unlock(&attribute_container_mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(attribute_container_register);

/**
 * attribute_container_unregister - remove a container registration
 *
 * @cont: previously registered container to remove
 */
int
attribute_container_unregister(struct attribute_container *cont)
{
	int retval = -EBUSY;

	mutex_lock(&attribute_container_mutex);
	spin_lock(&cont->containers.k_lock);
	if (!list_empty(&cont->containers.k_list))
		goto out;
	retval = 0;
	list_del(&cont->node);
 out:
	spin_unlock(&cont->containers.k_lock);
	mutex_unlock(&attribute_container_mutex);
	return retval;

}
EXPORT_SYMBOL_GPL(attribute_container_unregister);

/* private function used as class release */
static void attribute_container_release(struct device *classdev)
{
	struct internal_container *ic
		= container_of(classdev, struct internal_container, classdev);
	struct device *dev = classdev->parent;

	kfree(ic);
	put_device(dev);
}

/**
 * attribute_container_add_device - see if any container is interested in dev
 *
 * @dev: device to add attributes to
 * @fn:	 function to trigger addition of class device.
 *
 * This function allocates storage for the class device(s) to be
 * attached to dev (one for each matching attribute_container).  If no
 * fn is provided, the code will simply register the class device via
 * device_add.  If a function is provided, it is expected to add
 * the class device at the appropriate time.  One of the things that
 * might be necessary is to allocate and initialise the classdev and
 * then add it a later time.  To do this, call this routine for
 * allocation and initialisation and then use
 * attribute_container_device_trigger() to call device_add() on
 * it.  Note: after this, the class device contains a reference to dev
 * which is not relinquished until the release of the classdev.
 */
void
attribute_container_add_device(struct device *dev,
			       int (*fn)(struct attribute_container *,
					 struct device *,
					 struct device *))
{
	struct attribute_container *cont;

	mutex_lock(&attribute_container_mutex);
	list_for_each_entry(cont, &attribute_container_list, node) {
		struct internal_container *ic;

		if (attribute_container_no_classdevs(cont))
			continue;

		if (!cont->match(cont, dev))
			continue;

		ic = kzalloc(sizeof(*ic), GFP_KERNEL);
		if (!ic) {
			dev_err(dev, "failed to allocate class container\n");
			continue;
		}

		ic->cont = cont;
		device_initialize(&ic->classdev);
		ic->classdev.parent = get_device(dev);
		ic->classdev.class = cont->class;
		cont->class->dev_release = attribute_container_release;
		dev_set_name(&ic->classdev, "%s", dev_name(dev));
		if (fn)
			fn(cont, dev, &ic->classdev);
		else
			attribute_container_add_class_device(&ic->classdev);
		klist_add_tail(&ic->node, &cont->containers);
	}
	mutex_unlock(&attribute_container_mutex);
}

/* FIXME: can't break out of this unless klist_iter_exit is also
 * called before doing the break
 */
#define klist_for_each_entry(pos, head, member, iter) \
	for (klist_iter_init(head, iter); (pos = ({ \
		struct klist_node *n = klist_next(iter); \
		n ? container_of(n, typeof(*pos), member) : \
			({ klist_iter_exit(iter) ; NULL; }); \
	})) != NULL;)


/**
 * attribute_container_remove_device - make device eligible for removal.
 *
 * @dev:  The generic device
 * @fn:	  A function to call to remove the device
 *
 * This routine triggers device removal.  If fn is NULL, then it is
 * simply done via device_unregister (note that if something
 * still has a reference to the classdev, then the memory occupied
 * will not be freed until the classdev is released).  If you want a
 * two phase release: remove from visibility and then delete the
 * device, then you should use this routine with a fn that calls
 * device_del() and then use attribute_container_device_trigger()
 * to do the final put on the classdev.
 */
void
attribute_container_remove_device(struct device *dev,
				  void (*fn)(struct attribute_container *,
					     struct device *,
					     struct device *))
{
	struct attribute_container *cont;

	mutex_lock(&attribute_container_mutex);
	list_for_each_entry(cont, &attribute_container_list, node) {
		struct internal_container *ic;
		struct klist_iter iter;

		if (attribute_container_no_classdevs(cont))
			continue;

		if (!cont->match(cont, dev))
			continue;

		klist_for_each_entry(ic, &cont->containers, node, &iter) {
			if (dev != ic->classdev.parent)
				continue;
			klist_del(&ic->node);
			if (fn)
				fn(cont, dev, &ic->classdev);
			else {
				attribute_container_remove_attrs(&ic->classdev);
				device_unregister(&ic->classdev);
			}
		}
	}
	mutex_unlock(&attribute_container_mutex);
}

static int
do_attribute_container_device_trigger_safe(struct device *dev,
					   struct attribute_container *cont,
					   int (*fn)(struct attribute_container *,
						     struct device *, struct device *),
					   int (*undo)(struct attribute_container *,
						       struct device *, struct device *))
{
	int ret;
	struct internal_container *ic, *failed;
	struct klist_iter iter;

	if (attribute_container_no_classdevs(cont))
		return fn(cont, dev, NULL);

	klist_for_each_entry(ic, &cont->containers, node, &iter) {
		if (dev == ic->classdev.parent) {
			ret = fn(cont, dev, &ic->classdev);
			if (ret) {
				failed = ic;
				klist_iter_exit(&iter);
				goto fail;
			}
		}
	}
	return 0;

fail:
	if (!undo)
		return ret;

	/* Attempt to undo the work partially done. */
	klist_for_each_entry(ic, &cont->containers, node, &iter) {
		if (ic == failed) {
			klist_iter_exit(&iter);
			break;
		}
		if (dev == ic->classdev.parent)
			undo(cont, dev, &ic->classdev);
	}
	return ret;
}

/**
 * attribute_container_device_trigger_safe - execute a trigger for each
 * matching classdev or fail all of them.
 *
 * @dev:  The generic device to run the trigger for
 * @fn	  the function to execute for each classdev.
 * @undo  A function to undo the work previously done in case of error
 *
 * This function is a safe version of
 * attribute_container_device_trigger. It stops on the first error and
 * undo the partial work that has been done, on previous classdev.  It
 * is guaranteed that either they all succeeded, or none of them
 * succeeded.
 */
int
attribute_container_device_trigger_safe(struct device *dev,
					int (*fn)(struct attribute_container *,
						  struct device *,
						  struct device *),
					int (*undo)(struct attribute_container *,
						    struct device *,
						    struct device *))
{
	struct attribute_container *cont, *failed = NULL;
	int ret = 0;

	mutex_lock(&attribute_container_mutex);

	list_for_each_entry(cont, &attribute_container_list, node) {

		if (!cont->match(cont, dev))
			continue;

		ret = do_attribute_container_device_trigger_safe(dev, cont,
								 fn, undo);
		if (ret) {
			failed = cont;
			break;
		}
	}

	if (ret && !WARN_ON(!undo)) {
		list_for_each_entry(cont, &attribute_container_list, node) {

			if (failed == cont)
				break;

			if (!cont->match(cont, dev))
				continue;

			do_attribute_container_device_trigger_safe(dev, cont,
								   undo, NULL);
		}
	}

	mutex_unlock(&attribute_container_mutex);
	return ret;

}

/**
 * attribute_container_device_trigger - execute a trigger for each matching classdev
 *
 * @dev:  The generic device to run the trigger for
 * @fn	  the function to execute for each classdev.
 *
 * This function is for executing a trigger when you need to know both
 * the container and the classdev.  If you only care about the
 * container, then use attribute_container_trigger() instead.
 */
void
attribute_container_device_trigger(struct device *dev,
				   int (*fn)(struct attribute_container *,
					     struct device *,
					     struct device *))
{
	struct attribute_container *cont;

	mutex_lock(&attribute_container_mutex);
	list_for_each_entry(cont, &attribute_container_list, node) {
		struct internal_container *ic;
		struct klist_iter iter;

		if (!cont->match(cont, dev))
			continue;

		if (attribute_container_no_classdevs(cont)) {
			fn(cont, dev, NULL);
			continue;
		}

		klist_for_each_entry(ic, &cont->containers, node, &iter) {
			if (dev == ic->classdev.parent)
				fn(cont, dev, &ic->classdev);
		}
	}
	mutex_unlock(&attribute_container_mutex);
}

/**
 * attribute_container_trigger - trigger a function for each matching container
 *
 * @dev:  The generic device to activate the trigger for
 * @fn:	  the function to trigger
 *
 * This routine triggers a function that only needs to know the
 * matching containers (not the classdev) associated with a device.
 * It is more lightweight than attribute_container_device_trigger, so
 * should be used in preference unless the triggering function
 * actually needs to know the classdev.
 */
void
attribute_container_trigger(struct device *dev,
			    int (*fn)(struct attribute_container *,
				      struct device *))
{
	struct attribute_container *cont;

	mutex_lock(&attribute_container_mutex);
	list_for_each_entry(cont, &attribute_container_list, node) {
		if (cont->match(cont, dev))
			fn(cont, dev);
	}
	mutex_unlock(&attribute_container_mutex);
}

/**
 * attribute_container_add_attrs - add attributes
 *
 * @classdev: The class device
 *
 * This simply creates all the class device sysfs files from the
 * attributes listed in the container
 */
int
attribute_container_add_attrs(struct device *classdev)
{
	struct attribute_container *cont =
		attribute_container_classdev_to_container(classdev);
	struct device_attribute **attrs = cont->attrs;
	int i, error;

	BUG_ON(attrs && cont->grp);

	if (!attrs && !cont->grp)
		return 0;

	if (cont->grp)
		return sysfs_create_group(&classdev->kobj, cont->grp);

	for (i = 0; attrs[i]; i++) {
		sysfs_attr_init(&attrs[i]->attr);
		error = device_create_file(classdev, attrs[i]);
		if (error)
			return error;
	}

	return 0;
}

/**
 * attribute_container_add_class_device - same function as device_add
 *
 * @classdev:	the class device to add
 *
 * This performs essentially the same function as device_add except for
 * attribute containers, namely add the classdev to the system and then
 * create the attribute files
 */
int
attribute_container_add_class_device(struct device *classdev)
{
	int error = device_add(classdev);

	if (error)
		return error;
	return attribute_container_add_attrs(classdev);
}

/**
 * attribute_container_add_class_device_adapter - simple adapter for triggers
 *
 * @cont: the container to register.
 * @dev:  the generic device to activate the trigger for
 * @classdev:	the class device to add
 *
 * This function is identical to attribute_container_add_class_device except
 * that it is designed to be called from the triggers
 */
int
attribute_container_add_class_device_adapter(struct attribute_container *cont,
					     struct device *dev,
					     struct device *classdev)
{
	return attribute_container_add_class_device(classdev);
}

/**
 * attribute_container_remove_attrs - remove any attribute files
 *
 * @classdev: The class device to remove the files from
 *
 */
void
attribute_container_remove_attrs(struct device *classdev)
{
	struct attribute_container *cont =
		attribute_container_classdev_to_container(classdev);
	struct device_attribute **attrs = cont->attrs;
	int i;

	if (!attrs && !cont->grp)
		return;

	if (cont->grp) {
		sysfs_remove_group(&classdev->kobj, cont->grp);
		return ;
	}

	for (i = 0; attrs[i]; i++)
		device_remove_file(classdev, attrs[i]);
}

/**
 * attribute_container_class_device_del - equivalent of class_device_del
 *
 * @classdev: the class device
 *
 * This function simply removes all the attribute files and then calls
 * device_del.
 */
void
attribute_container_class_device_del(struct device *classdev)
{
	attribute_container_remove_attrs(classdev);
	device_del(classdev);
}

/**
 * attribute_container_find_class_device - find the corresponding class_device
 *
 * @cont:	the container
 * @dev:	the generic device
 *
 * Looks up the device in the container's list of class devices and returns
 * the corresponding class_device.
 */
struct device *
attribute_container_find_class_device(struct attribute_container *cont,
				      struct device *dev)
{
	struct device *cdev = NULL;
	struct internal_container *ic;
	struct klist_iter iter;

	klist_for_each_entry(ic, &cont->containers, node, &iter) {
		if (ic->classdev.parent == dev) {
			cdev = &ic->classdev;
			/* FIXME: must exit iterator then break */
			klist_iter_exit(&iter);
			break;
		}
	}

	return cdev;
}
EXPORT_SYMBOL_GPL(attribute_container_find_class_device);
